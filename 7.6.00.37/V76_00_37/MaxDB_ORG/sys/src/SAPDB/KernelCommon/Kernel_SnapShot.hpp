/*!

  @file         Kernel_SnapShot.hpp
  @author       TorstenS
  @ingroup      Kernel_Common
  @brief        This class is used to create, drop and read the snapshot
                restart record which is used when a converter snapshot
                is defined.
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


#ifndef KERNEL_SNAPSHOT_HPP
#define KERNEL_SNAPSHOT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Converter/Converter_Version.hpp"

#include "gkb00.h"
#include "hkb57_1.h" // k57restartrec


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Kernel_DateTime;
class RTETask_ITask;
class IOMan_PackedBlockAddress;



class Kernel_SnapShot
{
public:

    void MarkSnapShotRestartPageAsOccupied(
        const RTETask_ITask             &task,
        const tkb00_ConfigRestartParam  &rstConfig,
        IOMan_PackedBlockAddress        &packedSnapShotRootBlock,
        Converter_Version               &snapShotVersion,
        Kernel_DateTime                 &snapShotCreationTimeStamp );

    void Create( const RTETask_ITask  &task,
                 tkb00_PagePtr        pRstPage );

    void Drop( const RTETask_ITask  &task,
               tkb00_PagePtr        pRstPage );

    void Activate( const RTETask_ITask  &task );

};

#endif  /* KERNEL_SNAPSHOT_HPP */
