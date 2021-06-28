#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
# All rights reserved.
#
# Script to get, if applicable, the current state of the git tree.
#

if ! [ -x "$(command -v git)" ] ; then
    echo "n/a"
    exit 0
fi

GIT_COMMIT=$(git log -1 --pretty='%h' 2> /dev/null)
if [ $? -ne 0 ] ; then
    echo "n/a"
    exit 0
fi

git diff --quiet
if [ $? -ne 0 ] ; then
    GIT_DIRTY="-dirty" ;
fi

echo "$GIT_COMMIT$GIT_DIRTY"
