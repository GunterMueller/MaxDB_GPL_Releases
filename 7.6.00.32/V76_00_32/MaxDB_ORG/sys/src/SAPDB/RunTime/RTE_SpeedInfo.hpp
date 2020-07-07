/*!---------------------------------------------------------------------
  @file           RTE_SpeedInfo.hpp
  @author         JoergM, RobinW
  @brief          DBM Support: SAPDB Instance and Database Registration and Management Interfaces
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
---------------------------------------------------------------------*/


#ifndef RTE_SPEEDINFO_HPP
#define RTE_SPEEDINFO_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "Container/Container_List.hpp"

#include <ctype.h>

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MAXRTESPEEDNAME_   SAPDB_MAX(SAPDB_MAX(SAPDB_MAX(sizeof(RTE_QUICK),sizeof(RTE_SLOW)),sizeof(RTE_FAST)),sizeof(RTE_TEST))

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
  @class RTE_SpeedInfo
  @brief speed info access class
 */
class RTE_SpeedInfo
{
private:
    SAPDB_UTF8        m_speedName[MAXRTESPEEDNAME_];
    SAPDB_Int4        m_speedId;
    bool              m_exists;
    const SAPDB_Char *m_ExecutableName;
public:
    /*!
      @brief ctor
      @param speedName [in] speed name
      @param speedId [in] speed id
      @param exists [in] flag to indicate speed exists 
      @para executableName [in] executable associated with speed
     */
    RTE_SpeedInfo(const SAPDB_Char *speedName,SAPDB_Int4 speedId,bool exists,const SAPDB_Char *executableName)
        : 
        m_speedId(speedId),
        m_exists(exists),
        m_ExecutableName(executableName)
    {
        SAPDB_UInt i=0;
        while(*(speedName+i))
        {
            m_speedName[i] = tolower(*(speedName+i));
            i++;
        }
    }
    
    /*!
      @brief access speed name
     */
    const SAPDB_UTF8 *Name(void) const
    {
        return m_speedName;
    }

    /*!
      @brief access executable name
     */
    const SAPDB_Char *ExecutableName(void) const
    {
        return m_ExecutableName;
    }

    /*!
      @brief update existence flag
     */
    void SetExists(bool exists)
    {
        m_exists = exists;
    }

    /*!
      @brief get existence flag
     */
    bool Exists(void) const
    {
        return m_exists;
    }

    /*!
      @brief assignment operator
     */
    inline bool operator == (const RTE_SpeedInfo &other) const
    {
        return m_speedId == other.m_speedId;
    }
};

extern RTE_SpeedInfo speedNone; ///< global none speed object
extern RTE_SpeedInfo speedFast; ///< global fast speed object
extern RTE_SpeedInfo speedQuick;///< global quick speed object
extern RTE_SpeedInfo speedSlow; ///< global slow speed object
extern RTE_SpeedInfo speedTest; ///< global test speed object

/*! speed list container */
typedef Container_List<RTE_SpeedInfo> RTE_SpeedList;

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif
