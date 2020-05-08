/**
 * @file  preview.h
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
 * @brief  Wallpaper preview
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef PREVIEW_H
#define PREVIEW_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @fn  void get_screen_size (int *i_scr_w,
 *                            int *i_scr_h)
 * @brief  Get screen size.
 *
 * @param[out] i_scr_w  Screen width
 * @param[out] i_scr_h  Screen height
 * @return     none
 *
 * @fn  void preview_from_file (GtkWidget  *gw_img_prev,
 *                              const char *s_file)
 *
 * @brief  Make preview image widget of image (file path).
 *
 * @param[out] gw_img_prev  Preview image widget
 * @param[in]  s_file       File to make preview image
 * @return     none
 *
 * @fn  GtkWidget * create_preview_label (void)
 *
 * @brief  Create labels with screen resolution info.
 *
 * @return GtkWidget with screen resolution info.
 */
/*----------------------------------------------------------------------------*/
void        get_screen_size      (int        *i_scr_w,
                                  int        *i_scr_h);
/*----------------------------------------------------------------------------*/
void        preview_from_file    (GtkWidget  *gw_img_prev,
                                  const char *s_file);
/*----------------------------------------------------------------------------*/
GtkWidget * create_preview_label (void);
/*----------------------------------------------------------------------------*/
#endif

