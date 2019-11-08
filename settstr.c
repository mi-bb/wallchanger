/**
* @file  settstr.c
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
* @brief  Wallpaper settings WallSett structure and functions
* 
* @author Michał Bąbik <michalb1981@o2.pl>
*/
#include "settstr.h"

/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[out]  ws_sett  Pointer to WallSett with all settings
 * @return      none
 */
void
free_wall_sett (WallSett *ws_sett)
{
    g_slist_free_full (ws_sett->gsl_files, g_free);
    g_free (ws_sett->s_bgcmd);
    g_free (ws_sett->s_cfgfile);
    g_free (ws_sett->s_lastused);
}
/*----------------------------------------------------------------------------*/
