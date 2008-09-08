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

import wpath

from afile import AFile
from arecord import ARecord

from wrecord  import WRecord
from wfile import WFile

import warc
import arc

from optparse import OptionParser

def main () :

    usage =  "./app/python/arc2warc.py  -a <file.arc> -f <file.warc> [-c] [-t <working_dir>]\n "\
             "\t-a    : valid ARC file name\n"\
             "\t-f    : valid WARC file name\n"\
             "\t[-c]  : WARC file will be GZIP compressed (default no)\n"\
             "\t[-t]  : temporary working directory (default './')\n"
             
 
    parser = OptionParser(usage)

    parser.add_option("-a", "--afile", dest="afilename",
                      help="read data from ARC FILENAME")
    parser.add_option("-f", "--wfile", dest="wfilename",
                      help="write data to WARC FILENAME")
    parser.add_option("-c", "--compressed", dest="cmode",
                      action="store_true", help="compressed outpout  WARC FILE")
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default="./")

    (options, args) = parser.parse_args()


    if len (args) != 0 :
       parser.error(" Incorrect arguments")

    if (not (options.afilename)) :
        parser.error(" You must give ARC file name")

    if (not (options.wfilename)) :
        parser.error(" You must give the outpout WARC file name desired")

    a = AFile (options.afilename, arc.ARC_FILE_DETECT_COMPRESSION, options.tmpdir)

    if (not (a)) :
       print "ARC file not found "
       return

    if (options.cmode) :
        cmode = warc.WARC_FILE_COMPRESSED_GZIP
    else :
        cmode = warc.WARC_FILE_UNCOMPRESSED

    w = WFile(options.wfilename, 600 * 1024 * 1024,
              warc.WARC_FILE_WRITER, cmode, options.tmpdir);
  

    if (not (w)) :
       print "given temporary directory does not exist "
       a . destroy ()
       return

    uuid = warc.bless (warc.cvar.WUUID)

    if (not (uuid)) :
       print " can not create wuuid object "
       a . destroy ()
       w . destroy ()
       return
   
    while (a . hasMoreRecords () ) :

          ar = a. nextRecord ()

          if (not (ar)) :
             print "bad ARC file"
             a . destroy ()
             w . destroy ()
             warc.destroy (uuid)
             return

          wr = WRecord ()

          if (not (wr)) :
             print "can not create WARC record object"
             a . destroy ()
             w . destroy ()
             warc.destroy (uuid)
             ar . destroy ()
             return

          wr . setRecordType (warc.WARC_RESOURCE_RECORD)

          uri = ar . getUrl ()
          wr . setTargetUri (uri, len (uri))

          date = ar . getCreationDate ()
          wr . setDateFromArc  (date, len(date))

          mime = ar . getMimeType ()
          wr . setContentType (mime, len (mime))

          ip = ar . getIpAddress()
          wr . setIpAddress (ip, len(ip))
          
          warc.WUUID_hash (uuid, uri , len (uri))
          warc.WUUID_hash (uuid, date , len (date))
          wr. setRecordId  (warc.WUUID_text (uuid), len (warc.WUUID_text (uuid)))
          warc.WUUID_reinit (uuid)

          if (ar . transferContent (wr, a)) :
              print "Unable to pass content to the WRecord"
              a . destroy ()
              w . destroy ()
              warc.destroy (uuid)
              ar . destroy ()
              return

          if (w . storeRecord (wr) ) :
              print "failed to write WRecord" 
              a . destroy ()
              w . destroy ()
              warc.destroy (uuid)
              ar . destroy ()
              return

          ar . destroy ()
          wr . destroy ()
    
    warc.destroy (uuid)
    a . destroy ()
    w . destroy ()
    return


if __name__ == "__main__":
    main()
