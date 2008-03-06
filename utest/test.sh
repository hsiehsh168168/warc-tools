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


# NOTE: never call this script directly. Use "make test" instead.

if [ $# = 0 ]; then
    echo "usage: make test" >&2
    exit 1
fi

echo
echo "######################################" >&2
echo "# Starting unit tests for warc-tools #" >&2
echo "######################################" >&2
echo

i=0
j=0
for t in $@;
do
    if [ "$t" = "utest/object" ]; then
        trap "Normal behaviour for this special unit test" ABRT INT TERM EXIT
        $t >/dev/null 2>&1
        trap - ABRT INT TERM EXIT
    else
        $t >/dev/null 2>&1
    fi

    if [ $? = 0 ]; then 
        let "i=$i + 1"
        echo "+ do test: $t [OK]"
    else 
        let "j=$j + 1"
        echo "- do test: $t [NOK]"
    fi
done

let "a=$i + $j"

echo
echo "$a tests: $i passed   $j failed"
echo

exit 0