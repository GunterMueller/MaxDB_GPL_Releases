/*!
  @file           Converter_IDiagnose.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          Interface for diagnose/repair purpose
  
\if EMIT_LICENCE
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
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
\endif
*/



#ifndef CONVERTER_IDIAGNOSE_HPP
#define CONVERTER_IDIAGNOSE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_Types.hpp"

class Data_PageRecoveryMode;
class IOMan_BlockAddress;
class RTETask_ITask;

/*!
   @class   Converter_IDiagnose
   @brief   This interface is used for diagnose/repair purpose.
*/

class Converter_IDiagnose
{
public:

    /*!
       @brief   This method returns the access to the Converter_IDiagnose interface
                of component Converter. It is assumed that the Converter singleton 
                is available! No CreateInstance call will be done.
       @return  Access to the Converter_IDiagnose interface
     */

    static  Converter_IDiagnose & Instance();

    /*!
       @brief   This method is used to get the block address for a given data
                page number. This operation is possible in db_state admin and 
                db_state online. It will be executed by the following command:
                DIAGNOSE GET BLOCK ADDRESS FOR 'MODE' PAGE 'PNO'.
       @param   task     [in] own task
       @param   pageNo   [in] page number for which the block address is requested
       @param   recMode  [in] recovery mode for the given page number
       @param   block   [out] if a block address is available than the
                              returned block address is valid, else it's invald. 
                              Reserved page no's are handled as invalid.
       @return  (SAPDB_Bool) true, if valid block could be returned
     */

    virtual SAPDB_Bool GetBlockAddress(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &recMode,
        IOMan_BlockAddress          &block) = 0;

    /*!
       @brief   This method is used to set the block address for a given data
                page number. This operation is possible in db_state admin only 
                It will be executed by the following command:
                DIAGNOSE SET BLOCK ADDRESS FOR 'MODE' PAGE 'PNO' TO VOLUMENO
                'VOLUMENO' BLOCKNO 'BLOCKNO'
                If the given parameter are out of range or invalid nothing will
                happen. The return value will show whether a modification was done.
       @param   task     [in] own task
       @param   pageNo   [in] page number for which the block address is requested
       @param   recMode  [in] recovery mode for the given page number
       @param   block    [in] wanted new block address for the given data page no.
       @return  (SAPDB_Bool) true. if the operation was executed.
     */

    virtual SAPDB_Bool SetNewBlockAddress(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &recMode,
        const IOMan_BlockAddress    &block ) = 0;

    /*!
       @brief   This method is used to free the given data page no and to 
                release the corresponding block. This operation is possible 
                in db_state admin only. It will be executed by the following 
                command:
                DIAGNOSE DELETE BLOCK ADDRESS FOR 'MODE' PAGE 'PNO' 
                If the given parameter are out of range or invalid nothing will
                happen. The return value will show whether a modification was done.
       @param   task    [in] own task
       @param   pageNo  [in] page number for which the block address is requested
       @param   recMode [in] recovery mode for the given page number
       @return  (SAPDB_Bool) true. if the operation was executed
     */

    virtual SAPDB_Bool DeleteBlockAddress(
        const RTETask_ITask         &task,
        const Data_PageNo           &pageNo,
        const Data_PageRecoveryMode &recMode ) = 0;
};

#endif  /* CONVERTER_IDIAGNOSE_HPP */
