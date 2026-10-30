/**
 * @file  webwidget_struct.h
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
 * @brief  Web widget structure.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WEBWIDGET_STRUCT_H
#define WEBWIDGET_STRUCT_H

#include <gtk/gtk.h>
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
 * @var   WebWidget::image_info_widget
 * @brief Widget with image info widgets
 *
 * @var   WebWidget::service_image
 * @brief Widget with image service image
 *
 * @var   WebWidget::author_label
 * @brief Widget with image author name
 *
 * @var   WebWidget::page_link
 * @brief Widget with link to author's page
 *
 * @var   WebWidget::dim_label
 * @brief Widget with image dimensions
 *
 * @var   WebWidget::query
 * @brief String with search query
 *
 * @var   WebWidget::search_opts
 * @brief String with search options
 *
 * @var   WebWidget::config_file
 * @brief String with config file path
 *
 * @var   WebWidget::wallpaper_dir
 * @brief String with wallpaper directory
 *
 * @var   WebWidget::page
 * @brief Current page number
 *
 * @var   WebWidget::found_cnt
 * @brief Number of found images
 *
 * @var   WebWidget::active_service
 * @brief Id of active seaech service
 *
 * @var   WebWidget::thumb_quality
 * @brief Thumbnail jpg quality
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
    GtkWidget *image_info_widget;
    GtkWidget *service_image;
    GtkWidget *author_label;
    GtkWidget *page_link;
    GtkWidget *dim_label;
    char      *query;
    char      *search_opts;
    char      *config_file;
    char      *wallpaper_dir;
    int        page;
    int        found_cnt;
    int        active_service;
    int        thumb_quality;
} WebWidget;
/*----------------------------------------------------------------------------*/
#endif

