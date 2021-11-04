/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/embed.h"
#include "egt/filedialog.h"
#include <experimental/filesystem>

#ifdef SRCDIR
EGT_EMBED(internal_folder, SRCDIR "/icons/32px/folder.png")
#endif

namespace fs = std::experimental::filesystem;

namespace egt
{
inline namespace v1
{

FileDialog::FileDialog(const std::string& filepath, const Rect& rect) noexcept
    : Dialog(rect),
      m_flist(std::make_shared<egt::ListBox>()),
      m_filepath(filepath)
{
    name("FileDialog" + std::to_string(m_widgetid));
    initialize();
}

FileDialog::FileDialog(const Rect& rect) noexcept
    : FileDialog( {}, rect)
{}

void FileDialog::initialize()
{
    if (m_filepath.empty())
        m_filepath = fs::current_path().string();

    title(Image("res:internal_folder"), m_filepath),

          widget(expand(m_flist));

    m_flist->on_selected([this](size_t index)
    {
        list_item_selected(index);
    });
}

FileDialog::FileDialog(Serializer::Properties& props, bool is_derived) noexcept
    : Dialog(props, true),
      m_flist(std::make_shared<egt::ListBox>())
{
    name("FileDialog" + std::to_string(m_widgetid));

    initialize();

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
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

    m_title.text(m_filepath);

    EGTLOG_DEBUG("FileDialog : file path is {}", m_filepath);

    m_flist->clear();

    if (filepath != "/")
    {
        m_flist->add_item(std::make_shared<StringItem>("./", Rect(), AlignFlag::left | AlignFlag::center));

        m_flist->add_item(std::make_shared<StringItem>("../", Rect(), AlignFlag::left | AlignFlag::center));
    }

    try
    {
        for (auto& dir : fs::directory_iterator(m_filepath))
        {
            m_flist->add_item(std::make_shared<StringItem>(dir.path().filename().string(), Rect(), AlignFlag::left | AlignFlag::center));
        }
    }
    catch (const fs::filesystem_error& ex)
    {
        EGTLOG_DEBUG("FileDialog : Error: {}", ex.what());
        return false;
    }

    damage();

    return true;
}

void FileDialog::list_item_selected(int index)
{
    auto item = dynamic_cast<StringItem*>(m_flist->item_at(index).get());
    if (!item)
        return;

    auto fselect = item->text();

    EGTLOG_DEBUG("FileDialog : File Selected is : {}", fselect);

    if (fselect == "./")
    {
        return;
    }
    else if (fselect == "../")
    {
        fs::path p = m_filepath;
        m_filepath = p.parent_path().string();
        EGTLOG_DEBUG("FileDialog : parent dir {}", m_filepath);
        selected("");
        list_files(m_filepath);
    }
    else if (fs::is_directory(m_filepath + "/" + fselect))
    {
        EGTLOG_DEBUG("FileDialog : {} is a directory", fselect);
        selected("");
        if (m_filepath == "/")
            m_filepath += fselect;
        else
            m_filepath =  m_filepath + "/" + fselect;
        list_files(m_filepath);
    }
    else if (fs::is_regular_file(m_filepath + "/" + fselect))
    {
        EGTLOG_DEBUG("FileDialog : {} is a regular file", fselect);
        selected(fselect);
    }
}

void FileDialog::show()
{
    list_files(m_filepath);
    Popup::show();
}

void FileDialog::show_centered()
{
    list_files(m_filepath);
    Popup::show_centered();
}

void FileDialog::serialize(Serializer& serializer) const
{
    serializer.add_property("filepath", m_filepath);
    Popup::serialize(serializer);
}

void FileDialog::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "filepath")
        {
            list_files(std::get<1>(p));
            return true;
        }
        return false;
    }), props.end());
}

FileOpenDialog::FileOpenDialog(const std::string& filepath, const Rect& rect) noexcept
    : FileDialog(filepath, rect)
{
    name("FileOpenDialog" + std::to_string(m_widgetid));
    initialize();
}

FileOpenDialog::FileOpenDialog(Serializer::Properties& props, bool is_derived) noexcept
    : FileDialog(props, true)
{
    name("FileOpenDialog" + std::to_string(m_widgetid));
    initialize();

    if (!is_derived)
        deserialize_leaf(props);
}

FileOpenDialog::FileOpenDialog(const Rect& rect) noexcept
    : FileOpenDialog( {}, rect)
{}

void FileOpenDialog::initialize()
{
    on_button1_click([this]()
    {
        this->list_item_selected(this->m_flist->selected());
    });

    on_button2_click([this]()
    {
        this->m_fselected = std::string();
        this->m_flist->clear();
        this->hide();
    });
}

void FileOpenDialog::selected(const std::string& fselect)
{
    m_fselected = fselect;
    if (!m_fselected.empty())
        on_selected.invoke();
}

std::string FileOpenDialog::selected() const
{
    return (m_filepath + "/" + m_fselected);
}

FileSaveDialog::FileSaveDialog(const std::string& filepath, const Rect& rect) noexcept
    : FileDialog(filepath, rect),
      m_fsave_box("", Size(rect.width() * 0.50, rect.height() * 0.15))
{
    name("FileSaveDialog" + std::to_string(m_widgetid));
    initialize();
}

FileSaveDialog::FileSaveDialog(Serializer::Properties& props, bool is_derived) noexcept
    : FileDialog(props, true),
      m_fsave_box("", Size(box().width() * 0.50, box().height() * 0.15))
{
    name("FileSaveDialog" + std::to_string(m_widgetid));
    initialize();

    if (!is_derived)
        deserialize_leaf(props);
}

FileSaveDialog::FileSaveDialog(const Rect& rect) noexcept
    : FileSaveDialog(std::string(), rect)
{}

void FileSaveDialog::initialize()
{
    m_grid->remove_all();
    m_layout.remove(m_grid.get());

    m_grid.reset(new StaticGrid(Size(box().width(), (box().height() * 0.15)), StaticGrid::GridSize(3, 1)));
    m_grid->margin(5);
    m_grid->horizontal_space(5);
    m_grid->vertical_space(5);
    m_layout.add(expand_horizontal(m_grid));

    m_fsave_box.margin(5);
    m_grid->add(expand(m_fsave_box));

    m_grid->add(expand(m_button1));
    m_grid->add(expand(m_button2));

    on_button1_click([this]()
    {
        if (!m_fsave.empty())
        {
            on_selected.invoke();
        }
        else if (!m_fsave_box.text().empty())
        {
            m_fsave = m_fsave_box.text();
            on_selected.invoke();
        }
    });

    on_button2_click([this]()
    {
        this->m_fsave = std::string();
        this->m_flist->clear();
        this->m_fsave_box.text(std::string());
        this->hide();
    });

}

void FileSaveDialog::show()
{
    m_fsave_box.text("");
    FileDialog::show();
}

void FileSaveDialog::show_centered()
{
    m_fsave_box.text("");
    FileDialog::show_centered();
}

void FileSaveDialog::selected(const std::string& fselect)
{
    m_fsave = fselect;
    m_fsave_box.text(m_fsave);
}

std::string FileSaveDialog::selected() const
{
    return (m_filepath + "/" + m_fsave);
}

}
}
