/****************************************************************************/
/*!

  @file        SysView_Miscellaneous.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "Miscellaneous" view class.

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
#include "SystemViews/SysView_Miscellaneous.hpp"


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

void SysView_Miscellaneous::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Miscellaneous::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_TABLENAME, 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_COLUMNNAME, 32);
	m_Table->AppendCol (ITOCT_FIXED ,SV_NUMERICVALUE, 20);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_VALUE, 256);
	m_Table->AppendCol (ITOCT_LONGUNICODE ,SV_COMMENT, 0);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Miscellaneous::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Miscellaneous::GetColCount", SysView_Trace, 5);

    return SV_CC_MISCELLANEOUS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Miscellaneous::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Miscellaneous::EstimateRows", SysView_Trace, 5);

    return SV_ER_MISCELLANEOUS;
}

/*---------------------------------------------------------------------------*/

void SysView_Miscellaneous::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Miscellaneous::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
