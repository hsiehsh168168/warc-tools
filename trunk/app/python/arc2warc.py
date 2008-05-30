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

    a = arc.bless (arc.cvar.AFile, options.afilename, arc.ARC_FILE_DETECT_COMPRESSION, options.tmpdir)

    if (not (a)) :
       print "ARC file not found "
       return

    if (options.cmode) :
        cmode = warc.WARC_FILE_COMPRESSED_GZIP
    else :
        cmode = warc.WARC_FILE_UNCOMPRESSED

    w = warc.bless(warc.cvar.WFile, options.wfilename, 600 * 1024 * 1024 , warc.WARC_FILE_WRITER, cmode, options.tmpdir);
  

    if (not (w)) :
       print "given temporary directory does not exist "
       arc.destroy (a)
       return

    uuid = warc.bless (warc.cvar.WUUID)

    if (not (uuid)) :
       print " can not create wuuid object "
       arc.destroy (a)
       warc.destroy (w)
       return
   
    while (arc.AFile_hasMoreRecords (a) ) :

          ar = arc.AFile_nextRecord (a)

          if (not (ar)) :
             print "bad ARC file"
             arc.destroy (a)
             warc.destroy (w)
             warc.destroy (uuid)
             return

          wr = warc.bless (warc.cvar.WRecord)

          if (not (wr)) :
             print "can not create WARC record object"
             arc.destroy (a)
             warc.destroy (w)
             warc.destroy (uuid)
             arc.destroy (ar)
             return

          warc.WRecord_setRecordType (wr, warc.WARC_RESOURCE_RECORD)

          uri = arc.ARecord_getUrl (ar)
          warc.WRecord_setTargetUri (wr, uri, len (uri))

          date = arc.ARecord_getCreationDate (ar)
          warc.WRecord_setDateFromArc  (wr, date, len(date))

          mime = arc.ARecord_getMimeType (ar)
          warc.WRecord_setContentType (wr, mime, len (mime))

          ip = arc.ARecord_getIpAddress(ar)
          warc.WRecord_setIpAddress (wr, ip, len(ip))
          
          warc.WUUID_hash (uuid, uri , len (uri))
          warc.WUUID_hash (uuid, date , len (date))
          warc.WRecord_setRecordId  (wr, warc.WUUID_text (uuid), len (warc.WUUID_text (uuid)))
          warc.WUUID_reinit (uuid)

          if (arc.ARecord_transferContent (ar, wr, a)) :
              print "Unable to pass content to the Warc Record"
              arc.destroy (a)
              warc.destroy (w)
              warc.destroy (uuid)
              arc.destroy (ar)
              return

          if (warc.WFile_storeRecord (w, wr)) :
              print "failed to write  WRecord" 
              arc.destroy (a)
              warc.destroy (w)
              warc.destroy (uuid)
              arc.destroy (ar)
              return

          arc.destroy (ar)
          warc.destroy (wr)
    
    warc.destroy (uuid)
    arc.destroy (a)
    warc.destroy (w)
    return


if __name__ == "__main__":
    main()
