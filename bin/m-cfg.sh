#!/bin/bash
echo "#ifndef MCFG161688"
echo "#define MCFG161688"
for c in $@ ; do 
	echo -n "#define "
	echo $c|sed -e 's/-D//g' -e 's/=/\t/g'
done
echo "#endif"
