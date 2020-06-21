/****************************************************************************

  module      : Kernel_RestartPage.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Restart Page
  description : This module is used to encapsulate the restart record 
                pascal implementaion for the I/O management.

                This class offers all methods needed by the restart page
                io management.

  last changed: 2001-02-27  12:11
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "hgg01_3.h"

#include "KernelCommon/Kernel_Page.hpp"
#include "KernelCommon/Kernel_RestartPage.hpp"
#include "RunTime/RTE_Message.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define RESTART_BODY_BEGIN          13
#define PAGE_STEP                   128
#define RESTART_BODY_CHECK_AREA     512
#define RESTART_BODY_STEP           32

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SAPDB_Bool
Kernel_RestartPage::CheckAfterRead() const
{
    SAPDBERR_ASSERT_STATE( IsAssigned() );

    if( Kernel_DataPageType != Kernel_Page::ReadPageType() )
    {
        if( Kernel_SysPageType != Kernel_Page::ReadPageType() ) // in case of migration to 7.4
            return( SAPDB_FALSE );
    }

    if( pt2Checkpt_egg00 != Kernel_Page72::ReadPageType2() )
        return( SAPDB_FALSE );

    SAPDBErr_MessageList    msgList;

    if( ! Kernel_Page72::CheckHeaderTrailer( msgList ))
    {
        RTE_Message( msgList );
        return( SAPDB_FALSE );
    }

    if( chckNil_egg00 == GetPageCheck() || chckWriteCnt_egg00 == GetPageCheck()
      )
        return( SAPDB_TRUE );

    // Calculate checksum and check against checksum value on given page
    if( Kernel_Page72::GetPageChecksum() == CalculateChecksum() )
        return( SAPDB_TRUE );

    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Kernel_RestartPage::PrepareForWrite()
{
    SAPDBERR_ASSERT_STATE( IsAssigned() );

    // Header - trailer check, to guarantee that during access of
    // the restart record a write operation has not destroyed the
    // trailer and the following memory!

    SAPDBErr_MessageList    msgList;

    if( ! Kernel_Page72::CheckHeaderTrailer( msgList ))
    {
        RTE_Message( msgList );
        return( SAPDB_FALSE );
    }

    // Calculate checksum
    Kernel_Page72::SetPageCheck( chckChecksumLogInfo_egg00 );
    Kernel_Page72::CopyHeaderToTrailer();
    Kernel_Page72::SetPageChecksum( CalculateChecksum() );

    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Kernel_Page72::PageChecksum
Kernel_RestartPage::CalculateChecksum() const
{
    PageChecksum    Checksum   = 0;
    SAPDB_UInt4     CurrentPos = 1;
    SAPDB_Int       PrimeIndex = 6;

    // Calculate checksum for the page number
    for( CurrentPos = 1; CurrentPos < 5 ; ++CurrentPos )
    {
        Checksum += m_Frame.GetByteAt( CurrentPos-1 ) * g01hashprime[ CurrentPos-1 ];
    }
    // Calculate checksum for page type two
    Checksum += ( Kernel_Page72::ReadPageType2() * g01hashprime[ 4 ]);

    // Calculate checksum for body part
    // Check bytes: 13+14, 13+16, 45+46, 45+58 and so on
    CurrentPos = RESTART_BODY_BEGIN;
    while
    (
        ( CurrentPos < RESTART_BODY_CHECK_AREA ) &&
        ( PrimeIndex < MAX_HASH_PRIME_GG00 )
    )
    {
        Checksum += (m_Frame.GetByteAt( CurrentPos-1 ) * g01hashprime[ PrimeIndex-1 ]);
        PrimeIndex++;
        Checksum += (m_Frame.GetByteAt(( CurrentPos-1 )+3) * g01hashprime[ PrimeIndex-1 ]);
        PrimeIndex++;
        CurrentPos += RESTART_BODY_STEP;
    }

    for( CurrentPos = RESTART_BODY_BEGIN;
            (CurrentPos <= sizeof( tgg00_LogBody )) && ( PrimeIndex <= MAX_HASH_PRIME_GG00 );
            CurrentPos += PAGE_STEP, ++PrimeIndex )
    {
        Checksum += (m_Frame.GetByteAt( CurrentPos-1 ) * g01hashprime[ PrimeIndex-1 ]);

    }
    return( Checksum );
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
