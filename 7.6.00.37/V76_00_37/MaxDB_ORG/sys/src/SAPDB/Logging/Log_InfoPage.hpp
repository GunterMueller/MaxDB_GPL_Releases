/*!
    @file           Log_InfoPage.hpp
    @author         UweH
    @author         TillL
    @ingroup        Logging
    @brief          Handler class for log info page

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
#ifndef LOG_INFOPAGE_HPP
#define LOG_INFOPAGE_HPP
#include <memory.h>
#include "gkb00.h"
#include "hgg08.h"  // g08loginfo
#include "heo55k.h" // vbegexcl/vendexcl/visexcl
#include "RunTime/RTE_Types.h"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "KernelCommon/Kernel_Page74.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "Logging/Log_Types.hpp"
/*!
   @interface      Log_InfoPage
   @brief          Handler class for log info pages
 */
class Log_InfoPage : public Kernel_Page74
{
public:
    /// Default constructor
    Log_InfoPage()
    {}
    /*!
    	@brief Constructor; assigns frame
        Does not change frame contents.
        It is assumed that the frame contains a valid log info page.
     */
    Log_InfoPage (const Kernel_Page::PageFrame &frame)
    :Kernel_Page74(frame)
    {}
    /*!
        @brief Constructor; assigns frame and initializes frame contents
        It is assumed that the frame contains an empty page which may be overwritten.
     */
    Log_InfoPage (const Kernel_Page::PageFrame &frame, 
                  const Kernel_Page::Id     &pageid)
    : Kernel_Page74(frame, Kernel_LogInfoPageType)
    {
        InitializeFrame(pageid);
    }
    /// Assigns frame and initializes frame contents
    void SetAndInitializeFrame
        (const PageFrame&               frame, 
         const Kernel_Page::Id&                  pageid);
	/// Initializes frame contents
    void InitializeFrame
        (const Kernel_Page::Id&                  pageid);
    /*!
       @brief          enter the region for log info page changes
     */
    void EnterRegion(tsp00_TaskId taskid)
    {
        vbegexcl (taskid, g08loginfo);
    }
    /*!
       @brief          leaves the region for log info page changes
     */
    void LeaveRegion(tsp00_TaskId taskid)
    {
        vendexcl (taskid, g08loginfo);
    }
    /*!
       @brief          returns true if in log info page region
     */
    bool InRegion(tsp00_TaskId taskid)
    {
        return visexcl (taskid, g08loginfo);
    }
    /*!
       @brief          Reads log date from frame
       @return         (Kernel_Date) log date
     */
    Kernel_Date ReadLogDate() const;
    /*!
       @brief          Reads log time from frame
       @return         (Kernel_Time) log time
     */
    Kernel_Time ReadLogTime() const;
    /*!
       @brief          Reads log flush mode from frame
       @return         (Log_FlushMode) log mode
     */
    Log_FlushMode ReadFlushMode() const;
    /*!
       @brief          Reads redo until state from frame
       @return         (Log_DeviceState) log redo until state
     */
    Log_DeviceState ReadDeviceState() const;
    /*!
       @brief          Reads if archive log writing is enabled
       @return         (bool) true if after images are written else false
     */
    bool ReadDevspaceEnabled() const;
    /*!
       @brief          Reads if the archive log can be automatically been overwritten
              Returns true if backups are not necassary.
       @return         (bool)
     */
    bool ReadAutoOverwrite() const
    {
        // PTS 1120106 UH 2003-01-17 new
        return PersistentHeaderPtr()->m_AutoOverwrite;
    }
    /*!
       @brief          Reads number of log queues from frame
       @return         (SAPDB_UInt2) queue count
     */
    SAPDB_UInt2 ReadQueueCount() const;
    /*!
       @brief          Reads cycle related log end offset from frame
       @return         (Log_RawDeviceOffset) cycle related offset
       - The size of the log device and with it the end offset on device                 may grow during log operation by adding new devspaces with the                  'add log devspace' command. 
       - However, as long as no log pages have been written to the                 new devspace(s), the cyclic sequence of i/o seqnos is                  consistent within the old size limits of the log device.
       - The present value says which of the offsets (old or new one)                 is the right one when calculating offsets within the log                 device.
     */
    Log_RawDeviceOffset ReadCycleRelatedEndOffset() const
    {
        return PersistentHeaderPtr()->m_CycleRelatedEndOffset;
    }
    /*!
       @brief          Reads raw offset of 'oldest not saved' page from frame
       @return         (Log_RawDeviceOffset) raw log device offset
       - The 'oldest not saved' log page is the oldest page on                  log device that has not (yet) been saved to tape by a                  'log save' operation.
       - It is the first log page that must be saved with the next                 'log save'.
     */
    Log_RawDeviceOffset ReadOldestNotSavedOffset() const;
    /*!
       @brief          Reads writer i/o seqno of 'oldest not saved' page from frame
       @return         (Log_IOSequenceNo) writer i/o seqno
       - The 'oldest not saved' log page has been marked by the                  log writer with this seqno.
       - see also ReadOldestNotSavedOffset()
     */
    Log_IOSequenceNo ReadOldestNotSavedIOSequenceNo() const;
    /*!
       @brief          Reads the oldest known IOSequenceNo from the Log_InfoPage
       @return         (Log_IOSequenceNo) writer i/o seqno
       The 'last known' seqno is the writer i/o seqno of the oldest log page that is known to be existent
       on the Log_Volume.
     */
    Log_IOSequenceNo ReadOldestKnownIOSequenceNo() const;
    /*!
       @brief          Reads 'forced last known' writer i/o seqno from frame
       @return         (Log_IOSequenceNo) writer i/o seqno
       - In certain cycles of a few hundred log pages, the log info                 page is made persistent, even when there is no savepoint                 involved (write i/o of the log info page).
       - The 'last known' seqno is the writer i/o seqno of                 the last log page that was made persistent.
     */
    Log_IOSequenceNo ReadLastKnownIOSequenceNo() const;
    /*!
       @brief          Reads 'forced last known' raw log device offset from frame
       @return         (Log_RawDeviceOffset) raw log device offset
       - In certain cycles of a few hundred log pages, the log info                 page is made persistent, even when there is no savepoint                 involved (write i/o of the log info page).
       - The 'last known' offset is the writer offset of                 the last log page that was made persistent.
     */
    Log_RawDeviceOffset ReadLastKnownOffset() const;
    /*!
       @brief          Reads number of log backups from frame
       @return         (SAPDB_UInt4) log backup count
       - This is the number of log save's that have                  been performed so far for the present database.
     */
    SAPDB_UInt4 ReadLogBackupCount() const;
    /*!
       @brief          Reads date of last successful redo until from frame
       @return         (Kernel_Date) log redo until date
     */
    Kernel_Date ReadRedoUntilDate() const;
    /*!
       @brief          Reads time of last successful redo until from frame
       @return         (Kernel_Time) log redo until time
     */
    Kernel_Time ReadRedoUntilTime() const;
    /*!
       @brief          Reads lower offset of 'clear until' from frame
       @return         (Log_RawDeviceOffset) lower offset of 'clear until'
       - With the 'clear until' command, log pages between lower and                  upper offset, respectively, are cleared (initialized) on the                 log device.
       - see also ReadClearUntilUpperOffset()
     */
    Log_RawDeviceOffset ReadClearUntilLowerOffset() const
    {
        return PersistentHeaderPtr()->m_ClearUntilLowerOffset;
    }
    /*!
       @brief          Reads upper offset of 'clear until' from frame
       @return         (Log_RawDeviceOffset) upper offset of 'clear until'
       - With the 'clear until' command, log pages between lower and                  upper offset, respectively, are cleared (initialized) on the                 log device.
       - see also ReadClearUntilLowerOffset()
     */
    Log_RawDeviceOffset ReadClearUntilUpperOffset() const
    {
        return PersistentHeaderPtr()->m_ClearUntilUpperOffset;
    }
    /*!
       @brief          Formerly used for restore log until, in Rel. 7.4.* unused and set to Invalid
       @return         (Log_IOSequenceNo) writer i/o seqno
     */
    Log_IOSequenceNo ReadIOSequenceNoValidAfterRedoUntil() const
    {
        return PersistentHeaderPtr()->m_IOSequenceNoValidAfterRedoUntil;
    }
    /*!
       @brief          Reads i/o seqno of final log page of last savepoint from frame
       @return         (Log_IOSequenceNo) writer i/o seqno
       - This page contains the 'savepoint complete' log entry                 of the last savepoint.
     */
    Log_IOSequenceNo ReadSavepointIOSequenceNo() const
    {
        return PersistentHeaderPtr()->m_RestartRecordPart.m_SaveptParam.svpIOsequence_kb00;
    }
    /*!
       @brief          Reads offset of 'savepoint complete' log entry.
       @return         (Log_RawDeviceOffset) entry offset within log device
       - see also ReadSavepointIOSequenceNo()
     */
    Log_RawDeviceOffset ReadSavepointEntryOffset() const;
    /*!
       @brief          Reads offset of 'savepoint complete' entry in log page.
       @return         (Kernel_Page::OffsetType) entry offset within a log page
       - see also ReadSavepointIOSequenceNo()
     */
    Kernel_Page::OffsetType ReadSavepointEntryOffsetInPage() const;
    /*!
       @brief          Reads internal database identifier from frame
       @return         (const SAPDB_Byte*) pointer to (not null terminated) string
       - This is an internal string identifying the actual database                 instance.
     */
    const SAPDB_Byte* ReadDBIdentifier() const
    {
        return PersistentHeaderPtr()->m_RestartRecordPart.m_DBIdent + 0; // +0 is for type conversion
    }
    /*!
       @brief          Reads the Kernel-Version of the last savepoint
    */
    const tsp00_Version & ReadDBVersion() const
    {
        return PersistentHeaderPtr()->m_RestartRecordPart.m_DBVersion ; // +0 is for type conversion
    }
	/// Reads the master node name. returns false if the given string is not large enough.
    /// It is NOT a 0x00 terminated string returned.
    bool ReadMasterNodeName (RTE_Nodename &masterNodeName)
    {
        SAPDB_UInt length = PersistentHeaderPtr()->m_MasterNodeName.length();
        
        if ( length > sizeof(PersistentHeaderPtr()->m_MasterNodeName) )
           length = sizeof(PersistentHeaderPtr()->m_MasterNodeName);
        
        if ( length > sizeof(masterNodeName) )
           return false;
        
        SAPDB_MemCopyNoCheck ( masterNodeName, PersistentHeaderPtr()->m_MasterNodeName.asCharp(), length );
        *(masterNodeName+length) = 0;
        return true;
    }
    /*!
       @brief          Writes log date to frame
       @param          date [in] log date
     */
    void WriteLogDate
        (const Kernel_Date&                date);
    /*!
       @brief          Writes log time to frame
       @param          time [in] log time
     */
    void WriteLogTime
        (const Kernel_Time&                time);
    /*!
       @brief          Writes log flush mode to frame
       @param          mode [in] log flush mode
     */
    void WriteLogFlushMode
        (const Log_FlushMode&       mode);
    /*!
       @brief          Writes redo until state to frame
       @param          state [in] log redo until state
     */
    void WriteDeviceState
        (const Log_DeviceState&      state);
    /*!
       @brief          Writes if devspace is enabled
       @param          enabled [in] bool
     */
    void WriteDevspaceEnabled
        (bool enabled);
    /*!
        @brief write the auto overwrite state
        @param          enabled [in] bool
     */
    void WriteAutoOverwrite
        (bool enabled);
    /*!
       @brief          Writes number of log queues to frame
       @param          count [in] queue count
     */
    void WriteQueueCount
        (const SAPDB_UInt2&              count);
    /*!
       @brief          Writes cycle related log end offset to frame
       @param          size [in] log device offset
     */
    void WriteCycleRelatedEndOffset
        (const Log_RawDeviceOffset&        size);
    /*!
       @brief          Writes raw offset 'first not saved' page to frame
       @param          offset [in] raw log device offset
     */
    void WriteOldestNotSavedOffset
        (const Log_RawDeviceOffset&         offset);
    /*!
       @brief          Writes writer i/o seqno of 'first not saved' page to frame
       @param          seqno [in] writer i/o seqno
     */
    void WriteOldestNotSavedIOSequenceNo
        (const Log_IOSequenceNo&            seqno);
    /*!
       @brief          Writes writer i/o seqno of 'oldest known' page to frame
       @param          seqno [in] writer i/o seqno
     */
    void WriteOldestKnownIOSequenceNo
        (const Log_IOSequenceNo&            seqno);
    /*!
       @brief          Writes 'forced last known' writer i/o seqno to frame
       @param          seqno [in] writer i/o seqno
     */
    void WriteLastKnownIOSequenceNo
        (const Log_IOSequenceNo&            seqno);
    /*!
       @brief          Writes 'forced last known' writer offset to frame
       @param          offset [in] log dev offset
     */
    void WriteLastKnownOffset
        (const Log_RawDeviceOffset&            offset);
    /*!
       @brief          Writes number of log backups to frame
       @param          count [in] log backup count
     */
    void WriteLogBackupCount
        (SAPDB_UInt4                    count);
    /*!
       @brief          Writes date of last successful redo until to frame
       @param          date [in] log redo until date
     */
    void WriteRedoUntilDate
        (const Kernel_Date&                date);
    /*!
       @brief          Writes time of last successful redo until to frame
       @param          time [in] log redo until time
     */
    void WriteRedoUntilTime
        (const Kernel_Time&                time);
    /*!
       @brief          Writes lower offset of 'clear until' to frame
       @param          lwb [in] lower offset of 'clear until'
     */
    void WriteClearUntilLowerOffset
        (const Log_RawDeviceOffset&            lwb);
    /*!
       @brief          Writes upper offset of 'clear until' to frame
       @param          upb [in] upper offset of 'clear until'
     */
    void WriteClearUntilUpperOffset
        (const Log_RawDeviceOffset&            upb);
    /*!
       @brief          Writes next writer i/o seqno to frame
       @param          seqno [in] writer i/o seqno
       @return         none
     */
    void WriteIOSequenceNoValidAfterRedoUntil
        (const Log_IOSequenceNo&            seqno);
    /*!
       @brief          Writes content of restart record to frame
       @param          rrpart [in] part of restart record to be copied
     */
    void WriteSavepointParameters (const SAPDBFields_Field& rrpart);
    /*!
       @brief          Writes the db_identifier into the peristent part of the log_InfoPage
              New for PTS 1113550 mb 2002-04-19
       @param          dbIdent [in] the new db_identifier
     */
    void WriteDBIdentifier(const tsp00_Line dbIdent);	
    /// writes the kernel-version into the log_infopage
    void WriteDBVersion(const tsp00_Version & dbVersion);	
	/// Writes the master node id
    bool WriteMasterNodeName ( const RTE_Nodename &masterNodeName )
    {
        if ( strlen(masterNodeName) > sizeof(PersistentHeaderPtr()->m_MasterNodeName) )
            return false;
        PersistentHeaderPtr()->m_MasterNodeName.c2p(masterNodeName);
        return true;
    }
    /*!
       @brief          The savepoint offset and sequence are set to the given values
                       This is used during ClearLog.
     */
    void ResetSavepointPart(const Log_IOSequenceNo savePointIOSequence,
                            const Log_RawDeviceOffset    savePointOffset);
    /*!
       @brief          Verifies consistency of page contents
       @return         (SAPDB_Bool) true if verify successful
     */
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const
    {
        return true;
    }
	/// writes the values of the info page to the vtrace.
    void WriteToTrace () const
    {
        Kernel_VTrace trace;
        
        if ( ! IsAssigned() )
        {
            trace << "log info page not assigned\n";
            return;
        }
        
        trace << "InfoPageId: " << ReadPageId()
              << ", Date: " << ReadLogDate()
              << ", Time: " << ReadLogTime()
              << FlushLine;
        trace << "QueueCount: " << ReadQueueCount()
              << ", State: " << Log_DeviceStateStrings[ReadDeviceState()]
              << ", Mode: " << ReadFlushMode()
              << FlushLine;
        trace << "CycleRelatedEndOffset: " << ReadCycleRelatedEndOffset()
              << ", OldestKnownIOSeq: " << ReadOldestKnownIOSequenceNo()
              << FlushLine;
        trace << "LastKnown-Offset: " << ReadLastKnownOffset() 
              << ", -IOSequence: " << ReadLastKnownIOSequenceNo()
              << FlushLine;
        trace << "OldestNotSaved-Offset: " << ReadOldestNotSavedOffset() 
              << ", -IOSequence: " << ReadOldestNotSavedIOSequenceNo()
              << ", LogBackupCount: " << ReadLogBackupCount()
              << FlushLine;
        trace << "LastSavepoint-Position: " << ReadSavepointEntryOffset() 
              << "." << ReadSavepointEntryOffsetInPage() 
              << ", -IOSequence: " << ReadSavepointIOSequenceNo()
              << FlushLine;
        trace << ", oldestKnownIOSEq: "  << ReadOldestKnownIOSequenceNo()
              << FlushLine;
    }

#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS)
public:
#else
protected:
#endif

    /// defines the parts from the restart record, which are also saved
    /// in the Log_InfoPage
    struct RestartRecordPart
    {
        tkb00_SaveptParam m_SaveptParam;     ///< 104 all redo related parameters
        tsp00_Version     m_DBVersion;       ///< Softwareversion of the kernel
        tsp00_Line        m_DBIdent;         ///< 132 database identifier (internal format)
    };                                       ///< 276 sum of bytes

    /// persistent structure of the Log_InfoPage
    struct PersistentHeader
    {
        /// At Offset 0 starts the page with the standard Header
        Kernel_Page74::PersistentHeader m_KernelPage74Header;
        /// @copydoc Log_InfoPage::ReadLogDate
        SAPDB_UInt4                     m_LogDate;              
        /// @copydoc Log_InfoPage::ReadLogTime
        SAPDB_UInt4                     m_LogTime;
        /// @copydoc Log_InfoPage::ReadFlushMode
        SAPDB_UInt1                     m_LogFlushMode;
        /// @copydoc Log_InfoPage::ReadDeviceState
        SAPDB_UInt1                     m_DeviceState;
        /// @copydoc Log_InfoPage::ReadQueueCount
        SAPDB_UInt2                     m_QueueCount;
        /// @copydoc Log_InfoPage::ReadCycleRelatedEndOffset
        SAPDB_UInt4                     m_CycleRelatedEndOffset;
        /// @copydoc Log_InfoPage::ReadLastKnownIOSequenceNo
        SAPDB_UInt4                     m_LastKnownIOSequenceNo;
        /// @copydoc Log_InfoPage::ReadLastKnownOffset
        SAPDB_UInt4                     m_LastKnownOffset;
        /// @copydoc Log_InfoPage::ReadOldestNotSavedOffset
        SAPDB_UInt4                     m_OldestNotSavedOffset;
        /// @copydoc Log_InfoPage::ReadOldestNotSavedIOSequenceNo
        SAPDB_UInt4                     m_OldestNotSavedIOSequenceNo;
        /// @copydoc Log_InfoPage::ReadDevspaceEnabled
        SAPDB_Bool1                     m_DevspaceEnabled;
        /// @copydoc Log_InfoPage::ReadAutoOverwrite
        SAPDB_Bool1                     m_AutoOverwrite;
        /// An unused filler
        SAPDB_UInt2	                    m_Filler2;
        /// @copydoc Log_InfoPage::ReadLogBackupCount
        SAPDB_UInt4                     m_LogBackupCount;
        /// @copydoc Log_InfoPage::ReadRedoUntilDate
        SAPDB_UInt4                     m_RedoUntilDate;
        /// @copydoc Log_InfoPage::ReadRedoUntilTime
        SAPDB_UInt4                     m_RedoUntilTime;
        /// @copydoc Log_InfoPage::ReadClearUntilLowerOffset
        SAPDB_UInt4                     m_ClearUntilLowerOffset;
        /// @copydoc Log_InfoPage::ReadClearUntilUpperOffset
        SAPDB_UInt4                     m_ClearUntilUpperOffset;
        /// @copydoc Log_InfoPage::ReadIOSequenceNoValidAfterRedoUntil
        SAPDB_UInt4                     m_IOSequenceNoValidAfterRedoUntil;
        /// @copydoc Log_InfoPage::ReadOldestKnownIOSequenceNo
        SAPDB_UInt4                     m_OldestKnownIOSequenceNo;
        /// a filler
        tsp00_C64                       m_Filler4;
        /// At Offset 80  the copied part from the RestartRecord starts
        RestartRecordPart               m_RestartRecordPart;
        /// At Offset 356 the next filler starts
        tsp00_C256                      m_Filler5;
        /// @copydoc Log_InfoPage::ReadMasterNodeName 
        tsp00_NodeId                    m_MasterNodeName;
    };

    /// Returns pointer to page header on frame
    PersistentHeader* PersistentHeaderPtr();

    /// Returns const pointer to page header on frame
    const PersistentHeader* PersistentHeaderPtr() const;

    /// Returns size of page header
    SizeType GetHeaderSize() const;
private:
    /// updates the value of OldestNotSavedIOSequenceNo depending on the IOSequences of
    /// the last savepoint and oldestNotSavedIOSequenceNo (new with PTS 1124727 mb 2003-10-20 )
    void  UpdateOldestKnownSeq();
};
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::SetAndInitializeFrame
    (const PageFrame&               frame, 
     const Kernel_Page::Id&                  pageid)
{
    SetFrame(frame);
    InitializeFrame(pageid);
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::InitializeFrame
    (const Kernel_Page::Id&                  pageid)
{
    memset (GetFramePtr(), 0, GetFrame().GetLength()); // PTS 1125372 UH 2003-11-11 added
    Kernel_Page74::InitializeFrame(Kernel_LogInfoPageType);
    WritePageId(pageid);
}
//------------------------------------------------------------------------------
inline Log_InfoPage::PersistentHeader* 
    Log_InfoPage::PersistentHeaderPtr()
{
    return reinterpret_cast<PersistentHeader*> (m_Frame.GetDataPtr());
}
//------------------------------------------------------------------------------
inline const Log_InfoPage::PersistentHeader* 
    Log_InfoPage::PersistentHeaderPtr() const
{
    return reinterpret_cast<const PersistentHeader*> (m_Frame.GetDataPtr());
}
//------------------------------------------------------------------------------

inline Log_InfoPage::SizeType 
    Log_InfoPage::GetHeaderSize() const
{
    return sizeof(PersistentHeader);
}
//------------------------------------------------------------------------------
inline Kernel_Date 
    Log_InfoPage::ReadLogDate() const
{
    return PersistentHeaderPtr()->m_LogDate;
}
//------------------------------------------------------------------------------
inline Kernel_Time 
    Log_InfoPage::ReadLogTime() const
{
    return PersistentHeaderPtr()->m_LogTime;
}
//------------------------------------------------------------------------------
inline Log_FlushMode 
    Log_InfoPage::ReadFlushMode() const
{
    return Log_FlushMode(PersistentHeaderPtr()->m_LogFlushMode);
}
//------------------------------------------------------------------------------
inline Log_DeviceState 
    Log_InfoPage::ReadDeviceState() const
{
    return Log_DeviceState(PersistentHeaderPtr()->m_DeviceState);
}
//------------------------------------------------------------------------------
inline bool
    Log_InfoPage::ReadDevspaceEnabled() const
{
    // PTS 1113190 UH 2001-12-19
    return PersistentHeaderPtr()->m_DevspaceEnabled;
}
//------------------------------------------------------------------------------
inline SAPDB_UInt2
    Log_InfoPage::ReadQueueCount() const
{
    return PersistentHeaderPtr()->m_QueueCount;
}
//------------------------------------------------------------------------------
inline Log_RawDeviceOffset 
    Log_InfoPage::ReadOldestNotSavedOffset() const
{
    return PersistentHeaderPtr()->m_OldestNotSavedOffset;
}
//------------------------------------------------------------------------------
inline Log_IOSequenceNo 
    Log_InfoPage::ReadOldestNotSavedIOSequenceNo() const
{
    return PersistentHeaderPtr()->m_OldestNotSavedIOSequenceNo;
}
//------------------------------------------------------------------------------
inline Log_IOSequenceNo 
    Log_InfoPage::ReadOldestKnownIOSequenceNo() const
{
    return PersistentHeaderPtr()->m_OldestKnownIOSequenceNo;
}
//------------------------------------------------------------------------------
inline Log_IOSequenceNo 
    Log_InfoPage::ReadLastKnownIOSequenceNo() const
{
    return PersistentHeaderPtr()->m_LastKnownIOSequenceNo;
}
//------------------------------------------------------------------------------
inline Log_RawDeviceOffset
    Log_InfoPage::ReadLastKnownOffset() const
{
    return PersistentHeaderPtr()->m_LastKnownOffset;
}
//------------------------------------------------------------------------------
inline SAPDB_UInt4 
    Log_InfoPage::ReadLogBackupCount() const
{
    return PersistentHeaderPtr()->m_LogBackupCount;
}
//------------------------------------------------------------------------------
inline Kernel_Date 
    Log_InfoPage::ReadRedoUntilDate() const
{
    return PersistentHeaderPtr()->m_RedoUntilDate;
}
//------------------------------------------------------------------------------
inline Kernel_Time 
    Log_InfoPage::ReadRedoUntilTime() const
{
    return PersistentHeaderPtr()->m_RedoUntilTime;
}
//------------------------------------------------------------------------------
inline Log_RawDeviceOffset
    Log_InfoPage::ReadSavepointEntryOffset() const
{
    return PersistentHeaderPtr()->m_RestartRecordPart.m_SaveptParam.svpStartOffset_kb00;
}
//------------------------------------------------------------------------------
inline Kernel_Page::OffsetType
    Log_InfoPage::ReadSavepointEntryOffsetInPage() const
{
    return PersistentHeaderPtr()->m_RestartRecordPart.m_SaveptParam.svpStartEntryOffset_kb00;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteLogDate
    (const Kernel_Date&                date)
{
    PersistentHeaderPtr()->m_LogDate = date;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteLogTime
    (const Kernel_Time&                time)
{
    PersistentHeaderPtr()->m_LogTime = time;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteLogFlushMode
    (const Log_FlushMode&       mode)
{
    PersistentHeaderPtr()->m_LogFlushMode = mode;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteDeviceState
    (const Log_DeviceState&      state)
{
    #ifdef SAPDB_QUICK
    // PTS 1111525 mb 2002-01-29 new
    // the allowed previous states are commented out. By this
    // an assertion is throwed when a not allowed previous state is found
    switch (state)
    {
        case Log_DeviceStateOkay:
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateOkay              != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateHistoryLost       != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateCleared           != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateClearing          != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyCleared  != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyClearing != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceState_DO_NOT_USE       != PersistentHeaderPtr()->m_DeviceState);
            break;

        case Log_DeviceStateHistoryLost:
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateOkay              != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateHistoryLost       != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateCleared           != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateClearing          != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyCleared  != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyClearing != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceState_DO_NOT_USE       != PersistentHeaderPtr()->m_DeviceState);
            break;

        case Log_DeviceStateCleared:
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateOkay              != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateHistoryLost       != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateCleared           != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateClearing          != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyCleared  != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyClearing != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(ReadClearUntilLowerOffset().IsInvalid());
            SAPDBERR_ASSERT_ARGUMENT(ReadClearUntilUpperOffset().IsInvalid());
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceState_DO_NOT_USE       != PersistentHeaderPtr()->m_DeviceState);
            break;

        case Log_DeviceStateClearing:
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateOkay              != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateHistoryLost       != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateCleared           != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateClearing          != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyCleared  != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyClearing != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(ReadClearUntilLowerOffset().IsValid());
            SAPDBERR_ASSERT_ARGUMENT(ReadClearUntilUpperOffset().IsValid());
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceState_DO_NOT_USE       != PersistentHeaderPtr()->m_DeviceState);
            break;

        case Log_DeviceStatePartiallyCleared:
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateOkay              != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateHistoryLost       != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateCleared           != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateClearing          != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyCleared  != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyClearing != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_STATE(ReadClearUntilLowerOffset().IsInvalid());
            SAPDBERR_ASSERT_STATE(ReadClearUntilUpperOffset().IsInvalid());
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceState_DO_NOT_USE       != PersistentHeaderPtr()->m_DeviceState);
            break;

        case Log_DeviceStatePartiallyClearing:
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateOkay              != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateHistoryLost       != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateCleared           != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStateClearing          != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyCleared  != PersistentHeaderPtr()->m_DeviceState);
            // SAPDBERR_ASSERT_ARGUMENT(Log_DeviceStatePartiallyClearing != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceState_DO_NOT_USE       != PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_STATE(ReadClearUntilLowerOffset().IsValid());
            SAPDBERR_ASSERT_STATE(ReadClearUntilUpperOffset().IsValid());
            break;

        default:
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceState_DO_NOT_USE == PersistentHeaderPtr()->m_DeviceState);
            SAPDBERR_ASSERT_ARGUMENT(Log_DeviceState_DO_NOT_USE != PersistentHeaderPtr()->m_DeviceState);
    }

    #endif

    PersistentHeaderPtr()->m_DeviceState = state;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteDevspaceEnabled
    (bool enabled)
{
    // PTS 1113190 UH 2001-12-19
    PersistentHeaderPtr()->m_DevspaceEnabled = enabled;
}
//------------------------------------------------------------------------------

inline void 
    Log_InfoPage::WriteAutoOverwrite
    (bool enabled)
{
    // PTS 1120106 UH 2003-01-17 new
    PersistentHeaderPtr()->m_AutoOverwrite   = enabled;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteQueueCount
    (const SAPDB_UInt2&              count)
{
    PersistentHeaderPtr()->m_QueueCount = count;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteCycleRelatedEndOffset
    (const Log_RawDeviceOffset&        size)
{
    PersistentHeaderPtr()->m_CycleRelatedEndOffset = size.RawValue();
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteOldestNotSavedOffset
    (const Log_RawDeviceOffset&     offset)
{
    PersistentHeaderPtr()->m_OldestNotSavedOffset = offset.RawValue();
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteOldestNotSavedIOSequenceNo
    (const Log_IOSequenceNo&            seqno)
{
    PersistentHeaderPtr()->m_OldestNotSavedIOSequenceNo = seqno.RawValue();
    // PTS 1124727 mb 2003-10-20 update values for comparison
    UpdateOldestKnownSeq();
}
inline void Log_InfoPage::UpdateOldestKnownSeq()
{
	Log_IOSequenceNo oldestNotSaved = ReadOldestNotSavedIOSequenceNo();
	Log_IOSequenceNo lastSavepoint  = ReadSavepointIOSequenceNo();

    if ( oldestNotSaved.IsInvalid() || lastSavepoint.IsInvalid() )
    	return;

    if ( Log_IOSequenceNo::LeftIsOlder(lastSavepoint, oldestNotSaved, 
                                       ReadOldestKnownIOSequenceNo()) )
        WriteOldestKnownIOSequenceNo(lastSavepoint);
    else
        WriteOldestKnownIOSequenceNo(oldestNotSaved);
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteOldestKnownIOSequenceNo
    (const Log_IOSequenceNo&            seqno)
{
    PersistentHeaderPtr()->m_OldestKnownIOSequenceNo = seqno.RawValue();
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteLastKnownIOSequenceNo
    (const Log_IOSequenceNo&            seqno)
{
    PersistentHeaderPtr()->m_LastKnownIOSequenceNo = seqno.RawValue();
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteLastKnownOffset
        (const Log_RawDeviceOffset&            offset)
{
    PersistentHeaderPtr()->m_LastKnownOffset = offset.RawValue();
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteLogBackupCount
    (SAPDB_UInt4                    count)
{
    PersistentHeaderPtr()->m_LogBackupCount = count;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteRedoUntilDate
    (const Kernel_Date&                date)
{
    PersistentHeaderPtr()->m_RedoUntilDate = date;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteRedoUntilTime
    (const Kernel_Time&                time)
{
    PersistentHeaderPtr()->m_RedoUntilTime = time;
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteClearUntilLowerOffset
    (const Log_RawDeviceOffset&            lwb)
{
    PersistentHeaderPtr()->m_ClearUntilLowerOffset = lwb.RawValue();
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteClearUntilUpperOffset
    (const Log_RawDeviceOffset&            upb)
{
    PersistentHeaderPtr()->m_ClearUntilUpperOffset = upb.RawValue();
}
//------------------------------------------------------------------------------
inline void 
    Log_InfoPage::WriteIOSequenceNoValidAfterRedoUntil
    (const Log_IOSequenceNo&            seqno)
{
    PersistentHeaderPtr()->m_IOSequenceNoValidAfterRedoUntil = seqno.RawValue();
}
//------------------------------------------------------------------------------
inline void Log_InfoPage::WriteSavepointParameters (const SAPDBFields_Field& rrpart)
{
    tkb00_SaveptParam &param = PersistentHeaderPtr()->m_RestartRecordPart.m_SaveptParam;
    SAPDBERR_ASSERT_ARGUMENT(rrpart.GetLength() == sizeof(param));
    SAPDB_MemCopyNoCheck(&param, rrpart.GetDataPtr(), rrpart.GetLength());

    if (ReadSavepointEntryOffset().IsValid()) // PTS 1133214 mb 2004-01-04
    // For Redosavepoints without offset (e.g. from tape) do not set the lastKnownOffset to invalid
    {
        if (ReadLastKnownIOSequenceNo().IsValid())
        {
            if (Log_IOSequenceNo::LeftIsOlder(  ReadLastKnownIOSequenceNo(), 
                                                ReadSavepointIOSequenceNo(), 
                                                ReadOldestKnownIOSequenceNo()))
            // update only if the lastknown gets a better (i.e. newer) value then update it
            // PTS 141061 mb 2006-04-18
            {
                WriteLastKnownIOSequenceNo(ReadSavepointIOSequenceNo());
                WriteLastKnownOffset(ReadSavepointEntryOffset());
            }
        }
        else
        // there is no value yet in the loginfopage written
        {
            WriteLastKnownIOSequenceNo(ReadSavepointIOSequenceNo());
            WriteLastKnownOffset(ReadSavepointEntryOffset());
        }
    }
    // PTS 1128080 mb 2004-02-26 change OldestKnown not for restart-savepoints
    if (!Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing())
    {
        UpdateOldestKnownSeq();
    }
}
//------------------------------------------------------------------------------
inline void  
    Log_InfoPage::WriteDBIdentifier(const tsp00_Line dbIdent)		// new PTS 1113550 mb 2002-04-19
{
    PersistentHeaderPtr()->m_RestartRecordPart.m_DBIdent = dbIdent;
}
//------------------------------------------------------------------------------
inline void  
    Log_InfoPage::WriteDBVersion(const tsp00_Version & dbVersion)	
{
    PersistentHeaderPtr()->m_RestartRecordPart.m_DBVersion = dbVersion;
}
//------------------------------------------------------------------------------
inline void Log_InfoPage::ResetSavepointPart(const Log_IOSequenceNo savePointIOSequence,
                                             const Log_RawDeviceOffset    savePointOffset)
{
    tkb00_SaveptParam &param  = PersistentHeaderPtr()->m_RestartRecordPart.m_SaveptParam;
    param.svpIOsequence_kb00  = savePointIOSequence.RawValue();
    param.svpStartOffset_kb00 = savePointOffset.RawValue();
}
#endif  // LOG_INFOPAGE_HPP
