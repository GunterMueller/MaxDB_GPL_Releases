/*!
  @file           Log_ActionReorganizeFile.hpp
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
#ifndef LOG_ACTION_REORGANIZE_FILE_H
#define LOG_ACTION_REORGANIZE_FILE_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "ggg00.h"

/// defines a class to handle the after image of create file
class Log_ActionReorganizeFile: public Log_ReadWriteActionImage
{

private:
    
    /// the physical header in the undo/redo-file
    struct Head
    {
        tgg00_Filename filename;       ///< the name of the file without index (16 byte)
        SAPDB_UInt2    lastKeyLength;  ///< the length of the last key (2 byte)
        SAPDB_UInt2    filler1;        ///< filler (2 byte)
        SAPDB_UInt4    filler2;        ///< filler (4 byte)

        /// constructor
        Head()
        {
            filename.SetZero();
            filler1       = 0;
            filler2       = 0;
            lastKeyLength = 0;
        }

        /// constructor
        Head( const tgg00_Filename &newfilename,
              SAPDB_UInt2           newlastKeyLength )
        {
            filename      = newfilename;
            lastKeyLength = newlastKeyLength;
            filler1       = 0;
            filler2       = 0;
        }
    };
    
    /// the in memory representation
    Head                    m_head;

    /// pointer to the last key
    SAPDB_Byte             *m_pLastKey;

    /// allocator for dynamic memory allocation
    SAPDBMem_IRawAllocator *m_pAllocator;

public:
    
    /// brief          This is used for reading persistent images.
    Log_ActionReorganizeFile()
    : Log_ReadWriteActionImage (Log_ReorganizeFile),
      m_head(),
      m_pLastKey(0),
      m_pAllocator(0)
    {}

    /// brief          This is used for begin table load
    Log_ActionReorganizeFile( const tgg00_Filename &filename,
                         SAPDB_UInt2           lastKeyLength,
                         SAPDB_Byte           *pLastKey )
    : Log_ReadWriteActionImage (Log_ReorganizeFile),
      m_head    (filename, lastKeyLength),
      m_pLastKey(pLastKey),
      m_pAllocator(0)
    {}
    
    /// brief          deallocates allocated memory
    ~Log_ActionReorganizeFile()
    {
        if ( m_pAllocator != 0 )
            // m_pLastKey cannot be 0
            m_pAllocator->Deallocate(m_pLastKey);
    }

    /// called command
    void ExecuteAction (tgg00_TransContext &TransContext,
                        tgg00_FileId       *pCommandFileId = NULL) const;

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof(Head);
    }

    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry() + Log_AlignedImageLen(m_head.lastKeyLength);
    }

    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    
    virtual bool ReadImagePersistent (Log_ActionImageReader &reader,
                                      SAPDBErr_MessageList  &errlist)
    {
        if (! reader.CopyImage( reinterpret_cast<SAPDB_Byte*>(&m_head),sizeof(m_head),errlist ))
            return false;

        if ( m_head.lastKeyLength > 0 )
            if(!reader.PersistentRead (m_pLastKey,
                                   m_head.lastKeyLength,
                                   m_pAllocator,
                                   errlist))
                return false;
        return true;
    }

    /// write the memory-image to the writer
    virtual bool WriteImagePersistent (Log_ActionImageWriter &writer,
                                       SAPDBErr_MessageList  &errlist) const
    {
        if(!writer.PersistentWrite (&m_head, sizeof (m_head), errlist))
            return false;
        if ( m_head.lastKeyLength > 0 )
            if(!writer.PersistentWrite (m_pLastKey,m_head.lastKeyLength,errlist))
                return false;
        return true;
    }
};
#endif  /* LOG_ACTION_REORGANIZE_FILE_H */
