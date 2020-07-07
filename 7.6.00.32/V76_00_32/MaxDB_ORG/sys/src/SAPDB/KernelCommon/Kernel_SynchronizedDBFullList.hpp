/*!
  @file     Kernel_SynchronizedDBFullList.hpp
  @author   TorstenS
  @ingroup  KernelCommon
  @brief    This class can be used to collect tasks to suspend or resume them
            in case of db full. Additional external object is updated to
            detect db full situations for the dbmcli.
*/

/*
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

*/


#ifndef KERNEL_SYNCHRONIZEDDBFULLLIST_HPP
#define KERNEL_SYNCHRONIZEDDBFULLLIST_HPP

#include "KernelCommon/Kernel_SynchronizedTaskWaitList.hpp"
#include "KernelCommon/Kernel_ExtendedState.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"


/*!
   @class   Kernel_SynchronizedDBFullList
   @brief   wait queue class used to suspend and resume tasks in case
            of db full. Additional external object is updated to notify
            db full situations to the dbmcli.
 */

class Kernel_SynchronizedDBFullList : public Kernel_SynchronizedTaskWaitList
{
public:

    /*!
        @class   Kernel_DBFullWaitListAction
        @brief   this class implements the callback of the
                 Kernel_SynchronizedTaskWaitList class, which is used
                 to execute an action before a task is suspended or
                 resumed. The action contains the setting of the db full
                 indicator in the RTE shared object whichis used by the dbmcli
                 to handle db full situations
    */

    class Kernel_DBFullWaitListAction : public Kernel_IWaitListAction
    {
        /*!
            @brief   set db full state before the task is suspended and
                     the state is not yet db full
            @param   bIsEmpty [in] true if no task is waiting because of db full
            @return  none
        */
        virtual void ExecuteBeforeSuspend( const bool bIsEmpty ) const
        {
            if( bIsEmpty ){
                Kernel_ExtendedState::UpdateDatabaseFullState( 1 ); // db full
            }
        }

        /*!
            @brief   remove db full state before the task is resumed
            @return  none
        */
        virtual void ExecuteBeforeResume() const{
            Kernel_ExtendedState::UpdateDatabaseFullState( 0 ); // not db full
        }
    };

    /*!
       @brief   constructor for a synchronized db full wait queue
       @param   suspendReason  [in] reason used if a task should be suspended
       @return  none
     */

    Kernel_SynchronizedDBFullList( const SAPDB_Int2   suspendReason )
    :
    Kernel_SynchronizedTaskWaitList( suspendReason, &m_DBFullAction )
    {}

private:

    Kernel_DBFullWaitListAction   m_DBFullAction;
};

#endif  /* KERNEL_SYNCHRONIZEDDBFULLLIST_HPP */
