/*****************************************************************************/
/*!
  @file         Data_UpdStatRequestFile.cpp
  @author       MartinKi
  @ingroup

  @brief        Implementation of class Data_UpdStatRequestFile.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"

#include "DataAccess/Data_UpdStatRequestFile.hpp"
#include "DataAccess/Data_Types.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Data.h"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "Messages/Msg_List.hpp"
#include "RunTime/System/RTESys_Time.h"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hbd02.h"
#include "hbd01.h"
#include "hbd998.h"
#include "hgg04.h"
#include "ggg00.h"
#include "heo52.h" // vtimestamp
#include "hsp41.h" // s41p4int

const char* c_fileComment = "UpdStatRequestFile";

/**************************************************************************/

Data_UpdStatRequestFile::Data_UpdStatRequestFile()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Data_UpdStatRequestFile", DataTree_Trace, 5 );
}

/**************************************************************************/

FileDir_FileNo Data_UpdStatRequestFile::Create(
    tgg00_TransContext& trans,
    Msg_List&           errMsg)

{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Create", DataTree_Trace, 5 );

    tgg00_FileId fileId;
    g04build_temp_tree_id( fileId, trans );
    fileId.fileType_gg00().addElement( ftsConcurrent_egg00 );
    fileId.fileType_gg00().addElement( ftsShared_egg00 );

    bd998CreateTempFile( trans, fileId );
    if ( trans.trError_gg00 == e_ok ) {
        FileDir_FileNo fileNo( fileId.fileTabId_gg00() );
        FileDir_IDirectory& fDir =
            FileDir_IDirectory::Instance( trans.trIndex_gg00 );
        fDir.GetTableFile( trans.trTaskId_gg00, fileNo, m_file, errMsg );
        m_file.SetComment( c_fileComment );

        m_file.BuildFileId( m_fileId );
    } else {
        m_file.Detach();
    }

    if ( m_file.IsAssigned() ) {
        return m_file.GetFileNo();
    } else {
        return FileDir_FileNo();
    }
}

/**************************************************************************/

Data_UpdStatRequestFile::ReturnCode Data_UpdStatRequestFile::Open(
    tgg00_TransContext&   trans,
    const FileDir_FileNo& fileNo,
    Msg_List&             errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Open", DataTree_Trace, 5 );

    if ( m_file.IsAssigned() ) {
        m_file.Detach();
    }

    FileDir_IDirectory& fDir =
        FileDir_IDirectory::Instance( trans.trIndex_gg00 );
    fDir.GetTableFile( trans.trTaskId_gg00, fileNo, m_file, errMsg );

    if ( ! m_file.IsAssigned() ) {
        return FileNotFound;
    }

    m_file.BuildFileId( m_fileId );

    SAPDBERR_ASSERT_STATE( m_file.GetComment() == c_fileComment );
    return Okay;
}

/**************************************************************************/

Data_UpdStatRequestFile::ReturnCode Data_UpdStatRequestFile::Destroy(
    tgg00_TransContext&   trans,
    Msg_List&             errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Destroy", DataTree_Trace, 5 );

    if ( ! m_file.IsAssigned() ) {
        return FileNotFound;
    }

    tgg00_Surrogate fileSurrogate;
    fileSurrogate.rawAssign( m_file.GetFileNo().GetCharArray() );
    bd998DestroyTempFile ( trans, fileSurrogate );

    trans.trError_gg00 = e_ok;
    return Okay;
}

/**************************************************************************/

FileDir_FileNo Data_UpdStatRequestFile::GetFileNo() const
{
    if ( ! m_file.IsAssigned() ) {
        // return invalid fileno
        return FileDir_FileNo();
    }

    return m_file.GetFileNo();
}

/**************************************************************************/

Data_UpdStatRequestFile::ReturnCode Data_UpdStatRequestFile::AddTable(
    tgg00_TransContext&     transContext,
    const FileDir_FileNo&   tableFileNo,
    const SAPDB_Int4        noOfPagesToSample,
    Msg_List&               errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::AddTable", DataTree_Trace, 5 );

    RequestRecord requestRec;

    // get table size
    FileDir_Table tableFile;
    FileDir_IDirectory& fDir =
        FileDir_IDirectory::Instance( transContext.trIndex_gg00 );
    fDir.GetTableFile( transContext.trTaskId_gg00,
                       tableFileNo, tableFile, errMsg );

    if ( ! tableFile.IsAssigned() ) {
        return FileNotFound;
    }

    const pasbool c_countRecords = true;
    SAPDB_Int4    leafPageCount;
    SAPDB_Int4    dummyCount;
    tgg00_FileId  tableFileId;
    tableFile.BuildFileId( tableFileId );

    bd01GetLeafPageAndRecordCount(
        transContext, tableFileId, ! c_countRecords, leafPageCount, dummyCount);

    if ( transContext.trError_gg00 != e_ok ) {
        transContext.trError_gg00 = e_ok;

        if ( tableFile.GetFileState() == Data_FileDeleted ) {
            return FileNotFound;
        }

        errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_DATA_UPDSTAT_GETTABLESIZE_FAILED,
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_GETTABLESIZE_FAILED__FILENO,
                         FileDir_FileNoToString( tableFileNo ) )
            );
        return SystemError;
    }
    // end of get table size

    requestRec.keyLen = sizeof( requestRec.tableFileSize ) + sizeof( tableFileNo );

    requestRec.varColOffset = 0;
    requestRec.varColCount  = 0;

    requestRec.recLen = sizeof( RequestRecord );

    // insert table size (key column)
    tsp00_NumError numErr;
    s41p4int(
        &requestRec,
        RTE_OFFSET_OF( RequestRecord, tableFileSize ) + 1,
        leafPageCount, numErr );
    SAPDBERR_ASSERT_STATE( numErr == num_ok );

    // insert table file no (key column)
    SAPDB_MemCopyNoCheck(
        requestRec.tableFileNo, tableFileNo.GetCharArray(), sizeof( tableFileNo ) );

    // insert sample size
    SAPDB_MemCopyNoCheck(
        &requestRec.pageCntToSample, &noOfPagesToSample,
        sizeof( requestRec.pageCntToSample ) );

    // insert status (not done)
    requestRec.isDone = 0;

    // now insert record
    b02add_record( transContext, m_fileId,
                   *reinterpret_cast<tgg00_Rec*>( &requestRec ) );
    if ( transContext.trError_gg00 != e_ok ) {
        SAPDBERR_ASSERT_STATE( transContext.trError_gg00 == e_duplicate_key );
        transContext.trError_gg00 = e_ok;
    }

    return Okay;
}

/**************************************************************************/

Data_UpdStatRequestFile::Iterator Data_UpdStatRequestFile::GetIterator(
    tgg00_TransContext& trans)
{
    return Iterator( trans, *this );
}

/**************************************************************************/

Data_UpdStatRequestFile::Iterator::Iterator(
    tgg00_TransContext&      trans,
    Data_UpdStatRequestFile& file)
    : m_file( file )
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Iterator", DataTree_Trace, 5 );

    m_curKey.keyLen_gg00() = PERM_KEY_MXSP00;
    m_curKey.keyVal_gg00() = b01fullkey;

}

/**************************************************************************/

Data_UpdStatRequestFile::Iterator::Iterator(const Iterator& it)
    : m_file( it.m_file ),
      m_curKey( it.m_curKey )
{
    if ( it.IsValid() ) {

        Msg_List errMsg;
        SAPDB_MemCopyNoCheck(
            &m_requestRec, &it.m_requestRec, sizeof( m_requestRec ) );

        this->ParseRecord( errMsg );
    } else {
        m_curKey.keyLen_gg00() = PERM_KEY_MXSP00;
        m_curKey.keyVal_gg00() = b01fullkey;
    }
}

/**************************************************************************/

void Data_UpdStatRequestFile::Iterator::SetStartKey()
{
    m_curKey.keyLen_gg00() = PERM_KEY_MXSP00;
    m_curKey.keyVal_gg00() = b01fullkey;
}

/**************************************************************************/

void Data_UpdStatRequestFile::Iterator::SetEndKey()
{
    m_curKey.keyLen_gg00() = 0;
}

/**************************************************************************/

SAPDB_Bool Data_UpdStatRequestFile::Iterator::IsValid() const
{
    return
        (m_curKey.keyLen_gg00() != PERM_KEY_MXSP00)
        && (m_curKey.keyLen_gg00() != 0);
}

/**************************************************************************/

void Data_UpdStatRequestFile::Iterator::SetFirst()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::SetFirst", DataTree_Trace, 5 );

    this->SetStartKey();
}

/**************************************************************************/

Data_UpdStatRequestFile::Iterator::ReturnCode
Data_UpdStatRequestFile::Iterator::Next(
    tgg00_TransContext& trans,
    Msg_List&           errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Next", DataTree_Trace, 5 );

    // invalidate current entry
    m_tableKey.tableFileNo.Invalidate();

    const pasbool c_inclusive = true;
    tgg00_Rec recBuf;

    b02prev_record(
        trans, m_file.m_fileId, m_curKey, ! c_inclusive, recBuf );

    if ( trans.trError_gg00 == e_no_prev_record ) {
        trans.trError_gg00 = e_ok;
        this->SetStartKey();
        return EndOfFile;
    }

    if ( (trans.trError_gg00 != e_ok)
         && (trans.trError_gg00 != e_key_not_found) ) {
        errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_DATA_UPDSTAT_POSITION_ITERATOR_FAILED,
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_POSITION_ITERATOR_FAILED__FILENO,
                         FileDir_FileNoToString( m_file.GetFileNo() ) ),
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_POSITION_ITERATOR_FAILED__RETURNCODE,
                         SQLManErr_Interface::GetInstance().GetBaseErrorText(
                             trans.trError_gg00) )
            );
        trans.trError_gg00 = e_ok;
        this->SetEndKey();
        return SystemError;
    }

    trans.trError_gg00 = e_ok;

    // update key
    SAPDB_MemCopy(
        &m_curKey, recBuf.recBuf_gg00(),
        recBuf.recKeyLen_gg00() + cgg_rec_key_offset,
        "Data_UpdStatRequestFile", __FILE__, __LINE__, errMsg );

    SAPDBERR_ASSERT_STATE( recBuf.recLen_gg00() == sizeof( RequestRecord ) );

    SAPDB_MemCopyNoCheck(
        &m_requestRec, recBuf.recBuf_gg00(), sizeof( RequestRecord ) );

    if ( ! this->ParseRecord( errMsg ) ) {
        return SystemError;
    }

    return Okay;
}

/**************************************************************************/

SAPDB_Int4
Data_UpdStatRequestFile::Iterator::GetNoOfPagesToSample() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Iterator::GetNoOfPagesToSample",
        DataTree_Trace, 5 );

    return m_pageCntToSample;
}

/**************************************************************************/

Data_UpdStatRequestFile::Iterator::ReturnCode
Data_UpdStatRequestFile::Iterator::SetRequestProcessed(
    tgg00_TransContext& trans,
    const SAPDB_Bool    isProcessed,
    Msg_List&           errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Iterator::SetBeginTimeStamp",
        DataTree_Trace, 5 );

    if ( isProcessed ) {
        m_requestRec.isDone = 1;
    } else {
        m_requestRec.isDone = 0;
    }

    m_isDone = ( m_requestRec.isDone != 0 );

    return this->UpdateRecord( trans, errMsg );
}

/**************************************************************************/

SAPDB_Bool Data_UpdStatRequestFile::Iterator::ParseRecord(
    Msg_List& errMsg)
{
    SAPDB_MemCopyNoCheck(
        m_tableKey.tableFileSize, m_requestRec.tableFileSize,
        sizeof( m_requestRec.tableFileSize ) );

    tgg00_Surrogate tableSurrogate;
    SAPDB_MemCopyNoCheck(
        &tableSurrogate, m_requestRec.tableFileNo, sizeof( m_requestRec.tableFileNo ) );
    m_tableKey.tableFileNo = tableSurrogate;

    // page count
    SAPDB_MemCopyNoCheck(
        &m_pageCntToSample, &m_requestRec.pageCntToSample,
        sizeof( m_pageCntToSample ) );

    // isDone
    m_isDone = (m_requestRec.isDone == 1);

    return true;
}

/**************************************************************************/

Data_UpdStatRequestFile::Iterator::ReturnCode
Data_UpdStatRequestFile::Iterator::Delete(
    tgg00_TransContext& trans,
    Msg_List&           errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Iterator::Delete",
        DataTree_Trace, 5 );

    if ( ! this->IsValid() ) {
        return KeyNotFound;
    }

    // delete current record, returns true, if the iterator is still valid
    b02del_record( trans, m_file.m_fileId, m_curKey );

    SAPDBTRACE_WRITELN(
        DataTree_Trace, 5,
        "del returned:"
        << SQLManErr_Interface::GetInstance().GetBaseErrorText(
            trans.trError_gg00) );

    if ( (trans.trError_gg00 != e_ok)
         && (trans.trError_gg00 != e_key_not_found) ) {
        errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_DATA_UPDSTAT_DELETE_ENTRY_FAILED,
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_DELETE_ENTRY_FAILED__FILENO,
                         FileDir_FileNoToString( m_file.GetFileNo() ) ),
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_DELETE_ENTRY_FAILED__KEYASHEX,
                         m_curKey.keyVal_gg00(),
                         m_curKey.keyLen_gg00(),
                         m_curKey.keyLen_gg00(),
                         SAPDB_ToStringClass::hex ),
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_DELETE_ENTRY_FAILED__RETURNCODE,
                         SQLManErr_Interface::GetInstance().GetBaseErrorText(
                             trans.trError_gg00) )
            );
        SAPDBERR_ASSERT_STATE( false );
        trans.trError_gg00 = e_ok;
        return SystemError;
    }
    trans.trError_gg00 = e_ok;

    return this->Next( trans, errMsg );
}

/**************************************************************************/

Data_UpdStatRequestFile::Iterator::ReturnCode
Data_UpdStatRequestFile::Iterator::UpdateRecord(
    tgg00_TransContext& trans,
    Msg_List&           errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdStatRequestFile::Iterator::UpdateRecord",
        DataTree_Trace, 5 );

    if ( ! this->IsValid() ) {
        return KeyNotFound;
    }

    // update file
    b02repl_record( trans, m_file.m_fileId,
                    *reinterpret_cast<tgg00_Rec*>( &m_requestRec ) );

    if ( trans.trError_gg00 != e_ok ) {
        errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_DATA_UPDSTAT_UPDATERECORD_FAILED,
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_UPDATERECORD_FAILED__FILENO,
                         FileDir_FileNoToString( m_file.GetFileNo() ) ),
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_UPDATERECORD_FAILED__KEYASHEX,
                         m_curKey.keyVal_gg00(),
                         m_curKey.keyLen_gg00(),
                         m_curKey.keyLen_gg00(),
                         SAPDB_ToStringClass::hex ),
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_UPDATERECORD_FAILED__RETURNCODE,
                         SQLManErr_Interface::GetInstance().GetBaseErrorText(
                             trans.trError_gg00) )
            );
        trans.trError_gg00 = e_ok;
        return SystemError;
    }
    return Okay;
}
