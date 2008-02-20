
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


#define WARC_MAX_SIZE 629145600
#define makeS(s) (s), strlen((s))



int main (int argc, const char ** argv)
{
  void           * p       = NIL; /* WGetOpt object */
  void           * w       = NIL; /* warc file object */
  void           * r       = NIL; /* to recover records */
  warc_i32_t       c       = 0;
  char           * flags   = "cf:";
  char           * fname   = NIL;
  wfile_comp_t     cmode   = WARC_FILE_COMPRESSED_GZIP;
  warc_u32_t       ret     = 0;

  
  if (argc < 2 || argc > 4) 
   {
     fprintf (stderr, "Dump a WARC file\n");
     fprintf (stderr, "Usage: %s -f <file.warc> [-c]\n", argv [0]);
     fprintf (stderr,"\t-f    : valid WARC file name\n");
     fprintf (stderr,"\t[-c]  : GZIP compressed WARC (default true)\n");
     return (2);
   }
  

  p = bless (WGetOpt, makeS (flags));
  assert (p);

  /* parse command line parameters */
  while ((c = WGetOpt_parse (p, argc, argv)) != -1)
    {
      switch (c)
        {
        case 'f' :
          if (w_index (flags, c) [1] == ':')
            fname = WGetOpt_argument (p);
          break;
        case 'c' :
          cmode = WARC_FILE_UNCOMPRESSED;
          break;
        case '?' :  /* illegal option or missing argument */
          destroy (p);
          return (1);
        }
    }
  
  unless (fname)
    {
      fprintf (stderr, "missing WARC file name. Use -f option\n");
      destroy (p);
      return (1);
    }


  w = bless (WFile, fname , WARC_MAX_SIZE,  WARC_FILE_READER, cmode);
  assert (w);

  fprintf (stderr, "%-10s %-10s %-10s %-10s %-15s %-14s %-20s %-56s %-100s\n", 
           "Offset", "CSize", "WarcId", "DataLength",  
           "RecordType", "CreationDate", "ContentType", "RecordId", 
           "SubjectUri");

  while (WFile_hasMoreRecords (w))
    {
      unless ((r = WFile_nextRecord (w)))
        {
          ret = 1;
          break;
        }

      /* dump WRecord */

      fprintf (stdout, "%-10li ",  (unsigned long) WRecord_getOffset  (r));
      fprintf (stdout, "%-10lu ",  
               (unsigned long) WRecord_getCompressedSize (r));

      fprintf (stdout, "%-10s ",   WRecord_getWarcId      (r));
      fprintf (stdout, "%-10u ",   WRecord_getDataLength  (r));
      fprintf (stdout, "%-15u ",   WRecord_getRecordType  (r));
      fprintf (stdout, "%-14s ",   WRecord_getCreationDate(r));
      fprintf (stdout, "%-20s ",   WRecord_getContentType (r));
      fprintf (stdout, "%-56s ",   WRecord_getRecordId    (r));
      fprintf (stdout, "%-100s\n", WRecord_getSubjectUri  (r));
      
      destroy (r);
    }
  
  destroy (p);
  destroy (w);
  
  return (ret);
}
