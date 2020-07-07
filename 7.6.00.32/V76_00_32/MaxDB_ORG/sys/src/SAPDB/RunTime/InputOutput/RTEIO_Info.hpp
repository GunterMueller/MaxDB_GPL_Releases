/*!
  @file           RTEIO_Info.hpp
  @author         JoergM
  @ingroup        RunTime
  @brief          class collecting I/O object information needed for I/O operations

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
#ifndef RTEIO_INFO_HPP
#define RTEIO_INFO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class RTEIO_Info
  @brief This class collects the i/o object specific information. It allows to derive
  backup I/O objects (with specified name) and RTE_VolumeInfo objects with implicit specified
  name. It defines the I/O types used to specify file/pipe/raw device/tape etc.

  THIS CLASS IS SUBJECT TO IMMEDIATE CHANGE WITH NEW IO CONCEPTS OF RUN TIME ENVIRONMENT
  */
class RTEIO_Info
{
public:
    /*!
      IO Object type specifier
     */
    enum IOType
    {
        IOType_Unknown      = 0 //!< type not specified (default)
       ,IOType_File         = 1 //!< file object
       ,IOType_Pipe         = 2 //!< pipe object (unidirectional)
       ,IOType_Raw          = 3 //!< raw device
       ,IOType_NorewindTape = 4 //!< tape that does not automatically rewind on close
       ,IOType_RewindType   = 5 //!< tape that does automatically rewind on close
       ,IOType_Dafs_File    = 6 //!< file accessed via DAFS
    };

  /*!
    @brief default ctor

    The path and type are set explicitly by RTETask_Task...
   */
    RTEIO_Info()
        : m_Type(IOType_Unknown)
        , m_AssignedIdentifier(0)
    {
        m_Path[0] = 0;
    }

private:
    RTE_Path   m_Path;               //!< Specified path to I/O object
    IOType     m_Type;               //!< Specified type of I/O object
    SAPDB_Int4 m_AssignedIdentifier; //!< Assigned identifier (modified by RTE_ITask::AsyncOpen)
    friend struct RTETask_Task;
}; /*! @endclass RTEIO_Info */

#endif  /* RTEIO_INFO_HPP */
