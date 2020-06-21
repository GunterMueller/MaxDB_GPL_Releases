/*!**************************************************************************

  module      : vkb521.hpp

  -------------------------------------------------------------------------

  responsible : UlrichJ

  special area: SQL locks reader
  description : This class implements a reader for SQL locks.

  last changed: 2002-08-14
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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



*****************************************************************************/


#ifndef VKB521_HPP
#define VKB521_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "vak001.h"
#include "ggg91.h"
#include "gkb05.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Container/Container_List.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!***************************************************************************

   class: ckb521_SQLLockReader

   description: This class implements a reader object for the SQL locks to be
                able to read all locks sequentially.

******************************************************************************/

class ckb521_SQLLockReader
{

public:

    struct SQLLockElement
    {
        tgg91_TransNo               sleTransID_kb521;
        tsp00_Int4                  sleLockReqTimeout_kb521;
        tgg00_LockReqMode_Enum      sleLockMode_kb521;
        tgg00_LockReqState          sleLockState_kb521;
        tgg00_LockReqMode_Enum      sleReqMode_kb521;
        tgg00_LockReqState          sleReqState_kb521;
        tgg00_Surrogate             sleTblID_kb521;
        SAPDB_Bool                  sleRowIDUndef_kb521;
        tsp00_Int2                  sleRowIDLen_kb521;
        tkb05_RowId                 sleRowIDHex_kb521;
    };


    typedef Container_List<SQLLockElement> SQLLockList;


    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!-------------------------------------------------------------------------
    function    : ckb521_SQLLockReader()
    description : constructor 
    arguments   : none
    return value: none
    --------------------------------------------------------------------------*/

    ckb521_SQLLockReader(SAPDBMem_IRawAllocator	&alloc,
                         tgg00_TransContext      &trans);
    ~ckb521_SQLLockReader(void);

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool Begin (void);
    SQLLockElement * Next (void);
    void FreeAll (void);

private:

    SAPDB_Bool ReadList(void);
    SAPDB_Bool ReadLocks(void);
    SAPDB_Bool ReadCollisions(void);

    void PutLoopError ( const SAPDB_Char    *msg,
                        SAPDB_Int4          iError);

    void AddTableLock (tkb05_TransEntryPtr     &CurrTrans,
                       tkb05_TabTransPtr       &CurrTab,
                       SAPDB_Bool              &IsTabLock,
                       SAPDB_Bool              &IsTabReq);

    void AddRowLock (tkb05_TransEntryPtr     &CurrTrans,
                     tkb05_TabTransPtr       &CurrTab,
                     tkb05_RowTransPtr       &CurrRow);

    void AddObjToList (tgg00_TransIndex    &TransIndex, 
                       tgg92_KernelOid     &Oid, 
                       SAPDB_Bool          bIsRequest);


    SAPDBMem_IRawAllocator	                &m_Allocator;
    tgg00_TransContext                      &m_Trans;
    SQLLockList                             *m_List;
    SQLLockList::Iterator                   m_Current;
};

/*!***************************************************************************

   endclass: ckb521_SQLLockReader

******************************************************************************/

#endif  /* VKB521_HPP */

