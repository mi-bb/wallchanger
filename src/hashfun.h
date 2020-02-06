/**
 * @file  hashfun.h
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
 * @brief  Hash function
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef HASHFUN_H
#define HASHFUN_H

#include <stdint.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  djb2 hash function.
 *
 * This is Daniel J. Bernstein's "times 33" hash function posted by him many
 * years ago on comp.lang.c.
 *
 * @param[in]  str  String to count hash
 * @return     Hash value
 */
uint_fast32_t hash (const char * __restrict str)
              __attribute__ ((pure, nonnull (1)));
/*----------------------------------------------------------------------------*/
#endif

