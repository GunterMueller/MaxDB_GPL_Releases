/*!***************************************************************************

  module      : SQLMan_SQLStatementContext.hpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : internal call of SQL statements.

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#ifndef SQLMan_INTERNALSQL_HPP
#define SQLMan_INTERNALSQL_HPP

#include "ggg00.h"
#include "gsp00.h"

class  SQLMan_Context;
class  SQLMan_InternalSqlStatement;
struct tsp1_packet;

/*!
   @brief creates new sql manager statement context 
*/

class SQLMan_SQLStatementContext
{
public :
   /*!
      @brief constructor, creates a new statement context. The current context is saved into heap memory 
             and will be reconstructed, when the new constructed instance is destructed
      @param context the current statement context
   */
   SQLMan_SQLStatementContext(SQLMan_Context& context);
   /*!
      @brief destructor, restores the previous statement context
   */
   virtual ~SQLMan_SQLStatementContext();
   /*!
      @brief not yet implemented
   */
   SQLMan_InternalSqlStatement* CreatePreparedStatement(const char* stmt);
   /*!
      @brief returns the current sqlmanager context
   */
   SQLMan_Context& GetContext() const { return m_context; }
private :
   SAPDB_Int4       m_statementId;           //!< stores the previous statement identification
   SAPDB_Int4       m_maxResId;              //!< stores current maximal result id of context
   SQLMan_Context&  m_context;               //!< stores the context given at constructor call
   void*            m_pMessBlock;            //!< stores the current mess block at constructor call
   void*            m_pStatementContext;     //!< stores the statement part of the context at constructor call
   void*            m_pParameterInfo;        //!< stores a handle to parameter descriptions in case of a select into
   void*            m_pColumnInfo;           //!< stores a handle to column descriptions in case of a select into
   tsp00_WarningSet m_warnings;              //!< stores the current warnings
   bool             m_localCacheUsageOnly;   //!< stores the current state of a_localCacheUsageOnly
   tgg00_FileId     m_rollbackFile;
};

#endif
