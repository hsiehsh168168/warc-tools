
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
#include <Basic.h>
#include <Console.h>
#include <Automated.h>
#include <CUnit.h>
#include <CUError.h>
#include <TestDB.h>
#include <TestRun.h>
#include <assert.h>
#include <stdio.h>
#include <menu.h>
#include <warc.h>

#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))



int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }

/*         les tests                                  */

void test1 (void)
{ 
  void * c = bless (WClient, makeS("127.0.0.1"), 8080, makeS ("warcserver"));
 fprintf(stdout,"////////// test 1//////////\n"); 
  CU_ASSERT_PTR_NOT_EQUAL(c,NIL);

 if (WClient_getWRecord (c,makeS("gzip"), 0, 
                          makeS("app/wdata/testwfile/file.warc.gz"), 
                          uS("./out1.warc.gz")))
  {
   CU_PASS   ( "request not satisfied");
  }
  else CU_FAIL("request satisfied");
  destroy (c);
}


void test2 (void)
{
  void * c = bless (WClient, makeS("127.0.0.1"), 8080, makeS ("warcserver"));
fprintf(stdout,"////////// test 2//////////\n");  
CU_ASSERT_PTR_NOT_EQUAL(c,NIL);

  if (WClient_getWFile (c, makeS ("gzip"), 0, makeS("app/wdata/testwfile/cwarc.warc.gz"),
                        uS("./out2.warc.gz")))
  {
   CU_PASS   ( "request not satisfied");
  }
  else CU_FAIL("request satisfied");
  destroy (c);

}


void test3 (void)
{

  void * c = bless (WClient, makeS("127.0.0.1"), 8080, makeS ("warcserver"));
fprintf(stdout,"////////// test 3//////////\n");
  
CU_ASSERT_PTR_NOT_EQUAL(c,NIL);


  if (WClient_getWFile (c, makeS ("gzip"), 0, makeS("app/wdata/testwfile/cwarc.warc.gz"),
                        uS("./out2.warc.gz")))
  {
   CU_PASS   ( "request not satisfied");
  }
  else CU_FAIL("request satisfied");
  destroy (c);

}


void test4 (void)
{

  void * c = bless (WClient, makeS("127.0.0.1"), 8080, makeS ("warcserver"));
fprintf(stdout,"////////// test 4//////////\n");
  CU_ASSERT_PTR_NOT_EQUAL(c,NIL);
  if (WClient_getFiltredWFile (c, makeS("uncompressed"), 0, makeS ("uri"), makeS (""), makeS("app/wdata/testwfile/warcfile.warc"),
                        uS("./out3.warc")))
              
  {
   CU_PASS   ( "request not satisfied");
  }
  else CU_FAIL("request satisfied");
  destroy (c);
  
}


int main (void)
{

CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */

   if ((NULL == CU_add_test(pSuite, "TEST 1: Requesting a single record in a compressed warc file", test1)) ||
       (NULL == CU_add_test(pSuite, "TEST 2: Requesting a whole warc file", test2))||
       (NULL == CU_add_test(pSuite, "TEST 3: sending a bad request", test3))||
       (NULL == CU_add_test(pSuite,"TEST 4: sending a filter request", test4)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the automated interface*/ 
    switch (menu()) 
  {
        case 1: {CU_console_run_tests();} 
	case 2:  {CU_basic_run_tests();}
        case 3:{
                CU_set_output_filename("./utest/outputs/client");
    		CU_set_output_filename("./utest/outputs/client" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	}
   }
 /*CU_console_run_tests();*/
   CU_cleanup_registry();
   return CU_get_error();
}
