/*!
  @file           SQLMan_ContextSessionMembers.hpp
  @author         ThomasA
  @special area   SQL Manager
  @brief          defines the c++ sesion members of SQLMan_Context
  @see            example.html ...
  @first created  000-03-09  18:21

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


#ifndef SQLMAN_CONTEXTSESSIONMEMBERS_HPP
#define SQLMAN_CONTEXTSESSIONMEMBERS_HPP

#include "SQLManager/Catalog/Catalog_SessionCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SQLManager/SQLMan_ActiveStatementDirectory.hpp"
#include "SQLManager/SQLMan_TempFileCollection.hpp"
#include "SQLManager/SQLMan_SessionInfo.hpp"
#include "SystemViews/SysView_PseudoLongHandler.hpp"
#include "SQLManager/KernelSQL/KSQL_Runtime.hpp"
#include "SAPDB/Messages/Msg_List.hpp"


class DBProc_Debugger;

class SQLMan_Context;

class Proc_SessionList;

class SQLMan_SchemaMap;

/*!
   @brief defines the cpp members of a SQLMan_Context.
          Due to the history of SAPDB the initial part of the SQLMan_Context is defined
          via PASCAL. For the new implemented parts, the cpp memberes are clustered
          by this class and are linked to the PASCAL part by use of a pointer.
*/
class SQLMan_ContextSessionMembers
{
public :
    /*!
       @brief contructor
    */
    SQLMan_ContextSessionMembers(SQLMan_Context& context);
    /*!
       @brief destructor
    */
    ~SQLMan_ContextSessionMembers();
    /*!
       @brief session catalog cache
    */
    Catalog_SessionCache   m_sessionCache;
    /*!
       @brief shared sql session mamanger
    */
    SharedSQL_SQLCache m_sharedSQL_SQLCache;
    /*!
       @brief directory of active sql statements
    */
    SQLMan_ActiveStatementDirectory m_activeStatementDirectory;
    /*!
       @brief dbproc debug context
    */
    DBProc_Debugger* m_dbprocDebugger;
    /*!
       @brief handler for pseudo long columns of system views
    */
    SysView_PseudoLongHandler m_pseudoLongHandler;
    /*!
       @brief runtime interface for sqldbc
     */
    KSQL_Runtime m_sqlRuntime;
    /*!
       @brief handler for grouped temporary files
    */
    SQLMan_TempFileCollection m_tempFileCollection;
    /*!
       @brief sessions in UDE servers
    */
    Proc_SessionList * UDESessions ();
    /*!
      @brief  session info object
    */
    SQLMan_SessionInfo m_sessionInfo;
    /*!
      @brief schema mapping table
     */
    SQLMan_SchemaMap*  m_schemaMap;
    /*!
       @brief deletes the current schema map
     */
    void DestroySchemaMap();
    /*!
      @brief returns a pointer to the schema map
     */
    SQLMan_SchemaMap* GetSchemaMap();
private :
    SQLMan_Context& m_context; //!< reference to the session context (acv)
    Proc_SessionList * m_udeSessions;
};

#endif
