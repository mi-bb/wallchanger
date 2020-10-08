/**
 * @file  searchitem.h
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
 * @brief  Web search result
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef SEARCHITEM_H
#define SEARCHITEM_H

#include <stdint.h>

/*----------------------------------------------------------------------------*/
/**
 * @struct SearchItem
 *
 * @brief  Structure for storing wallpaper search data.
 *
 * @var   SearchItem::i_id
 * @brief Image id
 *
 * @var   SearchItem::s_file_name
 * @brief Name for file save
 *
 * @var   SearchItem::s_display_name
 * @brief Image name to display
 *
 * @var   SearchItem::s_display_markup
 * @brief Image name to display
 *
 * @var   SearchItem::s_page_url
 * @brief Url for image page
 *
 * @var   SearchItem::s_image_url
 * @brief Url for image file
 *
 * @var   SearchItem::s_thumb_url
 * @brief Url of image thumbnail
 *
 * @var   SearchItem::i_width
 * @brief Image width
 *
 * @var   SearchItem::i_height
 * @brief Image height
 */
typedef struct
SearchItem {
    uint64_t i_id;
    char *s_file_name;
    char *s_display_name;
    char *s_display_markup;
    char *s_page_url;
    char *s_image_url;
    char *s_thumb_url;
    int   i_width;
    int   i_height;
}
SearchItem;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new SearchItem item.
 *
 * @return New SearchItem item
 */
SearchItem * searchitem_new                (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free SearchItem item.
 *
 * @param[out] si_item  SearchItem item to free
 * @return     none
 */
void         searchitem_free               (SearchItem *si_item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's file name.
 *
 * @param[out] si_item  SearchItem item to set file name
 * @param[in]  s_val    File name string to set
 * @return     none
 */
void         searchitem_set_file_name      (SearchItem *si_item,
                                            const char *s_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's display name.
 *
 * @param[out] si_item  SearchItem item to set display name
 * @param[in]  s_val    Display name string to set
 * @return     none
 */
void         searchitem_set_display_name   (SearchItem *si_item,
                                            const char *s_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's markup.
 *
 * @param[out] si_item  SearchItem item to set markup
 * @param[in]  s_val    Markup string to set
 * @return     none
 */
void         searchitem_set_display_markup (SearchItem *si_item,
                                            const char *s_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's page url.
 *
 * @param[out] si_item  SearchItem item to set page url
 * @param[in]  s_val    Page url string to set
 * @return     none
 */
void         searchitem_set_page_url       (SearchItem *si_item,
                                            const char *s_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's image url.
 *
 * @param[out] si_item  SearchItem item to set image url
 * @param[in]  s_val    Image url string to set
 * @return     none
 */
void         searchitem_set_image_url      (SearchItem *si_item,
                                            const char *s_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's thumb url.
 *
 * @param[out] si_item  SearchItem item to set thumb url
 * @param[in]  s_val    Thumb url string to set
 * @return     none
 */
void         searchitem_set_thumb_url      (SearchItem *si_item,
                                            const char *s_val);

/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's id.
 *
 * @param[out] si_item  SearchItem item to set id
 * @param[in]  val      Id integer value to set
 * @return     none
 */
static inline void searchitem_set_id (SearchItem    *si_item,
                                      const uint64_t val) {
    si_item->i_id = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's width.
 *
 * @param[out] si_item  SearchItem item to set width
 * @param[in]  val      Width integer value to set
 * @return     none
 */
static inline void searchitem_set_width (SearchItem *si_item,
                                         const int   val) {
    si_item->i_width = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's height.
 *
 * @param[out] si_item  SearchItem item to set height
 * @param[in]  val      Height integer value to set
 * @return     none
 */
static inline void searchitem_set_height (SearchItem *si_item,
                                          const int   val) {
    si_item->i_height = val;
}
/*----------------------------------------------------------------------------*/
#endif

