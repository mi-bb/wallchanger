/**
 * @file  webwidget_c.h
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
 * @brief  Common functions for webwidgets.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef WEBWIDGET_C_H
#define WEBWIDGET_C_H

#include <gtk/gtk.h>
#include "../config.h"
#include "webwidget_s.h"
#include "nstrings.h"
#include "setting.h"
#include "searchitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbers of image search services.
 */
enum e_wall_sites {
    WEB_WIDGET_PEXELS,    /**< Pexels */
    WEB_WIDGET_PIXBAY,    /**< Pixbay */
    WEB_WIDGET_WALLHAVEN, /**< Wallhaven */
    WEB_WIDGET_WALLABYSS, /**< Wallpaper Abyss */
#ifdef HAVE_FLICKCURL
    WEB_WIDGET_FLICKR,    /**< Flickr */
#endif
    WEB_WIDGET_CNT        /**< Services count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Image list columns
 */
enum e_img_list_columns {
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
 * @param[in] i_site  Site id
 * @return    String with webwidget name
 */
const char * ww_name                (const int i_site) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget options name.
 *
 * @param[in] i_site  Site id
 * @return    String with webwidget name
 */
const char * ww_opts                (const int i_site) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get webwidget logo image id based on webwidget if.
 *
 * @param[in] i_site  Site id
 * @return    Logo id
 */
int          ww_logo_id             (const int i_site) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get API key data of given service from settings.
 *
 * @param[in] st_settings  Settings item
 * @param[in] i_site       Image service id
 * @return    NStrings item with API key data
 */
NStrings   * ww_get_api_key_data    (Setting   *st_settings,
                                     const int  i_site);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update API key data in settings.
 *
 * @param[in] s_cfg_file   Config file path
 * @param[in] ns_key_data  NStrings item with API key data
 * @param[in] i_site       Image service id
 * @return    Process result
 */
int          ww_update_api_key_data (const char *s_cfg_file,
                                     NStrings   *ns_key_data,
                                     const int   i_site);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string s_str is null or empty and show message s_msg
 *         if it is.
 *
 * @param[in]  s_str  String to check
 * @param[in]  s_msg  Message to show
 * @return     1 if string is empty, 0 if it is not.
 */
int    str_is_empty_msg           (const char *s_str,
                                   const char *s_msg);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string s_str is made of alphanumeric chars and spaces
 *         and show message s_msg if it is not.
 *
 * @param[in]  s_str  String to check
 * @param[in]  s_msg  Message to show
 * @return     1 if string is made of alphanumeric chars and spaces,
 *             0 if it is not.
 */
int    str_is_alnum_or_space      (const char *s_str,
                                   const char *s_msg);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string s_str is made of unicode chars
 *         and show message s_msg if it is not.
 *
 * @param[in]  s_str  String to check
 * @param[in]  s_msg  Message to show
 * @return     1 if string is made of unicode chars, 0 if it is not.
 */
int    check_unicode              (const char *s_str,
                                   const char *s_msg);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check from string s_str all non alphanumeric chars and non spaces.
 *
 * @param[in]  s_str  String to process
 * @return     none
 */
void   remove_non_alpha_space     (char *s_str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string value from Combobox's active i_col column.
 *
 * @param[in] gw_combo  Combobox
 * @param[in] i_col     Column with data
 * @return    String value
 */
char * combo_get_active_str       (GtkWidget  *gw_combo,
                                   const int   i_col);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add image based on si_item data to icon view.
 *
 * @param[out] gw_iconview       IconView to add image
 * @param[in]  si_item           SearchItem with image data
 * @param[in]  s_wallp_dir       String with app's wallpaper directory
 * @param[in]  s_service_name    Name of search service
 * @param[in]  i_thumb_quality   Thumbnail jpg quality
 * @return     none
 */
void   add_searchitem_to_img_view (GtkWidget        *gw_iconview,
                                   const SearchItem *si_item,
                                   const char       *s_wallp_dir,
                                   const char       *s_service_name,
                                   const int         i_thumb_quality);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check for search query cached info, get info and add items if info
 *         was found.
 *
 * @param[in,out] ww_widget         WebWidget item
 * @param[in]     s_service_name    Name of search service
 * @return        1 if cached query was found, 0 if not
 */
int    check_for_cached_query     (WebWidget        *ww_widget,
                                   const char       *s_service_name);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Converts image search options in Setting format to string for url.
 *
 * @param[in] st_setts  Setting item with search options
 * @return    String with image options. After use it should be freed
 *            using free.
 */
char * search_opts_to_str         (const Setting    *st_setts);
/*----------------------------------------------------------------------------*/
#endif

