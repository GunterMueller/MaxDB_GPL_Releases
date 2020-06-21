/*!
    @file     SharedSQL_CommandStatistics.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    Statistics for Commands
    @see            

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

#ifndef SHAREDSQL_COMMANDSTATISTICS_HPP
#define SHAREDSQL_COMMANDSTATISTICS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_CommandStatistics
    @brief          CommandStatistics
 */
class SharedSQL_CommandStatistics
{
public:
    /*!
        @brief  Constructor
    */
    SharedSQL_CommandStatistics( void ) :
        mTotalPrepareCount(0),
        mTotalExecuteCount(0),
        mParseTime(0),		
        mExecuteTime(0),
        mMinExecuteTime(0),	
        mMaxExecuteTime(0),	
        mReadRowCount(0),	
        mQualifiedRowCount(0),	
        mReadCount(0),
        mPhysicalReadCount(0),
        mVirtualReadCount(0),
        mFetchRowCount(0),	
        mSuspendCount(0),
        mWaitCount(0)
    {};
    
    /*!
        @brief  Destructor
    */
    ~SharedSQL_CommandStatistics( void ) 
    {
    };

    /*!
        @brief  Operator=
        @param  S [in]
        @return ...
    */
    SharedSQL_CommandStatistics& operator=( const SharedSQL_CommandStatistics& S ) 
    {
        SAPDB_MemCopyNoCheck(this, &S, sizeof(*this));
        return *this;
    }

    /*!
        @brief  Operator+=
        @param  S [in]
        @return ...
    */
    SharedSQL_CommandStatistics& operator+=( const SharedSQL_CommandStatistics& S ) 
    {
        mTotalPrepareCount       += S.mTotalPrepareCount;
        mTotalExecuteCount       += S.mTotalExecuteCount;
        mParseTime               += S.mParseTime;		
        mExecuteTime             += S.mExecuteTime;
        mMinExecuteTime           = (mMinExecuteTime < S.mMinExecuteTime ? mMinExecuteTime : S.mMinExecuteTime);	
        mMaxExecuteTime           = (mMaxExecuteTime > S.mMaxExecuteTime ? mMaxExecuteTime : S.mMaxExecuteTime);		
        mReadRowCount            += S.mReadRowCount;	
        mQualifiedRowCount       += S.mQualifiedRowCount;	
        mReadCount               += S.mReadCount;
        mPhysicalReadCount       += S.mPhysicalReadCount;
        mVirtualReadCount        += S.mVirtualReadCount;
        mFetchRowCount           += S.mFetchRowCount;	
        mSuspendCount            += S.mSuspendCount;
        mWaitCount               += S.mWaitCount;
        return *this;
    }

    /*!
        @brief  Returns mTotalPrepareCount
        return  SAPDB_Int4
    */
    SAPDB_Int4  GetTotalPrepareCount( void )  { return (mTotalPrepareCount); };

    /*!
        @brief  Returns mTotalExecuteCount
        return  SAPDB_Int4
    */
    SAPDB_Int4  GetTotalExecuteCount( void )  { return (mTotalExecuteCount); };

    /*!
        @brief  Returns mParseTime
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetParseTime( void )          { return RTESys_AtomicRead(mParseTime); };

    /*!
        @brief  Returns mExecuteTime
        return  SAPDB_Int8
    */

    SAPDB_Int8  GetExecuteTime( void )        { return RTESys_AtomicRead(mExecuteTime); };

    /*!
        @brief  Returns mMinExecuteTime
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetMinExecuteTime( void )     { return RTESys_AtomicRead(mMinExecuteTime); };

    /*!
        @brief  Returns mMaxExecuteTime
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetMaxExecuteTime( void )     { return RTESys_AtomicRead(mMaxExecuteTime); };	

    /*!
        @brief  Returns mReadRowCount
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetReadRowCount( void )       { return RTESys_AtomicRead(mReadRowCount); };	

    /*!
        @brief  Returns mQualifiedRowCount
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetQualifiedRowCount( void )  { return RTESys_AtomicRead(mQualifiedRowCount); };	

    /*!
        @brief  Returns mReadCount
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetReadCount( void )          { return RTESys_AtomicRead(mReadCount); };

    /*!
        @brief  Returns mPhysicalReadCount
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetPhysicalReadCount( void )  { return RTESys_AtomicRead(mPhysicalReadCount); };

    /*!
        @brief  Returns mVirtualReadCount
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetVirtualReadCount( void )   { return RTESys_AtomicRead(mVirtualReadCount); };

    /*!
        @brief  Returns mFetchRowCount
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetFetchRowCount( void )      { return RTESys_AtomicRead(mFetchRowCount); };	

    /*!
        @brief  Returns mSuspendCount
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetSuspendCount( void )       { return RTESys_AtomicRead(mSuspendCount); };

    /*!
        @brief  Returns mWaitCount
        return  SAPDB_Int8
    */
    SAPDB_Int8  GetWaitCount( void )          { return RTESys_AtomicRead(mWaitCount); };


    /*!
        @brief  Increment mTotalPrepareCount
        @param  Delta [in]
    */
    void IncTotalPrepareCount( SAPDB_Int4 Delta=1 )    {  mTotalPrepareCount += Delta; };

    /*!
       @brief  Increment mTotalExecuteCount
       @param  Delta [in]
    */
    void IncTotalExecuteCount( SAPDB_Int4 Delta=1 )    {  mTotalExecuteCount += Delta; };

    /*!
       @brief  Set mParseTime (Time for the last parsing)
       @param  Time [in]
    */
    void SetParseTime( SAPDB_Int8 Time=0 )            {  mParseTime = Time; };

    /*!
       @brief  Increment mExecuteTime
       @param  Delta [in]
    */
    void IncExecuteTime( SAPDB_Int8 Delta=1 )         {  RTESys_AtomicModify(mExecuteTime, Delta); };

    /*!
       @brief  Set mMinExecuteTime
       @param  Val [in]
    */
    void SetMinExecuteTime( SAPDB_Int8 Val )          {  RTESys_AtomicWrite(mMinExecuteTime, Val); };

    /*!
       @brief  Set mMaxExecuteTime
       @param  Val [in]
    */
    void SetMaxExecuteTime( SAPDB_Int8 Val )          {  RTESys_AtomicWrite(mMaxExecuteTime, Val); };	

    /*!
       @brief  Increment mReadRowCount
       @param  Delta [in]
    */
    void IncReadRowCount( SAPDB_Int8 Delta=1 )         {  RTESys_AtomicModify(mReadRowCount, Delta); };	

    /*!
       @brief  Increment mQualifiedRowCount
       @param  Delta [in]
    */
    void IncQualifiedRowCount( SAPDB_Int8 Delta=1 )    {  RTESys_AtomicModify(mQualifiedRowCount, Delta); };	

    /*!
       @brief  Increment mReadCount
       @param  Delta [in]
    */
    void IncReadCount( SAPDB_Int8 Delta=1 )            {  RTESys_AtomicModify(mReadCount, Delta); };

    /*!
       @brief  Increment mPhysicalReadCount
       @param  Delta [in]
    */
    void IncPhysicalReadCount( SAPDB_Int8 Delta=1 )    {  RTESys_AtomicModify(mPhysicalReadCount, Delta); };

    /*!
       @brief  Increment mVirtualReadCount
       @param  Delta [in]
    */
    void IncVirtualReadCount( SAPDB_Int8 Delta=1 )     {  RTESys_AtomicModify(mVirtualReadCount, Delta); };

    /*!
       @brief  Increment mFetchRowCount
       @param  Delta [in]
    */
    void IncFetchRowCount( SAPDB_Int8 Delta=1 )        {  RTESys_AtomicModify(mFetchRowCount, Delta); };	

    /*!
       @brief  Increment mSuspendCount
       @param  Delta [in]
    */
    void IncSuspendCount( SAPDB_Int8 Delta=1 )         {  RTESys_AtomicModify(mSuspendCount, Delta); };

    /*!
       @brief  Increment mWaitCount
       @param  Delta [in]
    */
    void IncWaitCount( SAPDB_Int8 Delta=1 )            {  RTESys_AtomicModify(mWaitCount, Delta); };

private:
    /// Number of preparations
    SAPDB_Int4  mTotalPrepareCount;
    /// Number of excutions
    SAPDB_Int4  mTotalExecuteCount;
    /// Cumulated parse time
    SAPDB_Int8  mParseTime;	
    /// Cumulated execution time
    SAPDB_Int8  mExecuteTime;
    /// Minimum execution time
    SAPDB_Int8  mMinExecuteTime;
    /// Maximum execution time
    SAPDB_Int8  mMaxExecuteTime;	
    /// Number of read rows during execution
    SAPDB_Int8  mReadRowCount;	
    /// Number of qualified rows
    SAPDB_Int8  mQualifiedRowCount;	
    /// Number of reads during execution
    SAPDB_Int8  mReadCount;
    /// Number of physical read rows during execution
    SAPDB_Int8  mPhysicalReadCount;
    /// Number of veirtual read rows during execution
    SAPDB_Int8  mVirtualReadCount;
    /// Number of fetched rows during execution
    SAPDB_Int8  mFetchRowCount;	
    /// Number of suspends during execution
    SAPDB_Int8  mSuspendCount;
    /// Number of waits during execution
    SAPDB_Int8  mWaitCount;
    //
};  // SharedSQL_CommandStatistics

#endif 

