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
 * @param[out] mem     RandMem object
 * @param[in]  number  Number to check
 * @return     1 if it is, 0 if it is not
 */
static int  randomm_check_number (RandMem *mem,
                                  size_t   number);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 *
 * @param[out] mem     RandMem object
 * @param[in]  number  Number to check
 * @return     none
 */
static void randomm_set_number   (RandMem *mem,
                                  size_t   number);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize random memory object.
 *
 * @param[out] mem  RandMem object
 * @return     none
 */
static void randomm_init         (RandMem *mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Advance and return this object's own pseudo-random generator.
 *
 * Each RandMem carries its own xorshift32 state (RandMem::seed) instead of
 * relying on libc's process-global rand()/srand(). That keeps instances
 * independent of one another (and of any other code in the process calling
 * rand()/srand()), which a shared global generator cannot guarantee.
 *
 * @param[out] mem  RandMem object
 * @return     Next pseudo-random 32-bit value
 */
static uint32_t randomm_next     (RandMem *mem);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if number is in random numbers memory
 */
static int
randomm_check_number (RandMem *mem,
                      size_t   number)
{
    size_t word = number / 32; /* Number position in array */
    size_t bit  = number % 32; /* Number bit position in integer */

    return (word < mem->allocn) && (mem->randm[word] & (1u << bit)) ? 1 : 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Store number in random numbers memory
 */
static void
randomm_set_number (RandMem *mem,
                    size_t   number)
{
    size_t word = number / 32;  /* Number position in array */
    size_t bit  = number % 32;  /* Number bit position in integer */

    if (word < mem->allocn) {
        mem->randm[word] |= (1u << bit);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize random memory object.
 */
static void
randomm_init (RandMem *mem)
{
    /* Disambiguates RandMem objects created within the same clock tick. */
    static unsigned int counter = 0;

    mem->randm  = nullptr;
    mem->cnt    = 0;
    mem->allocn = 0;
    mem->range  = 0;

    mem->seed = (uint32_t) time (nullptr)
              ^ (uint32_t) (uintptr_t) mem
              ^ ++counter
              ^ 0x9E3779B9u;

    /* xorshift32 needs a non-zero state to ever produce non-zero output. */
    if (mem->seed == 0)
        mem->seed = 0x9E3779B9u;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Advance and return this object's own pseudo-random generator.
 */
static uint32_t
randomm_next (RandMem *mem)
{
    uint32_t x = mem->seed;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return mem->seed = x;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reset random memory.
 */
void
randomm_reset (RandMem *mem)
{
    size_t i = 0;

    mem->cnt = mem->range;

    for (i = 0; i < mem->allocn; ++i)
        mem->randm[i] = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random numbers range.
 */
void
randomm_set_range (RandMem *mem,
                   size_t   range) {

    mem->allocn = range / (8 * sizeof (*mem->randm)) + 1;

    mem->range = range;

    if (mem->randm == nullptr) {
        mem->randm = malloc (mem->allocn * sizeof (*mem->randm));

        if (mem->randm == nullptr)
            exit (EXIT_FAILURE);
    }
    else {
        uint32_t *resized = realloc (mem->randm,
                                     mem->allocn * sizeof (*mem->randm));

        if (resized == nullptr) {
            free (mem->randm);
            exit (EXIT_FAILURE);
        }
        mem->randm = resized;
    }

    randomm_reset (mem);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RandMem item.
 */
RandMem *
randomm_new (void)
{
    RandMem *mem;

    if ((mem = malloc (sizeof (RandMem))) == nullptr)
        exit (EXIT_FAILURE);

    randomm_init (mem);

    return mem;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RandMem item with given range.
 */
RandMem *
randomm_new_with_range (size_t range)
{
    RandMem *mem = randomm_new ();

    randomm_set_range (mem, range);

    return mem;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free RandMem item.
 */
void
randomm_free (RandMem *mem)
{
    free (mem->randm);
    free (mem);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random number
 */
size_t
randomm_get_number (RandMem *mem)
{
    size_t result = 0; /* Random number to return */
    size_t pick   = 0; /* Randomed number position to get */
    size_t      i = 0; /* i */
    size_t      j = 0; /* j */

    if (mem->range == 0)
        return 0;

    if (mem->cnt == 0) {
        randomm_reset (mem);
    }
    pick = (size_t) randomm_next (mem) % mem->cnt + 1;

    for (i = 0; i < mem->range; ++i) {
        if (!randomm_check_number (mem, i)) {
            if (++j == pick) {
                result = i;
                break;
            }
        }
    }
    --mem->cnt;
    /* Set number in memory */
    randomm_set_number (mem, result);
    return result;
}
/*----------------------------------------------------------------------------*/
