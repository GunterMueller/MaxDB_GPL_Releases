/*!
    @defgroup File Handling of database internal files
*/
/*!
  @file           Data_IFile.hpp
  @author         UweH
  @ingroup        File
  @brief          Interface to general file methods

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
#ifndef Data_IFile_HPP
#define Data_IFile_HPP

/// Interface for operations on files.

class Data_IFile
{

public:

    /// All pages referring to the file are deleted.
    virtual bool Create(SAPDBErr_MessageList &errlist) = 0;

    /// This returns true if the file has been created
    virtual bool IsCreated() const = 0;

    /// All pages referring to the file are deleted.
    virtual bool Drop(SAPDBErr_MessageList &errlist) = 0;

    /// returns true, if the file is consistent
    virtual bool Verify (bool                  isCold,
                         SAPDBErr_MessageList &errlist) = 0;

    /// This traces some basic information about the file to the trace.
    virtual void WriteToTrace (const char * title = 0) const = 0;

    /// This is used in case of an error to collect context information
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const = 0;
};
#endif // Data_IFile_HPP
