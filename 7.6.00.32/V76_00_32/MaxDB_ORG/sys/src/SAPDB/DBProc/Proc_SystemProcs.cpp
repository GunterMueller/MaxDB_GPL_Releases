/*!*********************************************************************

    @file         Proc_SystemProcs.cpp

    @author       DanielD

    @brief

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
***********************************************************************/

#include "DBProc/Proc_SystemProcs.hpp"

#include <stdlib.h>

#include "hsp42.h"
#include "SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SQLManager/DBProc/DBProc_DBProcDescriptor.hpp"
#include "SQLManager/DBProc/DBProc_Handler.hpp"
#include "SQLManager/DBProc/DBProc_Messages.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/KernelSQL/KSQL_Connection.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"
#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp"
#include "DBProc/Proc_ParameterCopy.hpp"
#include "DBProc/Proc_SharedInternals.hpp"
#include "DBProc/Proc_IFactory.hpp"
#include "DBProc/Proc_IServerControl.hpp"
#include "DBProc/Proc_ISession.hpp"
#include "DBProc/Proc_FactoryDirectory.hpp"
#include "DBProc/Proc_ServerSessionIterator.hpp"

/*----------------------------------------*/

#define UDE_SCHEMA_C "SYSUDE."

/*----------------------------------------*/

static void
addDbprocError (
    SQLMan_Context    *        context,
    SAPDB_Char  const * const  ContextName,
    SAPDB_Char  const * const  FileName,
    SAPDB_UInt4 const          LineNumber,
    SAPDBErr_MessageList::MessageType const Type,
    SAPDBErr_MessageList::MessageID   const ID,
    SAPDB_Char  const * const  Message,
    SAPDB_UInt4 const          NumOfArgs,
    SAPDB_Char  const * const  Arg0 = NULL,
    SAPDB_Char  const * const  Arg1 = NULL,
    SAPDB_Char  const * const  Arg2 = NULL,
    SAPDB_Char  const * const  Arg3 = NULL,
    SAPDB_Char  const * const  Arg4 = NULL,
    SAPDB_Char  const * const  Arg5 = NULL,
    SAPDB_Char  const * const  Arg6 = NULL,
    SAPDB_Char  const * const  Arg7 = NULL,
    SAPDB_Char  const * const  Arg8 = NULL,
    SAPDB_Char  const * const  Arg9 = NULL )
{
    SAPDBErr_MessageList msg (ContextName, FileName, LineNumber,
        Type, ID, Message, NumOfArgs,
        Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9);
    SAPDBErr_MessageList& errList = context->GetErrorList();
    errList.AppendNewMessage(msg);
    context->ThrowError (e_system_error); // TODO
}

/*----------------------------------------*/

/*!

  @class Proc_SystemSession
  @brief execute SYSTEM procs

 */
class Proc_SystemSession
    : public Proc_ISession
{
public:
    virtual Proc::Error callDBProc (SQLMan_Context      * context,
                             DBProc_DBProcDescriptor    * procDescriptor,
                             Proc_ParameterCopy         * parameterMap);
    virtual Proc_IServerControl * getController ();
    virtual void dumpOn (SAPDBTrace_Stream & printer, int indent = 0) const;
};

/*----------------------------------------*/


Proc_SystemSession systemSession;

/*!

  @class Proc_SystemSessionFactory

 */
class Proc_SystemSessionFactory
    : public Proc_IFactory
{
public:
    Proc_SystemSessionFactory () {};
    virtual ~Proc_SystemSessionFactory () {};
    virtual Proc_ISession * createSession (SAPDBErr_MessageList * messageList);
    virtual void dumpOn (SAPDBTrace_Stream & printer, int indent = 0) const;
    virtual void collectAllServerControls (Proc_ControllerVector & allControls) const;
};

Proc_SystemSessionFactory factoryInstance;

Proc_ISession * Proc_SystemSessionFactory::
createSession (
    SAPDBErr_MessageList * messageList)
{
    return &systemSession;
}

/*----------------------------------------*/

void Proc_SystemSessionFactory::
dumpOn (
    SAPDBTrace_Stream & printer,
    int indent) const
{
    printer << SAPDB_Indent (indent) << "Proc_SystemSessionFactory ("
        << SAPDB_ToString (this) << ")" << FlushLine;
    systemSession.dumpOn (printer, indent + 1);
}

/*----------------------------------------*/

void Proc_SystemSessionFactory::
collectAllServerControls (
    Proc_ControllerVector & allControls) const
{
    // TODO: create dummy controller to allow access to pseudo session
}

/*----------------------------------------*/

Proc_IFactory *
Proc_SystemProcs::factory ()
{
    return &factoryInstance;
}

Proc_ISession * createSession (SAPDBErr_MessageList * messageList);

/*----------------------------------------*/

typedef Proc::Error systemProc_FT (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap);

typedef struct NameFunctionPairT {
    const char    * procname;
    systemProc_FT * procpointer;
} NameFunctionPairT;

static systemProc_FT DEFINE_SERVER;
static systemProc_FT DEFINE_DEBUGSERVER;
static systemProc_FT INVALIDATE_CATALOGCACHE;
static systemProc_FT UNDEFINE_SERVER;
static systemProc_FT MANAGE_SERVER;
static systemProc_FT SET_QUERY_REWRITE_RULE;
static systemProc_FT SHOW_SERVER;
static systemProc_FT SHOW_SESSIONS;
static systemProc_FT ZZZ_ARGUMENTTEST;

static NameFunctionPairT procMapping [] = {
    {"DEFINE_SERVER", DEFINE_SERVER},
    {"DEFINE_DEBUGSERVER", DEFINE_DEBUGSERVER},
    {"INVALIDATE_CATALOGCACHE", INVALIDATE_CATALOGCACHE},
    {"UNDEFINE_SERVER", UNDEFINE_SERVER},
    {"MANAGE_SERVER", MANAGE_SERVER},
    {"SET_QUERY_REWRITE_RULE", SET_QUERY_REWRITE_RULE},
    {"SHOW_SERVER", SHOW_SERVER},
    {"SHOW_SESSIONS", SHOW_SESSIONS},
    {"ZZZ_ARGUMENTTEST", ZZZ_ARGUMENTTEST},
    {NULL, NULL}
};

/*----------------------------------------*/

Proc::Error Proc_SystemSession::
callDBProc (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    Proc_ParameterCopy         * parameterMap)
{
    const char * procname = procDescriptor->GetExternalInfo ();
    NameFunctionPairT * current = &procMapping [0];
    while (current->procname != NULL) {
        if (strcmp (procname, current->procname) == 0) {
            return current->procpointer (context, procDescriptor, parameterMap);
        }
        ++current;
    }
    // TODO: name not found
    return Proc::internalError;
}

/*----------------------------------------*/

Proc_IServerControl * Proc_SystemSession::
getController ()
{
    return NULL;
}

/*----------------------------------------*/

void Proc_SystemSession::dumpOn (
    SAPDBTrace_Stream & printer,
    int indent) const
{
    printer << SAPDB_Indent (indent) << "Proc_SystemSession ("
        << SAPDB_ToStringClass (this) << ")\n";
}

/*----------------------------------------*/

const char * truestr  = "True";
const char * falsestr = "False";

#define BOOLSTR(boolexpr) #boolexpr << (boolexpr ? truestr : falsestr)

/*----------------------------------------*/

static Proc::Error
execSQL (
    SQLMan_Context          * context,
    const char              * statement,
    Proc_ParameterCopy      * parameters,
    int                       rcToIgnore = 0)
{
    Proc::Error result;
    if (context->IsOk()) {
        /*
         * creat connection object
         */
        KSQL_PreparedStatement::SQLCode rc = 0;
        SQLMan_SQLStatementContext newStatementScope(*context);
        KSQL_Connection con;
        SAPDBTRACE_WRITELN (DBProc_Trace, 5, BOOLSTR (con.isConnected()));
        if (con.isConnected()) {
            /*
             * parse
             */
            KSQL_PreparedStatement stmt = con.createPreparedStatement();
            rc = stmt.prepare(statement);
            SAPDBTRACE_WRITELN (DBProc_Trace, 5, "prepare rc: " << rc << " " << statement);
            /*
             * execute
             */
            if (rc == 0) {
                if (parameters != NULL) {
                    int parameterCount = parameters->GetFieldCount ();
                    for (int i = 0; i < parameterCount; ++ i) {
                        parameters->bindParameter (&stmt, i, false);
                    }
                }
                rc = stmt.execute();
                SAPDBTRACE_WRITELN (DBProc_Trace, 5, "execute rc: " << rc);
            }
        }
        if ((rc == 0) || (rc == rcToIgnore)) {
            result = Proc::ok;
        }
        else {
            result = Proc::internalSQLFailed;
            addDbprocError (context,
                DBPROC_ERR_SQL_FAILURE, ToStr(rc), statement);
        }
    }
    return result;
}

/*----------------------------------------*/

typedef void tableFiller_F (
    SQLMan_Context * context,
    KSQL_PreparedStatement  * prepared);

static Proc::Error
fillSystemView (
    SQLMan_Context          * context,
    const char              * dropStatement,
    const char              * createStatement,
    const char              * insertStatement,
    tableFiller_F             tableFiller)
{
    if (!context->IsOk()) {
        return Proc::internalError;
    }
    /*
     * creat connection object
     */
    KSQL_PreparedStatement::SQLCode rc = 0;
    SQLMan_SQLStatementContext newStatementScope(*context);
    KSQL_Connection con;
    SAPDBTRACE_WRITELN (DBProc_Trace, 5, BOOLSTR (con.isConnected()));
    if (con.isConnected()) {
        KSQL_Statement stmt (con.createStatement());
        KSQL_Statement::SQLCode sqlRC;
        /*
         * drop table (ignore errors)
         */
        stmt.asciiExecute (dropStatement);
        /*
         * create table
         */
        sqlRC = stmt.asciiExecute (createStatement);
        if (sqlRC != 0) {
            addDbprocError (context,
                DBPROC_ERR_SQL_FAILURE, ToStr(rc), createStatement);
            return Proc::internalSQLFailed;
        }
        /*
         * parse
         */
        KSQL_PreparedStatement insert = con.createPreparedStatement();
        rc = insert.prepare(insertStatement);
        SAPDBTRACE_WRITELN (DBProc_Trace, 5, "prepare rc: " << rc << " " << insertStatement);
        if (rc != 0) {
            addDbprocError (context,
                DBPROC_ERR_SQL_FAILURE, ToStr(rc), insertStatement);
            return Proc::internalSQLFailed;
        }
        /*
         * execute
         */
        tableFiller (context, &insert);
    }
    return Proc::ok;
}

/*----------------------------------------*/

static Proc::Error
DEFINE_SERVER (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DEFINE_SERVER", DBProc_Trace, 1);
    Proc::Error result;

    Proc_ParameterCopy parameters (&context->GetAllocator (), parameterMap);
    result = execSQL (context, "INSERT INTO " UDE_SCHEMA_C "SERVER_CONFIG (SERVERKIND, INFOKIND, COMMANDLINE, COMMENT) VALUES (?,'C',?,?) UPDATE DUPLICATES",
        &parameters);
    return result;
}

/*----------------------------------------*/

static Proc::Error
DEFINE_DEBUGSERVER (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DEFINE_DEBUGSERVER", DBProc_Trace, 1);
    Proc::Error result;
    SAPDBFields_AttributedField portParameter;
    SAPDBErr_MessageList        msgList;
    char                        portString [41];
    int                         outlen;
    tsp00_NumError              numErr;
    const int                   portDigits = 5; // must match definition of DBProc

    Proc_ParameterCopy parameters (&context->GetAllocator (), parameterMap);
    parameters.GetField (1, portParameter, msgList);
    s42gchr (portParameter.GetDataPtr () + 1, 1, portDigits, 0,
            portDigits, portString + 1, 1, outlen, numErr);
    portString [0] = ' ';
    parameters.PatchParameter (1, portString, outlen + 1);
    result = execSQL (context, "INSERT INTO " UDE_SCHEMA_C "SERVER_CONFIG (SERVERKIND, INFOKIND, COMMANDLINE, COMMENT) VALUES (?,'P',?,?) UPDATE DUPLICATES",
        &parameters);
    return result;
}

/*----------------------------------------*/

static Proc::Error
UNDEFINE_SERVER (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("UNDEFINE_SERVER", DBProc_Trace, 1);
    Proc::Error result;

    Proc_ParameterCopy parameters (&context->GetAllocator (), parameterMap);
    result = execSQL (context, "DELETE FROM " UDE_SCHEMA_C "SERVER_CONFIG WHERE SERVERKIND = ?",
        &parameters, 200);
    return result;
}


/*----------------------------------------*/

static Proc::Error
MANAGE_SERVER (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("MANAGE_SERVER", DBProc_Trace, 1);
    Proc::Error result;

    Proc_ParameterCopy parameters (&context->GetAllocator (), parameterMap);
    result = Proc::ok;
    return result;
}

/*----------------------------------------*/

static Proc::Error
SET_QUERY_REWRITE_RULE (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SET_QUERY_REWRITE_RULE", DBProc_Trace, 1);
    Proc::Error result;

    Proc_ParameterCopy parameters (&context->GetAllocator (), parameterMap);
    SQLMan_Identifier rule;
    bool              active;

    SAPDBFields_AttributedField portParameter;
    SAPDBErr_MessageList        msgList;
    parameters.GetField (0, portParameter, msgList);
    tgg00_BasisError e = e_ok;
    SAPDB_RangeMove( __FILE__,   1, 1 + portParameter.GetLength(), sizeof(rule),
        portParameter.GetDataPtr (), 2, &rule[0], 1, portParameter.GetLength() - 1, e);
    if (e_ok == e)
    {
        parameters.GetField (1, portParameter, msgList);
        unsigned char yesNo[6];
        SAPDB_RangeMove( __FILE__,   1, 1 + portParameter.GetLength(), sizeof(yesNo),
            portParameter.GetDataPtr (), 2, &yesNo[0], 1, portParameter.GetLength() - 1, e);
        // Note : since a constraint exists which allows just 'YES' and 'NO' the following is satisfactory 
        if (csp_unicode_mark == yesNo[0])
        {
            active = 'Y' == yesNo[1]; 
        }
        else
        {
            active = 'Y' == yesNo[0];
        }
    }
    if (e_ok == e)
    {
        if (!QueryRewrite_GlobalRuleList::Instance()->SetGlobalRuleStatus(rule, active))
        {
            context->SetStopMsg (-31001, " unknown rule", 13);
            result = Proc::udeReturnedError;
        }
        else
        {
            result = Proc::ok;
        }
    }
    else
    {
        context->SetStopMsg (-31002, " move error", 11);
        result = Proc::udeReturnedError;
    }
    return result;
}

/*----------------------------------------*/

static Proc::Error
INVALIDATE_CATALOGCACHE (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("INVALIDATE_CATALOGCACHE", DBProc_Trace, 1);
    
    Proc_ParameterCopy parameters (&context->GetAllocator (), parameterMap);
    SQLMan_Identifier schema;
    SQLMan_Identifier tablename;

    SAPDBFields_AttributedField portParameter;
    SAPDBErr_MessageList        msgList;
    parameters.GetField (0, portParameter, msgList);
    tgg00_BasisError e = e_ok;
    SAPDB_RangeMove( __FILE__,   1, 1 + portParameter.GetLength(), sizeof(schema),
        portParameter.GetDataPtr (), 2, &schema[0], 1, portParameter.GetLength() - 1, e);
    if (e_ok == e)
    {
        parameters.GetField (1, portParameter, msgList);
        SAPDB_RangeMove( __FILE__,   1, 1 + portParameter.GetLength(), sizeof(tablename),
            portParameter.GetDataPtr (), 2, &tablename[0], 1, portParameter.GetLength() - 1, e);
    }
    if (e_ok == e)
    {
		const bool cConsiderResultTables = true;
		const bool cConsiderSYSDDTables  = true;
		Catalog_TableDescriptor tableDesc(*context, schema, tablename, !cConsiderResultTables, !cConsiderSYSDDTables);
		if (tableDesc) 
		{
			Catalog_Cache::InvalidateTable (context->TransContext().trTaskId_gg00, tableDesc.GetTableId());
		}
    }
    return Proc::ok; // ignore all errors
}

/*----------------------------------------*/

static void fill_SHOW_SERVER (
    SQLMan_Context * context,
    KSQL_PreparedStatement  * insert)
{
    Proc_ControllerVector allController (context->GetAllocator ());
    Proc_FactoryDirectory::collectAllServerControls (allController);
    for (unsigned int i = 0; i < allController.GetSize (); ++i) {
        Proc_IServerControl * current = allController [i];
        /*
         * get controller values
         */
        const char * kind = current->kind ();
        SAPDB_UInt4 id = current->id ();
        SAPDB_UInt4 sessionCount = current->sessionCount ();
        const char * cmdline = current->cmdline ();
        /*
         * bind parameters
         */
        insert->bindAsciiInput (1, kind, strlen (kind));
        insert->bind (2, id);
        insert->bind (3, sessionCount);
        insert->bindAsciiInput (4, cmdline, strlen (cmdline));
        /*
         * execute
         */
        int rc = insert->execute ();
        if (rc != 0) {
            return;
        }
    }
}

/*----------------------------------------*/

static Proc::Error
SHOW_SERVER (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SHOW_SERVER", DBProc_Trace, 1);
    Proc::Error result;

    result = fillSystemView (context,
        "DROP TABLE TEMP.SHOW_SERVER",
        "CREATE TABLE TEMP.SHOW_SERVER (SERVERKIND CHAR (32), SERVERNO INTEGER,"
            "SESSIONCOUNT INTEGER, COMMANDLINE CHAR (255))",
        "INSERT INTO TEMP.SHOW_SERVER VALUES (?, ?, ?, ?)",
        fill_SHOW_SERVER);
    return result;
}

/*----------------------------------------*/

static void
fill_SHOW_SESSIONS (
    SQLMan_Context * context,
    KSQL_PreparedStatement  * insert)
{
    Proc_ControllerVector allController (context->GetAllocator ());
    Proc_FactoryDirectory::collectAllServerControls (allController);
    for (unsigned int i = 0; i < allController.GetSize (); ++i) {
        Proc_IServerControl * currentControl = allController [i];
        Proc_ServerSessionIterator sessions;
        sessions.initFrom (currentControl);
        /*
         * keys of controller
         */
        const char * kind = currentControl->kind ();
        SAPDB_UInt4 controlId = currentControl->id ();
        /*
         * iterate over sessions
         */
        Proc_ISession * currentSession;
        SAPDB_UInt4 sessionID = 0;
        while (sessions.next (&currentSession)) {
            ++sessionID;
            /*
             * bind parameters
             */
            insert->bindAsciiInput (1, kind, strlen (kind));
            insert->bind (2, controlId);
            insert->bind (3, sessionID);
            /*
             * execute
             */
            int rc = insert->execute ();
            if (rc != 0) {
                return;
            }
        }
    }
}

/*----------------------------------------*/

static Proc::Error
SHOW_SESSIONS (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SHOW_SESSIONS", DBProc_Trace, 1);
    Proc::Error result;

    result = fillSystemView (context,
        "DROP TABLE TEMP.SHOW_SESSIONS",
        "CREATE TABLE TEMP.SHOW_SESSIONS (SERVERKIND CHAR (32), SERVERNO INTEGER,"
            "TASKID INTEGER)",
        "INSERT INTO TEMP.SHOW_SESSIONS VALUES (?, ?, ?)",
        fill_SHOW_SESSIONS);
    return result;
}


/*----------------------------------------*/

static Proc::Error
ZZZ_ARGUMENTTEST (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("ZZZ_ARGUMENTTEST", DBProc_Trace, 1);
    Proc::Error result;

    // get field
    // copy value to outparameter
    result = Proc::ok;
    return result;
}

