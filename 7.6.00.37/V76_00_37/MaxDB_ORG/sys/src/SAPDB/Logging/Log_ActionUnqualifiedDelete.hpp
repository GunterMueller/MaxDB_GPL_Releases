/*!
   @file           Log_ActionUnqualifiedDelete.hpp
   @author         MartinKi
   @ingroup        ActionHandling
   @brief          Handles the unqualified delete operation on a table.

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
#ifndef LOG_ACTION_UNQUALIFIED_DELETE_H
#define LOG_ACTION_UNQUALIFIED_DELETE_H

#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Container/Container_Vector.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/// Defines a class to handle the unqualified delete operation on a
/// table file. Only the table itself is handled, indices and a
/// potential short column file must be handled separately. This is
/// used in combination with the new filedirectory.
class Log_ActionUnqualifiedDelete : public Log_ReadWriteActionImage
{
public:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith760000 = 0,
        actionVersionStartingWith760007 = 1
    };

    /// default constructor
    Log_ActionUnqualifiedDelete()
        : Log_ReadWriteActionImage (Log_UnqualifiedDelete)
    {}

    /*!
     * @brief factory method to create the a log object of the latest
     * entry version
     *
     * Use of SAPDB_AutoPtr<Log_ActionUnqualifiedDelete> is
     * recommended when working with the returned pointer.
     */
	static Log_ActionUnqualifiedDelete* CreateObject(
        tgg00_TransContext&   trans,
        const FileDir_FileNo& tableFileNo,
        const FileDir_FileNo& dropFileNo);

    /*!
     * @brief factory method to create the correct object depending on
     * actionVersion
     *
     * Use of SAPDB_AutoPtr<Log_ActionUnqualifiedDelete> is
     * recommended when working with the returned pointer.
     */
	static Log_ActionUnqualifiedDelete* CreateObject(
        SAPDBMem_IRawAllocator& allocator,
        Log_ActionVersion       actionVersion);

    /// returns the minimal length, which is needed to copy this entry
    static SAPDB_UInt GetMinimalLengthToCopyEntry(Log_ActionVersion version);

	/// call for rollback of action
    virtual void Undo (tgg00_TransContext &TransContext, Log_IImage &Image) const = 0;

	/// call for redo of action
    virtual void Redo (tgg00_TransContext &TransContext, Log_IImage &Image, SAPDBErr_MessageList &Errlist) const = 0;

    /// This is used by the GarbageColletor
    virtual bool RemoveGarbage (tgg00_TransContext   &trans,
                                SAPDBErr_MessageList &errlist) const = 0;

    virtual void Execute (tgg00_TransContext &trans) const = 0;
};

//---------------------------------------------------------------------------

/// Handler class for unqualified delete starting with 7.6.00.07
class Log_ActionUnqualifiedDelete760007 : public Log_ActionUnqualifiedDelete
{
private:
    // no copy-construction or assignment
    Log_ActionUnqualifiedDelete760007 (const Log_ActionUnqualifiedDelete760007&);
    Log_ActionUnqualifiedDelete760007& operator =(const Log_ActionUnqualifiedDelete760007&);

    /// defines the physical structure of this action in undo/redo-files
    /// starting with version 7.6.00.07
    struct Header
    {
        FileDir_FileNo tableFileNo;
        FileDir_FileNo dropFileNo;
        SAPDB_UInt2    secondaryFileCount;
        SAPDB_Int2     filler;
        SAPDB_Int4     filler1;

        Header ()
            : secondaryFileCount (0)
        {}

        Header (const FileDir_FileNo tableFileNo,
                const FileDir_FileNo dropFileNo)
            : tableFileNo        (tableFileNo),
              dropFileNo         (dropFileNo),
              secondaryFileCount (0),
              filler             (0),
              filler1            (0)
        {}
    };

    /// this is the structured persistent entry information
    Header            m_Head;
public:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith760000 = 0,
        actionVersionStartingWith760007 = 1
    };

    /// this constructor is designed for a further call of ReadImagePersistent
	Log_ActionUnqualifiedDelete760007(
        SAPDBMem_IRawAllocator& allocator)
    : m_Head                   (),
      m_secondaryFiles         (allocator)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionUnqualifiedDelete760007 (
        tgg00_TransContext&   trans,
        const FileDir_FileNo& tableFileNo,
        const FileDir_FileNo& dropFileNo);

    /// returns the minimal length, which is needed to copy this entry
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Header));
    }

	/// call for rollback of action
    virtual void Undo (tgg00_TransContext &TransContext,
                       Log_IImage &Image) const;

	/// call for redo of action
    virtual void Redo (tgg00_TransContext &TransContext, Log_IImage &Image, SAPDBErr_MessageList &Errlist) const
    {
        Execute (TransContext, RedoExecution);
    }
    /// This is used by the GarbageColletor
    bool RemoveGarbage (tgg00_TransContext   &trans,
                        SAPDBErr_MessageList &errlist) const;

    virtual void Execute (tgg00_TransContext &trans) const
    {
        Execute (trans, OnlineExecution);
    }

    virtual SAPDB_UInt GetPersistentLength () const;

    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }

    virtual SAPDB_UInt GetMinimalPersistentLength760000() const
    {
        return GetMinimalLengthToCopyEntry();
    }

    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const;

    virtual void WriteToTrace (const char * Title) const;
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760007;
    }
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

    typedef Container_Vector<FileDir_FileNo> SecondaryFileList;

    SecondaryFileList m_secondaryFiles;

    void Execute (tgg00_TransContext &TransContext,
                  ExecutionType       executionType) const;
};

//---------------------------------------------------------------------------

/// Handler class for unqualified delete from 7.6.00.00 to 7.6.00.07
class Log_ActionUnqualifiedDelete760000 : public Log_ActionUnqualifiedDelete
{

private:
    /// physical structure of this action in undo/redo-files starting
    /// with version 7.6.00.00
    struct Header
    {
        FileDir_FileNo tableFileNo;
        FileDir_FileNo dropFileNo;

        Header ()
        {}

        Header (const FileDir_FileNo tableFileNo,
                const FileDir_FileNo dropFileNo)
            : tableFileNo        (tableFileNo),
              dropFileNo         (dropFileNo)
        {}
    };

    /// this is the structured persistent entry information
    Header         m_Head;

public:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith760000 = 0
    };

    /// this constructor is designed for a further call of ReadImagePersistent
	Log_ActionUnqualifiedDelete760000()
    : m_Head                   ()
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionUnqualifiedDelete760000 (
        const FileDir_FileNo& tableFileNo,
        const FileDir_FileNo& dropFileNo)
        : m_Head (tableFileNo, dropFileNo)
    {}

    /// returns the minimal length, which is needed to copy this entry
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Header));
    }
	/// call for rollback of action
    virtual void Undo (tgg00_TransContext &TransContext, Log_IImage &Image) const;

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

    virtual void Execute (tgg00_TransContext &trans) const
    {
        Execute (trans, OnlineExecution);
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
                                       SAPDBErr_MessageList  &errlist) const;

    virtual void WriteToTrace (const char * Title) const;
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760000;
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

    void UndoReassignDuringRestart (
        tgg00_TransContext   &trans,
        SAPDBErr_MessageList &errList) const;
};

#endif  /* LOG_ACTION_UNQUALIFIED_DELETE_H */
