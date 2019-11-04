/**
 * @file  settstr.h
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
 * @brief  Wallpaper settings WallSett structure
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef SETTSTR_H
#define SETTSTR_H
#include <gmodule.h>
#include <stdint.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Structore with settings for wallpaper changing
 */
typedef struct
WallSett {
    GSList   *gsl_files;     /**< Background file list */
    char     *s_bgcmd;       /**< Background set command */
    char     *s_lastused;    /**< Last used wallpaper path */
    char     *s_cfgfile;     /**< Path to configuratoin file */
    uint8_t   i_random;      /**< Random background change */
    uint32_t  i_chinterval;  /**< Background change interval */
    uint8_t   i_lastsett;    /**< Remember last used wallpeper */
    uint32_t  i_lastused;    /**< Last used wallpaper index */
    uint64_t  i_hash;        /**< Last data hash */
} WallSett;
/*----------------------------------------------------------------------------*/
#endif

