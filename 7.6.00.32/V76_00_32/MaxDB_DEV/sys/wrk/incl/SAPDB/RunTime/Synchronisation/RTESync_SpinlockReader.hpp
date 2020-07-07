/*!**************************************************************************

  module      : RTESync_SpinlockReader.hpp

  -------------------------------------------------------------------------

  responsible : UlrichJ

  special area: RTE Spinlock Reader
  description : This is a Reader class for sequential reading of the
                spinlock information.
                For more information about how to use this reader
                class, please take a look at: 
                SystemViews\SysView_SpinLockStatistics.cpp -> Execute()

  last changed: 2002-08-14
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/


#ifndef RTESYNC_SPINLOCKREADER_HPP
#define RTESYNC_SPINLOCKREADER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Synchronisation/RTESync_SpinlockRegister.hpp"

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



/*!***************************************************************************

   class: RTESync_SpinlockReader

   description: 

******************************************************************************/

class RTESync_SpinlockReader
{

public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    RTESync_SpinlockReader (void);
    
    ~RTESync_SpinlockReader (void);

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Iteration                                                    */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool First (RTESync_Spinlock::Mode mode = RTESync_Spinlock::normal);

    SAPDB_Bool Next (void);

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Iteration                                                 */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Data Access                                                  */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool Name (SAPDB_UTF8 name[]);

    SAPDB_Bool LockCount (SAPDB_Int8 &lockcount);

    SAPDB_Bool CollisionCount (SAPDB_Int8 &collisioncount);

    SAPDB_Bool SpinLoopCount (SAPDB_Int4 &spinloopcount);

    SAPDB_Bool YieldLoopCount (SAPDB_Int4 &yieldloopcount);

    SAPDB_Bool MaxSpinLoopCount (SAPDB_Int4 &maxspinloopcount);

    SAPDB_Bool MaxYieldLoopCount (SAPDB_Int4 &maxyieldloopcount);

    SAPDB_Bool CurrentLoops (SAPDB_Int4 &currentloops);

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Data Access                                               */
    /* -----------------------------------------------------------------------*/


private:

    void FreeArray (void);

    RTESync_SpinlockRegister::StatisticInfo     *m_pInfoArray;
    SAPDB_Int4                                  m_Elements;
    SAPDB_Int4                                  m_Current;
    
};

/*!***************************************************************************

   endclass: RTESync_SpinlockReader

******************************************************************************/

#endif  /* RTESYNC_SPINLOCKREADER_HPP */

