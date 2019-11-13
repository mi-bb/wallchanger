/**
 * @file  setts.c
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
#include "setts.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read program settings. 
 */
int
settings_read (WallSett *ws_sett) 
{
    return js_settings_read (ws_sett, settings_get_cfg_fn (ws_sett));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save program settings. 
 */
int
settings_write (WallSett *ws_sett)
{
    return js_settings_write (ws_sett, settings_get_cfg_fn (ws_sett));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in program settings. 
 */
int
settings_update_last_used (WallSett *ws_sett)
{
    return js_settings_update_last_used (ws_sett,
                                         settings_get_cfg_fn (ws_sett));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are different that saved ones.
 */
int
settings_check_changed (WallSett *ws_sett,
                        uint8_t  *i_changed)
{
    return js_settings_check_changed (ws_sett, i_changed);
}
/*----------------------------------------------------------------------------*/

