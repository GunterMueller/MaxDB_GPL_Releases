/*!***************************************************************************

    @file     Query_Quantifier.hpp
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

#ifndef Query_Quantifier_H
#define Query_Quantifier_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/SQLMan_Types.hpp"

#include "SAPDB/SQLManager/Query/Query_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_Node.hpp"
#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/Container/Container_Vector.hpp"

// ------------------------------------------------------------------------------------------------

class   Query_Expression;

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_Quantifier
    @brief          ...
 */
class Query_Quantifier
{
private:    // private copyconstuctor used by CreateDuplicate
    /// ...
    Query_Quantifier( const Query_Quantifier& Q, Query_Node* Father, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
        : mRef(0)
        , mDistinctType(Q.mDistinctType)
        , mQuantifierType(Q.mQuantifierType)
        , mDisabled(Q.mDisabled)
    {
        if ( Q.mRef )
            mRef = Q.mRef->CreateDuplicate(Father, this, mpMap, mpQMap);
    }
public:
    /// ...
    Query_Quantifier( void )
        : mRef(0)
        , mDistinctType(DT_Unknown)
        , mQuantifierType(QT_Unknown)
        , mDisabled(false)
    {
    }

    /// ...
    Query_Quantifier( Query_Node* Ref, Query_DistinctType DistinctType, Query_QuantifierType QuantifierType )
        : mRef(Ref)
        , mDistinctType(DistinctType)
        , mQuantifierType(QuantifierType)
        , mDisabled(false)
    {
        mRef->FatherQuantifier() = this;
    }

    /// ...
    ~Query_Quantifier( void )
    {
	    if ( mRef )
	    {
		    SAPDBMem_IRawAllocator& Alloc = mRef->GetAllocator();
		    destroy(mRef, Alloc);
            mRef = 0;
	    }
    }

    /// ...
    bool Parse( Query_Node* FatherNode, SyntaxTreeNodeNo NodeNo );

    /// ...
    inline Query_Quantifier* CreateDuplicate( Query_Node* Father, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );

    /// ...
    inline bool ReplaceComplexViews( void );

    /// ...
    inline SAPDB_Int2 OutputColumns( void );
    /// ...
	inline SAPDB_Int2 GetOutputColumnSize( SAPDB_Int2 LogColIndex );
    /// ...
    inline bool GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName, bool& IsNullable );
    /// ...
    inline bool GetOutputColumnExtNo( SAPDB_Int2 LogColIndex, SAPDB_Int2& ExtColNo );
    /// ...
    inline bool CheckForColumn( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    /// ...
    inline bool CheckForColumn( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    /// ...
    inline bool CheckQuantifierName( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table );
    /// ...
    inline void GetIdentificationNames( SQLMan_Identifier& Schema, SQLMan_Identifier& Table );
    /// ...
	inline bool HasReferenceName( void );
    /// ...
    inline bool IqualIdentificationNames(Query_Quantifier*& Q);

    /// ...
	inline bool SelectPrivilegesForAllColumns( void );
    /// ...
	inline bool HasSysKey( void );

    /// ...
	inline SAPDB_Int2 ExtColNo2LogicalColNo( SAPDB_Int2 ColumnIndex );
    /// ...
	inline SAPDB_Int2 LogicalColNo2ExtColNo( SAPDB_Int2 ColumnIndex );

    /// ...
    inline bool PullDownSearchCond( Query_Expression& SearchCondition, SyntaxTreeNodeNo SearchCond_AP, SAPDB_Int2 QID, bool MustCreateCopy );

    /// ...
	inline bool OneTupleCondition( void );
    /// ...
    inline bool NoDuplicateCondition( void ); 

    /// ...
    inline SAPDB_UInt2              NodeID( void );
    /// ...
    inline SyntaxTreeNodeNo         SyntaxNode( void );
    /// ...
    inline Query_Node::NodeType	    NodeType( void );
    /// ...
    inline bool                     NodeTypeTable( void );
    /// ...
    inline bool                     NodeTypeViewToBeReplaced( void );
    /// ...
    inline bool                     NodeTypeSelect( void );
    /// ...
    inline bool                     NodeTypeSelectOrSet( void );
    /// ...
    inline bool                     NodeTypeSet( void );
    /// ...
	inline bool                     FromTableSpec( void );
    /// ...
    inline bool                     SpecialJoinFromSelect( void );
    /// ...
	inline Query_JoinType			JoinType( void );
    /// ...
    inline bool                     OuterJoin( void );
    /// ...
    inline bool                     Aggregates( void );

    /// ...
    inline Query_Node*&			    NodeRef( void )						{ return mRef; }
    /// ...
	inline void                     SetNodeRef( Query_Node* Ref )		{ mRef = Ref; }
    /// ...
    inline Query_DistinctType&      DistinctType( void )				{ return mDistinctType; }
    /// ...
    inline Query_QuantifierType&    QuantifierType( void )				{ return mQuantifierType; }
    /// ...
    inline bool                     IsDisabled( void )                  { return mDisabled; }
    /// ...
    inline void                     Disable( void )                     { mDisabled = true; }
    /// ...
    inline void                     Enable( void )                      { mDisabled = false; }

private:
    Query_Node*             mRef;               /// ...
    Query_DistinctType      mDistinctType;      /// ...
    Query_QuantifierType    mQuantifierType;    /// ...
    bool                    mDisabled;          /// ...
};

// ------------------------------------------------------------------------------------------------

//Query_Quantifier::Query_Quantifier( void )
//    : mRef(0)
//    , mDistinctType(DT_Unknown)
//    , mQuantifierType(QT_Unknown)
//    , mDisabled(false)
//{
//}

//Query_Quantifier::Query_Quantifier( Query_Node* Ref, Query_DistinctType DistinctType, Query_QuantifierType QuantifierType )
//    : mRef(Ref)
//    , mDistinctType(DistinctType)
//    , mQuantifierType(QuantifierType)
//    , mDisabled(false)
//{
//    mRef->FatherQuantifier() = this;
//}

//Query_Quantifier::Query_Quantifier( const Query_Quantifier& Q, Query_Node* Father, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
//    : mRef(0)
//    , mDistinctType(Q.mDistinctType)
//    , mQuantifierType(Q.mQuantifierType)
//    , mDisabled(Q.mDisabled)
//{
//    if ( Q.mRef )
//        mRef = Q.mRef->CreateDuplicate(Father, this, mpMap, mpQMap);
//}

//Query_Quantifier::~Query_Quantifier( void )
//{
//	if ( mRef )
//	{
//		SAPDBMem_IRawAllocator& Alloc = mRef->GetAllocator();
//		destroy(mRef, Alloc);
//        mRef = 0;
//	}
//}

bool Query_Quantifier::ReplaceComplexViews( void )
{
    return mRef->ReplaceComplexViews();
}

Query_Quantifier* Query_Quantifier::CreateDuplicate( Query_Node* Father, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    return new(Father->GetAllocator()) Query_Quantifier(*this, Father, mpMap, mpQMap );
}

bool Query_Quantifier::OneTupleCondition( void )
{
    return mRef->OneTupleCondition();
}

bool Query_Quantifier::NoDuplicateCondition( void )
{
    return mRef->NoDuplicateCondition();
}

SAPDB_UInt2 Query_Quantifier::NodeID( void )
{
    return mRef->NodeID();
}

SyntaxTreeNodeNo Query_Quantifier::SyntaxNode( void )
{
    return mRef->SyntaxNode();
}


Query_Node::NodeType Query_Quantifier::NodeType( void )
{
    return mRef->GetNodeType();
}

bool Query_Quantifier::NodeTypeSelect( void )
{
    return (mRef->GetNodeType() == Query_Node::Select);
}

bool Query_Quantifier::NodeTypeTable( void )
{
	return (mRef->GetNodeType()==Query_Node::TableAccess);
}

bool Query_Quantifier::NodeTypeViewToBeReplaced( void )
{
    return mRef->IsViewToBeReplaced();
}

bool Query_Quantifier::NodeTypeSelectOrSet( void )
{
    return mRef->IsSelectOrSet();
}
bool Query_Quantifier::NodeTypeSet( void )
{
    Query_Node::NodeType T = mRef->GetNodeType();
    return (( T == Query_Node::Union )     ||
            ( T == Query_Node::Intersect ) ||
            ( T == Query_Node::Difference ));
}

bool Query_Quantifier::FromTableSpec( void )
{
	return ( mRef->Position() == P_FromPart );
}

bool Query_Quantifier::SpecialJoinFromSelect( void )
{
    return mRef->IsSpecialJoinFromSelect();
}

bool Query_Quantifier::HasSysKey( void )
{
	return mRef->HasSysKey();
}

Query_JoinType Query_Quantifier::JoinType( void )
{
    return mRef->GetJoinType();
}

bool Query_Quantifier::OuterJoin( void )
{
    return mRef->OuterJoin();
}

bool Query_Quantifier::Aggregates( void )
{
    return mRef->Aggregates();
}

SAPDB_Int2 Query_Quantifier::OutputColumns( void )
{
    return mRef->OutputColumns();
}

SAPDB_Int2 Query_Quantifier::ExtColNo2LogicalColNo( SAPDB_Int2 ColumnIndex )
{
   	return mRef->ExtColNo2LogicalColNo(ColumnIndex);
}

SAPDB_Int2 Query_Quantifier::LogicalColNo2ExtColNo( SAPDB_Int2 ColumnIndex )
{
    return mRef->LogicalColNo2ExtColNo(ColumnIndex);
}

bool Query_Quantifier::PullDownSearchCond( Query_Expression& SearchCondition, SyntaxTreeNodeNo SearchCond_AP, SAPDB_Int2 QID, bool MustCreateCopy )
{
	return mRef->PullDownSearchCond(SearchCondition, SearchCond_AP, QID, MustCreateCopy);
}

SAPDB_Int2 Query_Quantifier::GetOutputColumnSize( SAPDB_Int2 LogColIndex )
{
	return mRef->GetOutputColumnSize(LogColIndex);
}

bool Query_Quantifier::GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName, bool& IsNullable )
{
    return mRef->GetOutputColumnName(LogColIndex, ColName, IsNullable);
}

bool Query_Quantifier::GetOutputColumnExtNo( SAPDB_Int2 LogColIndex, SAPDB_Int2& ExtColNo )
{
	bool rc = mRef->GetOutputColumnExtNo(LogColIndex, ExtColNo);
	if ( FromTableSpec() )
	{
		if ( NodeTypeSelectOrSet() || mRef->IsViewToBeReplaced() ) // (NodeTypeTable() && mRef->HasSysKey()) )
			ExtColNo=LogColIndex+1;	// adjust columnindex as there will be a syskey
	}
	return rc;
}

bool Query_Quantifier::CheckForColumn( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    bool rc = mRef->CheckForColumn(Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
	if ( rc && FromTableSpec() )
	{
		if ( NodeTypeSelectOrSet() || mRef->IsViewToBeReplaced() ) // (NodeTypeTable() && mRef->HasSysKey()) )
			ExtColNo=LogColumnIndex+1;	// adjust columnindex as there will be a syskey
	}
	return rc;
}

bool Query_Quantifier::CheckForColumn( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    bool rc = mRef->CheckForColumn(Schema, Table, Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
	if ( rc && FromTableSpec() )
	{
		if ( NodeTypeSelectOrSet() || mRef->IsViewToBeReplaced() ) // (NodeTypeTable() && mRef->HasSysKey()) )
			ExtColNo=LogColumnIndex+1;	// adjust columnindex as there will be a syskey
	}
	return rc;
}

bool Query_Quantifier::CheckQuantifierName( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table)
{
    return mRef->CheckQuantifierName(Schema, Table);
}

void Query_Quantifier::GetIdentificationNames( SQLMan_Identifier& Schema, SQLMan_Identifier& Table )
{
    return mRef->GetIdentificationNames(Schema, Table);
}

bool Query_Quantifier::HasReferenceName( void )
{
	return mRef->HasReferenceName();
}

bool Query_Quantifier::IqualIdentificationNames( Query_Quantifier*& Q )
{
    SQLMan_Identifier S1(false), S2(false), T1(false), T2(false);
    GetIdentificationNames(S1, T1);
    Q->GetIdentificationNames(S2, T2);

	if ( !S1.IsBlank() && !S2.IsBlank() )
		return ((S1==S2) && (T1==T2));
	if ( !T1.IsBlank() || !T2.IsBlank() )
	    return (T1==T2);
	return false;
}

bool Query_Quantifier::SelectPrivilegesForAllColumns( void )
{
	return mRef->SelectPrivilegesForAllColumns();
}

// ------------------------------------------------------------------------------------------------

#endif