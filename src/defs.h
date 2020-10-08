/**
 * @file  defs.h
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
 * @brief  Application defines
 * 
 * @author Michał Bąbik <michal.babik@pm.me>
 */
#ifndef DEFS_H
#define DEFS_H
/*----------------------------------------------------------------------------*/
/**
 * @def   APP_NAME
 * @brief String with application name
 *
 * @def   APP_VER
 * @brief String with application version
 *
 * @def   APP_WEB
 * @brief Application webpage address
 *
 * @def   APP_DSC
 * @brief Application description
 *
 * @def   STATUS_CONFIG
 * @brief Prefix for statusbar config file info
 */
/*----------------------------------------------------------------------------*/
#define APP_NAME      "Wall Changer"
#define APP_VER       "1.6.2"
#define APP_WEB       "https://www.nongnu.org/wallchanger/"
#define APP_DSC       "Wall Changer is an automatic wallpaper changer."
#define STATUS_CONFIG "Config file in use: "
/*----------------------------------------------------------------------------*/
/**
 * @def   DEFAULT_BG_CMD
 * @brief Default command to set wallpapers
 *
 * @def   DEFAULT_INTERVAL_VAL
 * @brief Default wallpaper change interval
 *
 * @def   DEFAULT_LAST_USED_OPT
 * @brief Default setting for setting last used wallpaper on start
 *
 * @def   DEFAULT_RANDOM_OPT
 * @brief Default setting for random wallpaper change
 *
 * @def   DEFAULT_TIME_ALIGN_OPT
 * @brief Default setting for time align
 *
 * @def   DEFAULT_LAST_USED_WM
 * @brief Default setting for last used window manager
 */
/*----------------------------------------------------------------------------*/
#define DEFAULT_BG_CMD        "feh --bg-fill \"[F]\""
#define DEFAULT_INTERVAL_VAL   30
#define DEFAULT_LAST_USED_OPT  0
#define DEFAULT_RANDOM_OPT     0
#define DEFAULT_TIME_ALIGN_OPT 0
#define DEFAULT_LAST_USED_WM   "Unknown"
/*----------------------------------------------------------------------------*/
/**
 * @def   IMGS_ON_PAGE
 * @brief Number of images to show on one page
 *
 * @def   SEL_TH_WIDTH
 * @brief Thumbnail width size for selected images combo list
 *
 * @def   SEL_TH_HEIGHT
 * @brief Thumbnail height size for selected images combo list
 */
/*----------------------------------------------------------------------------*/
#define IMGS_ON_PAGE  12

#define SEL_TH_WIDTH  50.0
#define SEL_TH_HEIGHT 50.0
/*----------------------------------------------------------------------------*/
/**
 * @def   DEFAULT_WIN_WIDTH
 * @brief Default settings dialog window width
 *
 * @def   DEFAULT_WIN_HEIGHT
 * @brief Default settings dialog window height
 *
 * @def   DEFAULT_WEB_DLG_WIDTH
 * @brief Default wallpaper web search dialog width
 *
 * @def   DEFAULT_WEB_DLG_HEIGHT
 * @brief Default wallpaper web search dialog height
 */
/*----------------------------------------------------------------------------*/
#define DEFAULT_WIN_WIDTH      1024
#define DEFAULT_WIN_HEIGHT     768

#define DEFAULT_WEB_DLG_WIDTH  1200
#define DEFAULT_WEB_DLG_HEIGHT 800
/*----------------------------------------------------------------------------*/
/**
 * @def   PATH_AUTOSTART_DATA
 * @brief Path for autostart file template in application data directory.
 *
 * @def   PATH_AUTOSTART_HOME
 * @brief Path for user's autostart desktop file for whcnagerd.
 *
 * @def   PATH_WMINFO_DATA
 * @brief Path for window manager info file in application data directory.
 *
 * @def   PATH_WMINFO_HOME
 * @brief Path for user's config file with window manager info.
 *
 * @def   PATH_APP_SHARE
 * @brief Path for application directory in system's data directory.
 *
 * @def   PATH_WALLPAPERS
 * @brief Wallpaper save path
 */
/*----------------------------------------------------------------------------*/
#define PATH_AUTOSTART_DATA "/wchangerd.autostart"
#define PATH_AUTOSTART_HOME "/autostart/wchangerd.desktop"
#define PATH_WMINFO_DATA    "/wms.json"
#define PATH_WMINFO_HOME    "/wchanger/wms.json"
#define PATH_APP_SHARE      "/wchanger"
#define PATH_WALLPAPERS     "/wchanger/wallpapers"
/*----------------------------------------------------------------------------*/
#endif

