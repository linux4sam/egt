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

SRC_TREE=$1
cd "$SRC_TREE"

# Check the top-level directory of the git repository. If it returns a string,
# it means the current directory is not the top-level one, don't use the git
# commit id in this case as it's not related to EGT.
if ! [ -z "$(git rev-parse --show-cdup 2>/dev/null)" ] ; then
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
