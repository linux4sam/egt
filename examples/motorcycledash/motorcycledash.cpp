/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <sys/stat.h>
#include <egt/ui>
#include "eraw.h"


using QueueCallback = std::function<std::string()>;

typedef unsigned char u8;

typedef struct
{
    int y;
    int h;
} st_y_height;

st_y_height g_fuel_table[] =
{
    {173, 176},
    {194, 155},
    {216, 133},
    {238, 111},
    {259, 90},
    {281, 68},
    {303, 46}
};

st_y_height g_temp_table[] =
{
    {324, 25},
    {298, 51},
    {271, 78},
    {245, 104},
    {220, 129},
    {194, 167}
};

size_t getFileSize(const char* fileName);

class MotorDash : public egt::experimental::SVGDeserial
{
public:

    explicit MotorDash(egt::TopWindow& parent)
        : egt::experimental::SVGDeserial(parent)
    {
        set_gear_deserial_state(false);
        set_fuel_deserial_state(false);
        set_temp_deserial_state(false);
        set_speed_deserial_state(false);
        set_call_deserial_state(false);
        set_blink_deserial_state(false);
        set_tc_deserial_state(false);
        set_vsc_deserial_state(false);
        set_bat_deserial_state(false);
        set_snow_deserial_state(false);
        set_wifi_deserial_state(false);
        set_high_q_state(true);
        set_low_q_state(true);
    }

    bool get_high_q_state() { return m_is_high_q_quit; }
    void set_high_q_state(bool is_high_q_quit) { m_is_high_q_quit = is_high_q_quit; }

    bool get_low_q_state() { return m_is_low_q_quit; }
    void set_low_q_state(bool is_low_q_quit) { m_is_low_q_quit = is_low_q_quit; }

    bool get_gear_deserial_state() { return m_gear_deserial_finish; }
    void set_gear_deserial_state(bool gear_deserial_finish) { m_gear_deserial_finish = gear_deserial_finish; }

    bool get_fuel_deserial_state() { return m_fuel_deserial_finish; }
    void set_fuel_deserial_state(bool fuel_deserial_finish) { m_fuel_deserial_finish = fuel_deserial_finish; }

    bool get_temp_deserial_state() { return m_temp_deserial_finish; }
    void set_temp_deserial_state(bool temp_deserial_finish) { m_temp_deserial_finish = temp_deserial_finish; }

    bool get_speed_deserial_state() { return m_speed_deserial_finish; }
    void set_speed_deserial_state(bool speed_deserial_finish) { m_speed_deserial_finish = speed_deserial_finish; }

    bool get_call_deserial_state() { return m_call_deserial_finish; }
    void set_call_deserial_state(bool call_deserial_finish) { m_call_deserial_finish = call_deserial_finish; }

    bool get_blink_deserial_state() { return m_blink_deserial_finish; }
    void set_blink_deserial_state(bool blink_deserial_finish) { m_blink_deserial_finish = blink_deserial_finish; }

    bool get_tc_deserial_state() { return m_tc_deserial_finish; }
    void set_tc_deserial_state(bool tc_deserial_finish) { m_tc_deserial_finish = tc_deserial_finish; }

    bool get_vsc_deserial_state() { return m_vsc_deserial_finish; }
    void set_vsc_deserial_state(bool vsc_deserial_finish) { m_vsc_deserial_finish = vsc_deserial_finish; }

    bool get_bat_deserial_state() { return m_bat_deserial_finish; }
    void set_bat_deserial_state(bool bat_deserial_finish) { m_bat_deserial_finish = bat_deserial_finish; }

    bool get_snow_deserial_state() { return m_snow_deserial_finish; }
    void set_snow_deserial_state(bool snow_deserial_finish) { m_snow_deserial_finish = snow_deserial_finish; }

    bool get_wifi_deserial_state() { return m_wifi_deserial_finish; }
    void set_wifi_deserial_state(bool wifi_deserial_finish) { m_wifi_deserial_finish = wifi_deserial_finish; }

private:
    bool m_is_high_q_quit;
    bool m_is_low_q_quit;
    bool m_gear_deserial_finish;
    bool m_fuel_deserial_finish;
    bool m_temp_deserial_finish;
    bool m_speed_deserial_finish;
    bool m_call_deserial_finish;
    bool m_blink_deserial_finish;
    bool m_tc_deserial_finish;
    bool m_vsc_deserial_finish;
    bool m_bat_deserial_finish;
    bool m_snow_deserial_finish;
    bool m_wifi_deserial_finish;
};

size_t getFileSize(const char* fileName)
{
    if (fileName == NULL)
        return 0;

    struct stat statbuf;
    stat(fileName, &statbuf);
    size_t filesize = statbuf.st_size;
    return filesize;
}


int main(int argc, char** argv)
{
    std::cout << "EGT start" << std::endl;

    std::queue<QueueCallback> high_pri_q;
    std::queue<QueueCallback> low_pri_q;

    //Widgets handler
    std::vector<std::shared_ptr<egt::experimental::GaugeLayer>> NeedleBase;
    std::shared_ptr<egt::experimental::GaugeLayer> TempPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> FuelPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> TempwPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> FuelrPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> takeoffPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> left_blinkPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> right_blinkPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> farlightPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> vscPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> wifiPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> btPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> enginePtr;
    std::shared_ptr<egt::experimental::GaugeLayer> batPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> egoilPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> hazardsPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> snowPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> absPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> mainspeedPtr;
    std::shared_ptr<egt::experimental::GaugeLayer> speed18Ptr;
    std::shared_ptr<egt::experimental::GaugeLayer> speed25Ptr;
    std::shared_ptr<egt::experimental::GaugeLayer> speed61Ptr;
    std::shared_ptr<egt::experimental::GaugeLayer> speed99Ptr;

    egt::Application app(argc, argv);  //This call will cost ~270ms on 9x60ek board

#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::TopWindow window;
    window.color(egt::Palette::ColorId::bg, egt::Palette::black);

    MotorDash motordash(window);

    window.show();

    // Read eraw.bin to buffer
    const std::string eraw_file_path = egt::resolve_file_path("eraw.bin");
    std::string erawname;
    size_t buff_size = getFileSize(eraw_file_path.c_str());
    void* buff_ptr = NULL;
    if (buff_size)
    {
        buff_ptr = malloc(buff_size);
    }
    else
    {
        std::cerr << "eraw.bin is blank" << std::endl;
        return -1;
    }

    std::ifstream f(eraw_file_path, std::ios::binary);
    if (!f)
    {
        std::cout << "read eraw.bin failed" << std::endl;
        free(buff_ptr);
        return -1;
    }
    f.read((char*)buff_ptr, buff_size);

    //offset_table[idx]: idx value meaning please refer to eraw.h
    auto DeserialNeedles = [&]()
    {
        motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[0].offset, offset_table[0].len, true);
        for (auto i = 847, j = 0; i <= 1327; i += 4, j++)
        {
            for (auto k = 1; k < 122; k++)
            {
                erawname = offset_table[k].name;
                if (erawname.substr(0, 4) == "path")
                {
                    std::string pathdigi = erawname.substr(4);
                    if (i == std::stoi(pathdigi))
                    {
                        NeedleBase.push_back(motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[k].offset, offset_table[k].len, false));
                    }
                }
            }
        }
        mainspeedPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[137].offset, offset_table[137].len, true);
    };

    int speed_index = 0;
    int needle_index = 0;
    bool is_increasing = true;
    bool is_needle_finish = false;
    std::string lbd_ret = "0";
    int timer_cnt;
    int temp_index = 0;
    int fuel_index = 0;

    egt::PeriodicTimer timer(std::chrono::milliseconds(10));
    timer.on_timeout([&]()
    {
        //motordash.get_high_q_state/get_low_q_state are used to protect high q not be interrupted by other event re-enter
        //if the state is false, it means this queue has not execute finished yet.
        if (!motordash.get_high_q_state())
            return;
        if (!high_pri_q.empty())
        {
            motordash.set_high_q_state(false);
            lbd_ret = high_pri_q.front()();
            high_pri_q.pop();
            motordash.set_high_q_state(true);
        }
        else
        {
            if (!motordash.get_low_q_state())
                return;
            if (!low_pri_q.empty())
            {
                motordash.set_low_q_state(false);
                lbd_ret = low_pri_q.front()();
                low_pri_q.pop();
                motordash.set_low_q_state(true);
                return;
            }
        }

        //viariable to control the animation and frequency
        timer_cnt = (20000 <= timer_cnt) ? 0 : timer_cnt + 1;

        auto needle_move = [&]()
        {
            for (int i = 0; i < 8; i++)
            {
                if (is_increasing)
                {
                    NeedleBase[needle_index]->show();
                }
                else
                {
                    NeedleBase[needle_index]->hide();
                }

                if (is_increasing && 120 == needle_index)
                {
                    is_increasing = false;
                    is_needle_finish = true;
                    return "needle_move";
                }
                else if (!is_increasing && 0 == needle_index)
                {
                    is_increasing = true;
                    is_needle_finish = true;
                    return "needle_move";
                }
                else
                {
                    needle_index = is_increasing ? needle_index + 1 : needle_index - 1;
                }
            }
            return "needle_move";
        };

        //make sure the needles run a circle finish, then can enter to low priority procedure
        if (is_needle_finish)
        {
            is_needle_finish = false;

            if (!motordash.get_speed_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    speed18Ptr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[140].offset, offset_table[140].len, false);
                    speed25Ptr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[179].offset, offset_table[179].len, false);
                    speed61Ptr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[184].offset, offset_table[184].len, false);
                    speed99Ptr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[190].offset, offset_table[190].len, false);
                    return "lspeed_text";
                });
                low_pri_q.push([&]()
                {
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[151].offset, offset_table[151].len, true);
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[154].offset, offset_table[154].len, true);
                    return "rspeed_text";
                });
                motordash.set_speed_deserial_state(true);
                return;
            }
            else
            {
                if (!(timer_cnt % 3))
                {
                    low_pri_q.push([&]()
                    {
                        if (4 < speed_index)
                            speed_index = 0;
                        switch (speed_index)
                        {
                        case 0:
                            mainspeedPtr->show();
                            speed18Ptr->hide();
                            speed25Ptr->hide();
                            speed61Ptr->hide();
                            speed99Ptr->hide();
                            break;
                        case 1:
                            mainspeedPtr->hide();
                            speed18Ptr->show();
                            speed25Ptr->hide();
                            speed61Ptr->hide();
                            speed99Ptr->hide();
                            break;
                        case 2:
                            mainspeedPtr->hide();
                            speed18Ptr->hide();
                            speed25Ptr->show();
                            speed61Ptr->hide();
                            speed99Ptr->hide();
                            break;
                        case 3:
                            mainspeedPtr->hide();
                            speed18Ptr->hide();
                            speed25Ptr->hide();
                            speed61Ptr->show();
                            speed99Ptr->hide();
                            break;
                        case 4:
                            mainspeedPtr->hide();
                            speed18Ptr->hide();
                            speed25Ptr->hide();
                            speed61Ptr->hide();
                            speed99Ptr->show();
                            break;
                        default:
                            std::cout << "spd full!" << std::endl;
                            break;
                        }
                        speed_index++;
                        return "temp";
                    });
                }
            }

            if (!motordash.get_gear_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[144].offset, offset_table[144].len, true);
                    return "gear_deserial";
                });
                motordash.set_gear_deserial_state(true);
                return;
            }

            if (!motordash.get_temp_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    TempwPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[135].offset, offset_table[135].len, true);
                    TempPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[136].offset, offset_table[136].len, true);
                    return "temp_deserial6";
                });
                motordash.set_temp_deserial_state(true);
                return;
            }
            else
            {
                if (!(timer_cnt % 5))
                {
                    low_pri_q.push([&]()
                    {
                        if (7 <= temp_index)
                            temp_index = 0;
                        switch (temp_index)
                        {
                        case 0:
                            TempPtr->hide();
                            TempwPtr->show();
                            TempwPtr->y(g_temp_table[0].y);
                            TempwPtr->height(g_temp_table[0].h);
                            break;
                        case 1:
                            TempPtr->hide();
                            TempwPtr->show();
                            TempwPtr->y(g_temp_table[1].y);
                            TempwPtr->height(g_temp_table[1].h);
                            break;
                        case 2:
                            TempPtr->hide();
                            TempwPtr->show();
                            TempwPtr->y(g_temp_table[2].y);
                            TempwPtr->height(g_temp_table[2].h);
                            break;
                        case 3:
                            TempPtr->hide();
                            TempwPtr->show();
                            TempwPtr->y(g_temp_table[3].y);
                            TempwPtr->height(g_temp_table[3].h);
                            break;
                        case 4:
                            TempPtr->hide();
                            TempwPtr->show();
                            TempwPtr->y(g_temp_table[4].y);
                            TempwPtr->height(g_temp_table[4].h);
                            break;
                        case 5:
                            TempPtr->hide();
                            TempwPtr->show();
                            TempwPtr->y(g_temp_table[5].y);
                            TempwPtr->height(g_temp_table[5].h);
                            break;
                        case 6:
                            TempwPtr->hide();
                            TempPtr->show();
                            break;
                        default:
                            std::cout << "temp full!" << std::endl;
                            break;
                        }
                        temp_index++;
                        return "temp";
                    });
                }
            }

            if (!motordash.get_fuel_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    FuelrPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[122].offset, offset_table[122].len, true);
                    FuelPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[129].offset, offset_table[129].len, true);
                    return "fuel_deserial";
                });
                motordash.set_fuel_deserial_state(true);
                return;
            }
            else
            {
                if (!(timer_cnt % 3))
                {
                    low_pri_q.push([&]()
                    {
                        if (8 <= fuel_index)
                            fuel_index = 0;
                        switch (fuel_index)
                        {
                        case 0:
                            FuelrPtr->hide();
                            FuelPtr->show();
                            FuelPtr->y(g_fuel_table[0].y);
                            FuelPtr->height(g_fuel_table[0].h);
                            break;
                        case 1:
                            FuelrPtr->hide();
                            FuelPtr->show();
                            FuelPtr->y(g_fuel_table[1].y);
                            FuelPtr->height(g_fuel_table[1].h);
                            break;
                        case 2:
                            FuelrPtr->hide();
                            FuelPtr->show();
                            FuelPtr->y(g_fuel_table[2].y);
                            FuelPtr->height(g_fuel_table[2].h);
                            break;
                        case 3:
                            FuelrPtr->hide();
                            FuelPtr->show();
                            FuelPtr->y(g_fuel_table[3].y);
                            FuelPtr->height(g_fuel_table[3].h);
                            break;
                        case 4:
                            FuelrPtr->hide();
                            FuelPtr->show();
                            FuelPtr->y(g_fuel_table[4].y);
                            FuelPtr->height(g_fuel_table[4].h);
                            break;
                        case 5:
                            FuelrPtr->hide();
                            FuelPtr->show();
                            FuelPtr->y(g_fuel_table[5].y);
                            FuelPtr->height(g_fuel_table[5].h);
                            break;
                        case 6:
                            FuelrPtr->hide();
                            FuelPtr->show();
                            FuelPtr->y(g_fuel_table[6].y);
                            FuelPtr->height(g_fuel_table[6].h);
                            break;
                        case 7:
                            FuelPtr->hide();
                            FuelrPtr->show();
                            break;
                        default:
                            std::cout << "fuel full!" << std::endl;
                            break;
                        }
                        fuel_index++;
                        return "fuel";
                    });
                }
            }

            if (!motordash.get_call_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[145].offset, offset_table[145].len, true);
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[148].offset, offset_table[148].len, true);
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[175].offset, offset_table[175].len, true);
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[176].offset, offset_table[176].len, true);
                    return "callname_text";
                });
                low_pri_q.push([&]()
                {
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[157].offset, offset_table[157].len, true);
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[178].offset, offset_table[178].len, true);
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[177].offset, offset_table[177].len, true);
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[125].offset, offset_table[125].len, true);
                    return "phone_text";
                });
                low_pri_q.push([&]()
                {
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[133].offset, offset_table[133].len, true);
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[165].offset, offset_table[165].len, true);
                    return "call_text";
                });
                motordash.set_call_deserial_state(true);
                return;
            }

            if (!motordash.get_tc_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[160].offset, offset_table[160].len, true);
                    return "eng5tc_text";
                });
                motordash.set_tc_deserial_state(true);
                return;
            }

            if (!motordash.get_blink_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    left_blinkPtr =  motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[161].offset, offset_table[161].len, true);
                    right_blinkPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[173].offset, offset_table[173].len, true);
                    return "blink_deserial";
                });
                motordash.set_blink_deserial_state(true);
                return;
            }
            else
            {
                if (!(timer_cnt % 3))
                {
                    low_pri_q.push([&]()
                    {
                        left_blinkPtr->visible_toggle();
                        right_blinkPtr->visible_toggle();
                        return "left_right_blink";
                    });
                }
            }

            if (!motordash.get_vsc_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    vscPtr =  motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[164].offset, offset_table[164].len, true);
                    farlightPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[163].offset, offset_table[163].len, true);
                    takeoffPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[162].offset, offset_table[162].len, true);
                    return "vsc_deserial";
                });
                motordash.set_vsc_deserial_state(true);
                return;
            }
            else
            {
                if (!(timer_cnt % 5))
                {
                    low_pri_q.push([&]()
                    {
                        vscPtr->visible_toggle();
                        farlightPtr->visible_toggle();
                        takeoffPtr->visible_toggle();
                        return "vsc_farlight_takeoff";
                    });
                }
            }

            if (!motordash.get_bat_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    batPtr =  motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[166].offset, offset_table[166].len, true);
                    egoilPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[167].offset, offset_table[167].len, true);
                    hazardsPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[168].offset, offset_table[168].len, true);
                    return "bat_deserial";
                });
                motordash.set_bat_deserial_state(true);
                return;
            }
            else
            {
                if (!(timer_cnt % 9))
                {
                    low_pri_q.push([&]()
                    {
                        batPtr->visible_toggle();
                        egoilPtr->visible_toggle();
                        hazardsPtr->visible_toggle();
                        return "bat_egoil_hazards";
                    });
                }
            }

            if (!motordash.get_snow_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    snowPtr =  motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[169].offset, offset_table[169].len, true);
                    absPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[170].offset, offset_table[170].len, true);
                    enginePtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[174].offset, offset_table[174].len, true);
                    return "snow_deserial";
                });
                motordash.set_snow_deserial_state(true);
                return;
            }
            else
            {
                if (!(timer_cnt % 7))
                {
                    low_pri_q.push([&]()
                    {
                        snowPtr->visible_toggle();
                        absPtr->visible_toggle();
                        enginePtr->visible_toggle();
                        return "snow_abs_engin";
                    });
                }
            }

            if (!motordash.get_wifi_deserial_state())
            {
                low_pri_q.push([&]()
                {
                    wifiPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[171].offset, offset_table[171].len, true);
                    btPtr = motordash.AddWidgetByBuf((const u8*)buff_ptr + offset_table[172].offset, offset_table[172].len, true);
                    free(buff_ptr);
                    return "wifi_deserial";
                });
                motordash.set_wifi_deserial_state(true);
                return;
            }
            else
            {
                if (!(timer_cnt % 9))
                {
                    low_pri_q.push([&]()
                    {
                        wifiPtr->visible_toggle();
                        btPtr->visible_toggle();
                        return "wifi_bt";
                    });
                }
            }
        }
        else  //this branch exec the high priority task
        {
            high_pri_q.push(needle_move);
        }
    });

    DeserialNeedles();
    std::cout << "EGT show" << std::endl;
    timer.start();
    return app.run();
}
