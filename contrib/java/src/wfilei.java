package warctools;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
 
 
public interface wfilei extends Library 
{
    wfilei INSTANCE = (wfilei) Native.loadLibrary("warc", wfilei.class);
    
    Pointer bless_Wfile (String path, int dim, int mode, int compressed, 
                         String dname );
        
    int     WFile_hasMoreRecords (Pointer x );
    Pointer WFile_nextRecord     (Pointer x);
    long    WFile_isFull         (Pointer x );
    int     WFile_setMaxSize     (Pointer x, long    size);
    int     WFile_storeRecord    (Pointer x, Pointer wrec);
    int     WFile_seek           (Pointer x, long    offset);
     
     int    isValid (Pointer x);
	 void   destroy (Pointer x);
}
