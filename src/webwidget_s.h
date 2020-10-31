/**
 * @file  webwidget_s.h
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
 * @brief  Web widget structure.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef WEBWIDGET_S_H
#define WEBWIDGET_S_H

#include <gtk/gtk.h>
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
 * @var   WebWidget::gw_ii_widget
 * @brief Widget with image info widgets
 *
 * @var   WebWidget::gw_ii_simage
 * @brief Widget with image service image
 *
 * @var   WebWidget::gw_ii_author_label
 * @brief Widget with image author name
 *
 * @var   WebWidget::gw_ii_page_link
 * @brief Widget with link to author's page
 *
 * @var   WebWidget::gw_ii_dim_label
 * @brief Widget with image dimensions
 *
 * @var   WebWidget::s_query
 * @brief String with search query
 *
 * @var   WebWidget::s_search_opts
 * @brief String with search options
 *
 * @var   WebWidget::s_cfg_file
 * @brief String with config file path
 *
 * @var   WebWidget::s_wallp_dir
 * @brief String with wallpaper directory
 *
 * @var   WebWidget::i_page
 * @brief Current page number
 *
 * @var   WebWidget::i_found_cnt
 * @brief Number of found images
 *
 * @var   WebWidget::i_active_service
 * @brief Id of active seaech service
 *
 * @var   WebWidget::i_thumb_quality
 * @brief Thumbnail jpg quality
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
    GtkWidget *gw_ii_widget;
    GtkWidget *gw_ii_simage;
    GtkWidget *gw_ii_author_label;
    GtkWidget *gw_ii_page_link;
    GtkWidget *gw_ii_dim_label;
    char      *s_query;
    char      *s_search_opts;
    char      *s_cfg_file;
    char      *s_wallp_dir;
    int        i_page;
    int        i_found_cnt;
    int        i_active_service;
    int        i_thumb_quality;
} WebWidget;
/*----------------------------------------------------------------------------*/
#endif

