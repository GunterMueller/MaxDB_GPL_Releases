/*****************************************************************************/
/*!
  @file         Join_HashAccessAllocator.hpp
  @author       MartinKi
  @ingroup      Join

  @brief        Header file for class Join_HashAccessAllocator.

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

#ifndef JOIN_HASHACCESSALLOCATOR_HPP
#define JOIN_HASHACCESSALLOCATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
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

class SAPDBMem_IRawAllocator;
class SAPDBMem_SynchronizedRawAllocator;

/*!
 * @class Join_HashAccessAllocator
 * @author MartinKi
 *
 * @brief A singleton to provide a bounded allocator for the
 *        Join_HashAccessOperator class.
 */
class Join_HashAccessAllocator : public SAPDB_Singleton
{
public:
    /*!
     * @brief Create hash access allocator singleton. The singleton
     *        will use the given allocator for its memory requests.
     */
    static SAPDB_Bool Create(SAPDBMem_IRawAllocator& allocator);

    /*!
     * @brief Returns the Join_HashAccessAllocator. If it didn't exist
     *        it will be created using the default allocator.
     */
    static Join_HashAccessAllocator& Instance();

    /*!
     * @brief Returns the maximum size for a single hash table. If the
     *        allocator could not be created a size of 0 is returned.
     *
     * This limit is not enforced by the allocator but users of the
     * allocator should honor it.
     */
    SAPDB_UInt GetMaxHashTableSize() const;

    /*!
     * @brief Returns the allocator that should be used for join hash
     *        table creation. If GetMaxHashTableSize() returned 0 this
     *        method * *must not* be called.
     */
    SAPDBMem_IRawAllocator& GetAllocator();

    /*!
     * @brief Sets upper limit of memory to be used by allocator.
     *
     * If no allocator is available, or if the amount of memory
     * currently in use is greater than the new limit the limit will
     * not be changed.
     *
     * @param[in] newMaxHashTableMemory new limit for allocator
     *
     * @return true, if new limit could be set; false if the limit
     * could not be changed
     */
    SAPDB_Bool SetMaxHashTableMemory(const SAPDB_UInt newMaxHashTableMemory);

    /*!
     * @brief Sets upper limit of memory available for a single hash table.
     *
     * This limit is not enforced by the allocator but users of the
     * allocator should honor it.
     *
     * @param[in] newMaxHashTableSize new memory limit for a single
     *        hash table
     *
     * @return true if allocator is available; false otherwise
     */
    SAPDB_Bool SetMaxHashTableSize(const SAPDB_UInt newMaxHashTableSize);

private:
    SAPDB_UInt m_maxSingleTableSize;
    SAPDB_UInt m_maxHashTableMemory;
    SAPDBMem_SynchronizedRawAllocator* m_allocator;

    static Join_HashAccessAllocator* m_instance;

    Join_HashAccessAllocator(SAPDBMem_IRawAllocator& allocator);
};

/*************************************************************************/

inline Join_HashAccessAllocator& Join_HashAccessAllocator::Instance()
{
    if ( m_instance ) {
        return *m_instance;
    }

    SAPDBMem_IRawAllocator& alloc = RTEMem_Allocator::Instance();
    m_instance = new (alloc) Join_HashAccessAllocator(alloc);
    return *m_instance;
}

/*************************************************************************/

inline SAPDB_UInt Join_HashAccessAllocator::GetMaxHashTableSize() const
{
    return m_maxSingleTableSize;
}

/*************************************************************************/

inline SAPDBMem_IRawAllocator& Join_HashAccessAllocator::GetAllocator()
{
    return *m_allocator;
}

/*************************************************************************/

#endif // JOIN_HASHACCESSALLOCATOR_HPP
