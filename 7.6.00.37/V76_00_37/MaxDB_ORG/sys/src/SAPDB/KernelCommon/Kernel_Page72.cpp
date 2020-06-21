/*!
  @file           Kernel_Page72.cpp
  @author         Till Luhmann, TorstenS
  @ingroup        Pages
  @brief          page handler base class

    The class kernel_page72 is used to encapsulate the old pages defined with pascal strucures.

\if EMIT_LICENCE
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

\endif
*/
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "KernelCommon/Kernel_Page72.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "KernelCommon/Kernel_Messages.hpp"  
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#include "hgg17.h"

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
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


bool
Kernel_Page72::CheckHeaderTrailer( SAPDBErr_MessageList &msgList ) const
{
    if( ! memcmp( Kernel_Page72::PersistentHeaderPtr(),
                  Kernel_Page72::PersistentTrailerPtr(),
                  Kernel_Page72::GetHeaderSize())
      )
    {
        return( true );
    }

    char pageType [ 65 ];
    char pageType2[ 65 ];
    char pageCheck[ 65 ];
    char pageMode [ 65 ];

    {
        // WRITE TRAILER INTO MESSAGELIST -> bottom element/last in the message list

        int  typeLength = 0, type2Length = 0, checkLength =0, modeLength = 0;

        // Very UGLY code, because of Kernel_Page::PageType and tgg00_PageType mix!
        if( Kernel_DataPageType == Kernel_Page::ReadPageType())
            g17pagetype_to_line( tgg00_PageType::fromConst( ptData_egg00 ), typeLength, pageType );
        else if( Kernel_SysPageType == Kernel_Page::ReadPageType())
            g17pagetype_to_line( tgg00_PageType::fromConst( ptSys_egg00 ), typeLength, pageType );
        else // page type is corupted
            g17pagetype_to_line( tgg00_PageType::fromConst( ptNil_egg00 ), typeLength, pageType );

        g17pagetype2_to_line( ReadTrailerPageType2(), type2Length, pageType2 );
        g17pagecheck_to_line( static_cast<tgg00_PageCheckType_Enum> (GetTrailerPageCheck()),
                              checkLength, pageCheck );
        tgg00_PageMode myMode = GetTrailerPageMode();
        g17pagemode_to_line( myMode, false, modeLength, pageMode );

        Kernel_Exception errMsg3( __CONTEXT__, KERNEL_ERR_TRAILER_INFO,
                                  SAPDB_ToString( GetTrailerPageID(), _T_d ),
                                  SAPDB_ToString( pageType, typeLength, typeLength ),
                                  SAPDB_ToString( pageType2, type2Length, type2Length ),
                                  SAPDB_ToString( pageCheck, checkLength, checkLength ),
                                  SAPDB_ToString( pageMode, modeLength, modeLength ));
        msgList = errMsg3;
    }
    {
        // WRITE HEADER INTO MESSAGELIST

        int  typeLength = 0, type2Length = 0, checkLength = 0, modeLength = 0;

        // Very UGLY code, because of Kernel_Page::PageType and tgg00_PageType mix!

        if( Kernel_DataPageType == Kernel_Page::ReadPageType())
            g17pagetype_to_line( tgg00_PageType::fromConst( ptData_egg00 ), typeLength, pageType );
        else if( Kernel_SysPageType == Kernel_Page::ReadPageType())
            g17pagetype_to_line( tgg00_PageType::fromConst( ptSys_egg00 ), typeLength, pageType );
        else // page type is corupted
            g17pagetype_to_line( tgg00_PageType::fromConst( ptNil_egg00 ), typeLength, pageType );

        g17pagetype2_to_line( ReadPageType2(), type2Length, pageType2 );
        g17pagecheck_to_line( static_cast<tgg00_PageCheckType_Enum> (GetPageCheck()),
                              checkLength, pageCheck );
        tgg00_PageMode myMode = GetPageMode();
        g17pagemode_to_line( myMode, false, modeLength, pageMode );

        Kernel_Exception errMsg2( __CONTEXT__, KERNEL_ERR_HEADER_INFO,
                                  SAPDB_ToString( GetPageID(), _T_d ),
                                  SAPDB_ToString( pageType, typeLength, typeLength ),
                                  SAPDB_ToString( pageType2, type2Length, type2Length ),
                                  SAPDB_ToString( pageCheck, checkLength, checkLength ),
                                  SAPDB_ToString( pageMode, modeLength, modeLength ));

        msgList.AppendNewMessage( errMsg2 );
    }

    // write top element into message list
    Kernel_Exception errMsg1( __CONTEXT__, KERNEL_ERR_HEADER_TRAILER_MISMATCH );

    msgList.AppendNewMessage( errMsg1 );

    return( false );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
