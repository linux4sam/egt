/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
    egt::TopWindow win;

    auto hsizer1 =
        std::make_shared<egt::BoxSizer>(egt::Orientation::horizontal);
    win.add(egt::expand(hsizer1));

    auto scrollwheel_day =
        std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 50, 100), 1, 32, 1);
    scrollwheel_day->remove_item("32");

    const egt::Scrollwheel::ItemArray months =
    {
        "January", "February", "March",
        "April", "May", "June", "July",
        "August", "September", "October",
        "November", "December"
    };

    auto scrollwheel_month =
        std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 200, 100), months);
    scrollwheel_month->add_item("");

    auto scrollwheel_year =
        std::make_shared<egt::Scrollwheel>(egt::Rect(0, 0, 100, 100),
                                           1900, 2020, 1);
    scrollwheel_year->selected(120);
    scrollwheel_year->reversed(true);

    auto label_day =
        std::make_shared<egt::Label>(scrollwheel_day->value(),
                                     egt::Rect(0, 0, 50, 30));

    auto label_month =
        std::make_shared<egt::Label>(scrollwheel_month->value(),
                                     egt::Rect(0, 0, 100, 30));

    auto label_year =
        std::make_shared<egt::Label>(scrollwheel_year->value(),
                                     egt::Rect(0, 0, 75, 30));

    auto weak_label_day = std::weak_ptr<egt::Label>(label_day);
    auto weak_scrollwheel_day = std::weak_ptr<egt::Scrollwheel>(scrollwheel_day);
    scrollwheel_day->on_value_changed([weak_label_day, weak_scrollwheel_day]()
    {
        auto label_day = weak_label_day.lock();
        auto scrollwheel_day = weak_scrollwheel_day.lock();
        if (label_day && scrollwheel_day)
            label_day->text(scrollwheel_day->value());
    });

    auto weak_label_month = std::weak_ptr<egt::Label>(label_month);
    auto weak_scrollwheel_month = std::weak_ptr<egt::Scrollwheel>(scrollwheel_month);
    scrollwheel_month->on_value_changed([weak_label_month, weak_scrollwheel_month]()
    {
        auto label_month = weak_label_month.lock();
        auto scrollwheel_month = weak_scrollwheel_month.lock();
        if (label_month && scrollwheel_month)
            label_month->text(scrollwheel_month->value());
    });

    auto weak_label_year = std::weak_ptr<egt::Label>(label_year);
    auto weak_scrollwheel_year = std::weak_ptr<egt::Scrollwheel>(scrollwheel_year);
    scrollwheel_year->on_value_changed([weak_label_year, weak_scrollwheel_year]()
    {
        auto label_year = weak_label_year.lock();
        auto scrollwheel_year = weak_scrollwheel_year.lock();
        if (label_year && scrollwheel_year)
            label_year->text(scrollwheel_year->value());
    });

    hsizer1->add(scrollwheel_day);
    hsizer1->add(scrollwheel_month);
    hsizer1->add(scrollwheel_year);

    hsizer1->add(label_day);
    hsizer1->add(label_month);
    hsizer1->add(label_year);

    scrollwheel_month->selected(4);

    win.show();

    return app.run();
}
