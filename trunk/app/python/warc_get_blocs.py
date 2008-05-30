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


def callback (env, buff, size) :
    
    
    datafile =  env[0]+"BlocOfRecord" + str (env[1])
    f = open (datafile , 'a')
    f.write (buff[0:size])
    f.close() 
    return 1


def main () :

    usage =  "./app/python/getBlocs -f <file.warc> [-o <outpout_dir> ] [-t <working_dir>]\n "\
             "\t-f    : valid WARC file name\n"\
             "\t[-t]  : temporary working directory (default './')\n"\
             "\t[-o]  : valide directory name"
 
    parser = OptionParser(usage)
    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")
   
    
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default="./")

    parser.add_option("-o", "--blocdir", dest="datadir",
                      help="Temporary working directory", default="./")

    (options, args) = parser.parse_args()

    if len (args) != 0 :
       parser.error(" Incorrect arguments")

    if (not (options.filename)) :
        parser.error(" You must give WARC file name")
   
    if (options.datadir [len (options.datadir) - 1] != '/') :
        print " You must add '/' at the the end of given directory ", options.datadir, "and check that this directory exists"
        return 

    w = warc.bless (warc.cvar.WFile, options.filename, 600 * 1024 * 1024, warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, options.tmpdir)

    if (not (w)) :
             print "WARC file  not found  "
             return


    
    env = [options.datadir, 0]
    while (warc.WFile_hasMoreRecords (w)) :

            r = warc.WFile_nextRecord(w)

            if (not (r)) :
                print "bad WARC file "
                warc.destroy (w)
                return   
 
            env [1] = env [1] + 1
     
            warc.pyWFile_register (w, r,callback, env) 

            warc.WRecord_getContent (r)
               

            warc.destroy (r)
    
    warc.destroy (w)
    return

if __name__ == "__main__":
    main()
    
    


