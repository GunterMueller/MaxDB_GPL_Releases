/*!---------------------------------------------------------------------
  @file           RTEConf_ParameterAccessKernelInterface.cpp
  @author         RobinW
  @brief          RTE configuration parameters:  class for kernel configurable parameter access

  first created  2000-03-22  17:49

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

/*PTS 1115041*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include "gsp11.h"
#include "heo11.h"
#include "RunTime/Configuration/RTEConf_BuildParamFileName.h"

#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

extern SAPDBTrace_Topic Runtime_Trace; ///< global runtime trace object

#define USE_KGS
#include "geo50_0.h"    // KGS
#include "SAPDBCommon/SAPDB_sprintf.h"

#include "heo00.h"      // for KERNEL_GLOBAL_DBNAME

#include <math.h>       /* nocheck */

#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/MemoryManagement/RTEMem_SystemPageCache.hpp"

#include "KernelCommon/ParameterNames/KernelParam_IOManager.hpp"
#include "KernelCommon/ParameterNames/KernelParam_Converter.hpp"
#include "KernelCommon/ParameterNames/KernelParam_DataAccess.hpp"
#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

#include "SAPDB/SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"

#include "hsp100.h"
#include "RunTime/RTE_Version.h"

#ifndef KERNEL_LZU
#define KERNEL_LZU
#endif

#include "RunTime/Tasking/RTETask_Task.hpp"

#include "SAPDB/SAPDBCommon/Messages/SDBMsg_RTEConf.h"
#include "RunTime/Threading/RTEThread_ConsoleStandardWorker.hpp"

#ifndef _WIN32
#include "RunTime/RTE_saveUNIXcalls.h"  /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SIZE_OF_NUMBER_BUFFER_  256

#define MAX_TASKCLUSTER_PARTS 10

#define MAX_EXCLUSIVE     250
#define MAX_REGIONS       40
#define ACT_PARAM_VERSION 0

#define MIN_DISP_LOOPS     1

#define UNKNOWN_PARAMETER_MESSAGE "unknown parameter found in XParam file: "

#define XP_V_UPDATE_OFFLINE 0
#define XP_V_UPDATE_ONLINE  1

#define XP_V_SET_TASKCLUSTER  0
#define XP_V_SET_DEVSP_NAME   1
#define XP_V_SET_DEVSP_SIZE   2
#define XP_V_SET_VOLUME_MODE  3
#define XP_V_SET_HS_NODE_NAME 4
#define XP_V_SET_FILE_SYSTEM_ACCESS_TYPE 5

#define XP_V_BOOL_Y   0  /* Bool type (TRUE if Y or y) (for YES   */
#define XP_V_BOOL_N   1  /* Bool type (TRUE if N or n) (for NO)   */
#define XP_V_BOOL_A   2  /* Bool type (TRUE if A or a) (for AUTO) */
#define XP_V_BOOL_S   3  /* Bool type (TRUE if S or s) (for STACK) */
#define XP_V_LONG     4  /* Signed Long type */
#define XP_V_ULONG    5  /* Unsigned Long type */
#define XP_V_REGIONS  6  /* Unsigned Long Region Count */
#define XP_V_STRING   7  /* Any String type */
#define XP_V_IGNORED  8  /* Accepted, but ignored */
#define XP_V_CRYPT    9  /* Crypted Password */
#define XP_V_SPECIAL_STRING  10 /* String Parameter with side effect */
#define XP_V_SPECIAL_LONG    11 /* Long Parameter with side effect */
#define XP_V_SPECIAL_ULONG   12 /* Unsigned Long Parameter with side effect */
#define XP_V_SPECIAL_SPECIAL 13 /* Other Parameter with side effect */
#define XP_V_BOOL_Y_WITH_DEFAULT    14  /* Bool type (TRUE if Y or y) "DEFAULT" allowed, means: calculate value depending on MAXCPU   */

#define XP_PARAM_CHANGED        "__PARAM_CHANGED___"

#ifdef WIN32
#define LAST_SYSTEM_ERROR_          ((SAPDB_ULong)GetLastError())
#else
#define LAST_SYSTEM_ERROR_          errno
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define ALLOCATE_ARRAY(TypeOfElement,NumberOfElements)                          \
    (TypeOfElement *)AllocateMemory(sizeof(TypeOfElement) * (NumberOfElements))

#define DEALLOCATE_MEMORY(address)                                              \
{                                                                               \
    RTEMem_RteAllocator::Instance().Deallocate((void *)(address));              \
    address = NULL;                                                             \
}

#define XP_PTR(_x,_t) (((SAPDB_Byte *)_x)+_t->offset)

#define RTE_NUMBER_OF_ELEMENTS_IN_XPARAM_TABLE1 \
	(sizeof(RteXparamTable1)/sizeof(struct xparam_strncmp_table))

#define XP_SZ_OFF(_x)  sizeof(((RTE_XPARAM_REC *)0)->_x), \
							((size_t)&(((RTE_XPARAM_REC *)0)->_x))

#define RTE_NUMBER_OF_ELEMENTS_IN_XPARAM_TABLE2 \
	(sizeof(RteXparamTable2)/sizeof(struct xparam_strcmp_table))

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
  @brief small helper class for sorted parmameter dump
 */
class  RTEConf_ParameterNameValuePair
{
public:
    RTEConf_ParameterNameValuePair( SAPDB_Char *name
                                  , SAPDB_Char *value )
                                  : m_Name(name)
                                  , m_Value(value)
                                  , m_Prev(0)
                                  , m_Next(0)
    {
    }

    void Enqueue(RTEConf_ParameterNameValuePair *prev, RTEConf_ParameterNameValuePair *next)
    {
        m_Prev = prev;
        if ( prev )
        {
            prev->SetNext(this);
            m_Prev = prev;
        }
        m_Next = next;
        if ( next )
        {
            next->SetPrev(this);
        }
    }

    /*!
      @brief compare stored name with given name

      Name comparison is done case insensitive and leading underscores are ignored

      @param name [in] name to compare
      @return 0 if identical, -1 if stored name lexicographic smaller, +1 if stored name lexicographic larger
     */
    SAPDB_Int NameCompare(SAPDB_Char const *name)
    {
        SAPDB_Char *storedName = (SAPDB_Char *)m_Name;

        while ( *name && *name == '_' )
        {
            ++name;
        }

        while ( *storedName && *storedName == '_' )
        {
            ++storedName;
        }

        while ( *storedName && *name )
        {
            SAPDB_Int delta = (((SAPDB_Int)toupper(*storedName)) - ((SAPDB_Int)toupper(*name)));

            if ( delta > 0 )
            {
                return 1;
            }
            else if (  delta < 0 )
            {
                return -1;

            }
            else
            {
                ++storedName;
                ++name;
            }
        }
        return ( *name ? 1 : -1 );
    }

    RTEConf_ParameterNameValuePair *GetPrev()          { return m_Prev; }
    RTEConf_ParameterNameValuePair *GetNext()          { return m_Next; }


    void Show()
    {
        RTE_Message( Msg_List(RTE_CONTEXT, RTEINFO_STARTUP_ARGUMENT, m_Name, m_Value) );
    }

private:
    void SetPrev(RTEConf_ParameterNameValuePair *prev) { m_Prev = prev; }
    void SetNext(RTEConf_ParameterNameValuePair *next) { m_Next = next; }

    SAPDB_ToStringClass m_Name;
    SAPDB_ToStringClass m_Value;
    RTEConf_ParameterNameValuePair *m_Prev;
    RTEConf_ParameterNameValuePair *m_Next;
};

enum xp_paramType
{
    NXP_INT_TYPE,
    NXP_STRING_TYPE,
    NXP_CRYPT_TYPE
};

//----

/*!
  tagged union of parameter records of different type
 */
struct  RTEConf_ParameterValueRecord_
{
    xp_paramType xp_value_t ;
    /*!
      union of parameter records of different type
     */
    union           of_xp_value_t
    {
        RTEConf_Parameter::Integer      xp_integer ;
        RTEConf_Parameter::String       xp_string;
        RTEConf_Parameter::CryptInfo    xp_crypt ;
    }  xp_type ;
}  ;

typedef struct RTEConf_ParameterValueRecord_   RTEConf_ParameterValueRecord ;

static SAPDB_Char* szDevType[ NUM_DEVSP_TYPES ] =
{
    (SAPDB_Char*)"SYS",
    (SAPDB_Char*)"MSYS",
    (SAPDB_Char*)"ALOG" ,
    (SAPDB_Char*)"MALOG",
    (SAPDB_Char*)"TLOG",
    (SAPDB_Char*)"DATA",
    (SAPDB_Char*)"MDATA",
    (SAPDB_Char*)"TRACE"
} ;

static struct szAccessModeMapping
{
    SAPDB_Char const *accessModeName;
    RTE_VolumeAccessMode accessMode;
} accessModeList[] = ///< list of volume access modes
{
    { "NORMAL",     RTE_VolumeAccessModeNormal },
    { "SEQUENTIAL", RTE_VolumeAccessModeSequential },
    { 0,            RTE_VolumeAccessModeNormal }
};

struct xparam_strncmp_table
{
    RTEConf_Parameter::Name name;	   /* Name found in XParam file */
    size_t                  len;       /* Maximum length to compare */

    SAPDB_Int4              type;      /* Classification of expected type */

    RTE_VolumeType          dt;
};

static  struct xparam_strncmp_table RteXparamTable1[]=
{
 { KERNELPARAM_LOG_VOLUME_NAME_ ,       PAN_ARCHIVE_LOG_N_LN,
		XP_V_SET_DEVSP_NAME,    RTE_VolumeTypeLog         },
 { KERNELPARAM_LOG_VOLUME_SIZE_,        PAN_ARCHIVE_LOG_N_SIZE_LN,
		XP_V_SET_DEVSP_SIZE,    RTE_VolumeTypeLog         },

 { KERNELPARAM_DATA_VOLUME_NAME_,       PAN_DATADEV_N_LN,
		XP_V_SET_DEVSP_NAME,    RTE_VolumeTypeData        },
 { KERNELPARAM_DATA_VOLUME_SIZE_,       PAN_DATADEV_N_SIZE_LN,
		XP_V_SET_DEVSP_SIZE,    RTE_VolumeTypeData        },
 { KERNELPARAM_DATA_VOLUME_MODE_,       PAN_DATA_VOLUME_MODE_N_LN,
		XP_V_SET_VOLUME_MODE,   RTE_VolumeTypeIgnore      },

 { KERNELPARAM_HS_NODE_,                PAN_HS_NODE_N_LN,
		XP_V_SET_HS_NODE_NAME,  RTE_VolumeTypeIgnore      },

 { KERNELPARAM_M_LOG_VOLUME_NAME_,      PAN_M_ARCHIVE_LOG_N_LN,
        XP_V_SET_DEVSP_NAME,    RTE_VolumeTypeMirrorLog   },
 { KERNELPARAM_M_DATA_VOLUME_NAME_ ,    PAN_M_DATADEV_N_LN,
        XP_V_IGNORED,           RTE_VolumeTypeIgnore      },   // mirrored data devspaces are ignored!
 { KERNELPARAM_M_SYSDEV_ ,              PAN_M_SYS_N_LN,
        XP_V_IGNORED,           RTE_VolumeTypeIgnore      },   // mirrored system devspace is ignored!

 { KERNELPARAM_SYSDEV_ ,                PAN_SYS_N_LN,
		XP_V_SET_DEVSP_NAME,    RTE_VolumeTypeSystem      },
 { KERNELPARAM_SYS_SIZE_ ,              PAN_SYS_N_SIZE_LN,
		XP_V_SET_DEVSP_SIZE,    RTE_VolumeTypeSystem      },

 { KERNELPARAM__TASKCLUSTER_ ,          PAN_TASKCLUSTER_N_LN,
		XP_V_SET_TASKCLUSTER,   RTE_VolumeTypeIgnore      },

    /* For DAFS support volume type is now relevant... PTS 1124541 */
 { KERNELPARAM_LOG_VOLUME_TYPE_ ,       PAN_ARCHIVE_LOG_N_LN,
		XP_V_SET_FILE_SYSTEM_ACCESS_TYPE,    RTE_VolumeTypeLog         },
 { KERNELPARAM_M_LOG_VOLUME_TYPE_ ,     PAN_M_ARCHIVE_LOG_N_LN,
		XP_V_SET_FILE_SYSTEM_ACCESS_TYPE,    RTE_VolumeTypeMirrorLog   },
 { KERNELPARAM_DATA_VOLUME_TYPE_,       PAN_DATADEV_N_LN,
		XP_V_SET_FILE_SYSTEM_ACCESS_TYPE,    RTE_VolumeTypeData        },
};

struct xparam_strcmp_table
{
    RTEConf_Parameter::Name name;		/* Name found in XParam file */

    SAPDB_Int4              type;       /* Claasification of expected type */

    size_t                  size;       /* Size in RTE struct */
    SAPDB_ULong             offset;     /* Offset in RTE struct */
};

static struct xparam_strcmp_table RteXparamTable2[] =
{
 { KERNELPARAM__AK_DUMP_ALLOWED,    XP_V_BOOL_Y,                XP_SZ_OFF(fAKDumpAllowed)        },
 { KERNELPARAM_SYMBOL_DEMANGLING,   XP_V_BOOL_Y,                XP_SZ_OFF(allowDemangle)         },
 { KERNELPARAM__BACKUP_HISTFILE,    XP_V_STRING,                XP_SZ_OFF(szBackupHistFile)      },
 { KERNELPARAM__BACKUP_MED_DEF,     XP_V_STRING,                XP_SZ_OFF(szBackupMedDefFile)    },
 { KERNELPARAM_CACHE_SIZE ,         XP_V_LONG,                  XP_SZ_OFF(cacheSize)             },
 { KERNELPARAM_DIAG_HISTORY_NUM,    XP_V_ULONG,                 XP_SZ_OFF(uCrashHistoryNum)      },
 { KERNELPARAM_DIAG_HISTORY_PATH,   XP_V_STRING,                XP_SZ_OFF(szCrashHistoryPath)    },
 { KERNELPARAM__DELAY_COMMIT,       XP_V_BOOL_Y,                XP_SZ_OFF(fDelayCommit)          },
 { KERNELPARAM__DELAY_LOGWRITER,    XP_V_LONG,                  XP_SZ_OFF(lDelayLW)              },
 { KERNELPARAM__IOPROCS_SWITCH,     XP_V_LONG,                  XP_SZ_OFF(lDevQueue)             },
 { KERNELPARAM_KERNELDIAGSIZE,      XP_V_ULONG,                 XP_SZ_OFF(ulDiagSize)            },
 { KERNELPARAM__EVENTSIZE,          XP_V_ULONG,                 XP_SZ_OFF(ulEventSize)           },
 { KERNELPARAM__EVENTFILE,          XP_V_STRING,                XP_SZ_OFF(szEventFile)           },
 { KERNELPARAM_FORMAT_DATAVOLUME,   XP_V_BOOL_Y,                XP_SZ_OFF(fFormatDataVolume)     },
 { KERNELPARAM_DATA_IO_BLOCK_COUNT, XP_V_LONG,                  XP_SZ_OFF(ulDataIOBlockCount)    },
 { KERNELPARAM__IOPROCS_FOR_PRIO,   XP_V_LONG,                  XP_SZ_OFF(lPrioDeviPerDevspace)  },
 { KERNELPARAM__IOPROCS_FOR_READER, XP_V_LONG,                  XP_SZ_OFF(lReaderDeviPerDevspace) },
 { KERNELPARAM__KERNELDUMPFILE,     XP_V_STRING,                XP_SZ_OFF(szDumpFilename)        },
 { KERNELPARAM__KERNELTRACEFILE,    XP_V_IGNORED,               XP_SZ_OFF(szTraceDevspace)       },
 { KERNELPARAM_KERNELTRACESIZE,     XP_V_IGNORED,               XP_SZ_OFF(lTraceDevspaceSize)    },
 { KERNELPARAM_LOAD_BALANCING_CHK,  XP_V_LONG,                  XP_SZ_OFF(uLoadBalancingCheck)   },
 { KERNELPARAM_LOAD_BALANCING_DIF,  XP_V_LONG,                  XP_SZ_OFF(uLoadBalancingDiff)    },
 { KERNELPARAM_LOAD_BALANCING_EQ,   XP_V_LONG,                  XP_SZ_OFF(uLoadBalancingEquality)},
 { KERNELPARAM_LOG_MIRRORED,        XP_V_BOOL_Y,                XP_SZ_OFF(fLogMirrored)          },
 { KERNELPARAM_MAXUSERTASKS,        XP_V_ULONG,                 XP_SZ_OFF(ulMaxUser)             },
 { KERNELPARAM_MAXBACKUPDEVS,       XP_V_LONG,                  XP_SZ_OFF(lNoOfBackupDevs)       },
 { KERNELPARAM__MAX_BACKUP_TASKS,   XP_V_ULONG,                 XP_SZ_OFF(ulMaxBackupTasks)      },
 { KERNELPARAM_MAXCPU,              XP_V_ULONG,                 XP_SZ_OFF(ulMaxCPU)              },
 { KERNELPARAM_MAXPAGER,            XP_V_ULONG,                 XP_SZ_OFF(ulMaxDataWriter)       },
 { KERNELPARAM_MAXVOLUMES,          XP_V_IGNORED,               XP_SZ_OFF(lMaxDevspaces)         },
 { KERNELPARAM__MAXEVENTS,          XP_V_ULONG,                 XP_SZ_OFF(ulMaxEvents)           },
 { KERNELPARAM__MAXEVENTTASKS,      XP_V_ULONG,                 XP_SZ_OFF(ulMaxEventTasks)       },
 { KERNELPARAM__MAXGARBAGE_COLL,    XP_V_ULONG,                 XP_SZ_OFF(ulMaxGarbageCollector) },
 { KERNELPARAM__IOPROCS_PER_DEV,    XP_V_LONG,                  XP_SZ_OFF(lDeviPerDevspace)      },
 { KERNELPARAM__MAX_MESSAGE_FILES,  XP_V_ULONG,                 XP_SZ_OFF(ulMaxMessageFiles)     },
 { KERNELPARAM_MAXSERVERTASKS,      XP_V_ULONG,                 XP_SZ_OFF(ulMaxServer)           },
 { KERNELPARAM__MAXTASK_STACK,      XP_V_ULONG,                 XP_SZ_OFF(ulMaxUsUtStack)        },
 { KERNELPARAM_MEMORY_SLOT_SIZE,    XP_V_LONG,                  XP_SZ_OFF(memorySlotSize)        },
 { KERNELPARAM__MINREPLY_SIZE,      XP_V_ULONG,                 XP_SZ_OFF(ulMinReplySize)        },
 { KERNELPARAM__MIRRORED_DATA,      XP_V_BOOL_Y,                XP_SZ_OFF(fMirrData)             },
 { KERNELPARAM__MP_DISP_LOOPS,      XP_V_ULONG,                 XP_SZ_OFF(ulDispLoopsBefSemop)   },
 { KERNELPARAM__MP_DISP_PRIO,       XP_V_BOOL_Y_WITH_DEFAULT,   XP_SZ_OFF(fDynDispQueSrch)       },
 { KERNELPARAM__MP_RGN_BUSY_WAIT,   XP_V_BOOL_Y_WITH_DEFAULT,   XP_SZ_OFF(fEndExclBusyWait)      },
 { KERNELPARAM__MP_RGN_DIRTY_READ,  XP_V_BOOL_Y_WITH_DEFAULT,   XP_SZ_OFF(fBegExclDirtyRead)     },
 { KERNELPARAM__MP_RGN_PRIO,        XP_V_BOOL_Y_WITH_DEFAULT,   XP_SZ_OFF(fBegExclQueWhenPrio)   },
 { KERNELPARAM__MP_RGN_QUEUE,       XP_V_BOOL_Y,                XP_SZ_OFF(fExclQueue)            },
 { KERNELPARAM_MMAP_MEM_POOL,       XP_V_LONG,                  XP_SZ_OFF(mmapMemPoolSize)       },
 { KERNELPARAM_OFFICIAL_NODE,       XP_V_STRING,                XP_SZ_OFF(szOfficialNode)        },
 { KERNELPARAM_OMS_HEAP_LIMIT  ,    XP_V_ULONG,                 XP_SZ_OFF(ulOMSHeapLimit)        },
 { KERNELPARAM__PACKET_SIZE,        XP_V_ULONG,                 XP_SZ_OFF(ulPacketSize)          },
 { KERNELPARAM_KERNELVERSION,       XP_V_STRING,                XP_SZ_OFF(szKernelVersion)       },
 { KERNELPARAM__PRIO_BASE_COM,      XP_V_LONG,                  XP_SZ_OFF(lPrioBaseCOM)          },
 { KERNELPARAM__PRIO_BASE_IOC,      XP_V_LONG,                  XP_SZ_OFF(lPrioBaseIOC)          },
 { KERNELPARAM__PRIO_BASE_RAV,      XP_V_LONG,                  XP_SZ_OFF(lPrioBaseRAV)          },
 { KERNELPARAM__PRIO_BASE_REX,      XP_V_LONG,                  XP_SZ_OFF(lPrioBaseREX)          },
 { KERNELPARAM__PRIO_BASE_U2U,      XP_V_LONG,                  XP_SZ_OFF(lPrioBaseU2U)          },
 { KERNELPARAM__PRIO_FACTOR,        XP_V_LONG,                  XP_SZ_OFF(lPrioFactor)           },
 { KERNELPARAM__RTEDUMPFILE,        XP_V_STRING,                XP_SZ_OFF(szRTEDumpFilename)     },
 { KERNELPARAM_RUNDIRECTORY,        XP_V_STRING,                XP_SZ_OFF(szRunDirectory)        },
 { KERNELPARAM_SET_VOLUME_LOCK,     XP_V_BOOL_Y,                XP_SZ_OFF(fSetVolumeLock)        },
 { KERNELPARAM_SHM_MEM_POOL,        XP_V_LONG,                  XP_SZ_OFF(shmMemPoolSize)        },
 { KERNELPARAM_SHOW_MAX_STACK_USE,  XP_V_BOOL_Y,                XP_SZ_OFF(showMaxStackUse)       },
 { KERNELPARAM_TRACE_PAGES_LW,      XP_V_LONG,                  XP_SZ_OFF(tracePagesAL)          },
 { KERNELPARAM_TRACE_PAGES_BUP,     XP_V_LONG,                  XP_SZ_OFF(tracePagesBUP)         },
 { KERNELPARAM_TRACE_PAGES_PG,      XP_V_LONG,                  XP_SZ_OFF(tracePagesDW)          },
 { KERNELPARAM_TRACE_PAGES_EV,      XP_V_LONG,                  XP_SZ_OFF(tracePagesEV)          },
 { KERNELPARAM_TRACE_PAGES_GC,      XP_V_LONG,                  XP_SZ_OFF(tracePagesGC)          },
 { KERNELPARAM_TRACE_PAGES_SV,      XP_V_LONG,                  XP_SZ_OFF(tracePagesSV)          },
 { KERNELPARAM_TRACE_PAGES_TI,      XP_V_LONG,                  XP_SZ_OFF(tracePagesTI)          },
 { KERNELPARAM_TRACE_PAGES_TW,      XP_V_LONG,                  XP_SZ_OFF(tracePagesTW)          },
 { KERNELPARAM_TRACE_PAGES_US,      XP_V_LONG,                  XP_SZ_OFF(tracePagesUS)          },
 { KERNELPARAM_TRACE_PAGES_UT,      XP_V_LONG,                  XP_SZ_OFF(tracePagesUT)          },
 { KERNELPARAM__USE_ASYNC_IO,       XP_V_BOOL_Y,                XP_SZ_OFF(fUseAsyncIO)           },
 { KERNELPARAM__USE_IOPROCS_ONLY,   XP_V_BOOL_N,                XP_SZ_OFF(fAllowSingleIO)        },
 { KERNELPARAM__UTILITY_PROTFILE,   XP_V_STRING,                XP_SZ_OFF(szUtilityProtFile)     },
 { KERNELPARAM_UTILITY_PROTSIZE,    XP_V_ULONG,                 XP_SZ_OFF(ulUtilityProtSize)     },
 { KERNELPARAM_VALLOC_MEM_POOL,     XP_V_LONG,                  XP_SZ_OFF(vallocMemPoolSize)     },
 { KERNELPARAM___PARAM_CHANGED___,  XP_V_ULONG,                 XP_SZ_OFF(ulXparamChanged)       },
 { KERNELPARAM_TIME_MEASUREMENT,    XP_V_BOOL_Y,                XP_SZ_OFF(fTimeMeasure)          },
 { KERNELPARAM_SYMBOL_RESOLUTION,   XP_V_BOOL_Y,                XP_SZ_OFF(allowSymbolResolution) },
 { KERNELPARAM_PREALLOCATE_IOWORKER,XP_V_BOOL_Y,                XP_SZ_OFF(preallocateIOWorker)   },
/***************************************************************/
/* OS Depending Parameter                                      */
/***************************************************************/
#if defined(_WIN32)
 { KERNELPARAM_ALLOW_COMPR_DEVSP,   XP_V_BOOL_Y,    XP_SZ_OFF(fAllowCompressedDevsp) },
 { KERNELPARAM_DYNAMIC_SERVER,      XP_V_BOOL_Y,    XP_SZ_OFF(fDynamicServerTasks)   },
 { KERNELPARAM_MEM_ENHANCE_LIMIT,   XP_V_LONG,      XP_SZ_OFF(lMemEnhancementLimit)  },
 { KERNELPARAM_USE_FIBERS,          XP_V_BOOL_Y,    XP_SZ_OFF(fUseFibers)            },
 { KERNELPARAM_USE_MEM_ENHANCE,     XP_V_BOOL_Y,    XP_SZ_OFF(fUseMemEnhancement)    },
#else
 { KERNELPARAM_OPMSG1,              XP_V_STRING,    XP_SZ_OFF(szOpMsg1)              },
 { KERNELPARAM_OPMSG2,              XP_V_STRING,    XP_SZ_OFF(szOpMsg2)              },
 { KERNELPARAM__SHMKERNEL,          XP_V_LONG,      XP_SZ_OFF(lShmKernel)            },
 { KERNELPARAM__USE_PROC_SCOPE,     XP_V_BOOL_Y,    XP_SZ_OFF(fUseProcessScope)      },  /* PTS 1109046 */
 { KERNELPARAM_SUPPRESS_CORE,       XP_V_BOOL_Y,    XP_SZ_OFF(suppressCore)          },
 { KERNELPARAM_USE_COROUTINES,      XP_V_BOOL_Y,    XP_SZ_OFF(fUseCoroutines)        },
 { KERNELPARAM_USE_STACK_ON_STACK,  XP_V_BOOL_Y,    XP_SZ_OFF(fUseStackOnStack)      },
 { KERNELPARAM_USE_UCONTEXT,        XP_V_BOOL_Y,    XP_SZ_OFF(fUseUcontext)          },
 { KERNELPARAM_SIMULATE_VECTORIO,   XP_V_STRING,    XP_SZ_OFF(szVectorIoMode)        },
#endif
#if defined(_WIN32) || defined(LINUX)
 { KERNELPARAM_HIRES_TIMER_TYPE,    XP_V_STRING,    XP_SZ_OFF(szHiResTimerType)      },
#endif
 { KERNELPARAM_FORBID_LOAD_BALANCING, XP_V_BOOL_Y,  XP_SZ_OFF(fForbidLoadBalancing)  },

/***************************************************************/
/* End of OS Depending Parameter                               */
/***************************************************************/
};
/***************************************************************/
/* the follwoing entries in the array were not used and therefore have been removed: */
// { KERNELPARAM__COMMENT,            XP_V_IGNORED,   0, 0                             },
// { KERNELPARAM_FORMATTING_MODE,     XP_V_IGNORED,   0, 0                             },
// { KERNELPARAM_HEAP_MEM_POOL,       XP_V_LONG,      XP_SZ_OFF(heapMemPoolSize)       },
// { KERNELPARAM_LOG_MODE,            XP_V_IGNORED,   0, 0                             },
// { KERNELPARAM_SERVERDB,            XP_V_IGNORED,   0, 0                             },
// { KERNELPARAM__PAGE_SIZE,          XP_V_IGNORED,   0, 0 },
/*
 To enable page size parameter via 'XPARAM' use this, but since the kernel still uses tsp00_Page...

 { KERNELPARAM__PAGE_SIZE,          XP_V_IGNORED,   XP_SZ_OFF(ulDBPageSize)          },
 */
// { KERNELPARAM_GATEWAYTYPE,         XP_V_IGNORED,   0, 0                             },
// { KERNELPARAM__TASK_STACK_AREA,    XP_V_IGNORED,   0, 0                             },

class ScopeLockedNonExclusive
{
public:
    ScopeLockedNonExclusive(RTESync_IRWRegion *pRWRegion):
      m_pRWRegion(pRWRegion)
    {
        m_task = RTETask_Task::Self();
        if(m_task)
        {
            if(m_pRWRegion->getLastEntered() != m_task->ID())
            {
                m_pRWRegion->enter(false,m_task->ID());
                m_enteredNonExclusive = true;
            }
            else
            {
                m_enteredNonExclusive = false;
            }
        }
    }

    ~ScopeLockedNonExclusive()
    {
        if(m_task)
        {
            if( m_enteredNonExclusive )
                m_pRWRegion->leave(false,m_task->ID());
        }
    }
private:
    tsp00_TaskId m_taskId;
    RTETask_Task *m_task;
    RTESync_IRWRegion *m_pRWRegion;
    SAPDB_Bool m_enteredNonExclusive;
};

class ScopeLockedExclusive
{
public:
    ScopeLockedExclusive(RTESync_IRWRegion *pRWRegion):
      m_pRWRegion(pRWRegion)
    {
        m_task = RTETask_Task::Self();
        if(m_task)
            m_pRWRegion->enter(true,m_task->ID());
    };

    ~ScopeLockedExclusive()
    {
        if(m_task)
            m_pRWRegion->leave(true,m_task->ID());
    };
private:
    RTETask_Task *m_task;
    RTESync_IRWRegion *m_pRWRegion;
};



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static inline void *AllocateMemory(size_t size);

static SAPDB_Bool RTEConf_AnalyzeParam  ( RTE_XPARAM_REC               *XParam,
                                          RTEConf_Parameter::Name      name,
                                          RTEConf_ParameterValueRecord *xpr ,
                                          SAPDB_Bool                   inOnlineMode,
                                          SAPDBErr_MessageList         &err) ;

static SAPDB_Int4 RTEConf_FindParam ( RTEConf_Parameter::Name             name ,
                                      RTEConf_ParameterValueRecord        *xpr ,
                                      SAPDBErr_MessageList &err);

static void RTEConf_InitParam          ( RTE_XPARAM_REC        *XParam  );

static SAPDB_Bool RTEConf_ReadSpecialParameters(RTE_XPARAM_REC *XParam,SAPDBErr_MessageList &err);

static void RTEConf_ModifyParameters(RTE_XPARAM_REC *XParam);

static void RTEConf_SetTaskCluster ( RTE_XPARAM_REC            *XParam,
                                       RTEConf_Parameter::String szTaskCluster,
                                       SAPDB_ULong               ulTCNo );

static SAPDB_Bool RTEConf_SetDevspName  ( RTE_XPARAM_REC               *XParam,
                                          RTEConf_Parameter::Name      DevspName ,
                                          RTEConf_ParameterValueRecord *xpr ,
                                          SAPDB_Int                    lDevType ,
                                          SAPDB_ULong                  ulDevspNo ,
                                          SAPDBErr_MessageList         &err);

static SAPDB_Bool RTEConf_SetDevspSize  ( RTE_XPARAM_REC *XParam,
                                          SAPDB_Long     lDevType ,
                                          SAPDB_ULong    ulDevspNo ,
                                          SAPDB_Int      ulDevspSize );

static  SAPDB_Bool RTEConf_SetVolumeMode ( RTE_XPARAM_REC           *XParam,
                                           SAPDB_Char               *szAccessMode,
                                           SAPDB_ULong               ulDevspNo ,
                                           SAPDBErr_MessageList     &err);

static RTEConf_Parameter::Integer  RTEConf_XParamInt ( RTEConf_ParameterValueRecord  *xpr ,
                                                       SAPDBErr_MessageList          &err) ;

static void  RTEConf_AllocDevspDescr ( RTE_XPARAM_REC *XParam,
                                       SAPDB_Long     lDevspType,
                                       SAPDB_Int      ulNumDevsp);

static SAPDB_Bool RTEConf_SetFileSystemAccessType  ( RTE_XPARAM_REC *XParam,
                                          SAPDB_Long                lDevType ,
                                          SAPDB_Char               *szFileSystemAccessType,
                                          SAPDB_ULong               ulDevspNo ,
                                          SAPDBErr_MessageList     &err);


static SAPDB_Bool RTEConf_UpdateXparamParameter(    RTEConf_Parameter::Name name,
                                                    RTEConf_Parameter::Type type,
                                                    void *newValue,
                                                    void *context,
                                                    SAPDBErr_MessageList &err);

static SAPDB_Bool RTEConf_UpdateTimeMeasurementParameter( RTEConf_Parameter::Name name,
                                                          RTEConf_Parameter::Type type,
                                                          void *newValue,
                                                          void *context,
                                                          SAPDBErr_MessageList &err);

static SAPDB_Bool RTEConf_SetulRegionCollisionLoop(     RTEConf_Parameter::Integer newValue,
                                                        RTE_XPARAM_REC *XParam,
                                                        SAPDBErr_MessageList &err);

static SAPDB_Bool RTEConf_UpdateulRegionCollisionLoop(  RTEConf_Parameter::Name name,
                                                        RTEConf_Parameter::Type type,
                                                        void *newValue,
                                                        void *context,
                                                        SAPDBErr_MessageList &err);

static SAPDB_Bool RTEConf_SetulregLockSlice(            RTEConf_Parameter::Integer newValue,
                                                        RTE_XPARAM_REC *XParam,
                                                        SAPDBErr_MessageList &err);

static SAPDB_Bool RTEConf_UpdateulregLockSlice(         RTEConf_Parameter::Name name,
                                                        RTEConf_Parameter::Type type,
                                                        void *newValue,
                                                        void *context,
                                                        SAPDBErr_MessageList &err);

static SAPDB_Bool RTEConf_UpdateMessageListLimit(       RTEConf_Parameter::Name name,
                                                        RTEConf_Parameter::Type type,
                                                        void *newValue,
                                                        void *context,
                                                        SAPDBErr_MessageList &err);

static SAPDB_Bool RTEConf_FullPathsOfFilenamesAreEqual( SAPDB_Char *pFileName1,
                                                        SAPDB_Char *pFileName2 );

static SAPDB_Bool isRelativePath(SAPDB_Char *path);

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!--------------------------------------------------------------------
   @description    Check if the configured volumes do not exceed a given limit
   @param          limit [in] the limit in bytes to check the volume sizes against
   @return value   false if the size of one or more configured volumes exceeds the limit, true if not

 --------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_CheckVolumeSize(RTE_VolumeType type,RTE_XPARAM_REC *XparamParameters,SAPDB_UInt8 limit)
{
    SAPDB_UInt4 i;
    for(i=0;i<XparamParameters->DevspDescr[type].ulNumDevs;i++)
    {
        if(( DEVSPACE_UNUSED != XparamParameters->DevspDescr[type].Devspaces[i].lDevspaceType )
                             && XparamParameters->DevspDescr[type].Devspaces[i].sizeLimitedByUlimit )
        {
            SAPDB_UInt8 volumeSizeInBytes = XparamParameters->DevspDescr[type].Devspaces[i].lDevspaceSize * XparamParameters->ulDBPageSize;
            if(volumeSizeInBytes > limit)
            {
                RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_FSIZE_EXCEEDS_ULIMIT1,
                    SAPDB_ToString(volumeSizeInBytes / (1024*1024)),
                    SAPDB_ToString(type),
                    SAPDB_ToString(i+1),
                    SAPDB_ToString(limit / (1024*1024))));
                RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_FSIZE_EXCEEDS_ULIMIT2));
                return false;
            }
        }
    }
    return true;
}


externC SAPDB_Bool RTEConf_GetVolumeType( tsp00_VFilename &volumeName, RTE_FileSystemAccessType &fsType )
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_GetVolumeType", Runtime_Trace, 5);

    SAPDB_Bool   Ok = true;;
    SAPDB_Int4   devSpaceIterator;
    tsp00_VFilenamec volumeNamec;

    volumeNamec.p2c(volumeName);

    for(devSpaceIterator = (SAPDB_Int4)RTE_VolumeTypeSystem ; devSpaceIterator <= (SAPDB_Int4)RTE_VolumeTypeTrace ; devSpaceIterator++)
    {
        DEVSPACES_DESCR   *pDevspDesc;
        SAPDB_UInt4        ulDevNo;
        SAPDB_Char      *pDevspaceName;

        pDevspDesc = KGS->XParam->DevspDescr + devSpaceIterator;

        for ( ulDevNo = 0; ulDevNo < pDevspDesc->ulNumDevs; ulDevNo++ )
        {
            if (pDevspDesc->Devspaces[ulDevNo].lDevspaceType == DEVSPACE_UNUSED)
                continue;

            pDevspaceName = (SAPDB_Char *)pDevspDesc->Devspaces[ulDevNo].szDevspaceName;
            Ok = RTEConf_FullPathsOfFilenamesAreEqual( pDevspaceName,volumeNamec );
            if ( Ok )
            {
                fsType = pDevspDesc->Devspaces[ulDevNo].fileSystemAccessType;
                return true;
            }
        }
    }

    return false;
}


/*!--------------------------------------------------------------------
   @description    Check if given names has duplicate in devspacesArray
   @param          devspacesArray [in] Array of devspace description arrays
   @param          szDevspaceName [in] Devspacename to check
   @return value   true is no duplicate, false if duplicate

 --------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_CheckDuplicateDevSpaceName( DEVSPACES_DESCR *devspacesArray,
                                                       SAPDB_Char      *szDevspaceName,
                                                       Msg_List        &errList )
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_CheckDuplicateDevSpaceName", Runtime_Trace, 5);

    SAPDB_Bool   Ok = true;;
    SAPDB_Int4   devSpaceIterator;
    for(devSpaceIterator = (SAPDB_Int4)RTE_VolumeTypeSystem ; devSpaceIterator <= (SAPDB_Int4)RTE_VolumeTypeTrace ; devSpaceIterator++)
    {
        DEVSPACES_DESCR   *pDevspDesc;
        SAPDB_UInt4        ulDevNo;
        SAPDB_Char      *pDevspaceName;

        pDevspDesc = devspacesArray + devSpaceIterator;

        for ( ulDevNo = 0; ulDevNo < pDevspDesc->ulNumDevs; ulDevNo++ )
        {
            if (pDevspDesc->Devspaces[ulDevNo].lDevspaceType == DEVSPACE_UNUSED)
                continue;

            pDevspaceName = (SAPDB_Char *)pDevspDesc->Devspaces[ulDevNo].szDevspaceName;

            Ok = !RTEConf_FullPathsOfFilenamesAreEqual( pDevspaceName,szDevspaceName );
            if ( !Ok )
            {
                errList = Msg_List(
	                Msg_List::Error,
	                SDBMSG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION,
                    Msg_Arg(SDBMSGTAG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION__DEVICE_OR_FILE,szDevspaceName),
	                Msg_Arg(SDBMSGTAG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION__VOLUME_TYPE,SAPDB_ToString((RTE_VolumeType)devSpaceIterator)),
	                Msg_Arg(SDBMSGTAG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION__VOLUME_ID,ulDevNo+1)
                    );
                return Ok;
            }
        }
    }

    return Ok;
}

static void MessageAndCrash(const SAPDBErr_MessageList &err)
{
    RTE_Message(err);
    SLEEP(10000);
    SAPDBErr_MessageList emptyMessage;
    RTE_Crash(emptyMessage);
}
/* some parameters have to be read before others can be read, for example MAXARCHIVELOGS must be read before ARCHIVE_LOG_* can be read */

static SAPDB_Bool RTEConf_ReadSpecialParameters(RTE_XPARAM_REC *XParam,SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Integer valueInt;
    RTEConf_Parameter::String valueString;

    if((RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM_KERNELVERSION,valueString,err))
    {
        if ((strncmp ( (SAPDB_Char *)valueString + BEG_OF_1ST_VERS_SUBSTR_SP100,
                RTE_GetKernelVersion() + BEG_OF_1ST_VERS_SUBSTR_SP100,
                END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100  + 1 )) ||
            (strncmp ( (SAPDB_Char *)valueString + BEG_OF_2CND_VERS_SUBSTR_SP100,
                RTE_GetKernelVersion() + BEG_OF_2CND_VERS_SUBSTR_SP100,
                END_OF_2CND_VERS_SUBSTR_SP100 - BEG_OF_2CND_VERS_SUBSTR_SP100  + 1 )))
        {
            Msg_List errList = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INCOMPATIBLE_VERSIONS);
            errList.AppendNewMessage(Msg_List(RTE_CONTEXT,RTEERR_CONF_RUNNING_KERNEL_VERSION,RTE_GetKernelVersion()));
            errList.AppendNewMessage(Msg_List(RTE_CONTEXT,RTEERR_CONF_PARAMFILE_VERSION,SAPDB_ToString((SAPDB_Char *)valueString)));
            RTE_Crash(errList);
        }
    }
    else
    {
        return false;
    }
    RTEConf_AllocDevspDescr ( XParam, RTE_VolumeTypeTrace, 1 ) ;
/*
    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM__PAGE_SIZE,valueInt,err))
    {
		if ( valueInt != XParam->ulDBPageSize )
		{
        // put a trace output here...
        }
    }
 */
    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_MAXCPU,valueInt,err))
    {
        XParam->ulMaxCPU = (tsp00_Uint)valueInt;
    }

    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_MAXRGN_REQUEST,valueInt,err))
    {
        RTEConf_SetulregLockSlice(valueInt,XParam,err);
    }

    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_MP_RGN_LOOP,valueInt,err))
    {
        RTEConf_SetulRegionCollisionLoop(valueInt,XParam,err);
    }

    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_MAX_MESSAGE_LIST_LENGTH,valueInt,err))
    {
        RTE_IInterface::Instance().SetMessageLimit((SAPDB_UInt4)valueInt);
    }

    if((RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM_FORMATTING_MODE,valueString,err))
    {
		switch ( valueString[0] )
		{
            case 'a' :
            case 'A' :
                XParam->ulFormattingMode = RTE_FORMATTING_MODE_AUTO;
                break ;
            case 'p' :
            case 'P' :
                XParam->ulFormattingMode = RTE_FORMATTING_MODE_PARALLEL;
                break ;
            case 's' :
            case 'S' :
                XParam->ulFormattingMode = RTE_FORMATTING_MODE_SERIAL;
                break ;
            default  :
                MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_XP_INVALID_FORMATTING_MODE,(SAPDB_Char*)(valueString)));
            break;
		}
    }
    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_MAXVOLUMES,valueInt,err))
    {
        XParam->lMaxDevspaces = (SAPDB_Int)valueInt;
    }

#ifdef SYSTEMDEVSPACE_MIGRATION_QUESTION_ANSWERED
    SAPDB_Int maxSysDevspaces = 0; /* ---> default 0 would be more accurate, but TorstenS needs some  */
#else
    SAPDB_Int maxSysDevspaces = 1;
#endif
    /* Still needed, but only for inplace migration... */
    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM__MAXSYSDEVSPACES,valueInt,err))
    {
        maxSysDevspaces = (SAPDB_Int)valueInt;
    }
    if ( maxSysDevspaces > 0 )
    {
        RTEConf_AllocDevspDescr ( XParam, RTE_VolumeTypeSystem, maxSysDevspaces ) ;
    }
#ifdef SYSTEMDEVSPACE_MIGRATION_QUESTION_ANSWERED
    XParam->lMaxDevspaces += maxSysDevspaces;
#else
    ++(XParam->lMaxDevspaces); /* always add a worker for System Devspace migration... see above... */
#endif

    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_MAXLOGVOLUMES,valueInt,err))
    {
        SAPDB_Int maxArchiveLogs = (SAPDB_Int)valueInt;
        RTEConf_AllocDevspDescr ( XParam,
                                  RTE_VolumeTypeLog,
                                  maxArchiveLogs);
        RTEConf_AllocDevspDescr ( XParam,
                                  RTE_VolumeTypeMirrorLog,
                                  maxArchiveLogs );
    }
    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_MAXDATAVOLUMES,valueInt,err))
    {
        SAPDB_Int maxDataDevspaces = (SAPDB_Int)valueInt;
        RTEConf_AllocDevspDescr ( XParam,
                                  RTE_VolumeTypeData,
                                  maxDataDevspaces ) ;
    }

    if((RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_KERNELTRACESIZE,valueInt,err))
    {
        XParam->lTraceDevspaceSize =
            XParam->DevspDescr [ RTE_VolumeTypeTrace ].Devspaces[0].lDevspaceSize =
                (SAPDB_Int)valueInt;
    }

    if((RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM__KERNELTRACEFILE,valueString,err))
    {
        strncpy(XParam->szTraceDevspace,(char *)valueString,sizeof(tsp2_devnamec));
        strncpy(XParam->DevspDescr [ sp2dt_trace ].Devspaces[0].szDevspaceName,(char *)valueString,sizeof(tsp2_devnamec));
        XParam->DevspDescr [ RTE_VolumeTypeTrace ].Devspaces[0].lDevspaceType = RTE_VolumeTypeTrace ;
    }

    return true;
}

// Since Windows Runtime does not allow message voutput in a very early state, these parameter handlings
// are withdrawn from RTEConf_ReadSpecialParameters()... If knldiag is usable, this is no longer needed
extern "C" void RTEConf_HandleSpecialParametersWithMessageOutput()
{
    RTEConf_Parameter::Integer valueInt;
    RTEConf_Parameter::String valueString;
    SAPDBErr_MessageList err;

    if( (RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_MEMORY_ALLOCATION_LIMIT,valueInt,err)
      && 0 != valueInt )
    {
        SAPDB_ULong oldLimit;
        RTE_ISystem::Instance().SetMemoryLimitInBytes(((SAPDB_Int)valueInt) * 1024, oldLimit);
        RTE_Message(SAPDBErr_MessageList( RTE_CONTEXT,
                                          RTEINFO_CONF_MEMORY_ALLOCATION_LIMIT,
                                          SAPDB_ToString((SAPDB_Int)valueInt)
                                        ));
    }
    else
    {
        RTE_Message(SAPDBErr_MessageList( RTE_CONTEXT,
                                          RTEINFO_CONF_MEMORY_ALLOCATION_UNLIMITED
                                        ));
    }

    if( !(RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM_USE_SYSTEM_PAGE_CACHE,valueString,err)
	  || ( *valueString == 'n' || *valueString == 'N' ) )
    {
        RTEMem_SystemPageCache::Instance().Disable();
        RTE_Message(SAPDBErr_MessageList( RTE_CONTEXT,
                                          RTEINFO_CONF_SYSTEM_PAGE_CACHE_DISABLED
                                        ));
    }
    else
    {
        RTE_Message(SAPDBErr_MessageList( RTE_CONTEXT,
                                          RTEINFO_CONF_SYSTEM_PAGE_CACHE_ENABLED
                                        ));
    }
}

//----

static void RTEConf_ModifyParameters(RTE_XPARAM_REC *XParam)
{
    RTEConf_ParameterAccess * parameter;
    RTEConf_Parameter::Integer valueInt;
    RTEConf_Parameter::String valueString;
    SAPDBErr_MessageList err;

    XParam->ulMaxUsUtStack  <<= 10;

    XParam->ulPacketSize    &= ~7;
    XParam->ulMinReplySize  &= ~7;

	if ( XParam->ulDispLoopsBefSemop < 2 )
		XParam->ulDispLoopsBefSemop = 2;

    parameter = RTEConf_ParameterAccess::Instance();

    if(parameter->GetInteger(KERNELPARAM__DATA_CACHE_RGNS,valueInt,err))
    { XParam->ulNoOfRegions += (SAPDB_Int)valueInt;  }
    if(parameter->GetInteger(KERNELPARAM__TRANS_RGNS,valueInt,err))
    { XParam->ulNoOfRegions += (SAPDB_Int)valueInt;  }
    if(parameter->GetInteger(KERNELPARAM__TAB_RGNS,valueInt,err))
    { XParam->ulNoOfRegions += (SAPDB_Int)valueInt;  }
    if(parameter->GetInteger(KERNELPARAM__ROW_RGNS,valueInt,err))
    { XParam->ulNoOfRegions += (SAPDB_Int)valueInt;  }
    if(parameter->GetInteger(KERNELPARAM__OMS_RGNS,valueInt,err))
    { XParam->ulNoOfRegions += (SAPDB_Int)valueInt;  }
    if(parameter->GetInteger(KERNELPARAM_CONVERTER_REGIONS,valueInt,err))
    { XParam->ulNoOfRegions += (SAPDB_Int)valueInt;  }
    if(parameter->GetInteger(KERNELPARAM__TREE_RGNS,valueInt,err))
    { XParam->ulNoOfRegions += /* NumSplitt = NumTreev */ 2 * (SAPDB_Int)valueInt; }
    if(parameter->GetInteger(KERNELPARAM_USED_MAX_LOG_QUEUE_COUNT,valueInt,err)) // PTS 1133794 mb 2005-02-07
    { XParam->ulNoOfRegions += (SAPDB_Int)valueInt;  }

    if(parameter->GetString(KERNELPARAM_LOG_MODE,valueString,err))
    {
        if ( 'U' == toupper(valueString[1]) )   //log mode dUal ?
        {
            XParam->fLogMirrored = true;
        }
    }

    if( XParam->fLogMirrored )
    {
        SAPDB_ULong numberOfLogDevSpaces = XParam->DevspDescr[RTE_VolumeTypeLog].ulNumDevs;
        for(SAPDB_ULong currentDevSpace = 0;currentDevSpace < numberOfLogDevSpaces;currentDevSpace++)
        {
            XParam->DevspDescr[RTE_VolumeTypeMirrorLog].Devspaces[currentDevSpace].lDevspaceSize
                = XParam->DevspDescr[RTE_VolumeTypeLog].Devspaces[currentDevSpace].lDevspaceSize;
        }
    }
    else
    {
        RTEMem_RteAllocator::Instance().Deallocate(XParam->DevspDescr[RTE_VolumeTypeMirrorLog].Devspaces);
        XParam->DevspDescr[RTE_VolumeTypeMirrorLog].Devspaces = 0;
        XParam->DevspDescr[RTE_VolumeTypeMirrorLog].ulNumDevs = 0 ;
    }

    if ( XParam->fForbidLoadBalancing )
    {
        XParam->uLoadBalancingCheck = 0;
    }
    else if (( XParam->uLoadBalancingCheck != 0) && (XParam->uLoadBalancingCheck < 2 ))
    {
        XParam->uLoadBalancingCheck = 2;
    }

#ifndef WIN32
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1132876 */
         if ( 0 == SAPDB_strcasecmp(XParam->szVectorIoMode.asCharp(), "NEVER") )
    {
        XParam->simulateVectorIo = RTE_SimVioNever;
    }
    else if ( 0 == SAPDB_strcasecmp(XParam->szVectorIoMode.asCharp(), "IF_OPEN_DIRECT") )
    {
        XParam->simulateVectorIo = RTE_SimVioIfOpenDirect;
    }
    else if ( 0 == SAPDB_strcasecmp(XParam->szVectorIoMode.asCharp(), "IF_RAW_DEVICE") )
    {
        XParam->simulateVectorIo = RTE_SimVioIfRawDevice;
    }
    else if ( 0 == SAPDB_strcasecmp(XParam->szVectorIoMode.asCharp(), "IF_OPEN_DIRECT_OR_RAW_DEVICE") )
    {
        XParam->simulateVectorIo = RTE_SimVioIfOpenDirectOrRawDevice;
    }
    else if ( 0 == SAPDB_strcasecmp(XParam->szVectorIoMode.asCharp(), "ALWAYS") )
    {
        XParam->simulateVectorIo = RTE_SimVioAlways;
    }
    else
    {
#if defined(LINUX)
        SAPDB_strcpy(XParam->szVectorIoMode.asCharp(), "IF_OPEN_DIRECT_OR_RAW_DEVICE");
        XParam->simulateVectorIo = RTE_SimVioIfOpenDirectOrRawDevice;
#else
        SAPDB_strcpy(XParam->szVectorIoMode.asCharp(), "NEVER");
        XParam->simulateVectorIo = RTE_SimVioNever;
#endif
    }
#endif
}

//----

void RTEConf_AddParameterToParameterList(RTEConf_ParameterNameValuePair *&pList, SAPDB_Char *name, SAPDB_Char *value)
{
    RTEConf_ParameterNameValuePair *pNewEntry = new ( RTEMem_RteAllocator::Instance() ) 
                                RTEConf_ParameterNameValuePair(name, value);
    if ( !pNewEntry )
    {
        return; /* out of memory ignored */
    }

    RTEConf_ParameterNameValuePair *pLast = pList;
    for ( RTEConf_ParameterNameValuePair *pSearch = pList; pLast && pSearch; pSearch = pSearch->GetNext() )
    {
        pLast = pSearch;

        switch ( pSearch->NameCompare(name) )
        {
        case 0:
            /* doubled entry ignored!!!! */
            destroy(pNewEntry, RTEMem_RteAllocator::Instance());
            pLast = 0;
            break;
        case 1:
            pNewEntry->Enqueue(pSearch->GetPrev(), pSearch);
            pLast = 0;
            break;
        default:
            break;
        }
    }

    if ( pNewEntry )
    {
        if ( pLast )
        {
            pNewEntry->Enqueue(pLast, 0);
        }

        if ( 0 == pNewEntry->GetPrev() )
        {
            pList = pNewEntry; /* if new header, set it as new header */
        }
    }
}

//----

void RTEConf_DumpParameterList(RTEConf_ParameterNameValuePair *pList)
{
    RTEConf_ParameterNameValuePair *pNext;
    for ( RTEConf_ParameterNameValuePair *pEntry = pList; pEntry; pEntry = pNext )
    {
        pNext = pEntry->GetNext();
        pEntry->Show();
        destroy(pEntry, RTEMem_RteAllocator::Instance());
    }
}

//----

externC SAPDB_Bool RTEConf_ReadAndAnalyzeParam ( tsp00_DbNamec VAR_ARRAY_REF    ServerDB,
                                                 RTE_XPARAM_REC                 *XParam ,
                                                 Msg_List                       &err)
{
    SAPDB_memset (XParam, 0, sizeof(RTE_XPARAM_REC) );
    RTEConf_ParameterReader paramReader;
    RTEConf_ParameterNameValuePair *pList = 0; 

    /* --- Set XPARAM default values */
    RTEConf_InitParam ( XParam );

    RTE_Message( SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_BEGIN_OF_STARTUP_ARGUMENTS) );

    /* read the special parameters other parameters depend on (MAXARCHIVELOGS etc.) */
    if(RTEConf_ReadSpecialParameters(XParam,err))
    {
        paramReader.StartIteration();

        RTEConf_Parameter::Name name;
        RTEConf_Parameter::Type type;
        RTEConf_ParameterValueRecord valueRec;
        while(paramReader.Next(name,type,err))
        {
            switch (type)
            {
            case RTEConf_Parameter::t_Integer:
            {
                RTEConf_Parameter::Integer value;
                RTEConf_ParameterAccess::Instance()->GetInteger(name,value,err);
                RTEConf_AddParameterToParameterList(pList, (SAPDB_Char *)name, SAPDB_ToString( (SAPDB_Int4)value ));
                valueRec.xp_value_t = NXP_INT_TYPE;
                valueRec.xp_type.xp_integer = (SAPDB_Int4)value;
                break;
            }
            case RTEConf_Parameter::t_Real:
                /* No real parameters yet.. */
                break;
            case RTEConf_Parameter::t_String:
            {
                RTEConf_Parameter::String value;
                RTEConf_ParameterAccess::Instance()->GetString(name,value,err);
                RTEConf_AddParameterToParameterList(pList, (SAPDB_Char *)name, (SAPDB_Char *)value );
                valueRec.xp_value_t = NXP_STRING_TYPE;
                valueRec.xp_type.xp_string = value;
                break;
            }
            case RTEConf_Parameter::t_CryptInfo:
            {
                RTEConf_Parameter::CryptInfo value;
                RTEConf_ParameterAccess::Instance()->GetCryptInfo(name,value,err);
                RTEConf_AddParameterToParameterList(pList, (SAPDB_Char *)name, (SAPDB_Char *)"***" );
                valueRec.xp_value_t = NXP_CRYPT_TYPE;
                SAPDB_MemCopyNoCheck(valueRec.xp_type.xp_crypt,(SAPDB_Byte *)value,sizeof(RTEConf_Parameter::CryptInfo));
                break;
            }
            default:
                continue;
            }
            err.ClearMessageList();
            if(!RTEConf_AnalyzeParam ( XParam , name , &valueRec ,false, err))
            {
                return false;
            }
        }

        RTEConf_DumpParameterList(pList);

        RTE_Message( SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_END_OF_STARTUP_ARGUMENTS) );

        RTEConf_ModifyParameters(XParam);
    }

    if(!(err.IsEmpty() || SAPDBErr_MessageList::Info == err.Type()) || !(paramReader.EndIteration(err)))
    {
        return false;
    }
    else
    {
        return true;
    }
}

/* This routine should only be used in ( .c ) modules that cannot handle Msg_Lists */

externC SAPDB_Bool RTEConf_ReadAndAnalyzeParamWithOutput ( tsp00_DbNamec VAR_ARRAY_REF    ServerDB,
                                                            RTE_XPARAM_REC                 *XParam )
{
    Msg_List errList;
    SAPDB_Bool returnValue = RTEConf_ReadAndAnalyzeParam( ServerDB, XParam, errList );
    if(!returnValue)
    {
        RTE_Message(errList);
    }
    return returnValue;
}

externC void   RTEConf_FreeTaskClusterArrayInXParamStructure( RTE_XPARAM_REC *XParam)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_FreeTaskClusterArrayInXParamStructure", Runtime_Trace, 5);

    if ( XParam->TaskClusterArray != NULL )
    {
        int i ;
        for ( i=0; XParam->TaskClusterArray[ i ] ; i++ )
        {
            RTEMem_RteAllocator::Instance().Deallocate ( XParam->TaskClusterArray[ i ] ) ;
        }
        RTEMem_RteAllocator::Instance().Deallocate ( XParam->TaskClusterArray ) ;
    }
}

externC void RTEConf_FreeDevDescrInXParamStructure( RTE_XPARAM_REC *XParam,int idx )
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_FreeDevDescrInXParamStructure", Runtime_Trace, 5);

    RTEMem_RteAllocator::Instance().Deallocate(XParam->DevspDescr[idx].Devspaces);
}

static void  RTEConf_CopyStringOutOfValueRecord ( unsigned char *s, RTEConf_ParameterValueRecord * xpr ,size_t sizeOfDestinationArea )

{
    size_t lengthOfSourceString = strlen((char *)xpr->xp_type.xp_string);
    if( lengthOfSourceString > sizeOfDestinationArea)
    {
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_ENOUGH_SPACE_FOR_STRING_PARAM,SAPDB_ToString(sizeOfDestinationArea),SAPDB_ToString(lengthOfSourceString)));
    }
    if ( NXP_STRING_TYPE != xpr->xp_value_t  )
    {
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_TYPE_MISMATCH,SAPDB_ToString(xpr->xp_value_t),SAPDB_ToString(NXP_STRING_TYPE)));
    }

    strncpy  ((char *) s , (char *)xpr->xp_type.xp_string , lengthOfSourceString + 1 );
}

static inline void *AllocateMemory(size_t size)
{
    void *adr = RTEMem_RteAllocator::Instance().Allocate(size);
    if(!adr)
    {
        SAPDB_Char sizeString[256];
        SAPDB_sprintf(sizeString,256,"%d bytes",size);
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ALLOCATION_FAILED,sizeString));
    }
    return adr;
}

static SAPDB_Bool RTEConf_AnalyzeParam ( RTE_XPARAM_REC                 *XParam  ,
                                         RTEConf_Parameter::Name        name ,
                                         RTEConf_ParameterValueRecord   *xpr ,
                                         SAPDB_Bool                     inOnlineMode,
                                         SAPDBErr_MessageList           &err)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_AnalyzeParam", Runtime_Trace, 5);
    SAPDB_Bool fOk = true;
    SAPDB_Int4 which;
    RTEConf_Parameter::String valueString = xpr->xp_type.xp_string;
    struct xparam_strcmp_table *pE;

	which = RTEConf_FindParam(name, xpr, err);
	if ( which < 0 )
	{
        if(inOnlineMode)
        {
            return false;
        }
        else
        {
/*IST on request of Juergen Primsch 1993-01-05: suppress this warning */
            SAPDB_UInt8 lengthOfMessage = (SAPDB_UInt8)strlen((SAPDB_Char *)name) + strlen(UNKNOWN_PARAMETER_MESSAGE) + 1;
            SAPDB_Char *message = (SAPDB_Char *)alloca((int)lengthOfMessage);
            SAPDB_sprintf(message,(int)lengthOfMessage,UNKNOWN_PARAMETER_MESSAGE"%s",(SAPDB_Char *)name);
            SAPDBTRACE_WRITELN (Runtime_Trace, 1, message);
            return true;
        }
	}

	if ( which < RTE_NUMBER_OF_ELEMENTS_IN_XPARAM_TABLE1 )
	{
        struct xparam_strncmp_table *pN;
        SAPDB_UInt2 indexOfElement;

		pN = &RteXparamTable1[which];

        indexOfElement = atoi( (SAPDB_Char *)name + pN->len + 1 ) - 1;

		switch( pN->type )
		{
		case XP_V_SET_TASKCLUSTER:
            RTEConf_SetTaskCluster( XParam, xpr->xp_type.xp_string, indexOfElement ) ;
            // if anything had gone wrong, RTEConf_SetTaskCluster() would have called RTE_Crash()...
            break;

		case XP_V_SET_DEVSP_NAME:
            fOk = RTEConf_SetDevspName ( XParam, name, xpr, pN->dt, indexOfElement ,err);
			break;

        case XP_V_SET_DEVSP_SIZE:
			fOk = RTEConf_SetDevspSize ( XParam, pN->dt,
									indexOfElement, (SAPDB_Int)RTEConf_XParamInt ( xpr ,err) ) ;
			break;

		case XP_V_SET_VOLUME_MODE:
            fOk = RTEConf_SetVolumeMode ( XParam, (SAPDB_Char *)xpr->xp_type.xp_string, indexOfElement, err);
			break;

        case XP_V_SET_HS_NODE_NAME:
            fOk = (RTEConf_ParameterAccess::Instance())->AddHssNode((SAPDB_Char *)valueString,indexOfElement);
            if(!fOk)
                MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_INVALID_NODE_NAME_ENTRY,(SAPDB_Char *)name,(SAPDB_Char *)valueString ));
			break;

        case XP_V_SET_FILE_SYSTEM_ACCESS_TYPE:
            fOk = RTEConf_SetFileSystemAccessType ( XParam,
                                                    pN->dt,
                                                    (SAPDB_Char *)xpr->xp_type.xp_string,
                                                    indexOfElement,
                                                    err);
			break;

        default:
            break;
        }
	}
	else
	{

		/*
		 * Reduced to offset into RteXparamTable2
		 */
		which -= RTE_NUMBER_OF_ELEMENTS_IN_XPARAM_TABLE1;
		pE = &RteXparamTable2[which];

        switch( pE->type )
		{
		case XP_V_BOOL_Y:
			*(tsp00_Bool *)XP_PTR(XParam, pE) =
					(tsp00_Bool)( *valueString == 'y' || *valueString == 'Y' );
			break;

		case XP_V_BOOL_Y_WITH_DEFAULT:
            switch(*valueString)
            {
            case 'd':
            case 'D':
                if(1 == XParam->ulMaxCPU)
                {
                    *(tsp00_Bool *)XP_PTR(XParam, pE) = false;
                }
                else
                {
                    *(tsp00_Bool *)XP_PTR(XParam, pE) = true;
                }
                break;
            case 'y':
            case 'Y':
                *(tsp00_Bool *)XP_PTR(XParam, pE) = true;
                break;
            case 'n':
            case 'N':
                *(tsp00_Bool *)XP_PTR(XParam, pE) = false;
                break;
            }
			break;
        case XP_V_BOOL_N:
			*(tsp00_Bool *)XP_PTR(XParam, pE) =
					(tsp00_Bool)( *valueString == 'n' || *valueString == 'N' );
			break;

		case XP_V_BOOL_A:
			*(tsp00_Bool *)XP_PTR(XParam, pE) =
					(tsp00_Bool)( *valueString == 'a' || *valueString == 'A' );
			break;

		case XP_V_BOOL_S:
			*(tsp00_Bool *)XP_PTR(XParam, pE) =
					(tsp00_Bool)( *valueString == 's' || *valueString == 'S' );
			break;

		case XP_V_LONG:
			*(tsp00_Int *)XP_PTR(XParam, pE) = (tsp00_Int)RTEConf_XParamInt( xpr , err);
			break;

		case XP_V_ULONG:
			*(tsp00_Uint *)XP_PTR(XParam, pE) = (tsp00_Uint)RTEConf_XParamInt( xpr ,err);
			break;

        case XP_V_STRING:
            RTEConf_CopyStringOutOfValueRecord (  XP_PTR(XParam, pE), xpr, pE->size);
            break;

		case XP_V_CRYPT:
			SAPDB_MemCopyNoCheck( XP_PTR(XParam,pE), xpr->xp_type.xp_crypt , pE->size ) ;
			break;

		case XP_V_IGNORED:
			break;

        case XP_V_SPECIAL_LONG:
			*(tsp00_Int *)XP_PTR(XParam, pE) = (tsp00_Int)RTEConf_XParamInt( xpr, err);
			/* Currently, there are no special longs */
            break;

        case XP_V_SPECIAL_ULONG:
			*(tsp00_Uint *)XP_PTR(XParam, pE) = (tsp00_Uint)RTEConf_XParamInt( xpr, err);
            break;

		case XP_V_SPECIAL_STRING:
            break;
        case XP_V_SPECIAL_SPECIAL:
			break;

		default:
			break;
		}
	}
    return fOk;
}

/*
 * ===========================================================================
 */

static  SAPDB_Bool RTEConf_SetDevspSize ( RTE_XPARAM_REC            *XParam,
                                          SAPDB_Long                lDevType ,
                                          SAPDB_ULong               ulDevspNo ,
                                          SAPDB_Int                 ulDevspSize )
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_SetDevspSize", Runtime_Trace, 5);

    SAPDB_Bool fOk ;

    if ( ulDevspSize == 0 )
        return ( TRUE ) ;

    if ( ( fOk = XParam->DevspDescr [ lDevType ]. ulNumDevs > ulDevspNo ) )
    {
        DEV_DESCR *ActDevsp = XParam->DevspDescr[lDevType].Devspaces + ulDevspNo;
        ActDevsp->lDevspaceSize = ulDevspSize ;
    }
    else
    {
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_XP_TOO_MANY_DEVSPACES, szDevType[lDevType] ,SAPDB_ToString(XParam->DevspDescr [ lDevType ]. ulNumDevs )));
    }

    return (fOk) ;
}

/*
 * ===========================================================================
 */

static  SAPDB_Bool RTEConf_SetVolumeMode ( RTE_XPARAM_REC           *XParam,
                                           SAPDB_Char               *szAccessMode,
                                           SAPDB_ULong               ulDevspNo,
                                           SAPDBErr_MessageList     &err)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_SetVolumeMode", Runtime_Trace, 5);

    SAPDB_Bool fOk ;
    if ( ( fOk = XParam->DevspDescr [ RTE_VolumeTypeData ]. ulNumDevs > ulDevspNo ) )
    {
        DEV_DESCR *ActDevsp = XParam->DevspDescr[RTE_VolumeTypeData].Devspaces + ulDevspNo;
        int iMode;

        for ( iMode = 0; accessModeList[iMode].accessModeName; iMode++ )
        {
            if ( 0 == strncmp( szAccessMode,
                               accessModeList[iMode].accessModeName,
                               strlen(accessModeList[iMode].accessModeName)) )
            {
                ActDevsp->accessMode = accessModeList[iMode].accessMode;
                RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT,RTEINFO_CONF_DATA_VOLUME_MODE, SAPDB_ToString(szAccessMode), SAPDB_ToString(ulDevspNo+1)));
                return fOk;
            }
        }

        RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT,RTEWARN_CONF_UNKNOWN_VOLUME_ACCESS_MODE, SAPDB_ToString(szAccessMode), SAPDB_ToString(ulDevspNo+1)));
    }
    else
    {
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_XP_TOO_MANY_DEVSPACES, szDevType[RTE_VolumeTypeData] ,SAPDB_ToString(XParam->DevspDescr [ RTE_VolumeTypeData ]. ulNumDevs )));
    }

    return (fOk) ;
}

/*
 * ===========================================================================
 */

/* for DAFS support PTS 1124541 */
static SAPDB_Bool RTEConf_SetFileSystemAccessType  ( RTE_XPARAM_REC *XParam,
                                          SAPDB_Long                lDevType ,
                                          SAPDB_Char               *szFileSystemAccessType,
                                          SAPDB_ULong               ulDevspNo ,
                                          SAPDBErr_MessageList     &err)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_SetVolumeMode", Runtime_Trace, 5);

    SAPDB_Bool fOk ;

    if ( ( fOk = XParam->DevspDescr [ lDevType ]. ulNumDevs > ulDevspNo ) )
    {
        DEV_DESCR *ActDevsp = XParam->DevspDescr[lDevType].Devspaces + ulDevspNo;
        switch( toupper(*szFileSystemAccessType) )
        {
        case 'F': /* file */
            ActDevsp->sizeLimitedByUlimit = true;
        case 'L': /* link */
        case 'R': /* raw device */
        case 'P': /* pipe */
            ActDevsp->fileSystemAccessType = RTE_FileSystemAccessTypeNormal;
            break;
        case 'D': /* DAFS file */
            ActDevsp->fileSystemAccessType = RTE_FileSystemAccessTypeDAFS;
            break;
        default:
            ActDevsp->fileSystemAccessType = RTE_FileSystemAccessTypeNormal;
            RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT,RTEWARN_CONF_UNKNOWN_FILE_SYSTEM_ACCESS_TYPE, SAPDB_ToString(szFileSystemAccessType), szDevType[lDevType], SAPDB_ToString(ulDevspNo+1)));
            break;
        }
    }
    else
    {
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_XP_TOO_MANY_DEVSPACES, szDevType[lDevType] ,SAPDB_ToString(XParam->DevspDescr [ lDevType ]. ulNumDevs )));
    }

    return (fOk) ;
}

/*
 * ===========================================================================
 */

static  void   RTEConf_SetTaskCluster( RTE_XPARAM_REC            *XParam,
                                       RTEConf_Parameter::String szTaskCluster,
                                       SAPDB_ULong               ulTCNo )
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_SetTaskCluster", Runtime_Trace, 5);

    size_t TaskClusterLen = STRLEN_UTF8( szTaskCluster ) + 1;

    if ( XParam->TaskClusterArray == NULL )
    {
        XParam->TaskClusterArray = (char **)RTEMem_RteAllocator::Instance().Allocate(MAX_TASKCLUSTER_PARTS * sizeof ( char * ) );
        if ( 0 == XParam->TaskClusterArray)
        {
            MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_MALLOC_FAILED_FOR,"set TASKCLUSTER" ));
        }
        else
        {
            SAPDB_memset ( XParam->TaskClusterArray, 0,
                MAX_TASKCLUSTER_PARTS * sizeof ( char * ) ) ;
        }
    }

    if (ulTCNo >= MAX_TASKCLUSTER_PARTS-1 )
    {
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_XP_TOO_MANY_TASKCLUSTER,SAPDB_ToString(ulTCNo),SAPDB_ToString(MAX_TASKCLUSTER_PARTS)));
    }
    XParam->TaskClusterArray[ulTCNo] = (char *)RTEConf_MakeACopyOfString(szTaskCluster);
}


/*
 * ===========================================================================
 */

static void RTEConf_InitParam ( RTE_XPARAM_REC *XParam  )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEConf_InitParam", Runtime_Trace, 5);

  /* All parameters not set here are automatically set to 0 / FALSE */

  XParam->ulRegLockSlice        = MAX_EXCLUSIVE;
  XParam->ulDispLoopsBefSemop   = MIN_DISP_LOOPS;
  XParam->fExclQueue            = TRUE;
  XParam->lPrioBaseU2U          = 100;
  XParam->lPrioBaseRAV          = 80;
  XParam->lPrioBaseIOC          = 80;
  XParam->lPrioBaseCOM          = 10;
  XParam->lPrioBaseREX          = 40;
  XParam->lPrioFactor           = 80;
  XParam->fFormatDataVolume     = TRUE;
  XParam->ulMaxCPU              = 1;

  XParam->ulMaxEvents           = 100;
  XParam->ulMaxMessageFiles     = 5;

/***************************************************************/
#if defined(_WIN32)
/***************************************************************/
#if defined ( DYNAMIC_SERVER )
   XParam->fDynamicServerTasks  = TRUE;
#endif

  XParam->fUseMemEnhancement    = FALSE;
  XParam->lMemEnhancementLimit  = 0;
  XParam->fUseFibers            = TRUE;
  XParam->fUseAsyncIO           = 1;

/***************************************************************/
#else
/***************************************************************/
#ifdef OSF1
  XParam->fUseAsyncIO           = 1 ;
#endif
/***************************************************************/
#endif
/***************************************************************/

  XParam->ulDBPageSize          = sizeof ( tsp00_Page ) ; /* XXX default value, overwritten by XParam... Will detect if tsp00_Page is really gone.. */
  XParam->ulNoOfRegions         = MAX_REGIONS ;

  XParam->memorySlotSize        = 1; /* 1 MB memory slot size */

  /* Specify fastest allocation on the system */
#if defined(_WIN32)
  XParam->vallocMemPoolSize     = -1; /* Use valloc allocated memory */
#elif defined(HPUX)
  XParam->shmMemPoolSize        = -1; /* Use 'SYSV IPC Shm' allocated memory */
#else /* all other UNIX */
  XParam->mmapMemPoolSize       = -1; /* Use mmap allocated memory */
#endif

  XParam->allowDemangle         = true;
  XParam->allowSymbolResolution = true;

  return;
}


void RTEConf_ParameterAccess::RegisterUpdateFunctions(void)
{
    SAPDBErr_MessageList err;
    // parameters that need special treatment because they may be set to default values depending on MAXCPU
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM_MP_RGN_LOOP,       RTEConf_UpdateulRegionCollisionLoop, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM_MAXRGN_REQUEST,    RTEConf_UpdateulregLockSlice,  NULL, err);
    // parameters that can be found in RteXparamTable1/2
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__USE_IOPROCS_ONLY, RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__IOPROCS_SWITCH,   RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__MP_RGN_DIRTY_READ,RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__MP_RGN_BUSY_WAIT, RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__MP_DISP_LOOPS,    RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__MP_DISP_PRIO,     RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__MP_RGN_PRIO,      RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__PRIO_BASE_U2U,    RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__PRIO_BASE_IOC,    RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__PRIO_BASE_RAV,    RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__PRIO_BASE_REX,    RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__PRIO_BASE_COM,    RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM__PRIO_FACTOR,      RTEConf_UpdateXparamParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM_TIME_MEASUREMENT,  RTEConf_UpdateTimeMeasurementParameter, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM_MAX_MESSAGE_LIST_LENGTH, RTEConf_UpdateMessageListLimit, NULL, err);
    m_Conf_Parameter->RegisterUpdateFunction(KERNELPARAM_SYMBOL_RESOLUTION, RTEConf_UpdateXparamParameter, NULL, err);
}

static  void    RTEConf_AllocDevspDescr ( RTE_XPARAM_REC         *XParam,
                                          SAPDB_Long             lDevspType,
                                          SAPDB_Int              ulNumDevsp)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_AllocDevspDescr", Runtime_Trace, 5);

    SAPDB_Int i ;

    XParam->DevspDescr[lDevspType].Devspaces = (struct dev_descr *)RTEMem_RteAllocator::Instance().Allocate(ulNumDevsp * sizeof ( DEV_DESCR ) );
    if(0 == XParam->DevspDescr[lDevspType].Devspaces )
    {
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_MALLOC_FAILED_FOR,"DEVSPACE_DESCR" ));
    }

    for ( i=0; i < ulNumDevsp; i++ )
    {
        XParam->DevspDescr[lDevspType].Devspaces[i].lDevspaceType = DEVSPACE_UNUSED;
        XParam->DevspDescr[lDevspType].Devspaces[i].physicalDevID = RTE_UNDEF_ID;
        XParam->DevspDescr[lDevspType].Devspaces[i].accessMode    = RTE_VolumeAccessModeNormal;
        XParam->DevspDescr[lDevspType].Devspaces[i].fileSystemAccessType = RTE_FileSystemAccessTypeNormal;
        XParam->DevspDescr[lDevspType].Devspaces[i].lDevspaceSize = 0;
        XParam->DevspDescr[lDevspType].Devspaces[i].sizeLimitedByUlimit = false;
    }

    XParam->DevspDescr[lDevspType].ulNumDevs = ulNumDevsp ;
}

/*
 * ===========================================================================
 */

static  SAPDB_Bool RTEConf_SetDevspName ( RTE_XPARAM_REC                  *XParam,
                                          RTEConf_Parameter::Name         DevspName ,
                                          RTEConf_ParameterValueRecord    *xpr ,
                                          SAPDB_Int                       lDevType ,
                                          SAPDB_ULong                     ulDevspNo ,
                                          SAPDBErr_MessageList            &err)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_SetDevspName", Runtime_Trace, 5);

    SAPDB_Bool fOk ;

    RTEConf_Parameter::String szDevName = xpr->xp_type.xp_string;
    if ( szDevName[0] == '\0' )
    {
        fOk = FALSE;
    }
    else
    {
        fOk = RTEConf_CheckDuplicateDevSpaceName( XParam->DevspDescr, (SAPDB_Char *)szDevName, err );

        if ( fOk )
        {
            if (( fOk = XParam->DevspDescr [ lDevType ]. ulNumDevs > ulDevspNo ))
            {
                DEV_DESCR *ActDevsp = XParam->DevspDescr[lDevType].Devspaces + ulDevspNo;
                size_t count = strlen((const char*) szDevName);
                if (count > sizeof(tsp2_devname))
                {
                    SAPDB_MemCopyNoCheck (ActDevsp->szDevspaceName, szDevName, sizeof(tsp2_devname));
                    ActDevsp->szDevspaceName[sizeof(tsp2_devname)] = 0;
                }
                else
                {
                    SAPDB_MemCopyNoCheck (ActDevsp->szDevspaceName, szDevName, count + 1);
                }
                ActDevsp->lDevspaceType = lDevType ;
            }
            else
            {
                MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_XP_TOO_MANY_DEVSPACES, szDevType[lDevType] ,SAPDB_ToString(XParam->DevspDescr [ lDevType ]. ulNumDevs )));
            }
        }
        else
        {
            err.Overrule(Msg_List(
                Msg_List::Error,SDBMSG_RTECONF_ERROR_IN_VOLUME_DEFINTION,
                Msg_Arg("VOLUME_TYPE",SAPDB_ToString((RTE_VolumeType)lDevType)),
                Msg_Arg("VOLUME_ID",ulDevspNo+1)));
        }
    }
    return (fOk) ;
}

static RTEConf_Parameter::Integer  RTEConf_XParamInt ( RTEConf_ParameterValueRecord  *xpr ,SAPDBErr_MessageList &err)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_XParamInt", Runtime_Trace, 5);

    if ( NXP_INT_TYPE  == xpr->xp_value_t )
    {
        return ( xpr->xp_type.xp_integer );
    }
    else
    {
   		err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ILLEGAL_XP_TYPE, SAPDB_ToString( xpr->xp_value_t ));
        return ( -1 );
    }

}

/*
 * Search parameter in table and return index if found. -1 if not found
 * The returned index can be used as index into RteXparamTable1.
 */
static SAPDB_Int4 RTEConf_FindParam( RTEConf_Parameter::Name             name ,
                                     RTEConf_ParameterValueRecord        *xpr ,
                                     SAPDBErr_MessageList                &err)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEConf_FindParam", Runtime_Trace, 5);

    SAPDB_Int4 which;

	/*
     * First entry is special! All other are compared with strcmp()...
     */
	for ( which = 0; which < RTE_NUMBER_OF_ELEMENTS_IN_XPARAM_TABLE1; which ++ )
	{
		struct xparam_strncmp_table *pN;

		pN = &RteXparamTable1[which];
		if ( !STRNCMP_UTF8(name, pN->name, pN->len) )
		{
          if ( xpr )
			switch( pN->type )
            {
			case XP_V_SET_TASKCLUSTER:
			case XP_V_SET_DEVSP_NAME:
			case XP_V_SET_VOLUME_MODE:
            case XP_V_SET_FILE_SYSTEM_ACCESS_TYPE:
                if( NXP_STRING_TYPE != xpr->xp_value_t )
                {
   		            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ILLEGAL_XP_TYPE, SAPDB_ToString(  xpr->xp_value_t));
					which = -1;
				}
				break;

			case XP_V_SET_DEVSP_SIZE:
                if( NXP_INT_TYPE != xpr->xp_value_t )
                {
   		            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ILLEGAL_XP_TYPE, SAPDB_ToString( xpr->xp_value_t ));
					which = -1;
				}
				break;
            default:
				break;
			}
			return which;
		}
	}

    for ( which = 0; which < RTE_NUMBER_OF_ELEMENTS_IN_XPARAM_TABLE2; which++ )
	{
		struct xparam_strcmp_table *pE;
		pE = &RteXparamTable2[which];

        if ( !STRNCMP_UTF8( name, pE->name, STRLEN_UTF8(pE->name) ) )
		{
          if ( xpr )
			switch( pE->type )
			{
			case XP_V_LONG:
			case XP_V_ULONG:
			case XP_V_REGIONS:
			case XP_V_SPECIAL_LONG:
			case XP_V_SPECIAL_ULONG:
                if( NXP_INT_TYPE != xpr->xp_value_t )
                {
   		            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ILLEGAL_XP_TYPE ,SAPDB_ToString( xpr->xp_value_t ));
					which = -1;
				}
                break;

			case XP_V_BOOL_Y:
			case XP_V_BOOL_Y_WITH_DEFAULT:
			case XP_V_BOOL_N:
			case XP_V_BOOL_A:
			case XP_V_BOOL_S:
			case XP_V_STRING:
			case XP_V_SPECIAL_STRING:
                if( NXP_STRING_TYPE != xpr->xp_value_t )
                {
   		            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ILLEGAL_XP_TYPE ,SAPDB_ToString( xpr->xp_value_t ));
					which = -1;
				}
				break;

			case XP_V_IGNORED:
			case XP_V_SPECIAL_SPECIAL:
				break;

			case XP_V_CRYPT:
                if( NXP_CRYPT_TYPE != xpr->xp_value_t )
                {
   		            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ILLEGAL_XP_TYPE, SAPDB_ToString( xpr->xp_value_t ));
					which = -1;
				}
				break;

			default:
				which = -1;
				break;
			}
			break;
		}
	}

	if ( which == RTE_NUMBER_OF_ELEMENTS_IN_XPARAM_TABLE2 )
	{
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_XP_UNKNOWN_PARAM,(SAPDB_Char *)(name));
		which = -1;
	}

	/*
	 * Add number of elements in first table (subtracted in analyze_param...
	 */
	if ( which >= 0 )
	{
		which += RTE_NUMBER_OF_ELEMENTS_IN_XPARAM_TABLE1;
	}

	return which;
}


/*!--------------------------------------------------------------------
   @description    Compare the fully expanded file paths of two given file names
   @param          pFileName1 [in] pointer to the first file name
   @param          pFileName2 [in] pointer to the second file name
   @return value   true if the expanded file paths are the same, false if not

 --------------------------------------------------------------------*/

static SAPDB_Bool RTEConf_FullPathsOfFilenamesAreEqual( SAPDB_Char *pFileName1,SAPDB_Char *pFileName2 )
{
    SAPDB_Char *pFilePath1;
    SAPDB_Char *pFilePath2;
#if defined (_WIN32)
    SAPDB_UInt4 bytesNeeded = GetFullPathName( pFileName1,0,NULL,NULL ); // this call returns the number of bytes needed for the full path
    if ( 0 != bytesNeeded )
    {
        pFilePath1 = (SAPDB_Char *)alloca(bytesNeeded);
        bytesNeeded = GetFullPathName( pFileName1,bytesNeeded,pFilePath1,NULL );
    }
    if ( 0 == bytesNeeded ) // bytesNeeded = 0 means that something went wrong (unlikely), so take the base name
    {
        pFilePath1 = pFileName1;
    }

    bytesNeeded = GetFullPathName( pFileName2,0,NULL,NULL ); // this call returns the number of bytes needed for the full path
    if ( 0 != bytesNeeded ) 
    {
        pFilePath2 = (SAPDB_Char *)alloca(bytesNeeded);
        bytesNeeded = GetFullPathName( pFileName2,bytesNeeded,pFilePath2,NULL );
    }
    if ( 0 == bytesNeeded )// bytesNeeded = 0 means that something went wrong (unlikely), so take the base name
    {
        pFilePath2 = pFileName2;
    }

    return ( 0 == _stricmp( pFilePath1, pFilePath2 ) );
#else
    struct stat stat1;
    struct stat stat2;

    SAPDB_Bool match = false;
    if( (0 == RTE_save_stat(pFileName1,&stat1)) && (0 == RTE_save_stat(pFileName2,&stat2)) )
    {
        if( stat1.st_mode == stat2.st_mode) // st_mode tells if the object represented by name is a device or a file or...
        {                                   // if the st_mode fields do not match, the two objects are not the same
            if( S_ISBLK( stat1.st_mode ) || S_ISCHR( stat1.st_mode ) )
            {
                if( stat1.st_rdev == stat2.st_rdev )    // for two devices that are the same, the st_rdev fields match
                {
                    match = true;                
                }
            }
            else
            {
                if( (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino) )  // for two regular files that are the same, the st_dev and st_ino fields match
                {
                    match = true;                
                }
            }
        }
    }
    return match;
#endif
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool isRelativePath(SAPDB_Char *path)
{
#ifdef _WIN32
    return path[0]!='\\' && (path[1]!=':' || path[2]!='\\');
#else
    return path[0]!='/';
#endif
}

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                       class RTEConf_ParameterAccess                       */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/

RTEConf_ParameterAccess *RTEConf_ParameterAccess::m_Instance=NULL;

/*---------------------------------------------------------------------------*/

void RTEConf_ParameterReader::StartIteration(void)
{
    m_NumberBuffer = ALLOCATE_ARRAY(SAPDB_Char,SIZE_OF_NUMBER_BUFFER_);
    m_Iterator = new RTEConf_ParameterIterator(*(RTEConf_ParameterAccess::Instance())->m_Conf_Parameter);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterReader::EndIteration(SAPDBErr_MessageList &err)
{
	if(NULL == m_Iterator)
	{
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ITERATION_HAD_NOT_BEEN_STARTED);
        return false;
	}
    else
    {
		DEALLOCATE_MEMORY(m_NumberBuffer);
		delete m_Iterator;
		m_Iterator = NULL;
		return true;
	}
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterReader::Next(RTEConf_Parameter::Name &parameterName,SAPDB_Char *&valueString,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err)
{
	if(NULL == m_Iterator)
	{
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ITERATION_HAD_NOT_BEEN_STARTED);
        return false;
	}
    else
    {
		RTEConf_Parameter::Type parameterType;
		SAPDB_Bool endOfListReached;
        SAPDBErr_MessageList err;
        while(m_Iterator->Next(parameterName,parameterType,endOfListReached,err))
		/* this is done in a loop, because it is possible that the Iterator::Next() gives elements that are actually not in the parameter set anymore
           (this happens when elements are Delete()d after the construction of the iterator. If such an element is detected, the next element is searched */
		{
			switch(parameterType)
			{
			case RTEConf_Parameter::t_String:
				{
					RTEConf_Parameter::String stringValue;
					if((RTEConf_ParameterAccess::Instance())->GetString(parameterName,stringValue,updateOnline,permanentChange,err))
					{
						valueString = (SAPDB_Char *)stringValue;
						return true;
					}
				}
				break;
			case RTEConf_Parameter::t_Integer:
				{
					RTEConf_Parameter::Integer integerValue;
					if((RTEConf_ParameterAccess::Instance())->GetInteger(parameterName,integerValue,updateOnline,permanentChange,err))
					{
						SAPDB_sprintf(m_NumberBuffer,SIZE_OF_NUMBER_BUFFER_,"%qd",integerValue);
						valueString = m_NumberBuffer;
						return true;
					}
				}
				break;
			case RTEConf_Parameter::t_Real:
				{
					RTEConf_Parameter::Real realValue;
					if((RTEConf_ParameterAccess::Instance())->GetReal(parameterName,realValue,updateOnline,permanentChange,err))
					{
						SAPDB_sprintf(m_NumberBuffer,SIZE_OF_NUMBER_BUFFER_,"%f",realValue);
						valueString = m_NumberBuffer;
						return true;
					}
				}
				break;
			case RTEConf_Parameter::t_CryptInfo:
				{
					RTEConf_Parameter::CryptInfo cryptInfoValue;
					if((RTEConf_ParameterAccess::Instance())->GetCryptInfo(parameterName,cryptInfoValue,updateOnline,permanentChange,err))
					{
        				valueString = (SAPDB_Char *)"";
						return true;
					}
				}
				break;
			}
		}
	}
	return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterReader::Next(RTEConf_Parameter::Name &parameterName,RTEConf_Parameter::Type &parameterType,SAPDBErr_MessageList &err)
{
	if(NULL == m_Iterator)
	{
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ITERATION_HAD_NOT_BEEN_STARTED);
        return false;
	}
    else
	{
		SAPDB_Bool endOfListReached;
		return m_Iterator->Next(parameterName,parameterType,endOfListReached,err);
	}
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterAccess *RTEConf_ParameterAccess::Instance(void)
{
    if(NULL == m_Instance)
    {
        SAPDB_Char *dbName;
        dbName = KERNEL_GLOBAL_DBNAME;
        m_Instance = new RTEConf_ParameterAccess(dbName);
    }
    return m_Instance;
}

/*---------------------------------------------------------------------------*/
#define FORMAT_STRING_FOR_FORMAT_STRING "%%s%%0%dd"
#define NUMBER_OF_DOUBLED_PERCENTS      2

SAPDB_Char *RTEConf_ParameterAccess::BuildIndexedParamName(
    SAPDB_Char *baseParamName,
    SAPDB_UInt indexDigits,
    SAPDB_UInt indexToUse)
{
    SAPDB_UInt sizeOfFormatString   = (SAPDB_UInt)strlen(FORMAT_STRING_FOR_FORMAT_STRING)
                                    - NUMBER_OF_DOUBLED_PERCENTS
                                    + (SAPDB_UInt)log10((float)indexDigits);
    SAPDB_Char *formatString = (SAPDB_Char *)alloca(sizeOfFormatString);
    SAPDB_sprintf(formatString,sizeOfFormatString,FORMAT_STRING_FOR_FORMAT_STRING,indexDigits);

    SAPDB_UInt sizeOfIndexedParamName = (SAPDB_UInt)strlen(baseParamName) + indexDigits + 1;
    SAPDB_Char *indexedParamName = (SAPDB_Char *)AllocateMemory(sizeOfIndexedParamName);
    SAPDB_sprintf(indexedParamName,sizeOfIndexedParamName,formatString,baseParamName,indexToUse);

    return indexedParamName;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::AddHssNode(SAPDB_Char *nodeName,SAPDB_UInt indexOfElement)
{
    // indexOfElement is a C-array index. That means that for HS_NODE_001 indexOfElement is 0 etc..
    SAPDB_UInt numberOfElement = indexOfElement + 1;
    RTE_HssNode newNode;
    newNode.index = numberOfElement;
    strncpy(newNode.name , nodeName,sizeof(RTE_Nodename));

    Container_List < RTE_HssNode > ::Iterator iterator = m_hssNodeList.Begin();
    while(iterator != m_hssNodeList.End())
    {
        if((0 == strcmp(iterator->name,nodeName)))
        {
            m_hssNodeListConsistent = false;
        }
        ++iterator;
    }

    if(!m_hssNodeList.InsertEnd( newNode ).IsValid())
    {
        return false;
    }

    SAPDB_Char *localNodeName = (SAPDB_Char *)RTE_ISystem::Instance().GetLocalNodeName();
    if( 0 == strcmp(nodeName,localNodeName))
    {
        m_localHSSIndex = numberOfElement;
    }

    if(1 == numberOfElement) // def.: the node #1 is the default node. It must be present.
    {
        m_defaultNodeIsPresent = true;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetHssConfiguration(
    SAPDB_Bool &hssNodesConfigured,
    RTEConf_Parameter::Integer &delayTime,
    SAPDBErr_MessageList &err)
{
    // first check: consistent HSS configuration?
    if(!m_hssNodeListConsistent)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INCONSISTENT_HSS_CONFIG);
        return false;
    }

    // second check: are there any HSS nodes configured at all?
    hssNodesConfigured = !m_hssNodeList.IsEmpty();
    if(hssNodesConfigured)
    {
        // third check: is a default node configured?
        if(!m_defaultNodeIsPresent)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_MISSING_DEFAULT_HSS_NODE);
            return false;
        }
        // fourth check: if there are HSS nodes configured: Is my own node name among them?
        if(0 == m_localHSSIndex)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_LOCAL_NODE_NOT_FOUND_IN_HSS_CONFIG,(SAPDB_Char *)RTE_ISystem::Instance().GetLocalNodeName());
            return false;
        }

        // fifth check: is a delay time configured for my node name?
        SAPDB_Char *delayTimeName = BuildIndexedParamName((SAPDB_Char *)KERNELPARAM_HS_DELAY_TIME_ , PAN_HS_DELAY_TIME_DIGITS , m_localHSSIndex);
        SAPDBErr_MessageList errGettingParameter;
        RTEConf_Parameter::Integer myDelayTime;
        if((RTEConf_ParameterAccess::Instance())->GetInteger((RTEConf_Parameter::Name)delayTimeName,myDelayTime,errGettingParameter))
        {
            delayTime = myDelayTime;
        }
        else
        {
            // As the delay time parameters are not mandatory, a missing one is not fatal. But any other errors are.
            if(RTEERR_CONF_NOT_FOUND_ID == errGettingParameter.ID())
            {
                delayTime = 0;
            }
            else
            {
                err = errGettingParameter;
                return false;
            }
        }
        DEALLOCATE_MEMORY(delayTimeName);
    }
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetInteger(RTEConf_Parameter::Name name,RTEConf_Parameter::Integer &integerValue,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }
    return m_Conf_Parameter->GetValue(name,integerValue,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetInteger(RTEConf_Parameter::Name name,RTEConf_Parameter::Integer &integerValue,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }
    return m_Conf_Parameter->GetValue(name,integerValue,updateOnline,permanentChange,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetReal(RTEConf_Parameter::Name name,RTEConf_Parameter::Real &realValue,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }
    return m_Conf_Parameter->GetValue(name,realValue,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetReal(RTEConf_Parameter::Name name,RTEConf_Parameter::Real &realValue,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }
    return m_Conf_Parameter->GetValue(name,realValue,updateOnline,permanentChange,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetString(RTEConf_Parameter::Name name,RTEConf_Parameter::String &stringValue,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }
    return m_Conf_Parameter->GetReference(name,stringValue,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetString(RTEConf_Parameter::Name name,RTEConf_Parameter::String &stringValue,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }
    return m_Conf_Parameter->GetReference(name,stringValue,updateOnline,permanentChange,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetCryptInfo(RTEConf_Parameter::Name name,RTEConf_Parameter::CryptInfo &cryptInfo,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }
    return m_Conf_Parameter->GetValue(name,cryptInfo,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetCryptInfo(RTEConf_Parameter::Name name,RTEConf_Parameter::CryptInfo &cryptInfo,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }
    return m_Conf_Parameter->GetValue(name,cryptInfo,updateOnline,permanentChange,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::GetRundirectoryAbsolutePath(RTEConf_Parameter::String &stringValue,SAPDBErr_MessageList &err)
{
    ScopeLockedNonExclusive thisScope(m_pRWRegion);
    if(!m_errorInFileRead.IsEmpty())
    {
        err = m_errorInFileRead;
        return false;
    }

    if( 0 == m_runDirectory[0] )
    {
        RTEConf_Parameter::String runDirectoryFromFile;
        if (!m_Conf_Parameter->GetReference(KERNELPARAM_RUNDIRECTORY, runDirectoryFromFile, err))
        {
            return false;
        }
        if( isRelativePath((SAPDB_Char *)runDirectoryFromFile) )
        {
            tsp00_Pathc     indepDataPath;
            tsp01_RteError  rteError;
            if( !sqlGetIndependentDataPath   (   indepDataPath,
                                                 TERM_WITH_DELIMITER_EO01,
                                                 &rteError ))
            {
                return false;
            }
            SAPDB_sprintf(m_runDirectory,sizeof(RTE_Path),"%s%s",indepDataPath.asCharp(),runDirectoryFromFile);
        }
        else
        {
            SAPDB_strncpy_zero_terminated((SAPDB_Char *)m_runDirectory,(SAPDB_Char *)runDirectoryFromFile,sizeof(RTE_Path));
        }
    }
    stringValue = (RTEConf_Parameter::String)&m_runDirectory;
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::SetInteger(RTEConf_Parameter::Name name, RTEConf_Parameter::Integer  value,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Type type;
    if(!m_Conf_Parameter->GetType(name,type,err))
    {
        return false;
    }
    if(RTEConf_Parameter::t_Integer != type)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_TYPE_MISMATCH, SAPDB_ToString(RTEConf_Parameter::t_Integer), SAPDB_ToString(type));
        return false;
    }
    if(!m_Conf_Parameter->UpdateOnlinePossible(name))
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CANT_CHANGE_PARAMETER_ONLINE, (SAPDB_Char *)name);
        return false;
    }
    else
    {
        ScopeLockedExclusive thisScope(m_pRWRegion);
        void *context;
        RTEConf_Parameter::UpdateFuncType updateFunc = m_Conf_Parameter->GetUpdateFunc(name,context);
        if(!updateFunc || updateFunc(name,RTEConf_Parameter::t_Integer,(void *)&value,context,err))
        {
            return m_Conf_Parameter->SetValue(name,value,true,permanentChange,err);
        }
        else
        {
            return false;
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::SetReal(RTEConf_Parameter::Name name, RTEConf_Parameter::Real  value,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Type type;
    if(!m_Conf_Parameter->GetType(name,type,err))
    {
        return false;
    }
    if(RTEConf_Parameter::t_Integer != type)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_TYPE_MISMATCH, SAPDB_ToString(RTEConf_Parameter::t_Real), SAPDB_ToString(type));
        return false;
    }
    if(!m_Conf_Parameter->UpdateOnlinePossible(name))
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CANT_CHANGE_PARAMETER_ONLINE, (SAPDB_Char *)name);
        return false;
    }
    else
    {
        ScopeLockedExclusive thisScope(m_pRWRegion);
        void *context;
        RTEConf_Parameter::UpdateFuncType updateFunc = m_Conf_Parameter->GetUpdateFunc(name,context);
        if(!updateFunc || updateFunc(name,RTEConf_Parameter::t_Real,(void *)&value,context,err))
        {
            return m_Conf_Parameter->SetValue(name,value,true,permanentChange,err);
        }
        else
        {
            return false;
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::SetString(RTEConf_Parameter::Name name, RTEConf_Parameter::String  value,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Type type;
    if(!m_Conf_Parameter->GetType(name,type,err))
    {
        return false;
    }
    if(RTEConf_Parameter::t_String != type)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_TYPE_MISMATCH), SAPDB_ToString(RTEConf_Parameter::t_String), SAPDB_ToString(type);
        return false;
    }
    if(!m_Conf_Parameter->UpdateOnlinePossible(name))
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CANT_CHANGE_PARAMETER_ONLINE, (SAPDB_Char *)name);
        return false;
    }
    else
    {
        ScopeLockedExclusive thisScope(m_pRWRegion);
        void *context;
        RTEConf_Parameter::UpdateFuncType updateFunc = m_Conf_Parameter->GetUpdateFunc(name,context);
        if(!updateFunc || updateFunc(name,RTEConf_Parameter::t_String,(void *)value,context,err))
        {
            return m_Conf_Parameter->SetValue(name,value,true,permanentChange,err);
        }
        else
        {
            return false;
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::SetCryptInfo(RTEConf_Parameter::Name name,RTEConf_Parameter::CryptInfo &value,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Type type;
    if(!m_Conf_Parameter->GetType(name,type,err))
    {
        return false;
    }
    if(RTEConf_Parameter::t_CryptInfo != type)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_TYPE_MISMATCH, SAPDB_ToString(RTEConf_Parameter::t_CryptInfo), SAPDB_ToString(type));
        return false;
    }
    if(!m_Conf_Parameter->UpdateOnlinePossible(name))
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CANT_CHANGE_PARAMETER_ONLINE, (SAPDB_Char *)name);
        return false;
    }
    else
    {
        ScopeLockedExclusive thisScope(m_pRWRegion);
        void *context;


        RTEConf_Parameter::UpdateFuncType updateFunc = m_Conf_Parameter->GetUpdateFunc(name,context);
        if(!updateFunc || updateFunc(name,RTEConf_Parameter::t_CryptInfo,(void *)value,context,err))
        {
            return m_Conf_Parameter->SetValue(name,value,true,permanentChange,err);
        }
        else
        {
            return false;
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::AddInteger(RTEConf_Parameter::Name name, RTEConf_Parameter::Integer  value,SAPDB_Bool changeOnline,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Type type;
    // check if the parameter already exists...
    SAPDBErr_MessageList errListForGetType;
    if(m_Conf_Parameter->GetType(name,type,errListForGetType))
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMETER_ALREADY_EXISTS, SAPDB_ToString((SAPDB_Char *)name));
        return false;
    }
    else
    {
        ScopeLockedExclusive thisScope(m_pRWRegion);
        return m_Conf_Parameter->SetValue(name,value,changeOnline,permanentChange,err);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::AddReal(RTEConf_Parameter::Name name, RTEConf_Parameter::Real  value,SAPDB_Bool changeOnline,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Type type;
    // check if the parameter already exists...
    SAPDBErr_MessageList errListForGetType;
    if(m_Conf_Parameter->GetType(name,type,errListForGetType))
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMETER_ALREADY_EXISTS, SAPDB_ToString((SAPDB_Char *)name));
        return false;
    }
    else
    {
        ScopeLockedExclusive thisScope(m_pRWRegion);
        return m_Conf_Parameter->SetValue(name,value,changeOnline,permanentChange,err);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::AddString(RTEConf_Parameter::Name name, RTEConf_Parameter::String  value,SAPDB_Bool changeOnline,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Type type;
    // check if the parameter already exists...
    SAPDBErr_MessageList errListForGetType;
    if(m_Conf_Parameter->GetType(name,type,errListForGetType))
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMETER_ALREADY_EXISTS, SAPDB_ToString((SAPDB_Char *)name));
        return false;
    }
    else
    {
        ScopeLockedExclusive thisScope(m_pRWRegion);
        return m_Conf_Parameter->SetValue(name,value,changeOnline,permanentChange,err);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::Remove(RTEConf_Parameter::Name name, SAPDBErr_MessageList &err)
{
    return m_Conf_Parameter->Delete(name,err);
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterAccess::RTEConf_ParameterAccess(SAPDB_Char *dbName)
    :
    m_hssNodeListConsistent(true),
    m_localHSSIndex(0),
    m_hssNodeList(RTEMem_RteAllocator::Instance()),
    m_defaultNodeIsPresent(false),
    m_pool((SAPDB_UTF8 *)"Parameter Access Lock",1,RTEMem_RteAllocator::Instance()),
    m_pRWRegion(RTESync_CreateRWRegion (1, m_pool, RTEMem_RteAllocator::Instance()))
{
    SAPDB_Bool dataRead;
    SAPDB_Bool fileReadSuccess;

    m_Conf_Parameter = new (AllocateMemory(sizeof(RTEConf_Parameter))) RTEConf_Parameter(dbName,0,RTECONF_MAXNAMELENGTH,RTECONF_MAXSTRINGLENGTH);
    fileReadSuccess = m_Conf_Parameter->Read(dataRead,m_errorInFileRead);

    // if the file does not exist, Read() returns true and sets dataRead to false. For the kernel, this is an error case, so
    // an error message has to be generated and m_FileReadOk must be false.
/*
    if( !fileReadSuccess )
    {
        MessageAndCrash(err);
    }
    if( !dataRead )
    {
        MessageAndCrash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMFILE_NOT_FOUND,dbName,SAPDB_ToString(LAST_SYSTEM_ERROR_)));
    }
*/
    RegisterUpdateFunctions();
}

/*---------------------------------------------------------------------------*/

void *RTEConf_ParameterAccess::operator new(size_t size)
{
    return AllocateMemory(size);
}

/*---------------------------------------------------------------------------*/

externC void vconf_param_get_int4      ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
									     tsp00_Int4                 VAR_VALUE_REF conf_param_value,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
									     tsp00_ErrText              VAR_ARRAY_REF errtext,
									     tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret )
{
    SAPDB_Int4 lengthOfName = conf_param_name.length();
    SAPDB_UTF8 *nameAsCString = (SAPDB_UTF8 *)AllocateMemory(lengthOfName+1);
    RTEConf_Parameter::Integer value;
    SAPDB_MemCopyNoCheck(nameAsCString,conf_param_name.asCharp(),lengthOfName);
    nameAsCString[lengthOfName]=0;
    SAPDBErr_MessageList err;
    SAPDB_Bool updateOnline;
    SAPDB_Bool permanentChange;
    if(RTEConf_ParameterAccess::Instance()->GetInteger(nameAsCString,value,updateOnline,permanentChange,err))
	{
        update_online = updateOnline;
        permanent_change = permanentChange;
		conf_param_ret.becomes(ok_sp11);
        conf_param_value = (tsp00_Int4)value;
	}
	else
	{
		conf_param_ret.becomes(not_ok_sp11);
		errtext.c2p(err.Message());
	}
    DEALLOCATE_MEMORY(nameAsCString);
}

/*---------------------------------------------------------------------------*/

externC void vconf_param_get_integer   ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
									     tsp00_Int8                 VAR_VALUE_REF conf_param_value,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
									     tsp00_ErrText              VAR_ARRAY_REF errtext,
									     tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret )
{
    SAPDB_Int4 lengthOfName = conf_param_name.length();
    SAPDB_UTF8 *nameAsCString = (SAPDB_UTF8 *)AllocateMemory(lengthOfName+1);
    SAPDB_MemCopyNoCheck(nameAsCString,conf_param_name.asCharp(),lengthOfName);
    nameAsCString[lengthOfName]=0;
    SAPDBErr_MessageList err;
    SAPDB_Bool updateOnline;
    SAPDB_Bool permanentChange;
    if(RTEConf_ParameterAccess::Instance()->GetInteger(nameAsCString,conf_param_value,updateOnline,permanentChange,err))
	{
        update_online = updateOnline;
        permanent_change = permanentChange;
        conf_param_ret.becomes(ok_sp11);
	}
	else
	{
		conf_param_ret.becomes(not_ok_sp11);
		errtext.c2p(err.Message());
	}
    DEALLOCATE_MEMORY(nameAsCString);
}

/*---------------------------------------------------------------------------*/

externC void vconf_param_get_real      ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
									     tsp00_Longreal             VAR_VALUE_REF conf_param_value,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
									     tsp00_ErrText              VAR_ARRAY_REF errtext,
									     tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret )
{
    SAPDB_Int4 lengthOfName = conf_param_name.length();
    SAPDB_UTF8 *nameAsCString = (SAPDB_UTF8 *)AllocateMemory(lengthOfName+1);
    SAPDB_MemCopyNoCheck(nameAsCString,conf_param_name.asCharp(),lengthOfName);
    nameAsCString[lengthOfName]=0;
    SAPDBErr_MessageList err;
    SAPDB_Bool updateOnline;
    SAPDB_Bool permanentChange;

    if(RTEConf_ParameterAccess::Instance()->GetReal(nameAsCString,conf_param_value,updateOnline,permanentChange,err))
	{
        update_online = updateOnline;
        permanent_change = permanentChange;
		conf_param_ret.becomes(ok_sp11);
	}
	else
	{
		conf_param_ret.becomes(not_ok_sp11);
		errtext.c2p(err.Message());
	}
    DEALLOCATE_MEMORY(nameAsCString);
}

/*---------------------------------------------------------------------------*/

externC void vconf_param_get_string    ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
									     tsp11_ConfParamValue       VAR_ARRAY_REF conf_param_value,
									     tsp00_Int4                 VAR_VALUE_REF conf_param_length,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
									     tsp00_ErrText              VAR_ARRAY_REF errtext,
									     tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret )
{
	RTEConf_Parameter::String StringValue;
    SAPDB_Int4 lengthOfName = conf_param_name.length();
    SAPDB_UTF8 *nameAsCString = (SAPDB_UTF8 *)RTEMem_RteAllocator::Instance().Allocate(lengthOfName+1);
    SAPDB_MemCopyNoCheck(nameAsCString,conf_param_name.asCharp(),lengthOfName);
    nameAsCString[lengthOfName]=0;
    SAPDBErr_MessageList err;
    SAPDB_Bool updateOnline;
    SAPDB_Bool permanentChange;

    if(RTEConf_ParameterAccess::Instance()->GetString(nameAsCString,StringValue,updateOnline,permanentChange,err))
	{
        update_online = updateOnline;
        permanent_change = permanentChange;
		conf_param_ret.becomes(ok_sp11);
        conf_param_length = (SAPDB_Int4)strlen((SAPDB_Char *)StringValue);
		conf_param_value.c2p((SAPDB_Char *)StringValue);
	}
	else
	{
		conf_param_ret.becomes(not_ok_sp11);
		errtext.c2p(err.Message());
	}
    DEALLOCATE_MEMORY(nameAsCString);
}

/*---------------------------------------------------------------------------*/

externC void vconf_param_get_cryptinfo ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
									     tsp00_CryptName            VAR_ARRAY_REF conf_param_value,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
									     tsp00_ErrText              VAR_ARRAY_REF errtext,
									     tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret )
{
    SAPDB_Int4 lengthOfName = conf_param_name.length();
    SAPDB_UTF8 *nameAsCString = (SAPDB_UTF8 *)RTEMem_RteAllocator::Instance().Allocate(lengthOfName+1);
    SAPDB_MemCopyNoCheck(nameAsCString,conf_param_name.asCharp(),lengthOfName);
    nameAsCString[lengthOfName]=0;
    SAPDBErr_MessageList err;
    SAPDB_Bool updateOnline;
    SAPDB_Bool permanentChange;

    if(RTEConf_ParameterAccess::Instance()->GetCryptInfo(nameAsCString,*((RTEConf_Parameter::CryptInfo *)(conf_param_value.asCharp())),updateOnline,permanentChange,err))
	{
        update_online = updateOnline;
        permanent_change = permanentChange;
		conf_param_ret.becomes(ok_sp11);
	}
	else
	{
		conf_param_ret.becomes(not_ok_sp11);
		errtext.c2p(err.Message());
	}

    DEALLOCATE_MEMORY(nameAsCString);
}


/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_ParameterAccessGetInteger   ( SAPDB_UTF8  *  conf_param_name,
									                     SAPDB_Int8  *  conf_param_value,
									                     SAPDB_Char  ** errtext)
{
    SAPDBErr_MessageList err;

    RTEConf_ParameterAccess* instance = RTEConf_ParameterAccess::Instance();
    if(!instance)
    {
        *errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)"database name not yet available");
        return false;
    }

    if(instance->GetInteger(conf_param_name,*conf_param_value,err))
	{
		return true;
	}
	else
	{
		*errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)err.Message());
        return false;
    }
}

/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_ParameterAccessGetReal      ( SAPDB_UTF8  *  conf_param_name,
									                     SAPDB_Real8 *  conf_param_value,
									                     SAPDB_Char  ** errtext)
{
    SAPDBErr_MessageList err;

    RTEConf_ParameterAccess* instance = RTEConf_ParameterAccess::Instance();
    if(!instance)
    {
        *errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)"database name not yet available");
        return false;
    }

    if(instance->GetReal(conf_param_name,*conf_param_value,err))
	{
		return true;
	}
	else
	{
		*errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)err.Message());
        return false;
    }
}

/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_ParameterAccessGetString    ( SAPDB_UTF8  *  conf_param_name,
									                     SAPDB_Char  ** conf_param_value,
									                     SAPDB_Char  ** errtext)
{
	RTEConf_Parameter::String StringValue = (RTEConf_Parameter::String)conf_param_value;
    SAPDBErr_MessageList err;

    RTEConf_ParameterAccess* instance = RTEConf_ParameterAccess::Instance();
    if(!instance)
    {
        *errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)"database name not yet available");
        return false;
    }

    if(instance->GetString(conf_param_name,StringValue,err))
	{
        *conf_param_value = (SAPDB_Char *)StringValue;
		return true;
	}
	else
	{
		*errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)err.Message());
        return false;
    }
}

/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_ParameterAccessGetCryptInfo ( SAPDB_UTF8  *  conf_param_name,
									                     SAPDB_Byte  *  conf_param_value,
									                     SAPDB_Char  ** errtext)
{
    SAPDBErr_MessageList err;

    RTEConf_ParameterAccess* instance = RTEConf_ParameterAccess::Instance();
    if(!instance)
    {
        *errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)"database name not yet available");
        return false;
    }

    if(instance->GetCryptInfo(conf_param_name,*((RTEConf_Parameter::CryptInfo *)conf_param_value),err))
	{
		return true;
	}
	else
	{
		*errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)err.Message());
        return false;
    }
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool RTEConf_UpdateXparamParameter(RTEConf_Parameter::Name name,RTEConf_Parameter::Type type,void *newValue,void *context,SAPDBErr_MessageList &err)
{
    RTEConf_ParameterValueRecord xpr;
    if(RTEConf_Parameter::t_Integer == type)
    {
        xpr.xp_value_t = NXP_INT_TYPE;
        xpr.xp_type.xp_integer = *((SAPDB_Int8 *)newValue);
    }
    else
    {
        xpr.xp_value_t = NXP_STRING_TYPE;
        xpr.xp_type.xp_string = (SAPDB_UTF8 *)newValue;
    }
    return RTEConf_AnalyzeParam  ( KGS->XParam, name, &xpr, true, err);
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool RTEConf_UpdateTimeMeasurementParameter(RTEConf_Parameter::Name name,RTEConf_Parameter::Type type,void *newValue,void *context,SAPDBErr_MessageList &err)
{
    switch ( *(SAPDB_Char const *)newValue)
    {
        case 'y':
        case 'Y':
            RTEThread_ConsoleStandardWorker::ChangeTimeEnableValue(true);
            break;
        default:
            RTEThread_ConsoleStandardWorker::ChangeTimeEnableValue(false);
            break;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool RTEConf_UpdateulRegionCollisionLoop(RTEConf_Parameter::Name name,RTEConf_Parameter::Type type,void *newValue,void *context,SAPDBErr_MessageList &err)
{
    if(RTEConf_Parameter::t_Integer != type)
    {
        return false;
    }

    RTEConf_Parameter::Integer valueInt = *((RTEConf_Parameter::Integer *)newValue);

    return RTEConf_SetulRegionCollisionLoop(valueInt ,KGS->XParam,err);
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool RTEConf_SetulRegionCollisionLoop(RTEConf_Parameter::Integer newValue,RTE_XPARAM_REC *XParam,SAPDBErr_MessageList &err)
{
    if(-1 == newValue)
    {
        if(1 == XParam->ulMaxCPU)
        {
            XParam->ulRegionCollisionLoop = 0;
        }
        else
        {
            XParam->ulRegionCollisionLoop = 100;
        }
    }
    else
    {
        XParam->ulRegionCollisionLoop = (tsp00_Uint)newValue;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool RTEConf_SetulregLockSlice(RTEConf_Parameter::Integer newValue,RTE_XPARAM_REC *XParam,SAPDBErr_MessageList &err)
{
    if(-1 == newValue)
    {
        if(1 == XParam->ulMaxCPU)
        {
            XParam->ulRegLockSlice = 300;
        }
        else
        {
            XParam->ulRegLockSlice = 3000;
        }
    }
    else
    {
        XParam->ulRegLockSlice = (tsp00_Uint)newValue;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool RTEConf_UpdateulregLockSlice(RTEConf_Parameter::Name name,RTEConf_Parameter::Type type,void *newValue,void *context,SAPDBErr_MessageList &err)
{
    if(RTEConf_Parameter::t_Integer != type)
    {
        return false;
    }

    RTEConf_Parameter::Integer valueInt = *((RTEConf_Parameter::Integer *)newValue);

    return RTEConf_SetulregLockSlice(valueInt,KGS->XParam,err);
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool RTEConf_UpdateMessageListLimit(RTEConf_Parameter::Name name,RTEConf_Parameter::Type type,void *newValue,void *context,SAPDBErr_MessageList &err)
{
    if(RTEConf_Parameter::t_Integer != type)
    {
        return false;
    }

    RTEConf_Parameter::Integer valueInt = *((RTEConf_Parameter::Integer *)newValue);

    RTE_IInterface::Instance().SetMessageLimit((SAPDB_UInt4)valueInt);

    return true;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Int HexToDec(SAPDB_Char hex)
{
    if((hex >= '0')&&(hex <= '9'))
        return hex-'0';
    if((hex >= 'a')&&(hex <= 'f'))
        return hex-'a'+10;
    if((hex >= 'A')&&(hex <= 'F'))
        return hex-'A'+10;
    return 0;
}


externC void vconf_param_put           ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
                                         tsp11_ConfParamValue       VAR_ARRAY_REF conf_param_value,
                                         boolean                                  is_numeric,
                                         boolean                                  permanent_change,
                                         tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret )
{
    SAPDB_Int4 lengthOfName = conf_param_name.length();
    SAPDB_UTF8 *nameAsCString = (SAPDB_UTF8 *)AllocateMemory(lengthOfName+1);

    SAPDB_MemCopyNoCheck(nameAsCString,conf_param_name.asCharp(),lengthOfName);
    nameAsCString[lengthOfName]=0;

    SAPDB_Int4 lengthOfValue = conf_param_value.length();
    SAPDB_UTF8 *valueAsCString = (SAPDB_UTF8 *)AllocateMemory(lengthOfValue+1);

    SAPDB_MemCopyNoCheck(valueAsCString,conf_param_value.asCharp(),lengthOfValue);
    valueAsCString[lengthOfValue]=0;

    SAPDBErr_MessageList err;

    conf_param_ret.becomes(ok_sp11);

    SAPDB_Bool success;
    if(is_numeric)
    {
        RTEConf_Parameter::Integer valueAsInteger = (RTEConf_Parameter::Integer)atoi((SAPDB_Char *)valueAsCString);
        success = RTEConf_ParameterAccess::Instance()->SetInteger(nameAsCString,valueAsInteger,permanent_change?true:false,err);
    }
    else
    {
        success = (RTEConf_ParameterAccess::Instance())->SetString(nameAsCString,valueAsCString,permanent_change?true:false,err);
        if(!success && err.ID() == RTEERR_CONF_TYPE_MISMATCH_ID)
        {
            RTEConf_Parameter::CryptInfo cryptInfo;
            SAPDB_Int index;

            SAPDB_Byte rawElements[sizeof(RTEConf_Parameter::CryptInfo)];
            for(index = 0 ; index < sizeof(RTEConf_Parameter::CryptInfo) ; index++ )
            {
                rawElements[index] =    HexToDec(valueAsCString[index*2+1]);
                rawElements[index] +=   HexToDec(valueAsCString[index*2])*16;
            }

            SAPDB_MemCopyNoCheck(cryptInfo,rawElements,sizeof(RTEConf_Parameter::CryptInfo));
            success = (RTEConf_ParameterAccess::Instance())->SetCryptInfo(nameAsCString,cryptInfo,permanent_change?true:false,err);
        }
    }
    if(!success)
    {
        switch(err.ID())
        {
        case RTEERR_CANT_CHANGE_PARAMETER_ONLINE_ID:
            conf_param_ret.becomes(cannot_change_online_sp11);
            break;
        case RTEERR_NO_CHANGE_PERMISSION_ID:
            conf_param_ret.becomes(no_change_permission_sp11);
            break;
        case RTEERR_CONF_TYPE_MISMATCH_ID:
            conf_param_ret.becomes(type_mismatch_sp11);
            break;
        case RTEERR_CONF_NOT_FOUND_ID:
            conf_param_ret.becomes(key_not_found_sp11);
            break;
        default:
            conf_param_ret.becomes(not_ok_sp11);
            break;
        }
        SQLMan_Context::GetContext()->GetErrorList().AppendNewMessage(err);
    }
    DEALLOCATE_MEMORY(nameAsCString);
    DEALLOCATE_MEMORY(valueAsCString);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_ParameterAccess::CheckUpdateFunctions(SAPDBErr_MessageList & messages)
{
    RTEConf_ParameterReader paramReader;
    paramReader.StartIteration();

    RTEConf_Parameter::Name name;
    SAPDB_Char *valueString;
    SAPDB_Bool updateOnline;
    SAPDB_Bool permanentChange;
    SAPDB_Bool returnValue = true;
    SAPDBErr_MessageList err;
    while(paramReader.Next(	name,valueString,updateOnline,permanentChange,err))
    {
        void *context;
        RTEConf_Parameter::UpdateFuncType updateFunc = (RTEConf_ParameterAccess::Instance())->m_Conf_Parameter->GetUpdateFunc(name,context);
        if(updateOnline)
        {
            if(!updateFunc)
            {
                messages = messages + (SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_CHANGEABLE_BUT_NO_UPDATE_FUNC,SAPDB_ToString((SAPDB_Char *)name)));
                returnValue = false;
            }
        }
        if(!updateOnline)
        {
            if(updateFunc)
            {
                messages = messages + (SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_UPDATE_FUNC_BUT_NOT_CHANGEABLE,SAPDB_ToString((SAPDB_Char *)name)));
                returnValue = false;
            }
        }
    }
    return returnValue;
}

/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_ParameterAccessSetCrashSem( SAPDB_Int8 conf_param_value,
                                                       SAPDB_Char ** errtext)
{
    SAPDBErr_MessageList err;
    if((RTEConf_ParameterAccess::Instance())->MarkCrashed(conf_param_value ,err))
    {
        return true;
    }
    *errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)err.Message());
    return false;
}

/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_ParameterAccessResetParamChangedFlag ( SAPDB_Char ** errtext )
{
    SAPDBErr_MessageList err;
    if((RTEConf_ParameterAccess::Instance())->MarkAsVerified(err))
    {
        return true;
    }
    else
    {
        *errtext = RTEConf_MakeACopyOfString((SAPDB_Char *)err.Message());
        return false;
    }
}

/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_ParameterAccessIsVerified ( )
{
    if(!RTEConf_ParameterAccess::Instance()->FileInOldFormat())
    {
      return RTEConf_ParameterAccess::Instance()->IsVerified();
    }
    else
    {
      return true;
    } // end if
}

/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_ParameterAccessGetCrashSem ( )
{
    if(!RTEConf_ParameterAccess::Instance()->FileInOldFormat())
    {
      return RTEConf_ParameterAccess::Instance()->IsCrashed();
    }
    else
    {
      return true;
    } // end if
}

/*---------------------------------------------------------------------------*/

externC void RTEConf_ParameterAccessFreeErrorMessage(SAPDB_Char *errtext)
{
    DEALLOCATE_MEMORY(errtext);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

