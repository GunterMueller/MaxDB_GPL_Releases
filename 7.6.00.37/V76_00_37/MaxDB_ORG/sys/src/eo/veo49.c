/*!
  @file           veo49.c
  @author         JoergM
  @ingroup        Security
  @brief          Security specific functions

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
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo49.h"

#if defined(WIN32)                        /*&if $OSSPEC = WIN32*/
#include "gos00.h"                        /* nocheck but_imkdep */
#include <aclapi.h>
#else                                     /*&else*/
#include <sys/stat.h>
#include <fcntl.h>
#include "RunTime/RTE_UNIXAuthenticate.h" /* nocheck but_imkdep */
#endif                                    /*&endif*/

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
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC bool eo49GetFileOwner(
const teo49_FileHandle FileHandle, 
tsp00_Namec OwnerName)
{
#if defined(WIN32)
    TCHAR Account[sizeof(tsp00_KnlIdentifierc)];
    TCHAR Domain[sizeof(tsp00_KnlIdentifierc)];
    PSID OwnerSID;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    SID_NAME_USE aUser = SidTypeUser;

    if ( sql02_get_platform_id() != VER_PLATFORM_WIN32_NT )
    {
        DWORD NameSize = sizeof(tsp00_Namec);
        return ( GetUserName( (char *)OwnerName, &NameSize) != 0 );
    }
    else
    {
        bool ok;
        DWORD AccountSize = sizeof(tsp00_KnlIdentifier);
        DWORD DomainSize = sizeof(tsp00_KnlIdentifier);

        if ( GetSecurityInfo( FileHandle,
                              SE_FILE_OBJECT,
                              OWNER_SECURITY_INFORMATION,
                             &OwnerSID,
                              NULL, // Group SID not needed
                              NULL, // DACL not needed
                              NULL, // SACL not needed
                             &SecurityDescriptor ) != ERROR_SUCCESS )
        {
            return false;
        }

        ok = ( LookupAccountSid( NULL,
                               OwnerSID,
                               Account,
                              &AccountSize,
                               Domain,
                              &DomainSize,
                              &aUser ) != false );

        LocalFree(SecurityDescriptor);

        if ( ok )
        {
            if ( strcmp(Domain, "BUILTIN") != 0 )
            {
                ok = (bool)( (strlen(Account) + strlen(Domain)) < sizeof(tsp00_Namec) );
                if ( ok )
                {
                    strcpy(OwnerName, Domain);
                    strcat(OwnerName, "\\");
                    strcat(OwnerName, Account);
                }
            }
            else
            {
                strcpy(OwnerName, Account);
            }
        }
        return ok;
    }
#else
    struct stat statBuffer;
    SAPDB_UInt4 neededSize;

    if ( fstat(FileHandle, &statBuffer) == 0 )
    {
        return RTE_GetUsernameFromPasswdById( (SAPDB_Int4)statBuffer.st_uid
                                            , &OwnerName[0]
                                            , sizeof(tsp00_Namec)
                                            , &neededSize);
    }
    return false;    
#endif
}

externC bool eo49SetFileGroup(
const teo49_FileHandle FileHandle, 
tsp00_Namec const GroupName)
{
#if defined(WIN32)
    return true;
#else
    struct stat statBuffer;
    SAPDB_Int4 groupId;
    if ( !RTE_GetGroupIdFromGroupByName(&GroupName[0], &groupId) )
    {
        return false;
    }

    if ( fstat(FileHandle, &statBuffer) == 0 )
    {
        return ( fchown(FileHandle, statBuffer.st_uid, (gid_t)groupId) == 0 );
    }
    return false;    
#endif
}

externC bool eo49GetFileGroup(
const teo49_FileHandle FileHandle, 
tsp00_Namec GroupName)
{
#if defined(WIN32)
    memset(GroupName, 0, sizeof(tsp00_Namec));
    return true;
#else
    struct stat statBuffer;
    SAPDB_UInt4 neededSize = 0;

    if ( fstat(FileHandle, &statBuffer) == 0 )
    {
        return RTE_GetGroupNameFromGroupById( (SAPDB_Int4)statBuffer.st_gid
                                            , &GroupName[0]
                                            , sizeof( tsp00_Namec)
                                            , &neededSize );
    }
    return false;    
#endif
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
