#include <stdio.h>
#include <string.h>


#include <warc.h>


#ifndef WARC_MAX_SIZE
#define WARC_MAX_SIZE 1629145600
#endif

#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))

#define CR   ('\r')
#define LF   ('\n')
#define M(c) (((c) == CR) || ((c) == LF))
#define X(c) (! (M(c)) )

enum states { S1 = 1, S2, S3, S4, S5, S11, S12};
enum actions {SEARCHING_HTTP = 1, IGNORE_ALL, PRINTING};

struct user_struct {
   warc_u32_t * fsm;
   warc_u32_t * action;
   void * line; /* a WString object */
   warc_bool_t was_cr;
   warc_u32_t next_offset;
};

warc_bool_t callback (void * env, const char* buff, const warc_u32_t size)
{
  struct user_struct * user_data = env;

  warc_u32_t * fsm = user_data -> fsm;
  warc_u32_t * action = user_data -> action;
  void * line = user_data -> line;
  void * regexp = NIL;
  
  warc_u32_t i = 0;

check_action:
switch (*action)
   {
    case SEARCHING_HTTP: /* Here, we look for the HTTP response code */

         while (i < size)
             {
              if (buff [i] == CR)
                 {
                  i++;
                  user_data -> was_cr = WARC_TRUE;

                  if (i == size)
                     break;
                 }
               /* if we detecte a CRLF */
              if (user_data -> was_cr && buff[i] == LF)
                 {
                 user_data -> was_cr = WARC_FALSE;
                 unless (w_strcmp (WString_getText (line), uS ("")))
                      {
                       fprintf (stderr, "\t997 %d\n", user_data -> next_offset); /* we found a double CRLF but no HTTP Response code */
                       i++;
                       *action = PRINTING; /*what comes next is considered as the content and it will be printed*/
                       goto check_action;
                      }

                  regexp = bless (WRegexp, makeS("HTTP.+"));

                  if (WRegexp_match (regexp, WString_getText (line), WString_getLength(line))) /* we chack if it is the HTTP code line */
                     {
                     /* if it is, we print the code */
                      *action = IGNORE_ALL;
                      i++;
                      fprintf (stderr, "\t%c", * (WString_getText (line) + 9));
                      fprintf (stderr, "%c", * (WString_getText (line) + 10));
                      fprintf (stderr, "%c %d\n", * (WString_getText (line) + 11), user_data -> next_offset);
                      *fsm = S11;
                      destroy (regexp);
                      goto check_action;
                     }
                 /* else, we get an other line */
                 destroy (regexp);
                 WString_setText (line, makeS (""));
                 }
             else
                 {
                  if (user_data -> was_cr)
                      {
                       user_data -> was_cr = WARC_FALSE;
                       WString_append (line, uS("\r"), 1);
                       i--;
                      }
                  else
                     WString_append (line, uS(buff + i), 1);
                 }
              i++;
             }

         break;

    case IGNORE_ALL: /* here, we ignore th rest of the HTTP response until finding 2 CRLF*/

         while (*fsm != S5 && i < size)
             {
              if (*fsm == S11) /* State 11, we come from a detection of the HTTP code line */
                 {
                  if (buff [i] == CR) /* if the character is a CR, xwe look for the LF */
                    *fsm = S12;
                  else
                    *fsm = S1;
                  }
              else if (*fsm == S12) /* State 12, from state 11 */
                 {
                  if (buff [i] == LF) /* If we find an LF, we end the ognoring phase */
                    *fsm = S5;
                  else
                    *fsm = S1;
                 }
              else if (*fsm == S1) /* state 1, we look for the first CR */
                 {
                  if (buff [i] == CR) /* we go looking for the first LF */
                    *fsm = S2;
                  else
                    *fsm = S1;
                 }
              else if (*fsm == S2) /* state 2, we look for the first LF */
                 {
                  if (buff [i] == LF) /* we go looking for the second CR */
                    *fsm = S3;
                  else
                    *fsm = S1;
                 }
              else if (*fsm == S3) /* state 3, we lok for the second CR */
                 {
                  if (buff [i] == CR) /* we go looking for the second LF */
                    *fsm = S4;
                  else
                    *fsm = S1;
                 }
              else if (*fsm == S4) /* state 4, we lok for the second LF */
                 {
                  if (buff [i] == LF) /* double CRLF found, we end the ignoring phase */
                    *fsm = S5;
                  else
                    *fsm = S1;
                 }

             i++;
             }

         if (*fsm == S5)
            {
             *action = PRINTING;
             if (i < size) /* writing what reamins in the buffer on the stdout */
                if (fwrite( (buff + i), 1, size - i, stdout) != size - i || ferror(stdout))
                   {
                    destroy (line);
                    user_data -> line = NIL;
                    return (WARC_FALSE);
                   }
            }
         break;

   case PRINTING: /* writing what reamins in the data bloc on the stdout */

        if (fwrite( (buff + i), 1, size - i, stdout) != size - i || ferror(stdout))
           {
            destroy (line);
            user_data -> line = NIL;
            return (WARC_FALSE);
           }

        break;

   default: break;
   }

  return (WARC_TRUE);
}


void printOrdinaryDate (warc_u8_t * field)
{
warc_u32_t i = 0;
warc_u32_t size = w_strlen (field);

fprintf (stderr, "\t");

while (i < size)
  {
   if (field[i] != ':' && field[i] != '-' && field[i] != 'Z' && field[i] != 'T' && field[i] != 't' && field[i] != 'z')
     fprintf (stderr, "%c", field [i]);
   i++;
  }
}


int main (int argc, const char ** argv)
{
  warc_u8_t      * off     = NIL;
  warc_u8_t      * fname   = NIL;
  warc_u8_t      * field   = NIL;
  warc_u8_t      * wdir    = uS(".");
  warc_u32_t       offset  = 0;
  warc_u32_t       special_code = 0;
  warc_u8_t       * flags    = uS ("f:o:t:");
  warc_i32_t       c;
  wfile_comp_t     cmode   = WARC_FILE_DETECT_COMPRESSION;

  void           * r = NIL;
  void           * w = NIL;
  void           * line = NIL;
  void           * p = NIL;
  void           * regexp1 = NIL;
  void           * regexp2 = NIL;

  warc_u32_t     fsm = 0;
  warc_u32_t     act = 0;
  struct user_struct user_data;

  if (argc < 3 || argc > 7)
    {
      fprintf (stderr, "Extract WARC's content block only\n");
      fprintf (stderr, "Usage: %s -f <file.warc> [-v] [-t <working_dir>]\n", argv [0]);
      fprintf (stderr, "\t-f   : valid WARC file name\n");
      fprintf (stderr, "\t-o   : the offset of the record\n");
      fprintf (stderr, "\t[-t] : temporary working directory (default \".\")\n");
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
              fname = uS(WGetOpt_argument (p));

            break;


          case 'o' :
          
            if (w_index (flags, c) [1] == ':')
              off = (warc_u8_t *) WGetOpt_argument (p);
               if (w_atou (off, w_strlen(off), & offset))
                 {
                   fprintf (stderr, "invalid offset number: %s\n", off);
                   destroy (p);
                   return (3);
                 }
          
            break;

          case 't' :
          
            if (w_index (flags, c) [1] == ':')
                wdir = uS(WGetOpt_argument (p));
          break;

          case '?' :  /* illegal option or missing argument */
            destroy (p);

            return (1);
        }
    }

  unless (fname)

  {
    fprintf (stderr, "missing WARC file name. Use -f option\n");
    return (4);
  }

  w = bless (WFile, fname, WARC_MAX_SIZE,
             WARC_FILE_READER, cmode, wdir);

  unless (w)
   {
    fprintf (stderr, "Could not open WARC file %s\n", fname);
    return (5);
   }

destroy (p);

  if (WFile_seek (w, offset))
     {
      fprintf (stderr, "Could not reach the offset %d\n", offset);
      destroy (w);
      return (9);
     }

  unless (WFile_hasMoreRecords (w))
      {
       fprintf (stderr, "End of file reached\n");
       destroy (w);
       return (6);
      }

  r = WFile_nextRecord (w);

  unless (r)
      {
      fprintf (stderr, "No valid record at this offset \n");
      destroy (w);
      return (7);
      }

  field = uS(WRecord_getTargetUri (r));

  unless (field)
      field = uS("unknown");
  else unless (w_strcmp (field, uS("")))
      field = uS("unknown");

  fprintf (stderr, "\"%s\"", field);

  regexp1 = bless (WRegexp, makeS("filedesc://.*"));
  regexp2 = bless (WRegexp, makeS ("dns:.*"));

  if (WRegexp_match (regexp1, field, w_strlen (field)))
     special_code = 998;
  else if (WRegexp_match (regexp2, field, w_strlen (field)))
     special_code = 999;


  destroy (regexp1);
  destroy (regexp2);

  field = NIL;

  field = uS(WRecord_getDate (r));

  printOrdinaryDate (field);

  field = NIL;

  field = uS(WRecord_getPayloadType (r));

  unless (field || (field != NIL && !w_strcmp (field, uS(""))))
     {
      field = uS(WRecord_getContentType (r));
      unless (field || (field != NIL && !w_strcmp (field, uS(""))))
           field = uS("unknown");
     }

  fprintf (stderr, "\t%s", field);
  field = NIL;

  line = bless (WString, makeS (""));
  fsm = S1;

  if (special_code)
      {
       fprintf (stderr, "\t%d\t%d\n", special_code, (warc_u32_t) (offset + WRecord_getCompressedSize (r)));
       act = PRINTING;
      }
  else
     {
      act = SEARCHING_HTTP;
      user_data . next_offset = offset + WRecord_getCompressedSize (r);
     }

  user_data . fsm = & fsm;
  user_data . action = & act;
  user_data . line = line;
  user_data . was_cr = WARC_FALSE;


  WFile_register (w, r, callback, (void *) & user_data);

  WRecord_getContent (r);

  unless (line)
     {
      fprintf (stderr, "A problem appeared while reading data \n");
      destroy (w);
      destroy (r);
      return (8);
     }

destroy (line);
destroy (w);
destroy (r);
return (0);
}
