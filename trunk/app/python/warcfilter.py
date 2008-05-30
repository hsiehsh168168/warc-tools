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
from optparse import OptionParser
import re

def  searchFromAnyPosition (field, string) :

     res = None

     for i  in range (len (string)) :

         res = re.compile (field).match(string, i)

         if (res) : 
            break

     return res
        


def main () :
    
    usage =  "./app/python/warcfilter -f <file.warc> [-u <uri>] [-m <mime>] [-r <rtype>] [-v] [-t <working_dir>]\n "\
             "\t-f    : valid WARC file name\n"\
             "\t[-u]  : regular expression comparison with URI\n"\
             "\t[-m]  : regular expression comparison with MIME\n"\
             "\t[-r]  : regular expression comparison with record types (see 'public/wrectype.h' for possible values)\n"\
             "\t[-t]  : temporary working directory (default './')\n"\
             "\t[-v]  : dump ANVL (default false)"
 
    parser = OptionParser(usage)

    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")

    parser.add_option("-u", "--uri", dest="uri",
                      help="filter applied to uri field")

    parser.add_option("-m", "--mime", dest="mime",
                      help="filter applied to mime field")

    parser.add_option("-r", "--rtype", dest="rtype",
                      help="filter applied to record type field")

    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose")
    
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default="./")

    (options, args) = parser.parse_args()

    if len (args) != 0 :
       parser.error(" Incorrect arguments")

    if (not (options.filename)) :
        parser.error(" You must give WARC file name")
   
    cpt = 0

    rectype = {0 : 'WARC_UNKNOWN_RECORD', 1 : 'WARC_INFO_RECORD', 2 : 'WARC_RESPONSE_RECORD', 3 :  'WARC_REQUEST_RECORD',  4 :  'WARC_METADATA_RECORD', \
               5 : 'WARC_REVISIT_RECORD', 6 :  'WARC_CONVERSION_RECORD', 7 : 'WARC_CONTINUATION_RECORD', 8 : 'WARC_RESOURCE_RECORD'}

    if (options.uri) :
       cpt = cpt + 1

    if (options.mime) :
       cpt = cpt +1 

    if (options.rtype) :
       cpt = cpt +1


    if  cpt != 1  :
        parser.error(" You must appply filter to on of fields : uri or mime or record type")

    count = 0
    w = warc.bless (warc.cvar.WFile, options.filename, 600 * 1024 * 1024, warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, options.tmpdir)

    if (not (w)) :
             print "WARC file  not found  "
             return

    while (warc.WFile_hasMoreRecords (w)) :

          r = warc.WFile_nextRecord(w)
     
          if (not (r)) :
             print "bad WARC file "
             return              

          m1 = None
               
         
          if (options.uri) :
        
              string = warc.WRecord_getTargetUri (r)

              if (string) :

                 m1 = searchFromAnyPosition (options.uri, string)
               
                
          if (options.mime) :
        
              string = warc.WRecord_getContentType (r)

              if (string) :

                 m1 =  searchFromAnyPosition (options.mime, string)

          if (options.rtype) :

                 rt =  warc.WRecord_getRecordType (r)
                 m1 =  searchFromAnyPosition (options.rtype, rectype [rt])
          
          if m1 :

              count = count + 1

              print "Warc record number :", count, "\n"
              print "********************************************Mandatory fields**************************************************\n"

              print "WarcId:\t ", warc.WRecord_getWarcId (r),"\n"
              print "WARC-Date:\t ", warc.WRecord_getDate (r), "\n"
              print "WARC-Record-ID:\t ", warc.WRecord_getRecordId (r), "\n"
              print "WARC-Type\t ", warc.WRecord_getRecordType (r), "\n"
              print "Content-Length:\t ", warc.WRecord_getContentLength (r), "\n"
              print "***************************************************************************************************************\n\n"

              print "**********************************************Other fields*****************************************************\n"
          
              print "Offset:\t ", warc.WRecord_getOffset (r),"\n"
              print "Compressed-Size:\t ", warc.WRecord_getCompressedSize (r), "\n"

              if (warc.WRecord_getTargetUri (r)) :
                   print "WARC-Target-URI :\t ", warc.WRecord_getTargetUri (r),"\n"

              if (warc.WRecord_getContentType (r)) :
                   print "Content-Type:\t ", warc.WRecord_getContentType (r),"\n"

              if (warc.WRecord_getConcurrentTo(r)) :
                   print "WARC-Concurent-To:\t ", warc.WRecord_getConcurrentTo(r), "\n"

              if (warc.WRecord_getBlockDigest (r)) :
                   print "WARC-Bloc-Digest:\t ", warc.WRecord_getBlockDigest (r), "\n"

              if (warc.WRecord_getPayloadDigest (r)) :
                   print "WARC-Payload-Digest:\t ", warc.WRecord_getPayloadDigest (r),"\n"

              if (warc.WRecord_getIpAddress (r)) :
                   print "WARC-IP-Address:\t ", warc.WRecord_getIpAddress (r), "\n"

              if ( warc.WRecord_getRefersTo (r)) :
                   print "WARC-Refers-To:\t ", warc.WRecord_getRefersTo (r), "\n"

              if (warc.WRecord_getTruncated (r)) :
                   print "WARC-Truncated:\t ", warc.WRecord_getTruncated (r),"\n"

              if ( warc.WRecord_getWarcInfoId (r)) :
                   print "WARC-Warcinfo-ID:\t ", warc.WRecord_getWarcInfoId (r), "\n"

              if (warc.WRecord_getFilename (r)) :
                   print "WARC-Filename:\t ", warc.WRecord_getFilename (r),"\n"

              if (warc.WRecord_getProfile (r)) :
                   print "WARC-Profile:\t ", warc.WRecord_getProfile (r), "\n"

              if (warc.WRecord_getPayloadType (r)) :
                  print "WARC-Identified-Payload-type:\t ", warc.WRecord_getPayloadType (r), "\n"

              if (warc.WRecord_getSegmentOriginId  (r)) :
                  print "WARC-Segment-Origin-ID:\t ", warc.WRecord_getSegmentOriginId  (r),"\n"

              if (warc. WRecord_getSegmentNumber (r)) :
                  print "WARC-Segment-Number:\t ", warc. WRecord_getSegmentNumber (r), "\n"

              if (warc.WRecord_getSegTotalLength(r)) :
                  print "WARC-Segment-Total-Length:\t ", warc.WRecord_getSegTotalLength(r), "\n\n"
                  print "***************************************************************************************************************\n\n"

              if (options.verbose) :

                  al = warc.WRecord_getAnvl (r)
                  if  al :
                      i = 0
                      j = warc.WList_size(al)
                      if (j) :
                        print "********************************************More info***************************************************\n"
                        while ( i < j ) :
                                   a = warc.WList_getElement (al, i)
                                   print  "key :\t ",warc.WAnvl_getKey   (a) ,"|Value :\t ",  warc.WAnvl_getValue (a)
                                   i = i + 1
                        print "***************************************************************************************************************\n\n"

          warc.destroy (r)

    warc.destroy (w)
    return

if __name__ == "__main__":
    main()
