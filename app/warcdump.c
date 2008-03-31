
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

#ifndef WARC_MAX_SIZE
#define WARC_MAX_SIZE 629145600
#endif

#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))


int main (int argc, const char ** argv)
{
  void           * p       = NIL; /* WGetOpt object */
  void           * w       = NIL; /* WARC file object */
  void           * r       = NIL; /* WARC record object */
  warc_i32_t       c       = 0;
  warc_u8_t      * flags   = uS ("vcf:t:");
  char           * fname   = NIL;
  char           * wdir    = ".";
  warc_bool_t      amode   = WARC_FALSE;
  wfile_comp_t     cmode   = WARC_FILE_UNCOMPRESSED;
  warc_u32_t       ret     = 0;


  if (argc < 2 || argc > 7)
    {
      fprintf (stderr, "Dump a WARC file\n");
      fprintf (stderr, "Usage: %s -f <file.warc> [-c] [-v] [-t <working_dir>]\n", argv [0]);
      fprintf (stderr, "\t-f    : valid WARC file name\n");
      fprintf (stderr, "\t[-c]  : GZIP compressed WARC (default no)\n");
      fprintf (stderr, "\t[-v]  : dump ANVL (default false)\n");
      fprintf (stderr, "\t[-t]  : temporary working directory (default \".\")\n");
      return (2);
    }


  /* parse command line parameters */
  p = bless (WGetOpt, makeS (flags) );

  assert (p);

  while ( (c = WGetOpt_parse (p, argc, argv) ) != -1)
    {
      switch (c)
        {
        case 'f' :
          
          if (w_index (flags, c) [1] == ':')
            fname = WGetOpt_argument (p);
          
          break;
          
        case 'c' :
          cmode = WARC_FILE_COMPRESSED_GZIP;
          
          break;
          
        case 'v' :
          amode = WARC_TRUE;
          
          break;
          
        case 't' :
          
          if (w_index (flags, c) [1] == ':')
            wdir = WGetOpt_argument (p);
          
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


  w = bless (WFile, fname , WARC_MAX_SIZE,
             WARC_FILE_READER, cmode, wdir);
  assert (w);

  fprintf (stderr, "%-10s %-10s %-10s %-10s %-15s %-14s %-20s %-56s %-100s\n",
           "Offset", "CSize", "WarcId", "DataLength",
           "RecordType", "CreationDate", "ContentType", "RecordId",
           "SubjectUri");

  while (WFile_hasMoreRecords (w) )
    {
      const void * al  = NIL; /* ANVL list object */

      unless ( (r = WFile_nextRecord (w) ) )
      {
        ret = 1;
        break;
      }

      /* dump WRecord */

      fprintf (stdout, "%-10llu ", (unsigned long long) WRecord_getOffset (r) );
      fprintf (stdout, "%-10llu ", (unsigned long long) WRecord_getCompressedSize (r) );
      fprintf (stdout, "%-10s ",   WRecord_getWarcId      (r) );
      fprintf (stdout, "%-10u ",   WRecord_getDataLength  (r) );
      fprintf (stdout, "%-15u ",   WRecord_getRecordType  (r) );
      fprintf (stdout, "%-14s ",   WRecord_getCreationDate (r) );
      fprintf (stdout, "%-20s ",   WRecord_getContentType (r) );
      fprintf (stdout, "%-56s ",   WRecord_getRecordId    (r) );
      fprintf (stdout, "%-100s\n", WRecord_getSubjectUri  (r) );

      /* dump ANVLs */

      if (amode == WARC_TRUE && (al = WRecord_getAnvl (r) ) != NIL)
        {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */

          while ( i < j )
            {
              const void  * a = WList_get (al, i); /* ANVL record */

              fprintf (stdout, "\tkey: %s\n", (char *) WAnvl_getKey   (a) );

              /* we assume here that the ANVL value was in ASCII. */
              /* use your own encoding to print it otherwise. */
              fprintf (stdout, "\tval: %s\n", (char *) WAnvl_getValue (a) );

              ++ i;
            }
        }


      destroy (r);
    }

  destroy (p);

  destroy (w);

  return (ret);
}
