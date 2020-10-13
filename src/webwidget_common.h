/**
 * @file  webwidget_common.h
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
#ifndef WEBWIDGET_COMMON_H
#define WEBWIDGET_COMMON_H

#include <gtk/gtk.h>
#include "searchitem.h"

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
 * @brief  Check if string s_str is null or empty and show message s_msg
 *         if it is.
 *
 * @param[in]  s_str  String to check
 * @param[in]  s_msg  Message to show
 * @return     1 if string is empty, 0 if it is not.
 */
int    check_is_empty             (const char *s_str,
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
int    check_is_alnum_space       (const char *s_str,
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
 * @param[out] gw_iconview  IconView to add image
 * @param[in]  si_item      SearchItem with image data
 * @param[in]  s_cache_dir  String with app's cache directory
 * @param[in]  s_prefix     File name prefix
 * @return     none  
 */
void   add_searchitem_to_img_view (GtkWidget        *gw_iconview,
                                   const SearchItem *si_item,
                                   const char       *s_cache_dir,
                                   const char       *s_prefix);
/*----------------------------------------------------------------------------*/
#endif

