/*!
   @file           Log_ReadWriteActionImage.hpp
   @author         JuergenA
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines a class to read/write before and after images of any action

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef LOG_READ_WRITE_ACTION_IMAGE_H
#define LOG_READ_WRITE_ACTION_IMAGE_H

/*===========================================================================*
*  INCLUDES                                                                  *
*============================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "DataAccess/Data_SplitSpaceWriter.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_IAction.hpp"

#include "ggg00.h"

/*===========================================================================*
*  EXTERNAL FUNCTIONS                                                        *
*============================================================================*/

/// utility-function for assuring that all log_entries are 8-byte aligned
inline SAPDB_UInt4 Log_AlignedImageLen (SAPDB_UInt4 Len)
{
    return ( ((Len + 7) >> 3) << 3 );
}


/*===========================================================================*
*  CLASSES                                                                   *
*============================================================================*/


//===================================================================================
/*!
    @brief          reading the image of an action
 */

class Log_ActionImageReader
{

private:
    /// real source for reading: the splitspaces
    Data_SplitSpaceReader  &m_SplitSpaceReader;

    /// allocator for dynamic memory allovation
    SAPDBMem_IRawAllocator &m_IRawAllocator;
    
public:

    /*!
       @brief          constructor
       @param          SplitSpaceReader [in out] 
       @param          IRawAllocator [in out] is used, if an image part is not contigous in a page
     */
	Log_ActionImageReader (Data_SplitSpaceReader  &SplitSpaceReader,
                           SAPDBMem_IRawAllocator &IRawAllocator):
    m_SplitSpaceReader (SplitSpaceReader),
    m_IRawAllocator    (IRawAllocator)
    { }

    /// copies ImageLen bytes from the undo or redo file to the memory specified by pImage
    bool CopyImage (SAPDB_Byte           *pImage,
                    SAPDB_UInt4           ImageLen,
                    SAPDBErr_MessageList &errlist);
    
    /// return current valid Allocator
    SAPDBMem_IRawAllocator &GetAllocator ()
    {
        return (m_IRawAllocator);
    }
        
    /*!
       @brief          locates the image onto a page of the redo/undo file or onto an allocated space
       @param          pImage [in] 
       @param          ImageLen [in] 
       @param          pUsedAllocator [out] not null, if pImage points to an allocated space
       @param          errlist [out] contains errors

       an internaly aligned ImageLen is used to locate the image

     */
    bool PersistentRead (SAPDB_Byte*             &pImage,
                         SAPDB_UInt4              ImageLen,
                         SAPDBMem_IRawAllocator* &pUsedAllocator,
                         SAPDBErr_MessageList    &errlist);
};

/// writing the image of an action
class Log_ActionImageWriter
{

private:

    /// all data is written into splitspaces
    Data_SplitSpaceWriter         &m_SplitSpaceWriter;

    /// result of the writing
    Data_SplitSpaceWriter::Result  m_WriteResult;   
    
    /// return true, if no errors detected
    bool IsResultOk () const
    {
        return (
            (Data_SplitSpaceWriter::ok                 == m_WriteResult)
            ||
            (Data_SplitSpaceWriter::moreSpaceAvailable == m_WriteResult) );
    }
    
public:

    /// constructor
    Log_ActionImageWriter (Data_SplitSpaceWriter &SplitSpaceWriter)
    : m_SplitSpaceWriter (SplitSpaceWriter),
      m_WriteResult      (Data_SplitSpaceWriter::moreSpaceAvailable)
    {}

    /// appends padding bytes, if the length of the written image is less than the aligned one
    bool AlignImage (SAPDB_UInt4           WrittenLength,
                     SAPDBErr_MessageList &errlist);

    /// return true, if the image is written completely
    bool IsWrittenCompletely () const
    {
        // return (Data_SplitSpaceWriter::ok == m_WriteResult);
        return ( IsResultOk () );
    }

    /// copies the source to the undo/redo file. If SourceLen is less than
    /// the aligned SourceLen, padding bytes are appended
    bool PersistentWrite (const void           *pSource,
                          SAPDB_UInt4           SourceLen,
                          SAPDBErr_MessageList &errlist)
    {
        if ( ! UnalignedPersistentWrite (pSource, SourceLen, errlist) )
            return false;
    
        return AlignImage (SourceLen, errlist);
    }

    /// copies the source to the undo/redo file without any alignment
    bool UnalignedPersistentWrite (const void           *pSource,
                                   SAPDB_UInt4           SourceLen,
                                   SAPDBErr_MessageList &errlist);

    /// @return the intern state of the processing
    SAPDB_Int WriteResultValue () const
    {
        return (m_WriteResult);
    }
};



//===================================================================================
/*!
    @brief          writing of before and after images of any action
    @copydoc        Log_IAction
 */

class Log_ReadWriteActionImage: public Log_IAction
{
protected:

    /// constructor
    Log_ReadWriteActionImage (Log_ActionType ActionType)
    : Log_IAction(ActionType)
    {}

    /// reads the image of the action from the reader
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist)       = 0;
    
    /// writes the image of the action to the writer
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const = 0;
    

public:

    virtual bool ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                       SAPDBMem_IRawAllocator &Allocator,
                                       SAPDBErr_MessageList   &errlist);
    
    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const;
 
    /// triggers the writing of the after image
    void WriteAfterImage (tgg00_TransContext &trans) const;

    /// triggers the writing of the before image
    void WriteBeforeImage (tgg00_TransContext &trans) const;
    /// triggers the writing of the before image
    void WriteBeforeImage (tgg00_TransContext &TransContext, tgg00_ObjTransInfo &TransInfo) const;

};
#endif  /* LOG_READ_WRITE_ACTION_IMAGE_H */
