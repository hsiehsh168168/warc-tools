#!/bin/sh

set -e

if [ $# != 1 ]; then
    echo "Code beautifier"
    echo "Usage:   $0 dirname" >&2
    echo "Example: $0 ." >&2
    exit 1
fi

if [ ! -d $1 ]; then
    echo "dirname '$1' must be a valid directory name"
    exit 1
fi

beauty="astyle"
if ! type "$beauty" &>/dev/null; then
    echo ">> \"$beauty\" command is missing" >&2
    echo "Install it from: http://sourceforge.net/projects/astyle/"
    exit 1
fi

for i in `find $1 -name "*.c" -o -name "*.h" -o -name "*.x" | grep -v "plugin/gzip" | grep -v "app/wdata" | grep -v "doc" | grep -v "license"`;
do
  $beauty --mode=c --style=gnu -b -S -w -M -m -F -p -d -n -Z $i
done
