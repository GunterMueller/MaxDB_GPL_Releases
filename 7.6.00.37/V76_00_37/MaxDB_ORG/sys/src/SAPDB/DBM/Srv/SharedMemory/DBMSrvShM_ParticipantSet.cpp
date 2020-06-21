/*!
    @file           DBMSrvShM_ParticipantSet.cpp
    @author         MarcW
    @brief          administrating participants of shm communication

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

*/

#include "RunTime/System/RTESys_ProcessState.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ParticipantSet.hpp"

const RTE_OSPid DBMSrvShM_ParticipantSet::m_InvalidPID(0);

bool DBMSrvShM_ParticipantSet::removeZombies(DBMSrvMsg_Error& errOut) {
    for(int i=0; i<MAX_DBMSHM_PARTICIPANTS; ) {
        bool mustIncrement(true); 
        RTE_OSPid pid;
        m_PlayerPid[i].getValue(pid);
        if( pid != m_InvalidPID ) {
            RTESys_ProcessState procState(pid);
            if( procState.State() == RTESys_ProcessState::Died ) {
                // this one is dead, move all successors one position to front
                int j(i);
                RTE_OSPid tmpPid(m_InvalidPID);
                while( j < MAX_DBMSHM_PARTICIPANTS-1 ) {
                    m_PlayerPid[j+1].getValue(tmpPid);
                    m_PlayerPid[j].setValue(tmpPid);
                    ++j;
                }
                m_PlayerPid[i].setValue(m_InvalidPID);
                m_PlayerPid[MAX_DBMSHM_PARTICIPANTS-1].setValue(m_InvalidPID);
                mustIncrement = false;
            }
        }
        if( mustIncrement )
            ++i;
    }
    return true;
}
