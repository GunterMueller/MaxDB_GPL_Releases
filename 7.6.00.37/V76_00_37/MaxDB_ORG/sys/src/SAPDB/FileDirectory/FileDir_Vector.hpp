/*****************************************************************************/
/*!

  @file         FileDir_Vector.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for template class FileDir_Vector.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#ifndef FILEDIR_VECTOR_HPP
#define FILEDIR_VECTOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Vector.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "FileDirectory/FileDir_Common.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*!
  @class          FileDir_Vector
  @ingroup        FileDirectory_Classes
  @author         MartinKi

  @brief          This class implements a vector class with auto
                  resize. If a memory allocation fails the kernel will
                  be shutdown.
  @copydoc        Container_Vector
*/
template <class T>
class FileDir_Vector : public Container_Vector<T>
{
private:
    /// do not allow copy constructor
    FileDir_Vector(const FileDir_Vector& v)
    {};

public:
    /*!
     * @brief Constructor.
     *
     * @param RawAlloc [in] allocator to be used for memory allocations
     */
    FileDir_Vector(SAPDBMem_IRawAllocator& RawAlloc)
        : Container_Vector<T>(RawAlloc)
    {}

    /************************ typedefs *********************************/
    /// Typedef for SizeType.
    typedef typename Container_Vector<T>::SizeType  SizeType;
    /// Typedef for Iterator.
    typedef typename Container_Vector<T>::Iterator  Iterator;
    /// Typedef for IndexType.
    typedef typename Container_Vector<T>::IndexType IndexType;


    /************************** methods ********************************/

    /*!
     * @brief Reserves enough memory to accommodate NewCapacity entries.
     *
     * @return
     *    - true on success
     *    - false on failure
     */
    SAPDB_Bool Reserve(SizeType NewCapacity);

    /*!
     * @brief Increases the vector's capacity by <em>at least</em>
     *        growBy elements.
     *
     * @param growBy [in] number of elements vector should at least be
     *                    extended by
     *
     * @return
     *    - true if resize was successful
     *    - false if resize failed
     */
    SAPDB_Bool Extend(const SizeType growBy);

    /*!
     * @brief Makes sure vector can accommodate at least an additional
     *        noOfElements elements.
     *
     * This method initiates a resize if neccessary.
     *
     * @param noOfElements [in] noOfElements vector should be able to
     *                          accommodate
     *
     * @return
     *    - true, if enough space is available
     *    - false, if vector cannot accommodate all elements
     */
    SAPDB_Bool AssertSpace(const SizeType noOfElements);

    /*!
     * @brief Makes sure vector can accommodate at least an additional
     *        noOfElements elements.
     *
     * This method initiates a resize if neccessary. The given
     * iterator will still be valid even if a resize was triggered.
     *
     * @param noOfElements [in] noOfElements vector should be able to
     *                          accommodate
     * @param iterator     [in/out] iterator of the vector; this
     *                          iterator will still be valid even
     *                          if a resize was necessary to assert
     *                          enough free space
     *
     * @return
     *    - true, if enough space is available
     *    - false, if vector cannot accommodate all elements
     */
    SAPDB_Bool AssertSpace(const SizeType noOfElements, Iterator& iterator);

    /*!
     * @brief Inserts an element at the position the iterator Pos is
     *        pointing to. Pos will stay valid even if a resize occurs
     *        during the insert operation. Other iterators most likely
     *        will not be working any more if a resize should occur.
     */
    void Insert(Iterator& Pos, const T& Elem);

    /*!
     * @brief Insert Elem at the end of the vector. Resize if necessary.
     *
     * @return true on success; on failure a shutdown will be initiated.
     */
    SAPDB_Bool InsertEnd(const T& Elem);
};

/**************************************************************************/

template <class T>
SAPDB_Bool FileDir_Vector<T>::Reserve(SizeType NewCapacity)
{
    if ( NewCapacity == 0 ) {
        NewCapacity = 2;
    }

    return Container_Vector<T>::Reserve(NewCapacity);
}

/**************************************************************************/

template <class T>
SAPDB_Bool FileDir_Vector<T>::InsertEnd(const T& Elem)
{
    if ( this->IsFull() ) {
        this->Extend(1);
    }
    return Container_Vector<T>::InsertEnd(Elem);
}

/**************************************************************************/

template <class T>
void FileDir_Vector<T>::Insert(Iterator& Pos, const T& Elem)
{
    if ( this->IsFull() ) {
        // resize and reposition iterator:
        IndexType i = this->GetIndex(Pos);
        if ( this->Extend(1) != true ) {
            FileDir_Common::OutOfMemoryCrash( sizeof(T) );
        }
        Pos = this->Position(i);
    }
    Container_Vector<T>::Insert(Pos, Elem);
}

/**************************************************************************/

template <class T>
SAPDB_Bool FileDir_Vector<T>::Extend(const SizeType growBy)
{
    if ( Reserve( this->GetSize() * 2 ) ) {
        return true;
    }

    if (  Reserve( static_cast<SizeType>(this->GetSize() * 1.5) ) ) {
        return true;
    }

    if (  Reserve( this->GetSize() + growBy ) ) {
        return true;
    }

    return false;
}

/**************************************************************************/

template <class T>
SAPDB_Bool FileDir_Vector<T>::AssertSpace(const SizeType noOfElements)
{
    while ( (this->GetCapacity() - this->GetSize()) < noOfElements ) {
        if ( !this->Extend(noOfElements) ) {
            return false;
        }
    }
    return true;
}

/**************************************************************************/

template <class T>
SAPDB_Bool FileDir_Vector<T>::AssertSpace(
    const SizeType noOfElements,
    Iterator& iterator)
{
    while ( (this->GetCapacity() - this->GetSize()) < noOfElements ) {
        // resize and reposition iterator:
        IndexType i = this->GetIndex( iterator );
        if ( !this->Extend(noOfElements) ) {
            return false;
        }
        iterator = this->Position(i);
    }
    return true;
}


/**************************************************************************/

#endif // FILEDIR_VECTOR_HPP
