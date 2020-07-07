/*!
   @file           Log_ActionDropFile75.hpp
   @author         JuergenA
   @author         UweH
   @author         MartinKi
   @ingroup        ActionHandling
   @brief          defines a class to handle drop file log entries of releases < 7.6

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
#ifndef LOG_ACTION_DROP_FILE75_H
#define LOG_ACTION_DROP_FILE75_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg91.h"
#include "ggg00.h"


/// defines a class to handle the before image of any create except create object
class Log_ActionDropFile75: public Log_ReadWriteActionImage
{

private:

    /// defines the physical structure of this action in undo/redo-files
    tgg00_Filename m_Filename;

public:
    /// this constructor is designed for a further call of ReadImagePersistent
	Log_ActionDropFile75 ()
    : Log_ReadWriteActionImage (Log_CreateFile)
    {}

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
        return false;
    }
};

#endif  /* LOG_ACTION_DROP_FILE75_H */
