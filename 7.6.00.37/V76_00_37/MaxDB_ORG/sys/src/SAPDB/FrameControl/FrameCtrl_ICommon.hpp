/*!
  @file     FrameCtrl_ICommon.hpp
  @author   TorstenS
  @ingroup  FrameCrontrol
  @brief    Interface to get common information about the component FrameControl.

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



#ifndef FRAMECTRL_ICOMMON_HPP
#define FRAMECTRL_ICOMMON_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "KernelCommon/Kernel_Common.hpp"

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
  @interface    FrameCtrl_ICommon
   @brief       This interface is used to get common information about
                the component FrameControl.
 */

class FrameCtrl_ICommon
{
public:

    /*!
        @brief  This method returns the access to the FrameCtrl_ICommon 
                interface of component FrameControl. It is assumed that the 
                FrameControl singleton is available! No CreateInstance call 
                will be done.
        @return Access to the FrameCtrl_ICommon interface
     */

    static FrameCtrl_ICommon& GetInstance();

    /*!
       @brief   This method is used to get the number of frames currently 
                available and therefore requestable by the new methods.
       @return  SAPDB_UInt Number of unused frames
     */

    virtual SAPDB_UInt GetFreeFrameCount() const = 0;

    /*!
       @brief   This method is used to get the page size of the data base
       @return  SAPDB_UInt page size in bytes
     */
    virtual SAPDB_UInt GetPageSize() const = 0;

    /*!
       @brief   This method is used to get the value of the configured CACHE_SIZE
       @return  SAPDB_UInt number of pages
     */
    virtual SAPDB_UInt GetCacheSize() const = 0;

  
    /*!
       @brief   This method is used to get the number of converter pages in use
       @return  SAPDB_UInt number of pages
    */
      virtual SAPDB_UInt GetConverterSize() const = 0;

    /*!
       @brief   This method is used to get the number of pages in use for the data cache
       @return  SAPDB_UInt number of pages
     */
    virtual SAPDB_UInt GetDataCacheSize() const = 0;

    /*!
       @brief   This method is used to get the number of pages in use for the log queue
       @return  SAPDB_UInt number of pages
     */
    virtual SAPDB_UInt GetLogQueueSize() const = 0;

    /*!
       @brief   This method is used to get the number of pages in use for the IOManagement
       @return  SAPDB_UInt number of pages
     */
    virtual SAPDB_UInt GetIOManagementSize() const = 0;

    /*!
       @brief   This method is used to get the number of pages in use for the file directory
       @return  SAPDB_UInt number of pages
     */
    virtual SAPDB_UInt GetFileDirSize() const = 0;

    /*!
       @brief   This method is used to get the number of pages in use for the (snapshot) restart page(s)
       @return  SAPDB_UInt number of pages
     */
    virtual SAPDB_UInt GetRestartRecordSize() const = 0;
   
    /*!
       @brief   This method is used to get the number of pages in use for the block allocator
       @return  SAPDB_UInt number of pages
     */
    virtual SAPDB_UInt GetBlockAllocatorSize() const = 0;

    /*!
       @brief   This method is used to get the number of pages in use by the parallel index creation
       @return  SAPDB_UInt number of pages
     */
    virtual SAPDB_UInt GetCreateIndexSize() const = 0;

};

#endif  /* FRAMECTRL_ICOMMON_HPP */
