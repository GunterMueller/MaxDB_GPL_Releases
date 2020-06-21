/****************************************************************************

  module      : RTESync_SpinlockReader.cpp

  -------------------------------------------------------------------------

  responsible : UlrichJ

  special area: RTE Spinlock Reader
  description : 

  last changed: 2002-08-21
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "gsp00.h"
#include "RunTime/Synchronisation/RTESync_SpinlockRegister.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockReader.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTESync_SpinlockReader::RTESync_SpinlockReader()
{
    m_pInfoArray  = NULL;
    m_Elements    = 0;
    m_Current     = 0;
}

/*---------------------------------------------------------------------------*/

RTESync_SpinlockReader::~RTESync_SpinlockReader()
{
    FreeArray();
}

void RTESync_SpinlockReader::FreeArray()
{
    if (m_pInfoArray)
    {
        RTESync_SpinlockRegister::Instance().FreeStatisticInfo(m_pInfoArray);
        m_pInfoArray  = NULL;
        m_Elements    = 0;
        m_Current     = 0;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::First( RTESync_Spinlock::Mode mode )
{
    if ( RTESync_SpinlockRegister::Instance().GetStatisticInfo(m_pInfoArray, m_Elements, mode ) )
    {
        m_Current = -1;
        return (true);
    }
    else
    {
        return (false);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::Next()
{
    m_Current ++;

    if ((!m_pInfoArray) || (m_Current >= m_Elements))
    {
        FreeArray();
        return (false);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::Name (SAPDB_UTF8 name[])
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        strcpy((SAPDB_Char *)name, (SAPDB_Char *)m_pInfoArray[m_Current].SpinlockName);
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::LockCount (SAPDB_Int8 &lockcount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        lockcount = m_pInfoArray[m_Current].StatisticRecord.locks;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::CollisionCount (SAPDB_Int8 &collisioncount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        collisioncount = m_pInfoArray[m_Current].StatisticRecord.collisions;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::SpinLoopCount (SAPDB_Int4 &spinloopcount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        spinloopcount = m_pInfoArray[m_Current].StatisticRecord.totalSpinLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::YieldLoopCount (SAPDB_Int4 &yieldloopcount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        yieldloopcount = m_pInfoArray[m_Current].StatisticRecord.totalYieldLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::MaxSpinLoopCount (SAPDB_Int4 &maxspinloopcount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxspinloopcount = m_pInfoArray[m_Current].StatisticRecord.maxSpinLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::MaxYieldLoopCount (SAPDB_Int4 &maxyieldloopcount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxyieldloopcount = m_pInfoArray[m_Current].StatisticRecord.maxYieldLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockReader::CurrentLoops (SAPDB_Int4 &currentloops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        currentloops = m_pInfoArray[m_Current].StatisticRecord.currentLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
