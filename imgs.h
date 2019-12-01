/**
 * @file  imgs.h
 * @copyright Copyright (C) 2019 Michał Bąbik
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
 * @brief  Application icons
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef IMGS_H
#define IMGS_H
#include <gdk-pixbuf/gdk-pixbuf.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  enum for icons and images
 */
typedef enum IconImg {
    W_ICON_ADD,
    W_ICON_ADD_DIR,
    W_ICON_REMOVE,
    W_ICON_SORT,
    W_ICON_UP,
    W_ICON_DOWN,
    W_ICON_FLOPPY,
    W_ICON_SCREEN,
    W_ICON_DUPL,
    W_ICON_INFO,
    W_ICON_EXIT,
    W_ICON_ABOUT,
    W_IMG_SCREEN_1,
    W_IMG_SCREEN_2,
    W_IMG_SCREEN_3
} IconImg;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image from stored data.
 *
 * @param[in]  i_img  Image number
 * @return     Pixbuf with image
 */
GdkPixbuf *get_image (const IconImg i_img);
/*----------------------------------------------------------------------------*/
#endif
