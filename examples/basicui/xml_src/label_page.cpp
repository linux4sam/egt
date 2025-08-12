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

    auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 5));
    grid0->margin(5);
    grid0->horizontal_space(5);
    grid0->vertical_space(5);
    win.add(egt::expand(grid0));

    auto label1 = std::make_shared<egt::Label>("left align",
                  egt::AlignFlag::left | egt::AlignFlag::center_vertical);
    grid0->add(egt::expand(label1));

    auto label2 = std::make_shared<egt::Label>("right align",
                  egt::AlignFlag::right | egt::AlignFlag::center_vertical);
    grid0->add(egt::expand(label2));

    auto label3 = std::make_shared<egt::Label>("top align",
                  egt::AlignFlag::top | egt::AlignFlag::center_horizontal);
    grid0->add(egt::expand(label3));

    auto label4 = std::make_shared<egt::Label>("bottom align",
                  egt::AlignFlag::bottom | egt::AlignFlag::center_horizontal);
    grid0->add(egt::expand(label4));

    auto label5 = std::make_shared<egt::Label>("Multiline\nLabel Text", egt::AlignFlag::center);
    grid0->add(egt::expand(label5));

    auto imagelabel0 = std::make_shared<egt::ImageLabel>(egt::Image("icon:lock.png"), "Lock");
    grid0->add(expand(imagelabel0));
    imagelabel0->image_align(egt::AlignFlag::top);
    imagelabel0->text_align(egt::AlignFlag::center);

    auto image = egt::Image("icon:camera.png");
    auto imagelabel1 = std::make_shared<egt::ImageLabel>(image, "Camera");
    grid0->add(expand(imagelabel1));
    imagelabel1->text_align(egt::AlignFlag::center);

    auto imagelabel2 = std::make_shared<egt::ImageLabel>(image, "Camera");
    grid0->add(expand(imagelabel2));
    imagelabel2->text_align(egt::AlignFlag::center_vertical | egt::AlignFlag::right);
    imagelabel2->image_align(egt::AlignFlag::right);

    auto imagelabel3 = std::make_shared<egt::ImageLabel>(image, "Camera");
    grid0->add(expand(imagelabel3));
    imagelabel3->text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::top);
    imagelabel3->image_align(egt::AlignFlag::bottom);

    auto imagelabel4 = std::make_shared<egt::ImageLabel>(image, "Camera");
    imagelabel4->fill_flags(egt::Theme::FillFlag::blend);
    imagelabel4->border(1);
    grid0->add(egt::expand(imagelabel4));
    imagelabel4->text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::bottom);

    auto imagelabel5 = std::make_shared<egt::ImageLabel>(image);
    imagelabel5->fill_flags(egt::Theme::FillFlag::blend);
    imagelabel5->border(1);
    grid0->add(egt::expand(imagelabel5));

    win.show();

    return app.run();
}
