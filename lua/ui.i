/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
%include "lua_fnptr.i"
%include "typemaps.i"
%include "stl.i"
%include "stdint.i"

%module egt
%{
#include "egt/ui"
using namespace egt;

class LuaFnPtr
{
public:
    LuaFnPtr(SWIGLUA_REF fn) : fn(fn) {};
    void operator()()
    {
        swiglua_ref_get(&fn);
        lua_call(this->fn.L, 0, 0);
    };
private:
    SWIGLUA_REF fn;
};

class LuaEventFnPtr
{
public:
    LuaEventFnPtr(SWIGLUA_REF fn) : fn(fn) {};
    void operator()(Event& event)
    {
        swiglua_ref_get(&fn);
        lua_pushinteger(this->fn.L, (int)event.id());
        lua_pcall(this->fn.L, 1, 0, 0);
    };
private:
    SWIGLUA_REF fn;
};


%}

// %import for type info, but don't generate bindings
%import "egt/types.h"
%import "egt/utils.h"

%ignore egt::v1::detail::Object::on_event(event_callback_t);
%ignore egt::v1::detail::Object::on_event(event_callback_t,filter_type);
%ignore egt::v1::Timer::on_timeout(timer_callback_t);

%include "egt/detail/object.h"
%include "egt/widgetflags.h"
%include "egt/geometry.h"
%include "egt/easing.h"
%include "egt/animation.h"
%include "egt/event.h"
%include "egt/app.h"
%include "egt/canvas.h"
%include "egt/font.h"
%include "egt/keycode.h"
%include "egt/timer.h"
%include "egt/input.h"
%include "egt/color.h"
%include "egt/palette.h"
%include "egt/image.h"

%include "egt/widget.h"
%include "egt/detail/textwidget.h"
%include "egt/valuewidget.h"

%include "egt/button.h"
%include "egt/buttongroup.h"
%include "egt/chart.h"
%include "egt/checkbox.h"

%include "egt/label.h"
%include "egt/progressbar.h"
%include "egt/radial.h"
%include "egt/radiobox.h"

%include "egt/shapes.h"
%include "egt/slider.h"
%include "egt/text.h"

%include "egt/frame.h"
%include "egt/window.h"
%include "egt/popup.h"
%include "egt/view.h"
%include "egt/sizer.h"
%include "egt/virtualkeyboard.h"
%include "egt/list.h"
%include "egt/notebook.h"
%include "egt/dialog.h"
%include "egt/filedialog.h"
%include "egt/form.h"
%include "egt/grid.h"
%include "egt/gauge.h"
%include "egt/combo.h"
%include "egt/sideboard.h"
%include "egt/sprite.h"
%include "egt/scrollwheel.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

%extend Object
{
    virtual void on_event(SWIGLUA_REF callback)
    {
        LuaEventFnPtr lua_fn_ptr(callback);
        $self->on_event(lua_fn_ptr);
    }
}

}

%template(find_child) Frame::find_child<Widget>;

%extend Timer
{
    void change_duration_ms(int ms)
    {
        $self->change_duration(std::chrono::milliseconds(ms));
    }

    void on_timeout(SWIGLUA_REF callback)
    {
        LuaFnPtr lua_fn_ptr(callback);
        $self->on_timeout(lua_fn_ptr);
    }
}

%extend PeriodicTimer
{
    void change_duration_ms(int ms)
    {
        $self->change_duration(std::chrono::milliseconds(ms));
    }

    void on_timeout(SWIGLUA_REF callback)
    {
        LuaFnPtr lua_fn_ptr(callback);
        $self->on_timeout(lua_fn_ptr);
    }
}

}
}
