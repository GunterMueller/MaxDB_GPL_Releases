/*!***************************************************************************

    @file     Query_NodeTable.hpp
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

#ifndef Query_NodeTable_H
#define Query_NodeTable_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/Query/Query_Node.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp" 

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_TableColumn
    @brief          Column of some TABLE
 */
class Query_TableColumn 
{
public:
    /// ...
    Query_TableColumn( SAPDB_Int2 Len, SAPDB_Int2 Log, SAPDB_Int2 Ext, const SQLMan_Identifier& Name, bool Nullable, bool IsSysKey, bool IsVisible )
        : mColLength(Len)
		, mLogColNo(Log)
        , mExtColNo(Ext)
        , mRefCount(0)
        , mColName(Name)
        , mIsNullable(Nullable)
        , mIsSysKey(IsSysKey)
        , mIsVisible(IsVisible)
    {
    };

    /// ...
    const SQLMan_Identifier& ColName( void )    { return mColName;  };
    /// ...
    SAPDB_Int2&        ColLength( void )		{ return mColLength; };
    /// ...
    SAPDB_Int2&        LogColNo( void )			{ return mLogColNo; };
    /// ...
    SAPDB_Int2&        ExtColNo( void )			{ return mExtColNo; };
    /// ...
    bool&              IsNullable( void )		{ return mIsNullable; };
    /// ...
    bool               IsSysKey( void )			{ return mIsSysKey; }
    /// ...
    bool               IsVisible( void )		{ return mIsVisible; }

    /// ...
    void               IncRefCount( void ) { ++mRefCount; }
    /// ...
    void               DecRefCount( void ) { --mRefCount; }
    /// ...
    SAPDB_Int2         GetRefCount( void ) { return mRefCount; }

private:    
    /// ...
    SQLMan_Identifier   mColName;       
    /// ...
	SAPDB_Int2          mColLength;     
    /// ...
    SAPDB_Int2          mLogColNo;      
    /// ...
    SAPDB_Int2          mExtColNo;      
    /// ...
    SAPDB_Int2          mRefCount;      
    /// ...
    bool                mIsNullable;    
    /// ...
    bool                mIsSysKey;      
    /// ...
    bool                mIsVisible;  
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_TableNode
    @brief          Node that represents a TABLE
    @copydoc        Query_Node
 */
class Query_TableNode : public Query_Node
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
    Query_TableNode( const Query_TableNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
public:
    /// ...
    enum Type { T_BaseTable, T_Synonym, T_Cursor, T_View, T_JoinView, T_ComplexView, T_Unknown };

    virtual Query_Node* CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );

    /*!
        @brief  Constructor
        @param  Context    [in]
        @param  QueryGraph [in]
        @param  Position   [in]
        @param  APNode     [in]
        @param  ComplexViewReplacement [in]
    */     
    Query_TableNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, Query_Position Position, SyntaxTreeNodeNo APNode, bool ComplexViewReplacement );

    virtual NodeType GetNodeType( void ) { return TableAccess; }
    virtual void PrintToTrace( Kernel_VTrace& VTrace );

    virtual bool Parse( bool ComplexViewReplacement );
    virtual bool Semantics( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement );  

	virtual bool IsViewToBeReplaced( void );

    virtual bool HasSysKey( void )
    {
	    return mSysKey;
    }

	virtual bool SelectPrivilegesForAllColumns( void )
    {
        return mSelectPrivForAllColumns;
    }

	virtual bool ReplaceComplexViews( void );

	virtual SAPDB_Int2 ExtColNo2LogicalColNo( SAPDB_Int2 ColumnIndex );
	virtual SAPDB_Int2 LogicalColNo2ExtColNo( SAPDB_Int2 ColumnIndex );

    virtual SAPDB_Int2 OutputColumns( void );
	virtual SAPDB_Int2 GetOutputColumnSize( SAPDB_Int2 LogColIndex );
    virtual bool GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName, bool& IsNullable );
    virtual bool GetOutputColumnExtNo( SAPDB_Int2 LogColIndex, SAPDB_Int2& ColumnIndex );
    virtual bool CheckForColumn( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    virtual bool CheckForColumn( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    virtual bool CheckQuantifierName( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table );
    virtual void GetIdentificationNames(SQLMan_Identifier& Schema, SQLMan_Identifier& Table);
	virtual bool HasReferenceName( void );
    virtual bool DecRefCountForOutputColumn( SAPDB_Int2 ColNo );
    virtual bool IncRefCountForOutputColumn( SAPDB_Int2 ColNo );
    virtual bool IncRefCountForOutputColumn( SAPDB_Int2 QId, SAPDB_Int2 ColNo ) { return true; } // just to make the sun compiler happy
    virtual bool DecRefCountForOutputColumn( SAPDB_Int2 QId, SAPDB_Int2 ColNo ) { return true; } // just to make the sun compiler happy
    virtual bool AddRefCountForOutputColumn( SAPDB_Int2 i, SAPDB_Int2 Count );

    virtual bool DefineUniqueOutput(Container_Vector<SAPDB_Int2>& LogOutColNr);

private: // ???

    /// ...
    inline bool BasicTableName( SQLMan_Identifier& SchemaName, SQLMan_Identifier& TableName )
    {
        SAPDBTRACE_METHOD_DEBUG ("Query_TableNode::TableName", QueryRewrite_Trace, 6);

        if ( mTableDescriptor )
        {
            mTableDescriptor.GetTableName(TableName);
            mTableDescriptor.GetSchemaName(SchemaName);
            return true;
        }
        return false;
    }

    /// ...
    inline bool TableName( SQLMan_Identifier& SchemaName, SQLMan_Identifier& TableName )
    {
        SAPDBTRACE_METHOD_DEBUG ("Query_TableNode::TableName", QueryRewrite_Trace, 6);

        SchemaName = mSchemaName;
        TableName  = mTableName;
        return true;
    }

    /// ...
    inline Catalog_ObjectId TabID( void )
    {
        return mTableDescriptor.GetTableId();
    }

    /// ...
    SAPDB_Int2 CollectColumnInfo( bool IsView, bool ToBeReplaced );

private:

    /// ...
    Catalog_TableDescriptor mTableDescriptor;           
    /// ...
    Container_Vector<Query_TableColumn> mTableColumns;  
    /// ...
	SQLMan_Identifier       mSchemaName;                
    /// ...
	SQLMan_Identifier       mTableName;                 
    /// ...
    SQLMan_Identifier       mReferenceName;             
    /// ...
    SAPDB_Int2              mOutputColumnCnt;           
    /// ...
	bool					mSelectPrivForAllColumns;   
    /// ...
    bool                    mExplicitSchema;            
    /// ...
    Type                    mType;                      
    /// ...
	bool					mReferencedViaSynonym;      
    /// ...
	bool                    mComplexViewReplacement;    
    /// ...
    bool                    mSysKey;                    
    /// ...
    bool                    mPrimaryKey;                
    /// ...
    bool                    mUniqueIndex;               
};

// ------------------------------------------------------------------------------------------------

#endif