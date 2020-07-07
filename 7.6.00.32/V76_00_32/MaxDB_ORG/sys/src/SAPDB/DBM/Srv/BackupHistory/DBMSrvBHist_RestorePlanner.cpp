/*!
  \file           DBMSrvBHist_RestorePlanner.cpp
  \author         TiloH
  \ingroup        backup history handling by the DBMServer
  \brief          implements a class for determining a plan for a recovery
                  from a backup history

\if EMIT_LICENCE

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



\endif
*/


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlanner.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "hcn20.h"
#include "hcn30.h"
#include "hcn35b.h"
#include "hcn31.h"
#include "hcn40.h"
#include "hcn90.h"


//-----------------------------------------------------------------------------
// member of class DBMSrvBHist_RestorePlanner
//-----------------------------------------------------------------------------

DBMSrvBHist_RestorePlanner::DBMSrvBHist_RestorePlanner()
{
    LastBackupType[0]='\0';
}

tcn00_Error DBMSrvBHist_RestorePlanner::AutomaticRecovery(VControlDataT * vcontrol,
                                                          CommandT      * command,
                                                          char          * replyData,
                                                          int           * replyLen,
                                                          int             replyLenMax)
{
    FUNCTION_DBG_MCN00_1("DBMSrvBHist_RestorePlanner::AutomaticRecovery");

    tcn00_Error rc=OK_CN00;
    int         LogOnly=0;
    char        szParam[255];
    char        UntilDate[255];
    char        UntilTime[255];
    char        IBID[255];
    char        Key[255];
    char        Label[255];
    int         nParam=1;

    DBMSrvBHist_RestorePlanner Planner;
    DBMSrvBHist_History        History;

    teo00_Uint8 UsedLogPage=DBMSrvBHist_History_NOLOG,
                FirstLogPage=DBMSrvBHist_History_NOLOG;
    int         Restartable=0;

    UntilDate[0]='\0';//SAPDB_strcpy(UntilDate, "99991231"); //init to maximal until date
    UntilTime[0]='\0';//SAPDB_strcpy(UntilTime, "235959");   //init to maximal hour
    IBID[0]='\0';

    //analyse command parameters ([LOG], [UNTIL <date> <time>], [IBID <int_backup_ID>])
    while(OK_CN00==rc && cn90GetToken(command->args, szParam, nParam, 255))
    {
        if(0==cn90Stricmp(szParam, "LOG"))
            LogOnly=1;
        else
            if(0==cn90Stricmp(szParam, BA_KEY_UNTIL_CN003))
            {
                // until time is only valid for log restore
                if(cn90GetToken(command->args, UntilDate, nParam+1, 255) &&
                   cn90GetToken(command->args, UntilTime, nParam+2, 255)   )
                {
                    nParam+=2;
                }
                else
                    rc=cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
            }
            else
                if(0==cn90Stricmp(szParam, BA_KEY_IBID_CN003))
                {
                    if(cn90GetToken(command->args, IBID, nParam+1, 255))
                        ++nParam;
                    else
                        rc=cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
                }
                else
                    if(0==cn90Stricmp(szParam, BA_KEY_KEY_CN003))
                    {
                        if(cn90GetToken(command->args, Key, nParam+1, 255) &&
                           cn90GetToken(command->args, szParam, nParam+2,255) &&
                           0==cn90Stricmp(szParam, BA_KEY_LABEL_CN003) &&
                           cn90GetToken(command->args, Label, nParam+3, 255)      )
                        {
                            SAPDB_strcpy(IBID, Key);
                            strcat(IBID, "|");
                            strcat(IBID, Label);

                            nParam+=3;
                        }
                        else
                            rc=cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
                    }

        ++nParam;
    }

    if(OK_CN00==rc && '\0'==IBID[0])    //without an IBID we have to find out the complete strategy with the help of the available restart info
        rc=DetermineRestartInfo(vcontrol, replyData, replyLen, replyLenMax, UsedLogPage, FirstLogPage, Restartable);

    if(OK_CN00==rc)
    {
        if(Restartable &&                               //is restartable and
           DBMSrvBHist_History_NOLOG!=FirstLogPage &&   //log is available in the log volume and 
           ('\0'==UntilDate[0] || '\0'==UntilTime[0]))  //no until time was requested  -> just restart
        {
            rc=DBOnline(vcontrol, replyData, replyLen, replyLenMax, UntilDate, UntilTime);
        }
        else
        {
            rc=History.Open(vcontrol, 1, replyData, replyLen, replyLenMax);
            
            if(OK_CN00==rc)
            {
                const DBMSrvBHist_RestorePlan * Plan=Planner.DeterminePlanFor(History, UntilDate, UntilTime); //determine the history graph

                if(0!=Plan)
                {
                    //determine the run directory
                    tcn002_XpValueString      Value;
                    tsp00_DbNamec   DBName;
                    char          * TheRunDirectory=0;

                    DBName.rawAssign((char *)vcontrol->dbname);

                    DBMSrvMsg_Error  oParamGetMsg;
                    if (!cn20XParamGetValue(DBName, PAN_RUNDIR, Value, oParamGetMsg))
                        rc=cn90AnswerEvent(replyData, replyLen, replyLenMax, oParamGetMsg);

                    if(OK_CN00==rc && !cn36_StrAllocCpy(TheRunDirectory, (const char *)Value))
                        rc=cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);

                    //determine the restore strategy
                    if(OK_CN00==rc)
                    {
                        rc=Planner.FindRestoreStrategy(FirstLogPage, UsedLogPage, LogOnly, TheRunDirectory, IBID, Restartable, ('\0'!=UntilDate[0] && '\0'!=UntilTime[0]));

                        if(OK_CN00!=rc)
                            cn90AnswerIError(replyData, replyLen, rc);
                    }

                    //restore according to the choosen strategy
                    if(OK_CN00==rc)
                    {
                        const DBMSrvBHist_RestorePlanNode * CurrentBackup=Plan->GetFirstNeededBackup();
                        int                                 BackupRC=0;

                        while(OK_CN00==rc && 0!=CurrentBackup)
                        {
                            rc=Planner.RestoreABackup(CurrentBackup,
                                                      0,
                                                      UntilDate,
                                                      UntilTime,
                                                      vcontrol,
                                                      replyData,
                                                      replyLen,
                                                      replyLenMax,
                                                      BackupRC);

                            if(OK_CN00==rc)
                            {
                                if(CurrentBackup->getBackup()->IsALog() && 0==BackupRC) //if the db kernel answered with return code 0 after a log restore
                                    CurrentBackup=0;                                    //finish the recovery, as it was already finished by the db-kernel
                                else
                                    CurrentBackup=CurrentBackup->NextNeededBackup();    //in all other cases, just restore the next backup
                            }
                        }
                    }

                    //check if the database is in state WARM or if it can be restarted
                    if(OK_CN00==rc)
                    {
                        DBMSrvKnl_State::StateValue DBState;
  
                        DBState=DBMSrvKnl_State::State(vcontrol->dbname);
                        
                        if(DBMSrvKnl_State::StateAdmin==DBState)
                        {
                            rc=DetermineRestartInfo(vcontrol, replyData, replyLen, replyLenMax, UsedLogPage, FirstLogPage, Restartable);

                            if(OK_CN00==rc && Restartable)
                                rc=DBOnline(vcontrol, replyData, replyLen, replyLenMax, UntilDate, UntilTime);
                        }
                    }

                    cn36_StrDealloc(TheRunDirectory);   //last chance
                }
                else
                    rc=cn90AnswerIError(replyData, replyLen, ERR_CN00); //TODO more specific error messages
            }
        }
    }

    return rc;
}

const DBMSrvBHist_RestorePlan * DBMSrvBHist_RestorePlanner::DeterminePlanFor(DBMSrvBHist_History & history,
                                                                             const char          * UntilDate,
                                                                             const char          * UntilTime)
{
    const DBMSrvBHist_RestorePlan * rc=&RestorePlan;

    const Tools_List<DBMSrvBHist_Backup *>                   & historyList=history.GetHistoryList();
    Tools_List<DBMSrvBHist_Backup *>::const_reverse_iterator   currentHistoryEntry=historyList.rbegin();

    Tools_List<DBMSrvBHist_RestorePlanNode *>::reverse_iterator lastLogBackup=RestorePlan.rend();

    Tools_List<DBMSrvBHist_RestorePlanNode *> unmatchedDataPagesBackups;        //data or pages backups not matched to a log backup so far
    Tools_List<DBMSrvBHist_RestorePlanNode *> unmatchedLogBackups;              //log backups that can still match new data/pages backups
    Tools_List<DBMSrvBHist_RestorePlanNode *> unmatchedDataRestores;            //data restores not matched with a data backup so far
    Tools_List<DBMSrvBHist_RestorePlanNode *> SuccessfulBackupsAndDataRestores; //successfull backups and data restores

    int UntilPointAlreadyReached=0;

    for(; 0!=rc && currentHistoryEntry!=historyList.rend(); ++currentHistoryEntry) //walk backwards through the history
    {
        if((*currentHistoryEntry)->WasSuccessfulForDB())
        {
            if((*currentHistoryEntry)->IsABackup())
            {
                if('\0'==UntilDate[0] ||                                                //no until date was set -> use all backups
                   '\0'==UntilTime[0] ||                                                //no until time was set -> use all backups
                   ((*currentHistoryEntry)->IsALog() && UntilPointAlreadyReached) ||    //use all logs after found first matching commit in a log
                   (*currentHistoryEntry)->HasContentOlderThan(UntilDate, UntilTime)) //use all backups that are old enough
                {
                    //create a new node of the plan and add it to the list of all nodes 
                    DBMSrvBHist_RestorePlanNode * NewRestorePlanNode=new DBMSrvBHist_RestorePlanNode(*currentHistoryEntry);

                    if(0==NewRestorePlanNode)
                        rc=0;
                    else
                        if(!RestorePlan.push_back(NewRestorePlanNode) ||
                           !SuccessfulBackupsAndDataRestores.push_back(NewRestorePlanNode))
                        {
                            rc=0;
                        }

                    if(rc)
                    {
                        if((*currentHistoryEntry)->IsALogBackup())
                        {
                            Tools_List<DBMSrvBHist_RestorePlanNode *>::reverse_iterator NewerLogBackup=lastLogBackup;
                            int                                                         FoundFirstNonmatchingLog=0;

                            if(!UntilPointAlreadyReached)   //no log backup matched the until point before
                                UntilPointAlreadyReached=1; //the current one is matching -> we just found the first matching of the history

                            while(RestorePlan.rend()!=NewerLogBackup && !FoundFirstNonmatchingLog)                                  //we have passed newer log backups before and
                            {
                                if((*NewerLogBackup)->getBackup()->IsALogBackup())                                                  //we are looking onto a log backup again
                                {
                                    if((*currentHistoryEntry)->GiveStopLogPage()+1==(*NewerLogBackup)->getBackup()->GiveStartLogPage()) //that log backup is following the current history entry immediately
                                    {
                                        if(!linkTogether(NewRestorePlanNode, *NewerLogBackup))                                          //->they can be restored one after another
                                            rc=0;
                                    }
                                    else
                                        FoundFirstNonmatchingLog=1;
                                }

                                ++NewerLogBackup;
                            }

                            lastLogBackup=RestorePlan.rbegin(); //the current log backup is the latest log backup

                            if(rc)
                            {
                                //check if we have a so far unmatched data/pages backup, that is matching the log backup
                                int                                                 FoundMatch=0;
                                Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator unmatched=unmatchedDataPagesBackups.begin();

                                for(; unmatched!=unmatchedDataPagesBackups.end(); ++unmatched)
                                {
                                    const DBMSrvBHist_Backup * unmatchedBackup=(*unmatched)->getBackup();

                                    if((*currentHistoryEntry)->GiveStartLogPage()<=unmatchedBackup->GiveStartLogPage() && //TODO check +-1 for data backups
                                    (*currentHistoryEntry)->GiveStopLogPage()>=unmatchedBackup->GiveStartLogPage()    )
                                    {
                                        linkTogether(*unmatched, NewRestorePlanNode);   //unmatched is indeed matching (can preceed) the current log backup

                                        FoundMatch=1;   //does not finish loop, as more than one data/pages backup can match this log backup
                                    }
                                }

                                if(FoundMatch) //found all matches
                                {
                                    unmatchedLogBackups.clear();       //newer log backups can not match anymore (log backups are created strictly sequentially)
                                    unmatchedDataPagesBackups.clear(); //newer data/pages backups can not match anymore (data/pages backups -"-)
                                }

                                unmatchedLogBackups.push_front(NewRestorePlanNode); //the curent logbackup is unresolved as there might be more matching data/pages backups in any way
                            }
                        }
                        else
                        {
                            if((*currentHistoryEntry)->IsAData())
                            {
                                int currentIsValid=1;
                                Tools_List<DBMSrvBHist_RestorePlanNode *>::reverse_iterator x=SuccessfulBackupsAndDataRestores.rbegin();

                                //link all matching pages backups to the current data backup
                                for(++x; rc && currentIsValid && x!=SuccessfulBackupsAndDataRestores.rend(); ++x) //first ++x for not testing against NewRestorePlanNode
                                {
                                    if((*x)->getBackup()->IsAData() ||      //another data backup or restore makes current one invalid
                                       (*x)->WasFirstAfterHistlost()   )    //same for the first backup after a histlost
                                    {
                                        currentIsValid=0;
                                    }
                                    else
                                        if((*x)->getBackup()->IsAPagesBackup())
                                        {
                                            if(!linkTogether(NewRestorePlanNode, *x))
                                                rc=0;
                                        }
                                }

                                //link all pages backups matching through a restore to the current data backup
                                x=unmatchedDataRestores.rbegin();
                                
                                while(rc && x!=unmatchedDataRestores.rend())
                                {
                                    if((*x)->getBackup()->HasSameLabelAndTimeStampsAs(*(NewRestorePlanNode->getBackup())))
                                    {
                                        if(!takeoverLinks(NewRestorePlanNode, *x))
                                            rc=0;
                                        else
                                            x=unmatchedDataRestores.erase(x);
                                    }
                                    else
                                        ++x;
                                }
                            }
                            else
                                if((*currentHistoryEntry)->IsAPages())
                                {
                                    //nothing to do in 7.4 but in 7.3 pages backups must be linked with each other!
                                }

                            Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator unmatchedLog=unmatchedLogBackups.begin();
                            int                                                 FoundMatch=0;

                            //check if we have a log backup, that is matching the current data/pages backup
                            for(; !FoundMatch && unmatchedLog!=unmatchedLogBackups.end(); ++unmatchedLog)
                            {
                                const DBMSrvBHist_Backup * unmatchedLogBackup=(*unmatchedLog)->getBackup();

                                if(unmatchedLogBackup->GiveStartLogPage()<=(*currentHistoryEntry)->GiveStartLogPage() && //TODO check +-1 for data backups
                                   unmatchedLogBackup->GiveStopLogPage()>=(*currentHistoryEntry)->GiveStartLogPage()    )
                                {
                                    linkTogether(NewRestorePlanNode, *unmatchedLog);    //unmatchedLog is indeed matching (can follow) the current data/pages backup

                                    FoundMatch=1;

                                    unmatchedLogBackups.erase(++unmatchedLog, unmatchedLogBackups.end()); //newer log backups can not match anymore (log backups are created strictly sequentially)
                                    unmatchedDataPagesBackups.clear();                                    //newer data/pages backups can not match anymore (data/pages backups -"-)
                                }
                            }

                            if(!FoundMatch)
                                unmatchedDataPagesBackups.push_front(NewRestorePlanNode);
                        }
                    }
                }
            }
            else
                if((*currentHistoryEntry)->IsAHistLost())
                {
                    unmatchedLogBackups.clear();
                    unmatchedDataPagesBackups.clear();

                    lastLogBackup=RestorePlan.rend();

                    if(!RestorePlan.empty())
                        RestorePlan.back()->SetAsFirstAfterHistlost();
                }
                else
                    if((*currentHistoryEntry)->IsARestore())
                    {
                        //ignore restores? NO!!!!
                        if((*currentHistoryEntry)->IsAData())
                        {
                            //create a new node of the plan and add it to the list of all nodes 
                            DBMSrvBHist_RestorePlanNode * NewUnmatchedRestore=new DBMSrvBHist_RestorePlanNode(*currentHistoryEntry);

                            if(0==NewUnmatchedRestore)
                                rc=0;
                            else
                                if(!unmatchedDataRestores.push_front(NewUnmatchedRestore) ||
                                   !SuccessfulBackupsAndDataRestores.push_back(NewUnmatchedRestore))
                                {
                                    rc=0;
                                }

                            if(rc)
                            {
                                //add all pages backups until found new data backup or Histlost
                                int currentIsValid=1;
                                Tools_List<DBMSrvBHist_RestorePlanNode *>::reverse_iterator x=SuccessfulBackupsAndDataRestores.rbegin();

                                //link all newer matching pages backups to the current data backup
                                for(++x; rc && currentIsValid && x!=SuccessfulBackupsAndDataRestores.rend(); ++x)
                                {
                                    if((*x)->getBackup()->IsAData() ||      //another data backup or restore makes current one invalid
                                       (*x)->WasFirstAfterHistlost()   )    //same for the first backup after a histlost
                                    {
                                        currentIsValid=0;
                                    }
                                    else
                                        if((*x)->getBackup()->IsAPagesBackup())
                                        {
                                            if(!NewUnmatchedRestore->addFollowingBackup(*x))
                                                rc=0;
                                        }
                                }
                            }
                        }
                    }
        }
    }

    //last chance to free the ListNodes of restores
    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator h=SuccessfulBackupsAndDataRestores.begin();

    for(;h!=SuccessfulBackupsAndDataRestores.end(); ++h)
    {
        if((*h)->getBackup()->IsARestore())
            delete (*h);
    }

    return rc;
}

tcn00_Error DBMSrvBHist_RestorePlanner::RestoreABackup(const DBMSrvBHist_RestorePlanNode * Backup,
                                                       int                                 OnlyCheck,
                                                       const char                        * UntilDate,
                                                       const char                        * UntilTime,
                                                       VControlDataT                     * vcontrol,
                                                       char                              * replyData,
                                                       int                               * replyLen,
                                                       int                                 replyLenMax,
                                                       int                               & BackupResult)
{
    tcn00_Error rc=OK_CN00;

    char * EBID=Backup->getBackup()->ConstructEBIDOfFirstAvailableCopy();

    if(0==EBID)
        rc=cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
    else
    {
        tcn003_Media        Media;
        int                 MediaUsedByBackupSession=0;
        tcn003_MediaGroup * MediaGroup=0;
        tcn003_Medium     * Medium=0;

        //construct medium or mediagroup
        rc=Backup->getBackup()->ConstructMediumOfFirstAvailableCopy(Medium, MediaGroup);

        if(OK_CN00==rc)
        {
            //construct media structure that is containing the medium or the mediumgroup
            cn30InitMedia(&Media);

            if(0!=Medium) //add the medium with help of an artificial group
            {
                rc=cn30AddMedium(replyData, replyLen, &Media, Medium);

                if(OK_CN00!=rc)
                {
                    if(0!=Medium)
                        cn00MyFree(Medium);
                }
            }
            else    //simply add the group
            {
                Media.pFirstGroup=MediaGroup;
                Media.pLastGroup=MediaGroup;
            }
        }

        if(OK_CN00==rc)
        {
            if(0!=cn36_StrUprCmp(Backup->getBackup()->GiveBackupType(), LastBackupType))
            {
                SAPDB_strcpy(LastBackupType, Backup->getBackup()->GiveBackupType());

                rc=cn31BackupRestoreRequest(vcontrol,
                                            replyData,
                                            replyLen,
                                            replyLenMax,
                                            Backup->getBackup()->GiveMediumName(),
                                            Backup->getBackup()->GiveBackupType(),
                                            EBID,
                                            MediaGroup,
                                            Medium,
                                            &Media,
                                            MediaUsedByBackupSession,
                                            UntilDate,
                                            UntilTime,
                                            Backup->getBackup()->GiveLabel());
            }
            else
            {
                if(OK_CN00==rc)
                {
                    rc=cn31BackupRestoreReplaceRequest(vcontrol,
                                                       replyData,
                                                       replyLen,
                                                       replyLenMax,
                                                       Backup->getBackup()->GiveMediumName(), //TODO necessary????
                                                       EBID,
                                                       Medium,
                                                       MediaGroup,
                                                       &Media,
                                                       MediaUsedByBackupSession);
                }
            }
        }

        //delete members of Media (last chance)
        if(!MediaUsedByBackupSession)
            cn30FreeMedia(&Media);

        cn36_StrDealloc(EBID);
    }
                               
    if(OK_CN00==rc)
    {
        CommandT               DummyCommand;
        tcn003_BackupSession * pBackupSession=0;
        DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

        rc=cn31BackupReplyReceive(vcontrol,
                                  &DummyCommand,
                                  replyData,
                                  replyLen,
                                  replyLenMax);

        rc=cn31BackupSession(vcontrol, reply, &pBackupSession, ForRestore_cn31);

        //do explicit autoignore, if needed
        if(OK_CN00==rc &&
           (0==Backup->NextNeededBackup() ||                                                                                        //if this is the last backup
            0!=cn36_StrUprCmp(Backup->getBackup()->GiveBackupType(), Backup->NextNeededBackup()->getBackup()->GiveBackupType())))   //the next backup has not the same type (e.g. LOG after Pages, but not LOG after LOG)
        {
            while(OK_CN00==rc &&
                  BA_NEXTMEDIUM_RC_CN003==pBackupSession->tBackupResult.nBackupRC) //we got a -8020 from the kernel
            {
                if(0!=pBackupSession->pBackupTool)    //If we used a backup tool, forget it (so we make sure that cn31BackupRestoreIgnore(...) works as advertised)
                {
                    delete pBackupSession->pBackupTool;
                    pBackupSession->pBackupTool=0;
                }

                rc=cn31BackupRestoreIgnore(vcontrol, &DummyCommand, replyData, replyLen, replyLenMax);
            }
        }

        BackupResult=pBackupSession->tBackupResult.nBackupRC;
    }

    return rc;
}

int DBMSrvBHist_RestorePlanner::linkTogether(DBMSrvBHist_RestorePlanNode *previous, DBMSrvBHist_RestorePlanNode * following)
{
    int rc=1;
    
    if(!previous->addFollowingBackup(following) ||
       !following->addPreviousBackup(previous)    )
    {
        rc=0;
    }

    return rc;
}

int DBMSrvBHist_RestorePlanner::takeoverLinks(DBMSrvBHist_RestorePlanNode *previous, DBMSrvBHist_RestorePlanNode * supplier)
{
    int rc=1;

    const Tools_List<DBMSrvBHist_RestorePlanNode *> & FollowingList=supplier->GiveFollowingBackups();
    Tools_List<DBMSrvBHist_RestorePlanNode *>::const_iterator h=FollowingList.begin();

    for(;rc && h!=FollowingList.end(); ++h)
    {
        if(!linkTogether(previous, *h))
            rc=0;
    }

    return rc;
}

tcn00_Error DBMSrvBHist_RestorePlanner::FindRestoreStrategy(const teo00_Uint8   FirstLogPage,
                                                            const teo00_Uint8   UsedLogPage,
                                                            const int           LogOnly,
                                                            const char        * DefaultDirectory,
                                                            const char        * FirstInternalBackupID,
                                                            int                 IsRestartable,
                                                            int                 IsForPointInTime)
{
    tcn00_Error                                         rc=OK_CN00;
    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator Backup=RestorePlan.begin();
    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator FromIBID=RestorePlan.begin();
    int                                                 Found=0;
    DBMSrvBHist_RestorePlanNode *                       LastBackup=0;

    int EmptyStrategy=0;

    //init history graph
    RestorePlan.SetFirstNeededBackup(0);  //reset the starting point of the restore strategy

    for(; Backup!=RestorePlan.end(); ++Backup)  //reset all nodes
        (*Backup)->InitNode();

    //find the last suitable backup defined by internal backup ID or by the db's restart info
    if('\0'==FirstInternalBackupID[0])  //if we have no explicit starting backup ...,
    {                                   //...find the newest backup matching the restart info instead
        if(DBMSrvBHist_History_NOLOG==FirstLogPage ||    //need to restore some log
           FirstLogPage>UsedLogPage                   )
        {
            //find last needed backup
            Backup=RestorePlan.begin();

            if(DBMSrvBHist_History_NOLOG!=FirstLogPage) //we have information on the log volume and need a log backup as end, try to find a matching one
            {
                while(!Found && Backup!=RestorePlan.end())
                {
                    if((*Backup)->getBackup()->IsALog() &&
                        (*Backup)->getBackup()->GiveStartLogPage()<=FirstLogPage &&
                        (*Backup)->getBackup()->GiveStopLogPage()>=FirstLogPage    )
                    {
                            Found=1;
                    }
                    else
                        ++Backup;
                }
            }
            else    //no information on the log (DBMSrvBHist_History_NOLOG==FirstLogPage!)
            {
                if(LogOnly) //find the latest log backup (kernel checks, if it (it's DB identifier) matches the data volumes)
                {
                    while(!Found && Backup!=RestorePlan.end())
                    {
                        if((*Backup)->getBackup()->IsALog())
                            Found=1;
                        else
                            ++Backup;
                    }

                    if(!Found && IsRestartable)
                        EmptyStrategy=1; //the only case (clear log volume and restartable) where an empty strategy is allowed
                }
                else //find the backup with the highest log IO sequence number (log or data)
                {
                    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator LastLog=RestorePlan.begin();

                    //look for log backups after the last histlost, that has backups after it
                    while(!Found && LastLog!=RestorePlan.end())
                    {
                        if((*LastLog)->WasFirstAfterHistlost())	//even a log can be first after Histlost (e.g. the corresponding data backup did not match the UNTIL point in time)
                            LastLog=RestorePlan.end();
                        else
                            if((*LastLog)->getBackup()->IsALog())
                                Found=1;
                            else
                                ++LastLog; //did not "pass" a histlost nor found a log backup so far -> just search on
                    }

                    Found=0;

                    while(!Found && Backup!=RestorePlan.end())  //search the latest data or pages backup
                    {
                        if((*Backup)->getBackup()->IsAData() || (*Backup)->getBackup()->IsAPages())
                            Found=1;
                        else
                            ++Backup;
                    }

                    if(Backup==RestorePlan.end() ||                                             //if we have not found a data or pages backup (that will not be funny later on!)
                       (LastLog!=RestorePlan.end() &&                                           //or if we have found both types, were LastLog has a higher IO sequence
                        (*LastLog)->getBackup()->HasHigherLogPageAs(*((*Backup)->getBackup()))))
                    {
                        Backup=LastLog;                                                         //use LastLog as final backup to recover
                    }                                                                           //else just use Backup as final backup
                }
            }

            if(Backup!=RestorePlan.end())
                LastBackup=(*Backup);
            else
                LastBackup=0;
        }
        else
            if(IsForPointInTime)    //but without specified data backup to start from
            {
                //find the first available data backup (only backups older than the until point are in RestorePlan
                FromIBID=RestorePlan.begin();
        
                while(OK_CN00==rc && !Found && FromIBID!=RestorePlan.end())
                {
                    if((*FromIBID)->getBackup()->IsADataBackup())
                    {
                        int Available=0;

                        rc=(*FromIBID)->getBackup()->IsAvailable(DefaultDirectory, Available);

                        if(OK_CN00==rc && Available)
                            Found=1;
                    }
                    else
                        ++FromIBID;
                }

                //find the last log backup (reachable from FromIBID)
                if(FromIBID==RestorePlan.end())
                    rc=ERR_NORESTORESTRATEGY_CN00_1;
                else
                {
                    Tools_List<DBMSrvBHist_RestorePlanNode *>   List;
                    DBMSrvBHist_RestorePlanNode               * Current=(*FromIBID);
                    teo00_Uint8                                 MaximalLogPage=Current->getBackup()->GiveMaxLogPage();

                    LastBackup=0;
                    rc=Current->AddUnvisitedFollowingToList(List);

                    while(OK_CN00==rc && !List.empty()) //List will be empty, because graph is acyclic
                    {
                        Current=List.front();

                        if(Current->getBackup()->IsALogBackup() &&
                            MaximalLogPage<Current->getBackup()->GiveMaxLogPage())   //do we have a new maximum?
                        {
                            MaximalLogPage=Current->getBackup()->GiveMaxLogPage();
                            LastBackup=Current;
                        }

                        Current->AddUnvisitedFollowingToList(List);
                        List.pop_front();
                    }
                }
            }
            else                //else: just restart
                rc=ERR_CN00;    //we schould never reach this point, as restarts are handeled from calling function
    }
    else    //an internal backup ID was specified
    {
        //find the backup specified by the internal backup ID
        FromIBID=RestorePlan.begin();
        
        while(!Found && FromIBID!=RestorePlan.end())
        {
            if((*FromIBID)->getBackup()->HasInternalBackupID(FirstInternalBackupID))
                Found=1;
            else
                ++FromIBID;
        }

        //find the backup (reachable from FromIBID) with the highest IO sequence
        if(FromIBID==RestorePlan.end())
            rc=ERR_BACKNOTINHIST_CN00;
        else
        {
            Tools_List<DBMSrvBHist_RestorePlanNode *>   List;
            DBMSrvBHist_RestorePlanNode               * Current=(*FromIBID);
            teo00_Uint8                                 MaximalLogPage=Current->getBackup()->GiveMaxLogPage();

            LastBackup=Current;
            rc=Current->AddUnvisitedFollowingToList(List);

            while(OK_CN00==rc && !List.empty()) //List will be empty, because graph is acyclic
            {
                Current=List.front();

                if(MaximalLogPage<Current->getBackup()->GiveMaxLogPage())   //do we have a new maximum?
                {
                    MaximalLogPage=Current->getBackup()->GiveMaxLogPage();
                    LastBackup=Current;
                }

                Current->AddUnvisitedFollowingToList(List);
                List.pop_front();
            }
        }
    }

    if(!EmptyStrategy) //if db has no cleared log volume or is not restartable
    {
        //we must have a last backup
        if(OK_CN00==rc && 0==LastBackup)  //we need at least one backup
            rc=ERR_NORESTORESTRATEGY_CN00_1;

        //a copy of the last backup must be available
        if(OK_CN00==rc)
        {
            int LastBackupAvailable=0;

            rc=LastBackup->getBackup()->IsAvailable(DefaultDirectory, LastBackupAvailable);

            if(OK_CN00==rc && !LastBackupAvailable)
                rc=ERR_NORESTORESTRATEGY_CN00_1;
        }

        //find a restore strategy using breadth-first search and starting from the last needed backup
        if(OK_CN00==rc)
        {
            DBMSrvBHist_RestorePlanNode * Current=LastBackup;
            unsigned int                  Level=0;

            DBMSrvBHist_RestorePlanNode * Minimal=0;
            int                           MinimalLevelFound=0;

            Tools_List<DBMSrvBHist_RestorePlanNode *> List[2];
            int                                       CurrentList=0;
            int                                       NextList=1;
            
            //mark paths of suitable backup strategies, calculate costs for every strategy and determine an "optimal" strategy
            //use breadth-first search

            rc=Current->AddToNextLevel(Level, List[CurrentList], 0); //0 because the last backup has no following backup

            while(OK_CN00==rc && !List[CurrentList].empty() && !MinimalLevelFound)
            {
                ++Level;

                for(Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator i=List[CurrentList].begin(); OK_CN00==rc && i!=List[CurrentList].end(); ++i)
                {
                    if(!MinimalLevelFound)
                    {
                        if('\0'==FirstInternalBackupID[0]) //no specific first backup was requested by the user
                        {
                            //check for the optimal starting backup
                            if(LogOnly)
                            {
                                if((*i)->getBackup()->IsALogBackup() &&
                                (*i)->getBackup()->GiveStartLogPage()<=UsedLogPage &&
                                (*i)->getBackup()->GiveStopLogPage()>=UsedLogPage     )
                                {
                                    MinimalLevelFound=1;
                                }
                            }
                            else
                            {
                                if((*i)->getBackup()->IsADataBackup())
                                    MinimalLevelFound=1;
                            }
                        }
                        else //a specififc start backup was requested by the user, Minimal must be exactly that backup
                        {
                            if((*FromIBID)==(*i))
                                MinimalLevelFound=1;
                        }

                        if(MinimalLevelFound)
                            Minimal=(*i);
                    }

                    rc=(*i)->AddNonMinimalPreviousToNextLevel(Level, List[NextList], DefaultDirectory);
                }

                List[CurrentList].clear();

                //use List[NextList] as List[CurrentList] in the next loop and recycle emptied List[CurrentList]
                int j=CurrentList;
                CurrentList=NextList;
                NextList=j;
            }

            if(MinimalLevelFound)
                RestorePlan.SetFirstNeededBackup(Minimal);
        }

        if(OK_CN00==rc && 0==RestorePlan.GetFirstNeededBackup())
            rc=ERR_NORESTORESTRATEGY_CN00_1;
    }

    return rc;
}

tcn00_Error DBMSrvBHist_RestorePlanner::DetermineRestartInfo(VControlDataT * vcontrol,
                                                             char          * replyData,
                                                             int           * replyLen,
                                                             int             replyLenMax,
                                                             teo00_Uint8   & UsedLogPage,
                                                             teo00_Uint8   & FirstLogPage,
                                                             int           & Restartable)
{
    FUNCTION_DBG_MCN00_1("DBMSrvBHist_RestorePlanner::DetermineRestartInfo");

    tcn00_Error              rc=OK_CN00;
    tcn003_BackupRestartInfo RestartInfo;

    //determine the restart info of the database kernel
    if(OK_CN00==rc)
    {
        if(DBMSrvKnl_State::StateAdmin!=DBMSrvKnl_State::State(vcontrol->dbname)) //restore is only possible in state ADMIN
            rc=cn90AnswerIError(replyData, replyLen, ERR_NOTADMIN_CN00);
        else
            rc=cn31BackupRestartInfo(vcontrol, replyData, replyLen, replyLenMax, &RestartInfo);
    }

    if(OK_CN00==rc)
    {
        if(!cn36_StrToUInt8(UsedLogPage, RestartInfo.FirstIoSequenceNeeded))
            rc=cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_UTLIOSEQNEED, Msg_Arg(SDBMSGTAG_DBMSRV_UTLIOSEQNEED__DBMSRV_COMP,RestartInfo.FirstIoSequenceNeeded)));

        if(!cn36_StrToUInt8(FirstLogPage, RestartInfo.FirstIoSequenceOnLog))
            rc=cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_UTLIOSEQAVAIL, Msg_Arg(SDBMSGTAG_DBMSRV_UTLIOSEQNEED__DBMSRV_COMP,RestartInfo.FirstIoSequenceOnLog)));

        Restartable=(RestartInfo.Restartable[0]=='1');
    }

    return rc;
}

tcn00_Error DBMSrvBHist_RestorePlanner::DBOnline(VControlDataT * vcontrol,
                                                 char          * replyData,
                                                 int           * replyLen,
                                                 int             replyLenMax,
                                                 const char    * UntilDate,
                                                 const char    * UntilTime)
{
    tcn00_Error rc=OK_CN00;

    char * Arguments=0;

    CommandT DBOnlineCommand;

    if('\0'!=UntilDate[0] && '\0'!=UntilTime[0])
    {
        if(!cn36_StrAlloc(Arguments, strlen("-u ")+strlen(UntilDate)+1+strlen(UntilTime)))
            rc=cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
        else
        {
            SAPDB_strcpy(Arguments, "-u ");
            strcat(Arguments, UntilDate);
            strcat(Arguments, " ");
            strcat(Arguments, UntilTime);
        }
    }
    else
    {
        if(!cn36_StrAllocCpy(Arguments, ""))
            rc=cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
    }

    DBOnlineCommand.nIndex=CommandT::InvalidIndex;
    DBOnlineCommand.setArgs(Arguments);

    if(OK_CN00==rc)
        rc=cn40DBWarm(vcontrol,
                      &DBOnlineCommand,
                      replyData,
                      replyLen,
                      replyLenMax);

    cn36_StrDealloc(Arguments);

    return rc;
}
