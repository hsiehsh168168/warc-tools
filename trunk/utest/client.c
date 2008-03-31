
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

#include <warc.h>

#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))

int test1 (void)
{

  void * c = bless (WClient, makeS("127.0.0.1"), 8080);
  const char * t = "TEST 1: Requesting a single record in a compressed warc file";
  
  assert (c);

  fprintf (stderr, "%s\n", t);

  if (WClient_getWRecord (c, makeS("gzip"), 0, 
                          makeS("app/wdata/testwfile/file.warc.gz"), 
                          uS("./out1.warc.gz")))
    fprintf (stderr, "request not satisfied\n");

  destroy (c);

  return 0;
}


int test2 (void)
{

  void * c = bless (WClient, makeS("127.0.0.1"), 8080);
  const char * t = "TEST 2: Requesting a whole warc file";
  
  assert (c);

  fprintf (stderr, "%s\n", t);

  if (WClient_getWFile (c, makeS ("gzip"), 0, makeS("app/wdata/testwfile/cwarc.warc.gz"),
                        uS("./out2.warc.gz")))
    fprintf (stderr, "request not satisfied\n");
  
  destroy (c);

  return 0;
}


int test3 (void)
{

  void * c = bless (WClient, makeS("127.0.0.1"), 8080);
  const char * t = "TEST 3: sending a bad request";
  
  assert (c);

  fprintf (stderr, "%s\n", t);

  if (WClient_getWFile (c, makeS ("gzip"), 0, makeS("app/wdata/testwfile/cwarc.warc.gz"),
                        uS("./out2.warc.gz")))
     fprintf (stderr, "request not satisfied\n");
  
  destroy (c);

  return 0;
}


int test4 (void)
{

  void * c = bless (WClient, makeS("127.0.0.1"), 8080);
  const char * t = "TEST 4: sending a filter request";
  
  assert (c);

  fprintf (stderr, "%s\n", t);

  if (WClient_getFiltredWFile (c, makeS("uncompressed"), 0, makeS ("uri"), makeS (""), makeS("app/wdata/testwfile/warcfile.warc"),
                        uS("./out3.warc")))
     fprintf (stderr, "request not satisfied\n");
  
  destroy (c);

  return 0;
}


int main (void)
{
  int (* tests []) () = { test1, test2, test3, test4 };

  warc_u32_t  i      = 0;

  for (i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
