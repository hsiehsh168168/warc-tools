
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

#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))

#define free_obj(x,ret) destroy (x);return (ret)

int main (int argc, const char ** argv)
{
  void           * s       = NIL; /* WClient object */
  void           * p       = NIL; /* WGetOpt object */
  warc_u8_t      * flags   = uS("i:p:o:crf:t:");
  warc_u8_t      * ip      = uS("0.0.0.0");
  warc_u8_t      * ps      = NIL;
  warc_u8_t      * off     = NIL;
  warc_u8_t      * wfn     = NIL;
  warc_u8_t      * target  = NIL;
  warc_bool_t      request = WARC_FALSE;
  warc_u8_t      * comp    = uS("uncompressed");
  warc_u32_t       port    = 0;
  warc_u32_t       offset  = 0;
  warc_i32_t       c       = 0;

  if (argc <= 7)
    {
      fprintf (stderr, "WARC client to access remote WARC resources\n");
      fprintf (stderr, "Usage: %s -i <ip> -p <port> -t <remote_warc> -o <local_warc> [-f <offset>] [-c] [-r]\n", argv [0]);
      fprintf (stderr, "\t[-i]  : ip address\n");
      fprintf (stderr, "\t[-p]  : port number\n");
      fprintf (stderr, "\t[-t]  : remote WARC filename\n");
      fprintf (stderr, "\t[-o]  : output WARC filename\n");
      fprintf (stderr, "\t[-c]  : assume GZIP compressed WARC file (default no)\n");
      fprintf (stderr, "\t[-f]  : WARC offset (default 0)\n");
      fprintf (stderr, "\t[-r]  : get all the WARC file (default no)\n");
      return (1);
    }


  /* parse command line parameters */
  p = bless (WGetOpt, makeS (flags) );
  assert (p);

  while ( (c = WGetOpt_parse (p, argc, argv) ) != -1)
    {
      switch (c)
        {
          case 'i' :
            if (w_index (flags, c) [1] == ':')
              ip = (warc_u8_t *) WGetOpt_argument (p);

            break;

          case 'p' :
           if (w_index (flags, c) [1] == ':')
             {
               ps = (warc_u8_t *) WGetOpt_argument (p);
               if (w_atou ( ps, w_strlen(ps), & port))
                 {
                   fprintf (stderr, "invalid port number: %s\n", ps);
                   free_obj(p, 2);
                 }
             }
            
            break;
            
        case 'o' :
          if (w_index (flags, c) [1] == ':')
            wfn = (warc_u8_t *) WGetOpt_argument (p);

          break;
          
        case 'c' :
          comp = uS("gzip");
          break;
          
        case 'f' :
          if (w_index (flags, c) [1] == ':')
            {
               off = (warc_u8_t *) WGetOpt_argument (p);
               if (w_atou (off, w_strlen(ps), & offset))
                 {
                   fprintf (stderr, "invalid offset number: %s\n", off);
                   free_obj(p, 3);
                 }
             }
          
          break;

        case 't' :
          if (w_index (flags, c) [1] == ':')
            target = (warc_u8_t *) WGetOpt_argument (p);

          break;

        case 'r' :
          request = WARC_TRUE;
          break;


        case '?' :  /* illegal option or missing argument */
          free_obj(p, 4);
        }
    }
  
  if (ip == NIL || * ip == '\0')
    {
      fprintf (stderr, "error: empty IP address\n");
      free_obj(p, 5);
    }

 if (wfn == NIL || * wfn == '\0')
    {
      fprintf (stderr, "error: empty output WARC filename\n");
      free_obj(p, 6);
    }


  s = bless (WClient, makeS(ip), port);
  unless (s)
    {
     fprintf (stderr, "Unable to create the WARC client using the IP address %s on the port %d.\n", ip, port);
     free_obj(p, 7);
    }
  
  destroy (p);

  if (request)
    {
      if (WClient_getWFile (s, makeS(comp), offset, makeS(target), wfn))
        {
          fprintf (stderr, "WClient_getWFile error : request not satisfied\n");
          free_obj(s, 8);
        }
    }
  else
    {
      if (WClient_getWRecord (s, makeS(comp), offset, makeS(target), wfn))
        {
          fprintf (stderr, "WClient_getWRecord error: request not satisfied\n");
          free_obj(s, 9);
        }
    }

  destroy (s);
  
  return (0);
}
