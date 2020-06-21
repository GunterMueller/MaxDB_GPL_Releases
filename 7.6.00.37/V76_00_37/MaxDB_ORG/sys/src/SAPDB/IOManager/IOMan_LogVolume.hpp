/*!
  @file     IOMan_LogVolume.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module is used to handle a log volume.

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



#ifndef IOMAN_LOG_VOLUME_HPP
#define IOMAN_LOG_VOLUME_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vsp002.h"  // PASCAL: IO Types

#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_Volume.hpp"
#include "IOManager/IOMan_KernelPages.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "IOManager/IOMan_IPage.hpp"
#include "Logging/Log_Types.hpp"

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
   @class   IOMan_LogVolume
   @brief   This class is used to handle log volumes.
 */
/* -----------------------------------------------------------------------*/


class IOMan_LogVolume : public IOMan_Volume
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Constructor. 
                This method is used to set the member data to an initial state, 
                in particular the type to log. Additional the constructor of 
                the derived IOMan_Volume class is called. Note that the 
                logical devo won't be set.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_LogVolume()
    :
    IOMan_Volume( tsp2_dev_type::fromConst( sp2dt_log )),
    m_FirstOffset( 0 ),
    m_LastOffset( 0 )
    {}

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the individual logical log volume
                identifier.
       @param   devNo [in] Logical volume identifier
       @param   frame [in] Frame for the IOManInfoPage of the log volume
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void Initialize(
        const IOMan_DeviceNo            devNo,
        const Data_PageFrame   &frame)
    {
        IOMan_Volume::Initialize( devNo, frame );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Administration */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to bring a log volume from close mode into
                open mode. For more informations see IOMan_Volume::Open().
       @param   taskId        [in] Identification of the calling task.
       @param   logCapacity   [in] Current size in pages of log devices opened so far.
       @param   bOpenForWrite [in] true means that the log volume is read- and
                                   writable; else it is only readable.
       @return  (IOMan_ReturnCode) IOMan_Okay means open was successful
     */
    /* -----------------------------------------------------------------------*/

    IOMan_ReturnCode Open(
        const tsp00_TaskId      taskId,
        const IOMan_BlockCount  logCapacity,
        const bool              bOpenForWrite );

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Common information */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method provides the first logical block number of the
                log volume. For example if a log device consist of two 
                log volume with the capacity of 100 pages at each case. 
                The FirstOffset of the first volume is zero and of the 
                second is 100.
       @return  Log_NonCyclicRawDeviceOffset
     */
    /* -----------------------------------------------------------------------*/

    Log_NonCyclicRawDeviceOffset FirstOffset() const
    {
        return( m_FirstOffset );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method provides the last logical block number of the
                log volume. For example if a log device consist of two 
                log volume with the capacity of 100 pages at each case. 
                The LastOffset of the first volume is 99 and of the 
                second is 199.
       @return  Log_NonCyclicRawDeviceOffset
     */
    /* -----------------------------------------------------------------------*/

    Log_NonCyclicRawDeviceOffset LastOffset() const
    {
        return( m_LastOffset );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Input/Output */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to read one log page from a log volume 
                into the given memory. The correctness  of the read page will 
                be checked as far as three times if an error has occured. Each 
                error is loged in the knldiag file and the io buffer is dumped.
                In any case of a fatal error an emergency shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler of the wanted Page.
       @param   block  [in] Position of the wanted page within the volume.
       @return  (bool) true if no error occur; false if the checksum
                isn't correct
     */
    /* -----------------------------------------------------------------------*/

    bool ReadPage(
        const tsp00_TaskId         taskId,
        IOMan_IPage               &page,
        const IOMan_BlockAddress   &block )
    {
        SAPDBERR_ASSERT_STATE( block.GetDeviceNo() == m_LogicalDevNo );

        return( IOMan_Volume::Read( taskId, page, block.GetBlockNo()));
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to read a couple of log pages from a log 
                volume into the given memory. The correctness  of the read 
                pages will be checked as far as three times if an error has 
                occured. Each error is loged in the knldiag file and the io 
                buffer is dumped. In any case of a fatal error an emergency 
                shutdown is executed.
       @param   taskId     [in] Identification of the calling task.
       @param   pages      [in] Handler of the pages to be read
       @param   cluster    [in] Address of the pages to be read
       @param   startIndex [in] Specifies the first page in the page collection 
                                which is relevant for the following I/O. Usually
                                the startIndex is zero. Only in case of read
                                orders splited over more than one log volume
                                the startIndex is greater than zero.
       @return  (bool) true if no error occur; false if the checksum 
                isn't correct
     */
    /* -----------------------------------------------------------------------*/

    bool ReadPages(
        const tsp00_TaskId         taskId,
        IOMan_IKernelPages         &pages,
        const IOMan_ClusterAddress &cluster,
        const SAPDB_UInt           startIndex );

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write one log page from given memory into
                a log volume. Before the proper io is executed the correctness
                of the given page will be checked. Each error is loged in the 
                knldiag file and the io buffer is dumped. In any case of error 
                an emergency shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler of the wanted Page.
       @param   block  [in] Position of the wanted page within the volume.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void WritePage(
        const tsp00_TaskId          taskId,
        IOMan_IPage                &page,
        const IOMan_BlockAddress    &block )
    {
        SAPDBERR_ASSERT_STATE( block.GetDeviceNo() == m_LogicalDevNo );

        IOMan_Volume::Write( taskId, page, block.GetBlockNo());
    }


    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write log pages from given memory into
                a volume. Before the proper io is executed the correctness of  
                the given page will be checked. Each error is loged in the 
                knldiag file and the io buffer is dumped. In any case of error 
                an emergency shutdown is executed.
       @param   taskId     [in] Identification of the calling task.
       @param   pages      [in] Handler of the pages to be written
       @param   cluster    [in] Destination address of the pages to be written
       @param   startIndex [in] Specifies the first page in the page collection 
                                which is relevant for the following I/O. Usually
                                the startIndex is zero. Only in case of write
                                orders splited over more than one log volume
                                the startIndex is greater than zero.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void WritePages(
        const tsp00_TaskId          taskId,
        IOMan_IKernelPages          &pages,
        const IOMan_ClusterAddress  &cluster,
        const SAPDB_UInt            startIndex )
    {
        SAPDBERR_ASSERT_STATE( cluster.GetDeviceNo() == m_LogicalDevNo );

        IOMan_Volume::WriteVector( taskId, pages, cluster, startIndex );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write a log page onto a volume 
                The correctness of the page won't be  checked. In case of a 
                fatal errror during the write operation caused by the IO subsystem
                the error is logged in the knldiag file and the return code is false.
       @param   taskId  [in] Identification of the calling task.
       @param   pFrame  [in] Memory of the writeable pages.
       @param   blockNo [in] Position for the page within the volume.
       @return  (bool) true if the write operation is successful; else false.
     */
    /* -----------------------------------------------------------------------*/

    bool WriteSimple(
        const tsp00_TaskId      taskId,
        const tsp00_PageAddr    pFrame,
        const IOMan_BlockNo     blockNo )
    {
        return( IOMan_Volume::WriteSimple( taskId, pFrame, blockNo ));
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

protected:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Constructor. 
                This method is used to set the member data to an initial state, 
                in particular the type to log. Additional the constructor of 
                the derived IOMan_Volume class is called. Note that the 
                logical devo won't be set.
       @param   bIsSecondaryVolume [in] specifies whether this log volume is
                                        a secondary volume. This is only 
                                        possible if log mode dual is configured.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_LogVolume( const bool bIsSecondaryVolume )
    :
    IOMan_Volume( bIsSecondaryVolume ?
                  (tsp2_dev_type::fromConst( sp2dt_mirr_log )):
                  (tsp2_dev_type::fromConst( sp2dt_log ))),
    m_FirstOffset( 0 ),
    m_LastOffset( 0 )
    {}

private:

    /// Assignment operator is not supported

    IOMan_LogVolume& operator=( const IOMan_LogVolume &volume );

    // copy constructor is needed for newarray operation during initialization
    //IOMan_LogVolume( const IOMan_LogVolume &volume );

private:

    /*-------------------------------------------------------------------------
    declaration: m_FirstOffset
    description: This defines the logical first block number of an log volume.
                 For example if a log device consist of two log volume with 
                 the capacity of 100 pages at each case. The m_FirstOffset of 
                 the first volume is zero and of the second is 100.
    --------------------------------------------------------------------------*/

    Log_NonCyclicRawDeviceOffset m_FirstOffset;

    /*-------------------------------------------------------------------------
    declaration: m_LastOffset
    description: This defines the logical last block number of an log volume.
                 For example if a log device consist of two log volume with 
                 the capacity of 100 pages at each case. The m_LastOffset of 
                 the first volume is 99 and of the second is 199.
    --------------------------------------------------------------------------*/

    Log_NonCyclicRawDeviceOffset m_LastOffset;
};


#endif  /* IOMAN_LOG_VOLUME_HPP */
