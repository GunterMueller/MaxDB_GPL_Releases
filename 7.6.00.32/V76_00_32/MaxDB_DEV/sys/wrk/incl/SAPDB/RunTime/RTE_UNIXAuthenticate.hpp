/*!
  @file           RTE_UNIXAuthenticate.hpp
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
#ifndef RTE_UNIXAUTHENTICATE_HPP
#define RTE_UNIXAUTHENTICATE_HPP

#include "RunTime/RTE_Types.hpp"

class SAPDBErr_MessageList;

/*! @brief Retrieve UNIX user id from UNIX user name
    @param username [in] user name
    @param userId [out] corresponding user id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetUserIdFromUsername(SAPDB_Char const *username,
                          SAPDB_Int4       &userId,
                          SAPDBErr_MessageList &errList);

/*! @brief Retrieve UNIX user name from UNIX user id
    @param userId [in] user id
    @param usernameBuffer [out] space for user name
    @param usernameBufferSize [in] number of bytes writable in usernameBuffer
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetUsernameFromUserId(SAPDB_Int4  userId,
                          SAPDB_Char *usernameBuffer,
                          SAPDB_Int4  usernameBufferSize,
                          SAPDBErr_MessageList &errList);

/*! @brief Retrieve UNIX group id from UNIX group name
    @param Groupname [in] group name
    @param groupId [out] corresponding group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetGroupIdFromGroupname( SAPDB_Char const *Groupname,
                             SAPDB_Int4       &groupId,
                             SAPDBErr_MessageList &errList);

/*! @brief Retrieve UNIX group name from UNIX group id
    @param groupId [in] group id
    @param groupnameBuffer [out] space for group name
    @param groupnameBufferSize [in] number of bytes writable in groupnameBuffer
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetGroupnameFromGroupId(SAPDB_Int4  groupId,
                            SAPDB_Char *groupnameBuffer,
                            SAPDB_Int4  groupnameBufferSize,
                            SAPDBErr_MessageList &errList);

/*! @brief Set effective and real user to given UNIX user and group ids
    Call is used in root (or setuid root) programs mainly to set the 'correct' user/group or
    in normal programs to check that the right user is logged in.
    @param userId [in] user id
    @param groupId [in] group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_SetUserAndGroupId(SAPDB_Int4 userId,
                      SAPDB_Int4 groupId,
                      SAPDBErr_MessageList &errList);

/*! @brief Set effective and real user to SapdbOwner/SapdbGroup.
    Call is used in root (or setuid root) programs (x_server/dbmsrv/x_start/x_stop) to set
    the 'correct' user/group which was setup during installation

    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_SetSapdbOwnerAndGroupId(SAPDBErr_MessageList &errList);

/*! @brief Get the effective UNIX user and group id
    The effectiv user and group is used to set the owner of created files etc.
    @param userId [in] user id
    @param groupId [in] group id
 */
void
RTE_GetEffectivUserIdAndGroupId(SAPDB_Int4 &userId, SAPDB_Int4 &groupId);

/*! @brief Get the real UNIX user and group id
    The real user and group differs only if the executable was a setuid or setgid program.
    @param userId [in] user id
    @param groupId [in] group id
 */
void
RTE_GetRealUserIdAndGroupId(SAPDB_Int4 &userId, SAPDB_Int4 &groupId);

/*! @brief Check to see if effectiv user is or is allowed to become SAPDB owner user
    The effectiv user and group is checked against the SAPDB owner user name which is found in UNIX registration.
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_IsSapdbOwnerOrRoot(SAPDBErr_MessageList &errList);

/*! @brief Get the Default SAPDB owner and group ids
    The SAPDB owner user name and SAPDB group name are found in UNIX registration and converted into ids.
    @param userId [out] user id
    @param groupId [out] group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetDefaultSapdbUserAndGroupIds(SAPDB_Int4 &userId,
                                   SAPDB_Int4 &groupId,
                                   SAPDBErr_MessageList &errList);

/*! @brief Get the Default SAPDB owner and SAPDB support group ids using installation root path 
    The SAPDB owner user name is found in UNIX registration, the support group is retrieved from kernel
    executable files group id. The kernel executable is search as 'dbroot'/pgm/kernel
    @param dbroot [in] path to installation dependend root
    @param userId [out] user id
    @param groupId [out] group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetDefaultOwnerAndSupportGroupFromDbroot( SAPDB_Char const *dbroot,
                                              SAPDB_Int4 &userId,
                                              SAPDB_Int4 &groupId,
                                              SAPDBErr_MessageList &errList);

/*! @brief Get the Default SAPDB owner and SAPDB support group ids using database name
    The SAPDB owner user name is found in UNIX registration, the support group is retrieved from
    user profile container files group id. This allows to have separate support groups for each database
    instance.
    @param dbname [in] database instance name
    @param userId [out] user id
    @param groupId [out] group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetOwnerAndSupportGroupIdFromDatabaseName( SAPDB_Char const *dbname,
                                               SAPDB_Int4 &userId, 
                                               SAPDB_Int4 &groupId, 
                                               SAPDBErr_MessageList &errList);

/*! @brief Set the UNIX file owner and group for given path using owner and group ids
    @param pathname [in] the path
    @param userId [in] user id
    @param groupId [in] group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_SetFileOwnerAndGroup(SAPDB_Char const *pathname,
                         SAPDB_Int4 userId,
                         SAPDB_Int4 groupId,
                         SAPDBErr_MessageList &errList);

/*! @brief Set the UNIX file owner and group for given path using owner and group names
    @param pathname [in] the path
    @param username [in] user id
    @param Groupname [in] group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_SetFileOwnerAndGroup(SAPDB_Char const *pathname,
                         SAPDB_Char const *username,
                         SAPDB_Char const *Groupname,
                         SAPDBErr_MessageList &errList);

/*! @brief Get the UNIX file owner and group for given path as owner and group ids
    @param pathname [in] the path
    @param userId [out] user id
    @param groupId [out] group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetFileOwnerAndGroup(SAPDB_Char const *pathname,
                         SAPDB_Int4 &userId,
                         SAPDB_Int4 &groupId,
                         SAPDBErr_MessageList &errList);

/*! @brief Get the UNIX file owner and group for given path handle as owner and group ids
    @param handle [in] a open path handle
    @param userId [out] user id
    @param groupId [out] group id
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetFileOwnerAndGroup(RTE_FileHandle handle,
                         SAPDB_Int4 &userId,
                         SAPDB_Int4 &groupId,
                         SAPDBErr_MessageList &errList);

/*! @brief Get the UNIX file owner and group for given path as owner and group names
    @param pathname [in] the path
    @param usernameBuffer [out] user name buffer
    @param usernameBufferSize [in] number of bytes writable in usernameBuffer
    @param groupnameBuffer [out] group name buffer
    @param groupnameBufferSize [in] number of bytes writable in groupnameBuffer
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetFileOwnerAndGroup(SAPDB_Char const *pathname,
                         SAPDB_Char *usernameBuffer,
                         SAPDB_Int4  usernameBufferSize,
                         SAPDB_Char *groupnameBuffer,
                         SAPDB_Int4  groupnameBufferSize,
                         SAPDBErr_MessageList &errList);

/*! @brief Get the UNIX file owner and group for given path handle as owner and group names
    @param handle [in] a open path handle
    @param usernameBuffer [out] user name buffer
    @param usernameBufferSize [in] number of bytes writable in usernameBuffer
    @param groupnameBuffer [out] group name buffer
    @param groupnameBufferSize [in] number of bytes writable in groupnameBuffer
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_GetFileOwnerAndGroup(RTE_FileHandle handle,
                         SAPDB_Char *usernameBuffer,
                         SAPDB_Int4  usernameBufferSize,
                         SAPDB_Char *groupnameBuffer,
                         SAPDB_Int4  groupnameBufferSize,
                         SAPDBErr_MessageList &errList);

/*! @brief Verify that given UNIX user name is member of SAPDB administation group
    The SAPDB administration group membership verifies that creating a database is allowed.
    The group name is retrieved from registry.
    @param username [in] user name
    @param errList [out] message list filled in case of problems
    @return true if user is administrator, false if not (errList filled)
 */
SAPDB_Bool
RTE_VerifyUserAsSapdbAdministrator(
    SAPDB_Char const *username,
    SAPDBErr_MessageList &errList);


/*! @brief Verify that given UNIX user is member of SAPDB administation group
    The SAPDB administration group membership verifies that creating a database is allowed.
    The group name is retrieved from registry. The primary group id and the current process
    group list are used to verify that user is member of SdbAdministrator group.

    @param userId [in] user id
    @param primaryGroupId [in] group id
    @param errList [out] message list filled in case of problems
    @return true if user is administrator, false if not (errList filled)
 */
SAPDB_Bool
RTE_VerifyUserIdGroupIdCombinationAsSapdbAdministrator(
    SAPDB_Int4            userId,
    SAPDB_Int4            primaryGroupId,
    SAPDBErr_MessageList &errList);

/*! @brief Verify current process is authorized as member of SAPDB administation group
    The SAPDB administration group membership verifies that creating a database is allowed.
    The group name is retrieved from registry. The real group id and the current process
    group list are used to verify that user is member of SdbAdministrator group.
    A real superuser is always veriffied as SdbAdministrator too.

    @param errList [out] message list filled in case of problems
    @return true if user is administrator, false if not (errList filled)
 */
SAPDB_Bool
RTE_VerifyAsSapdbAdministrator(
    SAPDBErr_MessageList &errList);

/*! @brief Verify given UNIX user name password combination and check if member of SAPDB administation group
    The SAPDB administration group membership verifies that creating a database is allowed.
    The group name is retrieved from registry. 

    The password check is only allowed to super user, so the caller implicitly uses a setuid root program
    to verify the password. The usage of this routine is limited to 'SapdbOwner' and 'root' user.

    @param username [in] user name
    @param passwd [in] the crypted password to check
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_VerifyUserAsSapdbAdministrator(
    SAPDB_Char const     *username,
    SAPDB_Char const     *passwd,
    SAPDBErr_MessageList &errList);

/*! @brief Verify given UNIX user name password combination

    The password check is only allowed to super user, so the caller implicitly uses a setuid root program
    to verify the password. The usage of this routine is limited to 'SapdbOwner' and 'root' user.

    @param username [in] user name
    @param passwd [in] the crypted password to check
    @param errList [out] message list filled in case of problems
    @return true if call succeeded, false if failed (errList filled)
 */
SAPDB_Bool
RTE_VerifyUserPasswordCombination(
    SAPDB_Char const     *username,
    SAPDB_Char const     *passwd,
    SAPDBErr_MessageList &errList);
#endif /* RTE_UNIXAUTHENTICATE_HPP */
