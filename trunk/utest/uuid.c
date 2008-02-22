
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

#include <assert.h>
#include <stdio.h>

#include <warc.h>

#define makeU(s) (const warc_u8_t *) (s), (warc_u64_t) w_strlen((warc_u8_t *) (s))

int test1 (void)
{	
  const char * t = "TEST 1";
  void       * u = NIL; 
  const char * s = NIL; 

  fprintf (stdout, "%s>\n", t);

  /* empty string */
  u = bless (WUUID);
  assert (u);

  WUUID_hash (u, makeU(""));
  s = WUUID_text(u);
  fprintf (stdout, "%s\n", s);


  WUUID_reinit(u); /* re-initialize the Tiger buffer */
  WUUID_hash (u, makeU("Hello World"));
  s = WUUID_text(u);
  fprintf (stdout, "%s\n", s);

  destroy (u);

  return 0;
}

int test2 (void)
{	
  const char * t = "TEST 2";
  void       * u = NIL; 
  const char * s = NIL; 

  fprintf (stdout, "%s>\n", t);

  /* empty string */
  u = bless (WUUID);
  assert (u);

  WUUID_hash (u, makeU("Tiger Hashing"));
  s = WUUID_text(u);
  fprintf (stdout, "%s\n", s);

  WUUID_reinit(u); /* re-initialize the Tiger buffer */
  s = WUUID_text(u);
  fprintf (stdout, "%s\n", s);

  destroy (u);

  return 0;
}

int test3 (void)
{	
  const char * t = "TEST 3";
  void       * u = NIL; 
  const char * s = NIL; 

  fprintf (stdout, "%s>\n", t);

  /* empty string */
  u = bless (WUUID);
  assert (u);

  WUUID_hash (u, makeU("Hello"));
  WUUID_hash (u, makeU("World"));
  s = WUUID_text(u);
  fprintf (stdout, "%s\n", s);

  WUUID_reinit(u); /* re-initialize the Tiger buffer */
  WUUID_hash (u, makeU("Hello World"));
  s = WUUID_text(u);
  fprintf (stdout, "%s\n", s);

  destroy (u);

  return 0;
}


int main (void)
{	
  int (* tests [])() = { test1, test2, test3 };

  warc_u32_t  i      = 0;

  for(i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
