/*!
  @file    Msg_OutputKnlDiag.cpp
  @author  Robin
  @ingroup Common
  @brief   KnlDiag Message output class
  
  implementation of diagnostic output used in the database kernel


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
#include "heo07.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"
#include "geo007_2.h"
#include "RunTime/System/RTESys_Time.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "hsp100.h"
#include "RunTime/RTE_Speed.h"
#include "geo007_1.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "Messages/Msg_Arg.hpp"
#define KERNEL_LZU
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "Messages/Msg_FileProcessor.hpp"
#include "SAPDBCommon/SAPDB_Names.h"
#include "RunTime/System/RTESys_AtomicOperation.h"

#ifdef _WIN32   // for writing to event log (subject to change...)
#include "geo60.h"
#include "geo007_1.h"
#endif

#define MSG_MAX_LINE_LEN    512

class BooleanTLS
{
public:
    BooleanTLS ()
    {
        teo07_ThreadErr terr;
        tsp00_ErrTextc ttext;
        sqlcreatetls(&m_Key, ttext, &terr);
        sqlsettls(m_Key,0,ttext, &terr);
    }

    SAPDB_Bool Set(SAPDB_Bool value)
    {
        teo07_ThreadErr terr;
        tsp00_ErrTextc ttext;
        sqlsettls(m_Key,(void *)value,ttext, &terr);
        return ( THR_OK_EO07 == terr );
    }

    SAPDB_Bool Get(void)
    {
        return 0 != sqlgettls(m_Key);
    }
private:
    tsp00_Int4 m_Key;
};


class OutputFileKnlDiag;
class OutputFileKnlDiagErr;
class OutputEmergency;

static OutputFileKnlDiag    *pInstance          = NULL;
static OutputFileKnlDiagErr *pErrorArchive      = NULL;
static OutputEmergency      *pEmergencyOutput   = NULL;

/*!
  @class OutputEmergency
 */
class OutputEmergency : public Msg_IOutput
{
public:
    OutputEmergency()
    {
        RTE_ISystem::GetLocalDateTime(m_CreationDateTime);
    }

    virtual SAPDB_Bool Output(Msg_List  &outputMsgList, Msg_List &errList, Msg_IOutput::PassToErrorArchive toArchive )
    {
#ifdef _WIN32
        SAPDB_ULong eventType;
        switch(errList.Type())
        {
        case Msg_List::Error:
            eventType = ERR_TYPE;
            break;
        case Msg_List::Warning:
            eventType = WRN_TYPE;
            break;
        default:
            eventType = INFO_TYPE;
            break;
        }
        sql60_msg_event_log(errList.ID()/*LONG  lMsgID*/,
                            eventType/*ULONG ulEventType*/,
                            "RTE"/*const char * pszLabel*/,
                            outputMsgList.Message()/*const char * pszFormatStr, ...*/);
#endif
        return true;
    }

    virtual CreationDateTime GetCreationDateTime() const
    {
        return m_CreationDateTime;
    }

    virtual OutputStyle const GetOutputStyle() const
    {
        return Single;
    }

    virtual Phase const GetPhase() const
    {
        return Running;
    }

    virtual SAPDB_Bool StartupPhaseCompleted(Msg_List &errList)
    {
        return true;
    }

    virtual SAPDB_Bool BeginOfTerminationPhase(Msg_List &errList)
    {
        return true;
    }
private:
    CreationDateTime           m_CreationDateTime;      //!< the timestamp when file was created
};



class OutputFileKnlDiagErr : public Msg_OutputFile
{
public:
    OutputFileKnlDiagErr ()
     : Msg_OutputFile   ( SAPDB_KERNEL_MESSAGES_ARCHIVE_FILE_NAME   /*SAPDB_Char const      *filename*/
                        , (SAPDB_UTF8 *)"knldiag_err_lock"              /*SAPDB_UTF8 const      *lockname*/
                        , ContinueExistingOutput                        /*HandleExistingOutput const handleExistingOutput*/
                        , 0                                             /*SAPDB_UInt8 const      fixedSizeInBytes*/
                        )
    {

    }

    virtual SAPDB_Bool Output(Msg_List  &outputMsgList, Msg_List &errList, PassToErrorArchive toArchive=toArchiveIfError )
    {
        if(m_InUse.Get())
        {
            Msg_IOutput::GetEmergencyOutput().Output(outputMsgList, errList );
            return false;
        }
        m_InUse.Set(true);
        SAPDB_Bool retval = Msg_OutputFile::Output(outputMsgList, errList );
        m_InUse.Set(false);
        return retval;
    }

    virtual SAPDB_Bool PrepareStartingPhase(Msg_List &errList)
    {
        if(!Msg_OutputFile::PrepareStartingPhase(errList))
        {
            Msg_List tmpList;
            Msg_IOutput::GetEmergencyOutput().Output( errList,tmpList );
            return false;
        }

        SAPDB_Char szOpenLine[MSG_MAX_LINE_LEN + 1];
        tsp00_Versionc   currentVersion;
        SAPDB_Char	speed[RTE_MAXSPEEDLEN + 1];	
        RTESys_Timestamp timeBufferGMT;
        RTESys_FillProtocolGMTTimeStamp(timeBufferGMT);
        memset (speed, 0, RTE_MAXSPEEDLEN + 1);
		strcpy (speed, RTE_GetSpeed ());
        if (!strcmp (speed, RTE_FAST) )
        {
            strcpy (speed, " ");
        }
        else
        {
            strcat (speed, " ");
        }

		sp100_GetVersionString( speed, 
	    						s100buildnumber,
		    					currentVersion);
		SAPDB_sprintf( szOpenLine,MSG_MAX_LINE_LEN + 1,
                  ERR_FILE_STARTING_TEXT,
                  &timeBufferGMT[0], &currentVersion[0] );
        Msg_List theList = Msg_List( Msg_List::Info, 27, MSG_MSGOUTPUT_COMPONENT, __FILE__, __LINE__, szOpenLine);
        return Output( theList, errList );
    }

    virtual SAPDB_Bool PrepareTerminatingPhase(Msg_List &errList)
    {
        Msg_FileProcessor fileProcessor(m_Filename,Msg_IOutput::Single);
        RTE_FileOffset writePosition;
        SAPDB_UInt8 biggestSequenceNumber;

        fileProcessor.SearchWritePosition(writePosition,biggestSequenceNumber,true/*indicate continuously written file*/,errList);

        RTE_FileOffset newPosition;
        tsp00_VfReturn_Param returnStatus;
        RTESys_IOSeek ( m_FileHandle,
                        writePosition,
                        RTESys_IOSeekSet,
                        newPosition,
                        returnStatus );
        m_FilePosition = newPosition;
        return Msg_OutputFile::PrepareTerminatingPhase(errList);
    }

    virtual SAPDB_Bool BeginOfTerminationPhase(Msg_List &errList)
    {
        if(m_InUse.Get())
        {
            Unlock();
            m_InUse.Set(false);
        }
        {
            Lock();
            SAPDB_Bool retVal = Msg_OutputFile::BeginOfTerminationPhase(errList);
            Unlock();
            return retVal;
        }
    }

private:
    BooleanTLS m_InUse;
};

class OutputFileKnlDiag : public Msg_OutputFile
{
public:
    OutputFileKnlDiag (  const SAPDB_UInt8 fixedSizeInBytes, OutputFileKnlDiagErr *pErrorArchive )
        : Msg_OutputFile    ( SAPDB_KERNEL_MESSAGES_FILE_NAME                     /*SAPDB_Char const      *filename*/
                            , (SAPDB_UTF8 *)"knldiag_lock"  /*SAPDB_UTF8 const      *lockname*/
                            , RenameExistingOutput          /*HandleExistingOutput const handleExistingOutput*/
                            , fixedSizeInBytes              /*SAPDB_UInt8 const      fixedSizeInBytes*/
                            , pErrorArchive
                            )
    {
        m_SequenceNumber = pErrorArchive->GetSequenceNumber();
    }

    virtual SAPDB_Bool Output(Msg_List  &outputMsgList, Msg_List &errList, PassToErrorArchive toArchive )
    {
        if(m_InUse.Get())
        {
            Msg_IOutput::GetEmergencyOutput().Output(outputMsgList, errList );
            return false;
        }
        m_InUse.Set(true);
        RTETask_Task *pCurrentTask = RTETask_Task::Self();
        outputMsgList.SetOutputIdentification   ( 0                                     /*m_SequenceNumber - will be really set in the Output() metmod*/
                                                , sqlgetthreadid()                      /* SAPDB_UInt4 const outputThreadId=0*/
                                                , pCurrentTask ? pCurrentTask->ID() : 0 /*, SAPDB_UInt4 const outputTaskId=0*/
                                                                                        /*, SAPDB_UInt4 const outputSchedulerId=0 */
                                                );
        SAPDB_Bool retval = Msg_OutputFile::Output(outputMsgList, errList, toArchive );
        m_InUse.Set(false);
        return retval;
    }

    virtual SAPDB_Bool BeginOfTerminationPhase(Msg_List &errList)
    {
        if(m_InUse.Get())
        {
            Unlock();
            m_InUse.Set(false);
        }
        {
            Lock();
            SAPDB_Bool retval = Msg_OutputFile::BeginOfTerminationPhase(errList);
            // on Unix, now the watchdog process is active again. As it has seen the increasing 
            // sequence numbers, the current sequence number has to be read from the file written
#ifndef _WIN32  // on windows everything is done in one process, so reading the sequence number is not necessary
            Msg_FileProcessor fileProcessor(m_Filename,Msg_IOutput::Single/*Triple*/);
            RTE_FileOffset writePosition;
            SAPDB_UInt8 biggestSequenceNumber;

            fileProcessor.SearchWritePosition(writePosition,biggestSequenceNumber,false/*indicate file not written continuously*/,errList);
            m_SequenceNumber = biggestSequenceNumber + 1;
#endif
            Unlock();
            return retval;
        }
    }

private:
    BooleanTLS m_InUse;
};

#define INITIALIZATION_PENDING (void *)1    // any value != 0 that is no valid memory address

/*!
    @brief return reference to default diagnostic output channel

    This reference can be used to 
*/

Msg_IOutput &Msg_IOutput::GetDiagnosticOutput()
{
    // first create the error archive (aka knldiag.err): existing file is continued, file size is not limited (no cyclic write)
    void *pOldValue;
    void **ppPointerPosition;
    if( pErrorArchive <= INITIALIZATION_PENDING )
    {
        do
        {
            ppPointerPosition = (void **)&pErrorArchive;    // this ugly statement is needed because of C++ type checks: you cannot pass a Singleton* to a void*& ...
            if(RTESys_CmpxchgPointer(   *ppPointerPosition        /*void * VAR_VALUE_REF pointerPosition*/,
                                        NULL                      /*void * const         expectedValue*/,
                                        INITIALIZATION_PENDING    /*void * const         newValue*/,
                                        pOldValue                 /*void * VAR_VALUE_REF oldValue*/ ))
            {   // could change from NULL to INITIALIZATION_PENDING --> must be the first thread to come along, do the initialization
                SAPDBMEM_STATIC_RAW_ALLOCATE(KnlDiagErr_InstanceData, sizeof(OutputFileKnlDiagErr));
                // use temporary variable to avoid other threads seeing a valid pErrorArchive before PrepareStartingPhase() has run
                OutputFileKnlDiagErr *pTemp = new(KnlDiagErr_InstanceData) OutputFileKnlDiagErr ();
                Msg_List errList;
                pTemp->PrepareStartingPhase(errList);
                pErrorArchive = pTemp;
            }
        }
        while( INITIALIZATION_PENDING == pOldValue );	// found another thread initializing the singleton, try again
    }
    // now create the diagnostic file itself (aka knldiag): existing files are renamed, file size is limited (cyclic write)
    if( pInstance <= INITIALIZATION_PENDING )
    {
        do
        {
            ppPointerPosition = (void **)&pInstance;
            if(RTESys_CmpxchgPointer(   *ppPointerPosition          /*void * VAR_VALUE_REF pointerPosition*/,
                                        NULL                      /*void * const         expectedValue*/,
                                        INITIALIZATION_PENDING    /*void * const         newValue*/,
                                        pOldValue                 /*void * VAR_VALUE_REF oldValue*/ ))
            {
                SAPDBMEM_STATIC_RAW_ALLOCATE(KnlDiag_InstanceData, sizeof(OutputFileKnlDiag));
                Msg_List errList;
                RTEConf_Parameter::Integer sizeFromParams;
                if( !RTEConf_ParameterAccess::Instance()
                || !(RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_KERNELDIAGSIZE,sizeFromParams,errList) )
                    sizeFromParams = 16;
                pInstance = new(KnlDiag_InstanceData) OutputFileKnlDiag   ( sizeFromParams*1024,pErrorArchive );
            }
        }
        while( INITIALIZATION_PENDING == pOldValue );
    }
    return *pInstance;
}


/*!
    @brief return reference to emergency diagnostic output channel

    This reference can be used in case the normal diagnostic messages output call returns an error message.
    */
Msg_IOutput &Msg_IOutput::GetEmergencyOutput()
{
    if(!pEmergencyOutput)
    {
        SAPDBMEM_STATIC_RAW_ALLOCATE(EmergencyOutput_InstanceData, sizeof(OutputEmergency));
        Msg_List errList;
        pEmergencyOutput = new(EmergencyOutput_InstanceData) OutputEmergency ();
    }
    return *pEmergencyOutput;
}

extern "C" void Msg_OutputToKnlDiag(SAPDB_Long messageID,SAPDB_ULong eventType,const SAPDB_Char *pLabel,const SAPDB_Char *pMessage)
{
    Msg_IOutput &diagnosticMessages = Msg_IOutput::GetDiagnosticOutput();
    Msg_List errList;
    Msg_List::MessageType messageType;
    switch(eventType)
    {
    case IERR_TYPE:
    case ERR_TYPE:
        messageType = Msg_List::Error;
        break;
    case WRN_TYPE:
        messageType = Msg_List::Warning;
        break;
    case INFO_TYPE:
        messageType = Msg_List::Info;
        break;
    }
#ifndef NO_OLD_DIAG_FILES
    Msg_List theList = Msg_List( messageType, messageID, MSG_MSGOUTPUT_COMPONENT, __FILE__, __LINE__, pMessage,Msg_Arg(MSG_LIST_LABEL_TAG,pLabel));
#else
    Msg_List theList = Msg_List( messageType, messageID, "RTEKERN", __FILE__, __LINE__, pMessage,Msg_Arg(MSG_LIST_LABEL_TAG,pLabel));
#endif
    diagnosticMessages.Output( theList, errList );
}

extern "C" void Msg_StartupPhaseCompleted(void)
{
    Msg_IOutput &diagnosticMessages = Msg_IOutput::GetDiagnosticOutput();
    Msg_List errList;
    diagnosticMessages.StartupPhaseCompleted(errList);
}


extern "C" void Msg_BeginOfTerminationPhase(void)
{
    Msg_List tmpMsgList;
    if ( !Msg_IOutput::GetDiagnosticOutput().BeginOfTerminationPhase(tmpMsgList) )
    {
        Msg_List dummyTmpMsgList;
        Msg_IOutput::GetEmergencyOutput().Output(tmpMsgList, dummyTmpMsgList);
    }
}

extern "C" void Msg_OutputToKnlDiagVArgs(SAPDB_Long messageID,SAPDB_ULong eventType,const SAPDB_Char *pLabel,const SAPDB_Char *pFormatString,va_list args)
{
    SAPDB_Char message[256];
#ifdef WIN32
    _vsnprintf      ( message, sizeof(message) - 1, pFormatString, args );
#else
    SAPDB_sprintf   ( message, sizeof(message), pFormatString, args ) ;
#endif
    Msg_OutputToKnlDiag(messageID,eventType,pLabel,&message[0]);
}

extern "C" void Msg_WriteStopMessage(void)
{
    SAPDB_Char stoppingLine[MSG_MAX_LINE_LEN + 1];
    tsp00_Versionc   currentVersion;
    SAPDB_Char	speed[RTE_MAXSPEEDLEN + 1];	
    RTESys_Timestamp timeBufferGMT;
    RTESys_FillProtocolGMTTimeStamp(timeBufferGMT);
    memset (speed, 0, RTE_MAXSPEEDLEN + 1);
	strcpy (speed, RTE_GetSpeed ());
    if (!strcmp (speed, RTE_FAST) )
    {
        strcpy (speed, " ");
    }
    else
    {
        strcat (speed, " ");
    }

	sp100_GetVersionString( speed, 
	    					s100buildnumber,
		    				currentVersion);
	SAPDB_sprintf( stoppingLine,MSG_MAX_LINE_LEN + 1,
                ERR_FILE_STOPPING_TEXT,
                &timeBufferGMT[0], &currentVersion[0] );
    Msg_List errList;
    Msg_List theList = Msg_List( Msg_List::Info, 28, MSG_MSGOUTPUT_COMPONENT, __FILE__, __LINE__, stoppingLine);

    Msg_IOutput::GetDiagnosticOutput(); // to make sure pInstace is not NULL,  PTS 1138752 
    pInstance->OutputToArchive( theList, errList );
}
