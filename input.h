/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cassert>
#include "geometry.h"

using namespace std;

enum
{
	MOUSE_DOWN = 1,
	MOUSE_UP,
	MOUSE_MOVE
};

const Point& mouse_position();

class Input
{
public:

};

class InputEvDev : public Input
{
public:

	InputEvDev(const string& path);

	static void process(int fd, uint32_t mask, void *data);

	virtual ~InputEvDev();

private:
	int m_fd;
};


class InputTslib : public Input
{
public:

	InputTslib(const string& path);

	static void process(int fd, uint32_t mask, void *data);

	virtual ~InputTslib();

private:
	int m_fd;
};

#endif
