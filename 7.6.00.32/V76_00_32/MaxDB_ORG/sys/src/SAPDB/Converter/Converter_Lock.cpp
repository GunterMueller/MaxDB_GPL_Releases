/****************************************************************************

  module      : Converter_Lock.cpp

  -------------------------------------------------------------------------

  responsible : Henrik

  special area:
  description : This description is currently not used, as
    documentation is only generated from header files
-
  last changed: 2002-08-06
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2004 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2004 SAP AG

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

#include "Converter/Converter_Lock.hpp"

#include "heo55k.h"
#include "hgg08.h"        // g08ConvIndex

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
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


Converter_Lock::Converter_Lock( const RTETask_ITask  &task )
        :
        m_Owner( task.ID()),
        m_RegionId( g08ConvIndex )
{
    vbegexcl( m_Owner, m_RegionId );
};

/*---------------------------------------------------------------------------*/

Converter_Lock::~Converter_Lock()
{
    if( RTE_UnknownTaskId != m_Owner ){
        vendexcl( m_Owner, m_RegionId );
    }
};

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/

