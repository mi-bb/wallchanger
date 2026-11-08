/**
 * @file  thumbnail.h
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
 * @brief  Thumbnail getting
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get thumbnail image from cached data or from web.
 *
 * @param[in] service_name  Name of image search service
 * @param[in] image_id      Image id
 * @param[in] jpg_quality   Thumbnail JPG save quality
 * @param[in] thumbnail_url Thumbnail url
 * @return    Pixbuf with thumbnail image
 */
GdkPixbuf * thumbnail_get (const char *service_name,
                           const char *image_id,
                           const int   jpg_quality,
                           const char *thumbnail_url);
/*----------------------------------------------------------------------------*/
#endif

