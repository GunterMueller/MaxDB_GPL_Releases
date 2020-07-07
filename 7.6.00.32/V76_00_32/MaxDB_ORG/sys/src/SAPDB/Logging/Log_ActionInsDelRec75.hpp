/*!
   @file           Log_ActionInsDelRec75.hpp
   @author         JuergenA
   @author         UweH
   @author         MartinKi
   @ingroup        ActionHandling
   @brief          defines a class to handle log entries for insert/delete record for releases < 7.6

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
#ifndef LOG_ACTION_INS_DEL_REC75_H
#define LOG_ACTION_INS_DEL_REC75_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"

/// defines a class to handle action entries for records or keys
class Log_ActionInsDelRec75: public Log_ReadWriteActionImage
{

private:

    /// the physical representation as before/after-image
    struct InsDelRecEntryHeader
    {
        SAPDB_UInt4    ImageLen;        ///< length if the record-image
        SAPDB_UInt4    KeyOrRecLen;     ///< key or reclen (if calculatable)
        SAPDB_UInt4    InvDescLen;      ///< size of the inversion
        tgg00_Filename PrimFilename;    ///< the filename
    };

    /// the inmemory representation
    InsDelRecEntryHeader    m_EntryHeader;

    /// the record used
    const tgg00_Rec        *m_pRec;

    /// pointer to the descriptor of the index-fileds
    const Log_InvDescMap   *m_pInvDescMap;

    /// only the key (not a record) is described
    bool                    m_bKeyOnly;

    /// pointer to the record for reading
    tgg00_Rec              *m_pRecForRead;

    /// the description of the index fields
    Log_InvDescMap          m_InvDescMapForRead;

    /// the allocator used for dynamic memory allocation
    SAPDBMem_IRawAllocator *m_pRecAllocator;

    /// previous update transaction
    tgg91_TransNo  m_PrevUpdTrans;

    /// previous history reference
    tgg91_PageRef  m_PrevRef;
    /// Initialization
    void InitHeader ();

    SAPDB_Bool     m_IsBeforeImage;
    Log_ActionType m_ActionType;
public:

    /// creates a key or record action depending on before or after image
    Log_ActionInsDelRec75 (
        Log_ActionType ActionType,
        bool           IsBeforeImage);

    Log_ActionInsDelRec75 (
        Log_ActionType       ActionType,
        const tgg91_TransNo& PrevUpdTrans,
        const tgg91_PageRef& PrevRef);

    /// deallocation
    ~Log_ActionInsDelRec75 ();

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof(InsDelRecEntryHeader);
    }

    /// The action is written to the trace file.
    virtual void WriteToTrace (const char * Title) const;


    void GarbageDelete (tgg00_TransContext &TransContext, tgg91_PageRef &wantedPageref);

    /// returns the fileid of the file containing the record
    void GetPrimaryFile (
        tgg00_TransContext& TransContext,
        tgg00_FileId&       PrimFileId) const;

    /// writes the record to the specified space
    void MaterializeRecord (
        SAPDB_UInt4       DestinSize,
        void             *pDestin,
        tgg00_BasisError &Error) const;

    /// deletes or inserts the record and its secondary keys
    virtual void Execute (tgg00_TransContext &TransContext) const;

    /// the persistent length of the image of the action
    virtual SAPDB_UInt GetPersistentLength () const
    {
        return (m_EntryHeader.ImageLen);
    }

    /// this length is the minimum length of the entry
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }

    /// reads the image of the action
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);

    /// writes the image of the action
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const;
};


#endif  /* LOG_ACTION_INS_DEL_REC75_H */
