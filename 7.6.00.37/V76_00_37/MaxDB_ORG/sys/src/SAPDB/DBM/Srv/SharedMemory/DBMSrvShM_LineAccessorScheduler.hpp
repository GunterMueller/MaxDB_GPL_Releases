/*!
    @file           DBMSrvShM_LineAccessorSchduler.hpp
    @author         MarcW
    @brief          line accessor class for the one and only scheduler line

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

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorBase.hpp"

#if !defined(DBMSrvShM_LineAccessorScheduler_HPP)
#define DBMSrvShM_LineAccessorScheduler_HPP

/*! @brief accessor for lines of type scheduler */
class DBMSrvShM_LineAccessorScheduler : public DBMSrvShM_LineAccessorBase {
public:
    enum InfoType {
        IT_None,
        IT_JobID,
        IT_SerializedJobForCreation,
        IT_SerializedJobList,
        IT_SerializedJobForUpdate,
        IT_RequestJobList,
        IT_ActivateJob,
        IT_DeactivateJob,
        IT_DeleteJob,
        IT_ErrorWrongUser,
        IT_ErrorJobHasSuccessors,
        IT_ErrorJobNotExist,
        IT_ErrorPredecessorNotExist
    };

    DBMSrvShM_LineAccessorScheduler(DBMSrvMsg_Error& errOut);
    ~DBMSrvShM_LineAccessorScheduler() {}
    bool removeLayout(DBMSrvMsg_Error& errOut); // called from scheduler when it stops...

    bool setSchedulerPid(DBMSrvMsg_Error& errOut);
    bool getSchedulerPid(RTE_OSPid& pid, DBMSrvMsg_Error& errOut);
    bool removeSchedulerPid(DBMSrvMsg_Error& errOut);

    bool doCancel(DBMSrvMsg_Error& errOut);
    bool unCancel(DBMSrvMsg_Error& errOut);
    bool isCancelled(bool& iscanc, DBMSrvMsg_Error& errOut);

    bool clearContent(DBMSrvMsg_Error& errOut);
    bool setRecipientClient(DBMSrvMsg_Error& errOut);
    bool setRecipientScheduler(DBMSrvMsg_Error& errOut);
    bool clientWait(DBMSrvMsg_Error& errOut);
    bool isSchedulerCommand(DBMSrvMsg_Error& errOut);


    bool getInfoType(InfoType& aInfoType, DBMSrvMsg_Error& errOut);
    bool setInfoType(InfoType aInfoType, DBMSrvMsg_Error& errOut);

    bool getUserName(char*& aUserName, DBMSrvMsg_Error& errOut);
    bool setUserName(const char* aUserName, DBMSrvMsg_Error& errOut);

    bool getVariablePart(char*& aVarPart, DBMSrvMsg_Error& errOut);
    bool setVariablePart(const char* aVarPart, DBMSrvMsg_Error& errOut);

    enum Recipient {
        Rec_None,
        Rec_Client,
        Rec_Scheduler
    };

    static const char* getRecipientName( Recipient recipient );

private:

    bool setRecipient( Recipient aRecipient, DBMSrvMsg_Error& errOut);
    
    // returns true if aRecipient is really waiting. If true is returned, that can be taken,
    // if false is returned, check for an error...
    bool recipientWait( Recipient aRecipient, DBMSrvMsg_Error& errOut );
    
    // here the return value means if the current recipient equals aRecipient. If true is returned,
    // that can be taken, if false is returned, check for an error...
    bool recipientCheck( Recipient aRecipient, DBMSrvMsg_Error& errOut );

    static const char* const TEXT_NONE;
    static const char* const TEXT_CLIENT;
    static const char* const TEXT_SCHEDULER;
};

#endif //DBMSrvShM_LineAccessorScheduler_HPP
