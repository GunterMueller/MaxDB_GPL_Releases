/****************************************************************************/
/*!

  @file        SysView_ClassContainerStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "ClassContainerStatisticsReset" view class.

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
#include "SystemViews/SysView_ClassContainerStatisticsReset.hpp"


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

void SysView_ClassContainerStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_FIXED ,SV_FILEID, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDSIZEPERCENTAGE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ALLOCATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CREATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DELETEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_UPDATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READOBJECTVIAKEYCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SCANOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SCANOBJECTVIAKEYRANGECOUNT, 20);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ClassContainerStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_CLASSCONTAINERSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ClassContainerStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatisticsReset::EstimateRows", SysView_Trace, 5);

    return SV_ER_CLASSCONTAINERSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_ClassContainerStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatisticsReset::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
