/**
 * @file  preview.h
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
 * @brief  Wallpaper preview
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef PREVIEW_H
#define PREVIEW_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @fn  void get_screen_size (int *width,
 *                            int *height)
 * @brief  Get screen size.
 *
 * @param[out] width   Screen width
 * @param[out] height  Screen height
 * @return     none
 *
 * @fn  void preview_from_file (GtkWidget  *image,
 *                              const char *file)
 *
 * @brief  Make preview image widget of image (file path).
 *
 * @param[out] image  Preview image widget
 * @param[in]  file   File to make preview image
 * @return     none
 *
 * @fn  GtkWidget * create_preview_label (void)
 *
 * @brief  Create labels with screen resolution info.
 *
 * @return GtkWidget with screen resolution info.
 */
/*----------------------------------------------------------------------------*/
void        get_screen_size      (int        *width,
                                  int        *height);
/*----------------------------------------------------------------------------*/
void        preview_from_file    (GtkWidget  *image,
                                  const char *file);
/*----------------------------------------------------------------------------*/
GtkWidget * create_preview_label (void);
/*----------------------------------------------------------------------------*/
#endif

