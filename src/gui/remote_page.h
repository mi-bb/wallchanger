/**
 * @file  remote_page.h
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
 * @brief  Main window page with remote live wallpaper services.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef REMOTE_PAGE_H
#define REMOTE_PAGE_H

#include <gtk/gtk.h>
#include "settings/setting_item.h"
#include "dialog_data.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create the remote live wallpaper services page.
 *
 * @param[in] data  DialogData object with settings and widget data, used by
 *                  the "Set wallpaper now" button
 * @return    Page widget
 */
[[nodiscard]] GtkWidget * remote_page_new (DialogData *data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the page settings: enabled services and keywords.
 *
 * @param[in] page  Remote page widget
 * @return    List of Setting items
 */
[[nodiscard]] Setting * remote_page_get_settings (GtkWidget *page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the page widgets from settings.
 *
 * @param[in,out] page      Remote page widget
 * @param[in]     settings  Program settings
 * @return        none
 */
void remote_page_set_settings (GtkWidget *page,
                               Setting   *settings);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if any service is enabled on the page.
 *
 * @param[in] page  Remote page widget
 * @return    true if at least one service is enabled
 */
[[nodiscard]] bool remote_page_has_service (GtkWidget *page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Show a wallpaper preview with its title, author and license.
 *
 * @param[in,out] page  Remote page widget
 * @param[in]     file  Wallpaper file, can be null
 * @return        none
 */
void remote_page_show_wallpaper (GtkWidget  *page,
                                 const char *file);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the screen size in pixels found when the page was created.
 *
 * @param[in]  page    Remote page widget
 * @param[out] width   Screen width, 0 when it is not known
 * @param[out] height  Screen height, 0 when it is not known
 * @return     none
 */
void remote_page_get_screen_size (GtkWidget *page,
                                  int       *width,
                                  int       *height);
/*----------------------------------------------------------------------------*/
#endif
