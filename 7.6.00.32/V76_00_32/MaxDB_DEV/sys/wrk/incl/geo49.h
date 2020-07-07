/*!
  @file           geo49.h
  @author         JoergM
  @ingroup        Security
  @brief          Security and User specific functions

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



\endif
*/



#ifndef GEO49_H
#define GEO49_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


#if defined(WIN32)
#include <windows.h>
/*! Typedefinition for NT file handle */
typedef HANDLE teo49_FileHandle;
#else
/*! Typedefinition for UNIX file handle */
typedef int    teo49_FileHandle;
#endif

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*!
  @brief Get user name of file owner

  @param FileHandle [in] handle of already opened file
  @param UserName [out] the owner name for the file
  @return true if call succeeded, false if problem
 */
externC bool eo49GetFileOwner(const teo49_FileHandle FileHandle, tsp00_Namec VAR_ARRAY_REF UserName);

/*!
  @brief Get group name associated with file
  On windows a group concept does not exist, so group name is always ""

  @param FileHandle [in] handle of already opened file
  @param GroupName [out] the group name for the file
  @return true if call succeeded, false if problem
 */
externC bool eo49GetFileGroup(const teo49_FileHandle FileHandle, tsp00_Namec VAR_ARRAY_REF GroupName);

/*!
  @brief Set group name associated with file
  On windows a group concept does not exist, so this call always returns true

  @param FileHandle [in] handle of already opened file
  @param GroupName [in] the new group name for the file
  @return true if call succeeded, false if problem
 */
externC bool eo49SetFileGroup(const teo49_FileHandle FileHandle, tsp00_Namec const VAR_ARRAY_REF GroupName);

#endif  /* GEO49_H */
