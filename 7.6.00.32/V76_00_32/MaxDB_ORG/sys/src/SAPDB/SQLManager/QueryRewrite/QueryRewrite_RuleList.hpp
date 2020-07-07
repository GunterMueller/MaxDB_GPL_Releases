/*!***************************************************************************

    @file     QueryRewrite_RuleList.hpp
    @ingroup  QueryRewrite
    @author   DirkT
    @brief    
    @see    

  -------------------------------------------------------------------------

  responsible : DirkT

  special area: SQL Manager / QueryRewrite
  description : 

  last changed: 2005-06-08  12:00
  first created:2005-06-04  10:00

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

#ifndef QUERYREWRITE_RULELIST
#define QUERYREWRITE_RULELIST

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_Types.hpp"
#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rules.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_RawAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

#include "SAPDB/Container/Container_Vector.hpp"

// ------------------------------------------------------------------------------------------------

class Query_Node;

// ------------------------------------------------------------------------------------------------

/*!
    @class          QueryRewrite_GlobalRuleList
    @brief          Global List of rule status that can be modified by UPDATE of table SYSDD.QUERYREWRITERULES
 */
class QueryRewrite_GlobalRuleList 
{
public:
    /*!
       @brief       Returns an instance of the global rulelist
       @return      Pointer to QueryRewrite_GlobalRuleList
    */
    static QueryRewrite_GlobalRuleList* Instance()
    {
        static RTESync_Spinlock lock;
        RTESync_LockedScope scope(lock);

        if ( !mInstance )
        {
            SAPDBMem_IRawAllocator& Alloc = RTEMem_RawAllocator::Instance();
            mInstance = new(Alloc) QueryRewrite_GlobalRuleList(Alloc);
        }
        return mInstance;
    }

    /*!
       @brief       Stets the status of some rule
	   @param       RuleName [in] Name of the rule whose status is to be changed
	   @param       Active   [in] TRUE=ACTIVE or FALSE=INACTIVE
       @return      TRUE if ok, else FALSE
    */
	bool SetGlobalRuleStatus( SQLMan_Identifier& RuleName, bool Active )
    {
        for ( SAPDB_Int2 i=0; i<GetCnt(); i++)
        {
            if ( RuleName == mRules[i]->GetRuleName() )
            {
                mRules[i]->ChangeActive(Active);
                return true;
            }
        }
        return false;
    }

    /*!
       @brief       Returns the number of rules
       @return      Number of rules
    */
    inline const SAPDB_Int2 GetCnt( void )
    {
        return QUERY_RULECOUNT;
    }

    /*!
       @brief       Returns pointer to the i-th rule
       @param       i [in] Rule number
       @return      QueryRewrite_Rule*
    */
    inline QueryRewrite_Rule* operator[](int i)
    {
        SAPDBERR_ASSERT_ARGUMENT( 0<=i && i<GetCnt() );
        if ( 0<=i && i<GetCnt() )
            return mRules[i];
        return 0;
    }

private:
    /*!
       @brief       Constructor
    */
    QueryRewrite_GlobalRuleList( SAPDBMem_IRawAllocator& Alloc )
    {
        // eliminate orderby
        mRules[ 0] = new(Alloc) Rule_EliminateOrderBy(0, false);

        // merge FROMSELECTs
        mRules[ 1] = new(Alloc) Rule_MergeF(1, true);
        mRules[ 2] = new(Alloc) Rule_MergeE(2, false);

        // convert EXIST to FROMSELECT
        mRules[ 3] = new(Alloc) Rule_ConvertExist(3, false);
        // convert INTERSECT and EXCEPT to EXIST
        mRules[ 4] = new(Alloc) Rule_ConvertToExist(4, false);

        // move distinct types
        mRules[ 5] = new(Alloc) Rule_DistinctPullUp(5, false);
        mRules[ 6] = new(Alloc) Rule_DISTPDTO(6, false);
        mRules[ 7] = new(Alloc) Rule_DISTPDFR(7, false); 
        mRules[ 8] = new(Alloc) Rule_DistinctForSubqueries(8, false);

        //
        mRules[ 9] = new(Alloc) Rule_OptimizeSubquery(9, false);

        // simplify predicates and eliminate groupby//distinct
        mRules[10] = new(Alloc) Rule_SimplifyPredicates(10, false);
        mRules[11] = new(Alloc) Rule_EliminateGroupByOrDistinct(11, false);


        // push down whatever possible
        mRules[12] = new(Alloc) Rule_PushDownPredicates(12, false);     
        mRules[13] = new(Alloc) Rule_PushDownProjection(13, false);
        mRules[14] = new(Alloc) Rule_PushDownQuantifier(14, false);  
    }

    static QueryRewrite_GlobalRuleList* mInstance;    
    QueryRewrite_Rule*                  mRules[QUERY_RULECOUNT];
};

// ------------------------------------------------------------------------------------------------

// ...
typedef QueryRewrite_GlobalRuleList QueryRewrite_RuleList;

// ------------------------------------------------------------------------------------------------

#endif