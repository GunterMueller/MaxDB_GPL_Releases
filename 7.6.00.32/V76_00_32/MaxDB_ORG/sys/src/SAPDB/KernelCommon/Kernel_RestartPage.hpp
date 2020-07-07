/*!**************************************************************************

  module      : Kernel_RestartPage.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Restart Page
  description : This module is used to encapsulate the restart record 
                pascal implementaion for the I/O management.

                This class offers all methods needed by the restart page
                io management.

  last changed: 2001-02-22  18:22
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


#ifndef KERNEL_RESTARTPAGE_HPP
#define KERNEL_RESTARTPAGE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gkb00.h"

#include "KernelCommon/Kernel_Page72.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"


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

   class: Kernel_RestartPage

   description: This class is the encapsulate the restart record 
                pascal implementaion for the I/O management.
   
              
*******************************************************************************/

class Kernel_RestartPage : public Kernel_Page72 
{
public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!-------------------------------------------------------------------------
    function:     Kernel_RestartPage()
    description:  This is the default constructor of a restart page.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    Kernel_RestartPage(){}

    /*!-------------------------------------------------------------------------
    function:     Kernel_RestartPage()
    description:  Construct a restart page with a given page image.
    arguments:    Frame [in] restart page
    return value: none
    --------------------------------------------------------------------------*/

    Kernel_RestartPage( const PageFrame &Frame ):Kernel_Page72( Frame ){}
    
    /*!-------------------------------------------------------------------------
    function:     Kernel_RestartPage()
    description:  This method is used to assign a restart page a given page image.
    arguments:    pRestartPage [in] restart page image
    return value: none
    --------------------------------------------------------------------------*/

    Kernel_RestartPage( tkb00_PagePtr  pRestartPage )
    {
        m_Frame.Assign( pRestartPage, sizeof( *pRestartPage ));
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Restart Page I/O management access                           */
    /* -----------------------------------------------------------------------*/

    /*!-------------------------------------------------------------------------
    function:     CheckAfterRead()
    description:  Prepares page for input from device
                  - Performs parity check on the frame contents
                  - Recalculates parity from frame contents and compares
                    the result with parity bytes kept in page header and trailer
                  - Applicable immediately after a page is read from device
    arguments:    none
    return value: true if check successful
    --------------------------------------------------------------------------*/
    
    virtual SAPDB_Bool CheckAfterRead() const;
   
    /*!-------------------------------------------------------------------------
    function:     PrepareForWrite()
    description:  Prepares page for output to device
                  - Reads and check parity information of frame contents
                  - Updates parity information with actual frame contents
                  - Applicable immediately before a page is written to device
    arguments:    none
    return:       true if check successful
    --------------------------------------------------------------------------*/
    
    virtual SAPDB_Bool PrepareForWrite();
 
    /*!-------------------------------------------------------------------------
    function:     Verify()
    description:  Verifies consistency of page contents
    arguments:    none
    return:       true if verify successful
    --------------------------------------------------------------------------*/
    
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const
    {
        // dummy TODO
        return true;
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Restart Page I/O management access                        */
    /* -----------------------------------------------------------------------*/

private:

    PageChecksum  CalculateChecksum() const;
};

/*!*****************************************************************************

   endclass: Kernel_RestartPage

*******************************************************************************/

#endif  /* KERNEL_RESTARTPAGE_HPP */
