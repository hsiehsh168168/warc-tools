
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

#include <stdio.h>
#include <string.h>
#include <assert.h>


#include <warc.h>

#define makeS(s) (s), strlen((s))
#define makeU(s) (const unsigned char *) (s), (warc_u64_t) strlen((s))


warc_bool_t callback (void * env, const char* buff, const warc_u32_t size)
{ 
  UNUSED(env);

  if (size)
    {
      warc_u32_t  i = 0;

      while (i < size) 
        {
          fprintf (stdout, "%c", buff[i]);
          ++i;
        }

      fprintf (stdout, "\n");
    }

  return (WARC_TRUE); /* return WARC_FALSE to stop extraction */
}


int test1 (void)
{
  const char * t = "TEST 1: creating Warc File \"./app/wdata/testwfile/awanvl.warc\" \
                    with one record an no anvl field";

  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./app/wdata/testwfile/awanvl.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
  void * u = NIL;

  fprintf (stdout, "%s\n", t);

  if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r,makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  u = bless (WUUID);
  WUUID_hash (u,makeU("record1://anunknownplace"));

  if (WRecord_setRecordId (r, makeS (WUUID_text(u))))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
          destroy (w);
          destroy (u);
          return 1;
        }
   destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
    {
      fprintf (stdout, "record not filled\n");
      destroy (r);
      destroy (w);
      return 1;
    }

  if (WFile_storeRecord (w, r))
    {
      fprintf (stdout, "record not stored\n");
      destroy (r);
      destroy (w); 
      return 1;
    }

  fprintf (stdout, "OK\n");
  destroy (r); 
  destroy (w);
  
  return 0;
}


int test2 (void)
{
  const char * t = "TEST 2: creating a Warc File \"/app/wdata/awarcavl.warc\" \
                    with one record containing one anvl field";

  const char * file1 = "./app/wdata/testwfile/unidesc.html";
 
  void * r = bless (WRecord);
  void * w = bless (WFile, "./app/wdata/testwfile/awarcavl.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
  void * u = NIL;

  fprintf  (stdout, "%s\n", t);
   
  if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r,makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  u = bless (WUUID);
  WUUID_hash (u,makeU("record1://anunknownplace"));

  if (WRecord_setRecordId (r, makeS (WUUID_text(u))))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
          destroy (w);
          destroy (u);
          return 1;
        }
   destroy (u);


  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if  (WRecord_addAnvl(r, "key1", 4, "val1", 4))
     {
       fprintf (stdout,"Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return 1;
     }

  if (WRecord_setContentFromFileName (r, file1))
    {
      fprintf (stdout, "record not filled\n");
      destroy (r);
      destroy (w);
      return 1;
    }

  if (WFile_storeRecord (w, r))
    {
      fprintf (stdout, "record not stored\n");
      destroy (r);
      destroy (w); 
      return 1;
    }

  fprintf (stdout, "OK\n");
  destroy (r); 
  destroy (w);
  
  return 0;
} 
int test3 (void)
{
  const char * t = "TEST 3: Creating a Warc File \"/app/wdata/awarcmlp.warc\" with two Records";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
  const char * file2 = "./app/wdata/testwfile/anvlcom";

  void * r = bless (WRecord);
  void * w = bless (WFile,"./app/wdata/testwfile/awarcmlp.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
  void * u = NIL;

  fprintf (stdout,"%s\n",t);

   if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  u = bless (WUUID);
    WUUID_hash (u,makeU("record1://anunknownplace"));

  if (WRecord_setRecordId (r, makeS (WUUID_text(u))))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
          destroy (u);
          return 1;
        }
   destroy (u);


  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
       {
         fprintf (stdout, "record not filled\n");
         destroy (r);
         destroy (w);
         return 1;
       }

  if (WFile_storeRecord (w, r))
      {
        fprintf (stdout, "record not stored\n");
        destroy (r);
        destroy (w); 
        return 1;
      }

  fprintf (stdout, "OK\n");
  destroy (r);

   r = bless (WRecord);  

  if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
        }

  if (WRecord_setRecordId (r, makeS ("record1://anunknownplace")))
        {
         fprintf (stdout, "Corrupted record ID\n");
         destroy (r);
         destroy (w);
         return 1;
        }


  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file2))
       {
         fprintf (stdout, "record not filled\n");
         destroy (r);
         destroy (w);
         return 1;
       }

  if (WFile_storeRecord (w, r))
    {
      fprintf (stdout, "record not stored\n");
      destroy (r);
      destroy (w); 
      return 1;
    }

  fprintf (stdout, "OK\n");
  destroy (r);

  destroy (w);
  
  return 0;
} 

/* writing the firs rercord */
  

/*int test4 (void)
{
  const char * t = "TEST 4: Trying to create a Warc File with a Warc record \
                    having a corrupted Subject Uri field";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
 
  void * r = bless (WRecord);
  void * w = bless (WFile, "./app/wdata/testwfile/awafgri.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);

  fprintf (stdout,"%s\n",t);
  
  if (WRecord_setSubjectUri (r, makeS ("test:anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  if (WRecord_setRecordId (r, makeS ("record1://anunknownplace")))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
         destroy (w);
         return 1;
        }


  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
       {
         fprintf (stdout, "record not filled\n");
         destroy (r);
         destroy (w);
         return 1;
       }

  if (WFile_storeRecord (w, r))
    {
      fprintf (stdout, "record not stored\n");
      destroy (r);
      destroy (w); 
      return 1;
    }

  fprintf (stdout, "OK\n");
  destroy (r);
  destroy (w);
  
  return 0;
} 

int test5 (void)
{
  const char * t = "TEST 5: Trying to create a Warc File with a Warc record \
                     having a corrupted Content Type field";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
 
  void * r = bless (WRecord);
  void * w = bless (WFile, "./app/wdata/testwfile/awfgid.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);

  fprintf (stdout,"%s\n",t);
  
   if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
        }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  if (WRecord_setRecordId (r, makeS ("record1://anunknownplace")))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
          destroy (w);
          return 1;
        }


  if  (WRecord_setContentType (r, makeS ("Text|random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
       {
         fprintf (stdout, "record not filled\n");
         destroy (r);
         destroy (w);
         return 1;
       }

  if (WFile_storeRecord (w, r))
      {
        fprintf (stdout, "record not stored\n");
        destroy (r);
        destroy (w); 
        return 1;
      }

  fprintf (stdout, "OK\n");
  destroy (r);
  destroy (w);
  
  return 0;
} 
*/
int test6 (void)
{
  const char * t = "TEST 6:  Trying to create a Warc File with a Warc record \
                    having a corrupted Creation Date field";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
 
  void * r = bless (WRecord);
  void * w = bless (WFile, "./app/wdata/testwfile/awarfdt.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
  void * u = NIL;

  fprintf (stdout,"%s\n",t);

   if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008pe123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return  1;
       }

  u = bless (WUUID);
  WUUID_hash (u,makeU("record1://anunknownplace"));

  if (WRecord_setRecordId (r, makeS (WUUID_text(u))))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
          destroy (w);
          destroy (u);
          return 1;
        }
   destroy (u);


  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
       {
         fprintf (stdout,"record not filled\n");
         destroy (r);
         destroy (w);
         return 1;
       }

  if (WFile_storeRecord (w, r))
      {
         fprintf (stdout, "record not stored\n");
         destroy (r);
         destroy (w); 
         return 1;
      }

  fprintf (stdout, "OK\n");
  destroy (r);
  destroy (w);
  
  return 0;
} 


int test7 (void)
{
  const char * t = "TEST 7: Trying to create a Warc File with a Warc record \
                     having a corrupted Record Type field";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
 
  void * r = bless (WRecord);
  void * w = bless (WFile,"./app/wdata/testwfile/awarcfdt.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
  void * u = NIL;

  fprintf (stdout,"%s\n",t);

   if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, 11))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  u = bless (WUUID);
  WUUID_hash (u,makeU("record1://anunknownplace"));

  if (WRecord_setRecordId (r, makeS (WUUID_text(u))))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
         destroy (w);
          destroy (u);
          return 1;
        }
   destroy (u);


  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
   {
    fprintf (stdout, "record not filled\n");
    destroy (r);
    destroy (w);
    return 1;
   }

  if (WFile_storeRecord (w, r))
    {
      fprintf (stdout,"record not stored\n");
      destroy (r);
      destroy (w); 
      return 1;
    }

  fprintf (stdout, "OK\n");
  destroy (r);
  destroy (w);
  
  return 0;
} 

int test8 (void)
{
  const char * t = "TEST 8: Trying to create a Warc File with a Warc record \
                     having a corrupted anvl field";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
 
  void * r = bless (WRecord);
  void * w = bless (WFile, "./app/wdata/testwfile/awarfldt.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
  void * u = NIL;

  fprintf (stdout,"%s\n",t);



   if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  u = bless (WUUID);
  WUUID_hash (u,makeU("record1://anunknownplace"));

  if (WRecord_setRecordId (r, makeS (WUUID_text(u))))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
          destroy (w);
          destroy (u);
          return 1;
        }
   destroy (u);


  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
          fprintf (stdout, "Corrupted content type\n");
          destroy (r); 
          destroy (w); 
          return 1;
        }
   if  (WRecord_addAnvl(r, "key 2", 5, "val1", 4))
       {
         fprintf (stdout, "Corrupted anvl\n");
         destroy (r);
         destroy (w);
         return 1;
       }
  
  if (WRecord_setContentFromFileName (r, file1))
      {
         fprintf (stdout, "record not filled\n");
         destroy (r);
         destroy (w);
         return 1;
      }

  if (WFile_storeRecord (w, r))
     {
         fprintf (stdout,"record not stored\n");
         destroy (r);
         destroy (w); 
         return 1;
     }

  fprintf (stdout, "OK\n");
  destroy (r);
  destroy (w);
  
  return 0;
} 


int test9 (void)
{
  const char * t = "TEST 9: Trying to create a Warc File with a \
                    Warc record not having all fields";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
 
  void * r = bless (WRecord);
  void * w = bless (WFile,"./app/wdata/testwfile/awasdat.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);
  void * u = NIL;

  fprintf (stdout,"%s\n",t);
   if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }


  u = bless (WUUID);
  WUUID_hash (u,makeU("record1://anunknownplace"));

  if (WRecord_setRecordId (r, makeS (WUUID_text(u))))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
          destroy (w);
          destroy (u);
          return 1;
        }
   destroy (u);


  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
       {
        fprintf (stdout,"record not filled\n");
        destroy (r);
        destroy (w);
        return 1;
       }

  if (WFile_storeRecord (w, r))
       {
        fprintf (stdout,"record not stored\n");
        destroy (r);
        destroy (w); 
        return 1;
       }

  fprintf (stdout, "OK\n");
  destroy (r);
  destroy (w);
  
  return 0;
} 

 
int test10 (void)
{
  const char * t = "TEST 10: An other Try to create a Warc File with a \
                    Warc record not having all fields";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
 
  void * r = bless (WRecord);
  void * w = bless (WFile,"./app/wdata/testwfile/awauri.warc", 600 * 1024 * 1024,
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED);


  fprintf (stdout, "%s\n",t);
 
  if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  

  if  (WRecord_setContentType (r, makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
        {
          fprintf (stdout, "record not filled\n");
          destroy (r);
          destroy (w);
          return 1;
        }

  if (WFile_storeRecord (w, r))
        {
          fprintf (stdout,"record not stored\n");
          destroy (r);
          destroy (w); 
          return 1;
        }

  fprintf (stdout, "OK\n");
  destroy (r);
  destroy (w);
  
  return 0;
} 


 

int test11 (void)
{	

const char * t = "TEST 11: reding a warc file created manually";
void * w =  NIL;  /*warc file object */
void * r = NIL;  /*to recover records */
char  env[20];

  fprintf (stdout, "\n%s>\n", t);

w = bless (WFile, "./app/wdata/testwfile/warcfile.warc", 600 * 1024 * 1024,
           WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);
unless (w)
  return 1;

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


     fprintf (stdout,"--------------------------------------------------------------------------\n");
 
      fprintf (stdout, "\n\n");

     WFile_register (w, r, callback, (void *) env);
     WRecord_getContent (r);

     destroy (r);
     }

  destroy (w);
  return (0);
}

int test12 (void)
{	

const char * t = "TEST 12: reading a Warc file created with a warc writer (see above)";
void * w =  NIL;  /*warc file object */
void * r = NIL;  /*to recover records */



char env [20];
  fprintf (stdout, "\n%s>\n", t);

w = bless (WFile,"./app/wdata/testwfile/awarcavl.warc",
           660 * 1024 * 1024, WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);
 assert (w);

 while (WFile_hasMoreRecords (w))
   {
     r = WFile_nextRecord (w);
     unless (r)
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
 
     fprintf (stdout, "\n\n");

     WFile_register (w, r, callback, (void *) env);
     WRecord_getContent (r);

     destroy (r);
     }

  destroy (w);
  return (0);
}


/* Warc File records extraction on working case */
int test13 (void)
{	

const char * t = "TEST13: reding an automatically created warc file with showing anvl field value having key: key1";
void * w =  NIL; /* warc file object */
void * r = NIL;  /* to recover records */

  fprintf (stdout, "%s>\n", t);

w = bless (WFile, "./app/wdata/testwfile/awarcmlp.warc", 660 * 1024 * 1024,
           WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);
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


     fprintf (stdout,"--------------------------------------------------------------------------\n");
      /* Picking anvl field value w.r.t the given keys */

      fprintf (stdout,"Value of anvl having key: key1 = %s\n",WRecord_getAnvlValue (r,"key1"));

      fprintf (stdout, "\n\n");

      WFile_register (w,r,callback, (void *) 0);
      WRecord_getContent(r);

      destroy (r);
     }

  destroy (w);
  return (0);
}


/* Warc File records extraction on crashing case */
int test14 (void)
{	

const char * t = "TEST14:  reading a corrupted warc file";
void * w =  NIL; /* warc file object */
void * r = NIL;  /* to recover records */


  fprintf (stdout, "%s>\n", t);

w = bless (WFile, "./app/wdata/testwfile/ero1.warc", 300 * 1024 * 1024,
           WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);

  unless (w)
        return 1;


while (WFile_hasMoreRecords (w))
     {
     unless ((r = WFile_nextRecord (w)))
           {
	    destroy (w);
            return 1;
	  }

      fprintf (stdout,"==============================================================================\n");
      /* print WHDLine object for this WRecord */
      fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
      fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
      fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
      fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
      fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
      fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
      fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


     fprintf (stdout,"--------------------------------------------------------------------------\n");

     fprintf (stdout, "\n\n");

     destroy (r);
     }

  destroy (w);
  return (0);
}

int test15 (void)
{	

const char * t = "TEST15: reading another corrupted warc file";
void * w =  NIL; /* warc file object */
void * r = NIL;  /* to recover records */


  fprintf (stdout, "%s>\n", t);

w = bless (WFile, "./app/wdata/testwfile/ero2.warc", 300 * 1024 * 1024,
           WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);

  unless (w)
        return 1;


while (WFile_hasMoreRecords (w))
     {
     unless ((r = WFile_nextRecord (w)))
           {
	    destroy (w);
            return 1;
	   }

      fprintf (stdout,"==============================================================================\n");
      /* print WHDLine object for this WRecord */
      fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
      fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
      fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
      fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
      fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
      fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
      fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


     fprintf (stdout,"--------------------------------------------------------------------------\n");

     fprintf (stdout, "\n\n");

     destroy (r);
     }

  destroy (w);
  return (0);
}


int test16 (void)
{	

const char * t = "TEST16: reading a third corrupted warc file";
void * w =  NIL; /* warc file object */
void * r = NIL;  /* to recover records */


  fprintf (stdout, "%s>\n", t);

w = bless (WFile, "./app/wdata/testwfile/ero3.warc", 300 * 1024 * 1024,
           WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);

  unless (w)
        return 1;


while (WFile_hasMoreRecords (w))
     {
     unless ((r = WFile_nextRecord (w)))
           {
	    destroy (w);
            return 1;
	   }

      fprintf (stdout,"==============================================================================\n");
      /* print WHDLine object for this WRecord */
      fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r));
      fprintf (stdout, "DataLength: %-20d\n",   WRecord_getDataLength  (r));
      fprintf (stdout, "RecordType: %-20d\n",   WRecord_getRecordType  (r));
      fprintf (stdout, "SubjectUri: %-20s\n",   WRecord_getSubjectUri  (r));
      fprintf (stdout, "CreationDate: %-20s\n", WRecord_getCreationDate(r));
      fprintf (stdout, "ContentType: %-20s\n",  WRecord_getContentType (r));
      fprintf (stdout, "RecordId: %-20s\n",     WRecord_getRecordId    (r));


     fprintf (stdout,"--------------------------------------------------------------------------\n");

     fprintf (stdout, "\n\n");

     destroy (r);
     }

  destroy (w);
  return (0);
}




int test17 (void)
{	

const char * t = "TEST 17: trying to give another record  the read one in the registration phase";
void * w =  NIL;  /* warc file object */
void * r =  NIL;  /* to recover records */
void * r2 = NIL; /* the false record */


char  env[20];
  fprintf (stdout, "\n%s>\n", t);

w = bless (WFile, "./app/wdata/testwfile/warcfile.warc", 660 * 1024 * 1024,
           WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);

unless (w)
  return 1;

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


     fprintf (stdout,"--------------------------------------------------------------------------\n");
 
      fprintf (stdout, "\n\n");

     destroy (r);
     r2 = bless (WRecord);

     if (WFile_register (w, r2, callback, (void *) env))
        {
        WarcDebugMsg ("Unable to register the record\n");
        destroy (w);
        destroy (r2);
        return 1;
        }

     WRecord_getContent (r2);
     destroy (r2);
     }
  destroy (w);
  return (0);
}


int test18 (void)
{	

const char * t = "TEST 18: trying to give to register the record in another WFile object";
void * w =  NIL;  /*warc file object */
void * r = NIL;  /*to recover records */
 void * w2 = NIL; /* the false wfile object */ 
  char  env[20]; 

 fprintf (stdout, "\n%s>\n", t);

 w = bless (WFile,"./app/wdata/testwfile/warcfile.warc", 660 * 1024 * 1024,
            WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);

unless (w)
  return 1;

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
       fprintf (stdout,"--------------------------------------------------------------------------\n");

       fprintf (stdout, "\n\n");
      
        
       
        w2 = bless (WFile, "./app/wdata/testwfile/file.warc", 660 * 1024 * 1024,  
                    WARC_FILE_READER, WARC_FILE_UNCOMPRESSED); 
       
        if (WFile_register (w2, r, callback, (void *) env)) 
          { 
            WarcDebugMsg ("Unable to register the record\n"); 
            destroy (w2); 
            destroy (r);
            destroy (w);
            return 1; 
          } 

       WRecord_getContent (r);

       destroy (r);
     }
  
 destroy (w2); 
 destroy (w);
 return (0);
}




int test19 (void)
{
  const char * t = "TEST 19: Creating a compressed Warc File \"./app/wdata/testwfile/cwarc.warc.gz\" with two Records";
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
  const char * file2 = "./app/wdata/testwfile/anvlcom";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./app/wdata/testwfile/cwarc.warc.gz", 600 * 1024 * 1024, 
                    WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP_BEST_SPEED);

  fprintf (stdout,"%s\n",t);

   if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r); 
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r, WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r, makeS ("01052008123041")))
        {
          fprintf (stdout,"Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
        }

  if (WRecord_setRecordId (r, makeS ("record1://anunknownplace")))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
          destroy (w);
          return 1;
         }


  if  (WRecord_setContentType(r,makeS ("Text/random")))
        {
         fprintf (stdout, "Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file1))
    {
      fprintf (stdout, "record not filled\n");
      destroy (r);
      destroy (w);
      return 1;
    }

  if (WFile_storeRecord (w,r))
    {
      fprintf (stdout, "record not stored\n");
      destroy (r);
      destroy (w); 
      return 1;
    }

/* the firs record */
   
     fprintf (stdout, "OK first.\n");
       destroy (r);
      /* the second record */
      r = bless (WRecord);

     if (WRecord_setSubjectUri (r, makeS ("test://anunknownplace")))
        {
          fprintf (stdout, "Corrupted subject Uri\n");
          destroy (r);
          destroy (w);
          return 1;
         }

  if  (WRecord_setRecordType (r,WARCINFO_RECORD))
        {
          fprintf (stdout, "Corrupted Record type\n");
          destroy (r); 
          destroy (w);
          return 1;
        }

  if  (WRecord_setCreationDate (r,makeS ("01052008123041")))
        {
          fprintf (stdout, "Corrupted date\n");
          destroy (r);
          destroy (w);
          return 1;
       }

  if (WRecord_setRecordId (r, makeS ("record1://anunknownplace")))
        {
          fprintf (stdout, "Corrupted record ID\n");
          destroy (r);
         destroy (w);
         return 1;
        }


  if  (WRecord_setContentType(r, makeS ("Text/random")))
        {
         fprintf (stdout," Corrupted content type\n");
         destroy (r); 
         destroy (w); 
         return 1;
        }

  if (WRecord_setContentFromFileName (r, file2))
    {
      fprintf (stdout, "record not filled\n");
      destroy (r);
      destroy (w);
      return 1;
    }

  if (WFile_storeRecord (w,r))
    {
      fprintf (stdout, "record not stored\n");
      destroy (r);
      destroy (w); 
      return 1;
    }



  

 fprintf (stdout, "OK second.\n");
 destroy (r); 
 destroy (w);

 return 0;
}


int test20 (void)
{	
  const char * t = "TEST 20: reading a compressed Warc file created with a warc writer ";
  void * w =  NIL;  /*warc file object */
  void * r = NIL;  /*to recover records */

  fprintf (stdout, "\n%s>\n", t);

  w = bless (WFile,"./app/wdata/testwfile/cwarc.warc.gz", 600 * 1024 * 1024,
             WARC_FILE_READER, WARC_FILE_COMPRESSED_GZIP);
  assert (w);
  
  while (WFile_hasMoreRecords (w))
    {
      r = WFile_nextRecord (w);
      unless (r)
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
      
      fprintf (stdout,"--------------------------------------------------------------------------\n");
      
      fprintf (stdout, "\n\n");
      
/* if you want to read the content, uncomment the following */

       printf ("key1: %s \n", WRecord_getAnvlValue(r, "key1")); 
       printf ("key2: %s \n", WRecord_getAnvlValue(r, "key2")); 

       if (WFile_register (w, r, callback, (void *) 0)) 
         { 
           WarcDebugMsg ("Unable to register the record"); 
           destroy (r); 
           destroy (w); 
           return (1); 
         } 

      WRecord_getContent (r); 

      destroy (r);
    }
  
  destroy (w);
  return (0);
}


int main (void)
{	
  int (* tests [])() = {  test1, test2, test3, test6, test7, 
                           test8, test9, test10, test11, test12, test13,  
                           test14, test15, test16, test17, test18, test19, test20    }; 

  warc_u32_t  i      = 0;

  for(i = 0; i < ARRAY_LEN (tests)  ; ++ i)
    {
      tests[i] ();
    }

  return 0;
}



