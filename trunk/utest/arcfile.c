
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
#include <afile.h>       /* AFile */
#include <arecord.h>     /* ARecord */


warc_bool_t callback (void * env, const char* buff, const warc_u32_t size)
{
  UNUSED (env);

  if (size)
    {
      warc_u32_t  i = 0;

      while (i < size)
        {
          fprintf (stdout, "%c", buff[i]);
          ++i;
        }

      fprintf (stdout, "\n");
    }

  return (WARC_TRUE); /* return WARC_FALSE to stop extraction */
}


int test1 (void)
{
  const char  * t = "TEST 1: uncompressed ARC file Which contains a single record";
  void        * a = bless (AFile, "./app/wdata/arc2warc/file.arc",
                           WARC_FILE_UNCOMPRESSED);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callbck\n");
          destroy (a);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a);

  return 0;
}

int test2 (void)
{
  const char  * t = "TEST 2: uncompressed ARC file try to register another record in afile either the read one";
  void        * a = bless (AFile, "./app/wdata/arc2warc/file.arc",
                           WARC_FILE_UNCOMPRESSED);
  void        * a2  = bless (AFile, "./app/wdata/arc2warc/mrec.arc",
                             WARC_FILE_UNCOMPRESSED);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      destroy (a);

      if (AFile_register (a2, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callback\n");
          destroy (a2);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a2);

  return 0;
}

int test3 (void)
{
  const char  * t = "TEST 3: uncompressed  ARC file Which contains several records";
  void        * a = bless (AFile, "./app/wdata/arc2warc/mrec.arc",
                           WARC_FILE_UNCOMPRESSED);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callback\n");
          destroy (a);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a);

  return 0;
}

int test4 (void)
{
  const char  * t = "TEST 4: uncompressed ARC file Which contains several records ( the second record are corrupted ) ";
  void        * a = bless (AFile, "./app/wdata/arc2warc/err1.arc",
                           WARC_FILE_UNCOMPRESSED);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callback\n");
          destroy (a);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a);

  return 0;
}

int test5 (void)
{
  const char  * t = "TEST 5: uncompressed ARC file Which contains several records ( the first record are corrupted )";
  void        * a = bless (AFile, "./app/wdata/arc2warc/err2.arc",
                           WARC_FILE_UNCOMPRESSED);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callback\n");
          destroy (a);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a);

  return 0;
}

int test6 (void)
{
  const char  * t = "TEST 6: compressed ARC file  Which contains a single record";
  void        * a = bless (AFile, "./app/wdata/arc2warc/file.arc.gz"
                           , WARC_FILE_COMPRESSED_GZIP);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callback\n");
          destroy (a);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a);

  return 0;
}

int test7 (void)
{
  const char  * t = "TEST 7: compressed ARC file Which contains several records";
  void        * a = bless (AFile, "./app/wdata/arc2warc/sfile.arc.gz"
                           , WARC_FILE_COMPRESSED_GZIP);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callback\n");
          destroy (a);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a);

  return 0;
}

int test8 (void)
{
  const char  * t = "TEST 8: compressed ARC file Which contains several records ( the second record are corrupted ) ";
  void        * a = bless (AFile, "./app/wdata/arc2warc/mfile.arc.gz"
                           , WARC_FILE_COMPRESSED_GZIP);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callback\n");
          destroy (a);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a);

  return 0;
}

int test9 (void)
{
  const char  * t = "TEST 9: compressed ARC file Which contains several records ( the first record are corrupted ) ";
  void        * a = bless (AFile, "./app/wdata/arc2warc/mmfile.arc.gz"
                           , WARC_FILE_COMPRESSED_GZIP);

  fprintf (stdout, "%s\n", t);

  assert (a);

  while (AFile_hasMoreRecords (a) )
    {
      FILE * fin = NIL;
      void * ar  = AFile_nextRecord (a);
      warc_u64_t  size;

      unless (ar)
      {
        destroy (a);
        return  (1);
      }

      fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          fprintf (stderr, "unable to register the callback\n");
          destroy (a);
          destroy (ar);
          return 1;
        }

      ARecord_getContent (ar);

      fin = 0;
      size = 0;

      destroy (ar);
    }

  destroy (a);

  return 0;
}

int main (void)
{
  int (* tests [] ) () = {test1, test2, test3, test4, test5, test6, test7, test8, test9 };
  warc_u32_t  i  = 0;

  for (i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
