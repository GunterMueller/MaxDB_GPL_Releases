/*!
 * @addtogroup SQLManager
*/
/*!
 * @defgroup Optimizer Optimizer
 * @ingroup SQLManager
*/

/*!***************************************************************************

    @file     QueryRewrite_PascalWrapper.cpp
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

#include "hak664wrapper.h"
#include "hak101.h"
#include "hak542.h"
#include "hak40.h"
#include "hak42.h"

#include "SAPDB/SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SQLManager/Query/Query_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeRoot.hpp"
#include "SAPDB/SQLManager/Query/Query_ManipulationContext.hpp"
#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rewrite.hpp"
#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp"
#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rules.hpp"

#include "SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp"

#include "SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

// ------------------------------------------------------------------------------------------------

void a101_QueryRewrite_MonitorReset( void )
{
    QueryRewrite_GlobalRuleList* Rules = QueryRewrite_GlobalRuleList::Instance();
    if ( Rules )
    {
        SAPDB_Int2 n = Rules->GetCnt();
        for ( SAPDB_Int2 i=0; i<n; i++ )
        {
            (*Rules)[i]->ResetApplyCount();
        }
    }
}

void a101_QueryRewrite_MonitorOut(
                   tak_all_command_glob VAR_VALUE_REF acv,
                   tak40_show_glob      VAR_VALUE_REF a41v)
{
    QueryRewrite_GlobalRuleList* Rules = QueryRewrite_GlobalRuleList::Instance();
    if ( Rules )
    {
        SAPDB_Int2 n = Rules->GetCnt();
        for ( SAPDB_Int2 i=0; i<n; i++ )
        {
            tsp00_C40  Text;
            Text.c2p((*Rules)[i]->GetMonitorText());
            tsp00_Longreal Cnt  = (tsp00_Longreal)(*Rules)[i]->GetApplyCount();
            a42ref_stat_real (acv, a41v, Text, Cnt);
        }
    }
}

// ------------------------------------------------------------------------------------------------

/*!
    @brief  ...
    @param  acv             [in/out]
    @param  prepareHandle   [in]
*/
void a664_QueryRewrite (
    tak_all_command_glob    VAR_VALUE_REF   acv,
	tgg00_VoidPtr prepareHandle
    )
{
    SAPDBTRACE_ROUTINE_DEBUG("a664_QueryRewrite", QueryRewrite_Trace, 5);
    SAPDBERR_ASSERT_STATE(  acv.a_ap_tree == acv.a_session_ap_tree );

    if ( acv.a_sqlmode == sqlm_oracle || acv.a_sqlmode == sqlm_internal ) // do nothing for sqlmodes ANSI or DB2
    {
	    SharedSQL_PrepareHandle* pPrepareHandle = REINTERPRET_CAST(SharedSQL_PrepareHandle*, prepareHandle);

        SQLMan_Context&             SQLManContext = SQLMan_Context::AcvToContext(acv);
	    // ManipulationContext MUST be created before QueryGraph !!!
        Query_ManipulationContext   QRWContext(SQLManContext, pPrepareHandle);

        if ( QRWContext.IsOk() )
        {
            Query_QueryGraph QueryGraph(QRWContext);

            bool rc = QueryGraph.Parse();
            QueryGraph.Semantic();
            QueryGraph.Optimize();
            QueryGraph.GenerateAPTree();
            if ( QueryGraph.IsOk() )
            {
                if ( QRWContext.KeepChanges() )
                    QueryGraph.TransferApplyCounts();
            }
            else
            {
                QRWContext.DiscardChanges();
            }
        }
    }
}

/*!
    @brief  ...
    @param  acv             [in/out]
*/
void a664_FreeQueryRewriteCaches(
    tak_all_command_glob    VAR_VALUE_REF   acv
                                )
{
    SAPDBTRACE_ROUTINE_DEBUG("a664_FreeQueryRewriteCaches", QueryRewrite_Trace, 5);
	if ( acv.a_queryrewrite_modified )
	{
		SAPDB_Int2 oldReturnCode = acv.a_returncode;
		acv.a_returncode = 0;
		if ( acv.a_cmd_segm )
			if ( acv.a_cmd_segm->sp1c_producer() == sp1pr_queryrewrite )
				a542pop_packet(acv);

		SQLMan_Context::AcvToContext(acv).DeleteSyntaxTree();
		acv.a_ap_tree = acv.a_session_ap_tree;
		acv.a_queryrewrite_modified = false;
		acv.a_returncode = oldReturnCode;
	}
}

// ------------------------------------------------------------------------------------------------

