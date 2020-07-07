/*!
  @file     SysView_CommandStatistics.cpp
  @author   DirkT
  @ingroup  SystemViews
  @brief    Systemview COMMANDSTATISTICS for SharedSQL_Commands
  @see            

\if EMIT_LICENCE
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

\endif
*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "hgg01_3.h"
#include "hgg03.h"
#include "hsp78.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_CommandStatistics.hpp"
#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Command.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_Context.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_CommandStatistics::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Commands::Create", SysView_Trace, 5);
    //
	m_Table->AppendCol (ITOCT_CHARBYTE,     SV_COMMANDID, 8);		
	m_Table->AppendCol (ITOCT_CHAR,         SV_INTERNAL, 3);
    m_Table->AppendCol (ITOCT_FIXED,        SV_HASHVALUE, 10);
    m_Table->AppendCol (ITOCT_FIXED,        SV_HASHTABLESLOT, 5);
	m_Table->AppendCol (ITOCT_CHAR,         SV_STATUS, 10);
    //
    m_Table->AppendCol (ITOCT_CHARUNICODE,  SV_USERNAME, 32);
    m_Table->AppendCol (ITOCT_CHARUNICODE,  SV_SCHEMANAME, 32);
	m_Table->AppendCol (ITOCT_CHAR,         SV_SQLMODE, 8);
	m_Table->AppendCol (ITOCT_FIXED,        SV_ISOLATIONLEVEL, 3);
	m_Table->AppendCol (ITOCT_CHAR,         SV_DATEFORMAT, 50);
	m_Table->AppendCol (ITOCT_CHAR,         SV_MASSCOMMAND, 3);
	m_Table->AppendCol (ITOCT_CHAR,         SV_STATEMENTCODETYPE, 15);
	m_Table->AppendCol (ITOCT_CHAR,         SV_QUERYREWRITE,	10);
    //
    m_Table->AppendCol (ITOCT_FIXED,        SV_STATEMENTSIZE, 10);		
    m_Table->AppendCol (ITOCT_LONGUNICODE,  SV_STATEMENT, 0); 
    m_Table->AppendCol (ITOCT_FIXED,        SV_PLANSIZE, 10);
    //
    m_Table->AppendCol (ITOCT_CHARUNICODE,  SV_DBPROCEDURESCHEMANAME, 32);
    m_Table->AppendCol (ITOCT_CHARUNICODE,  SV_DBPROCEDURENAME, 32);
    //
    m_Table->AppendCol (ITOCT_FIXED,    SV_PREPARECOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED,    SV_REFERENCECOUNT, 5);
    m_Table->AppendCol (ITOCT_FIXED,    SV_CURRENTEXECUTECOUNT, 5);
    m_Table->AppendCol (ITOCT_FIXED,    SV_EXECUTECOUNT, 10);
    //
    m_Table->AppendCol (ITOCT_FIXED,    SV_PREPARETIME, 20);		
    m_Table->AppendCol (ITOCT_FIXED,    SV_EXECUTETIME, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_AVGEXECUTETIME, 20);	
    m_Table->AppendCol (ITOCT_FIXED,    SV_MINEXECUTETIME, 20);	
    m_Table->AppendCol (ITOCT_FIXED,    SV_MAXEXECUTETIME, 20);	
    //
    m_Table->AppendCol (ITOCT_FIXED,    SV_READROWCOUNT, 20);	
    m_Table->AppendCol (ITOCT_FIXED,    SV_QUALIFIEDROWCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_VIRTUALREADCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_PHYSICALREADCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_FETCHROWCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_SUSPENDCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_WAITCOUNT, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CommandStatistics::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Commands::GetColCount", SysView_Trace, 5);

	return SV_CC_COMMANDSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CommandStatistics::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Commands::EstimateRows", SysView_Trace, 5);

	return SV_ER_COMMANDSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_CommandStatistics::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Commands::Execute", SysView_Trace, 5);

// 34 Cols

    m_Table->GetCatalogTable();

    SharedSQL_SQLCache& SQLCache = m_Context.GetSharedSQL_SQLCache();

    SharedSQL_CommandCacheIterator Iter = SQLCache.GetCommandCacheIterator();
    while (Iter.IsValid())
    {
        SharedSQL_CommandInfo S;
        Iter->GetCommandInfo(S);
       
        SAPDB_Int8 AVGTime = 0;
        SAPDB_Int8 mInt8 = 0;
        SAPDB_Int4 mInt4 = 0;

        m_Table->MoveToCol (ITOVT_INT8,  &S.mCommandID, 8);	
	    m_Table->MoveToCol (ITOVT_YESNO, &S.mInternal, 0);
        SAPDB_UInt4 Hash = S.mHashValue;    m_Table->MoveToCol (ITOVT_UINT4, &Hash, 0);
        SAPDB_UInt4 Slot = S.mSlot;         m_Table->MoveToCol (ITOVT_UINT4, &Slot, 0);
			switch (S.mStatus)
			{
			case New: 
				m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"NEW", 3);
				break;
			case Preparing: 
				m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"PREPARING", 9);
				break;
			case Prepared: 
				m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"PREPARED", 8);
				break;
			case Dropped: 
				m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"DROPPED", 7);
				break;
			case Invalid: 
				m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"INVALID", 7);
				break;
			default:
				m_Table->MoveToCol (ITOVT_NULL, &AVGTime, 0);
				break;
			}
        //
        m_Table->MoveToCol (ITOVT_USER,	            S.mSQLContext.UserID, 0);
        m_Table->MoveToCol (ITOVT_SCHEMA,	        S.mSQLContext.CurrSchema, 0);
    	m_Table->MoveToCol (ITOVT_SQLMODE,          &S.mSQLContext.SQLMode, 0);
        SAPDB_Int2 Level = S.mSQLContext.IsolationLevel; m_Table->MoveToCol (ITOVT_INT2, &Level, 0);


        if (S.mSQLContext.SQLMode != sqlm_oracle)
        {
    	    m_Table->MoveToCol (ITOVT_DATETIMEFORMAT,   &S.mSQLContext.DateTimeFormat, 0);
        }
        else
        {
            char UserDateFormat[50];
            s78df_clear(g03dictionary.dict[1], 
                        (tsp00_MoveObjPtr)&S.mSQLContext.NLSParam_DateFormat, S.mSQLContext.NLSParam_DFLength,
                        (tsp00_MoveObjPtr)UserDateFormat, 50);
            m_Table->MoveToCol (ITOVT_CHARPTR,          (SAPDB_Char *) UserDateFormat, 50);
        }

	    m_Table->MoveToCol (ITOVT_YESNO,            &S.mSQLContext.MassCmd, 0);
        //
        m_Table->MoveToCol (ITOVT_CODETYPE,         &S.mSQLContext.CodeType, 0);
		//
		m_Table->MoveToCol (ITOVT_QUERYREWRITEMODE, (void*)&S.mQueryRewrite, 0);
        //
        m_Table->MoveToCol (ITOVT_INT4,             &(S.mStmtSize), 0);

        if ( S.mStmt )
        {
            // copy SQL statement and build long descriptor
            m_Table->MoveToLongColContainer (S.mStmt, S.mStmtSize);
        }
        else
        {
            m_Table->MoveToCol (ITOVT_NULL, &AVGTime, 0); // NULL
        }
        m_Table->MoveToCol (ITOVT_INT4,    &S.mPlanSize, 0);
        //
        AddProcedureSchemaAndName (S.mSQLContext.DBProc);
        //
        mInt4 = S.mStatistics.GetTotalPrepareCount();   m_Table->MoveToCol (ITOVT_INT4, &mInt4, 0);
        mInt4 = S.mParseIDCount;                        m_Table->MoveToCol (ITOVT_INT4, &mInt4, 0);
        mInt4 = S.mExecuteCount;                        m_Table->MoveToCol (ITOVT_INT4, &mInt4, 0);
        //
        mInt4 = S.mStatistics.GetTotalExecuteCount();   m_Table->MoveToCol (ITOVT_INT4, &mInt4, 0);
        mInt8 = S.mStatistics.GetParseTime();           m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);		
        mInt8 = S.mStatistics.GetExecuteTime();         m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);
        SAPDB_Int8 mExecTime  = S.mStatistics.GetExecuteTime();
        SAPDB_Int8 mExecCount = S.mStatistics.GetTotalExecuteCount();
        if ( mExecCount > 0 )
        {
            AVGTime = (mExecTime / mExecCount);
            m_Table->MoveToCol (ITOVT_UINT8, &AVGTime, 0); 
        }
        else
        {
            m_Table->MoveToCol (ITOVT_NULL, &AVGTime, 0); // NULL
        }
        //
        mInt8 = S.mStatistics.GetMinExecuteTime();      m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetMaxExecuteTime();      m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetReadRowCount();        m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetQualifiedRowCount();   m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetVirtualReadCount();    m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetPhysicalReadCount();   m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetFetchRowCount();       m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetSuspendCount();        m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetWaitCount();           m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	

        ++Iter;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_CommandStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandStatistics::ResetCounter", SysView_Trace, 5);

	SharedSQL_SQLCache& SQLCache = m_Context.GetSharedSQL_SQLCache();
	SQLCache.ResetCommandInfos();

    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
