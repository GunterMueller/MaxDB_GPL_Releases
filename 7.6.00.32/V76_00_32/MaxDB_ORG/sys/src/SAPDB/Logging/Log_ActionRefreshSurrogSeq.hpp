/*!
   @file           Log_ActionRefreshSurrogSeq.hpp
   @author         JuergenA
   @author         UweH
   @ingroup        ActionHandling
   @brief          defines a class to handle log entries for surrogates and sequences

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



#ifndef LOG_ACTION_REFRESH_SURROG_SEQ_H
#define LOG_ACTION_REFRESH_SURROG_SEQ_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "gsp00.h"
#include "ggg00.h"

/// defines a class to handle the after image of refresh surrogate
class Log_ActionRefreshSurrogate: public Log_ReadWriteActionImage
{

private:
    
    /// physical representation in the Redo/Undo-File
    struct RefreshSurrogateEntry
    {
        tgg00_Surrogate Surrogate;      /// refreshed surrogate 
        tgg00_Surrogate SysKey;         /// Syskey
    };
    
    /// in memory representation
    RefreshSurrogateEntry m_Entry;

public:
    
    /// this constructor is designed for a further call of ReadImagePersistent
    Log_ActionRefreshSurrogate ()
    : Log_ReadWriteActionImage (Log_RefreshSurrogate)
    {}

    /// this constructor is designed for a further call of ReadImagePersistent

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionRefreshSurrogate (
        const tgg00_Surrogate &Surrogate,
        const tgg00_Surrogate &SysKey);

    /// redo only: refreshes the last surrogate and the last system key
    void Execute (tgg00_TransContext &TransContext) const;
    
    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen(sizeof(Log_ActionRefreshSurrogate::RefreshSurrogateEntry));
    }

    virtual bool ForceLogWriting () const
    {
        return true;
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
                                      SAPDBErr_MessageList  &errlist);
    
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const
    {
        return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
    }
    
};

/// defines a class to handle the after image of refresh sequence
class Log_ActionRefreshSequence: public Log_ReadWriteActionImage
{
public:
    
    /// persistent structure used by PersistentRead/Write and in memory
    struct SequenceItem
    {
        tgg00_Surrogate Surrogate;          ///< this denotes the sequence
        tsp00_Number    Number;             ///< this is the value of the above sequence
    };
    
    
private:
    
    SAPDB_Int4              m_SequenceCount;
    const SequenceItem     *m_pSequenceList;
    
    SequenceItem           *m_pSequenceListForRead;
    SAPDBMem_IRawAllocator *m_pSequenceListAllocator;
    
public:
    
	Log_ActionRefreshSequence()
    :Log_ReadWriteActionImage (Log_RefreshSequence),
     m_SequenceCount          (0),
     m_pSequenceList          (0),
     m_pSequenceListForRead   (0),
     m_pSequenceListAllocator (0)
    {}

    /// this constructor is designed for a further call of WriteImagePersistent
    Log_ActionRefreshSequence (SAPDB_Int4  SequenceCount,
                               const void *pSequenceList)
	:Log_ReadWriteActionImage (Log_RefreshSequence),
     m_SequenceCount          (SequenceCount),
     m_pSequenceList          (reinterpret_cast<const SequenceItem*>(pSequenceList)),
     m_pSequenceListForRead   (0),
     m_pSequenceListAllocator (0)
    {}

    /// deallocation if necessary
    ~Log_ActionRefreshSequence ();

    /// redo only: refreshes the sequences
    void Execute (tgg00_TransContext &TransContext) const;
    

    /// this length is used for redo copy
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return Log_AlignedImageLen (sizeof(Log_ActionRefreshSequence::SequenceItem));
    }

    virtual bool ForceLogWriting () const
    {
        // PTS 1117320 UH 2002-10-28 added
        return true;
    }

    virtual SAPDB_UInt GetPersistentLength () const
    {
        return ( Log_AlignedImageLen (sizeof(m_SequenceCount))
            +    Log_AlignedImageLen (m_SequenceCount * sizeof(SequenceItem)) );
    }

    virtual SAPDB_UInt GetMinimalPersistentLength () const
    {
        return GetMinimalLengthToCopyEntry();
    }
    
    virtual bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                      SAPDBErr_MessageList  &errlist);
    
    virtual bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                       SAPDBErr_MessageList  &errlist)       const;
};

#endif  /* LOG_ACTION_REFRESH_SURROG_SEQ_H */
