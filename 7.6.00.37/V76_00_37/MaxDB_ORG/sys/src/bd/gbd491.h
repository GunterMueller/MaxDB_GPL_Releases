/*
  @file         gbd491.hpp
  @author       TorstenS
  @ingroup      create index
  @brief        This modul is used to sort and materialze the primary
                records of a table to be indexed.

\if EMIT_LICENCE


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

\endif
*/



#ifndef GBD491_H
#define GBD491_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

                     // Content of include files
#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gbd00.h"   // PASCAL: BD_kernel_constants_and_types
#include "gbd497.h"  // CPP   : StackDescInfo

#include "KernelCommon/Kernel_Common.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class cbd491_InvRecordCollection;
class cbd492_StorageArea;
class cbd300_InvCurrent;
class cbd495_DoubleWriteQueue;
class cbd460_InvListIterator;
class SAPDBMem_IRawAllocator;


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class cbd491_CreateIndexParticipant
{
public:
    
    cbd491_CreateIndexParticipant(
        Trans_Context&      trans,
        tbd_current_tree&   primCurrent,
        tgg00_StackDesc&    stackDesc,
        tgg00_IndexCounter& indexCounter,
        const bool           bUseMemorySort = true );
    
    void InsertionSort(
        Trans_Context&          trans,
        cbd300_InvCurrent&      invCurrent,
        tgg00_IndexMode         indexMode,
        SAPDB_Byte* const       pStartPrimKey,      
        const SAPDB_Byte* const pStopPrimKey,    
        const SAPDB_UInt        startPrimKeyLen, 
        const SAPDB_UInt        stopPrimKeyLen,  
        pasbool&                bEntriesExist ); 

    void MemorySort(
        Trans_Context&              trans,
        cbd495_DoubleWriteQueue&    resultInvQueue,
        SAPDB_Byte* const           pStartPrimKey,
        const SAPDB_Byte* const     pStopPrimKey,
        const SAPDB_UInt            startPrimKeyLen,
        const SAPDB_UInt            stopPrimKeyLen,
        const SAPDB_UInt            numLeavesToScan,
        const SAPDB_UInt            avgRecordsPerLeaf,
        pasbool&                    bEntriesExist ); 
    
private:

    void AddOneInvRecord(
        Trans_Context&      trans,
        cbd300_InvCurrent&  invCurrent,
        tgg00_IndexMode     indexMode,
        tgg00_RecPtr        pRec );
    
    void AppendInvRecords(
        Trans_Context&              trans,
        cbd495_DoubleWriteQueue&    resultInvQueue,
        cbd491_InvRecordCollection& invRecCollection );     
    
    void ProcessPrimTreeRange(
        Trans_Context&              trans,
        cbd491_InvRecordCollection& invRecCollection,
        cbd492_StorageArea&         sortArea,
        SAPDB_Byte* const           pStartPrimKey,
        const SAPDB_Byte* const     pStopPrimKey,
        const SAPDB_UInt            startPrimKeyLen,
        const SAPDB_UInt            stopPrimKeyLen );

private:

    SAPDBMem_IRawAllocator  &m_Allocator;

    tbd_current_tree        &m_PrimCurrent;
    tgg00_IndexCounter      &m_IndexCounter;
    
    cbd497_StackDescInfo    m_Stack;
    tsp00_Key               m_UndefNullKey;
    SAPDB_Byte*             m_pUndefNullKey;
};

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/

#endif  /* GBD491_H */
 
