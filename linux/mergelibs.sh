#!/bin/bash
set -e
set -x
rm -rf tmp/merge/$NAME
mkdir -p tmp/merge/$NAME

LIBDIR=`readlink -f $LIBDIR`

cd tmp/merge/$NAME
for i in `echo $LIBDIR/*.a`
do
	ar x $i
done

ar r libmskia.a *.o
rm *.o
