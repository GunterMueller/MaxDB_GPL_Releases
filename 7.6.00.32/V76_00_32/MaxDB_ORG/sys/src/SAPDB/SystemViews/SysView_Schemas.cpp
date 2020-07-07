/****************************************************************************/
/*!

  @file        SysView_Schemas.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "Schemas" view class.

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
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_Schemas.hpp"


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

void SysView_Schemas::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Schemas::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_OWNER, 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_SCHEMANAME, 32);
	m_Table->AppendCol (ITOCT_CHARBYTE ,SV_ID, 8);
	m_Table->AppendCol (ITOCT_TIMESTAMP ,SV_CREATEDATE, 0);
	m_Table->AppendCol (ITOCT_LONGUNICODE ,SV_COMMENT, 0);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Schemas::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Schemas::GetColCount", SysView_Trace, 5);

    return SV_CC_SCHEMAS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Schemas::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Schemas::EstimateRows", SysView_Trace, 5);

    return SV_ER_SCHEMAS;
}

/*---------------------------------------------------------------------------*/

void SysView_Schemas::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Schemas::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
