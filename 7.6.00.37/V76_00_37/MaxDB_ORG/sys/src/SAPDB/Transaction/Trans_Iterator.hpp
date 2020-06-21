/*!
  @defgroup  Transaction The component Transaction implements an interface to the global transaction handling.
*/
/*!
  @file           Trans_Iterator.hpp
  @author         UweH
  @ingroup        Transaction
  @since          2004-09-18

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
#ifndef Trans_Iterator_HPP
#define Trans_Iterator_HPP
//--------------------------------------------------------------------------------------
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/System/RTESys_Time.h"
#include "gsp00.h"
#include "gkb05.h"
//--------------------------------------------------------------------------------------
class RTETask_ITask;
class Log_Transaction;
class FileDir_IDirectory;
class tgg91_SessionNo;
class tgg91_TransNo;
class Trans_ExtendedMonitoringContext;
class Trans_Context;
//--------------------------------------------------------------------------------------
/// mustb equal to cgg_nil_transindex
#define TRANS_ITER_NIL_TRANS_INDEX 0
/// this value determines that no region is used
#define TRANS_ITER_NIL_REGION -1
//--------------------------------------------------------------------------------------
/// This iterator iterates over global transaction entries with a given criteria
class Trans_Iterator
{
public:
    /// The type of transactions the iterator should point to.
    enum TransType
    {
        All,
        WriteTransaction,
        RedoTransaction,
        HasTempFileDirectory,
        HasMonitoringInfo
    };
    /*!
    @brief default ctor inititlaizes internal structure
    @param task [in] rte task context used for region access
    @param type [in] type of entries the iterator should point to
    @param useRegion [in] (default=true) if this is set to false all trans regions should be entered before.
    */
    Trans_Iterator(RTETask_ITask &task,
                   TransType      type,
                   bool           useRegion = true)
    : m_TransIndex(TRANS_ITER_NIL_TRANS_INDEX),
      m_Type      (type),
      m_Task      (task),
      m_UseRegion (useRegion),
      m_InRegionId(TRANS_ITER_NIL_REGION)
    {}
    /// if a region was entered it is left now
    ~Trans_Iterator();
    /// returns true if this points to a valid entry
    bool IsValid() const
    {
        return m_TransIndex != TRANS_ITER_NIL_TRANS_INDEX;
    }
    /// sets the iterator to the next transaction which matches the given TransType
    /// This enters a region and does not releases it until next call or no entries found.
    bool Next();
    /// returns pointer to extended monitoring information block
    Trans_ExtendedMonitoringContext* GetMonitoringContext() const;
    /// returns the temp file directory pointer
    FileDir_IDirectory* GetTempFileDirContext() const;
    /// returns log trans context if available
    Log_Transaction* GetLogTransPtr() const;
    /// returns the session number
    void GetSessionNo (tgg91_SessionNo &sessiono) const;
    /// returns transaction SQL lock mode
    tkb05_TransLockMode_Param GetSqlLockMode() const;
    /// returns transaction OMS lock mode
    tkb05_TransLockMode_Param GetOmsLockMode() const;
    /*!
    @brief returns SQL lock request information
    @param task [out] ReqMode type of lock request ( share, exclusive )
    @return  request timeout time in seconds
    */
    SAPDB_Int GetSqlLockReqTimeout(tkb05_TransLockMode_Param &ReqMode) const;
    /*!
    @brief returns OMS lock request information
    @param task [out] ReqMode type of lock request ( share, exclusive )
    @return  request timeout time in seconds
    */
    SAPDB_Int GetOmsLockReqTimeout(tkb05_TransLockMode_Param &ReqMode) const;
    /// returns session startdate
    SAPDB_Int GetStartDate() const;
    /// returns session starttime
    SAPDB_Int GetStartTime() const;
    /// returns Task identification
    tgg00_TransIndex GetTaskId() const;
    /// returns the consistent view number
    void GetConsistViewNo (tgg91_TransNo &transno) const;
    /// returns the transaction number
    SAPDB_Int GetTimeLastWrite() const;
    /// returns transaction start time
    const SAPDB_Char* GetTransStartTime(RTESys_SQLTimestamp& timeBuffer);
    /// returns the transaction number
    void GetTransNo (tgg91_TransNo &transno) const;
    /// returns the write transno
    void GetWriteTransNo (tgg91_TransNo &transno) const;
    /// SelfTest
    static bool Test(Trans_Context &trans);
private:
   /// returns the next available global transentry if false is returned an error occured
    bool NextTransaction (SAPDB_Int2 &index,
                          bool        getFirst);

    void LockRegion(tkb05_RegionNo region);
    void UnlockRegion();
    SAPDB_Bool IsRegionLocked();

    /// internal pointer to current trans entry
    SAPDB_Int2 m_TransIndex;
    /// type of transactins accessed by this iterator
    const TransType  m_Type;
    /// this is used for synchronized access of the global transaction structures
    RTETask_ITask &m_Task;
    /// if set to true the trans entry region is used to access the trans entries
    bool m_UseRegion;
    /// This contains the region the iterator has entered. it is used to leave a region if necessary
    SAPDB_Int4 m_InRegionId;
};
#endif // Trans_Iterator_HPP
