#!/usr/bin/env python
import sys

import wpath

from wfile   import WFile
from wrecord import WRecord 
import time
import sha
import warc
from optparse import OptionParser


def addtowarc (fname, wfile, uri, mime, date, ip, cmode, maxsize, tmpdir):
	
##  creating a new record  ##

	r = WRecord()
	w = WFile(wfile, maxsize, warc.WARC_FILE_WRITER, cmode, tmpdir)
	r.setRecordType(warc.WARC_RESOURCE_RECORD)
	
	uri=prefix+ fname
	r.setTargetUri(uri, len(uri))	
	
	r.setDate(date, len(date))
	
	r.setContentType(mime, len(mime))
	
	t=time.localtime()
	s = time.strftime ("%a, %d%b%Y%H%M%S", t)
	sh = sha.new(s)
	rid = sh.hexdigest()
	rid = "uuid:" + rid
	r.setRecordId(rid, len(rid))

	
	r.setIpAddress(ip, len(ip))
	r.setContentFromFileName(fname)

	w.storeRecord(r)

	r.destroy()
	w.destroy()



def main () :

    usage = "Converts a file on disk to a WARC record \n\n"\
            + sys.argv[0] + " -f <file.> -w <file.warc> \n"\
            "                     -u <url> -m <mime> -d <date> -i <ip> \n"\
            "                     [-c] [-x <maxsize>] [-t <working_dir>]\n "\
            "\t-f    : valid file name\n"\
            "\t-w    : valid WARC file name\n"\
            "\t-u    : valid url\n"\
            "\t-m    : valid mime\n"\
            "\t-[x]    : max WARC file size\n"\
            "\t-d    : valid date\n"\
            "\t-i    : valid  ip\n"\
            "\t[-c]  : WARC file will be GZIP compressed (default no)\n"\
            "\t[-t]  : temporary working directory (default './')\n"

    parser = OptionParser(usage)

    parser.add_option("-f", "--file", dest="filename",
                      help="Input filename")
    parser.add_option("-w", "--wfile", dest="wfilename",
                      help="Output WARC filename")
    parser.add_option("-u", "--url", dest="url",
                      help="the data URL ")
    parser.add_option("-m", "--mime", dest="mime",
                      help="Document MIME type")
    parser.add_option("-d", "--date", dest="date",
                      help="write date")
    parser.add_option("-i", "--ipaddress", dest="ip",
                      help="Server IP address")
    parser.add_option("-c", "--compressed", dest="cmode",
                      action="store_true", help="Compressed WARC file")
    parser.add_option("-x", "--maxsize", dest="maxsize",
                      help="WARC file maximum size", default=1024 * 1024 * 1024)
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default=".")
    (options, args) = parser.parse_args()

    compmode = warc.WARC_FILE_UNCOMPRESSED
    if (options.cmode):
        compmode = warc.WARC_FILE_COMPRESSED_GZIP
    
	addtowarc(options.filename, options.wfilename, options.url,
              options.mime, options.date, options.ip, compmode,
              options.maxsize, options.tmpdir)
    
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "For help, try : " + sys.argv[0] + " -h"
        sys.exit(1)
    
    main()
