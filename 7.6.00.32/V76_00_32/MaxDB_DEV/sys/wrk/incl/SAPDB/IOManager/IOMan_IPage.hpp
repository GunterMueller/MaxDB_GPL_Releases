/*!
  @file           IOMan_IPage.hpp
  @author         UweH
  @ingroup        PageHandling
  @brief          page handler interface for page io

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
#ifndef IOMAN_IPAGE_HPP
#define IOMAN_IPAGE_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
/*!
@interface      IOMan_IPage
@brief          Interface class for IO Manager
*/
class IOMan_IPage
{
public:
    /*!
    @brief  Prepares page for input from device
    @return (SAPDB_Bool) true if check successful

    Performs parity check on the frame contents. 
    Recalculates parity from frame contents and compares the result
    with parity bytes kept in page header and trailer
    Applicable immediately after a page is read from device.
    */
    virtual SAPDB_Bool CheckAfterRead() const = 0;
    /*!
    @brief          Prepares page for output to device
    @return         (SAPDB_Bool) true if check successful

     Reads and check parity information of frame contents
     Updates parity information with actual frame contents
     Applicable immediately before a page is written to device
     */
    virtual SAPDB_Bool PrepareForWrite() = 0;
	/// Returns the pointer to the proper page frame
    virtual SAPDB_Byte* GetFramePtr() = 0;
    /// Returns the size in bytes of the page frame
    virtual SAPDB_UInt GetSize()const = 0;
    /// returns true if the page frame is assigned
    virtual SAPDB_Bool IsAssigned() const = 0;
    
};
#endif // IOMAN_IPAGE_HPP
