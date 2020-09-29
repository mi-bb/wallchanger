/**
 * @file  webwidget.h
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
 * @brief  Web widget
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef WEBWIDGET_H
#define WEBWIDGET_H

#include <gtk/gtk.h>
#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbers of image search services.
 */
enum e_wall_sites {
    WEB_WIDGET_PEXELS, /**< Pexels */
    WEB_WIDGET_CNT     /**< Services count */
};
/*----------------------------------------------------------------------------*/
/**
 * @struct WebWidget
 *
 * @brief  Structure with wallpaper web search widgets and data.
 *
 * @var   WebWidget::gw_search_box
 * @brief Widget with search widgets
 *
 * @var   WebWidget::gw_nav_box
 * @brief Widget with page navigation widgets
 *
 * @var   WebWidget::gw_combo
 * @brief Combobox with image services
 *
 * @var   WebWidget::gw_entry
 * @brief Search query entry
 *
 * @var   WebWidget::gw_nav_entry
 * @brief Custom page set entry
 *
 * @var   WebWidget::gw_img_view
 * @brief Image list icon view
 *
 * @var   WebWidget::gw_selected_combo
 * @brief Combobox with images selected for download
 *
 * @var   WebWidget::gw_selected_box
 * @brief Widget with select for download widgets
 *
 * @var   WebWidget::gw_count_label
 * @brief Label for search results information
 *
 * @var   WebWidget::s_query
 * @brief String with search query
 *
 * @var   WebWidget::s_cfg_file
 * @brief String with config file path
 *
 * @var   WebWidget::i_page
 * @brief Current page number
 *
 * @var   WebWidget::i_per_page
 * @brief Number with images per page
 *
 * @var   WebWidget::i_found_cnt
 * @brief Number of found images
 */
typedef struct
WebWidget {
    GtkWidget *gw_search_box;
    GtkWidget *gw_nav_box;
    GtkWidget *gw_combo;
    GtkWidget *gw_entry;
    GtkWidget *gw_nav_entry;
    GtkWidget *gw_img_view;
    GtkWidget *gw_selected_combo;
    GtkWidget *gw_selected_box;
    GtkWidget *gw_count_label;
    char      *s_query;
    char      *s_cfg_file;
    int        i_page;
    int        i_per_page;
    int        i_found_cnt;
} WebWidget;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free WebWidget item.
 *
 * @param[out] ww_widget  WebWidget item to free
 * @return     none
 */
void        webwidget_free           (WebWidget  *ww_widget);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create WebWidget item with widgets and data for wallpaper search.
 *
 * @param[in] st_settings  Settings needed to create widgets
 * @param[in] s_cfg_file   Config file path
 * @return    WebWidget item
 */
WebWidget * webwidget_create         (Setting    *st_settings,
                                      const char *s_cfg_file);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save selected wallpapers in user's app share folder.
 *
 * @param[in] gw_dialog  Getting wallpapers from web dialog
 * @param[in] ww_widget  WebWidget item
 * @return    List with wallpapers for adding to wallpaper list.
 */
GList     * save_selected_wallpapers (GtkWidget  *gw_dialog,
                                      WebWidget  *ww_widget);
/*----------------------------------------------------------------------------*/
#endif

