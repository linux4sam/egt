#include "egt/button.h"
#include "egt/input.h"
#include "egt/keyboard.h"
#include "egt/popup.h"
#include "egt/sizer.h"

using namespace std;

namespace egt
{
inline namespace v1
{

Key::Key(std::string label, int link, double length)
    : Button(label),
      m_link(link),
      m_length(length)
{}

Key::Key(std::string label, shared_ptr<Panel> multichoice, double length)
    : Button(label),
      m_link(-1),
      m_length(length),
      m_multichoice(multichoice)
{
}

double Key::length() const
{
    return m_length;
}

int Key::link() const
{
    return m_link;
}

Panel::Panel(std::vector<std::vector<std::shared_ptr<Key>>> keys, Size key_size)
    : VerticalBoxSizer(),
      m_keys(keys)
{
    set_align(alignmask::center);

    for (auto& row : m_keys)
    {
        auto hsizer = make_shared<HorizontalBoxSizer>();
        hsizer->set_align(alignmask::center | alignmask::top);
        add(hsizer);

        for (auto& key : row)
        {
            key->resize(Size(key_size.w * key->length(), key_size.h));
            key->set_border(1);

            hsizer->add(key);
        }
    }
}

Keyboard::Keyboard(std::vector<shared_ptr<Panel>> panels, Size size)
    : Frame(Rect(Point(), size))
{
    m_main_panel.set_align(alignmask::expand);
    add(m_main_panel);
    m_multichoice_popup.resize(Size(100, 100));
    add(m_multichoice_popup);

    unsigned multichoice_id = 0;

    for (auto& panel : panels)
    {
        for (auto& row : panel->m_keys)
        {
            for (auto& key : row)
            {
                if (key->link() >= 0)
                {
                    key->on_event([this, key](eventid event)
                    {
                        m_main_panel.set_select(key->link());

                        return 0;
                    }, {eventid::pointer_click});
                }
                else
                {
                    key->on_event([this, key](eventid event)
                    {
                        if (!key->text().empty())
                        {
                            if (event == eventid::raw_pointer_down)
                            {
                                m_in.m_keys.key = key->text()[0];
                                m_in.m_keys.code = 0;
                                m_in.dispatch(eventid::keyboard_down);
                            }
                            else if (event == eventid::raw_pointer_up)
                            {
                                m_in.m_keys.key = key->text()[0];
                                m_in.m_keys.code = 0;
                                m_in.dispatch(eventid::keyboard_up);
                            }
                        }

                        return 0;
                    });

                    if (key->m_multichoice)
                    {
                        for (auto& multichoice_raw : key->m_multichoice->m_keys)
                        {
                            for (auto& multichoice_key : multichoice_raw)
                            {
                                multichoice_key->on_event([this, key, multichoice_key](eventid event)
                                {
                                    // hide popup first as it is modal
                                    m_multichoice_popup.hide();

                                    if (!multichoice_key->text().empty())
                                    {
                                        m_in.m_keys.key = multichoice_key->text()[0];
                                        m_in.m_keys.code = 0;
                                        m_in.dispatch(eventid::keyboard_down);
                                        m_in.dispatch(eventid::keyboard_up);
                                        // the modal popup caught the raw_pointer_up event
                                        key->set_active(false);
                                    }

                                    return 0;
                                }, {eventid::raw_pointer_up}); //user may just move his finger
                            }
                        }

                        auto m = make_shared<NotebookTab>();
                        m->add(key->m_multichoice);
                        m_multichoice_popup.m_notebook.add(m);

                        key->on_event([this, key, multichoice_id](eventid event)
                        {
                            m_multichoice_popup.m_notebook.set_select(multichoice_id);
                            m_multichoice_popup.resize(key->m_multichoice->size());
                            m_multichoice_popup.show_modal(true);

                            return 0;
                        }, {eventid::pointer_hold});

                        ++multichoice_id;
                    }
                }
            }
        }

        auto p = make_shared<NotebookTab>();
        p->add(panel);
        m_main_panel.add(p);
    }
}

}
}
