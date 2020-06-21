/*!**************************************************************************

  module      : Data_PrimPage.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:  This class is used to handle primary data pages.
  description :  

  last changed: 2001-01-20
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


#ifndef DATA_PRIM_PAGE_HPP
#define DATA_PRIM_PAGE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_Page.hpp"
#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!*****************************************************************************

   class: Data_PrimPage

   description: This class is used to handle primary data pages.
              
*******************************************************************************/

class Data_PrimPage : public Data_Page
{
public:

    /*!-------------------------------------------------------------------------
    function:     GetRecordCount()
    description:  This method returns the number of records stored on the page
    arguments:    none
    return value: Data_RecordCount
    --------------------------------------------------------------------------*/

    Data_RecordCount GetRecordCount() const
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );

        return( static_cast< Data_RecordCount >( NodePtr()->nd_record_cnt() ));
    }

    /*!-------------------------------------------------------------------------
    function:     GetCovering()
    description:  This method returns the covering of the page
    arguments:    none
    return value: Data_RecordLength
    --------------------------------------------------------------------------*/

    Data_RecordLength GetCovering () const
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );

        return( Data_Page::SpaceUsed() + GetRecordCount() * POINTERSIZE_BD00 );
    }

    /*!-------------------------------------------------------------------------
    function:     GetRecord()
    description:  This method returns a pointer to the record specified by
                  the given record index.
    arguments:    recordIndex [in]
    return value: tgg00_Rec*
    --------------------------------------------------------------------------*/

    tgg00_Rec* GetRecord( const Data_RecordIndex recordIndex ) 
    {
        SAPDBERR_ASSERT_STATE( recordIndex < GetRecordCount());

        const Data_PageOffset recordPosition =
            NodePtr()->nd_pointer_list()[ MAX_POINTERINDEX_BD00 - recordIndex - POS_OFF_DIFF_BD00 ];

        return( reinterpret_cast< tgg00_Rec* >( FramePtr() + recordPosition - POS_OFF_DIFF_BD00 ));
    }

    const tgg00_Rec* GetRecord( const Data_RecordIndex recordIndex ) const
    {
        SAPDBERR_ASSERT_STATE( recordIndex < GetRecordCount());

        const Data_PageOffset recordPosition =
            NodePtr()->nd_pointer_list()[ MAX_POINTERINDEX_BD00 - recordIndex - POS_OFF_DIFF_BD00 ];

        return( reinterpret_cast< const tgg00_Rec* >( FramePtr() + recordPosition - POS_OFF_DIFF_BD00 ));
    }

    /*!-------------------------------------------------------------------------
    function:     GetLastRecord()
    description:  This method returns a pointer to the record which contains
                  the biggest key. It is assumed that at least one record 
                  exist.
    arguments:    none
    return value: tgg00_Rec*
    --------------------------------------------------------------------------*/

    tgg00_Rec* GetLastRecord() 
    {
        SAPDBERR_ASSERT_STATE( 0 != GetRecordCount());

        return( GetRecord( GetRecordCount() -1 ));
    }

    const tgg00_Rec* GetLastRecord() const
    {
        SAPDBERR_ASSERT_STATE( 0 != GetRecordCount());

        return( GetRecord( GetRecordCount() -1 ));
    }

    /*!-------------------------------------------------------------------------
    function:     GetFirstRecord()
    description:  This method returns a pointer to the record which contains
                  the smallest key. It is assumed that at least one record 
                  exist.
    arguments:    none
    return value: tgg00_Rec*
    --------------------------------------------------------------------------*/

    tgg00_Rec* GetFirstRecord() 
    {
        SAPDBERR_ASSERT_STATE( 0 != GetRecordCount());

        return( GetRecord( FIRST_REC_INDEX_BD00 ));
    }

    const tgg00_Rec* GetFirstRecord() const
    {
        SAPDBERR_ASSERT_STATE( 0 != GetRecordCount());

        return( GetRecord( FIRST_REC_INDEX_BD00 ));
    }

};

/*!*****************************************************************************

   endclass: Data_PrimPage

*******************************************************************************/


#endif  /* DATA_PRIM_PAGE_HPP */

