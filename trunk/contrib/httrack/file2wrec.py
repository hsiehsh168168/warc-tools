#!/usr/bin/env python

# -------------------------------------------------------------------  #
# Copyright (c) 2007-2008 Hanzo Archives Limited.                      #
#                                                                      #
# Licensed under the Apache License, Version 2.0 (the "License");      #
# you may not use this file except in compliance with the License.     #
# You may obtain a copy of the License at                              #
#                                                                      #
#     http://www.apache.org/licenses/LICENSE-2.0                       #
#                                                                      #
# Unless required by applicable law or agreed to in writing, software  #
# distributed under the License is distributed on an "AS IS" BASIS,    #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      #
# implied.                                                             #
# See the License for the specific language governing permissions and  #
# limitations under the License.                                       #
#                                                                      #
# You may find more information about Hanzo Archives at                #
#                                                                      #
#     http://www.hanzoarchives.com/                                    #
#                                                                      #
# You may find more information about the WARC Tools project at        #
#                                                                      #
#     http://code.google.com/p/warc-tools/                             #
# -------------------------------------------------------------------  #

import sys
sys.path.insert (0, "../../lib/private/plugin/python/")

import warc
from wrecord import WRecord
from wfile import WFile
from wrecord import WRecord 

import time
import sha
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

    usage =  "./app/python/addtowarc.1.py  -f <file.> -w <file.warc>  -u <url> -m <mime> -d <date> -i <ip>  [-c] [-x <maxsize>] [-t <working_dir>]\n "\
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
    main()
