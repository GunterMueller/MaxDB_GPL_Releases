/*!
  @file           Log_ActionBlobCopySame.hpp
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



#ifndef LOG_ACTION_BLOB_COPY_SAME_H
#define LOG_ACTION_BLOB_COPY_SAME_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"

/*!
   @brief  after image action that copies a part of a blob into the same blob

   The auxiliary file will be destroyed at the end of the execution phase.
*/
class Log_ActionBlobCopySame: public Log_ReadWriteActionImage
{
public:
    
    /// Defines the different existing versions of the action.
    enum
	{
        actionVersionStartingWith760011 = 1
    };

    /// this has at least to be copied in one step
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionBlobCopySame::Header));
    }

    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionBlobCopySame ()
    :Log_ReadWriteActionImage (Log_CopySameBlob)
    {}

    /*!
       @brief          this constructor is designed for a further call of WriteImagePersistent
       @param          TableId        [in] 
       @param          BlobFilename [in] 
       @param          AuxBlobFilename [in] 
       @param          NewBlobVersion [in] 
       @param          CopyLength [in] 
       @param          SourceOffset [in] 
       @param          DestinOffset [in] 

     */

    Log_ActionBlobCopySame ( const tgg00_Surrogate &TableId,
                             const tgg00_Filename &BlobFilename,
                             const tgg00_Filename &AuxBlobFilename,
                             SAPDB_Int4            NewBlobVersion,
                             SAPDB_UInt4           CopyLength,
                             SAPDB_UInt4           SourceOffset,
                             SAPDB_UInt4           DestinOffset);

    /*!
       @brief          copies the blob into itself using an auxiliary blob
       @param          TransContext [in out] 
       @param          BlobId [in out] 
       @param          AuxBlobId [in] the auxiliary file will be created by this function

       The auxiliary file will be destroyed at the end of the execution phase.

       The blob is truncated depened on NewTruncatedBlobLen.
       The copy actions are supressed, if the blob shows the NewBlobVersion.
       The copy from the blob to the auxiliary blob is supressed, if the auxiliary
       blob has the correct length already.
       The auxiliary file will be destroyed at the end of the execution phase.
     */
    void ExecuteAction ( tgg00_TransContext   &TransContext,
                         tgg00_FileId         &BlobId,
                         tgg00_FileId         &AuxBlobId) const;
    virtual void Execute (tgg00_TransContext &TransContext) const;
    virtual SAPDB_UInt GetPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist)
    {
        return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Head)), sizeof(m_Head), errlist );
    }
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Head, sizeof (m_Head), errlist);
    }
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return actionVersionStartingWith760011;
    }

private:
    
    /// defines the pysical structur of this action in undo/redo-files
    struct Header
    {
        tgg00_Surrogate TableId;         ///< primary table id to update filedirectory statistics
        tgg00_Filename  BlobFilename;    ///< filename of the blob
        tgg00_Filename  AuxBlobFilename; ///< temporary filename used in operation
        SAPDB_Int4      NewBlobVersion;  ///< new version
        SAPDB_UInt4     CopyLength;      ///< length to be copied
        SAPDB_UInt4     SourceOffset;    ///< source offset in file
        SAPDB_UInt4     DestinOffset;    ///< destination-offset in file
    };
    
    /// in-memory representation of the action
    Header m_Head;
};

#endif  /* LOG_ACTION_BLOB_COPY_SAME_H */
