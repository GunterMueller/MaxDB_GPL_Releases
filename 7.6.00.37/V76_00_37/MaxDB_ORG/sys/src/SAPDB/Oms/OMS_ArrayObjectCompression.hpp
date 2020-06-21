
/*!
* \file    OMS_ArrayObjectCompression.hpp
* \author  ReinerSi
* \brief   Compression implementation for array objects.
*/

/*

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



*/

#ifndef OMS_ARRAY_OBJECT_COMPRESSION_HPP
#define OMS_ARRAY_OBJECT_COMPRESSION_HPP

#include "Oms/OMS_ArrayObjectProxy.hpp"

#define OMS_ARRAY_COMPRESSION_BITS_PER_CLASS 2 

// byte range from 0..8 ( -> 9 different values )
#define OMS_ARRAY_COMPRESSION_MAX_BYTE_COUNT 9 

#define OMS_ARRAY_COMPRESSION_CLASS_COUNT 4 

/// mask first compression class from compression index value
#define OMS_ARRAY_COMPRESSION_INDEX_ENTRY_MASK 3 

#define OMS_ARRAY_BITS_PER_BYTE 8
#define OMS_ARRAY_COMPRESSION_ENTRIES_PER_BYTE ( OMS_ARRAY_BITS_PER_BYTE / OMS_ARRAY_COMPRESSION_BITS_PER_CLASS )
#define OMS_ARRAY_COMPRESSION_ENTRIES_PER_INDEX_VALUE ( OMS_ARRAY_COMPRESSION_ENTRIES_PER_BYTE * sizeof( OmsTypeUInt4 ) )

#define OMS_ARRAY_BYTE_MASK 0xFF

extern const AFX_EXT_CLASS OmsTypeChar OMS_ARRAY_COMPRESSION_CLASSES[]; 
extern const AFX_EXT_CLASS OmsTypeChar OMS_ARRAY_COMPRESSION_OFFSET_LOOKUP_TABLE[]; 

//==========================================================================

/*!
* Data buffer, that can be used for temporary storage in memory. 
* The memory for the buffer is allocated from the current OmsSession
* and returned when the buffer is destroyed. 
*/
class AFX_EXT_CLASS OmsDataBuffer : public OmsSessionObject
{
public:

  OmsDataBuffer( OmsHandle& h, OmsTypeUInt4 size ); 
  ~OmsDataBuffer(); 

  OmsTypeByte* getBuffer()  { return m_buffer;  }

  /*!
  * Returns the size of this buffer. 
  */
  OmsTypeUInt4 getSize()          { return m_size; }

private: 
  OmsHandle& m_h; 
  OmsTypeUInt4 m_size; 
  OmsTypeByte* m_buffer; 
};

//==========================================================================

/*! 
 * Different compression schemes
 */
enum OmsCompressionScheme {
  CA_LightWeight_0_4_8_Rep_Local_Buffer = 2
};

class OmsArrayCompressionStatistics; 

/*!
* Data types for int8 data compression
*/
class AFX_EXT_CLASS OmsTypeInt8CompressionBase
{
public: 
  enum OmsCompressionClassEnum {
    CC_Rep_Group = 0,
    CC_4_Byte = 1,
    CC_8_Byte = 2,
    CC_0_Byte = 3
  };

  /*!
   * Indicates, if 4-Byte-Storage is sufficient for given value (signed or unsigned)
   * 
   * \param val value to check
   * 
   * \return true, if given value can be stored as 4-Byte integer, false otherwise
   */
  static bool is4ByteValue( OmsTypeInt8 val )
  {
    // make value positive
    if( val < 0 ) { val = -val; }
    // 4-Bytes are enough, if bit shift by 31 Bits return 0
    return ( ( val >> 31 ) == 0 ); 
  }

  /*!
   * Prints the statistic information to a message file. 
   * 
   * \param h OmsHandle
   * \param stats statistic data to print
   */
  static void printStatistics( OmsHandle& h,OmsSchemaHandle sh, OmsContainerNo cno, OmsTypeInt4 clsGuid, OmsArrayCompressionStatistics& stats  );

  static void clearStatistics( OmsHandle& h ); 

  /*! 
   * Calculates the byte offset in the data buffer for given index entries. 
   * 
   * \param ind_val the index value containing the index entries
   * \param start the number of the first index entry
   * \param end the number of the last index entry
   * 
   * \return total byte offset for the specified index entries
   */
  static size_t getOffsetFromIndexValue( OmsTypeUInt4 ind_val, short start=0, short end=16 );

  /*! 
   * Calculates the byte offset in the data buffer for multiple index values. 
   * The last index enty containing non repetion groups is returned as parameter. 
   * 
   * \param buf pointer to the first index entry
   * \param count number of index entries to use for calculation
   * \param last_ind last index entry with non-repetition-group
   * 
   * \return total byte offset for the specified index entries
   */
  static size_t getOffset( const OmsTypeUInt4* buf, size_t count, size_t diff, OmsTypeUInt4& last_ind ); 

};

//==========================================================================

/*!
 * Helper class used for gathering statistics about internal data. 
 */
class AFX_EXT_CLASS OmsArrayCompressionStatistics : public OmsTypeInt8CompressionBase
{
public: 

  OmsArrayCompressionStatistics();

  /*! 
   * Add value to the statistics. 
   * 
   * \param cl compression class the value is stored in
   * \param val uncompressed value
   */
  void addValue( OmsCompressionClassEnum cl, OmsTypeInt8 val );


  /*!
   * Returns the netto byte count of all elementes added to this object. 
   * The netto byte count is the number of bytes, that the object needs (object size)
   */
  OmsTypeUInt8 getNettoBytes()                 { return m_netto_bytes; }
  /*!
   * Adds the given amount to the netto byte count. 
   */
  void addNettoBytes( OmsTypeUInt8 netto )     { m_netto_bytes += netto; }

  /*!
   * Returns the reserved byte count of all elementes added to this object. 
   * The reserved byte count is the netto byte count plus space wasted due to 
   * the use of array categories. 
   */
  OmsTypeUInt8 getReservedBytes()              { return m_reserved_bytes;  }
  /*!
   * Adds the given amount to the netto byte count. 
   */
  void addReservedBytes( OmsTypeUInt8 res )    { m_reserved_bytes += res; }

  /*!
   * Returns the brutto byte count of all elementes added to this object. 
   * The brutto byte count is the reserved byte count plus any livecache internal alignment 
   * and administration data. 
   */
  OmsTypeUInt8 getBruttoBytes()                { return m_brutto_bytes; }
  /*!
   * Adds the given amount to the brutto byte count. 
   */
  void addBruttoBytes( OmsTypeUInt8 brutto )   { m_brutto_bytes += brutto; }


  /*!
   * Returns the highest compression class number. 
   */
  OmsTypeUInt4 getMaxCompressionClassCount() 
  {
    return OMS_ARRAY_COMPRESSION_CLASS_COUNT; 
  }

  /*!
   * Returns the number of elemetns for a given compression class. 
   */
  OmsTypeUInt4 getCompressionClassCount( OmsCompressionClassEnum idx )
  {
    if( idx < OMS_ARRAY_COMPRESSION_CLASS_COUNT )
    {
      return m_cl[ idx ]; 
    }
    return 0; 
  }

  /*!
   * Returns the maximum storage size. 
   */
  OmsTypeUInt4 getMaxStorageSizeCount()
  {
    return OMS_ARRAY_COMPRESSION_MAX_BYTE_COUNT; 
  }

  /*!
   * Returns the number of elements in this statistic object, 
   * for a given storage size. 
   * 
   * \param size storage size
   */
  OmsTypeUInt4 getStorageSizeCount( OmsTypeUInt4 size )
  {
    if( size < OMS_ARRAY_COMPRESSION_MAX_BYTE_COUNT )
    {
      return m_bytes[ size ]; 
    }
    return 0; 
  }

  /*!
   * Returns the number of elements that have been added to this statistics object. 
   *
   * \return number of elements
   */
  OmsTypeUInt4 getElemCount() 
  {
    OmsTypeUInt4 count( 0 ); 
    for( OmsTypeUInt4 i=0; i < OMS_ARRAY_COMPRESSION_CLASS_COUNT; ++i )
    {
      count += m_cl[ i ]; 
    }
    return count; 
  }

  /*!
   * Returns the number of arrays, that have been added. 
   */
  OmsTypeUInt4 getArrayCount() 
  {
    return m_array_count; 
  }

  /*!
   * Increment the array number counter
   */
  void incArrayCount() 
  {
    ++m_array_count; 
  }

private: 

  void incCompressionClass( OmsCompressionClassEnum cl ) { ( m_cl[ cl ] )++;  }
  void incStorageSize( OmsTypeUInt4 byte_cnt )           { ( m_bytes[ byte_cnt ] )++; }

  OmsTypeUInt4 m_cl[ OMS_ARRAY_COMPRESSION_CLASS_COUNT ];         ///< compression classes 0-4
  OmsTypeUInt4 m_bytes[ OMS_ARRAY_COMPRESSION_MAX_BYTE_COUNT ];   ///< bytes used for values

  OmsTypeUInt8 m_netto_bytes;      ///< total netto bytes 
  OmsTypeUInt8 m_brutto_bytes;     ///< total brutto bytes
  OmsTypeUInt8 m_reserved_bytes;   ///< total reserved bytes

  OmsTypeUInt4 m_array_count;      ///< number of arrays
};

//==========================================================================

/*!
* Abstract class, that represents a generic data sink for array compression. 
* The data sink is used for writing data from local compression buffer to 
* persistent data pages. 
*/
class OmsCompressionDataSink
{
public: 

  virtual OmsTypeByte* storeBuffer( const OmsTypeByte* data_ptr, OmsTypeUInt4 data_size ) = 0; 
  virtual OmsTypeByte* getNextDataBuffer( OmsTypeUInt4 size_needed, OmsTypeUInt4& best_size ) = 0; 
  virtual OmsTypeUInt4 getMaxChunkSize() = 0; 

  virtual ~OmsCompressionDataSink() { }
};

//==========================================================================

/*! 
 * Compression buffer object used for handling local compression index. 
 * With local compression index storage each data chunk has its own compression index, 
 * that contains the compression classes of only the elements, that are stored in the same chunk. 
 * This makes a data chunk self contained. 
 */
class AFX_EXT_CLASS OmsCompressionBufferLocalIndex : public OmsTypeInt8CompressionBase, public OmsArrayTrace
{
public: 

  OmsCompressionBufferLocalIndex( OmsHandle& h, OmsCompressionDataSink& data_sink, OmsTypeUInt4 n ); 

  /*! 
   * Append a repetition group to this compression buffer. 
   */
  void appendRepGroup( OmsTypeInt8 val ); 

  /*! 
   * Append a zero to this comression buffer. 
   */
  void append0Byte(); 

  /*! 
   * Append a 4 byte value to this compression buffer. 
   */
  void append4Byte( OmsTypeInt4 val ); 

  /*! 
   * Append a 8 byte value to this compression buffer.  
   */
  void append8Byte( OmsTypeInt8 val ); 

  /*! 
   * Returns the start pointer of the buffer. 
   */
  OmsTypeByte* getBuffer(); 

  /*! 
   * Flushes all remaining data to data sink. 
   * This is the very last operation, that should be called
   * during data compression. The flush() operation
   * also performans splitting the last values on different sized chunks. 
   */
  void flush(); 

private: 

  // prelimery!
  void searchSplitPointBackwards( OmsTypeUInt4 best_size
                                , OmsTypeUInt4 needed_size
                                , OmsTypeUInt4 elem_count
                                , OmsTypeByte* data_buf_start
                                , OmsTypeByte* index_buf_start 
                                , OmsTypeUInt4& elem_count_res
                                , OmsTypeByte*& data_buf_res
                                , OmsTypeByte*& index_buf_res
                                ); 

  /*!  
   * Search a point, at that the buffer can be split. 
   * When performing tail splitting. 
   * 
   * \param best_size wished split size
   * \param data_buf_start 
   */
  void searchSplitPointForward( OmsTypeUInt4 best_size
                              , OmsTypeByte* data_buf_start
                              , OmsTypeByte* index_buf_start 
                              , OmsTypeUInt4& elem_count_res
                              , OmsTypeByte*& data_buf_res
                              , OmsTypeByte*& index_buf_res
                              , OmsTypeUInt4& offset
                              ); 

  void copyBufferData( OmsTypeUInt4 elem_count
                     , OmsTypeByte* tgt
                     , OmsTypeUInt4 tgt_size
                     , OmsTypeByte* data_buf
                     , OmsTypeUInt4 data_size
                     , OmsTypeByte* index_buf
                     , OmsTypeUInt4 index_size 
                     ); 

  void adjustIndexBuffer( OmsTypeByte* cur_index
                        , OmsTypeByte* stop_index_ptr
                        , OmsTypeUInt4 offset 
                        ); 

  /*!
   * Append a new compression class to the compression buffer. 
   * If the new entry does not fit in the current data buffer, the operation will internally perform a buffer switch. 
   * 
   * \param cl compression class 
   * \param buf_elem_needed number of data elements in the data buffer, that are needed to store the new entry
   */
  void appendCompressionClass( OmsCompressionClassEnum cl, size_t buf_elem_needed ); 

  /*!
   * Makes the content of the current data buffer persistent and re-initializes 
   * the local buffer for new compressesd data. 
   */
  void switchBuffer(); 

  /*!
   * Returns the last compression class, that is not a repetition group, starting search from
   * the given value. 
   * 
   * \param index_buf_start start pointer of compression index
   * \param offset offset of current index entry at buffer start
   */
  OmsCompressionClassEnum getLastNonRepCompClass( OmsTypeByte* index_buf_start, OmsTypeUInt4 offset ); 

  OmsDataBuffer m_buffer;                 ///< compression buffer
  OmsCompressionDataSink& m_data_sink;    ///< sink for writing persistent data

  OmsTypeUInt4 m_pos;                     ///< number of elements currently in local buffer
  OmsTypeUInt4 m_size;                    ///< number of elements written to persistent storage so far

  OmsTypeUInt4* m_index_ptr;              ///< current index pointer
  OmsTypeInt4* m_data_ptr;                ///< current data pointer

  OmsTypeInt4* m_last_elem;               ///< last compressed element
  OmsTypeUInt4 m_last_copy_count; 
  OmsHandle& m_h;                         ///< OmsHandle used for tracing
};

//==========================================================================

/*!
 * Lightweight compression implementation for 8-Byte integer values. 
 * The Compression buffer is responsible for the physical data layout of the array data. 
 */
template < typename T, class Buffer >
class OmsArrayCompression : public OmsTypeInt8CompressionBase
{
public: 

  OmsArrayCompression( OmsHandle& h, OmsCompressionDataSink& data_sink, OmsTypeUInt4 count ) 
    : m_buffer( h, data_sink, count )
    , m_count( 0 )
  { }

  /*!
   * Compresses the all data in the given enumerator and writes
   * the compressed representation into the underlying data sink. 
   * 
   * \param input_data enumerator giving access to the compression data
   */
  void compress( OmsEnumerator< OmsTypeInt8 >& input_data ) 
  {
    // const OmsTypeInt8* data_input_ptr = data; 
    OmsTypeInt8 last_elem = 0; 

    // compress data elements
    for( ; input_data; ++input_data )
    {
      compressDataElement( input_data(), last_elem ); 
      last_elem = input_data(); 
      ++m_count; 
    }
    // flush remaining data in buffer
    m_buffer.flush(); 
  }

  /*!
   * Returns the internal data buffer, that is used for compression
   */
  OmsTypeByte* getBuffer() 
  {
    return m_buffer.getBuffer(); 
  }

  /*!
   * Returns the number of elements compressed so far
   * 
   * \return number of compressed elements
   */
  size_t getElemCount() 
  {
    return m_count; 
  }

private: 

  /*! 
   * Calculates the number of bytes the given value needs in compressed state. 
   * 
   * \param val value to be compressed
   * \param last_value predecessor element of the value to be compressed
   * 
   * \return size in bytes of the compressed data representation
   */
  size_t getCompressedSize( OmsTypeInt8 val, OmsTypeInt8 last_val )
  {
    // zero
    if( val == 0 )
    {
      return 0; 
    }
    // repetition group
    if( val == last_val )
    {
      return 0; 
    }
    // 4-byte value
    if( is4ByteValue( val ) )
    {
      return 4; 
    } 
    // otherwise 8 byte
    return 8; 
  }

  /*!
  * Compress next data element. last element is also needed in order
  * to detect repeating groups
  *
  * \param val Next data element to be compressed
  * \param last_val last data element
  */
  void compressDataElement( OmsTypeInt8 val, OmsTypeInt8 last_val = 0 ) 
  {
    // value 0 has priority over repetition group

    // zero
    if( val == 0 )
    {
      m_buffer.append0Byte(); 
      return; 
    }
    // repetition group
    if( val == last_val )
    {
      m_buffer.appendRepGroup( val ); 
      return; 
    }
    if( is4ByteValue( val ) )
    {
      // 4-Byte
      m_buffer.append4Byte( OmsTypeInt4( val ) ); 
    } else {
      // 8-Byte
      m_buffer.append8Byte( val ); 
    }
  }

  Buffer m_buffer;  ///< Buffer used for data compression
  size_t m_count;   ///< number of elements in array 
};

//==========================================================================

template < class T, int ClsGuid, class OmsArrayObjectBuffer >
class OmsCompressionLocalIndexIterator
{
};

template < int ClsGuid, class OmsArrayObjectBuffer >
class OmsCompressionLocalIndexIterator< OmsTypeInt8, ClsGuid, OmsArrayObjectBuffer > : public OmsTypeInt8CompressionBase
{
public: 
  typedef OmsArrayData< OmsTypeByte, ClsGuid > OmsArrayDataType; 
  typedef OmsOid< OmsArrayData< OmsTypeByte, ClsGuid > > OmsArrayDataOid; 

private: 
  OmsHandle& m_h;                         ///<! current OmsHandle

  OmsTypeUInt4 m_ind_val;                 ///<! current index value

  OmsTypeUInt4 m_pos ;                    ///<! current global array index
  OmsTypeUInt4 m_size;                    ///<! number of elements in the current array

  OmsTypeUInt4 m_local_pos;               ///<! element index in the current data chunk
  OmsTypeUInt4 m_local_size;              ///<! number of elements in the current data chunk

  const OmsArrayObjectBuffer& m_buffer;   ///<! chunk buffer

  const OmsTypeUInt4* m_index_cur;        ///<! pointer to current index value in chunk
  const OmsTypeByte* m_data_cur;          ///<! pointer to current data value in chunk
  OmsTypeInt8 m_cur_val;                  ///<! current decompressed data value

public: 

  OmsCompressionLocalIndexIterator( OmsHandle& h, const OmsArrayObjectBuffer& buf, OmsTypeUInt4 size ) 
    : m_h( h )
    , m_size( size )
    , m_ind_val( 0 )
    , m_pos( 0 )
    , m_local_pos( 0 )
    , m_local_size( 0 )
    , m_buffer( buf )
    , m_data_cur( 0 )
    , m_index_cur( 0 )
    , m_cur_val( 0 )
  { 
    // initialize iterator
    reset(); 
  }

  /*!
   * Sets the iterator to the first value
   */
  void reset() 
  {
    // if first chunk is not set, all elements are 0
    m_buffer.beforeFirst();     
    if( m_buffer.next() )
    {
      m_pos = 0; 
      setCurrentChunk( m_buffer.curr() ); 
    }
    setValueInternal( getCompressionClass() ); 
  }

  /*!
   * Returns the current value of the iterator
   */
  OmsTypeInt8& getValue() 
  {
    return m_cur_val; 
  }

  /*!
   * Returns the compression class in the current index entry
   */
  OmsCompressionClassEnum getCompressionClass() 
  {
    // current compression class is stored in first two bits of local index value
    return OmsCompressionClassEnum( m_ind_val & OMS_ARRAY_COMPRESSION_INDEX_ENTRY_MASK );
  }

  /*!
   * Set iterator on next element
   */
  void operator++() 
  {
    next(); 
  }

  /*!
  * Returns true, if more elements are available
  */
  operator bool() const 
  {
    return ( ( m_pos + m_local_pos ) < m_size ); 
  }

  /*! 
   * Set iterator to the element at the given position
   * 
   * \param index to set as current position
   */
  void absolute( OmsTypeUInt4 pos )
  {
    // reset current data
    reset(); 
    skip( pos ); 
  }

  /*!
   * Advances the current iterator posistion by the given count elements. 
   * 
   * \param mCount number of elements to skip forward 
   */
  void forward( OmsTypeUInt4 mCount )
  {
    skip( mCount ); 
  }

  /*!
   * Sets the iterator to the next position. 
   * 
   */
  void next() 
  {
    ++m_local_pos; 
    // advance only, if there is still data available
    if( ( m_pos + m_local_pos ) >= m_size )
      return; 

    // set index pointer to next index entry
    advanceIndex(); 
    OmsCompressionClassEnum cl = getCompressionClass(); 
    if( cl != CC_Rep_Group )
    {
      setValueInternal( cl ); 
    }
  }

  /*!
   * Skips the given number of entries, starting from the current
   * iterator position. After this operation the current element index is 
   * pos + toSkip
   * 
   * \param toSkip number of elements to skip
   */
  void skip( OmsTypeUInt4 toSkip )
  {
    // looking for chunk where value is stored in
    OmsTypeUInt4 local_remainder = m_local_size - m_local_pos; 
    while( toSkip >= local_remainder )
    {
      toSkip -= local_remainder; 
      setNextChunk(); 
      local_remainder = m_local_size; 
      setValueInternal( getCompressionClass() ); 
    }

    while( toSkip > 0 )
    {
      next(); 
      --toSkip;
    }
  }

private: 

  /*! 
   * Sets the current chunk to the next chunk in the internal chunk chain
   */
  void setNextChunk() 
  {
    m_pos += m_local_size; 
    setCurrentChunk( m_buffer.next() ); 
  }

  /*! 
   * Sets the current chunk to the given chunk. 
   * After setting the chunk, the first value in the chunk will 
   * be the next value to be read. 
   * 
   * \param chunk chunk to be the current chunk 
   */
  void setCurrentChunk( const OmsArrayDataType* chunk )
  {
    if( chunk )
    {
      m_local_pos = 0; 
      m_data_cur = chunk->omsBegin(); 
      m_index_cur = reinterpret_cast< const OmsTypeUInt4* >( chunk->omsBegin() + chunk->omsGetSize() - sizeof( OmsTypeUInt4 ) );
      m_local_size = *m_index_cur; 
      --m_index_cur; 
      m_ind_val = *m_index_cur; 
    } else {
      // should never happen
    }
  }

  /*!
   * Set the compression index to the next index entry. 
   * If the next entry does not fit in the current chunk, 
   * the next chunk is loaded and the index entry is read from the beginning of the new chunk. 
   */
  void advanceIndex()
  {
    // move index pointer to next value if new index is multiple of 16
    if( m_local_pos >= m_local_size )
    {
      m_pos += m_local_size; 
      setCurrentChunk( m_buffer.next() ); 
    } else {
      if( ! ( m_local_pos & 15 ) )
      {
        setNextIndexValue(); 
      } else {
        // shift bits, so that compression class of current entry
        // is stored in the 2 lower bits of the current index value
        m_ind_val = m_ind_val >> OMS_ARRAY_COMPRESSION_BITS_PER_CLASS; 
      }
    }
  }

  /*!
   * Sets the next next value as current index value. 
   */
  void setNextIndexValue() 
  {
    // advance index pointer 
    --m_index_cur; 
    // store next index value in local buffer 
    m_ind_val = *m_index_cur; 
  }

  /*!
   * Extracts and sets the value, that is stored at the current data pointer. 
   * The given compression class indicates, how many bytes the value occupies
   * in the data buffer. 
   *
   * \param cl Compression Class the value is stored in
   */
  void setValueInternal( OmsCompressionClassEnum cl ) 
  {
    switch( cl )
    {
    case CC_0_Byte: 
      m_cur_val = 0; 
      break; 
    case CC_4_Byte: 
      m_cur_val = *reinterpret_cast< const OmsTypeInt4* >( m_data_cur ); 
      m_data_cur += sizeof( OmsTypeInt4 ); 
      break; 
    case CC_8_Byte: 
      OmsTypeInt4* tgt = reinterpret_cast< OmsTypeInt4* >( &m_cur_val ); 
      *tgt++ = *reinterpret_cast< const OmsTypeInt4* >( m_data_cur ); 
      m_data_cur += sizeof( OmsTypeInt4 ); 
      *tgt = *reinterpret_cast< const OmsTypeInt4* >( m_data_cur ); 
      m_data_cur += sizeof( OmsTypeInt4 ); 
      break; 
    }
  }
};

//==========================================================================

/*!
 * Types that encapsulate compression and decompression for different algorithms. 
 */
template< int ClsGuid, OmsCompressionScheme scheme, class OmsArrayObjectBuffer >
class OmsCompressionTypes
{
}; 


/*!
 * Compressor/Decompressor for the 4 class compression scheme 0,4,8 Byte and repetition group
 */
template< int ClsGuid, class OmsArrayObjectBuffer >
class OmsCompressionTypes< ClsGuid, CA_LightWeight_0_4_8_Rep_Local_Buffer, OmsArrayObjectBuffer >
{
public: 
  typedef OmsArrayCompression< OmsTypeInt8, OmsCompressionBufferLocalIndex > Compressor; 
  typedef OmsCompressionLocalIndexIterator< OmsTypeInt8, ClsGuid, OmsArrayObjectBuffer > Iterator; 
};

//==========================================================================

/*! 
 * Implementation of persistent array object with internal data compression. 
 * Array object with compression always has invariant oid for external usage, 
 * even if the data size changes between different store operations. 
 * 
 * This implementation only supports data type OmsTypeInt8!
 * Using other data types will result in a compilation errors. 
 */
template< class T, int ClsGuid, OmsArraySplitPolicyEnum splitPolicy, class UserObject, OmsCompressionScheme scheme >
class OmsCompressionArrayImpl 
{
};

template< int ClsGuid, OmsArraySplitPolicyEnum splitPolicy, class UserObject, OmsCompressionScheme scheme >
class OmsCompressionArrayImpl< OmsTypeInt8, ClsGuid, splitPolicy, UserObject, scheme > : protected OmsProxyArrayImpl< OmsTypeByte, ClsGuid, splitPolicy, UserObject >, public OmsCompressionDataSink, public OmsTypeInt8CompressionBase
{
public: 
  typedef OmsTypeInt8 T; 
  typedef OmsCompressionArrayImpl< OmsTypeInt8, ClsGuid, splitPolicy, UserObject, scheme > Self; 
  typedef OmsProxyArrayImpl< OmsTypeByte, ClsGuid, splitPolicy, UserObject > ParentType; 
  typedef typename ParentType::ParentType DataImplType; 
  typedef typename ParentType::ParentType::OmsArrayObjectBuffer OmsArrayObjectBuffer; 
  typedef typename OmsCompressionTypes< ClsGuid, scheme, OmsArrayObjectBuffer >::Iterator OmsCompressionIterator; 
  typedef typename OmsCompressionTypes< ClsGuid, scheme, OmsArrayObjectBuffer >::Compressor Compressor; 
  typedef typename ParentType::OmsArrayProxyType OmsArrayProxyType; 
  typedef typename ParentType::OmsArrayProxyOid OmsCompressionArrayOid; 
  typedef typename ParentType::OmsArrayProxyOidIterator OmsCompressionArrayOidIterator; 
  typedef typename ParentType::ParentType::OmsArrayDataType OmsArrayDataType; 
  typedef typename ParentType::ParentType::OmsArrayDataOid OmsArrayDataOid; 

  /*!
   * Create a new compression array implementation for an existing array object. 
   * 
   * \param h OmsHandle
   * \param oid oid of existing array object
   */
  OmsCompressionArrayImpl( OmsHandle& h, const OmsCompressionArrayOid& oid )
    : ParentType( h, oid ) 
  { }

  /*!
   * Create a new compression array implementation for a new array object. 
   * The size is passed to the constructor, but can be changed later on by setting
   * the complete array data again by calling setData(). 
   * 
   * \param h OmsHandle
   * \param sh Schema Handle
   * \param cno container no
   */
  OmsCompressionArrayImpl( OmsHandle& h
                         , OmsSchemaHandle sh
                         , OmsContainerNo cno
                         , OmsTypeUInt4 n = 0
                         )
                         : ParentType( h, sh, cno, n, scheme )
  { }

  /*!
   * Returns an iterator, that is used for decompression of data chunks
   * 
   * \return iterator for data access
   */
  OmsCompressionIterator omsGetData()
  {
    return OmsCompressionIterator( this->getOmsHandle(), this->getBuffer(), this->m_const_proxy->getSize() ); 
  }

  /*!
   * Store data in this compressed array object. 
   *
   * \param input_data data to be stored
   * \param n number of elements
   * \param check_data indicates, if compression algorithm has to be checked
   */
  void omsSetData( OmsEnumerator< T >& input_data, OmsTypeUInt4 n, bool check_data )
  {
    if( this->isTracing() )
    {
      OmsCompressionArrayOid oid = this->m_const_proxy->omsGetOid(); 
      this->getOmsHandle().dbpTrace( "OmsCompressionArrayImpl::omsSetData(): size: %d, OID: %d.%d (vers %d)", n, oid.getPno(), oid.getPagePos(), oid.getGeneration() ); 
    }

    // get lock for proxy object
    OmsArrayProxyType* proxy = this->m_const_proxy->omsForUpdPtr( this->getOmsHandle() ); 

    OmsArrayObjectBuffer& buf( *( this->getBuffer().forUpdate() ) ); 
    buf.beforeFirst(); 
    // compress data and create chunks
    Compressor comp( this->getOmsHandle(), *this, n ); 
    comp.compress( input_data ); 
    // drop remaining chunks, that are not reused
    if( buf.next() )
    {
      buf.dropTail(); 
    }

    proxy->setSize( n ); 
    if( buf.getChunkCount() ) {
      buf.first(); 
      proxy->setOid( buf()->omsGetOid() ); 
    } else {
      // if no chunk was stored, oid to first chunk is nil
      proxy->setOid( OmsArrayDataOid() ); 
    }
    // store proxy
    proxy->omsStore( this->getOmsHandle() ); 

    // optionally check, if compression is working properly
    if( check_data )
    {
      // decompress data again in order to check compression algorithm
      input_data.reset(); 
      OmsCompressionIterator iter = omsGetData(); 
      int count = 0; 
      while( iter )
      {
        const T& v1 = iter.getValue(); 
        const T& v2 = input_data(); 
        if( v1 != v2 ) 
        {
          input_data.reset(); 
          // dump array into file, if error is found
          dumpArrayData( input_data ); 
          OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
        }
        ++iter; 
        ++input_data; 
        ++count; 
      }
      if( count != n )
      {
        // dump array into file, if error is found
        dumpArrayData( input_data ); 
        OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
      }
    }
  }

  /*!
   * Set element at the given index to a new value. 
   * THIS OPERATION IS NOT IMPLEMENTED FOR COMPRESSED ARRAYS!
   * 
   * \param idx index of element
   * \param val new value
   */
/*
  void omsSetAt( OmsTypeUInt4 idx, const T& val )
  { }
*/
  /*!
   * \return const reference to i-th array element
   * 
   * \param idx index to be returned
   */
  const T& omsGetAt( OmsTypeUInt4 idx ) const 
  {
    OmsCompressionIterator iter( this->getOmsHandle(), this->getBuffer(), this->m_const_proxy->getSize() ); 
    iter.absolute( idx ); 
    return iter.getValue(); 
  }

  void omsDelete() 
  {
    ParentType::omsDelete(); 
  }

  void omsRelease() 
  {
    ParentType::omsRelease(); 
  }

  /*!
   * Returns the oid of the compressed array. 
   * This oid is invariant, even if the number of element in the array changes. 
   *
   * \return oid of the compressed data array
   */
  OmsCompressionArrayOid omsGetOid() 
  {
    return ParentType::omsGetOid(); 
  }

  /*!
   * Returns the current size of this array object
   */
  OmsTypeUInt4 omsGetSize() 
  { 
    return ParentType::omsGetSize(); 
  }


  static bool omsIsEmpty( OmsHandle& h
                        , OmsSchemaHandle sh
                        , OmsContainerNo cno 
                        ) 
  {
    return ParentType::omsIsEmpty( h, sh, cno ); 
  }

  /*!
   * Returns an iterator for accessing all oids of the array objects
   * existing in the container identified by the given schema handle/container number. 
   */
  static OmsCompressionArrayOidIterator omsAllOids( OmsHandle& h
                                                  , OmsSchemaHandle sh
                                                  , OmsContainerNo cno 
                                                  )
  {
    return ParentType::omsAllOids( h, sh, cno ); 
  }

  /*!
   * Returns the number of bytes, that is used by object data
   */
  OmsTypeUInt8 omsGetUsedBytes() 
  {
    return ParentType::omsGetUsedBytes();
  }

  /*!
   * Returns the number of bytes, that is reserved by the array implementation
   * (used bytes + padding)
   */
  OmsTypeUInt8 omsGetReservedBytes() 
  {
    return ParentType::omsGetReservedBytes(); 
  }

  /*!
   * Returns the number of bytes, that is totally used
   * (used bytes + padding + oms internal admin data)
   */
  OmsTypeUInt8 omsGetBruttoBytes() 
  {
    return ParentType::omsGetBruttoBytes(); 
  }



  /*!
   * Re-initializes the server-side statistics file. 
   * 
   * \param h OmsHandle
   */
  static void omsClearStatistics( OmsHandle& h )
  {
    clearStatistics( h ); 
  }

  /*! 
   * Analyze array data and writes detailed statistics in a message file in the working
   * directory of the liveCache. 
   * 
   * \param h OmsHandle
   * \param sh Schema Handle
   * \param cno containerno
   */
  static void omsAnalyzeData( OmsHandle& h, OmsSchemaHandle sh , OmsContainerNo cno )
  {
    OmsArrayCompressionStatistics stats; 
    // populate statistic object
    getStatistics( h, sh, cno, stats ); 
    // print contents of statistic object to text file
    printStatistics( h, sh, cno, ClsGuid, stats ); 
  }

  /*!
   * Analyzes the data for this array object type and 
   * prints statistics to the given output stream. 
   * 
   * \param h OmsHandle for accessing liveCache
   * \param sh scheme of analyzed container
   * \param cno containerno of analyzed container
   * \param out output stream
   */
  template< class tout >
  static void omsAnalyzeData( OmsHandle& h, OmsSchemaHandle sh, OmsContainerNo cno, tout& out )
  {
    OmsArrayCompressionStatistics stats; 
    getStatistics( h, sh, cno, stats ); 

    // print statistics 
    out << "\n"; 
    out << "--- analyzing OmsArrayObjects OmsArrayTypeCompression ---" << "\n"; 
    out << " * type=OmsTypeInt8 guid=" << ClsGuid << " schema=" << sh << " container=" << cno << " * " << "\n"; 
    out << "\n"; 

    out << " total array count : " << stats.getArrayCount() << "\n"; 
    out << " total elem count  : " << stats.getElemCount() << "\n"; 

    out << "\n"; 
    out << " CC_Rep_Group : " << stats.getCompressionClassCount( CC_Rep_Group ) << "\n"; 
    out << " CC_0_Byte    : " << stats.getCompressionClassCount( CC_0_Byte ) << "\n"; 
    out << " CC_4_Byte    : " << stats.getCompressionClassCount( CC_4_Byte ) << "\n"; 
    out << " CC_8_Byte    : " << stats.getCompressionClassCount( CC_8_Byte ) << "\n"; 
    out << "\n"; 

    {
      for( OmsTypeUInt4 i=0; i < stats.getMaxStorageSizeCount(); ++i )
      {
        out << " " << i << " byte       : " << stats.getStorageSizeCount( i ) << "\n"; 
      }
    }
    out << "\n"; 
    out << " size (brutto): " << stats.getBruttoBytes() << "\n"; 
    out << " size (res)   : " << stats.getReservedBytes() << "\n"; 
    out << " size (netto) : " << stats.getNettoBytes() << "\n"; 
    OmsTypeUInt8 raw_bytes( stats.getElemCount() * sizeof( OmsTypeInt8 ) ); 
    out << " size (uncomp): " << raw_bytes << "\n"; 
    out << "\n";
    out << " netto compression ratio " << ( OmsTypeUInt8( stats.getNettoBytes() * 100 / raw_bytes ) ) << " % "<< "\n"; 
    out << "\n"; 
  }


  static void omsRegClass( OmsHandle& h )
  {
    Self::omsRegClass( h, "OmsArrayObjectCompression" ); 
  }

  static void omsRegClass( OmsHandle& h, const char* cl_name )
  {
    ParentType::omsRegClass( h, cl_name ); 
  }

  static void omsCreateContainer( OmsHandle& h, OmsSchemaHandle sh, OmsContainerNo cno )
  {
    ParentType::omsCreateContainer( h, sh, cno ); 
  }

  static bool omsCheckOid( OmsHandle& h, const OmsCompressionArrayOid& oid )
  {
    return ParentType::omsCheckOid( h, oid ); 
  }

  /*!
   * Return the size of the usable data part of a maximum-sized chunk
   * 
   * \return maximum chunk size
   */
  OmsTypeUInt4 getMaxChunkSize() 
  {
    return OMS_ARRAY_SPLIT_CHUNK_SIZES[ 0 ] - OmsArrayDataType::omsGetAdminDataSize(); 
  }

  /*!
   * Stores the data in the given transient data buffer in persistent data pages. 
   * Depending on the size of the buffer, the data is splitted and stored
   * in object chains consisting of different sized data containers in order to minimize page waste.  
   * 
   * \param data_ptr pointer to data buffer
   * \param data_size buffer size
   * 
   * \return pointer to first data buffer in page chain
   */
  OmsTypeByte* storeBuffer( const OmsTypeByte* data_ptr, OmsTypeUInt4 data_size ) 
  {
    if( this->isTracing() )
    {
      this->getOmsHandle().dbpTrace( "OmsCompressionArrayImpl::storeBuffer(): size: %d", data_size ); 
    }

    OmsArrayObjectBuffer& buf( *( this->getBuffer().forUpdate() ) ); 
    const OmsTypeByte* data_buf_ptr = data_ptr; 
    OmsTypeUInt4 remainder = data_size;
    OmsArrayDataType* new_chunk = 0; 
    while( remainder )
    {
      // add size for elem_count
      new_chunk = this->getTryReuseChunk( buf, remainder ); 
      // check, if this is first chunk
      OmsTypeUInt4 used_size = static_cast< OmsTypeUInt4 >( new_chunk->omsGetSize() ); 
      OmsArrayUtils::tmemcpy( (void*)( new_chunk->omsBegin() ), data_buf_ptr, used_size ); 
      data_buf_ptr += used_size; 
      remainder -= used_size; 
    }
    return new_chunk->omsBegin(); 
  }

  /*!
   * Returns a pointer to the next data buffer. 
   * If reusable chunks, that fit are available, they are used, 
   * otherwise new chunks are created
   * 
   * \param size_needed total size, that is still needed
   * \param best_size size of the returned chunk
   * 
   * \return pointer to the chunk
   */
  OmsTypeByte* getNextDataBuffer( OmsTypeUInt4 size_needed, OmsTypeUInt4& best_size )
  {
    if( this->isTracing() )
    {
      this->getOmsHandle().dbpTrace( "OmsCompressionArrayImpl::getNextDataBuffer(): size_needed: %d", size_needed ); 
    }
    // no buffer needed, if size == 0
    if( size_needed )
    {
      OmsArrayObjectBuffer& buf( *( this->getBuffer().forUpdate() ) ); 
      // get next data chunk, use reuse buffer, if possible
      OmsArrayDataType* new_chunk = this->getTryReuseChunk( buf, size_needed ); 
      // actual size of chunk is returned
      best_size = new_chunk->omsGetSize(); 

      return new_chunk->omsBegin(); 
    }
    return 0; 
  }

private: 

  /*!
   * Dump the current array data to a text file in the livecache working directory.  
   * 
   * \param input_data array data
   */
  void dumpArrayData( OmsEnumerator< OmsTypeInt8 >& input_data ) 
  {
    char buf1[20]; 
    char buf2[20];
    OmsHandle& h = this->getOmsHandle(); 
    OmsCompressionIterator iter = omsGetData(); 
    OmsTypeInt4 fhandle = h.dbpOpenMessageFile( "oms_array_dump.txt", 0, true ); 
    h.dbpWriteMessageFile( fhandle, "\n\n-------- array dump begin --------- (size=%d)\n\n", this->m_const_proxy->getSize() ); 
    h.dbpWriteMessageFile( fhandle, "             original         decompressed\n" ); 
    int count = 0; 
    for( OmsTypeInt4 i=0; iter; ++iter, ++input_data, ++i )
    {
      h.dbpWriteMessageFile( fhandle, "%6d: %s %s (%d)", i, OmsArrayUtils::convertToHex( buf1, input_data() ), OmsArrayUtils::convertToHex( buf2, iter.getValue() ), iter.getCompressionClass() ); 
      if( iter.getValue() != input_data() ) 
      {
        h.dbpWriteMessageFile( fhandle, " *** ERROR *** " ); 
      }
      h.dbpWriteMessageFile( fhandle, "\n" ); 
      count++; 
    }
    h.dbpCloseMessageFile( fhandle ); 
  }

  /*!
   * Populates a statistic object with statistic information for
   * the given container. 
   * 
   * \param h OmsHandle
   * \param sh Schema
   * \param cno containerno
   * \param stats statistic object ot be populated
   */
  static void getStatistics( OmsHandle& h
                           , OmsSchemaHandle& sh
                           , OmsContainerNo cno
                           , OmsArrayCompressionStatistics& stats 
                           )
  {
    OmsObjByClsIter< OmsArrayProxyType > iter = OmsArrayProxyType::omsAllOids( h, sh, cno ); 

    // build statistic 
    for( ; iter; ++iter )
    {
      Self obj( h, iter() ); 
      obj.analyze( stats ); 
    }
  }

  /*!
   * Analyze the this array object and add the information to the 
   * statistics object. 
   *
   * \param stats statistic object, the method adds the information to
   */
  void analyze( OmsArrayCompressionStatistics& stats ) 
  {
    stats.incArrayCount(); 
    // count data elements
    OmsCompressionIterator iter = omsGetData(); 
    for( ; iter; ++iter )
    {
      stats.addValue( iter.getCompressionClass(), iter.getValue() ); 
    }
    // add storage bytes
    stats.addNettoBytes( this->omsGetUsedBytes() ); 
    stats.addReservedBytes( this->omsGetReservedBytes() ); 
    stats.addBruttoBytes( this->omsGetBruttoBytes() ); 
  }
};

//========================================================================


inline OmsCompressionBufferLocalIndex::OmsCompressionBufferLocalIndex( OmsHandle& h, OmsCompressionDataSink& data_sink, OmsTypeUInt4 n ) 
: m_buffer( h, data_sink.getMaxChunkSize() ) // buffer size is one data page
, m_data_sink( data_sink )
, m_pos( 0 )
, m_size( 0 )
, m_index_ptr( 0 )
, m_data_ptr( 0 )
, m_last_elem( 0 )
, m_last_copy_count( 0 )
, m_h( h )
{ 
  m_data_ptr = reinterpret_cast< OmsTypeInt4* >( m_buffer.getBuffer() ); 
  // index_ptr = buffer_end - sizeof( index_elem ) - var_size( elem count in buffer ) 
  m_index_ptr = reinterpret_cast< OmsTypeUInt4* >( m_buffer.getBuffer() + m_buffer.getSize() - sizeof( OmsTypeUInt4 ) ); 
  // zero initialize first index entry
  *m_index_ptr = 0; 
}

inline void OmsCompressionBufferLocalIndex::appendRepGroup( OmsTypeInt8 val )
{
  appendCompressionClass( CC_Rep_Group, 0 ); 
  // switch occured while adding rep group?
  if( m_size == 1 )
  {
    // erase 'false' rep-group entry in index
    m_size = 0; 
    // set index one entry back, because it will be set forward again, when m_size is 0
    ++m_index_ptr; 
    // first element in buffer must not be a repetition group
    // if it is, the data chunk is not self contained because the value
    // needed for data extraction is stored in the previous chunk
    // -> in this case do not store the value as repetition but store the value itself
    if( is4ByteValue( val ) )
    {
      append4Byte( OmsTypeInt4( val ) ); 
    } else {
      append8Byte( val ); 
    }
  }
}

inline void OmsCompressionBufferLocalIndex::append0Byte() 
{
  // 0-Byte values have no data, append only index entry
  appendCompressionClass( CC_0_Byte, 0 ); 
}

inline void OmsCompressionBufferLocalIndex::append4Byte( OmsTypeInt4 val ) 
{
  appendCompressionClass( CC_4_Byte, 1 ); 
  *m_data_ptr++ = val; 
}

inline void OmsCompressionBufferLocalIndex::append8Byte( OmsTypeInt8 val ) 
{
  appendCompressionClass( CC_8_Byte, 2 ); 
  OmsTypeInt4* src = reinterpret_cast< OmsTypeInt4* >( &val ); 
  *m_data_ptr++ = *src++; 
  *m_data_ptr++ = *src; 
}

inline OmsTypeByte* OmsCompressionBufferLocalIndex::getBuffer() 
{
  return m_buffer.getBuffer(); 
}

#endif // OMS_ARRAY_OBJECT_COMPRESSION_HPP