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

#include "Oms/OMS_ArrayObjectCompression.hpp"

#define OMS_ARRAY_STATISTIC_FILENAME "omsarray_stat.txt"

const OmsTypeChar OMS_ARRAY_COMPRESSION_OFFSET_LOOKUP_TABLE[] =
{
  0,
    4,
    8,
    0,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    0,
    4,
    8,
    0,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    16,
    20,
    24,
    16,
    8,
    12,
    16,
    8,
    0,
    4,
    8,
    0,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    0,
    4,
    8,
    0,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    16,
    20,
    24,
    16,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    16,
    20,
    24,
    16,
    20,
    24,
    28,
    20,
    12,
    16,
    20,
    12,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    16,
    20,
    24,
    16,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    16,
    20,
    24,
    16,
    20,
    24,
    28,
    20,
    12,
    16,
    20,
    12,
    16,
    20,
    24,
    16,
    20,
    24,
    28,
    20,
    24,
    28,
    32,
    24,
    16,
    20,
    24,
    16,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    16,
    20,
    24,
    16,
    8,
    12,
    16,
    8,
    0,
    4,
    8,
    0,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    0,
    4,
    8,
    0,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    12,
    16,
    20,
    12,
    16,
    20,
    24,
    16,
    8,
    12,
    16,
    8,
    0,
    4,
    8,
    0,
    4,
    8,
    12,
    4,
    8,
    12,
    16,
    8,
    0,
    4,
    8
}; 

const OmsTypeChar OMS_ARRAY_COMPRESSION_CLASSES[] =
{
  /* 00 */ 0, // zero 
  /* 01 */ 4, // integer, that fits in 4 byte
  /* 10 */ 8, // integer, that fits in 8 byte
  /* 11 */ 0  // repeating group
}; 

/*======================================================================*/

OmsDataBuffer::OmsDataBuffer( OmsHandle& h, OmsTypeUInt4 size )
: OmsSessionObject( h )
, m_h( h )
, m_size( size )
{ 
  // allocate memory for buffer
  m_buffer = reinterpret_cast< OmsTypeByte* >( this->getAllocator().omsAllocate( size ) ); 
}

OmsDataBuffer::~OmsDataBuffer() 
{
  // release memory; exception safe
  try {
    this->getAllocator().omsDeallocate( (void*)m_buffer ); 
  } catch( DbpError& err )
  {   
    this->m_h.dbpOpError( "DbpError caught in OmsDataBuffer::~OmsDataBuffer() code: %d, text: ", err.m_errorNo, err.m_errorText ); 
    throw err; 
  }
}

//========================================================================

void OmsTypeInt8CompressionBase::clearStatistics( OmsHandle& h )
{
  OmsTypeInt4 fh = h.dbpOpenMessageFile( OMS_ARRAY_STATISTIC_FILENAME, 0, false ); 
  h.dbpWriteMessageFile( fh, "***** array statistics ***** \n\n" ); 
  h.dbpCloseMessageFile( fh ); 
}

void OmsTypeInt8CompressionBase::printStatistics( OmsHandle& h
                                                , OmsSchemaHandle sh
                                                , OmsContainerNo cno
                                                , OmsTypeInt4 clsGuid
                                                , OmsArrayCompressionStatistics& stats  
                                                )
{
  OmsTypeInt4 fh = h.dbpOpenMessageFile( OMS_ARRAY_STATISTIC_FILENAME, 0, true ); 

  // print statistics 
  h.dbpWriteMessageFile( fh, "\n--- analyzing OmsArrayObjects OmsArrayTypeCompression ---\n", clsGuid ); 
  h.dbpWriteMessageFile( fh, " * type=OmsTypeInt8 guid=%d schema=%d, container=%d *\n\n", clsGuid, sh, cno );

  h.dbpWriteMessageFile( fh, " total array count : " ); 
  OmsArrayUtils::writeInt8( h, fh, stats.getArrayCount() ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " total elem count  : " ); 
    OmsArrayUtils::writeInt8( h, fh, stats.getElemCount() ); 
  h.dbpWriteMessageFile( fh, "\n" ); 

  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " CC_Rep_Group : " ); 
  OmsArrayUtils::writeInt8( h, fh, stats.getCompressionClassCount( CC_Rep_Group ) ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " CC_0_Byte    : " ); 
  OmsArrayUtils::writeInt8( h, fh, stats.getCompressionClassCount( CC_0_Byte ) ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " CC_4_Byte    : " ); 
  OmsArrayUtils::writeInt8( h, fh, stats.getCompressionClassCount( CC_4_Byte ) ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " CC_8_Byte    : " ); 
  OmsArrayUtils::writeInt8( h, fh, stats.getCompressionClassCount( CC_8_Byte ) ); 
  h.dbpWriteMessageFile( fh, "\n" ); 

  h.dbpWriteMessageFile( fh, "\n" ); 
  {
    for( OmsTypeUInt4 i=0; i < stats.getMaxStorageSizeCount(); ++i )
    {
      h.dbpWriteMessageFile( fh, "  %d byte      : ", i ); 
      OmsArrayUtils::writeInt8( h, fh, static_cast< OmsTypeInt8 >( stats.getStorageSizeCount( i ) ) ); 
      h.dbpWriteMessageFile( fh, "\n" ); 
    }
  }
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " size (brutto): " ); 
  OmsArrayUtils::writeInt8( h, fh, stats.getBruttoBytes() ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " size (res)   : " ); 
  OmsArrayUtils::writeInt8( h, fh, stats.getReservedBytes() ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " size (netto) : " ); 
  OmsArrayUtils::writeInt8( h, fh, stats.getNettoBytes() ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " size (uncomp): " ); 
  OmsTypeInt8 raw_bytes( stats.getElemCount() * sizeof( OmsTypeInt8 ) );
  OmsArrayUtils::writeInt8( h, fh, raw_bytes ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  h.dbpWriteMessageFile( fh, " netto compression ratio  " ); 
  OmsArrayUtils::writeInt8( h, fh, ( OmsTypeInt8( stats.getNettoBytes() * 100 / raw_bytes ) )  ); 
  h.dbpWriteMessageFile( fh, " % " ); 
  h.dbpWriteMessageFile( fh, "\n" ); 
  // close output file
  h.dbpCloseMessageFile( fh );  
}

size_t OmsTypeInt8CompressionBase::getOffsetFromIndexValue( OmsTypeUInt4 ind_val, short start, short end )
{
  size_t off_total = 0; 
  // skip first "start" index entries
  ind_val = ind_val >> ( OMS_ARRAY_COMPRESSION_BITS_PER_CLASS * start ); 
  short count = end - start; 
  OmsTypeInt4 ind_mask( OMS_ARRAY_BYTE_MASK ); 
  if( ind_val )
  {
    do {
      switch( count )
      {
      case 0: 
        ind_mask = 0; break; 
      case 1: 
        ind_mask = 3; break; 
      case 2: 
        ind_mask = 15; break; 
      case 3: 
        ind_mask = 63; break; 
      }
      off_total += OMS_ARRAY_COMPRESSION_OFFSET_LOOKUP_TABLE[ ind_val & ind_mask ]; 
      // get next byte
      ind_val = ind_val >> OMS_ARRAY_BITS_PER_BYTE; 
      count-= OMS_ARRAY_COMPRESSION_ENTRIES_PER_BYTE; 
    } while( count > 0 ); 
  }
  return off_total; 
}

size_t OmsTypeInt8CompressionBase::getOffset( const OmsTypeUInt4* buf, size_t count, size_t diff, OmsTypeUInt4& last_ind )
{
  size_t off=0; 
  for( size_t i=0; i < count; ++i )
  {
    size_t tmp_off = getOffsetFromIndexValue( *buf ); 
    if( tmp_off )
    {
      last_ind = *buf; 
    }
    off += tmp_off; 
    buf += diff; 
  }
  return off; 
}

//========================================================================

void OmsCompressionBufferLocalIndex::flush() 
{
  bool tracing( isTracing() ); 
  if( tracing )
  {
    m_h.dbpTrace( "OmsCompressionBufferLocalIndex::flush()" ); 
  }

  OmsTypeByte* data_start_ptr = m_buffer.getBuffer(); 
  OmsTypeByte* index_start_ptr = m_buffer.getBuffer() + m_buffer.getSize() - sizeof( OmsTypeUInt4 ) - sizeof( OmsTypeUInt4 ); 
  OmsTypeUInt4 best_size; 
  OmsTypeUInt4 elem_count( 0 ); 
  OmsCompressionClassEnum last_non_rep_comp_class( CC_0_Byte ); 
  do {
    // check, if current element is a repetition group
    // if it is, search buffer for last non-repetition group and 
    // set first element in buffer to that element to make data chunks self contained

    if( ( *(reinterpret_cast< OmsTypeUInt4* >( index_start_ptr ) ) & OMS_ARRAY_COMPRESSION_INDEX_ENTRY_MASK ) == CC_Rep_Group )
    {
      // rep group has value 0, so simple or operation is good enough for adjustment
      *reinterpret_cast< OmsTypeUInt4* >( index_start_ptr ) |= last_non_rep_comp_class; 
      switch( last_non_rep_comp_class )
      {
      case CC_4_Byte:
        data_start_ptr -= sizeof( OmsTypeInt4 ); 
        break; 
      case CC_8_Byte:
        data_start_ptr -= sizeof( OmsTypeInt8 ); 
        break; 
      }
      if( tracing )
      {
        m_h.dbpTrace( "OmsCompressionBufferLocalIndex::flush() CC_Rep_Group as First Chunk elem -> Adjust Data Start Pointer " ); 
        m_h.dbpTrace( " + non_rep_CC: %d value: %d first_ind: %d", last_non_rep_comp_class, (*(reinterpret_cast< OmsTypeUInt4* >( data_start_ptr ))), *(reinterpret_cast< OmsTypeUInt4* >( index_start_ptr ) )); 
      }

    }
    OmsTypeUInt4 data_size = OmsTypeUInt4( reinterpret_cast< OmsTypeByte* >( m_data_ptr ) - data_start_ptr ); 
    OmsTypeUInt4 index_size = OmsTypeUInt4( index_start_ptr - reinterpret_cast< OmsTypeByte* >( m_index_ptr ) + sizeof( OmsTypeUInt4 ) ); 
    OmsTypeUInt4 needed_size = data_size + index_size + sizeof( OmsTypeUInt4 ); 
    OmsTypeByte* buf = m_data_sink.getNextDataBuffer( needed_size, best_size ); 

    if( tracing )
    {
      m_h.dbpTrace( "OmsCompressionBufferLocalIndex::flush() New Buffer " ); 
      m_h.dbpTrace( " + dat_sz: %d; ind_sz %d; needed_size: %d; bst_sz: %d; non_rep_CC: %d; rem_elems: %d )", data_size, index_size, needed_size, best_size, last_non_rep_comp_class, m_size ); 
    }
    if( best_size < needed_size )
    {

      OmsTypeUInt4 offset; 
      OmsTypeByte* data_buf_res;
      OmsTypeByte* index_buf_res; 

      // search for best split point in data
      searchSplitPointForward( best_size, data_start_ptr, index_start_ptr, elem_count, data_buf_res, index_buf_res, offset ); 
      // copy buffer data to chunk (m_size is changed internally)
      copyBufferData( elem_count
                    , buf
                    , best_size
                    , data_start_ptr
                    , OmsTypeUInt4( data_buf_res - data_start_ptr ) 
                    , index_buf_res
                    , OmsTypeUInt4( ( index_start_ptr - index_buf_res ) + sizeof( OmsTypeUInt4 ) )
                    ); 

      last_non_rep_comp_class = getLastNonRepCompClass( index_buf_res, offset ); 

      // adapt index entries for next run 
      // they are moved down in the index value, so that the first entry
      // for the next chunk starts at the beginning of a new index value
      adjustIndexBuffer( index_buf_res, reinterpret_cast< OmsTypeByte* >( m_index_ptr ), offset ); 
      data_start_ptr = data_buf_res; 
      index_start_ptr = index_buf_res; 
    } else {
      // copy rest of buffer to chunk
      copyBufferData( m_size, buf, best_size, data_start_ptr, data_size, reinterpret_cast< OmsTypeByte* >( m_index_ptr ), index_size ); 
    }
  } while( m_size ); 
}

OmsTypeInt8CompressionBase::OmsCompressionClassEnum OmsCompressionBufferLocalIndex::getLastNonRepCompClass( OmsTypeByte* index_buf_start, OmsTypeUInt4 offset )
{
  OmsTypeUInt4* index_buf = reinterpret_cast< OmsTypeUInt4* >( index_buf_start ); 
  // byte-position in index value is offset / 4 (index value has 4 bytes)
  if( offset )
  {
    --offset; 
  } else {
    ++index_buf; 
    offset = ( OMS_ARRAY_COMPRESSION_ENTRIES_PER_INDEX_VALUE - 1 ); 
  }
  OmsTypeInt4 byte_pos( offset >> 2 ); 
  // index in byte is offset modulo 4 ( mask first 2 bits is equal mod 4 )
  OmsTypeInt4 byte_index( offset & 3 ); 
  {
    while( true )
    {
      while ( byte_pos >= 0 ) {
        // get highest byte in current index value
        OmsTypeByte ind_byte = ( *index_buf >> byte_pos * OMS_ARRAY_BITS_PER_BYTE ) & OMS_ARRAY_BYTE_MASK; 
        while( byte_index >= 0 ) {
          if( ( ind_byte >> ( byte_index * OMS_ARRAY_COMPRESSION_BITS_PER_CLASS ) ) 
            & OMS_ARRAY_COMPRESSION_INDEX_ENTRY_MASK 
            )
          {
            return OmsCompressionClassEnum( ( ind_byte >> ( byte_index * OMS_ARRAY_COMPRESSION_BITS_PER_CLASS ) ) & OMS_ARRAY_COMPRESSION_INDEX_ENTRY_MASK ); 
          }
          --byte_index; 
        }
        byte_index = ( OMS_ARRAY_COMPRESSION_ENTRIES_PER_BYTE - 1 ); 
        --byte_pos; 
      }
      byte_pos = 3; 
      ++index_buf; 
    }
  }
  // should never happen
  return CC_0_Byte; 
}

void OmsCompressionBufferLocalIndex::adjustIndexBuffer( OmsTypeByte* cur_index
                                                      , OmsTypeByte* stop_index_ptr
                                                      , OmsTypeUInt4 offset 
                                                      )
{
  // adjustment only necessary, if offset > 0
  if( offset )
  {
    // all entries have to be moved down, so that the first
    // entry starts at index 0 after the operation 

    // number of bits shifts down for values above offset in in same index value
    size_t shift = offset * OMS_ARRAY_COMPRESSION_BITS_PER_CLASS; 

    // number of bit shifts down up to fill gap at end of current index value
    // with bits from next value
    size_t inverse_shift = ( OMS_ARRAY_COMPRESSION_ENTRIES_PER_INDEX_VALUE - offset ) * OMS_ARRAY_COMPRESSION_BITS_PER_CLASS; 

    // example for shift (to make it simpler only 16 bit values are used here)
    //         |    val i-1    | |      val i     |    offset = 3
    // before: 10010101011100111 001111010011010100 
    // after : 00000010010101011 100111001111010011 010100 (shifted out; not needed)
    //         bits shifted in from val-1 <-||-> bits shifted 'shift' down in same value
    //         they are shifted up 
    //         "inverse_shift" and OR-ed

    OmsTypeUInt4* cur_index_ptr = reinterpret_cast< OmsTypeUInt4* >( cur_index ); 
    while( cur_index_ptr > reinterpret_cast< OmsTypeUInt4* >( stop_index_ptr ) )
    {
      *cur_index_ptr = ( *cur_index_ptr >> shift ); 
      OmsTypeUInt4* tmp_index_ptr = cur_index_ptr; 
      --cur_index_ptr; 
      *tmp_index_ptr = ( *tmp_index_ptr ) | ( *cur_index_ptr << inverse_shift ); 
    }
    // last index has no upper values
    *cur_index_ptr = ( *cur_index_ptr >> shift ); 
  }
}

// prelimery! not finished yet 
void OmsCompressionBufferLocalIndex::searchSplitPointBackwards( OmsTypeUInt4 best_size
                                                              , OmsTypeUInt4 needed_size
                                                              , OmsTypeUInt4 elem_count
                                                              , OmsTypeByte* data_buf_start
                                                              , OmsTypeByte* index_buf_start 
                                                              , OmsTypeUInt4& elem_count_res
                                                              , OmsTypeByte*& data_buf_res
                                                              , OmsTypeByte*& index_buf_res
                                                              )
{
  OmsTypeUInt4* index_buf = reinterpret_cast< OmsTypeUInt4* >( index_buf_start ); 
  OmsTypeByte* data_buf = data_buf_start; 
  OmsTypeUInt4 count = 3; 
  OmsTypeUInt4 cur_index; 
  cur_index = *index_buf; 
  while( needed_size > best_size )
  {
    // get index_byte
    OmsTypeUInt4 ind = ( cur_index >> OMS_ARRAY_BITS_PER_BYTE * count ) & OMS_ARRAY_BYTE_MASK; 
    // get offset for byte
    OmsTypeUInt4 off = OMS_ARRAY_COMPRESSION_OFFSET_LOOKUP_TABLE[ ind ]; 
    --count; 
    if( ! count ) 
    {
      ++index_buf; 
      cur_index = *index_buf; 
      count = 3; 
      needed_size -= sizeof( OmsTypeUInt4 ); 
    }
    // split point found? 
    if( best_size > ( needed_size - off ) )
    {
      // search detail split
      OmsTypeUInt4 pos = 0; 
      OmsTypeUInt4 new_off = off; 
      OmsTypeUInt4 new_ind = ind; 
      OmsTypeUInt4 last_ind; 
      OmsTypeUInt4 last_off; 
      do {
        last_ind = new_ind; 
        last_off = new_off; 
        // mask index entry 
        new_ind = new_ind & ( OmsTypeUInt4( OMS_ARRAY_COMPRESSION_INDEX_ENTRY_MASK ) << pos );
        new_off = OMS_ARRAY_COMPRESSION_OFFSET_LOOKUP_TABLE[ new_ind ]; 
        ++pos; 
      } while( best_size > ( needed_size - new_off ) ); 
      data_buf -= new_off; 
      elem_count_res; 
    } else {
      needed_size -= off; 
      data_buf -= off; 
    }
  }
}

void OmsCompressionBufferLocalIndex::searchSplitPointForward( OmsTypeUInt4 best_size
                                                            , OmsTypeByte* data_buf_start
                                                            , OmsTypeByte* index_buf_start 
                                                            , OmsTypeUInt4& elem_count_res
                                                            , OmsTypeByte*& data_buf_res
                                                            , OmsTypeByte*& index_buf_res
                                                            , OmsTypeUInt4& offset
                                                            )
{
  OmsTypeUInt4* index_buf = reinterpret_cast< OmsTypeUInt4* >( index_buf_start ); 
  OmsTypeByte* data_buf = data_buf_start; 
  OmsTypeUInt4 remainder = best_size - sizeof( OmsTypeUInt4 ) - sizeof( OmsTypeUInt4 ); 
  OmsTypeUInt4 byte_pos = 0; 
  elem_count_res = 0; 
  while( remainder >= 0 )
  {
    // get index_byte
    OmsTypeUInt4 ind = ( *index_buf >> OMS_ARRAY_BITS_PER_BYTE * byte_pos ) & OMS_ARRAY_BYTE_MASK; 
    // get offset for index byte
    OmsTypeUInt4 off = OMS_ARRAY_COMPRESSION_OFFSET_LOOKUP_TABLE[ ind ];

    if( off >= remainder )
    {
      if( off > remainder )
      {
        OmsTypeUInt4 i( 1 ); 
        for( ; i < OMS_ARRAY_COMPRESSION_ENTRIES_PER_BYTE; ++i )
        {
          OmsTypeUInt4 new_ind = ind << i * OMS_ARRAY_COMPRESSION_BITS_PER_CLASS;
          off = OMS_ARRAY_COMPRESSION_OFFSET_LOOKUP_TABLE[ ( new_ind & OMS_ARRAY_BYTE_MASK ) ];
          if( off <= remainder )
          {              
            data_buf += off; 
            break; 
          }
        }
        // no element in this byte is used 

        offset = OmsTypeUInt4( byte_pos * OMS_ARRAY_COMPRESSION_ENTRIES_PER_BYTE + ( OMS_ARRAY_COMPRESSION_ENTRIES_PER_BYTE - i ) ); 
        elem_count_res = OmsTypeUInt4( OMS_ARRAY_COMPRESSION_ENTRIES_PER_INDEX_VALUE * ( reinterpret_cast< OmsTypeUInt4* >( index_buf_start ) - index_buf ) + offset ); 
        data_buf_res = data_buf; 
        index_buf_res = reinterpret_cast< OmsTypeByte* >( index_buf ); 

        return; 
      } else {

        // return result data
        offset = byte_pos * OMS_ARRAY_COMPRESSION_ENTRIES_PER_BYTE; 
        elem_count_res = OmsTypeUInt4( OMS_ARRAY_COMPRESSION_ENTRIES_PER_INDEX_VALUE * ( reinterpret_cast< OmsTypeUInt4* >( index_buf_start ) - index_buf ) + offset ); 
        data_buf_res = data_buf; 
        index_buf_res = reinterpret_cast< OmsTypeByte* >( index_buf ); 

        return; 
      }
    } else {
      remainder -= off; 
      data_buf += off; 
    }

    // byte three is the last one in the index value
    if( byte_pos == ( sizeof( OmsTypeUInt4 ) - 1 ) )
    {
      // next index value
      byte_pos = 0; 
      --index_buf; 
      remainder -= sizeof( OmsTypeUInt4 ); 
    } else {
      // next byte in current index value
      ++byte_pos; 
    }
  }
}

void OmsCompressionBufferLocalIndex::copyBufferData( OmsTypeUInt4 elem_count
                                                   , OmsTypeByte* tgt
                                                   , OmsTypeUInt4 tgt_size
                                                   , OmsTypeByte* data_buf
                                                   , OmsTypeUInt4 data_size
                                                   , OmsTypeByte* index_buf
                                                   , OmsTypeUInt4 index_size 
                                                   )
{
  OmsTypeUInt4 mod_tgt_size; 
  mod_tgt_size = tgt_size - sizeof( OmsTypeUInt4 ); 
  // store elem_count at very end of chunk
  OmsTypeUInt4* size_ptr = reinterpret_cast< OmsTypeUInt4* >( tgt + mod_tgt_size ); 
  *size_ptr = elem_count; 
  // store compression data at beginning of chunk
  OmsArrayUtils::tmemcpy( tgt, data_buf, data_size );
  // store compression index at end of chunk
  OmsArrayUtils::tmemcpy( tgt + mod_tgt_size - index_size, index_buf, index_size );
  m_size -= elem_count; 
  m_pos += elem_count; 
}

void OmsCompressionBufferLocalIndex::appendCompressionClass( OmsCompressionClassEnum cl, size_t buf_elem_needed ) 
{
  // 16 index values fit in a 4 byte integer 
  // -> the current index element offset is the element index modulo 16
  OmsTypeInt4 bit_off = OmsTypeInt4( m_size & ( OMS_ARRAY_COMPRESSION_ENTRIES_PER_INDEX_VALUE - 1 ) ); 

  // check, if current integer is already completely filled
  if( ! bit_off )
  {
    // next index elem
    --m_index_ptr; 
    // check, if index buffer is full and also data element fits in buffer
    if( m_data_ptr + buf_elem_needed > reinterpret_cast< OmsTypeInt4* >( m_index_ptr ) ) 
    {
      // current buffer is full, switch to new buffer
      switchBuffer(); 
      bit_off = 0; 
    } else {
      *m_index_ptr = 0; 
    }
  } else {
    // check, if index buffer is full and also data element fits in buffer
    if( m_data_ptr + buf_elem_needed > reinterpret_cast< OmsTypeInt4* >( m_index_ptr ) ) 
    {
      // current buffer is full, switch to new buffer
      switchBuffer(); 
      bit_off = 0; 
    }
  }

  // set index bits for current element
  *m_index_ptr = *m_index_ptr | ( cl << ( OMS_ARRAY_COMPRESSION_BITS_PER_CLASS * bit_off ) ); 
  ++m_size; 
}


void OmsCompressionBufferLocalIndex::switchBuffer()
{
  if( this->isTracing() )
  {
    m_h.dbpTrace( "OmsCompressionBufferLocalIndex::switchBuffer()" ); 
  }

  // store data buffer
  // write elem count to buffer
  OmsTypeUInt4* size_ptr = reinterpret_cast< OmsTypeUInt4* >( m_buffer.getBuffer() + m_buffer.getSize() - sizeof( OmsTypeUInt4 ) ); 
  *size_ptr = m_size; 
  OmsTypeInt4* storage_buf = reinterpret_cast< OmsTypeInt4* >( m_data_sink.storeBuffer( m_buffer.getBuffer(), m_buffer.getSize() ) ); 
  // reinitialize pointers
  // data pointer is at buffer start
  m_data_ptr = reinterpret_cast< OmsTypeInt4* >( m_buffer.getBuffer() ); 
  // index starts at buffer end 
  m_index_ptr = reinterpret_cast< OmsTypeUInt4* >( m_buffer.getBuffer() + m_buffer.getSize() - sizeof( OmsTypeUInt4 ) - sizeof( OmsTypeUInt4 ) ); 

  // zero initialize current index value
  *m_index_ptr = 0; 
  m_pos += m_size; 
  m_size = 0; 
}

//==========================================================================

OmsArrayCompressionStatistics::OmsArrayCompressionStatistics() 
: m_netto_bytes( 0 )
, m_brutto_bytes( 0 )
, m_reserved_bytes( 0 )
, m_array_count( 0 )
{
  // init all counters with 0 
  {
    for( size_t i=0; i < OMS_ARRAY_COMPRESSION_MAX_BYTE_COUNT; ++i )
    {
      m_bytes[ i ] = 0; 
    }
  }
  {
    for( size_t i=0; i < OMS_ARRAY_COMPRESSION_CLASS_COUNT; ++i )
    {
      m_cl[ i ] = 0; 
    }
  }
}

void OmsArrayCompressionStatistics::addValue( OmsCompressionClassEnum cl, OmsTypeInt8 val )
{
  incCompressionClass( cl ); 
  switch( cl )
  {
  case CC_0_Byte:
    incStorageSize( 0 ); 
    return; 
  }

  // make value positive in order to use bit shift
  OmsTypeUInt8 tmp_val( 0 ); 
  if( val < 0 )
    tmp_val = -val; 
  else
    tmp_val = val; 

  // search byte count
  OmsTypeUInt4 st_size( 0 ); 
  do {
    ++st_size; 
    tmp_val = tmp_val >> OMS_ARRAY_BITS_PER_BYTE; 
  } while( tmp_val ); 

  incStorageSize( st_size ); 
}
