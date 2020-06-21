/*!
  @file           RTE_UNIXAuthenticate.h
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
#ifndef RTE_UNIXAUTHENTICATE_H
#define RTE_UNIXAUTHENTICATE_H

#include "RunTime/RTE_Types.h"
#include "SAPDBCommon/SAPDB_C_Cplusplus.h"

/*! @brief Retrieve UNIX user id of SdbOwner

    @param sdbOwnerUserId [out] corresponding sdb owner user id
    @return true if call succeeded, false if failed
 */
externC SAPDB_Bool RTE_GetSapdbOwnerUserId(SAPDB_Int4 VAR_VALUE_REF sdbOwnerUserId);

/*! @brief get user id from user name
    @param userName [in] the user name
    @param userId [out] the user id
    @return true if user exists, false else
 */
externC SAPDB_Bool RTE_GetUserIdFromPasswdByName( SAPDB_Char const *       userName
                                                , SAPDB_Int4 VAR_VALUE_REF userId );

/*! @brief get user id and primary group id from user name
    @param userName [in] the user name
    @param userId [out] the user id
    @param groupId [out] the primary group id
    @return true if user exists, false else
 */
externC SAPDB_Bool RTE_GetUserAndGroupIdFromPasswdByName( SAPDB_Char const *       userName
                                                        , SAPDB_Int4 VAR_VALUE_REF userId
                                                        , SAPDB_Int4 VAR_VALUE_REF groupId );

/*! @brief get user name from user id
    @param userId [in] the user id
    @param userNameBuffer [out] the user name buffer to fill
    @param userNameBufferSize [in] the user name buffer size in bytes (including zero)
    @param neededSize [out] the needed buffer size
    @return true if user exists, false else
 */
externC SAPDB_Bool RTE_GetUsernameFromPasswdById( SAPDB_Int4 const          userId
                                                , SAPDB_Char  *             userNameBuffer
                                                , SAPDB_UInt4               userNameBufferSize
                                                , SAPDB_UInt4 VAR_VALUE_REF neededSize);

/*! @brief get user name from user id
    @param userId [in] the user id
    @param pathBuffer [out] the user name buffer to fill
    @param pathBufferSize [in] the user name buffer size in bytes (including zero)
    @param neededSize [out] the needed buffer size
    @return true if user exists, false else
 */
externC SAPDB_Bool RTE_GetHomeDirectoryFromPasswdById( SAPDB_Int4 const          userId
                                                     , SAPDB_Char  *             pathBuffer
                                                     , SAPDB_UInt4               pathBufferSize
                                                     , SAPDB_UInt4 VAR_VALUE_REF neededSize);

/*! @brief get group id from group name
    @param groupName [in] the group name
    @param groupId [out] the group id
    @return true if group exists, false else
 */
externC SAPDB_Bool RTE_GetGroupIdFromGroupByName( SAPDB_Char const *       groupName
                                                , SAPDB_Int4 VAR_VALUE_REF groupId );

/*! @brief get group name from group id
    @param groupId [in] the group id
    @param groupNameBuffer [out] the group name buffer to fill
    @param groupNameBufferSize [in] the group name buffer size in bytes (including zero)
    @param neededSize [out] the needed buffer size
    @return true if group exists, false else
 */
externC SAPDB_Bool RTE_GetGroupNameFromGroupById( SAPDB_Int4 const          groupId
                                                , SAPDB_Char  *             groupNameBuffer
                                                , SAPDB_UInt4               groupNameBufferSize
                                                , SAPDB_UInt4 VAR_VALUE_REF neededSize);

/*! @brief lookup user in given group member list
    The user is member of the group if his primary group id matches the group id or if he is found
    in the group member list.

    @param userName [in] the user name to look up
    @param primaryGroupId [in] the primary group id
    @param userName [in] the user name to look up
    @param pGroupFound [out] flag set if group was not found
    @return true if user is in group, false else
 */
externC SAPDB_Bool RTE_SearchUserNameInGroupMemberList( SAPDB_Char const *       userName
                                                      , SAPDB_Int4               primaryGroupId
                                                      , SAPDB_Char const *       groupName
                                                      , SAPDB_Bool VAR_VALUE_REF pGroupFound );

/*! @brief Special C interface to determine if the given user is in the SDB administrator group

    @param userId [in] the user id
    @param groupId [in] the group id
    @return true if user is in the administartor group, false else
 */

externC SAPDB_Bool RTE_IsSdbAdministrator           (SAPDB_UInt4 const    userId,
                                                     SAPDB_UInt4 const    groupId);


#endif /* RTE_UNIXAUTHENTICATE_H */
