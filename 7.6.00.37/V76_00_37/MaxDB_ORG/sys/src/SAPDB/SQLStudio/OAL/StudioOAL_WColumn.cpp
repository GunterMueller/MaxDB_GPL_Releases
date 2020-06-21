/*

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




*/


/*!**********************************************************************

  module: StudioOAL_WColumn.cpp

  -----------------------------------------------------------------------

  responsible: WolfgangA, Hans-GeorgB

  special area: WebQuery, ReplMan, DBAnalyzer

  description:  Unicode Version of OAL

  version     : 7.3


************************************************************************/

#include "SAPDB/SQLStudio/OAL/StudioOAL_WColumn.hpp"

/*!**********************************************************************

  Class: StudioOAL_WColumn

************************************************************************/
StudioOAL_WColumn::StudioOAL_WColumn()
{
	m_bColNullable    = SAPDB_FALSE;
	m_bColIsNull      = SAPDB_FALSE;
	m_bColIsKey       = SAPDB_FALSE;
	m_bColIsUpdatable = SAPDB_FALSE;
	m_pValue          = NULL;
	m_ValueLength     = -1;
}

StudioOAL_WColumn::~StudioOAL_WColumn()
{
	if ( m_pValue != NULL )
	{
		delete[] m_pValue;
		m_pValue = NULL;
	}
}

/*===========================================================================*
 *  END
 *===========================================================================*/


