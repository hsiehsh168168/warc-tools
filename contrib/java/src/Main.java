package warctools;

// Simple example of native library declaration and usage
public class Main 
{
    public static void main(String[] args) 
    {
        //open a wfile object in read mode
        wfile   a = new wfile("./awanvl.warc", 600 * 1024 * 1024, 1, 0, ".");
        wrecord b = new wrecord();
    	
        System.out.println(a.WFile_hasMoreRecords(a.file));
        b.record = a.WFile_nextRecord(a.file);
    	
        System.out.println(a.WFile_isFull(a.file));
        
        System.out.println(b.WRecord_getWarcId(b.record));
    	
        a.destroy(a.file);
        b.destroy(b.record);
    }
}
