/*!
  @file     IOMan_ILogIO.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for read and write access on the archive log.

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



#ifndef IOMAN_ILOGIO_HPP
#define IOMAN_ILOGIO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_LogPages.hpp"

class Log_Page;
class Log_InfoPage;
class Log_ClusterAddress;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
    @interface  IOMan_ILogIO
    @brief      This interface is used for read and write access on the archive log.
 */
/* -----------------------------------------------------------------------*/


class IOMan_ILogIO
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the access to the IOMan_ILogIO interface
                of component IOManager. It is assumed that the IOManager singleton 
                is available! No CreateInstance call will be done.
       @return  Access to the IOMan_ILogIO interface
     */
    /* -----------------------------------------------------------------------*/

    static IOMan_ILogIO& GetInstance();

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to read the log info page synchronous from 
                a given position of the archive log. The caller is blocked as 
                long as the read io is not completed. The correctness of the read 
                log info page will be checked. In case of errors detected by the 
                check method of the page the read io will be repeated. If the 
                error is still present after three read operations, the provided 
                buffer is dumped into the rundirectory of the kernel. Each failed 
                read operation is logged within the knldiag and knldiag error file. 
                In any case of errors an emergency shutdown is executed.If the
                log is mirrored the log device will be set to bad.
       @param   taskId  [in] Identification of the calling task.
       @param   page    [in] Handler for the wanted page.
       @param   address [in] Position of the wanted page within an given archive log
       @return  none
     */
    /* -----------------------------------------------------------------------*/
        
    virtual void ReadLogInfoPage(
        const tsp00_TaskId        taskId,
        Log_InfoPage              &page,
        const Log_ClusterAddress  &address ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write the log info page synchronous onto 
                a given position of the archive log. Before the proper write is 
                executed the correctness of the given page will be checked. In 
                case of an error the corrupted page is dumped into the kernel 
                rundirectory and an emergency shutdown is executed. In the other 
                 case the log info page is prepared for the io, for example the 
                checksum is calculated. In any case of errors an emergency 
                shutdown is executed.
       @param   taskId  [in] Identification of the calling task.
       @param   page    [in] Handler of the given pages
       @param   address [in] Position of the given page within an given archive log
       @return  none
     */
    /* -----------------------------------------------------------------------*/
    
    virtual void WriteLogInfoPage(
        const tsp00_TaskId        taskId,
        Log_InfoPage              &page,
        const Log_ClusterAddress  &address) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to read a log page synchronous from the
                archive log The caller is blocked as long as the read io is 
                not completed. The position of the wanted log page is given 
                as a log address  which will be transformed into a devspace 
                specific block address. The correctness of the read log info 
                page will be checked. In case of errors detected by the check 
                method of the page the read io will be repeated. If the error 
                is still present  after three read operations, the provided 
                buffer is dumped into the rundirectory of the kernel. Each 
                failed read operation is logged within the knldiag and knldiag 
                error file. In any case of errors an emergency shutdown is 
                executed. If the log is mirrored the log device will be set to bad.
       @param   taskId  [in] Identification of the calling task.
       @param   page    [in] Handler for the wanted page.
       @param   address [in] Position of the wanted page within an given archive log
       @return  none
     */
    /* -----------------------------------------------------------------------*/
    
    virtual void ReadLogPage(
        const tsp00_TaskId        taskId,
        Log_Page                  &page,
        const Log_ClusterAddress  &address ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to read a log page synchronous from the
                archive log The caller is blocked as long as the read io is 
                not completed. The position of the wanted log page is given 
                as a log address  which will be transformed into a devspace 
                specific block address. The correctness of the read log page 
                will NOT be checked. In case of IO-errors, the provided 
                buffer is dumped into the rundirectory of the kernel. Each 
                failed read operation is logged within the knldiag and knldiag 
                error file. In any case of errors an emergency shutdown is 
                executed. This method does not take care of LogMirroring;
       @attention Use this method carefully due to it's insecurity
       @param   taskId  [in] Identification of the calling task.
       @param   page    [in] Handler for the wanted page.
       @param   address [in] Position of the wanted page within an given LogVolume
       @return  none
     */
    /* -----------------------------------------------------------------------*/
    
    virtual void ReadLogPageWithoutCheck(
        const tsp00_TaskId        taskId,
        Log_Page                  &page,
        const Log_ClusterAddress  &address ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to read a couple of log pages synchronous 
                from the archive log.The caller is blocked as long as the read 
                io is  not completed. The position of the wanted log page is given 
                as a log address  which will be transformed into a devspace 
                specific block address. The correctness of the read log info 
                page will be checked. In case of errors detected by the check 
                method of the page the read io will be repeated. If the error 
                is still present  after three read operations, the provided 
                buffer is dumped into the rundirectory of the kernel. Each 
                failed read operation is logged within the knldiag and knldiag 
                error file. In any case of errors an emergency shutdown is 
                executed. If the log is mirrored the log device will be set to bad.
                NOTE: that it is assumed that the given page frames(!) contained
                in the page handler are in strictly ascending order within the 
                memory!
       @param   taskId  [in] Identification of the calling task.
       @param   pages   [in] Handler for the wanted pages.
       @param   address [in] Position of the wanted pages within an given archive log
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    virtual void ReadLogPages(
        const tsp00_TaskId          taskId,
        IOMan_LogPages              &pages,
        const Log_ClusterAddress    &address ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write one log page synchronous
                into one archive log. The caller is blocked as long as the write 
                io is not completed. Additional to the
                writeable log pages the caller has to provide a log address, which 
                 specifies the write destination on an log volume. The given log
                address will be transformed into a log devspace specific cluster 
                 address. Perhaps the I/O Management splits the order into separate 
                pieces conditional on the number of log devices belonging to one 
                 log volume. Before the proper write is executed the correctness of 
                the given pages will be checked. In case of an error the corrupted 
                page is dumped into the kernel rundirectory and an emergency shutdown 
                 is executed.In the other case the page is prepared for the io, for
                example the checksum is calculated. In any case of errors an emergency 
                shutdown is executed.
       @param  taskId  [in] Identification of the calling task.
       @param  page    [in] log page, which shall be written onto the volume
       @param  address [in] Position for the given pages within an given archive log
       @return none
     */
    /* -----------------------------------------------------------------------*/

    virtual void WriteLogPage(
        const tsp00_TaskId        taskId,
        Log_Page                  &page,
        const Log_ClusterAddress  &address ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write a number of log pages synchronous
                into one archive log. The caller is blocked as long as the write 
                io is not completed. The maximum number of writable log pages is
                 limited by a database configuration parameter. Additional to the
                writeable log pages the caller has to provide a log address, which 
                 specifies the write destination on an archive log. The given log
                address will be transformed into a log devspace specific cluster 
                 address. Perhaps the I/O Management splits the order into separate 
                pieces conditional on the number of log devspaces belonging to one 
                 archive log. Before the proper write is executed the correctness of 
                the given pages will be checked. In case of an error the corrupted 
                page is dumped into the kernel rundirectory and an emergency shutdown 
                 is executed.In the other case the page is prepared for the io, for
                example the checksum is calculated. Note that the writeable log pages 
                not necessarily stored in a contiguous memory block, because this 
                method uses scatter/gather io.In any case of errors an emergency 
                shutdown is executed.
       @param  taskId  [in] Identification of the calling task.
       @param  pages   [in] Handler of the given pages
       @param  address [in] Position for the given pages within an given archive log
       @return none
     */
    /* -----------------------------------------------------------------------*/
    
    virtual void WriteLogPages(
        const tsp00_TaskId        taskId,
        IOMan_LogPages            &pages,
        const Log_ClusterAddress  &address ) = 0;
};

#endif  /* IOMAN_ILOGIO_HPP */
