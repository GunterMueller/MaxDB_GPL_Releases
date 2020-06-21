/*!
   @file           Log_ActionModifyTable.hpp
   @author         JuergenA
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines a class to handle the after image of modify table

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
#ifndef LOG_ACTION_MODIFY_TABLE_H
#define LOG_ACTION_MODIFY_TABLE_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"

/// defines a class to handle the after image of modify table
class Log_ActionModifyTable: public Log_ReadWriteActionImage
{
    
private:
    
    /// the pysical layout of the undo/redo entry
    struct ModifyTableEntryHeader
    {
        SAPDB_UInt4    ImageLen;        ///< the size of the action
        SAPDB_UInt4    InvDescMapLen;   ///< the desciption of the index-fields 
        tgg00_Filename SourceFilename;  ///< the old file
        tgg00_Filename DestinFilename;  ///< the new file
    };
    
    /// the in memory-action
    ModifyTableEntryHeader  m_EntryHeader;
    
    /// pointer to the source-description of the index
    const Log_InvDescMap   *m_pSourceInvDescMap;
    /// pointer to the target-description of the index
    const Log_InvDescMap   *m_pDestinInvDescMap;
    
    /// the source-description of the index
    Log_InvDescMap          m_SourceInvDescMapForRead;
    /// the target-description of the index
    Log_InvDescMap          m_DestinInvDescMapForRead;
    
public:
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionModifyTable ();

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionModifyTable (
        const tgg00_Filename &SourceFilename,
        const tgg00_Filename &DestinFilename,
        const Log_InvDescMap *pSourceInvDescMap,
        const Log_InvDescMap *pDestinInvDescMap);

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof(ModifyTableEntryHeader);
    }

    /// modifies the table (redo only)
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }

    /*!
       @brief          modifies the table
       @param          TransContext [in out] 
       @param          pSourceCommandFileId [in out] in case of redo the source file id has to be omitted
       @param          pIndexCommandFileId [in out] in case of redo the index file id has to be omitted
       @param          pDestinCommandFileId [in out] in case of redo the destination file id has to be omitted
       @param          IsSorted [in] in case of redo this paramter has to be ommitted
       @param          TruncCheckPos [in] in case of redo checking is not necessary
       @param          TruncCheckCnt [in] in case of redo checking is not necessary
       @param          ResultSampleInfo [out] in case of redo this paramter has to be ommitted
     */
    virtual void ExecuteAction (
        tgg00_TransContext  &TransContext,
        tgg00_FileId        *pSourceCommandFileId = NULL,
        tgg00_FileId        *pIndexCommandFileId  = NULL,
        tgg00_FileId        *pDestinCommandFileId = NULL,
        bool                 IsSorted             = false,
        SAPDB_Int            TruncCheckPos        = 0,
        SAPDB_Int            TruncCheckCnt        = 0,
        tgg00_SampleInfoPtr  ResultSampleInfo     = NULL) const;

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return m_EntryHeader.ImageLen;
    }

    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);

    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const;
};
#endif  /* LOG_ACTION_MODIFY_TABLE_H */
