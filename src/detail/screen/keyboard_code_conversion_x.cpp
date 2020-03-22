// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "detail/screen/keyboard_code_conversion_x.h"
#include "detail/screen/x11wrap.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <spdlog/spdlog.h>
#define EKEY_UNSUPPORTED EKEY_UNKNOWN
namespace egt
{
inline namespace v1
{
namespace detail
{

namespace base
{
template<class T, size_t N>
constexpr size_t size(T(&)[N]) { return N; }
}

namespace
{
// MAP0 - MAP3:
// These are the generated EKEY code maps from these layout datas on Windows:
//   KBDA1.DLL
//   KBDAL.DLL
//   KBDARME.DLL
//   KBDARMW.DLL
//   KBDAZEL.DLL
//   KBDBE.DLL
//   KBDBENE.DLL
//   KBDBR.DLL
//   KBDCA.DLL
//   KBDCAN.DLL
//   KBDCR.DLL
//   KBDCZ.DLL
//   KBDCZ1.DLL
//   KBDCZ2.DLL
//   KBDDA.DLL
//   KBDDV.DLL
//   KBDES.DLL
//   KBDEST.DLL
//   KBDFC.DLL
//   KBDFI.DLL
//   KBDFI1.DLL
//   KBDFO.DLL
//   KBDFR.DLL
//   KBDHEB.DLL
//   KBDGAE.DLL
//   KBDGEO.DLL
//   kbdgeoer.dll
//   kbdgeoqw.dll
//   KBDGR.DLL
//   KBDGR1.DLL
//   KBDGRLND.DLL
//   KBDHAU.DLL
//   KBDHU.DLL
//   KBDHU1.DLL
//   KBDIBO.DLL
//   KBDIC.DLL
//   KBDIR.DLL
//   KBDIT.DLL
//   KBDIT142.DLL
//   KBDLA.DLL
//   KBDLT.DLL
//   KBDLT1.DLL
//   KBDLT2.DLL
//   KBDLV.DLL
//   KBDLV1.DLL
//   KBDMLT47.DLL
//   KBDMLT48.DLL
//   KBDNE.DLL
//   KBDNO.DLL
//   KBDNO1.DLL
//   KBDPL.DLL
//   KBDPL1.DLL
//   KBDPO.DLL
//   KBDRO.DLL
//   KBDROPR.DLL
//   KBDROST.DLL
//   KBDRU.DLL
//   KBDSF.DLL
//   KBDSG.DLL
//   KBDSL.DLL
//   KBDSL1.DLL
//   KBDSOREX.DLL
//   KBDSORS1.DLL
//   KBDSORST.DLL
//   KBDSP.DLL
//   KBDSW.DLL
//   KBDSW09.DLL
//   KBDUK.DLL
//   KBDUKX.DLL
//   KBDUS.DLL
//   KBDUSL.DLL
//   KBDUSR.DLL
//   KBDUSX.DLL
//   KBDVNTC.DLL
//   KBDYBA.DLL
// And the maps are only for special letter keys excluding [a-z] & [0-9].
//
// ch0: the keysym without modifier states.
// ch1: the keysym with shift state.
// ch2: the keysym with altgr state.
// sc: the hardware keycode (in Windows, it's called scan code).
// vk: the EKEY code.
//
// MAP0: maps from ch0 to vk.
// MAP1: maps from ch0+sc to vk.
// MAP2: maps from ch0+ch1+sc to vk.
// MAP3: maps from ch0+ch1+ch2+sc to vk.
// MAP0 - MAP3 are all sorted, so that finding VK can be binary search.
//
// Reason for creating these maps is because a hard-coded mapping in
// KeyboardCodeFromXKeysym() doesn't support non-US keyboard layouts.
// e.g. in UK keyboard, the key between Quote and Enter keys has the EKEY code
// EKEY_OEM_5 instead of EKEY_3.
//
// The key symbols which are not [a-zA-Z0-9] and functional/extend keys (e.g.
// TAB, ENTER, BS, Arrow keys, modifier keys, F1-F12, media/app keys, etc.)
// should go through these maps for correct EKEY codes.
//
// Please refer to crbug.com/386066.
//
const struct MAP0
{
    KeySym ch0;
    uint8_t vk;
    bool operator()(const MAP0& m1, const MAP0& m2) const
    {
        return m1.ch0 < m2.ch0;
    }
} map0[] =
{
    {0x0025, 0x35},  // XK_percent: EKEY_5
    {0x0026, 0x31},  // XK_ampersand: EKEY_1
    {0x003C, 0xDC},  // XK_less: EKEY_OEM_5
    {0x007B, 0xDE},  // XK_braceleft: EKEY_OEM_7
    {0x007C, 0xDC},  // XK_bar: EKEY_OEM_5
    {0x007D, 0xBF},  // XK_braceright: EKEY_OEM_2
    {0x007E, 0xDC},  // XK_asciitilde: EKEY_OEM_5
    {0x00A1, 0xDD},  // XK_exclamdown: EKEY_OEM_6
    {0x00AD, 0xC0},  // XK_hyphen: EKEY_OEM_3
    {0x00B2, 0xDE},  // XK_twosuperior: EKEY_OEM_7
    {0x00B5, 0xDC},  // XK_mu: EKEY_OEM_5
    {0x00BB, 0x39},  // XK_guillemotright: EKEY_9
    {0x00BD, 0xDC},  // XK_onehalf: EKEY_OEM_5
    {0x00BF, 0xDD},  // XK_questiondown: EKEY_OEM_6
    {0x00DF, 0xDB},  // XK_ssharp: EKEY_OEM_4
    {0x00E5, 0xDD},  // XK_aring: EKEY_OEM_6
    {0x00EA, 0x33},  // XK_ecircumflex: EKEY_3
    {0x00EB, 0xBA},  // XK_ediaeresis: EKEY_OEM_1
    {0x00EC, 0xDD},  // XK_igrave: EKEY_OEM_6
    {0x00EE, 0xDD},  // XK_icircumflex: EKEY_OEM_6
    {0x00F1, 0xC0},  // XK_ntilde: EKEY_OEM_3
    {0x00F2, 0xC0},  // XK_ograve: EKEY_OEM_3
    {0x00F5, 0xDB},  // XK_otilde: EKEY_OEM_4
    {0x00F7, 0xDD},  // XK_division: EKEY_OEM_6
    {0x00FD, 0x37},  // XK_yacute: EKEY_7
    {0x00FE, 0xBD},  // XK_thorn: EKEY_OEM_MINUS
    {0x01A1, 0xDD},  // XK_ohorn: EKEY_OEM_6
    {0x01B0, 0xDB},  // XK_uhorn: EKEY_OEM_4
    {0x01B5, 0x32},  // XK_lcaron: EKEY_2
    {0x01B6, 0xDD},  // XK_zstroke: EKEY_OEM_6
    {0x01BB, 0x35},  // XK_tcaron: EKEY_5
    {0x01E6, 0xDE},  // XK_cacute: EKEY_OEM_7
    {0x01EC, 0x32},  // XK_ecaron: EKEY_2
    {0x01F2, 0xDC},  // XK_ncaron: EKEY_OEM_5
    {0x01F5, 0xDB},  // XK_odoubleacute: EKEY_OEM_4
    {0x01F8, 0x35},  // XK_rcaron: EKEY_5
    {0x01F9, 0xBA},  // XK_uring: EKEY_OEM_1
    {0x01FB, 0xDC},  // XK_udoubleacute: EKEY_OEM_5
    {0x01FE, 0xDE},  // XK_tcedilla: EKEY_OEM_7
    {0x0259, 0xC0},  // XK_schwa: EKEY_OEM_3
    {0x02B1, 0xDD},  // XK_hstroke: EKEY_OEM_6
    {0x02B9, 0xBA},  // XK_idotless: EKEY_OEM_1
    {0x02BB, 0xDD},  // XK_gbreve: EKEY_OEM_6
    {0x02E5, 0xC0},  // XK_cabovedot: EKEY_OEM_3
    {0x02F5, 0xDB},  // XK_gabovedot: EKEY_OEM_4
    {0x03B6, 0xBF},  // XK_lcedilla: EKEY_OEM_2
    {0x03BA, 0x57},  // XK_emacron: EKEY_W
    {0x03E0, 0xDF},  // XK_amacron: EKEY_OEM_8
    {0x03EF, 0xDD},  // XK_imacron: EKEY_OEM_6
    {0x03F1, 0xDB},  // XK_ncedilla: EKEY_OEM_4
    {0x03F3, 0xDC},  // XK_kcedilla: EKEY_OEM_5
};
const struct MAP1
{
    KeySym ch0;
    unsigned sc;
    uint8_t vk;
    bool operator()(const MAP1& m1, const MAP1& m2) const
    {
        if (m1.ch0 == m2.ch0)
            return m1.sc < m2.sc;
        return m1.ch0 < m2.ch0;
    }
} map1[] =
{
    {0x0021, 0x0A, 0x31},  // XK_exclam+AE01: EKEY_1
    {0x0021, 0x11, 0x38},  // XK_exclam+AE08: EKEY_8
    {0x0021, 0x3D, 0xDF},  // XK_exclam+AB10: EKEY_OEM_8
    {0x0022, 0x0B, 0x32},  // XK_quotedbl+AE02: EKEY_2
    {0x0022, 0x0C, 0x33},  // XK_quotedbl+AE03: EKEY_3
    {0x0023, 0x31, 0xDE},  // XK_numbersign+TLDE: EKEY_OEM_7
    {0x0024, 0x23, 0xBA},  // XK_dollar+AD12: EKEY_OEM_1
    {0x0024, 0x33, 0xDF},  // XK_dollar+BKSL: EKEY_OEM_8
    {0x0027, 0x0D, 0x34},  // XK_quoteright+AE04: EKEY_4
    {0x0027, 0x18, 0xDE},  // XK_quoteright+AD01: EKEY_OEM_7
    {0x0027, 0x19, 0x57},  // XK_quoteright+AD02: EKEY_W
    {0x0027, 0x23, 0xBA},  // XK_quoteright+AD12: EKEY_OEM_1
    {0x0027, 0x3D, 0xDE},  // XK_quoteright+AB10: EKEY_OEM_7
    {0x0028, 0x0E, 0x35},  // XK_parenleft+AE05: EKEY_5
    {0x0028, 0x12, 0x39},  // XK_parenleft+AE09: EKEY_9
    {0x0028, 0x33, 0xDC},  // XK_parenleft+BKSL: EKEY_OEM_5
    {0x0029, 0x13, 0x30},  // XK_parenright+AE10: EKEY_0
    {0x0029, 0x14, 0xDB},  // XK_parenright+AE11: EKEY_OEM_4
    {0x0029, 0x23, 0xDD},  // XK_parenright+AD12: EKEY_OEM_6
    {0x002A, 0x23, 0xBA},  // XK_asterisk+AD12: EKEY_OEM_1
    {0x002A, 0x33, 0xDC},  // XK_asterisk+BKSL: EKEY_OEM_5
    {0x002B, 0x0A, 0x31},  // XK_plus+AE01: EKEY_1
    {0x002B, 0x15, 0xBB},  // XK_plus+AE12: EKEY_OEM_PLUS
    {0x002B, 0x22, 0xBB},  // XK_plus+AD11: EKEY_OEM_PLUS
    {0x002B, 0x23, 0xBB},  // XK_plus+AD12: EKEY_OEM_PLUS
    {0x002B, 0x2F, 0xBB},  // XK_plus+AC10: EKEY_OEM_PLUS
    {0x002B, 0x33, 0xBF},  // XK_plus+BKSL: EKEY_OEM_2
    {0x002C, 0x0C, 0x33},  // XK_comma+AE03: EKEY_3
    {0x002C, 0x0E, 0x35},  // XK_comma+AE05: EKEY_5
    {0x002C, 0x0F, 0x36},  // XK_comma+AE06: EKEY_6
    {0x002C, 0x12, 0x39},  // XK_comma+AE09: EKEY_9
    {0x002C, 0x19, 0xBC},  // XK_comma+AD02: EKEY_OEM_COMMA
    {0x002C, 0x30, 0xDE},  // XK_comma+AC11: EKEY_OEM_7
    {0x002C, 0x37, 0xBC},  // XK_comma+AB04: EKEY_OEM_COMMA
    {0x002C, 0x3A, 0xBC},  // XK_comma+AB07: EKEY_OEM_COMMA
    {0x002C, 0x3B, 0xBC},  // XK_comma+AB08: EKEY_OEM_COMMA
    {0x002D, 0x0B, 0x32},  // XK_minus+AE02: EKEY_2
    {0x002D, 0x0F, 0x36},  // XK_minus+AE06: EKEY_6
    {0x002D, 0x14, 0xBD},  // XK_minus+AE11: EKEY_OEM_MINUS
    {0x002D, 0x26, 0xBD},  // XK_minus+AC01: EKEY_OEM_MINUS
    {0x002D, 0x30, 0xBD},  // XK_minus+AC11: EKEY_OEM_MINUS
    {0x002E, 0x10, 0x37},  // XK_period+AE07: EKEY_7
    {0x002E, 0x11, 0x38},  // XK_period+AE08: EKEY_8
    {0x002E, 0x1A, 0xBE},  // XK_period+AD03: EKEY_OEM_PERIOD
    {0x002E, 0x1B, 0xBE},  // XK_period+AD04: EKEY_OEM_PERIOD
    {0x002E, 0x20, 0xBE},  // XK_period+AD09: EKEY_OEM_PERIOD
    {0x002E, 0x30, 0xDE},  // XK_period+AC11: EKEY_OEM_7
    {0x002E, 0x3C, 0xBE},  // XK_period+AB09: EKEY_OEM_PERIOD
    {0x002E, 0x3D, 0xBF},  // XK_period+AB10: EKEY_OEM_2
    {0x002F, 0x14, 0xDB},  // XK_slash+AE11: EKEY_OEM_4
    {0x002F, 0x18, 0x51},  // XK_slash+AD01: EKEY_Q
    {0x002F, 0x22, 0xBF},  // XK_slash+AD11: EKEY_OEM_2
    {0x002F, 0x31, 0xDE},  // XK_slash+TLDE: EKEY_OEM_7
    {0x002F, 0x33, 0xDC},  // XK_slash+BKSL: EKEY_OEM_5
    {0x002F, 0x3D, 0xBF},  // XK_slash+AB10: EKEY_OEM_2
    {0x003A, 0x0A, 0x31},  // XK_colon+AE01: EKEY_1
    {0x003A, 0x0E, 0x35},  // XK_colon+AE05: EKEY_5
    {0x003A, 0x0F, 0x36},  // XK_colon+AE06: EKEY_6
    {0x003A, 0x3C, 0xBF},  // XK_colon+AB09: EKEY_OEM_2
    {0x003B, 0x0D, 0x34},  // XK_semicolon+AE04: EKEY_4
    {0x003B, 0x11, 0x38},  // XK_semicolon+AE08: EKEY_8
    {0x003B, 0x18, 0xBA},  // XK_semicolon+AD01: EKEY_OEM_1
    {0x003B, 0x22, 0xBA},  // XK_semicolon+AD11: EKEY_OEM_1
    {0x003B, 0x23, 0xDD},  // XK_semicolon+AD12: EKEY_OEM_6
    {0x003B, 0x2F, 0xBA},  // XK_semicolon+AC10: EKEY_OEM_1
    {0x003B, 0x31, 0xC0},  // XK_semicolon+TLDE: EKEY_OEM_3
    {0x003B, 0x34, 0xBA},  // XK_semicolon+AB01: EKEY_OEM_1
    {0x003B, 0x3B, 0xBE},  // XK_semicolon+AB08: EKEY_OEM_PERIOD
    {0x003B, 0x3D, 0xBF},  // XK_semicolon+AB10: EKEY_OEM_2
    {0x003D, 0x11, 0x38},  // XK_equal+AE08: EKEY_8
    {0x003D, 0x15, 0xBB},  // XK_equal+AE12: EKEY_OEM_PLUS
    {0x003D, 0x23, 0xBB},  // XK_equal+AD12: EKEY_OEM_PLUS
    {0x003F, 0x0B, 0x32},  // XK_question+AE02: EKEY_2
    {0x003F, 0x10, 0x37},  // XK_question+AE07: EKEY_7
    {0x003F, 0x11, 0x38},  // XK_question+AE08: EKEY_8
    {0x003F, 0x14, 0xBB},  // XK_question+AE11: EKEY_OEM_PLUS
    {0x0040, 0x23, 0xDD},  // XK_at+AD12: EKEY_OEM_6
    {0x0040, 0x31, 0xDE},  // XK_at+TLDE: EKEY_OEM_7
    {0x005B, 0x0A, 0xDB},  // XK_bracketleft+AE01: EKEY_OEM_4
    {0x005B, 0x14, 0xDB},  // XK_bracketleft+AE11: EKEY_OEM_4
    {0x005B, 0x22, 0xDB},  // XK_bracketleft+AD11: EKEY_OEM_4
    {0x005B, 0x23, 0xDD},  // XK_bracketleft+AD12: EKEY_OEM_6
    {0x005B, 0x30, 0xDE},  // XK_bracketleft+AC11: EKEY_OEM_7
    {0x005C, 0x15, 0xDB},  // XK_backslash+AE12: EKEY_OEM_4
    {0x005D, 0x0B, 0xDD},  // XK_bracketright+AE02: EKEY_OEM_6
    {0x005D, 0x15, 0xDD},  // XK_bracketright+AE12: EKEY_OEM_6
    {0x005D, 0x22, 0xDB},  // XK_bracketright+AD11: EKEY_OEM_4
    {0x005D, 0x23, 0xDD},  // XK_bracketright+AD12: EKEY_OEM_6
    {0x005D, 0x31, 0xC0},  // XK_bracketright+TLDE: EKEY_OEM_3
    {0x005D, 0x33, 0xDC},  // XK_bracketright+BKSL: EKEY_OEM_5
    {0x005F, 0x11, 0x38},  // XK_underscore+AE08: EKEY_8
    {0x005F, 0x14, 0xBD},  // XK_underscore+AE11: EKEY_OEM_MINUS
    {0x00A7, 0x0D, 0x34},  // XK_section+AE04: EKEY_4
    {0x00A7, 0x0F, 0x36},  // XK_section+AE06: EKEY_6
    {0x00A7, 0x30, 0xDE},  // XK_section+AC11: EKEY_OEM_7
    {0x00AB, 0x11, 0x38},  // XK_guillemotleft+AE08: EKEY_8
    {0x00AB, 0x15, 0xDD},  // XK_guillemotleft+AE12: EKEY_OEM_6
    {0x00B0, 0x15, 0xBF},  // XK_degree+AE12: EKEY_OEM_2
    {0x00B0, 0x31, 0xDE},  // XK_degree+TLDE: EKEY_OEM_7
    {0x00BA, 0x30, 0xDE},  // XK_masculine+AC11: EKEY_OEM_7
    {0x00BA, 0x31, 0xDC},  // XK_masculine+TLDE: EKEY_OEM_5
    {0x00E0, 0x13, 0x30},  // XK_agrave+AE10: EKEY_0
    {0x00E0, 0x33, 0xDC},  // XK_agrave+BKSL: EKEY_OEM_5
    {0x00E1, 0x11, 0x38},  // XK_aacute+AE08: EKEY_8
    {0x00E1, 0x30, 0xDE},  // XK_aacute+AC11: EKEY_OEM_7
    {0x00E2, 0x0B, 0x32},  // XK_acircumflex+AE02: EKEY_2
    {0x00E2, 0x33, 0xDC},  // XK_acircumflex+BKSL: EKEY_OEM_5
    {0x00E4, 0x23, 0xDD},  // XK_adiaeresis+AD12: EKEY_OEM_6
    {0x00E6, 0x2F, 0xC0},  // XK_ae+AC10: EKEY_OEM_3
    {0x00E6, 0x30, 0xDE},  // XK_ae+AC11: EKEY_OEM_7
    {0x00E7, 0x12, 0x39},  // XK_ccedilla+AE09: EKEY_9
    {0x00E7, 0x22, 0xDB},  // XK_ccedilla+AD11: EKEY_OEM_4
    {0x00E7, 0x23, 0xDD},  // XK_ccedilla+AD12: EKEY_OEM_6
    {0x00E7, 0x30, 0xDE},  // XK_ccedilla+AC11: EKEY_OEM_7
    {0x00E7, 0x33, 0xBF},  // XK_ccedilla+BKSL: EKEY_OEM_2
    {0x00E7, 0x3B, 0xBC},  // XK_ccedilla+AB08: EKEY_OEM_COMMA
    {0x00E8, 0x10, 0x37},  // XK_egrave+AE07: EKEY_7
    {0x00E8, 0x22, 0xBA},  // XK_egrave+AD11: EKEY_OEM_1
    {0x00E8, 0x30, 0xC0},  // XK_egrave+AC11: EKEY_OEM_3
    {0x00E9, 0x0B, 0x32},  // XK_eacute+AE02: EKEY_2
    {0x00E9, 0x13, 0x30},  // XK_eacute+AE10: EKEY_0
    {0x00E9, 0x3D, 0xBF},  // XK_eacute+AB10: EKEY_OEM_2
    {0x00ED, 0x12, 0x39},  // XK_iacute+AE09: EKEY_9
    {0x00ED, 0x31, 0x30},  // XK_iacute+TLDE: EKEY_0
    {0x00F0, 0x22, 0xDD},  // XK_eth+AD11: EKEY_OEM_6
    {0x00F0, 0x23, 0xBA},  // XK_eth+AD12: EKEY_OEM_1
    {0x00F3, 0x15, 0xBB},  // XK_oacute+AE12: EKEY_OEM_PLUS
    {0x00F3, 0x33, 0xDC},  // XK_oacute+BKSL: EKEY_OEM_5
    {0x00F4, 0x0D, 0x34},  // XK_ocircumflex+AE04: EKEY_4
    {0x00F4, 0x2F, 0xBA},  // XK_ocircumflex+AC10: EKEY_OEM_1
    {0x00F6, 0x13, 0xC0},  // XK_odiaeresis+AE10: EKEY_OEM_3
    {0x00F6, 0x14, 0xBB},  // XK_odiaeresis+AE11: EKEY_OEM_PLUS
    {0x00F6, 0x22, 0xDB},  // XK_odiaeresis+AD11: EKEY_OEM_4
    {0x00F8, 0x2F, 0xC0},  // XK_oslash+AC10: EKEY_OEM_3
    {0x00F8, 0x30, 0xDE},  // XK_oslash+AC11: EKEY_OEM_7
    {0x00F9, 0x30, 0xC0},  // XK_ugrave+AC11: EKEY_OEM_3
    {0x00F9, 0x33, 0xBF},  // XK_ugrave+BKSL: EKEY_OEM_2
    {0x00FA, 0x22, 0xDB},  // XK_uacute+AD11: EKEY_OEM_4
    {0x00FA, 0x23, 0xDD},  // XK_uacute+AD12: EKEY_OEM_6
    {0x00FC, 0x19, 0x57},  // XK_udiaeresis+AD02: EKEY_W
    {0x01B1, 0x0A, 0x31},  // XK_aogonek+AE01: EKEY_1
    {0x01B1, 0x18, 0x51},  // XK_aogonek+AD01: EKEY_Q
    {0x01B1, 0x30, 0xDE},  // XK_aogonek+AC11: EKEY_OEM_7
    {0x01B3, 0x2F, 0xBA},  // XK_lstroke+AC10: EKEY_OEM_1
    {0x01B3, 0x33, 0xBF},  // XK_lstroke+BKSL: EKEY_OEM_2
    {0x01B9, 0x0C, 0x33},  // XK_scaron+AE03: EKEY_3
    {0x01B9, 0x0F, 0x36},  // XK_scaron+AE06: EKEY_6
    {0x01B9, 0x22, 0xDB},  // XK_scaron+AD11: EKEY_OEM_4
    {0x01B9, 0x26, 0xBA},  // XK_scaron+AC01: EKEY_OEM_1
    {0x01B9, 0x29, 0x46},  // XK_scaron+AC04: EKEY_F
    {0x01B9, 0x3C, 0xBE},  // XK_scaron+AB09: EKEY_OEM_PERIOD
    {0x01BA, 0x2F, 0xBA},  // XK_scedilla+AC10: EKEY_OEM_1
    {0x01BA, 0x3C, 0xBE},  // XK_scedilla+AB09: EKEY_OEM_PERIOD
    {0x01BE, 0x0F, 0x36},  // XK_zcaron+AE06: EKEY_6
    {0x01BE, 0x15, 0xBB},  // XK_zcaron+AE12: EKEY_OEM_PLUS
    {0x01BE, 0x19, 0x57},  // XK_zcaron+AD02: EKEY_W
    {0x01BE, 0x22, 0x59},  // XK_zcaron+AD11: EKEY_Y
    {0x01BE, 0x33, 0xDC},  // XK_zcaron+BKSL: EKEY_OEM_5
    {0x01BF, 0x22, 0xDB},  // XK_zabovedot+AD11: EKEY_OEM_4
    {0x01BF, 0x33, 0xDC},  // XK_zabovedot+BKSL: EKEY_OEM_5
    {0x01E3, 0x0A, 0x31},  // XK_abreve+AE01: EKEY_1
    {0x01E3, 0x22, 0xDB},  // XK_abreve+AD11: EKEY_OEM_4
    {0x01E8, 0x0B, 0x32},  // XK_ccaron+AE02: EKEY_2
    {0x01E8, 0x0D, 0x34},  // XK_ccaron+AE04: EKEY_4
    {0x01E8, 0x21, 0x58},  // XK_ccaron+AD10: EKEY_X
    {0x01E8, 0x2F, 0xBA},  // XK_ccaron+AC10: EKEY_OEM_1
    {0x01E8, 0x3B, 0xBC},  // XK_ccaron+AB08: EKEY_OEM_COMMA
    {0x01EA, 0x0C, 0x33},  // XK_eogonek+AE03: EKEY_3
    {0x01F0, 0x13, 0x30},  // XK_dstroke+AE10: EKEY_0
    {0x01F0, 0x23, 0xDD},  // XK_dstroke+AD12: EKEY_OEM_6
    {0x03E7, 0x0E, 0x35},  // XK_iogonek+AE05: EKEY_5
    {0x03EC, 0x0D, 0x34},  // XK_eabovedot+AE04: EKEY_4
    {0x03EC, 0x30, 0xDE},  // XK_eabovedot+AC11: EKEY_OEM_7
    {0x03F9, 0x10, 0x37},  // XK_uogonek+AE07: EKEY_7
    {0x03FE, 0x11, 0x38},  // XK_umacron+AE08: EKEY_8
    {0x03FE, 0x18, 0x51},  // XK_umacron+AD01: EKEY_Q
    {0x03FE, 0x35, 0x58},  // XK_umacron+AB02: EKEY_X
};
const struct MAP2
{
    KeySym ch0;
    unsigned sc;
    KeySym ch1;
    uint8_t vk;
    bool operator()(const MAP2& m1, const MAP2& m2) const
    {
        if (m1.ch0 == m2.ch0 && m1.sc == m2.sc)
            return m1.ch1 < m2.ch1;
        if (m1.ch0 == m2.ch0)
            return m1.sc < m2.sc;
        return m1.ch0 < m2.ch0;
    }
} map2[] =
{
    {
        0x0023, 0x33, 0x0027,
        0xBF
    },  // XK_numbersign+BKSL+XK_quoteright: EKEY_OEM_2
    {
        0x0027, 0x30, 0x0022,
        0xDE
    },  // XK_quoteright+AC11+XK_quotedbl: EKEY_OEM_7
    {
        0x0027, 0x31, 0x0022,
        0xC0
    },  // XK_quoteright+TLDE+XK_quotedbl: EKEY_OEM_3
    {
        0x0027, 0x31, 0x00B7,
        0xDC
    },  // XK_quoteright+TLDE+XK_periodcentered: EKEY_OEM_5
    {0x0027, 0x33, 0x0000, 0xDC},  // XK_quoteright+BKSL+NoSymbol: EKEY_OEM_5
    {0x002D, 0x3D, 0x003D, 0xBD},  // XK_minus+AB10+XK_equal: EKEY_OEM_MINUS
    {0x002F, 0x0C, 0x0033, 0x33},  // XK_slash+AE03+XK_3: EKEY_3
    {0x002F, 0x0C, 0x003F, 0xBF},  // XK_slash+AE03+XK_question: EKEY_OEM_2
    {0x002F, 0x13, 0x0030, 0x30},  // XK_slash+AE10+XK_0: EKEY_0
    {0x002F, 0x13, 0x003F, 0xBF},  // XK_slash+AE10+XK_question: EKEY_OEM_2
    {0x003D, 0x3D, 0x0025, 0xDF},  // XK_equal+AB10+XK_percent: EKEY_OEM_8
    {0x003D, 0x3D, 0x002B, 0xBB},  // XK_equal+AB10+XK_plus: EKEY_OEM_PLUS
    {0x005C, 0x33, 0x007C, 0xDC},  // XK_backslash+BKSL+XK_bar: EKEY_OEM_5
    {0x0060, 0x31, 0x0000, 0xC0},  // XK_quoteleft+TLDE+NoSymbol: EKEY_OEM_3
    {0x0060, 0x31, 0x00AC, 0xDF},  // XK_quoteleft+TLDE+XK_notsign: EKEY_OEM_8
    {0x00A7, 0x31, 0x00B0, 0xBF},  // XK_section+TLDE+XK_degree: EKEY_OEM_2
    {0x00A7, 0x31, 0x00BD, 0xDC},  // XK_section+TLDE+XK_onehalf: EKEY_OEM_5
    {0x00E0, 0x30, 0x00B0, 0xDE},  // XK_agrave+AC11+XK_degree: EKEY_OEM_7
    {0x00E0, 0x30, 0x00E4, 0xDC},  // XK_agrave+AC11+XK_adiaeresis: EKEY_OEM_5
    {0x00E4, 0x30, 0x00E0, 0xDC},  // XK_adiaeresis+AC11+XK_agrave: EKEY_OEM_5
    {0x00E9, 0x2F, 0x00C9, 0xBA},  // XK_eacute+AC10+XK_Eacute: EKEY_OEM_1
    {0x00E9, 0x2F, 0x00F6, 0xDE},  // XK_eacute+AC10+XK_odiaeresis: EKEY_OEM_7
    {0x00F6, 0x2F, 0x00E9, 0xDE},  // XK_odiaeresis+AC10+XK_eacute: EKEY_OEM_7
    {0x00FC, 0x22, 0x00E8, 0xBA},  // XK_udiaeresis+AD11+XK_egrave: EKEY_OEM_1
};
const struct MAP3
{
    KeySym ch0;
    unsigned sc;
    KeySym ch1;
    KeySym ch2;
    uint8_t vk;
    bool operator()(const MAP3& m1, const MAP3& m2) const
    {
        if (m1.ch0 == m2.ch0 && m1.sc == m2.sc && m1.ch1 == m2.ch1)
            return m1.ch2 < m2.ch2;
        if (m1.ch0 == m2.ch0 && m1.sc == m2.sc)
            return m1.ch1 < m2.ch1;
        if (m1.ch0 == m2.ch0)
            return m1.sc < m2.sc;
        return m1.ch0 < m2.ch0;
    }
} map3[] =
{
    {
        0x0023, 0x33, 0x007E, 0x0000,
        0xDE
    },  // XK_numbersign+BKSL+XK_asciitilde+NoSymbol: EKEY_OEM_7
    {
        0x0027, 0x14, 0x003F, 0x0000,
        0xDB
    },  // XK_quoteright+AE11+XK_question+NoSymbol: EKEY_OEM_4
    {
        0x0027, 0x14, 0x003F, 0x00DD,
        0xDB
    },  // XK_quoteright+AE11+XK_question+XK_Yacute: EKEY_OEM_4
    {
        0x0027, 0x15, 0x002A, 0x0000,
        0xBB
    },  // XK_quoteright+AE12+XK_asterisk+NoSymbol: EKEY_OEM_PLUS
    {
        0x0027, 0x30, 0x0040, 0x0000,
        0xC0
    },  // XK_quoteright+AC11+XK_at+NoSymbol: EKEY_OEM_3
    {
        0x0027, 0x33, 0x002A, 0x0000,
        0xBF
    },  // XK_quoteright+BKSL+XK_asterisk+NoSymbol: EKEY_OEM_2
    {
        0x0027, 0x33, 0x002A, 0x00BD,
        0xDC
    },  // XK_quoteright+BKSL+XK_asterisk+XK_onehalf: EKEY_OEM_5
    {
        0x0027, 0x33, 0x002A, 0x01A3,
        0xBF
    },  // XK_quoteright+BKSL+XK_asterisk+XK_Lstroke: EKEY_OEM_2
    {
        0x0027, 0x34, 0x0022, 0x0000,
        0x5A
    },  // XK_quoteright+AB01+XK_quotedbl+NoSymbol: EKEY_Z
    {
        0x0027, 0x34, 0x0022, 0x01D8,
        0xDE
    },  // XK_quoteright+AB01+XK_quotedbl+XK_Rcaron: EKEY_OEM_7
    {
        0x002B, 0x14, 0x003F, 0x0000,
        0xBB
    },  // XK_plus+AE11+XK_question+NoSymbol: EKEY_OEM_PLUS
    {
        0x002B, 0x14, 0x003F, 0x005C,
        0xBD
    },  // XK_plus+AE11+XK_question+XK_backslash: EKEY_OEM_MINUS
    {
        0x002B, 0x14, 0x003F, 0x01F5,
        0xBB
    },  // XK_plus+AE11+XK_question+XK_odoubleacute: EKEY_OEM_PLUS
    {
        0x002D, 0x15, 0x005F, 0x0000,
        0xBD
    },  // XK_minus+AE12+XK_underscore+NoSymbol: EKEY_OEM_MINUS
    {
        0x002D, 0x15, 0x005F, 0x03B3,
        0xDB
    },  // XK_minus+AE12+XK_underscore+XK_rcedilla: EKEY_OEM_4
    {
        0x002D, 0x3D, 0x005F, 0x0000,
        0xBD
    },  // XK_minus+AB10+XK_underscore+NoSymbol: EKEY_OEM_MINUS
    {
        0x002D, 0x3D, 0x005F, 0x002A,
        0xBD
    },  // XK_minus+AB10+XK_underscore+XK_asterisk: EKEY_OEM_MINUS
    {
        0x002D, 0x3D, 0x005F, 0x002F,
        0xBF
    },  // XK_minus+AB10+XK_underscore+XK_slash: EKEY_OEM_2
    {
        0x002D, 0x3D, 0x005F, 0x006E,
        0xBD
    },  // XK_minus+AB10+XK_underscore+XK_n: EKEY_OEM_MINUS
    {
        0x003D, 0x14, 0x0025, 0x0000,
        0xBB
    },  // XK_equal+AE11+XK_percent+NoSymbol: EKEY_OEM_PLUS
    {
        0x003D, 0x14, 0x0025, 0x002D,
        0xBD
    },  // XK_equal+AE11+XK_percent+XK_minus: EKEY_OEM_MINUS
    {
        0x005C, 0x31, 0x007C, 0x0031,
        0xDC
    },  // XK_backslash+TLDE+XK_bar+XK_1: EKEY_OEM_5
    {
        0x005C, 0x31, 0x007C, 0x03D1,
        0xC0
    },  // XK_backslash+TLDE+XK_bar+XK_Ncedilla: EKEY_OEM_3
    {
        0x005C, 0x33, 0x002F, 0x0000,
        0xDC
    },  // XK_backslash+BKSL+XK_slash+NoSymbol: EKEY_OEM_5
    {
        0x005C, 0x33, 0x002F, 0x01A3,
        0xDE
    },  // XK_backslash+BKSL+XK_slash+XK_Lstroke: EKEY_OEM_7
    {
        0x0060, 0x31, 0x007E, 0x0000,
        0xC0
    },  // XK_quoteleft+TLDE+XK_asciitilde+NoSymbol: EKEY_OEM_3
    {
        0x0060, 0x31, 0x007E, 0x0031,
        0xC0
    },  // XK_quoteleft+TLDE+XK_asciitilde+XK_1: EKEY_OEM_3
    {
        0x0060, 0x31, 0x007E, 0x003B,
        0xC0
    },  // XK_quoteleft+TLDE+XK_asciitilde+XK_semicolon: EKEY_OEM_3
    {
        0x0060, 0x31, 0x007E, 0x0060,
        0xC0
    },  // XK_quoteleft+TLDE+XK_asciitilde+XK_quoteleft: EKEY_OEM_3
    {
        0x0060, 0x31, 0x007E, 0x00BF,
        0xC0
    },  // XK_quoteleft+TLDE+XK_asciitilde+XK_questiondown: EKEY_OEM_3
    {
        0x0060, 0x31, 0x007E, 0x01F5,
        0xC0
    },  // XK_quoteleft+TLDE+XK_asciitilde+XK_odoubleacute: EKEY_OEM_3
    {
        0x00E4, 0x30, 0x00C4, 0x0000,
        0xDE
    },  // XK_adiaeresis+AC11+XK_Adiaeresis+NoSymbol: EKEY_OEM_7
    {
        0x00E4, 0x30, 0x00C4, 0x01A6,
        0xDE
    },  // XK_adiaeresis+AC11+XK_Adiaeresis+XK_Sacute: EKEY_OEM_7
    {
        0x00E4, 0x30, 0x00C4, 0x01F8,
        0xDE
    },  // XK_adiaeresis+AC11+XK_Adiaeresis+XK_rcaron: EKEY_OEM_7
    {
        0x00E7, 0x2F, 0x00C7, 0x0000,
        0xBA
    },  // XK_ccedilla+AC10+XK_Ccedilla+NoSymbol: EKEY_OEM_1
    {
        0x00E7, 0x2F, 0x00C7, 0x00DE,
        0xC0
    },  // XK_ccedilla+AC10+XK_Ccedilla+XK_Thorn: EKEY_OEM_3
    {
        0x00F6, 0x2F, 0x00D6, 0x0000,
        0xC0
    },  // XK_odiaeresis+AC10+XK_Odiaeresis+NoSymbol: EKEY_OEM_3
    {
        0x00F6, 0x2F, 0x00D6, 0x01DE,
        0xC0
    },  // XK_odiaeresis+AC10+XK_Odiaeresis+XK_Tcedilla: EKEY_OEM_3
    {
        0x00FC, 0x14, 0x00DC, 0x0000,
        0xBF
    },  // XK_udiaeresis+AE11+XK_Udiaeresis+NoSymbol: EKEY_OEM_2
    {
        0x00FC, 0x22, 0x00DC, 0x0000,
        0xBA
    },  // XK_udiaeresis+AD11+XK_Udiaeresis+NoSymbol: EKEY_OEM_1
    {
        0x00FC, 0x22, 0x00DC, 0x01A3,
        0xC0
    },  // XK_udiaeresis+AD11+XK_Udiaeresis+XK_Lstroke: EKEY_OEM_3
    {
        0x01EA, 0x3D, 0x01CA, 0x0000,
        0xBD
    },  // XK_eogonek+AB10+XK_Eogonek+NoSymbol: EKEY_OEM_MINUS
    {
        0x01EA, 0x3D, 0x01CA, 0x006E,
        0xBF
    },  // XK_eogonek+AB10+XK_Eogonek+XK_n: EKEY_OEM_2
    {
        0x03E7, 0x22, 0x03C7, 0x0000,
        0xDB
    },  // XK_iogonek+AD11+XK_Iogonek+NoSymbol: EKEY_OEM_4
    {
        0x03F9, 0x2F, 0x03D9, 0x0000,
        0xC0
    },  // XK_uogonek+AC10+XK_Uogonek+NoSymbol: EKEY_OEM_3
    {
        0x03F9, 0x2F, 0x03D9, 0x01DE,
        0xBA
    },  // XK_uogonek+AC10+XK_Uogonek+XK_Tcedilla: EKEY_OEM_1
};
template <class T_MAP>
KeyboardCode FindVK(const T_MAP& key, const T_MAP* map, size_t size)
{
    T_MAP comp = {0};
    const T_MAP* p = std::lower_bound(map, map + size, key, comp);
    if (p != map + size && !comp(*p, key) && !comp(key, *p))
        return static_cast<KeyboardCode>(p->vk);
    return EKEY_UNKNOWN;
}
// Check for TTY function keys or space key which should always be mapped
// based on KeySym, and never fall back to MAP0~MAP3, since some layouts
// generate them by applying the Control/AltGr modifier to some other key.
// e.g. in de(neo), AltGr+V generates XK_Enter.
bool IsTtyFunctionOrSpaceKey(KeySym keysym)
{
    KeySym keysyms[] =
    {
        XK_BackSpace,
        XK_Tab,
        XK_Linefeed,
        XK_Clear,
        XK_Return,
        XK_Pause,
        XK_Scroll_Lock,
        XK_Sys_Req,
        XK_Escape,
        XK_Delete,
        XK_space
    };
    for (size_t i = 0; i < base::size(keysyms); ++i)
    {
        if (keysyms[i] == keysym)
            return true;
    }
    return false;
}
}  // namespace
// Get an ui::KeyboardCode from an X keyevent
KeyboardCode KeyboardCodeFromXKeyEvent(const XEvent* xev)
{
    // Gets correct EKEY code from XEvent is performed as the following steps:
    // 1. Gets the keysym without modifier states.
    // 2. For [a-z] & [0-9] cases, returns the EKEY code accordingly.
    // 3. Find keysym in map0.
    // 4. If not found, fallback to find keysym + hardware_code in map1.
    // 5. If not found, fallback to find keysym + keysym_shift + hardware_code
    //    in map2.
    // 6. If not found, fallback to find keysym + keysym_shift + keysym_altgr +
    //    hardware_code in map3.
    // 7. If not found, fallback to find in KeyboardCodeFromXKeysym(), which
    //    mainly for non-letter keys.
    // 8. If not found, fallback to find with the hardware code in US layout.
    KeySym keysym = NoSymbol;
    XEvent xkeyevent = {0};
    if (xev->type == GenericEvent)
    {
        // Convert the XI2 key event into a core key event so that we can
        // continue to use XLookupString() until crbug.com/367732 is complete.
        //InitXKeyEventFromXIDeviceEvent(*xev, &xkeyevent);
        assert(0);
    }
    else
    {
        xkeyevent.xkey = xev->xkey;
    }
    KeyboardCode keycode = EKEY_UNKNOWN;
    XKeyEvent* xkey = &xkeyevent.xkey;
    // XLookupKeysym does not take into consideration the state of the lock/shift
    // etc. keys. So it is necessary to use XLookupString instead.
    XLookupString(xkey, NULL, 0, &keysym, NULL);
    if (IsKeypadKey(keysym) || IsPrivateKeypadKey(keysym) ||
        IsCursorKey(keysym) || IsPFKey(keysym) || IsFunctionKey(keysym) ||
        IsModifierKey(keysym) || IsTtyFunctionOrSpaceKey(keysym))
    {
        return KeyboardCodeFromXKeysym(keysym);
    }
    // If |xkey| has modifiers set, other than NumLock, then determine the
    // un-modified KeySym and use that to map, so that e.g. Ctrl+D correctly
    // generates EKEY_D.
    if (xkey->state & 0xFF & ~Mod2Mask)
    {
        xkey->state &= (~0xFF | Mod2Mask);
        XLookupString(xkey, NULL, 0, &keysym, NULL);
    }
    // [a-z] cases.
    if (keysym >= XK_a && keysym <= XK_z)
        return static_cast<KeyboardCode>(EKEY_A + keysym - XK_a);
    // [0-9] cases.
    if (keysym >= XK_0 && keysym <= XK_9)
        return static_cast<KeyboardCode>(EKEY_0 + keysym - XK_0);
    if (!IsKeypadKey(keysym) && !IsPrivateKeypadKey(keysym) &&
        !IsCursorKey(keysym) && !IsPFKey(keysym) && !IsFunctionKey(keysym) &&
        !IsModifierKey(keysym))
    {
        MAP0 key0 = {keysym & 0xFFFF, 0};
        keycode = FindVK(key0, map0, base::size(map0));
        if (keycode != EKEY_UNKNOWN)
            return keycode;
        MAP1 key1 = {keysym & 0xFFFF, xkey->keycode, 0};
        keycode = FindVK(key1, map1, base::size(map1));
        if (keycode != EKEY_UNKNOWN)
            return keycode;
        KeySym keysym_shift = NoSymbol;
        xkey->state |= ShiftMask;
        XLookupString(xkey, NULL, 0, &keysym_shift, NULL);
        MAP2 key2 = {keysym & 0xFFFF, xkey->keycode, keysym_shift & 0xFFFF, 0};
        keycode = FindVK(key2, map2, base::size(map2));
        if (keycode != EKEY_UNKNOWN)
            return keycode;
        KeySym keysym_altgr = NoSymbol;
        xkey->state &= ~ShiftMask;
        xkey->state |= Mod1Mask;
        XLookupString(xkey, NULL, 0, &keysym_altgr, NULL);
        MAP3 key3 = {keysym & 0xFFFF, xkey->keycode, keysym_shift & 0xFFFF,
                     keysym_altgr & 0xFFFF, 0
                    };
        keycode = FindVK(key3, map3, base::size(map3));
        if (keycode != EKEY_UNKNOWN)
            return keycode;
        // On Linux some keys has AltGr char but not on Windows.
        // So if cannot find EKEY with (ch0+sc+ch1+ch2) in map3, tries to fallback
        // to just find EKEY with (ch0+sc+ch1). This is the best we could do.
        MAP3 key4 = {keysym & 0xFFFF, xkey->keycode, keysym_shift & 0xFFFF, 0,
                     0
                    };
        const MAP3* p =
            std::lower_bound(map3, map3 + base::size(map3), key4, MAP3());
        if (p != map3 + base::size(map3) && p->ch0 == key4.ch0 &&
            p->sc == key4.sc && p->ch1 == key4.ch1)
            return static_cast<KeyboardCode>(p->vk);
    }
    keycode = KeyboardCodeFromXKeysym(keysym);
    if (keycode == EKEY_UNKNOWN && !IsModifierKey(keysym))
    {
        // Modifier keys should not fall back to the hardware-keycode-based US
        // layout.  See crbug.com/402320
        keycode = DefaultKeyboardCodeFromHardwareKeycode(xkey->keycode);
    }
    return keycode;
}
KeyboardCode KeyboardCodeFromXKeysym(unsigned int keysym)
{
    // TODO(sad): Have |keysym| go through the X map list?
    switch (keysym)
    {
    case XK_BackSpace:
        return EKEY_BACKSPACE;
    case XK_Delete:
    case XK_KP_Delete:
        return EKEY_DELETE;
    case XK_Tab:
    case XK_KP_Tab:
    case XK_ISO_Left_Tab:
    case XK_3270_BackTab:
        return EKEY_TAB;
    case XK_Linefeed:
    case XK_Return:
    case XK_KP_Enter:
    case XK_ISO_Enter:
        return EKEY_ENTER;
    case XK_Clear:
    case XK_KP_Begin:  // NumPad 5 without Num Lock, for crosbug.com/29169.
        return EKEY_CLEAR;
    case XK_KP_Space:
    case XK_space:
        return EKEY_SPACE;
    case XK_Home:
    case XK_KP_Home:
        return EKEY_HOME;
    case XK_End:
    case XK_KP_End:
        return EKEY_END;
    case XK_Page_Up:
    case XK_KP_Page_Up:  // aka XK_KP_Prior
        return EKEY_PAGEUP;
    case XK_Page_Down:
    case XK_KP_Page_Down:  // aka XK_KP_Next
        return EKEY_PAGEDOWN;
    case XK_Left:
    case XK_KP_Left:
        return EKEY_LEFT;
    case XK_Right:
    case XK_KP_Right:
        return EKEY_RIGHT;
    case XK_Down:
    case XK_KP_Down:
        return EKEY_DOWN;
    case XK_Up:
    case XK_KP_Up:
        return EKEY_UP;
    case XK_Escape:
        return EKEY_ESCAPE;
#if 0
    case XK_Kana_Lock:
    case XK_Kana_Shift:
        return EKEY_KANA;
    case XK_Hangul:
        return EKEY_HANGUL;
    case XK_Hangul_Hanja:
        return EKEY_HANJA;
    case XK_Kanji:
        return EKEY_KANJI;
    case XK_Henkan:
        return EKEY_CONVERT;
    case XK_Muhenkan:
        return EKEY_NONCONVERT;
    case XK_Zenkaku_Hankaku:
        return EKEY_DBE_DBCSCHAR;
#endif
    case XK_KP_0:
    case XK_KP_1:
    case XK_KP_2:
    case XK_KP_3:
    case XK_KP_4:
    case XK_KP_5:
    case XK_KP_6:
    case XK_KP_7:
    case XK_KP_8:
    case XK_KP_9:
        return static_cast<KeyboardCode>(EKEY_NUMPAD0 + (keysym - XK_KP_0));
    case XK_multiply:
    case XK_KP_Multiply:
        return EKEY_MULTIPLY;
    case XK_KP_Add:
        return EKEY_ADD;
    case XK_KP_Separator:
        return EKEY_MULTIPLY;
    case XK_KP_Subtract:
        return EKEY_SUBTRACT;
    case XK_KP_Decimal:
        return EKEY_DECIMAL;
    case XK_KP_Divide:
        return EKEY_DIVIDE;
    case XK_KP_Equal:
    case XK_equal:
    case XK_plus:
        return EKEY_EQUAL;
    case XK_comma:
    case XK_less:
        return EKEY_COMMA;
    case XK_minus:
    case XK_underscore:
        return EKEY_MINUS;
    case XK_greater:
    case XK_period:
        return EKEY_DOT;
    case XK_colon:
    case XK_semicolon:
        return EKEY_SEMICOLON;
    case XK_question:
    case XK_slash:
        return EKEY_SLASH;
    case XK_asciitilde:
    case XK_quoteleft:
        return EKEY_GRAVE;
    case XK_bracketleft:
    case XK_braceleft:
        return EKEY_LEFTBRACE;
    case XK_backslash:
    case XK_bar:
        return EKEY_BACKSLASH;
    case XK_bracketright:
    case XK_braceright:
        return EKEY_RIGHTBRACE;
    case XK_quoteright:
    case XK_quotedbl:
        return EKEY_APOSTROPHE;
#if 0
    case XK_ISO_Level5_Shift:
        return EKEY_OEM_8;
#endif
    case XK_Shift_L:
        return EKEY_LSHIFT;
    case XK_Shift_R:
        return EKEY_RSHIFT;
    case XK_Control_L:
        return EKEY_LCONTROL;
    case XK_Control_R:
        return EKEY_RCONTROL;
    case XK_Meta_L:
    case XK_Alt_L:
        return EKEY_LEFTALT;
    case XK_Meta_R:
    case XK_Alt_R:
        return EKEY_RIGHTALT;
#if 0
    case XK_ISO_Level3_Shift:
    case XK_Mode_switch:
        return EKEY_ALTGR;
#endif
    case XK_Multi_key:
        return EKEY_COMPOSE;
    case XK_Pause:
        return EKEY_PAUSE;
    case XK_Caps_Lock:
        return EKEY_CAPSLOCK;
    case XK_Num_Lock:
        return EKEY_NUMLOCK;
    case XK_Scroll_Lock:
        return EKEY_SCROLL;
    case XK_Select:
        return EKEY_SELECT;
    case XK_Print:
        return EKEY_SNAPSHOT;
#if 0
    case XK_Execute:
        return EKEY_EXECUTE;
#endif
    case XK_Insert:
    case XK_KP_Insert:
        return EKEY_INSERT;
    case XK_Help:
        return EKEY_HELP;
#if 0
    case XK_Super_L:
        return EKEY_LWIN;
    case XK_Super_R:
        return EKEY_RWIN;
    case XK_Menu:
        return EKEY_APPS;
#endif
    case XK_F1:
    case XK_F2:
    case XK_F3:
    case XK_F4:
    case XK_F5:
    case XK_F6:
    case XK_F7:
    case XK_F8:
    case XK_F9:
    case XK_F10:
    case XK_F11:
    case XK_F12:
    case XK_F13:
    case XK_F14:
    case XK_F15:
    case XK_F16:
    case XK_F17:
    case XK_F18:
    case XK_F19:
    case XK_F20:
    case XK_F21:
    case XK_F22:
    case XK_F23:
    case XK_F24:
        return static_cast<KeyboardCode>(EKEY_F1 + (keysym - XK_F1));
    case XK_KP_F1:
    case XK_KP_F2:
    case XK_KP_F3:
    case XK_KP_F4:
        return static_cast<KeyboardCode>(EKEY_F1 + (keysym - XK_KP_F1));
#if 0
    case XK_guillemotleft:
    case XK_guillemotright:
    case XK_degree:
    // In the case of canadian multilingual keyboard layout, EKEY_OEM_102 is
    // assigned to ugrave key.
    case XK_ugrave:
    case XK_Ugrave:
    case XK_brokenbar:
        return EKEY_OEM_102;  // international backslash key in 102 keyboard.
#endif
    // When evdev is in use, /usr/share/X11/xkb/symbols/inet maps F13-18 keys
    // to the special XF86XK symbols to support Microsoft Ergonomic keyboards:
    // https://bugs.freedesktop.org/show_bug.cgi?id=5783
    // In Chrome, we map these X key symbols back to F13-18 since we don't have
    // EKEYs for these XF86XK symbols.
    case XF86XK_Tools:
        return EKEY_F13;
    case XF86XK_Launch5:
        return EKEY_F14;
    case XF86XK_Launch6:
        return EKEY_F15;
    case XF86XK_Launch7:
        return EKEY_F16;
    case XF86XK_Launch8:
        return EKEY_F17;
    case XF86XK_Launch9:
        return EKEY_F18;
#if 0
    // For supporting multimedia buttons on a USB keyboard.
    case XF86XK_Back:
        return EKEY_BROWSER_BACK;
    case XF86XK_Forward:
        return EKEY_BROWSER_FORWARD;
    case XF86XK_Reload:
        return EKEY_BROWSER_REFRESH;
    case XF86XK_Stop:
        return EKEY_BROWSER_STOP;
    case XF86XK_Search:
        return EKEY_BROWSER_SEARCH;
    case XF86XK_Favorites:
        return EKEY_BROWSER_FAVORITES;
    case XF86XK_HomePage:
        return EKEY_BROWSER_HOME;
    case XF86XK_AudioMute:
        return EKEY_VOLUME_MUTE;
    case XF86XK_AudioLowerVolume:
        return EKEY_VOLUME_DOWN;
    case XF86XK_AudioRaiseVolume:
        return EKEY_VOLUME_UP;
    case XF86XK_AudioNext:
        return EKEY_MEDIA_NEXT_TRACK;
    case XF86XK_AudioPrev:
        return EKEY_MEDIA_PREV_TRACK;
    case XF86XK_AudioStop:
        return EKEY_MEDIA_STOP;
    case XF86XK_AudioPlay:
        return EKEY_MEDIA_PLAY_PAUSE;
    case XF86XK_Mail:
        return EKEY_MEDIA_LAUNCH_MAIL;
    case XF86XK_LaunchA:  // F3 on an Apple keyboard.
        return EKEY_MEDIA_LAUNCH_APP1;
    case XF86XK_LaunchB:  // F4 on an Apple keyboard.
    case XF86XK_Calculator:
        return EKEY_MEDIA_LAUNCH_APP2;
#endif
    case XF86XK_WLAN:
        return EKEY_WLAN;
    case XF86XK_PowerOff:
        return EKEY_POWER;
    case XF86XK_Sleep:
        return EKEY_SLEEP;
#if 0
    case XF86XK_MonBrightnessDown:
        return EKEY_BRIGHTNESS_DOWN;
    case XF86XK_MonBrightnessUp:
        return EKEY_BRIGHTNESS_UP;
    case XF86XK_KbdBrightnessDown:
        return EKEY_KBD_BRIGHTNESS_DOWN;
    case XF86XK_KbdBrightnessUp:
        return EKEY_KBD_BRIGHTNESS_UP;
        // TODO(sad): some keycodes are still missing.
#endif
    }
    SPDLOG_DEBUG("Unknown keysym: {}", keysym);
    return EKEY_UNKNOWN;
}

KeyboardCode DefaultKeyboardCodeFromHardwareKeycode(
    unsigned int hardware_code)
{
    // This function assumes that X11 is using evdev-based keycodes.
    static const KeyboardCode kHardwareKeycodeMap[] =
    {
        // Please refer to below links for the table content:
        // http://www.w3.org/TR/DOM-Level-3-Events-code/#keyboard-101
        // https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent.keyCode
        // http://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/translate.pdf
        EKEY_UNKNOWN,       // 0x00:
        EKEY_UNKNOWN,       // 0x01:
        EKEY_UNKNOWN,       // 0x02:
        EKEY_UNKNOWN,       // 0x03:
        EKEY_UNKNOWN,       // 0x04:
        EKEY_UNKNOWN,       // 0x05:
        EKEY_UNKNOWN,       // 0x06:
        EKEY_UNKNOWN,       // XKB   evdev (XKB - 8)      X KeySym
        EKEY_UNKNOWN,       // ===   ===============      ======
        EKEY_ESCAPE,        // 0x09: KEY_ESC              Escape
        EKEY_1,             // 0x0A: KEY_1                1
        EKEY_2,             // 0x0B: KEY_2                2
        EKEY_3,             // 0x0C: KEY_3                3
        EKEY_4,             // 0x0D: KEY_4                4
        EKEY_5,             // 0x0E: KEY_5                5
        EKEY_6,             // 0x0F: KEY_6                6
        EKEY_7,             // 0x10: KEY_7                7
        EKEY_8,             // 0x11: KEY_8                8
        EKEY_9,             // 0x12: KEY_9                9
        EKEY_0,             // 0x13: KEY_0                0
        EKEY_MINUS,         // 0x14: KEY_MINUS            minus
        EKEY_PLUS,          // 0x15: KEY_EQUAL            equal
        EKEY_BACKSPACE,     // 0x16: KEY_BACKSPACE        BackSpace
        EKEY_TAB,           // 0x17: KEY_TAB              Tab
        EKEY_Q,             // 0x18: KEY_Q                q
        EKEY_W,             // 0x19: KEY_W                w
        EKEY_E,             // 0x1A: KEY_E                e
        EKEY_R,             // 0x1B: KEY_R                r
        EKEY_T,             // 0x1C: KEY_T                t
        EKEY_Y,             // 0x1D: KEY_Y                y
        EKEY_U,             // 0x1E: KEY_U                u
        EKEY_I,             // 0x1F: KEY_I                i
        EKEY_O,             // 0x20: KEY_O                o
        EKEY_P,             // 0x21: KEY_P                p
        EKEY_LEFTBRACE,         // 0x22: KEY_LEFTBRACE        bracketleft
        EKEY_RIGHTBRACE,         // 0x23: KEY_RIGHTBRACE       bracketright
        EKEY_ENTER,        // 0x24: KEY_ENTER            Return
        EKEY_LCONTROL,      // 0x25: KEY_LEFTCTRL         Control_L
        EKEY_A,             // 0x26: KEY_A                a
        EKEY_S,             // 0x27: KEY_S                s
        EKEY_D,             // 0x28: KEY_D                d
        EKEY_F,             // 0x29: KEY_F                f
        EKEY_G,             // 0x2A: KEY_G                g
        EKEY_H,             // 0x2B: KEY_H                h
        EKEY_J,             // 0x2C: KEY_J                j
        EKEY_K,             // 0x2D: KEY_K                k
        EKEY_L,             // 0x2E: KEY_L                l
        EKEY_SEMICOLON,     // 0x2F: KEY_SEMICOLON        semicolon
        EKEY_APOSTROPHE,    // 0x30: KEY_APOSTROPHE       apostrophe
        EKEY_GRAVE,         // 0x31: KEY_GRAVE            grave
        EKEY_LSHIFT,        // 0x32: KEY_LEFTSHIFT        Shift_L
        EKEY_BACKSLASH,     // 0x33: KEY_BACKSLASH        backslash
        EKEY_Z,             // 0x34: KEY_Z                z
        EKEY_X,             // 0x35: KEY_X                x
        EKEY_C,             // 0x36: KEY_C                c
        EKEY_V,             // 0x37: KEY_V                v
        EKEY_B,             // 0x38: KEY_B                b
        EKEY_N,             // 0x39: KEY_N                n
        EKEY_M,             // 0x3A: KEY_M                m
        EKEY_COMMA,         // 0x3B: KEY_COMMA            comma
        EKEY_DOT,           // 0x3C: KEY_DOT              period
        EKEY_SLASH,         // 0x3D: KEY_SLASH            slash
        EKEY_RSHIFT,        // 0x3E: KEY_RIGHTSHIFT       Shift_R
        EKEY_MULTIPLY,      // 0x3F: KEY_KPASTERISK       KP_Multiply
        EKEY_LEFTALT,         // 0x40: KEY_LEFTALT          Alt_L
        EKEY_SPACE,         // 0x41: KEY_SPACE            space
        EKEY_CAPSLOCK,      // 0x42: KEY_CAPSLOCK         Caps_Lock
        EKEY_F1,            // 0x43: KEY_F1               F1
        EKEY_F2,            // 0x44: KEY_F2               F2
        EKEY_F3,            // 0x45: KEY_F3               F3
        EKEY_F4,            // 0x46: KEY_F4               F4
        EKEY_F5,            // 0x47: KEY_F5               F5
        EKEY_F6,            // 0x48: KEY_F6               F6
        EKEY_F7,            // 0x49: KEY_F7               F7
        EKEY_F8,            // 0x4A: KEY_F8               F8
        EKEY_F9,            // 0x4B: KEY_F9               F9
        EKEY_F10,           // 0x4C: KEY_F10              F10
        EKEY_NUMLOCK,       // 0x4D: KEY_NUMLOCK          Num_Lock
        EKEY_SCROLL,        // 0x4E: KEY_SCROLLLOCK       Scroll_Lock
        EKEY_NUMPAD7,       // 0x4F: KEY_KP7              KP_7
        EKEY_NUMPAD8,       // 0x50: KEY_KP8              KP_8
        EKEY_NUMPAD9,       // 0x51: KEY_KP9              KP_9
        EKEY_SUBTRACT,      // 0x52: KEY_KPMINUS          KP_Subtract
        EKEY_NUMPAD4,       // 0x53: KEY_KP4              KP_4
        EKEY_NUMPAD5,       // 0x54: KEY_KP5              KP_5
        EKEY_NUMPAD6,       // 0x55: KEY_KP6              KP_6
        EKEY_ADD,           // 0x56: KEY_KPPLUS           KP_Add
        EKEY_NUMPAD1,       // 0x57: KEY_KP1              KP_1
        EKEY_NUMPAD2,       // 0x58: KEY_KP2              KP_2
        EKEY_NUMPAD3,       // 0x59: KEY_KP3              KP_3
        EKEY_NUMPAD0,       // 0x5A: KEY_KP0              KP_0
        EKEY_DECIMAL,       // 0x5B: KEY_KPDOT            KP_Decimal
        EKEY_UNKNOWN,       // 0x5C:
        EKEY_UNSUPPORTED,   // 0x5D: KEY_ZENKAKUHANKAKU   Zenkaku_Hankaku
        EKEY_UNSUPPORTED,   // 0x5E: KEY_102ND            backslash
        EKEY_F11,           // 0x5F: KEY_F11              F11
        EKEY_F12,           // 0x60: KEY_F12              F12
        EKEY_UNSUPPORTED,   // 0x61: KEY_RO               Romaji
        EKEY_UNSUPPORTED,   // 0x62: KEY_KATAKANA         Katakana
        EKEY_UNSUPPORTED,   // 0x63: KEY_HIRAGANA         Hiragana
        EKEY_UNSUPPORTED,   // 0x64: KEY_HENKAN           Henkan
        EKEY_UNSUPPORTED,   // 0x65: KEY_KATAKANAHIRAGANA Hiragana_Katakana
        EKEY_UNSUPPORTED,   // 0x66: KEY_MUHENKAN         Muhenkan
        EKEY_COMMA,         // 0x67: KEY_KPJPCOMMA        KP_Separator
        EKEY_ENTER,         // 0x68: KEY_KPENTER          KP_Enter
        EKEY_RCONTROL,      // 0x69: KEY_RIGHTCTRL        Control_R
        EKEY_DIVIDE,        // 0x6A: KEY_KPSLASH          KP_Divide
        EKEY_SNAPSHOT,      // 0x6B: KEY_SYSRQ            Print
        EKEY_RIGHTALT,      // 0x6C: KEY_RIGHTALT         Alt_R
        EKEY_ENTER,         // 0x6D: KEY_LINEFEED         Linefeed
        EKEY_HOME,          // 0x6E: KEY_HOME             Home
        EKEY_UP,            // 0x6F: KEY_UP               Up
        EKEY_PAGEUP,        // 0x70: KEY_PAGEUP           Page_Up
        EKEY_LEFT,          // 0x71: KEY_LEFT             Left
        EKEY_RIGHT,         // 0x72: KEY_RIGHT            Right
        EKEY_END,           // 0x73: KEY_END              End
        EKEY_DOWN,          // 0x74: KEY_DOWN             Down
        EKEY_NEXT,          // 0x75: KEY_PAGEDOWN         Page_Down
        EKEY_INSERT,        // 0x76: KEY_INSERT           Insert
        EKEY_DELETE,        // 0x77: KEY_DELETE           Delete
        EKEY_UNSUPPORTED,   // 0x78: KEY_MACRO
        EKEY_VOLUME_MUTE,   // 0x79: KEY_MUTE             XF86AudioMute
        EKEY_VOLUME_DOWN,   // 0x7A: KEY_VOLUMEDOWN       XF86AudioLowerVolume
        EKEY_VOLUME_UP,     // 0x7B: KEY_VOLUMEUP         XF86AudioRaiseVolume
        EKEY_POWER,         // 0x7C: KEY_POWER            XF86PowerOff
        EKEY_PLUS,          // 0x7D: KEY_KPEQUAL          KP_Equal
        EKEY_UNSUPPORTED,   // 0x7E: KEY_KPPLUSMINUS      plusminus
        EKEY_PAUSE,         // 0x7F: KEY_PAUSE            Pause
        EKEY_UNSUPPORTED,   // 0x80: KEY_SCALE            XF86LaunchA
        EKEY_DECIMAL,       // 0x81: KEY_KPCOMMA          KP_Decimal
        EKEY_UNSUPPORTED,   // 0x82: KEY_HANGUEL          Hangul
        EKEY_UNSUPPORTED,   // 0x83: KEY_HANJA            Hangul_Hanja
        EKEY_UNSUPPORTED,   // 0x84: KEY_YEN              yen
        EKEY_UNSUPPORTED,   // 0x85: KEY_LEFTMETA         Super_L
        EKEY_UNSUPPORTED,   // 0x86: KEY_RIGHTMETA        Super_R
        EKEY_COMPOSE,       // 0x87: KEY_COMPOSE          Menu
    };
    if (hardware_code >= base::size(kHardwareKeycodeMap))
    {
        return EKEY_UNKNOWN;
    }
    return kHardwareKeycodeMap[hardware_code];
}

}
}
}
