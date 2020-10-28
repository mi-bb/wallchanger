/**
 * @file  chthumb.h
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
 * @brief  Thumbnails cache
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef CHTHUMB_H
#define CHTHUMB_H
#include <gtk/gtk.h>
#include "searchitem.h"
/*----------------------------------------------------------------------------*/
GdkPixbuf * thumbnail_get (const char *s_service_name,
                           const char *s_id,
                           const int   i_jpgq,
                           const char *s_thumb_url);
/*----------------------------------------------------------------------------*/
#endif

