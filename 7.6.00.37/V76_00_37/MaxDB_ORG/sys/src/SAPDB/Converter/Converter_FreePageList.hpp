/*!
  @file     Converter_FreePageList.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Simple container of data pageNo's and associated converter page versions
*/

/*
    ========== licence begin  GPL
    Copyright (c) 2000-2004 SAP AG

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
*/


#ifndef CONVERTER_FREEPAGELIST_HPP
#define CONVERTER_FREEPAGELIST_HPP

#include "Container/Container_Vector.hpp"
#include "Converter/Converter_Version.hpp"
#include "DataAccess/Data_Types.hpp"

/*!
   @class   Converter_FreePageItem
   @brief   Simple container of a data pageNo and the associated page Version
 */

class Converter_FreePageItem
{
public:

    /*!
      @brief   Constructor
      @param   pageNo      [in] page number to be freed
      @param   pageVersion [in] converter page version of the given page no
      @return  none
    */

    Converter_FreePageItem(
        const Data_PageNo       &pageNo,
        const Converter_Version &pageVersion )
    :
    m_PageNo( pageNo ),
    m_PageVersion( pageVersion )
    {}

    /*!
       @brief   Copy constructor
       @param   freePageItem [in] free page item to be copied
       @return  none
    */

    Converter_FreePageItem( const Converter_FreePageItem &freePageItem )
    :
    m_PageNo( freePageItem.GetPageNo() ),
    m_PageVersion( freePageItem.GetPageVersion() )
    {}

    /*!
       @brief   This method returns the data page number
       @return  Data_PageNo
    */

    Data_PageNo GetPageNo() const
    {
        return( m_PageNo );
    }

    /*!
       @brief   This method returns the converter version of the associated data page
       @return  Converter_Version
    */

    Converter_Version GetPageVersion() const
    {
        return( m_PageVersion );
    }

private:

    /// assignment operator not needed
    Converter_FreePageItem& operator=( const Converter_FreePageItem &freePageItem );

private:

    /// data page number
    Data_PageNo m_PageNo;

    /// current converter version of the data page identified by m_PageNo
    Converter_Version   m_PageVersion;
};

/*!
   @class   Converter_FreePageList
   @brief   Simple container for a couple of Converter_FreePageItem's
 */

class Converter_FreePageList : private Container_Vector<Converter_FreePageItem>
{
private:

    /// my type of Container_Vector
    typedef Container_Vector<Converter_FreePageItem> FreePageList;

public:

    /// container size in pagenos
    typedef FreePageList::SizeType                          SizeType;

    /// resource type used for dynamic memory allocation
    typedef FreePageList::Allocator                         Allocator;

    /// r/only iterator of all pagenos in the container
    typedef FreePageList::ConstIterator                     ConstIterator;

    /// of all pagenos in the container
    typedef FreePageList::Iterator                          Iterator;

    /*!
       @brief   Constructor
       @param   allocator [in] allocator for container resources
       @return  none
     */

    Converter_FreePageList( Allocator   &allocator )
    :
    FreePageList( allocator ),
    m_MinimumSize( 0 )
    {}

    /*!
       @brief   Initializes container structures; allocates memory
       @param   entryCount [in] container size
       @return  (SAPDB_Bool) true if initialization was successfully
     */

    SAPDB_Bool Initialize( const SizeType entryCount )
    {
        SAPDBERR_ASSERT_STATE( GetSize() == 0 );

        m_MinimumSize = entryCount;

        return( Reserve( entryCount ));
    }

    /*!
       @brief   Appends pageno to container
       @param   pageNo      [in] pageNo to be appended
       @param   pageVersion [in] converter page version of the given pageNo
       @return  (SAPDB_Bool) true if pageNo and pageVersion was successfully appended

       - A given pageNo and the associated pageVersion is inserted
         at the end of the container.
       - The method fails if the container is full and could not
         be expanded
     */

    SAPDB_Bool Append(
        const Data_PageNo       &pageNo,
        const Converter_Version &pageVersion )
    {
        if( IsFull() )
        {
            const SizeType newCapacity = 1 + 2 * GetSize();

            if (! Reserve( newCapacity ))
                return( SAPDB_FALSE );
        }
        InsertEnd( Converter_FreePageItem( pageNo, pageVersion ));
        return( SAPDB_TRUE );
    }

    /*!
       @brief   Checks whether specified pageNo is in the container
       @param   pageNo [in] pageno to be found
       @return  (SAPDB_Bool) true if found
     */

    SAPDB_Bool IsContained( const Data_PageNo   &pageNo )
    {
        ConstIterator iter    = Begin();
        ConstIterator enditer = End();
        SAPDB_Bool bFound     = SAPDB_FALSE;

        while (iter != enditer && !bFound)
        {
            bFound = ( *iter ).GetPageNo() == pageNo;
            iter++;
        }
        return( bFound );
    }

    /*!
       @brief   Clears the content of the container. If a minimum
                size was defined the conatiner is re-allocated up
                to this size.
       @return  none
     */

    void Clear()
    {
        if( GetSize() > m_MinimumSize )
        {
            Delete();
            Reserve( m_MinimumSize );
        }
        else
        {
            FreePageList::Clear();
        }
    }

    /*!
    @brief  Returns an iterator refencing the first pageno in the
            container
    @return (Iterator) iterator pointing on the first pageno
    */

    Iterator Begin()
    {
        return FreePageList::Begin();
    }

    /*!
    @brief  Returns an iterator pointing behind the last pageno in
            the container
    @return (Iterator) iterator pointing behind last pageno
    */

    Iterator End()
    {
        return FreePageList::End();
    }

    /*!
       @brief   Returns the number of page numbers stored in the container
       @return  (SizeType)  number of page numbers stored in the container
     */

private:

    SizeType GetSize() const
    {
        return FreePageList::GetSize();
    }

private:

    /// mininum container size
    SizeType    m_MinimumSize;
};

#endif // CONVERTER_FREEPAGELIST_HPP


