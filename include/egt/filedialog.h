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
#include <egt/detail/meta.h>
#include <egt/grid.h>
#include <egt/label.h>
#include <egt/list.h>
#include <egt/popup.h>
#include <egt/signal.h>
#include <egt/sizer.h>
#include <egt/text.h>
#include <string>

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
 * @note FileDialog widget is using a std::experimental::filesystem
 * library
 */
class EGT_API FileDialog : public Popup
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when a selection is made.
     */
    Signal<> on_selected;
    /** @} */

    /**
     * Create a file open dialog window and list the contents
     * of current directory.
     *
     * @param[in] rect Initial rectangle of the Widget.
     *
     */
    explicit FileDialog(const Rect& rect = {}) noexcept;

    /**
     * Create a file dialog window.
     *
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] filepath is to list the contents filepath directory.
     *
     * @note filepath should be a directory. If the file path is regular file,
     * then content of its parent directory will be listed.
     */
    explicit FileDialog(const std::string& filepath, const Rect& rect = {}) noexcept;

protected:

    /// Vertical BoxSizer
    BoxSizer m_vsizer;

    /// Title of a FileDialog.
    ImageLabel m_title;

    /// List Box for file listing.
    ListBox m_flist;

    /// File path of a directory.
    std::string m_filepath;

    /// List the contents of file path directory.
    bool list_files(const std::string& filepath);

    /// Get the List Item selected index.
    void list_item_selected(int index);

    /**
     * File selected.
     *
     * @param[in] fselect Selected filename.
     */
    virtual void selected(const std::string& fselect) = 0;
};

/**
 * A FileOpenDialog is a widget which inherits from FileDialog
 * and that allows user to:
 *
 * 1. Choose a file from the file system.
 * 2. View the contents of file system directories.
 *
 */
class EGT_API FileOpenDialog : public FileDialog
{
public:
    /**
     * Create a file open dialog window and list the contents
     * of current directory.
     *
     * @param[in] rect Initial rectangle of the Widget.
     */
    explicit FileOpenDialog(const Rect& rect = {}) noexcept;

    /**
     * Create a file open dialog window.
     *
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] filepath is to list the contents of filepath directory.
     *
     * @note filepath should be a directory. If the file path is regular file,
     * then content of its parent directory will be listed.
     */
    explicit FileOpenDialog(const std::string& filepath, const Rect& rect = {}) noexcept;

    /**
     * Show the Widget.
     *
     * This changes the visible() state of the Widget.
     */
    void show() override;

    void show_centered() override;

    /**
     * Return file selected in file open dialog window.
     *
     * @return full path of the selected file.
     */
    std::string selected() const;

protected:
    /// Grid for organizing okay & cancel Buttons.
    StaticGrid m_grid;

    /// Okay Button.
    Button m_okay;

    /// Cancel button.
    Button m_cancel;

    /// File path of a selected file.
    std::string m_fselected;

    /**
     * File selected.
     *
     * @param[in] fselect Selected filename.
     */
    void selected(const std::string& fselect) override;
};

/**
 * A FileSaveDialog is a widget which inherits from FileDialog
 * and that allows user to:
 *
 * 1. View the contents of file system directories.
 * 2. Select a location to save a file or select a
 *    file to override a existing file.
 */
class EGT_API FileSaveDialog : public FileDialog
{
public:
    /**
     * Create a file save dialog window and list the contents
     * of current directory.
     *
     * @param[in] rect Initial rectangle of the Widget.
     */
    explicit FileSaveDialog(const Rect& rect = {}) noexcept;

    /**
     * Create a file save dialog window.
     *
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] filepath is to list the contents of filepath directory.
     *
     * @note filepath should be a directory. If the file path is regular file,
     * then content of its parent directory will be listed.
     */
    explicit FileSaveDialog(const std::string& filepath, const Rect& rect = {}) noexcept;

    /**
     * Show the Widget.
     *
     * This changes the visible() state of the Widget.
     */
    void show() override;

    void show_centered() override;

    /**
     * Selected a location for saving a file.
     *
     * @return file path of the file save location.
     */
    std::string selected() const;

protected:

    /// Input a filename or file path.
    TextBox m_fsave_box;

    /// Grid for organizing okay & cancel Buttons.
    StaticGrid m_grid;

    /// Okay Button.
    Button m_okay;

    /// Cancel Button.
    Button m_cancel;

    /// File path of a file save location.
    std::string m_fsave;

    /**
     * File selected.
     *
     * @param[in] fselect Selected filename.
     */
    void selected(const std::string& fselect) override;
};

}
}

#endif
