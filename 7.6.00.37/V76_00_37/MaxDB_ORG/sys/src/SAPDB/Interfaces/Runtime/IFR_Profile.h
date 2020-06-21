/*!
  @file           IFR_Profile.h
  @author         MarcoP
  @ingroup        IFR_Profile
  @brief          Collect profile data of SQLDBC
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

#ifndef IFR_PROFILE_H
#define IFR_PROFILE_H

#include "SAPDB/Interfaces/Runtime/IFR_Types.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_LinkedList.h"

IFR_BEGIN_NAMESPACE

class IFR_Environment;

/**
 * @ingroup IFR_Profile
 * Class provides methods to collect, retrieve and reset profile data of SQLDBC. The class implements the singleton pattern.
 * An instance can get from the IFR_Environment class.
 */
class IFR_Profile {

public:
    IFR_Profile( IFRUtil_LinkedList<IFR_Environment>* list);
    
    /**
     * @brief Collects the counter values for all profile counters.
     */
   void collectCounters(); 


    /**
     * @brief Gets the value for the counter specified by value.
     * @param value - counter to retrieve the value
     * @return the value corresponding to the given counter
     */
    IFR_UInt8 getCounter(IFR_ProfileCountValues value);

    /**
     * @brief Gets the value for the counter specified by value.
     * @param value - counter to retrieve the value
     * @return the value corresponding to the given counter
     */
    const char* getProfileCounterName(IFR_ProfileCountValues value); 

    void resetProfileCounters();
    
private:
    friend class IFR_Environment;
    friend class IFR_ParseInfoCacheImpl;
    void initCounters(){
      memset(&m_counter[0], 0, IFR_PROFILE_MAX*sizeof(IFR_UInt8));
    }

    IFR_UInt8 m_counter[IFR_PROFILE_MAX];
    IFRUtil_LinkedList<IFR_Environment>* m_EnvironmentList;
};

IFR_END_NAMESPACE

#endif // IFR_PROFILE_H

