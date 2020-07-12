/**
 * @file  dialogdata.h
 * @copyright Copyright (C) 2019-2020 Michał Bąbik
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
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef DIALOGDATA_H
#define DIALOGDATA_H

#include <gtk/gtk.h>
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct DialogData
 *
 * @brief  Structure to pass widgets and settings to callback
 *
 * @var   DialogData::gw_window
 * @brief Pointer to main config dialog window
 * @var   DialogData::gw_view
 * @brief Pointer to TreeView with wallpaper list
 * @var   DialogData::gw_random
 * @brief Pointer to CheckBox for selecting random wallpapers
 * @var   DialogData::gw_lastused
 * @brief Pointer to CheckBox for restoring last used wallpaper on start
 * @var   DialogData::gw_timealign
 * @brief Pointer to CheckBox for time aligning
 * @var   DialogData::gw_command
 * @brief Pointer to Entry with command to set wallpaper
 * @var   DialogData::gw_interval
 * @brief Pointer to CheckBox for selecting wallpaper change interval
 * @var   DialogData::gw_inter_combo
 * @brief Pointer to ComboBox for selecting wallpapet change interval
 * @var   DialogData::gw_dm_label
 * @brief Daemon info label
 * @var   DialogData::gw_statusbar
 * @brief Status bar
 * @var   DialogData::gw_imgprev
 * @brief Wallpaper preview image
 * @var   DialogData::s_cfg_file
 * @brief Config file path
 */
typedef struct
DialogData {
    GtkWindow  *gw_window;      /* App window pointer */
    GtkWidget  *gw_view;        /* TreeView file list widget */
    GtkWidget  *gw_random;      /* Random background select check box */
    GtkWidget  *gw_lastused;    /* Set last used wallpaper check box */
    GtkWidget  *gw_timealign;   /* Time align check box */
    GtkWidget  *gw_command;     /* Set wallpaper command entry */
    GtkWidget  *gw_interval;    /* Time interval check button */
    GtkWidget  *gw_inter_combo; /* Time interval combo box */
    GtkWidget  *gw_dm_label;    /* Daemon info label */
    GtkWidget  *gw_statusbar;   /* Status bar */
    GtkWidget  *gw_imgprev;     /* Wallpaper preview image */
    char       *s_cfg_file;     /* Config file string */
} DialogData;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free DialogData object.
 *
 * @param[out] dd_data  DialogData object to free
 * @return     None
 */
void         dialogdata_free                   (DialogData       *dd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new DialogData object.
 *
 * @return  New DialogData object
 */
DialogData * dialogdata_new                    (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config file, try to create it if not found. When
 *         s_cfg_file from dd_data is not null, checks it. Exits on fail.
 *
 * @param[in] dd_data  DialogData object
 * @return    none
 */
void         dialogdata_do_config_file_stuff   (DialogData       *dd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get config file path from DialogData.
 *
 * @param[in] dd_data  DialogData object
 * @return    String with config file path
 */
static inline const char *
dialogdata_get_cfg_file (const DialogData *dd_data) {
    return (const char*) dd_data->s_cfg_file;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get information about config file for status bar.
 *
 * @param[in] dd_data  DialogData object
 * @return    String with config file information
 */
char       * dialogdata_get_status_config_info (const DialogData *dd_data);
/*----------------------------------------------------------------------------*/
#endif

