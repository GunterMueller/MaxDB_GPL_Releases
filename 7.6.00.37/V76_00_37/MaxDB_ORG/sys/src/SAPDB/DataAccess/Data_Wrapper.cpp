/****************************************************************************

  module      : Data_Wrapper.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Data_Wrapper
  description : 


  last changed: 2001-02-06  12:36
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "hbd998.h"

#include "DataAccess/Data_RandomTreeAccess.hpp"
#include "DataAccess/Data_ScanTreeAccess.hpp"
#include "DataAccess/Data_TraverseTreeAccess.hpp"
#include "DataAccess/Data_PrimTreeStatistic.hpp"
#include "DataAccess/Data_IndexOrderList.hpp"
#include "DataAccess/Data_Row.hpp"
#include "KernelCommon/Kernel_TableStatistic.hpp"

#include "SQLManager/SQLMan_TempFileCollection.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

void bd998GetExtendedTempFileInfo(
    tgg00_TransContext& trans,
    const tgg00_FileId& tempFileId,
    SAPDB_Int&          level,
    SAPDB_Int&          subLevel,
    SAPDB_Int4&         index);

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

externC
void bd999CalculateColumnStatistic(
    tgg00_TransContext          &trans,
    tbd_current_tree            &current,
    tbd_current_tree            &tempCurrent,
    tsp00_Int2                  columnCount,
    tsp00_Int2                  columnFirstPosition,
    tsp00_Int2                  columnLastPosition,
    tgg00_StEntryAddr           pColumnDescription,
    tsp00_Int4                  &numberOfLeaves,
    tsp00_Int4                  &numberOfRecords,
    tgg00_ColumnDistinctValues  &distinctValues )
{
    Data_PageCount  auxLeaves;
    Data_PageCount  auxRecords;

    Data_PrimTreeStatistic  primTreeStatistic( current );

    Data_ScanTreeAccess treeAccessor( current );

    primTreeStatistic.CalculateColumnStatistic(
        treeAccessor, tempCurrent,
        columnCount, columnFirstPosition, columnLastPosition,
        pColumnDescription, auxLeaves, auxRecords,
        distinctValues );

    numberOfLeaves = auxLeaves;
    numberOfRecords = auxRecords;
};

/*---------------------------------------------------------------------------*/

externC
void bd999SampleNewColumnStatistic(
    tgg00_TransContext          &trans,
    tbd_current_tree            &current,
    tbd_current_tree            &tempCurrent,
    tsp00_Int4                  numberOfSampleLeaves,
    tsp00_Int2                  columnCount,
    tsp00_Int2                  columnFirstPosition,
    tsp00_Int2                  columnLastPosition,
    tgg00_StEntryAddr           pColumnDescription,
    tsp00_Int4                  &numberOfLeaves,
    tsp00_Int4                  &numberOfRecords,
    tgg00_ColumnDistinctValues  &distinctValues )
{
    SAPDBERR_ASSERT_ARGUMENT( numberOfSampleLeaves > 0 );

    Data_PageCount  auxLeaves;
    Data_PageCount  auxRecords;
    const SAPDB_Bool bNoDuplicates          = true;

    Data_PrimTreeStatistic  primTreeStatistic( current );
    Data_RandomTreeAccess   treeAccessor(
        trans, current, numberOfSampleLeaves, ! bNoDuplicates );

    if( ! treeAccessor.Initialize(  numberOfSampleLeaves ))
        return;

    primTreeStatistic.SampleNewColumnStatistic(
        treeAccessor, tempCurrent,
        columnCount, columnFirstPosition, columnLastPosition,
        pColumnDescription, auxLeaves, auxRecords,
        distinctValues );

    numberOfLeaves = auxLeaves;
    numberOfRecords = auxRecords;
};


/*---------------------------------------------------------------------------*/

externC
void bd999CalculateCluster(
    tgg00_TransContext  &trans,
    tbd_current_tree    &current,
    tsp00_Int4          &numberOfLeaves,
    tsp00_Int4          &numberOfClusters )
{
    Data_PageCount  auxLeaves;
    Data_PageCount  auxClusters;

    Data_PrimTreeStatistic  primTreeStatistic( current );

    primTreeStatistic.CalculateCluster( auxLeaves, auxClusters );

    numberOfLeaves    = auxLeaves;
    numberOfClusters  = auxClusters;
}

/*---------------------------------------------------------------------------*/

externC
void bd999CalculateRecordsAndLeaves(
    tgg00_TransContext  &trans,
    tbd_current_tree    &current,
    tsp00_Int4          &numberOfLeaves,
    tsp00_Int4          &numberOfRecords )
{
    Data_PageCount  auxLeaves;
    Data_PageCount  auxRecords;

    Data_PrimTreeStatistic  primTreeStatistic( current );

    primTreeStatistic.CalculateRecordsAndLeaves( auxLeaves, auxRecords );

    numberOfLeaves  = auxLeaves;
    numberOfRecords = auxRecords;
};

/*---------------------------------------------------------------------------*/

externC
void bd999SampleRecordsAndLeaves(
    tgg00_TransContext  &trans,
    tbd_current_tree    &current,
    tsp00_Int4          numberOfSampleLeaves,
    tsp00_Int4          &numberOfLeaves,
    tsp00_Int4          &numberOfRecords )
{
    SAPDBERR_ASSERT_ARGUMENT( numberOfSampleLeaves > 0 );

    Data_PageCount  auxLeaves;
    Data_PageCount  auxRecords;

    Data_PrimTreeStatistic  primTreeStatistic( current );
    Data_RandomTreeAccess   treeAccessor(
        trans, current, numberOfSampleLeaves );

    if( ! treeAccessor.Initialize( numberOfSampleLeaves ))
        return;

    primTreeStatistic.SampleRecordsAndLeaves(
        treeAccessor, auxLeaves, auxRecords );

    numberOfLeaves  = auxLeaves;
    numberOfRecords = auxRecords;
};

/*---------------------------------------------------------------------------*/

externC
void bd999CalculateTableStatistic(
    tgg00_TransContext  &trans,
    tbd_current_tree    &current,
    tbd_current_tree    &shortCurrent,
    tsp00_Bool          bShortColumnFileExist,
    tsp00_Int4          numberOfLongColumns,
    tgg00_TableStatInfo &statisticStruct )
{

    if( 0 < numberOfLongColumns )
    {
        Kernel_TableStatistic   tableStatistic( statisticStruct );
        Data_PrimTreeStatistic  primTreeStatistic( current );

        if( bShortColumnFileExist )
        {
            tgg00_TableStatInfo     shortStruct;
            Kernel_TableStatistic   shortStatistic( shortStruct );
            Data_PrimTreeStatistic  shortTreeStatistic( shortCurrent );
            Data_TraverseTreeAccess traverseShortTreeAccessor( shortCurrent );

            shortTreeStatistic.CalculateTableStatistic(
                traverseShortTreeAccessor, 0, shortStatistic );

            if( e_ok != trans.trError_gg00 )
                return;

            tableStatistic.IntegrateShortColumnStatistic( shortStatistic );
        }

        Data_TraverseTreeAccess traverseTreeAccessor( current );
        primTreeStatistic.CalculateTableStatistic(
            traverseTreeAccessor, numberOfLongColumns, tableStatistic );
    }
    else
    {
        Data_PrimTreeStatistic  primTreeStatistic( current );
        Data_TraverseTreeAccess traverseTreeAccessor( current );
        Kernel_TableStatistic   tableStatistic( statisticStruct );

        primTreeStatistic.CalculateTableStatistic(
            traverseTreeAccessor, numberOfLongColumns, tableStatistic );
    }
};

/*---------------------------------------------------------------------------*/

externPascal pasbool bd998IsTempFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_FileId    VAR_VALUE_REF  fileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("bd998IsTempFile", DataTree_Trace, 1);


#       if defined(SAPDB_SLOW)
    if ( fileId.fileType_gg00().includes( ftsTemp_egg00 )
            || ( tfnTemp_egg00 == fileId.fileTfn_gg00() )
            || ( tfnTempMulti_egg00 == fileId.fileTfn_gg00() )
            || ( tfnTempAux_egg00 == fileId.fileTfn_gg00() )
            || ( tfnTempOms_egg00 == fileId.fileTfn_gg00() ) ) {
        SAPDBERR_ASSERT_STATE(
            fileId.fileType_gg00().includes( ftsTemp_egg00 )
            && ( (tfnTemp_egg00 == fileId.fileTfn_gg00())
                 || ( tfnTempMulti_egg00 == fileId.fileTfn_gg00() )
                 || ( tfnTempAux_egg00 == fileId.fileTfn_gg00() )
                 || ( tfnTempOms_egg00 == fileId.fileTfn_gg00() ) ) );
    }
#       endif
    return fileId.fileType_gg00().includes( ftsTemp_egg00 );
}

/*---------------------------------------------------------------------------*/

externPascal pasbool bd998IsExtendedTempFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG (
        "bd998IsExtendedTempFile", DataTree_Trace, 1);

    return tfnTemp_egg00 == tempFileId.fileTfn_gg00();
}

/*---------------------------------------------------------------------------*/

externPascal tgg00_TfnTemp_Param bd998GetExtendedTempFileType (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG (
        "bd998GetExtendedTempFileType", DataTree_Trace, 1);

    if ( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 ) {
        // we have a logical file id
        SAPDBERR_ASSERT_STATE( bd998IsTempFile (trans, tempFileId) );
        SAPDBERR_ASSERT_STATE( tfnTemp_egg00 == tempFileId.fileTfn_gg00() );

        if ( tfnTemp_egg00 != tempFileId.fileTfn_gg00() ) {
            trans.trError_gg00 = e_invalid_filetype;
            return ttfnNone_egg00;
        }

        return tempFileId.fileTfnTemp_gg00();
    }
    else {
        //        return tgg00_TfnTemp::fromInt( tempFileId.fileTempCnt_gg00() );
        return tgg00_TfnTemp::fromInt( tempFileId.fileName_gg00()[ 15 ] );
    }
}

/*----------------------------------------------------------------*/

externPascal void bd998SetTempFileLevel (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  tempFileId,
    const tsp00_Int2                     level)
{
    SAPDBTRACE_ROUTINE_DEBUG ("bd998SetTempFileLevel", DataTree_Trace, 1);

    // necessary for new file directory to work
    SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 );
    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    tsp00_IntMapC2 i2c;
    switch ( bd998GetExtendedTempFileType (trans, tempFileId) )
    {
    case ttfnFromSelect_egg00:
    case ttfnComplexSelect_egg00:
    case ttfnComplexView_egg00:
    case ttfnSubquery_egg00:
    case ttfnUnion_egg00:
        tempFileId.fileLevel_gg00()[ 0 ] = static_cast<char>(level);
        break;
    default:
        i2c.mapInt_sp00() = level;
        tempFileId.fileLevel_gg00() = i2c.mapC2_sp00();
    }
}

/*----------------------------------------------------------------*/

externPascal void bd998SetTempFileSublevel (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  tempFileId,
    const tsp00_Int2                     sublevel)
{
    SAPDBTRACE_ROUTINE_DEBUG ("bd998SetTempFileSublevel", DataTree_Trace, 1);

    // necessary for new file directory to work
    SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 );
    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    switch ( bd998GetExtendedTempFileType (trans, tempFileId) )
    {
    case ttfnFromSelect_egg00:
    case ttfnComplexSelect_egg00:
    case ttfnComplexView_egg00:
    case ttfnSubquery_egg00:
    case ttfnUnion_egg00:
        break;
    default:
        SAPDBERR_ASSERT_STATE( false );
        trans.trError_gg00 = e_invalid_filetype;
        return;
    }

    tempFileId.fileLevel_gg00()[ 1 ] = static_cast<char>(sublevel);
}

/*----------------------------------------------------------------*/

externPascal void bd998SetTempFileIndex (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  tempFileId,
    const tsp00_Int4                     index)
{
    SAPDBTRACE_ROUTINE_DEBUG ("bd998SetTempFileIndex", DataTree_Trace, 1);

    // necessary for new file directory to work
    SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 );
    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    tempFileId.fileTempCnt_gg00() = index;
}

/*----------------------------------------------------------------*/

externPascal tsp00_Int2 bd998GetTempFileLevel (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("bd998GetTempFileLevel", DataTree_Trace, 1);

    SAPDB_Int  level;
    SAPDB_Int  subLevel;
    SAPDB_Int4 index;
    bd998GetExtendedTempFileInfo( trans, tempFileId, level, subLevel, index );

    return level;
}

/*----------------------------------------------------------------*/

externPascal tsp00_Int2 bd998GetTempFileSublevel (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("bd998GetTempFileSublevel", DataTree_Trace, 1);

    SAPDB_Int  level;
    SAPDB_Int  subLevel;
    SAPDB_Int4 index;
    bd998GetExtendedTempFileInfo( trans, tempFileId, level, subLevel, index );

    return subLevel;
}

/*----------------------------------------------------------------*/

externPascal tsp00_Int4 bd998GetTempFileIndex (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("bd998GetTempFileIndex", DataTree_Trace, 1);

    SAPDB_Int  level;
    SAPDB_Int  subLevel;
    SAPDB_Int4 index;
    bd998GetExtendedTempFileInfo( trans, tempFileId, level, subLevel, index );

    return index;
}

/*----------------------------------------------------------------*/

void bd998GetExtendedTempFileInfo(
    tgg00_TransContext& trans,
    const tgg00_FileId& tempFileId,
    SAPDB_Int&          level,
    SAPDB_Int&          subLevel,
    SAPDB_Int4&         index)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetExtendedTempFileInfo", DataTree_Trace, 1 );

    if ( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 ) {
        // we have a logical file id
        tsp00_IntMapC2 i2c;
        switch ( bd998GetExtendedTempFileType (trans, tempFileId) )
        {
        case ttfnFromSelect_egg00:
        case ttfnComplexSelect_egg00:
        case ttfnComplexView_egg00:
        case ttfnSubquery_egg00:
        case ttfnUnion_egg00:
            level    = static_cast<tsp00_Int2>(tempFileId.fileLevel_gg00()[0]);
            subLevel = static_cast<tsp00_Int2>(tempFileId.fileLevel_gg00()[1]);
            break;
        default:
            i2c.mapC2_sp00() = tempFileId.fileLevel_gg00();
            level            = i2c.mapInt_sp00();
            subLevel         = 0;
        }
        index = tempFileId.fileTempCnt_gg00();
        return;
    }

    // file is already instantiated and info has to be retrieved from
    // temp file collection
    SAPDBErr_MessageList message;
    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    FileDir_FileNo fileNo;
    SAPDB_MemCopyNoCheck( &fileNo, &tempFileId.fileTabId_gg00(),
                          sizeof(fileNo) );

    Data_TempFileTypes::FileType fileType;
    SQLMan_TempFileReturnCode rc = fileCollection.GetFileInfo(
                                       fileNo,
                                       fileType,
                                       level, subLevel, index,
                                       message);

    if ( SQLMan_TempFileOkay != rc ) {
        trans.trError_gg00 = e_file_not_found;
    }
}

/*----------------------------------------------------------------*/

externPascal tgg00_RecPtr bd999PackRow (
    tgg00_Rec             VAR_VALUE_REF  Source,
    tgg00_Rec             VAR_VALUE_REF  Destination)
{
    if (Data_Row::ToRow(Source).Pack (Data_Row::ToRow(Destination)))
    {
        return &Destination;
    }
    else
    {
        return &Source;
    }
}

/*----------------------------------------------------------------*/

externPascal void bd999CopyRow (
    tgg00_Rec             VAR_VALUE_REF  Source,
    tgg00_Rec             VAR_VALUE_REF  Destination,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    e = ((Data_Row::ToRow(Source).CopyRow (Data_Row::ToRow(Destination)))) ? e_ok : e_move_error;
}

/*----------------------------------------------------------------*/

externPascal tgg00_RecPtr bd999UnpackRow (
    tgg00_Rec             VAR_VALUE_REF  Source,
    tgg00_Rec             VAR_VALUE_REF  Destination)
{
    if (Data_Row::ToRow(Source).Unpack (Data_Row::ToRow(Destination)))
    {
        return &Destination;
    }
    else
    {
        return &Source;
    }
}

/*----------------------------------------------------------------*/

externPascal void bd999CreateIndexOrderList (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr            VAR_VALUE_REF  pIndexOrderList)
{
    SAPDBERR_ASSERT_ARGUMENT( pIndexOrderList == 0 );

    SAPDBMem_IRawAllocator& alloc = Trans_Context::GetContext( trans ).Allocator();
    Data_IndexOrderList* const pIOL =
        new ( alloc ) Data_IndexOrderList( alloc );

    if ( pIOL != 0 )
    {
        pIndexOrderList = reinterpret_cast<tsp00_Addr>( pIOL );
    }
    else
    {
        pIndexOrderList    = 0;
        trans.trError_gg00 = e_no_more_memory;
    }
}

/*----------------------------------------------------------------*/

externPascal void bd999DestroyIndexOrderList (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr            VAR_VALUE_REF  pIndexOrderList)
{
    SAPDBMem_IRawAllocator& alloc = Trans_Context::GetContext( trans ).Allocator();

    Data_IndexOrderList* pIOL =
        reinterpret_cast<Data_IndexOrderList*>( pIndexOrderList );
    destroy( pIOL, alloc );
    pIndexOrderList = 0;
}

/*----------------------------------------------------------------*/

externPascal pasbool bd999PushIndexOrder (
    tsp00_Addr    pIndexOrderList,
    const tbd00_TreeIndexOrder VAR_VALUE_REF indexOrder)
{
    Data_IndexOrderList* pIOL =
        reinterpret_cast<Data_IndexOrderList*>( pIndexOrderList );

    SAPDBERR_ASSERT_STATE( pIOL != 0 );
    return pIOL->Push( indexOrder);
}

/*----------------------------------------------------------------*/

externPascal pasbool bd999InsertBottomIndexOrder (
    tsp00_Addr    pIndexOrderList,
    const tbd00_TreeIndexOrder VAR_VALUE_REF indexOrder)
{
    Data_IndexOrderList* pIOL =
        reinterpret_cast<Data_IndexOrderList*>( pIndexOrderList );

    SAPDBERR_ASSERT_STATE( pIOL != 0 );
    return pIOL->InsertBottom( indexOrder );
}

/*----------------------------------------------------------------*/

externPascal tbd00_TreeIndexOrderPtr bd999GetTopIndexOrder (
    tsp00_Addr    pIndexOrderList)
{
    Data_IndexOrderList* pIOL =
        reinterpret_cast<Data_IndexOrderList*>( pIndexOrderList );

    SAPDBERR_ASSERT_STATE( pIOL != 0 );
    return &pIOL->Top();
}

/*----------------------------------------------------------------*/

externPascal pasbool bd999IsEmptyIndexOrderList (
    tsp00_Addr  pIndexOrderList)
{
    Data_IndexOrderList* pIOL =
        reinterpret_cast<Data_IndexOrderList*>( pIndexOrderList );

    SAPDBERR_ASSERT_STATE( pIOL != 0 );
    return  pIOL->IsEmpty();
}

/*----------------------------------------------------------------*/

externPascal void bd999PopIndexOrder (
    tsp00_Addr          pIndexOrderList)
{
    Data_IndexOrderList* pIOL =
        reinterpret_cast<Data_IndexOrderList*>( pIndexOrderList );

    SAPDBERR_ASSERT_STATE( pIOL != 0 );
    pIOL->Pop();
}

/*----------------------------------------------------------------*/

externPascal void bd999PrintIndexOrderList (
    tsp00_Addr          pIndexOrderList)
{
    Data_IndexOrderList* pIOL =
        reinterpret_cast<Data_IndexOrderList*>( pIndexOrderList );

    SAPDBERR_ASSERT_STATE( pIOL != 0 );
    pIOL->WriteToVtrace();
}

/*----------------------------------------------------------------*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
