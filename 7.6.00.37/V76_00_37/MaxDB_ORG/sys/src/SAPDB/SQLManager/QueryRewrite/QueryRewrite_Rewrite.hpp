/*!
 * @addtogroup Optimizer
*/
/*!
 * @defgroup QueryRewrite QueryRewrite
 * @ingroup Optimizer
*/

/*!***************************************************************************

    @file     QueryRewrite_Rewrite.hpp
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

#ifndef QUERYREWRITE_REWRITE
#define QUERYREWRITE_REWRITE

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/Query/Query_ManipulationContext.hpp"
#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp"
#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rules.hpp"
#include "SAPDB/SQLManager/Query/Query_Types.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"

// ------------------------------------------------------------------------------------------------

class Query_QueryGraph;

// ------------------------------------------------------------------------------------------------

/*!
    @class          Opti_QueryRewriter
    @brief          Evaluates and performs a rewrite of some given QueryGraph
 */
class Opti_QueryRewriter
{
public:
    /*!
       @brief       Constructor
       @param       Context    [in]
       @param       QueryGraph [in]
    */
    Opti_QueryRewriter( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph )
        : mRewriteContext(Context)
        , mQueryGraph(QueryGraph)
    {
    }

    /*!
       @brief       Destructor
    */
    ~Opti_QueryRewriter( void ) 
    {
    }

    /*!
       @brief       Evaluate and apply RewriteRules to the QueryGraph
       @param       Rules    [in]  : List of rules
       @return      TRUE if there were no errors, else FALSE
    */
    bool Optimize( QueryRewrite_RuleList& Rules );

protected:
    /*!
       @brief       Evaluate and apply RewriteRules to some given Node
       @param       Rules    [in]  : List of rules
       @param       Node     [in]  : Node the rules shall be applied to
       @param       Optimzed [out] : TRUE if there was a real optimization (eliminate of fromselects etc)
       @return      TRUE if some rule was applied, else FALSE
    */
    bool ApplyRules( QueryRewrite_RuleList& Rules, Query_Node* Node, bool& Optimzed );

private:
    Query_QueryGraph&                       mQueryGraph;        /// QueryGraph to optimize
    Query_ManipulationContext&              mRewriteContext;    /// RewriteContext
};

// ------------------------------------------------------------------------------------------------

#endif