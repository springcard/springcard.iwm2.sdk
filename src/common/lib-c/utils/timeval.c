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
 * File for 'struct timeval' manipulation functions.
 *
 * @author Jeongyeup Paek
 * Embedded Networks Laboratory, University of Southern California
 * @modified 2/5/2007
 **/

#include "timeval.h"
#ifdef WIN32
#include "gettimeofday.h"
#endif
#include <stdlib.h>

/******************************************
  timeval manipulation functions
******************************************/

/* return an integer greater than, equal to, or less than 0, 
   according as the timeval a is greater than, 
   equal to, or less than the timeval b. */
int compare_timeval(struct timeval *a, struct timeval *b) 
{
    if (a->tv_sec > b->tv_sec)
        return 1;
    else if (a->tv_sec < b->tv_sec)
        return -1;
    else if (a->tv_usec > b->tv_usec)
        return 1;
    else if (a->tv_usec < b->tv_usec)
        return -1;
    return 0;
}

/* copy timeval from src to dst */
void copy_timeval(struct timeval *dst, struct timeval *src) 
{
    dst->tv_sec = src->tv_sec;
    dst->tv_usec = src->tv_usec;
}

/* Perform timeval subtraction
    - "to - from = result"
    - return -1 if 'from' is larget(later) than 'to'
    - return 0 if success */
int subtract_timeval(struct timeval *to, struct timeval *from, struct timeval *result) 
{
    if (compare_timeval(to, from) < 0)
        return -1;
    result->tv_sec = to->tv_sec - from->tv_sec;
    result->tv_usec = to->tv_usec - from->tv_usec;
    if(result->tv_usec < 0)    
		{
        result->tv_sec--;
        result->tv_usec += 1000000;
    }    
    return 0;
}

void add_timeval(struct timeval *a, struct timeval *b, struct timeval *result) 
{
    result->tv_sec = a->tv_sec + b->tv_sec;
    result->tv_usec = a->tv_usec + b->tv_usec;
    if (result->tv_usec > 1000000) 
		{
        result->tv_sec++;
        result->tv_usec -= 1000000;
    }
}

/* Adds 'interval_ms' to timeval 'a' and store in 'result'
    - 'interval_ms' is in milliseconds */
void add_ms_to_timeval(struct timeval *a, unsigned long interval_ms, struct timeval *result) 
{
    result->tv_sec = a->tv_sec + (interval_ms / 1000);
    result->tv_usec = a->tv_usec + ((interval_ms % 1000) * 1000);
    if (result->tv_usec > 1000000) 
		{
        result->tv_usec -= 1000000;
        result->tv_sec++;
    }
}

void add_s_to_timeval(struct timeval *a, unsigned long interval_s, struct timeval *result) 
{
    result->tv_sec = a->tv_sec + interval_s;
    result->tv_usec = a->tv_usec;
}

/* convert struct timeval to ms(milliseconds) */
unsigned long int tv2ms(struct timeval *a) 
{
    return ((a->tv_sec * 1000) + (a->tv_usec / 1000));
}

/* convert ms(milliseconds) to timeval struct */
void ms2tv(struct timeval *result, unsigned long interval_ms) 
{
    result->tv_sec = (interval_ms / 1000);
    result->tv_usec = ((interval_ms % 1000) * 1000);
}

void clear_timeval(struct timeval *a) 
{
    a->tv_sec = 0;
    a->tv_usec = 0;
}

unsigned long int gettimeofday_ms() 
{
    struct timeval curr;
    gettimeofday(&curr, NULL);
    return tv2ms(&curr);
}

