
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

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <warc.h>

#include <afile.h>
#include <arecord.h>


#define WARC_MAX_SIZE 629145600
#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))


int test1 (void)
{
  const char  * t = "TEST 1: converting an uncompressed  Arc which contain a single record File to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/file.arc", WARC_FILE_UNCOMPRESSED);
  void  * w = bless(WFile, "./app/wdata/arc2warc/file.warc",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
 
 

  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);

 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("Ip"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);

 return (0); 
}


         
int test2 (void)
{	

  const char * t = "TEST2: reading an automatically created warc file from arc file";
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */

  fprintf (stdout, "%s>\n", t);

   w = bless (WFile, "./app/wdata/arc2warc/file.warc", 660, WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);
   assert (w);

   while (WFile_hasMoreRecords (w))
      {
        unless ((r = WFile_nextRecord (w)))
           {
            destroy (w);
            return 1;
           }

        fprintf (stdout,"============================================================================\n");
        /*print WHDLine object for this WRecord */

        fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
        fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
        fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
        fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
        fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
        fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
        fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


        fprintf (stdout,"-----------------------------------------------------------------------\n");
        /* Picking anvl field value w.r.t the given keys */

        fprintf (stdout, "Value of anvl having key: IpAddress = %s\n", 
                 WRecord_getAnvlValue (r, (warc_u8_t *) "IpAddress"));

        fprintf (stdout, "\n\n");

        destroy (r);
     }

  destroy (w);
  return (0);
}

int test3(void)
{
 const char  * t = "TEST 3: converting a uncomprssed Arc File Which contains several records to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/mrec.arc", WARC_FILE_UNCOMPRESSED);
  void  * w = bless(WFile, "./app/wdata/arc2warc/mrec.warc",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);




  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);

 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("Ip"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);
 return (0);
 
}


 

int test4 (void)
{	

  const char  * t = "TEST4: reading an automatically created warc file from arc file";
  void  * w =  NIL; /* warc file object */
  void  * r = NIL;  /* to recover records */

  fprintf (stdout, "%s>\n", t);

  w = bless (WFile, "./app/wdata/arc2warc/mrec.warc", WARC_MAX_SIZE, WARC_FILE_READER,WARC_FILE_UNCOMPRESSED);
  assert (w);

  while (WFile_hasMoreRecords (w))
       {
       unless ((r = WFile_nextRecord (w)))
             {
             destroy (w);
              return 1;
              }

        fprintf (stdout,"===========================================================================\n");
         /*print WHDLine object for this WRecord */

        fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
        fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
        fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
        fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
        fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
        fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
        fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


        fprintf (stdout,"-----------------------------------------------------------------------\n");
      /* Picking anvl field value w.r.t the given keys */

        fprintf (stdout, "Value of anvl having key: IpAddress = %s\n", 
                 WRecord_getAnvlValue (r, (warc_u8_t *) "IpAddress"));

        fprintf (stdout, "\n\n");

        destroy (r);
       }

  destroy (w);
  return (0);
}


int test5(void)
{
  const char  * t = "TEST 5: converting an Arc File Which contains several records to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/test1.arc", WARC_FILE_UNCOMPRESSED);
  void  * w = bless(WFile, "./app/wdata/arc2warc/test1.warc",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);


 


  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);

 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("IpAddress"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);
 return (0); 
}


 
int test6 (void)
{	

  const char * t = "TEST 6: reading a warc file created automatically from a arc file  ";
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */

  fprintf (stdout, "%s>\n", t);

  w = bless (WFile, "./app/wdata/arc2warc/test1.warc", WARC_MAX_SIZE, WARC_FILE_READER,WARC_FILE_UNCOMPRESSED);
  assert (w);

  while (WFile_hasMoreRecords (w))
     {
     unless ((r = WFile_nextRecord (w)))
           {
            destroy (w);
            return (1);
            }

      fprintf (stdout,"==============================================================================\n");
       /*print WHDLine object for this WRecord */

      fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
      fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
      fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
      fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
      fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
      fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
      fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


     fprintf (stdout,"-----------------------------------------------------------------------\n");
      /* Picking anvl field value w.r.t the given keys */

      fprintf (stdout,"Value of anvl having key: IpAddress = %s\n",
               WRecord_getAnvlValue (r, (warc_u8_t *) "IpAddress"));

      fprintf (stdout, "\n\n");

     destroy (r);
     }

  destroy (w);
  return (0);
}



int test7 (void)
{
  const char  * t = "TEST 7: converting a corrupted  Arc File in the second record to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/err1.arc", WARC_FILE_UNCOMPRESSED);
  void  * w = bless(WFile, "./app/wdata/arc2warc/err1.warc",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);


  
  

  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);

 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("IpAddress"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);

 return (0); 
}


  


int test8 (void)
{
  const char  * t = "TEST 8: converting a corrupted Arc File in the first to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/err2.arc", WARC_FILE_UNCOMPRESSED);
  void  * w = bless(WFile, "./app/wdata/arc2warc/err2.warc",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
 


  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);

 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("IpAddress"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);
 return (0); 
}

 



   
int test9 (void)
{
  const char  * t = "TEST 9: converting a  Arc File wich contains several records to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/test2.arc", WARC_FILE_UNCOMPRESSED);
  void  * w = bless(WFile, "./app/wdata/arc2warc/test2.warc",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
 



  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);
  
 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("IpAddress"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);
 return (0); 
}

 
  
int test10 (void)
{	

  const char * t = "TEST10: reading an automatically created warc file from arc file";
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */

  fprintf (stdout, "%s>\n", t);

  w = bless (WFile, "./app/wdata/arc2warc/test2.warc", 660, WARC_FILE_READER,WARC_FILE_UNCOMPRESSED);
  assert (w);

  while (WFile_hasMoreRecords (w))
     {
     unless ((r = WFile_nextRecord (w)))
           {
           destroy (w);
           return (1);
           }

      destroy (r);
     }
  
  fprintf(stdout,"success\n");
  destroy (w);
  return (0);
}

int test11 (void)
{
  const char  * t = "TEST 11: converting a compressed Arc File wich contains a single record to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/file.arc.gz", WARC_FILE_COMPRESSED_GZIP);
  void  * w = bless(WFile, "./app/wdata/arc2warc/file.warc.gz",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP);
 



  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);
  
 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("IpAddress"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       {
       fprintf (stdout, "failed to write the WRecord\n");
       destroy (a);
       destroy (ar);
       destroy (w);
       destroy (wr);
       return (1);
       }
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);
 return (0); 
}

int test12 (void)
{	

  const char * t = "TEST12: reading an automatically created warc file from arc file";
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */

  fprintf (stdout, "%s>\n", t);

   w = bless (WFile, "./app/wdata/arc2warc/file.warc.gz", 660, WARC_FILE_READER, WARC_FILE_COMPRESSED_GZIP);
   assert (w);

   while (WFile_hasMoreRecords (w))
      {
        unless ((r = WFile_nextRecord (w)))
           {
            destroy (w);
            return 1;
           }

        fprintf (stdout,"==============================================================================\n");
        /*print WHDLine object for this WRecord */

        fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
        fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
        fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
        fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
        fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
        fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
        fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


        fprintf (stdout,"-----------------------------------------------------------------------\n");
        /* Picking anvl field value w.r.t the given keys */

        fprintf (stdout, "Value of anvl having key: IpAddress = %s\n", 
                 WRecord_getAnvlValue (r, (warc_u8_t *) "IpAddress"));

        fprintf (stdout, "\n\n");

        destroy (r);
     }

  destroy (w);
  return (0);
}


int test13 (void)
{
  const char  * t = "TEST 13: converting a compressed Arc File wich contains several record to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/sfile.arc.gz", WARC_FILE_COMPRESSED_GZIP);
  void  * w = bless(WFile, "./app/wdata/arc2warc/sfile.warc.gz",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP);
 


  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);
  
 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("IpAddress"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);
 return (0); 
}

int test14 (void)
{	

  const char * t = "TEST14: reading an automatically created warc file from arc file";
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */

  fprintf (stdout, "%s>\n", t);

   w = bless (WFile, "./app/wdata/arc2warc/sfile.warc.gz", 660, WARC_FILE_READER, WARC_FILE_COMPRESSED_GZIP);
   assert (w);

   while (WFile_hasMoreRecords (w))
      {
        unless ((r = WFile_nextRecord (w)))
           {
            destroy (w);
            return 1;
           }

        fprintf (stdout,"==============================================================================\n");
        /*print WHDLine object for this WRecord */

        fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
        fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
        fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
        fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
        fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
        fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
        fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


        fprintf (stdout,"-----------------------------------------------------------------------\n");
        /* Picking anvl field value w.r.t the given keys */

        fprintf (stdout, "Value of anvl having key: IpAddress = %s\n", 
                 WRecord_getAnvlValue (r, (warc_u8_t *) "IpAddress"));

        fprintf (stdout, "\n\n");

        destroy (r);
     }

  destroy (w);
  return (0);
}

int test15 (void)
{
  const char  * t = "TEST 15: converting a corrupted Arc File at the second record to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/mfile.arc.gz", WARC_FILE_COMPRESSED_GZIP);
  void  * w = bless(WFile, "./app/wdata/arc2warc/mfile.warc.gz",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP);
 


  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);
  
 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));
     
     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("IpAddress"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }
     

     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);
 

 return (0); 
}

int test16 (void)
{	

  const char * t = "TEST16: reading an automatically created warc file from arc file";
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */

  fprintf (stdout, "%s>\n", t);

   w = bless (WFile, "./app/wdata/arc2warc/mfile.warc.gz", 660, WARC_FILE_READER, WARC_FILE_COMPRESSED_GZIP);
   assert (w);

   while (WFile_hasMoreRecords (w))
      {
        unless ((r = WFile_nextRecord (w)))
           {
            destroy (w);
            return 1;
           }

        fprintf (stdout,"==============================================================================\n");
        /*print WHDLine object for this WRecord */

        fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
        fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
        fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
        fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
        fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
        fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
        fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


        fprintf (stdout,"-----------------------------------------------------------------------\n");
        /* Picking anvl field value w.r.t the given keys */

        fprintf (stdout, "Value of anvl having key: IpAddress = %s\n", 
                 WRecord_getAnvlValue (r, (warc_u8_t *) "IpAddress"));

        fprintf (stdout, "\n\n");

        destroy (r);
     }

  destroy (w);
  return (0);
}


int test17 (void)
{
  const char  * t = "TEST 17: converting a corrupted Arc File in the first record to the Warc format";

  void  * a = bless (AFile, "./app/wdata/arc2warc/mmfile.arc.gz", WARC_FILE_COMPRESSED_GZIP);
  void  * w = bless(WFile, "./app/wdata/arc2warc/mmfile.warc.gz",
           WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP);
 


  fprintf (stdout, "%s\n", t);

  assert (a);
  assert (w);
  
 while (AFile_hasMoreRecords (a))
   {
     void * ar   = AFile_nextRecord (a);
     void * wr   = NIL; 


     unless (ar)
       {
         fprintf(stderr,"unable to retrieve the ArcRecord object\n");
         destroy (w);
         destroy (a);
         return (5);
       }

     wr = bless (WRecord);
     unless (wr)
       {
         fprintf(stderr,"unable to create the WRecord object\n");
         destroy (w);
         destroy (a);
         destroy (ar);
         return (6);
       }
     
 
     WRecord_setSubjectUri (wr, makeS (ARecord_getUrl (ar)));

     WRecord_setRecordType (wr, RESOURCE_RECORD);
     
     WRecord_setCreationDate (wr, makeS (ARecord_getCreationDate (ar)));
     
     WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar)));
    
     WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar)));
    
     WRecord_addAnvl (wr, makeS ("IpAddress"), makeS (ARecord_getIpAddress (ar)));
     

     if (ARecord_transferContent (ar, wr, a))
        {
        fprintf (stderr,
                  "Unable to pass content to the Warc Record\n");
        destroy (w);
        destroy (a);
        destroy (wr);
        destroy (ar);
        return  (7);
        }


     if (WFile_storeRecord (w, wr))
       fprintf (stdout, "failed to write the WRecord\n");
     
     destroy (wr);
     destroy (ar);

   } /* end of while */
 

 
 destroy (w);
 destroy (a);

 return (0); 
}




int main (void)
{	
  int (* tests [] ) () = { test1, test2, test3, test4, test5, test6, test7, test8, test11, test12, test13, test14, test15, test16, test17  };


  warc_u32_t  i      = 0;

  for(i = 0; i < ARRAY_LEN (tests)      ; ++ i)
    {
      tests[i] ();
    }

  return 0;
}


