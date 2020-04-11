/**
 * @file  cres.h
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
 * @brief  Function to create / resize dynamic array
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef CRES_H
#define CRES_H

#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create or resize dynamic array
 *
 * @param[out] v_ptr    Pointer to data to create or resize
 * @param[in]  ul_num   Number of elements to allocate
 * @param[in]  ul_size  Size of each element
 * @return     None
 */
void cres (void         **v_ptr,
           const size_t   ul_num, 
           const size_t   ul_size);
/*----------------------------------------------------------------------------*/
#endif

