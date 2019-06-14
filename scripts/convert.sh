#!/bin/bash

#
# Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
#
# SPDX-License-Identifier: Apache-2.0
#
# Script to enforce the coding style of this project.
#

if ! [ -x "$(command -v inkscape)" ]; then
    echo 'Error: inkscape is not installed.' >&2
    echo 'install inkscape: apt-get install inkscape' >&2
    exit 1
fi

declare -a create_dirs=(16px 32px 64px 128px)

do_convert()
{
    dir="$1"
    filename="${2%.svg}"
    filename="${filename##*/}"
    # special handling of logo files
    if [[ $filename == *"logo"* ]]; then
	echo "Converting using inkscape: $2 to ./icons/$dir/$filename.png of size=${size}x"
	inkscape -z -e ./icons/$dir/$filename.png -w $size $2
    else
	echo "Converting using inkscape: $2 to ./icons/"$dir/$filename".png of size="$size"x"$size" "
	inkscape -z -e ./icons/$dir/$filename.png -w $size -h $size $2
    fi
}

for dir in "${create_dirs[@]}"
do
    echo "creating directory ./icons/$dir"
    mkdir -p ./icons/$dir

    size="${dir%px}"

    if [[ $# -eq 0 ]]; then
	for file in ./icons/svg/*.svg; do
	    do_convert $dir $file
	done
    else
	do_convert $dir $1
    fi
done
