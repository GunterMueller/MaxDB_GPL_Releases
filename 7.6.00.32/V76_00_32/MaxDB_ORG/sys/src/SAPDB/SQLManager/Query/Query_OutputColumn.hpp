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

#ifndef Query_ColumnOutput_H
#define Query_ColumnOutput_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/SQLMan_Types.hpp"

#include "SAPDB/SQLManager/Query/Query_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_ExpressionNode.hpp"
#include "SAPDB/SQLManager/Query/Query_Expression.hpp"
#include "SAPDB/SQLManager/Query/Query_Column.hpp"
#include "SAPDB/SQLManager/Query/Query_ColumnSpec.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"

class Query_NodeMapper;
class Query_QuantifierMapper;
class Query_NodeSelect;
class Query_SyntaxTree;
class Query_QueryGraph;
class SAPDBMem_IRawAllocator;

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_OutColType
    @brief          ...
 */
class Query_OutColType
{
public:
    /// ...
    enum Query_Main_OutColType  {
                                OCTM_Column,             // something like 'select a from t'
                                OCTM_Asterisk,           //                'select t1.* from t1,t2'
                                OCTM_MultiAsterisk,      //                'select * from t1,t2'
                                //
                                OCTM_Subquery,           //                'select (select a from t) from t'
                                OCTM_Aggregate,          //                'select max(X) from t'
                                OCTM_Expression,         //                'select X+Y from t'
                                //                      // whatever X,Y are !
                                OCTM_Unknown
                                };

    /// ...
    enum {  OCTS_ExpressionContainsAggr        = 1,
            OCTS_ExpressionContainsSubquery    = 2,
            OCTS_AggregationContainsExpression = 4,
            OCTS_AggregationContainsSubquery   = 8};

    /// ...
    Query_OutColType( void ) 
        : MainType(OCTM_Unknown)
        , SubType(0)
    {};

    /// ...
    bool ContainsAggregate( void )
    {
        return (MainType == OCTM_Aggregate) || (SubType & OCTS_ExpressionContainsAggr);
    }

    /// ...
    bool ContainsSubquery( void )
    {
        return (MainType == OCTM_Subquery) || (SubType & OCTS_ExpressionContainsSubquery) || (SubType & OCTS_AggregationContainsSubquery);
    }

    /// Main type
    Query_Main_OutColType   MainType;   
    /// Sub type; Bits are set with OCTS_*
    SAPDB_Int1              SubType;    
};

/*!
    @class          Query_OutputColumn
    @brief          ...
 */
class Query_OutputColumn : public Query_ColumnSpec
{
private:    // private copyconstuctor used by CreateDuplicate
    /// ...
    Query_OutputColumn( const Query_OutputColumn& C, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
public:

    /// ...
    Query_OutputColumn( void );
    /// ...
    Query_OutputColumn( SyntaxTreeNodeNo Node );
    /// ...
    ~Query_OutputColumn( void );
    /// ...
    void Destroy( SAPDBMem_IRawAllocator& Alloc );

    /// ...
    virtual Query_OutputColumn* CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );

    /// ...
    
    virtual bool Parse( Query_SelectNode* Father, Query_OutputColumn* /*OutCol*/, Query_Position /*Position*/, SyntaxTreeNodeNo Node, bool ComplexViewReplacement );
    /// ...
    virtual bool Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy );
    /// ...
            bool SemanticsMultiAsterisk( Query_SelectNode* Father );
    /// ...
            bool SemanticsAsterisk( Query_SelectNode* Father );

    /// ...
    bool AppendColumn( SAPDBMem_IRawAllocator& Alloc, Query_Column& Col );
    /// ...
    bool AppendColumn( Query_Expression& Expr );

	/// ...
    inline const SQLMan_Identifier& GetReferenceName( void )   { return mRefName;      }
    /// ...
    inline bool HasReferenceName( void )                { return !mRefName.IsBlank(); }

    /// ...
    inline SAPDB_Int2 GetColumnCount( void )            { return mColumnCount;  }
    /// ...
    inline void       SetColumnCount( SAPDB_Int2 c )            { mColumnCount = c;  }
    /// ...
    inline bool       IsSingleColumn( void )            { return mColumnCount==1; }

    /// ...
    inline bool IsAsterisk ( void )                     
    {   
        return mOutColType.MainType==Query_OutColType::OCTM_Asterisk 
            || mOutColType.MainType==Query_OutColType::OCTM_MultiAsterisk; 
    }

    /// ...
    inline bool IsColumnOrSingleAsterisk( void )
    {
        return mOutColType.MainType==Query_OutColType::OCTM_Column 
            || mOutColType.MainType==Query_OutColType::OCTM_Asterisk;
    }

// ---------
    /// ...
    inline SyntaxTreeNodeNo GetQuantifierID( void )   
    { 
        SAPDBERR_ASSERT_STATE( IsColumnOrSingleAsterisk() && mExpression.GetRoot() );

        if ( IsColumnOrSingleAsterisk() && mExpression.GetRoot() )
        {
            return (reinterpret_cast<Query_Column*>(mExpression.GetRoot()))->GetQuantifierID();
        }
        return cak_qrewrite_ref_init;    
    }
    /// ...
    inline SyntaxTreeNodeNo GetQuantifierSyntaxNode( void )   
    { 
        if ( IsColumnOrSingleAsterisk() && mExpression.GetRoot() )
        {
            return (reinterpret_cast<Query_Column*>(mExpression.GetRoot()))->GetQuantifierSyntaxNode();
        }
        return cak_qrewrite_ref_init;    
    }
    /// ...
    inline SAPDB_Int2 GetLogicalColumnIndex( void )     
    { 
        SAPDBERR_ASSERT_STATE( IsColumnOrSingleAsterisk() && mExpression.GetRoot() );

        if ( IsColumnOrSingleAsterisk() && mExpression.GetRoot() )
        {
            return (reinterpret_cast<Query_Column*>(mExpression.GetRoot()))->GetLogicalColumnIndex();
        }
        return cak_qrewrite_ref_init;  
    }
    /// ...
    inline SAPDB_Int2 GetExtColNo( void )				
    { 
        SAPDBERR_ASSERT_STATE( IsColumnOrSingleAsterisk() && mExpression.GetRoot() );

        if ( IsColumnOrSingleAsterisk() && mExpression.GetRoot() )
        {
            return (reinterpret_cast<Query_Column*>(mExpression.GetRoot()))->GetExtColNo();
        }
        return cak_qrewrite_ref_init;  
    }
    /// ...
    inline void GetColumnName( SQLMan_Identifier& Name )      
    { 
        SAPDBERR_ASSERT_STATE( IsColumnOrSingleAsterisk() && mExpression.GetRoot() );

        if ( IsColumnOrSingleAsterisk() && mExpression.GetRoot() )
            Name = (reinterpret_cast<Query_Column*>(mExpression.GetRoot()))->GetColumnName();
        else
            Name.SetBlank();      
    }
    /// ...
    inline const SQLMan_Identifier& GetIdentificationName( void )
    {
        SAPDBERR_ASSERT_STATE( mColumnCount && !IsAsterisk() );

        if ( mOutColType.MainType==Query_OutColType::OCTM_Column && mRefName==SQLMan_Identifier() )
            if ( mExpression.GetRoot() )
            {
                return (reinterpret_cast<Query_Column*>(mExpression.GetRoot()))->GetColumnName();
            }

        return mRefName;
    }
    /// ...
    inline bool IsNullable( void )
    {
        if ( mOutColType.MainType==Query_OutColType::OCTM_Column )
            if ( mExpression.GetRoot() )
                return (reinterpret_cast<Query_Column*>(mExpression.GetRoot()))->IsNullable();
        return true;
    }

// ---------

    /// ...
	inline void SetReferenceName( const SQLMan_Identifier& RefName )
	{
		mRefName = RefName;
	}

    /// ...
    void RenewReferenceName( Query_QueryGraph& Graph );

    /// ...
    inline void SetColType( Query_OutColType::Query_Main_OutColType MainType, SAPDB_Int1 SubType, SAPDB_Int2 ColCount )
    {
        if (   MainType == Query_OutColType::OCTM_Column 
            || MainType == Query_OutColType::OCTM_Asterisk 
            || MainType == Query_OutColType::OCTM_MultiAsterisk  )
        {
            SAPDBERR_ASSERT_ARGUMENT( SubType == 0 );
            SubType = 0;
        }

        if (   MainType!=Query_OutColType::OCTM_Asterisk 
            && MainType!=Query_OutColType::OCTM_MultiAsterisk )
        {
            SAPDBERR_ASSERT_ARGUMENT( ColCount == 1 );
            ColCount = 1;
        }

        mOutColType.MainType  = MainType;
        mOutColType.SubType   = SubType;
        mColumnCount = ColCount;
    }

    /// ...
    inline Query_OutColType&     GetColType( void )         { return mOutColType;          }
    /// ...
    inline Query_OutColType::Query_Main_OutColType GetMainColType( void )     { return mOutColType.MainType; }
    /// ...
    inline SAPDB_Int1&           GetSubColType( void )      { return mOutColType.SubType;  }

    /// ...
    inline SAPDB_Int2 GetRefCount( void )
    {
        return mRefCount;
    }

    /// ...
    inline void AddRefCount( SAPDB_Int2 Count )
    {
        mRefCount += Count;
    }

    /// ... 
    inline void IncRefCount( void )
    {
        ++mRefCount;
    }

    /// ... 
    inline void DecRefCount( void )
    {
        SAPDBERR_ASSERT_STATE( mRefCount>0 );
        --mRefCount;
    }

    /// ...
    void IncRefCountsToChildColumns( Query_Node* Node );
    /// ...
    void DelRefCountsToChildColumns( Query_Node* Node );

private:
    /// Reference name for the output column
    SQLMan_Identifier       mRefName;       
    /// ...
    SAPDB_Int2              mColumnCount;
    /// ...
    SAPDB_Int2              mRefCount;
    /// ...
    Query_OutColType        mOutColType;
};

// ------------------------------------------------------------------------------------------------

#endif