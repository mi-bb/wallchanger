/**
 * @file  dmfn.h
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
 * @brief  Daemon related functions
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef DMFN_H
#define DMFN_H
/*----------------------------------------------------------------------------*/
/**
 * @brief  Daemonize current process.
 *
 * @return none
 */
void dmfn_daemonize (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd runs in background.
 *
 * @return 1 if it is running 0 if not.
 */
int dmfn_check_presence (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd runs in background exit if it is.
 *
 * @return none
 */
void dmfn_check_exit (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print status of wchangerd daemon.
 *
 * @return none
 */
void dmfn_print_status (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Start wchangerd process
 *
 * @return none
 */
void dmfn_start (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill wchangerd process
 *
 * @return Non 0 if daemon was killed, 0 if not found
 */
int dmfn_kill (void);
/*----------------------------------------------------------------------------*/
#endif
