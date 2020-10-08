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
#include "../config.h"
#include "webwidget_s.h"
#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbers of image search services.
 */
enum e_wall_sites {
    WEB_WIDGET_PEXELS, /**< Pexels */
#ifdef HAVE_FLICKCURL
    WEB_WIDGET_FLICKR, /**< Flickr */
#endif
    WEB_WIDGET_CNT     /**< Services count */
};
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

