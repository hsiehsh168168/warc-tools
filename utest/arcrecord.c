
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

#include <afsmhdl.h>
#include <arecord.h>
#include <afile.h>


#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))


int test1 (void)
{	
  const char * t  = "TEST 1";
  void       * hl = bless (ARecord, 
                           makeS ("http://www.w3c.org"),
                           makeS ("192.168.4.1"),
                           makeS ("12172007"), 
                           makeS ("warcproject/testheaderline"),
                            12);

  assert (hl);

  fprintf (stdout, "%s>\n", t);
  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

  destroy (hl);
    
  return 0;
}


int test2 (void)
{	
  const char * t  = "TEST 2";
  
    void       * hl = bless (ARecord, 
                           makeS ("http://www.w3c.org"),
                           makeS ("192.168.4.1"),
                           makeS ("12172007"), 
                           makeS ("warcproject/testheaderline"),
                            12);


  fprintf (stdout, "%s>\n", t);

  assert (hl);

  ARecord_setDataLength   (hl, 15);
  ARecord_setUrl   (hl, makeS ("http://www.iso.net"));
  ARecord_setCreationDate (hl, makeS ("01012999"));
  ARecord_setMimeType  (hl, makeS ("application/xpdf"));
  ARecord_setIpAddress     (hl, makeS ("168.12.0.0"));

 
  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

  destroy (hl);
    
  return 0;
}


int test3 (void)
{	
  const char * t        = "TEST 3";
  const char * filename = "app/wdata/testarc/file.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      

  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}

int test4 (void)
{	
  const char * t        = "TEST 4";
  const char * filename = "app/wdata/testarc/err1.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;


  fprintf (stdout, "%s>\n", t);
  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      
 
  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}


int test5 (void)
{	
  const char * t        = "TEST 5";
  const char * filename = "app/wdata/testarc/err2.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      
 
  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}

int test6 (void)
{	
  const char * t        = "TEST 6";
  const char * filename = "app/wdata/testarc/err3.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      

  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}

int test7 (void)
{	
  const char * t        = "TEST 7";
  const char * filename = "app/wdata/testarc/err4.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      

  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}


int test8 (void)
{	
  const char * t        = "TEST 8";
  const char * filename = "app/wdata/testarc/err5.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      
 
  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}


int test9 (void)
{	
  const char * t        = "TEST 9";
  const char * filename = "app/wdata/testarc/err6.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      

  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}
int test10 (void)
{	
  const char * t        = "TEST 10";
  const char * filename = "app/wdata/testarc/err7.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;

  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      
  
  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}


int test11 (void)
{	
  const char * t        = "TEST 11";
  const char * filename = "app/wdata/testarc/err8.arc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;


  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = AFsmHDL_transform (fsm);
      
  fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl));
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl));
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl));
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate(hl));
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               AFsmHDL_state (fsm), ftell (fin), filename);
    }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}

int main (void)
{	
  int (* tests [])() = { test1, test2, test3, test4, test5, 
                       test6, test7, test8, test9, test10, test11};

  warc_u32_t  i      = 0;

  for(i = 0; i < 11; ++ i)
    {
      tests[i] ();
    }

  return 0;
}
