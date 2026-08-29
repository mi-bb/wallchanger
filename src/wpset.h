/**
 * @file  wpset.h
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
 * @brief  Wallpaper setting functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WPSET_H
#define WPSET_H
#include "randomm.h"
#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  int wpset_change (Setting    *st_settings,
 *                        RandMem    *rm_rand,
 *                        const char *s_cfg_file)
 *
 * @brief  Wallpaper change during program work.
 *
 * @param[in,out] st_settings  Program settings
 * @param[in,out] rm_rand      Raddom memory structure
 * @param[in]     s_cfg_file   Config file path
 * @return        Wallpaper change status
 *
 * @fn  int wpset_startup_set (Setting    *st_settings,
 *                             RandMem    *rm_rand,
 *                             const char *s_cfg_file)
 *
 * @brief  Setting wallpaper image at program startup.
 *
 * @param[in,out] st_settings  Program settings
 * @param[in,out] rm_rand      Raddom memory structure
 * @param[in]     s_cfg_file   Config file path
 * @return        Wallpaper set status
 *
 * @fn  void wallpaper_test_set (const char *s_cmd,
 *                               const char *s_file)
 *
 * @brief  Setting wallpaper with configuration dialog.
 *
 * @param[in]  s_cmd       Wallpaper set command
 * @param[in]  s_file      Wallpaper file path
 * @return     none
 *
 * @fn  int wallpaper_dialog_set (const char *s_cmd,
 *                                const char *s_file,
 *                                const char *s_cfg_file)
 *
 * @brief  Setting wallpaper with settings dialog.
 *
 * @param[in]  s_cmd       Wallpaper set command
 * @param[in]  s_file      Wallpaper file path
 * @param[in]  s_cfg_file  Config file path
 * @return     Wallpaper set status
 */
/*----------------------------------------------------------------------------*/
int  wpset_change         (Setting    *st_settings,
                           RandMem    *rm_rand,
                           const char *s_cfg_file);

int  wpset_startup_set    (Setting    *st_settings,
                           RandMem    *rm_rand,
                           const char *s_cfg_file);

void wallpaper_test_set   (const char *s_cmd,
                           const char *s_file);

int  wallpaper_dialog_set (const char *s_cmd,
                           const char *s_file,
                           const char *s_cfg_file)
                           __attribute__ ((nonnull (1, 2, 3)));
/*----------------------------------------------------------------------------*/
#endif

