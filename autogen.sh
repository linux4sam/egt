#! /bin/sh

#
# Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
#
# SPDX-License-Identifier: Apache-2.0
#

modules_initialized=0
SUBMODULES=$(cat .gitmodules | grep 'path.*=' | cut -d'=' -f2)
for submod in $SUBMODULES; do
    if [ -e $submod/.git ]; then
	modules_initialized=1
    fi
done

# don't update if there are any submodule directories
if [ -f .gitmodules ] && [ -d .git ] && [ $modules_initialized = 0 ]; then
    git submodule update --init --recursive
fi

autoreconf --force --verbose --install || exit 1
