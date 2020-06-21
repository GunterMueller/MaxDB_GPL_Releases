#ifndef JOIN_HASHACCESSOPERATOR_HPP
#define JOIN_HASHACCESSOPERATOR_HPP
/*!
 * @file  Join_HashAccessOperator.hpp
 * @brief realize one table access operator by means of Join_AccessDesc
 *
 * @author MartinKi
 * @ingroup Join
 *
 * @sa Join_AccessDesc.hpp
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
#include "Join/Join_IAccessOperator.hpp"
#include "Join/Join_AccessDesc.hpp"
#include "Join/Join_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "gsp00.h"
#include "ggg00.h"

class SAPDBMem_IRawAllocator; //<! forward declaration
class SQLMan_Context;

//! operator for base table access via key or index
class Join_HashAccessOperator: public Join_IAccessOperator
{
public:
    enum KeySizes {
        VarSizedKeys,
        FixSizedKeys
    };

    enum UniqueKeys {
        NoDuplicateKeys,
        AllowDuplicateKeys
    };

    //! @name constructor / destructor
    //@{
    //! constructor
    Join_HashAccessOperator(
            SQLMan_Context     &acv,
            const SAPDB_UInt2  tabno,
            const UniqueKeys   uniqueKeys         = AllowDuplicateKeys,
            const KeySizes     keySizes           = VarSizedKeys,
            const SAPDB_UInt4  &buffersize        = BUF8K_MXSP00,
            const SAPDB_Int2   keyLength          = -1,
            const SAPDB_Int2   lastKeyColStartPos = -1);

    //! destructor
    virtual ~Join_HashAccessOperator();
    //@}

    //! @name stream manipulation
    //@{
    //! open operator with keys given by strategy information
    virtual tgg00_BasisError Open();

    //! open stream operator and limit stream with start/stop key
    virtual tgg00_BasisError Open(
            const Join_TwoKeys& startkeys,
            const Join_TwoKeys& stopkeys );

    //! get next record from stream
    virtual tgg00_BasisError Next( tgg00_Rec*& record);

    //! close operator
    virtual void Close();
    //@}

    //! Sets the operator used to fetch the actual records.
    Join_IAccessOperator* SetAccessOperator(
        Join_IAccessOperator* accessOperator);

    //! Returns a pointer to the buffer where the records are stored.
    SAPDB_Byte*  GetRecordBuffer();

    //! Returns the size of the record buffer.
    SAPDB_UInt4* GetRecordBufferSize();

protected:
    void SelectRecords() {}

private:
    enum HashingResult {
        HashingOk,
        HashingError,
        HashingBufferTooSmall,
        HashingOutOfMemory,
        HashingDuplicateKeys
    };

    HashingResult FillHash();
    void HashingFailed(const SAPDB_Int4 accessOpMaximumRowsToRead);

    struct HashEntry
    {
        SAPDB_Byte* entryPtr;
        HashEntry* next;

        HashEntry()
            : entryPtr(0), next(0)
        {}

        SAPDB_Bool keyEqual(const SAPDB_Byte* keyPtr, SAPDB_Int2 keyLen) {
            return ( memcmp( this->entryPtr,
                             keyPtr, keyLen ) == 0 );
        }

        SAPDB_Bool keyEqual(const HashEntry& e, SAPDB_Int2 keyLen) {
            return ( memcmp( this->entryPtr,
                             e.entryPtr,
                             keyLen ) == 0 );
        }
    };

    class Hash;
    friend class Hash;

    class Hash
    {
    public:
        Hash(
            SAPDBMem_IRawAllocator& allocator,
            SAPDB_Bool distinctKeys,
            SAPDB_UInt4 size);

        ~Hash();

        SAPDB_Bool Init();

        // return values: true, insert ok; false, duplicate key
        SAPDB_Bool Insert(
            HashEntry& entryPtr,
            SAPDB_Int4 keyLen);

        HashEntry* Find(
            const SAPDB_Byte* keyPtr,
            SAPDB_Int4        keyLen) const;

        void GetStatistics(
            SAPDB_UInt4& bucketCount,
            SAPDB_UInt4& usedBucketCount,
            SAPDB_UInt&  maxBucketSize) const;

    private:
        typedef SAPDB_UInt4 HashSizeType;

        HashSizeType HashFunction(
            const SAPDB_Byte* keyPtr,
            SAPDB_Int2 keyLen) const;

        SAPDBMem_IRawAllocator& m_allocator;
        HashEntry**             m_list;
        HashSizeType            m_size;
        SAPDB_UInt4             m_usedBucketCount;
        SAPDB_UInt              m_maxBucketSize;
        SAPDB_Bool              m_distinctKeys;
    };

    SAPDBMem_IRawAllocator& m_allocator;
    Join_IAccessOperator*   m_accessOperator;
    Hash*                   m_hash;
    HashEntry*              m_currentEntry;
    HashEntry*              m_hashBuckets;
    SAPDB_UInt4             m_bufferSize;
    SAPDB_Byte*             m_recordBuffer;
    SAPDB_Bool              m_hashFilled;
    SAPDB_Bool              m_hashingFailed;
    SAPDB_Int2              m_maxKeyLength;
    SAPDB_Int2              m_lastKeyColStartPos;
    SAPDB_Bool              m_distinctKeys;
    SAPDB_Bool              m_varSizedKeys;
    SAPDB_Byte*             m_joinKey;
    SAPDB_Int2              m_joinKeyLen;
    SAPDB_UInt4             m_recCount;
    SAPDB_UInt4             m_successfulSelectCount;
    SAPDB_UInt4             m_failedSelectCount;
};

#endif
