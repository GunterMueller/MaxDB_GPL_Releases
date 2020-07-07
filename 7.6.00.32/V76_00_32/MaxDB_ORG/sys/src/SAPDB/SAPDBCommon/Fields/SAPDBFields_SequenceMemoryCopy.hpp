/*!*********************************************************************

    @file         SAPDBFields_SequenceMemoryCopy.hpp

    @author       DanielD

    @brief        copy a FieldSequence to memory as compact as possible

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#ifndef SAPDBFIELDS_SEQUENCEMEMORYCOPY_HPP
#define SAPDBFIELDS_SEQUENCEMEMORYCOPY_HPP

#include "SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp"
#include "SAPDBCommon/SAPDB_IPersistenceStream.hpp"
#include "Messages/Msg_List.hpp"

class SAPDBMem_IRawAllocator;
class SAPDB_IPersistenceStream;
/*!

  @class SAPDBFields_SequenceMemoryCopy

 */
class SAPDBFields_SequenceMemoryCopy : public SAPDBFields_MemorySequence
{
public:
    SAPDBFields_SequenceMemoryCopy (
                        SAPDBMem_IRawAllocator     & allocator,
                        SAPDBFields_IFieldSequence * source,
                        SAPDBFields_MemorySequence::RefcountMode refMode);

    static SAPDBFields_SequenceMemoryCopy * Create (
        SAPDBMem_IRawAllocator     & allocator,
        SAPDBFields_IFieldSequence * source,
        SAPDBFields_MemorySequence::RefcountMode refMode,
        Msg_List                   & msgList);

    SAPDBFields_SequenceMemoryCopy (
                        SAPDBMem_IRawAllocator   & allocator,
                        SAPDB_IPersistenceStream & instream,
                        int                        fieldCount,
                        int                        allDataLength,
                        SAPDBFields_MemorySequence::RefcountMode refMode,
                        Msg_List                 & msgList);

    static SAPDBFields_SequenceMemoryCopy * Create (
        SAPDBMem_IRawAllocator   & allocator,
        SAPDB_IPersistenceStream & instream,
        int                        fieldCount,
        int                        allDataLength,
        SAPDBFields_MemorySequence::RefcountMode refMode,
        Msg_List                 & msgList);

    virtual ~SAPDBFields_SequenceMemoryCopy ();

protected:
    /*!
        @brief return the allocator needed to destroy the object
    */
    virtual SAPDBMem_IRawAllocator * GetAllocator () const;

private:
    SAPDBFields_SequenceMemoryCopy (
        SAPDBFields_MemorySequence::RefcountMode refMode);

    bool initFromSequence (
        SAPDBMem_IRawAllocator     & allocator,
        SAPDBFields_IFieldSequence * source,
        Msg_List                   & msgList);

    bool initFromStream (
        SAPDBMem_IRawAllocator   & allocator,
        SAPDB_IPersistenceStream & instream,
        int                        fieldCount,
        int                        allDataLength,
        Msg_List                 & msgList);

private:
    SAPDBMem_IRawAllocator * allocator;
    tsp00_Byte             * mem;
};


#endif

