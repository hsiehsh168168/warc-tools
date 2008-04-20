
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
  warc_u8_t      * flags   = uS ("vf:t:");
  char           * fname   = NIL;
  char           * wdir    = ".";
  warc_bool_t      amode   = WARC_FALSE;
  wfile_comp_t     cmode   = WARC_FILE_DETECT_COMPRESSION;
  warc_u32_t       ret     = 0;


  if (argc < 2 || argc > 7)
    {
      fprintf (stderr, "Dump a WARC file\n");
      fprintf (stderr, "Usage: %s -f <file.warc> [-v] [-t <working_dir>]\n", argv [0]);
      fprintf (stderr, "\t-f    : valid WARC file name\n");
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

  fprintf (stderr, "%-20s %-20s %-10s %-20s %-45s %-44s %-86s \n",
           "Offset", "CSize", "WarcId", "Content-Length",
           "WARC-Type", "WARC-Date",  "WARC-Record-ID");

  while (WFile_hasMoreRecords (w) )
    {
      const void * al  = NIL; /* ANVL list object */
      warc_bool_t  m1  = WARC_FALSE;
      const warc_u8_t  * string = NIL;
      warc_u32_t   tlen = 0;

      unless ( (r = WFile_nextRecord (w) ) )
      {
        ret = 1;
        break;
      }

      /* dump WRecord */

      /* mandatory fields */
      fprintf (stdout, "%-20llu ", (unsigned long long) WRecord_getOffset (r) );

      fprintf (stdout, "%-20llu ", (unsigned long long) WRecord_getCompressedSize (r) );

      fprintf (stdout, "%-10s ",   WRecord_getWarcId        (r) );

      fprintf (stdout, "%-20u ",   WRecord_getContentLength (r) );

      fprintf (stdout, "%-45u ",   WRecord_getRecordType    (r) );

      fprintf (stdout, "%-44s ",   WRecord_getDate  (r) );

      fprintf (stdout, "%-86s\n",   WRecord_getRecordId      (r) );

      /* optionnal fields */

      m1 = WARC_FALSE;
      fprintf (stdout, "More Fields:\n\n"   );

      string =  WRecord_getContentType (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "Content-Type",  string);
        m1 = WARC_TRUE;
        }


      string =  WRecord_getConcurrentTo (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Concurrent-To",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getBlockDigest (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Block-Digest",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getPayloadDigest (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Payload-Digest", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getIpAddress (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-IP-Address", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getRefersTo (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Refers-To", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getTargetUri (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Target-URI",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getTruncated (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Truncated",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getWarcInfoId (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Warcinfo-ID", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getFilename (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Filename", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getProfile (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Profile",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getPayloadType (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Identified-Payload-type", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getSegmentOriginId (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Segment-Origin-ID",  string);

        fprintf (stdout, "%-35s: %-20d\n", "WARC-Segment-Number", WRecord_getSegmentNumber (r));
        m1 = WARC_TRUE;
        }

      tlen = WRecord_getSegTotalLength (r) ;
      if (tlen)
        {
        fprintf (stdout, "%-35s: %-20d\n", "WARC-Segment-Total-Length", tlen);
        m1 = WARC_TRUE;
        }

     unless (m1)
        fprintf (stdout, "-- No One --\n");

     fprintf (stdout, "\n");

      /* dump ANVLs */

      if (amode == WARC_TRUE && (al = WRecord_getAnvl (r) ) != NIL)
        {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */

          if (j)
             fprintf (stdout, "-- More Info--\n\n");

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