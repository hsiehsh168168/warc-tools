
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

#ifndef	__WARC_C_SAFE_H__
#define	__WARC_C_SAFE_H__

/**
 * Portability header file
 */

#include <wport.h>


/**
 * C default headers 
 */

#include <stddef.h>
#include <assert.h>  
#include <stdio.h>   
#include <stdlib.h> 
#include <string.h>  
#include <unistd.h> 
#include <ctype.h> 
#include <sys/types.h>
#include <sys/stat.h>


/**
 * Macro to test error stream
 */

#ifndef w_ferror
#define w_ferror(s) ferror((s))
#endif



/**
 * Macro to test EOF
 */

#ifndef w_feof
#define w_feof(s) feof((s))
#endif

/**
 * Macro to open files in read binary mode
 */

#ifndef w_fopen_rb
#define w_fopen_rb(s) fopen ((s), "r+b")
#endif

/**
 * Macro to open files in append binary mode
 */

#ifndef w_fopen_ab
#define w_fopen_ab(s) fopen ((s), "a+b")
#endif

/**
 * Macro to open files in append binary mode
 */


#ifndef w_fopen_wb
#define w_fopen_wb(s) fopen ((s), "w+b")
#endif

/**
 * Macro to read in files
 */

#ifndef w_fread
#define w_fread(buff, nbel, size, file) fread (buff, nbel, size, file)
#endif

/**
 * Macro to saftely write saftely in files
 */

#ifndef w_fwrite
#define w_fwrite(buff, nbel, size, file) fwrite (buff, nbel, size, file)
#endif

/**
 * Macro to get current offset of o file
 */

#ifndef w_ftell
#define w_ftell(file) ftell(file)
#endif

/**
 * Macro to safetly close a file
*/

#ifndef w_fclose
#define w_fclose(file) \
 do { \
 int ret = fclose (file); \
 assert (! ret); \
 file = NIL; \
 } while (0)
#endif

/**
 * Macro to saftely seek the begining of a file
 */

#ifndef w_fseek_start 
#define w_fseek_start(file) \
 do { \
 int ret = fseek (file, 0, SEEK_SET); \
 assert (! ret); \
 } while (0)
#endif

/**
 * Macro to safetly reach an offset from the begining of a file
 */

#ifndef w_fseek_from_start
#define w_fseek_from_start(file, offset) \
 do { \
 int ret = fseek (file, offset, SEEK_SET); \
 assert (! ret); \
 } while (0)
#endif

/**
 * Macro to safetly reach the end of a file
 */

#ifndef w_fseek_end
#define w_fseek_end(file); \
 do { \
 int ret = fseek (file, 0, SEEK_END); \
 assert (! ret); \
 } while (0)
#endif

/**
 * Macro te safetly reach an offset from the end of a file
 */

#ifndef w_fseek_from_end
#define w_fseek_from_end(file, offset) \
 do { \
 int ret = fseek (file, offset, SEEK_END); \
 assert (! ret); \
 } while (0)
#endif

/**
 * Macro to reach an offset from the current position in file
 */

#ifndef w_fseek_from_here 
#define w_fseek_from_here(file,offset) \
 do { \
 int ret = fseek ((file), (offset), SEEK_CUR); \
 assert (! ret); \
 } while (0)
#endif

/**
 * Macro to saftely seek the begining of a file
 */

#ifndef w_file_size 
#define w_file_size(file,size) \
 do { \
  w_fseek_end((file)); \
  (size) = (warc_u64_t) w_ftell((file)); \
  w_fseek_start((file)); \
 } while (0)
#endif

/**
 * Macro to print in the standard system output
 */

#ifndef w_fprintf 
#define w_fprintf fprintf
#endif

/**
 * Macro to write output according to a format into a string
 */

#ifndef w_snprintf
#define w_snprintf snprintf
#endif

/**
 * Macro to read a character from a file
 */

#ifndef w_fgetc
#define w_fgetc(file) fgetc (file)
#endif

/**
 * Macro to safetly put a character in a file
 */

#ifndef w_ungetc
#define w_ungetc(c,file) \
 do { \
 int ret = ungetc (c, file); \
 assert (ret); \
 } while (0)
#endif

/**
 * Macro to safetly zero fill memory area
 */

#ifndef w_memset_z
#define w_memset_z(a, size) \
 do { \
 void * s = memset (a, 0, size); \
 assert (s == a); \
 } while (0)
#endif



extern ptrdiff_t w_strncpy    (char *, const char *, size_t);
extern char *    w_strcasestr (const char *, const char *);

#endif /* __WARC_C_SAFE_H__ */



