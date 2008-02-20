
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

#define makeS(s) (s), strlen((s))


int test1 (void)
{
  const char * t     = "TEST 1";
  void       * g     = NIL;
  const char * flags = "abc:";
  const char * _av[] = { "test1", "-a", "-b" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;

  fprintf (stdout, "%s>\n", t);


  g = bless (WGetOpt, makeS(flags));
  assert (g);

  fprintf (stdout, "flags : %s\n", flags);
  fprintf (stdout, "params: ");
  for (i = 0; i < ac; ++ i)
    {
      fprintf (stdout, "%s ", av [i]);
    }
  fprintf (stdout, "\n");

  /* print flags */
  while ((c = WGetOpt_parse (g, ac, av)) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
       {
         destroy (g);
         return (1);
       }

      fprintf (stdout, "-%c %s ", c, 
               w_index (flags, c) [1] == ':' ? WGetOpt_argument (g) : "");
    }
  
  /* end of flags; print the rest of options. */
  fprintf (stdout, "-- ");
  
  i = WGetOpt_indice (g);
  if (i < ac)
    {
      for (; i < ac; ++ i)
        fprintf (stdout, "%s ", * (av + i));
    }

  fprintf (stdout, "\n");

  destroy (g);
  
  return (0);
}

int test2 (void)
{
  const char * t     = "TEST 2";
  void       * g     = NIL;
  const char * flags = "a:";
  const char * _av[] = { "test2", "-a", "15" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;

  fprintf (stdout, "%s>\n", t);


  g = bless (WGetOpt, makeS(flags));
  assert (g);

  fprintf (stdout, "flags : %s\n", flags);
  fprintf (stdout, "params: ");
  for (i = 0; i < ac; ++ i)
    {
      fprintf (stdout, "%s ", av [i]);
    }
  fprintf (stdout, "\n");

  /* print flags */
  while ((c = WGetOpt_parse (g, ac, av)) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
          return (1);
        }
      
      fprintf (stdout, "-%c %s ", c, 
               w_index (flags, c) [1] == ':' ? WGetOpt_argument (g) : "");
    }
  
  /* end of flags; print the rest of options. */
  fprintf (stdout, "-- ");
  
  i = WGetOpt_indice (g);
  if (i < ac)
    {
      for (; i < ac; ++ i)
        fprintf (stdout, "%s ", * (av + i));
    }

  fprintf (stdout, "\n");

  destroy (g);
  
  return (0);
}


int test3 (void)
{
  const char * t     = "TEST 3";
  void       * g     = NIL;
  const char * flags = "ab:";
  const char * _av[] = { "test3", "-a", "15", "-b", "true" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;

  fprintf (stdout, "%s>\n", t);


  g = bless (WGetOpt, makeS(flags));
  assert (g);

  fprintf (stdout, "flags : %s\n", flags);
  fprintf (stdout, "params: ");
  for (i = 0; i < ac; ++ i)
    {
      fprintf (stdout, "%s ", av [i]);
    }
  fprintf (stdout, "\n");

  /* print flags */
  while ((c = WGetOpt_parse (g, ac, av)) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
          return (1);
        }
      
      fprintf (stdout, "-%c %s ", c, 
               w_index (flags, c) [1] == ':' ? WGetOpt_argument (g) : "");
    }
  
  /* end of flags; print the rest of options. */
  fprintf (stdout, "-- ");
  
  i = WGetOpt_indice (g);
  if (i < ac)
    {
      for (; i < ac; ++ i)
        fprintf (stdout, "%s ", * (av + i));
    }

  fprintf (stdout, "\n");

  destroy (g);
  
  return (0);
}


int test4 (void)
{
  const char * t     = "TEST 4";
  void       * g     = NIL;
  const char * flags = "ab";
  const char * _av[] = { "test4", "-ab" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;

  fprintf (stdout, "%s>\n", t);


  g = bless (WGetOpt, makeS(flags));
  assert (g);

  fprintf (stdout, "flags : %s\n", flags);
  fprintf (stdout, "params: ");
  for (i = 0; i < ac; ++ i)
    {
      fprintf (stdout, "%s ", av [i]);
    }
  fprintf (stdout, "\n");

  /* print flags */
  while ((c = WGetOpt_parse (g, ac, av)) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
          return (1);
        }
      
      fprintf (stdout, "-%c %s ", c, 
               w_index (flags, c) [1] == ':' ? WGetOpt_argument (g) : "");
    }
  
  /* end of flags; print the rest of options. */
  fprintf (stdout, "-- ");
  
  i = WGetOpt_indice (g);
  if (i < ac)
    {
      for (; i < ac; ++ i)
        fprintf (stdout, "%s ", * (av + i));
    }

  fprintf (stdout, "\n");

  destroy (g);
  
  return (0);
}

int test5 (void)
{
  const char * t     = "TEST 5";
  void       * g     = NIL;
  const char * flags = "ab:";
  const char * _av[] = { "test5", "-ab" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;

  fprintf (stdout, "%s>\n", t);


  g = bless (WGetOpt, makeS(flags));
  assert (g);
  
  fprintf (stdout, "flags : %s\n", flags);
  fprintf (stdout, "params: ");
  for (i = 0; i < ac; ++ i)
    {
      fprintf (stdout, "%s ", av [i]);
    }
  fprintf (stdout, "\n");

  /* print flags */
  while ((c = WGetOpt_parse (g, ac, av)) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
          return (1);
        }
      
      fprintf (stdout, "-%c %s ", c, 
               w_index (flags, c) [1] == ':' ? WGetOpt_argument (g) : "");
    }
  
  /* end of flags; print the rest of options. */
  fprintf (stdout, "-- ");
  
  i = WGetOpt_indice (g);
  if (i < ac)
    {
      for (; i < ac; ++ i)
        fprintf (stdout, "%s ", * (av + i));
    }

  fprintf (stdout, "\n");

  destroy (g);
  
  return (0);
}

int test6 (void)
{
  const char * t     = "TEST 6";
  void       * g     = NIL;
  const char * flags = "ab";
  const char * _av[] = { "test6", "-a", "--b" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;

  fprintf (stdout, "%s>\n", t);


  g = bless (WGetOpt, makeS(flags));
  assert (g);
  
  fprintf (stdout, "flags : %s\n", flags);
  fprintf (stdout, "params: ");
  for (i = 0; i < ac; ++ i)
    {
      fprintf (stdout, "%s ", av [i]);
    }
  fprintf (stdout, "\n");

  /* print flags */
  while ((c = WGetOpt_parse (g, ac, av)) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
          return (1);
        }
      
      fprintf (stdout, "-%c %s ", c, 
               w_index (flags, c) [1] == ':' ? WGetOpt_argument (g) : "");
    }
  
  /* end of flags; print the rest of options. */
  fprintf (stdout, "-- ");
  
  i = WGetOpt_indice (g);
  if (i < ac)
    {
      for (; i < ac; ++ i)
        fprintf (stdout, "%s ", * (av + i));
    }

  fprintf (stdout, "\n");

  destroy (g);
  
  return (0);
}


int main (void)
{	
  int (* tests [])() = { test1, test2, test3, test4, test5, test6 };
  
  warc_u32_t  i      = 0;
  
  for(i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests [i] ();
    }
  
  return 0;
}
