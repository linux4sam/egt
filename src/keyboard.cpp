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
using Key = Keyboard::Key;
using Panel = Keyboard::Panel;
using MainPanel = Keyboard::MainPanel;
using MultichoicePanel = Keyboard::MultichoicePanel;

Key::Key(std::string label, double length)
    : Button(label),
      m_length(length)
{}

Key::Key(std::string label, int link, double length)
    : Button(label),
      m_link(link),
      m_length(length)
{}

Key::Key(std::string label, shared_ptr<MultichoicePanel> multichoice, double length)
    : Button(label),
      m_link(-1),
      m_length(length),
      m_multichoice(multichoice)
{}

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

MainPanel::MainPanel(std::vector<std::vector<std::shared_ptr<Key>>> keys,
                     Size key_size)
    : NotebookTab(),
      m_panel(make_shared<Panel>(keys, key_size))
{
    add(m_panel);
}

MultichoicePanel::MultichoicePanel(std::vector<std::vector<std::shared_ptr<Key>>> keys,
                                   Size key_size)
    : NotebookTab(),
      m_panel(make_shared<Panel>(keys, key_size))
{
    add(m_panel);
}

void Keyboard::set_key_link(const shared_ptr<Key>& k)
{
    k->on_event([this, k](eventid event)
    {
            m_main_panel.set_select(k->link());

            return 0;
    }, {eventid::pointer_click});
}

void Keyboard::set_key_input_value(const shared_ptr<Key>& k)
{
    k->on_event([this, k](eventid event)
    {
        if (!k->text().empty())
        {
            if (event == eventid::raw_pointer_down)
            {
                m_in.m_keys.key = k->text()[0];
                m_in.m_keys.code = 0;
                m_in.dispatch(eventid::keyboard_down);
            }
            else if (event == eventid::raw_pointer_up)
            {
                    m_in.m_keys.key = k->text()[0];
                    m_in.m_keys.code = 0;
                    m_in.dispatch(eventid::keyboard_up);
            }
        }

        return 0;
    });
}

void Keyboard::set_key_multichoice(const shared_ptr<Key>& k, unsigned id)
{
    for (auto& multichoice_raw : k->m_multichoice->m_panel->m_keys)
    {
        for (auto& multichoice_key : multichoice_raw)
        {
            multichoice_key->on_event([this, k, multichoice_key](eventid event)
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
                    k->set_active(false);
                }

                return 0;
            }, {eventid::raw_pointer_up}); //user may just move his finger
        }
    }

    m_multichoice_popup.m_notebook.add(k->m_multichoice);

    k->on_event([this, k, id](eventid event)
    {
        m_multichoice_popup.m_notebook.set_select(id);
        m_multichoice_popup.resize(k->m_multichoice->m_panel->size());
        m_multichoice_popup.show_modal(true);

        return 0;
    }, {eventid::pointer_hold});
}

Keyboard::Keyboard(std::vector<shared_ptr<MainPanel>> panels, Size size)
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
