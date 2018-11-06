#!/bin/sh

#
# Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
# Joshua Henderson <joshua.henderson@microchip.com>
#
# Script to enforce the coding style of this project.
#

set -x

OPTIONS="--style=allman \
--suffix=none \
--indent-namespaces
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

~/Downloads/astyle/astyle ${OPTIONS} $@
