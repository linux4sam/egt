/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_FMT_H
#define EGT_DETAIL_FMT_H

// <fmt/format.h> includes <windows.h>.  asio *requires* following the general
// rule, to include winsock2.h before windows.h, which does not happen because
// fmt does not include any winsock header.  So, force wrap {fmt} and include
// <winsock2.h> before <windows.h>.

#ifdef WIN32
#include <winsock2.h>
#endif

#include <fmt/ostream.h>
#include <fmt/format.h>

#endif
