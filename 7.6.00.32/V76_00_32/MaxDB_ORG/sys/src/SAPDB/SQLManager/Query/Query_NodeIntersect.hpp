/*!***************************************************************************

    @file     Query_NodeIntersect.hpp
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

#ifndef Query_NodeIntersect_H
#define Query_NodeIntersect_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/Query/Query_NodeSet.hpp"

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_IntersectNode
    @brief          Node that represents an INTERSECT
    @copydoc        Query_Node
 */
class Query_IntersectNode : public Query_SetNode
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
    Query_IntersectNode( const Query_IntersectNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
public:
    virtual Query_Node* CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );

    /*!
        @brief  Constructor
        @param  Context    [in]
        @param  QueryGraph [in]
        @param  Position   [in]
        @param  APNode     [in]
        @param  All        [in]
    */    
    Query_IntersectNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, Query_Position Position, SyntaxTreeNodeNo APNode, bool All);

    virtual NodeType GetNodeType( void ) { return Intersect; }
    virtual void PrintToTrace( Kernel_VTrace& VTrace );

    virtual Query_Position PositionForParse( void ) { return P_Intersect; }
};

// ------------------------------------------------------------------------------------------------

#endif