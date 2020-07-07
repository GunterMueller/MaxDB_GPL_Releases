/*!
  @file           Converter_ICommon.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          very common simple functions of the converter

*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


#ifndef CONVERTER_ICOMMON_HPP
#define CONVERTER_ICOMMON_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Types.hpp"
#include "DataAccess/Data_Types.hpp"

class Converter_DBSpaceStatistic;
class Kernel_DateTime;
class RTETask_ITask;
class Msg_List;

class Converter_Version;

/*!
  @interface    Converter_ICommon
  @brief        very common simple functions of the converter
 */


class Converter_ICommon
{

public:
    /*!
       @brief   Returns the common interface of the converter
                which allows access to all methods defined in
                this interface class
       @return  Converter_ICommon) instance of the converter
     */

    static  Converter_ICommon & Instance();

    /*!
       @brief   Returns the actual converter version
       @return  (Converter_Version)
     */
    virtual Converter_Version Version() const = 0;

    /*!
        @brief   Returns the converter version of the snapshot.
                 If no snapshot is available, the invalid version
                 is returned.
        @return  (Converter_Version)
    */

    virtual Converter_Version GetSnapShotVersion() const = 0;

    /*!
       @brief   Checks whether converter backup is active
       @return  (SAPDB_Bool) true if active
     */

    virtual SAPDB_Bool BackUpIsActive() const = 0;

    /*!
    @brief  Returns the numbers of all  pages which can be stored
            in the data area
    @return Data_PageCount
    */

    virtual Data_PageCount GetNumberOfAllPages() const = 0;

    /*!
       @brief   Returns a number of data describing the filling state of the converter
       @param   task         [in] own task
       @param   dbSpaceInfo [out] collection of data base statistics
       @return  none
     */

    virtual void GetDbSpaceStatistics(
        const RTETask_ITask         &task,
        Converter_DBSpaceStatistic  &dbSpaceInfo ) = 0;


    /*!
    @brief   Reset current peak value of permanent and temporary 
             page consumption.
    @return  none
    */

    virtual void ResetSystemViewDataStatistics() = 0;

    /*!
    @brief   Return the time stamp of the last reset of systemview 
             DataStatisticsReset
    @return  SAPDB_UInt8 timeStamp
    */

    virtual SAPDB_UInt8 GetSystemViewResetTimeStamp() const = 0;

    /*!
        @brief  Returns the numbers of all  used pages, i.e
                perm and temp data pages and perm converter pages.
        @return SAPDB_UInt
    */

    virtual SAPDB_UInt GetNumberOfUsedPages() const = 0;

    /*!
       @brief   Returns the number of perm and perm static data pages which
                were changed since the last complete data base backup.
       @param   task [in] own task
       @return  SAPDB_UInt
     */

    virtual SAPDB_UInt UpdatedPagesSinceLastSaveData( const RTETask_ITask &task ) = 0;

    /*!
       @brief   Returns the number of permanent and permanent static
                converter pages marked as changed
       @return  (SAPDB_UInt)
     */

    virtual SAPDB_UInt GetNumberOfChangedConvPages() const = 0;

    /*!
      @brief    Returns the numbers of all currently used
                permanent and temporary converter pages
      @return   SAPDB_UInt
    */

    virtual SAPDB_UInt UsedConverterPages() const = 0;

    /*!
      @brief    Is the highest number of temporary pages
                used since the data base is restarted.
                With each savepoint this value is updated.
      @return   SAPDB_UInt
    */

    virtual SAPDB_UInt GetPeakForTempPages() const = 0;

    /*!
      @brief    Is the highest number of temporary pages
                used since the data base is restarted resp.
                the last reset of the systemview DataStatistics.
                With each savepoint this value is updated.
      @return   SAPDB_UInt
    */

    virtual SAPDB_UInt GetPeakForFrozenTempPages() const = 0;

    /*!
      @brief    Is the highest number of permanent and
                permanent static pages used since the
                data base is restarted. With each savepoint
                this value is updated.
      @return   SAPDB_UInt
    */

    virtual SAPDB_UInt GetPeakForPermPages() const = 0;
    /*!
      @brief    Is the highest number of permanent and
                permanent static pages used since the
                data base is restarted resp. the last 
                reset of the systemview DataStatistics.
                With each savepoint this value is updated.
      @return   SAPDB_UInt
    */

    virtual SAPDB_UInt GetPeakForFrozenPermPages() const = 0;

    /*!
       @brief   Returns the total number of permanent and
                permanent static pages.
       @return  (SAPDB_UInt)
     */

    virtual SAPDB_UInt GetNumberOfPermPages() const = 0;

    /*!
       @brief   Returns the total number of temporary pages
       @return  (SAPDB_UInt)
     */

    virtual SAPDB_UInt GetNumberOfTempPages() const = 0;

    /*!
       @brief   Returns the size of the converter snapshot.
                Note that the pages being part of the snapshot
                COULD be counted also by the number of permanent
                pages (GetNumberOfPermPages). This will be done
                only if the page is not changed since the snapshot
                creation.
       @return  (SAPDB_UInt)
     */

    virtual SAPDB_UInt GetNumberOfSnapShotPages() const = 0;

    /*!
       @brief   True is returned, if the data area is (nearly) full.
                Database full could not be detected, if the data base
                is in state db_offline or db_admin. If the data base
                is in state recovery or online the event is available.
       @return  (bool)
     */

    virtual bool IsDBFull() const = 0;

    /*!
       @brief   If the database filling is nearly full and therefore a
                sensible work is not possible this method returns a true
                else false.
       @return  true if security is reached and database nearly full else false
     */
    virtual SAPDB_Bool SecuritySpaceReached() const = 0;

    /*!
       @brief   If the database filling is about 90 percent and the converter
                is online and not in maintenance mode, for example cold verify,
                diagnose repair or save data cold, this method returns an enum
                to  signalize that garbage collector support is needed. The
                value Converter_GCNeeded determines that a garbage collection
                is needed.
       @return  Converter_GarbageCollectionReturnCode
     */
    virtual Converter_GarbageCollectionReturnCode GarbageCollectionNeeded() const = 0;

    /*!
       @brief   Returns the maximum possible temporary page number
       @return  (Data_PageNo)
     */

    virtual Data_PageNo MaxTempPageNo() const = 0;

    /*!
       @brief   Returns the maximum possible permanent, dynamic page number
       @return  (Data_PageNo)
     */

    virtual Data_PageNo MaxPermPageNo() const = 0;

    /*!
       @brief   Returns the maximum possible permanent, static page number
       @return  (Data_PageNo)
     */

    virtual Data_PageNo MaxStaticPageNo() const = 0;

    /*!
       @brief   If the number of changed converter pages and changed pages
                within data cache is nearly to the number of free blocks within
                FBM a savepoint is started. As long as this requested
                savepoint is finished this method will not request a new savepoint.
                This will be done only if the converter is online and not
                in maintenance mode. Additional the data area filling is observed
                and if the data area is full a data base full event is send.
       @param   task    [in]     task
       @param   msgList [in|out] message list
       @return  (bool)  true if no error occur else false
     */

    virtual bool CheckAndStartSavepoint(
        RTETask_ITask   &task,
        Msg_List        &msgList ) const = 0;

    /*!
       @brief   If the number of changed converter pages and changed pages
                within data cache is nearly to the number of free blocks within
                FBM a savepoint is needed. This will be done only if the
                converter is online and not in maintenance mode.
       @return  true if a savepoint is needed; else false
     */
    virtual SAPDB_Bool SavepointNeeded() const = 0;

    /*!
       @brief   Returns the value of the configuration parameter VOLUMENO_BIT_COUNT.
       @return  SAPDB_UInt
     */

    virtual SAPDB_UInt GetVolumeNoBitCount() const = 0;

    /*!
      @brief   Returns the savepoint 'timestamp' of the restart record which
               was frozen with the create snapshot command.
      @return  Kernel_DateTime
    */

    virtual Kernel_DateTime GetSnapShotCreationTime() const = 0;
};


#endif //CONVERTER_ICOMMON_HPP
