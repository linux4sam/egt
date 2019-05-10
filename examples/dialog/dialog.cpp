/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <experimental/filesystem>
#include <string>

using namespace std;
using namespace egt;
namespace fs = std::experimental::filesystem;

using WindowType = Window;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "dialog");

    TopWindow win0;

    BoxSizer vsizer(orientation::vertical);
    win0.add(expand(vsizer));

    auto grid = make_shared<StaticGrid>(Rect(Point(), Size(0, win0.h() * 0.10)), Tuple(3, 1), 10);
    grid->set_color(Palette::ColorId::bg, Color(0xed2924ff));
    grid->set_boxtype(Theme::boxtype::blank);

    auto logo = make_shared<ImageLabel>(Image("@microchip_logo_white.png"));
    grid->add(logo, 1, 0);

    vsizer.add(expand_horizontal(grid));

    auto hsizer = make_shared<BoxSizer>(orientation::horizontal);
    vsizer.add(expand(hsizer));

    auto list = make_shared<ListBox>(Rect(Point(), Size(win0.w() * 0.25, 0)));
    list->add_item(make_shared<StringItem>("", Rect(), alignmask::left | alignmask::center));
    list->add_item(make_shared<StringItem>("File Open", Rect(), alignmask::left | alignmask::center));
    list->add_item(make_shared<StringItem>("File Save", Rect(), alignmask::left | alignmask::center));
    list->add_item(make_shared<StringItem>("Message Dialog", Rect(), alignmask::left | alignmask::center));
    list->add_item(make_shared<StringItem>("List Dialog", Rect(), alignmask::left | alignmask::center));
    list->add_item(make_shared<StringItem>("Slider Dialog", Rect(), alignmask::left | alignmask::center));
    list->set_align(alignmask::expand_vertical | alignmask::left);
    hsizer->add(list);

    auto label1 = std::make_shared<TextBox>("", Rect(0, 0, win0.w() * 0.75, win0.h() * 0.25), alignmask::left | alignmask::center);
    label1->text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});
    hsizer->add(label1);

    std::string RootDir = fs::current_path();

    auto win1 = std::make_shared<FileOpenDialog>(RootDir, Rect(0, 0, 640, 432));
    win1->on_event([win1, label1, list](eventid)
    {
        DBG("FileDialog : file selected is : " << win1->get_selected());
        label1->set_text("File OpenDialog: " + win1->get_selected() + " Selected");
        win1->hide();
        list->set_select(0);
        return 0;
    }, {eventid::property_changed});
    win0.add(win1);

    auto win2 = std::make_shared<FileSaveDialog>(RootDir, Rect(0, 0, 640, 432));
    win2->on_event([win2, label1, list](eventid)
    {
        DBG("FileDialog : save file is : " << win2->get_selected());
        label1->set_text("File SaveDialog: " + win2->get_selected() + " Selected");
        win2->hide();
        list->set_select(0);
        return 0;
    }, {eventid::property_changed});

    win0.add(win2);

    auto dialog = std::make_shared<Dialog>(Rect(0, 0, 440, 320));
    dialog->set_title("Message Dialog Example");
    dialog->set_message("This is a Example of Ensemble Graphics Toolkit Message Dialog with two button");
    dialog->set_button(Dialog::buttonid::button1, "OK");
    dialog->set_button(Dialog::buttonid::button2, "Cancel");
    win0.add(dialog);

    dialog->on_event([dialog, label1, list](eventid event)
    {
        if (event == eventid::event1)
        {
            DBG("FileDialog Okay button clicked");
            label1->set_text("Message Dialog: Okay button clicked");
        }
        else if (event == eventid::event2)
        {
            DBG("FileDialog Cancel button clicked");
            label1->set_text("Message Dialog: Cancel button clicked");
        }
        list->set_select(0);
        return 1;
    });

    auto dialog1 = std::make_shared<Dialog>(Rect(0, 0, 440, 320));
    dialog1->set_title("List Dialog Box Example");
    dialog1->set_button(Dialog::buttonid::button1, "OK");
    dialog1->set_button(Dialog::buttonid::button2, "Cancel");
    win0.add(dialog1);

    auto dlist0 = std::make_shared<ListBox>(Rect(0, 0, dialog1->w(), dialog1->h() * 0.75));
    for (auto x = 0; x < 25; x++)
        dlist0->add_item(std::make_shared<StringItem>("item " + std::to_string(x), Rect(), alignmask::left | alignmask::center));
    dlist0->set_align(alignmask::left | alignmask::expand_vertical);
    dialog1->set_widget(dlist0);

    dialog1->on_event([dialog1, label1, dlist0, list](eventid event)
    {
        if (event == eventid::event1)
        {
            DBG("FileDialog Okay button clicked");
            auto select = dynamic_cast<StringItem*>(dlist0->get_item(dlist0->selected()))->text();
            label1->set_text("List Dialog: " + select + " Selected");
        }
        else if (event == eventid::event2)
        {
            DBG("FileDialog Cancel button clicked");
            label1->set_text("List Dialog: Cancel button clicked");
        }
        list->set_select(0);
        return 1;
    });

    auto dialog2 = std::make_shared<Dialog>(Rect(0, 0, 440, 320));
    dialog2->set_title("Slider Dialog Example");
    dialog2->set_button(Dialog::buttonid::button1, "OK");
    dialog2->set_button(Dialog::buttonid::button2, "Cancel");
    win0.add(dialog2);

    auto slider1 = std::make_shared<Slider>(Rect(0, 0, 300, 100));
    slider1->set_value(50);
    slider1->slider_flags().set({Slider::flag::round_handle, Slider::flag::show_label});
    slider1->set_align(alignmask::center);
    dialog2->set_widget(slider1);

    dialog2->on_event([dialog2, label1, slider1, list](eventid event)
    {
        if (event == eventid::event1)
        {
            DBG("FileDialog Okay button clicked");
            auto select = slider1->value();
            label1->set_text("Slider Dialog: value = " + std::to_string(select));
        }
        else if (event == eventid::event2)
        {
            DBG("FileDialog Cancel button clicked");
            label1->set_text("Slider Dialog: Cancel button clicked");
        }
        list->set_select(0);
        return 1;
    });

    list->on_event([list, win1, win2, dialog, dialog1, dialog2, label1](eventid)
    {
        auto index = list->selected();
        DBG("FileDialog : Index value " << index);
        switch (index)
        {
        case 0:
        {
            break;
        }
        case 1:
        {
            label1->set_text("");
            win1->show_modal(true);
            break;
        }
        case 2:
        {
            label1->set_text("");
            win2->show_modal(true);
            break;
        }
        case 3:
        {
            label1->set_text("");
            dialog->show_modal(true);
            break;
        }
        case 4:
        {
            label1->set_text("");
            dialog1->show_modal(true);
            break;
        }
        case 5:
        {
            label1->set_text("");
            dialog2->show_modal(true);
            break;
        }
        default:
            break;
        }
        return 1;
    }, {eventid::property_changed});


    win0.show();

    return app.run();
}
