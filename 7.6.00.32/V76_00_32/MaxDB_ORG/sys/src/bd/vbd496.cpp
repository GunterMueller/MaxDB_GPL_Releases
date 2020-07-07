/*
  @file         vbd496.cpp
  @author       TorstenS
  @ingroup      create index
  @brief        This modul is used to handle the temporary files
                which come into being during the parallel index
                creation to hold the temporary sorted index keys

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


// Content of include files
#include "gbd496.h"
#include "hbd01_1.h"  // PASCAL: filesysteminterface_1

#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  DEFINITION OF METHODS PRIVATE DECLARED IN gbd496.h (CODE)                * 
 *===========================================================================*/

void
cbd496_FileHandling::DeleteFile( cbd496_File& file )
{
    if( ! file.IsAssigned()){
        return;
    }
    if( file.IsDoubleQueueFile())
    {
        if( 0 < m_NumOpenQueuesFiles ){
            --m_NumOpenQueuesFiles;
        }
    }
    else  // file must be a temporary inv tree
    {
        if( 0 < m_NumOpenTreesFiles ){
            --m_NumOpenTreesFiles;
        }
    }
    file.Deassign();
}

/*---------------------------------------------------------------------------*/

bool
cbd496_FileHandling::InitializeFile(
    Trans_Context&  trans,
    cbd496_File&    file,
    tgg00_FileId&   doubleQueueFileId,
    Data_PageNo&    primQueuePno,
    Data_PageNo&    secQueuePno )
{
    // create a file and request for two new pages
    cbd495_DoubleWriteQueue tempInvQueue( this->GetResultFileCurrent( trans ));

    if( e_ok != trans.OldTrans().trError_gg00 ){
        return false;
    }
    primQueuePno      = tempInvQueue.bd495GetPrimPno();
    secQueuePno       = tempInvQueue.bd495GetSecPno();
    doubleQueueFileId = this->GetResultFileCurrent( trans ).curr_tree_id;

    file.AssignDoubleQueueFile( primQueuePno, secQueuePno );
    ++m_NumOpenQueuesFiles;
    return true;
}

/*---------------------------------------------------------------------------*/

void
cbd496_FileHandling::Merge(
    Trans_Context&              trans,
    const bool                  bLastMergeStep,
    const  FileList::IndexType  lastFileIndex,
    const cbd497_StackDescInfo& stackDescInfo )

{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd496_FileHandling::Merge", DataIndex_Trace, 5 );

    SAPDB_Int4 tempFileCount     = 0;
    SAPDB_Int4 tempFilePageCount = 0;

    // Initialize Funnel Merge by creating all InvQueues within given memory
    // via placement new. Only InvQueues are created having a corresponding
    // entry within the FileList.
    // Note that the loop first creates the result files and then the ordinary sort files.
    // This warrants that the result files are always createable!

    // Use SAPDB_Int instead of FileList::IndexType( SAPDB_UInt), because zero minus 1
    // will not be -1, if indexType is unsigned
    for( SAPDB_Int currFileIndex = lastFileIndex;  0 <= currFileIndex; --currFileIndex )
    {
        cbd496_File& file = m_FileList[ currFileIndex ];

        if( ! file.IsAssigned()){
            continue;
        }
        if( file.IsDoubleQueueFile()){
            m_Funnel.AddDoubleQueueFile( trans, file.GetPrimQueueRoot(), file.GetSecQueueRoot());
        }
        else{
            m_Funnel.AddTempInvFile( trans, file.GetTempInvRoot());
        }
        if( e_ok != trans.OldTrans().trError_gg00 ){
            break;
        }
        tempFileCount     += 1;
        tempFilePageCount += file.GetPageCount();
        this->DeleteFile( file ); // added files will be released by Funnel
    }

    // Because of the incredible size of a TempInvTree it is possible that bd494Add is not
    // be able to create all entries of the FileList within the given memory. Therefore another
    // Funnel Merge step with the remaining FileList entries is necessary.

    if( e_no_more_memory == trans.OldTrans().trError_gg00 ){
        trans.OldTrans().trError_gg00 = e_ok;
    }
    if( e_ok == trans.OldTrans().trError_gg00 )
    {
        RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_STAT_2,
                                     SAPDB_ToString( tempFilePageCount, _T_d ),
                                     SAPDB_ToString( tempFileCount, _T_d )));
        if( bLastMergeStep ){
            m_Funnel.Merge( trans, stackDescInfo, m_InvTree );
        }
        else
        {
            tgg00_FileId   dummyFileId;
            Data_PageNo    primQueuePno;
            Data_PageNo    secQueuePno;

            if(  this->InitializeFile( trans, m_FileList[ this->GetTargetFileIndex() ],
                                       dummyFileId, primQueuePno, secQueuePno ))
            {
                cbd494_SortableDoubleWriteQueue tempResultQueue(
                    this->GetResultFileCurrent( trans ), primQueuePno, secQueuePno );
                m_Funnel.Merge( trans, stackDescInfo, tempResultQueue );
            }
        }
    }
#ifdef SAPDB_SLOW
    if( DataIndex_Trace.TracesLevel(5))
    {
        Kernel_VTrace() <<  "> cbd496_FileHandling::Merge/Funnel() "
        << Kernel_BasisErrorToString( trans.OldTrans().trError_gg00 ) << FlushLine;
    }
#endif
}

/*---------------------------------------------------------------------------*/

tbd_current_tree&
cbd496_FileHandling::GetResultFileCurrent( Trans_Context& trans )
{
    // set correct trans into 'shared' current
    m_ResultFileCurrent.curr_trans = &trans.OldTrans();
    return m_ResultFileCurrent;
}

/*===========================================================================*
 *  DEFINITION OF METHODS DECLARED IN gbd496.h (CODE)                        * 
 *===========================================================================*/

cbd496_FileHandling::cbd496_FileHandling(
    SAPDBMem_IRawAllocator& allocator,
    cbd494_SortableInvTree& invTree )
        :
        m_NumResultFiles( 62 ),
        m_NumOverflowFiles( 2 ),
        m_InvTree( invTree ),
        m_TargetFileIndex( 0 ),
        m_NumOpenQueuesFiles( 0 ),
        m_NumOpenTreesFiles( 0 ),
        m_MergeCount( 0 ),
        m_FileList( allocator ),
        m_Funnel( allocator )
{
    m_ResultFileCurrent              = b01nilcurrent;
    m_ResultFileCurrent.curr_trans   = 0; // not set because session specific;
    m_ResultFileCurrent.curr_tree_id = bd495InitTempFileId( NIL_PAGE_NO_GG00 );
}

/*---------------------------------------------------------------------------*/

bool
cbd496_FileHandling::Initialize( const SAPDB_UInt maxResultFiles )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd496_FileHandling::Init", DataIndex_Trace, 5 );

    if( DataIndex_Trace.TracesLevel( 6 ))
    {
        const SAPDB_UInt requestedBytes = cbd496_File::GetSize() * maxResultFiles;
        Kernel_VTrace() << "> cbd496_FileHandling::Init() Requestes bytes: "
        << requestedBytes << " maxResultFiles: " << maxResultFiles << FlushLine;
    }
    if( ! m_FileList.Resize( maxResultFiles )){
        return false;
    }
    m_TargetFileIndex = this->GetLastSortFileIndex(); // Incremented before first use!

    if( ! m_Funnel.Initialize( maxResultFiles )){
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

bool
cbd496_FileHandling::ConvertFile(
    Trans_Context&      trans,
    const SAPDB_UInt    fileIndex,
    const Data_PageNo&  tempInvRoot )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd496_FileHandling::ConverterFile", DataIndex_Trace, 5 );

    // Drop all pages of the DoubleWriteQueue and replace the anchor
    // entry with the pno of the TempInvTree

    SAPDBERR_ASSERT_STATE( fileIndex <= this->GetLastSortFileIndex())

    cbd496_File& file = m_FileList[ fileIndex ];

    if( file.IsAssigned())
    {
        cbd495_DoubleWriteQueue tempInvQueue( this->GetResultFileCurrent( trans ),
                                              file.GetPrimQueueRoot(), file.GetSecQueueRoot());
        if( e_ok != trans.OldTrans().trError_gg00 ){
            return false;
        }
        tempInvQueue.bd495Free();
        --m_NumOpenQueuesFiles;
    }
    file.AssignTempInvFile( tempInvRoot );
    ++m_NumOpenTreesFiles;
    return true;
}

/*---------------------------------------------------------------------------*/

bool
cbd496_FileHandling::CreateFile(
    Trans_Context&  trans,
    SAPDB_UInt&     newFileIndex,
    tgg00_FileId&   queueId,
    Data_PageNo&    primQueueRoot,
    Data_PageNo&    secQueueRoot )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd496_FileHandling::CreateFile", DataIndex_Trace, 5 );

    FileList::IndexType         fileIndex     = this->GetFirstSortFileIndex();
    const FileList::IndexType	lastFileIndex = this->GetLastSortFileIndex();

    for( ; fileIndex <= lastFileIndex ; fileIndex++ )
    {
        cbd496_File& file = m_FileList[ fileIndex ];

        if( file.IsAssigned()){
            continue;
        }
        if( ! this->InitializeFile( trans, file, queueId, primQueueRoot, secQueueRoot )){
            break;
        }
        newFileIndex = fileIndex;
        return true;
    }
    trans.OldTrans().trError_gg00 = e_file_limit; // All files in use -> Merge files and try again

#ifdef SAPDB_SLOW
    if( DataIndex_Trace.TracesLevel(5))
    {
        Kernel_VTrace() <<  "> cbd496_FileHandling::CreateFile() " <<
        Kernel_BasisErrorToString( trans.OldTrans().trError_gg00 ) << FlushLine;
    }
#endif

    return false;
}

/*---------------------------------------------------------------------------*/

void
cbd496_FileHandling::DeleteAllFiles( Trans_Context&  trans )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd496_FileHandling::DeleteFiles", DataIndex_Trace, 5 );

    FileList::Iterator        iter;
    FileList::ConstIterator   endIter = m_FileList.End();

    for( iter = m_FileList.Begin(); iter != endIter; iter++ ){
        this->DeleteFile( trans, m_FileList.GetIndex( iter ));
    }
}

/*---------------------------------------------------------------------------*/

void
cbd496_FileHandling::DeleteFile(
    Trans_Context&     trans,
    const SAPDB_UInt   fileIndex )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd496_FileHandling::DeleteFile", DataIndex_Trace, 5 );

    SAPDBERR_ASSERT_STATE( fileIndex <= this->GetLastIndex())

    cbd496_File& file = m_FileList[ fileIndex ];

    if( ! file.IsAssigned()){
        return;
    }
    const tgg00_BasisError  auxError = trans.OldTrans().trError_gg00;
    trans.OldTrans().trError_gg00 = e_ok;

    if( file.IsDoubleQueueFile())
    {
        // Drop DoubleWriteQueue
        cbd495_DoubleWriteQueue tempInvQueue( this->GetResultFileCurrent( trans ),
                                              file.GetPrimQueueRoot(), file.GetSecQueueRoot());
        if( e_ok != trans.OldTrans().trError_gg00 ){
            return;
        }
        tempInvQueue.bd495Free();
    }
    else
    {
        // Drop TempInvTree
        tgg00_FileId    tempFileId = bd495InitTempFileId( file.GetTempInvRoot());

        cbd495_TempInvTree tempInvTree( trans.OldTrans(), tempFileId, DROP_TREE_BD495 );
        if( e_ok != trans.OldTrans().trError_gg00 ){
            return;
        }
        tempInvTree.bd495Free();
    }
    this->DeleteFile( file );
    trans.OldTrans().trError_gg00 = auxError;
}

/*---------------------------------------------------------------------------*/

void
cbd496_FileHandling::SetFilePageCount(
    const SAPDB_UInt    fileIndex,
    const SAPDB_Int4    pageCount )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd496_FileHandling::SetFilePageCount", DataIndex_Trace, 5 );

    SAPDBERR_ASSERT_STATE( fileIndex <= this->GetLastIndex())

    if(  m_FileList[ fileIndex ].IsAssigned()){
        m_FileList[ fileIndex ].SetPageCount( pageCount );
    }
}

/*---------------------------------------------------------------------------*/

void
cbd496_FileHandling::MergeFiles (
    Trans_Context&              trans,
    const pasbool               bNoMorePrimData,
    const cbd497_StackDescInfo& stackDescInfo,
    tgg00_IndexCounter&         indexCounter)
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd496_FileHandling::MergeFiles", DataIndex_Trace, 5 );

    bool                    bLastMergeStep = false;
    FileList::IndexType     lastFileIndex;

    trans.OldTrans().trError_gg00 = e_ok; // base error could be only e_file_limit

    {
        Data_Exception  infoMsg( __CONTEXT__, DATA_CREATE_INDEX_STAT_1,
                                 SAPDB_ToString( indexCounter.idc_prim_leafnodes, _T_d ),
                                 SAPDB_ToString( indexCounter.idc_prim_keycount, _T_d ));
        if( bNoMorePrimData ){
            infoMsg.AppendNewMessage( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_STAT_INTRO_1 ));
        }
        else {
            infoMsg.AppendNewMessage( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_STAT_INTRO_2 ));
        }
        RTE_Message( infoMsg );
    }
    do
    {
        ++m_MergeCount;

        if( bNoMorePrimData )
        {
            // The decision if the last Funnel Merge step could be started depends
            // on two factors. First: all primary data is scaned. Second: all object
            // handler for the FileList could be instantiated in the given memory.
            bLastMergeStep = m_Funnel.MemoryAvailable( m_NumOpenQueuesFiles, m_NumOpenTreesFiles );
        }
        if( bLastMergeStep ){
            RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_FINAL_MERGE_1 ));
        }
        else{
            RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_INTERIM_MERGE_1 ));
        }
        if( bLastMergeStep ){
            lastFileIndex = this->GetLastIndex();
        }
        else
        {
            // Determine TargetFileIndex
            // FileList has an overflow therefore a funnel merge step is
            // necessary to obtain new free FileList entries.

            if (this->GetTargetFileIndex() < this->GetLastResultFileIndex())
            {
                ++m_TargetFileIndex;
                lastFileIndex = this->GetLastSortFileIndex();
            }
            else
            {
                // All result files are full therefore the next funnel
                // step has to merge both ordinary sort files and result
                // files into an overflow result file.

                lastFileIndex = this->GetLastIndex();
                const FileList::IndexType firstOverflowFileIndex = this->GetFirstOverflowFileIndex();

                cbd496_File& file = m_FileList[ firstOverflowFileIndex ];

                if( file.IsAssigned()){
                    m_TargetFileIndex = this->GetLastOverflowFileIndex();
                }
                else{
                    m_TargetFileIndex = this->GetFirstOverflowFileIndex();
                }
            }
        }
        if( e_ok == trans.OldTrans().trError_gg00 )
        {
            if( 1 < m_MergeCount ){
                m_Funnel.ReInitialize(); // reinitialize if it is used more than one time
            }
            this->Merge( trans, bLastMergeStep, lastFileIndex, stackDescInfo );
            if( e_ok != trans.OldTrans().trError_gg00 ){
                // Remove files which are only in the funnel sort area and
                // not in the m_FileList anymore
                m_Funnel.RemoveAddedFiles( trans );
            }
        }
        if (
            ( e_ok == trans.OldTrans().trError_gg00 ) &&
            (
                ( this->GetTargetFileIndex() == this->GetFirstOverflowFileIndex()) ||
                ( this->GetTargetFileIndex() == this->GetLastOverflowFileIndex() )
            )
        )
            m_TargetFileIndex = this->GetLastSortFileIndex(); // incremented before first use

        if( bLastMergeStep ){
            RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_FINAL_MERGE_2,
                                         SAPDB_ToString( m_MergeCount, _T_d )));
        }
        else{
            RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_INTERIM_MERGE_2,
                                         SAPDB_ToString( m_MergeCount, _T_d )));
        }
    }
    while(( bNoMorePrimData ) && ( !bLastMergeStep ) && ( e_ok == trans.OldTrans().trError_gg00 ));

    if( e_sysbuffer_overflow == trans.OldTrans().trError_gg00 ){
        RTE_Message( Data_Exception( __CONTEXT__, DATA_WRN_CREATE_INDEX_FUNNEL_OVERFLOW_1 ));

    }
    if( e_no_more_memory == trans.OldTrans().trError_gg00 ){
        RTE_Message( Data_Exception( __CONTEXT__, DATA_WRN_CREATE_INDEX_FUNNEL_MEMORY_1 ));
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

