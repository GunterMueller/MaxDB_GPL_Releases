/*!
  @file           SAPDBMem_PseudoAllocator.hpp
  @author         JoergM
  @ingroup        Memory Management
  @brief          Pseudo allocator for providing statistic information about memory consumption

  This header file defines and implements a non synchronized and a synchronized version of a pseudo
  allocator. Only the constructor implementation is in a separated implementation module.

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


#ifndef SAPDBMEM_PSEUDOALLOCATOR_HPP
#define SAPDBMEM_PSEUDOALLOCATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"

class SAPDBMem_AllocatorInfo;

/*!
   @class          SAPDBMem_PseudoAllocator
   @brief          This class manages the statistic information for memory blocks.

   This pseudo allocator is not synchronized and threrefore not multi thread save. The constructor
   allows to initalize base allocator and allocator name. The addtional member functions 
    void AddChunk(SAPDB_ULong chunkSizeInBytes)
    void ReleaseChunk(SAPDB_ULong chunkSizeInBytes)
    void AddBaseChunk(SAPDB_ULong baseChunkSizeInBytes)
    void ReleaseBaseChunk(SAPDB_ULong baseChunkSizeInBytes)
   allow to modify statistic information only. Allocate() always returns 0!
*/
class SAPDBMem_PseudoAllocator : public SAPDBMem_IRawAllocator
{
public:
    /*!
      @brief pseudo allocate
      @param count [in] ignored
      @return always 0
     */
    virtual void* Allocate(SAPDB_ULong count) { return 0; }

    /*!
      @brief pseudo allocate
      @param count [in] ignored
      @param hint [in] ignored
      @return always 0
     */
    virtual void* Allocate(SAPDB_ULong count, const void * hint) { return 0; }

    /*!
      @brief pseudo deallocate
      @param p [in] ignored
     */
    virtual void Deallocate(void * p) { return; }

    /*!
      @brief get allocator identifier
      @return the allocator identifier
     */
    virtual const SAPDB_UTF8 * GetIdentifier() const 
    { return (const SAPDB_UTF8 *)&m_Name[0]; }

    /*!
      @brief get allocator statistics
      @param BytesUsed [out] number of bytes used
      @param MaxBytesUsed [out] maximum number of bytes used
      @param BytesControlled [out] number of bytes controlled
     */
    virtual void CalcStatistics( SAPDB_ULong &BytesUsed,
                                 SAPDB_ULong &MaxBytesUsed,
                                 SAPDB_ULong &BytesControlled)
    { BytesUsed = m_BytesUsed; MaxBytesUsed = m_MaxBytesUsed; BytesControlled = m_BytesControlled; }

    /*!
      @brief get base allocator statistics
      @param CountAlloc [out] number of AddBaseChunk calls
      @param CountDealloc [out] number of ReleaseBaseChunk calls
     */
    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const
    { CountAlloc = m_CountBaseAlloc; CountDealloc = m_CountBaseDealloc; }

    /*!
      @brief get call statistics
      @param CountAlloc [out] number of AddChunk calls
      @param CountDealloc [out] number of ReleaseChunk calls
     */
    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const
    { CountAlloc = m_CountAlloc; CountDealloc = m_CountDealloc; }

    /*!
      @brief get chunk size
      @return always 1
     */
    virtual SAPDB_ULong GetChunkSize(const void* p)
    { return 1; }

    /*!
      @brief ctor
      Sets up identifiers
     */
    SAPDBMem_PseudoAllocator( SAPDB_UTF8 const *identifier
                            , SAPDB_UTF8 const *baseIdentifier);

    /*!
      @brief dtor
      does nothing, but must exist
     */
    virtual ~SAPDBMem_PseudoAllocator() 
    {}

    /*!
      @brief add a chunk of used bytes

      This call increases the number of bytes used and if needed the maximum number of bytes used.
      @param chunkSizeInBytes [in] number of bytes added to BytesUsed statistic
     */
    void AddChunk(SAPDB_ULong chunkSizeInBytes)
    {
        m_BytesUsed += chunkSizeInBytes;
        if ( m_MaxBytesUsed < m_BytesUsed )
        {
            m_MaxBytesUsed = m_BytesUsed;
        }
        ++m_CountAlloc;
    }

    /*!
      @brief release a chunk of used bytes

      This call decreases the number of bytes used
      @param chunkSizeInBytes [in] number of bytes subtracted from BytesUsed statistic
     */
    void ReleaseChunk(SAPDB_ULong chunkSizeInBytes)
    {
        m_BytesUsed -= chunkSizeInBytes;
        ++m_CountDealloc;
    }

    /*!
      @brief add a chunk of controlled bytes

      This call increases the number of bytes controlled
      @param chunkSizeInBytes [in] number of bytes added to BytesControlled statistic
     */
    void AddBaseChunk(SAPDB_ULong baseChunkSizeInBytes)
    {
        m_BytesControlled += baseChunkSizeInBytes;
        ++m_CountBaseAlloc;
    }

    /*!
      @brief release a chunk of controlled bytes

      This call decreases the number of bytes controlled
      @param chunkSizeInBytes [in] number of bytes subtracted from BytesControlled statistic
     */
    void ReleaseBaseChunk(SAPDB_ULong baseChunkSizeInBytes)
    {
        m_BytesControlled -= baseChunkSizeInBytes;
        ++m_CountBaseDealloc;
    }

protected:
    SAPDBMem_AllocatorInfo *m_pAllocatorInfo;
    SAPDB_ULong             m_BytesUsed;
    SAPDB_ULong             m_MaxBytesUsed;
    SAPDB_ULong             m_BytesControlled;
    SAPDB_ULong             m_CountAlloc;
    SAPDB_ULong             m_CountDealloc;
    SAPDB_ULong             m_CountBaseAlloc;
    SAPDB_ULong             m_CountBaseDealloc;
    SAPDB_UTF8              m_Name[40+1];
    SAPDB_UTF8              m_BaseName[40+1];
}; /*! @endclass  SAPDBMem_PseudoAllocator */

/*!
   @class          SAPDBMem_SynchronizedPseudoAllocator
   @brief          This class manages the statistic information for memory blocks.

   This pseudo allocator is synchronized and threrefore multi thread save. The constructor
   allows to initalize base allocator and allocator name. The addtional member functions 
    void AddChunk(SAPDB_ULong chunkSizeInBytes)
    void ReleaseChunk(SAPDB_ULong chunkSizeInBytes)
    void AddBaseChunk(SAPDB_ULong baseChunkSizeInBytes)
    void ReleaseBaseChunk(SAPDB_ULong baseChunkSizeInBytes)
   allow to modify statistic information only. Allocate() always returns 0!

  Its implementation is based on the SAPDBMem_PseudoAllocator.
*/
class SAPDBMem_SynchronizedPseudoAllocator : public SAPDBMem_PseudoAllocator
{
public:
    /*!
      @brief pseudo allocate
      @param count [in] ignored
      @return always 0
     */
    virtual void* Allocate(SAPDB_ULong count) { return 0; }

    /*!
      @brief pseudo allocate
      @param count [in] ignored
      @param hint [in] ignored
      @return always 0
     */
    virtual void* Allocate(SAPDB_ULong count, const void * hint) { return 0; }

    /*!
      @brief pseudo deallocate
      @param p [in] ignored
     */
    virtual void Deallocate(void * p) { return; }

    /*!
      @brief get allocator identifier
      @return the allocator identifier
     */
    virtual const SAPDB_UTF8 * GetIdentifier() const 
    { return (const SAPDB_UTF8 *)&m_Name[0]; }

    /*!
      @brief get allocator statistics
      @param BytesUsed [out] number of bytes used
      @param MaxBytesUsed [out] maximum number of bytes used
      @param BytesControlled [out] number of bytes controlled
     */
    virtual void CalcStatistics( SAPDB_ULong &BytesUsed,
                                 SAPDB_ULong &MaxBytesUsed,
                                 SAPDB_ULong &BytesControlled)
    {
        RTESync_LockedScope LockedScope( m_Lock );
        SAPDBMem_PseudoAllocator::CalcStatistics( BytesUsed, MaxBytesUsed, BytesControlled);
    }

    /*!
      @brief get base allocator statistics
      @param CountAlloc [out] number of AddBaseChunk calls
      @param CountDealloc [out] number of ReleaseBaseChunk calls
     */
    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const
    {
        SAPDBMem_PseudoAllocator::GetBaseAllocatorCallStatistics( CountAlloc, CountDealloc);
    }

    /*!
      @brief get call statistics
      @param CountAlloc [out] number of AddChunk calls
      @param CountDealloc [out] number of ReleaseChunk calls
     */
    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const
    {
        SAPDBMem_PseudoAllocator::GetBaseAllocatorCallStatistics( CountAlloc, CountDealloc);
    }

    /*!
      @brief get chunk size
      @return always 1
     */
    virtual SAPDB_ULong GetChunkSize(const void* p)
    { return 1; }

    /*!
      @brief ctor
      Sets up identifiers and initializes spinlock for synchronization
     */
    SAPDBMem_SynchronizedPseudoAllocator( SAPDB_UTF8 const *identifier
                                        , SAPDB_UTF8 const *baseIdentifier);

    /*!
      @brief dtor
      does nothing, but must exist
     */
    virtual ~SAPDBMem_SynchronizedPseudoAllocator() 
    {}

    /*!
      @brief add a chunk of used bytes

      This call increases the number of bytes used and if needed the maximum number of bytes used.
      @param chunkSizeInBytes [in] number of bytes added to BytesUsed statistic
     */
    void AddChunk(SAPDB_ULong chunkSizeInBytes)
    {
        RTESync_LockedScope LockedScope( m_Lock );
        SAPDBMem_PseudoAllocator::AddChunk(chunkSizeInBytes);
    }

    /*!
      @brief release a chunk of used bytes

      This call decreases the number of bytes used
      @param chunkSizeInBytes [in] number of bytes subtracted from BytesUsed statistic
     */
    void ReleaseChunk(SAPDB_ULong chunkSizeInBytes)
    {
        RTESync_LockedScope LockedScope( m_Lock );
        SAPDBMem_PseudoAllocator::ReleaseChunk(chunkSizeInBytes);
    }

    /*!
      @brief add a chunk of controlled bytes

      This call increases the number of bytes controlled
      @param chunkSizeInBytes [in] number of bytes added to BytesControlled statistic
     */
    void AddBaseChunk(SAPDB_ULong baseChunkSizeInBytes)
    {
        RTESync_LockedScope LockedScope( m_Lock );
        SAPDBMem_PseudoAllocator::AddBaseChunk(baseChunkSizeInBytes);
    }

    /*!
      @brief release a chunk of controlled bytes

      This call decreases the number of bytes controlled
      @param chunkSizeInBytes [in] number of bytes subtracted from BytesControlled statistic
     */
    void ReleaseBaseChunk(SAPDB_ULong baseChunkSizeInBytes)
    {
        RTESync_LockedScope LockedScope( m_Lock );
        SAPDBMem_PseudoAllocator::ReleaseBaseChunk(baseChunkSizeInBytes);
    }

private:
    RTESync_NamedSpinlock m_Lock; //!< synchronisation object
}; /*! @endclass  SAPDBMem_SynchronizedPseudoAllocator */

#endif /* SAPDBMEM_PSEUDOALLOCATOR_HPP */
