/*!
  @file    Msg_OutputFile.cpp
  @author  JoergM
  @ingroup Common
  @brief   Message output file class
  
  All message output classes are derived from class Msg_IOutput.

  This class is implementing the file output channel.
  
  The output file channel handles three files in two versions.

  The three files are
    the 'starting phase messages' extension .start
    the 'running phase messages' extension .run
    the 'termination phase messages' extension .stop

  The two versions are destinct by an additional extension .old

  The common used file name for kernel messages is 'knldiag'

  The following files were found in the rundirectory before

  'knldiag' and if ever started before 'knldiag.old'

  The following file are found now in the rundirectory

  'knldiag.start', 'knldiag.run', 'knldiag.stop'

  and if ever started before 'knldiag.start.old',  'knldiag.run.old', 'knldiag.stop.old'

  To convert from 'knldiag.start + knldiag.run + knldiag.stop' into knldiag use the tool

  'TransformIntoOldKnldiag DatabaseName'. This tool will convert the new \<xml\> alike output into
  a chronological correct knldiag in 'table form'.

  -------------------------------------------------------------------------

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

#include "Messages/Msg_OutputFile.hpp"
#include "Messages/Msg_List.hpp"
#include "Messages/Msg_FileProcessor.hpp"

#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/System/RTESys_IO.h"

#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

#include "SAPDBCommon/SAPDB_Names.h"

#include "SAPDBCommon/Messages/SDBMsg_Messages.h"

#define FILE_EXTENSION_SIZE         4096
#define SECURITY_BUFFER_SIZE        4096
// Runtime internal used phase changing routines

SAPDB_Char Msg_OutputFile::m_clear[FILE_EXTENSION_SIZE];

Msg_OutputFile::Msg_OutputFile( SAPDB_Char const      *filename
                , SAPDB_UTF8 const      *lockname
                , HandleExistingOutput const handleExistingOutput
                , SAPDB_UInt8 const      fixedSizeInBytes
                , Msg_IOutput           *pErrorArchive )
    : m_SequenceNumber(1)
    , m_FixedSizeInBytes(fixedSizeInBytes)
    , m_CurrentPhase(NoOutput)
    , m_HandleExistingOutput(handleExistingOutput)
    , m_pErrorArchive(pErrorArchive)
    , m_currentFileSize(0)
{
    memset(m_clear,' ',(size_t)FILE_EXTENSION_SIZE);
    sqlcreatemutex(&m_MessageMutex);
    Msg_List dummyList;
    CopyFilename(filename);
}

SAPDB_Bool Msg_OutputFile::ExtendFile(SAPDB_UInt8 size,Msg_List &errList)
{
    tsp00_VfReturn_Param returnStatus;
    RTE_FileOffset newPosition;

    RTESys_IOSeek ( m_FileHandle,
                    0,
                    RTESys_IOSeekEnd,
                    newPosition,
                    returnStatus );
    if( vf_notok == returnStatus )
    {
        errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"seek failed",SAPDB_ToString(RTESys_GetLastError()));
        return false;
    }
//  append the file with a block of spaces of the desired size
    RTE_FileOffset writtenSizeInBytes=0;
    RTE_FileOffset writtenSizeInBytesThisTime;
    for(SAPDB_UInt8 i=0;i<size/FILE_EXTENSION_SIZE;i++)
    {
        RTESys_IOWrite ( m_FileHandle,
                        m_clear,
                        FILE_EXTENSION_SIZE,
                        writtenSizeInBytesThisTime,
                        returnStatus );
        if( vf_notok == returnStatus )
        {
            errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"write failed",SAPDB_ToString(FILE_EXTENSION_SIZE),SAPDB_ToString((SAPDB_UInt8)writtenSizeInBytes),SAPDB_ToString(RTESys_GetLastError()));
            break;
        }
        else
        {
            writtenSizeInBytes += writtenSizeInBytesThisTime;
        }
    }
//  now reposition to the previous file position
    RTESys_IOSeek ( m_FileHandle,
                    m_FilePosition,
                    RTESys_IOSeekSet,
                    newPosition,
                    returnStatus );
    if( vf_notok == returnStatus )
    {
        errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"seek failed",SAPDB_ToString(RTESys_GetLastError()));
    }
    m_currentFileSize += writtenSizeInBytes;
    return ( size == writtenSizeInBytes );
}

/*!
    @brief spinlock protected open
    @param errList [out] filled in case of problem
    @return true if open succeded, false if still closed (errList filled)
    */
SAPDB_Bool Msg_OutputFile::TryOpen(SAPDB_Char *fileName,Msg_List &errList)
{
    tsp00_VfReturn_Param returnStatus;
    SAPDB_Bool fileDidExist = true;
    RTESys_IOOpen ( m_FileHandle,
                    fileName,
                    RTESys_IOWriteOnly/*Exclusive*/,
                    false/*DoCreate*/,
                    DEFAULT_SECURITY_RTESYS_IO,
                    returnStatus );
    if(vf_ok != returnStatus)
    {
        RTESys_IOOpen ( m_FileHandle,
                        fileName,
                        RTESys_IOWriteOnly/*Exclusive*/,
                        true/*DoCreate*/,
                        DEFAULT_SECURITY_RTESYS_IO,
                        returnStatus );
        fileDidExist = false;
    }
    if(vf_ok == returnStatus)
    {
        m_currentFileSize = 0;
        m_FilePosition = 0;
        // pre-fill the file when it is
        //  1.) written cyclically (== has a fixed size)
        //  AND
        //  2.) re-created on application start (== existing output is not continued)
        //  AND
        //  3.) it is the "body" file (== we are between phases starting and running, current phase is still "starting")
        if(0 != m_FixedSizeInBytes && (ContinueExistingOutput != m_HandleExistingOutput || !fileDidExist) && (m_CurrentPhase == Starting))
        {
            if(!ExtendFile(m_FixedSizeInBytes,errList))
            {
                errList = errList + Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"file initialization failed");
            }
        }
        else
        {
            if(ContinueExistingOutput == m_HandleExistingOutput)
            {
                RTE_FileOffset endOfFilePosition;
                RTESys_IOSeek ( m_FileHandle,
                                0,
                                RTESys_IOSeekEnd,
                                endOfFilePosition,
                                returnStatus );
                if( vf_notok == returnStatus )
                {
                    errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"seek failed",SAPDB_ToString(RTESys_GetLastError()));
                    return false;
                }
                else
                {
                    m_currentFileSize = endOfFilePosition;
                }
                // TODO: m_currentFileSize = RTESys_IOGetFileSize()
            }
        }
        return true;
    }
    else
    {
        errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"file open failed",fileName,SAPDB_ToString(RTESys_GetLastError()));
        return false;
    }
}
    
/*!
    @brief spinlock protected close
    @param errList [out] filled in case of problem
    @return true if close succeded, false if still open (errList filled)
    */
SAPDB_Bool Msg_OutputFile::TryClose(Msg_List &errList)
{
    if( 0 == m_FileHandle )
    {
        return true;
    }
    else
    {
        tsp00_VfReturn_Param returnStatus;
        RTESys_IOClose ( m_FileHandle,
                        returnStatus );
        
        if(vf_ok == returnStatus)
        {
            return true;
        }
        else
        {
            errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"closing failed",SAPDB_ToString(RTESys_GetLastError()));
            return false;
        }
    }
}
#define GUESSED_MESSAGE_LENGTH 256
    
SAPDB_Bool Msg_OutputFile::Output(Msg_List /*const*/ &outputMsgList, Msg_List &errList, PassToErrorArchive toArchive )
{
    SAPDB_Bool returnValue;
    SAPDB_Char buffer0[GUESSED_MESSAGE_LENGTH];
    SAPDB_Char *pbuffer;
	pbuffer = buffer0;
	Msg_List tryArchiveErrList;
    Lock();
	if ( NoOutput == m_CurrentPhase )
	{
		if( ContinueExistingOutput == m_HandleExistingOutput )
		{
			m_CurrentPhase = Starting;
			if ( !PrepareRunningPhase(errList) )
			{
				returnValue = false;
			}
			m_CurrentPhase = Running;
		}
		else
		{
			if ( !PrepareStartingPhase(errList) )
			{
				returnValue = false;
			}
		}
	}

	outputMsgList.SetOutputIdentification( m_SequenceNumber
						/*, SAPDB_UInt4 const outputThreadId=0
							, SAPDB_UInt4 const outputTaskId=0
							, SAPDB_UInt4 const outputSchedulerId=0 */);
    m_SequenceNumber++;
    Unlock();
    SAPDB_Bool outputToArchiveResult = true;
	// write output to error archive - depending on the existence of an error archive and the parameter toArchive
	if( m_pErrorArchive )
	{
		if(alwaysToArchive == toArchive || ((toArchiveIfError == toArchive) && (Msg_List::Error == outputMsgList.Type())))
		{
			outputToArchiveResult = m_pErrorArchive->Output(outputMsgList, tryArchiveErrList);
		}
	}

    memset(buffer0,' ',GUESSED_MESSAGE_LENGTH);
    SAPDB_ULong bufferSize = GUESSED_MESSAGE_LENGTH;

    SAPDB_UInt4 neededSize;
    // generate a xml line from outputMsgList
    outputMsgList.XML_Output(  bufferSize,
                                buffer0,
                                neededSize,
                                errList);

    // if the first buffer was not big enough, allocate a new one with the right size 
    if( neededSize > bufferSize )
    {
        pbuffer = (SAPDB_Char *)(RTEMem_RteAllocator::Instance()).Allocate(neededSize);
        outputMsgList.XML_Output(  neededSize,
                                    pbuffer,
                                    neededSize,
                                    errList);
    }
    else
    {
        pbuffer = buffer0;
    }
    if( neededSize > 0 )
        pbuffer[neededSize-1] = '\n';
    SAPDB_Bool returnValueFromTryOutput;
	Msg_List tryOutputErrList;
    Lock();
    returnValueFromTryOutput = TryOutput(pbuffer, neededSize, tryOutputErrList );
    Unlock();
    if ( !returnValueFromTryOutput )
    {
		if ( !outputToArchiveResult )
		{
			errList = tryArchiveErrList;
			errList.AppendNewMessage(tryOutputErrList);
		}
		else
		{
			errList = tryOutputErrList;
		}
		returnValue = false;
    }
	else
	{
		if ( !outputToArchiveResult )
		{
			errList = tryArchiveErrList;
			returnValue = false;
		}
		returnValue = true;
	}
    if( buffer0 != pbuffer )    // if we had to allocate a buffer for the textual representation of the message, deallocate it.
    {
        (RTEMem_RteAllocator::Instance()).Deallocate(pbuffer);
    }
    return returnValue;
}

SAPDB_Bool Msg_OutputFile::WriteExtendingFailedMessage(Msg_List &errList)
{
    SAPDB_UInt4 sizeofEmergencyMessage;
    Msg_List emergencyList = Msg_List(
	    Msg_List::Error,
	    SDBMSG_MESSAGES_FILE_EXTENSION_FAILED,
	    Msg_Arg(SDBMSGTAG_MESSAGES_FILE_EXTENSION_FAILED__FILENAME,m_Filename),
	    Msg_Arg(SDBMSGTAG_MESSAGES_FILE_EXTENSION_FAILED__BYTE_SIZE,SECURITY_BUFFER_SIZE)
    );
	emergencyList.SetOutputIdentification( m_SequenceNumber
						/*, SAPDB_UInt4 const outputThreadId=0
							, SAPDB_UInt4 const outputTaskId=0
							, SAPDB_UInt4 const outputSchedulerId=0 */);
    m_SequenceNumber++;
    emergencyList.XML_Output(  0,NULL,sizeofEmergencyMessage,errList);
//    m_sizeofEmergencyMessage += 32;
    SAPDB_Char *pbuffer = (SAPDB_Char *)alloca( sizeofEmergencyMessage );
    // generate a xml line from outputMsgList
    SAPDB_UInt4 neededSize;
    emergencyList.XML_Output(  sizeofEmergencyMessage,pbuffer,neededSize,errList);

    if( neededSize > 0 )
        pbuffer[SAPDB_MIN(sizeofEmergencyMessage,neededSize)-1] = '\n';

    SAPDB_Bool returnValue = TryOutput(pbuffer,neededSize,errList,true);
    return returnValue ;
}

/*!
    @brief spinlock protected output
    @brief the output routine for informational messages

    @param outputMsgList [in] the message to be written
    @param errList [out] filled if error occured

    @return the sequence number of the output message, or 0 if output failed
    */
// practice what you preach: supply RTETask_ITask object reference to find out taskId and uktId cheaply?
SAPDB_Bool Msg_OutputFile::TryOutput(SAPDB_Char *outputMsgBuffer, SAPDB_UInt4 neededSize , Msg_List &errList, SAPDB_Bool emergency)
{
    RTE_FileOffset writtenSizeInBytes;    
    tsp00_VfReturn_Param returnStatus;
// if cyclic, check if there is enough space. If not, fill the file with spaces (?), and seek to start of file
    if((HasFixedSize()) && Running == m_CurrentPhase)
    {
        if((m_FilePosition - m_StartOfCyclicPart) + neededSize > m_FixedSizeInBytes)
        {
            SAPDB_UInt8 leftover = m_FixedSizeInBytes - (m_FilePosition - m_StartOfCyclicPart);

            while( leftover > FILE_EXTENSION_SIZE )
            {
                RTESys_IOWrite ( m_FileHandle,
                                m_clear,
                                SAPDB_MIN(leftover,FILE_EXTENSION_SIZE),
                                writtenSizeInBytes,
                                returnStatus );
                if(vf_ok != returnStatus || SAPDB_MIN(leftover,FILE_EXTENSION_SIZE) != writtenSizeInBytes)
                {
                    errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"clear until end of file failed",SAPDB_ToString(RTESys_GetLastError()));
                    break;
                }
                leftover -= FILE_EXTENSION_SIZE;
            }

            RTE_FileOffset newPosition;
            RTESys_IOSeek ( m_FileHandle,
                            m_StartOfCyclicPart,
                            RTESys_IOSeekSet,
                            newPosition,
                            returnStatus );
            if(vf_ok != returnStatus || m_StartOfCyclicPart != newPosition)
            {
                errList = errList + Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"seek to start of file failed",SAPDB_ToString(RTESys_GetLastError()));
            }
            else
            {
                m_FilePosition = m_StartOfCyclicPart;
            }
        }
    }
    else    // in the other cases (no cyclic writes!), allocate a security buffer at the end of the file
            // (to be able to write a "file system full" message)
    {
        if(!emergency)  // emergency = true means that we have been called because extending the file failed, so don't try again..
        {
            if(m_currentFileSize - m_FilePosition <= SECURITY_BUFFER_SIZE)
            {
                if(!ExtendFile(SECURITY_BUFFER_SIZE,errList))
                {
                    return WriteExtendingFailedMessage(errList);
                }
            }
        }
    }
// Now write the message to the file
    RTESys_IOWrite ( m_FileHandle,
                     outputMsgBuffer,
                     neededSize,
                     writtenSizeInBytes,
                     returnStatus );
    m_FilePosition += writtenSizeInBytes;
    if(vf_ok != returnStatus || neededSize != writtenSizeInBytes)
    {
        errList = errList + Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"write failed",SAPDB_ToString(neededSize),SAPDB_ToString((SAPDB_UInt8)writtenSizeInBytes),SAPDB_ToString(RTESys_GetLastError()));
        return false;
    }
    else
    {
        return true;
    }
}
/*!
    @brief preparing for starting phase
    @param errList [out] filled in case of problems
    @return true if preparation succeeded, false if failed
    */
SAPDB_Bool Msg_OutputFile::PrepareStartingPhase(Msg_List &errList)
{
    // treatment of existing files: if not to be continued, delete them, making a backup copy if desired
    if((RenameExistingOutput == m_HandleExistingOutput)||(RemoveExistingOutput == m_HandleExistingOutput))
    {
        tsp00_VfReturn_Param returnStatus;
        RTE_Path temporaryNewFileName;

        SAPDB_sprintf(temporaryNewFileName,sizeof(RTE_Path),"%s"SAPDB_OUTPUTFILE_OLD_FILE_SUFFIX,m_Filename);

        SAPDB_Bool renameWasSuccessful = false;
        if(RenameExistingOutput == m_HandleExistingOutput)
        {
            RTESys_IOUnlink(temporaryNewFileName,
                            returnStatus );
            if( vf_ok != returnStatus )
            {
                Msg_List tmpList = Msg_List(RTE_CONTEXT,RTEINFO_GENERAL,"file deletion failed",temporaryNewFileName,SAPDB_ToString(RTESys_GetLastError()));
                Msg_List dummyErrList;
                Msg_IOutput::GetEmergencyOutput().Output(tmpList, dummyErrList );
            }
            RTESys_IORename(m_Filename,
                            temporaryNewFileName,
                            returnStatus);
            if( vf_ok != returnStatus )
            {
                Msg_List tmpList = Msg_List(RTE_CONTEXT,RTEINFO_GENERAL,"file rename failed",temporaryNewFileName,SAPDB_ToString(RTESys_GetLastError()));
                Msg_List dummyErrList;
                Msg_IOutput::GetEmergencyOutput().Output(tmpList ,dummyErrList );
            }
            else
            {
                renameWasSuccessful = true;
            }
        }
        if ((RemoveExistingOutput == m_HandleExistingOutput) || !renameWasSuccessful)   // if the file cannot be renamed (whyever..), it is being deleted to avoid partial overwrites
        {
            RTESys_IOUnlink ( m_Filename,
                              returnStatus );
            if(vf_ok != returnStatus)
            {
                Msg_List tmpList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"file deletion failed",m_Filename,SAPDB_ToString(RTESys_GetLastError()));
                Msg_List dummyErrList;
                Msg_IOutput::GetEmergencyOutput().Output(tmpList, dummyErrList );
            }
        }
    }

    if( ContinueExistingOutput == m_HandleExistingOutput )
    {
        // seek to the write position if existing files are to be continued
        Msg_FileProcessor fileProcessor(m_Filename,Msg_IOutput::Single);
        RTE_FileOffset writePosition;
        SAPDB_UInt8 biggestSequenceNumber;

        fileProcessor.SearchWritePosition(writePosition,biggestSequenceNumber,0 == m_FixedSizeInBytes/*indicate continuously written file*/,errList);

        m_SequenceNumber = biggestSequenceNumber+1;
        Msg_List openErrList;
        if(!TryOpen(m_Filename,errList))
        {
            return false;
        }
        RTE_FileOffset newPosition;
        tsp00_VfReturn_Param returnStatus;
        RTESys_IOSeek ( m_FileHandle,
                        writePosition,
                        RTESys_IOSeekSet,
                        newPosition,
                        returnStatus );
        m_FilePosition = newPosition;
    }
    else
    {
        Msg_List openErrList;
        if(!TryOpen(m_Filename,errList))
        {
            return false;
        }
    }


    m_CurrentPhase = Starting;
    return true;
}

/*!
    @brief preparing for running phase
    @param errList [out] filled in case of problems
    @return true if preparation succeeded, false if failed
    */
SAPDB_Bool Msg_OutputFile::PrepareRunningPhase(Msg_List &errList)
{
    if(HasFixedSize()/*GetOutputStyle()==Msg_IOutput::Triple*/)
    {
        if(!TryClose(errList))
            return false;
        
        if(ContinueExistingOutput == m_HandleExistingOutput)
        {
            // seek to the write position if existing files are to be continued
            Msg_FileProcessor fileProcessor(m_Filename,Msg_IOutput::Single);
            RTE_FileOffset writePosition;
            SAPDB_UInt8 biggestSequenceNumber;

            fileProcessor.SearchWritePosition(writePosition,biggestSequenceNumber,0 == m_FixedSizeInBytes/*indicate continuously written file*/,errList);

            m_SequenceNumber = biggestSequenceNumber+1;
            if(!TryOpen(m_Filename,errList))
                return false;
            RTE_FileOffset newPosition;
            tsp00_VfReturn_Param returnStatus;
            RTESys_IOSeek ( m_FileHandle,
                            writePosition,
                            RTESys_IOSeekSet,
                            newPosition,
                            returnStatus );
            RTE_FileOffset writtenSizeInBytes;
            RTESys_IOWrite ( m_FileHandle,
                             "\n",
                             1,
                             writtenSizeInBytes,
                             returnStatus );
            m_FilePosition = newPosition + 1;
        }
        else
        {
            m_StartOfCyclicPart = m_FilePosition;

            if(!TryOpen(m_Filename,errList))
                return false;

            tsp00_VfReturn_Param returnStatus;
            RTE_FileOffset newPosition;
            RTESys_IOSeek ( m_FileHandle,
                            m_StartOfCyclicPart,
                            RTESys_IOSeekSet,
                            newPosition,
                            returnStatus );

            m_FilePosition = (SAPDB_UInt8)newPosition;
            if( vf_notok == returnStatus )
            {
                errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"seek failed",SAPDB_ToString(RTESys_GetLastError()));
                return false;
            }
        }
    }
    m_CurrentPhase = Running;
    return true;
}

/*!
    @brief preparing for terminating phase
    @param errList [out] filled in case of problems
    @return true if preparation succeeded, false if failed
    */
SAPDB_Bool Msg_OutputFile::PrepareTerminatingPhase(Msg_List &errList)
{
    if ( m_CurrentPhase != Stopping )
    {
        if( HasFixedSize() )
        {
            if(!TryClose(errList))
                return false;

            if(!TryOpen(m_Filename,errList))
                return false;

            tsp00_VfReturn_Param returnStatus;
            RTE_FileOffset newPosition;
            RTESys_IOSeek ( m_FileHandle,
                            0,
                            RTESys_IOSeekEnd,
                            newPosition,
                            returnStatus );
            m_FilePosition = (SAPDB_UInt8)newPosition;
            if( vf_notok == returnStatus )
            {
                errList = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"seek failed",SAPDB_ToString(RTESys_GetLastError()));
                return false;
            }
        }
    }
    return true;
}
