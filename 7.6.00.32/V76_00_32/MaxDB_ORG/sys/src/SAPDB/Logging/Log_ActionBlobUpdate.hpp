/*!
  @file           Log_ActionBlobUpdate.hpp
  @author         UweH
  @ingroup        ActionHandling
  @brief          defines a class to handle log entries for binary large objects

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
#ifndef LOG_ACTION_BLOB_UPDATE_H
#define LOG_ACTION_BLOB_UPDATE_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"

/// defines an after image action that updates a binary large object
class Log_ActionBlobUpdate: public Log_ReadWriteActionImage
{
public:

    /// Defines the different existing versions of the action.
    enum
	{
        actionVersionStartingWith760011 = 1
    };
    
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionBlobUpdate::Header));
    }

    /// this constructor is designed for a further call of ReadImagePersistent
	Log_ActionBlobUpdate ()
    :Log_ReadWriteActionImage (Log_UpdateBlob)
    {}
    
    /*!
       @brief          this constructor is designed for a further call of WriteImagePersistent
       @param          TableId        [in] 
       @param          BlobFilename [in] 
       @param          InsertOffset [in] 
       @param          pBuf [in] 
       @param          BufLength [in] 
     */
    Log_ActionBlobUpdate ( tgg00_Surrogate &TableId,
                           const tgg00_Filename &BlobFilename,
                           SAPDB_UInt4           InsertOffset,
                           const SAPDB_Byte*     pBuf,
                           SAPDB_UInt4           BufLength);

	/// deallocation
    ~Log_ActionBlobUpdate ();

    /*!
       @brief          writes the buffer contents to the binary large object
       @param          TransContext [in out] 
       @param          pCommandBlobFileId [in out] in case of redo the file id has to be omitted
     */
    void ExecuteAction ( tgg00_TransContext &TransContext,
                         tgg00_FileId       *pCommandBlobFileId = NULL) const;

    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }
    virtual SAPDB_UInt GetPersistentLength () const
    {
        return m_Head.ImageLength;
    }
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const;
    virtual void WriteToTrace (const char * Title) const
    {
        if ( Title != NULL )
            Kernel_VTrace() << Title;
        Kernel_VTrace() << "ImageLength: "    << m_Head.ImageLength
                        << ", BufLength: "    << m_Head.BufLength
                        << ", InsertOffset: " << m_Head.InsertOffset;
        // Kernel_VTrace() << tgg00_Filename BlobFilename;
    }
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760011;
    }
private:
    
    /// defines the pysical structur of this action in undo/redo-files
    struct Header
    {
        SAPDB_UInt4     ImageLength;     ///< Length of the update
        SAPDB_UInt4     BufLength;       ///< Length of the copy-buffer
        SAPDB_UInt4     InsertOffset;    ///< offset where the update starts
        tgg00_Filename  BlobFilename;    ///< filename of the blob
        tgg00_Surrogate TableId;         ///< primary table id to update filedirectory statistics
    };
    
    /// in memory represeantion of this action
    Header   m_Head;

    /// pointer to a copy-buffer 
    const SAPDB_Byte       *m_pBuf;
    
    /// pointer to a copy-buffer only for reading
    SAPDB_Byte             *m_pBufForRead;

    /// allocator used for allocation the buffers
    SAPDBMem_IRawAllocator *m_pBufAllocator;
};
#endif  /* LOG_ACTION_BLOB_UPDATE_H */
