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
using Panel = Keyboard::Panel;
using MainPanel = Keyboard::MainPanel;
using MultichoicePanel = Keyboard::MultichoicePanel;

Keyboard::Key::Key(const string& label, double length)
    : Button(label),
      m_length(length)
{}

Keyboard::Key::Key(const string& label, int link, double length)
    : Button(label),
      m_link(link),
      m_length(length)
{}

Keyboard::Key::Key(const string& label, shared_ptr<MultichoicePanel> multichoice, double length)
    : Button(label),
      m_length(length),
      m_multichoice(multichoice)
{}

Panel::Panel(vector<vector<shared_ptr<Keyboard::Key>>> keys, Size key_size)
    : m_keys(std::move(keys))
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

MainPanel::MainPanel(vector<vector<shared_ptr<Key>>> keys,
                     Size key_size)
    : m_panel(make_shared<Panel>(keys, key_size))
{
    add(m_panel);
}

MultichoicePanel::MultichoicePanel(vector<vector<shared_ptr<Key>>> keys,
                                   Size key_size)
    : m_panel(make_shared<Panel>(keys, key_size))
{
    add(m_panel);
}

void Keyboard::set_key_link(const shared_ptr<Key>& k)
{
    k->on_event([this, k](Event&)
    {
        m_main_panel.set_select(k->link());
    }, {eventid::pointer_click});
}

void Keyboard::set_key_input_value(const shared_ptr<Key>& k)
{
    k->on_event([this, k](Event & event)
    {
        if (!k->text().empty())
        {
            Event event2(eventid::keyboard_down);
            event2.key().key = k->text()[0];
            event2.key().code = 0;
            m_in.dispatch(event2);

            event2.set_id(eventid::keyboard_up);
            m_in.dispatch(event2);
        }

        return 0;
    }, {eventid::pointer_click});
}

void Keyboard::set_key_multichoice(const shared_ptr<Key>& k, unsigned id)
{
    for (auto& multichoice_raw : k->m_multichoice->m_panel->m_keys)
    {
        for (auto& multichoice_key : multichoice_raw)
        {
            multichoice_key->on_event([this, k, multichoice_key](Event & event)
            {
                // hide popup first as it is modal
                m_multichoice_popup.hide();

                if (!multichoice_key->text().empty())
                {
                    Event down(eventid::keyboard_down);
                    down.key().key = multichoice_key->text()[0];
                    down.key().code = 0;
                    m_in.dispatch(down);
                    Event up(eventid::keyboard_up);
                    up.key().key = multichoice_key->text()[0];
                    up.key().code = 0;
                    m_in.dispatch(up);
                    // the modal popup caught the raw_pointer_up event
                    k->set_active(false);
                }
            }, {eventid::raw_pointer_up}); //user may just move his finger
        }
    }

    m_multichoice_popup.m_notebook.add(k->m_multichoice);

    k->on_event([this, k, id](Event&)
    {
        m_multichoice_popup.m_notebook.set_select(id);
        m_multichoice_popup.resize(k->m_multichoice->m_panel->size());

        auto display_origin = k->local_to_display(k->point());
        auto keyboard_origin = display_to_local(display_origin);
        // Popup on top of the widget.
        keyboard_origin.y -= m_multichoice_popup.size().h;
        // Popup centered on button center.
        keyboard_origin.x -= m_multichoice_popup.size().w / 2;
        keyboard_origin.x += k->size().w / 2;

        m_multichoice_popup.move(keyboard_origin);
        m_multichoice_popup.show_modal();
    }, {eventid::pointer_hold});
}

Keyboard::Keyboard(vector<shared_ptr<MainPanel>> panels, Size size)
    : Frame(Rect(Point(), size))
{
    m_main_panel.set_align(alignmask::expand);
    add(m_main_panel);
    m_multichoice_popup.resize(Size(100, 100));
    add(m_multichoice_popup);

    unsigned multichoice_id = 0;

    for (auto& panel : panels)
    {
        for (auto& row : panel->m_panel->m_keys)
        {
            for (auto& key : row)
            {
                if (key->link() >= 0)
                {
                    set_key_link(key);
                }
                else
                {
                    set_key_input_value(key);

                    if (key->m_multichoice)
                        set_key_multichoice(key, multichoice_id++);
                }
            }
        }

        m_main_panel.add(panel);
    }
}

}
}
