/**
 * @file  randomm.c
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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdlib.h>
#include <time.h>
#include "randomm.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if number is in random numbers memory
 *
 * @param[out] rm_mem  RandMem object
 * @param[in]  ui_no   Number to check
 * @return     1 if it is, 0 if it is not
 */
static uint8_t randomm_check_number (RandMem *rm_mem,
                                     uint32_t ui_no);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 *
 * @param[out] rm_mem  RandMem object
 * @param[in]  ui_no   Number to check
 * @return     none
 */
static void randomm_set_number (RandMem *rm_mem,
                                uint32_t ui_no);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Increment random number counts value
 *
 * @param[out] rm_mem  RandMem object
 * @return     none
 */
static void randomm_increment_cnt (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if number is in random numbers memory
 */
static uint8_t
randomm_check_number (RandMem *rm_mem,
                      uint32_t ui_no)
{
    uint8_t  ui_res = 0;          /* Result of number check */
    uint32_t ui_idx = ui_no / 32; /* Number position in array */
    uint32_t ui_pos = ui_no % 32; /* Number bit position in integer */

    if (ui_idx < RMMAX) {
        if (rm_mem->ui_rand[ui_idx] & (1 << ui_pos)) {
            ui_res = 1;
        }
    }
    return ui_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 */
static void
randomm_set_number (RandMem *rm_mem,
                    uint32_t ui_no)
{
    uint32_t ui_idx = ui_no / 32; /* Number position in array */
    uint32_t ui_pos = ui_no % 32; /* Number bit position in integer */

    if (ui_idx < RMMAX) {
        rm_mem->ui_rand[ui_idx] |= (1 << ui_pos);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Increment random number counts value
 */
static void
randomm_increment_cnt (RandMem *rm_mem)
{
    rm_mem->ui_cnt++;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clear / reset random memory.
 */
void
randomm_clear (RandMem *rm_mem)
{
    for (int i = 0; i < RMMAX; ++i)
        rm_mem->ui_rand[i] = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init random memory object.
 */
void
randomm_init (RandMem *rm_mem)
{
    randomm_clear (rm_mem);
    rm_mem->ui_cnt = 0;
    rm_mem->ui_range = 0;
    srand (time (0));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random numbers maximum range.
 */
void
randomm_set_range (RandMem *rm_mem,
                   uint32_t ui_rng)
{
    rm_mem->ui_range = ui_rng;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reset random number couts value
 */
void
randomm_reset_cnt (RandMem *rm_mem)
{
    rm_mem->ui_cnt = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random number
 */
uint32_t
randomm_get_number (RandMem *rm_mem)
{
    uint32_t ui_ret = 0; /* Random number to return */

    if (rm_mem->ui_range == 0)
        return 0;

    /* Get random number and check if it is in memory */
    do {
        ui_ret = (uint32_t) rand () % rm_mem->ui_range;
    }
    while (randomm_check_number (rm_mem, ui_ret));

    /* set number in memory */
    randomm_set_number (rm_mem, ui_ret);

    randomm_increment_cnt (rm_mem);

    if (rm_mem->ui_cnt >= rm_mem->ui_range) {
        randomm_clear (rm_mem);
        randomm_reset_cnt (rm_mem);
    }
    return ui_ret;
}
/*----------------------------------------------------------------------------*/

