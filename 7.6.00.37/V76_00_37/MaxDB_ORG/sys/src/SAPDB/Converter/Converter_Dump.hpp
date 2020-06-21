/****************************************************************************

  module      : Converter_Dump.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: converter
  description : Layout of converter dump structures


  last changed: 2001-06-12  18:47
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2004 SAP AG

  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end

*****************************************************************************/


#ifndef CONVERTER_DUMP_HPP
#define CONVERTER_DUMP_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/// dump structure for converter manager
struct ConverterManager 
{
    /// ...
    SAPDB_UInt4     dmpVersion;
    /// ...
    SAPDB_UInt4     dmpRootDevNo;
    /// ...
    SAPDB_UInt4     dmpRootBlockNo;
    /// ...
    SAPDB_UInt4     dmpNumPermPagesUsed;
    /// ...
    SAPDB_UInt4     dmpNumTempPagesUsed;
    /// ...
    SAPDB_UInt4     dmpNumAllPages;
    /// ...
    SAPDB_UInt4     dmpDeltaSnapShotPages;
    /// ...
    SAPDB_UInt4     dmpNumPagesForBackup;
    /// ...
    SAPDB_UInt4     dmpNumStatConvPagesForBackup;
    /// ...
    SAPDB_UInt4     dmpNumDynConvPagesForBackup;
    /// ...
    SAPDB_UInt4     dmpNumPermConvPages;
    /// ...
    SAPDB_UInt4     dmpNumTempConvPages;
    /// ...
    SAPDB_UInt4     dmpMapBlockingSize;
    /// ...
    SAPDB_Bool1     dmpConverterIsActive;
    /// ...
    SAPDB_Bool1     dmpSavePagesIsActive;
    /// ...
    SAPDB_Bool1     dmpSaveDataIsActive;
    /// ...
    SAPDB_Bool1     dmpRestoreDataIsActive;
    /// ...
    SAPDB_Bool1     dmpRestorePagesIsActive;
    /// ...
    SAPDB_Bool1     dmpColdVerifyIsActive;
};

/// dump structure for converter map section
struct  ConverterMapSection
{
    /// ...
    SAPDB_Bool1     dmpIsDynamic;
    /// ...
    SAPDB_Bool1     dmpIsPerm;
    /// ...
    SAPDB_Int2      dmpFiller1;
    /// ...
    SAPDB_UInt4     dmpSectionNo;
    /// ...
    SAPDB_UInt4     dmpRegionId;
    /// ...
    SAPDB_UInt4     dmpUnusedEntryChain;
    /// ...
    SAPDB_UInt4     dmpPartEntryChain;
    /// ...
    SAPDB_UInt4     dmpFullEntryChain;
    /// ...
    SAPDB_UInt4     dmpUsedPageNoCount;
    /// ...
    SAPDB_UInt4     dmpMaxUsedEntryNo;
    /// ...
    SAPDB_Bool1     dmpSavepointIsActive; // perm only
    /// ...
    SAPDB_Bool1     dmpLastDataSuccessful; // perm only
    /// ...
    SAPDB_UInt2     dmpPendingIO; // perm only
    /// ...
    SAPDB_UInt4     dmpNumUpdatedPermPages; // perm only
    /// ...
    SAPDB_UInt4     dmpNumPendingUpdatedPermPages; // perm only
};


/// dump structure for converter map entry
struct ConverterMapEntry
{
    /// ...
    SAPDB_UInt4     dmpEntryNo;
    /// ...
    SAPDB_UInt4     dmpSectionNo;
    /// ...
    SAPDB_UInt4     dmpBaseNo;
    /// ...
    SAPDB_UInt4     dmpReservedCnt;
    /// ...
    SAPDB_UInt4     dmpFreeCnt;
    /// ...
    SAPDB_UInt4     dmpUsedCnt;
    /// ...
    SAPDB_UInt4     dmpChainPrev;
    /// ...
    SAPDB_UInt4     dmpChainNext;
    /// ...
    SAPDB_Bool1     dmpIOPendig;
    /// ...
    SAPDB_Bool1     dmpChanged;
    /// ...
    SAPDB_UInt2     dmpFiller1;
};

/// dump structure for converter index entry
struct ConverterIndexEntry
{
    /// ...
    SAPDB_UInt4     dmpEntryNo;
    /// ...
    SAPDB_UInt4     dmpDevNo;
    /// ...
    SAPDB_UInt4     dmpBlockNo;
    /// ...
    SAPDB_UInt2     dmpChainPrev;
    /// ...
    SAPDB_UInt2     dmpChainNext;
    /// ...
    SAPDB_UInt2     dmpFreeEntryCnt;
    /// ...
    SAPDB_UInt2     dmpPageFreeEntryCnt;
    /// ...
    SAPDB_UInt2     dmpEntryCnt;
    /// ...
    SAPDB_UInt1     dmpChainType;
    /// ...
    SAPDB_UInt1     dmpFiller1;
};

#endif  /* CONVERTER_DUMP_HPP */
