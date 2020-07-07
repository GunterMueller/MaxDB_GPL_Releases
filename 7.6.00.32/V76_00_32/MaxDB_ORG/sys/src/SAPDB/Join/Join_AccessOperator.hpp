#ifndef JOIN_ACCESSOPERATOR_HPP
#define JOIN_ACCESSOPERATOR_HPP
/*!
 * @file
 * @brief realize one table access operator by means of Join_AccessDesc
 *
 * @author GertG
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

class SAPDBMem_IRawAllocator; // forward declaration
class SQLMan_Context;

//! operator for base table access via key or index
class Join_AccessOperator : public Join_IAccessOperator {
public:
    //! @name stream manipulation
    //@{
    //! open operator with keys given by strategy information
    virtual tgg00_BasisError Open();

    //! open stream operator and limit stream with start/stop key
    virtual tgg00_BasisError Open(
            const Join_TwoKeys& startkeys,
            const Join_TwoKeys& stopkeys );

    //! get next record from stream
    virtual tgg00_BasisError Next( tgg00_Rec*& record );

    //! close operator
    virtual void Close();
    //@}

    //! @name record stream properties
    //@{
    //! return key length of records in stream
    SAPDB_UInt2 GetKeyLength() const { return m_AccessDesc.GetKeyLength(); }

    //! return length of records in stream
    SAPDB_UInt2 GetRecordLength() const { return m_AccessDesc.GetRecordLength(); }
    //@}

    //! Returns true if the whole result set fit into the record buffer.
    virtual SAPDB_Bool  IsResultFullyBuffered() const;

    /*! Returns the number of records in the result set, if _all_
     *  records fit into the record buffer. Returns 0 otherwise.
     */
    virtual SAPDB_UInt4 GetBufferedRecordCount() const;
    
    //! destructor
    virtual ~Join_AccessOperator();

protected:
    // member methods
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_AccessOperator(
        SQLMan_Context        &acv,
        const SAPDB_UInt2     tabno,
        const bool            compressed_buf_usage,
        SAPDB_Byte            *bufferPtr  = 0,
        const SAPDB_UInt4&    buffersize = BUF8K_MXSP00 );

    //@}

    //! returns true if buffer is empty
    bool is_empty_buffer() const { return 0 == this->result_cnt(); }
    //! returns true if input keys actual used
    bool is_same_key( const Join_TwoKeys&, const Join_TwoKeys& ) const;
    //! check if we could reuse filled buffer
    void check_buffer_reusage( const Join_TwoKeys&, const Join_TwoKeys& );

    //! fill buffer with records described in Open(), Open(start,stop)
    virtual void SelectRecords() = 0;
    //! reset operator state
    virtual tgg00_BasisError reset();

    // member variables
    SAPDBMem_IRawAllocator& m_allocator;
    Join_TwoKeys            m_Actualkeys;
    void*                   m_RecBuf;
    SAPDB_UInt4             m_RecBufSize;
    SAPDB_Byte*             m_Record;
    bool                    m_CompressedBufferUsage;

private:

    virtual SAPDB_Int4& result_cnt() =0;
    virtual SAPDB_Int4 result_cnt() const =0;
    virtual SAPDB_Int4 rec_offset() const =0;
    
    // member variables
    tgg00_BasisError        m_BDState;
    SAPDB_Int4              m_AllRecsInBuf;
    SAPDB_Bool              m_foreignRecBuf;
    SAPDB_Bool              m_FirstBufferFill;
    SAPDB_Bool              m_BufferReusagePossible;
};

#endif
