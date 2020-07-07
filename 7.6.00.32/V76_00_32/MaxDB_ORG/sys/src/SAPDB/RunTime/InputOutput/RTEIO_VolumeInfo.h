/*!
  @file           RTEIO_VolumeInfo.h
  @author         StefanP
  @ingroup        InputOutput
  @brief          Updating physical volume information for volumes (C Interface)

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef RTEIO_VOLUMEINFO_H
#define RTEIO_VOLUMEINFO_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo002.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*===========================================================================*
 *  PROTOTYPS                                                                *
 *===========================================================================*/



/*!
  @brief Update information about physical volumes of a specific volume (UNIX only)

  @param pVolume [in/out]     Pointer to volume

 */
externC void RTEIO_UpdatePhysicalVolInfoForVolume (DEV_DESCR *pVolume);

#endif  /* RTEIO_VOLUMEINFO_H */
