/**
 * @file  errs.h
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
 * @brief  Error info
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef ERRS_H
#define ERRS_H

enum {
    ERR_OK,      /**< Everything OK */
    ERR_FILE,    /**< File error (cannot access / bad permissions */
    ERR_FILE_RW, /**< File read / write error */
    ERR_FILE_CR, /**< File can not be created */
    ERR_FILE_EX, /**< File does not exist */
    ERR_ALLOC    /**< Alloc error */
};

#endif
