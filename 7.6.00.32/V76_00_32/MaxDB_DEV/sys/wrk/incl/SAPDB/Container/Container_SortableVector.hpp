/*!
  @file           Container_SortableVector.hpp
  @author         HenrikH, ContainerMaint
  @brief          Class declaration and definition for a template vector class
  @see            
  @author         HenrikH

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2006 SAP AG

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


#ifndef CONTAINER_SORTABLEVECTOR_HPP
#define CONTAINER_SORTABLEVECTOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp"  // QuickSort
#include "Container/Container_Vector.hpp"


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  DECLARATION OF CLASSES, STRUCTURES, TYPES, UNIONS ...                    *
 *===========================================================================*/


/*!
  @class    Container_SortableVector
  @brief    This class implements a sortable vector class.
 */

template <class T>
class Container_SortableVector : public Container_Vector<T>
{

public:
    // typedef copied from Container_Vector.hpp to please various C++ compilers
    typedef SAPDBMem_IRawAllocator Allocator; // Allocator type used for vectors

    /*!
       @brief   Constructs a sortable vector
       @param   allocator [in] allocator
     */

    Container_SortableVector( SAPDBMem_IRawAllocator& allocator )
    : Container_Vector<T>( allocator )
    {}

    /*!
        @brief  data items will be sorted directly which could reduce performance 
                (more CPU is needed) if the items are to big or to much items should 
                be sorted. For big data items it is useful to implement an indirection 
                with pointers to avoid expensive copy operations. To compare to data
                items the default compare operators <,>,= are used.
        @return (bool) true if sort execution was successful
    */
    bool QuickSort(){
        return SAPDBAlgo_QuickSort( this->m_ArrayPtr, this->m_Size );
    }
    /*!
        @brief  data items will be sorted directly which could reduce performance 
                (more CPU is needed) if the items are to big or to much items should 
                be sorted. For big data items it is useful to implement an indirection 
                with pointers to avoid expensive copy operations. To be able to
                compare data items a comparator object derived from SAPDBAlgo_IComparator
                must be given.
      @param    comparator [in] compare operator
      @return (bool) true if sort execution was successful
    */
    bool QuickSort( SAPDBAlgo_IComparator<T>& comparator ){
        return SAPDBAlgo_QuickSort( this->m_ArrayPtr, this->m_Size, comparator );
    }

private:

    // Assignment operator is not supported
    Container_SortableVector& operator=( const Container_SortableVector& );

    // Copy constructor is not supported
    //Container_SortableVector( const Container_SortableVector& );
   
};


#endif // CONTAINER_SORTABLEVECTOR_HPP
