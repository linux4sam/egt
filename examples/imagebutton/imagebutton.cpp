/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[Example]
#include <egt/ui>
#include <sstream>
#include <iostream>

class TestButton : public egt::VerticalBoxSizer
{
public:
    TestButton(Frame& parent, const egt::Image& image, const std::string& text, const egt::Rect& rect, bool auto_scale, bool auto_resize)
        : egt::VerticalBoxSizer(parent),
          m_button(*this, image, text, rect)
    {
        m_button.auto_scale_image(auto_scale);
        m_button.autoresize(auto_resize);

        std::ostringstream ss_size;
        ss_size << "requested size: " << rect.size();
        m_label_size = std::make_shared<egt::Label>(*this, ss_size.str());

        std::ostringstream ss_scale;
        ss_scale << "auto scale: " << (auto_scale ? "true" : "false");
        m_label_as = std::make_shared<egt::Label>(*this, ss_scale.str());

        std::ostringstream ss_resize;
        ss_resize << "auto resize: " << (auto_resize ? "true" : "false");
        m_label_ar = std::make_shared<egt::Label>(*this, ss_resize.str());
    }

private:
    egt::ImageButton m_button;
    std::shared_ptr<egt::Label> m_label_size;
    std::shared_ptr<egt::Label> m_label_as;
    std::shared_ptr<egt::Label> m_label_ar;
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;

    egt::ScrolledView view(window);
    egt::expand(view);

    egt::VerticalBoxSizer vbox(view);

    egt::Label lbl0(vbox, "ImageButton with text, original image size is 128x128", egt::Rect(0, 0, 1400, 50));
    egt::HorizontalBoxSizer hbox_big(vbox);
    egt::expand_horizontal(hbox_big);
    hbox_big.justify(egt::Justification::justify);
    TestButton b01(hbox_big, egt::Image("icon:mgs_logo_icon.png;128"), std::string{"test"}, egt::Rect(0, 0, 100, 50), true, false);
    TestButton b02(hbox_big, egt::Image("icon:mgs_logo_icon.png;128"), std::string{"test"}, egt::Rect(0, 0, 100, 50), false, false);
    TestButton b03(hbox_big, egt::Image("icon:mgs_logo_icon.png;128"), std::string{"test"}, egt::Rect(0, 0, 100, 50), true, true);
    TestButton b04(hbox_big, egt::Image("icon:mgs_logo_icon.png;128"), std::string{"test"}, egt::Rect(0, 0, 100, 50), false, true);
    TestButton b05(hbox_big, egt::Image("icon:mgs_logo_icon.png;128"), std::string{"test"}, egt::Rect(0, 0, 0, 0), true, false);
    TestButton b06(hbox_big, egt::Image("icon:mgs_logo_icon.png;128"), std::string{"test"}, egt::Rect(0, 0, 0, 0), false, false);
    TestButton b07(hbox_big, egt::Image("icon:mgs_logo_icon.png;128"), std::string{"test"}, egt::Rect(0, 0, 0, 0), true, true);
    TestButton b08(hbox_big, egt::Image("icon:mgs_logo_icon.png;128"), std::string{"test"}, egt::Rect(0, 0, 0, 0), false, true);

    egt::Label lbl1(vbox, "ImageButton without text, original image size is 128x128", egt::Rect(0, 0, 1400, 50));
    egt::HorizontalBoxSizer hbox_big2(vbox);
    egt::expand_horizontal(hbox_big2);
    hbox_big2.justify(egt::Justification::justify);
    TestButton b11(hbox_big2, egt::Image("icon:mgs_logo_icon.png;128"), std::string{}, egt::Rect(0, 0, 100, 50), true, false);
    TestButton b12(hbox_big2, egt::Image("icon:mgs_logo_icon.png;128"), std::string{}, egt::Rect(0, 0, 100, 50), false, false);
    TestButton b13(hbox_big2, egt::Image("icon:mgs_logo_icon.png;128"), std::string{}, egt::Rect(0, 0, 100, 50), true, true);
    TestButton b14(hbox_big2, egt::Image("icon:mgs_logo_icon.png;128"), std::string{}, egt::Rect(0, 0, 100, 50), false, true);
    TestButton b15(hbox_big2, egt::Image("icon:mgs_logo_icon.png;128"), std::string{}, egt::Rect(0, 0, 0, 0), true, false);
    TestButton b16(hbox_big2, egt::Image("icon:mgs_logo_icon.png;128"), std::string{}, egt::Rect(0, 0, 0, 0), false, false);
    TestButton b17(hbox_big2, egt::Image("icon:mgs_logo_icon.png;128"), std::string{}, egt::Rect(0, 0, 0, 0), true, true);
    TestButton b18(hbox_big2, egt::Image("icon:mgs_logo_icon.png;128"), std::string{}, egt::Rect(0, 0, 0, 0), false, true);

    egt::Label lbl2(vbox, "ImageButton with text, original image size is 32x32", egt::Rect(0, 0, 1400, 50));
    egt::HorizontalBoxSizer hbox_small(vbox);
    egt::expand_horizontal(hbox_small);
    hbox_small.justify(egt::Justification::justify);
    TestButton b21(hbox_small, egt::Image("icon:mgs_logo_icon.png;32"), std::string{"test"}, egt::Rect(0, 0, 100, 50), true, false);
    TestButton b22(hbox_small, egt::Image("icon:mgs_logo_icon.png;32"), std::string{"test"}, egt::Rect(0, 0, 100, 50), false, false);
    TestButton b23(hbox_small, egt::Image("icon:mgs_logo_icon.png;32"), std::string{"test"}, egt::Rect(0, 0, 100, 50), true, true);
    TestButton b24(hbox_small, egt::Image("icon:mgs_logo_icon.png;32"), std::string{"test"}, egt::Rect(0, 0, 100, 50), false, true);
    TestButton b25(hbox_small, egt::Image("icon:mgs_logo_icon.png;32"), std::string{"test"}, egt::Rect(0, 0, 0, 0), true, false);
    TestButton b26(hbox_small, egt::Image("icon:mgs_logo_icon.png;32"), std::string{"test"}, egt::Rect(0, 0, 0, 0), false, false);
    TestButton b27(hbox_small, egt::Image("icon:mgs_logo_icon.png;32"), std::string{"test"}, egt::Rect(0, 0, 0, 0), true, true);
    TestButton b28(hbox_small, egt::Image("icon:mgs_logo_icon.png;32"), std::string{"test"}, egt::Rect(0, 0, 0, 0), false, true);

    egt::Label lbl3(vbox, "ImageButton without text, original image size is 32x32", egt::Rect(0, 0, 1400, 50));
    egt::HorizontalBoxSizer hbox_small2(vbox);
    egt::expand_horizontal(hbox_small2);
    hbox_small2.justify(egt::Justification::justify);
    TestButton b31(hbox_small2, egt::Image("icon:mgs_logo_icon.png;32"), std::string{}, egt::Rect(0, 0, 100, 50), true, false);
    TestButton b32(hbox_small2, egt::Image("icon:mgs_logo_icon.png;32"), std::string{}, egt::Rect(0, 0, 100, 50), false, false);
    TestButton b33(hbox_small2, egt::Image("icon:mgs_logo_icon.png;32"), std::string{}, egt::Rect(0, 0, 100, 50), true, true);
    TestButton b34(hbox_small2, egt::Image("icon:mgs_logo_icon.png;32"), std::string{}, egt::Rect(0, 0, 100, 50), false, true);
    TestButton b35(hbox_small2, egt::Image("icon:mgs_logo_icon.png;32"), std::string{}, egt::Rect(0, 0, 0, 0), true, false);
    TestButton b36(hbox_small2, egt::Image("icon:mgs_logo_icon.png;32"), std::string{}, egt::Rect(0, 0, 0, 0), false, false);
    TestButton b37(hbox_small2, egt::Image("icon:mgs_logo_icon.png;32"), std::string{}, egt::Rect(0, 0, 0, 0), true, true);
    TestButton b38(hbox_small2, egt::Image("icon:mgs_logo_icon.png;32"), std::string{}, egt::Rect(0, 0, 0, 0), false, true);

    window.show();

    return app.run();
}
/// @[Example]
