/*!
  @file     Converter_Pager.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    The class Converter_Pager is used to make io for converter pages.
            The pages are located in the data area. The block handling of
            the FBM is done in Converter_Pager class.
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2002-2004 SAP AG

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

#ifndef CONVERTER_PAGER_HPP
#define CONVERTER_PAGER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_PageFrameAllocator.hpp"
#include "Converter/Converter_Version.hpp"

class Converter_Index;
class Converter_Page;
class IOMan_BlockAddress;
class RTETask_ITask;

/*===========================================================================*
*  DEFINES                                                                  *
**/


/*===========================================================================*
 *  MACROS                                                                   *
 **/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 **/

/*!
  @interface    Converter_Pager
  @brief        Used to read and write converter pages from/into the data area.
 */

class Converter_Pager
{
public:

    /*!
        @brief  This constructor is used to build a converter pager.
                Note that he caller has to assure, that the read and write
                methods of this class never be called by another task, because
                the given taskId is used to lock concurrent used resources.
        @param  task            [in] own task
        @param  snapShotVersion [in] version of the converter snapshot
        @return none
    */

    Converter_Pager(
        const RTETask_ITask     &task,
        const Converter_Version &snapShotVersion );

    /// destructor
    virtual ~Converter_Pager(){
    };

    /*!
        @brief  Reads a converter page specified by a block address from the
                data area. A page frame will be allocated, if the given page
                handler has none. After reading the page the corresponding
                entry in the component FBM is updated. The method will crash
                the kernel if no memory is available or the read operations fails.
                Is is assumed and tested in the debug kernel that the running
                task is equal to the taskId given in the constructor.
        @param  block [in] block address of the requested page
        @param  page  [in] Handle to access the page in the main memory
        @return none
    */

    virtual void Read(
        const IOMan_BlockAddress    &block,
        Converter_Page              &page );

    /*!
        @brief  Writes a converter page into the data area. The state of the
                old block (if available) is updated in the component FBM. The
                new block location for the page is requested by the FBM.
                The method will crash, if the write operation fails.
                Is is assumed and tested in the debug kernel that the running
                task is equal to the taskId given in the constructor.
        @param  page  [in] Handle to access the page in the main memory
        @return none
    */

    void Write( Converter_Page  &page );

protected:

    /// Identification of running task
    const RTETask_ITask &m_Task;

    /// Version of the frozen converter
    const Converter_Version m_SnapShotVersion;

    /// Frame allocator used to get memory to store the requested converter page
    Converter_PageFrameAllocator    m_FrameAllocator;
};

/*!
  @interface    Converter_SnapShotPager
  @brief        This class is used to read converter pages from the data area
                being part of the converter snapshot. These converter leaves need
                a special FBM handling, i.e. if the read converter leaf page
                has the same version as the snapshot version the corresponding
                block is already set to occupied by the working converter.
 */

class Converter_SnapShotPager : public Converter_Pager
{
public:

    /*!
        @brief  This constructor is used to build a converter snapshot pager.
                Note that he caller has to assure, that the read and write
                methods of this class never be called by another task, because
                the given taskId is used to lock concurrent used resources.
        @param  task            [in] own task
        @param  snapShotVersion [in] version of the converter snapshot
        @param  index           [in] reference to the converter index
        @return none
    */

    Converter_SnapShotPager(
        const RTETask_ITask     &task,
        const Converter_Version &snapShotVersion,
        Converter_Index         &index )
    :
    Converter_Pager( task, snapShotVersion ),
    m_Index( index )
    {}

    /*!
    @brief  Reads a converter page specified by a block address from the
            data area. A page frame will be allocated, if the given page
            handler has none. After reading the page the corresponding
            entry in the component FBM is updated, if the converter version
            of the read page is greater than the converter snapshot version.
            The method will crash the kernel if no memory is available or
            the read operations fails.
            Is is assumed and tested in the debug kernel that the running
            task is equal to the taskId given in the constructor and that
            a converter snapshot is available.
    @param  block [in] block address of the requested page
    @param  page  [in] Handle to access the page in the main memory
    @return none
    */

    virtual void Read(
        const IOMan_BlockAddress    &block,
        Converter_Page              &page );

private:

    /// reference to the converter index

    Converter_Index &m_Index;
};

#endif  /* CONVERTER_PAGER_HPP */
