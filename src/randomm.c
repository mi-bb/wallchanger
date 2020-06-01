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
#include <err.h>
#include "randomm.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if number is in random numbers memory
 *
 * @param[out] rm_mem  RandMem object
 * @param[in]  i_no    Number to check
 * @return     1 if it is, 0 if it is not
 */
static int8_t randomm_check_number (RandMem *rm_mem,
                                    int32_t  i_no);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 *
 * @param[out] rm_mem  RandMem object
 * @param[in]  i_no    Number to check
 * @return     none
 */
static void randomm_set_number (RandMem *rm_mem,
                                int32_t  i_no);
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
static int8_t
randomm_check_number (RandMem *rm_mem,
                      int32_t  i_no)
{
    int8_t  i_res = 0;         /* Result of number check */
    int32_t i_idx = i_no / 32; /* Number position in array */
    int32_t i_pos = i_no % 32; /* Number bit position in integer */

    if (i_idx < RMMAX) {
        if (rm_mem->i_rand[i_idx] & (1 << i_pos)) {
            i_res = 1;
        }
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 */
static void
randomm_set_number (RandMem *rm_mem,
                    int32_t  i_no)
{
    int64_t ui_idx = i_no / 32; /* Number position in array */
    int64_t ui_pos = i_no % 32; /* Number bit position in integer */

    if (ui_idx < RMMAX) {
        rm_mem->i_rand[ui_idx] |= (1 << ui_pos);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Increment random number counts value
 */
static void
randomm_increment_cnt (RandMem *rm_mem)
{
    rm_mem->i_cnt++;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clear / reset random memory.
 */
void
randomm_clear (RandMem *rm_mem)
{
    int i = 0;

    for (i = 0; i < RMMAX; ++i)
        rm_mem->i_rand[i] = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize random memory object.
 */
void
randomm_init (RandMem *rm_mem)
{
    struct tm t_t0   = {0};
    double d_diff    = 0;
    rm_mem->i_cnt    = 0;
    rm_mem->i_range  = 0;
    t_t0.tm_year     = 100;
    t_t0.tm_mday     = 1;

    d_diff = difftime (time (NULL), mktime (&t_t0));

    randomm_clear (rm_mem);
    srand ((unsigned int) d_diff);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RandMem item.
 */
RandMem *
randomm_new (void)
{
    RandMem *rm_mem;

    if ((rm_mem = malloc (sizeof (RandMem))) == NULL)
        err (EXIT_FAILURE, NULL);

    return rm_mem;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free RandMem item.
 */
void
randomm_free (RandMem *rm_mem)
{
    free (rm_mem);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random numbers maximum range.
 */
void
randomm_set_range (RandMem *rm_mem,
                   int32_t  i_rng)
{
    rm_mem->i_range = i_rng;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reset random number couts value
 */
void
randomm_reset_cnt (RandMem *rm_mem)
{
    rm_mem->i_cnt = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random number
 */
uint32_t
randomm_get_number (RandMem *rm_mem)
{
    int32_t ui_ret = 0; /* Random number to return */

    if (rm_mem->i_range == 0)
        return 0;

    /* Get random number and check if it is in memory */
    do {
        ui_ret = (int32_t) rand () % rm_mem->i_range;
    }
    while (randomm_check_number (rm_mem, ui_ret));

    /* set number in memory */
    randomm_set_number (rm_mem, ui_ret);

    randomm_increment_cnt (rm_mem);

    if (rm_mem->i_cnt >= rm_mem->i_range) {
        randomm_clear (rm_mem);
        randomm_reset_cnt (rm_mem);
    }
    return (uint32_t) ui_ret;
}
/*----------------------------------------------------------------------------*/

