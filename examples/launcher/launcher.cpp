/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui.h>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace mui;

static float sliding_scale(int win_w, int item_w, int item_pos,
                           float min = 0.3, float max = 2.0)
{
    float range = win_w / 2;
    float delta = fabs(range - (item_pos + (item_w / 2)));
    float scale = 1.0 - (delta / range);
    if (scale < min || scale > max)
        return min;
    return scale;
}

#define NUM_ITEMS 10

class LauncherWindow;
class ThermostatWindow;

static LauncherWindow* win1;
static ThermostatWindow* win2;
static Window* win3;
static Window* win4;


class MyImage : public Image
{
public:
    MyImage(LauncherWindow* win, Window* target, const string& filename, int x = 0, int y = 0)
        : Image(filename, x, y),
          m_win(win),
          m_target(target),
          m_fd(-1),
          m_animation(0, 600, MyImage::animate, 1000, easing_snap, this)
    {}

    static void animate(float value, void* data)
    {
        MyImage* image = reinterpret_cast<MyImage*>(data);
        assert(image);

        image->scale(value, value);
    }

    static void timer_callback(int fd, void* data);

    int handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
        {
            if (!m_animation.running())
            {
                m_animation.set_easing_func(easing_snap);
                m_animation.starting(hscale());
                m_animation.ending(hscale() + 0.2);
                m_animation.duration(500);
                m_animation.start();
                m_fd = EventLoop::start_periodic_timer(1, MyImage::timer_callback, this);
                return 1;
            }

            break;
        }
        case EVT_MOUSE_UP:
            //EventLoop::quit();
            break;
        }

        return Image::handle(event);
    }

private:
    LauncherWindow* m_win;
    Window* m_target;
    int m_fd;
    Animation m_animation;
};

class ThermostatWindow : public Window
{
    WidgetPositionAnimator* m_a1;
    WidgetPositionAnimator* m_a2;
    WidgetPositionAnimator* m_a3;

    ImageLabel* il1;
    ImageLabel* il2;
    ImageLabel* il3;
    Label* l1;

public:
    ThermostatWindow(const Size& size = Size())
        : Window(size)
    {
    }

    virtual void exit()
    {
        Window::exit();

        m_a1->reset();
        m_a2->reset();
        m_a3->reset();
    }

    virtual void enter()
    {
        Window::enter();

        m_a1->start();
        m_a2->start();
        m_a3->start();
    }

    virtual int load()
    {
        Image* img = new Image("background2.png");
        add(img);

        Radial* radial1 = new Radial(Point(800 / 2 - 350 / 2, 480 / 2 - 350 / 2), Size(350, 350));
        add(radial1);
        radial1->label(" F");
        radial1->value(300);

        m_a2 = new WidgetPositionAnimator(radial1,
                                          WidgetPositionAnimator::CORD_Y,
                                          -350, 480 / 2 - 350 / 2,
                                          1000,
                                          easing_snap);

        il1 = new ImageLabel("day.png",
                             "Day",
                             Point(40, 150),
                             Size(200, 64),
                             mui::Font(32));
        il1->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color(0x80808055))
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(il1);
        il2 = new ImageLabel("night.png",
                             "Night",
                             Point(40, 214),
                             Size(200, 64),
                             mui::Font(32));
        il2->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color(0x80808055))
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(il2);
        il3 = new ImageLabel("vacation.png",
                             "Vacation",
                             Point(40, 278),
                             Size(200, 64),
                             mui::Font(32));
        il3->palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(il3);

        m_a1 = new WidgetPositionAnimator({il1, il2, il3},
                                          WidgetPositionAnimator::CORD_X,
                                          -200, 40,
                                          1500,
                                          easing_snap);

        Slider* slider1 = new Slider(0, 100,
                                     Point(700, 100),
                                     Size(50, 300),
                                     Slider::ORIENTATION_VERTICAL);
        add(slider1);
        slider1->position(50);

        l1 = new Label("Fan",
                       Point(700, 390),
                       Size(50, 64),
                       ALIGN_CENTER,
                       mui::Font(16));
        add(l1);
        l1->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY)
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        m_a3 = new WidgetPositionAnimator({slider1, l1},
                                          WidgetPositionAnimator::CORD_X,
                                          800, 700,
                                          1500,
                                          easing_snap);

        m_a1->reset();
        m_a2->reset();
        m_a3->reset();

        return 0;
    }
};

class LauncherWindow : public Window
{
public:
    LauncherWindow()
        : m_moving(false)
    {}

    virtual int load()
    {
        Image* img = new Image("background.png");
        add(img);

        for (int t = 0; t < NUM_ITEMS; t++)
        {
            Window* win = 0;
            switch (t)
            {
            case 0:
                win = win2;
                break;
            case 1:
                win = win3;
                break;
            case 2:
                win = win4;
                break;
            }

            stringstream os;
            os << "_image" << t << ".png";
            MyImage* box = new MyImage(this, (Window*)win, os.str());
            add(box);

            box->position(t * 200, (h() / 2) - (box->h() / 2));
            m_boxes[t] = box;

            scale_box(box, t * 200);
        }

        for (int t = 0; t < NUM_ITEMS; t++)
            m_positions[t] = m_boxes[t]->x();

        return 0;
    }

    int handle(int event)
    {
        int ret = Window::handle(event);
        if (ret)
            return 1;

        switch (event)
        {
        case EVT_MOUSE_DOWN:
            if (!m_moving)
            {
                m_moving = true;
                m_moving_x = mouse_position().x;
                for (int t = 0; t < NUM_ITEMS; t++)
                    m_positions[t] = m_boxes[t]->x();
            }

            return 1;
            break;
        case EVT_MOUSE_UP:
            m_moving = false;
            return 1;
            break;
        case EVT_MOUSE_MOVE:

            if (m_moving)
            {
                move();
                return 1;
            }
            break;
        }

        return 0;
    }

    void move()
    {
        int diff = mouse_position().x - m_moving_x;

        for (int t = 0; t < NUM_ITEMS; t++)
        {
            int pos = m_positions[t] + diff;

            Rect to(m_boxes[t]->box());
            to.x = pos;
            bool visible = Rect::is_intersect(Rect::merge(to, m_boxes[t]->box()), this->box());

            if (visible)
            {
                scale_box(m_boxes[t], pos);
                m_boxes[t]->move(pos, m_boxes[t]->y());
            }
            else
            {
                m_boxes[t]->position(pos, m_boxes[t]->y());
            }
        }
    }

    void scale_box(MyImage* image, int pos)
    {
        float scale = sliding_scale(this->w(), image->w(), pos);
        image->scale(scale, scale);
    }

private:
    bool m_moving;
    int m_moving_x;
    int m_positions[NUM_ITEMS];
    MyImage* m_boxes[NUM_ITEMS];
};

void MyImage::timer_callback(int fd, void* data)
{
    MyImage* image = reinterpret_cast<MyImage*>(data);
    assert(image);

    if (!image->m_animation.next())
    {
        image->m_win->scale_box(image, image->x());
        EventLoop::cancel_periodic_timer(image->m_fd);

        if (image->m_target)
        {
            main_window()->exit();
            main_window() = image->m_target;
            main_window()->enter();
        }
    }
}

int main()
{
    Application app;

    set_image_path("/root/mui/share/mui/examples/launcher/");

    win1 = new LauncherWindow;

    win2 = new ThermostatWindow;
    win2->load();

    struct MyButton : public Image
    {
        MyButton(const std::string& image, const Point& point = Point())
            : Image(image, point.x, point.y)
        {}

        int handle(int event)
        {
            switch (event)
            {
            case EVT_MOUSE_DOWN:
            {
                main_window()->exit();
                main_window() = win1;
                main_window()->enter();
                return 1;
            }
            }
            return Image::handle(event);
        }
    };

    win3 = new Window();
    win3->palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

    PieChart pie2(Point(200, 40), Size(400, 400));
    win3->add(&pie2);

    std::map<std::string, float> data;
    data.insert(make_pair("truck", .25));
    data.insert(make_pair("car", .55));
    data.insert(make_pair("bike", .10));
    data.insert(make_pair("motorcycle", .10));
    pie2.data(data);

    win4 = new Window();
    win4->palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

#ifdef HAVE_KPLOT
    Chart chart1(Point(0, 0), Size(800, 480));
    win4->add(&chart1);
    std::vector<struct kpair> data2;
    for (int i = 0; i < 500; i++)
    {
        struct kpair p;
        p.x = i;
        p.y = sinf(float(i) / 10.);
        data2.push_back(p);
    }
    chart1.data(data2);
#endif

    MyButton home1("home.png", Point(10, 10));
    win1->add(&home1);
    MyButton home2("home.png", Point(10, 10));
    win2->add(&home2);
    MyButton home3("home.png", Point(10, 10));
    win3->add(&home3);
    MyButton home4("home.png", Point(10, 10));
    win4->add(&home4);

    win1->load();

    return app.run();
}
