/*!
  @file           IOMan_DataVolume.cpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This module is used to handle a data volume.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c)  2000-2006 SAP AG

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

#include "IOManager/IOMan_DataVolume.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "IOManager/IOMan_ComplexClusterAddress.hpp"
#include "IOManager/IOMan_Exception.hpp"
#include "IOManager/IOMan_Messages.hpp"
#include "Messages/Msg_IOutput.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/Messages/SDBMsg_IOMan.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


static void
IOMan_WriteMessageListIntoArchive( Msg_List& msgList )
{
    // only for 7.6 as long as Msg_IOutput did not write into knldiag and knldiag.err
    RTE_Message( msgList );

    Msg_List auxMsgList;
    if( Msg_IOutput::GetDiagnosticOutput().Output( msgList, auxMsgList, Msg_IOutput::alwaysToArchive )){
        return;
    }
    auxMsgList.Clear(); // ignore error
}

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS  -lll-                                                           *
 *===========================================================================*/

bool
IOMan_DataVolume::ReadPages(
    RTETask_ITask&                      task,
    Msg_List&                           msgList,
    IOMan_IKernelPages&                 pages,
    const SAPDB_UInt                    startIndex,
    const IOMan_ComplexClusterAddress&  cluster,
    SAPDB_UInt&                         badPageIndex )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataVolume::ReadPages", IOMan_Trace, 8 );

    SAPDBERR_ASSERT_STATE( cluster.GetVolumeNo() == GetLogicalDevNo());
    SAPDBERR_ASSERT_STATE( 0 != *( pages.GetFrameList( startIndex )));

    Msg_List    auxList;
    bool        bOkay         = false;
    SAPDB_UInt  badBlockIndex = 0;
    SAPDB_UInt  readCount     = 1;
    for( readCount = 1; IOMan_Volume::MaxReadRetries() >= readCount; ++readCount )
    {
        if( ! IOMan_Volume::ReadSimple( task.ID(), *( pages.GetFrameList( startIndex )),
                                        cluster.GetBlockNo( 0 ), cluster.GetBlockCount()))
        {
            IOMan_Exception errMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
            RTE_Crash( errMsg );
        }
        if( ExecuteIOCheck( task, auxList, pages, startIndex, cluster, badPageIndex, badBlockIndex )){
            bOkay = true;
            break; // io check successful executed
        }
        msgList.Overrule( auxList );
    }
    if( ! bOkay )
    {
        msgList.MarkAsSubMessages();
        msgList.Overrule(
            Msg_List( Msg_List::Error, SDBMSG_IOMAN_BAD_PAGE,
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_PAGE__VOLUME_TYPE, GetTypeName()),
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_PAGE__VOLUME_ID,GetLogicalDevNo()),
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_PAGE__BLOCK_NO, cluster.GetBlockNo( badBlockIndex )),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__VOLUME_ID_1, cluster.GetVolumeNo()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__BLOCK_NO_1, cluster.GetBlockNo( 0 )),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__NUMBER, cluster.GetBlockCount()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__NUMBER1, cluster.GetUsedBlocks()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__NUMBER2, startIndex ),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__NUMBER3, badPageIndex )));
        return false;
    }
    if( readCount > 1 )
    { // it takes more than one attempt to read the requested pages but NOW the pages are correct
        msgList = Msg_List(
                      Msg_List::Warning, SDBMSG_IOMAN_MULTIPLE_FAILED_READS,
                      Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__NUMBER,      cluster.GetBlockCount()),
                      Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__VOLUME_TYPE, GetTypeName()),
                      Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__VOLUME_ID,   GetLogicalDevNo()),
                      Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__BLOCK_NO,    cluster.GetBlockNo( 0 )),
                      Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__NUMBER1,     readCount ));
        IOMan_WriteMessageListIntoArchive( msgList );
        msgList.Clear();
    }
    return true;
}

/*---------------------------------------------------------------------------*/

IOMan_DataVolume::AsynReadPagesRc
IOMan_DataVolume::AsynReadPages(
    RTETask_ITask&                      task,
    Msg_List&                           msgList,
    const SAPDB_Int4                    asynIOHandle,
    IOMan_IKernelPages&                 pages,
    const SAPDB_UInt                    startIndex,
    const IOMan_ComplexClusterAddress&  cluster )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataVolume::AsynRead", IOMan_Trace, 8 );

    SAPDBERR_ASSERT_STATE( cluster.GetVolumeNo() == this->GetLogicalDevNo());
    SAPDBERR_ASSERT_STATE( 0 != *( pages.GetFrameList( startIndex )));

    Msg_List    auxList;
    bool        bOkay          = false;
    SAPDB_UInt  badPageIndex   = 0;
    SAPDB_UInt  badBlockIndex  = 0;
    SAPDB_UInt  readCount      = 1;
    for( readCount = 1; IOMan_Volume::MaxReadRetries() >= readCount; ++readCount )
    {
        const IOMan_Volume::AsynReadWriteRc ioRc = IOMan_Volume::AsynReadWrite(
                    task, msgList, asynIOHandle, pages, startIndex, cluster );

        if( IOMan_Volume::arwRcOkay != ioRc ){
            return ( AsynReadPagesRc )ioRc; // read attempt failed
        }
        if( ExecuteIOCheck( task, auxList, pages, startIndex, cluster, badPageIndex, badBlockIndex )){
            bOkay = true;
            break; // io check successful executed
        }
        msgList.Overrule( auxList ); // with each unsuccessful check the defect will be appended
    }
    if( ! bOkay )
    {
        msgList.MarkAsSubMessages();
        msgList.Overrule(
            Msg_List( Msg_List::Error, SDBMSG_IOMAN_BAD_PAGE,
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_PAGE__VOLUME_TYPE, GetTypeName()),
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_PAGE__VOLUME_ID,GetLogicalDevNo()),
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_PAGE__BLOCK_NO, cluster.GetBlockNo( badBlockIndex )),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__VOLUME_ID_1, cluster.GetVolumeNo()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__BLOCK_NO_1, cluster.GetBlockNo( 0 )),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__NUMBER, cluster.GetBlockCount()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__NUMBER1, cluster.GetUsedBlocks()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__NUMBER2, startIndex ),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_PAGE__NUMBER3, badPageIndex )));
        return arpRcIoCheckFailed;
    }
    if( readCount > 1 )
    { // it takes more than one attempt to read the requested pages but NOW the pages are correct
        msgList.Overrule(
            Msg_List(
                Msg_List::Warning, SDBMSG_IOMAN_MULTIPLE_FAILED_READS,
                Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__NUMBER,      cluster.GetBlockCount()),
                Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__VOLUME_TYPE, GetTypeName()),
                Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__VOLUME_ID,   GetLogicalDevNo()),
                Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__BLOCK_NO,    cluster.GetBlockNo( 0 )),
                Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__NUMBER1,     readCount )));
        IOMan_WriteMessageListIntoArchive( msgList );
        msgList.Clear();
    }
    return arpRcOkay;
}

/*---------------------------------------------------------------------------*/

IOMan_DataVolume::AsynWritePagesRc
IOMan_DataVolume::AsynWritePages(
    RTETask_ITask&                      task,
    Msg_List&                           msgList,
    const SAPDB_Int4                    asynIOHandle,
    IOMan_IKernelPages&                 pages,
    const SAPDB_UInt                    startIndex,
    const IOMan_ComplexClusterAddress&  cluster,
    SAPDB_UInt&                         badPageIndex )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataVolume::AsynWrite", IOMan_Trace, 8 );

    SAPDBERR_ASSERT_STATE( cluster.GetVolumeNo() == this->GetLogicalDevNo());
    SAPDBERR_ASSERT_STATE( 0 != *( pages.GetFrameList( startIndex )));

    SAPDB_UInt  badBlockIndex = 0;

    if( ! ExecuteIOCheck( task, msgList, pages, startIndex, cluster, badPageIndex, badBlockIndex ))
    {
        msgList.Overrule(
            Msg_List( Msg_List::Error, SDBMSG_IOMAN_BAD_OUTPUT_PAGE,
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__VOLUME_TYPE, GetTypeName()),
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__VOLUME_ID,GetLogicalDevNo()),
                      Msg_Arg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__BLOCK_NO, cluster.GetBlockNo( badBlockIndex )),

                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__VOLUME_ID_1, cluster.GetVolumeNo()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__BLOCK_NO_1, cluster.GetBlockNo( 0 )),

                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__NUMBER, cluster.GetBlockCount()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__NUMBER1, cluster.GetUsedBlocks()),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__NUMBER2, startIndex ),
                      Msg_OptArg(SDBMSGTAG_IOMAN_BAD_OUTPUT_PAGE__NUMBER3, badPageIndex )));

        return awpRcIoCheckFailed;
    }
    const IOMan_Volume::AsynReadWriteRc ioRc = IOMan_Volume::AsynReadWrite(
                task, msgList, asynIOHandle, pages, startIndex, cluster );

    if( IOMan_Volume::arwRcOkay != ioRc ){
        return ( AsynWritePagesRc )ioRc; // write attempt failed
    }
    return awpRcOkay;
}

/*---------------------------------------------------------------------------*/
/* PRIVATE METHODS                                                           */
/*---------------------------------------------------------------------------*/

bool
IOMan_DataVolume::ExecuteIOCheck(
    RTETask_ITask&                      task,
    Msg_List&                           msgList,
    IOMan_IKernelPages&                 pages,
    const SAPDB_UInt                    startIndex,
    const IOMan_ComplexClusterAddress&  cluster,
    SAPDB_UInt&                         badPageIndex,
    SAPDB_UInt&                         badBlockIndex )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataVolume::ExecuteIOCheckAfterRead", IOMan_Trace, 9 );

    IOMan_IKernelPagesIterator          iter( pages );
    IOMan_ClusterAddress::BlockIndex    blockIndex = 0;

    for( blockIndex = 0, iter.Begin( startIndex ); blockIndex < cluster.GetBlockCount();
            ++iter, ++blockIndex )
    {
        // Caused by performance reason it is possible that more data pages are processed
        // by this cluster io than marked as io pending in the data cache.
        // The io check must not (!) be executed for the pages which are not marked as
        // io pending in the data cache, because this pages may be changed during this
        // cluster io, because nothing prevents changes on the 'unsed/empty' pages in
        // he data area.
        if( ! cluster.IsBlockInUse( blockIndex )){
            continue;
        }
        SAPDBERR_ASSERT_STATE(( *iter ).IsAssigned());
        if(( *iter ).CheckAfterRead()){  // prepare future use of msgList
            continue;
        }
        {
            Kernel_FileIO   dumpFile( GetTypeName(), KERNEL_BAD_FILE_EXTENSION,
                                      SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                      SAPDB_ToString( cluster.GetBlockNo( blockIndex ), _T_d ));
            dumpFile.Write( *iter );
            dumpFile.Close();
        }
        badPageIndex  = iter.GetIndex(); // index in page collection of corrupted block
        badBlockIndex = blockIndex;      // index in cluster of corrupted block
        task.Sleep( IOMan_Volume::RetrySleepTime());
        return false;
    }
    return true;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

