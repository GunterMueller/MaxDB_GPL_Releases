/*!
  @file           Converter_ArchiveContainer.hpp
  @author         Henrik
  @ingroup        Converter
  @brief          Container to store the converter entries of the archive volumes for save data
*/

/*
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
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


#ifndef CONVERTER_ARCHIVECONTAINER_HPP
#define CONVERTER_ARCHIVECONTAINER_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "DataAccess/Data_Types.hpp"
#include "Container/Container_Vector.hpp"
#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_LeafPage.hpp"
#include "Converter/Converter_PageFrameAllocator.hpp"


/*!
  @class          Converter_ArchiveContainer
  @brief          Container to store the converter entries of the archive volumes for save data
 */
class Converter_ArchiveContainer
{
private:

    /// data storage container
    typedef Container_Vector<Converter_LeafPage>          LeafPageVector;

public:
    /* -----------------------------------------------------------------------*/

    /*! @name Type definitions */

    /// size type
    typedef LeafPageVector::SizeType                      SizeType;

    /// iterator of converter leaf pages (steps from page to page)
    typedef LeafPageVector::Iterator                      Iterator;

    /// resource type used for dynamic memory allocation
    typedef LeafPageVector::Allocator                     Allocator;

private:

    /*--------------------------------------------------------------------------
    declaration : IndexType
    description:  container index type
    --------------------------------------------------------------------------*/
    typedef LeafPageVector::IndexType                     IndexType;

public:

    /*!
    @brief          Constructor
    @param          alloc [in] allocator for map resources
    @param          sizedelta [in] realloc size increment if container full
    @return         none
  */
    Converter_ArchiveContainer
        (Allocator&                     alloc,
         SizeType                       sizedelta)
        :
         m_PageVector(alloc),
         m_SizeDelta(sizedelta)
    {}

    /*!
       @brief          Deletes container structures; deallocates memory and frames
       @param          framealloc [in] frame allocator for leaf pages
       @return         none
     */
    void Delete
        (Converter_PageFrameAllocator&  framealloc)
    {
        for (IndexType i=0; i<m_PageVector.GetSize(); i++)
            framealloc.Free(m_PageVector[i].GetFrame(), false);

        m_PageVector.Delete();
    }

    /*!
    @brief          Appends data page numbers to the container
    @param          leafpage [in] converter leaf page
    @param          framealloc [in] frame allocator for leaf pages,
    @return         (SAPDB_Bool) true if successful

    - Appends all data page numbers marked as used on
      the converter leaf page to the container.
  */
    SAPDB_Bool Append
        (Converter_LeafPage&      leafpage,
         Converter_PageFrameAllocator&  framealloc);

    /*!
    @brief          Returns an iterator referencing the first leaf page in the
                    container
    @return         (Iterator) iterator referencing the first container page
  */
    Iterator Begin()
    {
        return m_PageVector.Begin();
    }

    /*!
    @brief          Returns an iterator refencing behind the last leaf page in
                    the container
    @return         (Iterator) iterator pointing behind the last container page
  */
    Iterator End()
    {
        return m_PageVector.End();
    }

    /*!
    @brief          Returns true if the container contains at least one page
    @return         (SAPDB_Bool)true if the container contains at least one page
  */
    SAPDB_Bool IsEmpty()
    {
        return m_PageVector.IsEmpty();
    }

    /*!
       @brief          Returns the size of the container
       @return         (SizeType)true size of the container
     */
    SizeType GetSize()
    {
        return m_PageVector.GetSize();
    }

    /*!
       @brief          Returns an iterator to the &lt;n>-th converter  bit map page.
       @return         (Iterator) iterator pointing behind the last container page
     */
    Iterator Position(LeafPageVector::IndexType n)
    {
        return m_PageVector.Position (n);
    }

    /*!
       @brief          Changes the number of vector elements.
       @param          NewSize [in] The new number of vector elements.
       @returns        (SAPDB_Bool) true if successful

                    If the capacity of the vector is not large enough, the
                    memory for the vector elements is reallocated.
                    ATTENTION!! After reallocation all iterators may be invalid!
     */
    SAPDB_Bool Resize(SizeType NewSize)
    {
        return m_PageVector.Resize(NewSize);
    }

private:

    /*--------------------------------------------------------------------------
    declaration : m_PageVector
    description:  vector of leaf page handlers
    --------------------------------------------------------------------------*/
    LeafPageVector                    m_PageVector;
    /*--------------------------------------------------------------------------
    declaration : m_SizeDelta
    description:  vector realloc size delta
    --------------------------------------------------------------------------*/
    SizeType                            m_SizeDelta;
};

#endif  /* CONVERTER_ARCHIVECONTAINER_HPP */
