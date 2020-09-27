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
 * @var   WebWidget::search_box
 * @brief Widget with search widgets
 *
 * @var   WebWidget::nav_box
 * @brief Widget with page navigation widgets
 *
 * @var   WebWidget::combo
 * @brief Combobox with image services
 *
 * @var   WebWidget::entry
 * @brief Search query entry
 *
 * @var   WebWidget::nav_entry
 * @brief Custom page set entry
 *
 * @var   WebWidget::img_view
 * @brief Image list icon view
 *
 * @var   WebWidget::selected_combo
 * @brief Combobox with images selected for download
 *
 * @var   WebWidget::selected_box
 * @brief Widget with select for download widgets
 *
 * @var   WebWidget::count_label
 * @brief Label for search results information
 *
 * @var   WebWidget::query
 * @brief String with search query
 *
 * @var   WebWidget::cfg_file
 * @brief String with config file path
 *
 * @var   WebWidget::page
 * @brief Current page number
 *
 * @var   WebWidget::per_page
 * @brief Number with images per page
 *
 * @var   WebWidget::found_cnt
 * @brief Number of found images
 */
typedef struct
WebWidget {
    GtkWidget *search_box;
    GtkWidget *nav_box;
    GtkWidget *combo;
    GtkWidget *entry;
    GtkWidget *nav_entry;
    GtkWidget *img_view;
    GtkWidget *selected_combo;
    GtkWidget *selected_box;
    GtkWidget *count_label;
    char      *query;
    char      *cfg_file;
    int        page;
    int        per_page;
    int        found_cnt;
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

