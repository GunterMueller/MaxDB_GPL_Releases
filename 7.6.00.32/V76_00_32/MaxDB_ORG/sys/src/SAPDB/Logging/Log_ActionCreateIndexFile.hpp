/*!
   @file           Log_ActionCreateIndexFile.hpp
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines the class Log_ActionCreateIndexFile

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
#ifndef LOG_ACTION_CREATE_INDEX_FILE_H
#define LOG_ACTION_CREATE_INDEX_FILE_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "ggg00.h"

/// defines a class to handle the after image of create file
class Log_ActionCreateIndexFile : public Log_ReadWriteActionImage
{
public:
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionCreateIndexFile ()
    : Log_ReadWriteActionImage (Log_CreateIndexFile)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionCreateIndexFile ( const tgg00_Filename &tableFilename,
                                const tgg00_Filename &indexFilename,
                                const SAPDB_UInt      indexId )
    : Log_ReadWriteActionImage (Log_CreateIndexFile),
      m_Head (tableFilename, indexFilename, indexId)
    {}
    
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Head));
    }

    /// creates a new empty index file
    void ExecuteAction ( tgg00_TransContext &trans,
                         tgg00_FileId       *pCommandTableFileId = 0,
                         tgg00_FileId       *pCommandIndexFileId = 0 ) const;

    /// redo only: creates a new empty index file
    virtual void Execute ( tgg00_TransContext &trans ) const
    {
        ExecuteAction (trans);
    }

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }

    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    
    virtual void WriteToTrace (const char * Title) const;

    virtual bool ReadImagePersistent ( Log_ActionImageReader &reader,
                                       SAPDBErr_MessageList  &errlist )
    {
        return reader.CopyImage ( reinterpret_cast<SAPDB_Byte*>(&m_Head), sizeof(m_Head), errlist );
    }
    
    virtual bool WriteImagePersistent ( Log_ActionImageWriter &writer,
                                        SAPDBErr_MessageList  &errlist ) const
    {
        return writer.PersistentWrite (&m_Head, sizeof (m_Head), errlist);
    }

private:

    /// defines the physical structur of this action in undo/redo-files
    struct Head
    {
        tgg00_Filename tableFilename;   ///< filename of the primary table
        tgg00_Filename indexFilename;   ///< filename of the index file
        SAPDB_UInt4    indexId;         ///< the logical number of the index
        /// for online usage
        Head(const tgg00_Filename& newTableFilename,
             const tgg00_Filename& newIndexFilename,
             const SAPDB_UInt      newIndexId)
        {
            tableFilename = newTableFilename;
            indexFilename = newIndexFilename;
            indexId       = newIndexId;
        }
        /// for undo/redo usage - the image is read from log
        Head()
        {
            indexId = SAPDB_MAX_UINT4;
        }
        bool IsInitialized() const
        {
            return indexId != SAPDB_MAX_UINT4;
        }
    };
    
    /// in memory representation of this action
    Head m_Head;
};
#endif  /* LOG_ACTION_CREATE_INDEX_FILE_H */
