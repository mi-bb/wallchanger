/**
 * @file  wmsfn.h
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
 * @brief  Wallpaper set command functions.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef WMSFN_H
#define WMSFN_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @struct Wms
 *
 * @brief Structure with window manager info.
 *
 * @var   Wms::wm_id
 * @brief Window manager id
 *
 * @var   Wms::process
 * @brief Window manager process name used to find it
 *
 * @var   Wms::name
 * @brief Name of window manager
 *
 * @var   Wms::command
 * @brief Command to set wallpaper for window manager
 */
typedef struct Wms {
    int  wm_id;
    char process [32];
    char name    [16];
    char command [512];
} Wms;
/*----------------------------------------------------------------------------*/
enum {
    WM_ID_UNKNOWN = 0, /**< Id for unknown manager */
    WM_ID_I3,          /**< Id for i3 window manager */
    WM_ID_SPECTRWM,    /**< Id for Spectrwm window manager */
    WM_ID_MATE,        /**< Id foe MATE window manager */
    WM_ID_GNOME,       /**< Id for Gnome window manager */
    WM_ID_XFCE,        /**< Id for Xfce window manager */
    WM_ID_CINNAMON,    /**< Id for Cinnamon window manager */
    WM_ID_PLASMA,      /**< Id for KDE Plasma window manager */
    WM_ID_OPENBOX,     /**< Id for Openbox window manager */
    WM_ID_FLUXBOX,     /**< Id for Fluxbox window manager */
    WM_ID_LXDE,        /**< Id for LXDE window manager */
    WM_ID_FVWM,        /**< Id for FVWM window manager */
    WM_ID_WMAKER,      /**< Id for Window Maker window manager */
    WM_ID_END          /**< End of Ids */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get null terminated list of window manager's data.
 *
 * @return List of wm data. After use it should be freed using wms_list_free
 */
Wms ** wms_get_wm_list (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free list of window manager's data.
 *
 * @param[in,out]  wms_list  Wms list to free
 * @return         none
 */
void wms_free_wm_list (Wms **wms_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of Xfce displays possible to set wallpaper.
 *
 * @return Null terminated list of display strings.
 */
char ** wms_get_xfce_display_list (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free null terminated list of strings.
 *
 * @param[in,out]  s_list  Null terminated list of strings
 * @return         none
 */
void wms_free_xfce_display_list (char **s_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create Xfce wallpaper set command with display name given in s_disp.
 *
 * @param[in] s_disp  String with display for command
 * @return    New string with wallpaper set command. After use it should be
 *            freeed using free.
 */
char *  wms_get_xfce_command (const char *s_disp);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find window manager that is currently in use.
 *
 * return     Wms item with info about window manager or null if no wm was
 *            found. After use it should be freed using free.
 */
Wms * wms_get_current_wm (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find window manager that is currently in use and return it's id.
 *
 * return     Id of found window managaer or -1 if unknown
 */
int wms_get_current_wm_id (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find command for current window manager.
 *
 * @return String with wallpaper set command or null. After use it should be
 *         freed using free.
 */
char * wms_find_command (void);
/*----------------------------------------------------------------------------*/
#endif

