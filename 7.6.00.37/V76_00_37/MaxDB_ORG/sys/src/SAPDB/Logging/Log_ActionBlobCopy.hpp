/*!
   @file           Log_ActionBlobCopy.hpp
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines Log_ActionBlobCopy

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



#ifndef LOG_ACTION_BLOB_COPY_H
#define LOG_ACTION_BLOB_COPY_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"

/*!
   @brief   before/after image action that copies a part of a blob to another blob

   If the new length of the blob is less than the old length, the blob is
   truncated.  In case of before image, the source object consists of an
   auxiliary file. The auxiliary file has to be destroyed at the end of
   the execution phase.
*/

class Log_ActionBlobCopy: public Log_ReadWriteActionImage
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
        return Log_AlignedImageLen(sizeof(Header));
    }
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionBlobCopy ()
    : Log_ReadWriteActionImage (Log_CopyBlob)
    {}

    /*!
       @brief   this constructor is designed for a further call of WriteImagePersistent
       @param   TableId        [in] 
       @param   SourceFilename [in] 
       @param   DestinFilename [in] 
       @param   DestinVersion  [in] 
       @param   CopyLength     [in] 
       @param   SourceOffset   [in] 
       @param   DestinOffset   [in] 
       @param   NewTruncatedBlobLen [in] if greater 0, the blob will be truncated to this length
     */
    Log_ActionBlobCopy ( const tgg00_Surrogate &TableId,
                         const tgg00_Filename  &SourceFilename,
                         const tgg00_Filename  &DestinFilename,
                         SAPDB_Int4             DestinVersion,
                         SAPDB_UInt4            CopyLength,
                         SAPDB_UInt4            SourceOffset,
                         SAPDB_UInt4            DestinOffset,
                         SAPDB_UInt4            NewTruncatedBlobLen);
    /*!
       @brief          copies the blob and truncates the destination blob
                       if necessary
       @param          TransContext [in out] 
       @param          pSourceCommandFileId [in out] in case of undo/redo the file id has to be omitted
       @param          pDestinCommandFileId [in out] in case of undo/redo the file id has to be omitted

                       If the source object consists of an auxiliary file,
                       the file will be destroyed.
     */
    void ExecuteAction ( tgg00_TransContext &TransContext,
                         tgg00_FileId       *pSourceCommandFileId = 0,
                         tgg00_FileId       *pDestinCommandFileId = 0) const;
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
                                      SAPDBErr_MessageList  &errlist)
    {
        return ImageReader.CopyImage ( REINTERPRET_CAST (SAPDB_Byte*, &(m_Head)), sizeof(m_Head), errlist );
    }
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist) const
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
        tgg00_Filename  SourceFilename;  ///< source of the copy-operation
        tgg00_Filename  DestinFilename;  ///< destination of the operation   
        SAPDB_Int4      DestinVersion;   ///< new Version 
        SAPDB_UInt4     CopyLength;      ///< length to be copied
        SAPDB_UInt4     SourceOffset;    ///< offset of the source
        SAPDB_UInt4     DestinOffset;    ///< offset of the destination
        SAPDB_UInt4     NewTruncatedDestinLen;   ///<new filelen, if trancation needed
    };
    
    /// the in-memory representation 
    Header m_Head;
};
#endif  /* LOG_ACTION_BLOB_COPY_H */
