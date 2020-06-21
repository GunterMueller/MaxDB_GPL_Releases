/****************************************************************************/
/*!

  @file        SysView_OptimizerInformation.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "OptimizerInformation" view class.

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

#include "vak001.h"
#include "hak47.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_OptimizerInformation.hpp"
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

void SysView_OptimizerInformation::Create(void)
{
	m_Table->AppendCol (ITOCT_CHARUNICODE, SV_SCHEMANAME, 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE, SV_TABLENAME, 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE, SV_COLUMNNAME, 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE, SV_INDEXNAME, 32);
	m_Table->AppendCol (ITOCT_FIXED, SV_DISTINCTVALUECOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_CALCULATEDSIZE, 10);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OptimizerInformation::GetColCount(void)
{
	return SV_CC_OPTIMIZERINFORMATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OptimizerInformation::EstimateRows(void)
{
	return SV_ER_OPTIMIZERINFORMATION;
}

/*---------------------------------------------------------------------------*/

void SysView_OptimizerInformation::Execute(void)
{
    //a47optimize_statistics (m_ACV, a41v);
    a47show_table (m_Context, *(m_Table->GetPascalObjPtr()), cak_i_optimize);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
