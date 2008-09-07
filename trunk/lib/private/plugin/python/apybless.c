/* 
  Here, there is a specialized bless functin for warc-tools library classes
  It is for Ruby wrapper use purpuse, only the class used in this warpper may have
  a specialized blmess function here
 */

#include <warc.h>
#include <afile.h>



WPUBLIC void * bless_AFile (const char * fname, const afile_comp_t compressed, const char * dname)
{
void * x;

x = bless (AFile, fname, compressed, dname);

return (x);
}


