/**
 * @file  live.h
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
 * @brief  Getting wallpapers from remote live services.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef LIVE_H
#define LIVE_H

#include <stddef.h>
#include "settings/setting_item.h"
#include "util/randomm.h"
#include "live_source.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  size_t live_source_count (void)
 *
 * @brief  Get the number of remote live services.
 *
 * @return Number of services
 *
 * @fn  const LiveSource * live_source_get (const size_t index)
 *
 * @brief  Get a remote live service.
 *
 * @param[in] index  Service position, from 0 to live_source_count() - 1
 * @return    Service or null when index is out of range
 *
 * @fn  const LiveSource * live_source_find (const char *name)
 *
 * @brief  Find a remote live service by its name.
 *
 * @param[in] name  Service name, can be null
 * @return    Service or null when there is none of that name
 */
/*----------------------------------------------------------------------------*/
[[nodiscard]] size_t             live_source_count (void);

[[nodiscard]] const LiveSource * live_source_get   (const size_t  index);

[[nodiscard]] const LiveSource * live_source_find  (const char   *name);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count the remote live services enabled in settings.
 *
 * Names in the settings that are not known services are not counted.
 *
 * @param[in] settings  Program settings
 * @return    Number of enabled services
 */
[[nodiscard]] size_t live_enabled_count (Setting *settings);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a wallpaper from the enabled remote live services.
 *
 * The service is chosen randomly when the random wallpaper option is set,
 * else it is the one after the last used. When it fails the other enabled
 * services are tried. An image already in the cache is not downloaded
 * again, and a daily service is not asked again within an hour.
 *
 * @param[in]     settings  Program settings
 * @param[in,out] rand_mem  Random memory, its range is set to the number of
 *                          enabled services
 * @param[out]    file      Path of the wallpaper file, free it with free()
 * @return        ERR_OK, ERR_LIVE_NOSRC when no service is enabled or
 *                ERR_LIVE_NET when no service gave a wallpaper
 */
[[nodiscard]] int live_wallpaper_get (Setting  *settings,
                                      RandMem  *rand_mem,
                                      char    **file);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read the saved title, author and license of a wallpaper.
 *
 * @param[in]  wall_file  Wallpaper file path
 * @param[out] image      Image to fill, clear it with live_image_clear()
 * @return     false when there is no saved information about the file
 */
[[nodiscard]] bool live_info_read (const char *wall_file,
                                   LiveImage  *image);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the most recently downloaded remote live wallpaper.
 *
 * @return Path of the newest file in the cache or null when it is empty,
 *         free it with free()
 */
[[nodiscard]] char * live_newest_cached (void);
/*----------------------------------------------------------------------------*/
#endif
