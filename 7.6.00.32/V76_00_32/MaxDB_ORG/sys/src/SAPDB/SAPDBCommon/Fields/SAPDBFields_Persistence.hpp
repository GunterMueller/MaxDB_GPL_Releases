/*!*********************************************************************

    @file         SAPDBFields_Persistence.hpp

    @author       DanielD

    @brief        store and read field sequences

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

#ifndef SAPDBFIELDS_PERSISTENCE_HPP
#define SAPDBFIELDS_PERSISTENCE_HPP

#include "Messages/Msg_List.hpp"

#include "SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp"

class SAPDBMem_IRawAllocator;
class SAPDB_IPersistenceStream;
class SAPDBFields_IFieldSequence;
class SAPDBFields_IncrementalMemorySequence;
class SAPDBFields_SequenceMemoryCopy;

/*!

  @class SAPDBFields_Persistence

 */
class SAPDBFields_Persistence
{
public:
    /*!
        @brief writes sequence fields to a stream
     */
    static bool Store ( SAPDBFields_IFieldSequence & fields,
                        SAPDB_IPersistenceStream   & stream,
                        Msg_List                   & msgList);

    /*!
        @brief read a sequence from stream and create a SAPDBFields_IncrementalMemorySequence
     */
    static SAPDBFields_IncrementalMemorySequence * RestoreAsIncrementalMemorySequence (
                        SAPDB_IPersistenceStream   & stream,
                        SAPDBMem_IRawAllocator     & allocator,
                        SAPDBFields_MemorySequence::RefcountMode refMode,
                        Msg_List                   & msgList);

    /*!
        @brief read a sequence from stream and create a SAPDBFields_SequenceMemoryCopy
     */
    static SAPDBFields_SequenceMemoryCopy * RestoreAsSequenceMemoryCopy (
                        SAPDB_IPersistenceStream   & stream,
                        SAPDBMem_IRawAllocator     & allocator,
                        SAPDBFields_MemorySequence::RefcountMode refMode,
                        Msg_List                   & msgList);
private:
    /*!
        @brief static methods only
     */
    SAPDBFields_Persistence () {};

};

#endif

