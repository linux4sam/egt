/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_UI_H
#define MUI_UI_H

/**
 * @file
 * @brief Single header to include basic and common functionality.
 */

#include <mui/animation.h>
#include <mui/app.h>
#include <mui/button.h>
#include <mui/chart.h>
#include <mui/color.h>
#include <mui/event_loop.h>
#include <mui/font.h>
#include <mui/frame.h>
#include <mui/geometry.h>
#include <mui/grid.h>
#include <mui/image.h>
#include <mui/imagecache.h>
#include <mui/input.h>
#include <mui/kmsscreen.h>
#include <mui/label.h>
#include <mui/palette.h>
#include <mui/screen.h>
#include <mui/sprite.h>
#include <mui/text.h>
#include <mui/timer.h>
#include <mui/tools.h>
#include <mui/valuewidget.h>
#include <mui/video.h>
#include <mui/widget.h>
#include <mui/window.h>
#include <mui/x11screen.h>

/**
 * This is a wrapper around gettext.
 * @todo Move me.
 */
#include <libintl.h>
#include <locale.h>

#define _(String) gettext(String)

#endif
