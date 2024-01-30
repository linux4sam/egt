/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <egt/ui>
#include <iostream>
#include <string>
#include <cstring>
#include <cxxopts.hpp>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <erawimage.h>


using namespace std;
using namespace egt;

class SVG_CVT
{
public:
    SVG_CVT(const string& svgpath, SvgImage& svg) noexcept
        : m_svgpath(svgpath), m_svg(svg)
    {}

    void serializeSVG();
    void saveErawById(const string& filename, const string& id);
    void recursiveGLabel(rapidxml::xml_node<>* g1);
    void convertNotGLabel(rapidxml::xml_node<>* notg);

private:
    const string& m_svgpath;
    SvgImage& m_svg;
};

static void SerializePNG(const char* png_src);
static void InitErawHFile(void);
static void EndErawHFile(void);
static void WriteTableIndexFile(void);
static void ReadTableIndexFile(void);

constexpr uint32_t hash_str_to_uint32(const char* data)
{
    uint32_t h(0);
    for (int i = 0; data && ('\0' != data[i]); i++)
        h = (h << 6) ^ (h >> 26) ^ data[i];
    return h;
}

typedef struct
{
    string name;
    unsigned int offset = 0;
    unsigned int len = 0;
} eraw_st;

#define ERAW_NAME "eraw.bin"
unsigned int offset = 0;
unsigned int table_index = 0;

void SVG_CVT::saveErawById(const string& filename, const string& id)
{
    unsigned int len = 0;
    auto box = m_svg.id_box(id);
    auto layer = make_shared<Image>(m_svg.render(id, box));

    detail::ErawImage e;
    const auto data = cairo_image_surface_get_data(layer->surface().get());
    const auto width = cairo_image_surface_get_width(layer->surface().get());
    const auto height = cairo_image_surface_get_height(layer->surface().get());
    e.save(ERAW_NAME, data, box.x(), box.y(), width, height, &len);

    ofstream erawmap("eraw.h", ios_base::app);
    if (!erawmap.is_open())
    {
        cerr << "eraw.cfg open ERROR!" << endl;
        return;
    }

    erawmap << "    {";
    erawmap << "\"";
    erawmap << filename.c_str();
    erawmap << "\"";
    erawmap << ", ";
    erawmap << offset;
    erawmap << ", ";
    erawmap << len;
    erawmap << "},    //";
    erawmap << table_index++;
    erawmap << "\t\n";
    erawmap.close();
    WriteTableIndexFile();
    offset += len;
}

void SVG_CVT::recursiveGLabel(rapidxml::xml_node<>* g)
{
    rapidxml::xml_attribute<>* id_attr;
    string id;
    string path;
    auto svgWgt = g->first_node();

    while (NULL != svgWgt)
    {
        switch (hash_str_to_uint32(svgWgt->name()))
        {
        case hash_str_to_uint32("g"):
            id_attr = svgWgt->first_attribute("id");
            id = id_attr->value();
            path = id;
            id = "#" + id;
            saveErawById(path, id);
            recursiveGLabel(svgWgt);
            break;

        case hash_str_to_uint32("rect"):
        case hash_str_to_uint32("path"):
        case hash_str_to_uint32("text"):
        case hash_str_to_uint32("image"):
        case hash_str_to_uint32("use"):
        case hash_str_to_uint32("ellipse"):
        case hash_str_to_uint32("circle"):
        case hash_str_to_uint32("line"):
        case hash_str_to_uint32("polyline"):
        case hash_str_to_uint32("polygon"):
            id_attr = svgWgt->first_attribute("id");
            id = id_attr->value();
            path = id;
            id = "#" + id;
            saveErawById(path, id);
            break;

        default:
            break;
        }

        svgWgt = svgWgt->next_sibling();
    }
}

void SVG_CVT::convertNotGLabel(rapidxml::xml_node<>* notg)
{
    rapidxml::xml_attribute<>* id_attr;
    string id;
    string path;

    switch (hash_str_to_uint32(notg->name()))
    {
    case hash_str_to_uint32("rect"):
    case hash_str_to_uint32("path"):
    case hash_str_to_uint32("text"):
    case hash_str_to_uint32("image"):
    case hash_str_to_uint32("use"):
    case hash_str_to_uint32("ellipse"):
    case hash_str_to_uint32("circle"):
    case hash_str_to_uint32("line"):
    case hash_str_to_uint32("polyline"):
    case hash_str_to_uint32("polygon"):
        id_attr = notg->first_attribute("id");
        id = id_attr->value();
        path = id;
        id = "#" + id;
        saveErawById(path, id);
        break;

    default:
        break;
    }
}

void SVG_CVT::serializeSVG()
{
    rapidxml::file<> xml_file(m_svgpath.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(xml_file.data());

    auto svg = doc.first_node("svg");
    if (svg)
    {
        auto g = svg->first_node("g");
        while (NULL != g)
        {
            if (!strcmp("g", g->name()))
                recursiveGLabel(g);
            else
                convertNotGLabel(g);

            g = g->next_sibling();
        }
    }
    else
    {
        cerr << "file format is incorrect" << endl;
        return;
    }
}

static void SerializePNG(const char* png_src)
{
    unsigned int len = 0;

    shared_cairo_surface_t surface;
    detail::ErawImage e;
    surface =
        shared_cairo_surface_t(cairo_image_surface_create_from_png(png_src),
                               cairo_surface_destroy);
    const auto data = cairo_image_surface_get_data(surface.get());
    const auto width = cairo_image_surface_get_width(surface.get());
    const auto height = cairo_image_surface_get_height(surface.get());
    e.save(ERAW_NAME, data, 0, 0, width, height, &len);

    string s(png_src);
    string png = s.substr(0, s.length() - 4);
    ofstream erawmap("eraw.h", ios_base::app);
    if (!erawmap.is_open())
    {
        cerr << "eraw.cfg open ERROR!" << endl;
        return;
    }

    erawmap << "    {";
    erawmap << "\"";
    erawmap << png.c_str();
    erawmap << "\"";
    erawmap << ", ";
    erawmap << offset;
    erawmap << ", ";
    erawmap << len;
    erawmap << "},    //";
    erawmap << table_index++;
    erawmap << "\t\n";
    erawmap.close();
    offset += len;
    WriteTableIndexFile();
}

static void WriteTableIndexFile(void)
{
    ofstream indexfile("index.txt");
    if (!indexfile.is_open())
    {
        cerr << "indexfile open ERROR!" << endl;
        return;
    }

    indexfile << table_index << " " << offset;
    indexfile.close();
}

static void ReadTableIndexFile(void)
{
    ifstream indexfile("index.txt", ios::in);
    if (!indexfile.is_open())
    {
        cerr << "indexfile open ERROR!" << endl;
        return;
    }

    indexfile >> table_index >> offset;
    indexfile.close();
}

static void InitErawHFile(void)
{
    ofstream erawmap("eraw.h", ios_base::app);
    if (!erawmap.is_open())
    {
        cerr << "eraw.cfg open ERROR!" << endl;
        return;
    }

    erawmap << "typedef struct {";
    erawmap << "\t\n";
    erawmap << "    std::string name;";
    erawmap << "\t\n";
    erawmap << "    int offset;";
    erawmap << "\t\n";
    erawmap << "    int len;";
    erawmap << "\t\n";
    erawmap << "} eraw_st;";
    erawmap << "\t\n";
    erawmap << "\t\n";
    erawmap << "eraw_st offset_table[] = {";
    erawmap << "\t\n";
    erawmap.close();
}

static void EndErawHFile(void)
{
    ofstream erawmap("eraw.h", ios_base::app);
    if (!erawmap.is_open())
    {
        cerr << "eraw.cfg open ERROR!" << endl;
        return;
    }

    erawmap << "};";
    erawmap << "\t\n";
    erawmap.close();
}


int main(int argc, char** argv)
{
    cxxopts::Options options("svgconvertor", "SVG format convertor");
    options.add_options()
    ("h,help", "help")
    ("s,starttoken", "start token of eraw.h")
    ("e,endtoken", "end token of eraw.h")
    ("i,input-format", "input format (svg, png)",
     cxxopts::value<string>()->default_value("svg"))
    ("positional", "SOURCE", cxxopts::value<vector<string>>())
    ;
    options.positional_help("SOURCE");

    options.parse_positional({"positional"});
    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        cout << options.help() << endl;
        return 0;
    }

    if (result.count("starttoken"))
    {
        InitErawHFile();
        WriteTableIndexFile();
        cout << "Now you can convert SVG or PNG file!" << endl;
        return 0;
    }

    if (result.count("endtoken"))
    {
        EndErawHFile();
        if (-1 == system("rm index.txt"))
        {
            cerr << "rm index.txt failed, please check permission!!!" << endl;
            return 1;
        }
        cout << "Conversion finished! Copy eraw.h to source, and copy eraw.bin to target!" << endl;
        return 0;
    }

    if (result.count("positional") != 1)
    {
        cerr << options.help() << endl;
        return 1;
    }

    ReadTableIndexFile();

    auto& positional = result["positional"].as<vector<string>>();

    string in = positional[0];
    string filepath;

    filepath = "file:" + in;
    if (result["input-format"].as<string>() == "svg")
    {
        SvgImage svg(filepath, SizeF(0, 0));
        SVG_CVT svg_cvt(in, svg);
        svg_cvt.serializeSVG();
        cout << "SVG converted done!" << endl;
    }

    if (result["input-format"].as<string>() == "png")
    {
        SerializePNG(in.c_str());
        cout << "PNG converted done!" << endl;
    }

    return 0;
}
