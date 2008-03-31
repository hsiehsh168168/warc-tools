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


#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include <http_log.h>
#include <apr_strings.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <warc.h>



#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include <warc.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <wmem.h>     /* wmalloc, wfree */

#define PTR_OR(a, b) (((a) != NULL) ? (a) : (b))

#define uS(s) (warc_u8_t *)(s)
#define makeS(s) uS(s),w_strlen(uS(s))
#define makeWString(s) bless (WString, uS(s), w_strlen((uS(s))))

#ifndef WARC_MAX_SIZE
#define WARC_MAX_SIZE 629145600
#define MAX_BUFFER_SIZE 512
#endif

module AP_MODULE_DECLARE_DATA warc_module;

typedef struct {

  const char * warc_tmp_dir;
               } warc_dir_config;

char dname[100]; /* buffer who contains a name of a warc directorie */



/**
 * @param str: sample string
 
 * 
 * @return True if the string is digital value, False otherwise
 *
 * cheek the offset record parameter of the uri
 **/

WPRIVATE warc_bool_t strIsDigit (const char * str)
{
     warc_u32_t strsize = strlen (str);
     warc_u32_t i = 0;

     for (i =0; i< strsize; i++)
       {
         if (! isdigit (str [i]))
            return WARC_FALSE;
       }

      return WARC_TRUE;
}

/**
 * @param str: sample string
 * @param position: intger 
 * 
 * @return The actual Uri Field
 *
 * return the field of uri at the actual position
 **/

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
               return (OK);
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


  while ( uri [i] == '/')
        i ++ ;

  for (k = i; k < strlen (uri ); k++)
      {
      achar [0] = uri [k];
      if (WString_append (item, achar, 1))
             {
               destroy (item);
               return (OK);
             }
      
      }
  * (position) = i ;

  return item;
}

/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent
 * @param r: a request object
 *
 * @return False if succeed, True otherwise
 *
 * Sends a full file response to the client
 */

WPRIVATE warc_bool_t WSend_fullResponse (const warc_u8_t * fname, const warc_u8_t * prefix, wfile_comp_t cmode, warc_u32_t offset, request_rec * r)
{
   FILE          *   f        = NIL;
   warc_u8_t     *   buffer   = NIL;
   warc_u32_t        fsize    = 0  ;
   warc_u32_t        filled   = 0  ;
   warc_u32_t        sentsize = 0  ;


  buffer = wmalloc (MAX_BUFFER_SIZE);
  
  unless (buffer)
    {
    r->content_type = "text/html";
    ap_rprintf (r, "<font color=red size = 4>Failed in allocation of memory for respponse sending </font><br>");
    return (WARC_TRUE);
    }

  f = w_fopen_rb (fname);

  unless (f)
    {
    r->content_type = "text/html";
    ap_rprintf (r, "<font color=red size = 4>File not found</font><br>");
    wfree (buffer);
    return (WARC_TRUE);
    }

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
    ap_rwrite ((const void *) buffer, filled, r);
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
 * @param r: a request object
 *
 * @return False if succeed, True otherwise
 *
 * Sends a Warc Record Response
 */

WPRIVATE warc_bool_t WSend_record (const warc_u8_t * fname, const warc_u8_t * prefix, wfile_comp_t cmode, warc_u32_t offset, request_rec * r)
{
    void        *     w         = NIL;
    void        *     record    = NIL;
    warc_u8_t   *     buffer    = NIL;
    warc_u32_t        rsize     = 0  ;
    warc_u32_t        sentsize  = 0  ;
    warc_u32_t        filled    = 0  ;


    w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, cmode, prefix);

    unless (w)
      {
      r->content_type = "text/html";
      ap_rprintf (r, "<font color=red size = 4>File not found</font><br>");
      return (WARC_TRUE);
      }

    if (WFile_seek (w, offset))
       {
        r->content_type = "text/html";
        ap_rprintf (r, "<font color = red size = 4 > Can not reach the reclaimed offset </font><br>");
        destroy (w);
        return (WARC_TRUE);
       }

    record = WFile_nextRecord (w);

    unless (record)
       {
        r->content_type = "text/html";
        ap_rprintf (r, "<font color = red size = 4 > No records at this offset</font><br>");
        destroy (w);
        return (WARC_TRUE);
       } 

    WFile_seek (w, offset);

    buffer = wmalloc (MAX_BUFFER_SIZE);
 
    unless (buffer)
     {
      r->content_type = "text/html";
      ap_rprintf (r, "<font color = red size = 4 >Failed in allocation of memory for respponse sending </font><br>");
      destroy (w);
      destroy (record);
      return (WARC_TRUE);
     }

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
      WFile_fillBuffer  (w, buffer, sentsize, & filled);
      ap_rwrite ((const void *) buffer, filled, r);
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
 * @param r: a request object
 *
 * @return False if succeed, True otherwise
 *
 * Sends a filtred Warc file Response
 */


WPRIVATE WSend_filtredResponse (const warc_u8_t * fname, const warc_u8_t * prefix, wfile_comp_t cmode, warc_u32_t offset,
                                const warc_u8_t * fdfilter,const warc_u8_t * filter, request_rec * r)
{
  void         *    w         = NIL;
  void         *    record    = NIL;
  warc_u8_t    *    buffer    = NIL;
  const char   *    hofld     = NIL;
  warc_u32_t        rsize     = 0  ;
  warc_u32_t        sentsize  = 0  ;
  warc_u32_t        filled    = 0  ;
  warc_u32_t        rectype = 0;
 
 
  w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, cmode, prefix);

  unless (w)
      {
      r->content_type = "text/html";
      ap_rprintf (r, "<font color=red size = 4>File not found</font><br>");
      return (WARC_TRUE);
      }

   if (WFile_seek (w, offset))
      {
       r->content_type = "text/html";
       ap_rprintf (r, "<font color = red size = 4 > Can not reach the reclaimed offset </font><br>");
       destroy (w);
       return (WARC_TRUE);
      }

   buffer = wmalloc (MAX_BUFFER_SIZE);
    
   while (WFile_hasMoreRecords (w) )
      {
       unless ( (record = WFile_nextRecord (w) ) )
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          wfree (buffer);
          destroy (w);
          return (WARC_TRUE);
         }

       rsize = WRecord_getCompressedSize (record);

       unless (w_strcmp (fdfilter, "uri"))
              hofld =   WRecord_getSubjectUri  (record);
       else
         {
         unless (w_strcmp(fdfilter, "contenttype"))
            hofld =   WRecord_getContentType (record);
         else
            {
             rectype =   WRecord_getRecordType (record);
             hofld = "";

             switch (rectype)
                {
                case 0:
                     hofld = "unknownrecord";
                     break;

                case 1:
                     hofld = "warcinfo";
                     break;

                case 2:
                     hofld = "response";
                     break;

                case 3:
                     hofld = "request";
                     break;

                case 4:
                     hofld = "metadata";
                     break;

                case 5:
                     hofld = "revisit";
                     break;
 
                case 6:
                     hofld = "conversion";
                     break;
 
                case 7:
                     hofld = "contunuation";
                     break;

                case 8:
                     hofld = "resource";
                     break;

                default: break;

             }
          }
       }

       if (w_strcasestr (hofld, filter))
          {
           WFile_seek (w, offset);
           offset += rsize;          
 
          unless (buffer)
                 {
                 r->content_type = "text/html";
                 ap_rprintf (r, "<font color = red size = 4 >Failed in allocation of memory for respponse sending </font><br>");
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
                WFile_fillBuffer  (w, buffer, sentsize, & filled);
                ap_rwrite ((const void *) buffer, filled, r);
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
 * @param r: a request object
 * 
 * Sends a distant listing of the records in the warc file
 */

WPRIVATE warc_bool_t WSend_distantDumpResponse (const warc_u8_t * fname, const warc_u8_t * prefix, wfile_comp_t cmode,
                                                                                     warc_u32_t offset, request_rec * r)
{
  void      *     w      = NIL;
  void      *     record = NIL;

   ap_rprintf (r, "<center> <H2> <FONT COLOR = red>  extract  warc records from  warc file %s </FONT> </H2> </center><br>", fname); 

   w = bless (WFile, fname, WARC_MAX_SIZE,
                WARC_FILE_READER, cmode, prefix);

  
   unless (w)
      {
       r->content_type = "text/html";
      ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>  File  not found  </FONT><br>");
      return (WARC_TRUE);
      }
   
   if (WFile_seek (w, offset))
      {
       destroy (w);
       r->content_type = "text/html";
       ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Bad offset</FONT><br>");
       return (WARC_TRUE);
      }
     
    
   while (WFile_hasMoreRecords (w) )
      {
       unless ( (record = WFile_nextRecord (w) ) )
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          destroy (w);
          return (WARC_TRUE);
         }

      /*print WHDLine object for this WRecord */
      ap_rprintf (r, "<I> <B> <FONT SIZE = 5  COLOR = blue> Warc record that begin at offset  %d  fields : </FONT> </B> </I>\n\n",
                       WRecord_getOffset (record) );

      ap_rprintf (r, "<p> <FONT SIZE = 4 COLOR = purple >  Header line field </FONT>  <p>\n");
      ap_rprintf (r, "<BLOCKQUOTE> <TABLE BORDER = 2   BGCOLOR = gray>");
      ap_rprintf (r, " <TR>  <TD> <B>  WarcId  </B> </TD> <TD> %-20s </TD> </TR> \n",        WRecord_getWarcId      (record) );
      ap_rprintf (r,"<TR> <TD> <B>  DataLength  </B> </TD> <TD> %-20d </TD> </TR>\n",        WRecord_getDataLength  (record) );
      ap_rprintf (r, "<TR> <TD> <B>  RecordType  </B> </TD> <TD> %-20d </TD> </TR>\n",       WRecord_getRecordType  (record) );
      ap_rprintf (r, "<TR> <TD> <B>  SubjectUri  </B> </TD> <TD> %-20s </TD> </TR>\n",       WRecord_getSubjectUri  (record) );
      ap_rprintf (r, "<TR> <TD> <B>  CreationDate  </B> </TD> <TD> %-20s </TD> </TR>\n",     WRecord_getCreationDate (record));
      ap_rprintf (r, " <TR> <TD> <B> ContentType </B> </TD> <TD> %-20s </TD> </TR>\n",       WRecord_getContentType (record) );
      ap_rprintf (r, "<TR> <TD> <B>  RecordId   </B>   </TD> <TD> %-20s </TD> </TR> \n",     WRecord_getRecordId    (record) );
      
      ap_rprintf (r, "</TABLE> </BLOCKQUOTE>");
          
      const void * al = NIL;
      /* dump ANVLs */

      if ((al = WRecord_getAnvl (record) ))
         {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
            ap_rprintf (r, "<font color=darkred size = 4> <b> More informations </b> </font>");
            ap_rprintf (r, "<blockquote> <table BORDER = 2 bgcolor=lightgray>");
       
            while ( i < j )
               {
                 const void  * a = WList_get (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                /* use your own encoding to print it otherwise. */
                ap_rprintf (r, "<tr><td><font color=darkgreen size = 3> %s </font></td><td>%s</td></tr>",
                                             (const char *) WAnvl_getKey (a), (const char *) WAnvl_getValue (a) );

                ++ i;
               }
            ap_rprintf (r, "</table></blockquote><br><br>");
           }
        }

     destroy (record);
    }

  return (WARC_FALSE);
}



static warc_i32_t warc_handler(request_rec * r)
{ 
    wfile_comp_t     cmode;
    const char   *   reqnature;
    const char   *   cpmod ;
    const char   *   fname ;
    const char   *   ofrec;
    void         *   objfile;
    void         *   objmode;
    void         *   objrec;
    void         *   objnature;
    warc_u32_t pos             = 9;
    warc_i32_t       offset    = -1;



   
   
    if (w_strcmp(r->handler, "warc"))
        return DECLINED;


    r->content_type = "text/html";
    objnature = recoverUriField (r-> uri, & pos);
    reqnature = WString_getText (objnature);

    if ( w_strcmp (reqnature, "file" ))
          if ( w_strcmp (reqnature, "record"))
               if (  w_strcmp (reqnature, "list"))
                  
                   if (  w_strcmp (reqnature, "filter"))
                      {
                      destroy (objnature);
                      ap_rprintf (r, "<FONT SIZE = 4 COLOR = blue> Nature of request not recognized </FONT>\n<br>");
                      return OK;
                      }

    
    
    objmode = recoverUriField (r-> uri, & pos);
    cpmod = WString_getText (objmode);

    if ( w_strcmp (cpmod, "uncompressed"))
      {
       if (  w_strcmp (cpmod, "gzip"))
          {
             destroy (objnature);
             destroy (objmode);
             ap_rprintf (r, "<FONT SIZE = 4 COLOR = blue> mode of compression not recognized </FONT>\n<br>");
             return OK;
          }
       else
         cmode = WARC_FILE_COMPRESSED_GZIP;
      }
        
    else
       cmode = WARC_FILE_UNCOMPRESSED;

   objrec = recoverUriField (r -> uri, & pos);
   ofrec  = WString_getText(objrec);
   
   if ( strIsDigit (ofrec))
       offset = atoi(ofrec);
    else
      {
        destroy (objmode);
        destroy (objrec);
        destroy (objnature);
        ap_rprintf (r, "<FONT SIZE = 4 COLOR = %s> offset not digit </font>\n<br>");
        return OK;
      }

  unless (w_strcmp (reqnature, "filter"))
      {
       void         *   objfdfilter;
      void         *   objfilter;
      const char   *   fdfilter;
      const char   *   filter;    
 
      unless ( w_strcmp (cpmod, "gzip"))
          ap_set_content_type (r, "application/x-gzip");
      else
          ap_set_content_type (r, "text/random");
 
        
      objfdfilter =  recoverUriField (r -> uri, & pos);
      fdfilter = WString_getText (objfdfilter);

      if (w_strcmp (fdfilter, "uri"))
             if (w_strcmp (fdfilter, "contenttype"))
                    if (w_strcmp (fdfilter, "recordtype"))
                        {
                        ap_set_content_type (r, "text/html");
                        destroy (objmode);
                        destroy (objrec);
                        destroy (objnature);
                        destroy (objfdfilter);
                        ap_rprintf (r, "<FONT SIZE = 4 COLOR = red> can not apply this filter: field to filter must be content type or uri or record type </font>\n<br>");
                        return OK;
                        }
             
      objfilter =  recoverUriField (r -> uri, & pos);
      filter = WString_getText (objfilter);
       
      objfile = recoverFName (r -> uri, & pos);
      fname =   WString_getText(objfile);

     
      if (WSend_filtredResponse (fname, dname, cmode, offset, fdfilter, filter, r))
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<font color=red size=4>Filtred File sending aborted</font><br>");
         }      

      
     

      destroy (objfile);
      destroy (objmode);
      destroy (objrec);
      destroy (objnature);
      destroy (objfilter);
      destroy (objfdfilter);
      return OK;

      }
      
      
  objfile = recoverFName (r -> uri, & pos);
  fname =   WString_getText(objfile);

 /* we want here te recover a whole warc file */
  unless (w_strcmp (reqnature, "file"))
      {
         
      unless ( w_strcmp (cpmod, "gzip"))
          ap_set_content_type (r, "application/x-gzip");
      else
          ap_set_content_type (r, "text/random");
      
      if (WSend_fullResponse (fname, dname, cmode, offset, r))
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<font color=red size=4> File sending aborted</font><br>");
         }


  destroy (objfile);
  destroy (objmode);
  destroy (objrec);
  destroy (objnature);

  return OK;
  }


/* here, we wante only to send one record */
  unless (w_strcmp (reqnature, "record"))
    {

     unless ( w_strcmp (cpmod, "gzip"))
        ap_set_content_type (r, "application/x-gzip");
    else
        ap_set_content_type (r, "text/random");

    if (WSend_record (fname, dname, cmode, offset, r))
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<font color=red size=4> Record sending aborted</font><br>");
         }

    destroy (objfile);
    destroy (objmode);
    destroy (objrec);
    destroy (objnature);

  return OK;
  }

/* here, we want to list information about the warc file */
  unless ( w_strcmp (reqnature, "list"))
     {
      ap_set_content_type (r, "text/html");
   
      if (WSend_distantDumpResponse (fname, dname, cmode, offset, r))
         ap_rprintf (r, "<font color=red size=4> Records listing aborted</font><br>");

    destroy (objrec);
    destroy (objmode);
    destroy (objfile);
    destroy (objnature);
    return OK;
    
   } 

 destroy (objrec);
 destroy (objmode);
 destroy (objfile);
 destroy (objnature);
 return OK;
}

static void *  create_warc_dir_config(apr_pool_t *p, char *path) 

{
  warc_dir_config *conf;

  conf = (warc_dir_config *)apr_pcalloc(p, sizeof(warc_dir_config));
  conf->warc_tmp_dir = NULL;

  return (void *)conf;
}

static void *   merge_warc_dir_configs(apr_pool_t *p, void *bp, void *ap) {
     warc_dir_config *base = (warc_dir_config *)bp;
     warc_dir_config *add = (warc_dir_config *)ap;
     warc_dir_config *conf;

     conf = (warc_dir_config *)apr_pcalloc(p, sizeof(warc_dir_config));
 
     conf->warc_tmp_dir = PTR_OR(add->warc_tmp_dir, base->warc_tmp_dir);

 
     return conf;
     }

static const char *set_warc_tmp_dir(cmd_parms *cmd,
                                               void *tmp, const char *arg)
{
  warc_dir_config *conf = (warc_dir_config *) tmp;
  conf->warc_tmp_dir = apr_pstrdup(cmd->pool, arg);
  strcpy (dname, conf->warc_tmp_dir);
  return NULL;
}



static const  command_rec warc_cmds[] = {

     AP_INIT_TAKE1("WarcTmpdir", set_warc_tmp_dir,
     NULL, OR_OPTIONS, "tmpdir used to create temporary file in the compressed case"),
     {NULL}
     };


static void warc_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(warc_handler, NULL, NULL, APR_HOOK_MIDDLE);
}


/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA warc_module = {

    STANDARD20_MODULE_STUFF, 
    create_warc_dir_config,     /* create per-dir    config structures */
    merge_warc_dir_configs,     /* merge  per-dir    config structures */
    NULL,                       /* create per-server config structures */
    NULL,                       /* merge  per-server config structures */
    warc_cmds,                  /* table of config file commands       */
    warc_register_hooks         /* register hooks                      */
};

      