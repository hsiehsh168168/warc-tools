
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
#include <wsign.h>   /* CASSERT macro */
#include <wstring.h> /* for class's prototypes */
#include <wmem.h>    /* wmalloc, wfree */
#include <wmisc.h>   /* unless, ... */
#include <wcsafe.h>  /* w_strncpy, ... */
#include <wrecord.h>
#include <wmktmp.h>

#define SIGN 22

/**
 * WARC Record Data Bloc Class
 */


struct WBloc
  {

    const void * class;

    /*@{*/
    warc_u32_t    allocation_unit; /**< The amount of Byte to allocate */
    warc_u8_t   * buffer ; /**< The buffer whcich will contain the data */
    warc_bool_t   eob; /** end of bloc */
    void        * wfile; /** The Warc File wher the bloc will be got */
    void        * wrecord; /** The WRecord class instance containg the bloc */
    void        * wtfile; /** The WTempFile object that will contain the data bloc */
    warc_u8_t     http_code[4]; /** Will contain the http response code */
    
    /*@}*/
  };


#define ALLOC        (self -> allocation_unit)
#define BUFF         (self -> buffer)
#define WFILE        (self -> wfile)
#define RECORD       (self -> wrecord)
#define WTFILE       (self -> wtfile)
#define CODE         (self -> http_code)
#define EOB          (self -> eob)

#define DEFAULT_UNIT  64 * 1024


/**
 * @param _self: a WBloc object instance
 * @param bytes: will contain the number of got bytes
 *
 * @return A chunk from the content bloc as an warc_u8_t array
 *
 * Returns a chunk from the data Bloc of a WRecord 
 * without the encapsulated HTTP response if existing
 */

WPUBLIC warc_u8_t * WBloc_next (void * _self)
{
  struct WBloc *  self = _self;

  FILE       * tfile = NIL;
  warc_u32_t  size = 0;

  /* Preconditions  */
  CASSERT (self);

  tfile = WTempFile_handle (WTFILE);
  if(EOB || feof (tfile))
    {
      EOB = WARC_FALSE;
      w_fseek_start (tfile);
      return (NIL);
    }

  size = w_fread (BUFF, 1, ALLOC, tfile);
  if(size)
    BUFF [size] = '\0';

  if(size < ALLOC)
    {
      EOB = WARC_TRUE;
      return (BUFF);
    }

  return (BUFF);
}


/**
 * @param _self: a WBloc object
 *
 * @return the http response code as a char[4] array
 *
 * WARC Record content HTTP response code returning function
 */

WPUBLIC const warc_u8_t * WBloc_getHttpCode (const void * const _self)
{
  const struct WBloc * const self = _self;

  /* Preconditions */
  CASSERT (self);

  /* preconditions */

  unless (CODE)
    return (NIL);

  if (CODE[0] == '\0')
     return (NIL);

  return (CODE);
}




/**
 * WBloc_constructor - create a new WBloc object instance
 *
 * @param _self WBloc class object
 * @param app: constructor list parameters
 *
 * @return a valid WBloc object or NIL
 *
 * @brief WARC Bloc constructor
 */

WPRIVATE void * WBloc_constructor (void * _self, va_list * app)
{

  struct WBloc       * const self = _self;
  void  *             file        = va_arg (* app, void *);
  void  *             record      = va_arg (* app, void *);
  warc_bool_t         httpheaders = va_arg (* app, const warc_bool_t);
  const   warc_u32_t  alloc       = va_arg (* app, const warc_u32_t);

  warc_u32_t allocated = DEFAULT_UNIT;

  unless (record)
  { 
    destroy(self);
    return (NIL);
  }
  
  /* if alloc = 0, use allocated */
  if (alloc)
    allocated = alloc;
  
  ALLOC = allocated;
  BUFF = wmalloc (ALLOC + 1);

  unless (BUFF)
  {
    destroy (self);
    return (NIL);
  }

  WTFILE   = NIL;
  RECORD   = record;
  WFILE    = file;
  CODE [0] = '\0';
  EOB      = WARC_FALSE;

  unless (RECORD && WFILE)
    {
     wfree (BUFF), BUFF = NIL;
     destroy (self);
     return (NIL);
    }

  WTFILE = WRecord_getBloc (RECORD, WFILE, httpheaders, CODE);
  unless (WTFILE)
  {
    destroy (self);
    return (NIL);
  }

  w_fseek_start (WTempFile_handle (WTFILE));

  return (self);
}


/**
 * WBloc_destructor - delete an existing WBloc object
 *
 * @param _self WBloc object instance
 *
 * WARC Bloc destructor
 */

WPRIVATE void * WBloc_destructor (void * _self)
{

  struct WBloc * self = _self;

  /* preconditions */
  CASSERT (self);

  if (BUFF)
     wfree (BUFF), BUFF = NIL;

  if (WTFILE)
      destroy (WTFILE), WTFILE = NIL;

  return (self);
}


/**
 * WARC WBloc class
 */

static const struct Class _WBloc =
  {
    sizeof (struct WBloc),
    SIGN,
    WBloc_constructor, WBloc_destructor
  };

const void * WBloc = & _WBloc;