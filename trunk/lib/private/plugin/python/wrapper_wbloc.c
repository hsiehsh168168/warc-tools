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

#include <wmisc.h>   /* unless, ... */
#include <wbloc.h>   /* WBloc class */

#include <wrapper_wbloc.h>

WPUBLIC PyObject* WRAPPER_WBlock_next (void * _self)
{
  struct WBloc    * self = _self;
  const warc_u8_t * dat  = NIL;
  
  assert (self);
  
  dat = WBloc_next (self);
  if( dat )
    {
      warc_u32_t len = WBloc_getLastChunkSize (self);
      return (Py_BuildValue("s#", dat , len));
    }
  
  return (NIL);
}