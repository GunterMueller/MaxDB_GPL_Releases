/*!
   @file           Log_ActionFile.hpp
   @author         UweH
   @ingroup        ActionHandling
   @brief          handle operations on one file using the new file directory

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
#ifndef LOG_ACTION_FILE_H
#define LOG_ACTION_FILE_H

#include "DataAccess/Data_FileTypes.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "ggg91.h"
#include "ggg00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"


class FileDir_Table;

/// Defines a class to handle new file operations. Create,Drop,Rename,MarkForDelete
/// This is used in combination with the new filedirectory.
class Log_ActionFile: public Log_ReadWriteActionImage
{

private:

    /// defines the physical structur of this action in undo/redo-files
    struct Header
    {
        tgg00_Filename    filename;
        tgg91_FileVersion fileVersion;
        Data_FileState    fileState;

        Header ()
        {
            filename.SetBlank();
            fileState = Data_FileOk;
        }
        Header (const tgg00_Filename &newFilename)
        {
            filename  = newFilename;
            fileState = Data_FileOk;
            fileVersion.gg91SetDummyVers();
        }
    };
    /// the special type of file action
    Log_ActionType m_Type;
    /// this is the structured persistent entry information
    Header         m_Head;

public:

    /// this constructor is designed for a further call of ReadImagePersistent
	Log_ActionFile (Log_ActionType actionType)
    : Log_ReadWriteActionImage (actionType),
      m_Type                   (actionType),
      m_Head                   ()
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionFile (
        tgg00_TransContext   &TransContext,
        Log_ActionType        actionType,
        const tgg00_Filename &filename);

    /// Sets the fileversion necessary for undoing this action by Log_ActionFile::Execute
    void SetFileVersion(const tgg91_FileVersion fileVersion);

    /// this has at least to be copied in one step
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Header));
    }
	/// call for rollback of action
    virtual void Undo (tgg00_TransContext &TransContext,
                       Log_IImage         &Image) const
    {
        Execute (TransContext, UndoExecution);
    }
	/// call for redo of action
    virtual void Redo (tgg00_TransContext   &TransContext,
                       Log_IImage           &Image,
                       SAPDBErr_MessageList &Errlist) const
    {
        Execute (TransContext, RedoExecution);
    }
    /// This is used by the GarbageColletor
    bool RemoveGarbage (tgg00_TransContext   &trans,
                        SAPDBErr_MessageList &errlist) const;
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        Execute (TransContext, OnlineExecution);
    }
    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist)
	{
        return ImageReader.CopyImage( reinterpret_cast<SAPDB_Byte*>(&m_Head),
                                      sizeof(m_Head),
                                      errlist );
	}
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Head, sizeof (m_Head), errlist);
    }
    virtual void WriteToTrace (const char * Title) const;
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
    virtual bool GarbageCollectionNeeded() const
    {
        return true;
    }
private:
    enum ExecutionType
    {
        OnlineExecution,
        UndoExecution,
        RedoExecution
    };
    void Execute (tgg00_TransContext &TransContext,
                  ExecutionType       executionType) const;

    SAPDB_Bool SetFileVersionInRoot (
        tgg00_TransContext&   trans,
        FileDir_Table&        fileEntry,
        tgg91_FileVersion     newFileVersion) const;
};
#endif  /* LOG_ACTION_FILE_H */
