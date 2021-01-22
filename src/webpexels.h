/**
 * @file  webpexels.h
 * @copyright Copyright (C) 2019-2021 Michał Bąbik
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
 * @brief  Settings for searching the Pexels website.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef WEBPEXELS_H
#define WEBPEXELS_H

#include <gtk/gtk.h>
#include "nstrings.h"
#include "webwidget_s.h"

/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Pexels database.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @param[in]     ns_data    n strings with API keys
 * @return        none
 */
void   pexels_search             (WebWidget      *ww_widget,
                                  const NStrings *ns_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Pexels service settings.
 *
 * @param[in,out] ns_data  n strings to get and write API keys
 * @return        Dialog response
 */
int    pexels_settings_dialog    (NStrings       *ns_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Options for image search dialog.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @return        String with image options. After use it should be freed
 *                using free.
 */
char * pexels_search_opts_dialog (WebWidget      *ww_widget);
/*----------------------------------------------------------------------------*/
#endif
