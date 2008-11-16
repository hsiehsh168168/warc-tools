package warctools;

import com.sun.jna.Pointer;


/** Simple example of native library declaration and usage. */
public class wfile implements wfilei {
	
	public Pointer file = null;
	
	
public wfile (String path, int dim, int mode, int compressed, String dname)
{
	file = bless_Wfile( path,  dim,  mode,  compressed,  dname);
}

public Pointer bless_Wfile (String path, int dim, int mode, int compressed, String dname )
{
	return wfilei.INSTANCE.bless_Wfile( path,  dim,  mode,  compressed,  dname);
}

public int     WFile_hasMoreRecords (Pointer x )
{
	return wfilei.INSTANCE.WFile_hasMoreRecords(x);
}

public Pointer WFile_nextRecord     (Pointer x)
{
	return wfilei.INSTANCE.WFile_nextRecord(x);
}

public long      WFile_isFull          (Pointer x )
{
	return wfilei.INSTANCE.WFile_isFull(x);
}

public int     WFile_setMaxSize     (Pointer x,  long size)
{
	return wfilei.INSTANCE.WFile_setMaxSize(x, size);
}

public int     WFile_storeRecord    (Pointer x, Pointer wrec)
{
	return wfilei.INSTANCE.WFile_storeRecord(x, wrec);
}

public int     WFile_seek           (Pointer x,  long offset)
{
	return wfilei.INSTANCE.WFile_seek(x,offset);
}

public void     destroy             (Pointer x)
{
	wfilei.INSTANCE.destroy(x);
}

public  int isValid (Pointer x)
{
	return wfilei.INSTANCE.isValid(x);
}

}
