/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <memory>
#include <sstream>
#include <string>

class SevenSegment : public egt::experimental::Gauge
{
public:
    explicit SevenSegment(size_t width = 50)
    {
        padding(5);
        align(egt::AlignFlag::bottom);

        egt::SvgImage svg("file:7segment.svg", egt::SizeF(width, 0));

        auto layer = std::make_shared<egt::experimental::GaugeLayer>(svg.render("#bottom"));
        layer->move(egt::Point(moat(), moat()));
        add(layer);
        layer->name("#bottom");

        for (auto i = 0; i < 7; i++)
        {
            std::ostringstream ss;
            ss << "#pl_100_seg_" << static_cast<char>('a' + i);
            auto box = svg.id_box(ss.str());
            auto layer2 = std::make_shared<egt::experimental::GaugeLayer>(svg.render(ss.str(), box));
            layer2->name(ss.str());
            layer2->box(egt::Rect(box.x() + moat(),
                                  box.y() + moat(),
                                  std::ceil(box.width()),
                                  std::ceil(box.height())));
            layer2->hide();
            add(layer2);
        }
    }

    std::shared_ptr<egt::experimental::GaugeLayer> find_layer(const std::string& name)
    {
        return find_child<egt::experimental::GaugeLayer>(name);
    }

    void set_color(const egt::Color& color)
    {
        for (auto i = 0; i < 7; i++)
        {
            std::ostringstream ss;
            ss << "#pl_100_seg_" << static_cast<char>('a' + i);
            find_layer(ss.str())->mask_color(color);
        }
    }

    void apply(int digit)
    {
        // hide all of the segments first and then go turn the ones on we want
        for (auto& child : children())
        {
            if (child->name().rfind("#pl_100_seg_", 0) == 0)
                child->hide();
        }

        switch (digit)
        {
        case 0:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_c")->show();
            find_layer("#pl_100_seg_d")->show();
            find_layer("#pl_100_seg_e")->show();
            find_layer("#pl_100_seg_f")->show();
            break;
        case 1:
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_c")->show();
            break;
        case 2:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_e")->show();
            find_layer("#pl_100_seg_d")->show();
            break;
        case 3:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_c")->show();
            find_layer("#pl_100_seg_d")->show();
            break;
        case 4:
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_c")->show();
            break;
        case 5:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_c")->show();
            find_layer("#pl_100_seg_d")->show();
            break;
        case 6:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_c")->show();
            find_layer("#pl_100_seg_d")->show();
            find_layer("#pl_100_seg_e")->show();
            break;
        case 7:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_c")->show();
            break;
        case 8:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_c")->show();
            find_layer("#pl_100_seg_d")->show();
            find_layer("#pl_100_seg_e")->show();
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_g")->show();
            break;
        case 9:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_c")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_f")->show();
            break;
        case 10:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_e")->show();
            find_layer("#pl_100_seg_c")->show();
            break;
        case 11:
            find_layer("#pl_100_seg_c")->show();
            find_layer("#pl_100_seg_d")->show();
            find_layer("#pl_100_seg_e")->show();
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_g")->show();
            break;
        case 12:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_e")->show();
            find_layer("#pl_100_seg_d")->show();
            break;
        case 13:
            find_layer("#pl_100_seg_b")->show();
            find_layer("#pl_100_seg_c")->show();
            find_layer("#pl_100_seg_d")->show();
            find_layer("#pl_100_seg_e")->show();
            find_layer("#pl_100_seg_g")->show();
            break;
        case 14:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_e")->show();
            find_layer("#pl_100_seg_d")->show();
            break;
        case 15:
            find_layer("#pl_100_seg_a")->show();
            find_layer("#pl_100_seg_f")->show();
            find_layer("#pl_100_seg_g")->show();
            find_layer("#pl_100_seg_e")->show();
            break;
        default:
            break;
        }
    }
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::TopWindow window;
    window.color(egt::Palette::ColorId::bg, egt::Palette::black);

    egt::ImageLabel logo(egt::Image("icon:mgs_logo_white.png;128"));
    logo.margin(10);
    logo.align(egt::AlignFlag::center_horizontal | egt::AlignFlag::top);
    window.add(logo);

    egt::HorizontalBoxSizer sizer;
    window.add(center(sizer));

    SevenSegment seven0(25);
    seven0.set_color(egt::Palette::purple);
    sizer.add(seven0);

    SevenSegment seven1(50);
    seven1.set_color(egt::Palette::blue);
    sizer.add(seven1);

    SevenSegment seven2(100);
    seven2.set_color(egt::Palette::red);
    sizer.add(seven2);

    SevenSegment seven3(150);
    sizer.add(seven3);

    window.show();

    int digit = -1;
    egt::PeriodicTimer timer(std::chrono::seconds(1));
    timer.on_timeout([&]()
    {
        seven0.apply(digit);
        seven1.apply(digit);
        seven2.apply(digit);
        seven3.apply(digit);
        if (++digit > 15)
            digit = -1;
    });
    timer.start();

    return app.run();
}
