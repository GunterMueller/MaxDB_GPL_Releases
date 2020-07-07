/*!
  @file           RTEIO_VolumeInfo.cpp
  @author         StefanP
  @ingroup        InputOutput
  @brief          Wrapper of parallel devspace formatting

\if EMIT_LICENCE

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



\endif
*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/InputOutput/RTEIO_VolumeInfo.hpp"
#include "RunTime/InputOutput/RTEIO_VolumeInfo.h"
#include "RunTime/RTE_MessageList.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/Threading/RTEThread_KernelThread.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Tasking/RTETask_IWorkerJob.hpp"
#include "heo07.h"
#include "heo58.h"
#include "heo47.h"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#define KERNEL_LZU
#include "geo50_0.h"

#ifdef HAVE_DAFS_SUPPORT
#include "RunTime/InputOutput/RTEIO_DAFS.hpp" /* nocheck */
#endif

const SAPDB_Int4 RTEIO_VolumeInfo::RTEIO_DetachedId = 0;

#define FORMATTER_THREAD_STACK_SIZE (512*1024)

#if !(defined(_WIN32))  /*&if $OSSPEC not in [ WIN32 ]*/ 
#include "hen40.h"      /* nocheck */
#endif                  /*&endif*/

/*! external defined trace topic */
extern SAPDBTrace_Topic Devspace_Trace;

/*!
  @brief external call to formatting function
  @param type_spec_devno [in] type specific device number
  @param devtype [in] device type
  @param devcapacity [out] device capacity
  @param taskid [in] initiating task
  @param formattingPageImage [in] formatting image or 0 if zero fill wanted
  @param errtext [out] error text
  @param pOk [out] return false formatting failed, true if succeeded
 */
externC void vdevsizeWithImage ( tsp00_Int4                  type_spec_devno,
                                 tsp2_dev_type_Param         devtype,
                                 tsp00_Int4    VAR_VALUE_REF devcapacity,
                                 tsp00_TaskId                taskid,
                                 char                       *formattingPageImage,
                                 tsp00_ErrText VAR_ARRAY_REF errtext,
                                 tsp00_Bool    VAR_VALUE_REF pOk  );

/*===========================================================================*
 *  typedefs                                                                 *
 *===========================================================================*/

typedef Container_Vector<SAPDB_Int4> DeviceIndexVector;


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class FormatVolumeVectorWorkerJob
  @brief This is a worker job class. 
  
         The job will be done on behalf of the calling task and is used
         to format mutiple volumes without blocking the UKT.
*/
class FormatVolumeVectorWorkerJob : public RTETask_IWorkerJob
{
public:
        /*!
          @brief ctor for formatting job
          @param taskID [in] task job initiated
          @param infoVector [in] vector of volumes to format
         */
        FormatVolumeVectorWorkerJob ( RTE_TaskId             taskID,
                                      RTEIO_VolumeInfoVector &infoVector )
                                      : m_TaskID(taskID),
                                        m_InfoVector(infoVector),
                                        m_Ok(false) {}

        /*!
          @brief retrieve job result
          @param errList [in] contains error message
          @return true if job finished error free, false else
         */
        bool GetJobResult( Msg_List &errList ) { errList = m_ErrList; return  m_Ok; }

        /*!
          @brief get initiator task id
          @return task if of job initiator
         */
        virtual RTE_TaskId const GetTaskId() const { return m_TaskID; }

        /*!
          @brief the execution function
         */
        virtual void DoJob();
private:

    /*
      @brief    Find each devspace physical disk
 
      Build up a vector of physicalDeviceIndices comparing and returns the 
      number of threads needed (which is the number of unique devices).
    */
    SAPDB_Int4 AnalyzeVolumeVector( DeviceIndexVector            &physicalDeviceIndexVector,
                                    SAPDB_UInt                   formattingMode );


    /*
      @brief Update information about physical devices of all devspaces in the info vector
    */
    void UpdatePhysicalDeviceInformation ();

    /*
      @brief    Check given devspaces are placed on same physical device

      Depending on the formatting mode it is checked if the 
      devspaces are on the same device. 
     */
    bool IsOnSamePhysicalDevice( RTEIO_VolumeInfo *firstVolume, 
                                 RTEIO_VolumeInfo *secondVolume );


private:
    RTE_TaskId              m_TaskID;
    RTEIO_VolumeInfoVector  &m_InfoVector;
    SAPDBErr_MessageList    m_ErrList;
    bool                    m_Ok;
};

/*!
  @class FormatSingleVolumeWorkerJob
  @brief This is a worker job class. 
  
         The job will be done on behalf of the calling task and is used
         to format single volumes without blocking the UKT.
*/
class FormatSingleVolumeWorkerJob : public RTETask_IWorkerJob
{
public:
        /*!
          @brief ctor for single volue formatting job
          @param taskID [in] task job initiated
          @param volInfo [in] volume to format
         */
        FormatSingleVolumeWorkerJob ( RTE_TaskId             taskID,
                                      RTEIO_VolumeInfo      &volInfo )
                                      : m_TaskID(taskID),
                                        m_VolInfo(volInfo),
                                        m_Ok(false) {}

        /*!
          @brief retrieve job result
          @param errList [in] contains error message
          @return true if job finished error free, false else
         */
        bool GetJobResult( SAPDBErr_MessageList &errList ) { errList = m_ErrList; return  m_Ok; }

        /*!
          @brief get initiator task id
          @return task if of job initiator
         */
        virtual RTE_TaskId const GetTaskId() const { return m_TaskID; }

        /*!
          @brief the execution function
         */
        virtual void DoJob();

private:
    RTE_TaskId              m_TaskID;
    RTEIO_VolumeInfo       &m_VolInfo;
    SAPDBErr_MessageList    m_ErrList;
    bool                    m_Ok;
};

/*---------------------------------------------------------------------------*/

/*!
  @class FormattingThreadInfo
  @brief formatting worker thread
*/
class FormattingThreadInfo : public RTEThread_KernelThread 
{
public:
    /*!
      @brief ctor
     */
    FormattingThreadInfo() 
                    : RTEThread_KernelThread("VolumeFormattingThread"),
                      m_VolumeVector(RTEMem_Allocator::Instance()),
                      m_Ok ( false ),
                      m_ThreadStarted( false ),
                      m_TaskID(0) {}

    /*!
      @brief intialize thread information
      @param infoVector [in] volumes to format
      @param physicalDeviceIndexVector [in] physical device info vector
      @param first [out] first index
      @param taskID [in] initating task id
     */
    SAPDB_Bool Initialize( RTEIO_VolumeInfoVector  &infoVector, 
                           DeviceIndexVector       &physicalDeviceIndexVector,
                           SAPDB_Int4              &first, 
                           RTE_TaskId              taskID );

    /*!
      @brief begin thread execution
      @return true, if thread executes, false if problem
     */
    SAPDB_Bool BeginThread();

    /*!
      @brief retrieve formatting result
      @param errList [in] contains error message
      @return true if formatting finished error free, false else
     */
    bool GetResult( SAPDBErr_MessageList &errList ) { errList = m_ErrList; return  m_Ok; }

    /*!
      @brief Format the volumes 
      @return exit code of thread
    */
    SAPDB_Int4  FormatVolumes();

    /*!
      @brief check if thread could be started
      @returns true if the tread could be started
    */
    SAPDB_Bool  ThreadStarted() { return m_ThreadStarted; }

private:
    /*!
    @brief  The created thread runs the code of the methode 'ThreadMain'
    @return exit code of thread
    */
    virtual SAPDB_Int4  KernelThreadMain()
    {
        this->SetPriority(THR_PRIO_IDLE_EO07);
        return FormatVolumes();
    }

    /*!---------------------------------------------------------------------------
    @description   Count the number of identical entries beginning with first.

    @return value  Return the number of devspaces on device
    */
    SAPDB_Int4 FindNumberOfVolumeOnDevice( DeviceIndexVector  &physicalDeviceIndexVector, 
                                           SAPDB_Int4         first );

    /*!---------------------------------------------------------------------------
    @description    Copy references to devspace info for one device
                    Updates the first index to the next device in the physicalDeviceIndexVector
    */
    void FindVolumesOnSameDevice( RTEIO_VolumeInfoVector    &infoVector, 
                                  DeviceIndexVector         &physicalDeviceIndexVector, 
                                  SAPDB_Int4                &first );
    /*!
    @description    Destroy object
    */
    virtual void  DestroyObject() { this->~FormattingThreadInfo(); }


    RTEIO_VolumeInfoVector    m_VolumeVector;
    tsp00_TaskId              m_TaskID;
    SAPDBErr_MessageList      m_ErrList;
    bool                      m_ThreadStarted;
    bool                      m_Ok;
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*!---------------------------------------------------------------------------
  Function: DoFormatSingleVolume
  Description: Does the real formatting action

  Implemented as a wrapper for pascal interface coded vdevsizeWithImage

  Arguments: volInfo [in] volume info
             taskid [in] the original task initiating parallel formatting
             errList [out] error message filled if false is returned
  Return value: true if call succeeded, false if call failed
 */
static SAPDB_Bool DoFormatSingleVolume( RTEIO_VolumeInfo      &volInfo,
                                        tsp00_TaskId          taskid, 
                                        SAPDBErr_MessageList  &errList )
{
    SAPDB_Bool    ok;

    if ( RTE_FileSystemAccessTypeDAFS == volInfo.GetFileSystemAccessType() )
    {
#       ifdef HAVE_DAFS_SUPPORT
         ok = RTEIO_DAFS_FormatVolume( volInfo, taskid, errList );
#       else
         ok = false;
#       endif
    }
    else
    {
        tsp00_ErrText errtext;
        SAPDB_Int4    devcapacity = (SAPDB_Int4)volInfo.GetCapacityInPages();
        tsp00_Bool    devsizeOk;

        vdevsizeWithImage( volInfo.GetLogicalVolNo(), 
                           volInfo.GetType(), 
                           devcapacity, 
                           taskid, 
                           (char *)volInfo.GetFormattingPageImage(),
                           errtext, 
                           devsizeOk );
        if ( devsizeOk )
        {
            volInfo.SetCapacityInPages(devcapacity);
            ok = true;
        }
        else
        {
            tsp00_ErrTextc errTextC;
            errTextC.p2c(errtext);

            SAPDBTRACE_WRITELN(Devspace_Trace, 1, "Formatting devspace failed:" << errTextC.asCharp() );
            errList.AppendNewMessage(SAPDBErr_MessageList(RTE_CONTEXT, 0, RTEERR_FORMATTING_FAILED, errTextC.asCharp()));
            ok = false;
        }
    }
    return ok;
}

/*---------------------------------------------------------------------------*/

void FormatSingleVolumeWorkerJob::DoJob()
{
    SAPDBTRACE_ROUTINE_DEBUG("FormatSingleVolumeWorkerJob::DoJob", Devspace_Trace, 1);

    m_Ok = DoFormatSingleVolume( m_VolInfo, m_TaskID, m_ErrList );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool FormattingThreadInfo::Initialize( RTEIO_VolumeInfoVector  &infoVector, 
                                             DeviceIndexVector       &physicalDeviceIndexVector,
                                             SAPDB_Int4              &first, 
                                             RTE_TaskId              taskID )
{
    SAPDBTRACE_ROUTINE_DEBUG("FormattingThreadInfo::Initialize", Devspace_Trace, 1);

    SAPDB_Int4 numberOfVolumeOnDevice = FindNumberOfVolumeOnDevice( physicalDeviceIndexVector, first );

    if ( !m_VolumeVector.Initialize( numberOfVolumeOnDevice ) )
    {
        SAPDBTRACE_WRITELN(Devspace_Trace, 1, "allocation failed");
        m_ErrList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ALLOCATION_FAILED, "VolumeVector");
        return false;
    }

    FindVolumesOnSameDevice( infoVector, physicalDeviceIndexVector, first );

    m_TaskID = taskID;

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool FormattingThreadInfo::BeginThread()
{
    SAPDBTRACE_ROUTINE_DEBUG("FormattingThreadInfo::BeginThread", Devspace_Trace, 1);

    if ( Create( FormattingThreadInfo::Normal, FORMATTER_THREAD_STACK_SIZE, m_ErrList ) == FormattingThreadInfo::NoError )
    {
        m_ThreadStarted = true;
    }

    return m_ThreadStarted;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int4  FormattingThreadInfo::FormatVolumes()
{
    SAPDBTRACE_ROUTINE_DEBUG("FormattingThreadInfo::FormatVolumes", Devspace_Trace, 1);

    RTEIO_VolumeInfoVector::SizeType  size = m_VolumeVector.GetSize();

    for ( RTEIO_VolumeInfoVector::IndexType which = 0; which < size; which++ )
    {
        m_Ok = DoFormatSingleVolume( *(m_VolumeVector[which]), m_TaskID, m_ErrList );

        if ( !m_Ok )
            return which + 1;
    }
    return 0;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int4 FormattingThreadInfo
               ::FindNumberOfVolumeOnDevice( DeviceIndexVector  &physicalDeviceIndexVector, 
                                             SAPDB_Int4         first)
{
    SAPDBTRACE_ROUTINE_DEBUG("FormattingThreadInfo::FindNumberOfVolumeOnDevice", Devspace_Trace, 1);

    SAPDB_Int4 size = physicalDeviceIndexVector.GetSize();
    SAPDB_Int4 which;
    SAPDB_Int4 numberOfVolumesOnDevice = 1; // first is always found

    for ( which = (first+1); which < size; which++ )
    {
        if ( physicalDeviceIndexVector[which] == physicalDeviceIndexVector[first] )
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 7, which << " on same device as " << first);
            ++numberOfVolumesOnDevice;
        }
        else
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 9, which << " on other device as " << first);
        }
    }

    SAPDBTRACE_WRITELN(Devspace_Trace, 7, numberOfVolumesOnDevice << " on same device as " << first);

    return numberOfVolumesOnDevice;
}

/*---------------------------------------------------------------------------*/

void FormattingThreadInfo::
             FindVolumesOnSameDevice( RTEIO_VolumeInfoVector    &infoVector, 
                                       DeviceIndexVector        &physicalDeviceIndexVector, 
                                       SAPDB_Int4               &first )
{
    SAPDBTRACE_ROUTINE_DEBUG("FormattingThreadInfo::FindVolumesOnSameDevice", Devspace_Trace, 1);

    SAPDB_Int4 which;
    SAPDB_Int4 tmpWhich;
    SAPDB_Int4 size = physicalDeviceIndexVector.GetSize();
    SAPDB_Int4 next = size;

    m_VolumeVector[0] = infoVector[first];

    for ( tmpWhich = 1, which = (first+1); which < size; which++ )
    {
        if ( physicalDeviceIndexVector[which] == physicalDeviceIndexVector[first] )
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 7, which << " on same device as " << first);
            m_VolumeVector[tmpWhich] = infoVector[which];
            tmpWhich++;
        }
        else
        {
            if ( next > which )
            {
                next = which;
                SAPDBTRACE_WRITELN(Devspace_Trace, 7, which << " choosen as next first");
            }
            SAPDBTRACE_WRITELN(Devspace_Trace, 9, which << " on other device as " << first);
        }
    }

    first = next;
}

/*---------------------------------------------------------------------------*/

void FormatVolumeVectorWorkerJob::DoJob()
{
    SAPDBTRACE_ROUTINE_DEBUG("FormatVolumeVectorWorkerJob::DoJob", Devspace_Trace, 1);

    SAPDB_UInt formattingMode = RTE_FORMATTING_MODE_SERIAL;
    RTEConf_Parameter::String valueString;
    SAPDBErr_MessageList tmpErr;

    m_Ok = true;

    if((RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM_FORMATTING_MODE,valueString, tmpErr))
    {
        switch ( valueString[0] )
        {
            case 'a' :
            case 'A' : 
                formattingMode = RTE_FORMATTING_MODE_AUTO;
                break ;
            case 'p' :
            case 'P' : 
                formattingMode = RTE_FORMATTING_MODE_PARALLEL;
                break ;
            case 's' :
            case 'S' : 
                formattingMode = RTE_FORMATTING_MODE_SERIAL;
                break ;
            default  : 
            break;
        }
    }


    if ( m_InfoVector.IsEmpty() )
    {
        SAPDBTRACE_WRITELN(Devspace_Trace, 1, "nothing to do");
        return;
    }

    //---------------------------------------------------------------------------------------------
    // Analyze the given m_InfoVector to get physicalDeviceIndex vector and number of threads needed
    DeviceIndexVector physicalDeviceIndexVector(RTEMem_Allocator::Instance());

    if ( !physicalDeviceIndexVector.Initialize(m_InfoVector.GetSize()) )
    {
        SAPDBTRACE_WRITELN(Devspace_Trace, 1, "allocation failed");
        m_ErrList.AppendNewMessage(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ALLOCATION_FAILED, "DeviceIndexVector"));
        m_Ok = false;
        return;
    }

    SAPDB_Int4 numberOfThreadsNeeded = AnalyzeVolumeVector(physicalDeviceIndexVector, formattingMode);

    //---------------------------------------------------------------------------------------------
    // Build formatting thread info vector and try to start each thread
    Container_Vector<FormattingThreadInfo> formatterVector(RTEMem_Allocator::Instance());

    if ( !formatterVector.Initialize(numberOfThreadsNeeded) )
    {
        SAPDBTRACE_WRITELN(Devspace_Trace, 1, "allocation of " << numberOfThreadsNeeded << " failed");
        m_ErrList.AppendNewMessage(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ALLOCATION_FAILED, "ThreadVector"));
        m_Ok = false;
        return;
    }

    SAPDB_Int4  currIndex         = 0;
    SAPDB_Bool  threadStartFailed = false;
    SAPDB_Int4  threadIndex;

    for ( threadIndex = 0; threadIndex < numberOfThreadsNeeded; threadIndex++ )
    { 
        if (( !formatterVector[threadIndex].Initialize( m_InfoVector, physicalDeviceIndexVector, currIndex, m_TaskID ) ) ||
            ( !formatterVector[threadIndex].BeginThread() ))
        {
            threadStartFailed = true; // at least one thread could not be started
        }
    }

    //---------------------------------------------------------------------------------------------
    // handle format volumes for those threads that did not start
    if ( true == threadStartFailed )
    {
        tsp00_Int4      threadPrio;
        teo07_ThreadErr threadOk  = sqlgetmythreadpriority(threadPrio);

        if ( THR_OK_EO07 == threadOk )
        {
            threadOk = sqlsetmythreadpriority( THR_PRIO_IDLE_EO07 );

            if ( THR_OK_EO07 != threadOk )
            {
                SAPDBTRACE_WRITELN(Devspace_Trace, 7, "setting formatting thread idle priority failed" );
            }
        }
        else
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 7, "getting formatting thread priority failed" );
        }

        for ( threadIndex = 0; threadIndex < numberOfThreadsNeeded; threadIndex++ )
        {   
            if ( false == formatterVector[threadIndex].ThreadStarted() )
                formatterVector[threadIndex].FormatVolumes(); // start format for this thread
        }

        if ( THR_OK_EO07 == threadOk )
        {
            threadOk = sqlsetmythreadpriority(threadPrio);

            if ( THR_OK_EO07 != threadOk )
            {
                SAPDBTRACE_WRITELN(Devspace_Trace, 7, "resetting formatting thread priority failed" );
            }
        }
    }

    // Wait only for those threads that have been started
    for ( threadIndex = 0; threadIndex < numberOfThreadsNeeded; threadIndex++ )
    {
        if ( formatterVector[threadIndex].ThreadStarted() )
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 1, "waiting for formatter thread " << threadIndex);

            SAPDB_Int4 joinStatus;

            if ( formatterVector[threadIndex].Join( joinStatus, tmpErr ) != FormattingThreadInfo::NoError )
            {
                SAPDBTRACE_WRITELN(Devspace_Trace, 1, "formatter thread " << threadIndex << " join error "  )
            }
        }
    }

    // collect status information and build a complete list for caller
    for ( threadIndex = 0; threadIndex < numberOfThreadsNeeded; threadIndex++ )
    {
        if ( !formatterVector[threadIndex].GetResult( tmpErr ) )
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 1, "formatter thread " << threadIndex << " reported error");
            m_ErrList.AppendNewMessage(tmpErr);
            m_Ok = false;
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int4  FormatVolumeVectorWorkerJob
                              ::AnalyzeVolumeVector( DeviceIndexVector &physicalDeviceIndexVector,
                                                     SAPDB_UInt         formattingMode )
{
    SAPDBTRACE_ROUTINE_DEBUG("FormatVolumeVectorWorkerJob::AnalyzeVolumeVector", Devspace_Trace, 1);
    SAPDB_Int4 size = m_InfoVector.GetSize();
    SAPDB_Int4 index0;
    SAPDB_Int4 index1;
    SAPDB_Int4 numberOfThreadsNeeded;

    physicalDeviceIndexVector[0] = 0;
    numberOfThreadsNeeded        = 1;

    if ( RTE_FORMATTING_MODE_AUTO == formattingMode )
    {
        UpdatePhysicalDeviceInformation();
    }

    for ( index0 = 1; index0 < size; index0++ )
    {
        for ( index1 = 0; index1 < index0; index1++ )
        {
            SAPDB_Bool serializeFormat;

            switch ( formattingMode )
            {
            case RTE_FORMATTING_MODE_PARALLEL:
                serializeFormat = false;
                break;
            case RTE_FORMATTING_MODE_SERIAL:
                serializeFormat = true;
                break;
            default:
                serializeFormat = IsOnSamePhysicalDevice( m_InfoVector[index0], m_InfoVector[index1] );
                break;
            }


            if ( physicalDeviceIndexVector[index1] == index1 &&  serializeFormat )
            {
                SAPDBTRACE_WRITELN(Devspace_Trace, 7, index0 << " on same device as " << index1);
                physicalDeviceIndexVector[index0] = index1;
                break;
            }
            else
                SAPDBTRACE_WRITELN(Devspace_Trace, 9, index0 << " on other device as " << index1);
        }
        if ( index1 == index0 )
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 7, index1 << " on its own device");
            physicalDeviceIndexVector[index0] = index0;
            ++numberOfThreadsNeeded;
        }
    }

    return numberOfThreadsNeeded;
}

/*---------------------------------------------------------------------------*/

void FormatVolumeVectorWorkerJob::UpdatePhysicalDeviceInformation ()
{
    SAPDBTRACE_ROUTINE_DEBUG("FormatVolumeVectorWorkerJob::UpdatePhysicalDeviceInformation", Devspace_Trace, 1);
    SAPDB_Int4      size = m_InfoVector.GetSize();
    SAPDB_Int4      idx;
    DEV_DESCR*      pDevsp;

    for ( idx = 0; idx < size; ++idx )
    {
        pDevsp = sql58k_get_devsp_descr (m_InfoVector[idx]->GetType(), m_InfoVector[idx]->GetLogicalVolNo() - 1);

        RTEIO_UpdatePhysicalVolInfoForVolume ( pDevsp );

        if (pDevsp->physicalDevID == RTE_UNDEF_ID)
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 7, pDevsp->szDevspaceName << " on device " << "UNDEF_ID");
        }
        else
        {
            SAPDBTRACE_WRITELN(Devspace_Trace, 7, pDevsp->szDevspaceName << " on device " << pDevsp->physicalDevID);
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

bool FormatVolumeVectorWorkerJob::IsOnSamePhysicalDevice( RTEIO_VolumeInfo *firstVolume, 
                                                          RTEIO_VolumeInfo *secondVolume )
{
    SAPDBTRACE_ROUTINE_DEBUG("FormatVolumeVectorWorkerJob::IsOnSamePhysicalDevice", Devspace_Trace, 1);

    DEV_DESCR      *firstVol, *secondVol;

    firstVol  = sql58k_get_devsp_descr ( firstVolume->GetType(), firstVolume->GetLogicalVolNo() - 1);
    secondVol = sql58k_get_devsp_descr ( secondVolume->GetType(), secondVolume->GetLogicalVolNo() - 1);

    if ( RTE_UNDEF_ID == firstVol->physicalDevID || RTE_UNDEF_ID == secondVol->physicalDevID )
        return true; // we don't know it, but we assume they are

    return firstVol->physicalDevID == secondVol->physicalDevID;
}

/*!
  Function: RTEIO_UpdatePhysicalVolInfoForVolume
  Description: Update information about physical devices of a specific devspace
  
  Currently this update takes only place on UNIX. 

  Arguments: pVolume [in/out]     Pointer to devspace descriptor
 */
externC void RTEIO_UpdatePhysicalVolInfoForVolume (DEV_DESCR      *pVolume)
{
#if defined(_WIN32)
#else
    struct stat     info;
    char   errCodeStr [10];

    if ( RTE_FileSystemAccessTypeDAFS == pVolume->fileSystemAccessType )
    {
        /* DAFS is always on same device... This will enforce serial formatting in FORMATTING_MODE_AUTO */
        pVolume->physicalDevID = RTE_UNDEF_ID;
    }
    else
    {
        if (stat(pVolume->szDevspaceName, &info) != 0)
        {
            sql47_itoa(errno, errCodeStr, sizeof(errCodeStr) ) ;
            RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, RTEWARN_IO_GET_DEV_ID, pVolume->szDevspaceName, sqlerrs(), errCodeStr));
        }
        else
        {
            pVolume->physicalDevID = info.st_dev;
        }
    }
#endif

    return;
}


/*!---------------------------------------------------------------------------
  Function: RTEIO_FormatSingleVolume
  Description: Format a single devspaces given

  During formatting the devspace capacity information may be updated.

  Arguments: singleVolume [inout] devspace to format
             errList [out] message list to fill
  Return value: true on success for all, false on any error (errList is filled)
 */
bool RTEIO_FormatSingleVolume( RTE_TaskId taskId,
                               RTEIO_VolumeInfo &volInfo,
                               SAPDBErr_MessageList &errList)

{
    SAPDBTRACE_ROUTINE_DEBUG("RTEIO_FormatSingleVolume", Devspace_Trace, 1);

    FormatSingleVolumeWorkerJob  job( taskId, volInfo );

    RTETask_CallWorker(job);

    return job.GetJobResult( errList );
}


/*---------------------------------------------------------------------------*/

 /*!
  Function: RTEIO_FormatVolumeVector
  Description: Format all devspaces given in the vector in parallel

  The vector is inspected and a physical index vector is setup. This index vector
  contains a unique index for each physical device. The devspaces on each device 
  build a group, that is formatted in serial by a worker thread. For each group
  a worker thread is started, that works in parallel. The last group is handled
  by the routine itself in parallel with the worker threads. After all worker threads
  finished their work, the call returns. During formatting the devspace capacity 
  information may be updated.
  
  In case of error the combined errList of all errors reported by the worker threads is returned.

  A non starting thread is not considered an error, but the job is done by the routine
  itself.

  Arguments:    taskId [in]             the task id of the caller
                infoVector [inout]      vector of devspaces to format
                errList [out]           message list to fill

  Return value: true on success for all, false on any error (errList is filled)
 */
bool RTEIO_FormatVolumeVector( RTE_TaskId               taskId,
                               RTEIO_VolumeInfoVector   &infoVector,
                               SAPDBErr_MessageList     &errList)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEIO_FormatVolumeVector", Devspace_Trace, 1);

    FormatVolumeVectorWorkerJob  job( taskId, infoVector );

    RTETask_CallWorker(job);

    return job.GetJobResult( errList );
}

/*---------------------------------------------------------------------------*/

RTE_VolumeAccessMode RTEIO_VolumeInfo::GetAccessMode()
{
    if ( RTE_VolumeAccessModeUnknown == m_AccessMode )
    {
        DEV_DESCR      *pVolume;

        pVolume = sql58k_get_devsp_descr ( this->m_Type, this->m_LogicalVolNo - 1);

        if ( pVolume )
           m_AccessMode = pVolume->accessMode;
    }
    return m_AccessMode;
}

/*------------------------------------------------------------*/

RTE_FileSystemAccessType RTEIO_VolumeInfo::GetFileSystemAccessType()
{
    if ( RTE_FileSystemAccessTypeUnknown == m_FileSystemAccessType )
    {
        DEV_DESCR      *pVolume;

        pVolume = sql58k_get_devsp_descr ( this->m_Type, this->m_LogicalVolNo - 1);
        m_FileSystemAccessType = pVolume->fileSystemAccessType;
    }
    return m_FileSystemAccessType;
}

//----

SAPDB_Bool RTEIO_VolumeInfo::GetPhysicalCapacity(SAPDB_UInt8 &sizeInBytes, SAPDB_UInt4 &sizeInPages) const
{
    DEV_DESCR      *pVolume;

    pVolume = sql58k_get_devsp_descr ( this->m_Type, this->m_LogicalVolNo - 1);
    sizeInBytes = 0;
    sizeInPages = 0;
    SAPDB_Bool ok = true;

#if defined(_WIN32)
    HANDLE FileHandle = CreateFile( pVolume->szDevspaceName,
                                    GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);
    ok = (FileHandle != INVALID_HANDLE_VALUE);
    if ( ok )
    {
        LARGE_INTEGER sizeOfFile;
        ok = (0 != GetFileSizeEx(FileHandle, &sizeOfFile) );
        if ( ok )
        {
            sizeInBytes = (SAPDB_UInt8)sizeOfFile.QuadPart;
        }
        CloseHandle(FileHandle);
    }
#else
    if ( RTE_FileSystemAccessTypeDAFS != pVolume->fileSystemAccessType )
    {
        struct stat info;
        if ( stat(pVolume->szDevspaceName, &info) == 0 )
        {
            if( S_ISREG( info.st_mode ) )
            {
                sizeInBytes = (SAPDB_UInt8)info.st_size;
            }
            else
            {
                ok = false; // if its not a file we dont know...
            }
        }
        else if ( errno != ENOENT )
        {
            ok = false; // if it does not exist, it must have size 0 bytes, but else...
        }
    }
    else
    {
        ok = false; // we do not know enough about DAFS yet...
    }
#endif

    if ( sizeInBytes > 0 )
    {
        SAPDB_ULong pageSize = XPARAM(ulDBPageSize);
        if ( pageSize > 0 
          && sizeInBytes > pageSize )
        {
            sizeInPages = (SAPDB_UInt4)((sizeInBytes - pageSize)/pageSize);
        }
    }
    return ok;
}

/*------------------------------------------------------------*/

SAPDB_Bool RTEIO_VolumeInfo::GetAttachedDevNo(SAPDB_Int4 &attachedDevNo)
{
    if ( 0 != this->m_Type
      && 0 != this->m_LogicalVolNo )
    {
        DEV_DESCR *pVolume = sql58k_get_devsp_descr ( this->m_Type, this->m_LogicalVolNo - 1);
        if ( pVolume )
        {
#if defined(WIN32)
            for ( SAPDB_Int4 iDev = 0; iDev < KGS->XParam->lMaxDevspaces; ++iDev )
            {
                if ( KGS->Dev0.pIOCtrl[iDev].ulAttached
                 && !strcmp( (const char *)KGS->Dev0.pIOCtrl[iDev].szDevspace,
                             (const char *)pVolume->szDevspaceName ) )
                {
                    attachedDevNo = iDev;
                    return true;
                }
            }
#else
            struct DEV_PCB *devp = KGS->first_dev;
            SAPDB_Int4      devpsets = KGS->devpsets;
            for ( SAPDB_Int4 iDev = 0; iDev < XPARAM(lMaxDevspaces); ++iDev )
            {
                if ( devp->state != KT_INITIALIZED
                 && !strcmp( (const char *)KGS->dev0.dev[iDev].devspace,
                             (const char *)pVolume->szDevspaceName ) )
                {
                    attachedDevNo = iDev;
                    return true;
                }
                devp += devpsets;
            }
#endif
        }
    }

    return false;
}

/*------------------------------------------------------------*/

#ifdef TEST_CODE

#define KERNEL_LZU
#include "geo50_0.h" /* nocheck */
#include "heo58.h" /* nocheck */


externC void vgetpid ( tsp00_TaskId VAR_VALUE_REF callerPid );

/*!---------------------------------------------------------------------------
  Function: RTEIO_BuildFormatVolumeVector
  Description: Build a vector with all data and log devspaces found 

  Arguments: infoVector [inout] the vector to fill
             errlist [out] filled on error
  Return value: True
 */
static bool RTEIO_BuildFormatVolumeVector( RTEIO_VolumeInfoVector &infoVector,
                                           SAPDBErr_MessageList &errList)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEIO_BuildFormatVolumeVector", Devspace_Trace, 1);

    SAPDB_Int4 devspaceIndex;

    SAPDB_Int4 totalDataVolumes = KGS->XParam->lMaxDataDevspaces;
    for ( devspaceIndex = 0; devspaceIndex < KGS->XParam->lMaxDataDevspaces; ++devspaceIndex )
    {
        if ( KGS->XParam->DevspDescr[sp2dt_data].Devspaces[devspaceIndex].lDevspaceType == DEVSPACE_UNUSED )
        {
            --totalDataVolumes;
        }
    }

    SAPDB_Int4 totalLogVolumes = KGS->XParam->lMaxArchiveLogs;
    for ( devspaceIndex = 0; devspaceIndex < KGS->XParam->lMaxArchiveLogs; ++devspaceIndex )
    {
        if ( KGS->XParam->DevspDescr[sp2dt_log].Devspaces[devspaceIndex].lDevspaceType == DEVSPACE_UNUSED )
        {
            --totalLogVolumes;
        }
    }

    infoVector.Initialize(totalDataVolumes + totalLogVolumes);

    SAPDB_Int4 currentIndex = 0;
    for ( devspaceIndex = 0; devspaceIndex < KGS->XParam->lMaxDataDevspaces; ++devspaceIndex )
    {
        if ( KGS->XParam->DevspDescr[sp2dt_data].Devspaces[devspaceIndex].lDevspaceType != DEVSPACE_UNUSED )
        {
            infoVector[currentIndex] = new (RTEMem_Allocator::Instance()) RTEIO_VolumeInfo(devspaceIndex + 1,
                                                                                             sp2dt_data,
                                    KGS->XParam->DevspDescr[sp2dt_data].Devspaces[devspaceIndex].lDevspaceSize,
                                                                                             (const SAPDB_Byte *)0 );
            ++currentIndex;
        }
    }
    SAPDB_Byte logPage[sizeof(tsp00_Page)];
    memset(logPage, 0, sizeof(logPage));

    for ( devspaceIndex = 0; devspaceIndex < KGS->XParam->lMaxArchiveLogs; ++devspaceIndex )
    {
        if ( KGS->XParam->DevspDescr[sp2dt_log].Devspaces[devspaceIndex].lDevspaceType != DEVSPACE_UNUSED )
        {
            infoVector[currentIndex] = new (RTEMem_Allocator::Instance()) RTEIO_VolumeInfo(devspaceIndex + 1,
                                                                                             sp2dt_log,
                                    KGS->XParam->DevspDescr[sp2dt_log].Devspaces[devspaceIndex].lDevspaceSize,
                                                                                             &logPage[0] );
            ++currentIndex;
        }
    }

    return true;
}

/*!--------------------------------------------------------------------------
  Function: RTEIO_FormatAllDataAndLogdevspaces
  Description: Do formatting of all data and logdevspaces known

  Test code only!!!!
 */

extern "C" void RTEIO_FormatAllDataAndLogdevspaces()
{
    tsp00_TaskId    taskid;


    SAPDBTRACE_ROUTINE_DEBUG("RTEIO_FormatAllDataAndLogdevspaces", Devspace_Trace, 1);

    SAPDBErr_MessageList errList;

    RTE_Message( SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_START_FORMATTING) );

    RTEIO_VolumeInfoVector infoVector(RTEMem_Allocator::Instance());

    if ( !RTEIO_BuildFormatVolumeVector(infoVector, errList) )
    {
        RTE_Message(errList);
        return;
    }

    vgetpid(taskid);

    if ( !RTEIO_FormatVolumeVector((RTE_TaskId)taskid, infoVector, errList) )
    {
        RTE_Message(errList);
        return;
    }

    RTE_Message( SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_FORMATTING_DONE) );
}
#endif /* NO_TEST_CODE */
