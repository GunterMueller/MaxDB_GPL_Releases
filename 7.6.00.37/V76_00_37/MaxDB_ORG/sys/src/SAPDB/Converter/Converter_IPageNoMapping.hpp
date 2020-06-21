/*!
  @file           Converter_IPageNoMapping.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        Converter
  @brief          abstract interface class to those converter methods which concern
                  mapping of page numbers onto their physical device position
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


#ifndef CONVERTER_IPAGENOMAPPING_HPP
#define CONVERTER_IPAGENOMAPPING_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Types.hpp"
#include "DataAccess/Data_Types.hpp"

class IOMan_BlockAddress;
class Converter_Version;
class Data_PageRecoveryMode;
class RTETask_ITask;

/*!
  @interface      Converter_IPageNoMapping
  @brief          abstract interface class to those converter methods which concern
                  mapping of page numbers onto their physical device position
 */
class Converter_IPageNoMapping
{
public:

    /*!
       @brief   Returns the page number mapping interface of the converter
       @return  (Converter_IPageNoMapping) instance of the converter

       - This static function returns the interface of the converter which
         allows access to all methods defined in this interface class
     */
    static  Converter_IPageNoMapping & Instance();

    /*!
       @brief   Returns actual i/o address of data page with given page number
       @param   task             [in] own task
       @param   pageNo           [in] page number for which the corresponding block
                                      address is requested
       @param   pageRecoveryMode [in] recovery mode of the page for which data
                                      are position is wanted
       @param   block           [out] requested block address
       @return  (Converter_ReturnCode)
                Converter_Ok,
                Converter_NoConverterEntry,
                Converter_PageNoOutOfRange,
                Converter_InvalidBlockAddressFound
     */
  virtual Converter_ReturnCode GetPositionForReading(
            const RTETask_ITask         &task,
            const Data_PageNo           &pageNo,
            const Data_PageRecoveryMode &pageRecoveryMode,
            IOMan_BlockAddress          &block ) = 0;

    /*!
       @brief   Returns an i/o address where the data page can be written to.
                Depending on the Recoverability of the page an its converter version
                either the either the address of the currently occupied block is returned
                or a new block address is supplied and the currently used block is set to
                'free after save point'
       @param   task                 [in] own task
       @param   pageNo               [in] page number for which the corresponding
                                          block address is requested
       @param   pageRecoveryMode     [in] recovery mode of the page to write
       @param   pageConverterVersion [in] converter version of the page to write
       @param   bReqSequential       [in] access mode of volume
       @param   block               [out] requested block address
       @return  none
     */

    virtual void GetPositionForWriting(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &pageRecoveryMode,
        const Converter_Version     &pageConverterVersion,
        const SAPDB_Bool            bReqSequential,
        IOMan_BlockAddress          &block ) = 0;

    /*!
       @brief   Sets a new  i/o address into the converter entry of a given page.
                The block occupied so far is either set to 'free' for temp pages or
                set to 'free after save point' for permanent pages
       @param   task                 [in] own task
       @param   pageNo               [in] page number for which the corresponding
                                          block address is requested
       @param   pageRecoveryMode     [in] recovery mode of the page to write
       @param   pageConverterVersion [in] converter version of the page to write
       @param   block                [in] new block address which is to insert in the
                                          corresponding converter entry
       @return  none
     */

    virtual void SetPositionForWriting (
        const RTETask_ITask         &task,
        const Converter_Version     &pageConverterVersion,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &pageRecoveryMode,
        const IOMan_BlockAddress    &block ) = 0;
};

#endif  /* CONVERTER_IPAGENOMAPPING_HPP */
