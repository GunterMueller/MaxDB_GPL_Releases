/*!
    @file           DBMSrvShM_ParticipantSet.hpp
    @author         MarcW, TiloH
    @brief          administrating participants of shm communication

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#if !defined(DBMSrvShM_ParticipantSet_HPP)
#define DBMSrvShM_ParticipantSet_HPP

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_Types.hpp"
#include "RunTime/System/RTESys_ProcessID.hpp"
#include "ToolsCommon/Tools_Vector.hpp"


#define MAX_DBMSHM_PARTICIPANTS 256

/*!
    @brief class for administration participants of the shared memory

    Every process using the the shared memory (also known as a participant
    or player of/in the shared memory game) should enter his process ID
    in the instance of this ParticipantSet in the shared memory.

    This critical base administration data for the shared memory 
    must be maintained with a valid lock of the shared memory.

    Only a fixed number of participants is allowed.

    @see DBMSrvShM_Base
*/
class DBMSrvShM_ParticipantSet {
public:
    /*! \brief clear the Participant set, used during initialization of the shared memory.*/
    void clear() {m_PlayerPid[0].setValue(m_InvalidPID);}   //all functions must honor the m_InvalidPID as end of set

    /*! \brief inserts a new PID into the set 
        \return true if successful, false otherwise (fixed length set is exhausted) */
    bool insert(const RTE_OSPid& aPid);

    /*! \brief check of a certain PID is in the set
        \return true if aPid is in the set, false otherwise */
    bool contains(const RTE_OSPid& aPid) {return getPosition(aPid) >= 0;}

    /*! \brief get all PID's from the set
        \param resultVector [OUT] a vector containing the PIDs */
    void getPIDsInto(Tools_Vector<RTE_OSPid>& resultVector);

    /*! \brief remove a PID from the set */
    void remove(const RTE_OSPid& aPid);
    bool removeZombies(DBMSrvMsg_Error& errOut);


    /*! \brief give maximal allowed number of participants
        \return maximal allowed number of participants */
    int getMaximalNumberOfParticipants() const {return (int)MAX_DBMSHM_PARTICIPANTS;}

private:
    DBMSrvShM_SaveType<RTE_OSPid> m_PlayerPid[MAX_DBMSHM_PARTICIPANTS];

    // returns -1 if not found, else index
    int getPosition(const RTE_OSPid& aPid);

    static const RTE_OSPid m_InvalidPID;
};

inline bool DBMSrvShM_ParticipantSet::insert(const RTE_OSPid& aPid)
{
    bool rc=true;

    if( !contains(aPid) ) //if aPid is really new
    {
        int index(getPosition(0)); //returns -1 if participant set is full

        if( index >= 0 )
            m_PlayerPid[index].setValue(aPid);
        else
            rc=false;
    }

    return rc;
}
    
/*inline RTE_OSPid DBMSrvShM_ParticipantSet::getPidAt(const int& pos) {
    RTE_OSPid retPid(m_InvalidPid);
    if( 0<=pos && pos <= MAX_DBMSHM_PARTICIPANTS )
        m_PlayerPid[pos].getValue(retPid);
    return retPid;
}*/

inline void DBMSrvShM_ParticipantSet::getPIDsInto(Tools_Vector<RTE_OSPid> &resultVector)
{
    int       index=0;
    RTE_OSPid tempPID;

    while(index<MAX_DBMSHM_PARTICIPANTS)
    {
        m_PlayerPid[index].getValue(tempPID);

        if(tempPID==m_InvalidPID)
            break;
        else
        {
            resultVector.push_back(tempPID); //TODO add out of memory reaction
            index++;
        }
    }
}

inline void DBMSrvShM_ParticipantSet::remove(const RTE_OSPid& aPid) {
    int index(getPosition(aPid));
    if( index >=0 ) {
        RTE_OSPid tmpPid(m_InvalidPID);

        while( index < MAX_DBMSHM_PARTICIPANTS-1 ) {
            m_PlayerPid[index+1].getValue(tmpPid);
            m_PlayerPid[index].setValue(tmpPid);
            index++;
        }
        m_PlayerPid[MAX_DBMSHM_PARTICIPANTS-1].setValue(m_InvalidPID);
    }
}

inline int DBMSrvShM_ParticipantSet::getPosition(const RTE_OSPid& aPid) {

    RTE_OSPid thePid(0);
    int index(0);
    while( index < MAX_DBMSHM_PARTICIPANTS) {
        m_PlayerPid[index].getValue(thePid);

        if( thePid == aPid )
            return index;
        else
            if(m_InvalidPID==thePid)
                return -1;              //0!=aPid, but already at an invalid PID -> aPid is not in the set
            else
                index++;
    }
    return -1;
}

#endif //DBMSrvShM_ParticipantSet_HPP
