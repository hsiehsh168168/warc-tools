#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* HTTrack mandatory headers */
#include <httrack-library.h>
#include <htsopt.h>
#include <htsdefines.h>

#include <warc.h> /* WARC headers */

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
                           const char * file)
{
  
  void * w = (void *) CALLBACKARG_USERDEF (carg);
  
  void * r = NIL;
  void * u = NIL;
  
  
/*  if (CALLBACKARG_PREV_FUN(carg, filesave) != NULL) 
    {
      if (!CALLBACKARG_PREV_FUN (carg, filesave)(CALLBACKARG_PREV_CARG(carg), opt, file))
          return ;  
    }*/

  r = bless (WRecord);

  unless (r)
    return ;

  u = bless (WUUID);

  unless (u)
    {
     destroy (r);
     return ;
    }

  /* TU DEVRAIS TESTER LA AVLEUR DE RETOUR DES FONCTIONS LYES ICI */

  WRecord_setRecordType (r, WARC_RESOURCE_RECORD);
  Wrecord_setTargetUri  (r, makeS (uS("http://www.iipc.net")));
  WRecord_setDate       (r, makeS (uS("2008-09-10T00:00:00Z")));
  WRecord_setContentType(r, makeS (uS("octet/stream")));

  WUUID_hash (u, makeS(uS("onestring")));
  WUUID_hash (u, makeS(uS("towstring")));

  WRecord_setRecordId(r, makeS (WUUID_text (u) ));

  destroy (u);

  WRecord_setIpAddress(r, makeS(uS("0.0.0.0")));

  WRecord_setContentFromFileName (r, file);

  WFile_storeRecord (w, r);
}

/* local function called as "end" callback */
static int end_of_mirror(t_hts_callbackarg *carg, httrackp *opt)

{
  void * w = (void *) CALLBACKARG_USERDEF (carg);


  destroy (w);

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

  /* optional argument passed in the commandline we won't be using here */
  const char *arg = strchr(argv, ',');

  if (arg != NULL)
    arg ++;

#define WARC_TMP    "."
#define OUTPUT_WARC "outfile.warc"
#define WARC_SIZE   (1024 * 1024 * 1024)

  void * w = bless (WFile, OUTPUT_WARC, WARC_SIZE, 
                    WARC_FILE_COMPRESSED_GZIP, WARC_FILE_WRITER, 
                    WARC_TMP);

  unless (w)
    return (0); /* failure */

  /* plug callback functions */
  CHAIN_FUNCTION(opt, check_html, process_file,  & w);
  CHAIN_FUNCTION(opt, filesave,   store_in_warc, & w);
  CHAIN_FUNCTION(opt, end,        end_of_mirror, & w);

  return 1;  /* success */
}

/*
  module exit point the function name and prototype MUST match this prototype
*/

EXTERNAL_FUNCTION int hts_unplug(httrackp *opt) {
  fprintf(stderr, "Module unplugged");

  return 1;  /* success */
}
