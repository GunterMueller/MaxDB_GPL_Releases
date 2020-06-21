/*!
   @file           Log_ActionUpdateRec.hpp
   @author         JuergenA
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines a class to handle log entries for insert/delete record

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
#ifndef LOG_ACTION_UPDATE_REC_H
#define LOG_ACTION_UPDATE_REC_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_RecordColumnMap.hpp"

#include "ggg00.h"

/// defines a class to handle action entries for records or keys
class Log_ActionUpdateRec: public Log_RecordColumnMap
{

private:
    
    /// the physical structure of the LogEntry
    struct UpdateRecEntryHeader
    {
        SAPDB_UInt4    ImageLen;    ///< size of the action entry
        SAPDB_UInt2    KeyLen;      ///< length of the key
        SAPDB_UInt2    NewRecLen;   ///< new length of the record after the update
        SAPDB_UInt4    ColMapLen;   ///< length of the column-map
        SAPDB_UInt4    InvDescLen;  ///< length of the description of the indices
        tgg00_Filename PrimFilename;///< filename of the table

        /// constructor
        UpdateRecEntryHeader()
        {
            ImageLen  = 0;
        	KeyLen    = 0;
        	NewRecLen = 0;
        	ColMapLen = 0;
        	InvDescLen = 0;
        	PrimFilename.SetZero();
        }
    };
    
    /// in memory representation
    UpdateRecEntryHeader    m_EntryHeader;
    
    /// pointer to the key-values
    const tgg00_Lkey       *m_pKey;

    /// pointer to the description of the indices
    const Log_InvDescMap   *m_pInvDescMap;
    
    /// pointer to the keys for reading them
    tgg00_Lkey             *m_pKeyForRead;

    /// the description of the index-structure
    Log_InvDescMap          m_InvDescMapForRead;
    
    /// allocator for allocating dynamic memory
    SAPDBMem_IRawAllocator *m_pKeyAllocator;

    tgg91_TransNo  m_PrevUpdTrans;
 
    tgg91_PageRef  m_PrevRef; 

    /// initializes the update action entry
    void Init (
        const tgg00_Filename &PrimFilename,
        const Log_InvDescMap *pInvDescMap,
        const tgg00_Lkey     *pKey,
        const tgg00_Rec      *pNewRecord,
        const SAPDB_UInt2     NullValLen);

public:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith760002 = 1
    };
    
    /// empty action entry
    Log_ActionUpdateRec ();
    Log_ActionUpdateRec (const tgg91_TransNo& prevTrans, const tgg91_PageRef& prevRef);

    /// deallocates the private field and inv description map
    ~Log_ActionUpdateRec ();

    void AddCompoundIndexColumns (
        Log_InvDescMap  *pOwnWriteAccessInvDescMap,
        const tgg00_Rec &Image, 
        bool            &IsOk);

    void CreateBeforeImage (const tgg00_Rec& currImage, tgg00_Rec& beforeImage);

    /*!
       @brief          creates the new record using the old record and the column descriptions
       @param          PrimFilename [in] 
       @param          pInvDescMap [in] NULL or pointer to the initialized inv description map
       @param          pOldRecord [in] pointer to the existing record
       @param          pNewRecord [out] pointer to the record space reserved to build the new record
       @param          NewRecSize [in] size of the record space
       @param          BeforeImage [in out] corresponding before image
       @param          Error [out] 

       This function applies to the after image and creates a corresponding before image.
       In case of a key update, the new record is created but the before image is incomplete.
     */

    void CreateNewRecAndInitAfterBeforeImage (
        const tgg00_Filename   &PrimFilename,
        const Log_InvDescMap   *pInvDescMap,
        const tgg00_Rec        *pOldRecord,
        tgg00_Rec              *pNewRecord,
        SAPDB_Int4              NewRecSize,
        Log_ActionUpdateRec    &BeforeImage,
        tgg00_BasisError       &Error);

    /// The action is written to the trace file.
    virtual void WriteToTrace (const char * Title) const;

    /// updates the record with its secondary keys
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       &PrimFileId,
        const tgg00_Rec    *pOldRecord,
        const tgg00_Rec    *pNewRecord,
        bool                IsUndoOrRedo = false) const;

    void GarbageCollection(tgg00_TransContext &TransContext) const;
    /*!
       @brief          initializes this after and before image using an existing new record
       @param          RawAllocator [ref] 
       @param          PrimFilename [in] 
       @param          pInvDescMap [in] NULL or pointer to the initialized inv description map
       @param          pOldRecord [in] pointer to the existing record
       @param          pNewRecord [out] pointer to the record space reserved to build the new record
       @param          BeforeImage [in out] corresponding before image
       @param          Error [out] 

       This function applies to the after image and creates a corresponding
       before image.
     */
    void InitAfterBeforeImageWithExistingNewRec (
        SAPDBMem_IRawAllocator &RawAllocator,
        const tgg00_Filename   &PrimFilename,
        const Log_InvDescMap   *pInvDescMap,
        const tgg00_Rec        *pOldRecord,
        const tgg00_Rec        *pNewRecord,
        Log_ActionUpdateRec    &BeforeImage,
        tgg00_BasisError       &Error);
    /*!
       @brief          removes after image inv desc entries, which are not contained in the record column map
       @param          RawAllocator [ref] 
       @param          pOwnWriteAccessInvDescMap [in out] same pointer as m_pInvDescMap but without const
       @param          IsOk [out] 
       @return         the persistent length of index entries

       A new inv desc map is allocated without the redundant entries. The pointer
       to the inv desc map of the before image is replaced by this new allocated inv desc map.  

     */

    SAPDB_UInt4 RemoveRedundantInvDescEntries (
        SAPDBMem_IRawAllocator &RawAllocator,
        Log_InvDescMap         *pOwnWriteAccessInvDescMap,
        bool                   &IsOk);
    
    /*!
       @brief          sets InvDescLen to the given value
       @param          NewInvDescLen [in] 
       
       used to set it in BeforeImage after RemoveRedundantInvDescEntries
     */
    void SetInvDescLen (SAPDB_UInt4 NewInvDescLen)
    {
        m_EntryHeader.InvDescLen = NewInvDescLen;
    };
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof(UpdateRecEntryHeader);
    }

    /// undo/redo only: updates the record with its secondary keys
    virtual void Execute (tgg00_TransContext &TransContext) const;

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return (m_EntryHeader.ImageLen);
    }
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const;
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760002;
    }
};


#endif  /* LOG_ACTION_UPDATE_REC_H */
