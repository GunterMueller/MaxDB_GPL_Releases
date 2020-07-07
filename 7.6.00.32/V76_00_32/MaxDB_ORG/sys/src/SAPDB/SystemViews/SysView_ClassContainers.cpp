/****************************************************************************/
/*!

  @file        SysView_ClassContainers.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "ClassContainers" view class.

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
#include "SystemViews/SysView_ClassContainers.hpp"


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

void SysView_ClassContainers::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainers::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED ,SV_FILEID, 20);
	m_Table->AppendCol (ITOCT_CHAR ,SV_CLASSGUID, 40);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_SCHEMANAME, 32);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CONTAINERNUMBER, 10);
	m_Table->AppendCol (ITOCT_CHAR ,SV_CLASSNAME, 32);
	m_Table->AppendCol (ITOCT_FIXED ,SV_OBJECTSIZE, 5);
	m_Table->AppendCol (ITOCT_CHAR ,SV_VAROBJECTCONTAINER, 3);
	m_Table->AppendCol (ITOCT_CHAR ,SV_KEYDEFINED, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_KEYPARTITIONCOUNT, 5);
	m_Table->AppendCol (ITOCT_TIMESTAMP ,SV_CREATEDATE, 0);
	m_Table->AppendCol (ITOCT_LONGUNICODE ,SV_COMMENT, 0);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ClassContainers::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainers::GetColCount", SysView_Trace, 5);

    return SV_CC_CLASSCONTAINERS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ClassContainers::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainers::EstimateRows", SysView_Trace, 5);

    return SV_ER_CLASSCONTAINERS;
}

/*---------------------------------------------------------------------------*/

void SysView_ClassContainers::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainers::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
