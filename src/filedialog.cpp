/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/filedialog.h"
#include <experimental/filesystem>
#include <memory>
#include <spdlog/spdlog.h>

namespace fs = std::experimental::filesystem;

namespace egt
{
inline namespace v1
{

FileDialog::FileDialog(const std::string& filepath, const Rect& rect)
    : Popup(rect.size(), rect.point()),
      m_vsizer(std::make_shared<BoxSizer>(orientation::vertical)),
      m_title(std::make_shared<ImageLabel>(Image("@folder.png"), filepath)),
      m_flist(std::make_shared<ListBox>()),
      m_filepath(filepath)
{
    set_name("FileDialog" + std::to_string(m_widgetid));

    set_border(theme().default_border());
    set_padding(5);

    add(expand(m_vsizer));

    m_title->set_text_align(alignmask::left | alignmask::center);
    m_vsizer->add(expand_horizontal(m_title));

    m_vsizer->add(expand(m_flist));

    m_flist->on_event([this](Event&)
    {
        SPDLOG_DEBUG("FileDialog index is {}", this->m_flist->selected());
        list_item_selected(this->m_flist->selected());
    }, {eventid::property_changed});

    if (m_filepath.empty())
        m_filepath = fs::current_path().string();

    SPDLOG_DEBUG("FileDialog done");
}

FileDialog::FileDialog(const Rect& rect)
    : FileDialog({}, rect)
{

}

bool FileDialog::list_files(const std::string& filepath)
{
    if (fs::is_directory(filepath))
    {
        m_filepath = filepath;
    }
    else
    {
        /* if filepath is a regular file then list
         * files of parent directory.
         */
        fs::path p = filepath;
        m_filepath = p.parent_path().string();
    }

    m_title->set_text(m_filepath);

    SPDLOG_DEBUG("FileDialog : file path is {}", m_filepath);

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
            m_flist->add_item(std::make_shared<StringItem>(dir.path().filename().string(), Rect(), alignmask::left | alignmask::center));
        }
    }
    catch (const fs::filesystem_error& ex)
    {
        SPDLOG_DEBUG("FileDialog : Error: {}", ex.what());
        return false;
    }

    damage();

    return true;
}

void FileDialog::list_item_selected(int index)
{
    auto fselect = dynamic_cast<StringItem*>(m_flist->item_at(index))->text();

    SPDLOG_DEBUG("FileDialog : File Selected is : {}", fselect);

    if (fselect == "./")
    {
        return;
    }
    else if (fselect == "../")
    {
        fs::path p = m_filepath;
        m_filepath = p.parent_path().string();
        SPDLOG_DEBUG("FileDialog : parent dir {}", m_filepath);
        set_selected("");
        list_files(m_filepath);
    }
    else if (fs::is_directory(m_filepath + "/" + fselect))
    {
        SPDLOG_DEBUG("FileDialog : {} is a directory", fselect);
        set_selected("");
        m_filepath =  m_filepath + "/" + fselect;
        list_files(m_filepath);
    }
    else if (fs::is_regular_file(m_filepath + "/" + fselect))
    {
        SPDLOG_DEBUG("FileDialog : {} is a regular file", fselect);
        set_selected(fselect);
    }
}

FileOpenDialog::FileOpenDialog(const std::string& filepath, const Rect& rect)
    : FileDialog(filepath, rect),
      m_grid(std::make_shared<StaticGrid>(Size(0, (rect.height() * 0.15)), Tuple(2, 1), 5)),
      m_okay(std::make_shared<Button>("OK")),
      m_cancel(std::make_shared<Button>("Cancel"))
{
    set_name("FileOpenDialog" + std::to_string(m_widgetid));

    m_vsizer->add(expand_horizontal(m_grid));
    m_grid->add(expand(m_okay));
    m_grid->add(expand(m_cancel));

    m_okay->on_event([this](Event&)
    {
        list_item_selected(this->m_flist->selected());
    }, {eventid::pointer_click});

    m_cancel->on_event([this](Event&)
    {
        this->m_fselected = std::string();
        this->m_flist->clear();
        this->hide();
    }, {eventid::pointer_click});
}

FileOpenDialog::FileOpenDialog(const Rect& rect)
    : FileOpenDialog({}, rect)
{}

void FileOpenDialog::show()
{
    list_files(m_filepath);
    Popup::show();
}

void FileOpenDialog::show_centered()
{
    list_files(m_filepath);
    Popup::show_centered();
}

void FileOpenDialog::set_selected(const std::string& fselect)
{
    m_fselected = fselect;
    if (!m_fselected.empty())
        invoke_handlers(eventid::property_changed);
}

std::string FileOpenDialog::selected() const
{
    return (m_filepath + "/" + m_fselected);
}

FileSaveDialog::FileSaveDialog(const std::string& filepath, const Rect& rect)
    : FileDialog(filepath, rect),
      m_fsave_box(std::make_shared<TextBox>("", Size(0, rect.height() * 0.15))),
      m_grid(std::make_shared<StaticGrid>(Size(rect.width() * 0.30, rect.height() * 0.15), Tuple(2, 1), 5)),
      m_okay(std::make_shared<Button>("OK")),
      m_cancel(std::make_shared<Button>("Cancel"))
{
    auto hpositioner = std::make_shared<HorizontalBoxSizer>();
    m_vsizer->add(expand_horizontal(hpositioner));

    m_fsave_box->set_margin(5);
    hpositioner->add(expand_horizontal(m_fsave_box));
    hpositioner->add(m_grid);

    m_grid->add(expand(m_okay));
    m_grid->add(expand(m_cancel));

    m_okay->on_event([this](Event & event)
    {
        if (!m_fsave.empty())
        {
            invoke_handlers(eventid::property_changed);
        }
        else if (!m_fsave_box->text().empty())
        {
            m_fsave = m_fsave_box->text();
            invoke_handlers(eventid::property_changed);
        }
    }, {eventid::pointer_click});

    m_cancel->on_event([this](Event & event)
    {
        this->m_fsave = std::string();
        this->m_flist->clear();
        this->m_fsave_box->set_text(std::string());
        this->hide();
    }, {eventid::pointer_click});
}

FileSaveDialog::FileSaveDialog(const Rect& rect)
    : FileSaveDialog(std::string(), rect)
{

}

void FileSaveDialog::show()
{
    m_fsave_box->set_text("");
    list_files(m_filepath);
    Popup::show();
}

void FileSaveDialog::show_centered()
{
    m_fsave_box->set_text("");
    list_files(m_filepath);
    Popup::show_centered();
}

void FileSaveDialog::set_selected(const std::string& fselect)
{
    m_fsave = fselect;
    m_fsave_box->set_text(m_fsave);
}

std::string FileSaveDialog::selected() const
{
    return (m_filepath + "/" + m_fsave);
}


}
}
