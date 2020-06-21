/*

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
#include "SQLManager/Tables/SQLTab_SysUpgradeHistory.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp" 
#include "SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp" 
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SAPDBCommon/Messages/SDBMsg_KernelCommon.h"
#include "RunTime/RTE_Message.hpp"

#include "hgg11.h"
#include "hak362.h"

SQLTab_SysUpgradeHistory::SQLTab_SysUpgradeHistory()
: m_context(*SQLMan_Context::GetContext())
{
}

//-----------------------------------------------------------------------------

void SQLTab_SysUpgradeHistory::StoreUpgradeHistory()
{
    KSQL_PreparedStatement::SQLCode rc = 0;
    {
        SQLMan_SQLStatementContext newStatementScope(m_context);
        KSQL_Connection con;
        if (con.isConnected())
        {
            m_context.SetInDDLTrigger();
            a362OnlineTaskId = m_context.TransContext().trTaskId_gg00;
            KSQL_Statement createStmt = con.createStatement();
            rc = createStmt.asciiExecute("CREATE TABLE SYSDBA.SYSUPGRADEHISTORY (KERNELVERSION CHAR(40) ASCII,"
                                         "EXECUTEDATE TIMESTAMP, \"COMPONENT\" CHAR(18) ASCII, \"DESCRIPTION\" CHAR(2000) ASCII) INTERNAL");
            a362OnlineTaskId = UNDEF_SP00;
            if ((0 != rc) && (-6000 != rc))
            {
                Msg_List msg (
                    Msg_List::Warning,
                    SDBMSG_KERNELCOMMON_NO_UPGRADE_PROTOCOL,
                    Msg_OptArg("RETURNCODE", rc)
                    );
                RTE_Message (msg);
                return;
            }
            rc = 0;
            KSQL_PreparedStatement stmt = con.createPreparedStatement();
            stmt.prepare("INSERT INTO SYSDBA.SYSUPGRADEHISTORY VALUES (?, ?, upper(?), ?)");
         
            Kernel_Migration& migration = Kernel_IAdminConfig::Instance().MigrationHandler();
            
            tsp00_Version kernelVersion;
            g11kernel_version (kernelVersion);

            bool firstCall = true;
            const SAPDB_Char* pComponent;
            const SAPDB_Char* pComment;
            const SAPDB_Char* pTimeStamp;
            while (migration.NextMigrationInfo (firstCall, pComponent, pComment, pTimeStamp))
            {
                // read all migration info even if an error occurred. Otherwise the list of
                // migration info will not be destructed ==> memory leak
                firstCall = false;
                if (0 == rc)
                {
                    stmt.bindAsciiInput (1, (const char*) &kernelVersion[0], sizeof(kernelVersion));
                    stmt.bindAsciiInput (2, (const char*) pTimeStamp, sizeof(tsp00_Timestamp));
                    stmt.bindAsciiInput (3, pComponent, strlen(pComponent));
                    stmt.bindAsciiInput (4, pComment, strlen(pComment));
                    rc = stmt.execute();
                    if (0 != rc)
                    {
                        Msg_List msg (
                            Msg_List::Warning,
                            SDBMSG_KERNELCOMMON_NO_UPGRADE_PROTOCOL,
                            Msg_OptArg("RETURNCODE", rc)
                            );
                        RTE_Message (msg);
                    }
                }
            }
        }
    }
}