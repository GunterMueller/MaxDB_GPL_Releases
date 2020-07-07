/*!
  @file    Log_Types.hpp
  @ingroup Logging
  @author  UweH
  @brief   Contains types used from several modules in this component.

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
#ifndef LOG_TYPES_HPP
#define LOG_TYPES_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "SAPDBCommon/SAPDB_RangeIterator.hpp"
#include "SAPDBCommon/SAPDB_OStream.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"

/// messagelist
class SAPDBErr_MessageList;

/// This switches on all traces related to the log volume (writer and queue activity)
extern SAPDBTrace_Topic     LogVolume_Trace;
/// Switches on traces related to history writing and access
extern SAPDBTrace_Topic     LogHistory_Trace;
/// Switches on traces related to transactional behaviour
extern SAPDBTrace_Topic     LogTrans_Trace;
/// Switches on traces related to action handling.
extern SAPDBTrace_Topic     LogAction_Trace;
/// Switches on traces related to logpages.
extern SAPDBTrace_Topic     LogPage_Trace;
/// This switches on additional checks related to the log volume (writer and queue activity)
extern SAPDBDiag_CheckTopic LogVolume_Check;
/// Switches on additional checks related to history writing and access
extern SAPDBDiag_CheckTopic LogHistory_Check;
/// Switches on additional checks related to transactional behaviour
extern SAPDBDiag_CheckTopic LogTrans_Check;
/// Switches on additional checks related to action handling.
extern SAPDBDiag_CheckTopic LogAction_Check;
/// Switches on additional checks related to logpages.
extern SAPDBDiag_CheckTopic LogPage_Check;
/// any number of blocks on a raw log device
typedef SAPDB_UInt4 Log_DeviceBlockCount;
/// unsafe implementation of Log_RawDeviceOffsets 
/// @attention be careful when comparing and calculating with instances of this class
typedef SAPDB_Invalidatable<Log_DeviceBlockCount, SAPDB_MAX_UINT4> Log_NonCyclicRawDeviceOffset;
/// block offset on raw log device (PTS 1125634 mb 2003-12-01 reimplemented)
class Log_RawDeviceOffset:protected Log_NonCyclicRawDeviceOffset
{
    /// SAPDB_RangeIterator (i.e. Log_RawDeviceIterator) is the only class which is allowed
    /// to perform calculations
    friend class SAPDB_RangeIterator<Log_RawDeviceOffset, Log_DeviceBlockCount>;
public:
    /// constructor which returns an invalid offset
    Log_RawDeviceOffset() : Log_NonCyclicRawDeviceOffset()
        {};
    /// constructor with a specified position on the Log_Volume
    Log_RawDeviceOffset(const Log_DeviceBlockCount value) : Log_NonCyclicRawDeviceOffset(value)
        {};
    /// invalidates the offset
    void Invalidate() 
        { Log_NonCyclicRawDeviceOffset::Invalidate(); };
    /// returns true if the offset is valid
    SAPDB_Bool IsValid() const 
        { return Log_NonCyclicRawDeviceOffset::IsValid(); };
    /// returns true if the offset is invalid
    SAPDB_Bool IsInvalid() const
        { return Log_NonCyclicRawDeviceOffset::IsInvalid(); };
    /// compare two Log_RawDeviceOffsets  for equality
    SAPDB_Bool operator == (const Log_RawDeviceOffset operand) const {
        return RawValue() == operand.RawValue();
    };
    /// compare two Log_RawDeviceOffsets for inequality
    SAPDB_Bool operator != (const Log_RawDeviceOffset operand) const {
        return RawValue() != operand.RawValue();
    };
    /// compares two Log_RawDeviceOffsets. Be carefull using this method because you have to be aware
    /// of the cyclic nature of the Log_Volume.
    SAPDB_Bool operator >= (const Log_RawDeviceOffset operand) const {return RawValue() >= operand.RawValue();};
    /// compares two Log_RawDeviceOffsets. Be carefull using this method because you have to be aware
    /// of the cyclic nature of the Log_Volume.
    SAPDB_Bool operator >  (const Log_RawDeviceOffset operand) const {return RawValue() >  operand.RawValue();};
    /// compares two Log_RawDeviceOffsets. Be carefull using this method because you have to be aware
    /// of the cyclic nature of the Log_Volume.
    SAPDB_Bool operator <= (const Log_RawDeviceOffset operand) const {return RawValue() <= operand.RawValue();};
    /// compares two Log_RawDeviceOffsets. Be carefull using this method because you have to be aware
    /// of the cyclic nature of the Log_Volume.
    SAPDB_Bool operator <  (const Log_RawDeviceOffset operand) const {return RawValue() <  operand.RawValue();};
    /// This friend allows the output to a SABDB_OStream
    friend SAPDB_OStream & operator << (SAPDB_OStream &ostream, Log_RawDeviceOffset offset);
    /// allow access to the unsafe value of this instance
    const Log_NonCyclicRawDeviceOffset RawValue() const {  return (Log_NonCyclicRawDeviceOffset::operator ()());  };
};
/// converts a IOSequence into a string for tracing purpose
inline SAPDB_ToStringClass SAPDB_ToString(const Log_RawDeviceOffset offset, SAPDB_ToStringClass::fmtflags flags = _T_d  )
{
    if (offset.IsInvalid())
    {
        return SAPDB_ToStringClass("NIL");
    }
    else
    {
        return SAPDB_ToString(offset.RawValue()(), flags);
    }
}
    
/// unsafe implementation of i/o sequence number of log pages
/// @attention be careful when comparing and calculating with instances of this class
typedef SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4> Log_UnsafeIOSequenceNo;
/// safe implementation of i/o sequence number of log pages. Prohibist calculation and comparison
/// new with PTS 1124727 mb 2003-10-16
class Log_IOSequenceNo : private Log_UnsafeIOSequenceNo
{
public:
    /// constructs an object which is invalid
    Log_IOSequenceNo() : Log_UnsafeIOSequenceNo()
    {};
    /// constructs an object with a specific IOSequence
    Log_IOSequenceNo(const SAPDB_UInt4 initValue) : Log_UnsafeIOSequenceNo(initValue)
    {};
    /// helper function to compare overflown sequences.
    static bool LeftIsYounger(const Log_IOSequenceNo left,
                              const Log_IOSequenceNo right,
                              const Log_IOSequenceNo min)
    {
        // PTS 1120201 UH 2003-02-17 new
        // seq-----left-----invalid-0------right---------min
        if ( left == right )
            return false;
        if ( min.IsInvalid()
             ||
             (left >= min && right >= min)
             ||
             (left < min && right < min) )
            return left > right;
            
        return left <= min && right >= min;
    }
    /// helper function to compare overflown sequences.
    static bool LeftIsOlder(const Log_IOSequenceNo left,
                            const Log_IOSequenceNo right,
                            const Log_IOSequenceNo min)
    {
        return left != right && ! LeftIsYounger(left,right,min);
    }
    /// checks if the IOSequence is contained in a given range
    bool IsInRange( const Log_IOSequenceNo leftBorder,
                    const Log_IOSequenceNo rightBorder) const
    {
        if (leftBorder <= rightBorder) // PTS 1131546 mb 2004-09-27
        // no IOSequence overflow within logBackup
        {
            if ((leftBorder <= *this) && (*this <= rightBorder))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        // there is an overlow between leftBorder and rightBorder
        {
            if ((*this < leftBorder) && (rightBorder < *this))
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
        
    /// sets the Log_IOSequenceNo to invalid
    void Invalidate() {Log_UnsafeIOSequenceNo::Invalidate();}
	/// true, if the IOSequence is invalid
    SAPDB_Bool IsInvalid() const {return Log_UnsafeIOSequenceNo::IsInvalid();}
	/// true, if the IOSequence is valid
    SAPDB_Bool IsValid() const {return Log_UnsafeIOSequenceNo::IsValid();}
    /// compare two Log_IOSequences 
    SAPDB_Bool operator == (const Log_IOSequenceNo operand) const {
        return RawValue() == operand.RawValue();
    };
    /// compare Log_IOSequences with integer
    // SAPDB_Bool operator == (const int operand) const {
    //     return RawValue() == operand;
    // };
    /// compare two Log_IOSequences
    SAPDB_Bool operator != (const Log_IOSequenceNo operand) const {
        return RawValue() != operand.RawValue();
    };
    /// compare IOSequence with integer value
    // SAPDB_Bool operator != (const int operand) const {
    //     return RawValue() != operand;
    // };
    /// increments the IOSequence having regard to the IOSequenceOverflow
    Log_IOSequenceNo& operator ++()
    { 
        Log_UnsafeIOSequenceNo::operator ++();
        if ( IsInvalid() )
        {
                *this = 0;
        }
        return *this;
    }
    /// decrements the IOSequence having regard to the IOSequenceOverflow
    Log_IOSequenceNo& operator --()
    { 
        Log_UnsafeIOSequenceNo::operator --();
        if ( IsInvalid() )
        {
            Log_UnsafeIOSequenceNo::operator --();
        }
        return *this;
    }
    /// increments the IOSequence by a specified increment having regard to the IOSequenceOverflow
    Log_IOSequenceNo& operator -=(const SAPDB_UInt4 decrement)
    {
        *this = *this - decrement;
        return *this;
    }
    /// decrements the IOSequence by a specified increment having regard to the IOSequenceOverflow
    Log_IOSequenceNo operator - (const int decrement) const
    {
        return (*this) - SAPDB_UInt4(decrement);
    }
    /// decrements the IOSequence by a specified increment having regard to the IOSequenceOverflow
    Log_IOSequenceNo operator - (const SAPDB_UInt4 decrement) const
    {
        Log_IOSequenceNo result(this->RawValue() - decrement);
        if (    result.IsInvalid()
            || (result.RawValue() > this->RawValue()))
            {
                --result;
            }
        return result;
    }
        
    /// increments the IOSequence by a specified increment having regard to the IOSequenceOverflow
    Log_IOSequenceNo& operator +=(const SAPDB_UInt4 increment)
    {
        *this = *this + increment;
        return *this;
    };
    /// increments the IOSequence by a specified increment having regard to the IOSequenceOverflow
    /// This operator is neccessary to allow constructs like "ioseq + 1" 
    Log_IOSequenceNo operator + (const int increment) const
    {
        return *this + SAPDB_UInt4(increment);
    };
    /// increments the IOSequence by a specified increment having regard to the IOSequenceOverflow
    Log_IOSequenceNo operator + (const SAPDB_UInt4 increment) const
    {
        Log_IOSequenceNo result(this->RawValue() + increment);
        if (    result.IsInvalid()
            || (result.RawValue() < this->RawValue()))
        {
            ++result;
        }
        return result;
    };
    /// compares IOSequences
    /// @attention this method IGNORES a IOSequenceOverflow. Use it with care
    bool IsSmallerEqualWithoutOverflowCheck(const Log_IOSequenceNo rightOp) const
    {
        return RawValue() <= rightOp.RawValue();
    }
    /// compares IOSequences
    /// @attention this method IGNORES a IOSequenceOverflow. Use it with care
    bool IsSmallerWithoutOverflowCheck(const Log_IOSequenceNo rightOp) const
    {
        return RawValue() < rightOp.RawValue();
    }
    /// output to trace (new with PTS 1124727 mb 2003-10-27)
    friend SAPDB_OStream & operator << (SAPDB_OStream &ostream, Log_IOSequenceNo ioSeq);
    /// reading the UNSAFE value
    /// @attention calculations and comparisons with these unsigned values are dangerous
    const Log_UnsafeIOSequenceNo RawValue() const {  return (Log_UnsafeIOSequenceNo::operator ()());  }

private:

    /// private method for comparison
    /// @ATTENTION this method IGNORES a IOSequenceOverflow. Use it with care
    SAPDB_Bool operator >= (const Log_IOSequenceNo operand) const {return RawValue() >= operand.RawValue();};
    /// private method for comparison
    /// @ATTENTION this method IGNORES a IOSequenceOverflow. Use it with care
    SAPDB_Bool operator <= (const Log_IOSequenceNo operand) const {return RawValue() <= operand.RawValue();};
    /// private method for comparison
    /// @ATTENTION this method IGNORES a IOSequenceOverflow. Use it with care
    SAPDB_Bool operator > (const Log_IOSequenceNo operand) const  {return RawValue() >  operand.RawValue();};
    /// private method for comparison
    /// @ATTENTION this method IGNORES a IOSequenceOverflow. Use it with care
    SAPDB_Bool operator < (const Log_IOSequenceNo operand) const  {return RawValue() <  operand.RawValue();};
    /// private cast operator
    /// @ATTENTION calculations with unsigned integers does not take care of the IOSequenceOverflow
    operator SAPDB_UInt4()  const  { return RawValue(); }
    /// private cast operator
    /// @ATTENTION calculations with unsigned integers does not take care of the IOSequenceOverflow
    operator const SAPDB_UInt4()  const  { return RawValue(); }
    /// private method for reading a value
    /// @ATTENTION calculations with unsigned integers does not take care of the IOSequenceOverflow
    SAPDB_UInt4 operator()() const   {  return RawValue();  }
};
/// gives the first used Log_IOSequenceNo resp. the first Log-IOSequenceNo after an overflow
#define MIN_IOSEQUENCE Log_IOSequenceNo(0)
/// converts a IOSequence into a string for tracing purpose
inline SAPDB_ToStringClass SAPDB_ToString( Log_IOSequenceNo ioSeq, SAPDB_ToStringClass::fmtflags flags = _T_d  )
{
    if (ioSeq.IsInvalid())
    {
        return SAPDB_ToStringClass("NIL");
    }
    else
    {
        return SAPDB_ToString(ioSeq.RawValue(), flags); 
    }
}

/// log queue identifier
typedef SAPDB_Invalidatable<SAPDB_UInt2, SAPDB_MAX_UINT2> 
        Log_QueueID;
/// This is the sequence of log entries from on transaction
typedef SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4> 
        Log_EntrySequence;
/// This is the sequence of all savepoints.
typedef SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4> 
        Log_SavepointSequence;
/// Indicates whether partially filled log pages may be flushed more times onto the same block on log device, or not.
enum Log_FlushMode
{
    /// only works in single queue mode
    Log_DeviceMinimizeSpace,
    /// Every log page is only written once to an offset.
    Log_DeviceMaximizeSafety,
    ///
    Log_FlushMode_DO_NOT_USE
};
/// For convenient output
extern const char* Log_FlushModeStrings[Log_FlushMode_DO_NOT_USE];
extern const char* Log_SysviewFlushModeStrings[Log_FlushMode_DO_NOT_USE];
/*!
    @brief This describes what was written onto the logdevice and can be found.

    If a logdevice is just being cleared the state is: Log_DeviceStateClearing
        The next restart has to clear again.
    If a logdevice is new or was cleared it has the state: Log_DeviceStateCleared
        You can find nothing on it.
    If a logdevice was written with a savepoint it has the state: Log_DeviceStateHistoryLost
        The logdevice can be normally restarted.
    If a complete data backup was made, the logdevice state switches to: Log_DeviceStateOkay
        Only with the state Log_DeviceStateOkay a log backup is allowed.
 */
enum Log_DeviceState
{
    /// after SAVE DATA => SAVE LOG is now allowed
    Log_DeviceStateOkay              = 0,
    /// after writing a savepoint
    Log_DeviceStateHistoryLost       = 1,
    /// after CLEAR LOG
    Log_DeviceStateCleared           = 2,
    // during CLEAR LOG
    Log_DeviceStateClearing          = 3,
    /// after PARTIALLY CLEAR LOG (restore log until) PTS 1111525 mb 2002-01-29 new
    Log_DeviceStatePartiallyCleared  = 4,
    /// during PARTIALLY CLEAR LOG (restore log until) PTS 1111525 mb 2002-01-29 new
    Log_DeviceStatePartiallyClearing = 5,
    ///
    Log_DeviceState_DO_NOT_USE       = 6
};        
/// For convenient output
extern const char* Log_DeviceStateStrings[Log_DeviceState_DO_NOT_USE];
extern const char* Log_SysviewDeviceStateStrings[Log_DeviceState_DO_NOT_USE];

/// constants for different log error reasons
enum Log_ErrorState
{
    Log_Ok                              = 0,
    Log_LogAndDataIncompatible          = 1,
    Log_RawAllocationFailed             = 2,
    Log_PageAllocationFailed            = 3,
    Log_LogDeviceTooSmall               = 4,
    Log_InconsistentLogDeviceIOSequence = 5,
    Log_LogSaveDistanceTooLarge         = 6,
    Log_LogSizeHasDecreased             = 7,
    Log_InconsistentLogInfoPage         = 8,
    Log_SavepointPositionNotFound       = 9,
    Log_SaveForAbortedClearLog          = 10,
    Log_AreaOpenError                   = 11,
    Log_LogAndSoftwareIncompatible      = 12,
    Log_QueueCreationFailed             = 13,
    Log_ErrorState_DO_NOT_USE           = 14
};
/// for tracing the error states
extern const char* Log_ErrorStateStrings[Log_ErrorState_DO_NOT_USE];
/// All actions which can be un- and normally re-done are listed here.
enum Log_ActionType
{
    Log_NoOp = 0,

    /* transaction related actions */
    Log_Commit          = 1,
    Log_Rollback        = 2,
    Log_PartialRollback = 3,

    /* actions related to OMS */
    Log_NewObject        = 4,
    Log_InsertObject     = 5,
    Log_UpdateObject     = 6,
    Log_DeleteObject     = 7,
    Log_LockObject       = 8,
    Log_CreateObjectFile = 9,
    Log_DropObjectFile   = 10,

    /* actions related to SQL */
    Log_InsertRecord = 11,
    Log_DeleteRecord = 12,
    Log_UpdateRecord = 13,
    //
    Log_ModifyTable = 14,
    //
    Log_ChangeFileType   = 15,
    Log_CreateFile       = 16,
    Log_CreateIndex      = 17,
    Log_CreateTable      = 18,
    Log_RenameFile       = 19,
    Log_SuccFileVersion  = 20,
    Log_UpdFileStatistic = 21,
    //
    Log_CopyBlob     = 22,
    Log_CopySameBlob = 23,
    Log_ExpandBlob   = 24,
    Log_TruncateBlob = 25,
    Log_UpdateBlob   = 26,
    //
    Log_RefreshSequence  = 27,
    Log_RefreshSurrogate = 28,

    /* actions related to restart */
    Log_FinishOpenTrans = 29,
    Log_SavepointEntry  = 30,
    //
    Log_MarkedDeleted = 31,
    /* File States */
    Log_ReorganizeFile       = 32,
    Log_SetFileAttributes    = 33,
    Log_Reusable_2           = 34, // never used Log_UnloadFile
    Log_NotUsed_SetFileState = 35, // not used 
    //
    Log_CreateIndexFile   = 36,
    Log_ReassignSecondaryFile = 37,
    //
    Log_MarkFileAsDeleted = 38,
    Log_CreateShortColumnFile = 39,
    Log_UnqualifiedDelete = 40,
    Log_MultipleActions = 41,
    Log_SetFileDirCounter = 42,
    //
    Log_IllegalType      = 43, // PTS 1134693 UH 2005-04-06
    Log_ActionType_DO_NOT_USE = 44
};
/// for tracing the action types
const char* Log_GetActionTypeString(const Log_ActionType type);
/// this type defines the action version
// This should be use in combination only with Log_ActionType so it should become one class
typedef SAPDB_UInt1 Log_ActionVersion;

/// This differs the reasons to trigger a savepoint.
// keep in sync with Log_Types.cpp/Log_SavepointReasonStrings 
enum Log_SavepointReason
{
    Log_SVPReasonUnknown           = 0,
    Log_SVPReasonSaveData          = 1,
    Log_SVPReasonSavePages         = 2,
    Log_SVPReasonRestart           = 3,
    Log_SVPReasonShutdown          = 4,
    Log_SVPReasonDistance          = 5,
    Log_SVPReasonTime              = 6,
    Log_SVPReasonUserRequested     = 7,
    Log_SVPReasonTransRequested    = 8,  // not used
    Log_SVPReasonFBM               = 9,
    Log_SVPReasonDataWriter        = 10,
    Log_SVPReasonBDLockList        = 11, // not used
    Log_SVPReasonEndReadOnly       = 12,
    Log_SVPReasonDataCache         = 13, // not used
    Log_SVPReasonLogWriter         = 14,
    Log_SVPReasonConverter         = 15,
    Log_SVPReasonEndLoad           = 16,
    Log_SVPReasonCreateIndex       = 17,
    Log_SVPReasonCheckTable        = 18,
    Log_SVPReasonCatalogMigration  = 19,
    Log_SVPReasonSplitMirror       = 20,
    Log_SVPReasonDropVolume        = 21,
    Log_SVPReasonStartDropVolume   = 22,
    Log_SavepointReason_DO_NOT_USE = 23
};

/// for tracing the action types
extern const char* Log_SavepointReasonStrings[Log_SavepointReason_DO_NOT_USE];

/// this type combines all log entry characteristica
struct Log_EntryInfo
{
    /// IOSeq of the current entry
    Log_IOSequenceNo    iosequence;

    /// page-offset on the Device of the containing page
    Log_RawDeviceOffset pageoffset;

    /// offset of the entry in the page
    SAPDB_UInt2         entryoffset;

    /// action type of the entry
    Log_ActionType      entrytype;

    /// this is a speaking name
    SAPDB_UInt1         filler;

    /// date of this entry
    Kernel_Date         entrydate;

    /// time of this entry
    Kernel_Time         entrytime;

    /// default constructor
    Log_EntryInfo()
    {
        iosequence.Invalidate();
        pageoffset.Invalidate();
        entryoffset = SAPDB_MAX_UINT2;
        entrytype   = Log_NoOp;
        filler      = 0;
        entrydate.Invalidate();
        entrytime.Invalidate();
    }
    
    /// Writes the current entry-information to the trace
    void WriteToTrace(const char *title = 0);

    /// writes the current state/progress into the knldiag
    void Message (const char *title,
                  SAPDB_UInt  msgcount,
                  SAPDB_UInt  transno,
                  SAPDB_UInt  entrysequence) const;
	/// Appends info to errlist.
    void AppendContextInfo (SAPDB_Char  const * const  filename,
                            SAPDB_UInt4 const          linenumber,
                            SAPDB_UInt                 transno,
                            SAPDB_UInt                 entrysequence,
                            SAPDBErr_MessageList      &errlist) const;
};

#endif  /* LOG_TYPES_HPP */
