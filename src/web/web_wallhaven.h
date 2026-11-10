/**
 * @file  web_wallhaven.h
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
 * @brief  Settings for searching the Wallhaven website.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WEB_WALLHAVEN_H
#define WEB_WALLHAVEN_H

#include <gtk/gtk.h>
#include "webwidget_struct.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Wallhaven database.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @param[in]     api_key API key of the service
 * @return        none
 */
void wallhaven_search (WebWidget      *widget,
                       const char     *api_key);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Wallhaven service settings.
 *
 * @param[in,out] api_key  API key to show, replaced on accept
 * @return        Dialog response
 */
int wallhaven_settings_dialog (char **api_key);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Options for image search dialog.
 *
 * @param[in,out] widget  WebWidget with widgets and search data
 * @return        String with image options. After use it should be freed
 *                using free.
 */
char *wallhaven_search_opts_dialog (WebWidget *widget);
/*----------------------------------------------------------------------------*/
#endif 

