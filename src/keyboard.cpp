#include "egt/button.h"
#include "egt/input.h"
#include "egt/keyboard.h"
#include "egt/sizer.h"

using namespace std;

namespace egt
{
inline namespace v1
{

Key::Key(std::string label, double length)
    : Button(label),
      m_length(length)
{}

double Key::length() const
{
    return m_length;
}

Panel::Panel(std::vector<std::vector<std::shared_ptr<Key>>> k, Size size)
    : NotebookTab()
{
    m_sizer.set_align(alignmask::expand);
    add(m_sizer);

    for (auto& row : k)
    {
        auto hsizer = make_shared<HorizontalBoxSizer>();
        hsizer->set_align(alignmask::center | alignmask::top);
        hsizer->set_justify(justification::start);
        m_sizer.add(hsizer);

        for (auto& key : row)
        {
            key->resize(Size(size.w * key->length(), size.h));
            key->set_border(1);

            key->on_event([this, key](eventid event)
            {
                if (!key->text().empty())
                {
                    if (event == eventid::raw_pointer_down)
                    {
                        m_keyboard->m_in.m_keys.key = key->text()[0];
                        m_keyboard->m_in.m_keys.code = 0;
                        m_keyboard->m_in.dispatch(eventid::keyboard_down);
                    }
                    else if (event == eventid::raw_pointer_up)
                    {
                        m_keyboard->m_in.m_keys.key = key->text()[0];
                        m_keyboard->m_in.m_keys.code = 0;
                        m_keyboard->m_in.dispatch(eventid::keyboard_up);
                    }
                }

                return 0;
            });

            hsizer->add(key);
        }
    }
}

void Panel::set_keyboard(Keyboard* keyboard)
{
    m_keyboard = keyboard;
}

Keyboard::Keyboard(std::vector<shared_ptr<Panel>> panels, Size size)
    : Notebook(Rect(Point(), size))
{
    set_border(1);
    /* Create panels from their description */
    for (auto& panel : panels)
    {
        panel->set_keyboard(this);
        add(panel);
    }
}

}
}
