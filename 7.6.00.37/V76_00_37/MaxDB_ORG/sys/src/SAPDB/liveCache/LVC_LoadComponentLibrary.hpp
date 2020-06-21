/*!
 * \file    LVC_DispatcherParams.hpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   Load a component library and calculate an MD5 footprint.
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

#ifndef  __LVC_LOADCOMPONENTLIBRARY_HPP
#define  __LVC_LOADCOMPONENTLIBRARY_HPP

#include "gsp00.h"
#include "Oms/OMS_Defines.h"

/// Loads a component library return handle and calculates a MD5 footprint.
/*!
** Loads a component library and calculates a MD5 footprint.
**
** \param libName Library name, which may or may not contain a complete
**        or partial path specification. The file type may be ommitted
** \param PackageId assigned by catalog. 
** \param hInstance handle of loaded library
** \param MD5Footprint Out parameter containing the MD5 footprint if function 
**        signals success
** \return S_OK if succeeded or ERROR_FILE_NOT_FOUND
**/
externCpp
HRESULT LVC_LoadComponentLibrary(char const     *libName, 
                                 const tsp00_C8 &packageId,
                                 HANDLE         *hInstance,
                                 tsp00_C32      &MD5Footprint); 

/// Loads a component library, calculates/compares MD5 footprint and gets component factory pointer.
/*!
** Loads a component library, calculates/compares MD5 footprint and gets component factory pointer.
**
** \param libName Library name, which may or may not contain a complete
**        or partial path specification. The file type may be ommitted
** \param PackageId assigned by catalog. 
** \param CoClsId component class ID
** \param MD5Footprint Out parameter containing the MD5 footprint if function 
**        signals success
** \return S_OK if succeeded or ERROR_FILE_NOT_FOUND
**/
externCpp
HRESULT LVC_LoadComponentLibrary(char const      *libName, 
                                 const tsp00_C8  &packageId,
                                 const GUID      &CoClsId,
                                 const tsp00_C32 &MD5Footprint,
                                 tsp00_Addr      &ifactory);
#endif