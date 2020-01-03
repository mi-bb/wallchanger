/**
 * @file  cfgfile.h
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
 * @brief  Configuration file examine and set
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef CFGFILE_H
#define CFGFILE_H
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * check_config_path_file (int *i_err)
 * @brief  Check if config file and path exists, check read/write permissions. 
 *
 * Checks config path and file existence, creates them if needed. Function
 * returns config file path or null if something is wrong. Function writes
 * checking/creating status to i_err if process completed succefully or not.
 *
 * @param[out]  i_err  Error output
 * @return      Config file path
 *
 * @fn         int set_config_file_path (char *s_path)
 * @brief      Write config path to string
 * @param[out] s_path   String to write config path
 * @return     Result of operation
 */
/*----------------------------------------------------------------------------*/
char * check_config_path_file (int *i_err);

int    set_config_file_path   (char *s_path) __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
#endif

