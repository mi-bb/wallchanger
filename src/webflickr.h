/**
 * @file  webflickr.h
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
 * @brief  Settings for searching the Flickr website.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WEBFLICKR_H
#define WEBFLICKR_H

#include "../config.h"

#ifdef HAVE_FLICKCURL

#include "nstrings.h"
#include "webwidget_s.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Flickr database.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @param[in]     ns_data    N strings with API keys
 * @return        none
 */
void flickr_search          (WebWidget      *ww_widget,
                             const NStrings *ns_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Flickr service settings.
 *
 * @param[in,out] ns_data  N strings to get and write API keys
 * @return        Dialog response
 */
int  flickr_settings_dialog (NStrings       *ns_data);
/*----------------------------------------------------------------------------*/
#endif
#endif

