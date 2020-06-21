/*!
  @file            Log_ActionBlobExpand.hpp
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
#ifndef LOG_ACTION_BLOB_EXPAND_H
#define LOG_ACTION_BLOB_EXPAND_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"

/// defines an after image action that expands a binary large object by a specified byte
class Log_ActionBlobExpand: public Log_ReadWriteActionImage
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
        return Log_AlignedImageLen(sizeof(Log_ActionBlobExpand::Header));
    }

    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionBlobExpand ()
    :Log_ReadWriteActionImage (Log_ExpandBlob)
    {}

    /*!
       @brief          this constructor is designed for a further call of WriteImagePersistent
       @param          TableId        [in] 
       @param          BlobFilename [in] 
       @param          NewBlobLength [in] 
       @param          ExpandByte [in] 
     */
    Log_ActionBlobExpand ( const tgg00_Surrogate &TableId,
                           const tgg00_Filename &BlobFilename,
                           SAPDB_UInt4           NewBlobLength,
                           SAPDB_Byte            ExpandByte);

    /*!
       @brief          expands the binary large object using the specified expand byte
       @param          TransContext [in out] 
       @param          pCommandBlobFileId [in out] in case of redo the file id has to be omitted
     */
    void ExecuteAction ( tgg00_TransContext &TransContext,
                         tgg00_FileId       *pCommandBlobFileId = 0) const;

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
        return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Head)), sizeof(m_Head), errlist);
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
        SAPDB_UInt4     NewBlobLength;   ///< new length
        SAPDB_Byte      ExpandByte;      ///< byte used for filling
    };
    
    /// in memory representation
    Header m_Head;
};
#endif  /* LOG_ACTION_BLOB_EXPAND_H */
