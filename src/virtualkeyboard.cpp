#include "detail/utf8text.h"
#include "egt/button.h"
#include "egt/input.h"
#include "egt/keycode.h"
#include "egt/popup.h"
#include "egt/sizer.h"
#include "egt/virtualkeyboard.h"

using namespace std;

namespace egt
{
inline namespace v1
{

VirtualKeyboard::Key::Key(uint32_t unicode, double length) noexcept
    : m_button(make_shared<Button>()),
      m_unicode(unicode),
      m_length(length)
{
    string tmp;
    utf8::append(unicode, std::back_inserter(tmp));
    m_button->set_text(tmp);
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(const std::string& label, KeyboardCode keycode,
                          double length) noexcept
    : m_button(make_shared<Button>(label)),
      m_keycode(keycode),
      m_length(length)
{
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(const Image& img, KeyboardCode keycode,
                          double length) noexcept
    : m_button(make_shared<ImageButton>(img)),
      m_keycode(keycode),
      m_length(length)
{
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(const string& label, int link,
                          double length) noexcept
    : m_button(make_shared<Button>(label)),
      m_length(length),
      m_link(link)
{
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(const Image& img, int link,
                          double length) noexcept
    : m_button(make_shared<ImageButton>(img)),
      m_length(length),
      m_link(link)
{
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(uint32_t unicode,
                          panel_keys keys_multichoice,
                          double length) noexcept
    : m_button(make_shared<Button>()),
      m_unicode(unicode),
      m_length(length),
      m_keys_multichoice(keys_multichoice)
{
    string tmp;
    utf8::append(unicode, std::back_inserter(tmp));
    m_button->set_text(tmp);
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

void VirtualKeyboard::Key::set_color(Palette::ColorId id, const Palette::pattern_type& color, Palette::GroupId group)
{
    m_button->set_color(id, color, group);
}

void VirtualKeyboard::Key::set_font(const Font& font)
{
    m_button->set_font(font);
}

VirtualKeyboard::VirtualKeyboard(vector<panel_keys> keys, const Rect& rect)
    : Frame(rect)
{
    m_main_panel.set_align(alignmask::expand);
    add(m_main_panel);

    for (auto& keys_panel : keys)
    {
        m_max_rows = std::max(m_max_rows, unsigned(keys_panel.size()));

        for (auto& keys_row : keys_panel)
            m_max_cols = std::max(m_max_cols, unsigned(keys_row.size()));
    }

    for (auto& keys_panel : keys)
    {
        auto panel = make_shared<Panel>(keys_panel);
        panel->set_align(alignmask::expand);
        panel->update_key_space(m_key_space);
        m_main_panels.push_back(panel);

        auto main_panel = make_shared<NotebookTab>();
        main_panel->add(panel);
        main_panel->set_align(alignmask::expand);
        // By default NotebookTab are not transparent.
        main_panel->set_boxtype(Theme::boxtype::none);
        m_main_panel.add(main_panel);

        for (auto& keys_row : keys_panel)
        {
            for (auto& key : keys_row)
            {
                set_key_link(key);
                set_key_input_value(key);
                if (!key->m_keys_multichoice.empty())
                {
                    set_key_multichoice(key);

                }
            }
        }
    }
}

VirtualKeyboard::VirtualKeyboard(const Rect& rect) noexcept
    : VirtualKeyboard(
{
    QwertyLettersLowerCase(),
                           QwertyLettersUpperCase(),
                           QwertySymbols1(),
                           QwertySymbols2()
})
{
}

void VirtualKeyboard::resize(const Size& s)
{
    Frame::resize(s);

    auto key_base_size = Size(s.width() / m_max_cols, s.height() / m_max_rows);

    for (auto& main_panel : m_main_panels)
        main_panel->update_key_size(key_base_size);
}

void VirtualKeyboard::set_key_space(unsigned key_space)
{
    m_key_space = key_space;
    for (auto& main_panel : m_main_panels)
        main_panel->update_key_space(key_space);
}

VirtualKeyboard::Panel::Panel(panel_keys keys)
    : m_keys(keys)
{
    for (auto& row : keys)
    {
        auto hsizer = make_shared<HorizontalBoxSizer>();
        hsizer->set_align(alignmask::expand_horizontal | alignmask::top);
        add(hsizer);

        for (auto& key : row)
            hsizer->add(key->m_button);
    }
}

void VirtualKeyboard::Panel::update_key_space(unsigned key_space)
{
    for (auto& row : m_keys)
        for (auto& key : row)
            key->m_button->set_margin(key_space / 2);
}

void VirtualKeyboard::Panel::update_key_size(const Size& s)
{
    for (auto& row : m_keys)
        for (auto& key : row)
            key->m_button->resize(Size(s.width() * key->m_length, s.height()));
}

void VirtualKeyboard::set_key_link(const shared_ptr<Key>& k)
{
    k->m_button->on_event([this, k](Event&)
    {
        m_main_panel.set_select(k->m_link);
    }, {eventid::pointer_click});
}

void VirtualKeyboard::set_key_input_value(const shared_ptr<Key>& k)
{
    k->m_button->on_event([this, k](Event & event)
    {
        if (!k->m_button->text().empty())
        {
            Event event2(eventid::keyboard_down);
            event2.key().unicode = k->m_unicode;
            event2.key().keycode = k->m_keycode;
            m_in.dispatch(event2);

            event2.set_id(eventid::keyboard_up);
            event2.key().unicode = k->m_unicode;
            event2.key().keycode = k->m_keycode;
            m_in.dispatch(event2);
        }

        return 0;
    }, {eventid::pointer_click});
}

void VirtualKeyboard::set_key_multichoice(const shared_ptr<Key>& key)
{
    key->m_multichoice_panel = make_shared<Panel>(key->m_keys_multichoice);
    key->m_multichoice_panel->set_align(alignmask::center);

    // Create a Popup to display the multichoice panel.
    key->m_button->on_event([this, key](Event&)
    {
        key->m_multichoice_panel->update_key_space(m_key_space);
        auto key_multichoice_size = Size(width() / m_max_cols, height() / m_max_rows);
        key->m_multichoice_panel->update_key_size(key_multichoice_size * m_key_size_multichoice_factor);

        m_multichoice_popup.reset(new Popup());
        m_multichoice_popup->resize(key->m_multichoice_panel->size());
        m_multichoice_popup->add(key->m_multichoice_panel);
        main_window()->add(m_multichoice_popup);

        auto display_origin = key->m_button->display_origin();
        auto main_window_origin = main_window()->display_to_local(display_origin);

        // Popup on top of the key.
        main_window_origin.set_y(main_window_origin.y() - m_multichoice_popup->height());
        // If it goes out of the main_window, move it at the bottom of the key.
        if (main_window_origin.y() < 0)
            main_window_origin.set_y(main_window()->display_to_local(display_origin).y() + key->m_button->height());

        // Popup aligned with key.
        main_window_origin.set_x(main_window_origin.x() - m_multichoice_popup->width() / 2);
        main_window_origin.set_x(main_window_origin.x() + key->m_button->width() / 2);
        // Update the position if it goes out of the main_window.
        if (main_window_origin.x() < 0)
            main_window_origin.set_x(0);

        if (main_window_origin.x() + m_multichoice_popup->width() > main_window()->width())
            main_window_origin.set_x(main_window()->width() - m_multichoice_popup->width());

        m_multichoice_popup->move(main_window_origin);
        m_multichoice_popup->show_modal();
    }, {eventid::pointer_hold});

    // Create the Buttons for the multichoice panel.
    for (auto& key_multichoice_row : key->m_keys_multichoice)
    {
        for (auto& key_multichoice : key_multichoice_row)
        {
            key_multichoice->m_button->on_event([this, key, key_multichoice](Event & event)
            {
                m_multichoice_popup->hide();
                main_window()->remove(m_multichoice_popup.get());

                if (!key_multichoice->m_button->text().empty())
                {
                    Event down(eventid::keyboard_down);
                    down.key().unicode = key_multichoice->m_unicode;
                    down.key().keycode = key_multichoice->m_keycode;
                    m_in.dispatch(down);
                    Event up(eventid::keyboard_up);
                    up.key().unicode = key_multichoice->m_unicode;
                    up.key().keycode = key_multichoice->m_keycode;
                    m_in.dispatch(up);
                    // the modal popup caught the raw_pointer_up event
                    key->m_button->set_active(false);
                }
                // User may just move his finger so prefer the raw_pointer_up event to the pointer_click one.
            }, {eventid::raw_pointer_up});
        }
    }
}

static PopupVirtualKeyboard* the_popup_virtual_keyboard = nullptr;

PopupVirtualKeyboard::PopupVirtualKeyboard(shared_ptr<VirtualKeyboard> keyboard) noexcept
{
    // Make the keyboard partially transparent.
    set_boxtype(Theme::boxtype::fill);
    set_color(Palette::ColorId::bg, Color(Palette::transparent, 80));

    auto popup_width = main_screen()->size().width();
    auto popup_height = main_screen()->size().height() * 0.4;

    resize(Size(popup_width, popup_height));
    auto y_keyboard_position = main_screen()->size().height() - popup_height;
    move(Point(0, y_keyboard_position));

    m_vsizer.set_align(alignmask::expand);
    add(m_vsizer);

    m_hsizer.set_align(alignmask::top | alignmask::right);
    m_vsizer.add(m_hsizer);

    m_top_bottom_button.set_align(alignmask::top | alignmask::right);
    m_top_bottom_button.on_event([this, y_keyboard_position](Event&)
    {
        if (m_bottom_positionned)
        {
            move(Point(0, 0));
            m_top_bottom_button.set_image(Image("@arrow_down.png"));
        }
        else
        {
            move(Point(0, y_keyboard_position));
            m_top_bottom_button.set_image(Image("@arrow_up.png"));
        }

        m_bottom_positionned = !m_bottom_positionned;
    }, {eventid::pointer_click});
    m_hsizer.add(m_top_bottom_button);

    m_close_button.set_align(alignmask::top | alignmask::right);
    m_close_button.on_event([this, y_keyboard_position](Event&)
    {
        hide();
        // By default, the virtual keyboard is displayed at the bottom of the screen.
        move(Point(0, y_keyboard_position));
        m_top_bottom_button.set_image(Image("@arrow_up.png"));
        m_bottom_positionned = true;
    }, {eventid::pointer_click});
    m_hsizer.add(m_close_button);

    keyboard->set_align(alignmask::expand);
    m_vsizer.add(keyboard);

    the_popup_virtual_keyboard = this;
}

PopupVirtualKeyboard*& popup_virtual_keyboard()
{
    return the_popup_virtual_keyboard;
}

VirtualKeyboard::panel_keys& multichoice_e()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0117),
            make_shared<VirtualKeyboard::Key>(0x0119),
            make_shared<VirtualKeyboard::Key>(0x011b),
            make_shared<VirtualKeyboard::Key>(0x0115),
            make_shared<VirtualKeyboard::Key>(0x04d9),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00e8),
            make_shared<VirtualKeyboard::Key>(0x00e9),
            make_shared<VirtualKeyboard::Key>(0x00ea),
            make_shared<VirtualKeyboard::Key>(0x00eb),
            make_shared<VirtualKeyboard::Key>(0x0113),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_E()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0116),
            make_shared<VirtualKeyboard::Key>(0x0118),
            make_shared<VirtualKeyboard::Key>(0x011a),
            make_shared<VirtualKeyboard::Key>(0x0115),
            make_shared<VirtualKeyboard::Key>(0x04d8),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00c8),
            make_shared<VirtualKeyboard::Key>(0x00c9),
            make_shared<VirtualKeyboard::Key>(0x00ca),
            make_shared<VirtualKeyboard::Key>(0x00cb),
            make_shared<VirtualKeyboard::Key>(0x0112),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_r()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0155),
            make_shared<VirtualKeyboard::Key>(0x0159),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_R()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0154),
            make_shared<VirtualKeyboard::Key>(0x0158),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_t()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0fe),
            make_shared<VirtualKeyboard::Key>(0x0165),
            make_shared<VirtualKeyboard::Key>(0x021b),
            make_shared<VirtualKeyboard::Key>(0x0163),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_T()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0fe),
            make_shared<VirtualKeyboard::Key>(0x0164),
            make_shared<VirtualKeyboard::Key>(0x021a),
            make_shared<VirtualKeyboard::Key>(0x0162),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_y()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0fd),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_Y()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0dd),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_u()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0173),
            make_shared<VirtualKeyboard::Key>(0x0171),
            make_shared<VirtualKeyboard::Key>(0x016f),
            make_shared<VirtualKeyboard::Key>(0x016b),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00fc),
            make_shared<VirtualKeyboard::Key>(0x00fb),
            make_shared<VirtualKeyboard::Key>(0x00fa),
            make_shared<VirtualKeyboard::Key>(0x00f9),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_U()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0172),
            make_shared<VirtualKeyboard::Key>(0x0170),
            make_shared<VirtualKeyboard::Key>(0x016e),
            make_shared<VirtualKeyboard::Key>(0x016a),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00dc),
            make_shared<VirtualKeyboard::Key>(0x00db),
            make_shared<VirtualKeyboard::Key>(0x00da),
            make_shared<VirtualKeyboard::Key>(0x00d9),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_i()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0131),
            make_shared<VirtualKeyboard::Key>(0x012e),
            make_shared<VirtualKeyboard::Key>(0x012b),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00ef),
            make_shared<VirtualKeyboard::Key>(0x00ee),
            make_shared<VirtualKeyboard::Key>(0x00ed),
            make_shared<VirtualKeyboard::Key>(0x00ec),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_I()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0130),
            make_shared<VirtualKeyboard::Key>(0x012d),
            make_shared<VirtualKeyboard::Key>(0x012a),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00cf),
            make_shared<VirtualKeyboard::Key>(0x00ce),
            make_shared<VirtualKeyboard::Key>(0x00cd),
            make_shared<VirtualKeyboard::Key>(0x00cc),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_o()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0153),
            make_shared<VirtualKeyboard::Key>(0x0151),
            make_shared<VirtualKeyboard::Key>(0x00f8),
            make_shared<VirtualKeyboard::Key>(0x00f6),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00f5),
            make_shared<VirtualKeyboard::Key>(0x00f4),
            make_shared<VirtualKeyboard::Key>(0x00f3),
            make_shared<VirtualKeyboard::Key>(0x00f2),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_O()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0152),
            make_shared<VirtualKeyboard::Key>(0x0150),
            make_shared<VirtualKeyboard::Key>(0x00d8),
            make_shared<VirtualKeyboard::Key>(0x00d6),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00d5),
            make_shared<VirtualKeyboard::Key>(0x00d4),
            make_shared<VirtualKeyboard::Key>(0x00d3),
            make_shared<VirtualKeyboard::Key>(0x00d2),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_a()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00e5),
            make_shared<VirtualKeyboard::Key>(0x00e6),
            make_shared<VirtualKeyboard::Key>(0x0101),
            make_shared<VirtualKeyboard::Key>(0x0103),
            make_shared<VirtualKeyboard::Key>(0x0105),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00e0),
            make_shared<VirtualKeyboard::Key>(0x00e1),
            make_shared<VirtualKeyboard::Key>(0x00e2),
            make_shared<VirtualKeyboard::Key>(0x00e3),
            make_shared<VirtualKeyboard::Key>(0x00e4),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_A()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00c5),
            make_shared<VirtualKeyboard::Key>(0x00c6),
            make_shared<VirtualKeyboard::Key>(0x0100),
            make_shared<VirtualKeyboard::Key>(0x0102),
            make_shared<VirtualKeyboard::Key>(0x0104),
        }, {
            make_shared<VirtualKeyboard::Key>(0x00c0),
            make_shared<VirtualKeyboard::Key>(0x00c1),
            make_shared<VirtualKeyboard::Key>(0x00c2),
            make_shared<VirtualKeyboard::Key>(0x00c3),
            make_shared<VirtualKeyboard::Key>(0x00c4),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_s()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00df),
            make_shared<VirtualKeyboard::Key>(0x00a7),
            make_shared<VirtualKeyboard::Key>(0x015b),
            make_shared<VirtualKeyboard::Key>(0x0161),
            make_shared<VirtualKeyboard::Key>(0x015f),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_S()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00df),
            make_shared<VirtualKeyboard::Key>(0x00a7),
            make_shared<VirtualKeyboard::Key>(0x015a),
            make_shared<VirtualKeyboard::Key>(0x0160),
            make_shared<VirtualKeyboard::Key>(0x015e),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_d()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x010f),
            make_shared<VirtualKeyboard::Key>(0x0111),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_D()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x010e),
            make_shared<VirtualKeyboard::Key>(0x0110),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_g()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0123),
            make_shared<VirtualKeyboard::Key>(0x011f),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_G()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0122),
            make_shared<VirtualKeyboard::Key>(0x011e),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_k()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0137),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_K()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0136),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_l()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0142),
            make_shared<VirtualKeyboard::Key>(0x013e),
            make_shared<VirtualKeyboard::Key>(0x013c),
            make_shared<VirtualKeyboard::Key>(0x013a),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_L()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0141),
            make_shared<VirtualKeyboard::Key>(0x013d),
            make_shared<VirtualKeyboard::Key>(0x013b),
            make_shared<VirtualKeyboard::Key>(0x0139),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_z()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x017a),
            make_shared<VirtualKeyboard::Key>(0x017c),
            make_shared<VirtualKeyboard::Key>(0x017e),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_Z()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0179),
            make_shared<VirtualKeyboard::Key>(0x017b),
            make_shared<VirtualKeyboard::Key>(0x017d),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_c()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00e7),
            make_shared<VirtualKeyboard::Key>(0x0107),
            make_shared<VirtualKeyboard::Key>(0x010d),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_C()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00c7),
            make_shared<VirtualKeyboard::Key>(0x0106),
            make_shared<VirtualKeyboard::Key>(0x010c),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_n()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0148),
            make_shared<VirtualKeyboard::Key>(0x0146),
            make_shared<VirtualKeyboard::Key>(0x0144),
            make_shared<VirtualKeyboard::Key>(0x00f1),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& multichoice_N()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0147),
            make_shared<VirtualKeyboard::Key>(0x0145),
            make_shared<VirtualKeyboard::Key>(0x0143),
            make_shared<VirtualKeyboard::Key>(0x00d1),
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& QwertyLettersLowerCase()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0031),
            make_shared<VirtualKeyboard::Key>(0x0032),
            make_shared<VirtualKeyboard::Key>(0x0033),
            make_shared<VirtualKeyboard::Key>(0x0034),
            make_shared<VirtualKeyboard::Key>(0x0035),
            make_shared<VirtualKeyboard::Key>(0x0036),
            make_shared<VirtualKeyboard::Key>(0x0037),
            make_shared<VirtualKeyboard::Key>(0x0038),
            make_shared<VirtualKeyboard::Key>(0x0039),
            make_shared<VirtualKeyboard::Key>(0x0030)
        }, {
            make_shared<VirtualKeyboard::Key>(0x0071),
            make_shared<VirtualKeyboard::Key>(0x0077),
            make_shared<VirtualKeyboard::Key>(0x0065, multichoice_e()),
            make_shared<VirtualKeyboard::Key>(0x0072, multichoice_r()),
            make_shared<VirtualKeyboard::Key>(0x0074, multichoice_t()),
            make_shared<VirtualKeyboard::Key>(0x0079, multichoice_y()),
            make_shared<VirtualKeyboard::Key>(0x0075, multichoice_u()),
            make_shared<VirtualKeyboard::Key>(0x0069, multichoice_i()),
            make_shared<VirtualKeyboard::Key>(0x006f, multichoice_o()),
            make_shared<VirtualKeyboard::Key>(0x0070)
        }, {
            make_shared<VirtualKeyboard::Key>(0x0061, multichoice_a()),
            make_shared<VirtualKeyboard::Key>(0x0073, multichoice_s()),
            make_shared<VirtualKeyboard::Key>(0x0064, multichoice_d()),
            make_shared<VirtualKeyboard::Key>(0x0066),
            make_shared<VirtualKeyboard::Key>(0x0067, multichoice_g()),
            make_shared<VirtualKeyboard::Key>(0x0068),
            make_shared<VirtualKeyboard::Key>(0x006a),
            make_shared<VirtualKeyboard::Key>(0x006b, multichoice_k()),
            make_shared<VirtualKeyboard::Key>(0x006c, multichoice_l())
        }, {
            make_shared<VirtualKeyboard::Key>("\ua71b", 1, 1.5),
            make_shared<VirtualKeyboard::Key>(0x007a, multichoice_z()),
            make_shared<VirtualKeyboard::Key>(0x0078),
            make_shared<VirtualKeyboard::Key>(0x0063, multichoice_c()),
            make_shared<VirtualKeyboard::Key>(0x0076),
            make_shared<VirtualKeyboard::Key>(0x0062),
            make_shared<VirtualKeyboard::Key>(0x006e, multichoice_n()),
            make_shared<VirtualKeyboard::Key>(0x006d),
            make_shared<VirtualKeyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<VirtualKeyboard::Key>("!#\u263a", 2, 1.5),
            make_shared<VirtualKeyboard::Key>(Image("@microphone.png"), EKEY_UNKNOWN),
            make_shared<VirtualKeyboard::Key>(0x0020, 5.0),
            make_shared<VirtualKeyboard::Key>(0x002e),
            make_shared<VirtualKeyboard::Key>("Enter", EKEY_ENTER, 1.5)
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& QwertyLettersUpperCase()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0031),
            make_shared<VirtualKeyboard::Key>(0x0032),
            make_shared<VirtualKeyboard::Key>(0x0033),
            make_shared<VirtualKeyboard::Key>(0x0034),
            make_shared<VirtualKeyboard::Key>(0x0035),
            make_shared<VirtualKeyboard::Key>(0x0036),
            make_shared<VirtualKeyboard::Key>(0x0037),
            make_shared<VirtualKeyboard::Key>(0x0038),
            make_shared<VirtualKeyboard::Key>(0x0039),
            make_shared<VirtualKeyboard::Key>(0x0030)
        }, {
            make_shared<VirtualKeyboard::Key>(0x0051),
            make_shared<VirtualKeyboard::Key>(0x0057),
            make_shared<VirtualKeyboard::Key>(0x0045, multichoice_E()),
            make_shared<VirtualKeyboard::Key>(0x0052, multichoice_R()),
            make_shared<VirtualKeyboard::Key>(0x0054, multichoice_T()),
            make_shared<VirtualKeyboard::Key>(0x0059, multichoice_Y()),
            make_shared<VirtualKeyboard::Key>(0x0055, multichoice_U()),
            make_shared<VirtualKeyboard::Key>(0x0049, multichoice_I()),
            make_shared<VirtualKeyboard::Key>(0x004f, multichoice_O()),
            make_shared<VirtualKeyboard::Key>(0x0050)
        }, {
            make_shared<VirtualKeyboard::Key>(0x0041, multichoice_A()),
            make_shared<VirtualKeyboard::Key>(0x0053, multichoice_S()),
            make_shared<VirtualKeyboard::Key>(0x0044, multichoice_D()),
            make_shared<VirtualKeyboard::Key>(0x0046),
            make_shared<VirtualKeyboard::Key>(0x0047, multichoice_G()),
            make_shared<VirtualKeyboard::Key>(0x0048),
            make_shared<VirtualKeyboard::Key>(0x004a),
            make_shared<VirtualKeyboard::Key>(0x004b, multichoice_K()),
            make_shared<VirtualKeyboard::Key>(0x004c, multichoice_L())
        }, {
            make_shared<VirtualKeyboard::Key>("\ua71b", 0, 1.5),
            make_shared<VirtualKeyboard::Key>(0x005a, multichoice_Z()),
            make_shared<VirtualKeyboard::Key>(0x0058),
            make_shared<VirtualKeyboard::Key>(0x0043, multichoice_C()),
            make_shared<VirtualKeyboard::Key>(0x0056),
            make_shared<VirtualKeyboard::Key>(0x0042),
            make_shared<VirtualKeyboard::Key>(0x004e, multichoice_N()),
            make_shared<VirtualKeyboard::Key>(0x004d),
            make_shared<VirtualKeyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<VirtualKeyboard::Key>("!#\u263a", 2, 1.5),
            make_shared<VirtualKeyboard::Key>(Image("@microphone.png"), EKEY_UNKNOWN),
            make_shared<VirtualKeyboard::Key>(0x0020, 5.0),
            make_shared<VirtualKeyboard::Key>(0x002e),
            make_shared<VirtualKeyboard::Key>("Enter", EKEY_ENTER, 1.5)
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& QwertySymbols1()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0031),
            make_shared<VirtualKeyboard::Key>(0x0032),
            make_shared<VirtualKeyboard::Key>(0x0033),
            make_shared<VirtualKeyboard::Key>(0x0034),
            make_shared<VirtualKeyboard::Key>(0x0035),
            make_shared<VirtualKeyboard::Key>(0x0036),
            make_shared<VirtualKeyboard::Key>(0x0037),
            make_shared<VirtualKeyboard::Key>(0x0038),
            make_shared<VirtualKeyboard::Key>(0x0039),
            make_shared<VirtualKeyboard::Key>(0x0030)
        }, {
            make_shared<VirtualKeyboard::Key>(0x002b),
            make_shared<VirtualKeyboard::Key>(0x0078),
            make_shared<VirtualKeyboard::Key>(0x00f7),
            make_shared<VirtualKeyboard::Key>(0x003d),
            make_shared<VirtualKeyboard::Key>(0x002f),
            make_shared<VirtualKeyboard::Key>(0x005f),
            make_shared<VirtualKeyboard::Key>(0x20ac),
            make_shared<VirtualKeyboard::Key>(0x00a3),
            make_shared<VirtualKeyboard::Key>(0x00a5),
            make_shared<VirtualKeyboard::Key>(0x20a9)
        }, {
            make_shared<VirtualKeyboard::Key>(0x0021),
            make_shared<VirtualKeyboard::Key>(0x0040),
            make_shared<VirtualKeyboard::Key>(0x0023),
            make_shared<VirtualKeyboard::Key>(0x0024),
            make_shared<VirtualKeyboard::Key>(0x0025),
            make_shared<VirtualKeyboard::Key>(0x005e),
            make_shared<VirtualKeyboard::Key>(0x0026),
            make_shared<VirtualKeyboard::Key>(0x002a),
            make_shared<VirtualKeyboard::Key>(0x0028),
            make_shared<VirtualKeyboard::Key>(0x0029)
        }, {
            make_shared<VirtualKeyboard::Key>("1/2", 3, 1.5),
            make_shared<VirtualKeyboard::Key>(0x002d),
            make_shared<VirtualKeyboard::Key>(0x0027),
            make_shared<VirtualKeyboard::Key>(0x0022),
            make_shared<VirtualKeyboard::Key>(0x003a),
            make_shared<VirtualKeyboard::Key>(0x003b),
            make_shared<VirtualKeyboard::Key>(0x002c),
            make_shared<VirtualKeyboard::Key>(0x003f),
            make_shared<VirtualKeyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<VirtualKeyboard::Key>("ABC", 0, 1.5),
            make_shared<VirtualKeyboard::Key>(Image("@microphone.png"), EKEY_UNKNOWN),
            make_shared<VirtualKeyboard::Key>(0x0020, 5.0),
            make_shared<VirtualKeyboard::Key>(0x002e),
            make_shared<VirtualKeyboard::Key>("Enter", EKEY_ENTER, 1.5)
        }
    };
    return keys;
}

VirtualKeyboard::panel_keys& QwertySymbols2()
{
    static VirtualKeyboard::panel_keys keys =
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0031),
            make_shared<VirtualKeyboard::Key>(0x0032),
            make_shared<VirtualKeyboard::Key>(0x0033),
            make_shared<VirtualKeyboard::Key>(0x0034),
            make_shared<VirtualKeyboard::Key>(0x0035),
            make_shared<VirtualKeyboard::Key>(0x0036),
            make_shared<VirtualKeyboard::Key>(0x0037),
            make_shared<VirtualKeyboard::Key>(0x0038),
            make_shared<VirtualKeyboard::Key>(0x0039),
            make_shared<VirtualKeyboard::Key>(0x0030)
        }, {
            make_shared<VirtualKeyboard::Key>(0x0060),
            make_shared<VirtualKeyboard::Key>(0x007e),
            make_shared<VirtualKeyboard::Key>(0x005c),
            make_shared<VirtualKeyboard::Key>(0x007c),
            make_shared<VirtualKeyboard::Key>(0x003c),
            make_shared<VirtualKeyboard::Key>(0x003e),
            make_shared<VirtualKeyboard::Key>(0x007b),
            make_shared<VirtualKeyboard::Key>(0x007d),
            make_shared<VirtualKeyboard::Key>(0x005b),
            make_shared<VirtualKeyboard::Key>(0x005d)
        }, {
            make_shared<VirtualKeyboard::Key>(0x25c4),
            make_shared<VirtualKeyboard::Key>(0x25b2),
            make_shared<VirtualKeyboard::Key>(0x25bc),
            make_shared<VirtualKeyboard::Key>(0x25ba),
            make_shared<VirtualKeyboard::Key>(0x25a0),
            make_shared<VirtualKeyboard::Key>(0x25a1),
            make_shared<VirtualKeyboard::Key>(0x25cf),
            make_shared<VirtualKeyboard::Key>(0x25cb),
            make_shared<VirtualKeyboard::Key>(0x25cc),
            make_shared<VirtualKeyboard::Key>(0x25ca)
        }, {
            make_shared<VirtualKeyboard::Key>("2/2", 2, 1.5),
            make_shared<VirtualKeyboard::Key>(0x263a),
            make_shared<VirtualKeyboard::Key>(0x263b),
            make_shared<VirtualKeyboard::Key>(0x263c),
            make_shared<VirtualKeyboard::Key>(0x00ab),
            make_shared<VirtualKeyboard::Key>(0x00bb),
            make_shared<VirtualKeyboard::Key>(0x00a1),
            make_shared<VirtualKeyboard::Key>(0x00bf),
            make_shared<VirtualKeyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<VirtualKeyboard::Key>("ABC", 0, 1.5),
            make_shared<VirtualKeyboard::Key>(Image("@microphone.png"), EKEY_UNKNOWN),
            make_shared<VirtualKeyboard::Key>(0x0020, 5.0),
            make_shared<VirtualKeyboard::Key>(0x002e),
            make_shared<VirtualKeyboard::Key>("Enter", EKEY_ENTER, 1.5)
        }
    };
    return keys;
}

}
}
