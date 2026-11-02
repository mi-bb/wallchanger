/**
 * @file  search_item.h
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
 * @brief  Web search result
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef SEARCH_ITEM_H
#define SEARCH_ITEM_H

//#include <stdint.h>
#include <inttypes.h>

/*----------------------------------------------------------------------------*/
/**
 * @struct SearchItem
 *
 * @brief  Structure for storing wallpaper search data.
 *
 * @var   SearchItem::id
 * @brief Image id in string
 *
 * @var   SearchItem::author_name
 * @brief Name for image author
 *
 * @var   SearchItem::author_url
 * @brief Url to author's page
 *
 * @var   SearchItem::file_name
 * @brief Name for file save
 *
 * @var   SearchItem::display_name
 * @brief Image name to display
 *
 * @var   SearchItem::display_markup
 * @brief Image name to display
 *
 * @var   SearchItem::page_url
 * @brief Url for image page
 *
 * @var   SearchItem::image_url
 * @brief Url for image file
 *
 * @var   SearchItem::thumb_url
 * @brief Url of image thumbnail
 *
 * @var   SearchItem::service_name
 * @brief Name of image service
 *
 * @var   SearchItem::width
 * @brief Image width
 *
 * @var   SearchItem::height
 * @brief Image height
 */
typedef struct
SearchItem {
    char *id;
    char *author_name;
    char *author_url;
    char *file_name;
    char *display_name;
    char *display_markup;
    char *page_url;
    char *image_url;
    char *thumb_url;
    char *service_name;
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
SearchItem * search_item_new                (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free SearchItem item.
 *
 * @param[out] item  SearchItem item to free
 * @return     none
 */
void         search_item_free               (SearchItem *item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo id string and int value based on int value.
 *
 * @param[out] item  SearchItem item to set id
 * @param[in]  val   Image id to set
 * @return     none
 */
void         search_item_set_id_uint        (SearchItem    *item,
                                             const uint64_t val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo id string and int value based on string value.
 *
 * @param[out] item  SearchItem item to set id
 * @param[in]  val   Image id to set
 * @return     none
 */
void         search_item_set_id_string      (SearchItem   *item,
                                             const char   *val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo author name.
 *
 * @param[out] item  SearchItem item to set author name
 * @param[in]  val   Author name string to set
 * @return     none
 */
void         search_item_set_author_name    (SearchItem *item,
                                             const char *val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo author url.
 *
 * @param[out] item  SearchItem item to set author url
 * @param[in]  val   Author url string to set
 * @return     none
 */
void         search_item_set_author_url     (SearchItem *item,
                                             const char *val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's file name.
 *
 * @param[out] item  SearchItem item to set file name
 * @param[in]  val   File name string to set
 * @return     none
 */
void         search_item_set_file_name      (SearchItem *item,
                                             const char *val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's display name.
 *
 * @param[out] item  SearchItem item to set display name
 * @param[in]  val   Display name string to set
 * @return     none
 */
void         search_item_set_display_name   (SearchItem *item,
                                             const char *val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's markup.
 *
 * @param[out] item  SearchItem item to set display markup
 * @param[in]  val   Markup string to set
 * @return     none
 */
void         search_item_set_display_markup (SearchItem *item,
                                             const char *val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's page url.
 *
 * @param[out] item  SearchItem item to set page url
 * @param[in]  val   Page url string to set
 * @return     none
 */
void         search_item_set_page_url       (SearchItem *item,
                                             const char *val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's image url.
 *
 * @param[out] item  SearchItem item to set image url
 * @param[in]  val   Image url string to set
 * @return     none
 */
void         search_item_set_image_url      (SearchItem *item,
                                             const char *val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's thumb url.
 *
 * @param[out] item  SearchItem item to set thumb url
 * @param[in]  val   Thumb url string to set
 * @return     none
 */
void         search_item_set_thumb_url      (SearchItem *item,
                                             const char *val);

/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's service name.
 *
 * @param[out] item  SearchItem item to set service name
 * @param[in]  val   Service name string to set
 * @return     none
 */
void         search_item_set_service_name   (SearchItem *item,
                                             const char *val);

/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's width.
 *
 * @param[out] item  SearchItem item to set width
 * @param[in]  val   Width integer value to set
 * @return     none
 */
static inline void search_item_set_width (SearchItem *item,
                                          const int   val) {
    item->width = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's height.
 *
 * @param[out] item  SearchItem item to set height
 * @param[in]  val   Height integer value to set
 * @return     none
 */
static inline void search_item_set_height (SearchItem *item,
                                           const int   val) {
    item->height = val;
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
void search_item_print (SearchItem *item);
#endif
/*----------------------------------------------------------------------------*/
#endif

