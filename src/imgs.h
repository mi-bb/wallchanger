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
    W_ICON_ADD,      /**< Add images icon */
    W_ICON_ADD_DIR,  /**< Add images from directory icon*/
    W_ICON_REMOVE,   /**< Remove images from list icon */
    W_ICON_SORT,     /**< Sort wallpapers on list icon */
    W_ICON_UP,       /**< Move up icon */
    W_ICON_DOWN,     /**< Move down icon */
    W_ICON_FLOPPY,   /**< Save settings floppy icon */
    W_ICON_SCREEN,   /**< Set wallpaper screen icon */
    W_ICON_DUPL,     /**< Remove duplicates icon */
    W_ICON_INFO,     /**< Info about application icon */
    W_ICON_EXIT,     /**< Exit app icon */
    W_ICON_ABOUT,    /**< About app dialog image */
    W_IMG_SCREEN_1,  /**< Preview image top part (screen) */
    W_IMG_SCREEN_2,  /**< Preview image middle part (screen) */
    W_IMG_SCREEN_3   /**< Preview image bottom part (screen) */
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
