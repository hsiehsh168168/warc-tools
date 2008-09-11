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


#include <warc.h>      /* WARC headers */
#include <wmem.h>      /* wcalloc, wfree */

#include <warcmisc.h>


#define makeS(s) (s), w_strlen(s)
#define uS(s) ((warc_u8_t *) (s))

struct WPlugin {
  void * w;
  void * ip;
  void * url;
  void * mime;
  void * timestamp;
};


WPUBLIC void * blessWARC (const char * filename, unsigned int max,
                           const char * tmpdir)
{
  struct WPlugin * wst = NIL;

  wst = wmalloc(sizeof(struct WPlugin));
  unless (wst)
    return (NIL);

  wst -> w = bless (WFile, filename, max, WARC_FILE_WRITER, 
                    WARC_FILE_COMPRESSED_GZIP, tmpdir);

  unless (wst -> w)
    {
      destroyWARC (wst);
      return (NULL);
    }

  wst -> url = bless (WString, makeS(uS("")));
  unless (wst -> url)
    {
      destroyWARC (wst);
      return (NULL);
    }

  wst -> mime = bless (WString, makeS(uS("")));
  unless (wst -> mime)
   {
      destroyWARC (wst);
      return (NULL);
    }

  wst -> ip = bless (WString, makeS(uS("")));
  unless (wst -> ip)
   {
      destroyWARC (wst);
      return (NULL);
    }

  wst -> timestamp = bless (WString, makeS(uS("")));
  unless (wst -> timestamp)
   {
      destroyWARC (wst);
      return (NULL);
    }

  return (wst);
}


WPUBLIC void setTimeStamp (const char * timestamp, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;
  
  WString_setText(wst -> timestamp, makeS(uS(timestamp)));
}


WPUBLIC void setIP (const char * ip, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;
  
  WString_setText(wst -> ip, makeS(uS(ip)));
}


WPUBLIC void setMIME (const char * mime, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;
  
  WString_setText(wst -> mime, makeS(uS(mime)));
}


WPUBLIC void setURL (const char * domain, const char * file, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;
  
  WString_setText(wst -> url, makeS(uS(domain)));
  WString_append (wst -> url, makeS(uS(file)));
}


WPUBLIC void  destroyWARC (void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;

  unless (wst)
    return;

  if (wst -> ip)
    destroy (wst -> ip);

  if (wst -> url)
    destroy (wst -> url);

  if (wst -> mime)
    destroy (wst -> mime);

  if (wst -> timestamp)
    destroy (wst -> timestamp);

  if (wst -> w)
    destroy (wst -> w);

  wfree (wst), wst = NULL;
}


WPUBLIC void writeWRecord (const char * timestamp,
                           const char * mime, const char * ip,
                           const char * file, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;

  void * r = NULL;
  void * u = NULL;

  r = bless (WRecord);

  unless (r)
    return ;

  u = bless (WUUID);

  unless (u)
    {
     destroy (r);
     return ;
    }
  
  /* use fallback values for MIME, IP ... */
  unless (WString_getLength(wst -> url))
      return;

  unless (WString_getLength(wst -> mime))
      WString_setText(wst -> mime, makeS(uS(mime)));

  unless (WString_getLength(wst -> ip))
      WString_setText(wst -> ip, makeS(uS(ip)));

  unless (WString_getLength(wst -> timestamp))
      WString_setText(wst -> timestamp, makeS(uS(timestamp)));

  WRecord_setRecordType  (r, WARC_RESOURCE_RECORD);
  WRecord_setTargetUri   (r, makeS (uS(WString_getText(wst -> url))));
  WRecord_setDate        (r, makeS (uS(WString_getText(wst -> timestamp))));
  WRecord_setContentType (r, makeS (uS(WString_getText(wst -> mime))));
 /*  WRecord_setIpAddress   (r, makeS (uS(WString_getText(wst -> ip)))); */

  WUUID_hash (u, makeS(uS(WString_getText(wst -> url))));
  WUUID_hash (u, makeS(uS(WString_getText(wst -> timestamp))));

  WRecord_setRecordId(r, makeS (uS(WUUID_text (u))));

  destroy (u);

  if(WRecord_setContentFromFileName (r, file))
    {
      destroy (r);
      return;
    }

  WFile_storeRecord (wst -> w, r);

  /* printf("+ filesave2: %s - %s\n\n", WString_getText(wst -> url), file); */

  destroy (r);
}
