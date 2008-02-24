
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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <warc.h>

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
#define makeWString(s) (bless (WString, ((warc_u8_t *)s), w_strlen((warc_u8_t *) (s))))

int test1 (void)
{
  const char * t = "TEST 1";
  void       * s = NIL;

  fprintf (stdout, "%s>\n", t);

  /* empty string */
  s = makeWString ("");
  assert (s);

  fprintf (stdout, "[%s]\n", WString_getText   (s) );
  fprintf (stdout, "%d\n",   WString_getLength (s) );

  destroy (s);

  return 0;
}


int test2 (void)
{
  const char * t = "TEST 2";
  void       * s = NIL;

  fprintf (stdout, "%s>\n", t);

  /* non empty string */
  s = makeWString ("AABBCCDDEEF");
  assert (s);

  fprintf (stdout, "[%s]\n", WString_getText   (s) );
  fprintf (stdout, "%d\n", WString_getLength (s) );

  destroy (s);

  return 0;
}

int test3 (void)
{
  const char * t = "TEST 3";
  void       * s = NIL;

  fprintf (stdout, "%s>\n", t);

  /* non empty string */
  s = makeWString ("AA");
  assert (s);

  fprintf (stdout, "[%s]\n", WString_getText   (s) );
  fprintf (stdout, "%d\n", WString_getLength (s) );

  /* set string text */
  WString_setText (s, makeS ("BBBBB") );
  fprintf (stdout, "[%s]\n", WString_getText   (s) );
  fprintf (stdout, "%d\n", WString_getLength (s) );

  /* set string text */
  WString_setText (s, makeS ("AABBCCDDEEF") );
  fprintf (stdout, "[%s]\n", WString_getText   (s) );
  fprintf (stdout, "%d\n", WString_getLength (s) );

  destroy (s);

  return 0;
}

int test4 (void)
{
  const char * t = "TEST 4";
  void       * a = NIL;
  void       * b = NIL;

  fprintf (stdout, "%s>\n", t);

  a = makeWString ("ABCD");
  assert (a);

  fprintf (stdout, "a: [%s]\n", WString_getText   (a) );
  fprintf (stdout, "a: %d\n",   WString_getLength (a) );

  b = makeWString ("EF");
  assert (b);

  /* concatenate string "a" + string "b" and save the result in "a" */
  assert (! WString_concat (a, b) );
  fprintf (stdout, "a: [%s]\n", WString_getText   (a) );
  fprintf (stdout, "a: %d\n",   WString_getLength (a) );

  fprintf (stdout, "b: [%s]\n", WString_getText   (a) );
  fprintf (stdout, "b: %d\n",   WString_getLength (a) );

  destroy (b);
  destroy (a);

  return 0;
}


int test5 (void)
{
  const char          * t = "TEST 5";
  const warc_u8_t * x = (unsigned char *) "better";
  const warc_u8_t * y = (unsigned char *) "c++";
  void                * s = NIL;

  fprintf (stdout, "%s>\n", t);

  /* non empty string */
  s = makeWString ("warc is better than arc");
  assert (s);

  fprintf (stdout, "string is : %s\n", WString_getText (s) );

  fprintf (stdout, "found substring \"%s\" at index %d\n",
           x, WString_strstr (s, x) );

  fprintf (stdout, "found substring \"%s\" at index %d\n",
           y, WString_strstr (s, y) );

  destroy (s);

  return 0;
}


int test6 (void)
{
  const char * t = "TEST 6";
  void       * s = NIL;

  fprintf (stdout, "%s>\n", t);

  /* non empty string */
  s = makeWString ("XML is ... ");
  assert (s);

  WString_append (s, makeS ("for the old") );
  fprintf (stdout, "string is : %s\n", WString_getText (s) );

  WString_append (s, makeS (" school. ") );
  fprintf (stdout, "string is : %s\n", WString_getText (s) );

  WString_append (s, makeS ("JSON is better !!!") );
  fprintf (stdout, "string is : %s\n", WString_getText (s) );

  destroy (s);

  return 0;
}


int main (void)
{
  int (* tests []) () = { test1, test2, test3,
                          test4, test5, test6
                        };

  warc_u32_t  i      = 0;

  for (i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
