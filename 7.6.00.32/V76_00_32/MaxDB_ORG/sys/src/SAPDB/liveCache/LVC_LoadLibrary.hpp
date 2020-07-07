/*!
 * \file    LVC_LoadLibrary.hpp
 * \author  IvanS
 * \brief   Loading of shared libraries for OMS stored procedure.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



*/

#ifndef __LVC_LOADLIBRARY_HPP
#define __LVC_LOADLIBRARY_HPP

#include "heo02x.h"

void LVC_LoadLibrary(const char *path, 
                     bool      loadGlobal,
                     HANDLE    *hd,
                     char      *errtext,
                     int       errtext_size);

void LVC_FreeLibrary(HANDLE      hd,
                     bool        *ok,
                     char        *errtext,
                     int         errtext_size);

#endif  // __LVC_LOADLIBRARY_HPP
