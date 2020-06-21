/*!---------------------------------------------------------------------
  @file           RTESys_SystemInfo.h
  @author         JoergM
  @brief          System calls: Retrieving various machine information
  @see            

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
---------------------------------------------------------------------*/



#ifndef RTESYS_SYSTEMINFO_H
#define RTESYS_SYSTEMINFO_H

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_CompilerFeatures.h" /* for externC and VAR_ARRAY_REF ... */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*
  Function: RTESys_NumberOfCPU
  Description: Return the number of physical CPUs on the machine

  This call only returns the number of (logical) CPUs available on the machine. The returned number 
  might be the logical CPU count e.g. on Intel Hyperthreading Plattforms. If an external tool assign
  specific CPUs to specific processes or assign a group of CPUs to the program, it will not be detected.
  The call is mainly for deciding if Spinlocks should do some hot loops before giving up time slice.

  Return value: Maximum number of CPUs avaliable in the machine
 */
externC SAPDB_UInt4 RTESys_NumberOfCPU();


/*
  Function: RTESys_NumberOfPhysicalCPUs
  Description: Return the number of physical CPUs on the machine

  This call returns the number of physical CPUs available on the machine. This might differ to
  the number of logical CPUs returned by RTESys_NumberOfCPU() e.g. on Intel Hyperthreading Plattforms.

  Return value: Number of phsyical CPUs avaliable in the machine
 */
externC SAPDB_UInt4 RTESys_NumberOfPhysicalCPUs();

/*
  Function: RTESys_SystemPageSize
  Description: Return the number of bytes in a memory page on the system

  Return value: Number of bytes in a single memory paghe on the system
 */
externC SAPDB_UInt4 RTESys_SystemPageSize();

/*
  Function: RTESys_SystemAllocationGranularity
  Description: Return the granularity in bytes with which virtual memory is allocated.

  Return value: Granularity in bytes with which virtual memory is allocated
 */
externC SAPDB_UInt4 RTESys_SystemAllocationGranularity();

/*
  Function: RTESys_SwapSize
  Description: Return the size of the swap space in the system in MB and
               the number of free MB in the swap space

  Return value: nothing, the number of total and available MB in the systems 
                swap space are stored in the variables the parameters point to
 */
externC void RTESys_SwapSize(SAPDB_ULong *totalMB,SAPDB_ULong *freeMB);

/*
  Function: RTESys_PhysicalMemorySize
  Description: Return the size of the physical memory of the system in MB

  Return value: the size of the physical memory of the system in MB
 */
externC SAPDB_ULong RTESys_PhysicalMemorySize();

/*
  Function: RTESys_PhysicalMemoryAvailable
  Description: Return the available amount of the physical memory of the system in MB

  Return value: the size of available memory of the system in MB
 */
externC SAPDB_ULong RTESys_PhysicalMemoryAvailable();

/*
  Function: RTESys_MemInfo
  Description: 

  Return value: 
 */
externC void RTESys_MemInfo(SAPDB_ULong *totalPhysicalMB,SAPDB_ULong *freePhysicalMB,SAPDB_ULong *totalSwapMB,SAPDB_ULong *freeSwapMB);

/*
  Function: RTESys_Hostname
  Description: Return the local node name
  Return value: the local node name
 */
externC char const *RTESys_Hostname();


/*
  Function: RTESys_GetOSVersion
  Description: Return the operating system version
  Return value: true if succeeded, false if not
 */
externC SAPDB_Bool RTESys_GetOSVersion(SAPDB_Char *string,SAPDB_UInt4 length);

/*
  Function: RTESys_GetCPUType
  Description: Return the type of cpu
  Return value: true if succeeded, false if not
 */
externC SAPDB_Bool RTESys_GetCPUType(SAPDB_Char *string,SAPDB_UInt4 length);

/*!
  @brief    RTESys_Is64BitPlatform
            Detection if we run on a 64 bit platform
  @return   true/false
 */
externC SAPDB_Bool RTESys_Is64BitPlatform ();

/*
  Function: RTESys_GetCPULoad
  Description: Return the cpu load
  Return value: the cpu load or -1.0 if not available
 */
externC SAPDB_Real8 RTESys_GetCPULoad(void);

/*
  Function: RTESys_GetProcessTimes
  Description: Give the number of cpu seconds the program has spent in user and system mode
  Return value: 
 */
externC void RTESys_GetProcessTimes(SAPDB_UInt8 *secondsUser,SAPDB_UInt8 *secondsSystem);

/*
  Function: RTESys_GetMachineTimes
  Description: Give the number of cpu seconds the machine has spent in user, system and idle mode
  Return value: 
 */
externC void RTESys_GetMachineTimes(SAPDB_UInt8 *secondsUser,SAPDB_UInt8 *secondsSystem,SAPDB_UInt8 *secondsIdle);


#if defined(WIN32)
/*
  Function: RTESys_GetPhysicalProcessorInfomation
  Description: The Function retrieves information about the relationship between logical 
               processors in the system. If the function succeeds, the return value is 
               nonzero and the processor relatedness array has been filled. The size of the 
               array must be at least MAX_PROCESSORS. Each array element is representing a
               logical processor which corresponds to the same position of the process 
               affinity mask. Logical processors belonging to the same processor core
               have the array element content. 
               The functions return value is equal to the number of processor cores 
               (pysical processors) configured into the system. If the parameter 
               'affinitMaskDependent' is true, the return value represents the number of 
               processor cores the process is allowed to run on. Additionally the array
               values of the corresponding logical processors where the process is not
               allowed to run on is set to zero.

  Return value: Number of physical processors of the system or the number of physical processor 
                the process is allowed to run on.
 */
# define MAX_PROCESSORS  (sizeof(DWORD_PTR)*8)

externC SAPDB_UInt4 RTESys_GetPhysicalProcessorInfomation(  SAPDB_UInt4*       processorRelatedness,
                                                            SAPDB_UInt4  const arraySize,
                                                            SAPDB_Bool   const affinitMaskDependent );


#endif  /* WIN32 */
#endif  /* RTESYS_SYSTEMINFO_H */
