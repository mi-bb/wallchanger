/**
 * @file  randomm.h
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
 * @brief  Random without repeated values
 *
 * Structure and functions for a random number generator without repeating
 * values in a serie of randoming.
 * It gets random numbers until the range of numbers reached, resets memory
 * and starts getting random values again.
 * Maximum available range can be set with RMMAX, it will be RMMAX * 32.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef RANDOMM_H
#define RANDOMM_H

#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define RMMAX 200  /**< RMMAX * 32 maximum random number range */
/*----------------------------------------------------------------------------*/
/**
 * @brief  Random memory struct
 */
typedef struct
RandMem {
    uint32_t ui_rand[RMMAX]; /**< Memory table which numbers were randomed */
    uint32_t ui_range;       /**< Max random number to generate */
    uint32_t ui_cnt;         /**< How many times random was executed */
} RandMem;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clear / reset random memory.
 *
 * @param[out] rm_mem  RandMem object
 * @return     none
 */
void randomm_clear (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init random memory object.
 *
 * @param[out] rm_mem  RandMem object
 * @return     none
 */
void randomm_init (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random numbers maximum range.
 *
 * @param[out] rm_mem  RandMem object
 * @param[in]  ui_rng  Range value
 * @return     none
 */
void randomm_set_range (RandMem *rm_mem, uint32_t ui_rng);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reset random number couts value
 *
 * @param[out] rm_mem  RandMem object
 * @return     none
 */
void randomm_reset_cnt (RandMem *rm_mem);
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
uint32_t randomm_get_number (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
#endif

