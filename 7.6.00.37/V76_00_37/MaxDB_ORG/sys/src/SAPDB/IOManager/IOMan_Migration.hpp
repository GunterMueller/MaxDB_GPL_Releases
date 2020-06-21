/*!
  @file           IOMan_Migration.hpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This modul implements the inplace data base migration of
                  OLTP releases 7.2 and 7.3 to 7.4

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



#ifndef IOMAN_MIGRATION_HPP
#define IOMAN_MIGRATION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Vector.hpp"
#include "Converter/Converter_Page72.hpp"
#include "DataAccess/Data_Types.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_PageFrameAllocator.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "KernelCommon/Kernel_Page72.hpp"


class RTETask_ITask;
class IOMan_DataArea;
class IOMan_LogArea;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class    IOMan_Migration
  @brief    This class is used to handle the inplace data base migration
 */

class IOMan_Migration
{
public:

    /*!
       @brief   constructor
       @param   task          [in] own task
       @param   pageAllocator [in] allocator for i/o capable memory
       @param   allocator     [in] allocator for memory allocation
       @return  none
     */

    IOMan_Migration(
        RTETask_ITask&              task,
        IOMan_PageFrameAllocator&   pageAllocator,
        SAPDBMem_IRawAllocator&     allocator );

    /*!
       @brief   Used to initialize the IOMan_Migration object 
       @param   maxDataDevSpaces [in] maximum number of configurable data volumes
       @return  (SAPDB_Bool) true if successfully; else false
     */

    SAPDB_Bool Initialize(  const SAPDB_Int    maxDataDevSpaces );

    /*!
       @brief   destructor       
       @return  none
     */

    ~IOMan_Migration();

    /*!
       @brief   This static method is used to determine whether a data base 
                migration is needed, i.e. the system devspace has to migrate.
       @return  (SAPDB_Bool) true if a migration is needed; else false
     */

    static SAPDB_Bool IsMigrationNeeded();

    /*!
       @brief   This method is used to start migration
       @param   dataArea [in] handle for data area
       @param   logArea  [in] handle for log area
       @return  (SAPDB_Bool) true if migration was successfully
     */

    SAPDB_Bool ExecuteMigration(
        IOMan_DataArea  &dataArea,
        IOMan_LogArea   &logArea );

private:

    struct DevSpace
    {
        IOMan_DeviceNo      pddLogicalDevNo;
        SAPDB_Int4          pddPhysicalDevNo;
        IOMan_BlockCount    pddCapacity;
        SAPDB_Bool          pddOnline;
    };

    typedef Container_Vector<struct DevSpace>   DataDevSpaces;
    typedef DataDevSpaces::Iterator             DataDevSpaceIterator;
    typedef DataDevSpaces::ConstIterator        DataDevSpaceConstIterator;

    typedef Container_Vector<Data_PageNo>       PageNoList;
    typedef PageNoList::Iterator                PageNoListIterator;
    typedef PageNoList::ConstIterator           PageNoListConstIterator;

    typedef Container_Vector<IOMan_BlockAddress>    BlockList;
    typedef BlockList::Iterator                     BlockListIterator;
    typedef BlockList::ConstIterator                BlockListConstIterator;

    

    // Copy and assignment operator are not supported

    IOMan_Migration& operator=( const IOMan_Migration &migration );

    IOMan_Migration( const IOMan_Migration &migration );

    /* -----------------------------------------------------------------------*/
    /*! @name Devspace administration and I/O */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /*--------------------------------------------------------------------------
    function:     OpenSystemAndDataDevSpaces
    description:  This method is used to open the system devspace and all
                  configured data devspaces. 
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void OpenSystemAndDataDevSpaces();

    /*--------------------------------------------------------------------------
    function:     CloseSystemAndDataDevSpaces
    description:  This method is used to close the system devspace and all
                  opened data devspaces.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void CloseSystemAndDataDevSpaces();

    /*--------------------------------------------------------------------------
    function:     OpenDevSpace
    description:  This method is used to open a single devspace, which is 
                  identified by the given parameters.
    arguments:    devSpace [in/out] handle for the devspace
                  type     [in]     type of the devspace, for example sys or data
    return value: none
    --------------------------------------------------------------------------*/

    void OpenDevSpace(
        DevSpace                &devSpace,
        const tsp2_dev_type     type );

    /*--------------------------------------------------------------------------
    function:     CloseDevSpace
    description:  This method is used to close a single devspace, which is 
                  identified by the given parameters.
    arguments:    devSpace [in/out] handle for the devspace
    return value: none
    --------------------------------------------------------------------------*/

    void CloseDevSpace( DevSpace &devSpace );

    /*--------------------------------------------------------------------------
    function:     RestartFBM
    description:  This method is used to start the FBM. If an error occur
                  the return value is false.
    arguments:    none
    return value: bool
    --------------------------------------------------------------------------*/

    bool RestartFBM();

    /*--------------------------------------------------------------------------
    function:     ShutdownFBM
    description:  This method is used to stop the FBM.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void ShutdownFBM();

    /*--------------------------------------------------------------------------
    function:     FlushAndShutdownConverter
    description:  This method is used to flush the converter into the data
                  devspaces and to terminate the converter.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void FlushAndShutdownConverter();

    /*--------------------------------------------------------------------------
    function:     RemoveSystemDevspace
    description:  This method is used to remove the system devspace from the
                  data base configuration file. If an error occur the system 
                  will crash.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void RemoveSystemDevspace() const;

    /*--------------------------------------------------------------------------
    function:     CreateAndFormatLog
    description:  This method is used to create the log devspaces and to write
                  the nil log pages and the LogInfoPage in it. If an error occur 
                  the system will crash.
    arguments:    logArea  [in] handle for log devspace access
    return value: none
    --------------------------------------------------------------------------*/

    void CreateAndFormatLog( IOMan_LogArea   &logArea );

    /*--------------------------------------------------------------------------
    function:     ReadOldConverterPage
    description:  This method is used to read a old converter page from the
                  old system devspace. The page is identified by the converter
                  page no. The position of the page on the devspace is calculated
                  by this method. If a read error occur, the page will be read
                  until three time. If the error is still present the system will
                  crash.
    arguments:    convPageNo    [in] converter page no
                  converterPage [in] page handler
    return value: none
    --------------------------------------------------------------------------*/

    void ReadOldConverterPage(
        const Data_PageNo   convPageNo,
        Converter_Page72    &converterPage );

    /*--------------------------------------------------------------------------
    function:     ReadPageFromDevSpace
    description:  This method is used to read a page from the old system devspace
                  or the data devspace without using the IOMan_InfoPage or the
                  BAD flag. The page is identified by the page no and the block 
                  address. If a read error occur, the page will be read
                  up to three times. If the error is still present the system will
                  crash.
    arguments:    devSpaceType [in]     type of the given devspace
                  devSpace     [in/out] handle for the devspace to be read from
                  page         [in]     page handle
                  blockNo      [in]     block number within given devspace
                  blockCount   [in]     number of bytes to read default: 1
    return value: none
    --------------------------------------------------------------------------*/

    void ReadPageFromDevSpace(
        SAPDB_Char             *devSpaceType,
        DevSpace                &devSpace,
        Kernel_Page72           &page,
        const Data_PageNo       pageNo,
        const IOMan_BlockNo     blockNo,
        const IOMan_BlockCount  blockCount = 1 );

    /*--------------------------------------------------------------------------
    function:     WritePageToDevSpace
    description:  This method is used to write a page to the data base using
                  no IOMan_InfoPage and no BAD flag. If the "prepare for write"
                  check of the page fails or the I/O yields an error the system 
                  will crash.
    arguments:    devSpaceType [in]     type of the given devspace
                  devSpace     [in/out] handle for the devspace to be used
                  page         [in]     page handle
                  blockNo      [in]     block number within given devspace
                  blockCount   [in]     number of bytes to read default: 1
    return value: none
    --------------------------------------------------------------------------*/

    void WritePageToDevSpace(
        SAPDB_Char             *devSpaceType,
        DevSpace                &devSpace,
        Kernel_Page72           &page,
        const Data_PageNo       pageNo,
        const IOMan_BlockNo     blockNo,
        const IOMan_BlockCount  blockCount = 1 );

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Migration method */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /*--------------------------------------------------------------------------
    function:     DisplaceDataPages
    description:  This method is used to read data pages stored in the
                  reserved area of a data devspace and to write them into
                  the non reserved part of the data devspace. If an error 
                  occur the system will crash.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void DisplaceDataPages();

    /*--------------------------------------------------------------------------
    function:     MigrateRestartRecord
    description:  This method is used to read the restart record and subsequent
                  to make a migration from 7.2/7.3 format into 7.4. If the 
                  migration was successfully the restart record wil be stored 
                  in k57restart_rec. Before the migration starts the correctness
                  of the data devspaces and the restart record is checked as it
                  was made in 7.2/7.4. If an error occur the system will crash.
    arguments:    none
    return value: SAPDB_Bool
    --------------------------------------------------------------------------*/

    SAPDB_Bool MigrateRestartRecord();

    /*--------------------------------------------------------------------------
    function:     MigrateConverter
    description:  This method is used to read the old converter pages and to 
                  make a migration from 7.2/7.3 format into 7.4. The converter
                  entries are stored within the new converter, which is only
                  available in the main memory. No flush is executed! Additional
                  the FBM is used to update the block states, because the data 
                  pages stored in front of a data devspace hav eto be displaced,
                  because the space is needed for the IOMan_Info page and the
                  restart record. If an error occur the system will crash.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void MigrateConverter();

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    /*-------------------------------------------------------------------------
    declaration: m_Task
    description: own task 
    --------------------------------------------------------------------------*/

    RTETask_ITask   &m_Task;

    /*-------------------------------------------------------------------------
    declaration: m_PageAllocator
    description: Allocator for page frames. The page frame size is definded by
                 the data base configuration file.
    --------------------------------------------------------------------------*/

    IOMan_PageFrameAllocator    m_PageAllocator;

    /*-------------------------------------------------------------------------
    declaration: m_Frame
    description: page handle for page I/O
    --------------------------------------------------------------------------*/

    Data_PageFrame m_Frame;

    /*-------------------------------------------------------------------------
    declaration: m_DataDevSpaces
    description: handle for the data devspaces
    --------------------------------------------------------------------------*/

    DataDevSpaces   m_DataDevSpaces;

    /*-------------------------------------------------------------------------
    declaration: m_SysDevSpace
    description: handle for the system devspace 
    --------------------------------------------------------------------------*/

    DevSpace    m_SysDevSpace;

    /*-------------------------------------------------------------------------
    declaration: m_PageNoList
    description: The page no list contains the id's of the pages, which have
                 to be removed from there original position within the data
                 devspace. This is necessary because the IOMan_InfoPage and
                 the RestartRecord are stored on the first respectively second
                 block of a data devspace.
    --------------------------------------------------------------------------*/

    PageNoList  m_PageNoList;

    /*-------------------------------------------------------------------------
    declaration: m_BlockList
    description: The block list contains the block addresses of the pages, 
                 which have to be removed from there original position within 
                 the data devspace. This is necessary because the IOMan_InfoPage 
                 and the RestartRecord are stored on the first respectively second
                 block of a data devspace.
    --------------------------------------------------------------------------*/

    BlockList  m_BlockList;

    /*-------------------------------------------------------------------------
    declaration: m_UsedDataVolumes
    description: number of configured data devspaces 
    --------------------------------------------------------------------------*/

    SAPDB_UInt  m_UsedDataVolumes;

    /*-------------------------------------------------------------------------
    declaration: m_MaxDataPages
    description: maximum number of pages to be stored within data devspaces
    --------------------------------------------------------------------------*/

    SAPDB_UInt4  m_MaxDataPages;

    /*-------------------------------------------------------------------------
    declaration: m_ConverterVersion
    description: converter version of the data base to be migrated
    --------------------------------------------------------------------------*/

    Converter_Version  m_ConverterVersion;

    /*-------------------------------------------------------------------------
    declaration: m_UsmRoot
    description: page no which identifies the old USM root. This page will
                 be removed, because the 7,4 releases supports no USM.
    --------------------------------------------------------------------------*/

    Data_PageNo m_UsmRoot;
};

#endif  /* IOMAN_MIGRATION_HPP */
