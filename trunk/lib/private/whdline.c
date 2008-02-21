
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

/*
 * Portability header file
 */

#include <wport.h>


/*
 * WARC default headers 
 */

#include <wclass.h>   /* bless, destroy, cassert, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <whdline.h>  /* for class's prototypes */
#include <wstring.h>  /* for class's prototypes */
#include <wmem.h>     /* wmalloc, wfree */
#include "wmisc.h"    /* warc_bool_t ... */
#include <wrectype.h> /* revist, resource ... */
#include <wversion.h> /* WARC_VERSION */
#include <wcsafe.h>


#define SIGN 3


/**
 * WARC WString internal structure
 */


struct WHDLine 
{
  const void * class;
  
  /*@{*/
  void       * warc_id; /**< The WARC version */
  warc_u32_t   data_length; /**< The WARC record data length */
  warc_rec_t   record_type; /**< The WARC record type */
  void       * subject_uri; /**< The WARC record subject uri */
  void       * creation_date; /**< The WARC record date of creation */
  void       * content_type; /**< The WARC record content type */
  void       * record_id; /**< The WARC record id */
  /*@}*/
};


#define    WARC_ID         (self -> warc_id)
#define    DATA_LENGTH     (self -> data_length)
#define    RECORD_TYPE     (self -> record_type)
#define    SUBJECT_URI     (self -> subject_uri)
#define    CREATION_DATE   (self -> creation_date)
#define    CONTENT_TYPE    (self -> content_type)
#define    RECORD_ID       (self -> record_id)


/**
 * @param _self WHDLine object
 *
 * @return constant WARC id string
 *
 * Returns the WARC id of the WARC-record
 */

WIPUBLIC const char * WHDLine_getWarcId (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WString_getText (WARC_ID));
}


/**
 * @param _self WHDLine object instance
 *
 * @return the header line warc id field length
 * as a 4 bytes integer
 *
 * WHDLine warc id field length provider
 */

WIPUBLIC warc_u32_t WHDLine_getWarcIdLen (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WString_getLength (WARC_ID));
}


/**
 * @param _self WHDLine object
 * @param wid WARC id string
 * @param len WARC id string length
 *
 * @param[out]: false if succseeds, true otherwise
 *
 * Sets the WARC id field of the WARC-record to "wid"
 */

WIPUBLIC warc_bool_t WHDLine_setWarcId (void * const _self, const char * wid,
                                    const warc_u32_t len)
{
  struct WHDLine * const self = _self;
  
  /* preconditions */
  CASSERT (self);
  assert (wid);

  if (strcmp (wid, WARC_VERSION))
     return (WARC_TRUE);  
  /* reset the text */
   
  if (WString_setText (WARC_ID, wid, len))
    return (WARC_TRUE);
    
  return (WARC_FALSE);
}


/**
 * @param _self WHDLine object
 *
 * @return data length as a 32 bits unsigned integer
 *
 * Returns the data length of the WARC-record
 */

WIPUBLIC warc_u32_t WHDLine_getDataLength (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (DATA_LENGTH);
}

/**
 * @param _self WHDLine object
 * @param len data length in bytes
 *
 * @return False if it succeeds, True otherwise
 *
 * Sets the data length of the WARC-record
 */

WIPUBLIC warc_bool_t WHDLine_setDataLength (void * const _self, const warc_u32_t len)
{
	struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	unless (len)
          return (WARC_TRUE);

    DATA_LENGTH = len;
  return (WARC_FALSE);

}


/**
 * @param _self WHDLine object
 *
 * @return WARC-record type as an integer (see "whdline.h")
 *
 * Returns the WARC-record type
 */

WIPUBLIC warc_rec_t WHDLine_getRecordType (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (RECORD_TYPE);
}


/**
 * @param _self WHDLine object
 * @param t WARC-record type integer (see "whdline.h")
 *
 * @param[out]: False if succeeds, True otherwise
 *
 * Sets the WARC-record type
 */

WIPUBLIC warc_bool_t WHDLine_setRecordType (void * const _self,
                                            const warc_rec_t t)
{
	struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

    if (t > RESOURCE_RECORD)
       return (WARC_TRUE);
    
    RECORD_TYPE = t;
  return (WARC_FALSE);
}


/**
 * @param _self WHDLine object
 *
 * @return WARC-record subject URI string
 *
 * Returns a constant string which represent the 
 * subject URI of the WARC-record
 */

WIPUBLIC const char * WHDLine_getSubjectUri (const void * const _self)
{
  const struct WHDLine * const self = _self;
  
  /* preconditions */
  CASSERT (self);

  return (WString_getText (SUBJECT_URI));
}

/**
 * @param _self WHDLine object instance
 *
 * @return The subject uri field length
 * as a 4 bytes integer
 *
 * Warc header line subject Uri length string provider
 */

WIPUBLIC warc_u32_t WHDLine_getSubjectUriLen (const void * const _self)
{
  const struct WHDLine * const self = _self;
  
  /* preconditions */
  CASSERT (self);

  return (WString_getLength (SUBJECT_URI));
}


/**
 * @param _self WHDLine object
 * @param suri Subject URI string
 * @param len WARC Subject URI string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the subject URI of the WARC-record
 */

WIPUBLIC warc_bool_t WHDLine_setSubjectUri (void * const _self, const char * suri,
                                        const warc_u32_t len)
{
  struct WHDLine * const self = _self;
  /* preconditions */
  CASSERT (self);
  assert (suri);

/*   unless (w_strcasestr(suri,"://")) */
/*     return (WARC_TRUE);  */

  /* reset the text */
  if (WString_setText (SUBJECT_URI, suri, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param self WHDLine object
 *
 * @return creation date constant string
 *
 * Returns the WARC-record's creation date
 */

WIPUBLIC const char * WHDLine_getCreationDate (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WString_getText (CREATION_DATE));
}


/**
 * @param _self WHDLine object instance
 *
 * @return The creation date field string length
 * as a 4 bytes integer
 *
 * Warc Header line creation date field string length provider
 */

WIPUBLIC warc_u32_t WHDLine_getCreationDateLen (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WString_getLength (CREATION_DATE));
}



/**
 * @param _self WHDLine object
 * @param cd creation date string
 * @param len WARC Creation Date string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the creation date of the WARC-record
 */


WIPUBLIC warc_bool_t WHDLine_setCreationDate (void * const _self, 
                                              const char * cd,
                                              const warc_u32_t len)
{
  struct WHDLine * const self = _self;
  warc_u32_t i=0;
  /* preconditions */
  CASSERT (self);
  assert (cd);

  for(i=0;i<len;i++)
   {
     unless(isdigit(cd[i]))
      return (WARC_TRUE);
    }

  /* reset the text */
  if (WString_setText (CREATION_DATE, cd, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHDLine object
 *
 * @return content type constant string
 *
 * Returns the WARC-record's content type
 */

WIPUBLIC const char * WHDLine_getContentType (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WString_getText (CONTENT_TYPE));
}


/**
 * @param _self WHDLine object instance
 *
 * @return The content type field string length
 * as a 4 bytes integer
 *
 * Warc Header line content type field string length provider
 */


WIPUBLIC warc_u32_t WHDLine_getContentTypeLen (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WString_getLength (CONTENT_TYPE));
}


/**
 * @param _self WHDLine object
 * @param ct content type string
 * @param len WARC Content Type string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC-record's content type
 */

WIPUBLIC warc_bool_t WHDLine_setContentType (void * const _self, const char * ct,
                                         const warc_u32_t len)
{
  struct WHDLine * const self = _self;
  /* preconditions */
  CASSERT (self);
  assert (ct);

 /*  unless (w_strcasestr(ct,"/")) */
/*     return (WARC_TRUE); */

  /* reset the text */
  if (WString_setText (CONTENT_TYPE, ct, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHDLine object
 *
 * @return record id constant string
 *
 * Returns the WARC-record id.
 */

WIPUBLIC const char * WHDLine_getRecordId (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WString_getText (RECORD_ID));
}


/**
 * @param _self WHDLine object instance
 *
 * @return The record is field string length
 * as a 4 bytes integer
 *
 * Warc Header line record id field string length provider
 */

WIPUBLIC warc_u32_t WHDLine_getRecordIdLen (const void * const _self)
{
	const struct WHDLine * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WString_getLength (RECORD_ID));
}


/**
 * @param _self WHDLine object
 * @param rid record id constant string
 * @param len WARC Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC-record id
 */

WPUBLIC warc_bool_t WHDLine_setRecordId (void * const _self, const char * rid,
                                     const warc_u32_t len)
{
  struct WHDLine * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (rid);

/*   unless(w_strcasestr(rid,"://")) */
/*     return (WARC_TRUE); */

  /* reset the text */
  if (WString_setText (RECORD_ID, rid, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHDLine object instance
 *
 * @return none
 *
 * Free WHDLine's object memory
 */

WPRIVATE void freeWHDLine (void * _self)
{	
  struct WHDLine * self = _self;
  
  /* preconditions */
  CASSERT (self);

  if (WARC_ID)
    destroy (WARC_ID), WARC_ID = NIL;

  DATA_LENGTH = 0;

  RECORD_TYPE = UNKNOWN_RECORD;

  if (SUBJECT_URI)
    destroy (SUBJECT_URI), SUBJECT_URI = NIL;

  if (CREATION_DATE)
    destroy (CREATION_DATE), CREATION_DATE = NIL;

  if (CONTENT_TYPE)
    destroy (CONTENT_TYPE), CONTENT_TYPE = NIL;

  if (RECORD_ID)
    destroy (RECORD_ID), RECORD_ID = NIL;
}



/**
 * WHDLine_constructor - create a new WDLine object instance
 *
 * @param _self WDLine class object
 * @param app constructor list parameters
 *
 * @return a valid WDLine object or NIL
 *
 * WARC Header Line constructor
 */

WPRIVATE void * WHDLine_constructor (void * const _self, va_list * app)
{
  struct WHDLine * const   self  = _self;

  const char             * wid   = va_arg (* app, const char *);
  const warc_u32_t         wid_l = va_arg (* app, const warc_u32_t);

  const warc_u32_t         dl    = va_arg (* app, const warc_u32_t);

  const warc_rec_t         rt    = va_arg (* app, const warc_rec_t);

  const char             * su    = va_arg (* app, const char *);
  const warc_u32_t         su_l  = va_arg (* app, const warc_u32_t);

  const char             * cd    = va_arg (* app, const char *);
  const warc_u32_t         cd_l  = va_arg (* app, const warc_u32_t);

  const char             * ct    = va_arg (* app, const char *);
  const warc_u32_t         ct_l  = va_arg (* app, const warc_u32_t);

  const char             * rid   = va_arg (* app, const char *);
  const warc_u32_t         rid_l = va_arg (* app, const warc_u32_t);


  WARC_ID = bless (WString, wid, wid_l);
  unless (WARC_ID)
    {
      freeWHDLine (self);
      return (NIL);
    }

  DATA_LENGTH = dl;

  RECORD_TYPE = rt;

  SUBJECT_URI = bless (WString, su, su_l);
  unless (SUBJECT_URI)
    {
      freeWHDLine (self);
      return (NIL);
    }

  CREATION_DATE = bless (WString, cd, cd_l);
  unless (CREATION_DATE)
    {
      freeWHDLine (self);
      return (NIL);
    }

  CONTENT_TYPE = bless (WString, ct, ct_l);
  unless (CONTENT_TYPE)
    {
      freeWHDLine (self);
      return (NIL);
    }


  RECORD_ID = bless (WString, rid, rid_l);
  unless (RECORD_ID)
    {
      freeWHDLine (self);
      return (NIL);
    }

  return self;
}


/**
 * WDLine_destructor - delete an existing WString object
 *
 * @param _self WDLine object instance
 *
 * WARC Header Line destructor
 */

WPRIVATE void * WHDLine_destructor (void * _self)
{	
  freeWHDLine (_self);

  return (_self);
}


/**
 * WARC WDLine class
 */

static const struct Class _WHDLine = {
	sizeof (struct WHDLine),
    SIGN,
	WHDLine_constructor, WHDLine_destructor
};

const void * WHDLine = & _WHDLine;
