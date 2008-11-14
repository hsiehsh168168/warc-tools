/* 
  Here, there is a specialized bless functin for warc-tools library classes
 */


#include <warc.h>
#include <wbloc.h>

#include <wpybless.h>


WPUBLIC void * bless_WFile (const char  * fname, 
                            warc_u64_t max_size, 
                            wfile_mode_t mode, warc_bool_t compressed,
                            const char * dname)
{
  return (bless (WFile, fname, (warc_u64_t) max_size,  mode, compressed,dname));
}

WPUBLIC void * bless_WRecord (void)
{
  return (bless (WRecord));
}

WPUBLIC void * bless_WBloc (void  * wfile, void  * wrecord, 
                            warc_bool_t httpheaders, warc_u32_t alloc)
{
  return (bless (WBloc, wfile, wrecord, httpheaders, alloc));
}
