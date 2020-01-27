/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_EMBED_H
#define EGT_EMBED_H

/**
 * @file
 * @brief Working with embedded files.
 */

#define INCBIN_PREFIX resource_
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include <egt/detail/incbin.h>
#include <egt/resource.h>

/**
 * @def EGT_EMBED
 *
 * Embed a file into the compilation unit this is called from and register it as
 * an EGT resource.
 *
 * @param name Name of the resource.  This must be a safe C++ variable name.
 * @param path Path of the file to include.
 *
 * @b Example:
 * @code{.cpp}
 * EGT_EMBED(my_image, "../images/my_image.png");
 *
 * Image my_image(":my_image");
 * @endcode
 */
#define EGT_EMBED(name, path)						\
  INCBIN(name, path);							\
  namespace egt { namespace resources {					\
      struct resource_initializer ## name {				\
	resource_initializer ## name() {				\
	  egt::ResourceManager::instance().add(#name,			\
					 resource_ ## name ## _data,	\
					 resource_ ## name ## _size);	\
	}								\
      } resource_initializer ## name;					\
    }}

#endif
