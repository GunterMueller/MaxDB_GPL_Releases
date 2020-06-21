/*!***************************************************************************

    @file     Query_ManipulationContext.hpp
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

#ifndef QUERY_MANIPULATIONCONTEXT_H
#define QUERY_MANIPULATIONCONTEXT_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_Types.hpp"

#include "SAPDB/KernelCommon/Kernel_VTrace.hpp"

#include "hak01.h"
#include "hgg01_3.h"
#include "hsp30.h"

// ------------------------------------------------------------------------------------------------

class SQLMan_Context;
class SAPDBMem_RawAllocator;
class SharedSQL_PrepareHandle;

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_ManipulationContext
    @brief          ...
 */
class Query_ManipulationContext
{
public:
    /// ...
    Query_ManipulationContext( SQLMan_Context& SQLManContext, SharedSQL_PrepareHandle* prepareHandle );

    /// ...
    ~Query_ManipulationContext( void );

    /// ...
    inline bool KeepChanges( void )
    {
        SAPDBERR_ASSERT_STATE( !mDecision );

        mDecision = true;
        return KeepNewSyntaxTreeAndStatementBuffer();
    }

    /// ...
    inline bool DiscardChanges( void )
    {
        SAPDBERR_ASSERT_STATE( !mDecision );

        mDecision = true;
        return RestoreOldSyntaxTreeAndStatementBuffer();
    }

    /// ...
    inline bool IsOk( void )
    {
        return (!mError && !mNotYetImplemented);
    }

    /// ...
    void SetError( void )
    {
        mError = true;
    }

    /// ...
    void SetNotYetImplemented( void )
    {
        mNotYetImplemented = true;
    }

    /// ...
    SAPDBMem_RawAllocator& GetAllocator( void ) const;
    /// ...
    void* GetSQLStatementPtr( void ) const;

    /// ...
	SharedSQL_PrepareHandle* GetPrepareHandle( void ) const { return mPrepareHandle; }

    /// ...
    int GetSQLStatementSize( void ) const;
    /// ...
    bool AppendBlankToSQLStatement(void);
    /// ...
    SAPDB_Int4 AppendToSQLStatement( char* ToAdd );
    /// ...
    SAPDB_Int4 AppendToSQLStatement( void* ToAdd, SAPDB_Int4 AddSize );
    /// ...
    bool ExtendSQLStatementBuffer( SAPDB_Int4 RequiredSize );

    /// ...
    inline SQLMan_Context& SQLManContext( void )
    {
        return mSQLManContext;
    }

    /// ...
    inline Kernel_VTrace& VTrace( void )
    {
        return mVTrace;
    }

protected:
    /// ...
    bool CreateNewSyntaxTreeAndStatementBuffer( void );
    /// ...
    bool RestoreOldSyntaxTreeAndStatementBuffer( void );
    /// ...
    bool KeepNewSyntaxTreeAndStatementBuffer( void );

private:
    /// ...
    SQLMan_Context&     mSQLManContext;   

    /// ...
	SharedSQL_PrepareHandle* mPrepareHandle;            

    /// ...
    Kernel_VTrace       mVTrace;                        

    /// ...
	void*				mOldContextStmtPart;            

    /// ...
    SAPDB_Byte*         mSQLStatementBuffer;            
    /// ...
    SAPDB_Int4          mSQLStatementBufferMaxSize;     
    /// ...
    SAPDB_Int4          mSQLStatementBufferUsedSize;    
    /// ...
    bool                mSQLStatementBufferChanges;     

    /// ...
    bool                mError;                         
    /// ...
	bool                mNotYetImplemented;             
    /// ...
    bool                mDecision;     
#if defined( SAPDB_SLOW )
	/// ...
	SAPDBMem_RawAllocator* mRawAllocator;
    ///
    SAPDB_ULong         mSavedBytesUsed;
#endif
};

// ------------------------------------------------------------------------------------------------

#endif  // QUERY_MANIPULATIONCONTEXT_H