/*!
  @file           Converter_IVerify.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          abstract interface class to the verify function of the converter
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

#ifndef CONVERTER_IVERIFY_HPP
#define CONVERTER_IVERIFY_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "DataAccess/Data_Types.hpp"
#include "Converter/Converter_Types.hpp"

class RTETask_ITask;


/*!
  @interface      Converter_IVerify
  @brief          abstract interface class to the verify function of the converter
 */
class Converter_IVerify
{
public:

    /*!
       @brief          returns the verify interface of the converter
       @return         (Converter_IVerify) instance of the converter

       - This static function returns the interface of the converter which
         allows access to all methods defined in this interface class
     */
    static  Converter_IVerify & Instance();

    /*!
      @brief   Prepares converter check in database state admin.
               This functionality is used by "Check Data With Update".
      @param   taskId  [in] task id
      @param   errMsg [out] detailed info about error state
      @return  Converter_ReturnCode
                - Converter_Ok
                - Converter_MemoryAllocationFailed (+errMsg)
    */

    virtual Converter_ReturnCode BeginCheckData( 
        const RTETask_ITask &task,
        Msg_List            &errMsg ) = 0;

    /*!
       @brief          Finishes converter check in database state admin.
                       This functionality is used by "CheckData With Update".
       @param          TaskId                    [in] task id
       @param          NumStaticPermDataPages   [out] number of permanent data pages used
                                                      with static page numbers
       @param          NumDynamicPermDataPages  [out] number of permanent data pages used
                                                      with dynamic page numbers
       @param          NumPermConvPages         [out] number of converter pages used for
                                                      permanent data pages
       @param          NumReleaseBlocks         [out] number of released blocks, which were
                                                      not accessed during check data
       @return         none

        - All pages which are not marked as used (see method MarkPageNoAsUsed) are released
        - The routine returns several parameters describing the actual state of the converter and of the database.
      */
    virtual void EndCheckData(
        const RTETask_ITask &task,
        Data_PageCount      &NumStaticPermDataPages,
        Data_PageCount      &NumDynamicPermDataPages,
        Data_PageCount      &NumPermConvPages,
        Data_PageCount      &NumReleaseBlocks ) = 0;

    /*!
       @brief   Finishes a failed converter check
       @param   taskId [in] task id
       @return  none

       - The converter is restored to the state it had before the verification
         was started by the BeginColdVerify
     */
    virtual void EndFailedCheckData( const RTETask_ITask &task ) = 0;

    /*!
       @brief   Marks a page in the converter as used
       @param   task     [in] own task
       @param   pageNo   [in] page number to be marked as used
       @param   addrMode [in] addressing mode of the page to mark
       @return  none
     */
    virtual void MarkPageNoAsUsed(
        const RTETask_ITask        &task,
        const Data_PageNo          &pageNo,
        const Data_AddressingMode  &addrMode ) = 0;
};

#endif  /* CONVERTER_IVERIFY_HPP */
