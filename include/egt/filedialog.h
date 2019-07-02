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

/**
 * A FileDialog is a widget that allows user to:
 *
 * 1. Choose a file from the file system.
 * 2. View the contents of file system directories.
 * 3. Select a location for saving a file.
 *
 * Filedialog widget is using a std::experimental::filesystem library
 * Api's
 *
 */
class FileDialog : public Popup
{
public:
    /**
     * Create a file dialog window.
     *
     * @param[in] rect is a size of file dialog window.
     *
     * Note: list the content of current directory.
     */
    explicit FileDialog(const Rect& rect = {});

    /**
     * Create a file dialog window.
     *
     * @param[in] rect is a size of file dialog window.
     * @param[in] filepath is to list the contents filepath directory.
     *
     * Note: filepath should be a directory if the file path is regular
     * file, then the filedialog will list the content of parent directory.
     */
    explicit FileDialog(const std::string& filepath = {}, const Rect& rect = {});

    virtual void set_selected(const std::string& fselect) = 0;

    virtual ~FileDialog() = default;

protected:
    std::shared_ptr<BoxSizer> m_vsizer;
    std::shared_ptr<ImageLabel> m_title;
    std::shared_ptr<ListBox> m_flist;
    std::string m_filepath;

    /**
     * List the contents of filepath directory.
     */
    bool list_files(const std::string& filepath);

    /**
     * Get the currently selected index.
     */
    void list_item_selected(int index);

};

class FileOpenDialog : public FileDialog
{
public:
    /**
     * Create a file open dialog window.
     *
     * @param[in] rect is a size of file open dialog window.
     *
     * Note: list the content of current directory.
     */
    explicit FileOpenDialog(const Rect& rect = {});

    /**
     * Create a file open dialog window.
     *
     * @param[in] rect is a size of file open dialog window.
     * @param[in] filepath is to list the contents of filepath directory.
     *
     * Note: filepath should be a directory if the file path is regular
     * file, then the filedialog will list the content of parent directory.
     */
    explicit FileOpenDialog(const std::string& filepath = {}, const Rect& rect = {});

    virtual void show() override;

    virtual void show_centered() override;

    virtual void set_selected(const std::string& fselect) override;

    /**
     * user chosen file from file open dialog window.
     *
     * @return full path of the selected file.
     */
    virtual std::string selected() const;

    virtual ~FileOpenDialog() = default;

protected:
    std::shared_ptr<StaticGrid> m_grid;
    std::shared_ptr<Button> m_okay;
    std::shared_ptr<Button> m_cancel;
    std::string m_fselected;
};

class FileSaveDialog : public FileDialog
{
public:
    /**
     * Create a file save dialog window.
     *
     * @param[in] rect is a size of file open dialog window.
     *
     * Note: list the content of current directory.
     */
    explicit FileSaveDialog(const Rect& rect = {});

    /**
     * Create a file save dialog window.
     *
     * @param[in] rect is a size of file save dialog window.
     * @param[in] filepath is to list the contents of filepath directory.
     *
     * Note: filepath should be a directory. If file path is regular
     * file, then the filedialog will list the content of parent directory.
     */
    explicit FileSaveDialog(const std::string& filepath = {}, const Rect& rect = {});

    virtual void show() override;

    virtual void show_centered() override;

    virtual void set_selected(const std::string& fselect) override;

    /**
     * user chosen location for saving a file from save dialog window.
     *
     * @return full path of the file location.
     */
    virtual std::string selected() const;

    virtual ~FileSaveDialog() = default;

protected:
    std::shared_ptr<HorizontalBoxSizer> m_hpositioner;
    std::shared_ptr<TextBox> m_fsave_box;
    std::shared_ptr<StaticGrid> m_grid;
    std::shared_ptr<Button> m_okay;
    std::shared_ptr<Button> m_cancel;
    std::string m_fsave;
};

}

}

#endif
