/**
 * @file  webwidget_common.h
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
 * @brief  Common functions for webwidgets.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WEBWIDGET_COMMON_H
#define WEBWIDGET_COMMON_H

#include <gtk/gtk.h>
#include "config.h"
#include "webwidget_struct.h"
//#include "webservcs.h"
#include "nstrings.h"
#include "setting_item.h"
#include "search_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbers of image search services.
 */
enum e_wall_sites : int {
    WEB_SERV_WALLHAVEN, /**< Wallhaven */
    WEB_SERV_PIXABAY,   /**< Pixabay */
    WEB_SERV_WALLABYSS, /**< Wallpaper Abyss */
#ifdef HAVE_FLICKCURL
    WEB_SERV_FLICKR,    /**< Flickr */
#endif
    WEB_SERV_CNT        /**< Services count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Image list columns
 */
enum e_img_list_columns : int {
    WEB_COL_PIXBUF,    /**< Thumbnail pixbuf */
    WEB_COL_WIDTH,     /**< Image width */
    WEB_COL_HEIGHT,    /**< Image height */
    WEB_COL_ID,        /**< Image id */
    WEB_COL_DISP_NAME, /**< Name of image */
    WEB_COL_AUTHOR,    /**< Author of image */
    WEB_COL_MARKUP,    /**< Markup string */
    WEB_COL_FILE_NAME, /**< Name of image */
    WEB_COL_PAGE_URL,  /**< Picture page url */
    WEB_COL_IMAGE_URL, /**< Full size image url */
    WEB_COL_THUMB_URL, /**< Thumbnail url */
    WEB_NUM_CNT        /**< Column count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget name.
 *
 * @param[in] site  Site id
 * @return    String with webwidget name
 */
const char * ww_name                (const int site) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget options name.
 *
 * @param[in] site  Site id
 * @return    String with webwidget name
 */
const char * ww_opts                (const int site) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget logo image id based on webwidget if.
 *
 * @param[in] site  Site id
 * @return    Logo id
 */
int          ww_logo_id             (const int site) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get API key data of given service from settings.
 *
 * @param[in] settings  Settings item
 * @param[in] site      Image service id
 * @return    NStrings item with API key data
 */
NStrings   * ww_get_api_key_data    (Setting   *settings,
                                     const int  site);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update API key data in settings.
 *
 * @param[in] cfg_file  Config file path
 * @param[in] key_data  NStrings item with API key data
 * @param[in] site      Image service id
 * @return    Process result
 */
int          ww_update_api_key_data (const char *cfg_file,
                                     NStrings   *key_data,
                                     const int   site);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string str is null or empty and show message msg
 *         if it is.
 *
 * @param[in]  str  String to check
 * @param[in]  msg  Message to show
 * @return     1 if string is empty, 0 if it is not.
 */
int    str_is_empty_warn          (const char *str,
                                   const char *msg);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string str is made of alphanumeric chars and spaces
 *         and show message msg if it is not.
 *
 * @param[in]  str  String to check
 * @param[in]  msg  Message to show
 * @return     1 if string is made of alphanumeric chars and spaces,
 *             0 if it is not.
 */
int    str_is_alnum_or_space_warn (const char *str,
                                   const char *msg);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string str is made of unicode chars
 *         and show message msg if it is not.
 *
 * @param[in]  str  String to check
 * @param[in]  msg  Message to show
 * @return     1 if string is made of unicode chars, 0 if it is not.
 */
int    check_unicode_warn         (const char *str,
                                   const char *msg);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check from string str all non alphanumeric chars and non spaces.
 *
 * @param[in]  str  String to process
 * @return     none
 */
void   remove_non_alpha_space     (char *str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string value from Combobox's active col column.
 *
 * @param[in] combo  Combobox
 * @param[in] col    Column with data
 * @return    String value
 */
char * combo_get_active_str       (GtkWidget  *combo,
                                   const int   col);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add image based on item data to icon view.
 *
 * @param[out] iconview       IconView to add image
 * @param[in]  item           SearchItem with image data
 * @param[in]  wallpaper_dir  String with app's wallpaper directory
 * @param[in]  service_name   Name of search service
 * @param[in]  thumb_quality  Thumbnail jpg quality
 * @return     none
 */
void   add_search_item_to_img_view (GtkWidget        *iconview,
                                    const SearchItem *item,
                                    const char       *wallpaper_dir,
                                    const char       *service_name,
                                    const int         thumb_quality);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check for search query cached info, get info and add items if info
 *         was found.
 *
 * @param[in,out] widget        WebWidget item
 * @param[in]     service_name  Name of search service
 * @return        1 if cached query was found, 0 if not
 */
int    check_for_cached_query     (WebWidget        *widget,
                                   const char       *service_name);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Converts image search options in Setting format to string for url.
 *
 * @param[in] settings  Setting item with search options
 * @return    String with image options. After use it should be freed
 *            using free.
 */
char * search_opts_to_str         (const Setting    *settings);
/*----------------------------------------------------------------------------*/
#endif
