#if 0
/**********************************************************************
	Copyright 1988, 1989, 1991, 1992 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#ifdef linux
#include <stdlib.h>
#include <string.h>
#endif


#include "asn1.h"
#include "snmp.h"
#include "snmp_impl.h"
#include "snmp_api.h"
#include "parse.h"

#include "mib.h"

/* fwd: */
static void sprint_by_type();
static void set_functions();
static int parse_subtree();
static int lc_cmp();
static struct tree *get_symbol();
#if 0
/* moved to mib.h with external decl to allow external use: */
static void sprint_objid();
static void sprint_variable();
#endif

#define bool int

/* defined types table (from the ASN.1, SMI, RFC 1065) */
/* only filled as needed */
static char *stringtable[] = {
"UNIVERSAL PRIMITIVE",	"BOOLEAN",	"INTEGER",	"BIT STRING",
"OCTET STRING",	"NULL",		"OBJECT IDENTIFIER",	"",
"",		"",		"",		"",
"",		"",		"",		"",
"sequence",	"set",		"",		"",
"",		"",		"",		"",
"",		"",		"",		"",
"",		"",		"",		"",
"contructor",	"",		"",		"",
"",		"",		"",		"",
"",		"",		"",		"",
"",		"",		"",		"",
"",		"",		"",		"",
"",		"",		"",		"",
"",		"",		"",		"",
"",		"",		"",		"",
"IpAddress",	"Counter",	"Gauge",	"Timeticks",
"Opaque",	"NsapAddress",	"Counter64",	"UnsignedInteger32",
NULL
};

/*  	formerly used by sprint_*() functions:
 *
 * "UNIVERSAL PRIMITIVE", "BOOLEAN", "" (unused: "INT"), "BIT_STRING",
 * "", "NULL", "OID", "", ...
 * "IpAddress", "", "Gauge", "Timeticks",
 * "OPAQUE", "NsapAddress", "Counter64", "",
 */

/*	formerly used by check_*() functions:
 *
 * "UNIVERSAL PRIMITIVE", "BOOLEAN", "INTEGER", "BIT STRING",
 * "OCTET STRING", "NULL", "OBJECT IDENTIFIER", "", ...
 * "Ipaddress", "Counter", "Gauge", "Timeticks",
 * "Opaque", "NsapAddress", "Counter64", "UInteger32",
 */
static char *separator[16] ={
/* Separator="Wcew" White colon equal white */
"",	":",	"=",	":=",
" ",	" :",	" =",	" :=",
" ",	": ",	"= ",	":= ",
"  ",	" : ",	" = ",	" := "
};

static int Separator=14;
static int AddHexDumpMaxChar = 4;
static int HideOID = 0;
static int ShowType = 2;

static char*timeformat[4] ={
  /* Other libraries have the following format strings documented */
  "%ld", "%2$s", "(%ld) %s", NULL
};
static int TimeFormat = 2;

static bool
check_type(buf, var, type)
    char *buf;
    struct variable_list *var;
    u_char type;
{
    if (var->type != type) {
	if (ShowType > 0) {
	    sprintf(buf, "Wrong Type (should be %s): ", stringtable[type]);
	}
	buf += strlen(buf);
	sprint_by_type(buf, var, (struct enum_list *)NULL);
	return 0;
    }
    if (ShowType > 1) {
	sprintf(buf, "%s: ", stringtable[type]);
    }
    return 1;
}



static char *
uptimeString(timeticks, buf)
    u_long timeticks;
    char *buf;
{
    int	seconds, minutes, hours, days;

    timeticks /= 100;
    days = timeticks / (60 * 60 * 24);
    timeticks %= (60 * 60 * 24);

    hours = timeticks / (60 * 60);
    timeticks %= (60 * 60);

    minutes = timeticks / 60;
    seconds = timeticks % 60;

    if (days == 0){
	sprintf(buf, "%d:%02d:%02d", hours, minutes, seconds);
    } else if (days == 1) {
	sprintf(buf, "%d day, %d:%02d:%02d", days, hours, minutes, seconds);
    } else {
	sprintf(buf, "%d days, %d:%02d:%02d", days, hours, minutes, seconds);
    }
    return buf;
}

static void
concat_hexstring(buf, cp, len)
    char *buf;
    u_char  *cp;
    int	    len;
{
    if (! buf) {
      return;
    }
    buf += strlen(buf);
    /*
     * now starting former:
     * sprint_hexstring()
     */

    for(; len >= 16; len -= 16){
	sprintf(buf, "%02X %02X %02X %02X %02X %02X %02X %02X ",
		cp[0], cp[1], cp[2], cp[3], cp[4], cp[5], cp[6], cp[7]);
	buf += strlen(buf);
	cp += 8;
	sprintf(buf, "%02X %02X %02X %02X %02X %02X %02X %02X\n",
		cp[0], cp[1], cp[2], cp[3], cp[4], cp[5], cp[6], cp[7]);
	buf += strlen(buf);
	cp += 8;
    }
    for(; len > 0; len--){
	sprintf(buf, "%02X ", *cp++);
	buf += strlen(buf);
    }
    *buf = '\0';
}

static void
concat_asciistring(buf, cp, len)
    char *buf;
    u_char  *cp;
    int	    len;
{
    int	x = 0;

    buf   += strlen(buf);
    *buf++ = '"';
    for (x = 0; x < len; x++) {
	if (isprint(*cp)){
	    *buf++ = *cp++;
	} else {
	    *buf++ = '.';
	    cp++;
	}
#if 0
	if ((x % 48) == 47)
	    *buf++ = '\n';
#endif
    }
    *buf++ = '"';
    *buf = '\0';
}

#ifdef UNUSED
int
read_rawobjid(input, output, out_len)
    char *input;
    oid *output;
    int	*out_len;
{
    char    buf[12], *cp;
    oid	    *op = output;
    u_long  subid;

    while(*input != '\0'){
	if (!isdigit(*input))
	    break;
	cp = buf;
	while(isdigit(*input))
	    *cp++ = *input++;
	*cp = '\0';
	subid = atoi(buf);
	if(subid > MAX_SUBID){
	    fprintf(stderr, "sub-identifier too large: %s\n", buf);
	    return 0;
	}
	if((*out_len)-- <= 0){
	    fprintf(stderr, "object identifier too long\n");
	    return 0;
	}
	*op++ = subid;
	if(*input++ != '.')
	    break;
    }
    *out_len = op - output;
    if (*out_len == 0)
	return 0;
    return 1;
}

#endif /* UNUSED */

/*
  0
  < 4
  hex

  0 ""
  < 4 hex Hex: oo oo oo
  < 4     "fgh" Hex: oo oo oo
  > 4 hex Hex: oo oo oo oo oo oo oo oo
  > 4     "this is a test"

  */

static bool
contains_nonascii(str, len)
    u_char  *str;
    int	    len;
{
    int x = 0;

    for (x = 0; x < len; x++, str++) {
	if (! (isprint(*str) || isspace(*str))) {
	    return 1;	/* fast way out, instead of a local variable */
	}
    }
    /* note: an empty string is always ascii */
    return 0;
}


static void
sprint_octet_string(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    int hex = 0;
    int len = var->val_len;
    u_char *val = var->val.string;

    if (!check_type(buf, var, ASN_OCTET_STR) ){
	return;
    }
    val = var->val.string;
    len = var->val_len;
    hex = contains_nonascii(val, len);
    if (! hex){
	concat_asciistring(buf, val, len);
    }
    if (hex || (len <= AddHexDumpMaxChar && len > 0) ){
	strcat(buf, " Hex: ");
	concat_hexstring(buf, val, len);
    }
}


static void
sprint_opaque(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list	    *enums;
{
    if (!check_type(buf, var, OPAQUE) ){
	return;
    }
    concat_hexstring(buf, var->val.string, var->val_len);
}


static void
sprint_object_identifier(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list	    *enums;
{
    if (!check_type(buf, var, ASN_OBJECT_ID) ){
	return;
    }
    buf += strlen(buf);
    sprint_objid(buf, (oid *)(var->val.objid), var->val_len / sizeof(oid));
}


static void
sprint_timeticks(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list	    *enums;
{
    char timebuf[32];

    if (! check_type(buf, var, TIMETICKS)) {
	return;
    }
    buf += strlen(buf);
    sprintf (buf, timeformat[TimeFormat],
	    *(u_long *)(var->val.integer), 
	    uptimeString(*(u_long *)(var->val.integer), timebuf));
}


static void
sprint_integer(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    char    *enum_string = NULL;

    if (!check_type(buf, var, ASN_INTEGER) ){
	return;
    }
    buf += strlen(buf);
    for (; enums; enums = enums->next)
	if (enums->value == *var->val.integer){
	    enum_string = enums->label;
	    break;
	}
    if (enum_string == NULL)
	sprintf(buf, "%ld", *var->val.integer);
    else
	sprintf(buf, "%s(%ld)", enum_string, *var->val.integer);
}


static void
sprint_uinteger(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    char    *enum_string = NULL;

    if (!check_type(buf, var, UINTEGER) ){
	return;
    }
    buf += strlen(buf);
    for (; enums; enums = enums->next)
	if (enums->value == *var->val.integer){
	    enum_string = enums->label;
	    break;
	}
    if (enum_string == NULL)
	sprintf(buf, "%ld", *var->val.integer);
    else
	sprintf(buf, "%s(%ld)", enum_string, *var->val.integer);
}


static void
sprint_gauge(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    if (!check_type(buf, var, GAUGE) ){
	return;
    }
    buf += strlen(buf);
    sprintf(buf, "%lu", *var->val.integer);
}


static void
sprint_counter(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    if (!check_type(buf, var, COUNTER) ){
	return;
    }
    buf += strlen(buf);
    sprintf(buf, "%lu", *var->val.integer);
}


static void
sprint_networkaddress(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    int x, len;
    u_char *cp;

    sprintf(buf, "Network Address: ");
    buf += strlen(buf);
    cp = var->val.string;    
    len = var->val_len;
    for(x = 0; x < len; x++){
	sprintf(buf, "%02X", *cp++);
	buf += strlen(buf);
	if (x < (len - 1))
	    *buf++ = ':';
    }
}

static void
sprint_ipaddress(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    u_char *ip;

    if (!check_type(buf, var, IPADDRESS) ){
	return;
    }
    buf += strlen(buf);
    ip = var->val.string;
    sprintf(buf, "%d.%d.%d.%d",ip[0], ip[1], ip[2], ip[3]);
}

#if 0
static void
sprint_unsigned_short(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list	    *enums;
{
    if (var->type != ASN_INTEGER){
	sprintf(buf, "Wrong Type (should be INTEGER): ");
	buf += strlen(buf);
	sprint_by_type(buf, var, (struct enum_list *)NULL);
	return;
    }
    sprintf(buf, "INT: %lu", *var->val.integer);
}
#endif


static void
sprint_null(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    if (!check_type(buf, var, ASN_NULL) ){
	return;
    }
    strcat(buf, "NULL");
}


static void
sprint_bitstring(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    int len, bit;
    u_char *cp;
    char *enum_string;

    if (!check_type(buf, var, ASN_BIT_STR) ){
	return;
    }
    concat_hexstring(buf, var->val.bitstring, var->val_len);
    buf += strlen(buf);

    cp = var->val.bitstring + 1;
    for(len = 0; len < var->val_len - 1; len++){
	for(bit = 0; bit < 8; bit++){
	    if (*cp & (0x80 >> bit)){
		enum_string = NULL;
		for (; enums; enums = enums->next)
		    if (enums->value == (len * 8) + bit){
			enum_string = enums->label;
			break;
		    }
		if (enum_string == NULL)
		    sprintf(buf, "%d ", (len * 8) + bit);
		else
		    sprintf(buf, "%s(%d) ", enum_string, (len * 8) + bit);
		buf += strlen(buf);
	    }
	}
	cp ++;	    
    }
}


static void
sprint_nsapaddress(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    if (!check_type(buf, var, NSAP) ){
	return;
    }
    concat_hexstring(buf, var->val.string, var->val_len);
}


static void
sprint_counter64(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
    if (!check_type(buf, var, COUNTER64) ){
	return;
    }
/* XXX */
    concat_hexstring(buf, &var->val.counter64->high,
		     sizeof(var->val.counter64->high));
    concat_hexstring(buf, &var->val.counter64->low,
		     sizeof(var->val.counter64->low));
}


static void
sprint_unknowntype(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list *enums;
{
/*    sprintf(buf, "Variable has bad type"); */
    sprint_by_type(buf, var, NULL);
}

static void
sprint_badtype(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list	    *enums;
{
    sprintf(buf, "Variable has bad type");
}

static void
sprint_by_type(buf, var, enums)
    char *buf;
    struct variable_list *var;
    struct enum_list	    *enums;
{
    switch (var->type){
	case ASN_INTEGER:
	    sprint_integer(buf, var, enums);
	    break;
	case ASN_OCTET_STR:
	    sprint_octet_string(buf, var, enums);
	    break;
	case OPAQUE:
	    sprint_opaque(buf, var, enums);
	    break;
	case ASN_OBJECT_ID:
	    sprint_object_identifier(buf, var, enums);
	    break;
	case TIMETICKS:
	    sprint_timeticks(buf, var, enums);
	    break;
	case GAUGE:
	    sprint_gauge(buf, var, enums);
	    break;
	case COUNTER:
	    sprint_counter(buf, var, enums);
	    break;
	case IPADDRESS:
	    sprint_ipaddress(buf, var, enums);
	    break;
	case ASN_NULL:
	    sprint_null(buf, var, enums);
	    break;
	case UINTEGER:
	    sprint_uinteger(buf, var, enums);
	    break;
	default:
	    sprint_badtype(buf, var, enums);
	    break;
    }
}

struct tree *get_symbol();

oid RFC1213_MIB[] = { 1, 3, 6, 1, 2, 1 };
unsigned char RFC1213_MIB_text[] = ".iso.org.dod.internet.mgmt.mib-2";
unsigned char EXPERIMENTAL_MIB_text[] = ".iso.org.dod.internet.experimental";
unsigned char PRIVATE_MIB_text[] = ".iso.org.dod.internet.private";
unsigned char PARTY_MIB_text[] = ".iso.org.dod.internet.snmpParties";
unsigned char SECRETS_MIB_text[] = ".iso.org.dod.internet.snmpSecrets";
struct tree *Mib = 0;

static char Standard_Prefix[] = ".1.3.6.1.2.1.";
static char Prefix[256];
static int Suffix;


static void
configure (options)
    char* options;
{
  /**    char *getenv(); **/
    int opt_len = 0;
    int x;

    Suffix = FALSE;
    HideOID = 0;
    ShowType = 0;
    Separator = 0;
    TimeFormat = 0;

    opt_len = strlen(options);
    for( x = 0; x < opt_len; x++ ){
	switch(options[x])
	{
	case 's':
		Suffix = TRUE;
		break;
	case 't':
		ShowType = 2;
		break;
	case 'T':
		ShowType = 1;
		break;
	case '#':
		/* intended to print OIDs numerically */
		break;
	case 'f':
		TimeFormat = 2;
		break;
	case 'F':
		TimeFormat = 1;
		break;
	case 'c':
		Separator |= 1;
		break;
	case 'e':
		Separator |= 2;
		break;
	case 'W':
		Separator |= 4;
		break;
	case 'w':
		Separator |= 8;
		break;
	case 'h':
		HideOID = 1;
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		AddHexDumpMaxChar = atoi(&options[x]);
		while( isdigit(options[x+1]) ) x++;
	default:
		break;
	}
    }
}


void
init_mib()
{
    initmib( NULL );
}

/*
 * the old interface is kept alive
 * the new interface is now accessible
 */
void
initmib( cmuoptions )
    char* cmuoptions;
{
    char *file, *getenv(), *prefix;

    Mib = 0;
    file = getenv("MIBFILE");
    if (file) {
      if ((Mib = read_mib(file))) {
	fprintf (stderr, "info: using MIB-file from environment setting.\n");
      }
    }
    if (! Mib) {
      if ((Mib = read_mib("mib.txt"))) {
	fprintf (stderr, "info: using MIB-file in the working directory.\n");
      }
    }
#ifdef MIBFILEPATH
    if (! Mib) {
	char tmp [1024];
	sprintf (tmp, "%s/mib.txt", MIBFILEPATH);
	Mib = read_mib(tmp);
    }
#endif
    if (! Mib) {
      if ((Mib = read_mib("/etc/mib.txt"))) {
	fprintf (stderr, "info: using MIB-file in /etc directory.\n");
      }
    }
    if (! Mib) {
	fprintf(stderr, "Error: couldn't find mib file.\n");
	exit(2);
    }

    /* XXXXXX */

    prefix = getenv("PREFIX");
    if (! prefix) {
      prefix = Standard_Prefix;
    }

    /* save prefix: */
    snmp_new_prefix (prefix);

    if (getenv("SUFFIX"))
	Suffix = TRUE;
    else
	Suffix = FALSE;
    set_functions(Mib);

    /* XXXXXXXXXXXX */


    if (cmuoptions == NULL) {
	cmuoptions = getenv("CMUSNMP");
    }
    if (cmuoptions != NULL) {  /* DIFFERS! from (strlen(cmuoptions)!=0) */
	configure(cmuoptions);
    }
}



/* 
 * Phil Wood <cpw@lanl.gov>:
 *
 * [...] I made an addition to mib.c to accomodate some old perl snmp
 * code for a program called vulture that used a global pointer to the
 * prefix to change things.  
 */

char *
snmp_new_prefix (prefix) 
  char *prefix;
{
  char *lastchar;
  int  plen;

  if (prefix) {
    lastchar = ".";   
    if (*prefix == '.') { prefix++; }
    if ((plen = strlen (prefix))) {
      lastchar = prefix + plen - 1; 
    }
    strncpy (Prefix, prefix, sizeof (Prefix) - 2);
    Prefix [sizeof (Prefix) - 2] = 0;
    if (*lastchar != '.') {
      Prefix [plen++] = '.';
      Prefix [plen] = 0;
    }
    return Prefix;
  }
  return NULL;
}



static void
set_functions(subtree)
    struct tree *subtree;
{
    for(; subtree; subtree = subtree->next_peer){
	switch(subtree->type){
	    case TYPE_OBJID:
		subtree->printer = sprint_object_identifier;
		break;
	    case TYPE_OCTETSTR:
		subtree->printer = sprint_octet_string;
		break;
	    case TYPE_INTEGER:
		subtree->printer = sprint_integer;
		break;
	    case TYPE_NETADDR:
		subtree->printer = sprint_networkaddress;
		break;
	    case TYPE_IPADDR:
		subtree->printer = sprint_ipaddress;
		break;
	    case TYPE_COUNTER:
		subtree->printer = sprint_counter;
		break;
	    case TYPE_GAUGE:
		subtree->printer = sprint_gauge;
		break;
	    case TYPE_TIMETICKS:
		subtree->printer = sprint_timeticks;
		break;
	    case TYPE_OPAQUE:
		subtree->printer = sprint_opaque;
		break;
	    case TYPE_NULL:
		subtree->printer = sprint_null;
		break;
	    case TYPE_BITSTRING:
		subtree->printer = sprint_bitstring;
		break;
	    case TYPE_NSAPADDRESS:
		subtree->printer = sprint_nsapaddress;
		break;
	    case TYPE_COUNTER64:
		subtree->printer = sprint_counter64;
		break;
	    case TYPE_UINTEGER:
		subtree->printer = sprint_uinteger;
		break;
	    case TYPE_OTHER:
	    default:
		subtree->printer = sprint_unknowntype;
		break;
	}
	set_functions(subtree->child_list);
    }
}

#ifdef testing
int snmp_dump_packet = 0;

main(argc, argv)
     int argc;
     char *argv[];
{
    oid objid[64];
    int objidlen = sizeof (objid);
    int count;
    struct variable variable;

    init_mib();
    if (argc < 2)
	print_subtree(Mib, 0);
    variable.type = ASN_INTEGER;
    variable.val.integer = 3;
    variable.val_len = 4;
    for (argc--; argc; argc--, argv++) {
	objidlen = sizeof (objid);
	printf("read_objid(%s) = %d\n",
	       argv[1], read_objid(argv[1], objid, &objidlen));
	for(count = 0; count < objidlen; count++)
	    printf("%d.", objid[count]);
	printf("\n");
	print_variable(objid, objidlen, &variable);
    }
}

#endif testing


#if 0
static struct tree *
find_rfc1213_mib(root)
    struct tree *root;
{
    oid *op = RFC1213_MIB;
    struct tree *tp;
    int len;

    for(len = sizeof(RFC1213_MIB)/sizeof(oid); len; len--, op++){
	for(tp = root; tp; tp = tp->next_peer){
	    if (tp->subid == *op){
		root = tp->child_list;
		break;
	    }
	}
	if (tp == NULL)
	    return NULL;
    }
    return root;
}
#endif

int read_objid(input, output, out_len)
    char *input;
    oid *output;
    int	*out_len;   /* number of subid's in "output" */
{
    struct tree *root = Mib;
    oid *op = output;
    char buf[512];

    bzero (buf, sizeof(buf));

    if (*input == '.')
	input++;
    else {
        strcpy(buf, Prefix);
	strcat(buf, input);
	input = buf;
    }

    if (root == NULL){
	fprintf(stderr, "Mib not initialized.  Exiting.\n");
	exit(1);
    }
    if ((*out_len =
	 parse_subtree(root, input, output, out_len)) == 0)
	return (0);
    *out_len += output - op;

    return (1);
}

#ifdef notdef
int read_objid(input, output, out_len)
    char *input;
    oid *output;
    int	*out_len;   /* number of subid's in "output" */
{
    struct tree *root = Mib;
    oid *op = output;
    int i;

    if (*input == '.')
	input++;
    else {
	root = find_rfc1213_mib(root);
	for (i = 0; i < sizeof (RFC1213_MIB)/sizeof(oid); i++) {
	    if ((*out_len)-- > 0)
		*output++ = RFC1213_MIB[i];
	    else {
		fprintf(stderr, "object identifier too long\n");
		return (0);
	    }
	}
    }

    if (root == NULL){
	fprintf(stderr, "Mib not initialized.  Exiting.\n");
	exit(1);
    }
    if ((*out_len =
	 parse_subtree(root, input, output, out_len)) == 0)
	return (0);
    *out_len += output - op;

    return (1);
}
#endif

static int
parse_subtree(subtree, input, output, out_len)
    struct tree *subtree;
    char *input;
    oid	*output;
    int	*out_len;   /* number of subid's */
{
    char buf[128], *to = buf;
    u_long subid = 0;
    struct tree *tp;

    /*
     * No empty strings.  Can happen if there is a trailing '.' or two '.'s
     * in a row, i.e. "..".
     */
    if ((*input == '\0') ||
	(*input == '.'))
	return (0);

    if (isdigit(*input)) {
	/*
	 * Read the number, then try to find it in the subtree.
	 */
	while (isdigit(*input)) {
	    subid *= 10;
	    subid += *input++ - '0';
	}
	for (tp = subtree; tp; tp = tp->next_peer) {
	    if (tp->subid == subid)
		goto found;
	}
	tp = NULL;
    }
    else {
	/*
	 * Read the name into a buffer.
	 */
	while ((*input != '\0') &&
	       (*input != '.')) {
	    *to++ = *input++;
	}
	*to = '\0';

	/*
	 * Find the name in the subtree;
	 */
	for (tp = subtree; tp; tp = tp->next_peer) {
	    if (lc_cmp(tp->label, buf) == 0) {
		subid = tp->subid;
		goto found;
	    }
	}

	/*
	 * If we didn't find the entry, punt...
	 */
	if (tp == NULL) {
	    fprintf(stderr, "sub-identifier not found: %s\n", buf);
	    return (0);
	}
    }

found:
    if(subid > (u_long)MAX_SUBID){
	fprintf(stderr, "sub-identifier too large: %s\n", buf);
	return (0);
    }

    if ((*out_len)-- <= 0){
	fprintf(stderr, "object identifier too long\n");
	return (0);
    }
    *output++ = subid;

    if (*input != '.')
	return (1);
    if ((*out_len =
	 parse_subtree(tp ? tp->child_list : NULL, ++input, output, out_len)) == 0)
	return (0);
    return (++*out_len);
}

void
sprint_objid(buf, objid, objidlen)
    char *buf;
    oid	    *objid;
    int	    objidlen;	/* number of subidentifiers */
{
    char    tempbuf[2048], *cp;
    struct tree    *subtree = Mib;

    *tempbuf = '.';	/* this is a fully qualified name */
    get_symbol(objid, objidlen, subtree, tempbuf + 1);
    if (Suffix){
	for(cp =tempbuf; *cp; cp++)
	    ;
	while(cp >= tempbuf){
	    if (isalpha(*cp))
		break;
	    cp--;
	}
	while(cp >= tempbuf){
	    if (*cp == '.')
		break;
	    cp--;
	}
	cp++;
	if (cp < tempbuf)
	    cp = tempbuf;

    } else {
	cp = tempbuf;
	if ((strlen(tempbuf) > strlen((char *)RFC1213_MIB_text))
	    && !bcmp(tempbuf, (char *)RFC1213_MIB_text,
		     strlen((char *)RFC1213_MIB_text))){
	    cp += sizeof(RFC1213_MIB_text);
	}
	if ((strlen(tempbuf) > strlen((char *)EXPERIMENTAL_MIB_text))
	    && !bcmp(tempbuf, (char *) EXPERIMENTAL_MIB_text,
		     strlen((char *)EXPERIMENTAL_MIB_text))){
            cp += sizeof(EXPERIMENTAL_MIB_text);
	}
	if ((strlen(tempbuf) > strlen((char *)PRIVATE_MIB_text))
	    && !bcmp(tempbuf, (char *) PRIVATE_MIB_text,
		     strlen((char *)PRIVATE_MIB_text))){
            cp += sizeof(PRIVATE_MIB_text);
	}
	if ((strlen(tempbuf) > strlen((char *)PARTY_MIB_text))
	    && !bcmp(tempbuf, (char *) PARTY_MIB_text,
		     strlen((char *)PARTY_MIB_text))){
            cp += sizeof(PARTY_MIB_text);
	}
	if ((strlen(tempbuf) > strlen((char *)SECRETS_MIB_text))
	    && !bcmp(tempbuf, (char *) SECRETS_MIB_text,
		     strlen((char *)SECRETS_MIB_text))){
            cp += sizeof(SECRETS_MIB_text);
	}
    }
    strcpy(buf, cp);
}

void
print_objid(objid, objidlen)
    oid	    *objid;
    int	    objidlen;	/* number of subidentifiers */
{
    char    buf[256];

    sprint_objid(buf, objid, objidlen);
    printf("%s\n", buf);
}


void
print_variable(objid, objidlen, variable)
    oid     *objid;
    int	    objidlen;
    struct  variable_list *variable;
{
    char    buf[2048];

    sprint_variable(buf, objid, objidlen, variable);
    printf("%s", buf);
}


void
sprint_variable(buf, objid, objidlen, variable)
    char *buf;
    oid     *objid;
    int	    objidlen;
    struct  variable_list *variable;
{
    char    tempbuf[2048];
    struct tree    *subtree = Mib;

    sprint_objid(buf, objid, objidlen);
    if( HideOID ) buf[0] = 0;
    strcat(buf, separator[Separator]);
    buf += strlen(buf);

    if (variable->type == SNMP_NOSUCHOBJECT)
	sprintf(buf, "No Such Object available on this agent\n");
    else if (variable->type == SNMP_NOSUCHINSTANCE)
	sprintf(buf, "No Such Instance currently exists\n");
    else if (variable->type == SNMP_ENDOFMIBVIEW)
	sprintf(buf, "No more variables left in this MIB View\n");
    else {
	*tempbuf = '.';	/* this is a fully qualified name */
	subtree = get_symbol(objid, objidlen, subtree, tempbuf + 1);
	buf += strlen(buf);
	if (subtree->printer)
	    (*subtree->printer)(buf, variable, subtree->enums);
	else {
	    sprint_by_type(buf, variable, subtree->enums);
	}
	strcat(buf, "\n");
    }
}

void
sprint_value(buf, objid, objidlen, variable)
    char *buf;
    oid     *objid;
    int	    objidlen;
    struct  variable_list *variable;
{
    char    tempbuf[2048];
    struct tree    *subtree = Mib;

    if (variable->type == SNMP_NOSUCHOBJECT)
	sprintf(buf, "No Such Object available on this agent\n");
    else if (variable->type == SNMP_NOSUCHINSTANCE)
	sprintf(buf, "No Such Instance currently exists\n");
    else if (variable->type == SNMP_ENDOFMIBVIEW)
	sprintf(buf, "No more variables left in this MIB View\n");
    else {
	subtree = get_symbol(objid, objidlen, subtree, tempbuf);
	if (subtree->printer)
	    (*subtree->printer)(buf, variable, subtree->enums);
	else {
	    sprint_by_type(buf, variable, subtree->enums);
	}
    }
}

void
print_value(objid, objidlen, variable)
    oid     *objid;
    int	    objidlen;
    struct  variable_list *variable;
{
    char    tempbuf[2048];

    sprint_value(tempbuf, objid, objidlen, variable);
    printf("%s\n", tempbuf);
}

static struct tree *
get_symbol(objid, objidlen, subtree, buf)
    oid	    *objid;
    int	    objidlen;
    struct tree    *subtree;
    char    *buf;
{
    struct tree    *return_tree = NULL;

    for(; subtree; subtree = subtree->next_peer){
	if (*objid == subtree->subid){
	    strcpy(buf, subtree->label);
	    goto found;
	}
    }

    /* subtree not found */
    while(objidlen--){	/* output rest of name, uninterpreted */
	sprintf(buf, "%lu.", *objid++);
	while(*buf)
	    buf++;
    }
    *(buf - 1) = '\0'; /* remove trailing dot */
    return NULL;

found:
    if (objidlen > 1){
	while(*buf)
	    buf++;
	*buf++ = '.';
	*buf = '\0';
	return_tree = get_symbol(objid + 1, objidlen - 1, subtree->child_list,
				 buf);
    } 
    if (return_tree != NULL)
	return return_tree;
    else
	return subtree;
}


static int
lc_cmp(s1, s2)
    char *s1, *s2;
{
    char c1, c2;

    while(*s1 && *s2){
	if (isupper(*s1))
	    c1 = tolower(*s1);
	else
	    c1 = *s1;
	if (isupper(*s2))
	    c2 = tolower(*s2);
	else
	    c2 = *s2;
	if (c1 != c2)
	    return ((c1 - c2) > 0 ? 1 : -1);
	s1++;
	s2++;
    }

    if (*s1)
	return -1;
    if (*s2)
	return 1;
    return 0;
}

/*
 * Clone of get_symbol that doesn't take a buffer argument
 */
static struct tree *
get_tree(objid, objidlen, subtree)
    oid     *objid;
    int     objidlen;
    struct tree    *subtree;
{
    struct tree    *return_tree = NULL;

    for(; subtree; subtree = subtree->next_peer){
        if (*objid == subtree->subid)
            goto found;
    }

    return NULL;

found:
    if (objidlen > 1)
        return_tree = get_tree(objid + 1, objidlen - 1, subtree->child_list);
    if (return_tree != NULL)
        return return_tree;
    else
        return subtree;
}


#if 0
static char *
get_description(objid, objidlen)
    oid     *objid;
    int     objidlen;   /* number of subidentifiers */
{
    struct tree    *subtree = Mib;

    subtree = get_tree(objid, objidlen, subtree);
    if (subtree)
        return (subtree->description);
    else
        return NULL;
}
#endif


#if 0
static void
print_description(objid, objidlen)
    oid     *objid;
    int     objidlen;   /* number of subidentifiers */
{
    char *desc = get_description(objid, objidlen);

    if (desc && desc[0] != '\0')
        printf("Description: \"%s\"\n", desc);
    else
        printf("No description\n");
}
#endif


static struct tree *
find_node(name, subtree)
    char *name;
    struct tree *subtree;
{
    struct tree *tp, *ret;

    for(tp = subtree; tp; tp = tp->next_peer){
	if (!strcasecmp(name, tp->label))
	    return tp;
	ret = find_node(name, tp->child_list);
	if (ret)
	    return ret;
    }
    return 0;
}


#if 0
static int
get_node(name, objid, objidlen)
    char *name;
    oid *objid;
    int *objidlen;
{
    struct tree *tp;
    oid newname[64], *op;

    tp = find_node(name, Mib);
    if (tp){
	for(op = newname + 63; op >= newname; op--){
	    *op = tp->subid;
	    tp = tp->parent;
	    if (tp == NULL)
		break;
	}
	if (newname + 64 - op > *objidlen)
	    return 0;
	*objidlen = newname + 64 - op;
	bcopy(op, objid, (newname + 64 - op) * sizeof(oid));
	return 1;
    } else {
	return 0;
    }

    
}
#endif
#endif