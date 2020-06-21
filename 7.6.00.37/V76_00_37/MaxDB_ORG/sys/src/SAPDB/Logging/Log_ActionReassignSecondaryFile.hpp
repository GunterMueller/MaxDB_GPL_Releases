/*!
   @file           Log_ActionReassignSecondaryFile.hpp
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines the class Log_ActionReassignSecondaryFile

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
#ifndef LOG_ACTION_REASSIGN_INDEX_FILE_H
#define LOG_ACTION_REASSIGN_INDEX_FILE_H

#include "ggg00.h"

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Types.hpp"

/// defines a class ...
class Log_ActionReassignSecondaryFile : public Log_ReadWriteActionImage
{
public:

    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionReassignSecondaryFile ()
    : Log_ReadWriteActionImage (Log_ReassignSecondaryFile)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionReassignSecondaryFile ( const tgg00_Surrogate &secondaryFile,
                                      const tgg00_Surrogate &fromPrimaryFile,
                                      const tgg00_Surrogate &toPrimaryFile )
    : Log_ReadWriteActionImage (Log_ReassignSecondaryFile),
      m_Head (secondaryFile, fromPrimaryFile, toPrimaryFile)
    {}

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Head));
    }

    /// creates a new empty index file
    void ExecuteAction ( tgg00_TransContext &trans ) const;

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
        FileDir_FileNo secondaryFile;   ///< 8byte secondary file fileno
        FileDir_FileNo fromPrimaryFile; ///< 8byte primary fileno to delete secondary file from
        FileDir_FileNo toPrimaryFile;   ///< 8byte primary fileno to add secondary file to
        /// for online usage
        Head(const tgg00_Surrogate &SecondaryFile,
             const tgg00_Surrogate &FromPrimaryFile,
             const tgg00_Surrogate &ToPrimaryFile)
            : secondaryFile(SecondaryFile),
              fromPrimaryFile(FromPrimaryFile),
              toPrimaryFile(ToPrimaryFile)
        {}

        /// for undo/redo usage - the image is read from log
        Head()
        {
            secondaryFile.Invalidate();
            fromPrimaryFile.Invalidate();
            toPrimaryFile.Invalidate();
        }
        bool IsInitialized() const
        {
            return
                secondaryFile.IsValid()   ||
                fromPrimaryFile.IsValid() ||
                toPrimaryFile.IsValid();
        }
    };

    /// in memory representation of this action
    Head m_Head;
};
#endif  /* LOG_ACTION_REASSIGN_INDEX_FILE_H */
