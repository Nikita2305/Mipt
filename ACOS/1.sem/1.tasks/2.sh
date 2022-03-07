#!/bin/bash

count () {
    echo "scale=100;$1" | bc -l
}

index () {
    echo $(count "$1*$mod+$2")
}

item () {
    echo ${arr[$(index $1 $2)]}
}

pr () {
    for (( i=0; i < $n; i++ ))
    do
        for (( j=0; j < $mod; j++ ))
        do
            echo -n "$(item $i $j) "
        done
        echo ""
    done
}

IFS=','
arr=()
mod=0
while read line
do
    read -a strarr <<< $line
    mod=${#strarr[*]}
    for number in ${strarr[*]}
    do
        arr+=( $(count "($number)*1.0"))
    done
done < "${1:-/dev/stdin}"
n=$(count "$mod-1")

for (( string=0; string < $n; string++ ))
do
    found=$n
    for (( j=$string; j < $n; j++ ))
    do
        if [ $(count "sqrt(($(item $string $j))*($(item $string $j))) > 0.0000000001") -ne 0 ]; then
            found=$j
            break
        fi;
    done
    if [ $found -eq $n ]; then
        echo "Rank < n"
        exit
    fi;
    for (( j=0; j < $mod; j++ ))
    do
        temp=$(item $string $j)
        arr[$(index $string $j)]=$(item $found $j)
        arr[$(index $found $j)]=$temp
    done
    divisor=$(item $string $string)
    for (( j=0; j < $mod; j++ ))
    do
        arr[$(index $string $j)]=$(count "($(item $string $j))/($divisor)")
    done
    for (( another=0; another < $n; another++ ))
    do
        if [ $another -eq $string ]; then
            continue
        fi;
        multiplier=$(item $another $string)
        for (( j=0; j < $mod; j++ ))
        do
            arr[$(index $another $j)]=$(count "($(item $another $j))-($(item $string $j))*($multiplier)")
        done
    done
done

for (( i=0; i < $n; i++ ))
do
    str=$(item $i $n)
    echo ${str:0:40}
done
