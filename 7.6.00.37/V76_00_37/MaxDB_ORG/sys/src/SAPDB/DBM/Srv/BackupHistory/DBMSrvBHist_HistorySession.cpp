/*!
  \file    DBMSrvBHist_HistorySession.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   definition of classes handling the DBMServer
           backup history session and functionality working
           on it

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2006 SAP AG

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

#include "DBM/Srv/BackupHistory/DBMSrvBHist_HistorySession.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "ToolsCommon/CString/ToolsCStr_Str.hpp"

#include "hcn31.h"
#include "hcn42.h"
#include "hcn90.h"


//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const char * DBMSrvBHist_HistorySession_COL_ALL="*";
const char * DBMSrvBHist_HistorySession_KEY_ALL="*";
const char * DBMSrvBHist_HistorySession_LBL_ALL="*";
const char * DBMSrvBHist_HistorySession_ACT_ALL="*";

const char * DBMSrvBHist_HistorySession_RES_ALL="*";
const char * DBMSrvBHist_HistorySession_RES_LST="LAST";
const char * DBMSrvBHist_HistorySession_RES_CNT="CONTINUE";

const char * DBMSrvBHist_HistorySession_UNT_ALL="*";

const char * DBMSrvBHist_HistorySession_LISTPARAM_COLS      ="-C";
const char * DBMSrvBHist_HistorySession_LISTPARAM_KEY       ="-K";
const char * DBMSrvBHist_HistorySession_LISTPARAM_LABEL     ="-L";
const char * DBMSrvBHist_HistorySession_LISTPARAM_ACTION    ="-A";
const char * DBMSrvBHist_HistorySession_LISTPARAM_RESTORE   ="-R";
const char * DBMSrvBHist_HistorySession_LISTPARAM_MEDIA     ="-M";
const char * DBMSrvBHist_HistorySession_LISTPARAM_EBIDS     ="-E";
const char * DBMSrvBHist_HistorySession_LISTPARAM_UNTIL     ="-U";
const char * DBMSrvBHist_HistorySession_LISTPARAM_COMPRESSED="COMPRESSED";
const char * DBMSrvBHist_HistorySession_LISTPARAM_INVERTED  ="-INVERTED";
const char * DBMSrvBHist_HistorySession_LISTPARAM_INVERTED_S="-I";


//-----------------------------------------------------------------------------
// class DBMSrvBHist_HistorySessionListParameter
//-----------------------------------------------------------------------------

//! \brief a helper class for Parameter handling of the backup_history_list command
class DBMSrvBHist_HistorySessionListParameter
{
  public:
    const char *  KeyWord;
    char       ** Value;
    int           ChangeToUpper;
    bool          Found;
};


//-----------------------------------------------------------------------------
// members of class DBMSrvBHist_HistorySession
//-----------------------------------------------------------------------------

DBMSrvBHist_HistorySession::DBMSrvBHist_HistorySession()
{
    FirstBackup=History.GetHistoryList().end();

    CurrentBackup=OutputList.end();
    StartBackup  =OutputList.end();
    EndBackup    =OutputList.end();

    ColumnsAsString=0;
    Key    =0;
    Label  =0;
    Action =0;
    Restore=0;
    Until  =0;

    ForRestore  =false;
    RcOKOnly    =false;
    WithMedia   =false;
    WithEBIDs   =false;
    ListInverted=false;

    UsedLogPage =DBMSrvBHist_History_NOLOG;
    FirstLogPage=DBMSrvBHist_History_NOLOG;
    Restartable =false;
}

DBMSrvBHist_HistorySession::~DBMSrvBHist_HistorySession()
{
    Free();

    cn36_StrDealloc(ColumnsAsString);
    cn36_StrDealloc(Key);
    cn36_StrDealloc(Label);
    cn36_StrDealloc(Action);
    cn36_StrDealloc(Restore);
    cn36_StrDealloc(Until);
}

tcn00_Error DBMSrvBHist_HistorySession::allocateGlobalSession(VControlDataT               * vcontrol,
                                                              DBMSrv_Reply                & Reply,
                                                              DBMSrvBHist_HistorySession *& HistorySession)
{
    tcn00_Error rc=OK_CN00;

    if(0==HistorySession)
        HistorySession=new DBMSrvBHist_HistorySession();

    if(0==HistorySession)
        rc=Reply.startWithError(ERR_MEM_CN00);
    else
        vcontrol->pHistorySession=HistorySession;

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::getGlobalSession(VControlDataT               * vcontrol,
                                                         DBMSrv_Reply                & Reply,
                                                         DBMSrvBHist_HistorySession *& HistorySession)
{
    tcn00_Error rc=OK_CN00;

    HistorySession=vcontrol->pHistorySession;

    // is there no active history session?
    if(0==HistorySession)
        rc=Reply.startWithError(ERR_NOHISTORYSESSION_CN00);

    return rc;
}

void DBMSrvBHist_HistorySession::freeGlobalSession(VControlDataT * vcontrol)
{
    if(0!=vcontrol && 0!=vcontrol->pHistorySession)
    {
        delete vcontrol->pHistorySession;
        vcontrol->pHistorySession=0;
    }
}

tcn00_Error DBMSrvBHist_HistorySession::open(VControlDataT * vcontrol,
                                             CommandT      * command,
                                             DBMSrv_Reply  & Reply)
{
    tcn00_Error rc=OK_CN00;
    char        CurrentParameter[256];
    int         NumberOfParameter=1;
    int         CheckEBIDs=false;

    History.setVControlReplyBufferAndStandardErrorHandler(vcontrol, Reply.giveData(), Reply.giveLength(), Reply.giveMaximalLength());

    // look for switch -e
    CurrentParameter[0]='\0';

    while(cn90GetToken(command->args, CurrentParameter, NumberOfParameter, 256) && !CheckEBIDs)
    {
        if(0==cn90Stricmp(CurrentParameter, DBMSrvBHist_HistorySession_LISTPARAM_EBIDS))  //-e
            CheckEBIDs=true;

        NumberOfParameter++;
    }

    return Open(vcontrol, CheckEBIDs, Reply);
}

tcn00_Error DBMSrvBHist_HistorySession::list(VControlDataT * vcontrol,
                                             CommandT      * command,
                                             DBMSrv_Reply  & Reply)
{
    FUNCTION_DBG_MCN00_1("DBMSrvBHist_HistorySession::List");

    tcn00_Error rc=OK_CN00;

    History.setVControlReplyBufferAndStandardErrorHandler(vcontrol, Reply.giveData(), Reply.giveLength(), Reply.giveMaximalLength());

    rc=InitListParameters(Reply);

    if(OK_CN00==rc)
    {
        char ParamString[256];
        int  NumberOfParameter=1;

        DBMSrvBHist_HistorySessionListParameter ListPara[]=
            {{DBMSrvBHist_HistorySession_LISTPARAM_COLS,       &ColumnsAsString, 1, 0},  // -c <col1,col2,...>
            {DBMSrvBHist_HistorySession_LISTPARAM_KEY,        &Key,             1, 0},  // -k <key>
            {DBMSrvBHist_HistorySession_LISTPARAM_LABEL,      &Label,           1, 0},  // -l <label>
            {DBMSrvBHist_HistorySession_LISTPARAM_ACTION,     &Action,          1, 0},  // -a <action>
            {DBMSrvBHist_HistorySession_LISTPARAM_RESTORE,    &Restore,         1, 0},  // -r <what>
            {DBMSrvBHist_HistorySession_LISTPARAM_MEDIA,      0,                0, 0},  // -m
            {DBMSrvBHist_HistorySession_LISTPARAM_EBIDS,      0,                0, 0},  // -e
            {DBMSrvBHist_HistorySession_LISTPARAM_UNTIL,      &Until,           0, 0},  // -u <when>
            {DBMSrvBHist_HistorySession_LISTPARAM_COMPRESSED, 0,                0, 0},  // compressed
            {DBMSrvBHist_HistorySession_LISTPARAM_INVERTED,   0,                0, 0},  // -inverted == -i
            {DBMSrvBHist_HistorySession_LISTPARAM_INVERTED_S, 0,                0, 0},  // -i == -inverted
            {0,                                               0,                0, 0}};

        ParamString[0]='\0';

        // read the list of all parameters
        while(OK_CN00==rc && cn90GetToken(command->args, ParamString, NumberOfParameter, 256))
        {
            if('\0'!=ParamString[0]) //if there is a parameter...
            {
                int KeyWordFound=0;  //...try to find which keyword it is

                NumberOfParameter++;

                for(int i=0; !KeyWordFound && OK_CN00==rc && 0!=ListPara[i].KeyWord; i++)    //search in list of keywords until found or end of list
                {
                    if(0==cn90Stricmp(ParamString, ListPara[i].KeyWord))    //is the current parameter one of the keywords?
                    {
                        ListPara[i].Found=1;
                        KeyWordFound=1;

                        if(0!=ListPara[i].Value)                                                  //if i-th parameter is one with a value...
                        {
                            char ParamString2[256]="";

                            if(cn90GetToken(command->args, ParamString2, NumberOfParameter, 255)) //read that value
                            {
                                if(ListPara[i].ChangeToUpper)
                                    cn90StrUpperCopy(ParamString2, ParamString2, false);

                                NumberOfParameter++;

                                if(!cn36_StrAllocCpy(*(ListPara[i].Value), ParamString2))
                                    rc=Reply.startWithError(ERR_MEM_CN00);
                            }
                            else
                                rc=Reply.startWithError(ERR_PARAM_CN00);
                        }
                    }
                }

                if(OK_CN00==rc && !KeyWordFound)                    //no error, but parameter is also none of the allowed keyowrd...
                    rc=Reply.startWithError(ERR_PARAM_CN00); //...report a parameter error
            }
        }

        if(ListPara[4].Found) //!< \TODO use enums instead of simple number
        {
            ForRestore=true;
            RcOKOnly=true;
        }

        WithMedia=ListPara[5].Found; //!< \TODO use enums instead of simple number
        WithEBIDs=ListPara[6].Found; //!< \TODO use enums instead of simple number

        if(ListPara[9].Found || ListPara[10].Found) //!< \TODO use enums instead of simple number
            ListInverted=1;
        else
            ListInverted=0;

        // checking parameters (option mix)
        if(OK_CN00==rc)
        {
            if((ForRestore &&                                           //if ForRestore, then Key, Label and Action must be "*"
                (0!=strcmp(Key, DBMSrvBHist_HistorySession_KEY_ALL) ||
                 0!=strcmp(Label, DBMSrvBHist_HistorySession_LBL_ALL) || 
                 0!=strcmp(Action, DBMSrvBHist_HistorySession_ACT_ALL)  )) || 
               (!ForRestore &&                                          // until must only be used for restore
                0!=strcmp(Until, DBMSrvBHist_HistorySession_UNT_ALL))        )
            {
                rc=Reply.startWithError(ERR_PARAM_CN00);
            }
        }

        //translate columns argument into a list of columns objects
        if(OK_CN00==rc && 0!=strcmp(ColumnsAsString, DBMSrvBHist_HistorySession_COL_ALL)) //not just "*"
        {
            const char                 * CurPos=ColumnsAsString;
            DBMSrvBHist_Backup::Column   CurCol;

            Columns.clear(); //forget all previous columns

            while(OK_CN00==rc && 0!=CurPos && '\0'!=(*CurPos))
            {
                const char * Separator=strchr(CurPos, ',');

                if(0==Separator)
                    CurCol=DBMSrvBHist_Backup::ColumnFor(CurPos, strlen(CurPos));
                else
                    CurCol=DBMSrvBHist_Backup::ColumnFor(CurPos, Separator-CurPos);

                if(DBMSrvBHist_Backup::NoColumn!=CurCol && !Columns.push_back(CurCol))
                    rc=Reply.startWithError(ERR_MEM_CN00);

                if(0!=Separator)
                    CurPos=Separator+1;
                else
                    CurPos=0;
            }
        }

        //adjust the width of the output columns
        if(OK_CN00==rc)
        {
            if(ListPara[8].Found)   //if parameter "compress" was set //!< \TODO use enums instead of simple number
            {
                ColumnWidthsBackup.SetColumnWidthsToZero(); //set minimal lengths to 0
                ColumnWidthsMedia.SetColumnWidthsToZero();  //so no additional spaces will be added to the output
            }
            else
                EnlargeColumnWidths(); //scan through the history and adjust column widths to the widest row for each column
        }

        if(OK_CN00==rc && ForRestore)
        {
            // read the restart info
            tcn003_BackupRestartInfo RestartInfo;

            if(DBMSrvKnl_State::StateAdmin!=DBMSrvKnl_State::State(vcontrol->dbname))
                rc=ERR_NOTADMIN_CN00;
            else
                rc=cn31BackupRestartInfo(vcontrol, Reply.giveData(), Reply.giveLength(), Reply.giveMaximalLength(), &RestartInfo);

            if(OK_CN00==rc)
            {
                if(!cn36_StrToUInt8(UsedLogPage, RestartInfo.FirstIoSequenceNeeded))
                    rc=Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_UTLIOSEQNEED, Msg_Arg(SDBMSGTAG_DBMSRV_UTLIOSEQNEED__DBMSRV_COMP,RestartInfo.FirstIoSequenceNeeded)));

                if(!cn36_StrToUInt8(FirstLogPage, RestartInfo.FirstIoSequenceOnLog))
                    rc=Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_UTLIOSEQAVAIL, Msg_Arg(SDBMSGTAG_DBMSRV_UTLIOSEQNEED__DBMSRV_COMP,RestartInfo.FirstIoSequenceOnLog)));

                Restartable=('1'==RestartInfo.Restartable[0]); 
            }
            else
            {
                rc=OK_CN00;
                UsedLogPage =DBMSrvBHist_History_NOLOG;
                FirstLogPage=DBMSrvBHist_History_NOLOG;
                Restartable=false;
            }
        }

        // list the history
        if(OK_CN00==rc)
        {
            FirstBackup=History.GetHistoryList().begin();

            if(ForRestore)
            {
                rc=FindFirstRestoreItem();

                if(OK_CN00!=rc)
                    Reply.startWithError(rc);
            }

            if(OK_CN00==rc)
                rc=CreateOutputList(Reply);

            if(OK_CN00==rc)
                rc=BasicListHistory(Reply);

            if(OK_CN00==rc)
            {
                if(CurrentBackup!=OutputList.end())
                {
                    // prepare the next command
                    vcontrol->szNextCommand.rawAssign("backup_history_listnext");
                    vcontrol->nNextCommandSkip=1;
                }
            }
            else
                CurrentBackup=StartBackup;
        }
    }

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::listNext(VControlDataT * vcontrol,
                                                 CommandT      * command,
                                                 DBMSrv_Reply  & Reply)
{
    tcn00_Error rc=OK_CN00;

    History.setVControlReplyBufferAndStandardErrorHandler(vcontrol, Reply.giveData(), Reply.giveLength(), Reply.giveMaximalLength());

    if(0==ColumnsAsString) //InitListParameters was never called before (after the constructor)
    {
        rc=InitListParameters(Reply);

        if(OK_CN00==rc) //enlarge minimal column widths if necessary
        {
            EnlargeColumnWidths();

            FirstBackup=History.GetHistoryList().begin();

            rc=CreateOutputList(Reply);
        }
    }

    if(CurrentBackup==EndBackup) //if at the end, start from the beginning
        CurrentBackup=StartBackup;

    if(OK_CN00==rc)
    {
        Tools_List<DBMSrvBHist_Backup*>::iterator HistoryEntry=CurrentBackup;

        rc=BasicListHistory(Reply);

        if(OK_CN00==rc)
        {
            if(CurrentBackup!=OutputList.end())
            {
                // prepare the next command 
                vcontrol->szNextCommand.rawAssign("backup_history_listnext");
                vcontrol->nNextCommandSkip = 1;
            }
        }
        else
            CurrentBackup=HistoryEntry;
    }

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::close(VControlDataT * vcontrol,
                                              CommandT      * command,
                                              DBMSrv_Reply  & Reply)
{
    tcn00_Error rc=OK_CN00;

    History.setVControlReplyBufferAndStandardErrorHandler(vcontrol, Reply.giveData(), Reply.giveLength(), Reply.giveMaximalLength());

    Free();

    Reply.startWithOKIfRCIsOk(rc);

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::date(VControlDataT * vcontrol,
                                             CommandT      * command,
                                             DBMSrv_Reply  & Reply)
{
    tcn00_Error rc=OK_CN00;

    bool Exists;
    char Date[64];

    rc=CheckHistoryFile(Exists, Date, vcontrol->dbname, Reply);

    if(OK_CN00==rc)
    {
        if(!Exists)
            rc=Reply.startWithError(ERR_NOHISTORY_CN00);
        else
        {
            Reply.startWithOK();
            Reply.appendLine(Date);
        }
    }

    return rc;
}

void DBMSrvBHist_HistorySession::Free()
{
    History.Free();
    OutputList.clear();
}

tcn00_Error DBMSrvBHist_HistorySession::Open(VControlDataT * vcontrol,
                                             int             CheckEBIDs,
                                             DBMSrv_Reply  & Reply)
{
    tcn00_Error rc=History.Open(vcontrol, CheckEBIDs, Reply.giveData(), Reply.giveLength(), Reply.giveMaximalLength());

    Reply.startWithOKIfRCIsOk(rc);

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::CheckHistoryFile(bool                & Exists,
                                                         char                  Date[64],
                                                         const tsp00_DbNamec & DBName,
                                                         DBMSrv_Reply        & Reply)
{
    tcn00_Error          rc=OK_CN00;

    tsp05_RteFileError   rteFileError;
    tsp00_Pathc          szFile;
    tsp05_RteFileInfo    rteFileInfo;

    char               * szPos=0;

    Exists=false;

    // exists history file ?
    cn42GetFileName(DBName, FGET_BACKHIST_CN42, szFile);
    sqlfinfoc(szFile, &rteFileInfo, &rteFileError);

    //if it does not exist, try to "restore" it from the copy <dbname>.knl
    if(vf_ok==rteFileError.sp5fe_result && !rteFileInfo.sp5fi_exists)
    {
        DBMSrvMsg_Error oError;
        cn42RestoreFile(DBName, FGET_BACKHIST_CN42, oError);
        sqlfinfoc(szFile, &rteFileInfo, &rteFileError);
    }

    //if it exists now
    if(vf_ok==rteFileError.sp5fe_result)
    {
        if(rteFileInfo.sp5fi_exists)
        {
            Exists=true;

            if(64>sizeof(rteFileInfo.sp5fi_date_modified)+sizeof(rteFileInfo.sp5fi_time_modified))
            {
                SAPDB_memcpy(Date, rteFileInfo.sp5fi_date_modified, sizeof(rteFileInfo.sp5fi_date_modified));
                Date+=sizeof(rteFileInfo.sp5fi_date_modified);

                SAPDB_memcpy(Date, rteFileInfo.sp5fi_time_modified, sizeof(rteFileInfo.sp5fi_time_modified));
                Date[sizeof(rteFileInfo.sp5fi_time_modified)]='\0';
            }
            else
                rc=Reply.startWithError(ERR_CN00);
        }
        else
            rc=Reply.startWithError(ERR_NOHISTORY_CN00);
    }
    else
        rc=Reply.startWithMessageList(DBMSrvMsg_RTEError(rteFileError));

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::InitListParameters(DBMSrv_Reply & Reply)
{
    tcn00_Error rc=OK_CN00;

    EndBackup=StartBackup=CurrentBackup=OutputList.end();

    if(!cn36_StrAllocCpy(ColumnsAsString, DBMSrvBHist_HistorySession_COL_ALL) ||
       !cn36_StrAllocCpy(Key,             DBMSrvBHist_HistorySession_KEY_ALL) ||
       !cn36_StrAllocCpy(Label,           DBMSrvBHist_HistorySession_LBL_ALL) ||
       !cn36_StrAllocCpy(Action,          DBMSrvBHist_HistorySession_ACT_ALL) ||
       !cn36_StrAllocCpy(Restore,         DBMSrvBHist_HistorySession_RES_ALL) ||
       !cn36_StrAllocCpy(Until,           DBMSrvBHist_HistorySession_UNT_ALL)    )
    {
        rc=Reply.startWithError(ERR_MEM_CN00);
    }

    ForRestore  =false;
    RcOKOnly    =false;
    WithMedia   =false;
    WithEBIDs   =false;
    ListInverted=false;

    UsedLogPage =DBMSrvBHist_History_NOLOG;
    FirstLogPage=DBMSrvBHist_History_NOLOG;
    Restartable =false;

    Columns.clear();                                                    //forget previous columns settings ...

    for(int i=0; OK_CN00==rc && i<DBMSrvBHist_Backup::NoColumn; ++i)    //... and place all columns in the list
        if(!Columns.push_back(DBMSrvBHist_Backup::Column(i)))
            rc=Reply.startWithError(ERR_MEM_CN00);

    ColumnWidthsBackup.SetColumnWidthsToDefaults();
    ColumnWidthsMedia.SetColumnWidthsToDefaults();

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::FindFirstRestoreItem()
{
    tcn00_Error rc=OK_CN00;

    const Tools_List<DBMSrvBHist_Backup*> & HistoryList=History.GetHistoryList();
    
    Tools_List<DBMSrvBHist_Backup *>::const_reverse_iterator PMatch;

    FirstBackup=HistoryList.end();

    if(0==strcmp(Restore, DBMSrvBHist_HistorySession_RES_ALL))    // list all possible data saves
    {
        // oldest possible data save depends from restart info
        //   - if there is a valid restart info, use only data saves after the last "HISTLOST"
        //     if there is no data save after that last "HISTLOST", use all data saves
        //   - if there is no valid restart info, use all data saves
        if(DBMSrvBHist_History_NOLOG!=FirstLogPage && UsedLogPage>=FirstLogPage)
        {
            // search the last "HISTLOST" (backwards)
            for(PMatch=HistoryList.rbegin(); PMatch!=HistoryList.rend() && !((*PMatch)->IsAHistLost()); ++PMatch);

            // search the first data save after the last histlost (forwards)
            if(PMatch!=HistoryList.rend())
                for(FirstBackup=PMatch; FirstBackup!=HistoryList.end() && !((*FirstBackup)->IsADataBackup()); ++FirstBackup);

            if(PMatch==HistoryList.rend() || FirstBackup==HistoryList.end())    //no "HISTLOST" or no data save after the last "HISTLOST"
                FirstBackup=HistoryList.begin();                                //shit! force usage of all data saves
        }
        else                                  //no valid restart info...
            FirstBackup=HistoryList.begin();  //...force usage of all data saves

        ForRestore=false; // disable restoremanager (this action is only a datasave listing with a specific start position)

        if(!cn36_StrAllocCpy(Label, DBMSrvBHist_Backup::DBMSrvBHist_Backup_LBLDAT) ||
           !cn36_StrAllocCpy(Action, DBMSrvBHist_Backup::DBMSrvBHist_Backup_ACTSAV)  )
        {
            rc=ERR_MEM_CN00;
        }
    }
    else
    {
        if(0==strcmp(Restore, DBMSrvBHist_HistorySession_RES_LST)) // search the last data save (backward)
        {
            for(PMatch=HistoryList.rbegin(); PMatch!=HistoryList.rend(); ++PMatch)
            {
                if((*PMatch)->IsAHistLost())
                    FirstLogPage=DBMSrvBHist_History_NOLOG; // HISTLOST passed -> make restartinfo invalid

                if((*PMatch)->IsADataBackup() &&                                    //we are looking for the newest complete data backup ...
                   (*PMatch)->WasSuccessful() &&                                    //...that was successful (TODO: change to successful and available?)...
                   (0==strcmp(Until, DBMSrvBHist_HistorySession_UNT_ALL) ||        //... and unless Until equals "*" ...
                    0<strncmp(Until, (*PMatch)->GiveTimeStamp(), strlen(Until))))   //... TimeStamp of the data backup must be a point in time before Until
                {
                    break;  //found the successful data backup we were looking for
                }
            }

            // view only save actions
            if(!cn36_StrAllocCpy(Action, DBMSrvBHist_Backup::DBMSrvBHist_Backup_ACTSAV))
                rc=ERR_MEM_CN00;
        }
        else
            if(0==strcmp(Restore, DBMSrvBHist_HistorySession_RES_CNT)) // search for continue restore
            {
                Tools_List<DBMSrvBHist_Backup*>::const_reverse_iterator PRestore;
                Tools_List<DBMSrvBHist_Backup*>::const_reverse_iterator PSave;
                Tools_List<DBMSrvBHist_Backup*>::const_reverse_iterator PTemp;

                if(FirstLogPage>UsedLogPage && !Restartable)    // is the first log higher than the used log?
                {
                    PMatch=HistoryList.rend();
                    PSave=HistoryList.rend();

                    // looking for the last successful restore pages or restore data
                    for(PRestore=HistoryList.rbegin(); PRestore!=HistoryList.rend(); ++PRestore)
                    {
                        if(((*PRestore)->IsADataRestore() || (*PRestore)->IsAPagesRestore()) &&
                           (*PRestore)->WasSuccessful())
                        {
                            break;
                        }
                    }

                    // find the matching save item to the restore
                    if(PRestore!=HistoryList.rend())
                    {
                        for(PSave=PRestore, ++PSave; PSave!=HistoryList.rend(); ++PSave)
                        {
                            if((*PSave)->EqualsInLabelWith(*PRestore) &&
                               (*PSave)->EqualsInDBStamp1With(*PRestore) &&
                               (*PSave)->EqualsInDBStamp2With(*PRestore) &&
                               (*PSave)->WasSuccessful())
                            {
                                break;
                            }
                        }
                    }

                    // look for logs before the save
                    if(PSave!=HistoryList.rend())
                    {
                        for(PTemp=PSave, ++PTemp; PTemp!=HistoryList.rend(); ++PTemp)
                        {
                            if((*PTemp)->IsALogBackup())
                            {
                                if((*PSave)->GiveStartLogPage() < (*PTemp)->GiveStopLogPage())
                                    PMatch=PTemp;
                                else
                                    break;
                            }
                            else
                                if((*PTemp)->IsAHistLost()) // break at HISTLOST
                                    break;
                        }

                        if(PMatch==HistoryList.rend())  //no log is in front of the restored save
                        {
                            PMatch=PSave;
                            --PMatch;        //position to one entry after the save (this is at least the restore PRestore)
                        }
                    }
                }
                else
                    PMatch=HistoryList.rend();
            }
            else
            {   // search a successful data save with the key specified in Restore (search backwards)
                for(PMatch=HistoryList.rbegin(); PMatch!=HistoryList.rend(); ++PMatch)
                {
                    if((*PMatch)->IsAHistLost())
                        FirstLogPage=DBMSrvBHist_History_NOLOG; // HISTLOST passed -> make restartinfo invalid

                    if((*PMatch)->HasKey(Restore) &&    // check key
                       (*PMatch)->WasSuccessful()   )   // use successful backups only
                    {
                        break;
                    }
                }

                if(PMatch!=HistoryList.rend() &&    //we found a match ...
                   !(*PMatch)->IsADataBackup())     //...but it is not a complete data backup
                {
                    rc=ERR_PARAM_CN00;
                }

                if(!cn36_StrAllocCpy(Action, DBMSrvBHist_Backup::DBMSrvBHist_Backup_ACTSAV))    // view only save actions
                    rc=ERR_MEM_CN00;
            }

        if(PMatch!=HistoryList.rend())
            FirstBackup=PMatch;   //otherwise FirstBackup==HistoryList.end() which forces no output in BasicListHistory
    }

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::CreateOutputList(DBMSrv_Reply & Reply)
{
    tcn00_Error rc=OK_CN00;

    const Tools_List<DBMSrvBHist_Backup*> & HistoryList=History.GetHistoryList();

    bool GetLine=false;
    bool FirstDataSavePassed=false;
    bool SecondDataSavePassed=false;

    Tools_List<DBMSrvBHist_Backup *>::const_iterator Log=HistoryList.end();
    Tools_List<DBMSrvBHist_Backup *>::const_iterator Data=HistoryList.end();
    Tools_List<DBMSrvBHist_Backup *>::const_iterator TheData=HistoryList.end();

    Tools_List<DBMSrvBHist_Backup *>::const_iterator Backup=FirstBackup;

    //empty any previous output list and adjust the iterators pointing into it
    OutputList.clear();
    EndBackup=StartBackup=CurrentBackup=OutputList.end();

    if(Backup!=HistoryList.end())
    {
        // prepare continue restore
        if(ForRestore &&
           !(*Backup)->IsAData())
        {
            FirstDataSavePassed = true;
        }

        do
        {
            GetLine=true;

            // check the key (full match)
            if(0!=strcmp(Key, DBMSrvBHist_HistorySession_KEY_ALL) &&   //if a key was specified
               !(*Backup)->HasKey(Key)                                ) //and current backup line has another key
            {
                GetLine=false;  //ignore the backup line, as key doesn't match
            }

            // check the label (pattern match <label>*) (execpt it is a history line)
            if(0!=strcmp(Label, DBMSrvBHist_HistorySession_LBL_ALL) &&
               !(*Backup)->MatchesLabel(Label) &&
               !(*Backup)->IsAHistLost()                               )
            {
                GetLine=false;  // label doesn't match
            }

            // check the action (pattern match <action>*) (execpt it is a history line)
            if(0!=strcmp(Action, DBMSrvBHist_HistorySession_ACT_ALL) &&
               !(*Backup)->MatchesAction(Action) &&
               !(*Backup)->IsAHistLost()                                )
            {
                GetLine=false;// action doesn't match
            }

            // check the until timestamp (pattern match <timestamp>*)
            if(0!=strcmp(Until, DBMSrvBHist_HistorySession_UNT_ALL) &&
               strncmp(Until, (*Backup)->GiveTimeStamp(), strlen(Until))<0)  //Until < (*Backup)->GiveTimeStamp()
            {
                GetLine=false;// timestamp doesn't match (backup created after Until time)
            }

            // check existing log against log save when restore manager is active
            if(ForRestore && 
               (*Backup)->IsALog() &&
               FirstLogPage!=DBMSrvBHist_History_NOLOG &&
               FirstLogPage <= (*Backup)->GiveStartLogPage())
            {
                GetLine=false;
            }

            // skip logsaves during restore manager not according to the first datasave 
            if(ForRestore &&
               TheData!=HistoryList.end() &&    //TheData is set and pointing to the first passed complete data backup
               (*Backup)->IsALog()          )
            {
                if((*TheData)->GiveStartLogPage() > (*Backup)->GiveStopLogPage())
                    GetLine=false;  //current log backup is not needed, as it contains only information from before the data backup

                if(FirstLogPage!=DBMSrvBHist_History_NOLOG &&       //if there is log on the log volume and
                   (*TheData)->GiveStartLogPage() >= FirstLogPage)  //start log page of the data backup is larger than first log page of log volume
                {
                    GetLine=false;  //none of the log backups, including the current one, is needed
                }
            }

            // check rc 
            if(RcOKOnly && 
               !(*Backup)->WasSuccessful())
            {
                GetLine=false;  //rc does not match
            }

            // skip datasaves during restore manager
            if(ForRestore &&                //restore manager
               Data==HistoryList.end() &&   //no complete data backup to take into account
               (*Backup)->IsAData()      )
            {
                if(FirstDataSavePassed)
                {
                    GetLine=false;
                    SecondDataSavePassed=true;
                }
                else
                {
                    Tools_List<DBMSrvBHist_Backup*>::const_reverse_iterator Temp;

                    FirstDataSavePassed=true;
          
                    Data   =Backup;
                    TheData=Backup;

                    // look for logs before
                    for(Temp=Backup, ++Temp; Temp!=HistoryList.rend(); Temp!=HistoryList.rend()?++Temp:Temp)
                    {
                        if((*Temp)->IsALog())
                        {
                            if((*Data)->GiveStartLogPage()<(*Temp)->GiveStopLogPage())
                                Log=Temp;
                            else
                                Temp=HistoryList.rend();    //end the loop because (*Temp) contains only log from "before" (*Data)
                        }
                        else
                            if((*Temp)->IsAHistLost())
                                Temp=HistoryList.rend();    //end the loop at a HISTLOST
                    }

                    if(Log==HistoryList.end())  //if no log is before the data backup...
                        Data=HistoryList.end(); //forget about the data backup
                }
            }

            // reaching the data save while using logs before data save in restore mode
            if(ForRestore &&
               Data!=HistoryList.end() &&
               Log==HistoryList.end() &&
               Data==Backup              )
            {
                GetLine=false;
                Data=HistoryList.end();
            }

            // while using logs before data save in restore mode ignore all saves except logs
            if(ForRestore &&
               Data!=HistoryList.end() &&
               Log==HistoryList.end() &&
               !(*Backup)->IsALog()      )
            {
                GetLine=false;
            }

            // skip pagesaves (<-skiping any pages also restores) during restore manager not according to the first datasave
            if(ForRestore &&
               SecondDataSavePassed &&
               (*Backup)->IsAPages())
            {
                GetLine=false;
            }

            // skip everything, that is not a SAVE during restore manager
            if(ForRestore &&
               !(*Backup)->IsABackup())
            {
                GetLine=false;
            }

            //if the line is needed, add it to the OutputList
            if(GetLine &&
               !OutputList.push_back(*Backup))
            {
                rc=Reply.startWithError(ERR_MEM_CN00);
            }

            if(OK_CN00==rc)
            {
                // in restore mode use logs before data save
                if(ForRestore && Log!=HistoryList.end())
                {
                    Backup=Log;
                    Log=HistoryList.end();
                }
                else
                    Backup++;

                // restore manager aborts at next HISTLOST
                if(ForRestore && Backup!=HistoryList.end() && (*Backup)->IsAHistLost())
                    Backup=HistoryList.end();
            }
        }
        while(OK_CN00==rc && Backup!=HistoryList.end());
    }

    if(OK_CN00==rc)
    {
        if(ListInverted)
        {
            StartBackup=CurrentBackup=OutputList.rbegin();
            EndBackup=OutputList.rend();
        }
        else
        {
            StartBackup=CurrentBackup=OutputList.begin();
            EndBackup=OutputList.end();
        }
    }

    return rc;
}

tcn00_Error DBMSrvBHist_HistorySession::BasicListHistory(DBMSrv_Reply & Reply)
{
    tcn00_Error            rc=OK_CN00;
    DBMSrv_Reply::Position ContinueFlagPosition;
    bool                   Full=false;

    Tools_List<DBMSrvBHist_Backup*>::iterator firstEntry=CurrentBackup;

    Reply.startWithOK();

    ContinueFlagPosition=Reply.giveCurrentEnd();

    Reply.appendString(CONT_FLAG_CONTINUE);

    while(CurrentBackup!=EndBackup && !Full)
    {
        char * EndOfLastCompleteBackupInfo=Reply.giveCurrentEnd();

        //try to write information about (*CurrentBackup) into the reply buffer
        Full=PrintCurrentBackupInto(Reply);

        if(!Full)
        {
            if(ListInverted)
                CurrentBackup--;
            else
                CurrentBackup++;
        }
        else
            Reply.skipAt(EndOfLastCompleteBackupInfo); //ignore everything what was written for the current entry, because not all the information could be written
    }

    if(CurrentBackup==EndBackup)
        Reply.insertString(ContinueFlagPosition, CONT_FLAG_END);
    else
    {
        if(firstEntry==CurrentBackup) //we have not moved forward a single backup :(, some clients could loop endless in this situation
        {
            DBMSrvMsg_Error errNeverSeen;
            char *internalBackupID=(*CurrentBackup)->constructInternalBackupID(errNeverSeen);

            DBMSrvMsg_Error err(SDBMSG_DBMSRV_BHIST_TOOMUCHDATA,
                                Msg_Arg(SDBMSGTAG_DBMSRV_BHIST_TOOMUCHDATA__BACKUP_ID_INTERNAL,
                                     internalBackupID?internalBackupID:"unknown"));

            ToolsCStr_Str::Dealloc(internalBackupID);

            err.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_COMM_PACKTOOSMALL,
                                         Msg_Arg(SDBMSGTAG_DBMSRV_COMM_PACKTOOSMALL__BYTE_SIZE,
                                             Reply.giveMaximalLength())));

            rc=Reply.startWithMessageList(err);
        }
        else
            Reply.insertString(ContinueFlagPosition, CONT_FLAG_CONTINUE);
    }

    return rc;
}

bool DBMSrvBHist_HistorySession::PrintCurrentBackupInto(DBMSrv_Reply & Reply)
{
    bool   Full;
    char * Buffer=Reply.giveCurrentEnd();
    size_t MaxUsableBufferSize=Reply.giveAvailableSpace();

    Full=(*CurrentBackup)->PrintBackupInto(Buffer, MaxUsableBufferSize, Columns, ColumnWidthsBackup);

    if(!Full)
        Full=(*CurrentBackup)->PrintMediaAndEBIDsInto(Buffer, MaxUsableBufferSize, WithMedia, WithEBIDs, ColumnWidthsMedia);

    Reply.adaptLength();

    return Full;
}

void DBMSrvBHist_HistorySession::EnlargeColumnWidths()
{
    const Tools_List<DBMSrvBHist_Backup*> & HistoryList=History.GetHistoryList();

    for(Tools_List<DBMSrvBHist_Backup*>::const_iterator h=HistoryList.begin(); h!=HistoryList.end(); ++h)
        (*h)->EnlargeColumnWidths(Columns, ColumnWidthsBackup, ColumnWidthsMedia);
}
