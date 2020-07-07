/*!
  @file         Kernel_Migration.cpp
  @author       UweH
  @ingroup      KernelCommon
  @brief        Implementation of methods of class Kernel_Migration
  @since        2003-06-30
*/
/*!
\if EMIT_LICENCE

  copyright:    Copyright (c) 2001-2005 SAP AG


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
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "KernelCommon/Kernel_Types.hpp"
#include "KernelCommon/Kernel_Messages.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "KernelCommon/Kernel_MigrationMatrix.hpp"
#include "Container/Container_List.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SQLManager/Tables/SQLTab_SysUpgradeHistory.hpp"

#include "heo52.h" //vtimestamp

struct Kernel_MigrationInfoList
{
    struct ListEntry
    {
        int         m_component; 
        SAPDB_Char* m_comment;
        SAPDB_Char  m_timestamp[sizeof(tsp00_Timestamp) + 1];

        /// constructor
        ListEntry (int Component, const SAPDB_Char* Comment);
        /// destructor
        ~ListEntry ();
    };
    Container_List<ListEntry> m_list;
    /*!
       @brief constructor
     */
    Kernel_MigrationInfoList(SAPDBMem_IRawAllocator& Allocator);
    /*!
       @brief appends a migration info
     */
    void Append (int Component, const SAPDB_Char* Comment);
};

/* --------------------------------------------------------------------------- */

Kernel_MigrationInfoList::ListEntry::ListEntry (int Component, const SAPDB_Char* Comment)
: m_component(Component)
, m_comment(0)
{
    int length = strlen(Comment);
    m_comment  = reinterpret_cast<SAPDB_Char*> (RTEMem_Allocator::Instance().Allocate(length+1));
    if (m_comment)
    {
        strcpy (m_comment, Comment);
    }
    vtimestamp(*reinterpret_cast<tsp00_Timestamp*>(&m_timestamp));
    m_timestamp[sizeof(tsp00_Timestamp)] = 0;
}

/* --------------------------------------------------------------------------- */

Kernel_MigrationInfoList::ListEntry::~ListEntry()
{
    if (m_comment)
    {
        RTEMem_Allocator::Instance().Deallocate(m_comment);
    }
}

/* --------------------------------------------------------------------------- */

Kernel_MigrationInfoList::Kernel_MigrationInfoList(SAPDBMem_IRawAllocator& Allocator)
: m_list(Allocator)
{
}

/* --------------------------------------------------------------------------- */

void Kernel_MigrationInfoList::Append (int Component, const SAPDB_Char* Comment)
{
    ListEntry listEntry(Component, Comment);
    m_list.InsertEnd(listEntry);
    listEntry.m_comment = 0; // dirty, but saves reference counting for this silly case
}

/* --------------------------------------------------------------------------- */

static Kernel_MigrationInfoList theList(RTEMem_Allocator::Instance());

Kernel_MigrationInfoList& Kernel_Migration::m_migrationInfoList = theList;


/* --------------------------------------------------------------------------- */
SAPDB_Char* Kernel_Migration::PersistentComponentStrings[] =
{
    (SAPDB_Char*)"all",
    (SAPDB_Char*)"data",
    (SAPDB_Char*)"log",
    (SAPDB_Char*)"catalog",
    (SAPDB_Char*)"file directory",
    (SAPDB_Char*)"converter"
};
/* --------------------------------------------------------------------------- */
Kernel_Migration::ChangeInfo & Kernel_Migration::SearchForChangeInfo (const SAPDB_Char * changeTitle)
{
    for ( SAPDB_UInt change = 0; change < MAX_CHANGE_INFO; ++change )
    {
        if ( m_ChangeInfos[change].IsValid()
             &&
             ! strcmp( m_ChangeInfos[change].changeTitle, changeTitle) )
            return m_ChangeInfos[change];
    }
    return m_ChangeInfos[INVALID_CHANGE_INFO];
}
/* --------------------------------------------------------------------------- */
const Kernel_Migration::ChangeInfo & Kernel_Migration::SearchForChangeInfo (const SAPDB_Char * changeTitle) const
{
    for ( SAPDB_UInt change = 0; change < MAX_CHANGE_INFO; ++change )
    {
        if ( m_ChangeInfos[change].IsValid()
             &&
             ! strcmp( m_ChangeInfos[change].changeTitle, changeTitle) )
            return m_ChangeInfos[change];
    }
    return m_ChangeInfos[INVALID_CHANGE_INFO];
}
/* --------------------------------------------------------------------------- */
bool Kernel_Migration::CheckCompatibility(PersistentComponent   checkComponent,
                                          const Kernel_Version &checkVersion,
                                          Msg_List             &errlist)
{
    const Kernel_Version currentKernelVersion = Kernel_Version();

    if ( checkVersion == currentKernelVersion )
        return true;

    if ( checkVersion > currentKernelVersion )
    {
        errlist.AppendNewMessage(Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "The software version is too old"));
        checkVersion.AppendAsMessage(__CONTEXT__, errlist, "The persistent version is");
        return false;
    }

    for ( SAPDB_UInt change = 0; change < MAX_CHANGE_INFO; ++change )
    {
        if ( m_ChangeInfos[change].IsValid()
             &&
             m_ChangeInfos[change].component == checkComponent )
        {
            m_ChangeInfos[change].migrationEnabled =
                checkVersion <= m_ChangeInfos[change].changedInVersion;
            if ( m_ChangeInfos[change].migrationEnabled )
                RTE_Message ( Kernel_Exception( __CONTEXT__, KERNEL_ENABLE_MIGRATION, m_ChangeInfos[change].changeTitle ) );
        }
    }

    return true;
}
/* --------------------------------------------------------------------------- */

void Kernel_Migration::CollectMigrationInfo (PersistentComponent Component, const SAPDB_Char* Comment)
{
    m_migrationInfoList.Append(Component, Comment);
}

/* --------------------------------------------------------------------------- */

Kernel_Version Kernel_Migration::GetOldestCompatibleVersion(PersistentComponent checkComponent)
{
    bool           versionFound = false;
    Kernel_Version latestVersion(0,0,0,0);
    for ( SAPDB_UInt change = 0; change < MAX_CHANGE_INFO; ++change )
    {
        if ( m_ChangeInfos[change].IsValid()
             &&
             (m_ChangeInfos[change].component == all || m_ChangeInfos[change].component == checkComponent)
             &&
             m_ChangeInfos[change].changedInVersion > latestVersion )
        {
            versionFound  = true;
            latestVersion = m_ChangeInfos[change].changedInVersion;
        }
    }
    if ( versionFound )
        return latestVersion;
	else
        return Kernel_Version();
}

/* --------------------------------------------------------------------------- */

bool Kernel_Migration::NextMigrationInfo (bool First, const SAPDB_Char* &pComponent, const SAPDB_Char* &pComment, const SAPDB_Char* &pTimeStamp)
{

    static Container_List<Kernel_MigrationInfoList::ListEntry>::Iterator iter;
    static Container_List<Kernel_MigrationInfoList::ListEntry>::Iterator end;
    if (First)
    {
        iter = m_migrationInfoList.m_list.Begin();
        end  = m_migrationInfoList.m_list.End();
    }
    else
    {
        ++iter;
    }
    if (iter != end)
    {
        if (0 == iter->m_component)
        {
            static SAPDB_Char* blankComponent = " ";
            pComponent = blankComponent;
        }
        else
        {
            pComponent = PersistentComponentStrings[iter->m_component-1];
        }
        pComment   = iter->m_comment;
        pTimeStamp = &iter->m_timestamp[0];
        return true;
    }
    m_migrationInfoList.m_list.Delete();
    return false;
}

/* --------------------------------------------------------------------------- */
bool Kernel_Migration::Test(Msg_List &errlist)
{
    Kernel_Migration testMigration;

    testMigration.CollectMigrationInfo (log, "Migration log 1");
    testMigration.CollectMigrationInfo (log, "Migration log 2");
    testMigration.CollectMigrationInfo (catalog, "Catalog Migration 1");

    SQLTab_SysUpgradeHistory upgradeHistory;
    upgradeHistory.StoreUpgradeHistory();
    
    if ( ! testMigration.CheckCompatibility(log, Kernel_Version(7,3), errlist) )
    {
        errlist = errlist + Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "CheckCompatibility: 7.3");
        return false;
    }

    if ( ! testMigration.CheckCompatibility(catalog, Kernel_Version(), errlist) )
    {
        errlist = errlist + Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "CheckCompatibility: with own version");
        return false;
    }

    if ( ! testMigration.SearchForChangeInfo ("HISTORY ROOT ID CHECK").IsValid() )
    {
        errlist = errlist + Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "search for historyRootIdCheck");
        return false;
    }

    if ( testMigration.CheckCompatibility(data, Kernel_Version(8,0), errlist) )
    {
        errlist = errlist + Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "CheckCompatibility: 8.0");
        return false;
    }
    else
        errlist.ClearMessageList();


    if ( GetOldestCompatibleVersion() < Kernel_Version(7,4,3,2) )
    {
        errlist = Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "GetOldestCompatibleVersion()");
        return false;
    }
    
    return true;
}
