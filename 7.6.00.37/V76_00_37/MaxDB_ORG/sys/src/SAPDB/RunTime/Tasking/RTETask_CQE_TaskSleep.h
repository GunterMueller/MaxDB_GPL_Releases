/*!========================================================================

  @file         RTETask_CQE_TaskSleep.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Common queue element part (task sleep)

                The following class provides methodes to access
                task sleep part. This object is part of  the 
                common queue element. 

  @since        2004-12-10  13:37
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
 ============================================================================*/


#ifndef RTETASK_CQE_TASKSLEEP_H
#define RTETASK_CQE_TASKSLEEP_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!---------------------------------------------------------------------------
  @class   RTETask_CQE_TaskSleep
  @brief   Common queue element part (task sleep)

           This element part is used to handle task requests.

           This struct is designed to be used in C and C++ coding.
           It has no constructor and must be initialized via Init() methode.

           It is used as part of a union in conjunction with the legacy 
           part of the common queue element (RTETask_CommonQueueElement).
 ----------------------------------------------------------------------------*/
struct RTETask_CQE_TaskSleep
{
#if defined (__cplusplus)
public:
    /*!
    @brief         Initialize the this object.

                   The values must be set via set methodes.

    @param         WakeUpTime      [in] - Task wake up time
    */
    void Init( SAPDB_UInt8  WakeUpTime )
    {
        m_WakeUpTime = WakeUpTime;
    }



    /*!
    @brief         Get tasks wake up time.
    @return        Tasks wake up time
    */
    SAPDB_UInt8 WakeUpTime()
    {
        return m_WakeUpTime;
    }
private:
#endif
    /*  
     *  The C and C++ member data goes here....
     */
    SAPDB_UInt8    m_WakeUpTime;
};


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/



#endif  /* RTETASK_CQE_TASKSLEEP_H */
