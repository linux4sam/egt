/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "resource.h"
#include <map>
#include <string.h>
#include <string>
#include <cassert>

using namespace std;

namespace mui
{
    struct resource
    {
	const unsigned char* data;
	unsigned int len;
	unsigned int index;
    };

    /*
     * Because of static initialization order, make this a pointer and allocate
     * on demand instead of initializing statically.
     */
    static std::map<std::string,struct resource>* resources = 0;

    cairo_status_t read_resource_stream(void* closure, unsigned char* data, unsigned int length)
    {
	if (resources)
	{
	    string id = reinterpret_cast<const char*>(closure);
	    auto i = resources->find(id);
	    if (i != resources->end())
	    {
		memcpy(data,i->second.data + i->second.index, length);
		i->second.index += length;
		return CAIRO_STATUS_SUCCESS;
	    }
	}

	return CAIRO_STATUS_READ_ERROR;
    }

    unsigned int read_resource_length(const char* name)
    {
	if (resources)
	{
	    string id = name;
	    auto i = resources->find(id);
	    if (i != resources->end())
	    {
		return i->second.len;
	    }
	}

	return 0;
    }

    bool read_resource(const char* name, unsigned char* data, unsigned int length, unsigned int offset)
    {
	if (resources)
	{
	    string id = name;
	    auto i = resources->find(id);
	    if (i != resources->end())
	    {
		// TODO: no bounds checking
		memcpy(data, i->second.data + offset, length);
		return true;
	    }
	}

	return false;
    }

    void register_resource(const char* name, const unsigned char* data, unsigned int len)
    {
	if (!resources)
	    resources = new std::map<std::string,struct resource>();
	assert(resources);
	struct resource r = {data, len, 0};
	resources->insert(std::make_pair(name, r));
    }

    void unregister_resource(const char* name)
    {
	if (resources)
	{
	    string id = name;
	    auto i = resources->find(id);
	    if (i != resources->end())
		resources->erase(i);
	}
    }
}
