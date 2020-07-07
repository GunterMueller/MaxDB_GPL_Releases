/****************************************************************************

  module      : hbd911.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: 
  description : Declaration of exported functions of modul vbd911.cpp


  version     : 7.4.02.30
  last changed: 2005-03-31  19:48
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright by SAP AG, 1998



    ========== licence begin  GPL
    Copyright (C) 2000 SAP AG

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

*****************************************************************************/

#ifndef HBD911_H
#define HBD911_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "RunTime/System/RTESys_MemoryBarrier.h" 
#ifndef FILEDIR_IDIRECTORY_HPP
#   include "FileDirectory/FileDir_FileNo.hpp"
#endif

class cbd911RecreateIndexLock
{
public:
    cbd911RecreateIndexLock(tgg00_TransContext &Trans) 
        : m_Trans(Trans)
        , m_locked(false)
    {
        // RTESys_ReadMemoryBarrier();
    };
    ~cbd911RecreateIndexLock();
    bool Lock(bool  bExclusive);
    bool IsNeeded(const FileDir_FileNo& FileNo); 
private:
    tgg00_TransContext&     m_Trans;
    bool                    m_mode;
    bool                    m_locked;
};


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp tgg00_BasisError&
bd911RecreateIndexError();

/*---------------------------------------------------------------------------*/

externCpp tgg00_FileId& 
bd911GetKeyFileId(tsp00_Int4 ix);

/*---------------------------------------------------------------------------*/

externCpp FileDir_FileNo& 
bd911GetKeyFileNo(tsp00_Int4 ix);

/*---------------------------------------------------------------------------*/

externCpp void
bd911CheckIndex (tgg00_TransContext   &Trans,
                 FileDir_FileNo       &FileNo );

/*---------------------------------------------------------------------------*/

externCpp void
bd911KeyToConsole( void *const pKey, tsp00_Int4 ObjKeyLen);

/*---------------------------------------------------------------------------*/

externCpp void
bd911RecreateIndex (tgg00_TransContext   &Trans,
                    FileDir_FileNo       &FileNo );

/*---------------------------------------------------------------------------*/

externCpp void
bd911InitRWRecreateIndex(void);

/*---------------------------------------------------------------------------*/

externCpp void
bd911EnterRecreateIndex (tsp00_TaskId  Taskid,
                         bool          exclusiveRequest);

/*---------------------------------------------------------------------------*/

externCpp void
bd911LeaveRecreateIndex (tsp00_TaskId   Taskid,
                         bool           exclusiveRequest);

#endif /* HBD911_H */
