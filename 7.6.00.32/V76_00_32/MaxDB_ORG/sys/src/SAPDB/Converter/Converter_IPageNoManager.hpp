/*!
  @file           Converter_IPageNoManager.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        Converter
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


#ifndef CONVERTER_PAGENOMANAGEMENT_HPP
#define CONVERTER_PAGENOMANAGEMENT_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Types.hpp"
#include "DataAccess/Data_Types.hpp"

class Converter_Version;
class Trans_Context;
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
  @interface      Converter_IPageNoManager
  @brief ...
  */
class Converter_IPageNoManager
{
public:

    /*!
       @brief   Returns the page number manager interface of the converter
       @return  (Converter_IPageNoManager) instance of the converter

       - This static function returns the interface of the converter which
         allows access to all methods defined in this interface class
     */
    static Converter_IPageNoManager & Instance();

    /*!
       @brief   Returns an unused temporary pageNo. If no pageNo is available,
                the task will be suspended as long as a pageNo is available.
       @param   task       [in] own task
       @param   newPageNo [out] new pageNo
       @return  none
     */

    virtual void RequestNewTempPageNo(
        const RTETask_ITask &task,
        Data_PageNo         &newPageNo ) = 0;

    /*!
       @brief   Returns an unused permanent pageNo. If no pageNo is
                available, the task will be suspended as long as a pageNo
                is available.If bIgnoreSecureLimit is set to true
                (not default behaviour) no secure space limit is considered,
                i.e. the task will be suspended if the data area is realy
                full. If not bIgnoreSecureLimit is set a secure space is held.
       @param   task              [in] own task
       @param   bIgnoreSecureLimit[in] use complete data area
       @param   newPageNo        [out] new pageNo
       @return  none
     */

    virtual void RequestNewPermPageNo(
        const RTETask_ITask &task,
        const bool          bIgnoreSecureLimit,
        Data_PageNo         &newPageNo ) = 0;

    /*!
       @brief   Returns an unused static pageNo. If no pageNo is available,
                the task will be suspended as long as a pageNo is available.
       @param   task       [in] own task
       @param   newPageNo [out] new pageNo
       @return  none
     */

    virtual void RequestNewStaticPageNo(
        const RTETask_ITask &task,
        Data_PageNo         &newPageNo ) = 0;

    /*!
       @brief   Checks whether specified pageid is still free and returns it
       @param   task   [in] own task
       @param   pageNo [in] requested page number
       @return  (SAPDB_Bool) false if page not is not available else true

       - After this operation the supplied page number is "used".
       - In contrast to method 'RequestNewStaticPageNo()', not any page number
         at choice, but a specific page number is requested.
       - If the specified page number is already used, the error
         e_multiple_converter_entry  is set in the TransContext.
       - If one of the database fill percent limits 80, 90, 95, 96,
         97, 98 and 99 is reached, a message is written to the knldiag
     */

    virtual SAPDB_Bool RequestSpecificiedNewStaticPageNo(
         const RTETask_ITask    &task,
         const Data_PageNo      &pageNo ) = 0;

     /*!
       @brief   Puts back the given temporary page number to
                the "pool" of unused numbers.
       @param   task   [in] own task
       @param   pageNo [in] number of the page to release
       @return  (SAPDB_Bool) true if successfully executed
     */

    virtual SAPDB_Bool FreeTempPageNo(
        const RTETask_ITask &task,
        const Data_PageNo   &pageNo ) = 0;

    /*!
       @brief   Puts back the given  permanent, dynamic page number to
                the "pool" of unused numbers.
       @param   task                 [in] own task
       @param   pageNo               [in] number of the page to release
       @param   pageConverterVersion [in] converter version of the page to freed
       @return  (SAPDB_Bool) true if successfully executed
     */

    virtual SAPDB_Bool FreePermPageNo(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Converter_Version     &pageConverterVersion ) = 0;

    /*!
       @brief   Puts back the given  permanent, static page number to
                the "pool" of unused numbers.
       @param   task                 [in] own task
       @param   pageNo               [in] number of the page to release
       @param   pageConverterVersion [in] converter version of the page to freed
       @return  (SAPDB_Bool) true if successfully executed
     */

    virtual SAPDB_Bool FreeStaticPageNo(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Converter_Version     &pageConverterVersion ) = 0;


    /*!
       @brief   Checks if the required count of pages is available. Note
                this function does not reserve any number of pages for
                this tasks, i.e. the information that a particular number
                is available  is dirty and may be even false, when this
                task really tries to get wanted number of free pages.

               - If the required count of pages is not available, the task is
                 suspended and waits until the wanted number of pages is available.
                 Sometimes tasks cannot be suspended since this could cause dead
                 locks. If noWait option is set in trans context and db full is
                 detected the task gets return code Converter_TempSpaceExhausted
               - If a db full situation is detected and the calling task has a hugh
                 number number of temporary pages consumed the return code
                 Converter_TempSpaceExhausted is set to force a rollback. If the
                 task is already in rollback state no return code is set.
               - In case of db full the cancel flag is checked and the returncode
                 Converter_ExecutionCanceled occur

       @param   trans             [in|out] transaction context
       @param   numDataPagesRequested [in] number of pages wanted
                                           data base to gain free pages
       @return  Converter_ReturnCode
                - Converter_Ok
                - Converter_TempSpaceExhausted
                - Converter_ExecutionCanceled
     */

    virtual Converter_ReturnCode HandleDBFull (
        Trans_Context   &trans,
        SAPDB_UInt      numDataPagesRequested ) = 0;
};

#endif  /* CONVERTER_PAGENOMANAGEMENT_HPP */
