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


import arc
from optparse import OptionParser

def main () :
    usage =  "./app/python/arcdump.py -f <file.arc>  [-t <working_dir>]\n "\
             "\t-f    : valid ARC file name\n"\
             "\t[-t]  : temporary working directory (default './')"

    parser = OptionParser(usage)
    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")
    
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default="./")

    (options, args) = parser.parse_args()

    if len (args) != 0 :
       parser.error(" Incorrect arguments")

    if (not (options.filename)) :
        parser.error(" You must give ARC file name")

    count = 0
    a = arc.bless (arc.cvar.AFile, options.filename, arc.ARC_FILE_DETECT_COMPRESSION, options.tmpdir)

    if (not (a)) :
             print "ARC file  not found  "
             return
        
    while (arc.AFile_hasMoreRecords (a) ) :
 
          ar = arc.AFile_nextRecord (a)
          if (not (ar)) :
             print "bad arc file"
             arc.destroy (a)
             return
          count = count +1

          print "Arc record number :", count, "\n"
          print "***************************************************************************************************************\n\n"

          print "Url:\t ", arc.ARecord_getUrl (ar)
          print "CreationDate:\t ", arc.ARecord_getCreationDate (ar)
          print "MimeType:\t ", arc.ARecord_getMimeType (ar)
          print "IpAdress:\t ", arc.ARecord_getIpAddress(ar)
        
          print "***************************************************************************************************************\n\n"

          arc.destroy (ar)

    arc.destroy (a)
    return

if __name__ == "__main__":
    main()
