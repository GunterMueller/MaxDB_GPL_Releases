/*!***************************************************************************

    @file     QueryRewrite_Rules.hpp
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

#ifndef QUERYREWRITE_RULES
#define QUERYREWRITE_RULES

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/SQLManager/SQLMan_Types.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"

// ------------------------------------------------------------------------------------------------

class Query_Node;
class Query_SelectNode;
class Query_SetNode;
class Query_QueryGraph;

// ------------------------------------------------------------------------------------------------

/*!
    @class          QueryRewrite_Rule
    @brief          Base class for rewrite rules
 */
class QueryRewrite_Rule
{
public:
    /// Parameter Type
    enum ParamType{
                            NodeOnly,
                            NodeAndQuantifier
                  };

    /*!
       @brief       Constructor
	   @param       ID     [in] 
	   @param       Active [in] 
    */
    QueryRewrite_Rule( SAPDB_Int2 ID, bool Active )
        : mCount(0)
        , mActive(Active)
        , mRuleID(ID)
    {
    }

    /*!
       @brief       Returns the parameter type which describes how IsApplicabe and Apply have to be called
       @return      Parameter type
    */
    virtual QueryRewrite_Rule::ParamType GetParamType( void ) = 0;

    /*!
       @brief       Returns wheather or not the rule is applicable to the given node
       @param       Node [in] 
       @return      TRUE if applicable, else FALSE
    */
    virtual bool IsApplicable( Query_Node* Node ) = 0;
    /*!
       @brief       Apply the rule to the given node
       @param       Node        [in]
       @param       Optimized   [out] : TRUE if there was a real optimization (eliminate of fromselects etc)
       @return      TRUE if applied, else false
    */
    virtual bool Apply( Query_Node*& Node, bool& Optimized )  = 0;

    /*!
       @brief       Returns wheather or not the rule is applicable to the given node
       @param       Node            [in] 
       @param       QuantifierNo    [in]
       @return      TRUE if applicable, else FALSE
    */
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) = 0;
    /*!
       @brief       Apply the rule to the given node
       @param       Node            [in] 
       @param       QuantifierNo    [in]
       @param       Optimized   [out] : TRUE if there was a real optimization (eliminate of fromselects etc)
       @return      TRUE if applied, else false
    */
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) = 0;

    /*!
       @brief       Returns the rule name
       @return      SQLMan_Identifier
    */
    const SQLMan_Identifier GetAsciiRuleName( void ) { return SQLMan_Identifier(GetMonitorText(), (SAPDB_UInt2) strlen(GetMonitorText()), false /* ascii */); }
    /*!
       @brief       Returns the rule name
       @return      SQLMan_Identifier
    */
    const SQLMan_Identifier GetRuleName( void ) { return SQLMan_Identifier(GetMonitorText(), (SAPDB_UInt2) strlen(GetMonitorText()), true /* ascii */); }
	/*!
       @brief       Returns text for monitor output 
       @return      tsp00_C40
    */
    virtual const char* GetMonitorText( void ) = 0;

	/*!
       @brief       Returns wheather or not the rule es active
       @return      TRUE if active
    */
    inline bool IsActive( void )
    {
        return mActive;
    }

	/*!
       @brief       Sets the rule active or inactive
       @param       A [in] New Status
    */
    inline void ChangeActive( bool A )
    {
        mActive = A;
    }

	/*!
       @brief       Returns the rule ID
       @return      Rule ID
    */
    inline SAPDB_Int2 RuleID( void )
    {
        return mRuleID;
    }

    /*!
       @brief       Returns how often the rule was applied
       @return      SAPDB_Int8
    */
    SAPDB_Int8 GetApplyCount( void )    
    { 
        return RTESys_AtomicRead(mCount);
    }
    /*!
       @brief       Reset the counter 
    */
    void ResetApplyCount( void )  
    { 
        RTESys_AtomicWrite(mCount, (SAPDB_Int8)0);   
    }

    /*!
       @brief       Reset the counter 
    */
    void ChangeApplyCount( SAPDB_Int2 d )  
    { 
        RTESys_AtomicModify(mCount, (SAPDB_Int8)d);   
    }

private:
	/// Counter how often the rule has been applied
    SAPDB_Int8 mCount;
	/// Rule ID
    SAPDB_Int2 mRuleID;
	/// Status Active or Inactive
    bool       mActive;
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_MergeF
    @brief          Merge two Nodes
    @copydoc        QueryRewrite_Rule
 */
class Rule_MergeF : public QueryRewrite_Rule                                     // MergeF
{
public:
    /*!
       @brief       Constructor
    */
    Rule_MergeF( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeAndQuantifier; }
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo );
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node ) { return false; }
    virtual bool Apply( Query_Node*& Node, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "MergeFromSelectOrView"; }

private:
    /*!
       @brief       Merge node with quantifier
       @param       Node            [in] 
       @param       QuantifierNo    [in]
       @return      TRUE if no error occured, else FALSE
    */
     bool Merge( Query_Node*& Node, SAPDB_UInt2 QuantifierNo );
    /*!
       @brief       Set the resulting distinct type
       @param       UpperNode   [in] 
       @param       LowerNode   [in]
       @return      TRUE if no error occured, else FALSE
    */
     bool SetDistinctType( Query_SelectNode* UpperNode, Query_Node* LowerNode );
    /*!
       @brief       Set the resulting join type
       @param       UpperNode   [in] 
       @param       LowerNode   [in]
       @return      TRUE if no error occured, else FALSE
    */
     bool SetJoinType( Query_SelectNode* UpperNode, Query_Node* LowerNode );
    /*!
       @brief       Delete the upper node
       @param       UpperNode       [in] 
       @param       QuantifierNo    [in]
       @param       LowerNode       [in]
       @return      TRUE if no error occured, else FALSE
    */
     bool DeleteUpperNode( Query_SelectNode* UpperNode, SAPDB_UInt2 QuantifierNo, Query_Node* LowerNode );
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_MergeE
    @brief          Convert E quantifier to F quantifier
    @copydoc        QueryRewrite_Rule
 */
class Rule_MergeE : public QueryRewrite_Rule                                      // ConvertExistToFromSelect
{
public:
    /*!
       @brief       Constructor
    */
    Rule_MergeE( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    /*! @copydoc Rule_Merge::GetParamType()    */
    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeAndQuantifier; }
    /*! @copydoc Rule_Merge::IsApplicable()    */
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo );
    /*! @copydoc Rule_Merge::Apply()           */
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node ) { return false; }
    virtual bool Apply( Query_Node*& Node, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "MergeExistentialSubquery"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_ConvertExist
    @brief          Convert correlated E quantifier to IN clause
    @copydoc        Rule_MergeE
 */
class Rule_ConvertExist : public Rule_MergeE                                      // ConvertExist
{
public:
    /*!
       @brief       Constructor
    */
    Rule_ConvertExist( SAPDB_Int2 ID, bool Active ) : Rule_MergeE(ID, Active) {};

    /*! @copydoc Rule_Merge::GetParamType()    */
    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeAndQuantifier; }
    /*! @copydoc Rule_Merge::IsApplicable()    */
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo );
    /*! @copydoc Rule_Merge::Apply()           */
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node ) { return false; }
    virtual bool Apply( Query_Node*& Node, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "ConvertExistentialSubquery"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_ConvertToExist
    @brief          Converts INTERSECT to EXIST
    @copydoc        QueryRewrite_Rule
 */
class Rule_ConvertToExist : public QueryRewrite_Rule                    // IntersectOrExceptToExist
{
public:
    /*!
       @brief       Constructor
    */
    Rule_ConvertToExist( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "ConvertToExistentialSubquery"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_DistinctPullUp
    @brief          Pull up distinct
    @copydoc        QueryRewrite_Rule
 */
class Rule_DistinctPullUp : public QueryRewrite_Rule                       // DistinctPullUp   
{
public:
    /*!
       @brief       Constructor
    */
    Rule_DistinctPullUp( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "DistinctPullUp"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_DISTPDTO
    @brief          Distinct ...
    @copydoc        QueryRewrite_Rule
 */
class Rule_DISTPDTO : public QueryRewrite_Rule
{
public:
    /*!
       @brief       Constructor
    */
    Rule_DISTPDTO( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "DistinctPushDownTo"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_DISTPDFR
    @brief          Distinct ...
    @copydoc        QueryRewrite_Rule
 */
class Rule_DISTPDFR : public QueryRewrite_Rule
{
public:
    /*!
       @brief       Constructor
    */
    Rule_DISTPDFR( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "DistinctPushDownFrom"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_DistinctForSubqueries
    @brief          Distinct ...
    @copydoc        QueryRewrite_Rule
 */
class Rule_DistinctForSubqueries : public QueryRewrite_Rule
{
public:
    /*!
       @brief       Constructor
    */
    Rule_DistinctForSubqueries( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeAndQuantifier; }
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo );
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node ) { return false; }
    virtual bool Apply( Query_Node*& Node, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "DistinctForSubqueries"; }
};


// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_OptimizeSubquery
    @brief          Optimize subquery
    @copydoc        QueryRewrite_Rule
 */
class Rule_OptimizeSubquery : public QueryRewrite_Rule                       // OptimizeSubquery
{
public:
    /*!
       @brief       Constructor
    */
    Rule_OptimizeSubquery( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "OptimizeSubqueries"; }
};


// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_SimplifyPredicates
    @brief          Simplify predicates
    @copydoc        QueryRewrite_Rule
 */
class Rule_SimplifyPredicates : public QueryRewrite_Rule                       // SimplifyPredicates
{
public:
    /*!
       @brief       Constructor
    */
    Rule_SimplifyPredicates( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "SimplifyPredicates"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_EliminateGroupByOrDistinct
    @brief          Removes GroupBy 
    @copydoc        QueryRewrite_Rule
 */
class Rule_EliminateGroupByOrDistinct : public QueryRewrite_Rule                       // EliminateGroupByOrDistinct   
{
public:
    /*!
       @brief       Constructor
    */
    Rule_EliminateGroupByOrDistinct( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "EliminateGroupByOrDistinct"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_EliminateOrderBy
    @brief          Eliminate OrderBy
    @copydoc        QueryRewrite_Rule
 */
class Rule_EliminateOrderBy : public QueryRewrite_Rule                          // EliminateOrderBy
{
public:
    /*!
       @brief       Constructor
    */
    Rule_EliminateOrderBy( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "EliminateOrderBy"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_PushDownPredicates
    @brief          Push down predicates
    @copydoc        QueryRewrite_Rule
 */
class Rule_PushDownPredicates : public QueryRewrite_Rule                       // ConditionPushDown
{
public:
    /*!
       @brief       Constructor
    */
    Rule_PushDownPredicates( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeOnly; }
    virtual bool IsApplicable( Query_Node* Node );
    virtual bool Apply( Query_Node*& Node, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) { return false; }
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "PushDownPredicates"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_PushDownProjection
    @brief          Push down projection
    @copydoc        QueryRewrite_Rule
 */
class Rule_PushDownProjection : public QueryRewrite_Rule                       // PushDownProjection
{
public:
    /*!
       @brief       Constructor
    */
    Rule_PushDownProjection( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeAndQuantifier; }
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo );
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node ) { return false; }
    virtual bool Apply( Query_Node*& Node, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "PushDownProjection"; }
};

// ------------------------------------------------------------------------------------------------
/*!
    @class          Rule_PushDownQuantifier
    @brief          Push down quantifier
    @copydoc        QueryRewrite_Rule
 */
class Rule_PushDownQuantifier : public QueryRewrite_Rule                       // QuantifierPushDown
{
public:
    /*!
       @brief       Constructor
    */
    Rule_PushDownQuantifier( SAPDB_Int2 ID, bool Active ) : QueryRewrite_Rule(ID, Active) {};

    virtual QueryRewrite_Rule::ParamType GetParamType( void ) { return NodeAndQuantifier; }
    virtual bool IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo );
    virtual bool Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized );
    virtual bool IsApplicable( Query_Node* Node ) { return false; }
    virtual bool Apply( Query_Node*& Node, bool& Optimized ) { return false; }

    virtual const char* GetMonitorText( void )      { return "PushDownQuantifier"; }

private:
    /*!
       @brief       Push down of the quantifier
       @param       Node            [in] 
       @param       QuantifierNo    [in]
       @return      TRUE if no error occured, else FALSE
    */
    bool PushDown( Query_Node* Node, SAPDB_UInt2 QuantifierNo );
};

// ------------------------------------------------------------------------------------------------

#endif