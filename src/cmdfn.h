/**
 * @file  cmdfn.h
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
 * @brief  Command line parsing functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef CMDFN_H
#define CMDFN_H
/*----------------------------------------------------------------------------*/
/**
 * @var   CMD_OPT_NONE
 * @brief No option set
 *
 * @var   CMD_OPT_START
 * @brief Start option given
 *
 * @var   CMD_OPT_STOP
 * @brief Stop option given
 *
 * @var   CMD_OPT_RESTART
 * @brief Restart option given
 *
 * @var   CMD_OPT_STATUS
 * @brief Status option given
 *
 * @var   CMD_OPT_CONFIG
 * @brief Config path given
 *
 * @var   CMD_OPT_ONCE
 * @brief Run once and exit option given
 */
constexpr int CMD_OPT_NONE    = 0x00;
constexpr int CMD_OPT_START   = 0x01;
constexpr int CMD_OPT_STOP    = 0x02;
constexpr int CMD_OPT_RESTART = 0x04;
constexpr int CMD_OPT_STATUS  = 0x08;
constexpr int CMD_OPT_CONFIG  = 0x10;
constexpr int CMD_OPT_ONCE    = 0x20;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse command line options, set i_opt and s_cfgpath.
 *
 * @param[in] argc       Arguments passed to the program from the environment
 *                       in which the program is run
 * @param[in] argv       Pointer to the first element of an array of pointers
 *                       that represent the arguments passed to the program
 * @param[out] i_opt     Set here command line options
 * @param[out] s_cfgpath Copy here config path if given in options
 * @return     none
 */
void cmdfn_parse (int    argc,
                  char **argv,
                  int   *i_opt,
                  char **s_cfgpath);
/*----------------------------------------------------------------------------*/
#endif

