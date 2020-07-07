/*!
  @file     IOMan_IDiagnose.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for diagnose/repair
  @see            

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



#ifndef IOMAN_IDIAGNOSE_HPP
#define IOMAN_IDIAGNOSE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types

#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_Types.hpp"

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
  @interface    IOMan_IDiagnose
   @brief       This interface is used for diagnose and repair purposes.
 */

class IOMan_IDiagnose
{
public:

    /*!
       @brief  This method returns the access to the IOMan_IDiagnose interface
               of component IOManager. It is assumed that the IOManager singleton 
               is available! No CreateInstance call will be done.
       @return Access to the IOMan_IDiagnose interface
     */


    static IOMan_IDiagnose& GetInstance();

    /*!
       @brief   This method is used to read a page from the given block address.
                from a data devspace. Note that NO consistent check is made.
       @param   taskId [in] Identification of the calling task.
       @param   block  [in] Destination position for the given page frame
       @param   pPage  [in] Pointer to the page frame to be written
       @return  true means write was successful; else false
     */

    virtual bool ReadPageDirectFromDataVolume(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        const tsp00_PageAddr        pPage ) = 0;

    /*!
       @brief   This method is used to write a given page into the given
                data devspace. Note that NO consistent check is made and NO
                checksum is calculated! 
       @param   taskId [in] Identification of the calling task.
       @param   block  [in] Destination position for the given page frame
       @param   pPage  [in] Pointer to the page frame to be written
       @return  true means write was successful; else false
     */

    virtual bool WritePageDirectToDataVolume(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        const tsp00_PageAddr        pPage ) = 0;

    /*!
       @brief   This method is used to read a page from the given block address.
                from the archive log.Note that NO consistent check is made. If
                the log is mirrored, it's possible to read the log page from
                the mirrored log volume of the device only. In a non mirrored 
                configuration the parameter bFromPrimary has no effect.
       @param   taskId       [in] Identification of the calling task.
       @param   block        [in] Destination position for the given page frame
       @param   pPage        [in] Pointer to the page frame to be written
       @param   bFromPrimary [in] Use primary log volume to read the page from
       @return  true means write was successful; else false
     */

    virtual bool ReadPageDirectFromLogVolume(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        const tsp00_PageAddr        pPage,
        const SAPDB_Bool            bFromPrimary ) = 0;

    /*!
       @brief   This method is used to write a given page into the given
                log devspace. Note that NO consistent check is made and NO
                checksum is calculated! If the log is mirrored, it's possible 
                to write the log page to the mirrored log volume of the device
                only. In a non mirrored configuration the parameter bToPrimary 
                has no effect.
       @param   taskId     [in] Identification of the calling task.
       @param   block      [in] Destination position for the given page frame
       @param   pPage      [in] Pointer to the page frame to be written
       @param   bToPrimary [in] Use primary log volume to write the page from
       @return  true means write was successful; else false
     */

    virtual bool WritePageDirectToLogVolume(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        const tsp00_PageAddr        pPage,
        const SAPDB_Bool            bToPrimary ) = 0;
};

#endif  /* IOMAN_IDIAGNOSE_HPP */
