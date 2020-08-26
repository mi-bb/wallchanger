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
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include "randomm.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if number is in random numbers memory
 *
 * @param[out] rmm  RandMem object
 * @param[in]  i_no    Number to check
 * @return     1 if it is, 0 if it is not
 */
static int8_t randomm_check_number (RandMem *rmm,
                                    size_t  ui_no);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 *
 * @param[out] rm_mem  RandMem object
 * @param[in]  i_no    Number to check
 * @return     none
 */
static void randomm_set_number (RandMem *rm_mem,
                                size_t  ui_no);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize random memory object.
 *
 * @param[out] rm_mem  RandMem object
 * @return     none
 */
static void randomm_init (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if number is in random numbers memory
 */
static int8_t
randomm_check_number (RandMem *rmm,
                      size_t   ui_no)
{
    size_t i_idx = ui_no / 32; /* Number position in array */
    size_t i_pos = ui_no % 32; /* Number bit position in integer */

    return (i_idx < rmm->allocn) && (rmm->randm[i_idx] & (1 << i_pos)) ? 1 : 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 */
static void
randomm_set_number (RandMem *rm_mem,
                    size_t   ui_no)
{
    size_t ui_idx = ui_no / 32; /* Number position in array */
    size_t ui_pos = ui_no % 32; /* Number bit position in integer */

    if (ui_idx < rm_mem->allocn) {
        rm_mem->randm[ui_idx] |= (1 << ui_pos);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize random memory object.
 */
static void
randomm_init (RandMem *rm_mem)
{
    struct tm t_t0 = {0};
    double d_diff  = 0;
    rm_mem->randm  = NULL;
    rm_mem->cnt    = 0;
    rm_mem->allocn = 0;
    rm_mem->range  = 0;
    t_t0.tm_year   = 100;
    t_t0.tm_mday   = 1;

    d_diff = difftime (time (NULL), mktime (&t_t0));

    srand ((unsigned int) d_diff);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reset random memory.
 */
void
randomm_reset (RandMem *rm_mem)
{
    size_t i = 0;

    rm_mem->cnt = 0;

    for (i = 0; i < rm_mem->allocn; ++i)
        rm_mem->randm[i] = 0;
}
/*----------------------------------------------------------------------------*/
void
randomm_set_range (RandMem *rm_mem,
                   size_t   ui_rng) {

    rm_mem->allocn = ui_rng / (8 * sizeof (*rm_mem->randm)) + 1;

    rm_mem->range = ui_rng;

    if (rm_mem->randm == NULL) {
        rm_mem->randm = malloc (rm_mem->allocn * sizeof (*rm_mem->randm));
    }
    else {
        rm_mem->randm = realloc (rm_mem->randm,
                                 rm_mem->allocn * sizeof (*rm_mem->randm));
    }
    if (rm_mem->randm == NULL)
        err (EXIT_FAILURE, NULL);

    randomm_reset (rm_mem);
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

    randomm_init (rm_mem);

    return rm_mem;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free RandMem item.
 */
void
randomm_free (RandMem *rm_mem)
{
    free (rm_mem->randm);
    free (rm_mem);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random number
 */
size_t
randomm_get_number (RandMem *rm_mem)
{
    size_t ui_ret = 0; /* Random number to return */

    if (rm_mem->range == 0)
        return 0;

    /* Get random number and check if it is in memory */
    do {
        ui_ret = (size_t) rand () % rm_mem->range;
    }
    while (randomm_check_number (rm_mem, ui_ret));

    /* set number in memory */
    randomm_set_number (rm_mem, ui_ret);

    ++rm_mem->cnt;

    if (rm_mem->cnt >= rm_mem->range) {
        randomm_reset (rm_mem);
    }
    return ui_ret;
}
/*----------------------------------------------------------------------------*/

