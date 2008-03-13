
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


#define makeS(s) s, strlen(s)
#define makeI(i) &i, sizeof(warc_u32_t)
#define makeWString(s) (bless (WString, ((warc_u8_t *)s), w_strlen((warc_u8_t *) (s))))

int test1 (void)
{
  const char * t = "TEST 1";
  void       * h = NIL;
  void       * s = NIL;
  char       * k = NIL;
  const void * v = NIL;

  fprintf (stdout, "%s>\n", t);

  h = bless (WHash, 101);
  assert (h);

  s = makeWString ("arc");
  assert (s);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  s = makeWString ("warc");
  assert (s);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  s = makeWString ("iipc");
  assert (s);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);


  k = "warc";
  v = WHash_get (h, makeS (k) );

  if (v)
    fprintf (stdout, "found key: %s\n", WString_getText (v) );
  else
    fprintf (stdout, "no key   : %s\n", k);

  k = "arc";

  v = WHash_get (h, makeS (k) );

  if (v)
    fprintf (stdout, "found key: %s\n", WString_getText (v) );
  else
    fprintf (stdout, "no key   : %s\n", k);

  k = "ziipc";

  v = WHash_get (h, makeS (k) );

  if (v)
    fprintf (stdout, "found key: %s\n", WString_getText (v) );
  else
    fprintf (stdout, "no key   : %s\n", k);


  destroy (h);

  return 0;
}



int test2 (void)
{
  const char * t = "TEST 2";
  void       * h = NIL;
  void       * s = NIL;
  char       * k = NIL;
  const void * v = NIL;

  fprintf (stdout, "%s>\n", t);

  h = bless (WHash, 101);
  assert (h);

  s = makeWString ("arc");
  assert (s);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  s = makeWString ("warc");
  assert (s);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  k = "warc";
  v = WHash_get (h, makeS (k) );

  if (v)
    fprintf (stdout, "found key: %s\n", WString_getText (v) );
  else
    fprintf (stdout, "no key   : %s\n", k);

  k = "arc";

  v = WHash_get (h, makeS (k) );

  if (v)
    fprintf (stdout, "found key: %s\n", WString_getText (v) );
  else
    fprintf (stdout, "no key   : %s\n", k);

  k = "warc";

  s = WHash_delete (h, makeS (k) );

  destroy (s); /* need to explicitly free the deleted object */

  v = WHash_get (h, makeS (k) );

  if (v)
    fprintf (stdout, "foudn key: %s\n", WString_getText (v) );
  else
    fprintf (stdout, "no key   : %s\n", k);



  destroy (h);

  return 0;
}


int test3 (void)
{
  const char * t = "TEST 3";
  void       * h = NIL;
  void       * s = NIL;
  const void * v = NIL;
  warc_u32_t   k;


  fprintf (stdout, "%s>\n", t);

  h = bless (WHash, 101);
  assert (h);

  s = makeWString ("arc");
  assert (s);
  k = 1;
  WHash_insert (h, makeI (k), s);

  s = makeWString ("iipc");
  assert (s);
  k = 2;
  WHash_insert (h, makeI (k), s);


  k = 1;
  v = WHash_get (h, makeI (k) );

  if (v)
    fprintf (stdout, "found key: %s\n", WString_getText (v) );
  else
    fprintf (stdout, "no key   : %u\n", k);


  k = 7777;

  v = WHash_get (h, makeI (k) );

  if (v)
    fprintf (stdout, "found key: %s\n", WString_getText (v) );
  else
    fprintf (stdout, "no key   : %u\n", k);


  destroy (h);

  return 0;
}



int test4 (void)
{
  const char * t = "TEST 4";
  void       * h = NIL;
  void       * s = NIL;

  fprintf (stdout, "%s>\n", t);

  h = bless (WHash, 101);
  assert (h);

  s = makeWString ("arc");
  assert (s);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  destroy (h);

  return 0;
}



int main (void)
{
  int (* tests []) () = { test1, test2, test3, test4 };

  warc_u32_t  i      = 0;

  for (i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
