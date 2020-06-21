/*!
 * @addtogroup SQLManager
*/
/*!
 * @defgroup Query Query
 * @ingroup SQLManager
*/

/*!***************************************************************************

    @file     Query_Types.hpp
    @ingroup  Query
    @author   DirkT
    @brief    Certain Types used throughout Query
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

#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/SQLMan_Context.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "SAPDB/KernelCommon/Kernel_VTrace.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_OStream.hpp"

#include "vak001.h"

// ------------------------------------------------------------------------------------------------
// note: the following defines should be set to UNDEF as they are for testing only

///...
#undef  QRW_REPLACE_ALL_KIND_OF_VIEWS			// replace not only complex views but all views

///...
#undef  QRW_FIND_ROLLBACK_BY_ERROR			    // do rollback in any case BUT set a warning if it wouldn't have occured otherwise
///...
#undef  QRW_FIND_ROLLBACK_BY_NOTYETIMPLEMENTED  //  "

// ------------------------------------------------------------------------------------------------

extern SAPDBTrace_Topic QueryRewrite_Trace;

// ------------------------------------------------------------------------------------------------

/// ...
#define QUERY_RULECOUNT 15

/// ...
#define QRW_MaxTempKeyLen (KEY_MXSP00-4)		// DDT: -4 only in case of additional distinct

// ------------------------------------------------------------------------------------------------

///...
#define QRW_PRINT2TRACE(Level, X) \
    if (QueryRewrite_Trace.TracesLevel(Level)) \
        mVTrace << X << FlushLine;

///...
typedef tak_querycheck_node Query_NodeState;

///...
typedef SAPDB_Int2 Query_JoinType;
///...
#define JT_Unknown           0
///...
#define JT_NoJoin            1
///...
#define JT_Inner             2
///...
#define JT_Cross             4
///...
#define JT_FullOuter         8
///...
#define JT_RightOuter       16 
///...
#define JT_LeftOuter        32
///...
#define JT_Outer            (JT_FullOuter|JT_RightOuter|JT_LeftOuter)

// ------------------------------------------------------------------------------------------------

/// ...
enum Query_TrueFalseNull { QTRUE, QFALSE, QNULL };

///...
enum Query_Progress     { 
                            PG_Error, 
                            PG_Init, 
                            PG_Nodes, 
                            PG_SubstituteViews, 
                            PG_Semantics, 
                            PG_Normalized, 
                            PG_Optimized 
                        };

///...
enum Query_QuantifierType {
                            QT_F, // FROM Part or Part of UNION/INTERSECT/DIFFERENCE
                            QT_A, // Where Part: ALL (SELECT ...)
                            QT_S, // Whery Part =,<,>,etc (SELECT ...)
                            QT_E, // Where Part EXIST/IN/ANY/SOME (SELECT ...)
                            QT_L, // Select list
                            QT_Unknown
                          };
/// ...
enum Query_SubQueryType { 
                            SQT_None, 
                            SQT_SELLIST, 
                            SQT_EQ, 
                            SQT_ALL, 
                            SQT_IN, 
                            SQT_ANY, 
                            SQT_EXISTS, 
                            SQT_OTHER 
                        };

///...
enum Query_DistinctType   { 
                            DT_Enforce, 
                            DT_Permit,
                            DT_Preserve,
                            DT_Unknown
                          };

///...
enum Query_Position     { 
                            P_Start, 
                            P_Union, P_Difference, P_Intersect, 
                            P_Select, P_SelectList, P_FromPart, 
                            P_Where, P_Having, P_GroupBy, P_OrderBy, 
                            P_UpdateColumns, 
							P_Insert, P_Update, P_Delete, P_Unknown
                        };

///...
enum Query_ExpType      {
                            Exp_Column, Exp_Asterisk, Exp_MultiAsterisk, Exp_Unknown
                        };

///...
enum Query_MergeType    {
                            MT_NoMerge, MT_Normal, MT_Aggregates
                        };

///...
#define Query_MaxFromSelects            cak00_maxsubqueries
///...
#define Query_MaxParsCnt                cak00_maxsubqueries
///...
#define Query_MaxSubqueriesPerLevel     cak_maxsubcnt_per_level
///...
#define Query_MaxFQuantifier            cak00_maxsources
///...
#define Query_MaxCorrLevel              cak_maxcorlevel

// ------------------------------------------------------------------------------------------------

///...
typedef tsp00_Int2          SyntaxTreeNodeNo;
///...
//typedef SQLMan_Identifier   Query_Identifier;

// ------------------------------------------------------------------------------------------------

class Query_OutColType;

/*!
    @class          Query_MappingInfo
    @brief          ...
 */
class Query_MappingInfo
{
public:
    ///...
    Query_MappingInfo( Query_OutColType& ColumnType, Query_MergeType MergeType, Query_Position Position, bool AdjustRefname )
        : mColumnType(ColumnType)
        , mMergeType(MergeType)
        , mPosition(Position)
        , mAdjustRefname(AdjustRefname)
    {
    }

    /// ...
    Query_OutColType&   mColumnType;    
    /// ...
    Query_MergeType     mMergeType;     
    /// ...
    Query_Position      mPosition;      
    /// ...
    bool                mAdjustRefname; 
};

// ------------------------------------------------------------------------------------------------

#endif
