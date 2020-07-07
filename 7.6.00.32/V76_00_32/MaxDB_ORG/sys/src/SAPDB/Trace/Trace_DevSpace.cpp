/***************************************************************************

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp03.h"      // PASCAL: SP_message_constants_and_types
#include "gsp03_3.h"    // PASCAL: SP_message_constants_and_types_for_bd_layer
#include "heo54k.h"     // RTE   : Vattach/Vdetach/Vblockio
#include "heo58.h"      // RTE   : Vdiaginit
#include "hgg01_1.h"    // PASCAL: Configuration_Parameter
#include "vsp002.h"     // PASCAL: IO Handling

#include "Trace/Trace_DevSpace.hpp"
#include "Trace/Trace_InfoPage.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define FIRST_DEVSPACE_PNO  0

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


Trace_DevSpace::Trace_DevSpace()
        :
        m_Active( false ),
        m_LogicalDevNo( 1 ),
        m_PhysicalDevNo( 0 ),
        m_BlockCount( 64 ), // PTS 1110822 TS 2001-07-19
        m_InfoPage( 0 )
{
    Trace_InfoPage::GetInfoPage( m_InfoPage );
};

/*---------------------------------------------------------------------------*/

void
Trace_DevSpace::Initialize( tsp00_VFilename  &DumpFile )

{
    if (m_Active) return;

    vInitTraceAndDump( m_DevName, DumpFile );
};

/*---------------------------------------------------------------------------*/

void
Trace_DevSpace::Close( tsp00_TaskId TaskId )
{
    if (! m_Active ) return;

    m_Active = false;
    vdetach (m_PhysicalDevNo, TaskId);
};

/*---------------------------------------------------------------------------*/

bool
Trace_DevSpace::Open( tsp00_TaskId TaskId )
{
    if (m_Active ) return( true );

    tsp00_Bool      bOkay = TRUE;
    tsp00_ErrText   ErrText;

    vattach( m_LogicalDevNo, tsp2_dev_type::fromConst( sp2dt_trace ),
             m_PhysicalDevNo, TaskId, ErrText, bOkay );

    m_Active = ( TRUE == bOkay );
    return( m_Active );
};

/*---------------------------------------------------------------------------*/

bool
Trace_DevSpace::Write(
    tsp00_TaskId      TaskId,
    SAPDB_Int4        UKTId,
    tsp00_PageAddr    pPageList,
    SAPDB_Int4        NumPages)
{
    if (! m_Active )
        return( false );

    tsp00_Bool      bOkay = TRUE;
    tsp00_ErrText   ErrText;
    SAPDB_Int4      pagesWritten = 0;  // PTS 1110822 TS 2001-07-19
    SAPDB_Int4      blockCount   = 0;  // PTS 1110822 TS 2001-07-19
    tsp00_PageNo    DevSpacePno  = Trace_InfoPage( m_InfoPage ).GetUktPNO( UKTId - 1 ); // PTS 1108976 JM

    while(( pagesWritten < NumPages) && ( TRUE == bOkay ))  // PTS 1110822 TS 2001-07-19
    {
        blockCount = (( pagesWritten + m_BlockCount ) < NumPages ) ? m_BlockCount : NumPages - pagesWritten;

        vblockio (m_PhysicalDevNo, DevSpacePno + pagesWritten,
                  tsp2_io_op_code::fromConst( sp2ioc_write ),
                  pPageList + pagesWritten, blockCount, TaskId, ErrText, bOkay);

        pagesWritten += blockCount;
    }
    if ( TRUE == bOkay )
        return( true );

    g01optextmsg (sp3p_console, sp3m_error, csp3_io_error, csp3_n_io, ErrText);

    return( false );
};

/*---------------------------------------------------------------------------*/

bool
Trace_DevSpace::WriteInfoPage( tsp00_TaskId TaskId )
{
    if (! m_Active ) return( false );

    tsp00_Bool      bOkay = TRUE;
    tsp00_ErrText   ErrText;

    vblockio (m_PhysicalDevNo, FIRST_DEVSPACE_PNO,
              tsp2_io_op_code::fromConst( sp2ioc_write ), m_InfoPage, 1, TaskId,
              ErrText, bOkay);

    if ( TRUE == bOkay ) return( true );

    g01optextmsg (sp3p_console, sp3m_error, csp3_io_error, csp3_n_io, ErrText);

    return( false );
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
