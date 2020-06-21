/*!***************************************************************************

  module      : SQLMan_ActiveStatementDirectory.hpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : directory of currently active sql statements in session
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

#ifndef SQLMAN_ACTIVESTATEMENTDIRECTORY_HPP
#define SQLMAN_ACTIVESTATEMENTDIRECTORY_HPP

#include "Container/Container_AVLTree.hpp"

class SAPDBMem_IRawAllocator;

/*!
   @brief provides a directory for active sql statements of a session. The statements are
          identified via a unique number. Multiple sql statements may be active at a time in case
          of triggers or db-procedures.
*/
class SQLMan_ActiveStatementDirectory
{
public :
    typedef SAPDB_Int4 StatementId;  //!< identification of a sql statement
    
    /*!
       @brief constructor 
       @param alloc allocator to be used for memory allocation
    */
    SQLMan_ActiveStatementDirectory(SAPDBMem_IRawAllocator& alloc);
    
    /*!
       @brief destructor
    */
    ~SQLMan_ActiveStatementDirectory();
    
    /*!
       @brief  tells whether a given statement is currently active, i.e. has been started but
               is not finished yet.
       @param  statementId the identification of the sql statement
       @return true, if statementId is stored in the directory; otherwise false
    */
    bool IsActive(StatementId statementId) const;
    
    /*!
       @brief  stores a given statement in the directory
       @param  statementId the identification of the sql statement
       @return true, if the statement has been stored successfully; otherwise false (out of memory)  
    */
    bool RegisterStatement(StatementId statementId);
    
    /*!
       @brief  removes a given statement from the directory
       @param  statementId the identification of the sql statement
    */
    void UnregisterStatement(StatementId statementId);

    /*!
       @brief empties the directory
    */
    void UnregisterAll();

private :
    typedef Container_AVLNodeComparator<StatementId>   Comparator; //!< comparator for statement identifications
    typedef Container_AVLNode<StatementId, Comparator> Node;       //!< node of the avl tree implementing the directory

    static Comparator                              m_comparator;       //!< singleton instance of the comparator 
    Container_AVLTree<Node,StatementId,Comparator> m_activeStatements; //!< avl tree storing the active statements
};

#endif
