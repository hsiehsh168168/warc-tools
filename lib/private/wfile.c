
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

#include <wclass.h>   /* bless, destroy, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <wfile.h>    /* WFile ... */
#include <wmisc.h>    /* WARC_TRUE, WARC_FALSE ... */
#include <wlist.h>    /* WList  */
#include <wstring.h>  /* WString */
#include <wrecord.h>  /* WRecord */
#include <wrecord.x>  /* WRecord_getDataFile, WRecord_getDataFileExtern */
#include <wanvl.h>    /* WAnvl */
#include <whdline.h>  /* WHDLine */
#include <wfsmhdl.h>  /* WFsmHDL automata */
#include <wfsmanvl.h> /* WFsmANVL automata */
#include <wmktmp.h>   /* WTempFile */
#include <wgzip.h>    /* WGzip */
#include <wversion.h> /* WARC_VERSION */
#include <wcsafe.h>



#define SIGN 6


/**
 * WARC WFile class internal
 */


struct WFile
{ 
  const void * class;
 
  /*@{*/
  void * fname; /**< WString containing WarcFile name*/
  FILE * fh;    /**< file handle */
  wfile_mode_t mode;  /**< Warc object access mode*/
  wfile_comp_t compressed; /**< indicates if the record is compressed */
  warc_u32_t maxsize; /**< Maximum Warc file size in BYTES */ 
  warc_u64_t fsize; /**< The current WarcFile Size */
  /*@}*/
};


#define FNAME    (self -> fname)
#define FH       (self -> fh)
#define MODE     (self -> mode)
#define MAXSIZE  (self -> maxsize)
#define COMP     (self -> compressed)
#define FSIZE    (self -> fsize)


/* Header line field separator definition */
/* redefine it to change the number of spaces to put */
#define TSPS " "

/* Spaces separator size */
#define TSPSIZE (1)


/**
 * callback environnement for compressed reading
 */


struct CallbackEnv
{
  FILE     * out; /**< output file which will hold the uncompressed data */
  warc_u32_t crlf; /**< to find header end double CRLF */
  warc_u64_t usize; /**< the size of uncompressed data */
};


/* this callback uncompressing the entire WARC file */
warc_u32_t wrecover (const char * buffer, const warc_u32_t nbr, void * env)
{
  FILE * out = (FILE *) env;

  /* copy the uncompressed 'nbr' bytes to out */
  if (w_fwrite (buffer, 1, nbr, out) != nbr || w_ferror (out))
    return (Z_STOP_DECODING); /* return this value to stop the uncompression */

  return (Z_CONTINUE_DECODING);
}

/* this callback stop uncompressing when it find a double CRLF */ 
warc_u32_t getHeader (const char * buffer, const warc_u32_t nbr, void * _env) 
{
   struct CallbackEnv * env  = (struct CallbackEnv *) _env; 
   warc_u64_t           i    = 0; 

   switch (env -> crlf) 
     { 
     case 0 : goto CR1; 
     case 1 : goto LF1; 
     case 2 : goto CR2; 
     case 3 : goto LF2; 
     } 

  CR1: 
  while (i < nbr)  /* search the first CR  */
     { 
      if (buffer [i] == '\r') 
        { 
          env -> crlf ++; 
          ++ i; 
          goto LF1; 
        } 
      ++ i; 
    } 

  goto CONTINUE; 
  
  LF1: 
   if (i < nbr) 
     { 
       if (buffer [i] == '\n') 
         { 
           env -> crlf ++; 
           i++; 
           goto CR2; 
         } 
       else 
        { 
          env -> crlf = 0; 
         goto CONTINUE; 
        } 
    } 
 else 
    goto CONTINUE; 
  CR2: 
  if (i < nbr) 
    { 
      if (buffer [i] == '\r') 
         { 
           env -> crlf ++; 
           ++ i; 
           goto LF2; 
         } 
       else 
         { 
           env -> crlf = 0; 
           goto CONTINUE; 
         } 
     } 
   else 
     goto CONTINUE; 

  LF2: 
   if (i < nbr)  
     { 
       if (buffer [i] == '\n') 
         { 
           goto STOP; 
         } 
       else 
         { 
           env -> crlf = 0; 
           goto CONTINUE; 
         } 
     } 
   else 
     goto CONTINUE; 
  STOP: 
/*  we've found a double CRLF (stop uncompressing)   */
   env -> usize += i + 1; 
   w_fwrite (buffer, 1, i + 1, env -> out); 
   return (Z_STOP_DECODING); 
  CONTINUE: 
/*    copy the uncompressed 'nbr' bytes to out  */
   if (w_fwrite (buffer, 1, nbr, env -> out) != nbr || w_ferror (env -> out)) 
     return (Z_STOP_DECODING); 
   return (Z_CONTINUE_DECODING); 
 } 



/**
 * @param _self WFile object
 *
 * @return WARC_TRUE if succeeds, WARC_FALSE otherwise
 *
 * Check the presence of more Warc Records in the Warc File
 */

WIPUBLIC warc_bool_t WFile_hasMoreRecords (const void * const _self)
{
  const struct WFile * const self = _self;

  /* preconditions */
  CASSERT (self);

  /* we must be in read mode */
  if (MODE != WARC_FILE_READER) 
    return (WARC_TRUE);

  if (w_ftell (FH) == (warc_i64_t) FSIZE)
    {
      w_fseek_start (FH);
      return (WARC_FALSE);
    }
  else 
    return (WARC_TRUE);

  
  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param wtfile the temporary file to fill
 * @param size the size of data to write
 *
 * @return False if succeeds, True otrherwise
 *
 * fills the temporary file with the warc record data
 */


WPRIVATE warc_bool_t WFile_fillTempFile(const void* const _self, 
                                        FILE * wtfile, warc_u64_t size)
{
#define WFLUSH_SIZ 4096

  const struct WFile * const self = _self;
  char                       buf [WFLUSH_SIZ];
  warc_u32_t                 r ;
  warc_u32_t                 p ;
   
   /* Preconditions */
   CASSERT (self);
   assert (wtfile);

   while (size)
     {
       if(size > WFLUSH_SIZ)
         r = WFLUSH_SIZ;
       else
         r = size;
       
       size = size - r;
       
       p = w_fread (buf, 1, r, FH);
       if (w_ferror (wtfile) || r != p)
         {
           WarcDebugMsg("data fill temporary error");
           return (WARC_TRUE);
         }
       
       if (w_fwrite (buf, 1, r, wtfile) != r)
         return (WARC_TRUE);
     }
   
/*    while (size) */
/*      { */
/*        w_fread  (& byte, 1, 1, FH); */
/*        w_fwrite (& byte, 1, 1, wtfile); */
/*        -- size; */
/*      } */

   return (WARC_FALSE);
}


/**
 * @return True if succeeds, False otherwise
 *
 * checks if a WRecord ends with two CRLF
 */

WPRIVATE warc_bool_t WFile_checkEndCRLF(FILE *f)
{
  char str[5];


  if (4 != w_fread (str, 4, 1, f))
    return WARC_TRUE;

  if (w_ferror (f))
    return WARC_TRUE;

  str[4] = '\0';

  return (! (strcmp(str, "\x0D\x0A\x0D\x0A")));
}



/**
 * @param _self WFile object instance
 *
 * @return A Warc Record object reference got from a compressed warc file
 *
 * Warc Record extracting from compressed Warc File
 */


WPRIVATE void * WFile_nextRecordGzipCompressedFast (void * _self,
                                                    void * objrectfile,
                                                    FILE * rectfile,
                                                    void * gzobj,
                                                    warc_i64_t offset,
                                                    warc_u64_t usize,
                                                    warc_u64_t csize
                                                    )
{
  struct WFile  * self = _self;

  void 		    * wobject  = NIL; /* to recover the WRecord Object */
  warc_u32_t      wdatal   = 0;   /* to recover the Record dataLength */
  warc_u32_t      datasize = 0;
  warc_i64_t      blbegin = -1;
  void          * hdl      = NIL; /* To recover Header line */
  void          * anvl     = NIL; /* To recover One Anvl Fields */
  void          * lanvl    = NIL; /* To store anvl fields */
  void          * oldhdl   = NIL; /* for destruction of old value*/
  void          * oldlanvl = NIL; /* for destruction of old value*/
  void          * hfsm     = NIL; /* Header Line automatum */
  void          * afsm     = NIL; /* Anvl Field automatum */
  warc_u32_t      husize    = 0;   /* uncompressed data size */
  warc_u32_t      hcsize   = 0;   /* compressed data size stored in header */
  warc_u32_t      ret      = 0;
  warc_bool_t     more     = WARC_TRUE;
  struct CallbackEnv cbenv;

  cbenv . out   = rectfile;
  cbenv . crlf  = 0;
  cbenv . usize = 0;
  
  wobject = bless (WRecord);
  assert (wobject);
  
  if (WRecord_setWFileOffset (wobject, offset))
   {
   destroy (wobject);
   destroy (objrectfile);
   destroy (gzobj);
   return (NIL);
   }

  if (WRecord_setCompressedSize (wobject, csize))
   {
   destroy (wobject);
   destroy (objrectfile);
   destroy (gzobj);
   return (NIL);
   }

  if (WRecord_setUncompressedSize (wobject, usize))
   {
   destroy (wobject);
   destroy (objrectfile);
   destroy (gzobj);
   return (NIL);
   }

  /* setting the offset of the data bloc in the WRecord */
  if (WRecord_setWoffset(wobject, w_ftell (FH)))
    {
      destroy (objrectfile);
      destroy (wobject);
      return (NIL);
    }
  
  ret = WGzip_uncompress (gzobj, FH, w_ftell (FH), & husize, & hcsize, 
                          getHeader, (void *) & cbenv);
  if (ret)
    {
      WarcDebugMsg ("unable to uncompress more record");
      destroy (objrectfile);
      destroy (gzobj);
      destroy (wobject);
      return (NIL);
    }
  
  
  destroy (gzobj);
  
  offset += csize;
  w_fseek_start (rectfile);
  w_fseek_from_start (FH, offset);
  
  hfsm = bless (WFsmHDL, rectfile);
  assert (hfsm);
  
  unless (WFsmHDL_run (hfsm))
    {
      /* generate the WHDLine object from the FSM */
      hdl = WFsmHDL_transform (hfsm);
    }
  else 
    {
      /* error when parsing the WARC header line */
      w_fprintf (fprintf (stderr ,"error in FSM state address %p,\n", 
                          WFsmHDL_state (hfsm)));
      
      destroy (objrectfile);
      destroy (hfsm);
      destroy (wobject);
      return (NIL);
    }
  
  destroy (hfsm);
  
  lanvl = bless (WList);
  assert (lanvl);
  
  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, rectfile);
      assert (afsm);
      
      unless (WFsmANVL_run (afsm))
        {
          anvl = WFsmANVL_transform (afsm);
          if (anvl)
            WList_push (lanvl,anvl);
          else
            more = WARC_FALSE;
        }
      else
        {
          /* error when parsing the WARC header line */
          w_fprintf (fprintf (stderr ,"error in FSM state address %p\n", 
                   WFsmANVL_state (afsm)));
          
          destroy (objrectfile);
          destroy (lanvl);
          destroy (hdl);
          destroy (afsm);
          destroy  (wobject);
          return  (NIL);
        }
      
      destroy (afsm);
      
    } while (more);
  
  blbegin = w_ftell (rectfile);
  wdatal = WHDLine_getDataLength (hdl);
  destroy (objrectfile);
  
  WRecord_setCHeaderPresence (wobject, WARC_TRUE);
  
  oldhdl = WRecord_setHDLine (wobject, hdl);
  destroy (oldhdl);
  
  oldlanvl = WRecord_setAnvl(wobject, lanvl);
  destroy (oldlanvl);
  
  datasize = wdatal - blbegin;
  
  if (WRecord_setContentSize (wobject, datasize))
    {
      destroy (objrectfile);
      destroy (wobject);
      return (NIL);
    }
  
  
  /* setting the pointer to the WFile in the WRecord */
  if (WRecord_setWfile (wobject,self))
    {
      destroy (objrectfile);
      destroy (wobject);
      return (NIL);
    }
  
  return (wobject);
}


/**
 * @param _sel WFile object instance
 *
 * @return a Warc Record object reference got from a compressed warc file
 *
 * Warc Record extracting from compressed Warc File
 */


WPRIVATE void * WFile_nextRecordGzipCompressed (void * _self)
{
  struct WFile  * self = _self;

  void 		    * wobject  = NIL; /* to recover the WRecord Object */
  warc_u32_t      wdatal   = 0;   /* to recover the Record dataLength */
  warc_u32_t      datasize = 0;
  warc_i64_t      blbegin = -1, recend = -1;
  void          * hdl      = NIL; /* To recover Header line */
  void          * anvl     = NIL; /* To recover One Anvl Fields */
  void          * lanvl    = NIL; /* To store anvl fields */
  void          * oldhdl   = NIL; /* for destruction of old value*/
  void          * oldlanvl = NIL; /* for destruction of old value*/
  void          * hfsm     = NIL; /* Header Line automatum */
  void          * afsm     = NIL; /* Anvl Field automatum */
  void          * gzobj    = NIL; /* for decompression */
  warc_u32_t      usize    = 0;   /* uncompressed data size */
  warc_u32_t      csize    = 0;   /* compressed data size */
  warc_u32_t      ret      = 0;
  warc_i64_t      offset   = 0;
  FILE          * rectfile = NIL; /* to uncompress the WRecord */
  void          * objrectfile = NIL;
  warc_bool_t     more     = WARC_TRUE;


  objrectfile = bless (WTempFile);
  unless (objrectfile)
    {
      WarcDebugMsg ("Unable to create temporary file for record decompression");
      return (NIL);
    }
  rectfile = WTempFile_handle (objrectfile);
  
  gzobj = bless (WGzip);
  offset = w_ftell (FH);


  /* test if the we have the warc extra gzip header */
  ret = WGzip_analyzeHeader (gzobj, FH, w_ftell (FH), & usize, & csize);

  w_fseek_from_start (FH, offset);

  unless (ret)
    return (WFile_nextRecordGzipCompressedFast (self, objrectfile, 
                                                rectfile, gzobj, offset, usize, csize));

  wobject = bless (WRecord);
  assert (wobject);
  
  if (WRecord_setWFileOffset (wobject, offset))
    {
    destroy (wobject);
    destroy (gzobj);
    destroy (objrectfile);
    return (NIL);
    }

  ret = WGzip_uncompress (gzobj, FH, w_ftell (FH), & usize, & csize, 
                          wrecover, (void *) rectfile);

  if (WRecord_setCompressedSize (wobject, csize))
    {
    destroy (wobject);
    destroy (gzobj);
    destroy (objrectfile);
    return (NIL);
    }


 if ( WRecord_setUncompressedSize (wobject, usize))
    {
    destroy (wobject);
    destroy (gzobj);
    destroy (objrectfile);
    return (NIL);
    }



  if (ret)
    {
      WarcDebugMsg ("unable to uncompress more record");
      destroy (objrectfile);
      destroy (gzobj);
      destroy (wobject);
      return (NIL);
    }
  
  offset += csize;
  w_fseek_from_start (FH, offset);
     
  destroy (gzobj);
  w_fseek_start (rectfile);
  
  hfsm = bless (WFsmHDL, rectfile);
  assert (hfsm);
  
  unless (WFsmHDL_run (hfsm))
    {
      /* generate the WHDLine object from the FSM */
      hdl = WFsmHDL_transform (hfsm);
    }
  else 
    {
      /* error when parsing the WARC header line */
      w_fprintf (fprintf (stderr ,"error in FSM state address %p,\n", 
                          WFsmHDL_state (hfsm)));
      
      destroy (objrectfile);
      destroy (hfsm);
      destroy (wobject);
      return (NIL);
    }
  
  destroy (hfsm);
  
  lanvl = bless (WList);
  assert (lanvl);
  
  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, rectfile);
      assert (afsm);
      
      unless (WFsmANVL_run (afsm))
        {
          anvl = WFsmANVL_transform (afsm);
          if (anvl)
            WList_push (lanvl,anvl);
          else
            more = WARC_FALSE;
        }
      else
        {
          /* error when parsing the WARC header line */
          w_fprintf (fprintf (stderr ,"error in FSM state address %p\n", 
                              WFsmANVL_state (afsm)));
          
          destroy (objrectfile);
          destroy (lanvl);
          destroy (hdl);
          destroy (afsm);
          destroy (wobject);
          return  (NIL);
        }
      
      destroy (afsm);
      
    } while (more);
  
  blbegin = w_ftell (rectfile);
  wdatal = WHDLine_getDataLength (hdl);
  
  w_fseek_start (rectfile);
  w_fseek_from_here (rectfile, (warc_i64_t) wdatal);
  recend = (warc_i64_t) w_ftell(rectfile);
  
  /* checking the presence of the two end CRLF */      
  unless (WFile_checkEndCRLF (rectfile))
    {
      WarcDebugMsg ("Corrupted record found");
      destroy (objrectfile);
      destroy (hdl);
      destroy (wobject);
      destroy (lanvl);
      return (NIL);
    }
  
  w_fseek_start (rectfile);

  
  WRecord_setCHeaderPresence (wobject, WARC_FALSE);
  
  oldhdl = WRecord_setHDLine (wobject, hdl);
  destroy (oldhdl);
  
  oldlanvl = WRecord_setAnvl(wobject, lanvl);
  destroy (oldlanvl);
  
  datasize = recend;
  if (WRecord_setContentSize (wobject, datasize))
    {
      destroy (objrectfile);
      destroy (wobject);
      return (NIL);
    }
  
  /* setting the offset of the data bloc in the WRecord */
  if (WRecord_setWoffset(wobject, blbegin))
    {
      destroy (objrectfile);
      destroy (wobject);
      return (NIL);
    }
  
  /* setting the pointer to the WFile in the WRecord */
  if (WRecord_setWfile (wobject,self))
    {
      destroy (objrectfile);
      destroy (wobject);
      return (NIL);
    }
  
  /* setting the data file in The Record (only in compressed mode )*/
  if (WRecord_setContentFromFileHandle (wobject, objrectfile))
    {
      destroy (objrectfile);
      destroy (wobject);
      return (NIL);
    }
  
  return (wobject);
}


/**
 * @param _self WFile object instance
 *
 * @return a Warc Record object reference got from an uncompressed Warc file
 *
 * Warc Record object axtracting from uncompress Warc File
 */

WPRIVATE void * WFile_nextRecordUncompressed (void * _self)
{
  struct WFile  * self = _self;

  void 		    * wobject  = NIL; /* to recover the WRecord Object */
  warc_u32_t      wdatal   = 0;   /* to recover the Record dataLength */
  warc_u32_t      datasize = 0;
  warc_i64_t      recbegin = -1, blbegin = -1, recend = -1;
  void          * hdl      = NIL; /* To recover Header line */
  void          * anvl     = NIL; /* To recover One Anvl Fields */
  void          * lanvl    = NIL; /* To store anvl fields */
  void          * oldhdl   = NIL; /* for destruction of old value*/
  void          * oldlanvl = NIL; /* for destruction of old value*/
  void          * hfsm     = NIL; /* Header Line automatum */
  void          * afsm     = NIL; /* Anvl Field automatum */
  warc_bool_t     more     = WARC_TRUE;

  /* preconditions */
  CASSERT (self);
  
  
  /* stop at end of file */
  if (w_feof (FH))
    return (NIL);

  
  recbegin = (warc_i64_t) w_ftell (FH);

  wobject = bless (WRecord);
  assert (wobject);

  if (WRecord_setWFileOffset (wobject, recbegin))
   {
   destroy (wobject);
   return (NIL);
   }

  hfsm = bless (WFsmHDL,FH);
  assert (hfsm);

  unless (WFsmHDL_run (hfsm))
    {
      /* generate the WHDLine object from the FSM */
      hdl = WFsmHDL_transform (hfsm);
    }
  else
    {
      /* error when parsing the WARC header line */
      w_fprintf (fprintf (stderr ,"error in FSM state address %p, at offset %ld in The Warc File \n", 
                          WFsmHDL_state (hfsm), w_ftell (FH)));
      destroy (hfsm);
      destroy (wobject);
      return (NIL);
    }
  
  destroy (hfsm);
  
  lanvl = bless (WList);
  assert (lanvl);
  
  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, FH);
      assert (afsm);
      
      unless (WFsmANVL_run (afsm))
        {
          anvl = WFsmANVL_transform (afsm);
          if (anvl)
            WList_push (lanvl,anvl);
          else
            more = WARC_FALSE;
        }
      else
        {
          /* error when parsing the WARC header line */
          w_fprintf (fprintf (stderr ,"error in FSM state address %p, at offset %ld \"%s\"\n", 
                   WFsmANVL_state (afsm), w_ftell (FH), ""));
          
          destroy (lanvl);
          destroy (hdl);
          destroy (afsm);
          destroy (wobject);
          return  (NIL);
        }
      
      destroy (afsm);
      
    } while (more);
  
  blbegin = w_ftell (FH);
  
  wdatal = WHDLine_getDataLength (hdl);

  if (WRecord_setUncompressedSize (wobject, wdatal))
   {
   destroy (wobject);
   destroy (hdl);
   destroy (lanvl);
   return (NIL);
   }
  

  if (WRecord_setCompressedSize (wobject, wdatal))
   {
   destroy (wobject);
   destroy (hdl);
   destroy (lanvl);
   return (NIL);
   }
  
  /* checking the presence of the two end CRLF */
  w_fseek_from_start (FH, recbegin);
  w_fseek_from_here (FH, (warc_i64_t) wdatal);
  recend = (warc_i64_t) w_ftell(FH);
  
  unless (WFile_checkEndCRLF(FH))
    {
      WarcDebugMsg ("Failed in trying copying record in temporary file\n");
      destroy (hdl);
      destroy (lanvl);
      destroy (wobject);
      return (NIL);
    }
  
  
  oldhdl = WRecord_setHDLine (wobject, hdl);
  destroy (oldhdl);
  
  oldlanvl = WRecord_setAnvl(wobject, lanvl);
  destroy (oldlanvl);
      
  datasize = recend - blbegin;
  if (WRecord_setContentSize (wobject, datasize))
    {
      destroy (wobject);
      return (NIL);
    }
  
  /* setting the offset of the data bloc in the WRecord */
  if (WRecord_setWoffset(wobject, blbegin))
    {
      destroy (wobject);
      return (NIL);
    }
  
  /* setting the pointer to the WFile in the WRecord */
  if (WRecord_setWfile (wobject,self))
    {
      destroy (wobject);
      return (NIL);
    }
  
  return (wobject);
}


/**
 * @param _self WFile object
 *
 * @return WRecord if succeeds, NIL otherwise
 *
 * Returns a Warc Record from the Warc File
 */

WPUBLIC void * WFile_nextRecord (void * _self)
{
  struct WFile  * self = _self;

  /* preconditions */
  CASSERT (self);
  
  /* stop at end of file */
  if (w_feof (FH))
    return (NIL);

  /* We must be in WARC_FILE_READER mode to extract Warc Records */
  if (MODE != WARC_FILE_READER) 
    return (NIL);

  switch (COMP)
    {
    case WARC_FILE_COMPRESSED_GZIP:
      return (WFile_nextRecordGzipCompressed (self));
    case WARC_FILE_UNCOMPRESSED:
      return (WFile_nextRecordUncompressed (self));
    default:
      WarcDebugMsg ("WFile opened with an unknown mode\n");
    }
  
  return (NIL);
}


/**
 * @param _self WFile object instance
 * @param wrec the WRecord to register
 * @param callback function that user will call to extract data
 * @param env user data that will be filled
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record call back function registration
 */

WPUBLIC warc_bool_t WFile_register (void* _self, void * wrec,
                                    warc_bool_t(* callback) 
                                    (void*, const char *, const warc_u32_t),
                                    void * env)
{
  struct WFile * self      = _self;

  warc_i64_t     wcurrent  = -1;   /* to return to the 
                                      current postion of the WFile */
  warc_i64_t     offset    = -1;   /* to recover the bloc offset 
                                      in the Warc file */
  warc_u32_t     size      = 0;    /* to recover the bloc size from 
                                      the record */

  warc_u32_t     ret       = 0;
  void         * objwtfile = NIL;
  void         * gzobj     = NIL; /* to manage compression */
  warc_u32_t     usize     = 0;
  warc_u32_t     csize     = 0;
  FILE         * wtfile    = NIL;
  

  /* Precondition */
  CASSERT (self);
  assert (wrec);

  /* We must be in WFILE_READ mode to extract Warc Records */
  if (MODE != WARC_FILE_READER) 
    return (WARC_TRUE);
  
  /* check if "self" is the correct WFile object */
  if (self != WRecord_fromWho (wrec))
    return (WARC_TRUE);

  unless (callback)
    {
      WarcDebugMsg ("error: giving NULL callback function \n");
      return (WARC_TRUE);
    }
 
  if (WRecord_setCallback (wrec, callback))
    return (WARC_TRUE);

  if (WRecord_setEnv (wrec, env))
    return (WARC_TRUE);

  offset = WRecord_getWoffset (wrec);

  if (offset < 0)
    return (WARC_TRUE);

  switch (COMP)
    {
    case WARC_FILE_COMPRESSED_GZIP:
      unless (WRecord_getCHeaderPresent (wrec))
        {
          w_fseek_from_here (WRecord_getDataFile(wrec), offset);
        }
      else
        {
          wcurrent = w_ftell (FH);
          w_fseek_from_start (FH, offset);
          
          gzobj = bless (WGzip);
          assert (gzobj);
          
          objwtfile = bless (WTempFile);
          assert (WTempFile);
          
          wtfile = WTempFile_handle (objwtfile);
          
          ret = WGzip_uncompress (gzobj, FH, w_ftell (FH), & usize, 
                                  & csize, wrecover, (void *) wtfile);
          if (ret)
            {
              WarcDebugMsg ("Unable to uncompress compressed Record");
              destroy (objwtfile);
              destroy (gzobj);
              return (WARC_TRUE);
            }
         
          w_fseek_from_start (FH, wcurrent);
          destroy (gzobj);
          w_fseek_from_end (wtfile, -4);
          
          unless (WFile_checkEndCRLF (wtfile))
            {
              WarcDebugMsg ("Error: End DoubleCRLf missed\n");
              destroy (objwtfile);
              return (WARC_TRUE);
            }

          w_fseek_from_here (wtfile, (-1 * WRecord_getDataSize(wrec)));

          if (WRecord_setContentFromFileHandle (wrec, objwtfile))
            {
              WarcDebugMsg ("Faild in trying to guive the bloc file descriptor to the Record");
              destroy (objwtfile);
              return (WARC_TRUE);
            }
        }

      break;

    case WARC_FILE_UNCOMPRESSED:
      objwtfile = bless (WTempFile);
      unless (objwtfile)
        {
          WarcDebugMsg ("unable to create temporary file for the record bloc\n");
          return (WARC_TRUE);
        }
      wtfile = WTempFile_handle (objwtfile);

      size = WRecord_getDataSize (wrec);
      
      wcurrent = (warc_i64_t) w_ftell(FH);
      w_fseek_from_start (FH, offset);
      
      if (WFile_fillTempFile (self, wtfile, size))
        {
          destroy (objwtfile);
          w_fseek_from_start (FH, wcurrent);
          return (WARC_TRUE);
        }
      
      w_fseek_start (wtfile);
      w_fseek_from_start (FH, wcurrent);
      
      if(WRecord_setContentFromFileHandle (wrec, objwtfile))
        {
          destroy (objwtfile);
          w_fseek_from_start (FH, wcurrent);
          return (WARC_TRUE);
        }
      break;

    default:
      WarcDebugMsg ("WFile opened with an unknown mode\n");
      return (WARC_TRUE);
    }
  
  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param offset unsigned integer offset
 *
 * @return WARC_TRUE if succeeds, WARC_FALSE otherwise
 *
 * Moves to this offset in the WARC file
 */


WPUBLIC warc_bool_t WFile_seek (void * _self, const warc_u64_t offset)
{
   struct WFile * self = _self;

   /* Preconditions */
   CASSERT (self);

   w_fseek_from_start (FH, offset);

   return (WARC_TRUE);
}

/**
 * @param _self WFile object instance
 * @param max_size the new maximum Warc File size
 *
 * @return WARC_TRUE if succeeds, WARC_FALSE otherwise
 *
 * WFile File Maximum Size value updating function
 */
WPUBLIC warc_bool_t WFile_setMaxSize (void * _self, const warc_u64_t max_size)
{
   struct WFile * self = _self;

   /* Preconditions */
   CASSERT (self);

   unless (MAXSIZE)
     return (WARC_FALSE);

   if (MAXSIZE < max_size)
       MAXSIZE = max_size;
   else
       return (WARC_FALSE);

  return (WARC_TRUE);
}


/**
 * @param wtfile The output Temporary Warc Record File
 *
 * @return none
 *
 * Warc Record Warc ID field writing function
 */

#define WARC_ID_LENGTH 28
WIPRIVATE void WFile_writeWarcId (FILE * wtfile)
{
/*   w_fwrite ("warc/0.9                    ", WARC_ID_LENGTH, 1, wtfile); */
  w_fwrite (WARC_VERSION "                    ", WARC_ID_LENGTH, 1, wtfile);
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param Type The record type as a const WRecordType
 * @param [out]datalength The Warc Record Data Length field to be updated
 *
 * @return none
 *
 * Warc Record Type Field writing function
 */

WPRIVATE void WFile_writeRecordType (FILE * wtfile, warc_rec_t type, 
                                      warc_u32_t  * datalength)
{
  switch(type)
    {
    case UNKNOWN_RECORD: 
      w_fwrite("unknown", 7, 1, wtfile);
      (* datalength) += 7;
      break;
     
    case WARCINFO_RECORD: 
      w_fwrite("warcinfo", 8, 1, wtfile);
      (* datalength) += 8; 
      break;

    case RESPONSE_RECORD: 
      w_fwrite("response", 8, 1, wtfile);
      (* datalength) += 8;
      break;
            
    case REQUEST_RECORD: 
      w_fwrite("request", 7, 1, wtfile);
      (* datalength) += 7;
      break;
            
    case METADATA_RECORD: 
      w_fwrite("metadata", 8, 1, wtfile);
      (* datalength) += 8;
      break;

    case REVISIT_RECORD: 
      w_fwrite("revisit", 7, 1, wtfile);
      (* datalength) += 7;
      break;
    
      case CONVERSION_RECORD: 
        w_fwrite("conversion", 10, 1, wtfile);
        (* datalength) += 10;
        break;
    case CONTINUATION_RECORD: 
      w_fwrite("continuation", 12, 1, wtfile);
      (* datalength) += 12;
      break;
            
    case RESOURCE_RECORD: 
      w_fwrite("resource", 8, 1, wtfile);
      (* datalength) += 8;
      break;
    }

  w_fwrite (TSPS, TSPSIZE, 1, wtfile);
  (* datalength) += TSPSIZE;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param suri The Warc Record Subject Uri as a const char *
 * @param [out]datalength: The Warc Record datalength value to be updated
 *
 * @return none
 *
 * Warc Record Subject Uri Field writing function
 */

WIPRIVATE void WFile_writeSubjectUri (FILE * wtfile, const char * suri,
                                      warc_u32_t s,
                                      warc_u32_t * datalength)
{
  w_fwrite (suri, s, 1, wtfile);
  (* datalength) += s;

  w_fwrite (TSPS, TSPSIZE, 1, wtfile);
  (* datalength) += TSPSIZE;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param cdate The Warc Record Creation Date as a const char * 
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Creation Date field writing function
 */

WIPRIVATE void WFile_writeCreationDate (FILE * wtfile, const char * cdate,
                                        warc_u32_t s,
                                        warc_u32_t * datalength)
{
  w_fwrite (cdate, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite (TSPS, TSPSIZE, 1, wtfile);
  (* datalength) += TSPSIZE;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param ctype The Warc Record Contenet Type as a const char*
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 *@return none
 *
 * Warc Record Contenet Type writing function
 */

WIPRIVATE void WFile_writeContentType (FILE * wtfile, const char * ctype,
                                       warc_u32_t s,
                                       warc_u32_t * datalength)
{
  w_fwrite (ctype, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite (TSPS, TSPSIZE, 1, wtfile);
  (* datalength) += TSPSIZE;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param rec_id The Warc Record Id as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Id Filed writing function
 */

WIPRIVATE void WFile_writeRecordId (FILE * wtfile,const char * rec_id,
                                    warc_u32_t s,
                                    warc_u32_t * datalength)
{
  w_fwrite (rec_id, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\x0D\x0A", 2, 1,wtfile);
  (* datalength) +=2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param lanvl list of anvl fields
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record anvl fields writing function
*/

WPRIVATE void WFile_writeAnvls (FILE * wtfile, const void * lanvl, 
                                warc_u32_t * datalength)
{
   const void * anvl; /* recover anvl fields from Warc Record */
   warc_u32_t size, i, s;

  if (lanvl)
    {
      i    = 0;
      size = WList_size (lanvl);
      while (i < size)
        {
          /* recovering an anvl field */
          anvl = WList_get (lanvl, i);
          
          /* writing anvl field */

          /* writing key */
          s = WAnvl_getKeyLen (anvl);
          w_fwrite (WAnvl_getKey (anvl), 
                  s, 1, wtfile);
          (* datalength) += s + 1;
          w_fwrite (":", 1, 1, wtfile);
          
          /* writing anvl value */
          s = WAnvl_getValueLen (anvl);
          w_fwrite (WAnvl_getValue (anvl), 
                  s, 1, wtfile);

          (* datalength) += s + 2;
          w_fwrite ("\x0D\x0A", 2, 1, wtfile);

          ++ i;
        }
    }

  w_fwrite ("\x0D\x0A", 2, 1,wtfile);
  (* datalength) += 2;
}

/**
 * @param wtfile The output Temporary Warc Record File
 * @param bloc The Data Bloc as a File descriptor
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Reord Data Bloc Writing function
 */


WPRIVATE warc_bool_t WFile_writeDataBloc (FILE * wtfile, FILE * bloc, 
                                          warc_u64_t size)
{
#define WFLUSH_SIZ 4096

  char          buf [WFLUSH_SIZ];
  warc_u32_t    r;
  warc_u32_t    p;
   
  /* Preconditions */
  assert (wtfile);
  assert (bloc);
  
  while (size)
    {
      if(size > WFLUSH_SIZ)
        r = WFLUSH_SIZ;
      else
        r = size;
      
      size = size - r;
      
      p = w_fread (buf, 1, r, bloc);
      if (w_ferror (wtfile) || r != p)
        {
          WarcDebugMsg("data fill temporary error");
          return (WARC_TRUE);
        }
      
      if (w_fwrite (buf, 1, r, wtfile) != r)
        return (WARC_TRUE);
    }
  

/*   char Byte; */

/*   while (size) */
/*     { */
/*       w_fread  (& Byte, 1, 1, bloc); */
/*       w_fwrite (& Byte, 1, 1, wtfile); */
/*       -- size; */
/*     } */

  return (WARC_FALSE);
}

/**
 * @param numericvalue The Value to convert
 * @param strvalue The converted value as a char *
 *
 * @return The Numerical Value as a string
 *
 * WFile number to string transformation function
 */

WPRIVATE char * WFile_numtoString (warc_u32_t numericvalue, 
                                   char * strvalue)
{
  warc_u32_t i;
  warc_u32_t quaut = 1;
  
  while ((numericvalue / quaut) != 0) 
   quaut *= 10;
  
  quaut /= 10;
  i      = 0;
  
  while (quaut != 0)
    {
      strvalue [i]= '0' + (numericvalue / quaut);
      numericvalue %= quaut;
      quaut /=10;
      i++;
   }
  
  strvalue [i] = '\0';
  
  return (strvalue);
}


/**
 * @param warcf the Warc File descriptor
 * @param wtfile The output Temporary Warc Record File
 *
 * @return none
 *
 * Warc Record Temporary File Flushing function into the Warc File
 */


WPRIVATE warc_bool_t WFile_flushTemporary (FILE * warcf, FILE * wtfile)
{
#define WFLUSH_SIZ 4096

  warc_u32_t size;
  char       buf [WFLUSH_SIZ];

  /* Preconditions */
  assert (warcf);
  assert (wtfile);

  w_fseek_start (wtfile);

  while (WARC_TRUE)
    {
      size = w_fread  (buf, 1, WFLUSH_SIZ, wtfile);

      if (w_ferror(wtfile))
        {
          WarcDebugMsg("data flush error");
          return (WARC_TRUE);
        }

      unless (size)
        break;
  
      if(w_fwrite (buf, 1, size, warcf) != size)
        return (WARC_TRUE);
    }

  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param wrec the Warc record to store
 * @param datalength the size of data to write
 * @param bloc the data file from where data are stored
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record storing in compressed Warc File
 */

WPRIVATE warc_bool_t 
WFile_storeRecordGzipComressed (void * _self,
                                const void * wrec, warc_u32_t datalength,
                                FILE * bloc, FILE * wtfile, void * objwtfile)
{
  struct WFile * self = _self;

  char           strData [20];
  warc_u32_t     i       = 0;
  void         * gzobj   = NIL;
  warc_u32_t     csize   = 0;    /* the size of the compressed data */
  FILE         * crec    = NIL;  /* to store the compressed record */
  void         * objcrec = NIL;
  warc_gzlevel_t level;

  /* map the correct compression level */
  switch (COMP)
    {
    case WARC_FILE_COMPRESSED_GZIP:
    case WARC_FILE_COMPRESSED_GZIP_DEFAULT_COMPRESSION:
      level = WARC_GZIP_DEFAULT_COMPRESSION;
      break;
    case WARC_FILE_COMPRESSED_GZIP_NO_COMPRESSION:
      level = WARC_GZIP_NO_COMPRESSION;
      break;
    case WARC_FILE_COMPRESSED_GZIP_BEST_SPEED:
      level = WARC_GZIP_BEST_SPEED;
      break;
    case WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION:
      level = WARC_GZIP_BEST_COMPRESSION;
      break;
    default:
      return (WARC_TRUE);
    }

  /* writing the data Length Field */
  w_fseek_from_start (wtfile, 10);
  WFile_numtoString (datalength, strData);
  w_fwrite (strData, strlen (strData), 1, wtfile);
  w_fseek_end (wtfile);
  
  /* writing Data Bloc */
  WFile_writeDataBloc (wtfile, bloc, WRecord_getDataSize (wrec));
  
  /* writing final double CRLF */
  i = 0x0D;
  w_fwrite (& i, 1, 1,wtfile);
  i = 0x0A;
  w_fwrite (& i, 1, 1,wtfile);
  i = 0x0D;
  w_fwrite (& i, 1, 1,wtfile);
  i = 0x0A;
  w_fwrite (& i, 1, 1,wtfile);
  
  /* Creating the temporary file that will hold the 
     compressed record */
  objcrec = bless (WTempFile);
  unless (objcrec)
    {
      WarcDebugMsg ("Unable to create temporary file for the compressed record\n");
      destroy (objwtfile), wtfile = NIL;
      return (WARC_TRUE);
    }
  crec = WTempFile_handle (objcrec);
  
  /* Compressing the  record */
  w_fseek_start (wtfile);
  gzobj = bless (WGzip);

  if (WGzip_compress (gzobj, wtfile, crec, level, & csize))
    {
      WarcDebugMsg ("failed in compressing the record\n");
      destroy (objwtfile);
      destroy (objcrec);
      destroy (gzobj);
      return (WARC_TRUE);
    }
  
  destroy (objwtfile), wtfile = NIL;
  destroy (gzobj), gzobj = NIL;
  
  /* testing if the record will not overload the Warc File */
  if ((MAXSIZE - w_ftell (FH)) < csize)
    {
      destroy (objcrec);
      WarcDebugMsg ("Couldn't add record to the warc file, maximum size reached\n");
      return (WARC_TRUE);
    }
  
  /* adding the compressed record */
  w_fseek_start (crec);
  WFile_writeDataBloc (FH, crec, csize);
  destroy (objcrec), crec = NIL;

  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param wrec the Warc record to store
 * @param datalength the size of data to write
 * @param bloc the data file from where data are stored
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record storing in compressed Warc File
 */

WPRIVATE warc_bool_t 
WFile_storeRecordUncompressed (void* _self, const void * wrec,
                               warc_u32_t datalength, FILE * bloc,
                               FILE * wtfile, void * objwtfile)
{
  struct WFile * self = _self;
  
  char           strData [20];
  warc_u32_t     i    = 0;

  /* testing if the record writing will not overload the Warc File */
  if ((datalength + 4) > (datalength) && /* avoid integer overflow */
      (MAXSIZE - w_ftell (FH)) < (datalength + 4))
    {
      destroy (objwtfile);
      WarcDebugMsg ("Couldn't add record to the warc file, maximum size reached\n");
      return (WARC_TRUE);
    }
  
  /* writing the data Length Field */
  w_fseek_from_start (wtfile, 10);
  WFile_numtoString (datalength, strData);
  w_fwrite (strData, strlen (strData), 1, wtfile);
  
  /* flushing the Warc Record Header Temporary file into 
     the Warc File */
  WFile_flushTemporary (FH, wtfile);
  
  /* writing Data Bloc */
  WFile_writeDataBloc (FH, bloc, WRecord_getDataSize (wrec));
  
  /* writing final double CRLF */
  i = 0x0D;
  w_fwrite (& i, 1, 1,FH);  
  i = 0x0A;
  w_fwrite (& i, 1, 1,FH);
  i = 0x0D;  
  w_fwrite (& i, 1, 1,FH);
  i = 0x0A;
  w_fwrite (& i, 1, 1,FH);
      
  /* destroying the temporary file */
  destroy (objwtfile);

  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param wrec The Warc Record to write
 *
 * @return False if succeeds, True otherwise;
 *
 * Warc Records storing funtion
 */

WPUBLIC warc_bool_t WFile_storeRecord (void* _self, const void * wrec)
{
  struct WFile * self = _self;
  
  FILE         * bloc;       /* to open the data bloc file */
  const void   * hdline;     /* recover Header line from the WRecord */
  const void   * lanvl;      /* recover the anvl list from the Warc Record */
  FILE         * wtfile     = NIL;   /* for creation of the temporary file */
  void         * objwtfile  = NIL;
  warc_u32_t     datalength = 0;

  /* Preconditions */

  CASSERT (self);
  
  unless (wrec)
    {
    return (WARC_TRUE);
    }
  
  /* reject any other mode except for writing */
  if (MODE != WARC_FILE_WRITER)
    {
      return (WARC_TRUE);
    }

  /* ensure that we're at the end of file */
  w_fseek_end(FH);

  bloc = WRecord_getDataFileExtern (wrec);
  unless (bloc)
    return (WARC_TRUE);
  
  unless (WRecord_check (wrec))
    {
      WarcDebugMsg ("some header line fields are missing.\n");
      return (WARC_TRUE);
    }
  
  w_fseek_end (bloc);
  if (WRecord_getDataSize (wrec) != w_ftell(bloc))
    return (WARC_TRUE);

  w_fseek_start (bloc);
  
  objwtfile = bless (WTempFile);
  unless (objwtfile)
    {
      WarcDebugMsg ("failure in trying creating File pointer on temporary file\n");
      return (WARC_TRUE);
    }
  wtfile = WTempFile_handle (objwtfile);
  
  hdline = WRecord_getHDLine (wrec);
  lanvl = WRecord_getAnvl (wrec);
  
  /* writing header line*/
  
  /* Writing warc_id */
  WFile_writeWarcId (wtfile);
  datalength += WARC_ID_LENGTH;
  
  /* writing record_type field */
  WFile_writeRecordType (wtfile, WHDLine_getRecordType (hdline),
                         & datalength);
  
  /* writing subject uri field */
  WFile_writeSubjectUri (wtfile, WHDLine_getSubjectUri (hdline),
                         WHDLine_getSubjectUriLen (hdline),
                         & datalength);
  
  /* writing creation date field */
  WFile_writeCreationDate (wtfile, WHDLine_getCreationDate (hdline),
                           WHDLine_getCreationDateLen (hdline),
                           & datalength);
  
  /* writing content type field */
  WFile_writeContentType (wtfile, WHDLine_getContentType (hdline),
                          WHDLine_getContentTypeLen (hdline),
                          & datalength);
  
  /*writing record id field */
  WFile_writeRecordId (wtfile, WHDLine_getRecordId (hdline),
                       WHDLine_getRecordIdLen (hdline),
                       & datalength);
  
  /* Writing the anvl fields */
  WFile_writeAnvls (wtfile, lanvl, & datalength);
  
  datalength = datalength + WRecord_getDataSize(wrec);
  
  switch (COMP)
    {
    case WARC_FILE_COMPRESSED_GZIP:
    case WARC_FILE_COMPRESSED_GZIP_DEFAULT_COMPRESSION:
    case WARC_FILE_COMPRESSED_GZIP_NO_COMPRESSION:
    case WARC_FILE_COMPRESSED_GZIP_BEST_SPEED:
    case WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION:
      return (WFile_storeRecordGzipComressed (self, wrec, datalength, bloc,
                                              wtfile, objwtfile));
    case WARC_FILE_UNCOMPRESSED:
      return (WFile_storeRecordUncompressed (self, wrec, datalength, bloc,
                                             wtfile, objwtfile));
    default:
      WarcDebugMsg ("WFile file opened with an unknown mode\n");
    }

  return (WARC_TRUE); 
}



/**
 * @param _self WFile object instance
 *
 * @return The Warc File descriptor if succeeds, NIL otherwise
 *
 * Warc File Desctriptor providing function
 */

WIPUBLIC FILE * WFile_getFile (const void * const _self)
{
    const struct WFile * const self = _self;
    CASSERT (self);

    return (FH);
}

/**
 * WFile_constructor - create a new WFile object instance
 *
 * @param _self WFile class object
 * @param app constructor list parameters
 *
 * @return a valid WFile object or NIL
 *
 * WARC constructor
 */

WPRIVATE void * WFile_constructor (void * _self, va_list * app)
{
  struct WFile            * const self = _self;
  const char              * fname      = va_arg (* app, const char *);
  const warc_u32_t          max_size   = va_arg (* app, const warc_u32_t);
  wfile_mode_t              mode       = va_arg (* app, wfile_mode_t);
  const wfile_comp_t        compressed = va_arg (* app, const warc_bool_t);
  
  FNAME = bless (WString, fname, strlen(fname));
  assert (FNAME);

  COMP    = compressed;
  MODE    = mode;

  MAXSIZE = max_size;
  unless (MAXSIZE)
    {
      destroy (self);
      return (NIL);
    }

  if (MODE == WARC_FILE_READER) /* reading from a WARC file */
    {
      FH = w_fopen_rb (fname);
      unless (FH) 
        {
          destroy (self);
          return NIL;
        }

      w_file_size(FH, FSIZE);
      unless (FSIZE)
        {
          destroy (self);
          return (NIL);
        }

      /* uncompression levels meaning nothing when reading */
      switch ((warc_u32_t) (COMP))
        {
        case WARC_FILE_COMPRESSED_GZIP_DEFAULT_COMPRESSION:
        case WARC_FILE_COMPRESSED_GZIP_NO_COMPRESSION:
        case WARC_FILE_COMPRESSED_GZIP_BEST_SPEED:
        case WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION:
          COMP = WARC_FILE_COMPRESSED_GZIP;
          break;
        }

      /* check if it's a valid GZIP file */
      if (COMP == WARC_FILE_COMPRESSED_GZIP)
        {
          warc_u64_t   where = 0;
          void       * g     = bless (WGzip);

          unless (g)
            {
              destroy (self);
              return (NIL);
            }
          
          where = (warc_u64_t) w_ftell (FH);
          if (WGzip_check (g, FH, 0))
            {
              w_fprintf (fprintf (stderr, "not a valid GZIP WARC file\n"));
              destroy (g);
              destroy (self);
              return (NIL); 
            }
          w_fseek_from_start(FH, where);
          destroy (g);
        }

    }
  else if (MODE == WARC_FILE_WRITER) /* writing to a WARC file */
    {
      FH = w_fopen_wb (fname);
      unless (FH) 
        {
          destroy (self);
          return NIL;
        }
      w_fseek_end (FH);
      FSIZE = (warc_u64_t) w_ftell (FH);

      if (FSIZE >= MAXSIZE)
        {
          w_fprintf (fprintf (stderr ,"WARC file maximum size reached\n"));
          destroy (self);
          return NIL;
        }
    }
  else 
    {
      destroy (self);
      return (NIL);
    }
  

  return (self);
}

/**
 * @param _self WFile object instance
 *
 * WARC destructor
 */

WPRIVATE void * WFile_destructor (void * _self)
{	
  struct WFile  * const self = _self;


  /* preconditions */
  CASSERT (self);

  /* free the file handle */
  if (FH)
    w_fclose (FH);
  
  if (FNAME)
    destroy (FNAME), FNAME = NIL;

  MODE    = WARC_FILE_INVALID;
  COMP    = WARC_FALSE;
  MAXSIZE = 0;
  FSIZE   = 0;

  return (self);
}



/**
 * WARC WFile class
 */

static const struct Class _WFile = {
	sizeof(struct WFile),
    SIGN,
	WFile_constructor, WFile_destructor
};

const void * WFile = & _WFile;
