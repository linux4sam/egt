/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "widget.h"
#include "screen.h"
#include "event_loop.h"
#include "input.h"

#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

static float sliding_scale(int win_w, int item_w, int item_pos,
			   float min = 0.3, float max = 2.0)
{
	float range = win_w / 2;
	float delta = fabs(range - (item_pos + (item_w / 2)));
	float scale = 1.0 - (delta / range);
	if (scale < min || scale > max)
		return min;
	return scale;
}

#define NUM_ITEMS 10

class MyWindow : public Window
{
public:
	MyWindow()
		: Window(800,480),
		  m_moving(false)
	{}

	int load()
	{
		Image* img = new Image("background.png");
		add(img);

		for (int t = 0; t < NUM_ITEMS; t++)
		{
			stringstream os;
			os << "_image" << t << ".png";
			Image* box = new Image(os.str());
			box->position(t * 180, (h() / 2) - (box->h() / 2));
			add(box);
			m_boxes[t] = box;

			process(t, t * 180);
		}

		return 0;
	}

	int handle(int event)
	{
		switch (event)
		{
		case MOUSE_DOWN:
			if (!m_moving)
			{
				m_moving = true;
				m_moving_x = mouse_position().x;
				for (int t = 0; t < NUM_ITEMS; t++)
					m_positions[t] = m_boxes[t]->x();
			}

			return 1;
			break;
		case MOUSE_UP:
			m_moving = false;
			return 1;
			break;
		case MOUSE_MOVE:

			if (m_moving)
			{
				move();
				return 1;
			}
			break;
		}

		return Window::handle(event);
	}

	void move()
	{
		int diff = mouse_position().x - m_moving_x;

		for (int t = 0; t < NUM_ITEMS; t++)
		{
			int pos = m_positions[t] + diff;

			Rect to(m_boxes[t]->box());
			to.x = pos;
			bool visible = Rect::is_intersect(Rect::merge(to,m_boxes[t]->box()), this->box());

			if (visible)
			{
				//int delta = m_boxes[t]->x() - pos;
				//if (abs(delta) < 10)
				//	return;

				process(t, pos);
				m_boxes[t]->move(pos, m_boxes[t]->y());
			}
		}
	}

	void process(int t, int pos)
	{
		float scale = sliding_scale(this->w(), m_boxes[t]->w(), pos);
		m_boxes[t]->scale(scale);
	}

private:
	bool m_moving;
	int m_moving_x;
	int m_positions[NUM_ITEMS];
	Image* m_boxes[NUM_ITEMS];
};

int main()
{
	EventLoop::init();
	FrameBuffer fb("/dev/fb0");
	InputTslib input0("/dev/input/touchscreen0");
	InputEvDev input1("/dev/input/event4");

	MyWindow win;
	win.load();

	EventLoop::run();

	return 0;
}
