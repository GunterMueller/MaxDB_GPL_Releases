/*!***************************************************************************

    @file     Query_NodeTable.cpp
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

#include "SAPDB/SQLManager/Query/Query_NodeTable.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"

#include "SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp"

// ------------------------------------------------------------------------------------------------

Query_TableNode::Query_TableNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, Query_Position Position, SyntaxTreeNodeNo APNode, bool ComplexViewReplacement )
    : Query_Node(Context, QueryGraph, Position, APNode, false, DT_Unknown)
	, mSchemaName(false)
	, mTableName(false)
    , mReferenceName(false)
    , mTableDescriptor()
    , mOutputColumnCnt(0)
    , mType(T_Unknown)
    , mExplicitSchema(false)
	, mSelectPrivForAllColumns(false)
	, mReferencedViaSynonym(false)
    , mSysKey(false)
    , mPrimaryKey(false)
    , mUniqueIndex(false)
	, mComplexViewReplacement(ComplexViewReplacement)
    , mTableColumns(Context.GetAllocator())
{
}

// copy constructor
Query_TableNode::Query_TableNode( const Query_TableNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : Query_Node(N, Father, Q, mpMap, mpQMap)
    , mTableColumns(N.mAllocator) 
{
    mTableDescriptor            = Catalog_TableDescriptor(N.mTableDescriptor);
    mOutputColumnCnt            = N.mOutputColumnCnt;
    mType                       = N.mType;
	mSchemaName                 = N.mSchemaName;
	mTableName                  = N.mTableName;
    mReferenceName              = N.mReferenceName;
	mSelectPrivForAllColumns    = N.mSelectPrivForAllColumns;
    mExplicitSchema             = N.mExplicitSchema;
	mReferencedViaSynonym       = N.mReferencedViaSynonym;
    mSysKey                     = N.mSysKey;
    mPrimaryKey                 = N.mPrimaryKey;
    mUniqueIndex                = N.mUniqueIndex;
	mComplexViewReplacement     = N.mComplexViewReplacement;

    // Copy the Columns
    SAPDB_Int2 m = N.mTableColumns.GetSize();
    mTableColumns.Reserve(m);
    for ( SAPDB_Int2 i=0; i<m; i++ )
    {
        mTableColumns.InsertEnd(N.mTableColumns[i]);
    }
}

Query_Node* Query_TableNode::CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    return new(mAllocator) Query_TableNode(*this, Father, Q, mpMap, mpQMap);
}

// ------------------------------------------------------------------------------------------------

void Query_TableNode::PrintToTrace( Kernel_VTrace& VTrace )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_TableNode::PrintToTrace", QueryRewrite_Trace, 8);

    Query_Node::PrintToTrace(VTrace);
    if ( QueryRewrite_Trace.TracesLevel(2) )
    {
        VTrace << "NodeType  : TableAccess" << FlushLine;
        if ( mSemanticsDone )
        {
            switch ( mType )
            {
            case T_BaseTable:
                VTrace << "Type      : BaseTable" << FlushLine;
                break;
            case T_Cursor:
                VTrace << "Type      : Cursor" << FlushLine;
                break;
            case T_Synonym:
                VTrace << "Type      : Synonym" << FlushLine;
                break;
            case T_View:
                VTrace << "Type      : View" << FlushLine;
                break;
            case T_JoinView:
                VTrace << "Type      : JoinView" << FlushLine;
                break;
            case T_ComplexView:
                VTrace << "Type      : ComplexView" << FlushLine;
                break;
            case T_Unknown:
                VTrace << "Type      : Unknown ???" << FlushLine;
                break;
            }
            VTrace << "SchemaName  : " << mSchemaName << FlushLine;
            VTrace << "TableName   : " << mTableName << FlushLine;
            VTrace << "Output Cols : " << mOutputColumnCnt << FlushLine;
        }
    }
    if ( QueryRewrite_Trace.TracesLevel(1) && !QueryRewrite_Trace.TracesLevel(2) )
    {
        if ( mSemanticsDone )
            VTrace << "[" << mID << "] " // << mSchemaName << "."
                                        << mTableName << FlushLine;
        else
            VTrace << "[" << mID << "] 'Table'" <<  FlushLine;
    }
}

// ------------------------------------------------------------------------------------------------

bool Query_TableNode::Parse( bool ComplexViewReplacement )
{
	SAPDBERR_ASSERT_ARGUMENT ( ComplexViewReplacement == mComplexViewReplacement );

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo Node = SyntaxTree[mAPNode].n_lo_level;

    if ( SyntaxTree[Node].n_proc == no_proc && SyntaxTree[Node].n_symb == s_authid )
    {
        mExplicitSchema = true;
        mQueryGraph.GetSchemaName(Node, mSchemaName);
        mQueryGraph.GetIdentifier(Node, mTableName);
    }
    else
    {
        mExplicitSchema = false;
        mSchemaName.SetBlank();
        mQueryGraph.GetIdentifier(Node, mTableName);
    }

	if ( (Node = SyntaxTree[Node].n_sa_level ) > 0 )
	{
		if ( SyntaxTree[Node].n_proc == no_proc && SyntaxTree[Node].n_symb == s_identifier )
        	mQueryGraph.GetIdentifier(Node, mReferenceName);
        else
            mReferenceName.SetBlank();
	}
    else
    {
        mReferenceName.SetBlank();
    }
    return true;
}

bool Query_TableNode::Semantics( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_TableNode::Semantics", QueryRewrite_Trace, 6);
	SAPDBERR_ASSERT_ARGUMENT ( ComplexViewReplacement == mComplexViewReplacement );

    if ( !mQueryGraph.IsOk() )
        return false;

    if ( !mTableDescriptor )
    {
        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

        SyntaxTreeNodeNo Node = SyntaxTree[mAPNode].n_lo_level;

        mTableDescriptor = Catalog_TableDescriptor(mContext.SQLManContext(), mSchemaName, mTableName, UserId, 
                            /*ConsiderResultTables=*/ true, /* ConsiderSYSDD */ true);
		if ( !mContext.SQLManContext().IsOk() )
		{
			mQueryGraph.SetError("a_returncode != 0");
			return false;
		}

		if ( mTableDescriptor )
        {
			SQLMan_Context& context = mContext.SQLManContext();

			mReferencedViaSynonym = mTableDescriptor.ReferencedViaSynonym();
            if ( mReferenceName.IsBlank() ) // DDT
            {
                if ( mReferencedViaSynonym )
                    mReferenceName = mTableName;
            }

            bool IsView = false;
            mType = T_BaseTable;
            if ( mTableDescriptor.IsSynonym() )
                mType = T_Synonym;
            if ( mTableDescriptor.IsResultTable() )
			{
				context.a_parsingForSharedSql = false;
                mType = T_Cursor;
			}
            if ( mTableDescriptor.IsView() )
            {
                IsView = true;
                mType = T_View;
            }
            if ( mTableDescriptor.IsJoinView() )
            {
                IsView = true;
                mType = T_JoinView;
            }
            if ( mTableDescriptor.IsComplexView() )
            {
                IsView = true;
                mType = T_ComplexView;
            }

            bool ToBeReplaced = IsViewToBeReplaced();

            mOutputColumnCnt = CollectColumnInfo(IsView, ToBeReplaced);

            if ( mOutputColumnCnt == 0 )
            {
				// DDT : ???
				mQueryGraph.SetError("NoOutputColumns");
                return false;
            }

            SyntaxTreeNodeNo TabNameNode = SyntaxTree[mAPNode].n_lo_level;
			SyntaxTree[TabNameNode].n_querycheck.addElement(qcn_RightsChecked);

			// if we are not within a complex view replacement and we are parsing for sharedsql we have to
			// add information for SharedSQL
	        SharedSQL_PrepareHandle* pPrepareHandle = mContext.GetPrepareHandle();
			if ( !ComplexViewReplacement && pPrepareHandle )
			{
				SharedSQL_Table SSQLTable(mTableDescriptor.GetSchemaId(), mTableDescriptor.GetTableName(), mExplicitSchema);

				pPrepareHandle->PutTable(SSQLTable, context.GetErrorList());
			}
			//
            if ( IsViewToBeReplaced() )
                mQueryGraph.SetNeedViewReplacement();

// We need the following block only in 7.6
            if ( ComplexViewReplacement )
            {
/*DDT*/
                SQLMan_Identifier S(false);
                mTableDescriptor.GetSchemaName(S);
                if ( S == a01_i_sysdd )
                {
                    mQueryGraph.SetNotYetImplemented("SYSDD-Table");
                    return false;
                }
/**/
            }

            mSemanticsDone = true;
			return true;
        }
        mQueryGraph.SetError("NoValidTableDescriptor");
        return false;
    }
    return true;
}

SAPDB_Int2 Query_TableNode::CollectColumnInfo( bool IsView, bool ToBeReplaced )
{
	mSelectPrivForAllColumns = mTableDescriptor.HasAllSelectPrivileges(); 
	bool InsertSelectWithCatalogTable = mQueryGraph.CommandIsInsertSelect() && mTableDescriptor.IsCatalogTable();

    Catalog_TableDescriptor::ColumnIterator ColIt = mTableDescriptor.Begin();
	if ( !mContext.SQLManContext().IsOk() )
	{
		mQueryGraph.SetError("a_returncode != 0");
		return 0;
	}
	bool SysKeyFound=false;
    SAPDB_Int2 k=0;
    if ( ColIt ) 
    { 
        Catalog_TableDescriptor::ColumnIterator Last  = mTableDescriptor.End();
		if ( !mContext.SQLManContext().IsOk() )
		{
			mQueryGraph.SetError("a_returncode != 0");
			return false;
		}
		mTableColumns.Reserve(mTableDescriptor.GetColumnCount());
        for ( ; ColIt!=Last; ++ColIt )
        {
            Catalog_ColumnDescriptor TC = (*ColIt);
			SAPDBERR_ASSERT_STATE( mContext.SQLManContext().IsOk() );
            if ( !TC.IsDropped() )
            {
                if ( TC.IsVisible() 
                    && (mSelectPrivForAllColumns || mTableDescriptor.HasSelectPrivilege(TC))
                    && (!TC.IsSysKey() || IsView) )
                {
                    ++k;
                    Query_TableColumn Col(TC.GetIOLength(), k, (!ToBeReplaced?TC.ExtColNo():k+1),
                                             TC.GetColumnName(), 
                                             TC.IsNullable(),
                                             false, 
                                             true);
                    mTableColumns.InsertEnd(Col);
					if ( InsertSelectWithCatalogTable && TC.IsLongDataType() )
					{
						mQueryGraph.SetNotYetImplemented("PseudoLongColumn");
						return 0;
					}
                }
                else
                {
                    if ( TC.IsSysKey() )
                    {
                        SAPDBERR_ASSERT_STATE( !SysKeyFound );
                        SysKeyFound = true;
                        Query_TableColumn Col(TC.GetIOLength(), 0, TC.ExtColNo(),
                                                TC.GetColumnName(), 
                                                TC.IsNullable(),
                                                true, 
                                                TC.IsVisible()&(mSelectPrivForAllColumns || mTableDescriptor.HasSelectPrivilege(TC)));
                        mTableColumns.InsertFront(Col);
						if ( InsertSelectWithCatalogTable && TC.IsLongDataType() )
						{
							mQueryGraph.SetNotYetImplemented("PseudoLongColumn");
							return 0;
						}
                    }
                }
            }
        }
    }
    mSysKey = SysKeyFound;  // we need this as the syskey could be either invisible or without selectpriv ...

#if defined(SAPDB_SLOW)
    if ( mSysKey != mTableDescriptor.HasSysKey() )
    {
        SAPDBERR_ASSERT_STATE( 1 );
    }
#endif

    return k;
}

bool Query_TableNode::DefineUniqueOutput(Container_Vector<SAPDB_Int2>& LogOutColNr)
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

	SAPDB_Int2 n = LogOutColNr.GetSize();
	for ( SAPDB_Int2 i=0; i<n; i++ )
	{
		SAPDB_Int2 ExtColNo = LogicalColNo2ExtColNo(LogOutColNr[i]);
		LogOutColNr[i] = ExtColNo;
	}
	return mTableDescriptor.IsUniqueColumnSet(LogOutColNr);

 //   SAPDB_Int2 n = LogOutColNr.GetSize();
	//if ( n>0 )
	//{
	//	bool Unique = true;

	//	// Check the key first
	//	Catalog_TableDescriptor::KeyColumnIterator KeyColIt = mTableDescriptor.BeginKey();
	//	if ( !mContext.SQLManContext().IsOk() )
	//	{
	//		mQueryGraph.SetError("a_returncode != 0");
	//		return false;
	//	}
	//	if ( KeyColIt )
	//	{
	//		while ( KeyColIt )
	//		{
	//			for ( SAPDB_Int2 i=0; i<n; i++ )
	//			{
	//				if ( LogicalColNo2ExtColNo(LogOutColNr[i]) == (*KeyColIt).ExtColNo() )
	//					break;
	//				else
	//					Unique = false;
	//			}
	//			++KeyColIt;
	//		};
	//		if ( Unique )
	//			return true;
	//	}    

	//	if ( !mTableDescriptor.IsView() )
	//	{
	//		// Check the unique indexes
	//		Catalog_IndexIterator IndexIt = mTableDescriptor.GetIndexIterator(/*WithKey=*/ false);
	//		if ( !mContext.SQLManContext().IsOk() )
	//		{
	//			mQueryGraph.SetError("a_returncode != 0");
	//			return false;
	//		}
	//		while ( IndexIt )
	//		{
	//			Catalog_IndexDescriptor IndexDesc = (*IndexIt);
	//			if ( IndexDesc.IsUniqueIndex() /* && !IndexDesc.IsFunctionBasedIndex()*/ )  // DDT !!!
	//			{
	//				Catalog_IndexDescriptor::ColumnIterator ColIt = IndexDesc.Begin();
	//				if ( !mContext.SQLManContext().IsOk() )
	//				{
	//					mQueryGraph.SetError("a_returncode != 0");
	//					return false;
	//				}
	//				if ( ColIt )
	//				{
	//					bool Unique = true;
	//					while ( ColIt )
	//					{
	//						for ( SAPDB_Int2 i=0; i<n; i++ )
	//						{
	//							if ( LogicalColNo2ExtColNo(LogOutColNr[i]) == (*ColIt).ExtColNo() )
	//								break;
	//							else
	//								Unique = false;
	//						}
	//						++ColIt;
	//					};
	//					if ( Unique )
	//						return true;
	//				}
	//			}
	//			++IndexIt;
	//		}
	//	}
	//}
 //   return false;
}

bool Query_TableNode::IsViewToBeReplaced( void )
{
	if ( (mType == T_ComplexView) 
	  || (mType == T_JoinView)
#if defined(QRW_REPLACE_ALL_KIND_OF_VIEWS)
	  || (mType == T_View) 
#endif
	   )
			return true;
	return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_TableNode::ReplaceComplexViews( void )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

	if (    (mType != T_ComplexView)
		 && (mType != T_JoinView)
#if defined(QRW_REPLACE_ALL_KIND_OF_VIEWS)
		 && (mType != T_View)
#endif
		)
        return true;

    // replace 'this' !

    Query_ComplexViewInfo VInfo;
    VInfo.SyntaxNode			= SyntaxNode();
    VInfo.TabID					= TabID();
	VInfo.ReferencedViaSynonym	= mReferencedViaSynonym;

    SyntaxTreeNodeNo ColNameNodes = cak_qrewrite_ref_init;
    if ( mQueryGraph.SubstituteComplexView(VInfo, ColNameNodes/*, mType==T_ComplexView*/) )
	{
		mQueryGraph.SetNeedOptimization();

		// Call CreateQueryNodes for the new tree part
		SyntaxTreeNodeNo A631Node = mQueryGraph.SyntaxTree()[mAPNode].n_lo_level;

		Query_RootNode HelpRoot(this);
		HelpRoot.SetSyntaxNode(A631Node);
        HelpRoot.Parse(/*ComplexViewReplacement=*/true);

		SAPDB_Int2 ChildCount = 0;
		Query_Node* NewNode = HelpRoot.GetFirstChildNode(ChildCount);

		if ( mQueryGraph.IsOk() && ChildCount==1 )
		{
			// clear reference of HelpRoot to NewNode
			NewNode->FatherQuantifier()->SetNodeRef(0);

			// link NewNode with my FatherNode correctly
			NewNode->FatherNode()       = this->FatherNode();
			NewNode->FatherQuantifier() = this->FatherQuantifier();
			NewNode->FatherQuantifier()->SetNodeRef(NewNode);
			NewNode->SetNodeID(NodeID()); // we wan't the same ID as the Node that is replaced...

			// Call semantics for the new tree part
			// we don't need to check for unique output columns as the view must be defined that way!

            SQLMan_Surrogate UserId = mTableDescriptor.GetOwnerId();
			if ( NewNode->Semantics(UserId, /*UniqueOutputColumns=*/false, /*ComplexViewReplacement=*/true) )
			{
				if ( ColNameNodes > 0 )
				{
					// DDT : Asterisk must be replaced if there is a column list with the view name
					// e.g.  create view v(A,B,C) as select t1.*, t2.* from s1.t1, t2
					//                     ^              ^
                    if ( !NewNode->CreateColRefNamesAndResolveAsterisk(ColNameNodes) )
						mQueryGraph.SetError("ReplaceComplexViews");
				}
                if ( !mSelectPrivForAllColumns )
                {
                    if ( !NewNode->FatherNode()->RenewSemanticsForColumnsDueToColumnPrivs(NewNode) )
                        mQueryGraph.SetError("RenewSemanticsForQuantifier"); 
                }
                else
                {
/*DDT*/             // transfer Reference Counts from TableColumns to NewNode
                    for ( SAPDB_Int2 i = 1; i<= mOutputColumnCnt; i++ )
                    {
                        NewNode->AddRefCountForOutputColumn(i, mTableColumns[i-(HasSysKey()?0:1)].GetRefCount());
                    }
                }
				NewNode->SetViewReplacement();

                if ( mQueryGraph.IsOk() )
                {
                    mQueryGraph.SetProgress(PG_SubstituteViews);
                    // Continue replacing complex views
				    NewNode->ReplaceComplexViews();
                }
			}
			else
			{
				mQueryGraph.SetError("Semantics failed");
			}

			bool rc = mQueryGraph.IsOk();	// we cannot access mQueryGraph after destroying this anymore...
			Query_TableNode* T = this;
			destroy(T, mAllocator);
			return rc;
		}
	}
	mQueryGraph.SetError("ReplaceComplexViews");
	return false;
}

// ------------------------------------------------------------------------------------------------

SAPDB_Int2 Query_TableNode::ExtColNo2LogicalColNo( SAPDB_Int2 ColumnIndex )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    SAPDB_Int2 n=mTableColumns.GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        if ( mTableColumns[i].ExtColNo() == ColumnIndex )
            return mTableColumns[i].LogColNo();
    }
    return -1;
}

SAPDB_Int2 Query_TableNode::LogicalColNo2ExtColNo( SAPDB_Int2 LogColIndex )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    SAPDBERR_ASSERT_ARGUMENT( LogColIndex-(HasSysKey()?0:1)>=0 && LogColIndex-(HasSysKey()?0:1)<(int)mTableColumns.GetSize() );

    return mTableColumns[LogColIndex-(HasSysKey()?0:1)].ExtColNo();
}

// ------------------------------------------------------------------------------------------------

SAPDB_Int2 Query_TableNode::OutputColumns( void )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
//    SAPDBERR_ASSERT_STATE( mOutputColumnCnt == mTableColumns.GetSize()-(HasSysKey()?1:0) );
    return mOutputColumnCnt;
}

SAPDB_Int2 Query_TableNode::GetOutputColumnSize( SAPDB_Int2 LogColIndex )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    SAPDBERR_ASSERT_ARGUMENT( LogColIndex-(HasSysKey()?0:1)>=0 && LogColIndex-(HasSysKey()?0:1)<(int)mTableColumns.GetSize() );
    return mTableColumns[LogColIndex-(HasSysKey()?0:1)].ColLength();
}

bool Query_TableNode::GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    SAPDBERR_ASSERT_ARGUMENT( LogColIndex-(HasSysKey()?0:1)>=0 && LogColIndex-(HasSysKey()?0:1)<(int)mTableColumns.GetSize() );

    ColName     = mTableColumns[LogColIndex-(HasSysKey()?0:1)].ColName();
    IsNullable  = mTableColumns[LogColIndex-(HasSysKey()?0:1)].IsNullable();
    return true;
}

bool Query_TableNode::GetOutputColumnExtNo( SAPDB_Int2 LogColIndex, SAPDB_Int2& ExtColNo )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    SAPDBERR_ASSERT_ARGUMENT( LogColIndex-(HasSysKey()?0:1)>=0 && LogColIndex-(HasSysKey()?0:1)<(int)mTableColumns.GetSize() );

    ExtColNo = mTableColumns[LogColIndex-(HasSysKey()?0:1)].ExtColNo();
    return true;
}

bool Query_TableNode::CheckForColumn( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    SAPDB_Int2 n=mTableColumns.GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        if (   mTableColumns[i].IsVisible() 
            && mTableColumns[i].ColName() == Column )
        {
            QuantifierID            = mID;
            QuantifierSyntaxNode    = mAPNode;
            LogColumnIndex          = mTableColumns[i].LogColNo();
            ExtColNo                = mTableColumns[i].ExtColNo();
            IsNullable              = mTableColumns[i].IsNullable();
            mTableColumns[i].IncRefCount();
            return true;
        }
    }
    return false;
}

bool Query_TableNode::CheckForColumn( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    if ( Table.IsBlank() )
        return CheckForColumn(Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
    if ( CheckQuantifierName(Schema, Table) )
        return CheckForColumn(Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
    return false;
}

bool Query_TableNode::CheckQuantifierName( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    if ( mReferenceName.IsBlank() )
	{
		if ( !Schema.IsBlank() )
	        return mTableDescriptor.IsEqualTable(Schema, Table);
		return ( Table == mTableDescriptor.GetTableName() );
	}
    if ( (Schema.IsBlank()) && (Table==mReferenceName) )
        return true;
    return false;
}

void Query_TableNode::GetIdentificationNames( SQLMan_Identifier& Schema, SQLMan_Identifier& Table )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    if ( !mReferenceName.IsBlank() )
    {
        Schema.SetBlank();
        Table  = mReferenceName;
    }
    else
    {
        mTableDescriptor.GetSchemaName(Schema);
        mTableDescriptor.GetTableName(Table);
    }
}

bool Query_TableNode::HasReferenceName( void )
{
	return ( !mReferenceName.IsBlank() );
}

bool Query_TableNode::DecRefCountForOutputColumn( SAPDB_Int2 i )
{ 
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    mTableColumns[i-(HasSysKey()?0:1)].DecRefCount();
    return true; 
}

bool Query_TableNode::IncRefCountForOutputColumn( SAPDB_Int2 i ) 
{ 
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    mTableColumns[i-(HasSysKey()?0:1)].IncRefCount();
    return true; 
}

bool Query_TableNode::AddRefCountForOutputColumn( SAPDB_Int2 i, SAPDB_Int2 Count )
{
    SAPDBERR_ASSERT_STATE( 0 );
    return false;
}



// ------------------------------------------------------------------------------------------------
