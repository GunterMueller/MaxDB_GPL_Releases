/****************************************************************************/
/*!

  @file        SysView_OmsDiagnose.cpp

  -------------------------------------------------------------------------

  @author      MarkusSi

  @ingroup     SystemViews
  @brief       This module implements the "OMSDIAGNOSE" view class.

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
#include "SystemViews/SysView_OMSDiagnose.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "hak341.h"




/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_OMSDiagnose::Create()
{

	m_Table->AppendCol (ITOCT_FIXED, SV_SESSIONID,    10);
	m_Table->AppendCol (ITOCT_CHAR,  SV_OMSCOMPONENT, 24);
	m_Table->AppendCol (ITOCT_CHAR,  SV_OMSDIAGENTRY, 512);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OMSDiagnose::GetColCount()
{
  SAPDBTRACE_METHOD_DEBUG ("SysView_OMSDiagnose::GetColCount", SysView_Trace, 5);
	
  return SV_CC_OMSDIAGNOSE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OMSDiagnose::EstimateRows()
{
  SAPDBTRACE_METHOD_DEBUG ("SysView_OMSDiagnose::EstimateRows", SysView_Trace, 5);
	
  return SV_ER_OMSDIAGNOSE;
}

/*---------------------------------------------------------------------------*/

void SysView_OMSDiagnose::Execute()
{
  SAPDBTRACE_METHOD_DEBUG ("SysView_OMSDiagnose::Execute", SysView_Trace, 5);

  tsp00_Int2   phase;
  tsp00_Int4   session;
  tsp00_C24    component;
  tsp00_C512   entry; 

  m_Table->GetCatalogTable();

  phase = 1;
  while (ak341NextDiagnoseEntry (m_Context, phase, session, component, entry)){
    m_Table->MoveToCol (ITOVT_INT4,    &session,   0);
    m_Table->MoveToCol (ITOVT_CHARPTR, component, (SAPDB_Int)strlen((SAPDB_Char *)component));
    m_Table->MoveToCol (ITOVT_CHARPTR, entry,     (SAPDB_Int)strlen((SAPDB_Char *)entry));
  }

}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
