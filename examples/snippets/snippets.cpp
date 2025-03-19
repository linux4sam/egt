/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

static const std::pair<std::string, std::function<int(egt::Application& app)>> examples[] =
{
    {
        "label1", [](egt::Application & app)
        {
            /// @[label1]
            egt::TopWindow window;
            egt::Label label(window, "I'm a Label");
            center(label);
            window.show();
            /// @[label1]
            return app.run();
        }
    },
    {
        "label2", [](egt::Application & app)
        {
            /// @[label2]
            egt::TopWindow window;
            egt::Label label(window, "I'm a Bigger Label");
            label.font(egt::Font("Sans", 30));
            center(label);
            window.show();
            /// @[label2]
            return app.run();
        }
    },
    {
        "label3", [](egt::Application & app)
        {
            /// @[label3]
            egt::TopWindow window;
            egt::Label label(window, "I'm a Red Label");
            label.color(egt::Palette::ColorId::label_text, egt::Palette::red);
            center(label);
            window.show();
            /// @[label3]
            return app.run();
        }
    },
    {
        "button0", [](egt::Application & app)
        {
            /// @[button0]
            egt::TopWindow window;
            auto button = std::make_shared<egt::Button>("Button");
            window.add(center(button));
            window.show();
            /// @[button0]
            return app.run();
        }
    },
    {
        "button1", [](egt::Application & app)
        {
            /// @[button1]
            egt::TopWindow window;
            egt::Button button(window, "Button");
            center(button);
            window.show();
            /// @[button1]
            return app.run();
        }
    },
    {
        "button2", [](egt::Application & app)
        {
            /// @[button2]
            egt::TopWindow window;
            egt::Button button(window, "Press Me");
            center(button);

            button.on_event([](egt::Event & event)
            {
                std::cout << "received event " << event.id() << std::endl;
            });
            window.show();
            /// @[button2]
            return app.run();
        }
    },
    {
        "button3", [](egt::Application & app)
        {
            /// @[button3]
            egt::TopWindow window;
            egt::Button button(window, "Change Colors");
            center(button);

            egt::experimental::ColorMap colors({egt::Palette::red, egt::Palette::green});
            button.on_click([&button, &colors](egt::Event&)
            {
                static float step = 0;
                button.color(egt::Palette::ColorId::button_bg,
                             colors.interp(step += 0.1));
                button.color(egt::Palette::ColorId::button_bg,
                             colors.interp(1.0 - step),
                             egt::Palette::GroupId::active);
                if (step >= 1.0)
                    step = 0.;
            });

            window.show();
            /// @[button3]
            return app.run();
        }
    },
    {
        "image1", [](egt::Application & app)
        {
            /// @[image1]
            egt::TopWindow window;
            egt::HorizontalBoxSizer sizer;
            window.add(center(sizer));

            egt::ImageLabel label1(sizer, egt::Image("icon:unlock.png"));
            egt::ImageLabel label2(sizer, egt::Image("icon:paint.png"));
            egt::ImageLabel label3(sizer, egt::Image("icon:battery.png"));
            egt::ImageLabel label4(sizer, egt::Image("icon:ok.png"));

            window.show();
            /// @[image1]
            return app.run();
        }
    },
    {
        "animation0", [](egt::Application & app)
        {
            /// @[animation0]
            egt::TopWindow window;
            egt::Button button(window, "Can you see me now?");
            center(button);

            egt::PropertyAnimatorF animation;
            animation.starting(0.0);
            animation.ending(1.0);
            animation.duration(std::chrono::seconds(5));
            animation.easing_func(egt::easing_linear);
            animation.on_change([&button](egt::PropertyAnimatorF::Value value) { button.alpha(value); });
            animation.start();

            window.show();
            /// @[animation0]
            return app.run();
        }
    },
    {
        "animation1", [](egt::Application & app)
        {
            /// @[animation1]
            egt::TopWindow window;
            /// @[animation1.1]
            egt::Button button(window, "I Move");
            button.move_to_center(window.center());

            egt::PropertyAnimator animation;
            animation.starting(button.y());
            animation.ending(button.y() + 100);
            animation.duration(std::chrono::seconds(5));
            animation.easing_func(egt::easing_linear);
            animation.on_change([&button](egt::PropertyAnimator::Value value) { button.y(value); });

            animation.start();
            /// @[animation1.1]

            window.show();
            /// @[animation1]
            return app.run();
        }
    },
    {
        "animation2", [](egt::Application & app)
        {
            /// @[animation2]
            egt::TopWindow window;
            egt::Button button(window, "I Move");
            button.move_to_center(window.center());
            const egt::Point starting_point = button.point();

            egt::PropertyAnimator animation;
            animation.starting(0);
            animation.ending(100);
            animation.duration(std::chrono::seconds(5));
            animation.easing_func(egt::easing_linear);
            animation.on_change([&button, starting_point](egt::PropertyAnimator::Value value)
            {
                auto point = starting_point;
                point += value;
                button.move(point);
            });

            animation.start();

            window.show();
            /// @[animation2]
            return app.run();
        }
    },
    {
        "animation3", [](egt::Application & app)
        {
            /// @[animation3]
            egt::TopWindow window;
            egt::Label label(window, "EGT");
            center(label);

            egt::PropertyAnimatorF animation;
            animation.starting(8.f);
            animation.ending(120.f);
            animation.duration(std::chrono::seconds(5));
            animation.easing_func(egt::easing_linear);
            animation.on_change([&label](egt::PropertyAnimatorF::Value value)
            {
                auto font = label.font();
                font.size(value);
                label.font(font);
            });

            animation.start();

            window.show();
            /// @[animation3]
            return app.run();
        }
    },
    {
        "animation4", [](egt::Application & app)
        {
            /// @[animation4]
            egt::TopWindow window;
            egt::Button button(window, "Hello World");
            center(button);

            egt::PropertyAnimatorF animation;
            animation.starting(0.0);
            animation.ending(app.screen()->max_brightness());
            animation.duration(std::chrono::seconds(5));
            animation.easing_func(egt::easing_linear);
            animation.on_change([&app](egt::PropertyAnimatorF::Value value)
            {
                app.screen()->brightness(value);
            });
            animation.start();

            window.show();
            /// @[animation4]
            return app.run();
        }
    },
    {
        "events1", [](egt::Application & app)
        {
            /// @[events1]
            egt::TopWindow window;
            egt::VerticalBoxSizer sizer;
            window.add(expand(sizer));

            egt::Button button(sizer, "Disable Me");
            egt::CheckBox checkbox(sizer, "Button Disabled");
            checkbox.on_checked_changed([&button, &checkbox]()
            {
                button.disabled(checkbox.checked());
            });

            window.show();
            /// @[events1]
            return app.run();
        }
    },
    {
        "timer1", [](egt::Application & app)
        {
            /// @[timer1]
            egt::TopWindow window;
            egt::Label label(window);
            label.text_align(egt::AlignFlag::center);
            center(label);

            egt::PeriodicTimer timer(std::chrono::seconds(1));
            timer.on_timeout([&label]()
            {
                static auto count = 0;
                std::ostringstream ss;
                ss << ++count;
                label.text(ss.str());
            });
            timer.start();

            window.show();
            /// @[timer1]
            return app.run();
        }
    },
    {
        "theme1", [](egt::Application & app)
        {
            /// @[theme1]
            egt::TopWindow window;

            struct CustomTheme : public egt::Theme
            {
                using egt::Theme::Theme;
                void apply() override
                {
                    egt::Theme::apply();

                    palette().set(egt::Palette::ColorId::button_bg,
                                  egt::Palette::GroupId::normal,
                                  egt::Palette::green);
                    palette().set(egt::Palette::ColorId::bg,
                                  egt::Palette::GroupId::normal,
                                  egt::Palette::blue);
                }
            } theme;

            theme.apply();
            egt::global_theme(std::make_unique<egt::Theme>(theme));

            egt::Button button0(window, "Button");
            center(left(button0));
            egt::Button button1(window, "Button");
            center(right(button1));

            window.show();
            /// @[theme1]
            return app.run();
        }
    },
    {
        "theme2", [](egt::Application & app)
        {
            /// @[theme2]
            egt::TopWindow window;

            auto theme = egt::global_theme();
            theme.palette().set(egt::Palette::ColorId::button_bg,
                                egt::Palette::GroupId::normal,
                                egt::Palette::green);
            theme.palette().set(egt::Palette::ColorId::bg,
                                egt::Palette::GroupId::normal,
                                egt::Palette::blue);

            egt::global_theme(std::make_unique<egt::Theme>(theme));

            egt::Button button0(window, "Button");
            center(left(button0));

            egt::Button button1(window, "Button");
            center(right(button1));

            window.show();
            /// @[theme2]
            return app.run();
        }
    },
#ifdef EGT_HAS_SOUNDEFFECT
    {
        "sound0", [](egt::Application & app)
        {
            egt::TopWindow window;

            /// @[sound0]
            egt::SoundEffect sound("file:assets/tom.wav");
            sound.play();
            /// @[sound0]

            window.show();
            return app.run();
        }
    },
    {
        "sound1", [](egt::Application & app)
        {
            /// @[sound1]
            egt::TopWindow window;

            egt::SoundEffect sound("file:assets/tom.wav");

            egt::Button button(window, "Play Sound");
            center(button);
            button.on_click([&sound](egt::Event&)
            {
                sound.play();
            });

            window.show();
            /// @[sound1]
            return app.run();
        }
    },
#endif
#ifdef EGT_HAS_AUDIO
    {
        "audio1", [](egt::Application & app)
        {
            egt::TopWindow window;

            window.add(center(std::make_shared<egt::ImageLabel>(egt::Image("icon:volume_up.png"))));
            window.show();

            /// @[audio1]
            egt::AudioPlayer player("file:assets/concerto.mp3");
            player.play();
            /// @[audio1]

            window.show();
            return app.run();
        }
    },
#endif
#ifdef EGT_HAS_VIDEO
    {
        "video1", [](egt::Application & app)
        {
            /// @[video1]
            egt::TopWindow window;

            egt::VideoWindow player(window.content_area(), "file:assets/video.mp4");
            window.add(player);
            player.volume(50);
            player.show();
            player.play();

            window.show();
            /// @[video1]
            return app.run();
        }
    },
#endif
#ifdef EGT_HAS_CAMERA
    {
        "camera0", [](egt::Application & app)
        {
            /// @[camera0]
            egt::TopWindow window;

            egt::CameraWindow player("/dev/video0");
            window.add(player);
            player.play();

            window.show();
            /// @[camera0]
            return app.run();
        }
    },
#endif
#ifdef EGT_HAS_CAPTURE
    {
        "capture0", [](egt::Application & app)
        {
            /// @[capture0]
            egt::CameraCapture capture("output.avi");
            capture.start();
            /// @[capture0]
            return app.run();
        }
    },
#endif
};

int main(int argc, const char** argv)
{
    egt::Application app;

    if (argc == 2)
    {
        const auto f = std::find_if(begin(examples),
                                    end(examples),
                                    [argv](const auto & a)
        {
            return a.first == std::string(argv[1]);
        });

        if (f != end(examples))
            return f->second(app);

        return 1;
    }
    else if (argc == 3)
    {
        for (const auto& f : examples)
            std::cout << f.first << std::endl;

        return 0;
    }

    for (const auto& f : examples)
    {
        egt::Timer killtimer(std::chrono::seconds(5));
        killtimer.on_timeout([&app]()
        {
            app.event().quit();
        });
        killtimer.start();
        f.second(app);
    }

    return 1;
}
