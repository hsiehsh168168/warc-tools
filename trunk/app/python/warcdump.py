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
import sys

def main () :
    
    usage =  "./app/python/warcdump.py -f <file.warc> [-v] [-t <working_dir>]\n "\
             "\t-f    : valid WARC file name\n"\
             "\t[-t]  : temporary working directory (default './')\n"\
             "\t[-v]  : dump ANVL (default false)"
 
    parser = OptionParser(usage)

    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")

    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose")
    
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default="./")

    (options, args) = parser.parse_args()

    if len (args) != 0 :
       parser.error(" Incorrect arguments")

    if (not (options.filename)) :
        parser.error(" You must give WARC file name")

    w = warc.bless (warc.cvar.WFile, options.filename, 600 * 1024 * 1024, warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, options.tmpdir)

    if (not (w)) :
             print "WARC file  not found  "
             return
    print "%-20s %-20s %-10s %-20s %-45s %-44s %-86s " % ("Offset", "CSize", "WarcId", "Content-Length", \
           "WARC-Type", "WARC-Date",  "WARC-Record-ID")

    while (warc.WFile_hasMoreRecords (w)) :

          r = warc.WFile_nextRecord(w)

          if (not (r)) :
             print "bad WARC file "
             warc.destroy (w)
             return             

          sys.stdout.write ("%-20u " % warc.WRecord_getOffset (r))

          sys.stdout.write ("%-20u " % warc.WRecord_getCompressedSize (r))

	  sys.stdout.write ("%-10s " % warc.WRecord_getWarcId (r))

          sys.stdout.write ("%-20u " % warc.WRecord_getContentLength (r)) 

          sys.stdout.write ("%-45u " % warc.WRecord_getRecordType (r)) 

          sys.stdout.write ("%-44s " % warc.WRecord_getDate (r))

          sys.stdout.write ("%-86s " % warc.WRecord_getRecordId (r))      

          m1 = warc.WARC_FALSE
          print "More Fields:\n"

          if (warc.WRecord_getContentType (r)) :
              print "%-35s: %-20s" %  ("Content-Type" , warc.WRecord_getContentType (r))
              m1 = warc.WARC_TRUE 

          

          if (warc.WRecord_getConcurrentTo(r)) :
              print "%-35s: %-20s" % ("WARC-Concurrent-To" , warc.WRecord_getConcurrentTo(r))
              m1 = warc.WARC_TRUE

          if (warc.WRecord_getBlockDigest (r)) :
              print "%-35s: %-20s" % ( "WARC-Block-Digest", warc.WRecord_getBlockDigest (r))
              m1 = warc.WARC_TRUE

          if (warc.WRecord_getPayloadDigest (r)) :
              print "%-35s: %-20s"  % ("WARC-Payload-Digest", warc.WRecord_getPayloadDigest (r))
              m1 = warc.WARC_TRUE

          if (warc.WRecord_getIpAddress (r)) :
               print "%-35s: %-20s"  % ("WARC-IP-Address", warc.WRecord_getIpAddress (r))
               m1 = warc.WARC_TRUE

          if ( warc.WRecord_getRefersTo (r)) :
               print  "%-35s: %-20s" % ("WARC-Refers-To", warc.WRecord_getRefersTo (r))
               m1 = warc.WARC_TRUE

          if (warc.WRecord_getTargetUri (r)) :
               print   "%-35s: %-20s" % ( "WARC-Target-URI",warc.WRecord_getTargetUri (r))
               m1 = warc.WARC_TRUE

          if (warc.WRecord_getTruncated (r)) :
               print  "%-35s: %-20s" % ("WARC-Truncated", warc.WRecord_getTruncated (r))
               m1 = warc.WARC_TRUE

          if ( warc.WRecord_getWarcInfoId (r)) :
               print  "%-35s: %-20s" % ("WARC-Warcinfo-ID", warc.WRecord_getWarcInfoId (r))
               m1 = warc.WARC_TRUE

          if (warc.WRecord_getFilename (r)) :
               print "%-35s: %-20s" % ("WARC-Filename:", warc.WRecord_getFilename (r))
               m1 = warc.WARC_TRUE

          if (warc.WRecord_getProfile (r)) :
               print "%-35s: %-20s" % ("WARC-Profile", warc.WRecord_getProfile (r))
               m1 = warc.WARC_TRUE

          if (warc.WRecord_getPayloadType (r)) :
               print "%-35s: %-20s" % ("WARC-Identified-Payload-type", warc.WRecord_getPayloadType (r))
               m1 = warc.WARC_TRUE

          if (warc.WRecord_getSegmentOriginId  (r)) :
                print "%-35s: %-20s" % ("WARC-Segment-Origin-ID", warc.WRecord_getSegmentOriginId  (r))
                m1 = warc.WARC_TRUE

          if (warc. WRecord_getSegmentNumber (r)) :
                print "%-35s: %-20d" % ("WARC-Segment-Number", warc. WRecord_getSegmentNumber (r))
                m1 = warc.WARC_TRUE

          if (warc.WRecord_getSegTotalLength(r)) :
                print "%-35s: %-20d" % ("WARC-Segment-Total-Length", warc.WRecord_getSegTotalLength(r))
                m1 = warc.WARC_TRUE
         
          if (not (m1)) :
                print "--No One --"          

          if (options.verbose) :

             al = warc.WRecord_getAnvl (r)
            
             if  al :
                 i = 0
                 j = warc.WList_size(al)
                 if (j) :
                        print "-- More Info--\n"
                        while ( i < j ) :
                                   a = warc.WList_getElement (al, i)
                                   print  "key :  ",warc.WAnvl_getKey   (a) 
                                   print  "Value: ",  warc.WAnvl_getValue (a)
                                   i = i + 1
                                   

          warc.destroy (r)

    warc.destroy (w)
    return

if __name__ == "__main__":
    main()
