/**
 * @file  dialog_data.h
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
 *
 * This file is part of Wall Changer.
 *
 * Wall Changer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wall Changer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wall Changer.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @brief  DialogData structure and functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef DIALOG_DATA_H
#define DIALOG_DATA_H

#include <gtk/gtk.h>
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct DialogData
 *
 * @brief  Structure to pass widgets and settings to callback
 *
 * @var   DialogData::window
 * @brief Pointer to main config dialog window
 * @var   DialogData::view
 * @brief Pointer to TreeView with wallpaper list
 * @var   DialogData::random
 * @brief Pointer to CheckBox for selecting random wallpapers
 * @var   DialogData::last_used
 * @brief Pointer to CheckBox for restoring last used wallpaper on start
 * @var   DialogData::time_align
 * @brief Pointer to CheckBox for time aligning
 * @var   DialogData::command
 * @brief Pointer to Entry with command to set wallpaper
 * @var   DialogData::interval
 * @brief Pointer to CheckBox for selecting wallpaper change interval
 * @var   DialogData::interval_combo
 * @brief Pointer to ComboBox for selecting wallpapet change interval
 * @var   DialogData::daemon_label
 * @brief Daemon info label
 * @var   DialogData::status_bar
 * @brief Status bar
 * @var   DialogData::img_preview
 * @brief Wallpaper preview image
 * @var   DialogData::config_file
 * @brief Config file path
 */
typedef struct
DialogData {
    GtkWindow  *window;         /* App window pointer */
    GtkWidget  *view;           /* TreeView file list widget */
    GtkWidget  *random;         /* Random background select check box */
    GtkWidget  *last_used;      /* Set last used wallpaper check box */
    GtkWidget  *time_align;     /* Time align check box */
    GtkWidget  *command;        /* Set wallpaper command entry */
    GtkWidget  *interval;       /* Time interval check button */
    GtkWidget  *interval_combo; /* Time interval combo box */
    GtkWidget  *daemon_label;   /* Daemon info label */
    GtkWidget  *status_bar;     /* Status bar */
    GtkWidget  *img_preview;    /* Wallpaper preview image */
    char       *config_file;    /* Config file path */
} DialogData;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free DialogData object.
 *
 * @param[out] data  DialogData object to free
 * @return     None
 */
void         dialog_data_free                   (DialogData       *data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new DialogData object.
 *
 * @return  New DialogData object
 */
DialogData * dialog_data_new                    (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config file, try to create it if not found. When
 *         config_file from data is not null, checks it. Exits on fail.
 *
 * @param[in] data  DialogData object
 * @return    none
 */
void         dialog_data_do_config_file_stuff   (DialogData       *data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get config file path from dialog_data.
 *
 * @param[in] data  DialogData object
 * @return    String with config file path
 */
static inline const char *
dialog_data_get_cfg_file (const DialogData *data) {
    return (const char*) data->config_file;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get information about config file for status bar.
 *
 * @param[in] data  DialogData object
 * @return    String with config file information
 */
char       * dialog_data_get_status_config_info (const DialogData *data);
/*----------------------------------------------------------------------------*/
#endif
