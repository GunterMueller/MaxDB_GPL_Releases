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
#ifndef LOG_ACTION_CREATE_DROP_FILE_H
#define LOG_ACTION_CREATE_DROP_FILE_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg91.h"
#include "ggg00.h"

/// defines a class to handle the before or after image of change file type
class Log_ActionChangeFileType: public Log_ReadWriteActionImage
{

private:
    
    /// defines the physical structur of this action in undo/redo-files
    struct ChangeFileTypeEntry
    {
        tgg00_Filename    Filename;         ///< name of the file
        tgg00_FiletypeSet OldFiletypeSet;   ///< previous filetype
        tgg00_FiletypeSet NewFiletypeSet;   ///< new file type
    };
    
    /// in memory representation of this action
    ChangeFileTypeEntry m_Entry;

public:
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionChangeFileType ()
    : Log_ReadWriteActionImage(Log_ChangeFileType)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionChangeFileType (
        const tgg00_Filename &Filename,
        tgg00_FiletypeSet     OldFiletypeSet,
        tgg00_FiletypeSet     NewFiletypeSet);
    /// changes the file type set
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandFileId = NULL) const;
    /// undo/redo only: changes the file type set
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
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionChangeFileType::ChangeFileTypeEntry));
    }
};

/// defines a class to handle the after image of create file
class Log_ActionCreateFile: public Log_ReadWriteActionImage
{

private:
    
    /// defines the physical structur of this action in undo/redo-files
    struct CreateFileEntry
    {
        tgg00_Filename    Filename;     ///< filename of the newly created file
        tgg91_FileVersion FileVersion;  ///< version of the new file
        tgg00_FiletypeSet FiletypeSet;  ///< filetype of the new file
    };
    
    /// in memory representation of this action
    CreateFileEntry m_Entry;

public:
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionCreateFile ()
    : Log_ReadWriteActionImage(Log_CreateFile)
    {}
    
    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionCreateFile (
        const tgg00_Filename &Filename,
        tgg91_FileVersion     FileVersion,
        tgg00_FiletypeSet     FiletypeSet);

    /// creates a new file
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandFileId = NULL) const;
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen (sizeof(Log_ActionCreateFile::CreateFileEntry));
    }
    /// redo only: creates a new file
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
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
};

/// defines a class to handle the after image of create index
class Log_ActionCreateIndex: public Log_ReadWriteActionImage
{
    
private:
    
    /// defines the physical structur of this action in undo/redo-files
    struct CreateIndexEntry
    {
        tgg00_Filename TabFilename;     ///< surrogate of table index is defined on
        tgg00_Filename InvFilename;     ///< filename of the index
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
    
public:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith74     = 0,
        actionVersionStartingWith760002 = 1
    };
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionCreateIndex ()
	: Log_ReadWriteActionImage (Log_CreateIndex),
      m_InvDescMapForRead      (),
      m_pInvDescMap            (NULL)
    {}
    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionCreateIndex (
        const tgg00_Filename &TabFilename,
        const tgg00_Filename &InvFilename,
        const Log_InvDescMap *pInvDescMap,
        tgg00_Tfn             PrimTfn,
        SAPDB_UInt1           PrimTfnNo);

    /// creates a new index, in case of redo the pointers have to be ommitted
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandPermFileId   = NULL,
        tgg00_FileId       *pCommandInvFileId    = NULL,
        tgg00_IndexCounter *pCommandIndexCounter = NULL) const;
    /// creates an empty index file with the state 'not accessible'
    void ExecuteSetNotAccessible (tgg00_TransContext &TransContext) const;
    /// redo only: creates an empty index file with the state 'not accessible'
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionCreateIndex::CreateIndexEntry));
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
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760002;
    };
};

/// defines a class to handle the before image of any create except create object
class Log_ActionDropFile: public Log_ReadWriteActionImage
{

private:
    
    /// defines the physical structur of this action in undo/redo-files
    tgg00_Filename m_Filename;

public:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith74     = 0,
        actionVersionStartingWith760002 = 1
    };
    
    /// this constructor is designed for a further call of ReadImagePersistent
	Log_ActionDropFile ()
    : Log_ReadWriteActionImage (Log_CreateFile)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionDropFile (const tgg00_Filename &Filename)
    : Log_ReadWriteActionImage (Log_CreateFile)
    {
        m_Filename = Filename;
    }
    /// drops the file
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandFileId = NULL) const;
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen (sizeof(tgg00_Filename));
    }
    /// undo only: drops the file
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
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Filename, sizeof (m_Filename), errlist);
    }
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760002;
    };
};

/// defines a class to handle the before or after image of rename file
class Log_ActionRenameFile760002: public Log_ReadWriteActionImage
{

private:
    
    /// defines the physical structur of this action in undo/redo-files
    struct RenameFileEntry
    {
        tgg00_Filename    OldFilename;      ///< old filename
        tgg00_Filename    NewFilename;      ///< new filename
        tgg91_FileVersion NewFileVersion;   ///< new version of the filename
    };
    /// in memory representation of the action
    RenameFileEntry m_Entry;

public:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith74     = 0,
        actionVersionStartingWith760002 = 1
    };
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionRenameFile760002 ()
    :Log_ReadWriteActionImage (Log_RenameFile)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionRenameFile760002 (
        const tgg00_Filename &OldFilename,
        const tgg00_Filename &NewFilename,           
        tgg91_FileVersion     NewFileVersion);
    
    /// renames the file
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pOldCommandFileId = NULL,
        tgg00_FileId       *pNewCommandFileId = NULL) const;
    /// is used by garbage collection
    bool RemoveGarbage (tgg00_TransContext   &trans,
                        SAPDBErr_MessageList &errlist);
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionRenameFile760002::RenameFileEntry));
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
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
    /// The action is written to the trace file.
    virtual void WriteToTrace (const char * Title) const;
    /// append some info to the errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
    
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760002;
    };
};

/// defines a class to handle the before or after image of rename file
class Log_ActionRenameFile : public Log_ReadWriteActionImage
{

private:
    
    /// defines the physical structur of this action in undo/redo-files
    struct Header
    {
        tgg00_Surrogate   TableId;          ///< primary table id to update filedirectory statistics
        tgg00_Filename    OldFilename;      ///< old filename
        tgg00_Filename    NewFilename;      ///< new filename
        tgg91_FileVersion NewFileVersion;   ///< new version of the filename
    };
    /// in memory representation of the action
    Header m_Entry;

public:
    /// Defines the different existing versions of the action.
    enum
	{
        actionVersionStartingWith74     = 0,
        actionVersionStartingWith760002 = 1,
        actionVersionStartingWith760011 = 2
    };
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionRenameFile ()
    : Log_ReadWriteActionImage (Log_RenameFile)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionRenameFile ( const tgg00_Surrogate &TableId,
                           const tgg00_Filename  &OldFilename,
                           const tgg00_Filename  &NewFilename,           
                           tgg91_FileVersion      NewFileVersion );
    
    /// renames the file
    void ExecuteAction ( tgg00_TransContext &TransContext,
                         tgg00_FileId       *pOldCommandFileId = NULL,
                         tgg00_FileId       *pNewCommandFileId = NULL) const;
                         
    /// is used by garbage collection
    bool RemoveGarbage ( tgg00_TransContext   &trans,
                         SAPDBErr_MessageList &errlist );

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionRenameFile::Header));
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
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
    /// The action is written to the trace file.
    virtual void WriteToTrace (const char * Title) const;
    /// append some info to the errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
    
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760011;
    };
};

/// defines a class to handle the before or after image of succ file version
class Log_ActionSuccFileVersion: public Log_ReadWriteActionImage
{
    
private:
    
    /// defines the physical structur of this action in undo/redo-files
    struct SuccFileVersionEntry
    {
        tgg00_Filename    Filename;         ///< the filename
        SAPDB_Int4        NewLongCnt;       ///< the column counter
        tgg91_FileVersion NewFileVersion;   ///< the new version
    };
    
    /// the in memory representation
    SuccFileVersionEntry m_Entry;
    
public:
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionSuccFileVersion ()
    :Log_ReadWriteActionImage (Log_SuccFileVersion)
    {}
    
    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionSuccFileVersion (
        const tgg00_Filename &Filename,
        tgg91_FileVersion     NewFileVersion,
        SAPDB_Int4            NewLongCnt);
    /// changes the file version and the number of long columns
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandFileId = NULL) const;
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen (sizeof(Log_ActionSuccFileVersion::SuccFileVersionEntry));
    }
    /// undo/redo only: changes the file version and the number of long columns
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
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
};

/// defines a class to handle the before or after image of update file statistic
class Log_ActionUpdFileStatistic: public Log_ReadWriteActionImage
{
    
private:
    
    /// defines the physical structur of this action in undo/redo-files
    struct UpdFileStatEntry
    {
        tgg00_Filename Filename;    ///< the filename for the update-stat
        SAPDB_Int4     LeafNodes;   ///< before or after value of the stat-info
    };
    
    /// the in memory-representation
    UpdFileStatEntry m_Entry;
    
public:
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionUpdFileStatistic ()
    : Log_ReadWriteActionImage (Log_UpdFileStatistic)
    {}
    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionUpdFileStatistic (
        const tgg00_Filename &Filename,
        SAPDB_Int4            LeafNodes);
    /// changes the number of tree leaf nodes
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandFileId = NULL) const;
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen (sizeof(Log_ActionUpdFileStatistic::UpdFileStatEntry));
    }
    /// undo/redo only: changes the number of tree leaf nodes
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
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
};
#endif  /* LOG_ACTION_CREATE_DROP_FILE_H */
