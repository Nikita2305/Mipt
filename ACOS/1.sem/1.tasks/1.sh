#!/bin/bash

if [ ${#MANPATH} -ne 0 ]; then
    export MANPATH=${MANPATH}:
fi;
MANPATH=${MANPATH}/usr/share/man

IFS=':'
read -a strarr <<< "$MANPATH"
shopt -s nullglob
savepath=`pwd`/temp

while read line
do
    answer="---"
    for dir in "${strarr[@]}";
    do
        p=$dir/man3
        for f in $p/${line}.*
        do
            name=$f
            if gunzip -c $f > $savepath 
            then 
                name=$savepath
            fi
            answer=`sed -n 's/.*include <\(.*\)>.*/\1/p' $name`
        done
    done
    echo $answer
done < "${1:-/dev/stdin}"
exit
