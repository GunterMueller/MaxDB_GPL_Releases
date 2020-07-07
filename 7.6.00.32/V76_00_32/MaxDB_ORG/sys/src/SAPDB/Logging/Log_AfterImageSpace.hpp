/*!
  @file           Log_AfterImageSpace.hpp
  @author         UweH
  @author         TillL
  @ingroup        Logging
  @brief          Log after image space handler class

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
#ifndef LOG_AFTERIMAGESPACE_HPP
#define LOG_AFTERIMAGESPACE_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"
#include "Logging/Log_QueueEntry.hpp"

/// default number of parts per afterimage
#define LOG_AFTERIMAGESPACE_PARTS_DELTA 3
/*!
    @class          Log_AfterImageSpace
    @brief          Log after image space handler class

    An instance of this class holds a vector of log queue entries.
    These point to log pages on which space has been reserved for writing
    out the after image. 
 */
class Log_AfterImageSpace : public Data_ISplitSpace
{
public:

    /// any number of parts
    typedef PartNo                                              PartCount;

    /// allocator for queue entries
    typedef SAPDBMem_IRawAllocator                              Allocator;

    /// Constructor
    Log_AfterImageSpace (Allocator&                     alloc)
    : m_Parts(alloc)
    {}
    /*!
       @brief          Initializes after image structures including memory allocation
       @param          size [in] part count (default: LOG_AFTERIMAGESPACE_PARTS_DELTA)
       @return         (SAPDB_Bool) true if successful
    */
    SAPDB_Bool Initialize 
        (PartCount                      size = LOG_AFTERIMAGESPACE_PARTS_DELTA)
    {
        return m_Parts.Reserve(size);
    }
    
    /// Deletes after image structures including deallcation
    void Delete()
    {
        m_Parts.Delete();
    }
    
    /// Destructor
    ~Log_AfterImageSpace()
    {
        Delete();
    }

    /// returns the partNo of the currentPart
    virtual PartNo CurrentPart() const
    {
        return (PartNo)m_Parts.GetSize() - 1;
    }

    /*!
       @brief          Returns length and pointer of specified part
       @param          part [in] requested part number
       @param          length [out] size of part space in bytes
       @param          value [out] pointer to part space
    */
    virtual void GetPart 
        (PartNo                         part,
         SAPDB_UInt&                    length,
         SAPDB_Byte*&                   value) const
    {
        value  = m_Parts[part].m_Value;
		length = m_Parts[part].m_Length;
    }
    
    /*!
       @brief          Returns reference to queue entry associated with specified part
       @param          part [in] requested part number
       @return         (Log_QueueEntry&) reference to queue entry
     */
    Log_QueueEntry& GetPartEntry
        (PartNo                         part) const
    {
        return *(m_Parts[part].m_QueueEntry);
    }
    
    /// returns the number of parts
    virtual PartCount GetPartCount() const
    {
        return m_Parts.GetSize();
    }
    
    /// Calculates and returns the sum length of all used space parts
    virtual SAPDB_UInt Length() const
    {
        SAPDB_UInt Result = 0;
        for (SAPDB_UInt i=0; i<m_Parts.GetSize(); ++i)
        {
            Result += m_Parts[i].m_Length;
        }
        return Result;
    }
    /*! ---------------------------------------------------------------------------
    function:    CheckAlignment
    returns:     returns false, if space begin is not properly aligned
    description: This checks if the split space begins at an aligned position
    */
     virtual bool CheckAlignment() const
     {
         if ( ! IsAssigned() )
             return true;
             
         SAPDB_UInt8 address =
                         #ifdef BIT64
                         reinterpret_cast<SAPDB_UInt8>(m_Parts[0].m_Value);
                         #else
                         reinterpret_cast<SAPDB_UInt4>(m_Parts[0].m_Value);
                         #endif
         
         return address % LOG_PAGE_ALIGNMENT == 0;
     }

    /*!
       @brief          A part is appended to the part vector
       @param          entry [in] reference to log queue entry
       @param          length [in] size of reserved space in bytes
       @param          value [in] pointer to reserved space
    */
    void AppendPart 
        (Log_QueueEntry&                entry,
         SAPDB_UInt                     length,
         SAPDB_Byte*                    value)
    {
        if (m_Parts.IsFull())
            if ( ! m_Parts.Reserve(m_Parts.GetSize() + LOG_AFTERIMAGESPACE_PARTS_DELTA) )
                // PTS 1117126 UH 2002-08-07 if-clause added
                RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                              "m_Parts.Reserve() failed") );

        m_Parts.InsertEnd(Part(&entry, length, value));
    }

    /// Checks whether this part is assigned (true if assigned)
    virtual bool IsAssigned() const
    {
        return m_Parts.GetSize() > 0;
    }
    
    /*!
       @brief          Releases all parts

       @li All log queue entries associated with this split space                  are released.
       @li The part vector is cleared.
    */
    void Deassign()
    {
        for (SAPDB_UInt i=0; i<m_Parts.GetSize(); ++i)
        {
            Log_QueueEntry& queueentry = *m_Parts[i].m_QueueEntry;
            queueentry.DecBusyTaskCount();
         }
        if ( ! m_Parts.Resize(0) )
            // PTS 1117126 UH 2002-08-07 if-clause added
            RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "m_Parts.Resize(0) failed") );

    }
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const
	{}
    
private:

    /// after image part
    struct Part
    {
        Log_QueueEntry*     m_QueueEntry;   ///< the log queue entry
        SAPDB_UInt          m_Length;       ///< the length
        SAPDB_Byte*         m_Value;        ///< pointer to the entry-data

        /// constructor
        Part() 
             :
             m_QueueEntry(0),
             m_Length(0),
             m_Value(0)
        {}

        /// constructor for a specific entry
        Part(Log_QueueEntry*     queueentry,
             SAPDB_UInt          length,
             SAPDB_Byte*         value)
             :
             m_QueueEntry(queueentry),
             m_Length(length),
             m_Value(value)
        {}
    };

    /// vector of after image parts
    Container_Vector<Part>                      m_Parts;

};

#endif // LOG_AFTERIMAGESPACE_HPP
