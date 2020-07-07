/*!
  @file           SAPDBMem_Exceptions.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Declaration of the exception classes for component SAPDBMem

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



#ifndef SAPDBMem_Exceptions_HPP
#define SAPDBMem_Exceptions_HPP

#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

// Constants for error messages, generated from "SAPDBMem_Messages.msg"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Messages.hpp"     /* nocheck */

SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS (SAPDBMem); //!< define component exception class

/*!
   @brief defines an exception class for the component SAPDBMem

*/

class SAPDBMem_BadAllocException : public SAPDBMem_Exception
{
public:
    /*!
    @brief default contructor 
    */
    SAPDBMem_BadAllocException() {}
    /*!
    @brief construtor
    */
    SAPDBMem_BadAllocException(const SAPDB_Char* Filename, SAPDB_UInt4 LineNumber, int Size)
        : SAPDBMem_Exception(Filename, LineNumber, SAPDBMEM_ERR_BAD_ALLOC, SAPDB_ToString(Size)) {}
    /*!
    @brief destructor
    */
    virtual ~SAPDBMem_BadAllocException() {}
};

/*!
@brief thrown, if wrong pointer is to be deallocatted
*/

class SAPDBMem_BadDeallocException : public SAPDBMem_Exception
{
public:
    /*!
    @brief constructor
    */
    SAPDBMem_BadDeallocException() {}
    /*!
    @brief constructor
    */
    SAPDBMem_BadDeallocException(const SAPDB_Char* Filename, SAPDB_UInt4 LineNumber)
        : SAPDBMem_Exception(Filename, LineNumber, SAPDBMEM_ERR_BAD_ALLOC) {}
    /*!
    @brief destructor
    */
    virtual ~SAPDBMem_BadDeallocException() {}
};

/*!
@brief thrown, if allocator is inconsistent
*/

class SAPDBMem_ConsistencyException : public SAPDBMem_Exception
{
public:
    /*!
    @brief constructor
    */
    SAPDBMem_ConsistencyException() {}
    /*!
    @brief constructor
    */
    SAPDBMem_ConsistencyException (const SAPDB_Char* Filename,
                                   SAPDB_UInt4 LineNumber,
                                   const SAPDB_Char* Message)
        : SAPDBMem_Exception(Filename, LineNumber, SAPDBMEM_ERR_CONSISTENCY, Message) {}
    /*!
    @brief destructor
    */
    virtual ~SAPDBMem_ConsistencyException () {}
};


#endif // SAPDBMem_Exceptions_HPP

