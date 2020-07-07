/*!
   @file           Log_ActionCreateShortColumnFile.hpp
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines the class Log_ActionCreateShortColumnFile

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
#ifndef LOG_ACTION_CREATE_SHORT_COLUMN_FILE_H
#define LOG_ACTION_CREATE_SHORT_COLUMN_FILE_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "ggg00.h"

/// defines a class to handle the after image of create short column file
class Log_ActionCreateShortColumnFile : public Log_ReadWriteActionImage
{
public:

    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionCreateShortColumnFile ()
    : Log_ReadWriteActionImage (Log_CreateShortColumnFile)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionCreateShortColumnFile (
        const FileDir_FileNo &tableFileNo,
        const FileDir_FileNo &shortColFileNo )
    : Log_ReadWriteActionImage (Log_CreateShortColumnFile),
      m_Head (tableFileNo, shortColFileNo)
    {}

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Head));
    }

    /// creates a new empty index file
    void ExecuteAction ( tgg00_TransContext &trans,
                         FileDir_FileNo *pCommandTableFileNo = 0,
                         FileDir_FileNo *pCommandIndexFileNo = 0 ) const;

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
        FileDir_FileNo tableFileNo;     ///< fileNo of the primary table
        FileDir_FileNo shortColFileNo;  ///< fileNo of the short column file

        /// for online usage
        Head(const FileDir_FileNo& TableFileNo,
             const FileDir_FileNo& shortColFileNo)
            : tableFileNo (TableFileNo),
              shortColFileNo (shortColFileNo)
        {}

        /// for undo/redo usage - the image is read from log
        Head()
        {}

        bool IsInitialized() const
        {
            return this->shortColFileNo.IsValid();
        }
    };

    /// in memory representation of this action
    Head m_Head;
};
#endif  /* LOG_ACTION_CREATE_SHORT_COLUMN_FILE_H */
