#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
# All rights reserved.
#
# Script to enforce the coding style of this project.
#

if ! [ -x "$(command -v git)" ] ; then
    echo "not available"
    exit 1
fi

GIT_COMMIT=$(git log -1 --pretty='%h' 2> /dev/null)
if [ $? -ne 0 ] ; then
    echo "not available"
    exit 1
fi

git diff --quiet
if [ $? -ne 0 ] ; then
    GIT_DIRTY="-dirty" ;
fi

echo "$GIT_COMMIT$GIT_DIRTY"
