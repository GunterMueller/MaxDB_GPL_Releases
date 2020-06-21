/****************************************************************************

  module      : Pager_Lock.cpp

  -------------------------------------------------------------------------

  responsible : Henrik

  special area:
  description : This description is currently not used, as
    documentation is only generated from header files
-
  last changed: 2002-02-16
  see also    :

  -------------------------------------------------------------------------

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

#include "Pager/Pager_Lock.hpp"

#include "heo55k.h"
#include "hgg08.h"			// g08datawriter

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


Pager_Lock::Pager_Lock( tsp00_TaskId TaskId )
        :
        m_Owner( TaskId ),
        m_RegionId( g08datawrtr )
{
    vbegexcl( m_Owner, m_RegionId );
};

/*---------------------------------------------------------------------------*/

Pager_Lock::~Pager_Lock()
{
    if(0 != m_Owner){
        vendexcl( m_Owner, m_RegionId );
    }
};

void Pager_Lock::unlock(){
    if(0 != m_Owner){
        vendexcl( m_Owner, m_RegionId );
    }
    m_Owner = 0;
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/

