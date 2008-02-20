
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

/* unused callback */
warc_bool_t callback (void * env, const char* buff, const warc_u32_t size)
{ 
  UNUSED(env);
  UNUSED(size);
  UNUSED(buff);
  return (WARC_TRUE);
}

int main (int argc, const char ** argv)
{
  void           * p       = NIL;  /* WGetOpt object */
  void           * w       = NIL;  /* warc file object */
  void           * r       = NIL;  /* to recover records */
  warc_u32_t       ret     = 0;
  warc_i32_t       c       = 0;
  char           * flags   = "cvf:";
  char           * fname   = NIL;
  wfile_comp_t     cmode   = WARC_FILE_COMPRESSED_GZIP;
  warc_bool_t      verbose = WARC_TRUE;

  if (argc < 2 || argc > 5) 
   {
     fprintf (stderr, "Check WARC file consistency\n");
     fprintf (stderr, "Usage: %s -f <file.warc> [-c] [-v]\n", argv [0]);
     fprintf (stderr,"\t-f    : valid WARC file name\n");
     fprintf (stderr,"\t[-c]  : GZIP compressed WARC (default true)\n");
     fprintf (stderr,"\t[-v]  : verbose mode (default true)\n");
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
        case 'v' :
          verbose = WARC_FALSE;
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

  w = bless (WFile, fname, WARC_MAX_SIZE, WARC_FILE_READER, cmode);
  assert (w);

  while (WFile_hasMoreRecords (w))
    {
      r = WFile_nextRecord (w);
      unless (r)
        {
          ret = 1;
          break;
        }

      WFile_register (w, r, callback, (void *) 0);
      WRecord_getContent (r);
      
      destroy (r);
    }
  
  destroy (p);
  destroy (w);
  
  if (verbose)
    {
      if (ret)
        fprintf (stdout, "invalid\n");
      else
        fprintf (stdout, "valid\n");
    }

  return (ret);
}
