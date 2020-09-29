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

/*----------------------------------------------------------------------------*/
/**
 * @struct SearchItem
 *
 * @brief  Structure for storing wallpaper search data.
 *
 * @var   SearchItem::id
 * @brief Image id
 *
 * @var   SearchItem::name
 * @brief Image name
 *
 * @var   SearchItem::markup
 * @brief Markup for image
 *
 * @var   SearchItem::url
 * @brief Main url for image
 *
 * @var   SearchItem::thumb_url
 * @brief Url of image thumbnail
 *
 * @var   SearchItem::width
 * @brief Image width
 *
 * @var   SearchItem::height
 * @brief Image height
 */
typedef struct
SearchItem {
    int   id;
    char *name;
    char *markup;
    char *url;
    char *thumb_url;
    int   width;
    int   height;
}
SearchItem;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new SearchItem item.
 *
 * @return New SearchItem item
 */
SearchItem * searchitem_new           (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free SearchItem item.
 *
 * @param[out] si_item  SearchItem item to free
 * @return     none
 */
void         searchitem_free          (SearchItem *si_item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's name.
 *
 * @param[out] si_item  SearchItem item to set name
 * @param[in]  s_val    Name string to set
 * @return     none
 */
void         searchitem_set_name      (SearchItem *si_item,
                                       const char *s_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's markup.
 *
 * @param[out] si_item  SearchItem item to set markup
 * @param[in]  s_val    Markup string to set
 * @return     none
 */
void         searchitem_set_markup    (SearchItem *si_item,
                                       const char *s_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's url.
 *
 * @param[out] si_item  SearchItem item to set url
 * @param[in]  s_val    Url string to set
 * @return     none
 */
void         searchitem_set_url       (SearchItem *si_item,
                                       const char *s_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's thumb url.
 *
 * @param[out] si_item  SearchItem item to set thumb url
 * @param[in]  s_val    Thumb url string to set
 * @return     none
 */
void         searchitem_set_thumb_url (SearchItem *si_item,
                                       const char *s_val);

/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's id.
 *
 * @param[out] si_item  SearchItem item to set id
 * @param[in]  val      Id integer value to set
 * @return     none
 */
static inline void searchitem_set_id (SearchItem *si_item,
                                      const int   val) {
    si_item->id = val;
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
    si_item->width = val;
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
    si_item->height = val;
}
/*----------------------------------------------------------------------------*/
#endif

