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

declare -a create_dirs=(16px 24px 48px 64px 72px 96px 128px)

for dir in "${create_dirs[@]}"
do
	echo "creating directory ./icons/$dir"
	mkdir -p ./icons/$dir

	size="${dir%px}"

	if [[ $# -eq 0 ]]; then
		for file in ./icons/svg/*.svg; do
			filename="${file%.svg}"
			filename="${filename##*/}"
			echo "Converting using inkscape: $file to ./icons/"$dir/$filename".png of size="$size"x"$size" "
			inkscape -z -e ./icons/$dir/$filename.png -w $size -h $size $file
		done
	else
		filename="${1%.svg}"
		filename="${filename##*/}"
		echo "Converting using inkscape: $1 to ./icons/"$dir/$filename".png of size="$size"x"$size" "
		inkscape -z -e ./icons/$dir/$filename.png -w $size -h $size $1
	fi
done
