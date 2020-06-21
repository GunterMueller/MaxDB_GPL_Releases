/*!
   @file           Log_ActionCreateTable75.hpp
   @author         JuergenA
   @author         UweH
   @author         MartinKi
   @ingroup        ActionHandling
   @brief          defines a class to handle create table log entries for releases <=7.5

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
#ifndef LOG_ACTION_CREATE_TABLE75_H
#define LOG_ACTION_CREATE_TABLE75_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg91.h"
#include "ggg00.h"

/// defines a class to handle the after image of create table
class Log_ActionCreateTable75: public Log_ReadWriteActionImage
{

private:

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
    Log_ActionCreateTable75 ()
    : Log_ReadWriteActionImage(Log_CreateTable)
    {}

    /// creates a new table
    void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandFileId = NULL) const;

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionCreateTable75::CreateTableEntry));
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
        return false;
    }
};

#endif  /* LOG_ACTION_CREATE_TABLE75_H */
