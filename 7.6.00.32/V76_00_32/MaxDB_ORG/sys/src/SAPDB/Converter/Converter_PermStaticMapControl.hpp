/*!
  @file		Converter_PermStaticMapControl.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Manages the concurrent access to the structure mapping
            permanent static pageNo's to their physical block address
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


#ifndef CONVERTER_STATIC_MAPCONTROL_HPP
#define CONVERTER_STATIC_MAPCONTROL_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_PermMapControl.hpp"

#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_SynchronizedTaskWaitList.hpp"


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 **/

class Converter_PermMapSection;
class Converter_LeafPage;
class Converter_Pager;
class SAPDBMem_IRawAllocator;
class RTETask_ITask;


/*===========================================================================*
 *  DEFINES                                                                  *
 **/

/*!
  @class    Converter_PermStaticMapControl
  @brief    Administration object to handle concurrent access to the converter
            map, which deals with permanent static pages.
 */

class Converter_PermStaticMapControl: public Converter_PermMapControl
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction                                                    */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor
       @param   allocator [in|out] allocator for map resources
       @param   recMode       [in] recovery mode of the map control
     */

    Converter_PermStaticMapControl(
        SAPDBMem_IRawAllocator        &allocator,
        const Data_PageRecoveryMode   &recMode )
    :
    Converter_PermMapControl( allocator, recMode ),
    m_RequestSpecifiedNewPageNoWaitQueue( 238 )
    {}

    /* -----------------------------------------------------------------------*/
    /*! @name PageNo management                                               */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Reserves a particular page number, whereby the given
                pageNo is marked as used.
       @param   task                         [in] own task
       @param   pageNo                       [in] wanted page number
       @param   waitContext                  [in] wait item
       @param   freePageNoWaitQueue      [in|out] queue to manage suspended tasks
       @param   bWaitUntilSavepointFinished [out] inform caller about suspend situation
       @return  SAPDB_Bool false if wanted page is in use; else true
     */

    SAPDB_Bool  RequestSpecifiedNewPageNo(
        const RTETask_ITask                             &task,
        const Data_PageNo                               pageNo,
        Kernel_SynchronizedTaskWaitList::WaitContext    &waitContext,
        Kernel_SynchronizedTaskWaitList                 &freePageNoWaitQueue,           // PTS 1116298 UH 2002-06-21
        SAPDB_Bool                                      &bWaitUntilSavepointFinished ); // PTS 1116298 UH 2002-06-21

protected:

    /// virtual needed, because of re-implementation of PermMapControl method
    virtual void WriteChangedConverterPage(
        const RTETask_ITask      &task,
        Converter_PermMapSection &section,
        Converter_LeafPage       &leafPage,
        Converter_Pager          &pager,
        SAPDB_UInt               &totalPages,
        SAPDB_UInt               &totalIO );

private :

    /// This queue is used during restart if a new pageNo request wants
    /// to access a converter page which is still in I/O, because of a
    /// savepoint and therefore unavailable until the I/O operation is
    /// finsihed.

    Kernel_SynchronizedTaskWaitList m_RequestSpecifiedNewPageNoWaitQueue;

};

#endif // CONVERTER_STATIC_MAPCONTROL_HPP
