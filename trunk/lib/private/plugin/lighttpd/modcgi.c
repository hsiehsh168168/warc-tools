
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
#include <stdlib.h>
#include <warc.h>
#include <wfile.x>
#include <wmem.h>
#define uS(s) (warc_u8_t *)(s)
#define makeS(s) uS(s),w_strlen(uS(s))
#define makeWString(s) bless (WString, uS(s), w_strlen((uS(s))))

#ifndef WARC_MAX_SIZE
#define WARC_MAX_SIZE 629145600
#define MAX_BUFFER_SIZE 512
#endif




WPRIVATE void  * recoverUriField ( const char * uri,warc_u32_t * position)
{

  void * item = bless (WString, "", 0);  /* the return item */
  warc_u8_t achar[2] = {' ', '\0'};     /* to append item characters */
  warc_u32_t i = * (position);


  while ( uri [i] == '/')
        i ++ ;
  
  while ( uri [i] != '/')
        {
          achar [0] = uri [i];
          if (WString_append (item, achar, 1))
             {
               destroy (item);
               return (NIL);
             }
          i ++;
        }
  * (position) = i ;

  return item;
}

/**
 * @param str: sample string
 * @param position: intger 
 * 
 * @return The file name from uri
 *
 * return the file name (the last field of uri)
 **/

WPRIVATE void  * recoverFName ( const char * uri,warc_u32_t * position)
{

  void * item = bless (WString, "", 0);  /* the return item */
  warc_u8_t achar[2] = {' ', '\0'};     /* to append item characters */
  warc_u32_t i = * (position);
  warc_u32_t  k; 
  achar [0] ='/';
  WString_append (item, achar, 1);

  while ( uri [i] == '/')
        i ++ ;

  for (k = i; k < strlen (uri ); k++)
      {
      achar [0] = uri [k];
      if (WString_append (item, achar, 1))
             {
               destroy (item);
               return (NIL);
             }
      
      }
  * (position) = i ;

  return item;
}

/**
 * @param fname: the name of the wanted file
 * @param offset: the offset from where the data will be sent
 *
 * @return False if succeed, True otherwise
 *
 * Sends a full file response to the client
 */

WPRIVATE warc_bool_t WSend_fullResponse (const warc_u8_t * fname, wfile_comp_t cmode, warc_u32_t offset)
{
   FILE          *   f        = NIL;
   warc_u8_t     *   buffer   = NIL;
   warc_u32_t        fsize    = 0  ;
   warc_u32_t        filled   = 0  ;
   warc_u32_t        sentsize = 0  ;


  buffer = wmalloc (MAX_BUFFER_SIZE);
  
  unless (buffer)
    {
    fprintf (stdout, "content-type: text/html\n\n");
    fprintf (stdout, "<html>\n<body>\n");
    fprintf (stdout, "<font color=red size = 4>Failed in allocation of memory for respponse sending </font><br>");
    
    return (WARC_TRUE);
    }

  f = w_fopen_rb ((char *) fname);

  unless (f)
    {
    fprintf (stdout, "content-type: text/html\n\n");
    fprintf (stdout, "<html>\n<body>\n"); 
    fprintf (stdout, "<font color=red size = 4>File not found</font><br>");
    wfree (buffer);
    
    return (WARC_TRUE);
    }

   if (cmode == WARC_FILE_COMPRESSED_GZIP)
          fprintf (stdout, "content-type: application/x-gzip, \n\n");
          

    else
          fprintf (stdout, "content-type: text/random, \n\n");
          

  w_file_size_from_offset (f, fsize, offset);
  w_fseek_from_start (f, offset);

  while (fsize)
    {
     if (fsize > MAX_BUFFER_SIZE)
        {
        sentsize = MAX_BUFFER_SIZE;
        fsize = fsize - MAX_BUFFER_SIZE;
        }
     else
        {
        sentsize = fsize;
        fsize = 0;
        }
    filled = w_fread (buffer, 1, sentsize, f);
    fwrite (buffer, filled, 1, stdout);
 
    }
 
  wfree (buffer);
  w_fclose (f);
  return (WARC_FALSE);

}

/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param cmode: the compression mode
 * @param offset: the offset from where the data will be sent
 *
 * @return False if succeed, True otherwise
 *
 * Sends a Warc Record Response
 */

WPRIVATE warc_bool_t WSend_record (const warc_u8_t * fname, const warc_u8_t * prefix, wfile_comp_t cmode, warc_u32_t offset)
{
    void        *     w         = NIL;
    void        *     record    = NIL;
    warc_u8_t   *     buffer    = NIL;
    warc_u32_t        rsize     = 0  ;
    warc_u32_t        sentsize  = 0  ;
    warc_i32_t        filled    = 0  ;


    w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, cmode, prefix);

    unless (w)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout,  "<font color=red size = 4>File not found</font><br>");
      return (WARC_TRUE);
      }

    if (WFile_seek (w, offset))
       {
        fprintf (stdout, "content-type: text/html\n\n");
        fprintf (stdout, "<html>\n<body>\n");
        fprintf (stdout, "<font color = red size = 4 > Can not reach the reclaimed offset </font><br>");
        destroy (w);
        return (WARC_TRUE);
       }

    record = WFile_nextRecord (w);

    unless (record)
       {
        fprintf (stdout, "content-type: text/html\n\n");
        fprintf (stdout, "<html>\n<body>\n");
        fprintf (stdout, "<font color = red size = 4 > No records at this offset</font><br>");
        
        destroy (w);
        return (WARC_TRUE);
       } 

    WFile_seek (w, offset);

    buffer = wmalloc (MAX_BUFFER_SIZE);
 
    unless (buffer)
     {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout, "<font color = red size = 4 >Failed in allocation of memory for respponse sending </font><br>");
      destroy (w);
      destroy (record);
      return (WARC_TRUE);
     }
    
    
    if (cmode == WARC_FILE_COMPRESSED_GZIP)
          fprintf (stdout, "content-type: application/x-gzip, \n\n");
          

    else
          fprintf (stdout, "content-type: text/random, \n\n");

   
    

    rsize = WRecord_getCompressedSize (record);
 
  
  
    while (rsize)
      {
       if (rsize > MAX_BUFFER_SIZE)
          {
          sentsize = MAX_BUFFER_SIZE;
          rsize = rsize - MAX_BUFFER_SIZE;
          }
       else
          {
          sentsize = rsize;
          rsize = 0;
          }
      WFile_fillBuffer  (w, (char *) buffer, sentsize, & filled);
      fwrite (buffer, filled, 1, stdout);
      }
 
    wfree (buffer);
    destroy (record);
    destroy (w);
    return (WARC_FALSE);
}

/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param cmode: the compression mode
 * @param offset: the offset from where the data will be sent
 * @param fdfilter: fileld to filter (uri or content type)
 * @param filter: value of filter
 *
 * @return False if succeed, True otherwise
 *
 * Sends a filtred Warc file Response
 */


WPRIVATE warc_bool_t WSend_filtredResponse (const warc_u8_t * fname, const warc_u8_t * prefix, wfile_comp_t cmode, warc_u32_t offset,
                                const warc_u8_t * fdfilter,const warc_u8_t * filter)
{
  void         *    w         = NIL;
  void         *    record    = NIL;
  warc_u8_t    *    buffer    = NIL;
  const warc_u8_t    *    hofld     = NIL;
  warc_u32_t        rsize     = 0  ;
  warc_u32_t        sentsize  = 0  ;
  warc_i32_t        filled    = 0  ;
  warc_u32_t        rectype = 0;
 
 
  w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, cmode, prefix);

  unless (w)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout,  "<font color=red size = 4>File not found</font><br>");
      return (WARC_TRUE);
      }

   if (WFile_seek (w, offset))
      {
       fprintf (stdout, "content-type: text/html\n\n");
       fprintf (stdout, "<html>\n<body>\n");
       fprintf (stdout, "<font color = red size = 4 > Can not reach the reclaimed offset </font><br>");
       destroy (w);
       return (WARC_TRUE);
      }

   if (cmode == WARC_FILE_COMPRESSED_GZIP)
               fprintf (stdout, "content-type: application/x-gzip, \n\n");
          
   else
               fprintf (stdout, "content-type: text/random, \n\n");

   buffer = wmalloc (MAX_BUFFER_SIZE);
    
   while (WFile_hasMoreRecords (w) )
      {
       unless ( (record = WFile_nextRecord (w) ) )
         {
          fprintf (stdout, "content-type: text/html\n\n");
          fprintf (stdout, "<html>\n<body>\n");
          fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          wfree (buffer);
          destroy (w);
          return (WARC_TRUE);
         }

       rsize = WRecord_getCompressedSize (record);

       unless (w_strcmp (fdfilter, uS ("uri")))
              hofld =   WRecord_getTargetUri  (record);
       else
         {
         unless (w_strcmp(fdfilter, uS ("contenttype")))
            hofld =   WRecord_getContentType (record);
         else
            {
             rectype =   WRecord_getRecordType (record);
             hofld = uS ("");

             switch (rectype)
                {
                case 0:
                     hofld = uS ("unknownrecord");
                     break;

                case 1:
                     hofld = uS ("warcinfo");
                     break;

                case 2:
                     hofld = uS ("response");
                     break;

                case 3:
                     hofld = uS ("request");
                     break;

                case 4:
                     hofld = uS ("metadata");
                     break;

                case 5:
                     hofld = uS ("revisit");
                     break;
 
                case 6:
                     hofld = uS ("conversion");
                     break;
 
                case 7:
                     hofld = uS ("contunuation");
                     break;

                case 8:
                     hofld = uS ("resource");
                     break;

                default: break;

             }
          }
       }


       if ( (hofld) && (w_strcasestr (hofld, filter)))
          {
           WFile_seek (w, offset);
           offset += rsize;          
 
          unless (buffer)
                 {
                 fprintf (stdout, "content-type: text/html\n\n");
                 fprintf (stdout, "<html>\n<body>\n");
                 fprintf (stdout,  "<font color = red size = 4 >Failed in allocation of memory for respponse sending </font><br>");
                 destroy (w);
                 destroy (record);
                 wfree (buffer);
                 return (WARC_TRUE);
                 }
         
       

          while (rsize)
                {
                if (rsize > MAX_BUFFER_SIZE)
                   {
                   sentsize = MAX_BUFFER_SIZE;
                   rsize = rsize - MAX_BUFFER_SIZE;
                   }
                else
                  {
                  sentsize = rsize;
                  rsize = 0;
                  }
                WFile_fillBuffer  (w, (char *) buffer, sentsize, & filled);
                fwrite (buffer, filled, 1, stdout);
                }
          }
       else
          offset += rsize;

    destroy (record);
    }

  wfree (buffer);
  destroy (w);
  return (WARC_FALSE);
}



/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param cmode: the compression mode
 * @param offset: the offset from where the data will be sent

 * 
 * Sends a distant listing of the records in the warc file
 */

WPRIVATE warc_bool_t WSend_distantDumpResponse (const warc_u8_t * fname, const warc_u8_t * prefix, wfile_comp_t cmode,
                                                                                     warc_u32_t offset)
{
  void      *     w      = NIL;
  void      *     record = NIL;


  fprintf (stdout, "content-type: text/html\n\n");
  fprintf (stdout, "<html>\n<body>\n");
  fprintf (stdout, "<center> <H2> <FONT COLOR = red>  extract  warc records from  warc file %s </FONT> </H2> </center><br>", fname); 
 

   w = bless (WFile, fname, WARC_MAX_SIZE,
                WARC_FILE_READER, cmode, prefix);

  
   unless (w)
      {
      fprintf (stdout, "<FONT SIZE = 4 COLOR = red>  File  not found  </FONT><br>");
      fprintf (stdout, "</body>\n</html>\n");
      return 1;     
      return (WARC_TRUE);
      }
   
   if (WFile_seek (w, offset))
      {
       destroy (w);
      
       fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad offset</FONT><br>");
       fprintf (stdout, "</body>\n</html>\n");
       return (WARC_TRUE);
      }
     
    
   while (WFile_hasMoreRecords (w) )
      {
       const void * al = NIL;
       const warc_u8_t * string = NIL;
       warc_u32_t  tlen   = 0  ;
       warc_bool_t m      = WARC_FALSE;

       unless ( (record = WFile_nextRecord (w) ) )
         {
          
          fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          fprintf (stdout, "</body>\n</html>\n");
          destroy (w);
          return (WARC_TRUE);
         }



      /*print WHDLine object for this WRecord */
      fprintf (stdout, "<I> <B> <FONT SIZE = 5  COLOR = blue> Warc record that begin at offset  %10llu  fields : </FONT> </B> </I>\n\n",
                       (unsigned long long) WRecord_getOffset (record) );

      fprintf (stdout, "<p> <FONT SIZE = 4 COLOR = purple >  Header line field </FONT>  <p>\n");
      fprintf (stdout, "<BLOCKQUOTE> <TABLE BORDER = 2   BGCOLOR = gray>");
      fprintf (stdout, " <TR>  <TD> <B>  Warc ID  </B> </TD> <TD> %-20s </TD> </TR> \n",        WRecord_getWarcId      (record) );
      fprintf (stdout, "<TR> <TD> <B>  Content-Length  </B> </TD> <TD> %-20d </TD> </TR>\n",        WRecord_getContentLength  (record) );
      fprintf (stdout, "<TR> <TD> <B>  WARC-Type  </B> </TD> <TD> %-20d </TD> </TR>\n",       WRecord_getRecordType  (record) );
      fprintf (stdout, "<TR> <TD> <B>  WARC-Record-ID   </B>   </TD> <TD> %-20s </TD> </TR> \n",     WRecord_getRecordId    (record) );
      fprintf (stdout, "<TR> <TD> <B>  WARC-Date  </B> </TD> <TD> %-20s </TD> </TR>\n",     WRecord_getDate (record));
      fprintf (stdout, "</TABLE> </BLOCKQUOTE>");

      fprintf (stdout, "<font color = darkgreen size 3= > Other fields </font><br>");

      fprintf (stdout, "<BLOCKQUOTE> <TABLE BORDER = 2   BGCOLOR = gray>");

      string = WRecord_getContentType (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> Content-Type </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getConcurrentTo (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Concurrent-To </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getBlockDigest (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Block-Digest </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getPayloadDigest (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Payload-Digest </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getIpAddress (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-IP-Address </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getRefersTo (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Refers-To </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getTargetUri (record);
      if (string)
        {
         fprintf (stdout, "<TR> <TD> <B>  WARC-target-URI  </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getTruncated (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Truncated </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getWarcInfoId (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Warcinfo-ID </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getFilename (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Filename </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getProfile (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Profile </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getPayloadType (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Identified-Payload-Type </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getSegmentOriginId (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Segment-Origin-ID </B> </TD> <TD> %-20s </TD> </TR>\n", string);

         fprintf (stdout, " <TR> <TD> <B> WARC-Segment-Number </B> </TD> <TD> %-20d </TD> </TR>\n",
                     WRecord_getSegmentNumber (record));

         m = WARC_TRUE;
        }

      tlen = WRecord_getSegTotalLength (record);
      if (tlen)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Segment-Total-Length </B> </TD> <TD> %-20d </TD> </TR>\n", tlen);

         m = WARC_TRUE;
        }

      unless (m)
         fprintf (stdout, " <TR><TD> --No One-- </TD> </TR>\n");

         fprintf (stdout,  "</TABLE> </BLOCKQUOTE>");
      

          
      /* dump ANVLs */

      if ((al = WRecord_getAnvl (record) ))
         {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
            fprintf (stdout, "<font color=darkred size = 4> <b> More informations </b> </font>");
            fprintf (stdout, "<blockquote> <table BORDER = 2 bgcolor=lightgray>");
       
            while ( i < j )
               {
                 const void  * a = WList_get (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                /* use your own encoding to print it otherwise. */
                fprintf (stdout, "<tr><td><font color=darkgreen size = 3> %s </font></td><td>%s</td></tr>",
                                             (const char *) WAnvl_getKey (a), (const char *) WAnvl_getValue (a) );

                ++ i;
               }
            fprintf (stdout, "</table></blockquote><br><br>");
           }
        }

     destroy (record);
    }

  return (WARC_FALSE);
}



int main()
{

    wfile_comp_t     cmode;
    const warc_u8_t   *   reqnature;
    const warc_u8_t   *   cpmod ;
    const warc_u8_t   *   fname ;
    const warc_u8_t   *   ofrec;
    void         *   objfile;
    void         *   objmode;
    void         *   objrec;
    void         *   objnature;
    warc_u32_t pos             = w_strlen (uS(WARC_VERSION)) + 1 + w_strlen (uS (getenv ("SCRIPT_NAME")));
    warc_i32_t       offset    = -1;
    warc_u8_t         *   prefix    = uS (getenv ("DOCUMENT_ROOT"));
    const    char  * r = getenv ("REQUEST_URI");
   
    objnature = recoverUriField (r, & pos);
    reqnature = WString_getText (objnature);
    
    if ( w_strcmp (reqnature, uS ("file")))
          if ( w_strcmp (reqnature, uS ("record")))
               if (  w_strcmp (reqnature, uS ("list")))
                  
                   if (  w_strcmp (reqnature, uS ("filter")))
                      {
                      destroy (objnature);
                      fprintf (stdout, "content-type: text/html\n\n");
                      fprintf (stdout, "<html>\n<body>\n");
                      fprintf (stdout, "<FONT SIZE = 4 COLOR = blue> Nature of request not recognized </FONT>\n<br>");
                      fprintf (stdout, "</body>\n</html>\n");
                      return 1;
                      }
    objmode = recoverUriField (r, & pos);
    cpmod = WString_getText (objmode);
   
    if ( w_strcmp (cpmod, uS ("uncompressed")))
      {
       if (  w_strcmp (cpmod, uS ("gzip")))
          {
             destroy (objnature);
             destroy (objmode);
             fprintf (stdout, "content-type: text/html\n\n");
             fprintf (stdout, "<html>\n<body>\n");
             fprintf (stdout, "<FONT SIZE = 4 COLOR = blue> mode of compression not recognized </FONT>\n<br>");
             fprintf (stdout, "</body>\n</html>\n");
             return 1;
          }
       else
         cmode = WARC_FILE_COMPRESSED_GZIP;
      }
        
    else
        cmode = WARC_FILE_UNCOMPRESSED;
         
   objrec = recoverUriField (r, & pos);
   ofrec  = WString_getText(objrec);
    
   if ( w_atou(ofrec, w_strlen (ofrec), (warc_u32_t *) & offset))
      {
        destroy (objmode);
        destroy (objrec);
        destroy (objnature);
        fprintf (stdout, "content-type: text/html\n\n");
        fprintf (stdout, "<html>\n<body>\n");
        fprintf (stdout, "<FONT SIZE = 4 COLOR = red> offset not digit </font>\n<br>");
        fprintf (stdout, "</body>\n</html>\n");
        return 1;
      }

   unless (w_strcmp (reqnature, uS ("filter")))
       {
        void         *   objfdfilter;
        void         *   objfilter;
        const warc_u8_t   *   fdfilter;
        const warc_u8_t   *   filter;    
 
 
        
        objfdfilter =  recoverUriField (r, & pos);
        fdfilter = WString_getText (objfdfilter);

        if (w_strcmp (fdfilter, uS ("uri")))
              if (w_strcmp (fdfilter, uS ("contenttype")))
                    if (w_strcmp (fdfilter, uS ("recordtype")))
                        {
                        fprintf (stdout, "content-type: text/html\n\n");
                        fprintf (stdout, "<html>\n<body>\n");
                        destroy (objmode);
                        destroy (objrec);
                        destroy (objnature);
                        destroy (objfdfilter);
                        fprintf (stdout, "<FONT SIZE = 4 COLOR = red> can not apply this filter: field to filter must be content type or uri or record type </font>\n<br>");
                        fprintf (stdout, "</body>\n</html>\n");
                        return 1;
                        }
             
        objfilter =  recoverUriField (r, & pos);
        filter = WString_getText (objfilter);
       
        objfile = recoverFName (r, & pos);
        fname =   WString_getText(objfile);

     
      if (WSend_filtredResponse (fname, prefix, cmode, offset, fdfilter, filter))
         {
         fprintf (stdout, "<font color=red size=4>Filtred File sending aborted</font><br>");
         fprintf (stdout, "</body>\n</html>\n");
         }      

      
     

      destroy (objfile);
      destroy (objmode);
      destroy (objrec);
      destroy (objnature);
      destroy (objfilter);
      destroy (objfdfilter);
      return 0;

      }  

 
  objfile = recoverFName (r , & pos);
  fname =   WString_getText(objfile);
 
  unless ( w_strcmp (reqnature, uS ("list")))
     {
      /*ap_set_content_type (r, "text/html");*/
 
      
   
      if (WSend_distantDumpResponse (fname, prefix, cmode, offset))

         {
         fprintf (stdout, "<font color=red size=4> Records listing aborted</font><br>");
         fprintf (stdout, "</body>\n</html>\n");
         }

    destroy (objrec);
    destroy (objmode);
    destroy (objfile);
    destroy (objnature);
    return 1;
    
   } 

 /* we want here te recover a whole warc file */
  unless (w_strcmp (reqnature, uS ("file")))
      {
         
     
      
      if (WSend_fullResponse (fname, cmode, offset))
         {
          
          fprintf (stdout, "<font color=red size=4> File sending aborted</font><br>");
          fprintf (stdout, "</body>\n</html>\n");
         }

    destroy (objrec);
    destroy (objmode);
    destroy (objfile);
    destroy (objnature);
    return 1;
    
   } 


  /* here, we wante only to send one record */
  unless (w_strcmp (reqnature, uS ("record")))
    {
     

    if (WSend_record (fname, prefix, cmode, offset))
         {

          fprintf (stdout, "<font color=red size=4> Record sending aborted</font><br>");
          fprintf (stdout, "</body>\n</html>\n");
          return 0;
         }

    destroy (objfile);
    destroy (objmode);
    destroy (objrec);
    destroy (objnature);

  return 0;
  }


  

 destroy (objrec);
 destroy (objmode);
 destroy (objfile);
 destroy (objnature); 
 return 0;
}
