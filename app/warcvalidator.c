
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
  void           * w      = NIL;  /* warc file object */
  void           * r      = NIL;  /* to recover records */
  wfile_comp_t     cmode  = WARC_FILE_UNCOMPRESSED;
  warc_u32_t       ret    = 0;

  
  if (argc != 3) 
   {
     fprintf (stderr, "Validate a WARC file\n");
     fprintf (stderr, "Usage: %s file.warc <flag>\n", argv [0]);
     fprintf (stderr,"<flag>    : 0 for uncompressed WARC\n");
     return (10);
   }
  
  if (* argv [2] == '1')
    cmode = WARC_FILE_COMPRESSED_GZIP;
        

  w = bless (WFile, argv[1], WARC_MAX_SIZE, WARC_FILE_READER, cmode);
  assert (w);

  while (WFile_hasMoreRecords (w))
    {
      if ((r = WFile_nextRecord (w)))
        {
          ret = 1;
          break;
        }

      WFile_register (w, r, callback, (void *) 0);
      WRecord_getContent (r);
      
      destroy (r);
    }
  
  destroy (w);
  
  return (ret);
}
