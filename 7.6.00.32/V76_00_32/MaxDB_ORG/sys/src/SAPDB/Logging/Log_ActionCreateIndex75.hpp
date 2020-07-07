/*!
   @file           Log_ActionCreateDropFile.hpp
   @author         JuergenA
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines a class to handle log entries for files

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
#ifndef LOG_ACTIONCREATEINDEX75_H
#define LOG_ACTIONCREATEINDEX75_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg91.h"
#include "ggg00.h"


/// defines a class to handle the after image of create index
class Log_ActionCreateIndex75: public Log_ReadWriteActionImage
{

private:

    /// defines the physical structur of this action in undo/redo-files
    struct CreateIndexEntry
    {
        tgg00_Filename TabFilename;     ///< surrogate of table index is defined on
        SAPDB_UInt2    InvDescMapLen;   ///< size of the descriptor
        tgg00_Tfn      PrimTfn;         ///< name of the primary table
        SAPDB_UInt1    PrimTfnNo;       ///< number of the primary table
    };

    /// in memory representation
    CreateIndexEntry      m_EntryHeader;

    /// descriptor ot the invertation
    Log_InvDescMap        m_InvDescMapForRead;

    /// pointer to the descriptor
    const Log_InvDescMap *m_pInvDescMap;

    enum ExecutionType {
        executeCreate,
        executeSetNotAccessible
    };

    void ExecuteAll(
        tgg00_TransContext &TransContext,
        const ExecutionType execType) const;
public:
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionCreateIndex75 ()
	: Log_ReadWriteActionImage (Log_CreateIndex),
      m_InvDescMapForRead      (),
      m_pInvDescMap            (NULL)
    {}

    /// creates a new index
    void ExecuteAction (
        tgg00_TransContext &TransContext) const;

    /// creates an empty index file with the state 'not accessible'
    void ExecuteSetNotAccessible (tgg00_TransContext &TransContext) const;

    /// redo only: creates an empty index file with the state 'not accessible'
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionCreateIndex75::CreateIndexEntry));
    }
    /// redo only: creates a new index
    void RebuildIndex (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteSetNotAccessible (TransContext);
    }
    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry()
               +
               Log_AlignedImageLen(m_pInvDescMap->GetPersistentLength());
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

#endif  /* LOG_ACTIONCREATEINDEX75_H */
