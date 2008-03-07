
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

#include <afile.h>
#include <arecord.h>


#define WARC_MAX_SIZE 629145600

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
#define makeU(s) (const warc_u8_t *) (s), (warc_u64_t) w_strlen((warc_u8_t *) (s))

/* useful macros to simplify releasing objects */
#define free_p \
  destroy (p)

#define free_a \
  destroy (a)

#define free_w \
  destroy (w)

#define free_u \
  destroy (u)

#define free_out \
  do{ \
      free_p; \
      free_u; \
      free_w; \
      free_a; \
    }while(0)

#define free_err_out(ret) \
  do{ \
      free_out; \
      return (ret); \
    }while(0)

#define free_ar \
  destroy (ar)

#define free_wr \
  destroy (wr)

#define free_in \
  do{ \
      free_wr; \
      free_ar; \
    }while(0)

#define free_err_in(ret) \
  do{ \
      free_in; \
      return (ret); \
    }while(0)

#define free_err(ret) \
  do{ \
      free_out;  \
      free_in; \
      return (ret); \
    }while(0)



int main (int argc, const char ** argv)
{
  void           * p        = NIL; /* WGetOpt object */
  void           * a        = NIL; /* an ARC file object */
  void           * w        = NIL; /* a WARC file object */
  void           * u        = NIL; /* a UUID object */
  char           * aname    = NIL;
  afile_comp_t     amode    = ARC_FILE_COMPRESSED_GZIP;
  warc_bool_t      b        = WARC_FALSE;
  warc_i32_t       c        = 0;
  warc_u8_t      * flags    = (warc_u8_t *) "bca:f:";
  char           * fname    = NIL;
  wfile_comp_t     cmode    = WARC_FILE_COMPRESSED_GZIP;


  if (argc < 5 || argc > 7)
    {
      fprintf (stderr, "ARC to WARC convertor\n");
      fprintf (stderr, "Usage: %s -a <file.arc> [-b] -f <file.warc> [-c]\n",
               argv [0]);
      fprintf (stderr, "\t-a    : valid ARC file name\n");
      fprintf (stderr, "\t[-b]  : assume ARC file is GZIP compressed (default yes)\n");
      fprintf (stderr, "\t-f    : valid WARC file name\n");
      fprintf (stderr, "\t[-c]  : WARC file will be GZIP compressed (default yes)\n");
      return (2);
    }

  p = bless (WGetOpt, makeS (flags) );

  assert (p);

  /* parse command line parameters */

  while ( (c = WGetOpt_parse (p, argc, argv) ) != -1)
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

          case 'a' :
            if (w_index (flags, c) [1] == ':')
              aname = WGetOpt_argument (p);

            break;

          case 'b' :
            amode = WARC_FILE_UNCOMPRESSED;

            break;

          case '?' :  /* illegal option or missing argument */
            destroy (p);

            return (1);
        }
    }

  unless (aname)

  {
    fprintf (stderr, "missing ARC file name. Use -a option\n");
    destroy (p);
    return (1);
  }

  unless (fname)
  {
    fprintf (stderr, "missing WARC file name. Use -f option\n");
    destroy (p);
    return (1);
  }


  /* open an existing ARC file */
  a = bless (AFile, aname, amode);
  unless (a)
  {
    fprintf (stderr, "unable to create the Arc object\n");
    return (2);
  }

  /* open or create a WARC file */
  w = bless (WFile, fname, WARC_MAX_SIZE, WARC_FILE_WRITER, cmode);
  unless (w)
  {
    fprintf (stderr, "unable to create the Warc object\n");
    free_a;
    return (3);
  }

  /* create a UUID object */
  u = bless (WUUID);
  unless (u)
  {
    fprintf (stderr, "unable to create a UUID object\n");
    free_w;
    free_a;
    return (4);
  }


  /* loop over all ARC records */

  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;

      /* check the next ARC record */
      unless (ar)
      {
        fprintf (stderr, "corrupted ARC\n");
        free_err_out ( 5);
      }

      /* create an empty WARC record */
      wr = bless (WRecord);
      unless (wr)
      {
        fprintf (stderr, "unable to create the WARC record object\n");
        free_ar;
        free_err_out (6);
      }

      /* set the subject URI */
      b = WRecord_setSubjectUri  (wr, makeS (ARecord_getUrl (ar) ) );

      if (b)
        free_err (7);

      /* set the record tyep */
      b = WRecord_setRecordType  (wr, RESOURCE_RECORD);

      if (b)
        free_err (8);

      /* set the creation date */
      b = WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar) ) );

      if (b)
        free_err (9);

      /* set the content type */
      b = WRecord_setContentType  (wr, makeS (ARecord_getMimeType (ar) ) );

      if (b)
        free_err (10);

      /* Create a UUID (Universal Unique IDentifier) based on URL + Timestamp */
      WUUID_hash (u, makeU (ARecord_getUrl (ar) ) );

      WUUID_hash (u, makeU (ARecord_getCreationDate (ar) ) );

      b = WRecord_setRecordId (wr, makeS (WUUID_text (u) ) );

      if (b)
        free_err (11);

      WUUID_reinit (u); /* re-initialize the UUID object */

      /* add the ARC IP as an Anvl */
      b = WRecord_addAnvl (wr, makeS ("IpAddress"),
                           makeS (ARecord_getIpAddress (ar) ) );

      if (b)
        free_err (12);

      /* move the ARC record payload to the WARC record */
      b = ARecord_transferContent (ar, wr, a);

      if (b)
        free_err (13);

      /* save the WARC record into the WARC file */
      b = WFile_storeRecord (w, wr);

      if (b)
        free_err (14);

      /* free the ARC and the WARC records */
      free_in;

    } /* end of while */

  /* free the ARC and the WARC files */
  free_out;

  return (0);
}
