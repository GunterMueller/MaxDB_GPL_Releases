/*!*********************************************************************

    @file         Proc_SQL.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#include "DBProc/Proc_SQL.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "SQLManager/KernelSQL/KSQL_Connection.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"
#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SQLManager/DBProc/DBProc_Messages.hpp"
#include "DBProc/Proc_PointerList.hpp"

#define UDE_SCHEMA_C "SYSUDE."

/*----------------------------------------*/

bool Proc_SQL::
selectUdeServerConfig (
    SQLMan_Context * context,
    const char     * languageString,
    char           * serverKind,
    char           * commandLine,
    char           * extraInfo,
    int            * sessionLimit,
    SAPDBErr_MessageList * errInfo)
{
    bool            result = false;
    IFR_Length      kindLength = 1;
    IFR_Length      commandlineLength = 256;
    IFR_Length      extraLength = 256;
    static const char * configSelect = "SELECT INFOKIND, COMMANDLINE, EXTRAINFO INTO ?, ?, ? FROM " UDE_SCHEMA_C "SERVER_CONFIG WHERE SERVERKIND = ?";
    KSQL_PreparedStatement::SQLCode rc = 0;
    SQLMan_SQLStatementContext newStatementScope(*context);
    KSQL_Connection con;

    if (!con.isConnected()) {
        *errInfo = SAPDBErr_MessageList (
            DBPROC_ERR_SQL_FAILURE, "-8888", "not connected");
        return false;
    }
    /*
     * select command line
     */
    /*
     * prepare statement
     */
    KSQL_PreparedStatement stmt = con.createPreparedStatement();
    rc = stmt.prepare(configSelect);
    if (rc != 0) {
        *errInfo = SAPDBErr_MessageList (
            DBPROC_ERR_SQL_FAILURE, ToStr (rc), configSelect);
        return false;
    }
    /*
     * bind parameters
     */
    stmt.bindAsciiOutput (1, serverKind, &kindLength, false);
    stmt.bindAsciiOutput (2, commandLine, &commandlineLength);
    stmt.bindAsciiOutput (3, extraInfo, &extraLength);
    stmt.bindAsciiInput (4, languageString, strlen (languageString), true);
    /*
     * execute query
     */
    rc = stmt.execute();
    switch (rc) {
        case 0:
            result = true;
            break;
        case 100:
            *errInfo = SAPDBErr_MessageList (
                DBPROC_ERR_TYPE_NOT_DEFINED, languageString);
            result = false;
            break;
        default:
            *errInfo = SAPDBErr_MessageList (
                DBPROC_ERR_SQL_FAILURE, ToStr (rc), configSelect);
            result = false;
            break;
    }
    if (result == false) {
        return false;
    }
    /*
     * select other properties
     */
    /*
     * prepare statement
     */
    static const char * propertiesSelect = "SELECT INTVALUE INTO ? FROM " UDE_SCHEMA_C "SERVER_PROPERTIES "
        "WHERE SERVERKIND = ? AND NAME = 'MAX_SESSION_PER_UDESERVER'";
    KSQL_PreparedStatement stmt2 = con.createPreparedStatement();
    rc = stmt2.prepare(propertiesSelect);
    if (rc != 0) {
        *errInfo = SAPDBErr_MessageList (
            DBPROC_ERR_SQL_FAILURE, ToStr (rc), propertiesSelect);
        return false;
    }
    /*
     * bind parameters
     */
    SAPDB_Int4 localSessionLimit = 0;
    IFR_Length outlen = sizeof (localSessionLimit);
    stmt2.bindInt4Output (1, localSessionLimit, &outlen);
    stmt2.bindAsciiInput (2, languageString, strlen (languageString));
    /*
     * execute query
     */
    rc = stmt2.execute();
    switch (rc) {
        case 0:
            if (localSessionLimit > 0) {
                *sessionLimit = localSessionLimit;
            }
            else {
                *sessionLimit = Proc_PointerList::noLimitC;
            }
            break;
        case 100:
            *sessionLimit = Proc_PointerList::noLimitC;
            break;
        default:
            *errInfo = SAPDBErr_MessageList (
                DBPROC_ERR_SQL_FAILURE, ToStr (rc), propertiesSelect);
            result = false;
            break;
    }
    /*
     *
     */
    return result;
}

