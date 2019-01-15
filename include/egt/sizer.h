/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SIZER_H
#define EGT_SIZER_H

/**
 * @file
 * @brief Working with sizers.
 */

#include <egt/frame.h>

namespace egt
{
    inline namespace v1
    {
        class BoxSizer : public Frame
        {
        public:

            BoxSizer(orientation orient = orientation::HORIZONTAL, int spacing = 0)
                : Frame(Rect(), widgetmask::NO_BACKGROUND),
                  m_spacing(spacing),
                  m_orient(orient)
            {
                static auto boxsizer_id = 0;

                std::ostringstream ss;
                ss << "BoxSizer" << boxsizer_id++;
                set_name(ss.str());
            }

            virtual void move(const Point& point) override
            {
                Frame::move(point);
                reposition();
            }

            virtual void resize(const Size& size) override
            {
                Frame::resize(size);
                reposition();
            }

            virtual Widget* add(Widget* widget) override
            {
                Frame::add(widget);
                reposition();
                return widget;
            }

            virtual void remove(Widget* widget) override
            {
                Frame::add(widget);
                reposition();
            }

            /**
             * Reposition all child widgets.
             */
            virtual void reposition()
            {
                if (box().size().empty())
                    return;

                if (m_orient == orientation::VERTICAL)
                {
                    Rect bounding = Rect(Point(m_spacing, m_spacing), box().size() - Size(m_spacing, m_spacing));
                    for (auto& child : m_children)
                    {
                        Rect target = align_algorithm(child->box().size(),
                                                      bounding,
                                                      child->align(),
                                                      child->margin());

                        // reposition/resize widget
                        child->move(target.point());
                        child->resize(target.size());

                        bounding += Point(0, target.point().y + target.size().h + m_spacing);
                        bounding -= Size(0, target.size().h);
                    }

                }
                else
                {
                    Rect bounding = Rect(Point(m_spacing, m_spacing), box().size() - Size(m_spacing, m_spacing));
                    for (auto& child : m_children)
                    {
                        Rect target = align_algorithm(child->box().size(),
                                                      bounding,
                                                      child->align(),
                                                      child->margin());

                        // reposition/resize widget
                        child->move(target.point());
                        child->resize(target.size());

                        bounding += Point(target.point().x + target.size().w + m_spacing, 0);
                        bounding -= Size(target.size().w, 0);
                    }
                }

                damage();
            }

            virtual ~BoxSizer()
            {}

        protected:
            int m_spacing{0};
            orientation m_orient{orientation::HORIZONTAL};
        };


    }
}

#endif
