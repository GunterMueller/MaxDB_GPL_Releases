/*!*********************************************************************

    @file         SAPDBFields_Persistence.cpp

    @author       DanielD

    @brief

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

#include "SAPDBCommon/Fields/SAPDBFields_Persistence.hpp"

#include <limits.h>

#include "gsp00.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp"
#include "SAPDBCommon/SAPDB_IPersistenceStream.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_SequenceMemoryCopy.hpp"

const int protocolVersionC = 1;
/*
    Representation of field sequence on stream:
    HeaderT
        version     ui1     // protocol version
        swap        ui1
        intsize      i2     // 2 or 4
        fieldCount   i2
        alldataSize ui4     // combined length of all fields
                            //  without length info

    (
        fieldLen       see intsize
        field [fieldLen]
    )+
 */

typedef struct HeaderT {
    unsigned char version;
    unsigned char swap;
    tsp00_Int2    intsize;
    tsp00_Int2    fieldCount;
    tsp00_Int2    filler;
    tsp00_Uint4   allDataSize;
} HeaderT;


/*----------------------------------------*/

static inline tsp00_SwapKind_Enum
swapKind ()
{
    static tsp00_Uint4 intval = 1;
    const char * cval = (const char *) &intval;
    if (cval [0] == '\0') {
        return sw_normal;
    }
    else {
        return sw_full_swapped;
    }
}

/*----------------------------------------*/

bool SAPDBFields_Persistence::
Store (
    SAPDBFields_IFieldSequence & fields,
    SAPDB_IPersistenceStream   & stream,
    Msg_List                   & msgList)
{
    /*
     * store header information
     */
    HeaderT header;
    header.version = protocolVersionC;
    header.swap = swapKind ();
    header.intsize = 2;
    header.fieldCount = fields.GetFieldCount ();
    header.allDataSize = fields.GetTotalLength ();
    if (!stream.WriteBytes (&header, sizeof (header), msgList)) {
        return false;
    }
    /*
     * store fields
     */
    SAPDBErr_MessageList errlist;
    for (int i = 0; i < header.fieldCount; ++i) {
        SAPDBFields_AttributedField field;
        if (fields.GetField (i, field, msgList) != SAPDBFields_IFieldSequence::fieldFound) {
            return false;
        }
        SAPDBFields_IFieldSequence::SizeType fieldLen = field.GetLength ();
        SAPDBERR_MISSINGCHECK (fieldLen <= USHRT_MAX);
        tsp00_Int2 len = (tsp00_Int2) fieldLen;
        if (!stream.WriteBytes (&len, sizeof (len), msgList)) {
            return false;
        }
        if (!stream.WriteBytes (field.GetDataPtr (), fieldLen, msgList)) {
            return false;
        }
    }
    /*
     *
     */
    return true;
}

/*----------------------------------------*/

static bool
readHeader (
    HeaderT                    & header,
    SAPDB_IPersistenceStream   & stream,
    Msg_List                   & msgList)
{
    if (!stream.ReadBytes (&header, sizeof (header), msgList)) {
        return false;
    }
    if (header.version != protocolVersionC) {
        return false;
    }
    return true;
}

/*----------------------------------------*/

SAPDBFields_IncrementalMemorySequence * SAPDBFields_Persistence::
RestoreAsIncrementalMemorySequence (
    SAPDB_IPersistenceStream   & stream,
    SAPDBMem_IRawAllocator     & allocator,
    SAPDBFields_MemorySequence::RefcountMode refMode,
    Msg_List                   & msgList)
{
    /*
     * read header
     */
    HeaderT header;
    if (!readHeader (header, stream, msgList)) {
        return NULL;
    }
    /*
     * create result
     */
    SAPDBFields_IncrementalMemorySequence * result =
        new (allocator) SAPDBFields_IncrementalMemorySequence (
                allocator, refMode, header.fieldCount);
    if (result == NULL) {
        return NULL;
    }
    /*
     * read fields
     */
    for (int i = 0; i < header.fieldCount; ++i) {
        tsp00_Uint2 fieldLen;
        if (!stream.ReadBytes (&fieldLen, sizeof (fieldLen), msgList)) {
            destroy (result, allocator);
            return NULL;
        }
        tsp00_Byte * buf = result->CreateVariableField (fieldLen);
        if (!stream.ReadBytes (buf, fieldLen, msgList)) {
            destroy (result, allocator);
            return NULL;
        }
        result->CloseCurrentField (fieldLen);
    }
    /*
     *
     */
    return result;
}

/*----------------------------------------*/

SAPDBFields_SequenceMemoryCopy * SAPDBFields_Persistence::
RestoreAsSequenceMemoryCopy (
    SAPDB_IPersistenceStream   & stream,
    SAPDBMem_IRawAllocator     & allocator,
    SAPDBFields_MemorySequence::RefcountMode refMode,
    Msg_List                   & msgList)
{
    /*
     * read header
     */
    HeaderT header;
    if (!readHeader (header, stream, msgList)) {
        return NULL;
    }
    /*
     * create result
     */
    SAPDBFields_SequenceMemoryCopy * result =
        SAPDBFields_SequenceMemoryCopy::Create (
                allocator, stream, header.fieldCount, header.allDataSize, 
                refMode, msgList);
    if (result == NULL) {
        return NULL;
    }
    return result;
}

/*----------------------------------------*/

