/*!
  @file           Log_ActionCopyUpdBlob75.hpp
   @author         JuergenA
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



#ifndef LOG_ACTION_COPY_UPD_BLOB_75_H
#define LOG_ACTION_COPY_UPD_BLOB_75_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


//===================================================================================
/*!
   @deprecated only for compatibility (undo/redo log)
   @brief   before/after image action that copies a part of a blob to another blob

   If the new length of the blob is less than the old length, the blob is
   truncated.  In case of before image, the source object consists of an
   auxiliary file. The auxiliary file has to be destroyed at the end of
   the execution phase.
*/

class Log_ActionCopyBlob75: public Log_ReadWriteActionImage
{

private:
    
    /// defines the pysical structur of this action in undo/redo-files
    struct CopyBlobEntry
    {
        tgg00_Filename SourceFilename;  ///< source of the copy-operation
        tgg00_Filename DestinFilename;  ///< destination of the operation   
        SAPDB_Int4     DestinVersion;   ///< new Version 
        SAPDB_UInt4    CopyLength;      ///< length to be copied
        SAPDB_UInt4    SourceOffset;    ///< offset of the source
        SAPDB_UInt4    DestinOffset;    ///< offset of the destination
        SAPDB_UInt4    NewTruncatedDestinLen;   ///<new filelen, if trancation needed
    };
    
    /// the in-memory representation 
    CopyBlobEntry m_Entry;

public:
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionCopyBlob75 ()
    : Log_ReadWriteActionImage (Log_CopyBlob)
    {}

    /*!
       @brief   this constructor is designed for a further call of WriteImagePersistent
       @param   SourceFilename [in] 
       @param   DestinFilename [in] 
       @param   DestinVersion [in] 
       @param   CopyLength [in] 
       @param   SourceOffset [in] 
       @param   DestinOffset [in] 
       @param   NewTruncatedBlobLen [in] if greater 0, the blob will be truncated to this length
     */
    Log_ActionCopyBlob75 (
        const tgg00_Filename &SourceFilename,
        const tgg00_Filename &DestinFilename,
        SAPDB_Int4            DestinVersion,
        SAPDB_UInt4           CopyLength,
        SAPDB_UInt4           SourceOffset,
        SAPDB_UInt4           DestinOffset,
        SAPDB_UInt4           NewTruncatedBlobLen);
    
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionCopyBlob75::CopyBlobEntry));
    }
    /*!
       @brief          copies the blob and truncates the destination blob
                       if necessary
       @param          TransContext [in out] 

                       If the source object consists of an auxiliary file,
                       the file will be destroyed.
     */
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }
    /*!
       @brief          copies the blob and truncates the destination blob
                       if necessary
       @param          TransContext [in out] 
       @param          pSourceCommandFileId [in out] in case of undo/redo the file id has to be omitted
       @param          pDestinCommandFileId [in out] in case of undo/redo the file id has to be omitted

                       If the source object consists of an auxiliary file,
                       the file will be destroyed.
     */
    virtual void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pSourceCommandFileId = NULL,
        tgg00_FileId       *pDestinCommandFileId = NULL) const;
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
                                       SAPDBErr_MessageList  &errlist) const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
};



//===================================================================================
/*!
   @deprecated only for compatibility (undo/redo log)
   @brief  after image action that copies a part of a blob into the same blob

   The auxiliary file will be destroyed at the end of the execution phase.
*/
class Log_ActionCopySameBlob75: public Log_ReadWriteActionImage
{

private:
    
    /// defines the pysical structur of this action in undo/redo-files
    struct CopyBlobEntry
    {
        tgg00_Filename BlobFilename;    ///< filename of the blob
        tgg00_Filename AuxBlobFilename; ///< temporary filename used in operation
        SAPDB_Int4     NewBlobVersion;  ///< new version
        SAPDB_UInt4    CopyLength;      ///< length to be copied
        SAPDB_UInt4    SourceOffset;    ///< source offset in file
        SAPDB_UInt4    DestinOffset;    ///< destination-offset in file
    };
    
    /// in-memory representation of the action
    CopyBlobEntry m_Entry;

public:
    
	/// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionCopySameBlob75 ()
    :Log_ReadWriteActionImage (Log_CopySameBlob)
    {}

    /*!
       @brief          this constructor is designed for a further call of WriteImagePersistent
       @param          BlobFilename [in] 
       @param          AuxBlobFilename [in] 
       @param          NewBlobVersion [in] 
       @param          CopyLength [in] 
       @param          SourceOffset [in] 
       @param          DestinOffset [in] 

     */

    Log_ActionCopySameBlob75 (
        const tgg00_Filename &BlobFilename,
        const tgg00_Filename &AuxBlobFilename,
        SAPDB_Int4            NewBlobVersion,
        SAPDB_UInt4           CopyLength,
        SAPDB_UInt4           SourceOffset,
        SAPDB_UInt4           DestinOffset);

    /*!
       @brief          redo the copy of a blob into the same blob using an auxiliary blob.
       @param          TransContext [in out] 

       The blob is truncated depened on NewTruncatedBlobLen.
       The copy actions are supressed, if the blob shows the NewBlobVersion.
       The copy from the blob to the auxiliary blob is supressed, if the auxiliary
       blob has the correct length already.
       The auxiliary file will be destroyed at the end of the execution phase.

     */
    virtual void Execute (tgg00_TransContext &TransContext) const;

    /*!
       @brief          copies the blob into itself using an auxiliary blob
       @param          TransContext [in out] 
       @param          BlobId [in out] 
       @param          AuxBlobId [in] the auxiliary file will be created by this function

       The auxiliary file will be destroyed at the end of the execution phase.
     */
    virtual void ExecuteAction (
        tgg00_TransContext   &TransContext,
        tgg00_FileId         &BlobId,
        tgg00_FileId         &AuxBlobId) const;

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    /// this has at least to be copied in one step
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionCopySameBlob75::CopyBlobEntry));
    }
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
};

/*!
@deprecated only for compatibility (undo/redo log)
@brief defines an after image action that expands a binary large object by a specified byte
*/
class Log_ActionExpandBlob75: public Log_ReadWriteActionImage
{

private:
    
    /// defines the pysical structur of this action in undo/redo-files
    struct ExpandBlobEntry
    {
        tgg00_Filename BlobFilename;    ///< filename of the blob
        SAPDB_UInt4    NewBlobLength;   ///< new length
        SAPDB_Byte     ExpandByte;      ///< byte used for filling
    };
    
    /// in memory representation
    ExpandBlobEntry m_Entry;
    
public:
    
    /*!
       @brief          this constructor is designed for a further call of ReadImagePersistent
     */
    Log_ActionExpandBlob75 ()
    :Log_ReadWriteActionImage (Log_ExpandBlob)
    {}

    /*!
       @brief          this constructor is designed for a further call of WriteImagePersistent
       @param          BlobFilename [in] 
       @param          NewBlobLength [in] 
       @param          ExpandByte [in] 
     */
    Log_ActionExpandBlob75 (
        const tgg00_Filename &BlobFilename,
        SAPDB_UInt4           NewBlobLength,
        SAPDB_Byte            ExpandByte);

    /*!
       @brief          expands the binary large object using the specified expand byte
       @param          TransContext [in out] 
     */
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }

    /*!
       @brief          expands the binary large object using the specified expand byte
       @param          TransContext [in out] 
       @param          pCommandBlobFileId [in out] in case of redo the file id has to be omitted
     */
    virtual void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandBlobFileId = NULL) const;

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionExpandBlob75::ExpandBlobEntry));
    }
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
};

/*!
@deprecated only for compatibility (undo/redo log)
@brief defines a before or after image action that truncates a binary large object
*/
class Log_ActionTruncateBlob75: public Log_ReadWriteActionImage
{

private:
    
    /// defines the pysical structur of this action in undo/redo-files
    struct TruncateBlobEntry
    {
        tgg00_Filename BlobFilename;        ///< filename of the blob
        SAPDB_Int4     BlobVersion;         ///< version of the blob
        SAPDB_UInt4    NewTruncatedBlobLen; ///< new length
    };
    
    /// in memory representation
    TruncateBlobEntry m_Entry;

public:
    
    /*!
       @brief          this constructor is designed for a further call of ReadImagePersistent
     */
    Log_ActionTruncateBlob75 ()
    :Log_ReadWriteActionImage (Log_TruncateBlob)
    {}

    /*!
       @brief          this constructor is designed for a further call of WriteImagePersistent
       @param          BlobFilename [in] 
       @param          BlobVersion [in] 
       @param          NewTruncatedBlobLen [in] the blob will be truncated to this length
     */
    Log_ActionTruncateBlob75 (
        const tgg00_Filename &BlobFilename,
        SAPDB_Int4            BlobVersion,
        SAPDB_UInt4           NewTruncatedBlobLen);

    /*!
       @brief          truncates the binary large object
       @param          TransContext [in out] 
     */
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }

    /*!
       @brief          truncates the binary large object
       @param          TransContext [in out] 
       @param          pCommandBlobFileId [in out] in case of undo/redo the file id has to be omitted
     */
    virtual void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandBlobFileId = NULL) const;
    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionTruncateBlob75::TruncateBlobEntry));
    }
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
};

/*!
@deprecated only for compatibility (undo/redo log)
@brief defines an after image action that updates a binary large object
*/
class Log_ActionUpdateBlob75: public Log_ReadWriteActionImage
{

private:
    
    /// defines the pysical structur of this action in undo/redo-files
    struct UpdateBlobEntryHeader
    {
        SAPDB_UInt4    ImageLength;     ///< Length of the update
        SAPDB_UInt4    BufLength;       ///< Length of the copy-buffer
        SAPDB_UInt4    InsertOffset;    ///< offset where the update starts
        tgg00_Filename BlobFilename;    ///< filename of the blob
    };
    
    /// in memory represeantion of this action
    UpdateBlobEntryHeader   m_EntryHeader;

    /// pointer to a copy-buffer 
    const SAPDB_Byte       *m_pBuf;
    
    /// pointer to a copy-buffer only for reading
    SAPDB_Byte             *m_pBufForRead;

    /// allocator used for allocation the buffers
    SAPDBMem_IRawAllocator *m_pBufAllocator;

public:
	/// this constructor is designed for a further call of ReadImagePersistent
	Log_ActionUpdateBlob75 ()
    :Log_ReadWriteActionImage (Log_UpdateBlob)
    {}
    
    /*!
       @brief          this constructor is designed for a further call of WriteImagePersistent
       @param          BlobFilename [in] 
       @param          InsertOffset [in] 
       @param          pBuf [in] 
       @param          BufLength [in] 
     */
    Log_ActionUpdateBlob75 (
        const tgg00_Filename &BlobFilename,
        SAPDB_UInt4           InsertOffset,
        const SAPDB_Byte*     pBuf,
        SAPDB_UInt4           BufLength);

	/// deallocation
    ~Log_ActionUpdateBlob75 ();

    /*!
       @brief          writes the buffer contents to the binary large object
       @param          TransContext [in out] 
     */
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        ExecuteAction (TransContext);
    }

    /*!
       @brief          writes the buffer contents to the binary large object
       @param          TransContext [in out] 
       @param          pCommandBlobFileId [in out] in case of redo the file id has to be omitted
     */
    virtual void ExecuteAction (
        tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandBlobFileId = NULL) const;

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return m_EntryHeader.ImageLength;
    }
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionUpdateBlob75::UpdateBlobEntryHeader));
    }
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    
    /// brief          writes the image of the action
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const;
    virtual void WriteToTrace (const char * Title) const
    {
        if ( Title != NULL )
            Kernel_VTrace() << Title;
        Kernel_VTrace() << "ImageLength: "    << m_EntryHeader.ImageLength
                        << ", BufLength: "    << m_EntryHeader.BufLength
                        << ", InsertOffset: " << m_EntryHeader.InsertOffset;
        // Kernel_VTrace() << tgg00_Filename BlobFilename;
    }
};
#endif  /* LOG_ACTION_COPY_UPD_BLOB_75_H */
