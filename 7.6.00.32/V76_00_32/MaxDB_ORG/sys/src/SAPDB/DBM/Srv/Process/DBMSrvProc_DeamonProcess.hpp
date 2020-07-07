/*!
  \file    DBMSrvProc_DeamonProcess.hpp
  \author  MarcW
  \brief   deamon process declaration

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

#if !defined (WIN32)
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/types.h>
#include<signal.h>
#include<errno.h>
#endif

#include "DBM/Srv/Process/DBMSrvProc_ArgumentList.hpp"
#include "Messages/Msg_List.hpp"

#if !defined(DBMSrvProc_DeamonProcess_hpp)
#define DBMSrvProc_DeamonProcess_hpp

/*!
    @brief background process

    Via this class, a new process can easily be spawned. Communication through
    pipes and files is supported.

    For unix systems, the child process is not spawned directly. Instead, a
    double fork() is used. This way, parent processes don't need to wait for
    their child processes to get rid of the zombies...
    The parent spawns a child, this creates the really desired process and after
    this, it exits (and reports its status to the parent via exit code. The parent
    waits synchonously just for this child. The grand children get the mother of
    all processes (pid 1) as parent, that automatically waits for all processes.

    Todo: move tni33 stuff here.
*/
class DBMSrvProc_DeamonProcess {
public:
    DBMSrvProc_DeamonProcess(const char        * Cmdline,
                             const tni34_AFile * StdIn=0,
                             const tni34_AFile * StdOut=0,
                             const int           AppendStdOut=1,
                             const tni34_AFile * StdErr=0,
                             const int           AppendStdErr=1);

    DBMSrvProc_DeamonProcess(tni33_ArgumentList & Arguments,
                             const tni34_AFile *  StdIn=0,
                             const tni34_AFile *  StdOut=0,
                             const int            AppendStdOut=1,
                             const tni34_AFile *  StdErr=0,
                             const int            AppendStdErr=1);

    ~DBMSrvProc_DeamonProcess();

    void getMessagesInto( Msg_List& aMsgList);
    bool isRunning() const;
    RTE_OSPid getPid() const {return m_ThePid;}

private:
    tni33_ABackgroundProcess* m_BackgroundProcess;
    bool m_IsChild;
    RTE_OSPid m_ThePid;
    Msg_List m_MsgList;
    int m_PidShmId;
#if !defined (WIN32)
    int m_ExecCtrlShmId;
    char* m_ExecCtrlShmAddr;
    void initExecControl();
    RTE_OSPid* m_TheSharedPid;
    void spawnProcess();
    bool createShm();
#endif

    void init();

    enum EXIT_CODE {
        EXIT_OK,
        EXIT_ERR,
        EXIT_SHMERR_PID_DETACH,
        EXIT_SHMERR_EXECTRL_CREATE,
        EXIT_SHMERR_EXECTRL_ATTACH,
        EXIT_SHMERR_EXECTRL_DETACH,
        EXIT_SHMERR_EXECTRL_CHECK,
        EXIT_SHMERR_EXECTRL_DESTROY
    };
};

#endif //DBMSrvProc_DeamonProcess_hpp
