/****************************************************************************

  module      : Trace_Entry.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:
  description : 

  last changed: 2000-11-27  18:15
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg91.h"          // CPP   : GG_kernel_templates

#include "Trace/Trace_Entry.hpp"
#include "Trace/Trace_Manager.hpp"

#include "Transaction/Trans_Context.hpp"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp" 

#include "RunTime/Tasking/RTETask_ITask.hpp"

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

bool
Trace_OverflowEntry::Materialize(
    tsp00_BytePtr  pDestination,
    SAPDB_Int4     DestinationSize ) const
{

    if ( this->GetSize() > DestinationSize )
        return( false );

    SAPDB_Int4 Offset = 0;

    //Copy current part number into the trace
    SAPDB_MemCopyNoCheck( pDestination, &m_PartCount, sizeof( m_PartCount ));
    Offset += sizeof( m_PartCount );

    // Copy original trace layer of the complete entry into the trace
    SAPDB_MemCopyNoCheck(( pDestination + Offset ), &m_OriginalLayer, sizeof( m_OriginalLayer ));
    Offset += sizeof( m_OriginalLayer );

    // Copy proper entry into the trace
    SAPDB_MemCopyNoCheck(( pDestination + Offset ), m_pEntry, m_EntrySize );

    return( true );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Trace_CommonEntry::~Trace_CommonEntry()
{
    Trace_Manager::GetInstance().Insert( *this );
};

/*---------------------------------------------------------------------------*/

bool
Trace_CommonEntry::Materialize(
    tsp00_BytePtr  pDestination,
    SAPDB_Int4     DestinationSize ) const
{
    if ( m_EntrySize > DestinationSize )
        return( false );

    SAPDB_MemCopyNoCheck( pDestination, m_pEntry, m_EntrySize );
    return( true );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Trace_KernelFullEntry::Trace_KernelFullEntry(
    Trans_Context       &trans,
    Trace_IBaseEntry    &Entry,
    const Trace_SeqNo   SequenceNo )
        :
        m_Info( Entry )
{
    tgg91_SessionNo sessionNo;

    if ( trans.IsNilSession())
        sessionNo.gg91BuildTaskSession ( trans.Task().Type());
    else
    {
        if ( trans.IsNormalSession())
            sessionNo = trans.SessionId();
        else
            sessionNo.gg91BuildInternalTraceSession( trans.SubtransId());
    }

    m_Header.vhLen_gg11         = tsp00_Int2 (( sizeof( tgg11_VtraceHeader ) + Entry.GetSize() )); // cast to avoid warning C4244 on IA64
    m_Header.vhLayer_gg11       = Entry.GetLayer();
    m_Header.vhType_gg11        = Entry.GetType();
    m_Header.vhSequenceNo_gg11  = SequenceNo;
    m_Header.vhTask_gg11        = trans.Task().ID();
    m_Header.vhTrans_gg11       = trans.TransId();
    m_Header.vhSession_gg11     = sessionNo;
}

/*---------------------------------------------------------------------------*/

bool
Trace_KernelFullEntry::Materialize(
    tsp00_BytePtr    pDestination,
    SAPDB_Int4       DestinationSize  ) const
{
    if ( this->GetSize() > DestinationSize )
        return( false );

    // Copy m_header to destination
    SAPDB_MemCopyNoCheck( pDestination, &m_Header, sizeof( m_Header ));
    pDestination    += sizeof( m_Header );
    DestinationSize -= sizeof( m_Header );

    // Build entry body on destination space
    return( m_Info.Materialize( pDestination, DestinationSize ));
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
