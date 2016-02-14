#!/bin/bash
SRC=libhCOS
MOD=$1
if ! [ -d $SRC ]; then
	echo $SRC does not exists
	exit 1
fi
if [ -d $MOD ]; then
	echo $MOD already exists
	exit 1
fi
cp -r $SRC/src-hello $MOD
cd $MOD
	sed -e '/TEMPLATE/d' -i Makefile
	mv Makefile .tmp
	echo 'PREFIX ?= $(shell pwd)/../prefix/$(CROSS:%-=%)' > Makefile
	cat .tmp >>Makefile
	rm -f .tmp
cd -
