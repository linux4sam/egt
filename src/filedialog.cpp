/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/filedialog.h"
#include <memory>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace egt
{
inline namespace v1
{

FileDialog::FileDialog(const std::string& filepath, const Rect& rect)
    : Popup(rect.size(), rect.point()),
      m_vsizer(std::make_shared<BoxSizer>(orientation::vertical)),
      m_title(std::make_shared<ImageLabel>(Image("@folder.png"), filepath, Rect(Size(rect.w, (rect.h * 0.10))))),
      m_flist(std::make_shared<ListBox>(Rect(rect.x, rect.y, rect.w, (rect.h * 0.80)))),
      m_filepath(filepath)
{
    set_name("FileDialog" + std::to_string(m_widgetid));
    m_vsizer->set_align(alignmask::expand);
    add(m_vsizer);

    m_title->set_text_align(alignmask::left | alignmask::center);
    m_vsizer->add(m_title);

    m_flist->set_align(alignmask::expand_vertical);
    m_vsizer->add(m_flist);

    m_flist->on_event([this](eventid event)
    {
        ignoreparam(event);
        DBG("FileDialog index is  " << this->m_flist->selected());
        list_item_selected(this->m_flist->selected());
        return 1;
    }, {eventid::property_changed});

    if (m_filepath.empty())
        m_filepath = fs::current_path();

    DBG("FileDialog done ");
}

FileDialog::FileDialog(const Rect& rect)
    : FileDialog(std::string(), rect)
{

}

bool FileDialog::list_files(const std::string& filepath)
{
    if (!fs::is_directory(filepath))
    {
        /* if filepath is a regular file then list
         * files of parent directory.
         */
        fs::path p = filepath;
        m_filepath = p.parent_path();
    }
    else
    {
        m_filepath = filepath;
    }

    m_title->set_text(m_filepath);

    DBG("FileDialog :" << " file path is" << m_filepath);

    m_flist->clear();

    if (filepath != "/")
    {
        m_flist->add_item(std::make_shared<StringItem>("./", Rect(), alignmask::left | alignmask::center));

        m_flist->add_item(std::make_shared<StringItem>("../", Rect(), alignmask::left | alignmask::center));
    }

    try
    {
        for (auto& dir : fs::directory_iterator(m_filepath))
        {
            m_flist->add_item(std::make_shared<StringItem>(dir.path().filename(), Rect(), alignmask::left | alignmask::center));
        }
    }
    catch (const fs::filesystem_error& ex)
    {
        DBG("FileDialog :" << "Error: "  << ex.what());
        return false;
    }

    damage();

    return true;
}

void FileDialog::list_item_selected(int index)
{
    auto fselect = dynamic_cast<StringItem*>(m_flist->get_item(index))->text();

    DBG(" FileDialog " << ": File Selected is :" << fselect);

    if (fselect == "./")
    {
        return;
    }
    else if (fselect == "../")
    {
        fs::path p = m_filepath;
        m_filepath = p.parent_path();
        DBG(" FileDialog " <<  ": parent dir " << m_filepath);
    }
    else
    {
        if (m_filepath.back() == '/')
            m_filepath =  m_filepath + fselect;
        else
            m_filepath =  m_filepath + "/" + fselect;
    }

    if (fs::is_directory(m_filepath))
    {
        m_filepath = fs::absolute(m_filepath);
        DBG(" FileDialog " << ": " << fselect  << " is a directory");
        list_files(m_filepath);
    }
    else if (fs::is_regular_file(m_filepath))
    {
        DBG(" FileDialog " << ": " << fselect  << " is a regular file");
        set_selected(fselect);
    }
}

FileDialog::~FileDialog() { }

FileOpenDialog::FileOpenDialog(const std::string& title, const Rect& rect)
    : FileDialog(title, rect),
      m_grid(std::make_shared<StaticGrid>(Rect(Size(rect.w / 2, (rect.h * 0.10))), Tuple(2, 1), 1)),
      m_okay(std::make_shared<Button>("OK")),
      m_cancel(std::make_shared<Button>("Cancel"))
{
    set_name("FileOpenDialog" + std::to_string(m_widgetid));

    m_grid->set_align(alignmask::bottom | alignmask::right);
    m_grid->set_color(Palette::ColorId::border, Palette::transparent);
    m_grid->set_boxtype(Theme::boxtype::blank_rounded);
    m_vsizer->add(m_grid);

    m_okay->set_align(alignmask::center);
    m_grid->add(expand(m_okay));

    m_cancel->set_align(alignmask::center);
    m_grid->add(expand(m_cancel));

    m_okay->on_event([this](eventid event)
    {
        ignoreparam(event);
        list_item_selected(this->m_flist->selected());
        return 1;
    }, {eventid::pointer_click});

    m_cancel->on_event([this](eventid event)
    {
        ignoreparam(event);
        this->m_fselected = std::string();
        this->m_flist->clear();
        this->hide();
        return 1;
    }, {eventid::pointer_click});

}

FileOpenDialog::FileOpenDialog(const Rect& rect)
    : FileOpenDialog(std::string(), rect)
{

}

void FileOpenDialog::show(bool center)
{
    list_files(m_filepath);
    Popup::show(center);
}

void FileOpenDialog::set_selected(const std::string& fselect)
{
    m_fselected = fselect;
    invoke_handlers(eventid::property_changed);
}

const std::string FileOpenDialog::get_selected()
{
    return m_filepath;
}

FileSaveDialog::FileSaveDialog(const std::string& title, const Rect& rect)
    : FileDialog(title, rect),
      m_grid(std::make_shared<StaticGrid>(Rect(Size(rect.w, (rect.h * 0.10))), Tuple(3, 1), 1)),
      m_hpositioner(std::make_shared<HorizontalBoxSizer>()),
      m_fileselect_box(std::make_shared<TextBox>(Rect(0, 0, rect.w * 0.70, (rect.h * 0.10)))),
      m_okay(std::make_shared<Button>("OK")),
      m_cancel(std::make_shared<Button>("Cancel"))
{
    m_grid->set_align(alignmask::bottom);
    m_grid->set_color(Palette::ColorId::border, Palette::transparent);
    m_grid->set_boxtype(Theme::boxtype::blank_rounded);
    m_vsizer->add(m_grid);

    m_hpositioner->set_align(alignmask::bottom | alignmask::left);
    m_hpositioner->set_color(Palette::ColorId::border, Palette::transparent);
    m_hpositioner->set_boxtype(Theme::boxtype::blank_rounded);
    m_grid->add(m_hpositioner);

    m_fileselect_box->set_align(alignmask::left | alignmask:: center);
    m_fileselect_box->set_text("filename:");
    m_hpositioner->add(m_fileselect_box);

    m_okay->set_align(alignmask:: center);
    m_hpositioner->add(m_okay);

    m_cancel->set_align(alignmask:: center);
    m_hpositioner->add(m_cancel);

    m_okay->on_event([this](eventid event)
    {
        ignoreparam(event);
        if (!m_fselected.empty())
            invoke_handlers(eventid::property_changed);
        return 1;
    }, {eventid::pointer_click});

    m_cancel->on_event([this](eventid event)
    {
        ignoreparam(event);
        this->m_fselected = std::string();
        this->m_flist->clear();
        this->m_fileselect_box->set_text(std::string());
        this->hide();
        return 1;
    }, {eventid::pointer_click});

}

FileSaveDialog::FileSaveDialog(const Rect& rect)
    : FileSaveDialog(std::string(), rect)
{

}

void FileSaveDialog::show(bool center)
{
    m_fileselect_box->set_text("filename:");
    list_files(m_filepath);
    Popup::show(center);
}

void FileSaveDialog::set_selected(const std::string& fselect)
{
    m_fselected = fselect;
    m_fileselect_box->set_text(m_fselected);
}

const std::string FileSaveDialog::get_selected()
{
    return m_filepath;
}


}
}
