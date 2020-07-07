/*!
 * @file  Join_HashAccessOperator.cpp
 * @brief implements Join_HashAccessOperator
 *
 * @author MartinKi
 * @ingroup Join
 *
 * @sa Join_HashAccessOperator.hpp
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
#include "RunTime/RTE_Message.hpp"

#include "Join/Join_HashAccessOperator.hpp"

#include "Join/Join_HashAccessAllocator.hpp"
#include "Join/Join_IAccessOperator.hpp"
#include "Join/Join_AccessDesc.hpp"
#include "Join/Join_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"
#include "hbd73.h"
#include "hta01.h"
#include "hta01_3.h"
#include "hsp30.h"

class SQLMan_Context;

/* ******************** PUBLIC MEMBERS ********************* */

/*!
 * @param[in] acv global context
 * @param[in] distinctKeys true if keys of table will be unique
 * @param[in] buffersize size of table buffer
 * @param[in] keyLength maximum length of key
 * @param[in] lastKeyColStartPos start position of last key column
 *                               (positions start with 0)
 */
Join_HashAccessOperator::Join_HashAccessOperator(
        SQLMan_Context     &acv,
        const SAPDB_UInt2  tabno,
        const UniqueKeys   uniqueKeys,
        const KeySizes     keySizes,
        const SAPDB_UInt4  &buffersize,
        const SAPDB_Int2   keyLength,
        const SAPDB_Int2   lastKeyColStartPos)
    :
    Join_IAccessOperator(acv, tabno),
    m_allocator(Join_HashAccessAllocator::Instance().GetAllocator()),
    m_accessOperator(0),
    m_hash(0),
    m_hashBuckets(0),
    m_bufferSize(buffersize > BUF8K_MXSP00 ? buffersize : BUF8K_MXSP00),
    m_recordBuffer(0),
    m_hashFilled(false),
    m_hashingFailed(false),
    m_currentEntry(0),
    m_lastKeyColStartPos(lastKeyColStartPos),
    m_maxKeyLength(keyLength),
    m_distinctKeys(uniqueKeys == NoDuplicateKeys),
    m_varSizedKeys(keySizes   == VarSizedKeys),
    m_successfulSelectCount(0),
    m_failedSelectCount(0),
    m_joinKey(0),
    m_joinKeyLen(0)
{
    if ( ! m_acv.IsOk() ) return;

    SAPDBTRACE_METHOD_DEBUG( "Join_HashAccessOperator::ctor", Join_Trace, 1 );

    if ( 0 == Join_HashAccessAllocator::Instance().GetMaxHashTableSize() ) {
        m_hashingFailed = true;
        return;
    }

    m_recordBuffer = reinterpret_cast<SAPDB_Byte*>(
        m_allocator.Allocate( m_bufferSize ) );

    // m_recordBuffer is NULL on failure
}

/*!
 *
 */
Join_HashAccessOperator::~Join_HashAccessOperator()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_HashAccessOperator::dtor", Join_Trace, 1 );

    if ( this->IsOpened() ) this->Close();

    SAPDB_UInt4 bucketCount = 0;
    SAPDB_UInt4 usedBucketCount = 0;
    SAPDB_UInt  maxBucketSize = 0;

    if ( m_hash ) {
        m_hash->GetStatistics( bucketCount, usedBucketCount, maxBucketSize );
    }

    if ( m_hashFilled ) {
        b73cmd_count (ijoin_hash);
    }
            
    if ( Join_Trace.TracesLevel( 1 ) )
    {
        SAPDBTrace_Stream traceStream( &Join_Trace.GetTracePrinter() );
        traceStream
            << "Statistics for Join_HashAccessOperator: " << NewLine
            << "Hash used       : " << m_hashFilled << NewLine
            << "Hash distinct   : " << m_distinctKeys << NewLine
            << "Record count    : " << m_recCount << NewLine
            << "Buffer size     : " << m_bufferSize << NewLine
            << "Bucket count    : " << bucketCount << NewLine
            << "Buckets used    : " << usedBucketCount << NewLine
            << "Max bucket size : " << maxBucketSize << NewLine
            << "Selects ok      : " << m_successfulSelectCount << NewLine
            << "Selects failed  : " << m_failedSelectCount;
    }

    destroy ( m_accessOperator, m_acv.GetAllocator() );

    if ( 0 != Join_HashAccessAllocator::Instance().GetMaxHashTableSize() ) {
        destroy ( m_hash, m_allocator );
        destroyarray ( m_hashBuckets, m_recCount, m_allocator );
        m_allocator.Deallocate( m_recordBuffer );
    }
}

/*!
 *
 */
Join_IAccessOperator* Join_HashAccessOperator::SetAccessOperator(
    Join_IAccessOperator* accessOperator)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::SetAccessOperator", Join_Trace, 1 );
    m_accessOperator = accessOperator;
    return accessOperator;
}

/*!
 *
 */
SAPDB_Byte* Join_HashAccessOperator::GetRecordBuffer()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::GetRecordBuffer", Join_Trace, 1 );
    return m_recordBuffer;
}

/*!
 *
 */
tgg00_BasisError Join_HashAccessOperator::Open()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_HashAccessOperator::Open", Join_Trace, 1 );

    m_currentEntry = 0;
    // hash join should never be called without a key
    return e_system_error;
}

/*!
 * @param[in] startkeys restrict record stream to records with key greater/equal than startkey
 * @param[in] stopkeys restrict record stream to records with key lower/equal than stopkey
 */
tgg00_BasisError Join_HashAccessOperator::Open(
            const Join_TwoKeys& startkeys,
            const Join_TwoKeys& stopkeys )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_HashAccessOperator::Open(startkeys,stopkeys)", Join_Trace, 1 );

    SAPDBERR_ASSERT_STATE( m_accessOperator != 0 );
    tgg00_BasisError _e;

    _e = Join_IAccessOperator::Open( startkeys, stopkeys );

    if ( e_ok == _e && ! m_hashFilled && ! m_hashingFailed ) {
        if ( HashingDuplicateKeys == this->FillHash() ) 
            return e_duplicate_key;
    }

    if ( ! m_hashingFailed ) {
        m_joinKey    = reinterpret_cast<SAPDB_Byte*>(&m_Startkeys.reckey.k());
        m_joinKeyLen = m_Startkeys.reckey.len();
    } else {
        _e = m_accessOperator->Open( startkeys, stopkeys );
    }
    m_currentEntry = 0;
    return _e;
}

/*!
 * @param[in,out] recptr pointer to memory should be filled with record
 * @return information code [e_ok, e_no_next_record] / error code [ ... ]
 */
tgg00_BasisError Join_HashAccessOperator::Next( tgg00_Rec*& recptr )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_HashAccessOperator::Next", Join_Trace, 1 );

    if ( m_hashFilled ) {
        SAPDBERR_ASSERT_STATE( m_joinKeyLen > 0 );
        if ( 0 == m_currentEntry ) {
            m_currentEntry = m_hash->Find( m_joinKey, m_joinKeyLen );
        } else if ( !m_distinctKeys ) {
            // bucket entries are grouped by key, so if the next key
            // does not match we know we are done
            m_currentEntry = m_currentEntry->next;
            if ( (0 != m_currentEntry)
                 && (! m_currentEntry->keyEqual( m_joinKey, m_joinKeyLen) ) ) {
                m_currentEntry = 0;
            }
        } else {
            // do not count this as a miss
            m_currentEntry = 0;
            return e_no_next_record;
        }

        if ( ! m_currentEntry ) {
            ++m_failedSelectCount;
            return e_no_next_record;
        }

        // fake cgg_rec_key_offset byte header:
        recptr = reinterpret_cast<tgg00_Rec*>(
            m_currentEntry->entryPtr - cgg_rec_key_offset );
        ++m_successfulSelectCount;
        return e_ok;
    } else if ( m_hashingFailed ) {
        return m_accessOperator->Next( recptr );
    }

    return e_no_next_record;
}


/*!
 *
 */
void Join_HashAccessOperator::Close()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_HashAccessOperator::Close", Join_Trace, 1 );

    // possibly release table lock
    Join_IAccessOperator::Close();
    m_accessOperator->Close();
}

/* ******************* PROTECTED MEMBERS ******************* */


/* ******************** PRIVATE MEMBERS ******************** */
/*************************************************************************/

Join_HashAccessOperator::Hash::Hash(
    SAPDBMem_IRawAllocator& allocator,
    SAPDB_Bool distinctKeys,
    SAPDB_UInt4 size)
    : m_allocator(allocator),
      m_distinctKeys(distinctKeys),
      m_size(size),
      m_list(0),
      m_usedBucketCount(0),
      m_maxBucketSize(0)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::Hash::Hash",
        Join_Trace, 5 );
}

/*************************************************************************/

Join_HashAccessOperator::Hash::~Hash()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::Hash::~Hash",
        Join_Trace, 5 );
    destroyarray( m_list, m_size + 1, m_allocator );
}

/*************************************************************************/

SAPDB_Bool Join_HashAccessOperator::Hash::Init()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::Hash::Init",
        Join_Trace, 5 );

    newarray( m_list, m_size + 1, m_allocator, static_cast<HashEntry*>(0) );
    return 0 != m_list;
}

/*************************************************************************/

SAPDB_Bool Join_HashAccessOperator::Hash::Insert(
    HashEntry& newEntry,
    SAPDB_Int4 keyLen)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::Hash::Insert",
        Join_Trace, 5 );

    if ( 0 == m_list ) {
        return false;
    }

    SAPDB_UInt count;
    HashSizeType slot =
        HashFunction( newEntry.entryPtr, keyLen );
    if ( m_list[ slot ] == 0 ) {
        m_list[ slot ] = &newEntry;
        ++m_usedBucketCount;
        count = 1;
    } else {
        HashEntry* curEntry = m_list[ slot ];
        if ( m_distinctKeys ) {
            count = 2;
            // we have to make sure that we don't have any duplicates
            while ( curEntry->next ) {
                if ( curEntry->keyEqual( newEntry, keyLen ) ) {
                    return false;
                }
                curEntry = curEntry->next;
                ++count;
            }
            curEntry->next = &newEntry;
        } else {
            count = 1;
            // make sure we always have groups of entries with the same key
            while ( curEntry->next ) {
                if ( curEntry->keyEqual( newEntry, keyLen ) )
                    break;
                curEntry = curEntry->next;
                ++count;
            }
            newEntry.next  = curEntry->next;
            curEntry->next = &newEntry;

            // keep track of bucket size
            while ( curEntry->next ) {
                curEntry = curEntry->next;
                ++count;
            }
        }
    }
    SAPDBTRACE_WRITELN( Join_Trace, 5, "bucket size:" << count);
    if ( count > m_maxBucketSize ) {
        m_maxBucketSize = count;
    }
    return true;
}

/*************************************************************************/

Join_HashAccessOperator::HashEntry*
Join_HashAccessOperator::Hash::Find(
    const SAPDB_Byte* keyPtr,
    SAPDB_Int4        keyLen) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::Hash::Find",
        Join_Trace, 5 );

    if ( 0 == m_list ) {
        return 0;
    }

    HashSizeType slot = HashFunction( keyPtr, keyLen );
    HashEntry* curEntry = m_list[ slot ];
    while ( curEntry ) {
        if ( curEntry->keyEqual( keyPtr, keyLen) ) {
            return curEntry;
        }
        curEntry = curEntry->next;
    }
    return 0;
}

/*************************************************************************/

Join_HashAccessOperator::Hash::HashSizeType
Join_HashAccessOperator::Hash::HashFunction(
    const SAPDB_Byte* keyPtr, SAPDB_Int2 keyLen) const
{
    HashSizeType slot = 0;
    FNV32Hash( reinterpret_cast<const void*>(keyPtr), keyLen, slot );
    return slot % m_size;
}

/*************************************************************************/

void Join_HashAccessOperator::Hash::GetStatistics(
    SAPDB_UInt4& bucketCount,
    SAPDB_UInt4& usedBucketCount,
    SAPDB_UInt&  maxBucketSize) const
{
    bucketCount     = m_size;
    usedBucketCount = m_usedBucketCount;
    maxBucketSize   = m_maxBucketSize;
}

/*************************************************************************/

void Join_HashAccessOperator::HashingFailed(
    const SAPDB_Int4 accessOpMaximumRowsToRead)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::HashingFailed", Join_Trace, 1 );
    tgg00_BasisError _e;

    m_accessOperator->SetMaximumRowsReadAtATime( accessOpMaximumRowsToRead );

    m_accessOperator->Close();
    _e = m_accessOperator->Open( m_Startkeys, m_Stopkeys );
    if ( e_ok != _e )
        m_acv.ThrowError( _e );
    m_hashingFailed = true;
}

/*!
 *
 */
Join_HashAccessOperator::HashingResult Join_HashAccessOperator::FillHash()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_HashAccessOperator::FillHash", Join_Trace, 1 );
    tgg00_Rec* recPtr;

    // store access operator setting to be able to restore it in case
    // of failure
    const SAPDB_Int4 accessOpMaxRowsToRead =
        m_accessOperator->GetMaximumRowsReadAtATime();

    m_accessOperator->SetMaximumRowsReadAtATime( SAPDB_MAX_INT4 );

    // this should read the whole table
    tgg00_BasisError b_err;
    b_err = m_accessOperator->Open();
    if ( e_ok == b_err )
        b_err = m_accessOperator->Next( recPtr );

    if ( ! m_accessOperator->IsResultFullyBuffered() ) {
        SAPDBTRACE_WRITELN(
            Join_Trace, 5, "table could not be completely read" );
        this->HashingFailed( accessOpMaxRowsToRead );
        return HashingBufferTooSmall;
    }

    m_recCount = m_accessOperator->GetBufferedRecordCount();

    m_hash     = new (m_allocator) Hash(
        m_allocator, m_distinctKeys, m_recCount );
    if ( m_hash && (0 == m_recCount) ) {
        m_hashFilled = true;
        return HashingOk;
    }

    newarray( m_hashBuckets, m_recCount, m_allocator );

    if ( (0 == m_hash) || (0 == m_hashBuckets) || !m_hash->Init() ) {
        SAPDBTRACE_WRITELN(
            Join_Trace, 5, "out of hash memory" );
        this->HashingFailed( accessOpMaxRowsToRead );
        return HashingOutOfMemory;
    }

    SAPDB_Byte lastKeyColDefByte   =
        *(reinterpret_cast<SAPDB_Byte*>(recPtr)
          + m_lastKeyColStartPos
          + cgg_rec_key_offset);
    SAPDB_Int2 lastKeyColMaxLength = m_maxKeyLength - m_lastKeyColStartPos;

    SAPDBTRACE_WRITELN(
        Join_Trace, 5,
        "max keylen : " << m_maxKeyLength );

    SAPDB_UInt4 curRecNo = 0;
    while ( (e_ok == b_err) && (curRecNo <= m_recCount) ) {
        SAPDB_Byte* recStart = reinterpret_cast<SAPDB_Byte*>(recPtr)
            + cgg_rec_key_offset;

        m_hashBuckets[ curRecNo ].entryPtr = recStart;
        // determine actual key length
        SAPDB_Int2 keyLen = m_maxKeyLength;
        if ( m_varSizedKeys ) {
            keyLen =
                m_lastKeyColStartPos
                + s30lnr_defbyte(
                    recStart,
                    lastKeyColDefByte,
                    m_lastKeyColStartPos + 2, // start behind def byte
                    lastKeyColMaxLength - 1 )
                + 1; // def byte needs to be taken into account
        }

        if ( !m_hash->Insert( m_hashBuckets[ curRecNo ], keyLen ) ) {
            this->HashingFailed( accessOpMaxRowsToRead );
            SAPDBTRACE_WRITELN(
                Join_Trace, 5, "duplicate keys in hash" );
            return HashingDuplicateKeys;
        }
        b_err = m_accessOperator->Next( recPtr );
        ++curRecNo;
    }
    if ( (e_no_next_record != b_err) || (curRecNo > m_recCount) ) {
        RTE_Message(
            SAPDBErr_Exception(
                __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                "Hash rec count inconsistent" ));
        m_acv.TransContext().trError_gg00 = e_system_error;
        return HashingError;
    }

    m_accessOperator->Close();
    m_hashFilled = true;

    return HashingOk;
}
