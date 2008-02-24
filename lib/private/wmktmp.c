
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
#include <wmem.h>    /* wmalloc, wfree */
#include <wmisc.h>   /* unless, ... */
#include <wmktmp.h>  /* default class */
#include <wcsafe.h>


/**
 * WARC WTempFile internal structure
 */

#define SIGN 13

struct WTempFile
  {

    const void * class;

    /*@{*/
    FILE * handle; /**< temporary file handle */
    /*@}*/
  };


#define HANDLE  (self -> handle)




/**
 * @param tmps  a temporary structure
 *
 * @return file handle descriptor, NIL otherwise
 *
 * Returns the temporary file handle descriptor
 */

WIPUBLIC FILE * WTempFile_handle  (const void * const _self)
{

  const struct WTempFile * const self = _self;

  assert (self);

  return (HANDLE);
}


/**
 * WTempFile_constructor - create a new WTempFile object instance
 *
 * @param _self WTempFile class object
 * @param app constructor list parameters
 *
 * @return a valid WTempFile object or NIL
 *
 * @brief WARC Temporary File object constructor
 */

WPRIVATE void * WTempFile_constructor (void * _self, va_list * app)
{
#define  TEMPLATE_NAME  "w17XXXXXX"

  struct WTempFile * const self = _self;
  char                     template [] = TEMPLATE_NAME;
  warc_i32_t               wfd;
  mode_t                   old_mode;

  UNUSED (app);

  /* create file with restrictive permissions */
  old_mode = umask (077);

  if ( (wfd = mkstemp (template) ) < 0)
    {
      destroy (self);
      return (NIL);
    }

  umask (old_mode);

  if ( (HANDLE = fdopen (wfd, "w+b") ) == NIL || unlink (template) < 0)
    {
      close (wfd);
      destroy (self);
      return (NIL);
    }

  return (self);
}


/**
 * WTempFile_destructor - delete an existing WTempFile object
 *
 * @param _self: WTempFile object instance
 *
 * @brief WARC Temporary File object destructor
 */

WPRIVATE void * WTempFile_destructor (void * _self)
{

  struct WTempFile * self = _self;

  /* preconditions */
  CASSERT (self);

  if (HANDLE)
    fclose (HANDLE), HANDLE = NIL;

  return (self);
}


/**
 * WARC WTempFile class
 */

static const struct Class _WTempFile =
{
  sizeof (struct WTempFile),
  SIGN,
  WTempFile_constructor, WTempFile_destructor
};

const void * WTempFile = & _WTempFile;
