/*!
  @file           Log_ActionObjectFile.hpp
  @ingroup        ActionHandling
  @author         UweH
  @brief          defines class Log_ActionObjectFile

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



#ifndef Log_ActionObjectFile_H
#define Log_ActionObjectFile_H

#include "ggg00.h"  // tgg00_TransContext, tgg00_FileId
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_IAction.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "liveCache/LVC_Types.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "FileDirectory/FileDir_Types.hpp"  

class Log_AfterImage;
class Log_IImage;
class Data_SplitSpaceWriter;
class Data_SplitSpaceReader;
 
/// Handle for manipulating OMS objects.
class Log_ActionObjectFile : public Log_ReadWriteActionImage
{

public:

    /// abstract fileno type
    typedef FileDir_FileNo  FileNo;

    /// default constructor
    Log_ActionObjectFile()
    : Log_ReadWriteActionImage (Log_NoOp),
      m_pAllocator                       (0),
      m_pAllocatedPersistentSecFileIdList(0),
      m_pPersistentSecList               (0)
    {}
    
    /// deallocates if necessary
    ~Log_ActionObjectFile()
    {
        if ( m_pAllocator != 0 )
            m_pAllocator->Deallocate(m_pAllocatedPersistentSecFileIdList);
    }
    /// init the creation of a file 
    void InitCreation (FileDir_FileNo            &fileno,
                       Data_FileType              filetype,
                       SAPDB_UInt                 bodyLength,
                       SAPDB_UInt                 keyLength,
                       Data_PageNo                rootPage)
    {
        m_Type                     = Log_CreateObjectFile;
        m_Head.fileno              = fileno;
        m_Head.filetype            = filetype;
        m_Head.rootPage            = rootPage;
        m_Head.bodyLength          = bodyLength;
        m_Head.keyLength           = keyLength;
        m_Head.filler2             = 0;
        m_Head.filler1             = 0;
        m_Head.secListLength       = 0;
    }

    /// Appends the root/subroot list to this action
    void AppendFileNoList(const LVC_FileNoArray  &filenoArr)
    {
        m_Head.secListLength       = filenoArr.GetPersistentLength();       
        m_pPersistentSecList       = filenoArr.GetPersistentFormat();
    }

    /// init the deletion of a file
    void InitDeletion (FileNo fileno)
    {
        m_Type                     = Log_DropObjectFile;
        m_Head.fileno              = fileno;
        m_Head.filetype            = Data_OmsFile;
        m_Head.rootPage.Invalidate();
        m_Head.bodyLength          = 0;
        m_Head.keyLength           = 0;
        m_Head.filler2             = 0;
        m_Head.filler1             = 0;
        m_Head.secListLength       = 0;
    }
    /// get the minimum-entry-size
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof (Head);
    }
    /// is used by garbage collection UH 2004-06-01
    bool RemoveGarbage (tgg00_TransContext   &trans,
                        SAPDBErr_MessageList &errlist);
    virtual void Undo (tgg00_TransContext &TransContext, Log_IImage &Image) const;
    virtual void Redo (tgg00_TransContext &TransContext, Log_IImage &Image, SAPDBErr_MessageList &Errlist) const;
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const;
    virtual SAPDB_UInt GetMinimalPersistentLength() const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual SAPDB_UInt GetPersistentLength() const
    {
        return GetMinimalLengthToCopyEntry()
               +
               m_Head.secListLength;
    }
    virtual void WriteToTrace (const char * Title) const;
    virtual bool GarbageCollectionNeeded() const
    {
        return true;
    }
private:

    /// the physical representation in undo/redo-files

    struct Head
    {
        FileNo        fileno;                ///< 8 filenumber of the objectfile
        Data_PageNo   rootPage;              ///< 4 root page of this file
        SAPDB_UInt2   bodyLength;            ///< 2 length of the body
        SAPDB_UInt2   keyLength;             ///< 2 length of the key
        Data_FileType filetype;              ///< 1 filetype of the objectfile
        SAPDB_UInt1   filler1;               ///< 1 filler
        SAPDB_UInt2   secListLength;         ///< 2 length of sec root/fileno list
        SAPDB_UInt4   filler2;               ///< 4 filler
    };


    /// the in-memory representation of the header of this action
    Head                    m_Head;
    /// allocator for reading
    SAPDBMem_IRawAllocator *m_pAllocator;
    /// reference to allocated key file id list
    SAPDB_Byte      *m_pAllocatedPersistentSecFileIdList;
    /// reference to key file id list
    const SAPDB_Byte *m_pPersistentSecList;
};


#endif  /* Log_ActionObjectFile_H */
