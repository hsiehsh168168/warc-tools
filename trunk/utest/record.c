
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
#include <string.h>

#include <warc.h>

#include <whdline.h>
#include <wfsmhdl.h>
#include <wfsmanvl.h>

#define makeS(s) (s), strlen((s))


int test1 (void)
{	
  const char * t   = "TEST 1";
  void       * r   = bless (WRecord);


  fprintf (stdout, "%s>\n", t);
  

  if (r)
    {
     WRecord_setSubjectUri (r,makeS("http://www.w3c.org"));
     WRecord_setCreationDate (r,makeS("12172007"));
     WRecord_setContentType (r,makeS("warcproject/testheaderline"));
     WRecord_setRecordId (r,makeS("id://warc-x584jz39"));
     WRecord_setRecordType (r,WARCINFO_RECORD);

      fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
      fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
      fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
      fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
      fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
      fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
      fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));
    destroy (r);
    }



      
  return 0;
}


int test2 (void)
{	
  const char * t        = "TEST 2";
  const char * filename = "app/wdata/testwfile/file.warc";
  void       * fin      = NIL; 
  void       * hl       = NIL; 
  void       * fsm      = NIL;
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  int          errore   =   0;
  warc_bool_t      more     = WARC_TRUE;

  fprintf (stdout, "%s>\n", t);
   
  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
    return (1);

  /* init HDL FSM */
  fsm = bless (WFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (WFsmHDL_run (fsm))
    {
      /* generate the WHDLine object from the FSM */
      hl = WFsmHDL_transform (fsm);
      
      fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl));
      fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl));
      fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl));
      fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl));
      fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate(hl));
      fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl));
      fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl));

      destroy (hl);
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               WFsmHDL_state (fsm), ftell (fin), filename);
      errore=1;
    }

while (more && (errore !=1))
  {
  afsm = bless (WFsmANVL, fin);
  assert (afsm);

  unless (WFsmANVL_run (afsm ))
    {
    anvl = WFsmANVL_transform (afsm);
    if (anvl)
        {printf ("ANVL Key: %s\n",WAnvl_getKey(anvl));
        printf ("ANVL Value: %s\n",WAnvl_getValue(anvl));
          destroy (anvl);}
    else more = WARC_FALSE;
    }
  else
    {
      /* error when parsing the WARC header line */
     fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n", 
               WFsmHDL_state (fsm), ftell (fin), filename);
               destroy(afsm);
               break;
    }
  destroy (afsm);
  }

  
  destroy (fsm);
  fclose  (fin);

  return 0;
}

int main (void)
{	
  int (* tests [])() = { test1, test2};
  warc_u32_t  i      = 0;

  for(i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}



