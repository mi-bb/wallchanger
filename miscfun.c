/**
 * @file  miscfun.c
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
 * @brief  Other functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "miscfun.h"
// -----------------------------------------------------------------------------
/**
 * @brief  Hash function.
 *
 * @param[in]  str  String to count hash
 * @return     Hash value
 */
uint64_t
hash(const char *str)
{
    uint64_t hash = 5381;
    int         c = 0;
    if (str == NULL)
        return 0;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file.
 *
 * @param[in]  s_fname File name
 * @param[out] s_buff  Pointer to destination buffer
 * @return     Reading status
 * @retval     0  OK
 * @retval     1  File error
 * @retval     2  Reading error
 */
int
read_file_data (const char  *s_fname,
                char       **s_buff)
{
    FILE  *f_file;
    long   l_size;
    size_t st_res;

    f_file = fopen (s_fname, "rb");
    //if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
    if (f_file == NULL) {
        fputs ("File error\n", stderr);
        return 1;
    }
    // get file size:
    fseek (f_file , 0 , SEEK_END);
    l_size = ftell (f_file);
    rewind (f_file);

    if (l_size == 0) {
        *s_buff = NULL;
        fclose (f_file);
        return 0;
    }

    // allocate memory to contain the whole file:
    *s_buff = g_malloc0_n (l_size+1, sizeof (char));

    // copy the file into the buffer:
    st_res = fread (*s_buff, 1, l_size, f_file);
    if (st_res != (size_t) l_size) {
        fputs ("File reading error\n", stderr);
        return 2;
    }
    // terminate
    fclose (f_file);
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file and count hash.
 *
 * @param[in]  s_fname File name
 * @param[out] s_buff  Pointer to destination buffer
 * @param[out] i_hash  Pointer to hash
 * @return     Reading status
 * @retval     0  OK
 * @retval     1  File error
 * @retval     2  Reading error
 */
int read_file_data_hash (const char  *s_fname,
                         char       **s_buff,
                         uint64_t    *i_hash)
{
    int i_res = 0;

    i_res = read_file_data (s_fname, s_buff);
    if (i_res != 0) {
        *i_hash = 0;
        return i_res;
    }
    *i_hash = hash (*s_buff);
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save some data to file.
 *
 * @param[in]  s_fname File name
 * @param[in]  s_buff  Buffer with data
 * @return     Writting status
 * @retval     0  OK
 * @retval     1  File error
 * @retval     2  Writting error
 */
int
save_file_data (const char *s_fname,
                const char *s_buff)
{
    FILE   *f_file;
    size_t  st_res;
    size_t  st_size;

    f_file = fopen (s_fname, "wb");
    if (f_file == NULL) {
        fputs ("File error\n", stderr);
        return 1;
    }
    st_size = strlen(s_buff);
    st_res = fwrite (s_buff , sizeof(char), st_size, f_file);
    fclose (f_file);
    if (st_res != st_size) {
        fputs ("File writting error\n", stderr);
        return 2;
        }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  String compare function for getting string position function.
 *
 * @param[in]  a  String a 
 * @param[in]  b  String b
 * @return     Compare result
 */
int
compare_strings (const char *a,
                 const char *b)
{
    int i_res = 0;

    if (a == NULL || b == NULL) {
        if (a == NULL && b == NULL)
            i_res = 0;
        else {
            if (a == NULL)
                i_res = -1;
            else
                i_res = 1;
        }
    }
    else {
        i_res = g_utf8_collate (a, b);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string position in GSList. 
 *
 * @param[in]  gsl_list  List where should be the very string 
 * @param[in]  s_what   String which position is unknown
 * @return     String position or -1 if it was not found
 */
int
my_gslist_get_position (GSList     *gsl_list,
                        const char *s_what)
{
    GSList *gsl_fnd = NULL;

    if (gsl_list == NULL)
        return -1;
    if (s_what == NULL)
        return -1;

    gsl_fnd = g_slist_find_custom (gsl_list,
                                   (void *) s_what,
                                   (GCompareFunc) compare_strings);
    if (gsl_fnd == NULL) {
        return -1;
    }
    return g_slist_position (gsl_list, gsl_fnd);
}
/*----------------------------------------------------------------------------*/

