
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

#define makeWString(s) (bless (WString, ((warc_u8_t *) s), w_strlen ((warc_u8_t *) (s))))

int test1 (void)
{	
  const char * t = "TEST 1";
  void       * l = bless (WList);

  assert (l);

  WList_push (l, makeWString ("AAAA"));
  WList_push (l, makeWString ("BBBB"));
  WList_push (l, makeWString ("CCCC"));

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));
  
  destroy (l);
      
  return 0;
}

int test2 (void)
{	
  const char * t = "TEST 2";
  void       * l  = bless (WList);

  assert (l);

  WList_unshift (l, makeWString ("AAAA"));
  WList_unshift (l, makeWString ("BBB"));
  WList_unshift (l, makeWString ("CCCCC"));

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));

  destroy (l);
      
  return 0;
}

int test3 (void)
{	
  const char * t = "TEST 3";
  void       * l = bless (WList);

  assert (l);

  WList_unshift (l, makeWString ("AAAA"));
  WList_unshift (l, makeWString ("BBB"));
  WList_unshift (l, makeWString ("CCCCC"));
  WList_unshift (l, makeWString ("DDDD"));

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));

  destroy (l);
      
  return 0;
}

int test4 (void)
{	
  const char * t = "TEST 4";
  void       * l = bless (WList);
  void       * s = NIL;

  assert (l);

  WList_unshift (l, makeWString ("AAAA"));
  WList_unshift (l, makeWString ("BBB"));
  WList_unshift (l, makeWString ("CCCCC"));
  WList_push    (l, makeWString ("DD"));

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));

  while( WList_size (l) )
    {
      s = WList_shift(l); /* WList_shift */
      fprintf(stdout, "%s\n", WString_getText(s));
      destroy (s);
    }

  destroy (l);
      
  return 0;
}

int test5 (void)
{	
  const char * t = "TEST 5";
  void       * l = bless (WList);
  void       * s = NIL;

  assert (l);

  WList_push (l, makeWString ("AAAA"));
  WList_push (l, makeWString ("BBBB"));
  WList_push (l, makeWString ("CCCC"));
  WList_push (l, makeWString ("DDDD"));

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));

  while( WList_size (l) )
    {
      s = WList_pop(l); /* WList_pop */
      fprintf(stdout, "%s\n", WString_getText(s));
      destroy (s);
    }

  destroy (l);
      
  return 0;
}


int test6 (void)
{	
  const char * t = "TEST 6";
  void       * l = bless (WList);
  void       * s = NIL;
  warc_u32_t   i;

  assert (l);

  WList_push (l, makeWString ("AAAA"));
  WList_push (l, makeWString ("BBBB"));
  WList_push (l, makeWString ("CCCC"));

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));

  i = 1;
  s = (void *) WList_get (l, i);
  fprintf (stdout, "list index %d is : %s\n", i, WString_getText (s));

  i = 0;
  s = (void *) WList_get (l, i);
  fprintf (stdout, "list index %d is : %s\n", i, WString_getText (s));

  i = 2;
  s = (void *) WList_get (l, i);
  fprintf (stdout, "list index %d is : %s\n", i, WString_getText (s));

  destroy (l);
      
  return 0;
}

int test7 (void)
{	
  const char * t = "TEST 7";
  void       * l = bless (WList);
  void       * s = NIL;
  void       * o = makeWString ("ZZZZZZZ");
  warc_u32_t   i = 1;

  assert (l);

  WList_push (l, makeWString ("AAAA"));
  WList_push (l, makeWString ("BBBB"));
  WList_push (l, makeWString ("CCCC"));

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));

  s = (void *) WList_set (l, i, o);
  fprintf (stdout, "replace %s at index %d by %s\n", 
           WString_getText (s), i, WString_getText (o));

  destroy (s);
  destroy (l);
      
  return 0;
}


int test8 (void)
{	
  const char * t = "TEST 8";
  void       * l = bless (WList);
  void       * s = NIL;
  warc_u32_t   i;

  assert (l);

  WList_push (l, makeWString ("AAAA"));
  WList_push (l, makeWString ("BBBB"));
  WList_push (l, makeWString ("CCCC"));
  WList_push (l, makeWString ("DDDD"));

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));

  i = 2;
  s = WList_remove (l, i);
  fprintf (stdout, "remove %s from index %d\n", 
           WString_getText (s), i);
  destroy (s);

  i = 0;
  s = WList_remove (l, i);
  fprintf (stdout, "remove %s from index %d\n", 
           WString_getText (s), i);

  destroy (s);
  destroy (l);
      
  return 0;
}


int test9 (void)
{	
  const char * t = "TEST 9";
  void       * l = bless (WList);

  assert (l);

  fprintf (stdout, "%s> list size: %d\n", t, WList_size (l));

  WList_pop (l);

  destroy (l);
      
  return 0;
}


int main (void)
{	
  int (* tests [])() = { test1, test2, test3, test4, 
                         test5, test6, test7, test8,
                         test9 };
  warc_u32_t  i      = 0;

  for(i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
