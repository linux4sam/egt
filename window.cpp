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

    SimpleWindow::SimpleWindow(const Size& size, uint32_t flags)
	: Widget(0, 0, size.w, size.h, flags)
    {
	cout << "new window " << size << endl;
	windows().push_back(this);

	if (!the_window)
	{
	    the_window = this;
	    show();
	}
	else
	{
	    hide();
	}

	if (size.empty())
	{
	    assert(main_screen());

	    m_box.w = main_screen()->size().w;
	    m_box.h = main_screen()->size().h;
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

    void SimpleWindow::remove(Widget* widget)
    {
	auto i = find(m_children.begin(), m_children.end(), widget);
	if (i != m_children.end())
	{
	    (*i)->damage();
	    m_children.erase(i);
	}
    }

    void SimpleWindow::damage()
    {
	damage(m_box);
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
		if (child->disabled() && !child->active())
		    continue;

		Point pos = mouse_position() - box().point();
		if (Rect::point_inside(pos, child->box()))
		    if (child->handle(event))
			return 1;
	    }
	    break;
	case EVT_KEYDOWN:
	case EVT_KEYUP:
	    for (auto& child: reverse_iterate(m_children))
	    {
		if (child->disabled())
		    continue;

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
	if (m_damage.empty())
	    return;

	for (auto& damage: m_damage)
	{
	    if (!is_flag_set(FLAG_NO_BACKGROUND))
		screen()->rect(damage, palette().color(Palette::BG));

	    for (auto& child: m_children)
	    {
		if (!child->visible())
		    continue;

		if (child->is_flag_set(FLAG_PLANE_WINDOW))
		    continue;

		if (Rect::is_intersect(damage,child->box()))
		{
		    // don't give a child a rectangle that is outside of its own box
		    Rect r = Rect::intersect(damage, child->box());
		    child->draw(r);
		}
	    }
	}

	screen()->flip(m_damage);
	m_damage.clear();
    }

    void SimpleWindow::draw(const Rect& rect)
    {
	draw();
    }

#ifdef HAVE_LIBPLANES
    PlaneWindow::PlaneWindow(const Size& size, uint32_t flags, uint32_t format, bool heo)
	: SimpleWindow(size, flags | FLAG_PLANE_WINDOW),
	  m_format(format),
	  m_dirty(true),
	  m_heo(heo)
    {
	// default plane windows to transparent
	Palette p(palette());
	p.set(Palette::BG, Palette::GROUP_NORMAL, Color(0,0,0,0));
	set_palette(p);

	assert(KMSScreen::instance());

	if (!size.empty())
	{
	    resize(this->w(),this->h());
	}
    }

    void PlaneWindow::resize(int w, int h)
    {
	m_screen = new KMSOverlayScreen(
	    KMSScreen::instance()->allocate_overlay(Size(w,h), m_format, m_heo));
	assert(m_screen);
	size(w, h);
	damage();
    }

    void PlaneWindow::position(int x, int y)
    {
	if (x != box().x || y != box().y)
	{
	    KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	    screen->position(x, y);

	    m_box.x = x;
	    m_box.y = y;

	    m_dirty = true;
	}
    }

    void PlaneWindow::move(int x, int y)
    {
	position(x, y);
    }

    void PlaneWindow::draw()
    {
	if (!m_screen)
	    return;

	//if (m_dirty)
	//{
	KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	s->apply();
	m_dirty = false;
	//}

	if (m_damage.empty())
	    return;

	for (auto& damage: m_damage)
	{
	    if (!is_flag_set(FLAG_NO_BACKGROUND))
		screen()->rect(damage, palette().color(Palette::BG));

	    for (auto& child: m_children)
	    {
		if (!child->visible())
		    continue;

		if (child->is_flag_set(FLAG_PLANE_WINDOW))
		    continue;

		if (Rect::is_intersect(damage,child->box()))
		{
		    // don't give a child a rectangle that is outside of its own box
		    Rect r = Rect::intersect(damage, child->box());
		    child->draw(r);
		}
	    }
	}

	screen()->flip(m_damage);
	m_damage.clear();
    }

#else
    PlaneWindow::PlaneWindow(const Size& size, uint32_t flags, uint32_t format, bool heo)
	: SimpleWindow(size, flags),
	  m_dirty(true)
    {

    }

    void PlaneWindow::position(int x, int y)
    {
	SimpleWindow::position(x,y);
    }

    void PlaneWindow::move(int x, int y)
    {
	SimpleWindow::move(x,y);
    }

    void PlaneWindow::draw()
    {
	SimpleWindow::draw();
    }
#endif

    PlaneWindow::~PlaneWindow()
    {
    }

}
