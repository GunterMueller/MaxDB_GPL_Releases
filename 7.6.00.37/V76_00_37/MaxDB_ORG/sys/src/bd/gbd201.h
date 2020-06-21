/*!***************************************************************************

  module      : gbd201.h

  -------------------------------------------------------------------------

  author      : Henrik

  special area: DataCacheIO
  description : 


  last changed: 2004-09-01  14:00
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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



#ifndef GBD201_H
#define GBD201_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

                // Content of include files
                
#include "gbd02.h"

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "IOManager/IOMan_DataPages.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "Container/Container_Vector.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


class bd201_Clusterer {

public:

	bd201_Clusterer(SAPDBMem_IRawAllocator &allocator);

	void Initialize();

    void GetPagesForIO(const tsp00_TaskId taskId, 
                                   IOMan_DataPages   &pages,
                                   IOMan_BlockCount   numPages,
                                   SAPDB_Bool         inSavePoint);

    void ResetIOState(const tsp00_TaskId taskId, 
                                   IOMan_DataPages   &pages,
                                   IOMan_BlockCount  startPage,
                                   IOMan_BlockCount  pageCount);
                                
    void PrepareFlush(const tsp00_TaskId taskId);

    void EndSave( const tsp00_TaskId taskId);

private:

    Container_Vector<tbd02_pDataCBlock>  m_cBlocks;
    Container_Vector<SAPDB_Int4>         m_blockCount;
    
 };

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/

#endif  /* GBD201_H */

