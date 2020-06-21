/*!******************************************************************************

  @file         hak103.h

  ------------------------------------------------------------------------------

  @author       ThomasA

  @brief        CatalogWrapper

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

********************************************************************************/

#ifndef HAK103_H
#define HAK103_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xa5\xbb\x6d\x09\xb9\xe5\xf3\xf4\x71\x2e\xa3\xfc\x70\x5b\x18\x5e'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gak00.h"
#include "vak001.h"
#include "gak40.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a103AllRoles (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103AllUserStatistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CallCatalogCacheDeleteTrigger (
    tsp00_TaskId                         TaskId,
    integer                              FuncIdx,
    tgg00_Rec             VAR_VALUE_REF  BeforeImage,
    tgg00_BasisError      VAR_VALUE_REF  e);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CallCatalogCacheInsertTrigger (
    tsp00_TaskId                         TaskId,
    integer                              FuncIdx,
    tgg00_Rec             VAR_VALUE_REF  AfterImage);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CallCatalogCacheUpdateTrigger (
    tsp00_TaskId                         TaskId,
    integer                              FuncIdx,
    tgg00_Rec             VAR_VALUE_REF  BeforeImage,
    tgg00_Rec             VAR_VALUE_REF  AfterImage);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CatalogCacheInvalidateTable (
    tsp00_TaskId                         TaskId,
    tgg00_Surrogate       VAR_ARRAY_REF  TableId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CatalogSchemaId (
    tgg00_Surrogate       VAR_ARRAY_REF  catalogSchemaId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CheckCreateInPrivilege (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  schemaName,
    integer                              errorPos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CheckDropInPrivilege (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  schemaName,
    integer                              errorPos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CheckAlterInPrivilege (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  schemaName,
    integer                              errorPos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a103CreateCatalog ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103CreateContainer (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  SchemaId,
    integer                              ContainerNo,
    tsp00_C16             VAR_ARRAY_REF  Guid,
    integer                              NoOfKeyFiles,
    pasbool                              IsVarContainer,
    integer                              Size,
    tsp00_C8              VAR_ARRAY_REF  FileNo,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  ClassName);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103DeleteAuthorizationObject (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  authName);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103DropSchemaContainers (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  schemaId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tgg00_VoidPtr a103ExistsUser (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  userName);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tgg00_VoidPtr a103ExistsSchema (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  schemaName);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103AttachCatalogRecord (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  key,
    tgg00_VoidPtr         VAR_VALUE_REF  pHandle,
    tak_sysbufferaddress  VAR_VALUE_REF  p,
    tgg00_BasisError      VAR_VALUE_REF  e);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tak00_colinfo_ptr a103GetColumn (
    tak_baserecord        VAR_VALUE_REF  BaseRec,
    integer                              ColIndex);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tak00_colinfo_ptr a103GetLastKeyColumn (
    tak_baserecord        VAR_VALUE_REF  BaseRec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103GetSchemaId (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  schemaName,
    integer                              errorPos,
    tgg00_Surrogate       VAR_ARRAY_REF  schemaId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103GetSchemaName (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_Surrogate  VAR_ARRAY_REF  schemaId,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schemaName);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a103GetTriggerId (
    tsp00_KnlIdentifier   VAR_ARRAY_REF  TableName);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a103GrantRevokeSchemaPrivilege (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              isGrant,
    pasbool                              alterIn,
    pasbool                              createIn,
    pasbool                              dropIn,
    integer                              cntSchemas,
    tsp00_KnlIdentifierPtr                 pSchemas,
    integer                              cntUsers,
    tsp00_KnlIdentifierPtr                 pUser);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a103HandleInvalid (
    tgg00_VoidPtr                        pHandle);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103LoadAuthorizationById (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  authId,
    void                  *  buf,
    tgg00_BasisError      VAR_VALUE_REF  e);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103LoadAuthorizationByName (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  name,
    pasbool                              useCache,
    void                  *  buf,
    tgg00_BasisError      VAR_VALUE_REF  e);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103LoadAuthorizationBySchemaId (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int4                           schemaId,
    tgg00_LockReqMode_Param                 lockRequest,
    void                  *  buf,
    tgg00_BasisError      VAR_VALUE_REF  e);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103LockAuthorizationObject (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  authid,
    pasbool                              lockExclusive);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103MigrateCatalog ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103NextUser (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  userId,
    void                  *  buf,
    pasbool                              userGroup,
    pasbool                              nonUserGroup,
    tgg00_BasisError      VAR_VALUE_REF  e);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103ReleaseCacheHandle (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_VoidPtr                        Handle,
    pasbool                              DecRefCount);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103RenameAuthorizationObject (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  oldName,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  newName);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103RestartCatalog ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a103RestoreMessBlockFieldSequences (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  Key,
    tak_messblock_record  VAR_VALUE_REF  MBlock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103SetCacheHandleInUse (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_VoidPtr                        Handle,
    pasbool                              InUse);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103StoreCatalogObject (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 pObject,
    pasbool                              doUpdate);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a103StoreMessBlockFieldSequences (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  Key,
    tgg00_MessBlock       VAR_VALUE_REF  MBlock,
    integer                              MaxStoredKey);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103ToUnicode (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103UpdateGroupMembers (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 pGroup);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a103UpdateUserOwner (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  OldOwnerId,
    tgg00_Surrogate       VAR_ARRAY_REF  NewOwnerId,
    pasbool                              GroupsOnly);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

