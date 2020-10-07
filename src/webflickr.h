/**
 * @file  webflickr.h
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
 * @brief  Settings for searching in Flickr website.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef WEBFLICKR_H
#define WEBFLICKR_H

#include "../config.h"

#ifdef HAVE_FLICKCURL

#include "fourstrings.h"
#include "webwidget_s.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Flickr service settings.
 *
 * @param[in,out] fs_data  Four strings to get and write API keys
 * @return        Dialog response
 */
int  flickr_settings_dialog (FourStrings       *fs_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Flickr database.
 *
 * @param[in,out] ww_widget  WebWidget with widgets and search data
 * @param[in]     fs_data    Four strings with API keys
 * @return        none
 */
void flickr_search          (WebWidget         *ww_widget,
                             const FourStrings *fs_data);
/*----------------------------------------------------------------------------*/
#endif
#endif

