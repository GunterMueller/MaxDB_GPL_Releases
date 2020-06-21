/*!
 * \file    OMS_CheckedUserAllocator.hpp
 * \author  ChristophR
 * \brief   liveCache user allocator checked wrapper (for session-specific memory).
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#ifndef __OMS_CHECKEDUSERALLOCATOR_HPP
#define __OMS_CHECKEDUSERALLOCATOR_HPP

#include "ggg01.h"
#include "Oms/OMS_AllocatorInterface.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IAllocatorInfo.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"

class OMS_Session;
class OMS_UserAllocator;

class OMS_CheckedUserAllocator  : public OmsAllocatorInterface, public SAPDBMem_IAllocatorInfo
{
public:
    OMS_CheckedUserAllocator(const char* pName, tsp00_TaskId taskId, OMS_UserAllocator *parent, int code, bool DoRegister);

    ~OMS_CheckedUserAllocator();

    void SetSession(OMS_Session *pSession) {m_pSession = pSession;}

    virtual void *omsAllocate(size_t size);

    virtual void omsDeallocate(void *p);

    virtual void* Allocate(SAPDB_ULong ByteCount);

    virtual void* Allocate(SAPDB_ULong ByteCount, const void*);

    virtual void Deallocate(void* p);

    virtual bool omsCheckAllocatorEmpty();

    virtual void omsRegisterCallback(OmsCallbackInterface *cb);

    virtual OmsCallbackInterface *omsGetCallback() const;

    virtual int omsGetAllocatorType() const;

    // start of SAPDBMem_IAllocatorInfo interface


    /*!
    @brief          Outputs statistics about the memory usage of the allocator.
    @param          BytesUsed [out] The number of bytes used by this allocater.
    @param          BytesControlled [out] The number of bytes controlled by this allocator. 
    The default implementation returns GetInvalidSize() for               all values.
    @return         none

    */
    virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
        SAPDB_ULong &MaxBytesUsed,
        SAPDB_ULong &BytesControlled);

    /*!
    @brief          Outputs statistics about the number of Allocate and Deallocate calls of
    the allocator to the underlying base allocator.
    @param          CountAlloc [out] The number of calls of method Allocate of the base allocator.
    @param          CountDealloc [out] The number of calls of method Deallocate of the base.
    @return         none

    */
    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const;

    /*!
    @brief          Outputs statistics about the number of Allocate and Deallocate calls.
    @param          CountAlloc [out] The number of calls of method Allocate.
    @param          CountDealloc [out] The number of calls of method Deallocate.
    @return         none

    */
    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const;

    /*!
    @brief          Returns the identifier associated with this allocator
    This function is called to identify an allocator instance. The derived
    class is responsible to implement this feature (but must not do so...)
    @return         const SAPDB_UTF8 *
    */
    virtual const SAPDB_UTF8 *GetIdentifier() const;
private:
    OMS_Session            *m_pSession;
    OMS_UserAllocator      *m_alloc;
    SAPDB_ULong             m_bytesUsed;
    SAPDB_ULong             m_maxBytesUsed;
    SAPDB_ULong             m_allocCnt;
    SAPDB_ULong             m_deallocCnt;
    int                     m_code;
    unsigned long           m_allocSize;
    SAPDB_UTF8              m_name[40+1]; 
    RTEMem_AllocatorInfo    m_allocatorInfo;
    bool                    m_doRegister;
};

#endif  // __OMS_CHECKEDUSERALLOCATOR_HPP