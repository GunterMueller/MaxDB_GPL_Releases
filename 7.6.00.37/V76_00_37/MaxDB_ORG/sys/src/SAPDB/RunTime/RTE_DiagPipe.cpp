/*!---------------------------------------------------------------------
  @file           RTE_DiagPipe.cpp
  @author         RobinW
  @brief          Kernel RunTime: Message Output
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
---------------------------------------------------------------------*/


/* during kernel startup, messages given by MSGD are also written to a pipe that is read by dbmcli */

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/RTE_Types.h"
#include "RunTime/System/RTESys_IO.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/Communication/RTEComm_PacketHeader.h"
#include "RunTime/RTE_DiagPipe.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "heo07.h"

#define RTE_DIAGPIPE_PROTOCOL_VERSION   1

#define RTE_DIAGPIPE_DATA_PACKET        1
#define RTE_DIAGPIPE_KERNEL_REF         1
#define RTE_DIAGPIPE_DBM_REF            2

static volatile tRTESys_IOHandle pipeFileDes = RTE_UNDEF_HANDLE;
static teo07_Mutex diagPipeMutex;

#ifdef _WIN32
#define _PIPE_PREFIX "\\\\.\\pipe\\"
#else
#include "gen41.h"    /* nocheck */
#include "sys/poll.h" /* nocheck */
#endif

// functions for the kernel

typedef struct
{
    SAPDB_Byte header[PACKET_HEADER_BUFFER_SIZE];
    SAPDB_Byte              data[1];
}
pipePacket;

//-----------------------------------------------------------

externC void RTE_ConnectToDiagPipe(SAPDB_Char *dbName)
{
    tRTESys_IOHandle diagPipe;
    sqlcreatemutex(&diagPipeMutex);
#ifdef _WIN32   /* Windows only. On Unix, the kernel inherits a handle to the pipe from the dbm */
    tsp00_VfReturn_Param returnStatus;

    SAPDB_Int4 pipeNameLength = (SAPDB_Int4)(strlen(dbName)+strlen(_PIPE_PREFIX)+1);
    SAPDB_Char *pipeName = (SAPDB_Char *)alloca(pipeNameLength);

    SAPDB_sprintf(pipeName,pipeNameLength,"%s%s",_PIPE_PREFIX,dbName);
    RTESys_IOOpen(diagPipe,pipeName,RTESys_IOWriteOnly,false,NULL,returnStatus);

    if(vf_ok != returnStatus)
        return;
#else
    if((0 != en41GetPipeFDFromFile(dbName,&diagPipe)) || (0 == diagPipe))   /* if the pipe file does not exist, en41GetPipeFDFromFile() returns 0 and diagPipe is 0 */
        return;
#endif

    pipeFileDes = diagPipe;
}

//-----------------------------------------------------------

/* write to the pipe if it's open */
externC void RTE_WriteToDiagPipe( SAPDB_Char *message, SAPDB_Int length )
{
    // a dirty read: the dangerous case is some other thread closing the handle while we are in this routine
    if( RTE_UNDEF_HANDLE != pipeFileDes )   
    {
        tRTESys_IOPosition written;
        tsp00_VfReturn_Param returnStatus;
        
        sqlbeginmutex(&diagPipeMutex);
        if(RTE_UNDEF_HANDLE != pipeFileDes) // make sure that the handle is really open - it won't change between sqlbeginmutex() and sqlendmutex(), but might have changed before the sqlbeginmutex()
        {
            void *spaceForPipePacket = alloca(PACKET_HEADER_BUFFER_SIZE + length);

            RTEComm_PacketHeader header(spaceForPipePacket) ;
            header.Set( RTE_DIAGPIPE_DATA_PACKET,
                        RTE_DIAGPIPE_KERNEL_REF,
                        RTE_DIAGPIPE_DBM_REF,
                        length,
                        Prot_DiagPipe,
                        RTE_DIAGPIPE_PROTOCOL_VERSION);

            SAPDB_MemCopyNoCheck(((pipePacket *)spaceForPipePacket)->data,message,length);

            RTESys_IOWrite ( pipeFileDes, spaceForPipePacket, PACKET_HEADER_BUFFER_SIZE + length, written, returnStatus );
        }
        sqlendmutex(&diagPipeMutex);
    }
}

//-----------------------------------------------------------

/* close the pipe and invalidate the file descriptor */
externC void RTE_CloseDiagPipe(void)
{
    sqlbeginmutex(&diagPipeMutex);
    tsp00_VfReturn_Param returnStatus;

    if(RTE_UNDEF_HANDLE != pipeFileDes)
    {
        RTESys_IOClose(pipeFileDes,returnStatus );
        pipeFileDes = RTE_UNDEF_HANDLE;
    }
    sqlendmutex(&diagPipeMutex);
}

externC SAPDB_Bool RTE_DiagPipeIsOpen(void)
{
    sqlbeginmutex(&diagPipeMutex);
    SAPDB_Bool retval = ( RTE_UNDEF_HANDLE != pipeFileDes );
    sqlendmutex(&diagPipeMutex);
    return retval ;
}

// implementation of methods of the class that is used by the clients
#ifdef _WIN32
#define CLIENT_SIDE_HANDLE_ m_pipeHandle
#else
#define CLIENT_SIDE_HANDLE_ m_pipeHandle[0]
#define KERNEL_SIDE_HANDLE_ m_pipeHandle[1]
#endif

RTE_ClientDiagPipe::RTE_ClientDiagPipe()
{
    CLIENT_SIDE_HANDLE_ = RTE_UNDEF_HANDLE;
#ifdef _WIN32
#else
    KERNEL_SIDE_HANDLE_ = RTE_UNDEF_HANDLE;
#endif
};

RTE_ClientDiagPipe::~RTE_ClientDiagPipe()
{
    closeClientSide();
#ifdef _WIN32
#else
    closeKernelSide();
#endif
};

//-----------------------------------------------------------

SAPDB_Bool RTE_ClientDiagPipe::create(SAPDB_Char *dbName)
{
#ifdef _WIN32
    SAPDB_Int lengthOfPipeName = (SAPDB_Int)(strlen(_PIPE_PREFIX/*/"\\\\.\\pipe\\"*/)+strlen(dbName)+1);
	SAPDB_Char *pipeName  = (SAPDB_Char *)alloca(lengthOfPipeName);
	SAPDB_sprintf(pipeName,lengthOfPipeName,"%s%s",_PIPE_PREFIX/*/"\\\\.\\pipe\\%s"*/,dbName);
	m_pipeHandle = CreateNamedPipe(pipeName,PIPE_ACCESS_INBOUND/*|FILE_FLAG_OVERLAPPED*/,PIPE_WAIT,1,256,256,1000,NULL);
    return ( INVALID_HANDLE_VALUE != m_pipeHandle );
#else
    if(0 == pipe(m_pipeHandle))
    {
        en41SetPipeFDInFile(dbName, KERNEL_SIDE_HANDLE_);
        return true;
    }
    else
    {
        return false;
    }
#endif
};

//-----------------------------------------------------------

SAPDB_Bool RTE_ClientDiagPipe::readMessage(SAPDBErr_MessageList &message)
{
    SAPDB_Byte bufferForHeader[PACKET_HEADER_BUFFER_SIZE];
    RTE_FileOffset bytesRead;
    tsp00_VfReturn_Param   ReturnStatus;

#ifdef _WIN32
    do
#endif
    {
        RTESys_IORead ( CLIENT_SIDE_HANDLE_,
                        bufferForHeader,
                        RTEComm_PacketHeader::PacketHeaderBufferSize(),
                        bytesRead,
                        ReturnStatus );
    }
#ifdef _WIN32
    while( (ReturnStatus != vf_ok) && (ERROR_PIPE_LISTENING == RTESys_GetLastError()) );
#endif
    if(ReturnStatus != vf_ok)
    {
        return false;
    }
    else
    {
        RTEComm_PacketHeader    packetHeader(bufferForHeader);
        SAPDB_UInt4             length;
        SAPDBErr_MessageList    err;
        SAPDB_UInt1             ReqRespType;
        SAPDB_UInt4             SenderRef;
        SAPDB_UInt4             ReceiverRef;
        SAPDB_UInt4             DataLen;
        SAPDB_UInt4             ActDataLen;
        RTEComm_ProtocolID      ProtocolID;
        SAPDB_UInt1             ProtocolVersion;

        if(!packetHeader.Get( &ReqRespType,&SenderRef,&ReceiverRef,&DataLen,&ActDataLen,&ProtocolID,&ProtocolVersion,err )
            || ( RTE_DIAGPIPE_DATA_PACKET       != ReqRespType )
            || ( RTE_DIAGPIPE_KERNEL_REF        != SenderRef )
            || ( RTE_DIAGPIPE_DBM_REF           != ReceiverRef )
            || ( Prot_DiagPipe                  != ProtocolID )
            || ( RTE_DIAGPIPE_PROTOCOL_VERSION  <  ProtocolVersion ))
        {
            return false;            
        }
        packetHeader.CurrPacketSegLen(length,err);
        SAPDB_Char *bufferForData = (SAPDB_Char *)alloca(length-RTEComm_PacketHeader::PacketHeaderBufferSize()+1);

        RTESys_IORead ( CLIENT_SIDE_HANDLE_,
                        bufferForData,
                        length-RTEComm_PacketHeader::PacketHeaderBufferSize(),
                        bytesRead,
                        ReturnStatus );

        if(ReturnStatus != vf_ok)
        {
            return false;
        }
        else
        {
            bufferForData[bytesRead] = 0;				
            message = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_DBREG_KERNEL_STARTUP_MESSAGE, bufferForData );
        }
    }
    return true;
///    return readFromDiagPipe(CLIENT_SIDE_HANDLE_,messageList);
};

//-----------------------------------------------------------

void RTE_ClientDiagPipe::closeClientSide()
{
    tsp00_VfReturn_Param returnStatus;

    if(RTE_UNDEF_HANDLE != CLIENT_SIDE_HANDLE_)
    {
        RTESys_IOClose(CLIENT_SIDE_HANDLE_,returnStatus );
        CLIENT_SIDE_HANDLE_ = RTE_UNDEF_HANDLE;
    }
};

//-----------------------------------------------------------

#ifdef _WIN32
#else
void RTE_ClientDiagPipe::closeKernelSide()
{
    tsp00_VfReturn_Param returnStatus;

    if(RTE_UNDEF_HANDLE != KERNEL_SIDE_HANDLE_)
    {
        RTESys_IOClose(KERNEL_SIDE_HANDLE_,returnStatus );
        KERNEL_SIDE_HANDLE_ = RTE_UNDEF_HANDLE;
    }
}

//-----------------------------------------------------------

SAPDB_Bool RTE_ClientDiagPipe::waitForClose()
{
    struct pollfd fdForPoll;
    fdForPoll.fd = CLIENT_SIDE_HANDLE_;
    fdForPoll.events = POLLIN;
    SAPDB_Int retval;
    do
    {
        retval = poll(&fdForPoll,1,-1);
    }
    while( retval == -1 && ( errno == EAGAIN || errno == EINTR || errno == ENOMEM ));
    return ( retval > 0 );
}

//-----------------------------------------------------------

SAPDB_Bool RTE_ClientDiagPipe::HandleBelongsToDiagPipe(RTE_FileHandle handle)
{
    return ((handle == m_pipeHandle[0])||(handle == m_pipeHandle[1]));
}

//-----------------------------------------------------------

#endif

//-----------------------------------------------------------
