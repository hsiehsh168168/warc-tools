
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

#include <wgzip.h>       /* WGzip */

struct CallbackEnv
  {
    FILE     * out;
    warc_u32_t crlf;
    warc_u64_t usize;
  };

/* this callback uncompressing the entire WARC file */
warc_u32_t callback1_warc (const warc_u8_t * buffer, const warc_u32_t nbr, void * env)
{
  FILE * out = (FILE *) env;

  /* copy the uncompressed 'nbr' bytes to out */

  if (fwrite (buffer, 1, nbr, out) != nbr || ferror (out) )
    return (Z_STOP_DECODING); /* return this value to stop the uncompression */

  return (Z_CONTINUE_DECODING);
}



/* this callback stop uncompressing when it find a double CRLF in a WARC file */
warc_u32_t callback2_warc (const warc_u8_t * buffer, const warc_u32_t nbr, void * _env)
{

  struct CallbackEnv * env  = (struct CallbackEnv *) _env;
  warc_u32_t           i    = 0;

  switch (env -> crlf)
    {

      case 0 :
        goto CR1;

      case 1 :
        goto LF1;

      case 2 :
        goto CR2;

      case 3 :
        goto LF2;
    }

CR1:

  while (i < nbr) /* search the first CR */
    {
      if (buffer [i] == '\r')
        {
          env -> crlf ++;
          ++ i;
          goto LF1;
        }

      ++ i;
    }

  goto CONTINUE;

LF1:

  if (i < nbr)
    {
      if (buffer [i] == '\n')
        {
          env -> crlf ++;
          i++;
          goto CR2;
        }

      else
        {
          env -> crlf = 0;
          goto CONTINUE;
        }
    }

  else
    goto CONTINUE;

CR2:
  if (i < nbr)
    {
      if (buffer [i] == '\r')
        {
          env -> crlf ++;
          ++ i;
          goto LF2;
        }

      else
        {
          env -> crlf = 0;
          goto CONTINUE;
        }
    }

  else
    goto CONTINUE;

LF2:
  if (i < nbr)
    {
      if (buffer [i] == '\n')
        {
          goto STOP;
        }

      else
        {
          env -> crlf = 0;
          goto CONTINUE;
        }
    }

  else
    goto CONTINUE;

STOP:

  /* we've found a double CRLF (stop uncompressing)  */
  env -> usize += i + 1;

  fwrite (buffer, 1, i + 1, env -> out);

  return (Z_STOP_DECODING);

CONTINUE:
  /* copy the uncompressed 'nbr' bytes to out */
  if (fwrite (buffer, 1, nbr, env -> out) != nbr || ferror (env -> out) )
    return (Z_STOP_DECODING);

  return (Z_CONTINUE_DECODING);
}


/* this callback stop uncompressing when it finds 1 CRLF or 1 LF in ARC file */
warc_u32_t callback3_arc (const warc_u8_t * buffer, const warc_u32_t nbr, void * _env)
{

  struct CallbackEnv * env  = (struct CallbackEnv *) _env;
  warc_u32_t           i    = 0;

  switch (env -> crlf)
    {

      case 0 :
        goto CR_OR_LF;

      case 1 :
        goto LF1;
    }

CR_OR_LF:

  while (i < nbr) /* search the first CR */
    {
      if (buffer [i] == '\r')
        {
          env -> crlf ++;
          ++ i;
          goto LF1;
        }

      if (buffer [i] == '\n')
        {
          goto STOP;
        }

      ++ i;
    }

  goto CONTINUE;

LF1:

  if (i < nbr)
    {
      if (buffer [i] == '\n')
        {
          goto STOP;
        }

      else
        {
          env -> crlf = 0;
          goto CONTINUE;
        }
    }

  else
    goto CONTINUE;

STOP:

  /* we've found a double CRLF (stop uncompressing)  */
  env -> usize += i + 1;

  fwrite (buffer, 1, i + 1, env -> out);

  return (Z_STOP_DECODING);

CONTINUE:
  /* copy the uncompressed 'nbr' bytes to out */
  if (fwrite (buffer, 1, nbr, env -> out) != nbr || ferror (env -> out) )
    return (Z_STOP_DECODING);

  return (Z_CONTINUE_DECODING);
}


/* this is a callback for uncompressing the entire ARC record */
warc_u32_t callback4_arc (const warc_u8_t * buffer,
                          const warc_u32_t nbr, void * env)
{
  FILE * out = (FILE *) env;
  
  /* copy the uncompressed 'nbr' bytes to out */
  if (w_fwrite (buffer, 1, nbr, out) != nbr || w_ferror (out) )
    return (Z_STOP_DECODING); /* return this value to stop the uncompression */
  
  return (Z_CONTINUE_DECODING);
}


FILE * openReading (const char * fin)
{
  FILE * in;
  unless (in = fopen (fin, "r+b") )
  {
    fprintf (stdout, "error: cannot open file \"%s\" for reading\n",
             fin);
    return NIL;
  }

  return in;
}

FILE * openWriting (const char * fout)
{
  FILE * out;
  unless (out = fopen (fout, "w+b") )
  {
    fprintf (stdout, "error: cannot open file \"%s\" for binary writing\n",
             fout);
    return NIL;
  }

  return out;
}


int test1 (const char * fin)
{
  const char * t     = "TEST 1";
  const char * fout  = "uncompressed1";
  FILE       * in    = NIL;
  FILE       * out   = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_i32_t   ret   = 0;
  warc_u64_t   usize = 0;   /* uncompressed file size */
  warc_u64_t   csize = 0;   /* compressed file size */


  fprintf (stdout, "%s>\n", t);

  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);

  /* uncompress file from offset 0 using the callback with env = fout */
  ret = WGzip_uncompress (g, in, 0, & usize, & csize,
                          callback1_warc, (void *) out);
  assert (! ret);
  fprintf (stdout,
           "uncompressed \"%s\" to \"%s\" [usize: %llu][csize: %llu]\n",
           fin, fout, (unsigned long long) usize, (unsigned long long) csize);

  fclose (out);
  fclose (in);

  destroy (g);

  return 0;
}


int test2 (const char * fin)
{
  const char * t     = "TEST 2";
  const char * fout  = "uncompressed2";
  FILE       * in    = NIL;
  FILE       * out   = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_i32_t   ret   = 0;
  warc_u64_t   usize = 0;   /* uncompressed file size */
  warc_u64_t   csize = 0;   /* compressed file size */

  struct CallbackEnv cenv;

  fprintf (stdout, "%s>\n", t);

  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);

  /* set the environment variable */
  cenv . out   = out;
  cenv . crlf  = 0;
  cenv . usize = 0;

  /* uncompress file from offset 0 using the callback with env = fout */
  ret = WGzip_uncompress (g, in, 0, & usize, & csize,
                          callback2_warc, (void *) & cenv);
  assert (! ret);
  fprintf (stdout,
           "uncompressed \"%s\" to \"%s\" [usize: %llu][csize: %llu]\n",
           fin, fout,
           (unsigned long long) cenv . usize, (unsigned long long) csize);

  fclose (out);
  fclose (in);

  destroy (g);

  return 0;
}



int test3 (const char * fin)
{
  const char * t      = "TEST 3";
  const char * fout   = "uncompressed3";
  FILE       * in     = NIL;
  FILE       * out    = NIL;
  void       * g      = NIL; /* WGzip object */
  warc_i32_t   ret    = 0;
  warc_u64_t   usize  = 0;   /* uncompressed file size */
  warc_u64_t   csize  = 0;   /* compressed file size */
  warc_u64_t   offset = 0;   /* WARC record offset */

  fprintf (stdout, "%s>\n", t);

  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);

 
  /* first record at offset 0 */
  offset = 0;

  /* loop over all records */

  while (WARC_TRUE)
    {
      /* uncompress file from offset 0 using the callback with env = fout */
      ret = WGzip_uncompress (g, in, offset, & usize, & csize,
                              callback4_arc, (void *) out);
      if (ret)
        break;

      fprintf (stdout,
               "uncompressed \"%s\" to \"%s\" [usize: %llu][csize: %llu]\n",
               fin, fout,
               (unsigned long long) usize, (unsigned long long) csize);

      /* goto to the next record */
      offset += csize;
    }

  fclose (out);

  fclose (in);

  destroy (g);

  return 0;
}


int test4 (const char * fin)
{
  const char * t      = "TEST 4";
  const char * fout   = "uncompressed4";
  FILE       * in     = NIL;
  FILE       * out    = NIL;
  void       * g      = NIL; /* WGzip object */
  warc_i32_t   ret    = 0;
  warc_u64_t   usize  = 0;   /* uncompressed file size */
  warc_u64_t   csize  = 0;   /* compressed file size */
  warc_u64_t   offset = 0;   /* WARC record offset */

  fprintf (stdout, "%s>\n", t);

  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);

  /* first record at offset 0 */
  offset = 0;

  /* loop over all records */

  while (WARC_TRUE)
    {
      ret = WGzip_uncompress (g, in, offset, & usize, & csize,
                              callback1_warc, (void *) out);

      if (ret)
        break;

      fprintf (stdout,
               "uncompressed \"%s\" to \"%s\" [usize: %llu][csize: %llu]\n",
               fin, fout,
               (unsigned long long) usize, (unsigned long long) csize);

      /* goto to the next record */
      offset += csize;
    }

  fclose (out);

  fclose (in);

  destroy (g);

  return 0;
}

int test5 (const char * fin)
{
  const char * t     = "TEST 5";
  FILE       * in    = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_bool_t  ret   = WARC_FALSE;

  fprintf (stdout, "%s>\n", t);

  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  /* check the validity of GZIP file from offset 0 */
  ret = WGzip_check (g, in, 0);
  unless (ret)
  w_fprintf (fprintf (stdout, "\"%s\" is a valid GZIP file\n", fin) );
  else
    w_fprintf (fprintf (stdout, "\"%s\" is an invalid GZIP file\n", fin) );

  fclose (in);

  destroy (g);

  return 0;
}


int main (int argc, char ** argv)
{
  const char * f1 = "app/wdata/testwfile/file.warc.gz";
  const char * f2 = "app/wdata/testarc/test.arc.gz";
 
  UNUSED (argc);
  UNUSED (argv);

  /* uncomment to try a spesific test */
  test1 (f1);
  test2 (f1);
  test3 (f2);
  test4 (f1);
  test5 (f1);
  test5 (f2);

  return 0;
}
