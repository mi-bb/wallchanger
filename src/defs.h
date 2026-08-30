/**
 * @file  defs.h
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
 * @brief  Application defines
 *
 * @author Michał Bąbik <michal.babik@protonmail.com>
 */
#ifndef DEFS_H
#define DEFS_H
#include "config.h"
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
#define APP_VER       PACKAGE_VERSION
#define APP_WEB       "https://www.nongnu.org/wallchanger/"
#define APP_DSC       "Wall Changer is an automatic wallpaper changer."
#define STATUS_CONFIG "Config file in use: "
/*----------------------------------------------------------------------------*/
/**
 * @def   DEFAULT_BG_CMD
 * @brief Default command to set wallpapers
 *
 * @var   DEFAULT_INTERVAL_VAL
 * @brief Default wallpaper change interval
 *
 * @var   DEFAULT_LAST_USED_OPT
 * @brief Default setting for setting last used wallpaper on start
 *
 * @var   DEFAULT_RANDOM_OPT
 * @brief Default setting for random wallpaper change
 *
 * @var   DEFAULT_TIME_ALIGN_OPT
 * @brief Default setting for time align
 *
 * @def   DEFAULT_LAST_USED_WM
 * @brief Default setting for last used window manager
 *
 * @var   DEFAULT_THUMB_QUALITY
 * @brief Default thumbnail jpg quality
 */
/*----------------------------------------------------------------------------*/
#define DEFAULT_BG_CMD               "feh --bg-fill \"[F]\""
constexpr int DEFAULT_INTERVAL_VAL   = 30;
constexpr int DEFAULT_LAST_USED_OPT  = 0;
constexpr int DEFAULT_RANDOM_OPT     = 0;
constexpr int DEFAULT_TIME_ALIGN_OPT = 0;
#define DEFAULT_LAST_USED_WM         "Unknown"
constexpr int DEFAULT_THUMB_QUALITY  = 96;
/*----------------------------------------------------------------------------*/
/**
 * @var   IMGS_ON_PAGE
 * @brief Number of images to show on one page
 *
 * @var   SEL_TH_WIDTH
 * @brief Thumbnail width size for selected images combo list
 *
 * @var   SEL_TH_HEIGHT
 * @brief Thumbnail height size for selected images combo list
 */
/*----------------------------------------------------------------------------*/
constexpr int    IMGS_ON_PAGE  = 12;

constexpr double SEL_TH_WIDTH  = 50.0;
constexpr double SEL_TH_HEIGHT = 50.0;
/*----------------------------------------------------------------------------*/
/**
 * @var   DEFAULT_WIN_WIDTH
 * @brief Default settings dialog window width
 *
 * @var   DEFAULT_WIN_HEIGHT
 * @brief Default settings dialog window height
 *
 * @var   DEFAULT_WEB_DLG_WIDTH
 * @brief Default wallpaper web search dialog width
 *
 * @var   DEFAULT_WEB_DLG_HEIGHT
 * @brief Default wallpaper web search dialog height
 */
/*----------------------------------------------------------------------------*/
constexpr int DEFAULT_WIN_WIDTH      = 1024;
constexpr int DEFAULT_WIN_HEIGHT     = 768;

constexpr int DEFAULT_WEB_DLG_WIDTH  = 1200;
constexpr int DEFAULT_WEB_DLG_HEIGHT = 800;
/*----------------------------------------------------------------------------*/
/**
 * @def   PTH_SEP
 * @brief Path separator
 *
 * @def   PTH_APP_SHORT
 * @brief Short name of application, used for app directory names.
 *
 * @def   PTH_ASTART_FILE_S
 * @brief Source autostart file name for read.
 *
 * @def   PTH_ASTART_FILE_D
 * @brief Destination autostart file name for write.
 *
 * @def   PTH_WMINFO_FILE
 * @brief Nama of file with window manager info.
 *
 * @def   PTH_CONFIG_FILE
 * @brief Main configuration file name.
 *
 * @def   PTH_OLDCFG_FILE
 * @brief Old configuration file name.
 *
 * @def   PTH_WALLP_DIR
 * @brief Name of directory for downloaded wallpapers.
 *
 * @def   PTH_THUMB_DIR
 * @brief Name of thumbnails directory.
 *
 * @def   PTH_IINFO_DIR
 * @brief Directory for storing image info.
 *
 * @def   PTH_QUERY_DIR
 * @brief Directory for image information.
 */
/*----------------------------------------------------------------------------*/
#define PTH_SEP            "/"
#define PTH_APP_SHORT      "wchanger"
#define PTH_ASTART_FILE_S  "wchangerd.autostart"
#define PTH_ASTART_FILE_D  "wchangerd.desktop"
#define PTH_WMINFO_FILE    "wms.json"
#define PTH_CONFIG_FILE    "config.json"
#define PTH_OLDCFG_FILE    "wchanger.json"
#define PTH_WALLP_DIR      "wallpapers"
#define PTH_THUMB_DIR      "thumbnails"
#define PTH_IINFO_DIR      "iinfo"
#define PTH_QUERY_DIR      "query"
/*----------------------------------------------------------------------------*/
#endif

