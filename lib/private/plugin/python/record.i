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

/* record.i */
 %{
   #include <wport.h>
   #include <wrectype.h>
   #include <wrecord.h>
 %}
 
/*
    typedef enum
    {
      WARC_UNKNOWN_RECORD = 0,
      WARC_INFO_RECORD,
      WARC_RESPONSE_RECORD,
      WARC_REQUEST_RECORD,
      WARC_METADATA_RECORD,
      WARC_REVISIT_RECORD,
      WARC_CONVERSION_RECORD,
      WARC_CONTINUATION_RECORD,
      WARC_RESOURCE_RECORD
     } warc_rec_t;

    extern const void * WRecord;

    extern const char *  WRecord_getWarcId      (const void * const);
    extern warc_rec_t    WRecord_getRecordType  (const void * const);
    extern const char *  WRecord_getTargetUri  (const void * const);
    extern const char *  WRecord_getDate (const void * const);
    extern const char *  WRecord_getContentType (const void * const);
    extern const char *  WRecord_getRecordId    (const void * const);
    extern warc_bool_t   WRecord_getContent     (const void * const);
    extern const char *   WRecord_getAnvlValue   (const void * const,
          const char *);
    extern  unsigned int  WRecord_getContentLength        (const void * const );
    extern const   char * WRecord_getConcurrentTo  (const void * const );
    extern const   char * WRecord_getBlockDigest (const void * const   );
    extern const   char * WRecord_getPayloadDigest (const void * const );
    extern const   char * WRecord_getIpAddress      (const void * const );
    extern const   char * WRecord_getRefersTo        (const void * const );
    extern const   char * WRecord_getTruncated     (const void * const );
    extern const   char * WRecord_getWarcInfoId    (const void * const );
    extern const   char * WRecord_getProfile       (const void * const );
    extern const   char * WRecord_getPayloadType    (const void * const);
    extern const   char * WRecord_getSegmentOriginId (const void * const);
    extern  unsigned int  WRecord_getSegmentNumber        (const void * const );
    extern  unsigned int WRecord_getSegTotalLength        (const void * const );
    extern const char * WRecord_getFilename        (const void * const );


    extern const void * WRecord_getAnvl         (const void * const);


    extern warc_bool_t   WRecord_setRecordType   (void *, const warc_rec_t);
    extern warc_bool_t   WRecord_setTargetUri   (void *, const char *,
          const unsigned int);
    extern warc_bool_t   WRecord_setDate (void *, const char *,
          const unsigned int);
    extern warc_bool_t   WRecord_setContentType  (void *, const char *,
          const unsigned int );
    extern warc_bool_t   WRecord_setRecordId     (void *, const char *,
          const unsigned int );

    extern  warc_bool_t WRecord_setConcurrentTo (void *,  const char *,
          const unsigned int );

    extern warc_bool_t WRecord_setBlockDigest (void *, const char *,
          const unsigned int);

    extern warc_bool_t WRecord_setPayloadDigest (void *, const char *,
          const unsigned int);

    extern warc_bool_t  WRecord_setIpAddress (void *, const char *,
          const unsigned int );

    extern warc_bool_t  WRecord_setRefersTo (void *,  const char *,
          const unsigned int);

    extern warc_bool_t  WRecord_setTruncated (void *, const char *,
          const unsigned int );

    extern warc_bool_t   WRecord_setWarcInfoId (void * _self, const char *,
          const unsigned int );

    extern warc_bool_t   WRecord_setProfile (void *, const char *,
          const unsigned int );

    extern warc_bool_t  WRecord_setPayloadType (void *, const char *,
          const unsigned int );

    extern warc_bool_t   WRecord_setSegmentOriginId (void *, const char *,
          const unsigned int );

    extern warc_bool_t    WRecord_setSegmentNumber (void *,  const unsigned int);

    
    extern warc_bool_t    WRecord_setSegTotalLength (void *, const unsigned int);

    extern warc_bool_t    WRecord_setFilename (void *, const char *,
          const unsigned int );


    extern warc_bool_t   WRecord_addAnvl         (void *, const char *,
          const unsigned int, const char *,
          const unsigned int);

    extern warc_bool_t   WRecord_setContentFromFileName (void *, const char *);

    extern warc_bool_t    WRecord_getOffset           (const void * const );
    extern unsigned int    WRecord_getUncompressedSize (const void * const );
    extern unsigned int    WRecord_getCompressedSize   (const void * const);

    extern warc_bool_t WRecord_setDateFromArc   (void *  ,
                                                const char * ,
                                                const unsigned int );

*/