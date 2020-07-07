/*!
   @file           Log_ActionMultiple.hpp
   @author         UweH
   @ingroup        ActionHandling
   @brief          a class for bundling actions for blocked writing to the log

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
#ifndef Log_ActionMultiple_H
#define Log_ActionMultiple_H

#include "ggg00.h"  // tgg00_TransContext, tgg00_FileId
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_IAction.hpp"

class Log_AfterImage;

/// this is a simple buffer to write into
class Data_SplitSpaceBuffer : public Data_ISplitSpace
{
public:
    /// standard ctor
    Data_SplitSpaceBuffer()
    : m_pAllocator      (0),
      m_Buffer          (0),
      m_BufferSize      (0),
      m_FirstFreeOffset (0),
      m_pReservedSpace  (0),
      m_ReservedSize    (0)
    {}
    /// standard dtor (calls Deassign())
    ~Data_SplitSpaceBuffer()
    {
        Delete();
    }
    /// allocates memory
    bool Initialize(SAPDBMem_IRawAllocator &allocator,
                    SAPDB_UInt              size)
    {
        Delete();
        m_Buffer = reinterpret_cast<SAPDB_Byte*>(allocator.Allocate(size));
        if ( m_Buffer != 0 )
        {
            m_pAllocator = &allocator;
            m_BufferSize =  size;
            return true;
        }
        return false;
    }
    /// resets the buffer to its initial empty state
    void Reset()
    {
        Release();
        m_FirstFreeOffset = 0;
    }
    /// deallocates all memory
    void Delete();
    /// returns the pointer to the memory begin for reading
    const SAPDB_Byte* GetBufferPointer() const
    {
        return m_Buffer;
    }
    /*!
    @param usedSize [in] this give size is internal stored
    @return pointer to memory to be written or 0 on error
    @brief returns the pointer to the memory begin for writing
    
    If usedSize is too big for the internal buffer 0 is returned.
    This can be if Initialize() was called was a too small size.
    */
    SAPDB_Byte* GetBufferPointer(SAPDB_UInt usedSize)
    {
        if ( m_BufferSize < usedSize )
            return 0;
        m_FirstFreeOffset = usedSize;
        return m_Buffer;
    }
    /// returns the used buffer size
    SAPDB_UInt GetUsedBufferSize() const
    {
        return m_FirstFreeOffset;
    }
    /// returns true if buffer is not used
    bool IsEmpty() const
    {
        return 0 == m_FirstFreeOffset;
    }
    /// returns true if given size can be allocated in the buffer
    bool Reserve (SAPDB_UInt size)
    {
        if ( m_BufferSize - 1 - m_FirstFreeOffset >= size )
        {
            m_pReservedSpace = m_Buffer + m_FirstFreeOffset;
            m_ReservedSize     = size;
            m_FirstFreeOffset += size;
            return true;
        }
        return false;
    }
    /// releases currently reserved space
    void Release()
    {
        m_pReservedSpace = 0;
        m_ReservedSize   = 0;
    }
    virtual bool IsAssigned () const
    {
        return m_Buffer != 0;
    }
    virtual Data_ISplitSpace::PartNo CurrentPart () const
    {
        return 0;
    }
    virtual void GetPart (PartNo                    Part,
                          SAPDB_UInt               &Length,
                          SAPDB_Byte*              &Value) const
	{
        Length = m_ReservedSize;
        Value  = m_pReservedSpace;
	}
    virtual SAPDB_UInt Length() const
    {
        return m_BufferSize;
    }
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const
	{
	}
    virtual bool CheckAlignment() const
    {
         if ( m_pReservedSpace == 0 )
             return true;
             
         SAPDB_UInt8 address =
                         #ifdef BIT64
                         reinterpret_cast<SAPDB_UInt8>(m_pReservedSpace);
                         #else
                         reinterpret_cast<SAPDB_UInt4>(m_pReservedSpace);
                         #endif
         
         return address % 4/*LOG_PAGE_ALIGNMENT*/ == 0;
    }
private:
    /// used to allocate and deallocate the buffer
    SAPDBMem_IRawAllocator *m_pAllocator;
    /// local buffer used to store the logentries
    SAPDB_Byte *m_Buffer;
    /// size of allocated buffer
    SAPDB_UInt  m_BufferSize;
    /// offset used to append new contents
    SAPDB_UInt  m_FirstFreeOffset;
    /// currently reserved size
    SAPDB_UInt  m_ReservedSize;
    /// pointer to reserved space
    SAPDB_Byte *m_pReservedSpace;
};

/// this actions contains other actions
class Log_ActionMultiple : public Log_IAction
{
public:
    virtual SAPDB_UInt GetMinimalPersistentLength() const
    {
        return GetMinimalLengthToCopyEntry();
    }
    virtual SAPDB_UInt GetPersistentLength() const
    {
        return GetMinimalPersistentLength() + m_Head.totalLengthOfActions;
    }
    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const;
    virtual bool ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                       SAPDBMem_IRawAllocator &Allocator,
                                       SAPDBErr_MessageList   &errlist);
    virtual void WriteToTrace (const char * Title) const;
    virtual bool LocalRedoEntryCachingPossible() const
    {
        return false;
    }
    /// this is used to create an empty buffer
    Log_ActionMultiple()
    : Log_IAction(Log_MultipleActions)
    {}
    /// this is used for copying this entry
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof (Head);
    }
    /// allocate memory for appending actions
    bool Initialize (SAPDBMem_IRawAllocator &allocator,
                     SAPDB_UInt              wantedSize)
	{
        return m_Buffer.Initialize(allocator, wantedSize);
	}
    /// this returns the internally used buffer
    Data_ISplitSpace &GetBuffer()
    {
        return m_Buffer;
    }
    /*!
    @param pActionHead [in] is used to assign local header
    @param reserveSize [in] is used to reserve the space in the buffer
    @return false on any error
    @brief The internal buffer is prepared to store the new read MultipleAction
    */
    bool Reserve( SAPDB_Byte *pActionHead, 
                  SAPDB_UInt  reserveSize)
	{
        if ( 0 == pActionHead )
            return false;
        m_Head = *(reinterpret_cast<Head*>(pActionHead));
        if ( m_Head.totalLengthOfActions != reserveSize - sizeof(Head) )
            return false;
        return m_Buffer.Reserve(m_Head.totalLengthOfActions);
	}
    /// releases reserved buffer space
    void Release()
    {
        m_Buffer.Release();
    }
    /// resets the local buffer to its initial empty state
    void Reset()
    {
        m_Buffer.Reset();
        m_Head.actionCount          = 0;
        m_Head.totalLengthOfActions = 0;
    }
    /// deallcoates all memory
    void Delete()
    {
        m_Buffer.Delete();
    }
    /// returns true if no actions were appended
    bool IsEmpty() const
    {
        return 0 == m_Head.actionCount;
    }
    /// This type defines possible results for Append()
    enum AppendResult
    {
        appendOk,
        appendError,
        appendBufferIsFull
    };
    /*!
    @param redoSequence [in] transaction local redo log sequence
    @param unddoSequence [in] transaction local undo log sequence
    @param action [in] the action which should be written to the redo log
    @param errlist [out] message in case of an error
    @return AppendResult
    @brief The redo action is appended to the local buffer.
    */
    AppendResult Append (Log_EntrySequence     redoSequence,
                         Log_EntrySequence     undoSequence,
                         const Log_IAction    &action,
                         SAPDBErr_MessageList &errlist);
    /*!
    @class Iterator
    @brief This class defines a pointer to the stored actions
    */
    class Iterator
    {
	public:
        /// default ctor
        Iterator(const Data_SplitSpaceBuffer &buffer)
        : m_Buffer        (buffer),
          m_CurrentOffset (0)
        {}
        /// set pointer to next element, if there is one true is returned
        bool Next();
        /// if the iterator points to a valid action the data is assigned and true is returned
        bool GetAction( Log_AfterImage      &image,
                        const tgg91_TransNo &currentTransNo,
                        SAPDB_UInt          &actionLength,
                        const SAPDB_Byte*   &pActionSpace) const;
        /// returns true if the iterator actually points to a valid action
        bool IsValid() const
        {
            return m_CurrentOffset < m_Buffer.GetUsedBufferSize() - sizeof(ActionHeader);
        }
	private:
        const Data_SplitSpaceBuffer &m_Buffer;
        SAPDB_UInt                   m_CurrentOffset;
    };
    /// returns an iterator which points to the stored actions
    Log_ActionMultiple::Iterator GetFirstAction() const
    {
        return Log_ActionMultiple::Iterator(m_Buffer);
    }
    /// returns the size of the internal header
    static SAPDB_UInt GetHeaderSize()
    {
        return sizeof(Head);
    }
// --------------------------------------------------------------------------------
// SHOULD BE PRIVATE BUT SUN COMPILER CANNOT ACCES PRIVATE TYPES IN PUBLIC METHODES
public:
// --------------------------------------------------------------------------------
    /// This header is written before every persistent action in the body of this entry
    struct ActionHeader
    {
        SAPDB_UInt4       length;           ///< the length of this single entry
        SAPDB_UInt1       actionType;       ///< the kind of action (Log_ActionType) of this single entry
        SAPDB_UInt1       actionVersion;    ///< the version of the action
        SAPDB_UInt2       filler2;          ///< filler in order to align this header
        Log_EntrySequence redosequence;     ///< The redosequence in this transaction of this entry
        Log_EntrySequence undosequence;     ///< the undosequence in this transaction of this entry
        
        /// constructor
        ActionHeader(SAPDB_UInt4       newlength,
                     Log_ActionType    newtype,
                     Log_ActionVersion version,
                     Log_EntrySequence newredosequence,
                     Log_EntrySequence newundosequence)
        {
            if ( newtype > SAPDB_MAX_UINT1 )
                Crash();
            length        = newlength;
            actionType    = newtype;
            actionVersion = version;
            filler2       = 0;
            redosequence  = newredosequence;
            undosequence  = newundosequence;
        }
        /// forces a kernel-crash with the message that the actiontype is invalid
        void Crash();
	};
    /// header definition
    struct Head
    {
        SAPDB_UInt4 actionCount;            ///< the number of Logentries in this multiple action
        SAPDB_UInt4 totalLengthOfActions;   ///< the total length of all actions in this multiple action
        Head()                              ///< standard constructor
        : actionCount(0),
          totalLengthOfActions(0)
        {}
    };
    /// header
    Head m_Head;
    /// split space buffer used to write appended actions to a buffer
    Data_SplitSpaceBuffer m_Buffer;
};
#endif  /* Log_ActionMultiple_H */
