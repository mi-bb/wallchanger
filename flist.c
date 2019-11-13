/**
 * @file  flist.c
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
 * @brief  File list FList structure and functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "flist.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate string.
 *
 * @param[in]  s_str  String to duplicate
 * @return     Duplicated string or null pointer
 */
static char *
str_dup (const char *s_str)
{
    char *s_res = NULL;
    size_t st_len = 0;

    if (s_str == NULL)
        return NULL;

    st_len = strlen (s_str) + 1;

    s_res = calloc (st_len, sizeof (char));

    if (s_res == NULL)
        return NULL;

    memcpy (s_res, s_str, st_len);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  FList init
 */
void
flist_init (FList *fl_list)
{
    fl_list->i_cnt = 0;
    fl_list->s_file = NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reserve space for items in FList object
 */
int
flist_reserve (FList       *fl_list,
               const size_t i_size)
{
    /* No need to resie */
    if (fl_list->i_cnt == i_size)
        return 0;

    /* if larger free rest */
    while (i_size < fl_list->i_cnt) {
        free (fl_list->s_file[--fl_list->i_cnt]);
    }

    /* If size 0 clear list */
    if (i_size == 0) {
        free (fl_list->s_file);
        fl_list->s_file = NULL;
        return 0;
    }

    if (fl_list->s_file == NULL) {
        fl_list->s_file = calloc (i_size, sizeof (char*));
    }
    else {
        char **s_tmp = realloc (fl_list->s_file, (i_size) * sizeof(char*));
        if (s_tmp == NULL) {
            for (uint32_t i = 0; i < fl_list->i_cnt; ++i)
                free (fl_list->s_file[i]);
            free (fl_list->s_file);
            //return ERR_ALLOC;
        }
        else {
            fl_list->s_file = s_tmp;
        }
    }
    if (i_size != 0 && fl_list->s_file == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
        //return ERR_ALLOC;
    }
    fl_list->i_cnt = i_size;
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clear items in FList object
 */
void
flist_clear (FList *fl_list)
{
    flist_reserve (fl_list, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free FList data
 */
void
flist_free (FList *fl_list)
{
    flist_clear (fl_list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert data to FList
 */
int
flist_insert_data (FList      *fl_list,
                   const char *s_fn)
{
    if (s_fn == NULL)
        return ERR_OK;

    if (flist_reserve (fl_list, fl_list->i_cnt+1))
        return ERR_ALLOC;

    fl_list->s_file[fl_list->i_cnt-1] = str_dup (s_fn);

    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set data on i_pos position in FList
 */
void
flist_set_data (FList          *fl_list,
                const uint32_t  i_pos,
                const char     *s_fn)
{
    /* Position of data to get greater then number of items in list */
    if (i_pos >= fl_list->i_cnt)
        return;

    if (s_fn == NULL)
        return;

    /* Realloc space and copy new string */
    fl_list->s_file[i_pos] = realloc (fl_list->s_file[i_pos],
                                      (strlen (s_fn) + 1) * sizeof(char));
    strcpy (fl_list->s_file[i_pos], s_fn);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Gat data from FList
 */
const char *
flist_get_data (FList          *fl_list,
                const uint32_t  i_pos)
{
    /* Position to insert greater then number of items in list */
    if (i_pos >= fl_list->i_cnt)
        return NULL;

    return (const char *) fl_list->s_file[i_pos];
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Gat data from FList as a new string
 */
char *
flist_get_data2 (FList         *fl_list,
                const uint32_t  i_pos)
{
    char *s_res = NULL; /* Name string to return */

    if (i_pos >= fl_list->i_cnt)
        return NULL;

    s_res = str_dup (fl_list->s_file[i_pos]);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Gat number of items in FList
 */
uint32_t
flist_get_len (FList *fl_list)
{
    return fl_list->i_cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get position of given name in FList
 */
int32_t
flist_get_pos (FList *fl_list,
               const char *s_fn)
{
    int32_t i_pos = -1; /* Item position to return */

    if (s_fn == NULL)
        return -1;

    if (fl_list->i_cnt == 0)
        return -1;

    for (uint32_t i=0; i<fl_list->i_cnt; ++i) {
        if (strcmp (s_fn, flist_get_data (fl_list, i)) == 0) {
            i_pos = i; 
            break;
        }
    }
    return i_pos;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print FList data
 */
void
flist_print_data (FList *fl_list)
{
    for (uint32_t i = 0; i < fl_list->i_cnt; ++i) {
        printf ("%s\n", fl_list->s_file[i]);
    }
}
/*----------------------------------------------------------------------------*/

