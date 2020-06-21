#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_DBProcDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_DBProcObject.hpp"
#include "SQLManager/Catalog/Catalog_DBProcInfoObject.hpp"
#include "SQLManager/Catalog/Catalog_SessionCache.hpp"
#include "SQLManager/Catalog/Catalog_IAuthorization.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Catalog.h"


#define ASSERT_DBPROCDESCRIPTOR_CREATED if (0 == m_pDBProc) \
    {\
        ErrMsg = Msg_List( \
	       Msg_List::Error,	SDBMSG_CATALOG_DESCRIPTOR_NOT_CREATED, \
	       Msg_Arg(SDBMSGTAG_CATALOG_DESCRIPTOR_NOT_CREATED__DESCRIPTION, "Catalog_DBProcDescriptor"));\
        return gnRcSystemError;\
    }

#define LOAD_DBPROC_INFO  if (0 == m_pDBProcInfo) \
    {\
       m_pDBProcInfo = m_context.GetSessionCatalogCache().LoadDBProcInfoObject (m_pDBProc->GetId()); \
       if (0 == m_pDBProcInfo) \
       { \
       switch (m_context.GetSessionCatalogCache().GetLastError()) {\
           case e_cancelled : \
               return gnRcCancelled; \
           default : \
               return gnRcSystemError; \
       } \
       } \
    }

Catalog_DBProcDescriptor::Catalog_DBProcDescriptor(SQLMan_Context& Context)
: m_context(Context)
, m_pDBProc(0)
, m_pDBProcInfo(0)
{
}

Catalog_DBProcDescriptor::~Catalog_DBProcDescriptor()
{
    // TODO release references
}

//-----------------------------------------------------------------------------

Catalog_DBProcDescriptor::CreateRc Catalog_DBProcDescriptor::Create(const Catalog_ObjectId& DbprocId, Msg_List& ErrMsg)
{

    m_pDBProc = m_context.GetSessionCatalogCache().LoadDBProcObject (DbprocId);
    if (m_pDBProc)
    {
        return CreateRc(createRcOk);
    }
    int err = m_context.GetSessionCatalogCache().GetLastError();
    switch (err) {
    case e_sysinfo_not_found :
            return CreateRc(createRcUnknownProcedure);
    case e_cancelled :
        return CreateRc(createRcCancelled);
    default :
        {
            m_context.ThrowError (err);
            return createRcSystemError;
        }
    }
}

//-----------------------------------------------------------------------------

Catalog_DBProcDescriptor::GetNameRc Catalog_DBProcDescriptor::GetSchemaName (SQLMan_Identifier& SchemaName, Msg_List& ErrMsg) const
{
    ASSERT_DBPROCDESCRIPTOR_CREATED;
    LOAD_DBPROC_INFO;
    SchemaName = Catalog_IAuthorization::GetInstance().GetSchemaName (m_context, m_pDBProcInfo->GetSchemaId());
    return gnRcOk;
}

//-----------------------------------------------------------------------------

Catalog_DBProcDescriptor::GetNameRc Catalog_DBProcDescriptor::GetName (SQLMan_Identifier& Name, Msg_List& ErrMsg) const
{
    ASSERT_DBPROCDESCRIPTOR_CREATED;
    LOAD_DBPROC_INFO;
    Name = m_pDBProcInfo->GetName();
    return gnRcOk;
}
