/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "kmsscreen.h"
#include "window.h"
#include <algorithm>

using namespace std;

namespace mui
{
    static std::vector<SimpleWindow*> the_windows;
    static SimpleWindow* the_window = 0;

    SimpleWindow*& main_window()
    {
	return the_window;
    }

    std::vector<SimpleWindow*>& windows()
    {
	return the_windows;
    }

    SimpleWindow::SimpleWindow(int w, int h)
	: Widget(0, 0, w, h),
	  m_bgcolor(BG_COLOR)
    {
	cout << "new window " << w << "," << h << endl;
	windows().push_back(this);

	if (!the_window)
	{
	    the_window = this;
	    active(true);
	}

	damage();

	// go ahead and pick up the default screen
	m_screen = main_screen();
    }

    void SimpleWindow::add(Widget* widget)
    {
	if (find(m_children.begin(), m_children.end(), widget) == m_children.end())
	{
	    assert(!widget->m_parent);
	    widget->m_parent = this;
	    //widget->damage();
	    m_children.push_back(widget);
	}
    }

    void SimpleWindow::damage()
    {
	damage(m_box);
    }

    void SimpleWindow::active(bool value)
    {

	if (m_active != value)
	{
	    if (value)
	    {
		damage();
	    }

	    m_active = value;
	}
    }

/**
 * Mark the specified rect as a damaged area.
 *
 * This will merge the damaged area with any already existing damaged area that
 * it overlaps with into a super rectangle. Then, the whole array has to be
 * checked again to make sure the new rectangle doesn't conflict with another
 * existing rectangle.
 *
 * It may be worth investigating if this is optimal instead of removing the
 * overlap and still having two rectangles.  In some cases, this could result in
 * significantly less pixels.
 */
    void SimpleWindow::damage(const Rect& rect)
    {
#if 0
	if (!rect.is_clear())
	{
	    bool found = false;
	    for (auto& damage: m_damage)
	    {
		if (Rect::is_intersect(damage,rect))
		{
		    found = true;
		    damage = Rect::merge(damage,rect);
		}
	    }

	    if (!found)
		m_damage.push_back(rect);
	}
#else
	if (!rect.is_clear())
	{
	    for (auto i = m_damage.begin(); i != m_damage.end(); ++i)
	    {
		if (Rect::is_intersect(*i,rect))
		{
		    Rect merged(Rect::merge(*i,rect));
		    m_damage.erase(i);
		    damage(merged);
		    return;
		}
	    }

	    // if we get here, no intersect found so add it
	    m_damage.push_back(rect);
	}

#endif
    }

    int SimpleWindow::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	case EVT_MOUSE_UP:
	case EVT_MOUSE_MOVE:
	    for (auto& child: reverse_iterate(m_children))
	    {
		if (Rect::point_inside(mouse_position(), child->box()))
		    if (child->handle(event))
			return 1;
	    }
	    break;
	case EVT_KEYDOWN:
	case EVT_KEYUP:
	    for (auto& child: reverse_iterate(m_children))
	    {
		if (child->focus())
		    if (child->handle(event))
			return 1;
	    }
	    break;
	}


	return Widget::handle(event);
    }

    void SimpleWindow::draw()
    {
	for (auto& damage: m_damage)
	{
	    screen()->rect(damage, m_bgcolor);

	    for (auto& child: m_children)
	    {
		if (Rect::is_intersect(damage,child->box()))
		{
		    child->draw(damage);
		}
	    }
	}

	if (!m_damage.empty())
	    screen()->flip(m_damage);

	m_damage.clear();
    }

    void SimpleWindow::draw(const Rect& rect)
    {
	draw();
    }

    PlaneWindow::PlaneWindow(int w, int h)
	: SimpleWindow(w, h)
    {
	// default plane windows to transparent
	m_bgcolor = Color(0,0,0,0);

	assert(KMSScreen::instance());

	if (this->w() == 0 || this->h() == 0)
	{
	    m_box.w = KMSScreen::instance()->size().w;
	    m_box.h = KMSScreen::instance()->size().h;
	}

	m_screen = new KMSOverlayScreen(
	    KMSScreen::instance()->allocate_overlay(Size(this->w(),this->h())));
    }

    void PlaneWindow::position(int x, int y)
    {
	if (x != box().x || y != box().y)
	{
	    KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	    screen->position(x, y);

	    m_box.x = x;
	    m_box.y = y;
	}
    }

    void PlaneWindow::move(int x, int y)
    {
	position(x, y);
    }

    void PlaneWindow::draw()
    {
	KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	s->apply();

	for (auto& damage: m_damage)
	{
	    screen()->rect(damage, m_bgcolor);

	    for (auto& child: m_children)
	    {
		if (Rect::is_intersect(damage,child->box()))
		{
		    child->draw(damage);
		}
	    }
	}

	if (!m_damage.empty())
	    screen()->flip(m_damage);

	m_damage.clear();

    }

    PlaneWindow::~PlaneWindow()
    {
    }
}
