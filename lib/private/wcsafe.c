
/* ------------------------------------------------------------------- */
/* Copyright (c) 2007-2008 Hanzo Archives Limited.                     */
/*                                                                     */
/* Licensed under the Apache License, Version 2.0 (the "License");     */
/* you may not use this file except in compliance with the License.    */
/* You may obtain a copy of the License at                             */
/*                                                                     */
/*     http://www.apache.org/licenses/LICENSE-2.0                      */
/*                                                                     */
/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS,   */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or     */
/* implied.                                                            */
/* See the License for the specific language governing permissions and */
/* limitations under the License.                                      */
/*                                                                     */
/* You may find more information about Hanzo Archives at               */
/*                                                                     */
/*     http://www.hanzoarchives.com/                                   */
/*                                                                     */
/* You may find more information about the WARC Tools project at       */
/*                                                                     */
/*     http://code.google.com/p/warc-tools/                            */
/* ------------------------------------------------------------------- */

/*
 * Portability header file
 */

#include <wport.h>

/*
 * C default headers 
 */

#include <stddef.h>
#include <ctype.h>


#include <wmisc.h>
#include <wcsafe.h>


WPUBLIC ptrdiff_t w_strncpy (char * dst0, const char * src0, size_t size) 
{
#define INIT_BITS	3
#define INIT_SIZE	(1UL << (INIT_BITS))	/* must be power of two	*/
#define INIT_MASK	((INIT_SIZE) - 1)

  if (size) 
    {
      register size_t n   = (size + INIT_MASK) / INIT_SIZE;
      register char * dst = dst0;
      register char * src = (char *) src0;

      * (dst + size) = '\0';

      switch (size & INIT_MASK) {
       case 0:    do { if (!(*dst++ = *src++)) break;
       case 7:         if (!(*dst++ = *src++)) break;
       case 6:         if (!(*dst++ = *src++)) break;
       case 5:         if (!(*dst++ = *src++)) break;
       case 4:         if (!(*dst++ = *src++)) break;
       case 3:         if (!(*dst++ = *src++)) break;
       case 2:         if (!(*dst++ = *src++)) break;
       case 1:         if (!(*dst++ = *src++)) break;
       } while (--n > 0);
      }

      return ((ptrdiff_t) (dst - dst0 - 1));
    }

  return (0);
} 




/*
 * My personal strstr() implementation that beats most other algorithms.
 * Until someone tells me otherwise, I assume that this is the
 * fastest implementation of strstr() in C.
 * I deliberately chose not to comment it.  You should have at least
 * as much fun trying to understand it, as I had to write it :-).
 *
 * Stephen R. van den Berg, berg@pool.informatik.rwth-aachen.de	*/

/* Faster looping by precalculating bl, bu, cl, cu before looping.
 * 2004 Apr 08	Jose Da Silva, digital@joescat@com		*/

/* Port to WARC.
 * 2008 Feb 12	voidptrptr, Hanzo Archive Limited		*/

WPUBLIC char * w_strcasestr (const char * phaystack, const char * pneedle)
{
  register const unsigned char *haystack, *needle;
  register unsigned bl, bu, cl, cu;
  
  haystack = (const unsigned char *) phaystack;
  needle = (const unsigned char *) pneedle;
  
  bl = tolower (* needle);
  if (bl != '\0')
    {
      bu = _toupper (bl);
      haystack--;				/* possible ANSI violation */
      do
        {
          cl = *++haystack;
          if (cl == '\0')
            goto ret0;
        }
      while ((cl != bl) && (cl != bu));
      
      cl = tolower (*++needle);
      if (cl == '\0')
        goto foundneedle;
      cu = _toupper (cl);
      ++needle;
      goto jin;
      
      for (;;)
        {
          register unsigned a;
          register const unsigned char *rhaystack, *rneedle;
          
          do
            {
              a = *++haystack;
              if (a == '\0')
                goto ret0;
              if ((a == bl) || (a == bu))
                break;
              a = *++haystack;
              if (a == '\0')
                goto ret0;

            shloop:
              ;
            }
          while ((a != bl) && (a != bu));
          
        jin:	 
          a = *++haystack;
          if (a == '\0')
            goto ret0;
          
          if ((a != cl) && (a != cu))
            goto shloop;
          
          rhaystack = haystack-- + 1;
          rneedle = needle;
          a = tolower (*rneedle);
          
          if (tolower (*rhaystack) == (int) a)
            do
              {
                if (a == '\0')
                  goto foundneedle;
                ++rhaystack;
                a = tolower (*++needle);
                if (tolower (*rhaystack) != (int) a)
                  break;
                if (a == '\0')
                  goto foundneedle;
                ++rhaystack;
                a = tolower (*++needle);
              }
            while (tolower (*rhaystack) == (int) a);
          
          needle = rneedle;		/* took the register-poor approach */
          
          if (a == '\0')
            break;
        }
    }
 foundneedle:
  return (char*) haystack;
 ret0:
  return 0;
}




/* locate character in string */
WPUBLIC const char * w_index (const char *s, int c)
{
  while (* s)
    {
      if (* s == c)
        return (s);

      ++ s;
    }

  return (0);
}
