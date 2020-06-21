/*!***************************************************************************

    @file     QueryRewrite_Rewrite.cpp
    @ingroup  QueryRewrite
    @author   DirkT
    @brief    
    @see    

  -------------------------------------------------------------------------

  responsible : DirkT

  special area: SQL Manager / QueryRewrite
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

#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rewrite.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeRoot.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"

/// ...
#define QRW_MAX_LOOPS_PER_NODE  21
/// ...
#define QRW_MAX_TOP_LEVEL_LOOPS 11

// ------------------------------------------------------------------------------------------------

bool Opti_QueryRewriter::Optimize( QueryRewrite_RuleList& Rules )
{
    SAPDBTRACE_METHOD_DEBUG ("Opti_QueryRewriter::Optimize", QueryRewrite_Trace, 5);

	bool rc = false;
    if ( mQueryGraph.IsOk() )
    {
        if ( QueryRewrite_Trace.TracesLevel(1) )
            mRewriteContext.VTrace() << "-- QueryRewrite: Optimize START -----------------" << FlushLine;

        bool Optimized = false;
        bool applied = false;
        int k = 1;
        do
        {
            if ( QueryRewrite_Trace.TracesLevel(3) )
                mRewriteContext.VTrace() << "   Optimize Step " << k+1 << FlushLine;
            applied = ApplyRules(Rules, mQueryGraph.RootNode(), Optimized);
        }while ( applied && ++k<=QRW_MAX_TOP_LEVEL_LOOPS && mQueryGraph.IsOk() );

        if ( QueryRewrite_Trace.TracesLevel(1) )
            mRewriteContext.VTrace() << "-- QueryRewrite: Optimize END -------------------" << FlushLine;

        if ( mQueryGraph.IsOk() &&
             mRewriteContext.IsOk() )
        {
            if ( Optimized )
            {
                mQueryGraph.SetProgress(PG_Optimized);
                mRewriteContext.SQLManContext().SetTransInfoWarning(warn9_stmt_optimized, /*on=*/true);
                mRewriteContext.SQLManContext().a_queryrewrite_done.becomes(Operator_Rewrite);
            }
        }
		if ( QueryRewrite_Trace.TracesLevel(1) )
			mRewriteContext.VTrace() << "-------------------------------------------------" << FlushLine;
    }
	return rc;
}

bool Opti_QueryRewriter::ApplyRules( QueryRewrite_RuleList& Rules, Query_Node* Node, bool& Optimized )
{
    SAPDBTRACE_METHOD_DEBUG ("Opti_QueryRewriter::ApplyRules", QueryRewrite_Trace, 5);

    bool applied = false;
    Query_Node* QueryNode = Node;

    if ( QueryNode )
    {
        // Apply rules to the given QueryNode
        SAPDB_Int2 n = Rules.GetCnt();
        SAPDB_Int2 r = 0;
        SAPDB_Int2 LoopCnt = 0;
        SAPDB_Int2 m = 0;

        while ( r<n && LoopCnt<=QRW_MAX_LOOPS_PER_NODE && mQueryGraph.IsOk() )
        {
            bool a = false;
            if ( Rules[r]->IsActive() )
            {
                switch ( Rules[r]->GetParamType() )
                {
                case QueryRewrite_Rule::NodeOnly:
                    if ( Rules[r]->IsApplicable(QueryNode) )
                    {
                        if ( Rules[r]->Apply(QueryNode, Optimized) )
                        {
#if defined (SAPDB_SLOW)
                            if ( QueryRewrite_Trace.TracesLevel(3) )
                                mQueryGraph.PrintToTrace();
                            mRewriteContext.GetAllocator().CheckConsistency();
#endif
                            a = applied = true;
                        }
                    }
                    break;
                case QueryRewrite_Rule::NodeAndQuantifier:
                    m = QueryNode->GetQuantifier().GetSize();  
                    for ( SAPDB_Int2 q = m-1; (q>=0) && mQueryGraph.IsOk() && QueryNode; q-- )  
                    {
                        if ( Rules[r]->IsApplicable(QueryNode, q) )  
                        {
                            if ( Rules[r]->Apply(QueryNode, q, Optimized) )
                            {
#if defined (SAPDB_SLOW)
                                if ( QueryRewrite_Trace.TracesLevel(3) )
                                    mQueryGraph.PrintToTrace();
                                mRewriteContext.GetAllocator().CheckConsistency();
#endif
                                a = applied = true;
                            }
                            if ( QueryNode && (QueryNode!=Node) )   // Node changed by MERGE
                                q = QueryNode->GetQuantifier().GetSize();
                        }
                    }
                    break;
                }
            }
            r = ( a ? 0 : r+1 );
            LoopCnt += ( a ? 1 : 0 );
        }
    }

	if ( QueryNode && mQueryGraph.IsOk() )
    {
        // Apply rules to the children
        Container_Vector<Query_Quantifier*>& mQuantifier2 = QueryNode->GetQuantifier();
		for ( unsigned cb=0; (cb<mQuantifier2.GetSize()) && mQueryGraph.IsOk(); cb++ )
			applied |= ApplyRules(Rules, mQuantifier2[cb]->NodeRef(), Optimized);
    }

    return applied;
}

// ------------------------------------------------------------------------------------------------
