/**
 * @file  randomm.h
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
 * @brief  Random without repeated values
 *
 * Structure and functions for a random number generator without repeating
 * values in a serie of randoming.
 * It gets random numbers until the range of numbers reached, resets memory
 * and starts getting random values again.
 * Maximum available range can be set with RMMAX, it will be RMMAX * 32.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef RANDOMM_H
#define RANDOMM_H

#include <stdint.h>
#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @struct RandMem
 *
 * @brief  Random memory struct
 *
 * @var   RandMem::randm
 * @brief Integers to store information which number was randomed
 *
 * @var   RandMem::allocn
 * @brief Number of alloced integers
 *
 * @var   RandMem::range
 * @brief Max random number that will be generated
 *
 * @var   RandMem::cnt
 * @brief Number of random number generated in a serie
 */
typedef struct
RandMem {
    uint32_t *randm;
    size_t    allocn;
    size_t    range;
    size_t    cnt;
} RandMem;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reset random memory.
 *
 * @param[out] rm_mem  RandMem object
 * @return     none
 */
void      randomm_reset          (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RandMem item.
 *
 * @return New RandMem item
 */
RandMem * randomm_new            (void) __attribute__((malloc));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RandMem item with given range.
 *
 * @param[in] ui_range  Random numbers range
 * @return    New RandMem item
 */
RandMem * randomm_new_with_range (size_t ui_range) __attribute__((malloc));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free RandMem item.
 *
 * @param[out] rm_mem  RandMem object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void      randomm_free           (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random numbers maximum range.
 *
 * @param[out] rm_mem  RandMem object
 * @param[in]  ui_rng  Range value
 * @return     none
 */
void      randomm_set_range      (RandMem *rm_mem,
                                  size_t   ui_rng);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random number
 *
 * Function gets random number using standard rand function, later checks if
 * this number was already returned in this series of "randoms".
 * If number was previously returned it runs random again untit it gets a fresh
 * value.
 * When count of returned random numbers reaches the range that was previously
 * set it resets the memory of returned numbers, sets number counter to zero
 * and starts getting random numbers from the beginning.
 *
 * @param[out] rm_mem  RandMem object
 * @return     Ranom number
 */
size_t    randomm_get_number     (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
#endif

