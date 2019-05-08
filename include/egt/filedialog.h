/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FILEDIALOG_H
#define EGT_FILEDIALOG_H

/**
 * @file
 * @brief Working with file dialog pop-ups.
 */

#include <egt/button.h>
#include <egt/grid.h>
#include <egt/label.h>
#include <egt/list.h>
#include <egt/popup.h>
#include <egt/sizer.h>
#include <egt/text.h>
#include <egt/utils.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

class FileDialog : public Popup
{
public:
    explicit FileDialog(const Rect& rect);

    FileDialog(const std::string& filepath, const Rect& rect);

    virtual void set_selected(const std::string& fselect) = 0;

    virtual ~FileDialog();

protected:
    std::shared_ptr<BoxSizer> m_vsizer;

    std::shared_ptr<ImageLabel> m_title;

    std::shared_ptr<ListBox> m_flist;

    std::string m_filepath;

    bool list_files(const std::string& filepath);

    void list_item_selected(int index);

private:

    FileDialog() = delete;

};

class FileOpenDialog : public FileDialog
{
public:
    explicit FileOpenDialog(const Rect& rect);

    FileOpenDialog(const std::string& title, const Rect& rect);

    virtual void show(bool center = false) override;

    virtual void set_selected(const std::string& fselect) override;

    virtual const std::string get_selected();

    virtual ~FileOpenDialog() {}

protected:
    std::shared_ptr<StaticGrid> m_grid;

    std::shared_ptr<HorizontalPositioner> m_hpositioner;

    std::shared_ptr<Button> m_okay;

    std::shared_ptr<Button> m_cancel;

    std::string m_fselected;

private:

    FileOpenDialog() = delete;

};

class FileSaveDialog : public FileDialog
{
public:
    explicit FileSaveDialog(const Rect& rect);

    FileSaveDialog(const std::string& title, const Rect& rect);

    virtual void show(bool center = false) override;

    virtual void set_selected(const std::string& fselect) override;

    virtual const std::string get_selected();

    virtual ~FileSaveDialog() {}

protected:
    std::shared_ptr<StaticGrid> m_grid;

    std::shared_ptr<HorizontalPositioner> m_hpositioner;

    std::shared_ptr<Label> m_filename;

    std::shared_ptr<TextBox> m_fileselect_box;

    std::shared_ptr<Button> m_okay;

    std::shared_ptr<Button> m_cancel;

    std::string m_fselected;

private:
    FileSaveDialog() = delete;

};

}

}

#endif
