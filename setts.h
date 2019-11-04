/**
 * @file  setts.h
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
 * @brief  Settings manage functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef SETTS_H
#define SETTS_H
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "jsfun.h"
#include "settstr.h"
#include "miscfun.h"
/*----------------------------------------------------------------------------*/
int settings_set_last_used_data (WallSett *ws_sett, const char *ch_wallnew);
/*----------------------------------------------------------------------------*/
int settings_set_command (WallSett *ws_sett, const char *ch_cmd);
/*----------------------------------------------------------------------------*/
int settings_init (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int settings_read (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int settings_write (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int settings_update_last_used (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int settings_check_changed (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
#endif
