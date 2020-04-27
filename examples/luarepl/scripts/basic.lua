--
-- Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
--
-- SPDX-License-Identifier: Apache-2.0
--

-- ./luarepl -i ./scripts/basic.lua

win = egt.TopWindow()

l = egt.Label(win, "EGT Lua Bindings Example")
egt.center(l)
egt.top(l)

b = egt.Button("Press Me")
egt.center(b)

function button_pressed(eventid)
   -- print("button event " .. eventid)
   if eventid == egt.EventId_pointer_click then
      colors = {
	 egt.Pattern(egt.Color.rgb(0x0000ff)),
	 egt.Pattern(egt.Color.rgb(0x00ff00)),
	 egt.Pattern(egt.Palette.silver),
	 egt.Pattern(egt.Palette.orange),
	 egt.Pattern(egt.Palette.plum),
	 egt.Pattern(egt.Palette.slateblue),
      }
      i = math.random(1, 6)
      b:color(egt.Palette.ColorId_button_bg, colors[i])
   end
end

b:on_event(button_pressed)
win:add(b)

function timer_timeout()
   -- print("timer timeout")
end

timer = egt.PeriodicTimer()
timer:change_duration_ms(1000)
timer:on_timeout(timer_timeout)
timer:start()

win:show()

-- egt.Application.instance():dump()
