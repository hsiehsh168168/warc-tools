/* 
  Here, there is a specialized bless functin for warc-tools library classes
 */


#include <warc.h>
#include <wbloc.h>   /* WBloc class */

#include <wpybless.h>

WPUBLIC void * bless_WFile (const char  * fname, const warc_u32_t  max_size, wfile_mode_t mode, const wfile_comp_t compressed, const char * dname )
{
  return (bless (WFile, fname, max_size, mode, compressed, dname));
}

WPUBLIC void * bless_WRecord ()
{
return (bless (WRecord));
}

WPUBLIC void * bless_WBloc (void  * wfile, void  * wrecord, warc_bool_t httpheaders, const warc_u32_t alloc)
{
void * x;

 x = bless (WBloc, wfile, wrecord, httpheaders, alloc);

return (x);
}
