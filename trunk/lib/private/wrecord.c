
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


#include <wclass.h>  /* bless, destroy, cassert, struct Class */
#include <wsign.h>   /* CASSET macro */
#include <wrecord.h> /* for class's prototypes */
#include <whdline.h> /* WHDLine */
#include <wstring.h> /* WString */
#include <wlist.h>   /* WList */
#include <wanvl.h>   /* WAnvl */
#include <wmktmp.h>  /* WTempFile */
#include <wmisc.h>   /* unless */
#include <wcsafe.h>  /*w_strcasestr  */
#include <wcsafe.h>

#define _GNU_SOURCE



/**
 * WARC WRecord class internal
 */

#define SIGN 5

#define MASK 0x0000001F
struct WRecord
{ 
  const void * class;
  
  /*@{*/
  void * hdl; /**< WHDLine object */
  void * list; /**< WList of WAnvl objects */
  void * tdatafile; /**< Temporary Data File descriptor */
  FILE * externdfile; /**< User given data file descriptor */
  void * arccontent; /**< data File recovered from an Arc Record */
  warc_u32_t check; /**< Fields presence cheking value */
  warc_u64_t size; /**< Warc Data File size */
  warc_bool_t (* callback) (void* env, const char * buff, warc_u32_t size); /**< Pointer to the user call back function */
  void * env; /**< pointer to user data buffer */
  warc_i64_t woffset; /**< the offset of the data bloc in the warc file */
  void * who; /**< The WFile object representing the WarcFile which contains this record*/
  warc_bool_t presentchdr; /**< in compressed record case, to see if 
                           warc the compressionheader where present */
  warc_u64_t  csize; /**< the compressed size of the record (in compressed cas) */
  warc_u64_t  usize; /**< the uncompressed size of the record */
  warc_u64_t  offsetinfile; /**< the begining offset of the record in the Warc File */
  
  /*@}*/
};


#define HDL     (self -> hdl)
#define LIST    (self -> list)
#define DATAF   (self -> tdatafile)
#define EDATAF  (self -> externdfile)
#define CHECK   (self -> check)
#define CBACK   (self -> callback)
#define ENV     (self -> env)
#define SIZE    (self -> size)
#define OFFSET  (self -> woffset)
#define WHO     (self -> who)
#define CHDP    (self -> presentchdr)
#define ACONT   (self -> arccontent)
#define CSIZE   (self -> csize)
#define USIZE   (self -> usize)
#define IOFFSET (self -> offsetinfile)


/**
 * @param _self: WRecord object instance
 *
 * @return The Compressed size of the record if succeeeds, 0 otherwise
 *
 * Returns the Warc Record compressed size in compressed Warc File case
 */

WIPUBLIC warc_u64_t WRecord_getCompressedSize (const void * const _self)
{
    const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (CSIZE)
     return (0);

 return (CSIZE);

}


/**
 * @param _self WRecord object instance
 * @param csize the compressed size of the record
 *
 * @return false if succeeds, true otherwise
 *
 * Warc Record compressed size updating function
 */

WIPUBLIC warc_bool_t WRecord_setCompressedSize (void * _self, warc_u64_t csize)
{
    struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (csize)
     return (WARC_TRUE);

  CSIZE = csize;

  return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 * @param[out]: the uncompressed size of the record
 * Returns sthe size of the recird when it is uncompressed
 */

WIPUBLIC warc_u64_t WRecord_getUncompressedSize (const void * const _self)
{
    const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (USIZE)
       return (0);

  return (USIZE);
}


/**
 * @param _self WRecord object instance
 * @param usize The uncompreesed size of the record
 *
 * @return false if succeeds, True otherwise
 *
 * Warc Record uncompressed size updating function
 */

WIPUBLIC warc_bool_t WRecord_setUncompressedSize (void * _self, warc_u64_t usize)
{
    struct WRecord * self = _self;

  /* Precondition */
  CASSERT (self);

  unless (usize)
      return (WARC_TRUE);

  USIZE = usize;

  return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 *
 * @return The offset off the record in its Warc File
 *
 * Returns the WRecord offset in the Warc File
 */

WIPUBLIC warc_i64_t WRecord_getOffset (const void * const _self)
{
    const struct WRecord * const self = _self;

  /* Precondition */
  CASSERT (self);

  return (IOFFSET);
}


/**
 * @param _self WRecord object instance
 * @param inwoffset The offset of the Wrecord in its Warc File
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record in Warc File offset updating function
 */

WIPUBLIC warc_bool_t WRecord_setWFileOffset (void * _self, warc_i64_t inwoffset)
{
    struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  if (inwoffset < 0)
    return (WARC_TRUE);

 IOFFSET = inwoffset;
 return (WARC_FALSE);
}


/**
 * @param _self WRecord object
 *
 * @return constant WARC id string
 *
 * Returns the WARC id of the WARC-record
 */

WIPUBLIC const char * WRecord_getWarcId (const void * const _self)
{
	const struct WRecord * const self = _self;

   /* preconditions */
    CASSERT (self);

   return (WHDLine_getWarcId(HDL));
}


/**
 * @param _self WRecord object
 *
 * @return data length as a 32 bits unsigned integer
 *
 * Returns the data length of the WARC-record
 */

WIPUBLIC warc_u32_t WRecord_getDataLength (const void * const _self)
{
    const struct WRecord * const self = _self;

   /* preconditions */
    CASSERT (self);

   return (WHDLine_getDataLength (HDL));
    
}


/**
 * @param _self WRecord object
 *
 * @return WARC-record type as an integer (see "whdline.h")
 *
 * Returns the WARC-record type
 */

WIPUBLIC warc_rec_t WRecord_getRecordType (const void * const _self)
{
    const struct WRecord * const self = _self;

   /* preconditions */
    CASSERT (self);
 
   return (WHDLine_getRecordType (HDL));
    
}


/**
 * @param _self WRecord object
 *
 * @return WARC-record subject URI string
 *
 * Returns a constant string which represent the 
 * subject URI of the WARC-record
 */

WIPUBLIC const char * WRecord_getSubjectUri (const void * const _self)
{
  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHDLine_getSubjectUri(HDL));
}


/**
 * @param _self WRecord object
 *
 * @return creation date constant string
 *
 * Returns the WARC-record creation date
 */

WIPUBLIC const char * WRecord_getCreationDate (const void * const _self)
{
    const struct WRecord * const self = _self;

   /* preconditions */
    CASSERT (self);

     return (WHDLine_getCreationDate (HDL));
}


/**
 * @param _self WRecord object
 *
 * @return content type constant string
 *
 * Returns the WARC-record content type
 */

WIPUBLIC const char * WRecord_getContentType (const void * const _self)
{
   const struct WRecord * const self = _self;

   /* preconditions */
    CASSERT (self);

  return (WHDLine_getContentType (HDL));
}


/**
 * @param _self WRecord object
 *
 * @return record id constant string
 *
 * Returns the WARC-record id.
 */

WIPUBLIC const char * WRecord_getRecordId (const void * const _self)
{
    const struct WRecord * const self = _self;

   /* preconditions */
    CASSERT (self);

	return (WHDLine_getRecordId (HDL));
}


/**
 * @param _self WRecord object
 * @param t WARC-record type integer (see "whdline.h")
 *
 * @return boolean ,false if is failed true if success
 *
 * Sets the WARC-record type
 */


WIPUBLIC warc_bool_t WRecord_setRecordType (void * _self,const warc_rec_t t)
{
  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);

  wrt = WHDLine_setRecordType(HDL, t);
  if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0001;

  return wrt;
}


/**
 * @param _self WRecord object
 * @param suri Subject URI string
 * @param len WARC id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the subject URI of the WARC-record
 */

WIPUBLIC warc_bool_t WRecord_setSubjectUri (void * _self, const char * suri,
                                        const warc_u32_t len)
{
  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (suri);

  wrt = WHDLine_setSubjectUri(HDL, suri, len); 
  if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0002;
  
  return wrt;
 }


/**
 * @param _self WRecord object
 * @param cd creation date string
 * @param len creation date string length
 * 
 * @return false if succseeds, true otherwise
 *
 * Sets the creation date of the WARC-record
 */

WIPUBLIC warc_bool_t WRecord_setCreationDate (void *  _self, const char * cd,
                                          const warc_u32_t len)
{
  struct WRecord *  self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (cd);
  
  wrt = WHDLine_setCreationDate(HDL, cd, len);
  if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0004;

  return wrt;
} 


/**
 * @param _self WHDLine object
 * @param ct content type string
 * @param len content type string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC-record's content type
 */

WIPUBLIC warc_bool_t WRecord_setContentType (void * _self, const char * ct,
                                             const warc_u32_t len)
{
  struct WRecord * self = _self;
  warc_bool_t wrt;
  
  /* preconditions */
  CASSERT (self);
  assert (ct);

  wrt = WHDLine_setContentType(HDL, ct, len);
  if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0008;

  return wrt;
}


/**
 * @param _self WHDLine object
 * @param rid record id constant string
 * @param len record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC-record id
 */

WPUBLIC warc_bool_t WRecord_setRecordId (void * _self, const char * rid,
                                     const warc_u32_t len)
{
  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (rid);

  wrt = WHDLine_setRecordId(HDL, rid, len);
  if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0010;
  
  return (wrt);
}


/**
 * @param _self WRecord object instance
 * @param key the key of an anvl field as a const char *
 *
 * @return A valid anvl field value correspnding to the key, NIL otherwise
 *
 * Warc Record anvl field value corrsponding to a key
 */

WIPUBLIC const char* WRecord_getAnvlValue (const void* const _self, 
                                           const char * key)
{
    const struct WRecord * const self = _self;
    const void * anvl = NIL;
    warc_u32_t i = 0;
    warc_u32_t lsize;

    /* Preconditions */
    CASSERT (self);

    unless (LIST)
         return (NIL);

    unless (key)
          return (NIL);

    lsize = WList_size (LIST);
    while (i < lsize)
      {
        anvl = WList_get (LIST, i);
        unless (strcmp (WAnvl_getKey (anvl), key))
          return (WAnvl_getValue (anvl));
        else
          ++ i;
      }
    
    return (NIL);
}


/**
 * @param _self WRecord object instance
 * @param key the key of the anvl Field as a const char *
 * @param klen the key string length in bytes
 * @param value the value of the anvl field as a const char *
 * @param vlen the value string length in bytes
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord new anvl field adding function
 */

WIPUBLIC warc_bool_t WRecord_addAnvl (void * _self, const char * key, 
                                      const warc_u32_t klen, 
                                      const char * value, const warc_u32_t vlen)
{
    struct WRecord * self = _self;
    void * anvl = NIL;

    /* Preconditions */
    CASSERT (self);

    unless (key)
          return (WARC_TRUE);

    unless (value)
          return (WARC_TRUE);

    anvl = bless (WAnvl, key, klen, value, vlen);
    
    unless (anvl)
          return (WARC_TRUE);

    return (WList_push (LIST, anvl));
}


/**
 * @param _self a WRecord object
 *
 * @return the WRecord header Line object if succeeds, NIL otherwise
 *
 * Returns the Warc Record Geader Line object
 */

WIPUBLIC const void * WRecord_getHDLine (const void * const _self)
{
	const struct WRecord * const self = _self;

    /* preconditions */
    CASSERT (self);

    return (HDL);
}


/**
 * @param _self A WRecord object
 * @param h a WHDLine object reference
 *
 * @return The old stored WHDLine object reference if succeeds, NIL otherwise
 *
 * Set a new header line for the Warc Record and returns the old one for uage or destruction 
 */

WIPUBLIC void * WRecord_setHDLine (void * const _self, void * h)
{
	struct WRecord * const self = _self;
    void                 *       old;

    /* preconditions */
    CASSERT (self);

    if (h)
      {
        old = HDL;
        HDL = h;
        return (old);
      }
    
    return (NIL);
}


/**
 * @param _self a WRecord object
 *
 * @param The list of Anvl Fields of the WRecord if succeeds, NIL otherwise
 *
 * Returns the list of the Warc Record anvl fields
 */

WIPUBLIC const void * WRecord_getAnvl (const void * const _self)
{
	const struct WRecord * const self = _self;


    /* preconditions */
    CASSERT (self);

    return (LIST);
}



/**
 * @param _self a WRecord object
 * @param a a WList object reference
 *
 * @return The old stored anvl List reference if succeeds, NIL otherwise
 *
 * Sets a new List of anvl fields for the Warc record and 
 * returns the old one for usage or destruction
 */

WIPUBLIC void * WRecord_setAnvl (void * const _self, void * const a)
{
	struct WRecord * const self = _self;
    void                 *       old;

    /* preconditions */
    CASSERT (self);

    if (a)
      {
       old = LIST;
       LIST = a;
       return (old);
      }

return (NIL);
}


/**
 * @param _self WRecord object instance
 * @param file data file name as const char *
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord data file setting function
 */

WIPUBLIC warc_bool_t WRecord_setContentFromFileName (void * _self, 
                                                     const char * file)
{
   struct WRecord * self  = _self;
   FILE           * dfile = NIL;


  /* Preconditions */
  CASSERT (self);
  assert (! EDATAF);
  assert (! DATAF);

  unless (file) 
        return (WARC_TRUE);
  
  dfile = w_fopen_rb (file);
  unless (dfile)
    return (WARC_TRUE);
  
  EDATAF = dfile;

  w_file_size(dfile, SIZE);
  
  return (WARC_FALSE);
}


/**
 * @param _self Wrecord object instance
 * @param dataf Warc Data File *
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record Data File descriptor updating
 */

WIPUBLIC warc_bool_t WRecord_setContentFromFileHandle (void * _self, 
                                                       void * dataf)
{
  struct WRecord * self  = _self;

  /* Preconditions */
  CASSERT (self);
  assert (! EDATAF); /* EDATAF must be NIL (WRecord manually created) */
  assert (! DATAF);

  unless (dataf)
    return (WARC_TRUE);

  DATAF = dataf;

  return (WARC_FALSE);
}


/**
 * @param _self Wrecord object instance
 *
 * @return the WRecord data bloc file descriptor
 *
 * Warc Record Extern Data File descriptor provider
 */

WPUBLIC FILE * WRecord_getDataFile (const void * const _self)
{
  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);
  assert (DATAF);

  return (WTempFile_handle (DATAF));
}


/**
 * @param _self Wrecord object instance
 *
 * @return the WRecord data bloc file descriptor
 *
 * Warc  Record Extern Data File descriptor provider
 */

WPUBLIC FILE * WRecord_getDataFileExtern (const void * const _self)
{
  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);
  assert (EDATAF);

  return (EDATAF);
}


/**
 * @param _self WRecord object instance
 *
 * @return the Warc Data Bloc File size
 *
 * Warc Record Data Bloc file Size returning function
 */

WIPUBLIC warc_i64_t WRecord_getDataSize (const void * const _self)
{
    const struct WRecord * const self = _self;

    /* Preconditions */
    CASSERT (self);

    return (SIZE);
}


/**
 * @param _self WRecord object instance
 * @param sz the new DataBloc Size
 *
 * @return boolean false if success true otherwise
 *
 * Warc Record Data Bloc file set size
 */

WPUBLIC warc_bool_t WRecord_setContentSize (void * _self,warc_i64_t sz)
{
     struct WRecord *  self = _self;

    /* Preconditions */
    CASSERT (self);

    unless (sz)
      return (WARC_TRUE);

    SIZE = sz;

    return (WARC_FALSE);
}



/**
 * @param _self WRecord object instance
 *
 * @return True if still there is data in the record data file, 
 * false otherwise Warc Record data file access function
 *
 * Warc Record content accessing function
 */

WIPUBLIC warc_bool_t WRecord_getContent (const void* const _self)
{
#define CONTENT_BUFSIZ 4096

    const struct WRecord * const self = _self;
    char          buff [CONTENT_BUFSIZ];
    warc_u64_t    where  = 0;
    warc_u32_t    size   = 0;
    FILE        * handle = NIL;    


    /* preconditions */
    CASSERT (self);
    assert  (DATAF);
    assert  (! EDATAF);

    handle = WTempFile_handle(DATAF);
    
    /* save where we're */
    where = w_ftell (handle);

    while (WARC_TRUE)
      {
        size = w_fread(buff, 1, CONTENT_BUFSIZ, handle);
    
        if (size)
          {
            unless (CBACK (ENV, buff, size))
              break;
          }

        if (w_feof (handle))
          break;
      }
    
    /* return the we're we were */
    w_fseek_from_start (handle, where);

    return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 *
 * @return the offset of the data bloc in a Warc File
 *
 * WRecord data bloc offset provider
 */

WIPUBLIC warc_i64_t WRecord_getWoffset (const void * const _self)
{
    const struct WRecord * const self = _self;

/* Preconditions */
  CASSERT (self);

  if (OFFSET >= 0)
     return (OFFSET);

  return (-1);
}


/**
 * @param _self WRecord object instance
 * @param offset the new offset in a Warc file
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord offset in Warc File updating function
 */

WIPUBLIC warc_bool_t WRecord_setWoffset (void * _self, const warc_i64_t offset)
{
    struct WRecord * self = _self;

/* Preconditions */
CASSERT (self);

  if (offset >= 0)
     {
      OFFSET = offset;
      return (WARC_FALSE);
     }

  return (WARC_TRUE);
}


/**
 * @param _self WRecord object instance
 * @param cback the callback function
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord callback function setting
 */

WIPUBLIC warc_bool_t WRecord_setCallback (void * _self, 
                                          warc_bool_t (* cback) 
                                          (void* env, const char * buff, 
                                           warc_u32_t size))
{
  struct WRecord * self = _self;

  /* Precondition */
  CASSERT (self);
  
  unless (cback)
    return (WARC_TRUE);
  
  CBACK = cback;
  
  return (WARC_FALSE);
}


/**
 * @param _self WRecord instance
 * @param env The user environnement
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord user environnement setting function
 */

WIPUBLIC warc_bool_t WRecord_setEnv (void * _self, void * env)
{
    struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  ENV = env;
  
  return (WARC_FALSE);
}




/**
 * @param _self WRecord object instance
 *
 * @return True if all filled, False otherwise
 *
 * WRecord all fields filling checking function
 */

WPUBLIC warc_bool_t WRecord_check (const void * const _self)
{
   const struct WRecord * const self = _self;
   CASSERT (self);
  
   if (MASK == CHECK)
     return (WARC_TRUE);
   
   return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 *
 * @return The pointer to the WFile object containing the Record, NIL otherwise
 *
 * WRecord matching WFile object pointer provider
 */

WIPUBLIC void * WRecord_fromWho (const void * const _self)
{
    const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  return (WHO);
}


/**
 * @param _self WRecord object instance
 * @param wfile the pointer to the WFile object that cill contain the record
 *
 * @return False if succeeds, True otherwise
 *
 * Sets the WFile object containing the WRecord
 */

WIPUBLIC warc_bool_t WRecord_setWfile (void * _self, void * wfile)
{
    struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (wfile)
    return (WARC_TRUE);
 
  WHO = wfile;
 return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 * @param present warc_bool_t to indicate the presence of the warc compression
 * header in compressed record
 *
 * @return none
 *
 * Warc Record warc compression header field updating function
 */

WIPUBLIC void WRecord_setCHeaderPresence (void * _self, const warc_bool_t present)
{
   struct WRecord * self = _self;

  /* Precondition */
  CASSERT (self);

  CHDP = present;

}

/**
 * @param _self WRecord object instance
 *
 * @return True if the warc comlpression header was present, False otherwise
 *
 * compressed Warc Record warc compression header presence
 */

WIPUBLIC warc_bool_t WRecord_getCHeaderPresent (const void * const _self)
{
    const struct WRecord * const self = _self;

  /* Precondition */
  CASSERT (self);

  return (CHDP);

}


/**
 * @param _self WRecord object instanse
 * @param arcontent A data bloc file recovered from an Arc Record
 *
 * @return False if succeeds, True otherwise
 *
 * Sets the Warc Record context from an Arc record context
 * (Used only by Arc Records)
 */

WIPUBLIC warc_bool_t WRecord_setContentFromArc (void * _self, void * arcontent)
{
    struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);
  assert (! EDATAF);
  assert (! DATAF);

  unless (arcontent)
      return (WARC_TRUE);

  ACONT = arcontent;
  EDATAF = WTempFile_handle (ACONT);

  return (WARC_FALSE);

}


/**
 * WRecord_constructor - create a new WRecord object instance
 *
 * @param _self WRecord class object
 * @param app: constructor list parameters
 *
 * @return a valid WRecord object or NIL
 *
 * @brief WARC List constructor
 */

WPRIVATE void * WRecord_constructor (void * _self, va_list * app)
{
  struct WRecord * const self = _self;
  UNUSED(app);

  /* create a WHDLine object */ 
  HDL = bless (WHDLine, "", 0, 0, UNKNOWN_RECORD, "", 0, "", 0, "", 0, "", 0);
  assert (HDL);

  /* create a WAnvl object */ 
  LIST = bless (WList);
  assert (LIST);

  DATAF  = NIL;
  EDATAF = NIL;
  ACONT =  NIL;
  CHECK  = 0;
  CBACK  = NIL;
  SIZE   = 0;
  OFFSET = 0;
  WHO    = NIL;
  CHDP   = WARC_FALSE;


  return (self);
}


/**
 * @param _self WRecord object instance
 *
 * WRecord destructor
 */

WPRIVATE void * WRecord_destructor (void * _self)
{	
  struct WRecord  * self = _self;

  /* preconditions */
  CASSERT (self);

  /* free the WAnvl object */
  if (LIST)
    destroy (LIST), LIST = NIL;

  /* free the WHDLine object */
  if (HDL)
    destroy (HDL), HDL = NIL;

  if (DATAF)
    destroy (DATAF), DATAF = NIL;

  if (ACONT)
     destroy (ACONT), ACONT = NIL, EDATAF = NIL;

  if (EDATAF)
    fclose (EDATAF), EDATAF = NIL;

  if (CHECK)
     CHECK = 0;

  if (CBACK)
     CBACK = NIL;
  
  if (SIZE)
    SIZE = 0;

  if (OFFSET)
    OFFSET = 0;

  if (WHO)
    WHO = NIL;

  if (CHDP)
    CHDP = WARC_FALSE;


  return (self);
}


/**
 * WARC WRecord class
 */

static const struct Class _WRecord = {
	sizeof(struct WRecord),
    SIGN,
	WRecord_constructor, WRecord_destructor
};

const void * WRecord = & _WRecord;
