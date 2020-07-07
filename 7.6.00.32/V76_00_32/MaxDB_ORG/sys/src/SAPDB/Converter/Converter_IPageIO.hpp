/*!
  @file     Converter_IPageIO.hpp
  @author   Henrik
  @ingroup  Converter
  @brief    abstract restart interface class to the converter
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

#ifndef CONVERTER_IPAGEIO_HPP
#define CONVERTER_IPAGEIO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

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
  @interface    Converter_IPageIO
  @brief        abstract i/o interface class to the converter used to
                read and write converter pages from/to the data area.
 */
class Converter_IPageIO
{
public:

    /*!
    @brief  This method returns the access to the Converter_IPageIO interface
            of component Converter. It is assumed that the Converter singleton
            is available! No CreateInstance call will be done.
    @return Access to the Converter_IPageIO interface
    */
    static Converter_IPageIO& Instance();

    /*!
       @brief   This method is used to read a converter leaf page from the data area.
       @param   task        [in] own task
       @param   pagerIndex  [in] logical id of the calling pager task
       @param   totalPages [out] number of read pages
       @param   totalIO    [out] number of io's made to read the pages
       @return  none
     */

    virtual void ReadPages(
        const RTETask_ITask &task,
        SAPDB_UInt          pagerIndex,
        SAPDB_UInt          &totalPages,
        SAPDB_UInt          &totalIO ) = 0;
    /*!
       @brief   This method is used to read a converter leaf page which is
                part of the converter snap shot from the data area.
       @param   task        [in] own task
       @param   pagerIndex  [in] logical id of the calling pager task
       @param   totalPages [out] number of read pages
       @param   totalIO    [out] number of io's made to read the pages
       @return  none
     */

    virtual void ReadSnapShotPages(
        const RTETask_ITask &task,
        SAPDB_UInt          pagerIndex,
        SAPDB_UInt          &totalPages,
        SAPDB_UInt          &totalIO ) = 0;


    /*!
       @brief   Flush static and dynamic changed converter leaf pages of one
                section to the data area.
       @param   taskI       [in] own task
       @param   iSection    [in] converter section to flush
       @param   totalPages [out] number of changed pages
       @param   totalIO    [out] number of io's made to flush the pages
       @return  none
     */
    virtual void WritePages(
        const RTETask_ITask &task,
        const SAPDB_UInt    iSection,
        SAPDB_UInt          &totalPages,
        SAPDB_UInt          &totalIO ) = 0;
};

#endif  /* CONVERTER_IPAGEIO */
