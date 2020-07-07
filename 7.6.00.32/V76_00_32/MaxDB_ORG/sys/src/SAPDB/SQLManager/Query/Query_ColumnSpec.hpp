/*!***************************************************************************

    @file     Query_OutputColumn.hpp
    @ingroup  Query
    @author   DirkT
    @brief    
    @see  

  -------------------------------------------------------------------------

  responsible : DirkT

  special area: SQL Manager / Query
  description : 

  last changed: 2005-06-08  12:00
  first created:2004-04-29  10:00

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

#ifndef Query_OutputColumn_H
#define Query_OutputColumn_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/SQLMan_Types.hpp"

#include "SAPDB/SQLManager/Query/Query_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_ExpressionNode.hpp"
#include "SAPDB/SQLManager/Query/Query_Expression.hpp"
#include "SAPDB/SQLManager/Query/Query_Column.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"

class Query_NodeMapper;
class Query_QuantifierMapper;
class Query_SelectNode;
class Query_SyntaxTree;
class Query_QueryGraph;
class SAPDBMem_IRawAllocator;
class Query_OutputColumn;

/*!
    @class          Query_ColumnSpec
    @brief          ...
 */
class Query_ColumnSpec
{
private:    // private copyconstuctor used by CreateDuplicate
    /// ...
    Query_ColumnSpec( const Query_ColumnSpec& C, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
public:

    /// ...
    Query_ColumnSpec( void );
    /// ...
    Query_ColumnSpec( SyntaxTreeNodeNo Node );
    /// ...
    virtual ~Query_ColumnSpec( void );
    /// ...
    void Destroy( SAPDBMem_IRawAllocator& Alloc );

    /// ...
    virtual Query_ColumnSpec* CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );

    /// ...
    virtual bool Parse( Query_SelectNode* Father, Query_OutputColumn* OutCol, Query_Position Position, SyntaxTreeNodeNo Node, bool ComplexViewReplacement );
    /// ...
    virtual bool Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy );
    /// ...
    virtual bool CheckCorrelatedColumnsForExistToIn( Query_SelectNode* Father );
    /// ...
    inline SyntaxTreeNodeNo GetSyntaxNode( void )       { return mSyntaxNode;   }

    /// ...
	inline void SetSyntaxNode( const SyntaxTreeNodeNo SyntaxNode )
	{
		mSyntaxNode = SyntaxNode;
	}

    /// ...
    inline Query_Expression& GetExpression( void )
    {
        return mExpression;
    }

protected:
    /// ...
    SyntaxTreeNodeNo        mSyntaxNode;   
	/// Expression that specifies the column
    Query_Expression        mExpression;   
};

// ------------------------------------------------------------------------------------------------

#endif