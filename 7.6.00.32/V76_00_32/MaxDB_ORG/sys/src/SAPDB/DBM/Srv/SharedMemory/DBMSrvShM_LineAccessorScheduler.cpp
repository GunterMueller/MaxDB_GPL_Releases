/*!
    @file           DBMSrvShM_LineAccessorScheduler.cpp
    @author         MarcW
    @brief          

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

#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "SAPDBCommon/SAPDB_string.h"

const char* const DBMSrvShM_LineAccessorScheduler::TEXT_NONE("Undefined");
const char* const DBMSrvShM_LineAccessorScheduler::TEXT_CLIENT("DBM Server");
const char* const DBMSrvShM_LineAccessorScheduler::TEXT_SCHEDULER("DBM Command Scheduler");

const char* DBMSrvShM_LineAccessorScheduler::getRecipientName( Recipient recipient ) {
    switch( recipient ) {
        case Rec_Client:
            return TEXT_CLIENT;
            break;
        case Rec_Scheduler:
            return TEXT_SCHEDULER;
            break;
        default:
            return TEXT_NONE;
            break;
    };
}

DBMSrvShM_LineAccessorScheduler::DBMSrvShM_LineAccessorScheduler(DBMSrvMsg_Error& errOut)
    : DBMSrvShM_LineAccessorBase() {

    if( !DBMSrvShM_ShmAdmin::TheAdmin().isInitialized() ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SCHEDCOM_NO_CREATE));
    }
}

bool DBMSrvShM_LineAccessorScheduler::removeLayout(DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    if( DBMSrvShM_ShmAdmin::TheAdmin().removeLineLayoutScheduler(errOut) )
        m_Exists = false;    
    
    DBMSrvMsg_Error unlockError;
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
    errOut.Overrule(unlockError);
    return (!m_Exists) && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::setSchedulerPid(DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutScheduler* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);
    
    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);
    if( exists() ) {
        le->setBGSRVPid(RTESys_ProcessID());
    }
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::getSchedulerPid(RTE_OSPid& pid, DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    DBMSrvShM_LineLayoutScheduler* le(NULL);

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
       return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( exists() )
        pid = le->getBGSRVPid();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::removeSchedulerPid(DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();
    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutScheduler* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( exists() )
        le->removeBGSRVPid();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::doCancel(DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutScheduler* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);    

    if( exists() )
        le->doCancel();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::unCancel(DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutScheduler* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);    

    if( exists() )
        le->unCancel();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::isCancelled(bool& iscanc, DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    DBMSrvShM_LineLayoutScheduler* le(NULL);

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(false, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);
    iscanc = exists() && le->isCancelled();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::clearContent(DBMSrvMsg_Error& errOut) {
    
    bool out(false);
    out = setInfoType(IT_None, errOut);
    out = out && setVariablePart("", errOut);
    out = out && setUserName("", errOut);
    out = out && setRecipient(Rec_None, errOut);
    return out;
}

bool DBMSrvShM_LineAccessorScheduler::getInfoType(InfoType& aInfoType, DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutScheduler* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);    

    if( exists() )
        aInfoType = (InfoType)le->getInfoType();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::setInfoType(InfoType aInfoType, DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutScheduler* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);
    
    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }
    
    m_Exists = (le != NULL);    

    if( exists() )
        le->setInfoType( aInfoType );

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::getUserName(char*& aUserName, DBMSrvMsg_Error& errOut) {
    
    DBMSrvShM_LineLayoutScheduler* le(NULL);
    errOut.ClearMessageList();

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);
    
    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }
    
    m_Exists = (le != NULL);

    if( exists() ) {
        size_t len = le->getUserNameLen();
        aUserName = new char[len+1];
        SAPDB_memcpy(aUserName, le->getUserName(), len);
        aUserName[len] = '\0';
    }

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::setUserName(const char* aUserName, DBMSrvMsg_Error& errOut) {
    
    DBMSrvShM_LineLayoutScheduler* le(NULL);
    errOut.ClearMessageList();

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( exists() ) {
        SAPDB_Byte* start = le->getUserName();
        size_t oldLen = le->getUserNameLen();
        size_t newLen = strlen(aUserName);
        if( DBMSrvShM_ShmAdmin::TheAdmin().shift(DBMSrvShM_ShmAdmin::TheAdmin().LineTypeScheduler, start, (int)newLen - (int)oldLen, errOut) ) {
            le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(false, errOut);
            m_Exists = (le != NULL);
            if( exists() )
                le->setUserName(aUserName);
        }
    }
    
    DBMSrvMsg_Error unlockError;
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
    errOut.Overrule(unlockError);
    
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::getVariablePart(char*& aVarPart, DBMSrvMsg_Error& errOut) {
    
    DBMSrvShM_LineLayoutScheduler* le(NULL);
    errOut.ClearMessageList();

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( exists() ) {
        size_t len = le->getVariablePartLen();
        aVarPart = new char[len+1];
        SAPDB_memcpy(aVarPart, le->getVariablePart(), len);
        aVarPart[len] = '\0';
    }

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::setVariablePart(const char* aVarPart, DBMSrvMsg_Error& errOut) {
    
    DBMSrvShM_LineLayoutScheduler* le(NULL);
    errOut.ClearMessageList();

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);
    
    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }
    
    m_Exists = (le != NULL);

    if( exists() ) {
        SAPDB_Byte* start = le->getVariablePart();
        size_t oldLen = le->getVariablePartLen();
        size_t newLen = strlen(aVarPart);
        if( DBMSrvShM_ShmAdmin::TheAdmin().shift(DBMSrvShM_ShmAdmin::TheAdmin().LineTypeScheduler, start, (int)newLen - (int)oldLen, errOut) ) {
            le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(false, errOut);
            m_Exists = (le != NULL);
            if( exists() )
                le->setVariablePart(aVarPart);
        }
    }

    DBMSrvMsg_Error unlockError;
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
    errOut.Overrule(unlockError);
    
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::setRecipient( Recipient aRecipient, DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutScheduler* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);    

    if( exists() )
        le->setRecipient(aRecipient);

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return exists() && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::recipientCheck( Recipient aRecipient, DBMSrvMsg_Error& errOut ) {
    
    Recipient currentRec(Rec_None);
    errOut.ClearMessageList();

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutScheduler* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutScheduler(true, errOut);
    
    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }
    
    m_Exists = (le != NULL);

    if( exists() ) 
        currentRec = (Recipient)le->getRecipient();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return (currentRec == aRecipient) && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::recipientWait( Recipient aRecipient, DBMSrvMsg_Error& errOut) {
    
    int i=0;
    bool messageForRecipientAvailable(false);

    while( i<1000 && !messageForRecipientAvailable ) {
        errOut.ClearMessageList();
        messageForRecipientAvailable = recipientCheck( aRecipient, errOut );
        if( !errOut.IsEmpty() )
            break;
        ++i;
        RTE_ISystem::DoSleep(300);
    }

    if( !messageForRecipientAvailable && errOut.IsEmpty() ) {
        Recipient peer(Rec_None);
        if( aRecipient == Rec_Client ) // name of partner
            peer = Rec_Scheduler;
        else if( aRecipient == Rec_Scheduler )
            peer = Rec_Client;
        errOut.Overrule(DBMSrvMsg_Error( SDBMSG_DBMSRV_COMM_NO_REACT,
	        Msg_Arg(SDBMSGTAG_DBMSRV_COMM_NO_REACT__COMMPARTNER, getRecipientName(peer)),
	        Msg_Arg(SDBMSGTAG_DBMSRV_COMM_NO_REACT__TIME_VALUE, (1000 * 300) / 1000 )));
    }

    return errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorScheduler::setRecipientClient(DBMSrvMsg_Error& errOut) {
    return setRecipient(Rec_Client, errOut);
}

bool DBMSrvShM_LineAccessorScheduler::setRecipientScheduler(DBMSrvMsg_Error& errOut) {
    return setRecipient(Rec_Scheduler, errOut);
}

bool DBMSrvShM_LineAccessorScheduler::clientWait(DBMSrvMsg_Error& errOut) {
    return recipientWait(Rec_Client, errOut);
}

bool DBMSrvShM_LineAccessorScheduler::isSchedulerCommand(DBMSrvMsg_Error& errOut) {
    return recipientCheck(Rec_Scheduler, errOut);
}
