/*!
   @file           Log_ActionCreateTable.hpp
   @author         JuergenA
   @author         UweH,MartinKi
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
#ifndef LOG_ACTIONCREATETABLE_H
#define LOG_ACTIONCREATETABLE_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

#include "ggg91.h"
#include "ggg00.h"

/// defines a class to handle the after image of create table
class Log_ActionCreateTable : public Log_ReadWriteActionImage
{
public:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith74     = 0,
        actionVersionStartingWith760002 = 1,
        actionVersionStartingWith760018 = 2
    };

    /*!
     * @brief Constructor designed for a further call of
     * ReadImagePersistent
     */
    Log_ActionCreateTable ()
        : Log_ReadWriteActionImage( Log_CreateTable )
    {}

    /*!
     * @brief Constructor designed for a further call of
     * WriteImagePersistent
     *
     * @param TableFileNo [in] file no of table to create
     * @param FileVersion [in] file version of table to create
     * @param FiletypeSet [in] file type set of table to create
     * @param eFiletypeSet [in] extended file type set of table to create
     * @param LobColCnt   [in] number of lob columns of table to create
     */
    Log_ActionCreateTable (
        const FileDir_FileNo           TableFileNo,
        const tgg91_FileVersion         FileVersion,
        const tgg00_FiletypeSet         FiletypeSet,
        const tgg00_ExtendedFiletypeSet eFiletypeSet,
        const SAPDB_Int2                LobColCnt);

    /*!
     * @brief Creates a new table
     *
     * TransContext [in/out] trans context
     * IsRedo       [in] should be set to true if method is called during redo
     */
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        const SAPDB_Bool    IsRedo = false) const;

    /*!
     * @brief Returns minimal length that can be used for redo copy
     *
     * @return aligned length of log image
     */
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionCreateTable::CreateTableEntry));
    }

    /*!
     * @brief Creates table during redo
     *
     * @warning Should be used during <em>redo</em> only!
     * TransContext [in/out] trans context
     */
    virtual void Execute (tgg00_TransContext &TransContext) const;

    /*!
     * @brief Returns length of persistent log image
     */
    virtual SAPDB_UInt GetPersistentLength () const;

    /*!
     * @brief Returns minimum length of log image
     */
    virtual SAPDB_UInt GetMinimalPersistentLength () const;

    /*!
     * @brief Reads persistent image
     *
     * @param ImageReader [in] reader to read image from
     * @param errlist [out] possible error messages
     *
     * @return
     *  - true, if successful
     *  - false, if an error has occured
     */
    virtual bool ReadImagePersistent (
        Log_ActionImageReader &ImageReader,
        SAPDBErr_MessageList  &errlist);

    /*!
     * @brief Writes a persistent image of the entry
     *
     * @param ImageWriter [in] writer to write image to
     * @param errlist [out] possible error messages
     *
     * @return
     *  - true, if successful
     *  - false, if an error has occured
     */
    virtual bool WriteImagePersistent (
        Log_ActionImageWriter &ImageWriter,
        SAPDBErr_MessageList  &errlist)    const;

    /*!
     * @brief Returns the version of the persistent layout of this entry
     */
    virtual Log_ActionVersion GetPersistentVersion() const;

private:

    /// defines the physical structure of this action in undo/redo-files
    struct CreateTableEntry
    {
        FileDir_FileNo            TableFileNo;  ///< file no identifying the table
        tgg91_FileVersion         FileVersion;  ///< file version
        tgg00_FiletypeSet         FiletypeSet;  ///< file type set
        SAPDB_UInt1               Filler;       ///< filler for alignment
        tgg00_ExtendedFiletypeSet eFiletypeSet;  ///< file attributes
        SAPDB_UInt1               Filler1;       ///< filler for alignment
        SAPDB_Int2                LobColCnt;    ///< number of LOB-columns
    };

    CreateTableEntry m_Entry;
};

/**************************************************************************/
/**************************************************************************/

/// defines a class to handle the after image of create table
class Log_ActionCreateTable01 : public Log_ReadWriteActionImage
{

private:
    /// Defines the different existing versions of the action.
    enum {
        actionVersionStartingWith74     = 0,
        actionVersionStartingWith760002 = 1,
    };

    /// defines the physical structure of this action in undo/redo-files
    struct CreateTableEntry
    {
        tgg00_Filename    Filename;     ///< the name of the file
        tgg91_FileVersion FileVersion;  ///< the fileversion
        tgg00_FiletypeSet FiletypeSet;  ///< the used file-attributes
        SAPDB_Bool1       NoTreeIndex;  ///< for tableload <= 7.3 set to true
        SAPDB_Int2        StrColCnt;    ///< number of LONG-cols
        SAPDB_Int2        VarColCntOff; ///< numer of VAR-cols
    };

    /// in memory representation
    CreateTableEntry m_Entry;

public:
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionCreateTable01 ()
    : Log_ReadWriteActionImage(Log_CreateTable)
    {}

    /// creates a new table
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandFileId = NULL) const;

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionCreateTable01::CreateTableEntry));
    }

    /// redo only: creates a new table
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
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760002;
    };
};

#endif  /* LOG_ACTIONCREATETABLE_H */
