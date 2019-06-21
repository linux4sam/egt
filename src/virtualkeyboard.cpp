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
using Panel = VirtualKeyboard::Panel;

static PopupVirtualKeyboard* the_popup_virtual_keyboard = nullptr;

struct Multichoice_w : public Panel
{
    Multichoice_w()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0),
            make_shared<VirtualKeyboard::Key>(0x0),
            make_shared<VirtualKeyboard::Key>(0x0),
        }, {
            make_shared<VirtualKeyboard::Key>(0x0),
            make_shared<VirtualKeyboard::Key>(0x0),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_e : public Panel
{
    Multichoice_e()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_E : public Panel
{
    Multichoice_E()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_r : public Panel
{
    Multichoice_r()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0155),
            make_shared<VirtualKeyboard::Key>(0x0159),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_R : public Panel
{
    Multichoice_R()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0154),
            make_shared<VirtualKeyboard::Key>(0x0158),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_t : public Panel
{
    Multichoice_t()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0fe),
            make_shared<VirtualKeyboard::Key>(0x0165),
            make_shared<VirtualKeyboard::Key>(0x021b),
            make_shared<VirtualKeyboard::Key>(0x0163),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_T : public Panel
{
    Multichoice_T()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0fe),
            make_shared<VirtualKeyboard::Key>(0x0164),
            make_shared<VirtualKeyboard::Key>(0x021a),
            make_shared<VirtualKeyboard::Key>(0x0162),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_y : public Panel
{
    Multichoice_y()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0fd),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_Y : public Panel
{
    Multichoice_Y()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0dd),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_u : public Panel
{
    Multichoice_u()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_U : public Panel
{
    Multichoice_U()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_i : public Panel
{
    Multichoice_i()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_I : public Panel
{
    Multichoice_I()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_o : public Panel
{
    Multichoice_o()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_O : public Panel
{
    Multichoice_O()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_a : public Panel
{
    Multichoice_a()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_A : public Panel
{
    Multichoice_A()
        : Panel(
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
    }, Size(45, 75))
    {}
};

struct Multichoice_s : public Panel
{
    Multichoice_s()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00df),
            make_shared<VirtualKeyboard::Key>(0x00a7),
            make_shared<VirtualKeyboard::Key>(0x015b),
            make_shared<VirtualKeyboard::Key>(0x0161),
            make_shared<VirtualKeyboard::Key>(0x015f),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_S : public Panel
{
    Multichoice_S()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00df),
            make_shared<VirtualKeyboard::Key>(0x00a7),
            make_shared<VirtualKeyboard::Key>(0x015a),
            make_shared<VirtualKeyboard::Key>(0x0160),
            make_shared<VirtualKeyboard::Key>(0x015e),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_d : public Panel
{
    Multichoice_d()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x010f),
            make_shared<VirtualKeyboard::Key>(0x0111),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_D : public Panel
{
    Multichoice_D()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x010e),
            make_shared<VirtualKeyboard::Key>(0x0110),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_g : public Panel
{
    Multichoice_g()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0123),
            make_shared<VirtualKeyboard::Key>(0x011f),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_G : public Panel
{
    Multichoice_G()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0122),
            make_shared<VirtualKeyboard::Key>(0x011e),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_k : public Panel
{
    Multichoice_k()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0137),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_K : public Panel
{
    Multichoice_K()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0136),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_l : public Panel
{
    Multichoice_l()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0142),
            make_shared<VirtualKeyboard::Key>(0x013e),
            make_shared<VirtualKeyboard::Key>(0x013c),
            make_shared<VirtualKeyboard::Key>(0x013a),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_L : public Panel
{
    Multichoice_L()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0141),
            make_shared<VirtualKeyboard::Key>(0x013d),
            make_shared<VirtualKeyboard::Key>(0x013b),
            make_shared<VirtualKeyboard::Key>(0x0139),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_z : public Panel
{
    Multichoice_z()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x017a),
            make_shared<VirtualKeyboard::Key>(0x017c),
            make_shared<VirtualKeyboard::Key>(0x017e),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_Z : public Panel
{
    Multichoice_Z()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0179),
            make_shared<VirtualKeyboard::Key>(0x017b),
            make_shared<VirtualKeyboard::Key>(0x017d),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_c : public Panel
{
    Multichoice_c()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00e7),
            make_shared<VirtualKeyboard::Key>(0x0107),
            make_shared<VirtualKeyboard::Key>(0x010d),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_C : public Panel
{
    Multichoice_C()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x00c7),
            make_shared<VirtualKeyboard::Key>(0x0106),
            make_shared<VirtualKeyboard::Key>(0x010c),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_n : public Panel
{
    Multichoice_n()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0148),
            make_shared<VirtualKeyboard::Key>(0x0146),
            make_shared<VirtualKeyboard::Key>(0x0144),
            make_shared<VirtualKeyboard::Key>(0x00f1),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_N : public Panel
{
    Multichoice_N()
        : Panel(
    {
        {
            make_shared<VirtualKeyboard::Key>(0x0147),
            make_shared<VirtualKeyboard::Key>(0x0145),
            make_shared<VirtualKeyboard::Key>(0x0143),
            make_shared<VirtualKeyboard::Key>(0x00d1),
        }
    }, Size(45, 75))
    {}
};

struct QwertyLettersLowerCase : public Panel
{
    QwertyLettersLowerCase()
        : Panel(
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
            make_shared<VirtualKeyboard::Key>(0x0065, make_shared<Multichoice_e>()),
            make_shared<VirtualKeyboard::Key>(0x0072, make_shared<Multichoice_r>()),
            make_shared<VirtualKeyboard::Key>(0x0074, make_shared<Multichoice_t>()),
            make_shared<VirtualKeyboard::Key>(0x0079, make_shared<Multichoice_y>()),
            make_shared<VirtualKeyboard::Key>(0x0075, make_shared<Multichoice_u>()),
            make_shared<VirtualKeyboard::Key>(0x0069, make_shared<Multichoice_i>()),
            make_shared<VirtualKeyboard::Key>(0x006f, make_shared<Multichoice_o>()),
            make_shared<VirtualKeyboard::Key>(0x0070)
        }, {
            make_shared<VirtualKeyboard::Key>(0x0061, make_shared<Multichoice_a>()),
            make_shared<VirtualKeyboard::Key>(0x0073, make_shared<Multichoice_s>()),
            make_shared<VirtualKeyboard::Key>(0x0064, make_shared<Multichoice_d>()),
            make_shared<VirtualKeyboard::Key>(0x0066),
            make_shared<VirtualKeyboard::Key>(0x0067, make_shared<Multichoice_g>()),
            make_shared<VirtualKeyboard::Key>(0x0068),
            make_shared<VirtualKeyboard::Key>(0x006a),
            make_shared<VirtualKeyboard::Key>(0x006b, make_shared<Multichoice_k>()),
            make_shared<VirtualKeyboard::Key>(0x006c, make_shared<Multichoice_l>())
        }, {
            make_shared<VirtualKeyboard::Key>("\u21d1", 1, 1.5),
            make_shared<VirtualKeyboard::Key>(0x007a, make_shared<Multichoice_z>()),
            make_shared<VirtualKeyboard::Key>(0x0078),
            make_shared<VirtualKeyboard::Key>(0x0063, make_shared<Multichoice_c>()),
            make_shared<VirtualKeyboard::Key>(0x0076),
            make_shared<VirtualKeyboard::Key>(0x0062),
            make_shared<VirtualKeyboard::Key>(0x006e, make_shared<Multichoice_n>()),
            make_shared<VirtualKeyboard::Key>(0x006d),
            make_shared<VirtualKeyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<VirtualKeyboard::Key>("!#\u263a", 2, 1.5),
            make_shared<VirtualKeyboard::Key>(0x0),
            make_shared<VirtualKeyboard::Key>(0x0020, 5.0),
            make_shared<VirtualKeyboard::Key>(0x002e),
            make_shared<VirtualKeyboard::Key>("\u21b5", EKEY_ENTER, 1.5)
        }
    }, Size(30, 50), 4)
    {}
};

struct QwertyLettersUpperCase : public Panel
{
    QwertyLettersUpperCase()
        : Panel(
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
            make_shared<VirtualKeyboard::Key>(0x0045, make_shared<Multichoice_E>()),
            make_shared<VirtualKeyboard::Key>(0x0052, make_shared<Multichoice_R>()),
            make_shared<VirtualKeyboard::Key>(0x0054, make_shared<Multichoice_T>()),
            make_shared<VirtualKeyboard::Key>(0x0059, make_shared<Multichoice_Y>()),
            make_shared<VirtualKeyboard::Key>(0x0055, make_shared<Multichoice_U>()),
            make_shared<VirtualKeyboard::Key>(0x0049, make_shared<Multichoice_I>()),
            make_shared<VirtualKeyboard::Key>(0x004f, make_shared<Multichoice_O>()),
            make_shared<VirtualKeyboard::Key>(0x0050)
        }, {
            make_shared<VirtualKeyboard::Key>(0x0041, make_shared<Multichoice_A>()),
            make_shared<VirtualKeyboard::Key>(0x0053, make_shared<Multichoice_S>()),
            make_shared<VirtualKeyboard::Key>(0x0044, make_shared<Multichoice_D>()),
            make_shared<VirtualKeyboard::Key>(0x0046),
            make_shared<VirtualKeyboard::Key>(0x0047, make_shared<Multichoice_G>()),
            make_shared<VirtualKeyboard::Key>(0x0048),
            make_shared<VirtualKeyboard::Key>(0x004a),
            make_shared<VirtualKeyboard::Key>(0x004b, make_shared<Multichoice_K>()),
            make_shared<VirtualKeyboard::Key>(0x004c, make_shared<Multichoice_L>())
        }, {
            make_shared<VirtualKeyboard::Key>("\u21d1", 0, 1.5),
            make_shared<VirtualKeyboard::Key>(0x005a, make_shared<Multichoice_Z>()),
            make_shared<VirtualKeyboard::Key>(0x0058),
            make_shared<VirtualKeyboard::Key>(0x0043, make_shared<Multichoice_C>()),
            make_shared<VirtualKeyboard::Key>(0x0056),
            make_shared<VirtualKeyboard::Key>(0x0042),
            make_shared<VirtualKeyboard::Key>(0x004e, make_shared<Multichoice_N>()),
            make_shared<VirtualKeyboard::Key>(0x004d),
            make_shared<VirtualKeyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<VirtualKeyboard::Key>("!#\u263a", 2, 1.5),
            make_shared<VirtualKeyboard::Key>(0x0),
            make_shared<VirtualKeyboard::Key>(0x0020, 5.0),
            make_shared<VirtualKeyboard::Key>(0x002e),
            make_shared<VirtualKeyboard::Key>("\u21b5", EKEY_ENTER, 1.5)
        }
    }, Size(30, 50), 4)
    {}
};

struct QwertySymbols1 : public Panel
{
    QwertySymbols1()
        : Panel(
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
            make_shared<VirtualKeyboard::Key>(0x0),
            make_shared<VirtualKeyboard::Key>(0x0020, 5.0),
            make_shared<VirtualKeyboard::Key>(0x002e),
            make_shared<VirtualKeyboard::Key>("\u21b5", EKEY_ENTER, 1.5)
        }
    }, Size(30, 50), 4)
    {}
};

struct QwertySymbols2 : public Panel
{
    QwertySymbols2()
        : Panel(
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
            make_shared<VirtualKeyboard::Key>(0x00b0),
            make_shared<VirtualKeyboard::Key>(0x2022),
            make_shared<VirtualKeyboard::Key>(0x25cb),
            make_shared<VirtualKeyboard::Key>(0x25cf),
            make_shared<VirtualKeyboard::Key>(0x25a1),
            make_shared<VirtualKeyboard::Key>(0x25a0),
            make_shared<VirtualKeyboard::Key>(0x2664),
            make_shared<VirtualKeyboard::Key>(0x2661),
            make_shared<VirtualKeyboard::Key>(0x2662),
            make_shared<VirtualKeyboard::Key>(0x2667)
        }, {
            make_shared<VirtualKeyboard::Key>("2/2", 2, 1.5),
            make_shared<VirtualKeyboard::Key>(0x2606),
            make_shared<VirtualKeyboard::Key>(0x25aa),
            make_shared<VirtualKeyboard::Key>(0x0), //
            make_shared<VirtualKeyboard::Key>(0x00ab),
            make_shared<VirtualKeyboard::Key>(0x00bb),
            make_shared<VirtualKeyboard::Key>(0x00a1),
            make_shared<VirtualKeyboard::Key>(0x00bf),
            make_shared<VirtualKeyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<VirtualKeyboard::Key>("ABC", 0, 1.5),
            make_shared<VirtualKeyboard::Key>(0x0),
            make_shared<VirtualKeyboard::Key>(0x0020, 5.0),
            make_shared<VirtualKeyboard::Key>(0x002e),
            make_shared<VirtualKeyboard::Key>("\u21b5", EKEY_ENTER, 1.5)
        }
    }, Size(30, 50), 4)
    {}
};

VirtualKeyboard::Key::Key(uint32_t unicode, double length)
    : m_button(make_shared<Button>()),
      m_unicode(unicode),
      m_length(length)
{
    string tmp;
    utf8::append(unicode, std::back_inserter(tmp));
    m_button->set_text(tmp);
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(const std::string& label, KeyboardCode keycode, double length)
    : m_button(make_shared<Button>(label)),
      m_keycode(keycode),
      m_length(length)
{
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(const string& label, int link, double length)
    : m_button(make_shared<Button>(label)),
      m_link(link),
      m_length(length)
{
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(uint32_t unicode,
                          shared_ptr<Panel> multichoice,
                          double length, KeyboardCode keycode)
    : m_button(make_shared<Button>()),
      m_unicode(unicode),
      m_length(length),
      m_multichoice(multichoice)
{
    string tmp;
    utf8::append(unicode, std::back_inserter(tmp));
    m_button->set_text(tmp);
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

Panel::Panel(vector<vector<shared_ptr<VirtualKeyboard::Key>>> keys,
             Size key_size,
             int spacing)
    : m_vsizer(make_shared<VerticalBoxSizer>()),
      m_keys(std::move(keys))
{
    set_align(alignmask::center);

    m_vsizer->set_align(alignmask::center);
    add(m_vsizer);

    for (auto& row : m_keys)
    {
        auto hsizer = make_shared<HorizontalBoxSizer>();
        hsizer->set_align(alignmask::center | alignmask::top);
        m_vsizer->add(hsizer);

        for (auto& key : row)
        {
            key->m_button->resize(Size(key_size.width * key->length() + 2 * spacing,
                                       key_size.height + 2 * spacing));
            key->m_button->set_margin(spacing / 2);

            hsizer->add(key->m_button);
        }
    }
}

void VirtualKeyboard::set_key_link(const shared_ptr<Key>& k)
{
    k->m_button->on_event([this, k](Event&)
    {
        m_main_panel.set_select(k->link());
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

void VirtualKeyboard::set_key_multichoice(const shared_ptr<Key>& k, unsigned id)
{
    for (auto& multichoice_raw : k->m_multichoice->m_keys)
    {
        for (auto& multichoice_key : multichoice_raw)
        {
            multichoice_key->m_button->on_event([this, k, multichoice_key](Event & event)
            {
                // hide popup first as it is modal
                m_multichoice_popup.hide();

                if (!multichoice_key->m_button->text().empty())
                {
                    Event down(eventid::keyboard_down);
                    down.key().unicode = multichoice_key->m_unicode;
                    down.key().keycode = multichoice_key->m_keycode;
                    m_in.dispatch(down);
                    Event up(eventid::keyboard_up);
                    up.key().unicode = multichoice_key->m_unicode;
                    up.key().keycode = multichoice_key->m_keycode;
                    m_in.dispatch(up);
                    // the modal popup caught the raw_pointer_up event
                    k->m_button->set_active(false);
                }
                // User may just move his finger so prefer the raw_pointer_up event to the pointer_click one.
            }, {eventid::raw_pointer_up});
        }
    }

    m_multichoice_popup.m_notebook.add(k->m_multichoice);

    k->m_button->on_event([this, k, id](Event&)
    {
        m_multichoice_popup.m_notebook.set_select(id);
        m_multichoice_popup.resize(k->m_multichoice->m_vsizer->size());

        auto display_origin = k->m_button->display_origin();
        auto main_window_origin = main_window()->display_to_local(display_origin);
        // Popup on top of the key.
        main_window_origin.y -= m_multichoice_popup.size().height;
        // Popup aligned with key center.
        main_window_origin.x -= m_multichoice_popup.size().width / 2;
        main_window_origin.x += k->m_button->size().width / 2;

        m_multichoice_popup.move(main_window_origin);
        m_multichoice_popup.show_modal();
    }, {eventid::pointer_hold});
}

VirtualKeyboard::VirtualKeyboard(vector<shared_ptr<Panel>> panels, const Rect& rect)
    : Frame(rect)
{
    m_main_panel.set_align(alignmask::expand);
    add(m_main_panel);
    m_multichoice_popup.resize(Size(100, 100));
    main_window()->add(m_multichoice_popup);

    unsigned multichoice_id = 0;

    for (auto& panel : panels)
    {
        for (auto& row : panel->m_keys)
        {
            for (auto& key : row)
            {
                if (key->link() >= 0)
                {
                    set_key_link(key);
                }
                else
                {
                    set_key_input_value(key);

                    if (key->m_multichoice)
                        set_key_multichoice(key, multichoice_id++);
                }
            }
        }

        m_main_panel.add(panel);
    }
}

VirtualKeyboard::VirtualKeyboard(const Rect& rect)
    : VirtualKeyboard({ make_shared<QwertyLettersLowerCase>(),
    make_shared<QwertyLettersUpperCase>(),
    make_shared<QwertySymbols1>(),
    make_shared<QwertySymbols2>()
}, rect)
{
}

PopupVirtualKeyboard::PopupVirtualKeyboard(shared_ptr<VirtualKeyboard> keyboard)
{
    auto popup_width = main_screen()->size().width;
    auto popup_height = main_screen()->size().height * 0.4;

    resize(Size(popup_width, popup_height));
    move(Point(0, main_screen()->size().height - popup_height));

    m_vsizer.set_align(alignmask::expand);
    add(m_vsizer);

    m_hsizer.set_align(alignmask::top | alignmask::right);
    m_vsizer.add(m_hsizer);

    m_top_bottom_button.set_align(alignmask::top | alignmask::right);
    m_hsizer.add(m_top_bottom_button);

    m_close_button.set_align(alignmask::top | alignmask::right);
    m_close_button.on_event([this](Event&)
    {
        hide();
    }, {eventid::pointer_click});
    m_hsizer.add(m_close_button);

    m_vsizer.add(keyboard);

    the_popup_virtual_keyboard = this;
}

PopupVirtualKeyboard*& popup_virtual_keyboard()
{
    return the_popup_virtual_keyboard;
}

}
}
