/**
 * @file  randomm.c
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
 * @brief  Random without repeated values
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <time.h>
#include "randomm.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if number is in random numbers memory
 *
 * @param[out] rmm    RandMem object
 * @param[in]  ui_no  Number to check
 * @return     1 if it is, 0 if it is not
 */
static int  randomm_check_number (RandMem *rmm,
                                  size_t   ui_no);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 *
 * @param[out] rm_mem  RandMem object
 * @param[in]  ui_no   Number to check
 * @return     none
 */
static void randomm_set_number   (RandMem *rm_mem,
                                  size_t   ui_no);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize random memory object.
 *
 * @param[out] rm_mem  RandMem object
 * @return     none
 */
static void randomm_init         (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Advance and return this object's own pseudo-random generator.
 *
 * Each RandMem carries its own xorshift32 state (RandMem::seed) instead of
 * relying on libc's process-global rand()/srand(). That keeps instances
 * independent of one another (and of any other code in the process calling
 * rand()/srand()), which a shared global generator cannot guarantee.
 *
 * @param[out] rm_mem  RandMem object
 * @return     Next pseudo-random 32-bit value
 */
static uint32_t randomm_next     (RandMem *rm_mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if number is in random numbers memory
 */
static int
randomm_check_number (RandMem *rmm,
                      size_t   ui_no)
{
    size_t i_idx = ui_no / 32; /* Number position in array */
    size_t i_pos = ui_no % 32; /* Number bit position in integer */

    return (i_idx < rmm->allocn) && (rmm->randm[i_idx] & (1u << i_pos)) ? 1 : 0;
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
        rm_mem->randm[ui_idx] |= (1u << ui_pos);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize random memory object.
 */
static void
randomm_init (RandMem *rm_mem)
{
    /* Disambiguates RandMem objects created within the same clock tick. */
    static unsigned int ui_counter = 0;

    rm_mem->randm  = nullptr;
    rm_mem->cnt    = 0;
    rm_mem->allocn = 0;
    rm_mem->range  = 0;

    rm_mem->seed = (uint32_t) time (nullptr)
                 ^ (uint32_t) (uintptr_t) rm_mem
                 ^ ++ui_counter
                 ^ 0x9E3779B9u;

    /* xorshift32 needs a non-zero state to ever produce non-zero output. */
    if (rm_mem->seed == 0)
        rm_mem->seed = 0x9E3779B9u;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Advance and return this object's own pseudo-random generator.
 */
static uint32_t
randomm_next (RandMem *rm_mem)
{
    uint32_t ui_x = rm_mem->seed;

    ui_x ^= ui_x << 13;
    ui_x ^= ui_x >> 17;
    ui_x ^= ui_x << 5;

    return rm_mem->seed = ui_x;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reset random memory.
 */
void
randomm_reset (RandMem *rm_mem)
{
    size_t i = 0;

    rm_mem->cnt = rm_mem->range;

    for (i = 0; i < rm_mem->allocn; ++i)
        rm_mem->randm[i] = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random numbers range.
 */
void
randomm_set_range (RandMem *rm_mem,
                   size_t   ui_rng) {

    rm_mem->allocn = ui_rng / (8 * sizeof (*rm_mem->randm)) + 1;

    rm_mem->range = ui_rng;

    if (rm_mem->randm == nullptr) {
        rm_mem->randm = malloc (rm_mem->allocn * sizeof (*rm_mem->randm));

        if (rm_mem->randm == nullptr)
            exit (EXIT_FAILURE);
    }
    else {
        uint32_t *pui_new = realloc (rm_mem->randm,
                                     rm_mem->allocn * sizeof (*rm_mem->randm));

        if (pui_new == nullptr) {
            free (rm_mem->randm);
            exit (EXIT_FAILURE);
        }
        rm_mem->randm = pui_new;
    }

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

    if ((rm_mem = malloc (sizeof (RandMem))) == nullptr)
        exit (EXIT_FAILURE);

    randomm_init (rm_mem);

    return rm_mem;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RandMem item with given range.
 */
RandMem *
randomm_new_with_range (size_t ui_range)
{
    RandMem *rm_mem = randomm_new ();

    randomm_set_range (rm_mem, ui_range);

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
    size_t ui_rnd = 0; /* Randomed number position to get */
    size_t      i = 0; /* i */
    size_t      j = 0; /* j */

    if (rm_mem->range == 0)
        return 0;

    if (rm_mem->cnt == 0) {
        randomm_reset (rm_mem);
    }
    ui_rnd = (size_t) randomm_next (rm_mem) % rm_mem->cnt + 1;

    for (i = 0; i < rm_mem->range; ++i) {
        if (!randomm_check_number (rm_mem, i)) {
            if (++j == ui_rnd) {
                ui_ret = i;
                break;
            }
        }
    }
    --rm_mem->cnt;
    /* Set number in memory */
    randomm_set_number (rm_mem, ui_ret);
    return ui_ret;
}
/*----------------------------------------------------------------------------*/
