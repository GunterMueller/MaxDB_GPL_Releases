/****************************************************************************

  module      : Trace_PageNoEntry.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: 
  description : 


  last changed: 2001-05-17  11:56
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef TRACE_PAGENOENTRY_HPP
#define TRACE_PAGENOENTRY_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "ggg11.h"
#include "gsp00.h"

#include "Trace/Trace_Entry.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


class Trace_PageNoEntry : public Trace_CommonEntry
{
public:
    
    Trace_PageNoEntry(
        tsp00_PageNo                PageNo,
        tgg00_VtraceType_Param      TraceObject
        )
        :Trace_CommonEntry( tgg00_Debug::fromConst( bd ), 
        tgg00_VtraceType::fromConst( TraceObject ), 
        REINTERPRET_CAST( tsp00_BytePtr, &m_Entry ), 
        sizeof( m_Entry ) - 2 * sizeof( m_Entry.bdiInt_gg11[ 1 ] ))
    {
       m_Entry.bdiTrType_gg11.becomes( bdtrInt_egg11 );
       m_Entry.bdiInt_gg11[ 0 ] = PageNo;
    }
    
private:
    
    tgg11_BdIntTrace    m_Entry;
};

#endif  /* TRACE_PAGENOENTRY_HPP */
