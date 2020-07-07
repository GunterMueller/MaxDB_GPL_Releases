/*!
  @file     SysView_UnloadedStatements.cpp
  @author   DirkT
  @ingroup  SystemViews
  @brief    Systemview UNLOADEDSTATEMENTS for SharedSQL
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
#include "SystemViews/SysView_UnloadedStatements.hpp"
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

void SysView_UnloadedStatements::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_UnloadedStatements::Create", SysView_Trace, 5);
    //
	m_Table->AppendCol (ITOCT_CHARBYTE,     SV_COMMANDID, 8);		
    //
    m_Table->AppendCol (ITOCT_LONGUNICODE,  SV_STATEMENT, 0); 
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_UnloadedStatements::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_UnloadedStatements::GetColCount", SysView_Trace, 5);

	return SV_CC_UNLOADEDSTATEMENTS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_UnloadedStatements::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_UnloadedStatements::EstimateRows", SysView_Trace, 5);

	return SV_ER_UNLOADEDSTATEMENTS;
}

/*---------------------------------------------------------------------------*/

void SysView_UnloadedStatements::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_UnloadedStatements::Execute", SysView_Trace, 5);

// 2 Cols

    m_Table->GetCatalogTable();

    SharedSQL_SQLCache& SQLCache = m_Context.GetSharedSQL_SQLCache();

    SharedSQL_CommandCacheIterator Iter = SQLCache.GetCommandCacheIterator();

	
	void* Buffer=0;
	SAPDB_ULong BufferSize=0;
    while (Iter.IsValid())
    {
        SharedSQL_CommandInfo S;
        Iter->GetCommandInfo(S);
       
        if ( !S.mStmt )
        {
			SAPDB_ULong RequestSize = S.mStmtSize + S.mDescriptionSize;
			if ( BufferSize<RequestSize )
			{
				Buffer = m_Context.GetAllocator().Reallocate(RequestSize, Buffer);
			}
			if ( Buffer )
			{
				bool rc = Iter->ReloadStatementIntoBuffer(Buffer, RequestSize);

				m_Table->MoveToCol (ITOVT_INT8,  &S.mCommandID, 8);	
				// copy SQL statement and build long descriptor
				if ( rc )
					m_Table->MoveToLongColContainer (Buffer, S.mStmtSize);
				else
					m_Table->MoveToCol (ITOVT_NULL, &rc, 0); // NULL
			}
        }
        ++Iter;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
