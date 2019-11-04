/**
 * @file  wallset.h
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
 * @brief  Wallpaper setting functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef WALLSET_H
#define WALLSET_H
#include <stdio.h>
#include <gmodule.h>
#include "settstr.h"
#include "setts.h"
/*----------------------------------------------------------------------------*/
int wallpaper_change (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int wallpaper_startup_set (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int wallpaper_dialog_set (WallSett *ws_sett, const char *s_file);
/*----------------------------------------------------------------------------*/
#endif

