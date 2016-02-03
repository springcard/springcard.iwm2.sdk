#ifndef __XMLFILE_H__
#define __XMLFILE_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>


#define XML_FLAG_DUP_NAME       0x80      // name is malloced
#define XML_FLAG_DUP_CONTENT    0x40      // txt is malloced
#define XML_FLAG_DUP_ATTRIBUTE  0x20      // attribute name and value are strduped

typedef struct xml_st *XML_T;

struct xml_st
{
  char *name;                   // tag name
  char **attr;                  // tag attributes { name, value, name, value, ... NULL }
  char *txt;                    // tag character content, empty string if none
  size_t off;                   // tag offset from start of parent tag character content
  XML_T next;                 // next tag with same name in this section at this depth
  XML_T sibling;              // next tag with different name in same section and depth
  XML_T ordered;              // next tag, same section and depth, in original order
  XML_T child;                // head of sub tag list, NULL if none
  XML_T parent;               // parent tag, NULL if current tag is root tag
  short flags;                  // additional information
};

// Given a string of xml data and its length, parses it and creates an xml_st
// structure. For efficiency, modifies the data by adding null terminators
// and decoding ampersand sequences. If you don't want this, copy the data and
// pass in the copy. Returns NULL on failure.
XML_T XML_ParseStr(char *s, size_t len);

// A wrapper for XML_ParseStr() that accepts a file descriptor. First
// attempts to mem map the file. Failing that, reads the file into memory.
// Returns NULL on failure.
XML_T XML_ParseFd(int fd);

// a wrapper for XML_ParseFd() that accepts a file name
XML_T XML_ParseFile(const char *file);

// Wrapper for XML_ParseStr() that accepts a file stream. Reads the entire
// stream into memory and then parses it. For xml files, use XML_ParseFile()
// or XML_ParseFd()
XML_T XML_ParseFp(FILE * fp);

// returns the first child tag (one level deeper) with the given name or NULL
// if not found
XML_T XML_Child(XML_T xml, const char *name);

// returns the next tag of the same name in the same section and depth or NULL
// if not found
#define XML_Next(xml) ((xml) ? xml->next : NULL)

// Returns the Nth tag with the same name in the same section at the same depth
// or NULL if not found. An index of 0 returns the tag given.
XML_T XML_Walk(XML_T xml, int idx);

// returns the name of the given tag
#define XML_Name(xml) ((xml) ? xml->name : NULL)

// returns the given tag's character content or empty string if none
#define XML_Content(xml) ((xml) ? xml->txt : "")

// returns the value of the requested tag attribute, or NULL if not found
const char *XML_Attribute(XML_T xml, const char *attr);

// Traverses the xml_st sturcture to retrieve a specific subtag. Takes a
// variable length list of tag names and indexes. The argument list must be
// terminated by either an index of -1 or an empty string tag name. Example: 
// title = XML_Get(library, "shelf", 0, "book", 2, "title", -1);
// This retrieves the title of the 3rd book on the 1st shelf of library.
// Returns NULL if not found.
XML_T XML_Get(XML_T xml, ...);

// Converts an xml_st structure back to xml. Returns a string of xml data that
// must be freed.
char *XML_ToString(XML_T xml);

// returns a NULL terminated array of processing instructions for the given
// target
const char **XML_PI(XML_T xml, const char *target);

// frees the memory allocated for an xml_st structure
void XML_Free(XML_T xml);

// returns parser error message or empty string if none
const char *XML_Error(XML_T xml);

// returns a new empty xml_st structure with the given root tag name
XML_T XML_AllocEx(const char *name);

// wrapper for XML_Alloc() that strdup()s name
#define XML_Alloc(name) XML_SetFlag(XML_AllocEx(strdup(name)), XML_FLAG_DUP_NAME)

// Adds a child tag. off is the offset of the child tag relative to the start
// of the parent tag's character content. Returns the child tag.
XML_T XML_AddChild(XML_T xml, const char *name, size_t off);

// wrapper for XML_AddChild() that strdup()s name
#define XML_AddChild_d(xml, name, off) \
    XML_SetFlag(XML_AddChild(xml, strdup(name), off), XML_FLAG_DUP_NAME)

// sets the character content for the given tag and returns the tag
XML_T XML_SetContentEx(XML_T xml, const char *txt);

// wrapper for XML_SetContentEx() that strdup()s txt
#define XML_SetContent(xml, txt) \
    XML_SetFlag(XML_SetContentEx(xml, strdup(txt)), XML_FLAG_DUP_CONTENT)

// Sets the given tag attribute or adds a new attribute if not found. A value
// of NULL will remove the specified attribute. Returns the tag given.
XML_T XML_SetAttributeEx(XML_T xml, const char *name, const char *value);

// Wrapper for XML_SetAttributeEx() that strdup()s name/value. Value cannot be NULL
#define XML_SetAttribute(xml, name, value) \
    XML_SetAttributeEx(XML_SetFlag(xml, XML_FLAG_DUP_ATTRIBUTE), strdup(name), strdup(value))

// sets a flag for the given tag and returns the tag
XML_T XML_SetFlag(XML_T xml, short flag);

// removes a tag along with its subtags without freeing its memory
XML_T XML_Cut(XML_T xml);

// inserts an existing tag into an xml_st structure
XML_T XML_Insert(XML_T xml, XML_T dest, size_t off);

// Moves an existing tag to become a subtag of dest at the given offset from
// the start of dest's character content. Returns the moved tag.
#define XML_Move(xml, dest, off) XML_Insert(XML_Cut(xml), dest, off)

// removes a tag along with all its subtags
#define XML_Remove(xml) XML_Free(XML_Cut(xml))

#endif // _EZXML_H
