/**
 * @file  live_bing.h
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
 * @brief  Bing image of the day as a remote live wallpaper.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef LIVE_BING_H
#define LIVE_BING_H

#include "live_source.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Bing image of the day service.
 */
extern const LiveSource live_bing;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the size suffix of the Bing image that fits the screen best.
 *
 * @param[in] screen_width   Screen width
 * @param[in] screen_height  Screen height
 * @return    "1366x768", "1920x1080" or "UHD"
 */
[[nodiscard]] const char * live_bing_size (const int screen_width,
                                           const int screen_height);
/*----------------------------------------------------------------------------*/
#endif
