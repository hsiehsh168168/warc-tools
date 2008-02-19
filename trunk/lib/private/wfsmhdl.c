
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
#include <wclass.h>   /* bless, destroy, cassert, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <wmisc.h>    /* NIL ... */
#include <wmem.h>     /* wmalloc, wfree ... */
#include <wstring.h>  /* WString */
#include <whdline.h>  /* WHDLine */
#include <wfsm.x>     /* Transition */
#include <wfsmhdl.h>  /* headers */
#include <wrectype.h> /* revist, resource ... */
#include <wcsafe.h>


#define SIGN 7


/**
 * WARC WFsmHDL class internal
 */



typedef struct
{
  void * warc_id;       /* WString */
  void * data_length;   /* WString */
  void * record_type;   /* WString */
  void * subject_uri;   /* WString */
  void * creation_date; /* WString */
  void * content_type;  /* WString */
  void * record_id;     /* WString */

  FILE       * fin;     /* file handle to read from */
  Transition * state;   /* transition state */
  warc_bool_t      err;  /* parsing error flag */
  char         c;       /* current char in "fin" */
} HDLState;




struct WFsmHDL 
{ 
  const void * class;
  
  /*@{*/
  HDLState * hdls; /**< HDL State */
  /*@}*/
};


#define    HDLS            (self -> hdls)

#define    FIN             (HDLS -> fin)
#define    STATE           (HDLS -> state)
#define    ERROR_FLAG      (HDLS -> err)
#define    CAR             (HDLS -> c)

#define    WARC_ID         (HDLS -> warc_id)
#define    DATA_LENGTH     (HDLS -> data_length)
#define    RECORD_TYPE     (HDLS -> record_type)
#define    SUBJECT_URI     (HDLS -> subject_uri)
#define    CREATION_DATE   (HDLS -> creation_date)
#define    CONTENT_TYPE    (HDLS -> content_type)
#define    RECORD_ID       (HDLS -> record_id)


#define makeS(s) (s), strlen((s))
#define makeC(s) WString_getText((s)), WString_getLength((s))


/* prototypes of all events in the FSM (defined below) */
warc_bool_t WFsmHDL_isSpace   (void *), WFsmHDL_isText    (void *),
        WFsmHDL_isInteger (void *), WFsmHDL_isCR      (void *),
        WFsmHDL_isLF      (void *), WFsmHDL_isUnknown (void *);

/* prototypes of all actions in the FSM (defined below) */
void WFsmHDL_setWarcId       (void *), WFsmHDL_setDataLength  (void *), 
     WFsmHDL_setRecordType   (void *), WFsmHDL_setSubjectUri  (void *),
     WFsmHDL_setCreationDate (void *), WFsmHDL_setContentType (void *), 
     WFsmHDL_setRecordId     (void *), WFsmHDL_pushBack       (void *),
     WFsmHDL_checkRecordType (void *), WFsmHDL_raiseError     (void *),
     WFsmHDL_checkWarcID     (void *), 
     WFsmHDL_raiseErrorDlength (void *),  WFsmHDL_raiseErrorCrDate (void *);

/* WFsmHDL_checkContentType (void *), */
/* WFsmHDL_checkRecordId   (void *) */
/* WFsmHDL_checkUri       (void *), */

/* prototypes of all states in the FSM (defined below) */
State WANT_HDL_WARC_ID,      WANT_HDL_ID_SP,         WANT_HDL_DATA_LENGTH,
      WANT_HDL_DATA_SP ,     WANT_HDL_RECORD_TYPE,   WANT_HDL_RECORD_SP,
      WANT_HDL_SUBJECT_URI,  WANT_HDL_SUBJECT_SP ,   WANT_HDL_CREATION_DATE,
      WANT_HDL_CREATION_SP,  WANT_HDL_CONTENT_TYPE,  WANT_HDL_CONTENT_SP,
      WANT_HDL_RECORD_ID,    WANT_HDL_CR,            WANT_HDL_LF;



WPRIVATE warc_rec_t WFsmHDL_getRecordTypeNumber (const char * rectype)
{
  /* preconditions */
  assert (rectype);

 if      (! strcmp (rectype, "warcinfo"))
   return (WARCINFO_RECORD);
 else if (! strcmp (rectype, "response"))
   return (RESPONSE_RECORD);
 else if (! strcmp (rectype, "request"))
   return (REQUEST_RECORD);
 else if (! strcmp (rectype, "metadata"))
   return (METADATA_RECORD);
 else if (! strcmp (rectype, "revisit"))
   return (REVISIT_RECORD);
 else if (! strcmp (rectype, "conversion"))
   return (CONVERSION_RECORD);
 else if (! strcmp (rectype, "continuation"))
   return (CONTINUATION_RECORD);
 else if (! strcmp (rectype, "resource"))
   return (RESOURCE_RECORD);

 return (UNKNOWN_RECORD);
}


/* rewind the stream (n character before) */
WIPRIVATE void WFsmHDL_rewind (void * _hs, warc_u32_t n)
{
  const HDLState * const hs = _hs;

  assert (hs);
  
  w_fseek_from_here (hs -> fin, - n);
}





/*

@@@@@@@@   @@@@@@   @@@@@@@@@@   
@@@@@@@@  @@@@@@@   @@@@@@@@@@@  
@@!       !@@       @@! @@! @@!  
!@!       !@!       !@! !@! !@!  
@!!!:!    !!@@!!    @!! !!@ @!@  
!!!!!:     !!@!!!   !@!   ! !@!  
!!:            !:!  !!:     !!:  
:!:           !:!   :!:     :!:  
 ::       :::: ::   :::     ::   
 :        :: : :     :      :    
                                 
                                                          
 @@@@@@   @@@@@@@   @@@@@@   @@@@@@@  @@@@@@@@   @@@@@@   
@@@@@@@   @@@@@@@  @@@@@@@@  @@@@@@@  @@@@@@@@  @@@@@@@   
!@@         @@!    @@!  @@@    @@!    @@!       !@@       
!@!         !@!    !@!  @!@    !@!    !@!       !@!       
!!@@!!      @!!    @!@!@!@!    @!!    @!!!:!    !!@@!!    
 !!@!!!     !!!    !!!@!!!!    !!!    !!!!!:     !!@!!!   
     !:!    !!:    !!:  !!!    !!:    !!:            !:!  
    !:!     :!:    :!:  !:!    :!:    :!:           !:!   
:::: ::      ::    ::   :::     ::     :: ::::  :::: ::   
:: : :       :      :   : :     :     : :: ::   :: : :    

*/



State WANT_HDL_WARC_ID = 
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isText,          WFsmHDL_setWarcId,       WANT_HDL_WARC_ID},
    {WFsmHDL_isSpace,         WFsmHDL_checkWarcID,     WANT_HDL_ID_SP  },
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };


State WANT_HDL_ID_SP =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isSpace,         NIL,                           WANT_HDL_ID_SP       },
    {WFsmHDL_isInteger,       WFsmHDL_setDataLength  ,       WANT_HDL_DATA_LENGTH },
    {WFsmHDL_isUnknown,       WFsmHDL_raiseErrorDlength,     NIL}
  };


State WANT_HDL_DATA_LENGTH =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isInteger,       WFsmHDL_setDataLength,      WANT_HDL_DATA_LENGTH },
    {WFsmHDL_isSpace,         NIL,                        WANT_HDL_DATA_SP     },
    {WFsmHDL_isUnknown,       WFsmHDL_raiseErrorDlength,  NIL}
  };


State WANT_HDL_DATA_SP =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isSpace,         NIL,                     WANT_HDL_DATA_SP       },
    {WFsmHDL_isText,          WFsmHDL_setRecordType,   WANT_HDL_RECORD_TYPE   },
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };


State WANT_HDL_RECORD_TYPE =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isText,          WFsmHDL_setRecordType,   WANT_HDL_RECORD_TYPE},
    {WFsmHDL_isSpace,         WFsmHDL_checkRecordType, WANT_HDL_RECORD_SP},
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };


State WANT_HDL_RECORD_SP = 
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isSpace,         NIL,                     WANT_HDL_RECORD_SP },
    {WFsmHDL_isText,          WFsmHDL_setSubjectUri,   WANT_HDL_SUBJECT_URI},
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };


State WANT_HDL_SUBJECT_URI =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isText,          WFsmHDL_setSubjectUri,   WANT_HDL_SUBJECT_URI},
    {WFsmHDL_isSpace,         NIL,                     WANT_HDL_SUBJECT_SP },
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };


State WANT_HDL_SUBJECT_SP =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isSpace,         NIL,                       WANT_HDL_SUBJECT_SP   },
    {WFsmHDL_isInteger,       WFsmHDL_setCreationDate,   WANT_HDL_CREATION_DATE},
    {WFsmHDL_isUnknown,       WFsmHDL_raiseErrorCrDate,  NIL}
  };


State WANT_HDL_CREATION_DATE =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isInteger,       WFsmHDL_setCreationDate,  WANT_HDL_CREATION_DATE},
    {WFsmHDL_isSpace,         NIL,                      WANT_HDL_CREATION_SP  },
    {WFsmHDL_isUnknown,       WFsmHDL_raiseErrorCrDate, NIL}
  };


State   WANT_HDL_CREATION_SP =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isSpace,         NIL,                     WANT_HDL_CREATION_SP },
    {WFsmHDL_isText,          WFsmHDL_setContentType,  WANT_HDL_CONTENT_TYPE},
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };
 

State WANT_HDL_CONTENT_TYPE =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isText,          WFsmHDL_setContentType,   WANT_HDL_CONTENT_TYPE},
    {WFsmHDL_isSpace,         NIL, WANT_HDL_CONTENT_SP  },
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,       NIL}
  };


State WANT_HDL_CONTENT_SP =
   {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isSpace,         NIL,                     WANT_HDL_CONTENT_SP},
    {WFsmHDL_isText,          WFsmHDL_setRecordId,     WANT_HDL_RECORD_ID },
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };


State WANT_HDL_RECORD_ID   =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isCR,            WFsmHDL_pushBack,        WANT_HDL_CR       },
    {WFsmHDL_isText,          WFsmHDL_setRecordId,     WANT_HDL_RECORD_ID},
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };


State WANT_HDL_CR          =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isCR,            NIL,  WANT_HDL_LF},
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,     NIL}
  };


State WANT_HDL_LF          =
  {
    /* TEST_EVENT             ACTION                   NEXT_STATE */

    {WFsmHDL_isLF,            NIL,                     NIL},
    {WFsmHDL_isUnknown,       WFsmHDL_raiseError,      NIL}
  };




/*

@@@@@@@@   @@@@@@   @@@@@@@@@@   
@@@@@@@@  @@@@@@@   @@@@@@@@@@@  
@@!       !@@       @@! @@! @@!  
!@!       !@!       !@! !@! !@!  
@!!!:!    !!@@!!    @!! !!@ @!@  
!!!!!:     !!@!!!   !@!   ! !@!  
!!:            !:!  !!:     !!:  
:!:           !:!   :!:     :!:  
 ::       :::: ::   :::     ::   
 :        :: : :     :      :    
                                 
                                                           
@@@@@@@@  @@@  @@@  @@@@@@@@  @@@  @@@  @@@@@@@   @@@@@@   
@@@@@@@@  @@@  @@@  @@@@@@@@  @@@@ @@@  @@@@@@@  @@@@@@@   
@@!       @@!  @@@  @@!       @@!@!@@@    @@!    !@@       
!@!       !@!  @!@  !@!       !@!!@!@!    !@!    !@!       
@!!!:!    @!@  !@!  @!!!:!    @!@ !!@!    @!!    !!@@!!    
!!!!!:    !@!  !!!  !!!!!:    !@!  !!!    !!!     !!@!!!   
!!:       :!:  !!:  !!:       !!:  !!!    !!:         !:!  
:!:        ::!!:!   :!:       :!:  !:!    :!:        !:!   
 :: ::::    ::::     :: ::::   ::   ::     ::    :::: ::   
: :: ::      :      : :: ::   ::    :      :     :: : :    

 */


warc_bool_t WFsmHDL_isSpace (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  return ((hs -> c == ' ')  || (hs -> c == '\t') ||
          (hs -> c == '\r') || (hs -> c == '\n'));
}


warc_bool_t WFsmHDL_isText (void * _hs) 
{
  const HDLState * const hs = _hs;

  assert (hs);

  return ((hs -> c != ' ')  && (hs -> c != '\t') &&
          (hs -> c != '\r') && (hs -> c != '\n'));
}

warc_bool_t WFsmHDL_isInteger (void * _hs)
{
  const HDLState * const hs = _hs;
  
  assert (hs);

  return ((hs -> c >= 48) && (hs -> c <= 57));
}


warc_bool_t  WFsmHDL_isCR (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  return (hs -> c == '\r');
}


warc_bool_t  WFsmHDL_isLF (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  return (hs -> c == '\n');
}


warc_bool_t WFsmHDL_isUnknown (void * _hs)
{
  assert (_hs);

  return (WARC_TRUE);
}



/*

@@@@@@@@   @@@@@@   @@@@@@@@@@   
@@@@@@@@  @@@@@@@   @@@@@@@@@@@  
@@!       !@@       @@! @@! @@!  
!@!       !@!       !@! !@! !@!  
@!!!:!    !!@@!!    @!! !!@ @!@  
!!!!!:     !!@!!!   !@!   ! !@!  
!!:            !:!  !!:     !!:  
:!:           !:!   :!:     :!:  
 ::       :::: ::   :::     ::   
 :        :: : :     :      :    
                                 
                                                                
 @@@@@@    @@@@@@@  @@@@@@@  @@@   @@@@@@   @@@  @@@   @@@@@@   
@@@@@@@@  @@@@@@@@  @@@@@@@  @@@  @@@@@@@@  @@@@ @@@  @@@@@@@   
@@!  @@@  !@@         @@!    @@!  @@!  @@@  @@!@!@@@  !@@       
!@!  @!@  !@!         !@!    !@!  !@!  @!@  !@!!@!@!  !@!       
@!@!@!@!  !@!         @!!    !!@  @!@  !@!  @!@ !!@!  !!@@!!    
!!!@!!!!  !!!         !!!    !!!  !@!  !!!  !@!  !!!   !!@!!!   
!!:  !!!  :!!         !!:    !!:  !!:  !!!  !!:  !!!       !:!  
:!:  !:!  :!:         :!:    :!:  :!:  !:!  :!:  !:!      !:!   
::   :::   ::: :::     ::     ::  ::::: ::   ::   ::  :::: ::   
 :   : :   :: :: :     :     :     : :  :   ::    :   :: : :    

*/

 

void WFsmHDL_setWarcId (void * _hs)
{
  const HDLState * const hs  = _hs;
  
  assert (hs -> warc_id);

  WString_append (hs -> warc_id, & (hs -> c), 1);
}


void WFsmHDL_setDataLength (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> data_length, & (hs -> c), 1);
}

void WFsmHDL_setRecordType (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> record_type, & (hs -> c), 1);
}

void WFsmHDL_checkRecordType (void * _hs)
{
  HDLState * const hs  = _hs;
  warc_rec_t       t;

  assert (hs);

  t = WFsmHDL_getRecordTypeNumber (WString_getText (hs -> record_type)); 

 /* if unknown WARC record type, stop parsing */
 if (t == UNKNOWN_RECORD)
   {
     /* rewind the stream */
     WFsmHDL_rewind (hs, WString_getLength (hs -> record_type) + 1);

     /* raise the flag errore */
     WarcDebugMsg(">> Expepted  Record type\n");
     hs -> err = WARC_TRUE;
   }
}

void WFsmHDL_checkWarcID  (void * _hs)
{
  HDLState * const hs  = _hs;
  const char *      strtompon;

  assert (hs);

  strtompon = WString_getText (hs -> warc_id);
 
 /* if unknown WARC record type, stop parsing */
 if (strcmp(strtompon,"warc/0.9"))
   {
     /* rewind the stream */
     WFsmHDL_rewind (hs, WString_getLength (hs -> warc_id) + 1);

     /* raise the flag errore */
     WarcDebugMsg(">> Exepted Warc Id\n");
     hs -> err = WARC_TRUE;
   }
}


/* void WFsmHDL_checkUri  (void * _hs) */
/* { */
/*   HDLState * const hs  = _hs; */
  
/*   warc_i32_t        i; */
/*   assert (hs); */
/*   i= WString_strstr(hs->subject_uri,"://"); */
  
/*  /\* if unknown uri, stop parsing *\/ */
/*  if (i == -1) */
/*    { */
/*      /\* rewind the stream *\/ */
/*      WFsmHDL_rewind (hs, WString_getLength (hs -> subject_uri) + 1); */

/*      /\* raise the flag errore *\/ */
/*      WarcDebugMsg(">> Exepted Uri\n"); */
/*      hs -> err = WARC_TRUE; */
/*    } */
/* } */


/* void WFsmHDL_checkRecordId    (void * _hs) */
/* { */
/*   HDLState * const hs  = _hs; */
  
/*   warc_i32_t        i; */
/*   assert (hs); */
/*   i= WString_strstr(hs->record_id,"://"); */

/*  /\* if unknown Record id, stop parsing *\/ */
/*  if (i == -1) */
/*    { */
/*      /\* rewind the stream *\/ */
/*      WFsmHDL_rewind (hs, WString_getLength (hs -> record_id) + 1); */

/*      /\* raise the flag errore *\/ */
/*      WarcDebugMsg(">> Exepted Record Id\n"); */
/*      hs -> err = WARC_TRUE; */
/*    } */
/* } */


/* void   WFsmHDL_checkContentType (void * _hs) */
/* { */
/*   HDLState * const hs  = _hs; */
  
/*   warc_i32_t        i; */
/*   assert (hs); */
/*   i= WString_strstr(hs->content_type,"/"); */
/*  /\* if unknown Content type, stop parsing *\/ */
/*  if (i == -1) */
/*    { */
/*      /\* rewind the stream *\/ */
/*      WFsmHDL_rewind (hs, WString_getLength (hs -> content_type) + 1); */

/*      /\* raise the flag errore *\/ */
/*      WarcDebugMsg(">> Exepted ContentType\n"); */
/*      hs -> err = WARC_TRUE; */
/*    } */
/* } */



void WFsmHDL_setSubjectUri (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> subject_uri, & (hs -> c), 1);
}

void WFsmHDL_setCreationDate (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> creation_date, & (hs -> c), 1);  
}

void WFsmHDL_setContentType (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> content_type, & (hs -> c), 1);
}

void WFsmHDL_setRecordId (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> record_id, & (hs -> c), 1);
}

void WFsmHDL_pushBack (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  w_ungetc (hs -> c, hs -> fin);
}


void WFsmHDL_raiseError (void * _hs)
{
  HDLState * const hs  = _hs;

  assert (hs);

  /* raise "on" the error flag */
  hs -> err = WARC_TRUE;
}

void WFsmHDL_raiseErrorDlength (void * _hs)
{
  HDLState * const hs  = _hs;

  assert (hs);
  w_ungetc (hs -> c, hs -> fin);
  WarcDebugMsg(">> exepted DataLength\n");
  /* raise "on" the error flag */
  hs -> err = WARC_TRUE;
}

void  WFsmHDL_raiseErrorCrDate (void * _hs)
{
  HDLState * const hs  = _hs;
  
  assert (hs);
  w_ungetc (hs -> c, hs -> fin);
  WarcDebugMsg(">> expected Creation Date\n");
  /* raise "on" the error flag */
  hs -> err = WARC_TRUE;
}


/*

@@@@@@@@   @@@@@@   @@@@@@@@@@       @@@@@@   @@@@@@@   @@@  
@@@@@@@@  @@@@@@@   @@@@@@@@@@@     @@@@@@@@  @@@@@@@@  @@@  
@@!       !@@       @@! @@! @@!     @@!  @@@  @@!  @@@  @@!  
!@!       !@!       !@! !@! !@!     !@!  @!@  !@!  @!@  !@!  
@!!!:!    !!@@!!    @!! !!@ @!@     @!@!@!@!  @!@@!@!   !!@  
!!!!!:     !!@!!!   !@!   ! !@!     !!!@!!!!  !!@!!!    !!!  
!!:            !:!  !!:     !!:     !!:  !!!  !!:       !!:  
:!:           !:!   :!:     :!:     :!:  !:!  :!:       :!:  
 ::       :::: ::   :::     ::      ::   :::   ::        ::  
 :        :: : :     :      :        :   : :   :        :    

*/


/**
 * @param _self WFsmHDL object
 *
 * @return address pointer
 *
 * Returns the address of the actual state.
 * Useful for debugging purposes.
 */

WIPUBLIC const void * WFsmHDL_state (const void * const _self)
{
  const struct WFsmHDL * const self    = _self;

  /* preconditions */
  CASSERT (self);

  return (STATE);
}


/**
 * @param _self WFsmHDL object
 *
 * @return warc_bool_t
 *
 * Runs the FSM to detect a WARC header line. Returns a warc_bool_t FSM 
 * detection succeeds or not. 
 *
 * @brief FSM scheduler for WHDLine detection
 */

WPUBLIC warc_bool_t WFsmHDL_run (void * const _self)
{
  struct WFsmHDL * const self = _self;
  char                   c;

  /* preconditions */
  CASSERT (self);

  while (STATE != NIL)
    {
      Transition * tp;

      assert (HDLS);

      /* read 1 byte from FIN stream */
      c = w_fgetc (FIN);

      /* EOF or "read error" ? */
      if (! w_feof (FIN) && ! w_ferror (FIN))
        CAR = c;
      else
        return (WARC_TRUE);

      /* check all events in the current state */
      for (tp = STATE; tp -> thisEvent (HDLS) == WARC_FALSE; ++ tp) 
        /* empty body */ ;

      /* call the action corresponding to the event */
      if (tp -> action != NIL)
        tp -> action (HDLS);
      
      /* move to the new state if no error */
      /* don't forget to advance the "id" state number */
      unless (ERROR_FLAG)
        STATE = tp -> newState;
      else
        break;
}

  /* STATE != NIL means "error", otherwise "success" */
  return (STATE != NIL); 
}



/**
 * @param _self WFsmHDL object
 *
 * @return a WHDLine object or NIL
 *
 * Returns a WHDLine record object found by the FSM
 */

WPUBLIC void * WFsmHDL_transform (const void * const _self)
{
  const struct WFsmHDL * const self    = _self;
  warc_rec_t                   t;

  /* preconditions */
  CASSERT (self);

  t = WFsmHDL_getRecordTypeNumber (WString_getText (RECORD_TYPE));

  return (bless (WHDLine, 
                 makeC (WARC_ID), 
                 (warc_u32_t) atoi (WString_getText (DATA_LENGTH)),
                 t, 
                 makeC (SUBJECT_URI), 
                 makeC (CREATION_DATE),
                 makeC (CONTENT_TYPE), 
                 makeC (RECORD_ID)));
}




/**
 * WFsmHDL_constructor - create a new WFsmHDL object instance
 *
 * @param _self WFsmHDL class object
 * @param app: input stream opened for reading (ex.stdin ...)
 *
 * @return A valid WFsmHDL object or NIL
 *
 * Initialize all WString members of the HDLState pointer "_hs"
 */

WPRIVATE void * WFsmHDL_constructor (void * _self, va_list * app)
{
  struct WFsmHDL * const self = _self;
  FILE           *       fin  = va_arg (* app, FILE *);
  
  HDLS = wmalloc (sizeof (HDLState));
  assert (HDLS);

  FIN   = fin;              /* read from this readable stream */
  STATE = WANT_HDL_WARC_ID; /* start state */
  ERROR_FLAG = WARC_FALSE;       /* no error when starting */

  WARC_ID = bless (WString, makeS(""));
  assert (WARC_ID);

  DATA_LENGTH = bless (WString, makeS(""));
  assert (DATA_LENGTH);

  RECORD_TYPE = bless (WString, makeS(""));
  assert (RECORD_TYPE);

  SUBJECT_URI = bless (WString, makeS(""));
  assert (SUBJECT_URI);

  CREATION_DATE = bless (WString, makeS(""));
  assert (CREATION_DATE);

  CONTENT_TYPE = bless (WString, makeS(""));
  assert (CONTENT_TYPE);

  RECORD_ID = bless (WString, makeS(""));
  assert (RECORD_ID);
  
  return (self);
}


/**
 * @param _self WFsmHDL object instance
 *
 * WARC List destructor
 */

WPRIVATE void * WFsmHDL_destructor (void * _self)
{	
  struct WFsmHDL  * const self = _self;

  /* preconditions */
  CASSERT (self);

  FIN        = NIL;
  STATE      = NIL;
  ERROR_FLAG = WARC_FALSE;

  if (WARC_ID)
    destroy (WARC_ID), WARC_ID = NIL;

  if (DATA_LENGTH)
    destroy (DATA_LENGTH), DATA_LENGTH = NIL;

  if (RECORD_TYPE)
    destroy (RECORD_TYPE), RECORD_TYPE = NIL;

  if (SUBJECT_URI)
    destroy (SUBJECT_URI), SUBJECT_URI = NIL;

  if (CREATION_DATE)
    destroy (CREATION_DATE), CREATION_DATE = NIL;

  if (CONTENT_TYPE)
    destroy (CONTENT_TYPE), CONTENT_TYPE = NIL;

  if (RECORD_ID)
    destroy (RECORD_ID), RECORD_ID= NIL;

  wfree (HDLS), HDLS = NIL;

  return (self);
}


/**
 * WARC WFsmHDL class
 */

static const struct Class _WFsmHDL = {
	sizeof(struct WFsmHDL),
	SIGN,
	WFsmHDL_constructor, WFsmHDL_destructor
};

const void * WFsmHDL = & _WFsmHDL;
