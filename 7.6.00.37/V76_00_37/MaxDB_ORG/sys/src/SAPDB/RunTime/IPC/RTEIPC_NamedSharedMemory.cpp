/*!========================================================================

  @file         RTEIPC_NamedSharedMemory.cpp
  @ingroup      Interprocess Communication
  @author       StefanP

  @brief        Primitives for Named Shared Memory Handling

  @since        2004-07-13  14:26
  @sa           

  ==========================================================================

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined (_WIN32)                           /*&if $OSSPEC=WIN32*/
#include    "RunTime/RTE_saveUNIXcalls.h"       /*nocheck*/
#include    "RunTime/RTE_UNIXAuthenticate.hpp"  /*nocheck*/
#endif                                          /*&endif*/

#include    "RunTime/IPC/RTEIPC_NamedSharedMemory.hpp"
#include    "SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "RunTime/System/RTESys_SystemInfo.h"
#include    "RunTime/RTE_GlobalNameSpaceNT.h"
#include    "RunTime/System/RTESys_SecurityAttributes.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_IPC_Messages.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic IPC_Trace; ///<Trace for Interprocess Communication

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
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
  *  METHODS                                                                  *
 *===========================================================================*/

RTEIPC_NamedSharedMemory::RTEIPC_NamedSharedMemory ()   
    : m_Handle (RTE_UNDEF_HANDLE),
      m_pMem (NULL),
      m_Size (0),
      m_pName (NULL),
      m_OpenMode (RTE_OpenUndef)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_NamedSharedMemory::RTEIPC_NamedSharedMemory", IPC_Trace, 9);
}

/*---------------------------------------------------------------------------*/

RTEIPC_NamedSharedMemory::~RTEIPC_NamedSharedMemory ()
{
    SAPDBErr_MessageList       messageList;

    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_NamedSharedMemory::~RTEIPC_NamedSharedMemory", IPC_Trace, 9);

    if (NoError != Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_CLOSE);
        RTE_Message (messageList);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEIPC_NamedSharedMemory::AllocGranularity()
{
    return RTESys_SystemAllocationGranularity ();
}

/*---------------------------------------------------------------------------*/

RTEIPC_NamedSharedMemory::SHMRet    RTEIPC_NamedSharedMemory::Open            
(
    SAPDB_Char const * const            pSharedMemoryName,
    void *                              pBaseAddress,
    RTE_OpenMode                        openMode,
    RTE_SECURITY_ATTRIBUTES  &          secAttribute,   
    SAPDB_ULong                         shmSize,
    void **                             ppSharedMemory,
    SAPDBErr_MessageList &              messageList
)
{
    RTE_SystemRc                                rc = RTE_SYSTEMRC_NO_ERROR;    
    SHMRet                                      retCode = NoError;
    SAPDB_UInt4                                 nameBufferLength = 0; 

    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_NamedSharedMemory::Open", IPC_Trace, 1);
 
    if (RTE_UNDEF_HANDLE != m_Handle || NULL != m_pMem)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_OBJECT_ALREADY_OPEN); 
        return Error;
    }

    if (NULL == pSharedMemoryName)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_UNNAMED); //Can not open unnamed shared memory
        return Error;
    }

    //Check given address for correct alignment.
    //We do not want to depend on an error returned by a call to the system depending MAP call.
    if (NULL != pBaseAddress)
    {
        SAPDB_UInt4 allocGranularity = AllocGranularity();

        if (static_cast <SAPDB_Byte *>(pBaseAddress) - static_cast <SAPDB_Byte *> (0) != 
            SAPDB_ALIGN (static_cast <SAPDB_Byte *>(pBaseAddress) - 
                         static_cast <SAPDB_Byte *> (0), allocGranularity))
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_ADDRESS_ALIGNMENT,
                                                              pSharedMemoryName,
                                                              SAPDB_ToString (pBaseAddress),
                                                              SAPDB_ToString (allocGranularity));
            return BadAddress;
        }
    }


    if (NULL != ppSharedMemory)
      *ppSharedMemory = NULL;


/*The SHM-Name is build in the following manner:
    in:
        Name: e.g. SDBTech-COMMSEG-<UniqueID>

    m_pName: 
        NT:             SAPDBTech-COMMSEG-<UniqueID>
            or          Global\\SAPDBTech-COMMSEG-<UniqueID>

        UNIX:     
            SUN         /SAPDBTech-COMMSEG-<UniqueID>
            SOLARIS     /SAPDBTech-COMMSEG-<UniqueID>
            AIX/LINUX   /rel/ramdisk/SAPDB/SAPDBTech-COMMSEG-<UniqueID>
            else        SAPDBTech-COMMSEG-<UniqueID>
    
*/

#if defined (_WIN32)
    RTE_BuildGlobalNameSpaceName (pSharedMemoryName, NULL, &nameBufferLength); //Determine the length needed for the SHM name
    m_pName = (SAPDB_Char *)(RTEMem_RteAllocator::Instance().Allocate (nameBufferLength));
    if (NULL == m_pName)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_ALLOCATE_NAME,
                                                          SAPDB_ToString (nameBufferLength));
        return Error;
    }

    RTE_BuildGlobalNameSpaceName (pSharedMemoryName, m_pName, &nameBufferLength);

#else //UNIX

    nameBufferLength = strlen (pSharedMemoryName) + 1;
#if defined(AIX)// || defined(LINUX)
    nameBufferLength += strlen (RTE_WELL_KNOWN_RAMDISK_PATH);
#elif defined(SUN) || defined(SOLARIS) || defined (LINUX)
    if ( *pSharedMemoryName != '/' )
    {
        ++nameBufferLength; 
    }
#endif 

    m_pName = (SAPDB_Char *)(RTEMem_RteAllocator::Instance().Allocate (nameBufferLength));

    if (NULL == m_pName)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_ALLOCATE_NAME,
                                                          SAPDB_ToString (nameBufferLength));
        return Error;
    }

#if defined(AIX)// || defined(LINUX)
    strcpy(m_pName, RTE_WELL_KNOWN_RAMDISK_PATH);
#elif defined(SUN) || defined(SOLARIS) || defined (LINUX)
    if (*pSharedMemoryName != '/')
    {
        strcpy (m_pName, "/");
    }
    else
    {
        m_pName[0] = 0;
    }
#else
    {
        m_pName[0] = 0;
    }
#endif
    strcat (m_pName, pSharedMemoryName);

#endif

    m_Size = shmSize;
    m_OpenMode = openMode;

#if defined (_WIN32)
    if (RTE_OpenNew == m_OpenMode)
    {
#if defined (BIT64)
        DWORD   highOrderBytes = static_cast <DWORD> (m_Size >> 32);
#else
        DWORD   highOrderBytes = 0; // on i386, unfortunately (ulong >> 32) is made to (ulong >> 0), so (ulong >> 32) is  ulong  and NOT  0!
#endif
        DWORD   lowOrderBytes = static_cast <DWORD> (m_Size);

        m_Handle = CreateFileMapping (RTE_UNDEF_HANDLE, 
                                      &secAttribute, 
                                      PAGE_READWRITE | SEC_COMMIT, 
                                      highOrderBytes, 
                                      lowOrderBytes, 
                                      m_pName);

        rc = GetLastError ();
    }
    else
    {
        m_Handle = OpenFileMapping ( ((RTE_OpenExistingReadOnly == m_OpenMode) ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS)
                                    , FALSE
                                    , m_pName);
        if (NULL == m_Handle)
        {
            rc = GetLastError ();
        }
    }
    
    if (NULL == m_Handle)
    {
        if (RTE_OpenNew != m_OpenMode && RTE_SYSTEMRC_FILE_NOT_FOUND == rc)
        {
            retCode = ResourceMissing;
        }
        else
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_MAPPING,
                                                              RTE_OpenNew == m_OpenMode
                                                              ? "Create"
                                                              : RTE_OpenExistingReadOnly == m_OpenMode
                                                                ? "OpenReadOnly"
                                                                : "Open", 
                                                              m_pName, 
                                                              SAPDB_ToString (rc));
            retCode = Error;
        }

        m_Handle = RTE_UNDEF_HANDLE;
    }
    else
    {
        if (RTE_SYSTEMRC_ALREADY_EXISTS == rc)
        {//May happen if we try to Create a File Mapping for an existing object
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_MAPPING_EXIST,
                                                              m_pName);
            retCode = AlreadyExist;
        }
        else
        {
            if (NULL ==(m_pMem = MapViewOfFileEx (m_Handle, ((RTE_OpenExistingReadOnly == m_OpenMode) ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS), 0, 0, 0, pBaseAddress)))
            {
                rc = GetLastError();
                if (NULL != pBaseAddress && (ERROR_INVALID_ADDRESS == rc || 
                    ERROR_INVALID_PARAMETER == rc ||
                    ERROR_MAPPED_ALIGNMENT == rc ||
                    ERROR_NOT_ENOUGH_MEMORY == rc))
                {
                    messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_MAP_FIXED_ADDRESS, 
                        m_pName,
                        SAPDB_ToString (pBaseAddress),
                        SAPDB_ToString (rc));
                    retCode = BadAddress;
                }
                else
                {
                    messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_MAP, 
                        m_pName, 
                        SAPDB_ToString (rc));
                    retCode = Error;
                }
            }
            else if (NULL != ppSharedMemory)
            {
                *ppSharedMemory = m_pMem;
            }
        }
    }

#else //UNIX
    if (RTE_OpenNew == m_OpenMode)
    {
        mode_t  umask_old;
        umask_old = umask (0);
        m_Handle = OpenSHMObject (O_RDWR|O_CREAT, secAttribute);
        umask (umask_old);
    }
    else
    {
        m_Handle = OpenSHMObject (((RTE_OpenExistingReadOnly == m_OpenMode) ? O_RDONLY : O_RDWR));
    }


    if (RTE_UNDEF_HANDLE == m_Handle)
    {
        if (RTE_OpenNew != m_OpenMode && RTE_SYSTEMRC_FILE_NOT_FOUND == errno)
        {
            retCode = ResourceMissing;
        }
        else
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_OBJECT,
                                                              RTE_OpenNew == m_OpenMode
                                                              ? "Create"
                                                              : RTE_OpenExistingReadOnly == m_OpenMode
                                                                ? "OpenReadOnly"
                                                                : "Open", 
                                                              m_pName, 
                                                              RTE_save_strerror(errno));
            retCode = Error;
        }
    }
    else
    {
        if (RTE_OpenNew == m_OpenMode)
        {
            if ( RTE_SYSTEMRC_NO_ERROR != RTE_save_ftruncate (m_Handle, m_Size))
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_FTRUNCATE,
                                                                  m_pName,
                                                                  SAPDB_ToString (m_Size),
                                                                  RTE_save_strerror(errno));
                retCode = Error;            
            }
        }

        if (NoError == retCode)
        {
            if (NULL != pBaseAddress)
            {
                m_pMem = RTE_save_mmap (pBaseAddress, static_cast <SAPDB_ULong> (m_Size), 
                                (RTE_OpenExistingReadOnly == m_OpenMode) ? PROT_READ : PROT_READ|PROT_WRITE,
                                MAP_SHARED | MAP_FIXED, m_Handle, 
                                (off_t)0);
            }
            else
            {
                m_pMem = RTE_save_mmap (0, static_cast <SAPDB_ULong> (m_Size), 
                                (RTE_OpenExistingReadOnly == m_OpenMode) ? PROT_READ : PROT_READ|PROT_WRITE,
                                MAP_SHARED, m_Handle, (off_t)0);
            }

            if (MAP_FAILED == m_pMem)
            {
                if (NULL != pBaseAddress && (EINVAL == errno || 
                                             ENOMEM == errno ||
                                             ENXIO == errno ))

                {
                    messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_MAP_FIXED_ADDRESS, 
                                                        m_pName,
                                                        SAPDB_ToString (pBaseAddress),
                                                        RTE_save_strerror (errno));
                    retCode = BadAddress;
                }
                else
                {
                    messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_MAP, 
                                                                      m_pName, 
                                                                      RTE_save_strerror (errno));
                    retCode = Error;
                }

            }
            else if (NULL != ppSharedMemory)
            {
                *ppSharedMemory = m_pMem;
            }
        }
    }


#endif

    return (retCode);
}

/*---------------------------------------------------------------------------*/

RTEIPC_NamedSharedMemory::SHMRet    RTEIPC_NamedSharedMemory::Close   
(
    SAPDBErr_MessageList &      messageList 
)
{
    SAPDBErr_MessageList                       tmpMessageList;
    SHMRet                                     retCode = NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_NamedSharedMemory::Close", IPC_Trace, 1);

#if defined (_WIN32)
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;    

    if (NULL != m_pMem)
    {
        if (!UnmapViewOfFile(m_pMem))
        {
            rc = GetLastError();
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_UNMAP,
                                                 m_pName,
                                                 SAPDB_ToString (rc));
            retCode = Error;
        }
    }

    if (RTE_UNDEF_HANDLE != m_Handle)
    {
        if (!CloseHandle(m_Handle))
        {
            rc = GetLastError();
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_CLOSE_HANDLE, 
                                                              m_pName,
                                                              SAPDB_ToString (rc));
            retCode = Error;
        }
    }
#else
    if (NULL != m_pMem)
    {
        UnlinkSHMObject ();

        if (0 != RTE_save_munmap (m_pMem, m_Size))
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_UNMAP,
                                                              m_pName,
                                                              RTE_save_strerror (errno));
            retCode = Error;
        }
    }
    
    if (RTE_UNDEF_HANDLE != m_Handle)
    {
        if (0 != RTE_save_close (m_Handle))
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_CLOSE_HANDLE, 
                                                              m_pName,
                                                              RTE_save_strerror (errno));
            retCode = Error;
        }
    }

#endif

    if (NULL != m_pName)
    {
        RTEMem_RteAllocator::Instance().Deallocate(m_pName);
        m_pName = NULL;
    }

    m_pMem = NULL;
    m_Handle = RTE_UNDEF_HANDLE;

    if (NoError != retCode)
    {
        messageList = tmpMessageList;
    }

    return retCode;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTEIPC_NamedSharedMemory::Lock (SAPDBErr_MessageList &             messageList)
{
    RTE_SystemRc    rc = RTE_SYSTEMRC_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::Lock", 
                              IPC_Trace, 9);

#if defined (_WIN32)
    if (!SetKernelObjectSecurity (m_Handle,
                                  DACL_SECURITY_INFORMATION,
                                  RTESys_SecurityAttributes::Instance().GetLockSA().lpSecurityDescriptor))
    {
        rc = GetLastError();
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEWARN_IPC_SHM_SET_SECURITY, 
                                            m_pName, SAPDB_ToString (rc));

        return false;
    }
#else
    if (0 != UnlinkSHMObject ())
    {
		if (ENOENT != errno)
		{
			messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEWARN_IPC_SHM_UNLINK, 
				m_pName,
				RTE_save_strerror (errno));
			return false;
		}
    }
#endif
    return true;
}

/*---------------------------------------------------------------------------*/
#if !defined (_WIN32)

RTE_FileHandle RTEIPC_NamedSharedMemory::OpenSHMObject
(
    SAPDB_Int const openFlags,
    mode_t const    secAttribute 
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_NamedSharedMemory::OpenSHMObject", IPC_Trace, 9);

#if defined(AIX)// || defined(LINUX)
    if (O_CREAT == (openFlags & O_CREAT))
    {
        return (RTE_FileHandle) RTE_save_openCreate (m_pName, openFlags, secAttribute);
    }
    else
    {
        return (RTE_FileHandle) RTE_save_open (m_pName, openFlags);
    }
#else
    //shm_open and shm_unlink is supported by LINUX 2.4.x, glibc >= 2.2.3
    return (RTE_FileHandle) RTE_save_shm_open (m_pName, openFlags, secAttribute);
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_Int   RTEIPC_NamedSharedMemory::UnlinkSHMObject ()
{

    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_NamedSharedMemory::UnlinkSHMObject", IPC_Trace, 9);

#if defined(AIX) //|| defined(LINUX)
    return RTE_save_unlink (m_pName);
#else
    return RTE_save_shm_unlink (m_pName);
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool   RTEIPC_NamedSharedMemory::ChangeOwner (SAPDB_Int4                  groupId,
                                                    SAPDBErr_MessageList &      messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_NamedSharedMemory::ChangeOwner", IPC_Trace, 9);

    if (0 != RTE_save_fchown(m_Handle, RTE_save_getuid (), groupId))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_FCHOWN,
            m_pName,
            SAPDB_ToString (RTE_save_getuid ()),
            SAPDB_ToString (groupId),
            RTE_save_strerror (errno));
        return false;
    }

    return true;
}

#endif
    



