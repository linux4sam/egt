#!/bin/sh

#
# Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
#
# SPDX-License-Identifier: Apache-2.0
#
# Script to enforce the coding style of this project.
#

if [ "$(astyle -V)" \< "Artistic Style Version 3.1" ]
then
    echo "Your astyle version is too old. Update astyle to at least version 3.1"
    exit 1
fi

set -x

OPTIONS="--style=allman \
--suffix=none \
--unpad-paren \
--pad-comma \
--pad-oper \
--pad-header \
--mode=c \
--preserve-date \
--lineend=linux \
--indent-col1-comments \
--align-reference=type \
--align-pointer=type \
--keep-one-line-statements \
--keep-one-line-blocks \
--min-conditional-indent=0 \
--attach-closing-while \
--indent=spaces=4"

astyle ${OPTIONS} $@
