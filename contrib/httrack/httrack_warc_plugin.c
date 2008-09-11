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


/* For plugin construction, see: http://www.httrack.com/html/plug.html */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* HTTrack mandatory headers */
#include <httrack-library.h>
#include <htsopt.h>
#include <htsdefines.h>

/* WARC headers and misc functions */
#include <warcmisc.h> 

#define makeS(s) (s), w_strlen(s)
#define uS(s) ((warc_u8_t *) (s))


/* local function called as "check_html" callback */

static int process_file(t_hts_callbackarg * carg, httrackp * opt,
                        char * html, int len, const char * url_address, 
                        const char* url_file) 
{

  void * dummyArg = (void *) CALLBACKARG_USERDEF (carg);

  /* 
     call parent functions if multiple callbacks are chained. 
     you can skip this part, if you don't want previous callbacks
     to be called. 
  */
  
  if (CALLBACKARG_PREV_FUN(carg, check_html) != NULL) 
    {
      if (!CALLBACKARG_PREV_FUN(carg, check_html)(CALLBACKARG_PREV_CARG(carg),
                                                  opt, html, len, 
                                                  url_address, url_file))
        return 0;  /* abort */
    }

  fprintf(stderr, "+++ file %s%s\n", url_address, url_file);

  return 1;  /* success */
}


static void store_in_warc (t_hts_callbackarg *carg, httrackp *opt, 
                           const char * filename)
{
  
  void * w = (void *) CALLBACKARG_USERDEF (carg);
 
 
/*  if (CALLBACKARG_PREV_FUN(carg, filesave) != NULL) 
    {
      if (!CALLBACKARG_PREV_FUN (carg, filesave)(CALLBACKARG_PREV_CARG(carg), opt, file))
          return ;  
    }*/

  writeWRecord ("http://www.iipc.net",  "2008-09-10T00:00:00Z",
                "octet/stream",         "0.0.0.0",
                filename, w);

}

/* local function called as "end" callback */
static int end_of_mirror(t_hts_callbackarg *carg, httrackp *opt)

{
/*   void * w = (void *) CALLBACKARG_USERDEF (carg); */

/*   /\* close the WARC file  and free resources *\/ */
/*   if(w) */
/*     destroy (w); */

  /* processing */
  fprintf(stderr, "That's all, folks!\n");

  /* call parent functions if multiple callbacks are chained. you can skip this part, if you don't want previous callbacks to be called. */
  if (CALLBACKARG_PREV_FUN(carg, end) != NULL) 
    {
      /* status is ok on our side, return other callabck's status */
      return CALLBACKARG_PREV_FUN(carg, end)(CALLBACKARG_PREV_CARG(carg), opt);
    }

  return 1;  /* success */
}

/*
  module entry point
  the function name and prototype MUST match this prototype
*/

EXTERNAL_FUNCTION int hts_plug(httrackp *opt, const char* argv) {

  void * w = NULL;

  /* optional argument passed in the commandline we won't be using here */
  const char *arg = strchr(argv, ',');

  if (arg != NULL)
    arg ++;

#define WARC_TMP    "."
#define OUTPUT_WARC "outfile.warc.gz"
#define WARC_SIZE   (1024 * 1024 * 1024)
  w = blessWFile (OUTPUT_WARC, WARC_SIZE, WARC_TMP);

  if (! w)
    return (0); /* failure */

  /* plug callback functions */
  CHAIN_FUNCTION(opt, check_html, process_file,  w);
  CHAIN_FUNCTION(opt, filesave,   store_in_warc, w);
  CHAIN_FUNCTION(opt, end,        end_of_mirror, w);

  return 1;  /* success */
}

/*
  module exit point the function name and prototype MUST match this prototype
*/

EXTERNAL_FUNCTION int hts_unplug(httrackp *opt) {
  fprintf(stderr, "Module unplugged");

  return 1;  /* success */
}
