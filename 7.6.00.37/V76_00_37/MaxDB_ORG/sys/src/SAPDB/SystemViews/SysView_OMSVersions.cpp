/****************************************************************************/
/*!

  @file        SysView_OMSVersions.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "OMSVersions" view class.

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
#include "SystemViews/SysView_OMSVersions.hpp"


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

void SysView_OMSVersions::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_OMSVersions::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHAR ,SV_OMSVERSIONID, 22);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CONSISTENTVIEWID, 15);
	m_Table->AppendCol (ITOCT_TIMESTAMP ,SV_LASTOPENDATE, 0);
	m_Table->AppendCol (ITOCT_CHAR ,SV_OPENVERSION, 3);
	m_Table->AppendCol (ITOCT_CHAR ,SV_ROLLEDOUT, 3);
	m_Table->AppendCol (ITOCT_CHARBYTE ,SV_FILEID, 8);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_DESCRIPTION, 256);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OMSVersions::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_OMSVersions::GetColCount", SysView_Trace, 5);

    return SV_CC_OMSVERSIONS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OMSVersions::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_OMSVersions::EstimateRows", SysView_Trace, 5);

    return SV_ER_OMSVERSIONS;
}

/*---------------------------------------------------------------------------*/

void SysView_OMSVersions::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_OMSVersions::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
