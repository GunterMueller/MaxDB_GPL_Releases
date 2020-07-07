/*


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



*/

#include "hak103.h"
#include "hak362.h"
#include "hak42.h"
#include "hak43.h"
#include "hak10.h"
#include "SQLManager/Catalog/Catalog_IAuthorization.hpp"
#include "SQLManager/Catalog/Catalog_Interface.hpp"
#include "SQLManager/Catalog/Catalog_Instance.hpp"
#include "SQLManager/Catalog/Catalog_ICallBack.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SQLManager/Catalog/Catalog_ContainerObject.hpp"
#include "SQLManager/Catalog/Catalog_FieldSequenceMarshaler.hpp"
#include "liveCache/LVC_ContainerManager.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Persistence.hpp"

/*!
   @brief defines the callbacks called by Catalog_IAuthorization::ForEach. 
*/

namespace Catalog_PascalWrapper_Callbacks {

    /*!
       @brief callback for roles
     */
    class RolesCallback : public Catalog_ICallBack
    {
    public :
        /*!
           @brief constructor
         */
        RolesCallback(tak_all_command_glob& acv, tak40_show_glob& a41v)
            : m_acv(acv)
            , m_showGlob(a41v)
        {
        }
        /*!
          @brief callback method
         */
        virtual bool Callback(Catalog_Object* p);
    private :
        tak_all_command_glob& m_acv;
        tak40_show_glob&      m_showGlob;
    };

    /*!
       @brief callback for user statistics
     */
    class StatisticsCallback : public Catalog_ICallBack
    {
    public :
        /*!
           @brief constructor
         */
        StatisticsCallback(tak_all_command_glob& acv, tak40_show_glob& a41v)
            : m_acv(acv)
            , m_showGlob(a41v)
        {
        }
        /*!
          @brief callback method
         */
        virtual bool Callback(Catalog_Object* p);
    private :
        tak_all_command_glob& m_acv;
        tak40_show_glob&      m_showGlob;
    };
    /*!
       @brief callback for unicode migration
     */
    class ToUnicodeCallback : public Catalog_ICallBack
    {
    public :
        /*!
           @brief constructor
         */
        ToUnicodeCallback(tak_all_command_glob& acv)
            : m_acv(acv)
        {
        }
        /*!
          @brief callback method
         */
        virtual bool Callback(Catalog_Object* p);
    private :
        tak_all_command_glob& m_acv;
    };
};

using namespace Catalog_PascalWrapper_Callbacks;

// constants

static bool c_eachUser   = true;
static bool c_eachSchema = true;
static bool c_eachRole   = true;

//--------------------------------------------------------------------------

bool StatisticsCallback::Callback(Catalog_Object* p)
{
    a42UserStatisticsCallback (m_acv, m_showGlob, p);
    return (0 == m_acv.a_returncode);
}

//--------------------------------------------------------------------------

bool RolesCallback::Callback(Catalog_Object* p)
{
    a43AllRolesCallback (m_acv, m_showGlob, p);
    return (0 == m_acv.a_returncode);
}

//--------------------------------------------------------------------------

bool ToUnicodeCallback::Callback(Catalog_Object* p)
{
    SQLMan_Context& context = SQLMan_Context::AcvToContext(m_acv);
    tgg00_BasisError e = e_ok;
    a362AuthorizationConvertPasswords (*REINTERPRET_CAST(tak_userrecord*, p), e);
    if (e_ok == e)
    {
        const bool c_doUpdate = true;
        Catalog_IAuthorization::GetInstance().UpdatePasswords (context, REINTERPRET_CAST(Catalog_AuthorizationObject*, p));
    }
    else
    {
        context.ThrowError (e);
    }
    return (0 == m_acv.a_returncode);
}

//--------------------------------------------------------------------------

externPascal void a103AttachCatalogRecord (
     tak_all_command_glob        VAR_VALUE_REF  acv,
     tgg00_SysInfoKey            VAR_VALUE_REF  key,
     tgg00_VoidPtr               VAR_VALUE_REF  pHandle,
     tak_sysbufferaddress        VAR_VALUE_REF  p,
     tgg00_BasisError            VAR_VALUE_REF  e)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103AttachCatalogRecord", Catalog_Trace, 1);
    const Catalog_Object* pObj;
    bool                  objectExists;
    const bool            cNoWait = true;
    const Catalog_Cache::Catalog_ICacheHandle* pH =
         Catalog_Cache::Access (SQLMan_Context::AcvToContext(acv), *reinterpret_cast<Catalog_Object::Catalog_Key*>(&key), !cNoWait, pObj, objectExists);
    pHandle = reinterpret_cast<tgg00_VoidPtr>(const_cast<Catalog_Cache::Catalog_ICacheHandle*>(pH));
    p       = reinterpret_cast<tak_sysbufferaddress>(const_cast<Catalog_Object*>(pObj));
    if (!objectExists)
    {
        e = e_sysinfo_not_found;
    }
}

//--------------------------------------------------------------------------

void a103AllRoles (
                   tak_all_command_glob  VAR_VALUE_REF  acv,
                   tak40_show_glob       VAR_VALUE_REF  a41v)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103AllRoles", Catalog_Trace, 1);
    RolesCallback callback(acv, a41v);
    Catalog_IAuthorization::GetInstance().ForEach (SQLMan_Context::AcvToContext(acv), callback, !c_eachUser, !c_eachSchema, c_eachRole);
}

//--------------------------------------------------------------------------

void a103AllUserStatistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103AllUserStatistics", Catalog_Trace, 1);
    StatisticsCallback callback(acv, a41v);
    Catalog_IAuthorization::GetInstance().ForEach (SQLMan_Context::AcvToContext(acv), callback, c_eachUser, !c_eachSchema, !c_eachRole);
}

//--------------------------------------------------------------------------

void a103CallCatalogCacheDeleteTrigger (
    tsp00_TaskId                         TaskId,
    integer                              FuncIdx,
    tgg00_Rec             VAR_VALUE_REF  BeforeImage,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    e = Catalog_Cache::CallDeleteTrigger(TaskId, FuncIdx - 1, BeforeImage);
}

//--------------------------------------------------------------------------

void a103CallCatalogCacheInsertTrigger (
    tsp00_TaskId                         TaskId,
    integer                              FuncIdx,
    tgg00_Rec             VAR_VALUE_REF  AfterImage)
{
    Catalog_Cache::CallInsertTrigger(TaskId, FuncIdx - 1, AfterImage);
}

//--------------------------------------------------------------------------

externPascal void a103CallCatalogCacheUpdateTrigger (
    tsp00_TaskId                         TaskId,
    integer                              FuncIdx,
    tgg00_Rec             VAR_VALUE_REF  BeforeImage,
    tgg00_Rec             VAR_VALUE_REF  AfterImage)
{
    Catalog_Cache::CallUpdateTrigger(TaskId, FuncIdx - 1, BeforeImage, AfterImage);
}

//--------------------------------------------------------------------------

void a103DropSchemaContainers (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  schemaId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103DropSchemaContainers", Catalog_Trace, 1);
    LVC_ContainerManager::DropSchemaContainers (SQLMan_Context::AcvToContext(acv), schemaId);
}

//--------------------------------------------------------------------------

tak00_colinfo_ptr a103GetColumn (
    tak_baserecord        VAR_VALUE_REF  BaseRec,
    integer                              ColIndex)
{
    SAPDBTRACE_WRITELN( Catalog_Trace, 10, "ColIndex   " << ColIndex);
    SAPDBTRACE_WRITELN( Catalog_Trace, 10, "SplitIndex " << BaseRec.bcolumnSplitIdx);
    SAPDBTRACE_WRITELN( Catalog_Trace, 10, "Extension  " << ToStr(BaseRec.bcolumnExtension));
    if (ColIndex > BaseRec.bcolumnSplitIdx)
    {
        return (*BaseRec.bcolumnExtension)[ColIndex - BaseRec.bcolumnSplitIdx - 1];
    }
    else
    {
        return BaseRec.bcolumn()[ColIndex - 1];
    }
}

//--------------------------------------------------------------------------

tak00_colinfo_ptr a103GetLastKeyColumn (
    tak_baserecord        VAR_VALUE_REF  BaseRec)
{
    return a103GetColumn (BaseRec, BaseRec.blastkeyind);
}

//--------------------------------------------------------------------------

void a103CheckCreateInPrivilege (
    tak_all_command_glob       VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  schemaName,
    integer                                   errorPos)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103CheckCreateInPrivilege", Catalog_Trace, 1);
    Catalog_IAuthorization::GetInstance().CheckCreateInPrivilege(SQLMan_Context::AcvToContext(acv), schemaName, errorPos);
}

//--------------------------------------------------------------------------

void a103CheckDropInPrivilege (
    tak_all_command_glob       VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  schemaName,
    integer                                   errorPos)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103CheckDropInPrivilege", Catalog_Trace, 1);
    Catalog_IAuthorization::GetInstance().CheckDropInPrivilege(SQLMan_Context::AcvToContext(acv), schemaName, errorPos);
}

//--------------------------------------------------------------------------

void a103CheckAlterInPrivilege (
    tak_all_command_glob       VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  schemaName,
    integer                                   errorPos)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103CheckAlterInPrivilege", Catalog_Trace, 1);
    Catalog_IAuthorization::GetInstance().CheckAlterInPrivilege(SQLMan_Context::AcvToContext(acv), schemaName, errorPos);
}

//--------------------------------------------------------------------------

void a103DeleteAuthorizationObject (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  authName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103DeleteAuthorizationObject", Catalog_Trace, 1);
    Catalog_IAuthorization::GetInstance().DeleteAuthorizationObject (SQLMan_Context::AcvToContext(acv), authName);
}

//--------------------------------------------------------------------------

tgg00_VoidPtr a103ExistsUser (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier   VAR_ARRAY_REF  userName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103ExistsUser", Catalog_Trace, 1);
    return Catalog_IAuthorization::GetInstance().ExistsUser(SQLMan_Context::AcvToContext(acv), userName);
}

//--------------------------------------------------------------------------

tgg00_VoidPtr a103ExistsSchema (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier   VAR_ARRAY_REF  schemaName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103ExistsUser", Catalog_Trace, 1);
    return Catalog_IAuthorization::GetInstance().ExistsSchema(SQLMan_Context::AcvToContext(acv), schemaName);
}

//--------------------------------------------------------------------------

void a103GetSchemaId (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier   VAR_ARRAY_REF  schemaName,
    integer                              errorPos,
    tgg00_Surrogate       VAR_ARRAY_REF  schemaId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103GetSchemaId", Catalog_Trace, 1);
    schemaId = Catalog_IAuthorization::GetInstance().GetSchemaId (SQLMan_Context::AcvToContext(acv), schemaName, errorPos);
}

//--------------------------------------------------------------------------

void a103GetSchemaName (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_Surrogate  VAR_ARRAY_REF  schemaId,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schemaName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103GetSchemaName", Catalog_Trace, 1);
    schemaName = Catalog_IAuthorization::GetInstance().GetSchemaName (SQLMan_Context::AcvToContext(acv), schemaId);
}

//--------------------------------------------------------------------------

integer a103GetTriggerId (
    tsp00_KnlIdentifier   VAR_ARRAY_REF  TableName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103GetTriggerId", Catalog_Trace, 1);
    return Catalog_Cache::GetTriggerId(TableName);
}

//--------------------------------------------------------------------------

integer a103GrantRevokeSchemaPrivilege (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              isGrant,
    pasbool                              alterIn,
    pasbool                              createIn,
    pasbool                              dropIn,
    integer                              cntSchemas,
    tsp00_KnlIdentifierPtr               pSchemas,
    integer                              cntUsers,
    tsp00_KnlIdentifierPtr               pUser)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103GrantRevokeSchemaPrivilege", Catalog_Trace, 1);
    return Catalog_IAuthorization::GetInstance().GrantRevokeSchemaPrivilege (SQLMan_Context::AcvToContext(acv),
        isGrant > 0, alterIn > 0, createIn > 0, dropIn > 0, cntSchemas, pSchemas, cntUsers, pUser); 
}

//--------------------------------------------------------------------------

pasbool a103HandleInvalid (tgg00_VoidPtr pHandle)
{
     SAPDBTRACE_ROUTINE_DEBUG ("a103HandleInvalid", Catalog_Trace, 1);
    return Catalog_Cache::HandleInvalid(reinterpret_cast<Catalog_Cache::Catalog_CacheHandle*>(pHandle));
}

//--------------------------------------------------------------------------

void a103CatalogCacheInvalidateTable (
    tsp00_TaskId                         TaskId,
    tgg00_Surrogate       VAR_ARRAY_REF  TableId)
{
     SAPDBTRACE_ROUTINE_DEBUG ("a103CatalogCacheInvalidateTable", Catalog_Trace, 1);
     Catalog_Cache::InvalidateTable (TaskId, TableId);
}

//--------------------------------------------------------------------------

void a103LoadAuthorizationById (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  authId,
    void                  *  buf,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103LoadAuthorizationById", Catalog_Trace, 1);
    if (Catalog_IAuthorization::GetInstance().LoadAuthorizationObject (SQLMan_Context::AcvToContext(acv), authId, buf))
    {
        e = e_ok;
    }
    else
    {
        e = e_sysinfo_not_found;
    }
}

//--------------------------------------------------------------------------

externPascal void a103LoadAuthorizationByName (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  name,
    pasbool                              useCache,
    void                              *  buf,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103LoadAuthorizationByName", Catalog_Trace, 1);
    if (Catalog_IAuthorization::GetInstance().LoadAuthorizationObject (SQLMan_Context::AcvToContext(acv), name, useCache != 0, buf))
    {
        e = e_ok;
    }
    else
    {
        e = e_sysinfo_not_found;
    }
}

//--------------------------------------------------------------------------

void a103LoadAuthorizationBySchemaId (
                                      tak_all_command_glob  VAR_VALUE_REF  acv,
                                      tsp00_Int4              schemaId,
                                      tgg00_LockReqMode_Param lockRequest,
                                      void                  *  buf,
                                      tgg00_BasisError      VAR_VALUE_REF  e)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103LoadAuthorizationBySchemaId", Catalog_Trace, 1);
    Catalog_IAuthorization::LockRequest request =  Catalog_IAuthorization::LOCK_NONE;
    switch (lockRequest)
    {
    case lckRowShare_egg00 :
        request =  Catalog_IAuthorization::LOCK_SHARE;
        break;
    case lckRowExcl_egg00 :
       request =  Catalog_IAuthorization::LOCK_EXCLUSIVE;
    }
    if (Catalog_IAuthorization::GetInstance().LoadAuthorizationObject (SQLMan_Context::AcvToContext(acv), schemaId, request, buf))
    {
        e = e_ok;
    }
    else
    {
        e = e_sysinfo_not_found;
    }
}

//--------------------------------------------------------------------------

void a103MigrateCatalog ()
{
     SAPDBTRACE_ROUTINE_DEBUG ("a103MigrateCatalog", Catalog_Trace, 1);
     Catalog_Instance::Instance().Migrate();
}

//--------------------------------------------------------------------------

void a103NextUser (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  userId,
    void                  *  buf,
    pasbool                              userGroup,
    pasbool                              nonUserGroup,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103NextUser", Catalog_Trace, 1);
    if (Catalog_IAuthorization::GetInstance().NextAuthorizationObject (SQLMan_Context::AcvToContext(acv), 
        userId, userGroup != 0, nonUserGroup != 0, buf))
    {
        e = e_ok;
    }
    else
    {
        e = e_no_next_record;
    }
}

//--------------------------------------------------------------------------

void a103LockAuthorizationObject (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  authid,
    pasbool                              lockExclusive)
{
     SAPDBTRACE_ROUTINE_DEBUG ("a103LockAuthorizationObject", Catalog_Trace, 1);
     Catalog_IAuthorization::GetInstance().Lock (SQLMan_Context::AcvToContext(acv), authid, lockExclusive != 0);
}

//--------------------------------------------------------------------------

void a103ReleaseCacheHandle (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_VoidPtr                        Handle,
    pasbool                              DecRefCount)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103ReleaseCacheHandle", Catalog_Trace, 1);
    Catalog_Cache::Release(SQLMan_Context::AcvToContext(acv), reinterpret_cast<Catalog_Cache::Catalog_CacheHandle*>(Handle), DecRefCount != 0);
}

//--------------------------------------------------------------------------

void a103RenameAuthorizationObject (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  oldName,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  newName)
{
     SAPDBTRACE_ROUTINE_DEBUG ("a103RenameAuthorizationObject", Catalog_Trace, 1);
     Catalog_IAuthorization::GetInstance().Rename (SQLMan_Context::AcvToContext(acv), oldName, newName);
}

//--------------------------------------------------------------------------

integer a103RestoreMessBlockFieldSequences (
     tak_all_command_glob   VAR_VALUE_REF  acv,
     tgg00_SysInfoKey       VAR_VALUE_REF  Key,
     tak_messblock_record   VAR_VALUE_REF  MBlock)
{
    SQLMan_Context&        context      = SQLMan_Context::AcvToContext(acv);
    Catalog_ISessionCache& sessionCache = context.GetSessionCatalogCache();
    Catalog_FieldSequenceMarshaler serializer(*reinterpret_cast<Catalog_SessionCache*>(&sessionCache), 
        *reinterpret_cast<Catalog_Object::Catalog_Key*>(&Key), 0);
    if (MBlock.mbr_fieldSequenceInfo.includes(mfs_literals))
    {
        SAPDBFields_SequenceMemoryCopy* pCopy = 
            SAPDBFields_Persistence::RestoreAsSequenceMemoryCopy (serializer, context.GetAllocator(), 
                SAPDBFields_MemorySequence::sharedCount, context.GetErrorList());
        MBlock.mbr_mess_block.mb_fieldlists()[cgg_idx_literal_valuefieldlist] = reinterpret_cast<tsp00_MoveObjPtr>(pCopy);
    }
    if (MBlock.mbr_fieldSequenceInfo.includes(mfs_parameters) && context.IsOk())
    {
        SAPDBFields_SequenceMemoryCopy* pCopy = 
            SAPDBFields_Persistence::RestoreAsSequenceMemoryCopy (serializer, context.GetAllocator(), 
                SAPDBFields_MemorySequence::sharedCount, context.GetErrorList());
        MBlock.mbr_mess_block.mb_fieldlists()[cgg_idx_param_valuefieldlist] = reinterpret_cast<tsp00_MoveObjPtr>(pCopy);
    }
    if (MBlock.mbr_fieldSequenceInfo.includes(mfs_pars_result) && context.IsOk())
    {
        SAPDBFields_SequenceMemoryCopy* pCopy = 
            SAPDBFields_Persistence::RestoreAsSequenceMemoryCopy (serializer, context.GetAllocator(), 
                SAPDBFields_MemorySequence::sharedCount, context.GetErrorList());
        MBlock.mbr_mess_block.mb_fieldlists()[cgg_idx_pars_result_valuefieldlist] = reinterpret_cast<tsp00_MoveObjPtr>(pCopy);
    }
    if (MBlock.mbr_fieldSequenceInfo.includes(mfs_ex_result) && context.IsOk())
    {
        SAPDBFields_SequenceMemoryCopy* pCopy = 
            SAPDBFields_Persistence::RestoreAsSequenceMemoryCopy (serializer, context.GetAllocator(), 
                SAPDBFields_MemorySequence::sharedCount, context.GetErrorList());
        MBlock.mbr_mess_block.mb_fieldlists()[cgg_idx_ex_result_valuefieldlist] = reinterpret_cast<tsp00_MoveObjPtr>(pCopy);
    }
    return serializer.GetReadRows();
}

//--------------------------------------------------------------------------

integer a103StoreMessBlockFieldSequences (
     tak_all_command_glob  VAR_VALUE_REF  acv,
     tgg00_SysInfoKey      VAR_VALUE_REF  Key,
     tgg00_MessBlock       VAR_VALUE_REF  MBlock,
     integer                              MaxStoredKey)
{

    SQLMan_Context&        context = SQLMan_Context::AcvToContext(acv);
    Catalog_ISessionCache& sessionCache = context.GetSessionCatalogCache();
    Catalog_FieldSequenceMarshaler serializer (*reinterpret_cast<Catalog_SessionCache*>(&sessionCache),
        *reinterpret_cast<Catalog_Object::Catalog_Key*>(&Key), MaxStoredKey);
    for (int ix = 0; ix <= cgg_idx_max_valuefieldlist; ++ix)
    {
        SAPDBFields_IFieldSequence* pSequence = reinterpret_cast<SAPDBFields_IFieldSequence*>(MBlock.mb_fieldlists()[ix]);
        if (pSequence)
        {
            if (!SAPDBFields_Persistence::Store (*pSequence, serializer, context.GetErrorList()))
            {
                break;
            }
        }
    }
    serializer.Flush();
    return serializer.GetWrittenRows();
}

//--------------------------------------------------------------------------

void a103SetCacheHandleInUse (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_VoidPtr                        Handle,
    pasbool                              InUse)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103SetCacheHandleInUse", Catalog_Trace, 1);
    Catalog_Cache::SetHandleInUse (SQLMan_Context::AcvToContext(acv), reinterpret_cast<Catalog_Cache::Catalog_CacheHandle*>(Handle), InUse != 0);
}

//--------------------------------------------------------------------------

void a103StoreCatalogObject (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 pObject,
    pasbool                              doUpdate)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103StoreCatalogObject", Catalog_Trace, 1);
    REINTERPRET_CAST(Catalog_Object*, pObject)->Outcopy(SQLMan_Context::AcvToContext(acv), (doUpdate > 0));
}

//--------------------------------------------------------------------------

void a103UpdateUserOwner (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  OldOwnerId,
    tgg00_Surrogate       VAR_ARRAY_REF  NewOwnerId,
    pasbool                              GroupsOnly)
 {
    SAPDBTRACE_ROUTINE_DEBUG ("a103UpdateUserOwner", Catalog_Trace, 1);
    Catalog_IAuthorization::GetInstance().UpdateUserOwner(SQLMan_Context::AcvToContext(acv), OldOwnerId, NewOwnerId, GroupsOnly != 0);
 }

//--------------------------------------------------------------------------

void a103CatalogSchemaId (
    tgg00_Surrogate       VAR_ARRAY_REF  catalogSchemaId)
{
    catalogSchemaId = Catalog_Interface::GetInstance().GetCatalogSchemaId();
}

//--------------------------------------------------------------------------

integer a103CreateCatalog ()
{
    return Catalog_Instance::Instance().CreateCatalogTables();
}

//-------------------------------------------------------------------------------------------------

void a103RestartCatalog ()
{
    if (Catalog_Instance::Instance().Restart())
    {
        a10EnableTableAccess();
    }
}

//-------------------------------------------------------------------------------------------------

void a103ToUnicode (
                    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a103AllRoles", Catalog_Trace, 1);
    ToUnicodeCallback callback(acv);
    Catalog_IAuthorization::GetInstance().ForEach (SQLMan_Context::AcvToContext(acv), callback, c_eachUser, !c_eachSchema, c_eachRole);
    if (0 == acv.a_returncode)
    {
        Catalog_Instance::Instance().MigrateToUnicode();
    }
}

//-------------------------------------------------------------------------------------------------

void a103UpdateGroupMembers (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 pGroup)
{
     SAPDBTRACE_ROUTINE_DEBUG ("a103UpdateGroupUsers", Catalog_Trace, 1);
     Catalog_IAuthorization::GetInstance().UpdateGroupMembers(SQLMan_Context::AcvToContext(acv), REINTERPRET_CAST(Catalog_AuthorizationObject*, pGroup));
}

