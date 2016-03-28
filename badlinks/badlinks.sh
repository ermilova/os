#!/bin/bash
for i in `find -L $1`
do
	if test -L $i && ! test -e $i && [ $((`date +%s` - `stat --format=%Y $i` > 7 * 24 * 60 * 60)) -eq 1 ]
	then
		echo $i
	fi
done 
