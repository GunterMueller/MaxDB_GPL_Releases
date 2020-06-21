/*!
  @file           DBProc_DBProcDescriptor.cpp
  @author         ThomasA
  @ingroup        DBProc
  @brief          A descriptor for db-procedures

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

#include "SQLManager/DBProc/DBProc_DBProcDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_DBProcFileObject.hpp"
#include "SQLManager/Catalog/Catalog_DBProcInfoObject.hpp"
#include "SQLManager/Catalog/Catalog_ISessionCache.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "hsp77.h"

DBProc_DBProcDescriptor::DBProc_DBProcDescriptor(Catalog_ISessionCache& sessionCache, const SQLMan_Surrogate& procId)
: m_sessionCache(sessionCache)
, m_pDBproc(0)
, m_pDBProcFile(0)
, m_pDBProcInfo(0)
, m_procId(procId)
, m_error(0)
{
    m_dbprocName[0]     = 0;
    m_createDateTime[0] = 0;
    m_language[0]       = 0;
    m_pDBproc = sessionCache.LoadDBProcObject(procId);
    if (!m_pDBproc)
    {
       m_error = m_sessionCache.GetLastError();
    }
}

//----------------------------------------------------------------------------------------------

void DBProc_DBProcDescriptor::CopyIdentifier(const SQLMan_Identifier& source,
                       NullTerminatedIdentifier& dest)
{
    SAPDB_MemCopyNoCheck (&dest[0], &source[0], sizeof(dest) -1); // TODO does not work in unicode mode
    int ix = sizeof(dest) -1 -1;
    while ((ix >= 0) && (' ' == dest[ix]))
    {
        --ix;
    }
    dest[ix+1] = 0;
}

//----------------------------------------------------------------------------------------------

const char* DBProc_DBProcDescriptor::GetCreateDateTime()
{
    m_error = 0;
    if (0 == m_createDateTime[0])
    {
        this->LoadProcInfo();
         if (!m_pDBProcInfo)
         {
             return 0;
         }
         sp77sprintf (&m_createDateTime[0], sizeof(m_createDateTime), 
             "%08d%08d", m_pDBProcInfo->GetCreateDate(), m_pDBProcInfo->GetCreateTime());
    }
    return &m_createDateTime[0];
}

//----------------------------------------------------------------------------------------------

const char* DBProc_DBProcDescriptor::GetExternalInfo() 
{
    m_error = 0;
    if (!m_pDBProcFile)
    {

        m_pDBProcFile = m_sessionCache.LoadDBProcFileObject(m_procId);
        if (!m_pDBProcFile)
        {
            m_error = m_sessionCache.GetLastError();
            return 0;
        }
    }
    return m_pDBProcFile->GetExternalInfo();
}

//----------------------------------------------------------------------------------------------

const char* DBProc_DBProcDescriptor::GetLanguageString() 
{
    if (Catalog_DBProcObject::Unknown != this->GetLanguage())
    {
       return m_pDBproc->GetLanguageString();
    }
    if (0 == m_language[0])
    {
        this->LoadProcInfo();
        if (!m_pDBProcInfo)
        {
            return 0;
        }
        const SQLMan_Identifier& language = m_pDBProcInfo->GetLanguage();
        this->CopyIdentifier (language, m_language);
    }
    return &m_language[0];
}

//----------------------------------------------------------------------------------------------

 const char* DBProc_DBProcDescriptor::GetName() 
 {
     m_error = 0;
     if (0 == m_dbprocName[0])
     {
         this->LoadProcInfo();
         if (!m_pDBProcInfo)
         {
             return 0;
         }
         const SQLMan_Identifier& dbprocName = m_pDBProcInfo->GetName();
         this->CopyIdentifier (dbprocName, m_dbprocName);
     }
     return &m_dbprocName[0];
 }

 //----------------------------------------------------------------------------------------------

 void DBProc_DBProcDescriptor::LoadProcInfo()
 {
     if (!m_pDBProcInfo)
     {
         m_pDBProcInfo = m_sessionCache.LoadDBProcInfoObject(m_procId);
         if (!m_pDBProcInfo)
         {
             m_error = m_sessionCache.GetLastError();
         }
     }
 }