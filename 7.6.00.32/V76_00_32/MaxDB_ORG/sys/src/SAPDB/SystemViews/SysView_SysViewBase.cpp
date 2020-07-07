/****************************************************************************/
/*!

  @file        SysView_SysViewBase.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "DataCache" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "vak001.h"
#include "gak40.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SystemViews/SysView_SysViewBase.hpp"
#include "SystemViews/SysView_PascalTableObj.hpp"
#include "RunTime/System/RTESys_Time.h"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_DBProcDescriptor.hpp"

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

SysView_SysViewBase::SysView_SysViewBase( SQLMan_Context	&context,
                                          tak40_show_glob   &a41v)
: m_Context(context)
, AsTimestamp(1)
{
    m_Table = new(context.GetAllocator()) SysView_PascalTableObj( context, a41v );
}

SysView_SysViewBase::~SysView_SysViewBase()
{
    if (m_Table)
    {
        destroy (m_Table, m_Context.GetAllocator());
        m_Table = NULL;
    }

}

void SysView_SysViewBase::AddColumn(SAPDB_UInt8 timeStamp, const SAPDB_Int4)
{
    if ( 0 == timeStamp )
    {
        /* Add column with NIL value */
        AddColumn();
    }
    else
    {
        /* convert timestamp into readable string */
        RTESys_SQLTimestamp date;

        AddColumn(RTESys_BuildSQLTimeStamp(timeStamp, date), sizeof(RTESys_SQLTimestamp));
    }
}

static void int2string ( SAPDB_Char  *target,
                         SAPDB_UInt4  intval,
                         SAPDB_UInt   intsize)
{
    for ( SAPDB_Int i = intsize - 1; i >= 0; --i)
	{
        SAPDB_Char digit = (SAPDB_Char) (intval % 10);
        target [i] = digit + '0';
        intval /= 10;
    }
}

static bool ints2timestamp ( tsp00_Timestamp &datetime,
                             SAPDB_UInt4       date,
                             SAPDB_UInt4       time)
{
    if ( 0 == date || 0 == time )
        return false;

    for (SAPDB_UInt i = 0; i < sizeof(datetime); ++i)
        datetime [i] = '0';
        
    int2string (datetime.asCharp(),     date, 8);
    int2string (datetime.asCharp() + 8, time, 6);
    return true;
}

void SysView_SysViewBase::AddColumn(SAPDB_UInt4 date, SAPDB_UInt4 time)
{
    tsp00_Timestamp datetime;
    
    if ( ! ints2timestamp (datetime, date, time ) )
    {
        /* Add column with NULL value */
        AddColumn();
        return;
    }
	m_Table->MoveToCol (ITOVT_CHARPTR, &datetime[0], sizeof(datetime));  
}

void SysView_SysViewBase::AddColumn(const SQLMan_Identifier& Identifier)
{
    m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &Identifier[0], 0);
}

void SysView_SysViewBase::AddProcedureSchemaAndName (const Catalog_ObjectId& DBProcId)
{
     SAPDBTRACE_METHOD_DEBUG ("SysView_SysViewBase::AddProcedureSchemaAndName", SysView_Trace, 5);
     if ( !DBProcId.IsNull() )
     {
         Catalog_DBProcDescriptor dbprocDesc(m_Context);
         Msg_List msgList;
         Catalog_ReturnCodes rc = dbprocDesc.Create(DBProcId, msgList);
         if (Catalog_ReturnCodes::CatRcOk == rc)
         {
             SQLMan_Identifier schemaName;
             SQLMan_Identifier procedureName;
             rc = dbprocDesc.GetSchemaName (schemaName, msgList);
             if (Catalog_ReturnCodes::CatRcOk == rc)
             {
                 rc = dbprocDesc.GetName (procedureName, msgList);
             }
             if (Catalog_ReturnCodes::CatRcOk == rc)
             {
                 m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &schemaName[0], 0);
                 m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &procedureName[0], 0);
                 return;
             }
         }
     }
     AddColumn();
     AddColumn();
}


void SysView_SysViewBase::Execute (void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SysViewBase::Execute", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 1);
}
 
SAPDB_Bool SysView_SysViewBase::ResetCounter (void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SysViewBase::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_unknown_tablename, 7);
    return (false);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
