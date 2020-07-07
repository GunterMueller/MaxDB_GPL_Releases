/*!
    @defgroup   DataAccess DataAccessManager related items
*/
/*!
  @file         Data_BasePage.cpp
  @author       TorstenS
  @ingroup      DataAccess
  @brief        This module is used to encapsulate the data page pascal
                implementaion for the I/O management without problems 
                with the cache properties of the derived class Data_Page
                
                This class offers all methods needed by the data page 
                I/O management.

  last changed: 2001-02-26  9:49
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

#include "DataAccess/Data_BasePage.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"

#include "KernelCommon/Kernel_Page.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "KernelCommon/Kernel_Messages.hpp"  

#include "RunTime/RTE_Message.hpp"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp" 


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define DATA_PAGE_STEP               128
#define DATA_POINTERLIST_STEP        4
#define DATA_CHECK_POINTERLIST       true

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
Data_BasePage::CheckAfterRead() const
{
    if(! CheckPageForIO() )
        return( SAPDB_FALSE );

    switch( GetPageCheck() )
    {
    case chckChecksumData_egg00 :
        {
            if( Kernel_Page72::GetPageChecksum() == CalculateChecksum() )
                return( SAPDB_TRUE );
            else
            {
                Data_Exception errMsg( __CONTEXT__, DATA_ERR_CHECKSUM_MISMATCH,
                                       SAPDB_ToString( CalculateChecksum(), _T_d ),
                                       SAPDB_ToString( Kernel_Page72::GetPageChecksum(), _T_d ));
                errMsg.AppendNewMessage( Data_Exception( __CONTEXT__, DATA_ERR_BAD_CHECKSUM ));
                RTE_Message( errMsg );
            }
            break;
        }
    case chckWriteCnt_egg00 :
        {
            if( Kernel_Page72::GetPageChecksum() == GetWriteCount() )
                return( SAPDB_TRUE );
            else
            {
                Data_Exception errMsg( __CONTEXT__, DATA_ERR_WRITE_CNT_MISMATCH,
                                       SAPDB_ToString( GetWriteCount(), _T_d ),
                                       SAPDB_ToString( Kernel_Page72::GetPageChecksum(), _T_d ));
                errMsg.AppendNewMessage( Data_Exception( __CONTEXT__, DATA_ERR_BAD_WRITE_CNT ));
                RTE_Message( errMsg );
            }
            break;
        }
    default:
        {
            RTE_Message( Data_Exception( __CONTEXT__, DATA_ERR_BAD_ALGO ));
        }
    }
    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_BasePage::PrepareForWrite()
{
    if(! CheckPageForIO() )
        return( SAPDB_FALSE );

    // Increment the write count to show how often the date page
    // was written in the past.
    IncrementAndSetWriteCount();

    if(
        ( chckChecksumData_egg00 != GetPageCheck()) ||
        ( GetTrailerPageCheck()  != GetPageCheck())
    )
    {
        // Set all old data pages with too soft check algorithm
        // to the best check technic (see Data_BasePage::CheckPageForIO)

        Kernel_Page72::SetPageCheck( chckChecksumData_egg00 );
        Kernel_Page72::CopyHeaderToTrailer();
    }

    Kernel_Page72::SetPageChecksum( CalculateChecksum() );

    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_BasePage::Verify(bool                  isAdmin,
                      SAPDBErr_MessageList &errlist) const
{
    if( pt2UndoLog_egg00 == Kernel_Page72::ReadPageType2() )
    {
        if(( BODY_BEG_BD00 > GetBottom()) || ( MAX_BOTTOM_BD00 < GetBottom()))
            return( SAPDB_FALSE );
    }
    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

Data_VolumeMode
Data_BasePage::VolumeMode() const
{
    if( ! IsAssigned() )
        return( Data_VolumeUnknown );
    else if( IsForArchive() )
        return( Data_VolumeSequential );
    else
        return( Data_VolumeNormal );
}

/*===========================================================================*
 *  PRIVATE METHODS                                                          *
 *===========================================================================*/

SAPDB_Bool
Data_BasePage::IsForArchive() const
{
    SAPDBERR_ASSERT_STATE( IsAssigned() );

    if( ! GetPageMode().includes( pmArchive_egg00 ))
        return( SAPDB_FALSE );

    return( GetRight().IsValid()  );
}

/*---------------------------------------------------------------------------*/

Kernel_Page72::PageChecksum
Data_BasePage::CalculateChecksum() const
{
    PageChecksum    Checksum = 0;
    SAPDB_UInt4     FirstPos = 1;

    switch( Kernel_Page72::ReadPageType2() )
    {
    case pt2Inv_egg00:
    case pt2InvSubtree_egg00:
    case pt2Tab_egg00:
        {
            // Calculate checksum for body part
            CalculateBodyChecksum( FirstPos, GetBottom() -1  , DATA_PAGE_STEP, Checksum );

            // Calculate checksum for pointerlist starting with the pointer
            // list item 'pointing' to the record with the greatest key.
            FirstPos = MAX_BOTTOM_BD00 - (GetRecordCount() << DIVISOR_POINTERSIZE_BD00 );

            CalculateBodyChecksum( FirstPos, BODY_END_BD00,
                                   DATA_POINTERLIST_STEP, Checksum, DATA_CHECK_POINTERLIST );
            break;
        }
    case pt2Object_egg00:
    case pt2ContObject_egg00:
    case pt2VarObject_egg00:
        {
            CalculateBodyChecksum( FirstPos, m_Frame.GetLength() -1, DATA_PAGE_STEP, Checksum );
            break;
        }
    case pt2Usm_egg00:
        {   // unfortunately usm pages could be re-stored in 7.4 by a data base migration
            // recovery. In this case no checksum test is made. The USM pages could be
            // removed by a cold verify!

            Checksum = Kernel_Page72::GetPageChecksum();
            break;
        }
    default:
        {
            CalculateBodyChecksum( FirstPos, GetBottom() -1 , DATA_PAGE_STEP, Checksum );
            break;
        }
    }
    return( Checksum );
}

/*---------------------------------------------------------------------------*/

void
Data_BasePage::CalculateBodyChecksum(
    SAPDB_UInt4     CurrentPos,
    SAPDB_UInt4     LastPos,
    SAPDB_UInt      Step,
    PageChecksum   &Checksum,
    SAPDB_Bool     bCheckPointerlist ) const
{
    SAPDB_Int   PrimeIndex = 1;
    SAPDB_Int4  Sum;
    SAPDB_Int4  Value;

    while(( CurrentPos < LastPos ) && ( PrimeIndex <= MAX_HASH_PRIME_GG00 ))
    {
        if( SAPDB_TRUE == bCheckPointerlist )
        {
            SAPDB_MemCopyNoCheck( &Value, m_Frame.GetPointer( CurrentPos-1), sizeof( Value ));
            Sum = Value * g01hashprime[ PrimeIndex-1 ];
        }
        else
        {
            Sum = m_Frame.GetByteAt( CurrentPos-1 ) * g01hashprime[ PrimeIndex-1 ];
        }
        CurrentPos += Step;
        Checksum   += Sum;
        PrimeIndex++;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_BasePage::CheckPageForIO() const
{
    SAPDBERR_ASSERT_STATE( IsAssigned() );

    if( (Kernel_DataPageType    != Kernel_Page::ReadPageType()) &&
            (Kernel_FileDirPageType != Kernel_Page::ReadPageType()) )
    {
        RTE_Message( Data_Exception( __CONTEXT__, DATA_ERR_BAD_PAGE_TYPE,
                      Kernel_Page::ReadPageTypeName()));
        return( SAPDB_FALSE );
    }

    SAPDBErr_MessageList    msgList;

    if( Kernel_Page72::CheckHeaderTrailer( msgList )){
        return( SAPDB_TRUE );
    }

    // If a data page is copied within the data cache it's possible that
    // this takes place to the same time as the page is written onto
    // data area. If the page is treated with the old header/trailer
    // check algorithmn, than the check algorithmn is changed to CheckSum
    // (see Data_BasePage::PrepareForWrite). If this header/trailer update
    // is made at the same time as the page is copied within the data cache,
    // it's possible that the header/trailer is inconsistent concerning
    // the CheckSum algorithmn.

    if(
        ( GetPageID() == GetTrailerPageID()                   ) &&
        ( Kernel_Page::ReadPageType() == ReadTrailerPageType()) &&
        ( ReadPageType2() == ReadTrailerPageType2()           ) &&
        ( GetPageMode() == GetTrailerPageMode()               )
    )
    {
        SAPDBERR_ASSERT_STATE( GetTrailerPageCheck() != GetPageCheck());
        msgList.Clear();
        return( SAPDB_TRUE );
    }

    RTE_Message( msgList );

    return( SAPDB_FALSE ); // not only the Check algorithmn is different!
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
