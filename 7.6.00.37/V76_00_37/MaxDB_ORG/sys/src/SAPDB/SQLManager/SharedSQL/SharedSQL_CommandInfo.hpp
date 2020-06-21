/*!
    @file     SharedSQL_CommandInfo.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    SQLCommands cached by the SharedSQL_CommandCache
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

#ifndef SHAREDSQL_ICOMMAND_HPP
#define SHAREDSQL_ICOMMAND_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_CommandStatistics.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_CommandInfo
    @brief          Information about the stored SharedSQL_Command, used the SystemView COMMANDSTATISTICS
 */
class SharedSQL_CommandInfo
{
public:
    /// ResetTime
    SAPDB_UInt8                     mResetTime;
    /// Unique ID to identify the command
	SharedSQL_CommandID				mCommandID;
    /// Flag to indicate wheather or not the command is an internal one
    bool                            mInternal;
    /// Hash value of the command
	SharedSQL_HashValue				mHashValue;
    /// The hash table slot the command is stored in
    SharedSQL_HashValue             mSlot;
    //
    /// The context of the command
    SharedSQL_SQLContext            mSQLContext;
    //
    /// The Status (New, Dropped, Preparing, Prepared, Invalid)
    SharedSQL_CommandStatus	        mStatus;
    /// Flag to indicate that the sql statement is unloaded to the temporary file
	SAPDB_Byte			    	    mFlgExtern;
    /// ...
    SAPDB_Int4                      mPrepareCount; 	
    /// ...
    SAPDB_Int4                      mParseIDCount;
    /// ...
    SAPDB_Int4                      mExecuteCount;
    //
    /// Size of the sql statement
    SAPDB_Int4                      mStmtSize;
    /// Size of the sql statement
    SAPDB_Int4                      mDescriptionSize;
    /// Pointer to the sql statement (NULL if the statement is unloaded to the temporary file)
    void*	                        mStmt;
	/// Information if the statement has been rewritten
	SQLMan_QueryRewriteMode			mQueryRewrite;
    /// Size of the execution plan
    SAPDB_Int4                      mPlanSize;
    //
    /// Statistical information
    SharedSQL_CommandStatistics     mStatistics;
};  // SharedSQL_CommandInfo

/* ------------------------------------------------------------------------- */

#endif  /* SHAREDSQL_ICOMMAND_HPP */

