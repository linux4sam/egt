/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "script.h"
#include <stdlib.h>
#include <string.h>

static lua_State* state = NULL;

int script_init(lua_State* newstate)
{
    if (state)
        return 1;

    if (newstate)
        state = newstate;
    else
        state = luaL_newstate();
    if (state)
        luaL_openlibs(state);
    return !!state;
}

int script_load(const char* expr, char** pmsg)
{
    int err;
    char* buf;

    if (!state)
    {
        if (pmsg)
            *pmsg = strdup("LE library not initialized");
        return LUA_NOREF;
    }
    buf = malloc(strlen(expr) + 8);
    if (!buf)
    {
        if (pmsg)
            *pmsg = strdup("Insufficient memory");
        return LUA_NOREF;
    }
    strcpy(buf, "return ");
    strcat(buf, expr);
    err = luaL_loadstring(state, buf);
    free(buf);
    if (err)
    {
        if (pmsg)
            *pmsg = strdup(lua_tostring(state, -1));
        lua_pop(state, 1);
        return LUA_NOREF;
    }
    if (pmsg)
        *pmsg = NULL;
    return luaL_ref(state, LUA_REGISTRYINDEX);
}

double script_eval(int cookie, char** pmsg)
{
    int err;
    double ret;

    lua_rawgeti(state, LUA_REGISTRYINDEX, cookie);
    err = lua_pcall(state, 0, 1, 0);
    if (err)
    {
        if (pmsg)
            *pmsg = strdup(lua_tostring(state, -1));
        lua_pop(state, 1);
        return 0;
    }
    if (pmsg)
        *pmsg = NULL;
    if (lua_isboolean(state, -1))
        ret = lua_toboolean(state, -1);
    else
        ret = lua_tonumber(state, -1);
    lua_pop(state, 1);
    return ret;
}

void script_unref(int cookie)
{
    luaL_unref(state, LUA_REGISTRYINDEX, cookie);
}

void script_setvar(const char* name, double value)
{
    lua_pushnumber(state, value);
    lua_setglobal(state, name);
}

void script_setfunc(const char* name, SCRIPT_CALLBACK callback)
{
    lua_register(state, name, callback);
}

double script_getvar(const char* name)
{
    double ret;

    lua_getglobal(state, name);
    ret = lua_tonumber(state, -1);
    lua_pop(state, 1);
    return ret;
}
