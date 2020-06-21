/****************************************************************************

  module      : Converer_Page72.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: 7.2/7.3 converter page
  description : Page handler for old converter pages (< 7.4) needed for migration


  last changed: 2002-08-01
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2004 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2004 SAP AG

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
#include "hgg01_3.h"

#include "Converter/Converter_Page72.hpp"
#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_Messages.hpp"
#include "Converter/Converter_Exception.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"


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

SAPDB_Bool
Converter_Page72::CheckAfterRead() const
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Page72::CheckAfterRead", Converter_Trace, 6 );

    // Verify page type, header vs. trailer and if necessary the write count
    // against the checksum value.
    if(! CheckPageForIO() )
        return( SAPDB_FALSE );

    // Calculate checksum and check against checksum value on given page
    if(
        ( chckChecksumData_egg00 == GetPageCheck() )
        &&
        ( Kernel_Page72::GetPageChecksum() != CalculateChecksum() )
    )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_ERR_CHECKSUM_MISMATCH,
                                    SAPDB_ToString( CalculateChecksum(), _T_d ),
                                    SAPDB_ToString( Kernel_Page72::GetPageChecksum(), _T_d ));

        RTE_Message( errMsg );

        return( SAPDB_FALSE );
    }
    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Page72::CheckPageForIO() const
{
    SAPDBERR_ASSERT_STATE( IsAssigned() );

    if(
        ( Kernel_SysPageType != Kernel_Page::ReadPageType() )
        ||
        ( pt2Converter_egg00 != Kernel_Page72::ReadPageType2() )
    )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_ERR_BAD_PAGE_TYPE,
                                    SAPDB_ToString( Kernel_Page::ReadPageType(), _T_d ),
                                    SAPDB_ToString( Kernel_Page72::ReadPageType2(), _T_d ));

        RTE_Message( errMsg );

        return( SAPDB_FALSE );
    }

    SAPDBErr_MessageList    msgList;

    if( ! Kernel_Page72::CheckHeaderTrailer( msgList ))
    {
        RTE_Message( msgList );
        return( SAPDB_FALSE );
    }

    if(
        ( chckWriteCnt_egg00 == GetPageCheck() )
        &&
        ( GetWriteCount() != Kernel_Page72::GetPageChecksum() )
    )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_ERR_WRITE_COUNT_MISMATCH,
                                    SAPDB_ToString( GetWriteCount(), _T_d ),
                                    SAPDB_ToString( Kernel_Page72::GetPageChecksum(), _T_d ));
        RTE_Message( errMsg );

        return( SAPDB_FALSE );
    }
    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

Kernel_Page72::PageChecksum
Converter_Page72::CalculateChecksum() const
{
    const SAPDB_UInt step          = 128;
    const SAPDB_UInt stopPosition  = m_Frame.GetLength();
    PageChecksum     checkSum      = 0;
    SAPDB_UInt       primeIndex    = 1;
    SAPDB_UInt4      startPosition = 1;

    while(( startPosition < stopPosition ) && ( primeIndex <= MAX_HASH_PRIME_GG00 ))
    {
        checkSum += m_Frame.GetByteAt( startPosition-1 ) * g01hashprime[ primeIndex -1 ];

        startPosition += step;
        primeIndex++;
    }
    return( checkSum );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Converter_Page72EntryIterator
Converter_Page72::Begin()
{
    return( Converter_Page72EntryIterator( *this, 0 ));
}

/*---------------------------------------------------------------------------*/

Converter_Page72EntryIterator
Converter_Page72::End()
{
    return( Converter_Page72EntryIterator( *this, GetEntryCount()));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Page72EntryIterator::IsPermanentUsed() const
{
    SAPDBERR_ASSERT_STATE( m_pPage->IsAssigned() );

    if( 0 == GetBlockAddress().GetDeviceNo() ) // NIL_DEVNO_BD01 from 7.2
        return( false );

    // each flagField is 32 bits big.
    // each flagField contains the state information of 10 data pages
    // the state information is 3 bits big
    // the 3 bits represents the info for permanent,markForSavePages and willBeSaved

    const SAPDB_Int  flagFieldContent = 10;
    const SAPDB_Int4 *pFlags          = m_pPage->GetPointerToConverterFlagField();
    const SAPDB_Int4 *pFlagField      = pFlags + ( m_EntryNo / flagFieldContent );
    const SAPDB_Int2 flagPos          = (m_EntryNo % flagFieldContent ) * 3;

    const SAPDB_Int4 myFlag =
        ((( *pFlagField & ( 0x00000007UL << flagPos ))) >> flagPos ) & 0x00000007UL;

    const SAPDB_UInt1 bIsPermanent = (( SAPDB_UInt1 )( myFlag & 0x04)/4 );

    if( 0 == bIsPermanent )
        return( false );  // temporary data page
    else
        return( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
