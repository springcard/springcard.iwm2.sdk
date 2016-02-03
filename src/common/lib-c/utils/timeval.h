/*
* "Copyright (c) 2006 University of Southern California.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose, without fee, and without written
* agreement is hereby granted, provided that the above copyright
* notice, the following two paragraphs and the author appear in all
* copies of this software.
*
* IN NO EVENT SHALL THE UNIVERSITY OF SOUTHERN CALIFORNIA BE LIABLE TO
* ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
* DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
* DOCUMENTATION, EVEN IF THE UNIVERSITY OF SOUTHERN CALIFORNIA HAS BEEN
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* THE UNIVERSITY OF SOUTHERN CALIFORNIA SPECIFICALLY DISCLAIMS ANY
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
* PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
* SOUTHERN CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
* SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
*
*/

/**
 * Header file for 'struct timeval' manipulation functions.
 *
 * @author Jeongyeup Paek
 * Embedded Networks Laboratory, University of Southern California
 * @modified 2/5/2007
 **/

#ifndef _TIMEVAL_H_
#define _TIMEVAL_H_

#ifdef WIN32
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#endif

int compare_timeval(struct timeval *a, struct timeval *b);

void copy_timeval(struct timeval *dst, struct timeval *src);

int subtract_timeval(struct timeval *to, struct timeval *from, struct timeval *result);

void add_timeval(struct timeval *a, struct timeval *b, struct timeval *result);

void add_ms_to_timeval(struct timeval *a, unsigned long intval_ms, struct timeval *result);

void add_s_to_timeval(struct timeval *a, unsigned long interval_s, struct timeval *result);

unsigned long int tv2ms(struct timeval *a);

void ms2tv(struct timeval *result, unsigned long interval_ms);

void clear_timeval(struct timeval *a);

unsigned long int gettimeofday_ms();

#endif

