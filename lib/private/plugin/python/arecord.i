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

/* arecord.i.i */
 %{
   #include <wport.h>
   #include <arecord.x>
   #include <arecord.h>
   
   
 %}
    
    extern const void * ARecord;

    /* getters */

/*    extern const char *  ARecord_getUrl          (const void * const);
    extern const char *  ARecord_getCreationDate (const void * const);
    extern const char *  ARecord_getMimeType     (const void * const);
    extern const char *  ARecord_getIpAddress    (const void * const);
    extern warc_bool_t        ARecord_getContent      (const void* const );
*/
    extern warc_bool_t        ARecord_transferContent (void *, void *, void *);
    extern warc_bool_t        ARecord_setCallback     (void *,
          warc_bool_t (*)
          (void *,  const char *,
           warc_u32_t) );