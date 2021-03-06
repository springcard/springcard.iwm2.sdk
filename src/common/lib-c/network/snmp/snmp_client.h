/*
 * snmp_client.h
 */

/***********************************************************
  Copyright 1988, 1989 by Carnegie Mellon University

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

#ifndef SNMP_CLIENT_H
#define SNMP_CLIENT_H

#undef _ANSI_ARGS_
#if (defined(__STDC__) && ! defined(NO_PROTOTYPE)) || defined(USE_PROTOTYPE)
# define _ANSI_ARGS_(x) x
#else
# define _ANSI_ARGS_(x) ()
#endif


#ifdef  __cplusplus
extern "C" {
#endif

struct synch_state
{
  int waiting;
  int status;
/* status codes */
#define STAT_SUCCESS  0
#define STAT_ERROR  1
#define STAT_TIMEOUT 2
  int reqid;
  struct snmp_pdu *pdu;
};

extern struct synch_state snmp_synch_state;

extern struct snmp_pdu *snmp_pdu_create _ANSI_ARGS_((int cmd));

extern struct snmp_pdu *snmp_fix_pdu _ANSI_ARGS_((struct snmp_pdu *pdu,
              int cmd));
extern char *snmp_errstring _ANSI_ARGS_((int errstat));

#ifdef  __cplusplus
};
#endif

#endif
