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

static int check_html_cb (t_hts_callbackarg * carg, httrackp * opt,
                          char * html, int len, const char * url_address, 
                          const char* url_file) 
{

  void * dummyArg = (void *) CALLBACKARG_USERDEF (carg);

  /* 
     call parent functions if multiple callbacks are chained. 
     you can skip this part, if you don't want previous callbacks
     to be called. 
  */
  
  /* if (CALLBACKARG_PREV_FUN(carg, check_html) != NULL)  */
/*     { */
/*       if (!CALLBACKARG_PREV_FUN(carg, check_html)(CALLBACKARG_PREV_CARG(carg), */
/*                                                   opt, html, len,  */
/*                                                   url_address, url_file)) */
/*         return 0;  /\* abort *\/ */
/*     } */

  //fprintf(stderr, "+++ COUCOU file %s   -   %s\n", url_address, url_file);

  return 1;  /* success */
}


int savename_cb (t_hts_callbackarg *carg, httrackp* opt, 
                               const char* adr_complete, 
                               const char* fil_complete, 
                               const char* referer_adr, 
                               const char* referer_fil, char* save)
{
  void * wst = (void *) CALLBACKARG_USERDEF (carg);

  //fprintf(stderr, "+++ savename_cb: %s%s\n", adr_complete, fil_complete);

  setURL (adr_complete, fil_complete, wst);

  return 1;  /* success */
}

static void filesave_cb (t_hts_callbackarg *carg, httrackp *opt, 
                           const char * filename)
{
  void * wst = (void *) CALLBACKARG_USERDEF (carg);
 
 
/*  if (CALLBACKARG_PREV_FUN(carg, filesave) != NULL) 
    {
      if (!CALLBACKARG_PREV_FUN (carg, filesave)(CALLBACKARG_PREV_CARG(carg), opt, file))
          return ;  
    }*/


  writeWRecord ("http://www.iipc.net",  "2008-09-10T00:00:00Z",
                "octet/stream",         "0.0.0.0",
                filename, wst);

}

/* local function called as "end" callback */
static int end_cb(t_hts_callbackarg *carg, httrackp *opt)

{
  void * wst = (struct WPlugin *) CALLBACKARG_USERDEF (carg);

  /* close the WARC file  and free resources */
  destroyWARC (wst);


  /* processing */
/*   fprintf(stderr, "That's all, folks!\n"); */

  /* call parent functions if multiple callbacks are chained. you can skip this part, if you don't want previous callbacks to be called. */
/*   if (CALLBACKARG_PREV_FUN(carg, end) != NULL)  */
/*     { */
/*       /\* status is ok on our side, return other callabck's status *\/ */
/*       return CALLBACKARG_PREV_FUN(carg, end)(CALLBACKARG_PREV_CARG(carg), opt); */
/*     } */

  return 1;  /* success */
}

/*
  module entry point
  the function name and prototype MUST match this prototype
*/

EXTERNAL_FUNCTION int hts_plug(httrackp *opt, const char* argv) {

  void * wst = NULL;

  /* optional argument passed in the commandline we won't be using here */
  const char *arg = strchr(argv, ',');

  if (arg != NULL)
    arg ++;

#define WARC_TMP    "."
#define OUTPUT_WARC "outfile.warc.gz"
#define WARC_SIZE   (1024 * 1024 * 1024)
  wst = blessWARC (OUTPUT_WARC, WARC_SIZE, WARC_TMP);

  if (! wst)
    return (0); /* failure */

  /* plug callback functions */
/*   CHAIN_FUNCTION(opt, check_html, check_html_cb, wst); */
  CHAIN_FUNCTION(opt, savename,   savename_cb,   wst);
  CHAIN_FUNCTION(opt, filesave,   filesave_cb,   wst);
  CHAIN_FUNCTION(opt, end,        end_cb,        wst);

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
