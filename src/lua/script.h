/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_LUA_SCRIPT_H
#define MUI_LUA_SCRIPT_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int script_init(lua_State* newstate);
int script_load(const char *expr, char **pmsg);
double script_eval(int cookie, char **pmsg);
void script_unref(int cookie);
void script_setvar(const char *name, double value);
double script_getvar(const char *name);

typedef int (*SCRIPT_CALLBACK)(lua_State*);
void script_setfunc(const char *name, SCRIPT_CALLBACK callback);

#ifdef __cplusplus
}
#endif

#endif
