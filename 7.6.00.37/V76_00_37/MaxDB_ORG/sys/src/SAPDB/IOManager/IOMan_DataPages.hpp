/*!
  @file           IOMan_DataPages.hpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This module is used to define a variant of the template 
                  class IOMan_KernelPage suitable for data pages.
  @see            

\if EMIT_LICENCE
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

\endif
*/



#ifndef IOMAN_DATAPAGES_HPP
#define IOMAN_DATAPAGES_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_PageNoList.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "IOManager/IOMan_KernelPages.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
    @brief  Defines abbreviation for data page variant of a common
            page collection template
 */
typedef IOMan_Pages<Data_BasePage>          IOMan_DataPages;

/*!
    @brief  Defines abbreviation for data page iterator variant of a 
            common page collection template iterator.
 */
typedef IOMan_PagesIterator<Data_BasePage>  IOMan_DataPagesIterator;

/* -----------------------------------------------------------------------*/

class IOMan_ClusteredDataPages : public IOMan_DataPages
{
public:

    IOMan_ClusteredDataPages(   
        SAPDBMem_IRawAllocator&   allocator,
        const SAPDB_UInt          bufferSize,
        tbd_node* const           pBuffer );
    
    IOMan_ClusteredDataPages(
        SAPDBMem_IRawAllocator&   allocator,
        Data_PageFrame&           frames );
    
    ~IOMan_ClusteredDataPages(){
        this->Delete();
    }

    bool Initialize( const SAPDB_Int2   maxPages )
    {
        if( this->Init( maxPages )){
            return true;
        }
        this->Delete();
        return false;
    }

    void AppendPage(
        const Data_PageNo   pageNo,
        Data_BasePage&      page,
        const bool          bDoReadPage );

    void CopyFrames();

private:

    bool Init( const SAPDB_Int2   maxPages )
    {
        if( ! IOMan_DataPages::Initialize( maxPages )){
            return false;
        }
        if(  ! m_PageNoList.Reserve( maxPages )){
            return false;
        }
        if( ! m_NodePointerList.Reserve( maxPages )){
            return false;
        }
        return true;
    }

    void Delete()
    {
        IOMan_DataPages::Delete();
        m_PageNoList.Delete();
        m_NodePointerList.Delete();
    }

private:

    // Assignment operator is not supported
    IOMan_ClusteredDataPages& operator=( const IOMan_ClusteredDataPages& );

    // Copy constructor is not supported
    IOMan_ClusteredDataPages( const IOMan_ClusteredDataPages& );

private:

    // size in bytes of a data page
    const SAPDB_UInt  m_BlockSize;

    // size in pages of the contiguous memory chunk
    const SAPDB_UInt  m_BufferSize;

    // Anchor of multiple contiguous memory chunk used to perform one read io
    tbd_node* const m_pBuffer;

    // pageNo's of the data pages to be read
    Data_PageNoList  m_PageNoList;

    // contains the original node pointers of the pages which will be read
    Container_Vector< tbd_node* >  m_NodePointerList;

};

#endif  /* IOMAN_DATAPAGES_HPP */
