/*!***************************************************************************

    @file     Query_NodeRoot.hpp
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

#ifndef Query_NodeRoot_H
#define Query_NodeRoot_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/Query/Query_Node.hpp"

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_RootNode
    @brief          Node that represents the root of a QueryGraph
    @copydoc        Query_Node
 */
class Query_RootNode : public Query_Node
{
protected:    // private copyconstuctor used by CreateDuplicate
    /*!
        @brief  Protected constructor called by CreateDuplicates
        @param  N       [in]
        @param  Father  [in]
        @param  Q       [in]
        @param  mpMap   [in]
        @param  mpQMap  [in]
    */
    Query_RootNode( const Query_RootNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
public:
    virtual Query_Node* CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );

    /*!
        @brief  Constructor
        @param  Context    [in]
        @param  QueryGraph [in]
    */    
    Query_RootNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph );
    /*!
        @brief  Constructor
        @param  AnyOtherNode [in]
    */    
	Query_RootNode( Query_Node* AnyOtherNode );

	virtual NodeType GetNodeType( void ) { return Top; }
    virtual void PrintToTrace( Kernel_VTrace& VTrace );

//    virtual SAPDB_Int2 OutputColumns( void );
//    virtual bool GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName );

    virtual bool Parse( bool ComplexViewReplacement );
			bool AddQuery( SyntaxTreeNodeNo N, bool ComplexViewReplacement );
			bool AddInsert( SyntaxTreeNodeNo N, bool ComplexViewReplacement );
			bool AddUpdate( SyntaxTreeNodeNo N, bool ComplexViewReplacement );
			bool AddDelete( SyntaxTreeNodeNo N, bool ComplexViewReplacement );
    virtual bool Semantics( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement )
	{
		return Query_Node::Semantics(UserId,UniqueOutputColumnNames, ComplexViewReplacement);
	}

private:
    bool mIsExplain;
    //Query_Node*                mInput;
    //Query_Node*                mAccess;
    //Query_Node*                mOutput;
};

// ------------------------------------------------------------------------------------------------

#endif