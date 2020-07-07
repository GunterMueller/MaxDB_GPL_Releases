/*!*************************************************************************

  module      : Trace_DevSpace.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Handling of trace devspace
  description : This class implements the interface to the kernel trace devspace

  last changed: 2000-12-07  18:15
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#ifndef TRACE_DEVSPACE_HPP
#define TRACE_DEVSPACE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


class Trace_DevSpace
{
public:
	
	Trace_DevSpace();
	

	void Initialize( tsp00_VFilename &DumpFile );

	void Close( tsp00_TaskId TaskId );

	bool Open( tsp00_TaskId TaskId );
	
	bool Write(
		tsp00_TaskId	TaskId,
		SAPDB_Int4		UKTId,
		tsp00_PageAddr	pPageList,
		SAPDB_Int4		NumPages);

	bool WriteInfoPage(	tsp00_TaskId TaskId );

public:

	bool			    m_Active;
	SAPDB_Int4		    m_LogicalDevNo;
	SAPDB_Int4		    m_PhysicalDevNo;
    const SAPDB_Int4    m_BlockCount; // PTS 1110822 TS 2001-07-19 
	tsp00_VFilename	    m_DevName;
	tsp00_PageAddr      m_InfoPage; // PTS 1108976
};


/*===========================================================================*
 *  IMPLEMENTATION OF METHODS                                                *
 *===========================================================================*/

#endif  /* TRACE_DEVSPACE_HPP */
