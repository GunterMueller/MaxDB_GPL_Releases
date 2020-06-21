/*!
  @file           Data_BaseFile.hpp
  @author         UweH
  @ingroup        File
  @brief          general file methods

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
#ifndef Data_BaseFile_HPP
#define Data_BaseFile_HPP

#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_IFile.hpp"
#include "DataAccess/Data_FileTypes.hpp"

/// common file access methods

class Data_BaseFile : public Data_IFile
{

public:

    /// default contructor
    Data_BaseFile ( const Data_FileType     type,
                    const Data_PageId       rootId,
                    Data_PageAccessManager  pam )
        : m_Type              (type),
          m_RootId            (rootId),
          m_PageAccessManager (pam)
    {}

    /// returns the files type
    Data_FileType GetType() const
    {
        return m_Type;
    }

    /*!
        @returns Data_PageId - of the root
        @brief   This returns the Id of the root page of this file.
        More over this specifies what kind of recovery mode pages of the file are.
     */
    const Data_PageId& GetRootId() const
    {
        return m_RootId;
    }

    /// returns the used page access manager
    Data_PageAccessManager& PageAccessManager()
    {
        return m_PageAccessManager;
    }

    /// This traces some basic information about the file to the trace.
    virtual void WriteToTrace (const char * title = 0) const;
    
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;

    /// This returns true if the file has been created
    virtual bool IsCreated() const
    {
        return m_RootId.PageNo().IsValid();
    }

protected:

    /// Containes the type of the file.
    Data_FileType          m_Type;
	/// This identifes the first page in the container of this file.
    Data_PageId            m_RootId;
	/// This is the handle to access the pages of the file.
    Data_PageAccessManager m_PageAccessManager;
};
#endif // Data_BaseFile_HPP
