/*!
  @file           RTE_UNIXAuthenticate.cpp
  @author         JoergM
  @brief          DBM RunTime: User Authentification

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

#include "RunTime/RTE_UNIXAuthenticate.hpp"
#include "RunTime/RTE_IniFileHandling.h"
#include "RunTime/RTE_saveUNIXcalls.h"
#include "RunTime/System/RTESys_IO.h"
#include "RunTime/RTE_MessageList.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_Names.h"
#include "heo01.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "geo28.h"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDBCommon/Messages/SDBMsg_RunTime.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#include <unistd.h>
/* this .... is needed due to corrupted header file grp.h on tru64... */
#if defined(OSF1)
#  ifdef _XOPEN_SOURCE_EXTENDED
#    undef _XOPEN_SOURCE_EXTENDED
#      include <grp.h>
#    define _XOPEN_SOURCE_EXTENDED
#  else
#    include <grp.h>
#  endif
#else
#  include <grp.h>
#endif
#include <sys/wait.h>

#ifndef NO_CRYPT

#include "RunTime/RTE_Configure.h"  /* dep=RTE_Configure.pl */
#include "RunTime/System/RTESys_SharedLib.hpp"

#ifndef AIX
/* on AIX this produces a confilct with stdlib.h definition of setkey.... */
#include <crypt.h>
#endif /* AIX */

#ifdef HAS_PAM_SUPPORT
#include <security/pam_appl.h>
#endif /* HAS_PAM_SUPPORT */

#endif /* NO_CRYPT */


#include "RunTime/RTE_UNIXAuthenticate.h" /* implemented in RTE_IniFileHandling.c */

SAPDB_Bool
RTE_GetUserIdFromUsername(SAPDB_Char const *    userName,
                          SAPDB_Int4           &userId,
                          Msg_List &errList)
{
    if ( userName )
    {
        if ( !RTE_GetUserIdFromPasswdByName(userName, userId) )
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_USER_UNKNOWN,
                                           userName);
            return false;
        }
    }
    else
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                       RTEERR_USER_UNKNOWN,
                                       "no user given");
        return false;
    }

    return true;
}

/*--------------------*/

SAPDB_Bool
RTE_GetUserAndPrimaryGroupIdFromUsername( SAPDB_Char const     *userName,
                                          SAPDB_Int4           &userId,
                                          SAPDB_Int4           &groupId,
                                          Msg_List &errList)
{
    if ( userName )
    {
        if ( !RTE_GetUserAndGroupIdFromPasswdByName(userName, userId, groupId) )
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_USER_UNKNOWN,
                                           userName);
            return false;
        }

        return true;
    }
    else
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                       RTEERR_USER_UNKNOWN,
                                       "no user given");
        return false;
    }
}

/*--------------------*/

SAPDB_Bool
RTE_GetUsernameFromUserId(SAPDB_Int4  userId,
                          SAPDB_Char *userNameBuffer,
                          SAPDB_Int4  userNameBufferSize,
                          Msg_List &errList)
{
    SAPDBERR_ASSERT_STATE(userNameBuffer != 0);
    SAPDBERR_ASSERT_STATE(userNameBufferSize > 0);

    SAPDB_UInt4 neededSize = 0;

    if ( !RTE_GetUsernameFromPasswdById(userId, userNameBuffer, userNameBufferSize, neededSize) )
    {
        if ( neededSize == 0 )
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_USERID_UNKNOWN,
                                           SAPDB_ToString(userId) );
            return false;
        }
        else
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_USERNAME_TOO_LONG,
                                           SAPDB_ToString(userId),
                                           userNameBuffer);
            return false;
        }
    }

    return true;
}

/*--------------------*/

SAPDB_Bool
RTE_GetGroupIdFromGroupname( SAPDB_Char const *    groupName,
                             SAPDB_Int4           &groupId,
                             Msg_List &errList)
{
    if ( groupName )
    {
        if ( ! RTE_GetGroupIdFromGroupByName(groupName, groupId) )
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_GROUP_UNKNOWN,
                                           groupName);
            return false;
        }

        return true;
    }
    else
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                       RTEERR_GROUP_UNKNOWN,
                                       "no group given");
        return false;
    }
}

/*--------------------*/

SAPDB_Bool
RTE_GetGroupnameFromGroupId(SAPDB_Int4  groupId,
                            SAPDB_Char *groupNameBuffer,
                            SAPDB_Int4  groupNameBufferSize,
                            Msg_List &errList)
{
    SAPDBERR_ASSERT_STATE(groupNameBuffer != 0);
    SAPDBERR_ASSERT_STATE(groupNameBufferSize > 0);

    SAPDB_UInt4 neededSize = 0;

    if ( ! RTE_GetGroupNameFromGroupById(groupId, groupNameBuffer, groupNameBufferSize, neededSize) )
    {
        if ( neededSize == 0 )
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_GROUPID_UNKNOWN,
                                           SAPDB_ToString(groupId) );
            return false;
        }
        else
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_GROUPNAME_TOO_LONG,
                                           SAPDB_ToString(groupId),
                                           groupNameBuffer );
            return false;
        }
    }

    return true;
}

/*------------------------------*/

SAPDB_Bool
RTE_SetUserAndGroupId( SAPDB_Int4 userId
                     , SAPDB_Int4 groupId
                     , Msg_List &errList )
{
    RTE_SystemRc rc;
    RTESys_ErrorMessage spaceToFill;
    SAPDB_Char userNameBuffer[256];

    if ( !RTE_GetUsernameFromUserId( userId,
                                     &userNameBuffer[0],
                                     (SAPDB_UInt4)sizeof(userNameBuffer),
                                     errList) )
    {
        return false;
    }

    /* root user must become effectiv root user before it can set to 'any' user/group combination! */
    if ( RTE_save_getuid() == 0 )
    {
        if ( RTE_save_setuid(0) )
        {
            rc = RTESys_GetLastError();
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_SETUID_FAILED,
                                            "for effective id 0",
                                            RTESys_StrError(rc, spaceToFill) );
            return false;
        }
    }

    if ( RTE_save_geteuid() == 0 )
    {
        /* effective user is superuser */
        if ( RTE_save_setgid(groupId) )
        {
            rc = RTESys_GetLastError();
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_SETGID_FAILED,
                                            SAPDB_ToString(groupId),
                                            RTESys_StrError(rc, spaceToFill) );
            return false;
        }

        if ( initgroups(&userNameBuffer[0], groupId) )
        {
            rc = RTESys_GetLastError();
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_SETGID_FAILED,
                                            "initgroups failed",
                                            RTESys_StrError(rc, spaceToFill) );
            return false;
        }

        if ( RTE_save_setuid(userId) )
        {
            rc = RTESys_GetLastError();
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_SETUID_FAILED,
                                            SAPDB_ToString(userId),
                                            RTESys_StrError(rc, spaceToFill) );
            return false;
        }
    }
    else
    {
        /* effective user is no superuser */
        if ( RTE_save_setregid(groupId, groupId) )
        {
            rc = RTESys_GetLastError();
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_SETGID_FAILED,
                                            SAPDB_ToString(groupId),
                                            RTESys_StrError(rc, spaceToFill) );
            return false;
        }

        if ( 
         RTE_save_setreuid(userId, userId)
#if defined(HPUX) || defined(OSF1) || defined(AIX)
      /* some platform do not allow non authorized users to set real user id */
      /* but these at least do not switch back to real user in 'system(command)' calls */
      && RTE_save_setreuid(    -1, userId)
#endif
       )
        {
            rc = RTESys_GetLastError();
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_SETUID_FAILED,
                                            SAPDB_ToString(userId),
                                            RTESys_StrError(rc, spaceToFill) );
            return false;
        }
    }

    return true;
}

/*---------------------------------*/

SAPDB_Bool
RTE_SetSapdbOwnerAndGroupId(Msg_List &errList)
{
    SAPDB_Int4 userId;
    SAPDB_Int4 groupId;

    if ( !RTE_GetDefaultSapdbUserAndGroupIds( userId,
                                              groupId,
                                              errList ) )
    {
        return false;
    }

    return RTE_SetUserAndGroupId( userId,
                                  groupId,
                                  errList );
}

/*---------------------------------*/

void
RTE_GetEffectivUserIdAndGroupId(SAPDB_Int4 &userId, SAPDB_Int4 &groupId)
{
    userId = (SAPDB_Int4)RTE_save_geteuid(); 
    groupId = (SAPDB_Int4)RTE_save_getegid(); 
}

/*---------------------------------*/

void
RTE_GetRealUserIdAndGroupId(SAPDB_Int4 &userId, SAPDB_Int4 &groupId)
{
    userId = (SAPDB_Int4)RTE_save_getuid(); 
    groupId = (SAPDB_Int4)RTE_save_getgid(); 
}

/*---------------------------------*/

#define SUBPATH_TO_KERNEL_EXECUTABLE "/pgm/kernel"

SAPDB_Bool
RTE_IsSapdbOwnerOrRoot(Msg_List &errList)
{
    if ( geteuid() == (uid_t)0 )
    {
        return true;
    }

    SAPDB_Char userNameBuffer[256];
    SAPDB_Char OwnernameBuffer[256];

    if ( !RTE_GetUsernameFromUserId( (SAPDB_Int4)geteuid(),
                                     userNameBuffer,
                                     (SAPDB_Int4)sizeof(userNameBuffer),
                                     errList) )
    {
        return false;
    }

    RTE_IniFileResult   ok;
    tsp00_ErrTextc      errtext;
    RTE_GetInstallationConfigString( SAPDB_SAPDBOWNER_KEY, 
                                     OwnernameBuffer, 
                                     (SAPDB_Int4)sizeof(OwnernameBuffer),
                                     errtext,
                                     ok );
    if ( SAPDB_INIFILE_RESULT_OK != ok)
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_REGISTRY_KEY_NOT_FOUND,
                                        SAPDB_SAPDBOWNER_KEY,
                                        errtext.asCharp ()
                                      );
        return false;
    }

    SAPDB_Int iCheck;
    for ( iCheck = 0; userNameBuffer[iCheck] != 0; ++iCheck )
    {
        if ( toupper(userNameBuffer[iCheck]) != toupper(OwnernameBuffer[iCheck]) )
        {
            break;
        }
    }

    if ( toupper(userNameBuffer[iCheck]) != toupper(OwnernameBuffer[iCheck]) )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_OWNERNAME_MISMATCH,
                                        userNameBuffer,
                                        OwnernameBuffer
                                      );
        return false;
    }

    return true;
}

/*--------------------------*/

SAPDB_Bool
RTE_GetDefaultSapdbUserAndGroupIds(SAPDB_Int4 &userId,
                                   SAPDB_Int4 &groupId,
                                   Msg_List &errList)
{
    SAPDB_Char OwnernameBuffer[256];
    SAPDB_Char groupNameBuffer[256];
    RTE_IniFileResult  ok;
    tsp00_ErrTextc      errtext;

    RTE_GetInstallationConfigString( SAPDB_SAPDBOWNER_KEY, 
                                     OwnernameBuffer, 
                                     (SAPDB_Int4)sizeof(OwnernameBuffer),
                                     errtext,
                                     ok );
    if ( SAPDB_INIFILE_RESULT_OK != ok)
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_REGISTRY_KEY_NOT_FOUND,
                                        SAPDB_SAPDBOWNER_KEY,
                                        errtext.asCharp ()
                                      );
        return false;
    }

    RTE_GetInstallationConfigString( SAPDB_SAPDBGROUP_KEY, 
                                     groupNameBuffer, 
                                     (SAPDB_Int4)sizeof(groupNameBuffer),
                                     errtext,
                                     ok );
    if ( SAPDB_INIFILE_RESULT_OK != ok)
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_REGISTRY_KEY_NOT_FOUND,
                                        SAPDB_SAPDBGROUP_KEY,
                                        errtext.asCharp ()
                                      );
        return false;
    }

    if ( !RTE_GetUserIdFromUsername( OwnernameBuffer,
                                     userId,
                                     errList ) )
    {
        return false;
    }

    if ( !RTE_GetGroupIdFromGroupname( groupNameBuffer,
                                       groupId,
                                       errList ) )
    {
        return false;
    }

    return true;
}

/*--------------------------*/

SAPDB_Bool
RTE_GetDefaultOwnerAndSupportGroupFromDbroot( SAPDB_Char const *dbroot,
                                              SAPDB_Int4 &userId,
                                              SAPDB_Int4 &groupId,
                                              Msg_List &errList)
{
    RTE_Path           pathToKernel;

    if ( (strlen(dbroot) + strlen(SUBPATH_TO_KERNEL_EXECUTABLE)) >= sizeof(RTE_Path) )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_PATH_TOO_LONG,
                                        dbroot
                                      );
        return false;
    }

    strcpy(pathToKernel, dbroot);
    strcat(pathToKernel, SUBPATH_TO_KERNEL_EXECUTABLE);

    return RTE_GetFileOwnerAndGroup(pathToKernel, userId, groupId, errList);
}

/*--------------------------*/
                                
SAPDB_Bool
RTE_GetOwnerAndSupportGroupIdFromDatabaseName( SAPDB_Char const *dbname,
                                               SAPDB_Int4 &userId, 
                                               SAPDB_Int4 &groupId, 
                                               Msg_List &errList)
{
#if 1
    teo200_EventList oEvent;
    oEvent.eo200_ClearEventList();
    tsp00_Namec szDbname;

    szDbname.rawAssign(dbname);
    teo28_ProfileContainer  oProfile(szDbname, true, &oEvent);

    if (!oEvent) 
    {
        tsp00_Namec szUPCOwner;
        if (!oProfile.eo28_GetOwnerName(szUPCOwner, &oEvent)) 
        {
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_DATABASE_NOT_FOUND,
                                            dbname,
                                            oEvent.eo200_EventMsg()
                                          );
            return false;
        } // end if

        if ( !RTE_GetUserIdFromUsername( szUPCOwner.asCharp(),
                                         userId,
                                         errList ) )
        {
            return false;
        }
        tsp00_Namec szUPCGroup;
        if (!oProfile.eo28_GetGroupName(szUPCGroup, &oEvent)) 
        {
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_DATABASE_NOT_FOUND,
                                            dbname,
                                            oEvent.eo200_EventMsg()
                                          );
            return false;
        } // end if

        if ( !RTE_GetGroupIdFromGroupname( szUPCGroup.asCharp(),
                                           groupId,
                                           errList ) )
        {
            return false;
        }
    } // end if
    else
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_DATABASE_NOT_FOUND,
                                        dbname,
                                        oEvent.eo200_EventMsg()
                                      );
        return false;
    }

    return true;
#else
    RTE_Path            dbRoot;
    RTE_IniFileResult   ok;
    tsp00_ErrTextc      errtext;

    RTE_GetConfigString (SAPDB_DATABASES_INI_FILE, SAPDB_DATABASES_SECTION,
                    	    dbname, dbRoot, sizeof(RTE_Path), errtext, ok);

    if ( SAPDB_INIFILE_RESULT_OK != ok)
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_DATABASE_NOT_FOUND,
                                        dbname,
                                        errtext.asCharp ()
                                      );
        return false;
    }

    return RTE_GetDefaultOwnerAndSupportGroupFromDbroot(dbRoot, userId, groupId, errList);
#endif
}

/*---------------------------*/

SAPDB_Bool
RTE_SetFileOwnerAndGroup(SAPDB_Char const *pathName,
                         SAPDB_Int4 userId,
                         SAPDB_Int4 groupId,
                         Msg_List &errList)
{
    RTE_SystemRc rc;
    RTESys_ErrorMessage spaceToFill;
    if ( RTE_save_chown(pathName, userId, groupId) != 0 )
    {
        rc = RTESys_GetLastError();
        if ( rc != RTE_SYSTEMRC_FILE_NOT_FOUND )
        {
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_CHOWN_FAILED,
                                            pathName,
                                            SAPDB_ToString(userId),
                                            SAPDB_ToString(groupId),
                                            RTESys_StrError(rc, spaceToFill)
                                          );
            return false;
        }
    }

    return true;
}

/*----------------------------------------------*/

SAPDB_Bool
RTE_SetFileOwnerAndGroup(SAPDB_Char const *pathName,
                         SAPDB_Char const *userName,
                         SAPDB_Char const *groupName,
                         Msg_List &errList)
{
    SAPDB_Int4 ownerId;
    SAPDB_Int4 groupId;

    if ( !RTE_GetUserIdFromUsername(userName, ownerId, errList) )
    {
        return false;
    }

    if ( !RTE_GetGroupIdFromGroupname(groupName, groupId, errList) )
    {
        return false;
    }

    return RTE_SetFileOwnerAndGroup(pathName, ownerId, groupId, errList);
}

/*----------------------------------------------*/

SAPDB_Bool
RTE_GetFileOwnerAndGroup(SAPDB_Char const *pathName,
                         SAPDB_Int4 &userId,
                         SAPDB_Int4 &groupId,
                         Msg_List &errList)
{
    struct stat statBuffer;

    if ( RTE_save_stat(pathName, &statBuffer) )
    {
        RTE_SystemRc rc = RTESys_GetLastError();
        RTESys_ErrorMessage spaceToFill;
        errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                       RTEERR_STAT_FAILED,
                                       pathName,
                                       RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    userId = (SAPDB_Int4)statBuffer.st_uid;
    groupId = (SAPDB_Int4)statBuffer.st_gid;

    return true;
}

/*----------------------------------------------*/

SAPDB_Bool
RTE_GetFileOwnerAndGroup(RTE_FileHandle handle,
                         SAPDB_Int4 &userId,
                         SAPDB_Int4 &groupId,
                         Msg_List &errList)
{
    struct stat statBuffer;

    if ( RTE_save_fstat(handle, &statBuffer) )
    {
        RTE_SystemRc rc = RTESys_GetLastError();
        RTESys_ErrorMessage spaceToFill;
        errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                       RTEERR_FSTAT_FAILED,
                                       SAPDB_ToString(handle),
                                       RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    userId = (SAPDB_Int4)statBuffer.st_uid;
    groupId = (SAPDB_Int4)statBuffer.st_gid;

    return true;
}

/*----------------------------------------------*/

SAPDB_Bool
RTE_GetFileOwnerAndGroup(SAPDB_Char const *pathName,
                         SAPDB_Char *userNameBuffer,
                         SAPDB_Int4  userNameBufferSize,
                         SAPDB_Char *groupNameBuffer,
                         SAPDB_Int4  groupNameBufferSize,
                         Msg_List &errList)
{
    SAPDB_Int4 userId;
    SAPDB_Int4 groupId;

    SAPDBERR_ASSERT_STATE(userNameBuffer != 0);
    SAPDBERR_ASSERT_STATE(userNameBufferSize > 0);
    SAPDBERR_ASSERT_STATE(groupNameBuffer != 0);
    SAPDBERR_ASSERT_STATE(groupNameBufferSize > 0);

    if ( !RTE_GetFileOwnerAndGroup(pathName,
                                   userId,
                                   groupId,
                                   errList) )
    {
        return false;
    }

    if ( !RTE_GetUsernameFromUserId( userId,
                                     userNameBuffer,
                                     userNameBufferSize,
                                     errList) )
    {
        return false;
    }

    if ( !RTE_GetGroupnameFromGroupId( groupId,
                                       groupNameBuffer,
                                       groupNameBufferSize,
                                       errList) )
    {
        return false;
    }

    return true;
}

/*----------------------------------------------*/

SAPDB_Bool
RTE_GetFileOwnerAndGroup(RTE_FileHandle handle,
                         SAPDB_Char *userNameBuffer,
                         SAPDB_Int4  userNameBufferSize,
                         SAPDB_Char *groupNameBuffer,
                         SAPDB_Int4  groupNameBufferSize,
                         Msg_List &errList)
{
    SAPDB_Int4 userId;
    SAPDB_Int4 groupId;

    SAPDBERR_ASSERT_STATE(userNameBuffer != 0);
    SAPDBERR_ASSERT_STATE(userNameBufferSize > 0);
    SAPDBERR_ASSERT_STATE(groupNameBuffer != 0);
    SAPDBERR_ASSERT_STATE(groupNameBufferSize > 0);

    if ( !RTE_GetFileOwnerAndGroup(handle,
                                   userId,
                                   groupId,
                                   errList) )
    {
        return false;
    }

    if ( !RTE_GetUsernameFromUserId( userId,
                                     userNameBuffer,
                                     userNameBufferSize,
                                     errList) )
    {
        return false;
    }

    if ( !RTE_GetGroupnameFromGroupId( groupId,
                                       groupNameBuffer,
                                       groupNameBufferSize,
                                       errList) )
    {
        return false;
    }

    return true;
}

/*--------------------*/

SAPDB_Bool
RTE_VerifyUserAsSapdbAdministrator(
    SAPDB_Char const *userName,
    Msg_List &errList)
{
    RTE_IniFileResult  ok;
    tsp00_ErrTextc errtext;
    SAPDB_Char userNameBuffer[256];
    SAPDB_Char groupNameBuffer[256];
    SAPDB_Int4 userId;
    SAPDB_Int4 primaryGroupId;

    SAPDBERR_ASSERT_STATE(userName != 0);

    /* First get user id and primary group id */
    if ( !RTE_GetUserAndPrimaryGroupIdFromUsername( userName,
                                                    userId,
                                                    primaryGroupId,
                                                    errList ) )
    {
        return false;
    }

    /* let superuser be always administrator */
    if ( 0 == userId )
    {
        return true;
    }

    /* now get SdbOwner */
    RTE_GetInstallationConfigString( SAPDB_SAPDBOWNER_KEY, 
                                     userNameBuffer, 
                                     (SAPDB_Int4)sizeof(userNameBuffer),
                                     errtext,
                                     ok );
    if ( SAPDB_INIFILE_RESULT_OK != ok)
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_REGISTRY_KEY_NOT_FOUND,
                                        SAPDB_SAPDBOWNER_KEY,
                                        errtext.asCharp ()
                                      );
        return false;
    }

    /* Check if SdbOwner matches */
    SAPDB_Int iCheck;
 	for ( iCheck = 0; userName[iCheck] != 0; ++iCheck )
 	{
 		if ( toupper(userName[iCheck]) != toupper(userNameBuffer[iCheck]) )
 		{
 			break;
 		}
 	}
 	if ( toupper(userName[iCheck]) == toupper(userNameBuffer[iCheck]) )
    {
 	    return true; /* SdbOwner is always administrator */
    }
 
    /* now get SdbAdmin group */
    RTE_GetInstallationConfigString( SAPDB_SAPDBGROUP_KEY, 
                                     groupNameBuffer, 
                                     (SAPDB_Int4)sizeof(groupNameBuffer),
                                     errtext,
                                     ok );
    if ( SAPDB_INIFILE_RESULT_OK != ok)
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_REGISTRY_KEY_NOT_FOUND,
                                        SAPDB_SAPDBOWNER_KEY,
                                        errtext.asCharp ()
                                      );
        return false;
    }

    SAPDB_Bool groupFound = false;
    if ( !RTE_SearchUserNameInGroupMemberList(userName, primaryGroupId, groupNameBuffer, groupFound) )
    {
        if ( !groupFound )
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_GROUP_UNKNOWN,
                                           groupNameBuffer);
        }
        else
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                           RTEERR_USER_NOT_MEMBER_IN_GROUP,
                                           userName,
                                           groupNameBuffer);
        }
        return false;
    }

    return true;
}

/*--------------------*/

SAPDB_Bool
RTE_VerifyUserIdGroupIdCombinationAsSapdbAdministrator(
    SAPDB_Int4 userId,
    SAPDB_Int4 primaryGroupId,
    Msg_List &errList)
{
    RTE_IniFileResult  ok;
    tsp00_ErrTextc errtext;
    SAPDB_Char groupNameBuffer[256];

    /* let superuser be always administrator */
    if ( 0 == userId )
    {
        return true;
    }

    /* now get SdbAdmin group */
    RTE_GetInstallationConfigString( SAPDB_SAPDBGROUP_KEY, 
                                     groupNameBuffer, 
                                     (SAPDB_Int4)sizeof(groupNameBuffer),
                                     errtext,
                                     ok );
    if ( SAPDB_INIFILE_RESULT_OK != ok)
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_REGISTRY_KEY_NOT_FOUND,
                                        SAPDB_SAPDBOWNER_KEY,
                                        errtext.asCharp ()
                                      );
        return false;
    }

    SAPDB_Int4 foundGroupId;
    if ( !RTE_GetGroupIdFromGroupByName(groupNameBuffer, foundGroupId) )
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                       RTEERR_GROUP_UNKNOWN,
                                       groupNameBuffer);
        return false;
    }

    /* if primary group matches, user is member of database administrator group */
    if ( primaryGroupId == foundGroupId )
    {
        return true;
    }

    gid_t processGroupList[NGROUPS_MAX];
    int nGroups = getgroups(NGROUPS_MAX, processGroupList);
    if ( nGroups < 0 )
    {
        RTE_SystemRc rc = RTESys_GetLastError();
        RTESys_ErrorMessage spaceToFill;
        errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                       RTEERR_CANNOT_GET_PROCESS_GROUP_LIST,
                                       SAPDB_ToString(NGROUPS_MAX),
                                       RTESys_StrError(rc, spaceToFill) );
    }

    /* otherwise he must be explicitly listed in process group list */
    for ( int iGroup = 0; iGroup < nGroups; iGroup++ )
    {
        if ( foundGroupId == processGroupList[iGroup] )
        {
            return true; /* ok we found him */
        }
    }

    errList = SAPDBErr_MessageList(RTE_CONTEXT,
                                   RTEERR_USER_NOT_MEMBER_IN_GROUP,
                                   SAPDB_ToString(userId),
                                   groupNameBuffer);
    return false;
}

/*--------------------*/

SAPDB_Bool
RTE_VerifyAsSapdbAdministrator(
    Msg_List &errList)
{
    return RTE_VerifyUserIdGroupIdCombinationAsSapdbAdministrator(
        (SAPDB_Int4)RTE_save_getuid(),
        (SAPDB_Int4)RTE_save_getgid(),
        errList );
}

/*--------------------*/

SAPDB_Bool
RTE_IsSdbAdministrator (SAPDB_UInt4 const userId,
                        SAPDB_UInt4 const groupId)
{
    Msg_List errList;

    return RTE_VerifyUserIdGroupIdCombinationAsSapdbAdministrator(
        (SAPDB_Int4)userId,
        (SAPDB_Int4)groupId,
        errList );
}

/*----------------------*/
#ifndef NO_CRYPT

#ifdef HAS_PAM_SUPPORT

struct userdata  {
    SAPDB_Char const     *username;
    SAPDB_Char const     *passwd;
};

#if defined(HAS_CONST_PAM_MESSAGE_PARAMETER)
typedef const struct pam_message **pam_message_pointer_array;
#else
typedef       struct pam_message **pam_message_pointer_array;
#endif

extern "C" int
RTE_PAM_conv ( int               num_msg,
           pam_message_pointer_array msg,
           struct pam_response **resp,
           void                 *appdata_ptr )
{
    int                   replies  = 0;
    struct pam_response  *reply    = NULL;
    struct userdata      *p        = (struct userdata *)appdata_ptr;


    if ( p == NULL )
        return  PAM_SYSTEM_ERR;

    reply = (struct pam_response*) malloc ( sizeof (struct pam_response) * num_msg );
    if ( reply == NULL )
        return PAM_CONV_ERR;

    for ( replies = 0; replies < num_msg; replies++ ) {
        switch (msg[replies]->msg_style) {

        case PAM_PROMPT_ECHO_ON: /* assume it wants user name */
            reply[replies].resp_retcode = PAM_SUCCESS;
            reply[replies].resp = strdup ( p->username );
            /* PAM frees resp */
            break;

        case PAM_PROMPT_ECHO_OFF: /* assume it wants password */
            reply[replies].resp_retcode = PAM_SUCCESS;
            reply[replies].resp = strdup ( p->passwd );
            /* PAM frees resp */
            break;

        case PAM_TEXT_INFO:
        case PAM_ERROR_MSG:
            reply[replies].resp_retcode = PAM_SUCCESS;
            reply[replies].resp = NULL;
            break;

        default:
            free (reply);
            return PAM_CONV_ERR;
        } /* switch */
    } /* for */

    *resp = reply;
    return PAM_SUCCESS;
}

static struct pam_conv PAM_conversation = {
    &RTE_PAM_conv,      /* address of our interface function */
    NULL
};


#define PAM_LOADED_NEVER   0
#define PAM_LOAD_SUCCEEDED 1
#define PAM_LOAD_FAILED    2

static SAPDB_Int RTE_pam_library_loaded  = PAM_LOADED_NEVER;

extern "C"  {

static union pam_start_call {
    RTE_SharedLibProcAddress pAddr;
    int (*pCall)(const char *service, const char *user, const
       struct pam_conv *conv, pam_handle_t **pamh_p);
} RTE_pam_start_call;

static union pam_authenticate_call {
    RTE_SharedLibProcAddress pAddr;
    int (*pCall)(pam_handle_t *pamh, int  flags);
} RTE_pam_authenticate_call;

static union pam_acct_mgmt_call {
    RTE_SharedLibProcAddress pAddr;
    int (*pCall)(pam_handle_t *pamh, int  flags);
} RTE_pam_acct_mgmt_call;

static union pam_end_call {
    RTE_SharedLibProcAddress pAddr;
    int (*pCall)(pam_handle_t *pamh, int status);
} RTE_pam_end_call;

static union pam_strerror_call {
    RTE_SharedLibProcAddress pAddr;
    const  char  * (*pCall)(pam_handle_t *pamh, int pam_error);
} RTE_pam_strerror_call;

/* optional */
static union pam_fail_delay_call {
    RTE_SharedLibProcAddress pAddr;
    int (*pCall)(pam_handle_t *pamh, int  flags);
} RTE_pam_fail_delay_call;

};
    
static char globalPamConfiguration[16] = SAPDB_PAM_DEFAULT_SERVICE_NAME; ///< global PAM service configuration

/*
  Validate that PAM is available and dynamically load libpam and find the entry points
 */
SAPDB_Bool RTE_check_pam_authentification_available(SAPDB_Bool &pamAvailable, Msg_List &errList)
{
    static Msg_List loadErrorList;

    pamAvailable = false;

    /*
      Allow to bypass PAM if really wanted...
     */
    if ( getenv(SAPDB_PAM_NOT_USED) )
    {
        return true; /* simulates no PAM libraries installed... */
    }

    SAPDB_Bool ok = false;

    if ( PAM_LOADED_NEVER == RTE_pam_library_loaded )
    {
        char configurationValue[16];
        RTE_IniFileResult  resultOk;
        tsp00_ErrTextc errtext;

        RTE_GetInstallationConfigString( SAPDB_PAM_KEY, 
                                         configurationValue, 
                                         (SAPDB_Int4)sizeof(configurationValue),
                                         errtext,
                                         resultOk );
        if ( SAPDB_INIFILE_RESULT_OK == resultOk )
        {
            if ( 0 == strcmp("NOT_USED", configurationValue)
              || 0 == strcmp("PAM_NOT_USED", configurationValue)
              || 0 == strcmp("0", configurationValue)
              || 0 == strcmp("", configurationValue) )
            {
                return true;
            }
            else
            {
                strncpy(&globalPamConfiguration[0], &configurationValue[0], sizeof(configurationValue));
                globalPamConfiguration[sizeof(globalPamConfiguration)-1] = 0;
            }
        }
        else
        {
            return true;
        }

        const char *libpamPath;
#if defined(AIX)
# if defined(BIT64)
        libpamPath = "/usr/lib/libpam.a(shr_64.o)"; /* AIX is always somewhat special... */
# else
        libpamPath = "/usr/lib/libpam.a(shr.o)"; /* AIX is always somewhat special... */
# endif
#else
        libpamPath = "libpam";
#endif
        /* Memory leak accepted to prevent UnloadSharedLibrary... */
        RTESys_SharedLib *pPamLibrary = new ( RTEMem_Allocator::Instance() ) 
                RTESys_SharedLib(libpamPath, RTE_LoadSymbolsGlobal, ok, loadErrorList);
        if ( ok )
        {
            /* load needed entries */
            ok = pPamLibrary->GetProcAddress("pam_start", RTE_pam_start_call.pAddr, loadErrorList);
            if ( ok )
            {
                ok = pPamLibrary->GetProcAddress("pam_authenticate", RTE_pam_authenticate_call.pAddr, loadErrorList);
                if ( ok )
                {
                    ok = pPamLibrary->GetProcAddress("pam_acct_mgmt", RTE_pam_acct_mgmt_call.pAddr, loadErrorList);
                    if ( ok )
                    {
                        ok = pPamLibrary->GetProcAddress("pam_end", RTE_pam_end_call.pAddr, loadErrorList);
                        if ( ok )
                        {
                            ok = pPamLibrary->GetProcAddress("pam_strerror", RTE_pam_strerror_call.pAddr, loadErrorList);
                            if ( ok )
                            {
                                pamAvailable = true;

                                Msg_List dummyErrList;
                                /* load optional entries */
                                if ( !pPamLibrary->GetProcAddress("pam_fail_delay", RTE_pam_fail_delay_call.pAddr, dummyErrList) )
                                {
                                    RTE_pam_fail_delay_call.pAddr = 0;
                                }
                            }
                        }
                    }
                }
            }
        }

        if ( ok )
        {
            RTE_pam_library_loaded = PAM_LOAD_SUCCEEDED;
        }
        else
        {
            RTE_pam_library_loaded = PAM_LOAD_FAILED;
            errList = loadErrorList; /* copy load error */
        }
    }
    else /* PAM was already loaded, now check for previous load result */
    {
        if ( PAM_LOAD_SUCCEEDED == RTE_pam_library_loaded )
        {
            pamAvailable = true;
            ok = true;
        }
        else
        {
            errList = loadErrorList; /* copy load error */
            ok = false;
        }
    }

    return ok;
}

//-----

static SAPDB_Bool RTE_UsePAMUserPasswordVerification(
    SAPDB_Char const     *userName,
    SAPDB_Char const     *passwd,
    Msg_List             &errList)
{
    pam_handle_t *pamh = NULL;
    int retval;
    struct userdata pam_user_data;

    /* create structure for Pam conv */
    pam_user_data.username = userName;
    pam_user_data.passwd   = passwd;
    PAM_conversation.appdata_ptr = &pam_user_data;

    SAPDB_Char *pam_service = getenv(SAPDB_PAM_SERVICE_NAME);
    if ( !pam_service )
    {
        pam_service = &globalPamConfiguration[0];
    }

    retval = (*RTE_pam_start_call.pCall)(pam_service, userName, &PAM_conversation, &pamh);
    if (retval == PAM_SUCCESS)
    {
        /* optional switch off fail delay */
        if ( RTE_pam_fail_delay_call.pAddr )
           (*RTE_pam_fail_delay_call.pCall)(pamh, 0); // on failure no delay... delay handled later...

        retval = (*RTE_pam_authenticate_call.pCall)(pamh, 0);
        if (retval == PAM_SUCCESS)
        {
            /* check if current user is allowed to use service now */
            retval = (*RTE_pam_acct_mgmt_call.pCall)(pamh, 0);
            if (retval != PAM_SUCCESS) 
            {
                errList = Msg_List( Msg_List::Error,
                                    SDBMSG_RUNTIME_PAM_ACCOUNT_MANAGEMENT,
                                    Msg_Arg( SDBMSGTAG_RUNTIME_PAM_ACCOUNT_MANAGEMENT__DESCRIPTION, pam_service ),
                                    Msg_Arg( SDBMSGTAG_RUNTIME_PAM_ACCOUNT_MANAGEMENT__ERRORTEXT, (*RTE_pam_strerror_call.pCall)(pamh, retval) ),
                                    Msg_Arg( SDBMSGTAG_RUNTIME_PAM_ACCOUNT_MANAGEMENT__USERID, userName ) );
           }        
        }
        else
        {
            errList = Msg_List( Msg_List::Error,
                                SDBMSG_RUNTIME_PAM_AUTHENTICATION_FAILED,
                                Msg_Arg( SDBMSGTAG_RUNTIME_PAM_AUTHENTICATION_FAILED__DESCRIPTION, pam_service ),
                                Msg_Arg( SDBMSGTAG_RUNTIME_PAM_AUTHENTICATION_FAILED__ERRORTEXT, (*RTE_pam_strerror_call.pCall)(pamh, retval) ),
                                Msg_Arg( SDBMSGTAG_RUNTIME_PAM_AUTHENTICATION_FAILED__USERID, userName ) );
        }

        (*RTE_pam_end_call.pCall)(pamh, retval);
    }
    else
    {
        errList = Msg_List( Msg_List::Error,
                            SDBMSG_RUNTIME_PAM_START_FAILURE,
                            Msg_Arg( SDBMSGTAG_RUNTIME_PAM_START_FAILURE__DESCRIPTION, pam_service ),
                            Msg_Arg( SDBMSGTAG_RUNTIME_PAM_START_FAILURE__ERRORTEXT, (*RTE_pam_strerror_call.pCall)(pamh, retval) ),
                            Msg_Arg( SDBMSGTAG_RUNTIME_PAM_START_FAILURE__USERID, userName ) );
    }

    if ( PAM_SUCCESS == retval )
    {
        return true;
    }

    sleep(1);

    return false;
}

#endif /* HAS_PAM_SUPPORT */

#define RTE_VERIFY_PASSWORD_CALL "dbmverifypw"

SAPDB_Bool
RTE_VerifyUserPasswordCombination(
    SAPDB_Char const     *userName,
    SAPDB_Char const     *passwd,
    Msg_List &errList)
{
    RTE_SystemRc rc;
    RTESys_ErrorMessage spaceToFill;
    pid_t child;
    int pipeClientInput[2];
    int pipeClientOutput[2];
    char inputPipe[20];
    char outputPipe[20];
    tsp00_Pathc   binPath;
    tsp01_RteError rteError;
    SAPDB_Int4 userId;

#ifdef HAS_PAM_SUPPORT
    SAPDB_Bool pamAvailable = true;
    if ( !RTE_check_pam_authentification_available(pamAvailable, errList) )
    {
        errList.Overrule( Msg_List(Msg_List::Error, 
            SDBMSG_RUNTIME_PAM_AVAILIBILITIY_CHECK, 
            Msg_Arg(SDBMSGTAG_RUNTIME_PAM_AVAILIBILITIY_CHECK__USERID, userName) ) );
        return false;
    }

    if ( pamAvailable )
    {
        return RTE_UsePAMUserPasswordVerification( userName, passwd, errList );
    }
#endif /* HAS_PAM_SUPPORT */

    /* First make sure user exists, to prevent really ugly messages... */
    if ( !RTE_GetUserIdFromUsername(userName, userId, errList) )
    {
        return false;
    }

    if ( !sqlGetDbrootPgmPath( binPath,
                               TERM_WITH_DELIMITER_EO01,
                               &rteError ) )
    {
        rc = RTESys_GetLastError();
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        "Failed to get dependent pgm path",
                                        RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    if ( (strlen(binPath.asCharp()) + strlen(RTE_VERIFY_PASSWORD_CALL)) > sizeof(tsp00_Path) )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        "Path to verify password call executable too long" );
        return false;
    }

    strcat(binPath.asCharp(), RTE_VERIFY_PASSWORD_CALL);

    if ( RTE_save_access(binPath.asCharp(), X_OK) != 0 )
    {
        rc = RTESys_GetLastError();
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        "access to verify command failed",
                                        RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    if ( 0 != pipe(pipeClientInput) )
    {
        rc = RTESys_GetLastError();
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        "input pipe creation for password verification",
                                        RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    if ( 0 != pipe(pipeClientOutput) )
    {
        rc = RTESys_GetLastError();
        close(pipeClientInput[0]);
        close(pipeClientInput[1]);
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        "output pipe creation for password verification",
                                        RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    SAPDB_sprintf(inputPipe, sizeof(inputPipe), "%d", pipeClientInput[0]);
    SAPDB_sprintf(outputPipe, sizeof(outputPipe), "%d", pipeClientOutput[1]);

    child = fork();
    if ( child == (pid_t)-1 )
    {
        rc = RTESys_GetLastError();
        close(pipeClientInput[0]);
        close(pipeClientInput[1]);
        close(pipeClientOutput[0]);
        close(pipeClientOutput[1]);
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        "fork of verification process",
                                        RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    if ( child == (pid_t)0 )
    {
        char *cArgv[5];
        extern char **environ;

        close(pipeClientInput[1]);
        close(pipeClientOutput[0]);
        cArgv[0] = strdup(binPath.asCharp());
        cArgv[1] = strdup(userName);
        cArgv[2] = strdup(inputPipe);
        cArgv[3] = strdup(outputPipe);
        cArgv[4] = 0;
        RTE_save_execve(cArgv[0], cArgv, environ);
        _exit(89);
    }

    close(pipeClientInput[0]);
    close(pipeClientOutput[1]);
    char cryptBuffer[64];   /* Needed for MD5 and blowfish encrytped passwords */
    memset(cryptBuffer, 0, 64);
    memset(cryptBuffer, ' ', 15);

    if ( read(pipeClientOutput[0], cryptBuffer, 64) <= 1 )
    {
        rc = RTESys_GetLastError();
        close(pipeClientInput[1]);
        close(pipeClientOutput[0]);
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        "pipe read for password verification",
                                        RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    char const * GeneratedPassword;
    if ( passwd )
    {
        GeneratedPassword = crypt (passwd, cryptBuffer);
        if ( !GeneratedPassword || !*GeneratedPassword )
        {
            {
                GeneratedPassword = "??";
            }
        }
    }
    else
    {
        /* Empty password is not crypted... */
        GeneratedPassword = passwd;
    }

    if ( write(pipeClientInput[1], GeneratedPassword, strlen(GeneratedPassword) + 1  ) != (strlen(GeneratedPassword) + 1) )
    {
        rc = RTESys_GetLastError();
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        "pipe write for password verification",
                                        RTESys_StrError(rc, spaceToFill) );
        return false;
    }

    close(pipeClientInput[1]);
    close(pipeClientOutput[0]);

    int exitStatus = 0;
    pid_t donePid;

    while ( (donePid = waitpid(child, &exitStatus, 0)) != child )
    {
        if ( donePid == (pid_t)-1
             && errno == EINTR )
        {
           continue;
        }
        else
        {
            exitStatus = -2;
            break;
        }
    }

    if ( exitStatus != 0 )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_PASSWORD_MISMATCH );
        return false;
    }

    return true;
}

/*--------------------*/

SAPDB_Bool
RTE_VerifyUserAsSapdbAdministrator(
    SAPDB_Char const     *userName,
    SAPDB_Char const     *passwd,
    Msg_List &errList)
{
    if ( passwd )
    {
        if ( !RTE_VerifyUserPasswordCombination(userName, passwd, errList) )
        {
            return false;
        }
    }

    return RTE_VerifyUserAsSapdbAdministrator(userName, errList);
}

#endif /* NO_CRYPT */
