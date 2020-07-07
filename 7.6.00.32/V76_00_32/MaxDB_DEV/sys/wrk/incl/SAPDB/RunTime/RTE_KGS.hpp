/*!***************************************************************************

  @file RTE_KGS.hpp
  @author JoergM
  @ingroup Kernel Runtime
  @brief RTE Kernel Global Section

  Singleton used to reference all global information of the Kernel Runtime
  to ease debugging tasks.

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


#ifndef RTE_KGS_HPP
#define RTE_KGS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Singleton.hpp"
//! Define that this code references KernelRuntime only used structure elements
#if !defined (KERNEL_LZU)
# define KERNEL_LZU
#endif
#include "geo50_0.h"
#include "SAPDBCommon/SAPDB_ToString.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

// forward declerations
class  RTE_ISystem;                         // OS specific singleton
class  RTEMem_Allocator;                    // Synchronized allocator for common use
class  RTEMem_PageAllocator;                // Pageallocator used for I/O Manager Cache
class  SAPDBMem_IRawAllocator;              // Raw allocator interface class
class  RTETask_ICommonQueue;                // Free list for common tasking queue elements
struct RTETask_TaskScheduler;               // Linked list of TaskScheduler
class  RTECKC_KernelCommunication;          // Used to reference global kernel communication objects

/*!
  @class RTE_KGS
  @brief Container class for all global references

  To ease debugging all global singletons etc. should be added to this class.
  The header file itself should only use forward declerations and never be 
  dependend on the real implementation of the items referenced. Same is for structures
  referenced etc.

  The static member RTE_KGS::Instance() allows anybody who included this header to access
  this singleton.
 */
class RTE_KGS : public SAPDB_Singleton
{
public:
#if defined(WIN32)
    typedef KERNEL_GLOBAL_SEC     KernelGlobalSection; //!< C Structure used to define kernel runtime global variables
#else
    typedef KERNEL_GLOBAL_SECTION KernelGlobalSection; //!< C Structure used to define kernel runtime global variables
#endif

    /*! Database status */
    enum DbState {
        RTEKGS_StateUnknown        = SERVER_UNKOWN_EO00,
        RTEKGS_StateStarting       = SERVER_STARTING_EO00,
        RTEKGS_StateAdmin          = SERVER_ADMIN_EO00,
        RTEKGS_StateStandby        = SERVER_STANDBY_EO00,
        RTEKGS_StateOnline         = SERVER_ONLINE_EO00,

        // Only the states signaling a stop of the database instance
        // should be greate or equal 'RTEKGS_StateShutdown' !!!!!!
        RTEKGS_StateShutdown       = SERVER_SHUTDOWN_EO00,
        RTEKGS_StateShutdownReinit = SERVER_SHUTDOWNREINIT_EO00,
        RTEKGS_StateShutdownKill   = SERVER_SHUTDOWNKILL_EO00,
        RTEKGS_StateStop           = SERVER_STOP_EO00,
        RTEKGS_StateKill           = SERVER_KILL_EO00,
        RTEKGS_StateAbort          = SERVER_ABORT_EO00,
        RTEKGS_StateStopped        = SERVER_STOPPED_EO00,
        RTEKGS_StateUndefined      = SERVER_UNDEFINED_EO00
    };

    /*! SystemView INSTANCE content */
    struct Statistic
    {
        RTE_Nodename officialNodeName;  //!< node name known by clients
        RTE_Nodename localNodeName;     //!< local node name (ie. hot standby configuration)
        SAPDB_UInt8  timeOfAdminState;  //!< timestamp when ADMIN state reached
        SAPDB_UInt8  timeOfOnlineState; //!< timestamp when ONLINE state reached
        SAPDB_UInt8  processId;         //!< process id
    };

    /*! SystemView MACHINEINFO content */
    struct MachineInfo
    {
        SAPDB_UInt4 numberOfCPU;             //!< number of CPUs found on machine
        SAPDB_UInt4 numberOfCPUassigned;     //!< number of CPUs assigned to this process
        SAPDB_UInt8 physicalMemoryInstalled; //!< physical memory installed in machine
        SAPDB_UInt8 physicalMemoryAvailable; //!< physical memory currently available in machine
        SAPDB_UInt8 virtualMemoryInstalled;  //!< virtual memory installed in machine
        SAPDB_UInt8 virtualMemoryAvailable;  //!< virtual memory currently available in machine
        SAPDB_UTF8  operatingSystemVersion[SAPDB_ALIGNED_CSTRING_SIZE(64)]; //!< OS version info 
        SAPDB_UInt8 userTimeUsed;            //!< time spent by system in user + nice mode since process started
        SAPDB_UInt8 kernelTimeUsed;          //!< time spent by system in kernel mode since process started
        SAPDB_UInt8 idleTimeUsed;            //!< time spent by system in idle mode since process started
    };

    /*! SystemView PROCESSINFO content */
    struct ProcessInfo
    {
        SAPDB_UInt8 totalTimeUsed;            //!< time in microseconds process is running
        SAPDB_UInt8 userTimeUsed;             //!< CPU time in microseconds spent in user mode
        SAPDB_UInt8 systemTimeUsed;           //!< CPU time in microseconds spent in system mode
        SAPDB_UInt8 maxResidentSet;           //!< Maximum size of resident set
        SAPDB_UInt8 pageFaultsWithoutIO;      //!< Page faults not requiring physical IO operation
        SAPDB_UInt8 pageFaultsWithIO;         //!< Page faults requiring physical IO operation
        SAPDB_UInt8 blocksInputCount;         //!< Total number of block read
        SAPDB_UInt8 blocksOutputCount;        //!< Total number of block written
        SAPDB_UInt8 swapCount;                //!< Number of times porcess was swapped out
        SAPDB_UInt8 voluntaryContextSwitches; //!< voluntary context switches
        SAPDB_UInt8 forcedContextSwitches;    //!< involuntare context switches
    };

    /*!
      @brief Returns the singleton instance
      @return Reference to the one and only 
     */
    static RTE_KGS & Instance();

    //---------------------------------------------------------------------
    //! This RTE_KGS internal only macro defines singletons, that need no initialization code for instantiation
    //    Public interface is
    //   name & Get##name()
    //  i.e. RTEMem_Allocator & GetRTEMem_Allocator()
    //
#define RTE_KGS_REFERENCED_SINGLETON(name_) \
public: \
    name_ & Get##name_() { return m_##name_; } \
private: \
    name_ & m_##name_

    //! GetRTE_ISystem() return reference to RTE_ISystem singleton
    RTE_KGS_REFERENCED_SINGLETON(RTE_ISystem);
#undef RTE_KGS_REFERENCED_SINGLETON

    /*!
       @brief Return RTEMem_Allocator instance
           
       RTEMem_Allocator::Instance returns a different type, so macro RTE_KGS_REFERENCED_SINGLETON cannot be used
       @return Reference to synchronized shared RTEMem_Allocator
     */
public:
    SAPDBMem_IRawAllocator & GetRTEMem_Allocator() { return m_RTEMem_Allocator; }
private:
    SAPDBMem_IRawAllocator & m_RTEMem_Allocator;

    //! This RTE_KGS internal only macro defines singletons, that cannot exist without initialization code.
    // Public interfaces are
    //   name & Get##name()
    //   void   Set##name(name *pointerToInstance)
    // i.e. RTEMem_PageAllocator & GetRTEMem_PageAllocator()
    //      void                   SetRTEMem_PageAllocator(RTEMem_PageAllocator *pointerToInstance)
    //
#define RTE_KGS_POINTER_REFERENCED_SINGLETON(name_) \
public: \
    void Set##name_(name_ * val) { m_##name_ = val; } \
public: \
    name_ & Get##name_() { return *m_##name_; } \
private: \
    name_ * m_##name_

    RTE_KGS_POINTER_REFERENCED_SINGLETON(RTEMem_PageAllocator);
    //  ---> Macro defines two member function... Need 'doxygen special command fn to solve warning free...
    /*!
        @fn void RTE_KGS::SetRTEMem_PageAllocator(RTEMem_PageAllocator *pointerToInstance)
        @brief allows to set reference to RTEMem_PageAllocator singleton
        @param pointerToInstance [in] pointer to already created RTEMem_PageAllocator instance
     */

    /*!
        @fn RTEMem_PageAllocator & RTE_KGS::GetRTEMem_PageAllocator()
        @brief get the reference to RTEMem_PageAllocator singleton
        @return 
     */
#undef RTE_KGS_POINTER_REFERENCED_SINGLETON

public:
    /*!
      @brief Set the pointer to old C KernelGlobalSection

      Call this function with a valid KernelGlobalSection, especially 'XParam' member must be initialized!
      This function will also initialize the RTE_KGS statistic record for system view.

      @param pointerToKGS [in] pointer to old C KernelGlobalSection
     */
    void SetKGS(KernelGlobalSection *pointerToKGS)
    {
        m_KGS = pointerToKGS; 
    }

    /*!
      @brief Return pointer to old C KernelGlobalSection
      @return Pointer to old C KernelGlobalSection
     */
    KernelGlobalSection *GetKGS() { return m_KGS; }

    /*!
      @brief Initialize statistic information

      'XParam' member must be initialized!
      This function will initialize the RTE_KGS statistic record for system view.
     */
    void InitStatistic();

    /*!
      @brief Return reference of INSTANCE information

      @return Reference to content used for system view INSTANCE
     */
    Statistic const & GetStatistic() { return m_Statistic; }

    /*!
      @brief Return reference of MACHINEINFO information

      @return Reference to content used for system view MACHINEINFO
     */
    MachineInfo const & GetMachineInfo() { return m_MachineInfo; }

    /*!
      @brief  Return pointer to old XParam record

      Must be called adter SetKGS was called!

      @return Pointer to old C XParam record
     */
    struct rte_xparam_record *GetXParam() { return m_KGS->XParam; }

    /*!
      @brief set internal kernel state and make it visible for console
      
      On WIN32 in addition kernel state is made visible to service control manager
      or on desctop icon respectivly.

      If previous state differs, a message is written to knldiag.

      @param state [in] the new kernel state
     */
    void SetDBState(DbState state);

    /*!
      @brief  get internal kernel state
             
              The internal state of the database kernel.
      
      @return  the current kernel state
     */
    DbState DBState() const;


    /*!
      @brief  get the current time.
             
              returns the runtime global current time value (in seconds)
      
      @return  the current time 
     */
    SAPDB_ULong CurrTime()
    {
#   if defined (_WIN32)
     return m_KGS->ulCurrTime;
#   else
     return m_KGS->current_time;
#   endif
    }



    /*!
      @brief  get the kernel name
             
              The name of the database kernel.
      
      @return  the kernel name
     */
    SAPDB_Char const * DBName() const;

    /*!
      @brief set internal kernel state and make it visible for console
      
      On WIN32 in addition kernel state is made visible to service control manager
      or on desctop icon respectivly.

      If previous state differs, a message is written to knldiag.

      @param state [in] the new kernel state
     */
    SAPDB_Char * DBState_EnumToString(DbState state);


    /*!
      @brief Set the pointer to the global free list of common tasking queue elements

      Call this function with a valid free list.

      @param freeList [in] - pointer to the free list
     */
    void SetGlobalFreeList(RTETask_ICommonQueue &freeList)
    {
        m_FreeList = &freeList;
    }

    /*!
      @brief   Returns a pointer to the global free list of common tasking queue elements

               If a none task scheduler thread needs to wakeup/request
               a task this global free list has to be used!

               After the task has called 'WaitForReschedule' it  
               has to release the wakeup queue element by calling
               the queue elements methode 'Release'.

               also refer to 'CommunicationQueueElement' of 'RTETask_Task'

      @return  Pointer to the global free list of common tasking queue elements
     */
    RTETask_ICommonQueue* GlobalFreeList() 
    { 
        return m_FreeList; 
    }

    /*!
      @brief  Set the pointer to the first TaskScheduler in a linked list

      Call this function with a TaskScheduler list.

      @param taskSchedulerList [in] - Pointer to the first TaskScheduler in a linked list
     */
    void SetTaskSchedulerList( RTETask_TaskScheduler &taskSchedulerList )
    {
        m_TaskSchedulerList = &taskSchedulerList;
    }

    /*!
      @brief  Get a pointer to the first TaskScheduler in a linked list

      @return  Pointer to the first TaskScheduler in a linked list
     */
    RTETask_TaskScheduler* TaskSchedulerList() const
    {
        return m_TaskSchedulerList;
    }

    /*!
      @brief  Set the pointer to the kernel communication object

      Call this function with a kernel communication object.

      @param kernelCommunication [in] - Reference to the kernel communication object
     */
    void SetKernelCommunication( RTECKC_KernelCommunication &kernelCommunication )
    {
        m_KernelCommunication = &kernelCommunication;
    }

    /*!
      @brief  Get the pointer to the kernel communication object

      @return  Pointer to the kernel communication object
     */
    RTECKC_KernelCommunication* KernelCommunication() const
    {
        return m_KernelCommunication;
    }

    /*!
      @brief used to notify about last reset iothreadstatistics

      This call updates the internal timestamp stored for reset iothreadstatistics
     */
    void NotifyLastIOThreadStatisticsReset();

    /*!
      @brief used to notify about last reset iothreadstatistics
     */
    SAPDB_UInt8 GetLastIOThreadStatisticsReset() const { return m_LastIOThreadStatisticsReset; }

    /*!
      @brief used to notify about last reset iothreadstatistics

      This call updates the internal timestamp stored for reset iothreadstatistics
     */
    void NotifyLastTaskGroupStatisticsReset();

    /*!
      @brief used to notify about last reset iothreadstatistics
     */
    SAPDB_UInt8 GetLastTaskGroupStatisticsReset() const { return m_LastTaskGroupStatisticsReset; }

private:
    RTE_KGS();

private:
    static RTE_KGS*                      m_Instance;                    //!< The 

    KernelGlobalSection*                 m_KGS;                         //!< The pointer to the C structure 
    Statistic                            m_Statistic;                   //!< The instance information
    MachineInfo                          m_MachineInfo;                 //!< The machine information
    RTETask_ICommonQueue*                m_FreeList;                    //!< Free list for common tasking queue elements
    RTETask_TaskScheduler*               m_TaskSchedulerList;           //!< Linked list TaskScheduler objects
    RTECKC_KernelCommunication*          m_KernelCommunication;         //!< Global kernel communication objects
    SAPDB_UInt8                          m_LastIOThreadStatisticsReset; //!< time of last reset iothreadstatistics
    SAPDB_UInt8                          m_LastTaskGroupStatisticsReset; //!< time of last reset taskgroupstatistics

}; /*! @endclass RTE_KGS */

/*!
  @brief convert RTE_KGS::DbState into readable string
  @param state [in] the state to convert
 */
inline SAPDB_ToStringClass const SAPDB_ToString(RTE_KGS::DbState state)
{
    switch(state)
    {
        case RTE_KGS::RTEKGS_StateUnknown        : return SAPDB_ToStringClass("UNKNOWN");
        case RTE_KGS::RTEKGS_StateStarting       : return SAPDB_ToStringClass("STARTING");
        case RTE_KGS::RTEKGS_StateAdmin          : return SAPDB_ToStringClass("ADMIN");
        case RTE_KGS::RTEKGS_StateStandby        : return SAPDB_ToStringClass("STANDBY");
        case RTE_KGS::RTEKGS_StateOnline         : return SAPDB_ToStringClass("ONLINE");
        case RTE_KGS::RTEKGS_StateShutdown       : return SAPDB_ToStringClass("SHUTDOWN");
        case RTE_KGS::RTEKGS_StateShutdownReinit : return SAPDB_ToStringClass("SHUTDOWNREINIT");
        case RTE_KGS::RTEKGS_StateShutdownKill   : return SAPDB_ToStringClass("SHUTDOWNKILL");
        case RTE_KGS::RTEKGS_StateStop           : return SAPDB_ToStringClass("STOP");
        case RTE_KGS::RTEKGS_StateKill           : return SAPDB_ToStringClass("KILL");
        case RTE_KGS::RTEKGS_StateAbort          : return SAPDB_ToStringClass("ABORT");
        case RTE_KGS::RTEKGS_StateStopped        : return SAPDB_ToStringClass("STOPPED");
        case RTE_KGS::RTEKGS_StateUndefined      : return SAPDB_ToStringClass("UNDEFINED");
        default:                          break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)state); 
}

#endif  /* RTE_KGS_HPP */
