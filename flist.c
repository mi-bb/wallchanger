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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errs.h"
#include "flist.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate string.
 *
 * @param[in]  s_str  String to duplicate
 * @return     Duplicated string or null pointer
 */
static char * str_dup (const char *s_str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get file extenstion.
 *
 * @param[in] s_fn String with file path
 * @return    String with file extension
 */
static char * get_file_ext (const char *s_fn);
/*----------------------------------------------------------------------------*/
/**
* @brief  Swap content from one list to another.
*
* @param[in,out] fl_list1  FList object
* @param[in,out] fl_list2  FList object
* @return        none
*/
static void flist_swap_lists (FList *fl_list1,
                              FList *fl_list2);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate string.
 */
static char *
str_dup (const char *s_str)
{
    char   *s_res  = NULL;
    size_t  st_len = 0;

    if (s_str == NULL)
        return NULL;

    st_len = strlen (s_str);

    s_res = calloc (st_len+1, sizeof (char));

    if (s_res == NULL)
        return NULL;

    memcpy (s_res, s_str, st_len);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get file extenstion.
 */
static char *
get_file_ext (const char *s_fn)
{
    char *s_ext = NULL; /* Extension string */
    char *s_p   = NULL; /* Pointer to first right . */

    s_p = strrchr (s_fn, '.');
    if (s_p != NULL) {
        s_ext = s_p+1;
        /* s_p++; */
        /* s_ext = str_dup (s_p); */
    }
    return s_ext;
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
flist_reserve (FList        *fl_list,
               const size_t  i_size)
{
    char     **s_tmp = NULL;
    uint32_t   i     = 0;

    /* No need to resize */
    if (fl_list->i_cnt == i_size)
        return ERR_OK;

    /* if larger free rest */
    while (i_size < fl_list->i_cnt) {
        free (fl_list->s_file[--fl_list->i_cnt]);
    }

    /* If size 0 clear list */
    if (i_size == 0) {
        free (fl_list->s_file);
        flist_init (fl_list);
        return ERR_OK; 
    }

    /* Malloc if null, realloc if not null */
    if (fl_list->s_file == NULL) {
        fl_list->s_file = calloc (i_size, sizeof (char*));
    }
    else {
        s_tmp = realloc (fl_list->s_file, (i_size) * sizeof (char*));
        if (s_tmp == NULL) {
            for (i = 0; i < fl_list->i_cnt; ++i)
                free (fl_list->s_file[i]);
            free (fl_list->s_file);
            /*return ERR_ALLOC;*/
        }
        else {
            fl_list->s_file = s_tmp;
        }
    }

    if (fl_list->s_file == NULL && i_size != 0) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
        /*return ERR_ALLOC;*/
    }

    /* Update file list count */
    fl_list->i_cnt = (uint32_t) i_size;

    return ERR_OK;
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

    /* Resize file list by one */
    if (flist_reserve (fl_list, fl_list->i_cnt+1))
        return ERR_ALLOC;

    /* Set last item with s_fn copy */
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

    /* Realloc space */
    fl_list->s_file[i_pos] = realloc (fl_list->s_file[i_pos],
                                      (strlen (s_fn) + 1) * sizeof(char));

    /* Set file name on i_pos position with s_fn */
    strcpy (fl_list->s_file[i_pos], s_fn);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Gat data from FList
 */
const char *
flist_get_data (const FList    *fl_list,
                const uint32_t  i_pos)
{
    if (i_pos >= fl_list->i_cnt)
        return NULL;

    /* Return pointer to file name */
    return (const char *) fl_list->s_file[i_pos];
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Gat data from FList as a new string
 */
char *
flist_get_data_new (const FList    *fl_list,
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
flist_get_len (const FList *fl_list)
{
    return fl_list->i_cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get position of given name in FList
 */
int32_t
flist_get_pos (const FList *fl_list,
               const char  *s_fn)
{
    int32_t  i_pos = -1; /* Item position to return */
    uint32_t i     =  0;

    if (s_fn == NULL)
        return -1;

    if (fl_list->i_cnt == 0)
        return -1;

    for (i = 0; i < fl_list->i_cnt; ++i) {
        if (strcmp (s_fn, flist_get_data (fl_list, i)) == 0) {
            i_pos = (int32_t) i; 
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
flist_print_data (const FList *fl_list)
{
    uint32_t i = 0;

    for (i = 0; i < fl_list->i_cnt; ++i) {
        printf ("%s\n", fl_list->s_file[i]);
    }
}
/*----------------------------------------------------------------------------*/
/**
* @brief  Swap content from one list to another.
*/
static void
flist_swap_lists (FList *fl_list1,
                  FList *fl_list2)
{
    char     **s_tmp   = fl_list1->s_file;
    uint32_t   ui_tmp  = fl_list1->i_cnt;

    fl_list1->s_file = fl_list2->s_file;
    fl_list1->i_cnt  = fl_list2->i_cnt;
    fl_list2->s_file = s_tmp;
    fl_list2->i_cnt  = ui_tmp;
}
/*----------------------------------------------------------------------------*/
/**
* @brief  Remove duplicates from list.
*/
void
flist_remove_duplicates (FList *fl_list)
{
    FList       fl_filtered;   /* Temporary list without duplicates */
    const char *s_val  = NULL; /* Name from list */
    uint32_t    ui_len = 0;    /* List length */
    uint32_t    i      = 0;

    flist_init (&fl_filtered);

    ui_len = flist_get_len (fl_list);

    for (i = 0; i < ui_len; ++i) {
        /* Get value from unfiltered list */
        s_val = flist_get_data (fl_list, i);

        /* If it is not on filtered list add it there */
        if (flist_get_pos (&fl_filtered, s_val) == -1) {
            flist_insert_data (&fl_filtered, s_val);
        }
    }
    /* Swap filtered and unfiltered and free the old one */
    flist_swap_lists (fl_list, &fl_filtered);

    /* Free the temporary list */
    flist_free (&fl_filtered);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove from file list files that are not on provided extensions
 *         list.
 */
void
flist_filter_by_extensions_list (FList *fl_files,
                                 FList *fl_exts)
{
    const char *s_fn   = NULL; /* File name */
    char       *s_ext  = NULL; /* Pointer to ext in s_fn */
    uint32_t    ui_cnt = 0;    /* Length of file list */
    uint32_t    i      = 0;
    FList       fl_new;        /* Temporary filtered file list */

    flist_init (&fl_new);

    /* Get file list length */
    ui_cnt = flist_get_len (fl_files);

    /* Iterate over file list to filter */
    for (i = 0; i < ui_cnt; ++i) {
        /* Get file name from list */
        s_fn = flist_get_data (fl_files, i);

        /* Get file extension */
        s_ext = get_file_ext (s_fn);

        if (s_ext != NULL) {
            /* Check if file extension list add it to new list if it is */
            if (flist_get_pos (fl_exts, s_ext) >= 0) {
                flist_insert_data (&fl_new, s_fn);
            }
        }
    }
    /* Swap content of old file list and filetered one */
    flist_swap_lists (fl_files, &fl_new);

    /* Free the temporary list */
    flist_free (&fl_new);
}
/*----------------------------------------------------------------------------*/

