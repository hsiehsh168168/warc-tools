#!/bin/sh

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

usage () {

    echo "Convert all ARC files in a directory to WARC files"
    echo "Usage: $0 <-d dirname> [-b agzip] [-c wgzip] [-v] [-h]"  >&2
    echo "       -d     : directory name containing ARC files"  >&2
    echo "       -b     : ARC files are compressed (default yes)"  >&2
    echo "       -c     : WARC files will be compressed (default yes)"  >&2
    echo "       -h     : print this help message"  >&2
    echo "       -v     : output version information and exit"  >&2
    exit 1
}

# default settings
version="0.17"

while getopts hvbcd: o
do
  case "$o" in
    d)   dn=$OPTARG ;;
    b)   acomp="-b" ;;
    c)   ccomp="-c" ;;
    h)   usage ;;
    v)   echo "$0 v$version"; exit 0 ;;
    *)   usage ;;
  esac
done

if [ -z "$dn" ]; then
    echo ">> you must supply \"-d\" option" >&2
    usage
fi

if [ ! -d "$dn" ]; then
    echo ">> directory \"$dn\" doesn't exist" >&2
    usage
fi

orig_dir=$(pwd)
cd ${0%/*}/..
a2w="`pwd`/app/arc2warc"

if ! type $a2w &>/dev/null; then
    echo ">> \"$a2w\" doesn't exist" >&2
    usage
fi

for i in `find $dn -name "*.arc*" -type "f"`;
do
  wf=`echo "$i" | sed -e "s|arc\(\.[^.]*\)$|warc\1|"`
  if [ ! -z "$ccomp" ]; then
      wf=`echo "$wf" | sed -e "s|\.gz$||"`
  fi

  if [ -f "$wf" ]; then
      echo ">> \"$wf\" already exist, skipt it" >&2
      continue
  fi

  echo "converting $i"

  $a2w -a $i $acomp -f $wf $ccomp &>/dev/null
  if [ $? -ne 0 ]; then
      echo ">> error when converting \"$i\"" >&2
      exit 2
  fi
done 

exit 0
