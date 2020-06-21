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

#include "liveCache/LVC_ContainerManager.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Oms.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_ISessionCache.hpp"
#include "SQLManager/Catalog/Catalog_IAuthorization.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SQLManager/Catalog/Catalog_AuthorizationObject.hpp"
#include "SQLManager/Catalog/Catalog_ContainerObject.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"
#include "SAPDB/KernelCommon/Kernel_VTrace.hpp"
#include "Oms/OMS_Defines.h"
#include "SAPDBCommon/Messages/SDBMsg_LiveCache.h"

#include "hak51.h"
#include "ggg01.h"
#include "hkb04.h"
#include "hbd01_1.h"

static const int MAX_ATTEMPT_COUNT = 10;
static const int TRY_AGAIN_CODE    = -8;

class LVC_InternalTrans
{
public :
    LVC_InternalTrans(SQLMan_Context& Context)
        : m_created(false)
        , m_context(Context)
    {
    }
    void Create()
    {
        a51_internal_trans (m_context, m_transContext);
        if (e_ok != m_context.TransContext().trError_gg00)
        {
            m_context.ThrowError (m_context.TransContext().trError_gg00);
        }
        else
        {
            m_created = true;
        }
    }
    ~LVC_InternalTrans()
    {
        if (m_created)
        {
            tgg00_MessType mType;
            if (m_context.IsOk())
            {
                mType.becomes(m_commit);
            }
            else
            {
                mType.becomes(m_rollback);
            }
            m_context.TransContext().trError_gg00 = e_ok;
            a51close_internal_trans (m_context, m_transContext, mType);
        }
    }
private :
    bool            m_created;
    SQLMan_Context& m_context;
    tgg00_UnivTrans m_transContext;
};

//-----------------------------------------------------------------------------

static bool CheckIntegrityAndBuildFileId (SQLMan_Context& Context,  
                                          SAPDB_UInt4     Size,
                                          Catalog_ISessionCache::Catalog_Reference<Catalog_ContainerObject>& ContainerRef,
                                          tgg00_FileId&   ContainerFile)
{
    if ((0 != Size) && (ContainerRef->GetSize() != Size))
    {
        Context.ThrowError ( e_invalid_blocksize );
        return false;
    }
    FileDir_IDirectory& fileDir = FileDir_IDirectory::Instance(Context.TransContext().trIndex_gg00);
    FileDir_Oms pFile;
    Msg_List    errorList;
    FileDir_ReturnCode rc = fileDir.GetFile(
        Context.TransContext().trTaskId_gg00, ContainerRef->GetFileNo(), pFile, errorList);
    if (FileDir_Okay == rc)
    {
        pFile.BuildFileId(ContainerFile);
        return true;
    }
    Context.GetErrorList().AppendNewMessage(errorList);
    Kernel_VTrace() << "LVC_ContainerManager::RegisterContainer : FileDir error " << rc;
    Kernel_VTrace() << "FileNo is " << ContainerRef->GetFileNo().GetUInt8();
    Context.ThrowError ((FileDir_FileNoNotFound == rc) ? e_old_fileversion : e_file_not_found);
    return false;
}

//-----------------------------------------------------------------------------

class LVC_ObjectFileId : public tgg00_FileId
{
public :
    LVC_ObjectFileId(bool IsVarContainer,  FileDir_FileNo fileNo)
    {
        *reinterpret_cast<tgg00_FileId*>(this) = b01niltree_id;
        if (IsVarContainer)
        {
            fileObjFileType_gg00().becomes(oftVarLenObjFile_egg00);
        }
        else
        {
            fileObjFileType_gg00().becomes(oftFixLenObjFile_egg00);
        }
        SAPDB_MemCopyNoCheck (&fileDirFileId_gg00(), &fileNo, sizeof(fileNo));
        fileType_gg00().addElement(ftsPerm_egg00);
        fileType_gg00().addElement(ftsConcurrent_egg00);
        fileTfn_gg00().becomes(tfnObj_egg00);
    }
};

//-----------------------------------------------------------------------------

bool LVC_ContainerManager::ExistsContainer
(
 SQLMan_Context&          Context, 
 SAPDB_UInt4              SchemaId, 
 SAPDB_UInt4              ContainerNo, 
 const Catalog_GUID&      Guid,
 SAPDB_UInt4              ObjectSize,
 tgg00_FileId&            ContainerFile,
 bool&                    UseCachedKeys
 )
{
    int count = 0;
    do
    {
        ++count;
        Catalog_ISessionCache& sessionCache = Context.GetSessionCatalogCache();
        Catalog_AuthorizationObject schemaObject;
        if (!Catalog_IAuthorization::GetInstance().LoadAuthorizationObject (Context, SchemaId, Catalog_IAuthorization::LOCK_NONE, &schemaObject))
        {
            return false;
        }
        const bool noWait = true;
        Catalog_ISessionCache::Catalog_Reference<Catalog_ContainerObject> containerRef =
            sessionCache.LoadContainerObject (schemaObject.GetObjectId(), ContainerNo, Guid, !noWait); 
        if (containerRef)
        {
            UseCachedKeys = containerRef->UseCachedKeys();
            FileDir_FileNo fileNo = containerRef->GetFileNo();
            bool ok = CheckIntegrityAndBuildFileId (Context, ObjectSize, containerRef, ContainerFile); 
            if (!ok)
            {
                if (TRY_AGAIN_CODE == Context.ReturnCode())
                {
                    Catalog_ContainerObject::Catalog_Key key(schemaObject.GetObjectId(), ContainerNo, Guid);
                    Catalog_Cache::Invalidate (Context.TransContext().trTaskId_gg00, key);
                }
                else
                {
                    break;
                }
            }
            else
            {
                return true;
            }
        }
        if (TRY_AGAIN_CODE == Context.ReturnCode())
        {
            Context.ResetReturnCode();
        } 
        else
        {
            break;
        }
    }
    while (count <= MAX_ATTEMPT_COUNT);
    return false;
}

//-----------------------------------------------------------------------------

bool LVC_ContainerManager::DropContainer
(
 SQLMan_Context&          Context, 
 SAPDB_UInt4              SchemaId, 
 SAPDB_UInt4              ContainerNo, 
 const Catalog_GUID&      Guid 
 )
{
    Catalog_AuthorizationObject schemaObject;
    if (!Catalog_IAuthorization::GetInstance().LoadAuthorizationObject (Context, SchemaId, Catalog_IAuthorization::LOCK_SHARE, &schemaObject))
    {
        return false;
    }
    return Catalog_ContainerObject::Drop (Context, schemaObject.GetObjectId(), ContainerNo, Guid); 
}

//-----------------------------------------------------------------------------

bool LVC_ContainerManager::DropContainerCallback 
(
 SQLMan_Context& Context, 
 FileDir_FileNo& FileNo,
 bool            IsVarContainer
 )
{
    Msg_List errorList;
    FileDir_IDirectory& fileDir = FileDir_IDirectory::Instance(Context.TransContext().trIndex_gg00);
    FileDir_Oms pFile;
    FileDir_ReturnCode rc = fileDir.GetFile(
        Context.TransContext().trTaskId_gg00, FileNo, pFile, errorList);
    if (FileDir_Okay == rc)
    {
        tgg00_FileId objFileId;
        pFile.BuildFileId(objFileId);
        kb04DropObjFile (Context.TransContext(), objFileId);
        return e_ok == Context.TransContext().trError_gg00;
    }
    Context.GetErrorList().AppendNewMessage(errorList);
    Context.GetErrorList().AppendNewMessage(Msg_List(
        Msg_List::Error,
        SDBMSG_LIVECACHE_DROP_CONTAINER_TRIGGER_FAILED,
        Msg_Arg("FILENO", FileNo.GetUInt8()),
        Msg_Arg("RETURNCODE",rc)
        ));
    Context.TransContext().trError_gg00 = e_file_not_found;
    return false;
}

//-----------------------------------------------------------------------------

void LVC_ContainerManager::DropSchemaContainers 
        (
            SQLMan_Context&         Context,
            const Catalog_ObjectId& SchemaId
        )
{
    Catalog_ContainerObject::DropSchemaContainers (Context, SchemaId); 
}

//-----------------------------------------------------------------------------

void LVC_ContainerManager::GetContainerInfo
(
 SQLMan_Context&         Context,
 const FileDir_FileNo&   FileNo,
 Catalog_GUID&           Guid, 
 SAPDB_UInt4&            SchemaId, 
 SAPDB_UInt4&            ContainerNo,
 Catalog_ObjectId&       schemaObjectId
 )
{
    Catalog_ContainerObject::GetContainerInfo (Context, FileNo, Guid, schemaObjectId, ContainerNo);
    if (Context.IsOk())
    {
        Catalog_ISessionCache& sessionCache = Context.GetSessionCatalogCache();
        if (Catalog_AuthorizationObject* pSchemaObject = sessionCache.LoadAuthorizationObject (schemaObjectId))
        if (pSchemaObject)
        {
           SchemaId = pSchemaObject->GetSchemaHandle();
        }
        else
        {
            Context.ThrowError (e_unknown_schema);
        }
    }
}

//-----------------------------------------------------------------------------

LVC_ContainerManager::RegistrationResult LVC_ContainerManager::RegisterContainer 
(
 SQLMan_Context&          Context, 
 SAPDB_UInt4              SchemaId, 
 SAPDB_UInt4              ContainerNo, 
 const Catalog_GUID&      Guid, 
 bool                     KeyPartitioned,
 bool                     CachedKeys,
 bool                     IsVarObjectContainer,
 bool                     NoWait,
 SAPDB_UInt4              Size,
 SAPDB_UInt4              KeyPos,
 SAPDB_UInt4              KeySize,
 const SQLMan_Identifier& ClassName,
 bool                     UseInternalTrans,
 tgg00_FileId&            ContainerFile
 )
{
    Context.ResetReturnCode();
    LVC_InternalTrans internalTrans(Context);
    if (UseInternalTrans)
    {
        internalTrans.Create();
        if (!Context.IsOk())
        {
            return REG_ERROR;
        }
    }
    if (IsVarObjectContainer)
    {
        Size = 126; // yields good page usage
    }
    SQLMan_TransContext& trans = Context.TransContext();
    trans.trError_gg00         = e_ok;
    Catalog_ISessionCache& sessionCache = Context.GetSessionCatalogCache();
    Catalog_AuthorizationObject schemaObject;
    if (!Catalog_IAuthorization::GetInstance().LoadAuthorizationObject (Context, SchemaId, Catalog_IAuthorization::LOCK_SHARE, &schemaObject))
    {
        Context.ThrowError (e_unknown_schema); 
        return REG_ERROR;
    }
    int loopCnt = 0;
    while (loopCnt < MAX_ATTEMPT_COUNT)
    {
        ++loopCnt;
        Catalog_ISessionCache::Catalog_Reference<Catalog_ContainerObject> containerRef = 
            sessionCache.LoadContainerObject (schemaObject.GetObjectId(), ContainerNo, Guid, NoWait);
        if (containerRef)
        {
            bool ok = CheckIntegrityAndBuildFileId (Context, Size, containerRef, ContainerFile);
            if (!ok)
            {
                if (TRY_AGAIN_CODE == Context.ReturnCode())
                {
                    Catalog_ContainerObject::Catalog_Key key(schemaObject.GetObjectId(), ContainerNo, Guid);
                    Catalog_Cache::Invalidate (Context.TransContext().trTaskId_gg00, key);
                }
                else
                {
                    break;
                }
            }
            else
            {
                return REG_OK;
            }
        }
        if (Context.IsOk())
        {
            FileDir_FileNo fileno;
            if ( kb04GetNewFileId(trans, fileno) )
            {
                int noOfKeyFiles = 0;
                if (KeyPos > 0)
                {
                    noOfKeyFiles = (KeyPartitioned) ? 16 : 1; 
                }
                Catalog_ContainerObject::CreateContainer (Context,  schemaObject.GetObjectId(), ContainerNo, Guid, noOfKeyFiles, 
                    CachedKeys, IsVarObjectContainer, Size, fileno, ClassName);
                if (Context.IsOk())
                {
                    // create file, update file id in catalog
                    LVC_ObjectFileId objFile(IsVarObjectContainer, fileno);
                    kb04CreateObjFile (trans, objFile, Size, KeySize, noOfKeyFiles > 1);
                    if (e_ok == trans.trError_gg00)
                    {
                        ContainerFile = objFile;
                        return REG_CREATED;
                    }
                }
                else
                {
                    if (SQL_DUPLICATE_KEY == Context.ReturnCode())
                    {
                        // someone else may have been faster, try again
                        Context.ResetReturnCode();
                        Context.ResetErrorList();
                    }
                    else
                    {
                        return REG_ERROR;
                    }
                }
            }
            if (e_ok != trans.trError_gg00)
            {
                Context.ThrowError (trans.trError_gg00);
                return REG_ERROR;
            }
        }
        else
        {
            if (TRY_AGAIN_CODE == Context.ReturnCode())
            {
                Context.ResetReturnCode();
            }
            else
            {
                break;
            }
        }
    }
    Context.ThrowError (e_invalid);
    return REG_ERROR; 
}
