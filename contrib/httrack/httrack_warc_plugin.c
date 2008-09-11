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


/* For plugin buiding, see: http://www.httrack.com/html/plug.html */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* HTTrack mandatory headers */
#include <httrack-library.h>
#include <htsopt.h>
#include <htsdefines.h>

/* WARC headers and misc functions */
#include <warcmisc.h> 

/* default values */
#include <httrack_warc_plugin.h>


/* useful macros */
#define makeS(s) (s), w_strlen(s)
#define uS(s) ((warc_u8_t *) (s))


int check_mime_cb (t_hts_callbackarg *carg, httrackp* opt, const char* adr,
                  const char* fil, const char* mime, int status)
{
  void * wst = (void *) CALLBACKARG_USERDEF (carg);

  setMIME (mime, wst);
}

void filesave_cb (t_hts_callbackarg *carg, httrackp* opt, const char* file)
{
  void * wst = (void *) CALLBACKARG_USERDEF (carg);

  writeWRecord (HTTRACK_DEFAULT_TIMESTAMP,
                HTTRACK_DEFAULT_MIMETYPE,  HTTRACK_DEFAULT_IP,
                file,                  wst);
}

void filesave2_cb (t_hts_callbackarg *carg, httrackp* opt, 
                   const char* hostname, const char* filename, 
                   const char* localfile, int is_new, int is_modified, 
                   int not_updated)
{
  void * wst = (void *) CALLBACKARG_USERDEF (carg);

  setURL (hostname, filename, wst);
}

static int end_cb(t_hts_callbackarg *carg, httrackp *opt)

{
  void * wst = (struct WPlugin *) CALLBACKARG_USERDEF (carg);

  destroyWARC (wst);

  return 1;  /* success */
}


/* module entry point */
EXTERNAL_FUNCTION int hts_plug (httrackp *opt, const char* argv) {

  void * wst = NULL;

  /* optional argument passed in the commandline we won't be using here */
  const char *arg = strchr(argv, ',');

  if (arg != NULL)
    arg ++;


  wst = blessWARC (HTTRACK_OUTPUT_WARC, HTTRACK_WARC_SIZE, HTTRACK_TMP_DIR);
  if (! wst)
    return (0); /* failure */

  /* plug callback functions */
  CHAIN_FUNCTION(opt, check_mime,  check_mime_cb, wst);
  CHAIN_FUNCTION(opt, filesave2,   filesave2_cb,   wst);
  CHAIN_FUNCTION(opt, filesave,    filesave_cb,   wst);
  CHAIN_FUNCTION(opt, end,         end_cb,        wst);

  return 1;  /* success */
}

/*
  module exit point the function name and prototype MUST match this prototype
*/

#define WARC_MODULE_NAME "libhtswarc.so"
EXTERNAL_FUNCTION int hts_unplug(httrackp *opt) {
  fprintf(stderr, "Module " WARC_MODULE_NAME " unplugged\n");

  return 1;  /* success */
}
