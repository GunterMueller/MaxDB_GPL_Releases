/*!
  \file    DBMSrvProc_DeamonProcess.cpp
  \author  MarcW
  \brief   deamon process implementation

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


*/

#include <stdio.h>

#include "DBM/Srv/Process/DBMSrvProc_DeamonProcess.hpp"
#include "RunTime/System/RTESys_ProcessState.hpp"
#include "ToolsCommon/Tools_CommandlineToArgumentsConverter.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

bool DBMSrvProc_DeamonProcess::isRunning() const {
    if( m_IsChild )
        return m_BackgroundProcess->IsRunning() > 0;
    else {
        // check the state of the process that was created
        RTESys_ProcessState procState( m_ThePid );
        return procState.State() == RTESys_ProcessState::Alive;
    }
}

void DBMSrvProc_DeamonProcess::getMessagesInto( Msg_List& aMsgList) {
    aMsgList.Overrule(m_MsgList);
}

DBMSrvProc_DeamonProcess::~DBMSrvProc_DeamonProcess() {
    if( m_BackgroundProcess != NULL )
        delete m_BackgroundProcess;
}

void DBMSrvProc_DeamonProcess::init() {
    m_ThePid = m_BackgroundProcess->getPid();
#if !defined (WIN32)
    struct shmid_ds execCtrlShmAdmin;
    while( m_BackgroundProcess->IsRunning() > 0 ) {
        if( 0 > shmctl(m_ExecCtrlShmId, IPC_STAT, &execCtrlShmAdmin) )
            exit((int)EXIT_SHMERR_EXECTRL_CHECK);

        if( execCtrlShmAdmin.shm_nattch == 1 ) {
            // child either did an exec or exit...
            if( m_ExecCtrlShmId != -1 ) {
                if( 0 > shmdt(m_ExecCtrlShmAddr) )
                    exit((int)EXIT_SHMERR_EXECTRL_DETACH);
                if( 0 > shmctl(m_ExecCtrlShmId, IPC_RMID, 0) )
                    exit((int)EXIT_SHMERR_EXECTRL_DESTROY);
            }
            break;
        }
        RTE_ISystem::DoSleep(50);
    }

    if( m_BackgroundProcess->WasStarted() > 0 ) {
        // I could start the real one (maybe it has terminated already..., but that's fine
        // write its pid to shm and exit
        *m_TheSharedPid = m_ThePid;
        if( shmdt((char*)m_TheSharedPid) == -1 )
            exit((int)EXIT_SHMERR_PID_DETACH);
        else 
            exit((int)EXIT_OK);
    }
    else {
        *m_TheSharedPid = m_ThePid = RTE_UNDEF_OSPID;
        if( shmdt((char*)m_TheSharedPid) == -1 )
            exit((int)EXIT_SHMERR_PID_DETACH);
        else
            exit((int)EXIT_ERR);
    }
#endif
    // this code is executed only in windows
    if( !(m_BackgroundProcess->IsRunning() > 0) ) {
        char codeBuf[32];
        sprintf(codeBuf, "%d", (int)m_BackgroundProcess->GiveReturnCode());
        m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCSTARTERROR, Msg_Arg(SDBMSGTAG_DBMSRV_PROCSTARTERROR__DBMSRV_COMP, codeBuf)));


    }
}


DBMSrvProc_DeamonProcess::DBMSrvProc_DeamonProcess(
                             const char        * Cmdline,
                             const tni34_AFile * StdIn,
                             const tni34_AFile * StdOut,
                             const int           AppendStdOut,
                             const tni34_AFile * StdErr,
                             const int           AppendStdErr)
                             : m_IsChild(true),
                               m_BackgroundProcess(NULL),
                               m_PidShmId(-1) {
m_MsgList.ClearMessageList();

#if !defined (WIN32)
    spawnProcess();
#endif
    if( m_IsChild ) {
#if !defined (WIN32)
        initExecControl();
#endif
        // convert command line to array of arguments, respecting unix conversion rules
        Tools_CommandlineToArgumentsConverter converter;
        bool conversionOkay = converter.setTo(Cmdline, Tools_CommandlineToArgumentsConverter::UNIX_STYLE);
        if( conversionOkay ) {
            size_t argNum = converter.getNumberOfArguments();
            const char* const* values = converter.getArgumentArray();
            tni33_ArgumentList argList;
            for( size_t i=0; i<argNum; ++i )
                argList.AddArgument(values[i]);
            m_BackgroundProcess = new tni33_ABackgroundProcess(
                                            argList,
                                            StdIn,
                                            StdOut,
                                            AppendStdOut,
                                            StdErr,
                                            AppendStdErr,
                                            true);

        }
        else {
            m_BackgroundProcess = new tni33_ABackgroundProcess(
                                            Cmdline,
                                            StdIn,
                                            StdOut,
                                            AppendStdOut,
                                            StdErr,
                                            AppendStdErr,
                                            true);
        }
        init();
    }
}

DBMSrvProc_DeamonProcess::DBMSrvProc_DeamonProcess(
                             tni33_ArgumentList & Arguments,
                             const tni34_AFile *  StdIn,
                             const tni34_AFile *  StdOut,
                             const int            AppendStdOut,
                             const tni34_AFile *  StdErr,
                             const int            AppendStdErr)
                             : m_IsChild(true),
                               m_BackgroundProcess(NULL),
                               m_PidShmId(-1) {
m_MsgList.ClearMessageList();

#if !defined (WIN32)
    spawnProcess();
#endif
    if( m_IsChild ) {
#if !defined (WIN32)
        initExecControl();
#endif
        m_BackgroundProcess = new tni33_ABackgroundProcess(
                                        Arguments,
                                        StdIn,
                                        StdOut,
                                        AppendStdOut,
                                        StdErr,
                                        AppendStdErr);
        init();
    }
}

#if !defined (WIN32)
void DBMSrvProc_DeamonProcess::initExecControl() {

    m_ExecCtrlShmId = shmget((key_t) IPC_PRIVATE,
                    sizeof(int),
                    0666 | IPC_CREAT);
    if( 0 > m_ExecCtrlShmId ) {
        exit((int)EXIT_SHMERR_EXECTRL_CREATE);
    }
    m_ExecCtrlShmAddr = (char*)shmat(m_ExecCtrlShmId, 0, 0);
    if( (char* )0 > m_ExecCtrlShmAddr ) {
        exit((int)EXIT_SHMERR_EXECTRL_ATTACH);
    }
}

bool DBMSrvProc_DeamonProcess::createShm() {
    char buf[32];
    // create it
    m_PidShmId = shmget((key_t) IPC_PRIVATE,
                    sizeof(RTE_OSPid),
                    0666 | IPC_CREAT);
    if( m_PidShmId == -1 ) {
        // report error, use errno
        sprintf(buf,"%d", errno);
        m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCSHMCREATEERROR, Msg_Arg(SDBMSGTAG_DBMSRV_PROCSHMCREATEERROR__DBMSRV_COMP, buf)));
        return false;
    }
    // attach
    m_TheSharedPid = (RTE_OSPid*) shmat(m_PidShmId, 0, 0);
    if( m_TheSharedPid == (RTE_OSPid*) -1 ) {
        // report error, use errno
        sprintf(buf,"%d", errno);
        m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCSHMATTERROR, Msg_Arg(SDBMSGTAG_DBMSRV_PROCSHMATTERROR__DBMSRV_COMP, buf)));
        return false;
    }
    return true;
}
                               
void DBMSrvProc_DeamonProcess::spawnProcess() {
    pid_t aPid(0);
    pid_t deadMeat(0);
    int waitState(0);
    char buf[32];
    m_IsChild = false;
    // create a real shared memory segment, need this to tell parent the
    // pid of the grand child
    if( !createShm() )
        return;
    aPid = fork();
    switch(aPid) {
        case -1: {
            // in parent, error, report it
            m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCSPAWNFAILED));
            break;
        }
        case 0:
            // in child, fine
            m_IsChild = true;
            return;
            break;
        default: {
            // in parent
            deadMeat = waitpid(aPid, &waitState, WUNTRACED);
            if( deadMeat == -1 ) {
                // error, report errno
                sprintf(buf, "%d", errno);
                m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCWAITERROR, Msg_Arg(SDBMSGTAG_DBMSRV_PROCWAITERROR__DBMSRV_COMP, buf)));
            }
            else if( deadMeat == aPid ) {
                // fine
                if( WIFEXITED(waitState) != 0 ) {
                    // child exited normally
                    if( WEXITSTATUS(waitState)==(int)EXIT_OK ) {
                        // child told us via exit code, that it could start
                        // the desired background process
                        m_ThePid = *m_TheSharedPid;
                        if( shmdt((char*)m_TheSharedPid) == -1 ) {
                            // error detaching, report errno
                            sprintf(buf, "%d", errno);
                            m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCSHMDETERROR, Msg_Arg(SDBMSGTAG_DBMSRV_PROCSHMDETERROR__DBMSRV_COMP, buf)));
                        }
                        else {
                            if( shmctl(m_PidShmId, IPC_RMID, 0) == -1 ) {
                                // error destroying, report errno
                                sprintf(buf, "%d", errno);
                                m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCSHMDESTERRORT, Msg_Arg(SDBMSGTAG_DBMSRV_PROCSHMDESTERRORT__DBMSRV_COMP, buf)));
                            }
                        }
                    }
                    else {
                        // child told us via exit code, that it could not
                        // start the desired background process
                        char codeBuf[32];
                        sprintf(codeBuf, "%d", WEXITSTATUS(waitState));
                        m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCSTARTERROR, Msg_Arg(SDBMSGTAG_DBMSRV_PROCSTARTERROR__DBMSRV_COMP, codeBuf)));
                    }
                }
                else {
                    // error, child exited abnormally
                    m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCCHLDEXIT));
                }
            }
            else {
                // error for us, but no errno available
                m_MsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PROCWAITERROR, Msg_Arg(SDBMSGTAG_DBMSRV_PROCWAITERROR__DBMSRV_COMP, "")));
            }
            break;
        }
    }
}
#endif
