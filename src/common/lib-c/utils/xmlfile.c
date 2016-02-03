#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "xmlfile.h"

#define XML_WHITESPACES   "\t\r\n "    // whitespace

#define XML_BUFFER_SIZE   8192         // size of internal memory buffers
#define XML_MAX_ERROR_LEN 256          // maximum error string length

typedef struct xml_root_st *XML_ROOT_T;

struct xml_root_st
{                               // additional data for the root tag
  struct xml_st xml;             // is a super-struct built on top of xml_st struct
  XML_T cur;                  // current xml tree insertion point
  char *m;                      // original xml string
  size_t len;                   // length of allocated memory for mmap, -1 for malloc
  char *u;                      // UTF-8 conversion of string if original was UTF-16
  char *s;                      // start of work area
  char *e;                      // end of work area
  char **ent;                   // general entities (ampersand sequences)
  char ***attr;                 // default attributes
  char ***pi;                   // processing instructions
  short standalone;             // non-zero if <?xml standalone="yes"?>
  char err[XML_MAX_ERROR_LEN];         // error string
};

char *XML_NULL_ARRAY[] = { NULL };   // empty, null terminated array of strings

// returns the first child tag with the given name or NULL if not found
XML_T XML_Child(XML_T xml, const char *name)
{
  xml = (xml) ? xml->child : NULL;
  while (xml && strcmp(name, xml->name))
    xml = xml->sibling;
  return xml;
}

// returns the Nth tag with the same name in the same subsection or NULL if not
// found
XML_T XML_Walk(XML_T xml, int idx)
{
  for (; xml && idx; idx--)
    xml = xml->next;
  return xml;
}

// returns the value of the requested tag attribute or NULL if not found
const char *XML_Attribute(XML_T xml, const char *attr)
{
  int i = 0, j = 1;
  XML_ROOT_T root = (XML_ROOT_T) xml;

  if (!xml || !xml->attr)
    return NULL;
  while (xml->attr[i] && strcmp(attr, xml->attr[i]))
    i += 2;
  if (xml->attr[i])
    return xml->attr[i + 1];    // found attribute

  while (root->xml.parent)
    root = (XML_ROOT_T) root->xml.parent; // root tag
  for (i = 0; root->attr[i] && strcmp(xml->name, root->attr[i][0]); i++);
  if (!root->attr[i])
    return NULL;                // no matching default attributes
  while (root->attr[i][j] && strcmp(attr, root->attr[i][j]))
    j += 3;
  return (root->attr[i][j]) ? root->attr[i][j + 1] : NULL;  // found default
}

// same as XML_Get but takes an already initialized va_list
XML_T XML_VGet(XML_T xml, va_list ap)
{
  char *name = va_arg(ap, char *);
  int idx = -1;

  if (name && *name)
  {
    idx = va_arg(ap, int);

    xml = XML_Child(xml, name);
  }

  return (idx < 0) ? xml : XML_VGet(XML_Walk(xml, idx), ap);
}

// Traverses the xml tree to retrieve a specific subtag. Takes a variable
// length list of tag names and indexes. The argument list must be terminated
// by either an index of -1 or an empty string tag name. Example: 
// title = XML_Get(library, "shelf", 0, "book", 2, "title", -1);
// This retrieves the title of the 3rd book on the 1st shelf of library.
// Returns NULL if not found.
XML_T XML_Get(XML_T xml, ...)
{
  va_list ap;
  XML_T r;

  va_start(ap, xml);
  r = XML_VGet(xml, ap);
  va_end(ap);
  return r;
}

// returns a null terminated array of processing instructions for the given
// target
const char **XML_PI(XML_T xml, const char *target)
{
  XML_ROOT_T root = (XML_ROOT_T) xml;
  int i = 0;

  if (!root)
    return (const char **) XML_NULL_ARRAY;
  while (root->xml.parent)
    root = (XML_ROOT_T) root->xml.parent; // root tag
  while (root->pi[i] && strcmp(target, root->pi[i][0]))
    i++;                        // find target
  return (const char **) ((root->pi[i]) ? root->pi[i] + 1 : XML_NULL_ARRAY);
}

// set an error string and return root
XML_T XML_SetError(XML_ROOT_T root, char *s, const char *err, ...)
{
  va_list ap;
  int line = 1;
  char *t, fmt[XML_MAX_ERROR_LEN];

  for (t = root->s; t < s; t++)
    if (*t == '\n')
      line++;
  sprintf(fmt, "[error near line %d]: %s", line, err);

  va_start(ap, err);
  vsprintf(root->err, fmt, ap);
  va_end(ap);

  return &root->xml;
}

// Recursively decodes entity and character references and normalizes new lines
// ent is a null terminated array of alternating entity names and values. set t
// to '&' for general entity decoding, '%' for parameter entity decoding, 'c'
// for cdata sections, ' ' for attribute normalization, or '*' for non-cdata
// attribute normalization. Returns s, or if the decoded string is longer than
// s, returns a malloced string that must be freed.
char *XML_Decode(char *s, char **ent, char t)
{
  char *e, *r = s, *m = s;
  long b, c, d, l;

  for (; *s; s++)
  {                             // normalize line endings
    while (*s == '\r')
    {
      *(s++) = '\n';
      if (*s == '\n')
        memmove(s, (s + 1), strlen(s));
    }
  }

  for (s = r;;)
  {
    while (*s && *s != '&' && (*s != '%' || t != '%') && !isspace(*s))
      s++;

    if (!*s)
      break;
    else if (t != 'c' && !strncmp(s, "&#", 2))
    {                           // character reference
      if (s[2] == 'x')
        c = strtol(s + 3, &e, 16);  // base 16
      else
        c = strtol(s + 2, &e, 10);  // base 10
      if (!c || *e != ';')
      {
        s++;
        continue;
      }                         // not a character ref

      if (c < 0x80)
        *(s++) = (char) c;      // US-ASCII subset
      else
      {                         // multi-byte UTF-8 sequence
        for (b = 0, d = c; d; d /= 2)
          b++;                  // number of bits in c
        b = (b - 2) / 5;        // number of bytes in payload
        *(s++) = (char) ((0xFF << (7 - b)) | (c >> (6 * b)));  // head
        while (b)
          *(s++) = 0x80 | ((c >> (6 * --b)) & 0x3F);  // payload
      }

      memmove(s, strchr(s, ';') + 1, strlen(strchr(s, ';')));
    } else if ((*s == '&' && (t == '&' || t == ' ' || t == '*')) ||
               (*s == '%' && t == '%'))
    {                           // entity reference
      for (b = 0; ent[b] && strncmp(s + 1, ent[b], strlen(ent[b])); b += 2);  // find entity in entity list

      if (ent[b++])
      {                         // found a match
        if ((c = strlen(ent[b])) - 1 > (e = strchr(s, ';')) - s)
        {
          l = (d = (s - r)) + c + strlen(e);  // new length
          r = (r == m) ? strcpy(malloc(l), r) : realloc(r, l);
          e = strchr((s = r + d), ';'); // fix up pointers
        }

        memmove(s + c, e + 1, strlen(e)); // shift rest of string
        strncpy(s, ent[b], c);  // copy in replacement text
      } else
        s++;                    // not a known entity
    } else if ((t == ' ' || t == '*') && isspace(*s))
      *(s++) = ' ';
    else
      s++;                      // no decoding needed
  }

  if (t == '*')
  {                             // normalize spaces for non-cdata attributes
    for (s = r; *s; s++)
    {
      if ((l = strspn(s, " ")))
        memmove(s, s + l, strlen(s + l) + 1);
      while (*s && *s != ' ')
        s++;
    }
    if (--s >= r && *s == ' ')
      *s = '\0';                // trim any trailing space
  }
  return r;
}

// called when parser finds start of new tag
void XML_OnTagOpen(XML_ROOT_T root, char *name, char **attr)
{
  XML_T xml = root->cur;

  if (xml->name)
  {
    xml = XML_AddChild(xml, name, strlen(xml->txt));
  } else
  {
    xml->name = name;           // first open tag
  }

  xml->attr = attr;
  root->cur = xml;              // update tag insertion point
}

// called when parser finds character content between open and closing tag
void XML_OnContentChar(XML_ROOT_T root, char *s, size_t len, char t)
{
  XML_T xml = root->cur;
  char *m = s;
  size_t l;

  if (!xml || !xml->name || !len)
    return;                     // sanity check

  s[len] = '\0';                // null terminate text (calling functions anticipate this)
  len = strlen(s = XML_Decode(s, root->ent, t)) + 1;

  if (!*(xml->txt))
    xml->txt = s;               // initial character content
  else
  {                             // allocate our own memory and make a copy
    xml->txt = (xml->flags & XML_FLAG_DUP_CONTENT)  // allocate some space
      ? realloc(xml->txt, (l = strlen(xml->txt)) + len)
      : strcpy(malloc((l = strlen(xml->txt)) + len), xml->txt);
    strcpy(xml->txt + l, s);    // add new char content
    if (s != m)
      free(s);                  // free s if it was malloced by XML_Decode()
  }

  if (xml->txt != m)
    XML_SetFlag(xml, XML_FLAG_DUP_CONTENT);
}

// called when parser finds closing tag
XML_T XML_OnTagClose(XML_ROOT_T root, char *name, char *s)
{
  if (!root->cur || !root->cur->name || strcmp(name, root->cur->name))
    return XML_SetError(root, s, "unexpected closing tag </%s>", name);

  root->cur = root->cur->parent;
  return NULL;
}

// checks for circular entity references, returns non-zero if no circular
// references are found, zero otherwise
int XML_EntityReferencesOk(char *name, char *s, char **ent)
{
  int i;

  for (;; s++)
  {
    while (*s && *s != '&')
      s++;                      // find next entity reference
    if (!*s)
      return 1;
    if (!strncmp(s + 1, name, strlen(name)))
      return 0;                 // circular ref.
    for (i = 0; ent[i] && strncmp(ent[i], s + 1, strlen(ent[i])); i += 2);
    if (ent[i] && !XML_EntityReferencesOk(name, ent[i + 1], ent))
      return 0;
  }
}

// called when the parser finds a processing instruction
void XML_OnProcessInstruction(XML_ROOT_T root, char *s, size_t len)
{
  int i = 0, j = 1;
  char *target = s;

  s[len] = '\0';                // null terminate instruction
  if (*(s += strcspn(s, XML_WHITESPACES)))
  {
    *s = '\0';                  // null terminate target
    s += strspn(s + 1, XML_WHITESPACES) + 1; // skip whitespace after target
  }

  if (!strcmp(target, "xml"))
  {                             // <?xml ... ?>
    if ((s = strstr(s, "standalone")) && !strncmp(s + strspn(s + 10,
                                                             XML_WHITESPACES "='\"")
                                                  + 10, "yes", 3))
      root->standalone = 1;
    return;
  }

  if (!root->pi[0])
    *(root->pi = malloc(sizeof(char **))) = NULL; //first pi

  while (root->pi[i] && strcmp(target, root->pi[i][0]))
    i++;                        // find target
  if (!root->pi[i])
  {                             // new target
    root->pi = realloc(root->pi, sizeof(char **) * (i + 2));
    root->pi[i] = malloc(sizeof(char *) * 3);
    root->pi[i][0] = target;
    root->pi[i][1] = (char *) (root->pi[i + 1] = NULL); // terminate pi list
    root->pi[i][2] = strdup("");  // empty document position list
  }

  while (root->pi[i][j])
    j++;                        // find end of instruction list for this target
  root->pi[i] = realloc(root->pi[i], sizeof(char *) * (j + 3));
  root->pi[i][j + 2] = realloc(root->pi[i][j + 1], j + 1);
  strcpy(root->pi[i][j + 2] + j - 1, (root->xml.name) ? ">" : "<");
  root->pi[i][j + 1] = NULL;    // null terminate pi list for this target
  root->pi[i][j] = s;           // set instruction
}

// called when the parser finds an internal doctype subset
short XML_OnInternalDtd(XML_ROOT_T root, char *s, size_t len)
{
  char q, *c, *t, *n = NULL, *v, **ent, **pe;
  int i, j;

  pe = memcpy(malloc(sizeof(XML_NULL_ARRAY)), XML_NULL_ARRAY, sizeof(XML_NULL_ARRAY));

  for (s[len] = '\0'; s;)
  {
    while (*s && *s != '<' && *s != '%')
      s++;                      // find next declaration

    if (!*s)
      break;
    else if (!strncmp(s, "<!ENTITY", 8))
    {                           // parse entity definitions
      c = s += strspn(s + 8, XML_WHITESPACES) + 8; // skip white space separator
      n = s + strspn(s, XML_WHITESPACES "%");  // find name
      *(s = n + strcspn(n, XML_WHITESPACES)) = ';';  // append ; to name

      v = s + strspn(s + 1, XML_WHITESPACES) + 1;  // find value
      if ((q = *(v++)) != '"' && q != '\'')
      {                         // skip externals
        s = strchr(s, '>');
        continue;
      }

      for (i = 0, ent = (*c == '%') ? pe : root->ent; ent[i]; i++);
      ent = realloc(ent, (i + 3) * sizeof(char *)); // space for next ent
      if (*c == '%')
        pe = ent;
      else
        root->ent = ent;

      *(++s) = '\0';            // null terminate name
      if ((s = strchr(v, q)))
        *(s++) = '\0';          // null terminate value
      ent[i + 1] = XML_Decode(v, pe, '%');  // set value
      ent[i + 2] = NULL;        // null terminate entity list
      if (!XML_EntityReferencesOk(n, ent[i + 1], ent))
      {                         // circular reference
        if (ent[i + 1] != v)
          free(ent[i + 1]);
        XML_SetError(root, v, "circular entity declaration &%s", n);
        break;
      } else
        ent[i] = n;             // set entity name
    } else if (!strncmp(s, "<!ATTLIST", 9))
    {                           // parse default attributes
      t = s + strspn(s + 9, XML_WHITESPACES) + 9;  // skip whitespace separator
      if (!*t)
      {
        XML_SetError(root, t, "unclosed <!ATTLIST");
        break;
      }
      if (*(s = t + strcspn(t, XML_WHITESPACES ">")) == '>')
        continue;
      else
        *s = '\0';              // null terminate tag name
      for (i = 0; root->attr[i] && strcmp(n, root->attr[i][0]); i++);

      while (*(n = ++s + strspn(s, XML_WHITESPACES)) && *n != '>')
      {
        if (*(s = n + strcspn(n, XML_WHITESPACES)))
          *s = '\0';            // attr name
        else
        {
          XML_SetError(root, t, "malformed <!ATTLIST");
          break;
        }

        s += strspn(s + 1, XML_WHITESPACES) + 1; // find next token
        c = (strncmp(s, "CDATA", 5)) ? "*" : " "; // is it cdata?
        if (!strncmp(s, "NOTATION", 8))
          s += strspn(s + 8, XML_WHITESPACES) + 8;
        s = (*s == '(') ? strchr(s, ')') : s + strcspn(s, XML_WHITESPACES);
        if (!s)
        {
          XML_SetError(root, t, "malformed <!ATTLIST");
          break;
        }

        s += strspn(s, XML_WHITESPACES ")"); // skip white space separator
        if (!strncmp(s, "#FIXED", 6))
          s += strspn(s + 6, XML_WHITESPACES) + 6;
        if (*s == '#')
        {                       // no default value
          s += strcspn(s, XML_WHITESPACES ">") - 1;
          if (*c == ' ')
            continue;           // cdata is default, nothing to do
          v = NULL;
        } else if ((*s == '"' || *s == '\'') && // default value
                   (s = strchr(v = s + 1, *s)))
          *s = '\0';
        else
        {
          XML_SetError(root, t, "malformed <!ATTLIST");
          break;
        }

        if (!root->attr[i])
        {                       // new tag name
          root->attr = (!i) ? malloc(2 * sizeof(char **))
            : realloc(root->attr, (i + 2) * sizeof(char **));
          root->attr[i] = malloc(2 * sizeof(char *));
          root->attr[i][0] = t; // set tag name
          root->attr[i][1] = (char *) (root->attr[i + 1] = NULL);
        }

        for (j = 1; root->attr[i][j]; j += 3);  // find end of list
        root->attr[i] = realloc(root->attr[i], (j + 4) * sizeof(char *));

        root->attr[i][j + 3] = NULL;  // null terminate list
        root->attr[i][j + 2] = c; // is it cdata?
        root->attr[i][j + 1] = (v) ? XML_Decode(v, root->ent, *c) : NULL;
        root->attr[i][j] = n;   // attribute name 
      }
    } else if (!strncmp(s, "<!--", 4))
      s = strstr(s + 4, "-->"); // comments
    else if (!strncmp(s, "<?", 2))
    {                           // processing instructions
      if ((s = strstr(c = s + 2, "?>")))
        XML_OnProcessInstruction(root, c, s++ - c);
    } else if (*s == '<')
      s = strchr(s, '>');       // skip other declarations
    else if (*(s++) == '%' && !root->standalone)
      break;
  }

  free(pe);
  return !*root->err;
}

// Converts a UTF-16 string to UTF-8. Returns a new string that must be freed
// or NULL if no conversion was needed.
char *XML_UTF16ToUTF8(char **s, size_t * len)
{
  char *u;
  size_t l = 0, sl, max = *len;
  long c, d;
  int b, be = (**s == '\xFE') ? 1 : (**s == '\xFF') ? 0 : -1;

  if (be == -1)
    return NULL;                // not UTF-16

  u = malloc(max);
  for (sl = 2; sl < *len - 1; sl += 2)
  {
    c = (be) ? (((*s)[sl] & 0xFF) << 8) | ((*s)[sl + 1] & 0xFF) //UTF-16BE
      : (((*s)[sl + 1] & 0xFF) << 8) | ((*s)[sl] & 0xFF); //UTF-16LE
    if (c >= 0xD800 && c <= 0xDFFF && (sl += 2) < *len - 1)
    {                           // high-half
      d = (be) ? (((*s)[sl] & 0xFF) << 8) | ((*s)[sl + 1] & 0xFF)
        : (((*s)[sl + 1] & 0xFF) << 8) | ((*s)[sl] & 0xFF);
      c = (((c & 0x3FF) << 10) | (d & 0x3FF)) + 0x10000;
    }

    while (l + 6 > max)
      u = realloc(u, max += XML_BUFFER_SIZE);
    if (c < 0x80)
      u[l++] = (char) c;        // US-ASCII subset
    else
    {                           // multi-byte UTF-8 sequence
      for (b = 0, d = c; d; d /= 2)
        b++;                    // bits in c
      b = (b - 2) / 5;          // bytes in payload
      u[l++] = (char) ((0xFF << (7 - b)) | (c >> (6 * b)));  // head
      while (b)
        u[l++] = 0x80 | ((c >> (6 * --b)) & 0x3F);  // payload
    }
  }
  return *s = realloc(u, *len = l);
}

// frees a tag attribute list
void XML_Free_attr(char **attr)
{
  int i = 0;
  char *m;

  if (!attr || attr == XML_NULL_ARRAY)
    return;                     // nothing to free
  while (attr[i])
    i += 2;                     // find end of attribute list
  m = attr[i + 1];              // list of which names and values are malloced
  for (i = 0; m[i]; i++)
  {
    if (m[i] & XML_FLAG_DUP_NAME)
      free(attr[i * 2]);
    if (m[i] & XML_FLAG_DUP_CONTENT)
      free(attr[(i * 2) + 1]);
  }
  free(m);
  free(attr);
}

// parse the given xml string and return an xml_st structure
XML_T XML_ParseStr(char *s, size_t len)
{
  XML_ROOT_T root = (XML_ROOT_T) XML_Alloc(NULL);
  char q, e, *d, **attr, **a = NULL;  // initialize a to avoid compile warning
  int l, i, j;

  root->m = s;
  if (!len)
    return XML_SetError(root, NULL, "root tag missing");
  root->u = XML_UTF16ToUTF8(&s, &len); // convert utf-16 to utf-8
  root->e = (root->s = s) + len;  // record start and end of work area

  e = s[len - 1];               // save end char
  s[len - 1] = '\0';            // turn end char into null terminator

  while (*s && *s != '<')
    s++;                        // find first tag
  if (!*s)
    return XML_SetError(root, s, "root tag missing");

  for (;;)
  {
    attr = (char **) XML_NULL_ARRAY;
    d = ++s;

    if (isalpha(*s) || *s == '_' || *s == ':' || *s < '\0')
    {                           // new tag
      if (!root->cur)
        return XML_SetError(root, d, "markup outside of root element");

      s += strcspn(s, XML_WHITESPACES "/>");
      while (isspace(*s))
        *(s++) = '\0';          // null terminate tag name

      if (*s && *s != '/' && *s != '>') // find tag in default attr list
        for (i = 0; (a = root->attr[i]) && strcmp(a[0], d); i++);

      for (l = 0; *s && *s != '/' && *s != '>'; l += 2)
      {                         // new attrib
        attr = (l) ? realloc(attr, (l + 4) * sizeof(char *)) : malloc(4 * sizeof(char *));  // allocate space
        attr[l + 3] = (l) ? realloc(attr[l + 1], (l / 2) + 2) : malloc(2);  // mem for list of maloced vals
        strcpy(attr[l + 3] + (l / 2), " "); // value is not malloced
        attr[l + 2] = NULL;     // null terminate list
        attr[l + 1] = "";       // temporary attribute value
        attr[l] = s;            // set attribute name

        s += strcspn(s, XML_WHITESPACES "=/>");
        if (*s == '=' || isspace(*s))
        {
          *(s++) = '\0';        // null terminate tag attribute name
          q = *(s += strspn(s, XML_WHITESPACES "="));
          if (q == '"' || q == '\'')
          {                     // attribute value
            attr[l + 1] = ++s;
            while (*s && *s != q)
              s++;
            if (*s)
              *(s++) = '\0';    // null terminate attribute val
            else
            {
              XML_Free_attr(attr);
              return XML_SetError(root, d, "missing %c", q);
            }

            for (j = 1; a && a[j] && strcmp(a[j], attr[l]); j += 3);
            attr[l + 1] = XML_Decode(attr[l + 1], root->ent, (char) ((a && a[j]) ? *a[j + 2] : ' '));
            if (attr[l + 1] < d || attr[l + 1] > s)
              attr[l + 3][l / 2] = XML_FLAG_DUP_CONTENT;  // value malloced
          }
        }
        while (isspace(*s))
          s++;
      }

      if (*s == '/')
      {                         // self closing tag
        *(s++) = '\0';
        if ((*s && *s != '>') || (!*s && e != '>'))
        {
          if (l)
            XML_Free_attr(attr);
          return XML_SetError(root, d, "missing >");
        }
        XML_OnTagOpen(root, d, attr);
        XML_OnTagClose(root, d, s);
      } else if ((q = *s) == '>' || (!*s && e == '>'))
      {                         // open tag
        *s = '\0';              // temporarily null terminate tag name
        XML_OnTagOpen(root, d, attr);
        *s = q;
      } else
      {
        if (l)
          XML_Free_attr(attr);
        return XML_SetError(root, d, "missing >");
      }
    } else if (*s == '/')
    {                           // close tag
      s += strcspn(d = s + 1, XML_WHITESPACES ">") + 1;
      if (!(q = *s) && e != '>')
        return XML_SetError(root, d, "missing >");
      *s = '\0';                // temporarily null terminate tag name
      if (XML_OnTagClose(root, d, s))
        return &root->xml;
      if (isspace(*s = q))
        s += strspn(s, XML_WHITESPACES);
    } else if (!strncmp(s, "!--", 3))
    {                           // xml comment
      if (!(s = strstr(s + 3, "--")) || (*(s += 2) != '>' && *s) ||
          (!*s && e != '>'))
        return XML_SetError(root, d, "unclosed <!--");
    } else if (!strncmp(s, "![CDATA[", 8))
    {                           // cdata
      if ((s = strstr(s, "]]>")))
        XML_OnContentChar(root, d + 8, (s += 2) - d - 10, 'c');
      else
        return XML_SetError(root, d, "unclosed <![CDATA[");
    } else if (!strncmp(s, "!DOCTYPE", 8))
    {                           // dtd
      for (l = 0; *s && ((!l && *s != '>') || (l && (*s != ']' ||
                                                     *(s +
                                                       strspn(s + 1,
                                                              XML_WHITESPACES) +
                                                       1) != '>')));
           l = (*s == '[') ? 1 : l)
        s += strcspn(s + 1, "[]>") + 1;
      if (!*s && e != '>')
        return XML_SetError(root, d, "unclosed <!DOCTYPE");
      d = (l) ? strchr(d, '[') + 1 : d;
      if (l && !XML_OnInternalDtd(root, d, s++ - d))
        return &root->xml;
    } else if (*s == '?')
    {                           // <?...?> processing instructions
      do
      {
        s = strchr(s, '?');
      } while (s && *(++s) && *s != '>');
      if (!s || (!*s && e != '>'))
        return XML_SetError(root, d, "unclosed <?");
      else
        XML_OnProcessInstruction(root, d + 1, s - d - 2);
    } else
      return XML_SetError(root, d, "unexpected <");

    if (!s || !*s)
      break;
    *s = '\0';
    d = ++s;
    if (*s && *s != '<')
    {                           // tag character content
      while (*s && *s != '<')
        s++;
      if (*s)
        XML_OnContentChar(root, d, s - d, '&');
      else
        break;
    } else if (!*s)
      break;
  }

  if (!root->cur)
    return &root->xml;
  else if (!root->cur->name)
    return XML_SetError(root, d, "root tag missing");
  else
    return XML_SetError(root, d, "unclosed tag <%s>", root->cur->name);
}

// Wrapper for XML_ParseStr() that accepts a file stream. Reads the entire
// stream into memory and then parses it. For xml files, use XML_ParseFile()
// or XML_ParseFd()
XML_T XML_ParseFp(FILE * fp)
{
  XML_ROOT_T root;
  size_t l, len = 0;
  char *s;

  if (!(s = malloc(XML_BUFFER_SIZE)))
    return NULL;
  do
  {
    len += (l = fread((s + len), 1, XML_BUFFER_SIZE, fp));
    if (l == XML_BUFFER_SIZE)
      s = realloc(s, len + XML_BUFFER_SIZE);
  } while (s && l == XML_BUFFER_SIZE);

  if (!s)
    return NULL;
  root = (XML_ROOT_T) XML_ParseStr(s, len);
  root->len = -1;               // so we know to free s in XML_Free()
  return &root->xml;
}

#if 0
// A wrapper for XML_ParseStr() that accepts a file descriptor. First
// attempts to mem map the file. Failing that, reads the file into memory.
// Returns NULL on failure.
XML_T XML_ParseFd(int fd)
{
  XML_ROOT_T root;
  struct stat st;
  size_t l;
  void *m;

  if (fd < 0)
    return NULL;
  fstat(fd, &st);

  l = read(fd, m = malloc(st.st_size), st.st_size);
  root = (XML_ROOT_T) XML_ParseStr(m, l);
  root->len = -1;             // so we know to free s in XML_Free()

  return &root->xml;
}
#endif

// a wrapper for XML_ParseFd that accepts a file name
XML_T XML_ParseFile(const char *file)
{
  FILE *fp;
  XML_T xml;

  if (file == NULL)
    return NULL;

  fp = fopen(file, "rt");
  if (fp == NULL)
    return NULL;

  xml = XML_ParseFp(fp);

  fclose(fp);
  return xml;
}

// Encodes ampersand sequences appending the results to *dst, reallocating *dst
// if length excedes max. a is non-zero for attribute encoding. Returns *dst
char *XML_AmpersandEncode(const char *s, size_t len, char **dst, size_t * dlen,
                      size_t * max, short a)
{
  const char *e;

  for (e = s + len; s != e; s++)
  {
    while (*dlen + 10 > *max)
      *dst = realloc(*dst, *max += XML_BUFFER_SIZE);

    switch (*s)
    {
      case '\0':
        return *dst;
      case '&':
        *dlen += sprintf(*dst + *dlen, "&amp;");
        break;
      case '<':
        *dlen += sprintf(*dst + *dlen, "&lt;");
        break;
      case '>':
        *dlen += sprintf(*dst + *dlen, "&gt;");
        break;
      case '"':
        *dlen += sprintf(*dst + *dlen, (a) ? "&quot;" : "\"");
        break;
      case '\n':
        *dlen += sprintf(*dst + *dlen, (a) ? "&#xA;" : "\n");
        break;
      case '\t':
        *dlen += sprintf(*dst + *dlen, (a) ? "&#x9;" : "\t");
        break;
      case '\r':
        *dlen += sprintf(*dst + *dlen, "&#xD;");
        break;
      default:
        (*dst)[(*dlen)++] = *s;
    }
  }
  return *dst;
}

// Recursively converts each tag to xml appending it to *s. Reallocates *s if
// its length excedes max. start is the location of the previous tag in the
// parent tag's character content. Returns *s.
char *XML_ToString_r(XML_T xml, char **s, size_t * len, size_t * max, size_t start, char ***attr)
{
  int i, j;
  char *txt = (xml->parent) ? xml->parent->txt : "";
  size_t off = 0;

  // parent character content up to this tag
  *s = XML_AmpersandEncode(txt + start, xml->off - start, s, len, max, 0);

  while (*len + strlen(xml->name) + 4 > *max) // reallocate s
    *s = realloc(*s, *max += XML_BUFFER_SIZE);

  *len += sprintf(*s + *len, "<%s", xml->name); // open tag
  for (i = 0; xml->attr[i]; i += 2)
  {                             // tag attributes
    if (XML_Attribute(xml, xml->attr[i]) != xml->attr[i + 1])
      continue;
    while (*len + strlen(xml->attr[i]) + 7 > *max)  // reallocate s
      *s = realloc(*s, *max += XML_BUFFER_SIZE);

    *len += sprintf(*s + *len, " %s=\"", xml->attr[i]);
    XML_AmpersandEncode(xml->attr[i + 1], -1, s, len, max, 1);
    *len += sprintf(*s + *len, "\"");
  }

  for (i = 0; attr[i] && strcmp(attr[i][0], xml->name); i++);
  for (j = 1; attr[i] && attr[i][j]; j += 3)
  {                             // default attributes
    if (!attr[i][j + 1] || XML_Attribute(xml, attr[i][j]) != attr[i][j + 1])
      continue;                 // skip duplicates and non-values
    while (*len + strlen(attr[i][j]) + 7 > *max)  // reallocate s
      *s = realloc(*s, *max += XML_BUFFER_SIZE);

    *len += sprintf(*s + *len, " %s=\"", attr[i][j]);
    XML_AmpersandEncode(attr[i][j + 1], -1, s, len, max, 1);
    *len += sprintf(*s + *len, "\"");
  }
  *len += sprintf(*s + *len, ">");

  *s = (xml->child) ? XML_ToString_r(xml->child, s, len, max, 0, attr) //child
    : XML_AmpersandEncode(xml->txt, -1, s, len, max, 0);  //data

  while (*len + strlen(xml->name) + 4 > *max) // reallocate s
    *s = realloc(*s, *max += XML_BUFFER_SIZE);

  *len += sprintf(*s + *len, "</%s>", xml->name); // close tag

  while (txt[off] && off < xml->off)
    off++;                      // make sure off is within bounds
  return (xml->ordered) ? XML_ToString_r(xml->ordered, s, len, max, off, attr)
    : XML_AmpersandEncode(txt + off, -1, s, len, max, 0);
}

// Converts an xml_st structure back to xml. Returns a string of xml data that
// must be freed.
char *XML_ToString(XML_T xml)
{
  XML_T p = (xml) ? xml->parent : NULL, o = (xml) ? xml->ordered : NULL;
  XML_ROOT_T root = (XML_ROOT_T) xml;
  size_t len = 0, max = XML_BUFFER_SIZE;
  char *s = strcpy(malloc(max), ""), *t, *n;
  int i, j, k;

  if (!xml || !xml->name)
    return realloc(s, len + 1);
  while (root->xml.parent)
    root = (XML_ROOT_T) root->xml.parent; // root tag

  for (i = 0; !p && root->pi[i]; i++)
  {                             // pre-root processing instructions
    for (k = 2; root->pi[i][k - 1]; k++);
    for (j = 1; (n = root->pi[i][j]); j++)
    {
      if (root->pi[i][k][j - 1] == '>')
        continue;               // not pre-root
      while (len + strlen(t = root->pi[i][0]) + strlen(n) + 7 > max)
        s = realloc(s, max += XML_BUFFER_SIZE);
      len += sprintf(s + len, "<?%s%s%s?>\n", t, *n ? " " : "", n);
    }
  }

  xml->parent = xml->ordered = NULL;
  s = XML_ToString_r(xml, &s, &len, &max, 0, root->attr);
  xml->parent = p;
  xml->ordered = o;

  for (i = 0; !p && root->pi[i]; i++)
  {                             // post-root processing instructions
    for (k = 2; root->pi[i][k - 1]; k++);
    for (j = 1; (n = root->pi[i][j]); j++)
    {
      if (root->pi[i][k][j - 1] == '<')
        continue;               // not post-root
      while (len + strlen(t = root->pi[i][0]) + strlen(n) + 7 > max)
        s = realloc(s, max += XML_BUFFER_SIZE);
      len += sprintf(s + len, "\n<?%s%s%s?>", t, *n ? " " : "", n);
    }
  }
  return realloc(s, len + 1);
}

// free the memory allocated for the xml_st structure
void XML_Free(XML_T xml)
{
  XML_ROOT_T root = (XML_ROOT_T) xml;
  int i, j;
  char **a, *s;

  if (!xml)
    return;
  XML_Free(xml->child);
  XML_Free(xml->ordered);

  if (!xml->parent)
  {                             // free root tag allocations
    for (i = 10; root->ent[i]; i += 2)  // 0 - 9 are default entites (<>&"')
      if ((s = root->ent[i + 1]) < root->s || s > root->e)
        free(s);
    free(root->ent);            // free list of general entities

    for (i = 0; (a = root->attr[i]); i++)
    {
      for (j = 1; a[j++]; j += 2) // free malloced attribute values
        if (a[j] && (a[j] < root->s || a[j] > root->e))
          free(a[j]);
      free(a);
    }
    if (root->attr[0])
      free(root->attr);         // free default attribute list

    for (i = 0; root->pi[i]; i++)
    {
      for (j = 1; root->pi[i][j]; j++);
      free(root->pi[i][j + 1]);
      free(root->pi[i]);
    }
    if (root->pi[0])
      free(root->pi);           // free processing instructions

    if (root->len == -1)
      free(root->m);            // malloced xml data
    if (root->u)
      free(root->u);            // utf8 conversion
  }

  XML_Free_attr(xml->attr);   // tag attributes
  if ((xml->flags & XML_FLAG_DUP_CONTENT))
    free(xml->txt);             // character content
  if ((xml->flags & XML_FLAG_DUP_NAME))
    free(xml->name);            // tag name
  free(xml);
}

// return parser error message or empty string if none
const char *XML_Error(XML_T xml)
{
  while (xml && xml->parent)
    xml = xml->parent;          // find root tag

  if (xml == NULL)
    return "wrong parameter";

  if (strlen(((XML_ROOT_T) xml)->err))
    return ((XML_ROOT_T) xml)->err;

  return NULL;
}

// returns a new empty xml_st structure with the given root tag name
XML_T XML_AllocEx(const char *name)
{
  static char *ent[] = { "lt;", "&#60;", "gt;", "&#62;", "quot;", "&#34;",
    "apos;", "&#39;", "amp;", "&#38;", NULL
  };
  XML_ROOT_T root = (XML_ROOT_T) memset(malloc(sizeof(struct xml_root_st)),
                                            '\0', sizeof(struct xml_root_st));

  root->xml.name = (char *) name;
  root->cur = &root->xml;
  strcpy(root->err, root->xml.txt = "");
  root->ent = memcpy(malloc(sizeof(ent)), ent, sizeof(ent));
  root->attr = root->pi = (char ***) (root->xml.attr = XML_NULL_ARRAY);
  return &root->xml;
}

// inserts an existing tag into an xml_st structure
XML_T XML_Insert(XML_T xml, XML_T dest, size_t off)
{
  XML_T cur, prev, head;

  xml->next = xml->sibling = xml->ordered = NULL;
  xml->off = off;
  xml->parent = dest;

  if ((head = dest->child))
  {                             // already have sub tags
    if (head->off <= off)
    {                           // not first subtag
      for (cur = head; cur->ordered && cur->ordered->off <= off;
           cur = cur->ordered);
      xml->ordered = cur->ordered;
      cur->ordered = xml;
    } else
    {                           // first subtag
      xml->ordered = head;
      dest->child = xml;
    }

    for (cur = head, prev = NULL; cur && strcmp(cur->name, xml->name); prev = cur, cur = cur->sibling); // find tag type
    if (cur && cur->off <= off)
    {                           // not first of type
      while (cur->next && cur->next->off <= off)
        cur = cur->next;
      xml->next = cur->next;
      cur->next = xml;
    } else
    {                           // first tag of this type
      if (prev && cur)
        prev->sibling = cur->sibling; // remove old first
      xml->next = cur;          // old first tag is now next
      for (cur = head, prev = NULL; cur && cur->off <= off; prev = cur, cur = cur->sibling);  // new sibling insert point
      xml->sibling = cur;
      if (prev)
        prev->sibling = xml;
    }
  } else
    dest->child = xml;          // only sub tag

  return xml;
}

// Adds a child tag. off is the offset of the child tag relative to the start
// of the parent tag's character content. Returns the child tag.
XML_T XML_AddChild(XML_T xml, const char *name, size_t off)
{
  XML_T child;

  if (!xml)
    return NULL;
  child = (XML_T) memset(malloc(sizeof(struct xml_st)), '\0',
                           sizeof(struct xml_st));
  child->name = (char *) name;
  child->attr = XML_NULL_ARRAY;
  child->txt = "";

  return XML_Insert(child, xml, off);
}

// sets the character content for the given tag and returns the tag
XML_T XML_SetContentEx(XML_T xml, const char *txt)
{
  if (!xml)
    return NULL;
  if (xml->flags & XML_FLAG_DUP_CONTENT)
    free(xml->txt);             // existing txt was malloced
  xml->flags &= ~XML_FLAG_DUP_CONTENT;
  xml->txt = (char *) txt;
  return xml;
}

// Sets the given tag attribute or adds a new attribute if not found. A value
// of NULL will remove the specified attribute. Returns the tag given.
XML_T XML_SetAttributeEx(XML_T xml, const char *name, const char *value)
{
  int l = 0, c;

  if (!xml)
    return NULL;
  while (xml->attr[l] && strcmp(xml->attr[l], name))
    l += 2;
  if (!xml->attr[l])
  {                             // not found, add as new attribute
    if (!value)
      return xml;               // nothing to do
    if (xml->attr == XML_NULL_ARRAY)
    {                           // first attribute
      xml->attr = malloc(4 * sizeof(char *));
      xml->attr[1] = strdup("");  // empty list of malloced names/vals
    } else
      xml->attr = realloc(xml->attr, (l + 4) * sizeof(char *));

    xml->attr[l] = (char *) name; // set attribute name
    xml->attr[l + 2] = NULL;    // null terminate attribute list
    xml->attr[l + 3] = realloc(xml->attr[l + 1],
                               (c = strlen(xml->attr[l + 1])) + 2);
    strcpy(xml->attr[l + 3] + c, " ");  // set name/value as not malloced
    if (xml->flags & XML_FLAG_DUP_ATTRIBUTE)
      xml->attr[l + 3][c] = (char) XML_FLAG_DUP_NAME; // JDA
  } else if (xml->flags & XML_FLAG_DUP_ATTRIBUTE)
    free((char *) name);        // name was strduped

  for (c = l; xml->attr[c]; c += 2);  // find end of attribute list
  if (xml->attr[c + 1][l / 2] & XML_FLAG_DUP_CONTENT)
    free(xml->attr[l + 1]);     //old val
  if (xml->flags & XML_FLAG_DUP_ATTRIBUTE)
    xml->attr[c + 1][l / 2] |= XML_FLAG_DUP_CONTENT;
  else
    xml->attr[c + 1][l / 2] &= ~XML_FLAG_DUP_CONTENT;

  if (value)
    xml->attr[l + 1] = (char *) value;  // set attribute value
  else
  {                             // remove attribute
    if (xml->attr[c + 1][l / 2] & XML_FLAG_DUP_NAME)
      free(xml->attr[l]);
    memmove(xml->attr + l, xml->attr + l + 2, (c - l + 2) * sizeof(char *));
    xml->attr = realloc(xml->attr, (c + 2) * sizeof(char *));
    memmove(xml->attr[c + 1] + (l / 2), xml->attr[c + 1] + (l / 2) + 1, (c / 2) - (l / 2)); // fix list of which name/vals are malloced
  }
  xml->flags &= ~XML_FLAG_DUP_ATTRIBUTE;     // clear strdup() flag
  return xml;
}

// sets a flag for the given tag and returns the tag
XML_T XML_SetFlag(XML_T xml, short flag)
{
  if (xml)
    xml->flags |= flag;
  return xml;
}

// removes a tag along with its subtags without freeing its memory
XML_T XML_Cut(XML_T xml)
{
  XML_T cur;

  if (!xml)
    return NULL;                // nothing to do
  if (xml->next)
    xml->next->sibling = xml->sibling;  // patch sibling list

  if (xml->parent)
  {                             // not root tag
    cur = xml->parent->child;   // find head of subtag list
    if (cur == xml)
      xml->parent->child = xml->ordered;  // first subtag
    else
    {                           // not first subtag
      while (cur->ordered != xml)
        cur = cur->ordered;
      cur->ordered = cur->ordered->ordered; // patch ordered list

      cur = xml->parent->child; // go back to head of subtag list
      if (strcmp(cur->name, xml->name))
      {                         // not in first sibling list
        while (strcmp(cur->sibling->name, xml->name))
          cur = cur->sibling;
        if (cur->sibling == xml)
        {                       // first of a sibling list
          cur->sibling = (xml->next) ? xml->next : cur->sibling->sibling;
        } else
          cur = cur->sibling;   // not first of a sibling list
      }

      while (cur->next && cur->next != xml)
        cur = cur->next;
      if (cur->next)
        cur->next = cur->next->next;  // patch next list
    }
  }
  xml->ordered = xml->sibling = xml->next = NULL;
  return xml;
}

#ifdef EZXML_TEST               // test harness
int main(int argc, char **argv)
{
  XML_T xml;
  char *s;
  int i;

  if (argc != 2)
    return fprintf(stderr, "usage: %s xmlfile\n", argv[0]);

  xml = XML_ParseFile(argv[1]);
  printf("%s\n", (s = XML_ToString(xml)));
  free(s);
  i = fprintf(stderr, "%s", XML_Error(xml));
  XML_Free(xml);
  return (i) ? 1 : 0;
}
#endif // EZXML_TEST
