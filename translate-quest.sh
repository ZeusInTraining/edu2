#!/bin/sh

cd questions-exported

for q in `find * -type f | grep -v ".svn"` ; do
  mkdir -p "$(dirname ../questions/$q)"
  cat "$q" | sed 's/[[:blank:]]*$//;/^$/d;s/^	/dummy	/;s/	"\([^	]*\)"	/	\1	/g;s/	"\([^	]*\)"$/	\1/g;s/""/\\"/g' | iconv -f cp1252 -t utf-8  > "../questions/$q"
done

cd ..
