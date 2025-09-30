/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <regex>
#include <egt/ui>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
    egt::TopWindow win;

    auto grid1 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(2, 1));
    grid1->margin(5);
    grid1->horizontal_space(5);
    grid1->vertical_space(5);
    win.add(egt::expand(grid1));

    auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(1, 10));
    grid0->margin(5);
    grid0->horizontal_space(5);
    grid0->vertical_space(5);
    grid1->add(egt::expand(grid0));

    auto text1 = std::make_shared<egt::TextBox>("text 1", egt::TextBox::TextFlag::fit_to_width);
    grid0->add(egt::expand(text1));

    auto text2 = std::make_shared<egt::TextBox>("text 2 disabled");
    text2->disable();
    grid0->add(egt::expand(text2));

    auto text3 = std::make_shared<egt::TextBox>("right aligned text");
    text3->fill_flags(egt::Theme::FillFlag::blend);
    text3->border_flags({egt::Theme::BorderFlag::bottom, egt::Theme::BorderFlag::top});
    text3->text_align(egt::AlignFlag::right);
    grid0->add(egt::expand(text3));

    auto text4 = std::make_shared<egt::TextBox>("text 4");
    text4->fill_flags(egt::Theme::FillFlag::blend);
    text4->border_flags(egt::Theme::BorderFlag::bottom);
    text4->disable();
    grid0->add(egt::expand(text4));

    auto text5 = std::make_shared<egt::TextBox>("all characters allowed");
    text5->add_validator_function([](const std::string&) { return true; });
    text5->input_validation_enabled(true);
    grid0->add(egt::expand(text5));

    auto text6 = std::make_shared<egt::TextBox>("abc123 only");
    text6->add_validator_function([](const std::string & s)
    {
        return std::regex_match(s, std::regex("[abc123]*"));
    });
    text6->input_validation_enabled(true);
    grid0->add(egt::expand(text6));

    auto text7 = std::make_shared<egt::TextBox>(
                     u8"The Ensemble Graphics Toolkit (EGT) is a free and open-source C++ GUI widget "
                     " toolkit for microprocessors.  It is used to develop"
                     " graphical applications for Linux.  EGT provides modern and complete GUI"
                     " functionality, look-and-feel, and performance.\n\nThis multi-line TextBox fully"
                     " supports UTF-8 encoding.  See: \u2190\u2191\u2192\u2193",
                     egt::TextBox::TextFlags({egt::TextBox::TextFlag::multiline, egt::TextBox::TextFlag::word_wrap}));
    text7->selection(4, 25);
    grid1->add(egt::expand(text7));

    win.show();

    return app.run();
}
