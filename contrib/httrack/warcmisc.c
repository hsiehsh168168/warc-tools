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
#include <warcmisc.h>


#define makeS(s) (s), w_strlen(s)
#define uS(s) ((warc_u8_t *) (s))


/* WIPUBLIC void * blessWString (char * const s, unsigned int len) */
/* { */
/*   return bless(WString, (warc_u8_t *) s, (warc_u32_t) len); */
/* } */


WIPUBLIC void * blessWFile (const char * filename, unsigned int max,
                            const char * tmpdir)
{
  return bless (WFile, filename, max, 
                WARC_FILE_COMPRESSED_GZIP, WARC_FILE_WRITER, 
                tmpdir);
}


WIPUBLIC void writeWRecord (const char * url,  const char * timestamp,
                            const char * mime, const char * ip,
                            const char * file, void * w)
{
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

  /* TU DEVRAIS TESTER LA AVLEUR DE RETOUR DES FONCTIONS LYES ICI */

  WRecord_setRecordType (r, WARC_RESOURCE_RECORD);
  WRecord_setTargetUri  (r, makeS (uS(url)));
  WRecord_setDate       (r, makeS (uS(timestamp)));
  WRecord_setContentType(r, makeS (uS(mime)));

  WUUID_hash (u, makeS(uS("onestring")));
  WUUID_hash (u, makeS(uS("towstring")));

  WRecord_setRecordId(r, makeS (uS(WUUID_text (u))));

  destroy (u);

  WRecord_setIpAddress(r, makeS(uS(ip)));

  WRecord_setContentFromFileName (r, file);

  WFile_storeRecord (w, r);
}
