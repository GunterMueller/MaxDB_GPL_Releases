/*!
  @file     IOMan_IMigration.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for for data base inplace migration from 7.2/7.3 to 7.4,7.5,7.6
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



#ifndef IOMAN_IMIGRATION_HPP
#define IOMAN_IMIGRATION_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

class SAPDBErr_MessageList;
class RTETask_ITask;

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
   @interface   IOMan_IMigration
   @brief       This interface is used for data base inplace migration 
                from 7.2/7.3 to 7.4, 7.5, 7.6
 */

class IOMan_IMigration
{
public:

    /*!
       @brief   This method returns the access to the IOMan_IMigration interface
                of component IOManager. It is assumed that the IOManager singleton 
                is available! No CreateInstance call will be done.
       @return  Access to the IOMan_ILogIO interface
     */

    static IOMan_IMigration& GetInstance();

    /*!
       @brief   This method is used to migrate a non liveCache data base
                from release 7.2/7.3 to release 7.4 inplace. This includes
                the migration of the converter and the scattering over all
                data volumes, the creation of the IOManInfoPages and the
                initialization of the log volumes. In case of a fatal error
                the system will execute and emergency shutdown. In case of a
                soft error is returned
       @param   task [in] own task
       @return  IOMan_ReturnCode
              - IOMan_Okay
              - IOMan_NoMoreMemory
              - IOMan_NotPreparedForMigration
     */

    virtual IOMan_ReturnCode MigrateDataBase( RTETask_ITask&    task ) = 0;

    /*!
       @brief   This method is used to adjust the capacity of the data and
                log volumes stored in the IOManInfoPage's. After this migration
                in db state admin the physical volume size and the configured
                volume size is identically.
       @param   task        [in] own task
       @param   msgList [in|out] messageList
       @return  (SAPDB_Bool)
     */

    virtual SAPDB_Bool MigrateVolumes( 
        RTETask_ITask&          task,
        SAPDBErr_MessageList&   msgList ) = 0;
};

#endif  /* IOMAN_IMIGRATION_HPP */
