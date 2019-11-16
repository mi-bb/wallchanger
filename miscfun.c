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
 * @brief  Get file extenstion.
 */
char *
get_file_ext (const char *s_fn)
{
    char *s_ext = NULL; /* Extension string */
    char *s_p   = NULL; /* Pointer to first right . */

    s_p = strrchr (s_fn, '.');
    if (s_p != NULL) {
        s_ext = s_p+1;
        //s_p++;
        //s_ext = str_dup (s_p);
    }
    return s_ext;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create or resize dynamic array
 */
int
create_resize (void        **v_ptr,
               const size_t  num,
               const size_t  size)
{
    void *s_tmp = NULL; /* Temp pointer for realloc */

    if (size == 0 || num == 0) {
        if (*v_ptr != NULL) {
            free (*v_ptr);
            *v_ptr = NULL;
        }
        return ERR_OK;
    }
    else {
        if (*v_ptr == NULL) {
            *v_ptr = calloc (num, size);
            if (*v_ptr == NULL) {
                fputs ("Alloc error\n", stderr);
                //return ERR_ALLOC;
                exit (EXIT_FAILURE);
            }
        }
        else {
            s_tmp = realloc (*v_ptr, num * size);
            if (s_tmp == NULL) {
                free (*v_ptr);
                fputs ("Alloc error\n", stderr);
                //return ERR_ALLOC;
                exit (EXIT_FAILURE);
            }
            else {
            *v_ptr = s_tmp;
            }
        }
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate n bytes of string.
 */
char *
str_ndup (const char *s_str,
          size_t      st_len)
{
    char *s_res = NULL;

    if (s_str == NULL)
        return NULL;

    create_resize ((void**) &s_res, st_len+1, sizeof (char));
    //s_res = calloc (st_len, sizeof (char));
    //if (s_res == NULL)
    //    return NULL;

    //strcpy (s_res, s_str);
    memcpy (s_res, s_str, st_len);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate string.
 */
char *
str_dup (const char *s_str)
{
    return str_ndup (s_str, strlen (s_str));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file.
 */
int
read_file_data (const char  *s_fname,
                char       **s_buff)
{
    FILE  *f_file;     /* Data file */
    long   l_size = 0; /* File size */
    size_t st_res = 0; /* Read data count */

    f_file = fopen (s_fname, "rb");
    if (f_file == NULL) {
        fputs ("File error\n", stderr);
        perror("Error occurred");
        return ERR_FILE;
    }

    /* get file size */
    fseek (f_file , 0 , SEEK_END);
    l_size = ftell (f_file);
    rewind (f_file);

    if (l_size == 0) {
        *s_buff = NULL;
        fclose (f_file);
        return 0;
    }

    /* allocate memory to contain the whole file */
    *s_buff = calloc (l_size+1, sizeof (char));
    if (*s_buff == NULL) {
        fputs ("Alloc error\n", stderr);
        perror("Error occurred");
        exit (EXIT_FAILURE);
        //return ERR_ALLOC;
    }

    /* copy the file into the buffer */
    st_res = fread (*s_buff, 1, l_size, f_file);
    if (st_res != (size_t) l_size) {
        fputs ("File reading error\n", stderr);
        perror("Error occurred");
        return ERR_FILE_RW;
    }
    fclose (f_file);

    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file and count hash.
 */
int
read_file_data_hash (const char  *s_fname,
                     char       **s_buff,
                     uint64_t    *i_hash)
{
    int i_res = ERR_OK; /* Function result */

    i_res = read_file_data (s_fname, s_buff);
    if (i_res != 0) {
        *i_hash = 0;
        return i_res;
    }
    *i_hash = hash (*s_buff);

    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save a bunch of data to file.
 */
int
save_file_data (const char *s_fname,
                const char *s_buff)
{
    FILE   *f_file;  /* File to save data */
    size_t  st_res;  /* Save data count */
    size_t  st_size; /* Buffer size */

    f_file = fopen (s_fname, "wb");
    if (f_file == NULL) {
        fputs ("File error\n", stderr);
        perror("Error occurred");
        return ERR_FILE;
    }
    st_size = strlen(s_buff);
    st_res = fwrite (s_buff , sizeof(char), st_size, f_file);
    fclose (f_file);
    if (st_res != st_size) {
        fputs ("File writting error\n", stderr);
        perror("Error occurred");
        return ERR_FILE_RW;
        }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  String compare function for getting string position function.
 */
int
compare_strings (const char *a,
                 const char *b)
{
    int i_res = 0; /* Function result */

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
        i_res = strcmp (a, b);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory content in FList 
 */
void
get_directory_content_append_to_flist (const char *s_path1,
                                       FList      *fl_files)
{
    char   *s_pthfn   = NULL; /* Full file name with path */
    char   *s_path    = NULL; /* File path */
    size_t  i_dlen    = 0;    /* Path string length */
    DIR    *dr;               /* Dirent directory */
    struct  dirent *de;       /* Dirent struct */

    i_dlen = strlen (s_path1);

    /* Reserve 1 more for a slash later */
    create_resize ((void**) &s_path, i_dlen + 2, sizeof (char));
    strcpy (s_path, s_path1);
    if (s_path[i_dlen-1] != '/') {
        strcat (s_path, "/");
        i_dlen++;
    }

    dr = opendir (s_path); 
    if (dr == NULL) {
        printf ("Could not open current directory\n"); 
        free (s_path);
        return; 
    } 
    while ((de = readdir(dr)) != NULL) {
        //if (de->d_type == DT_REG) {
        if (de->d_type == 8) {
            s_pthfn = calloc ((i_dlen + strlen (de->d_name)+1), sizeof (char));
            if (s_pthfn == NULL) {
                fputs ("Alloc error\n", stderr);
                //return ERR_ALLOC;
                exit (EXIT_FAILURE);
            }
            strcpy (s_pthfn, s_path);
            strcat (s_pthfn, de->d_name);
            flist_insert_data (fl_files, s_pthfn);
            free (s_pthfn);
        }
    }
    if (s_path != NULL)
        free (s_path);

    closedir(dr);
}
/*----------------------------------------------------------------------------*/

