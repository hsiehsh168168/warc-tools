#!/bin/sh

set -e

if [ $# != 1 ]; then
    echo "Line of code statistics"
    echo "Usage:   $0 dirname" >&2
    echo "Example: $0 ." >&2
    exit 1
fi

if [ ! -d $1 ]; then
    echo "dirname '$1' must be a valid directory name"
    exit 1
fi

ccount="c_count"
if ! type "$ccount" &>/dev/null; then
    echo ">> \"$ccount\" command is missing" >&2
    echo "Install it from: http://www.chris-lott.org/resources/cmetrics/"
    exit 1
fi

cf=`find $1 -name "*.c" -o -name "*.h" -o -name "*.x" | grep -v "plugin/gzip" | wc -l`

echo " ------------------------------------------------"
echo "|                  SMARTLIB                      |"
echo "| C SOURCE STATEMENTS, AND SUMMARIZES STATISTICS |"
echo " ------------------------------------------------"
echo
echo " $cf SOURCE FILES"
echo
echo
echo " LINES    STATEMENTS          FILENAME"
echo

$ccount `find $1 -name "*.c" -o -name "*.h" -o -name "*.x" | grep -v "plugin/gzip" | sed -r "s|./|  |"`
