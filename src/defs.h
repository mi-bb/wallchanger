/**
 * @file  defs.h
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
 * @brief  Application defines
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef DEFS_H
#define DEFS_H
/*----------------------------------------------------------------------------*/
/**
 * @def   APP_NAME
 * @brief String with application name
 *
 * @def   APP_VERSION
 * @brief String with application version
 *
 * @def   DEFAULT_BG_CMD
 * @brief Default command to set wallpapers
 *
 * @def   DEFAULT_WIN_WIDTH
 * @brief Default settings dialog window width
 *
 * @def   DEFAULT_WIN_HEIGHT
 * @brief Default settings dialog window height
 *
 * @def   DEFAULT_INTERVAL_VAL
 * @brief Default wallpaper change interval
 *
 * @def   DEFAULT_LAST_USED_OPT
 * @brief Default setting for setting last used wallpaper on start
 *
 * @def   DEFAULT_RANDOM_OPT
 * @brief Default setting for random wallpaper change
 */
#define APP_NAME             "Wall Changer"
#define APP_VERSION          "1.3.5"
#define DEFAULT_BG_CMD       "feh --bg-fill"
#define DEFAULT_WIN_WIDTH     1024
#define DEFAULT_WIN_HEIGHT    768
#define DEFAULT_INTERVAL_VAL  30
#define DEFAULT_LAST_USED_OPT 0
#define DEFAULT_RANDOM_OPT    0
/*----------------------------------------------------------------------------*/
#endif

