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
from optparse import OptionParser

import wpath
import warc
from   wrecord  import WRecord
from   wfile    import WFile
from   wbloc    import WBloc


def main () :
    
    usage =  "./app/python/wgetbloc.py -f <file.warc> -n <num> [-v] [-t <working_dir>]\n "\
             "\t-f    : valid WARC file name\n"\
             "\t-n    : record number\n"\
             "\t[-t]  : temporary working directory (default './')\n"\
             "\t[-v]  : dump ANVL (default false)\n\n"\
             "./app/python/wgetbloc.py -f foo.warc.gz -n 7"
 
    parser = OptionParser(usage)

    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")

    parser.add_option("-n", "--num", dest="cnt",
                      help="record number", type="int")

    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose")
    
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default=".")

    (options, args) = parser.parse_args()

    if len (args) != 0 :
       parser.error(" Incorrect arguments")

    if (not (options.filename)) :
        parser.error(" You must give WARC file name")

    if options.cnt == None:
        parser.error(" You must provide a record number")

    w = WFile (options.filename ,600 * 1024 * 1024, warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, options.tmpdir)

    if (not (w)) :
             print "WARC file  not found "

    # go to the record number "cnt"
    cnt = options.cnt
    nr  = 1
    while nr:
        nr = w . hasMoreRecords ()
        r  = w . nextRecord ()
        if cnt == 0:
            break

        r . destroy()
        cnt = cnt - 1

    if cnt or nr == 0:
        print "End of file reached, no record with number", options.cnt
        sys.exit(0);

    # choose your buffer size (ex. 32K = 32 * 1024) to read the payload
    # (with the HTP headers) chunk by chunk 
    b = WBloc(w, r, 32 * 1024)

    while True:
        buff = b.getNext()
        if buff:
            print buff
        else: # no more data to read. reach the end of record
            break

    b.destroy()
    r.destroy()
    w.destroy ()

if __name__ == "__main__":
    main()
