/*!
   @file           Log_ActionRename75.hpp
   @author         JuergenA
   @author         UweH
   @author         MartinKi
   @ingroup        ActionHandling
   @brief          defines a class to handle rename entries for files for releases < 7.6

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
#ifndef LOG_ACTION_RENAME_FILE75_H
#define LOG_ACTION_RENAME_FILE75_H

#include "Logging/Log_Types.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg91.h"
#include "ggg00.h"

/// defines a class to handle the before or after image of rename file
/// for releases < 7.6
class Log_ActionRenameFile75 : public Log_ReadWriteActionImage
{

private:
    /// defines the physical structure of this action in undo/redo-files
    struct RenameFileEntry
    {
        tgg00_Filename    OldFilename;      ///< old filename
        tgg00_Filename    NewFilename;      ///< new filename
        tgg91_FileVersion NewFileVersion;   ///< new version of the filename
    };
    /// in memory representation of the action
    RenameFileEntry m_Entry;

    void RenameForDelete(
        tgg00_TransContext &TransContext,
        tgg00_FileId& oldFileId,
        tgg00_FileId& newFileId) const;

    void RenameForUndoDelete(
        tgg00_TransContext &TransContext,
        tgg00_FileId& oldFileId,
        tgg00_FileId& newFileId) const;

    void Reassign(
        tgg00_TransContext &TransContext,
        tgg00_FileId&       oldFileId,
        tgg00_FileId&       newFileId) const;
public:
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionRenameFile75 ()
    :Log_ReadWriteActionImage (Log_RenameFile)
    {}

    /// renames the file
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pOldCommandFileId = NULL,
        tgg00_FileId       *pNewCommandFileId = NULL) const;

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionRenameFile75::RenameFileEntry));
    }

    /// undo/redo only: renames the file
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }

    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }

    /// reads the image of the action
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);

    /// writes the image of the action
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return false;
    }
    /// The action is written to the trace file.
    virtual void WriteToTrace (const char * Title) const;
    /// append some info to the errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;

};

#endif  /* LOG_ACTION_RENAME_FILE75_H */
