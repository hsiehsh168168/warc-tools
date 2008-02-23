
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

#include <wfsmanvl.h>

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))


int test1 (void)
{	
  const char * t = "TEST 1";
  void       * a = bless (WAnvl, makeS ("key1"), makeS ("value1"));

  assert (a);

  fprintf (stderr, "%s>\n", t);

  fprintf (stdout, "key  : %s\n", WAnvl_getKey   (a) );
  fprintf (stdout, "value: %s\n", WAnvl_getValue (a) );

  destroy (a);
      
  return 0;
}



int test2 (void)
{	
  const char * t = "TEST 2";
  void       * a = bless (WAnvl, makeS ("key2"), makeS ("value2"));

  assert (a);

  fprintf (stderr, "%s>\n", t);

  WAnvl_setKey   (a, makeS ("CCCC"));
  WAnvl_setValue (a, makeS ("DDDD"));

  fprintf (stdout, "key  : %s\n", WAnvl_getKey   (a) );
  fprintf (stdout, "value: %s\n", WAnvl_getValue (a) );

  destroy (a);
      
  return 0;
}


int test3 (void)
{	
  const char * t        = "TEST 3 a valid anvl";
  const char * filename = "app/wdata/testanvl/anvl-1";
  void       * fin      = NIL; 
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  warc_bool_t  more     = WARC_TRUE;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, fin);
      assert (afsm);

      unless (WFsmANVL_run (afsm))
        {
          anvl = WFsmANVL_transform (afsm);
          if (anvl)
            {
              fprintf (stdout,"ANVL Key: %s\n",   WAnvl_getKey   (anvl));
              fprintf (stdout,"ANVL Value: %s\n", WAnvl_getValue (anvl));

              destroy (anvl);
            }
          else
            more = WARC_FALSE;

        }
      else
        {
          /* error when parsing the WARC header line */
          fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
                   WFsmANVL_state (afsm), ftell (fin), filename);
          more = WARC_FALSE;
        }

      destroy (afsm);

    } while (more);

  

  fclose  (fin);

  return 0;
}


int test4 (void)
{	
  const char * t        = "TEST 4 a corrupted anvl";
  const char * filename = "app/wdata/testanvl/anvl-err";
  void       * fin      = NIL; 
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  warc_bool_t  more     = WARC_TRUE;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  
 do
   {
     /* init ANVL FSM */
     afsm = bless (WFsmANVL, fin);
     assert (afsm);
     
     unless (WFsmANVL_run (afsm))
       {
        anvl = WFsmANVL_transform (afsm);
        if (anvl)
          {
            fprintf (stdout,"ANVL Key: %s\n",WAnvl_getKey(anvl));
            fprintf (stdout,"ANVL Value: %s\n",WAnvl_getValue(anvl));
            
            destroy (anvl);
          }
        else
          destroy (anvl); 
       }
     else
      {
        /* error when parsing the WARC header line */
        fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
                 WFsmANVL_state (afsm), ftell (fin), filename);
        
        more = WARC_FALSE;
      }
     
     
     destroy (afsm);
     
   } while (more);
 
 fclose  (fin);
 
 return 0;
}


int test5 (void)
{	
  const char * t        = "TEST 5 a valid anvl";
  const char * filename = "app/wdata/testanvl/anvl-2";
  void       * fin      = NIL; 
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  warc_bool_t  more     = WARC_TRUE;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, fin);
      assert (afsm);
      
      unless (WFsmANVL_run (afsm))
        {
          anvl = WFsmANVL_transform (afsm);
          if(anvl)
          {
            fprintf (stdout,"ANVL Key: %s\n",   WAnvl_getKey   (anvl));
            fprintf (stdout,"ANVL Value: %s\n", WAnvl_getValue (anvl));

            destroy (anvl);
          }
          else
            more = WARC_FALSE;
        }
      else
        {
          /* error when parsing the WARC header line */
          fprintf (stderr,  
               "error in FSM state address %p, at offset %ld in \"%s\"\n", 
                   WFsmANVL_state (afsm), ftell (fin), filename);

          more = WARC_FALSE;
        }
    

      destroy (afsm);

    } while (more);
 
  
  fclose (fin);
  
  return 0;
}


int test6 (void)
{	
  const char * t        = "TEST 6";
  const char * filename = "app/wdata/testanvl/anvl-ctlkey.anvl";
  void       * fin      = NIL; 
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  warc_bool_t  more     = WARC_TRUE;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, fin);
      assert (afsm);
      
      unless (WFsmANVL_run (afsm))
        {
          anvl = WFsmANVL_transform (afsm);
          if(anvl)
          {
            fprintf (stdout,"ANVL Key: %s\n",   WAnvl_getKey   (anvl));
            fprintf (stdout,"ANVL Value: %s\n", WAnvl_getValue (anvl));

            destroy (anvl);
          }
          else
            more = WARC_FALSE;
        }
      else
        {
          /* error when parsing the WARC header line */
          fprintf (stderr,  
               "error in FSM state address %p, at offset %ld in \"%s\"\n", 
                   WFsmANVL_state (afsm), ftell (fin), filename);

          more = WARC_FALSE;
        }
    

      destroy (afsm);

    } while (more);
 
  
  fclose (fin);
  
  return 0;
}


int test7 (void)
{	
  const char * t = "TEST 7";
  void       * a = bless (WAnvl, makeS ("key2"), makeS ("value2"));

  assert (a);

  fprintf (stderr, "%s>\n", t);

  if (WAnvl_setKey (a, makeS ("C\x7F")))
    {
      fprintf (stderr, ("Invalid key\n"));
      destroy (a);
      return 1;
    }

  WAnvl_setValue (a, makeS ("DDDD"));
  
  fprintf (stdout, "key  : %s\n", WAnvl_getKey   (a) );
  fprintf (stdout, "value: %s\n", WAnvl_getValue (a) );
  
  destroy (a);
      
  return 0;
}


int test8 (void)
{	
  const char * t = "TEST 8";
  void       * a = bless (WAnvl, makeS ("key2"), makeS ("value2"));

  assert (a);

  fprintf (stderr, "%s>\n", t);

  if (WAnvl_setKey (a, makeS ("C\x0Ausing")))
    {
      fprintf (stderr, ("An other invalid key\n"));
      destroy (a);
      return 1;
    }
  WAnvl_setValue (a, makeS ("DDDD"));
  
  fprintf (stdout, "key  : %s\n", WAnvl_getKey   (a) );
  fprintf (stdout, "value: %s\n", WAnvl_getValue (a) );
  
  destroy (a);
  
  return 0;
}



int main (void)
{	
  int (* tests [])() = { test1, test2, test3, test4,
                         test5, test6, test7, test8};

  warc_u32_t  i      = 0;

  for(i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
