/*!---------------------------------------------------------------------
  @file           RTE_PidListForExecutable.cpp
  @author         RobinW
  @brief          DBM: SAPDB Instance and Database Registration and Management Interfaces
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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDBCommon/SAPDB_Names.h"

#include "RunTime/RTE_saveUNIXcalls.h"
#include "SAPDB/RunTime/System/RTESys_CommandPipe.h"
#include "RunTime/System/RTESys_IO.h"

#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

#include "RunTime/RTE_PidListForExecutable.hpp"

#if (defined (AIX))
#include <sys/procfs.h>
#elif (defined(SVR4) || defined(SOLARIS)) && defined(BIT64)
#include <procfs.h>
#endif

#ifdef RTEInternal
using namespace RTEInternal;
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! define */
#define FUSER_BUFFERSIZE_   256
#ifdef _IBMR2
/*! define */
#define THIS_CHARACTER_MUST_FOLLOW_A_PID    'e'
/*! define */
#define OS_SPECIFIC_FUSER_OPTION_ "-fcx"
#else
/*! define */
#define OS_SPECIFIC_FUSER_OPTION_ ""
#endif
/*! define */
#define WAITING_FOR_BLANK_  0
/*! define */
#define WAITING_FOR_PID_    1
/*! define */
#define PARSING_PID_        2

#if (defined (AIX) || defined (SVR4) || defined(SOLARIS))
#define NAME_OF_PROC_FILE_ENTRY_   "psinfo"
#endif
#ifdef LINUX
#define NAME_OF_PROC_FILE_ENTRY_   "cmdline"
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/



RTE_PidListForExecutable::RTE_PidListForExecutable(RTE_Path &executablePath)
         : m_fuserCalled(false)
{
    SAPDB_strncpy_zero_terminated(m_executablePath,executablePath,sizeof(RTE_Path));
}

SAPDB_Bool RTE_PidListForExecutable::IsPidForExecutableAndArgument( RTE_OSPid pid, SAPDB_Char *pArgument )
{
    RTE_Path entryInProcFS;
#if (defined (AIX) || (defined (SVR4) || defined(SOLARIS)) && defined(BIT64))   /* for those who have a proc fs (AIX, solaris and LINUX), read the info from there*/
    struct psinfo contentsOfProcFSEntry;
#else
    SAPDB_Char contentsOfProcFSEntry[sizeof(RTE_Path)+1+sizeof(RTE_DBName)+1];
#endif
    RTE_FileHandle hendl;
    tsp00_VfReturn_Param ReturnStatus;
#if (defined (LINUX) || defined(AIX) || (defined (SVR4) || defined(SOLARIS)) && defined(BIT64))   /* for those who have a proc fs (AIX and LINUX), read the info from there*/
    SAPDB_sprintf(entryInProcFS,sizeof(RTE_Path),"/proc/%d/"NAME_OF_PROC_FILE_ENTRY_,pid);
    RTESys_IOOpen ( hendl,
                    entryInProcFS,
                    RTESys_IOReadOnly,
                    false,
                    0,
                    ReturnStatus );
    if( vf_ok == ReturnStatus )
    {
        memset(&contentsOfProcFSEntry,0,sizeof(contentsOfProcFSEntry));
        for(SAPDB_UInt numberOfAttempts=0;numberOfAttempts<30;numberOfAttempts++)
        {
            tRTESys_IOPosition BytesRead;
            RTESys_IORead(  hendl,
                            &contentsOfProcFSEntry,
                            sizeof(contentsOfProcFSEntry),
                            BytesRead,
                            ReturnStatus );
#if (defined (SVR4) || defined(SOLARIS))
            if ( 0 != contentsOfProcFSEntry.pr_psargs ) // on sun the read() returns vf_ok, but reads only zeroes...
#else
            if ( vf_ok == ReturnStatus )
#endif
            {
                break;  // can read from proc fs entry --> definetely running
            }
            else
            {
                if( ( 0 != kill( pid, 0 ) )  && ( ESRCH  == errno ) )
                {
                    break;  // process  is not alive --> definetely offline
                }
            }
            sleep(1);   // reading the proc-fs failed but process is alive --> wait a second and try again
        }
        RTESys_IOClose(hendl,ReturnStatus);
#if (defined (AIX) || defined (SVR4) || defined(SOLARIS))  /* On AIX the proc fs entry is the binary dump of a psinfo structure */
        SAPDB_Char *exeNameFromProcFS = contentsOfProcFSEntry.pr_psargs;
        SAPDB_Char *dbNameFromProcFS = strchr(contentsOfProcFSEntry.pr_psargs,' ');
        if(dbNameFromProcFS)
        {
            *(dbNameFromProcFS)=0;
            dbNameFromProcFS++;
        }
#else       /* On LINUX the proc fs entry is just a concatenation of zero-terminated strings */
        SAPDB_Char *exeNameFromProcFS = contentsOfProcFSEntry;
        SAPDB_Char *dbNameFromProcFS = contentsOfProcFSEntry+strlen(contentsOfProcFSEntry)+1;
#endif
        if( ( 0 == strcmp(m_executablePath,exeNameFromProcFS)) 
         && dbNameFromProcFS
         && ( 0 == strcmp(pArgument,dbNameFromProcFS)))
        {
           return true;
        }
    }
    else
#endif
#ifdef AIX  // on AIX if the proc fs is not available (changeroot!), fuser won't work either
        return true;
#else
    {   /* if proc fs access did not work or is not available at all, use the fuser executable */
        if( !m_fuserCalled )
        {
            if(!FillPidList(m_executablePath,m_pidList))    // as this is just used as an additional check, return true if technical checks occur!
            {
                return true;
            }
            m_fuserCalled = true;
        }
        return m_pidList.Contains(pid);
    }
#endif
    return false;
}

/*!
  @brief read one line from file
  @param fd [in] file handle
  @param buf [out] buffer
  @param nbyte [in] capacity
 */
static void readOneLine(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte)
{
    SAPDB_Long bytesRead=0;

    while(bytesRead < nbyte-1)  /* the very last character is filled with the terminating zero-byte */
    {
        if(1 != RTE_save_read(fd,(SAPDB_Char *)buf+bytesRead,1))
        {
            break;
        }
        if('\n' == ((SAPDB_Char *)buf)[bytesRead])
        {
            break;
        }
        bytesRead++;
    }
    ((SAPDB_Char *)buf)[bytesRead] = 0;
}

//-----------------------------------------------------------

SAPDB_Bool RTE_PidListForExecutable::FillPidList(const SAPDB_Char *pExecutablePath,RTE_PidList &pidList)
{
    RTE_FuserAccess::fuserState fuserState;
    fuserState = RTE_FuserAccess::GetAccess();
    if( RTE_FuserAccess::noAccess == fuserState )   // fuser cannot be executed
    {
        return false;
    }
    RTE_Path fuserCall;
    SAPDB_Char buf[FUSER_BUFFERSIZE_+1];
    buf[FUSER_BUFFERSIZE_] = 0;

    const SAPDB_Char *pFuserExecutable = NULL;
    if( RTE_FuserAccess::inPath == fuserState )
    {
        pFuserExecutable = "fuser";
    }
    else if( RTE_FuserAccess::inUsrSbin == fuserState )
    {
        pFuserExecutable = "/usr/sbin/fuser";
    }
    else if( RTE_FuserAccess::inSbin == fuserState )
    {
       pFuserExecutable = "/sbin/fuser";
    }
    if( pFuserExecutable )
    {
        SAPDB_sprintf(fuserCall,sizeof(RTE_Path),"%s %s %s"
            ,pFuserExecutable
            ,OS_SPECIFIC_FUSER_OPTION_
            ,pExecutablePath 
            );
        RTE_SystemRc rc;
        SAPDB_Int4 exitStatus;

        RTESys_CommandOutputPipeContext context;
        rc = RTESys_OpenCommandOutputPipe( context, fuserCall );
        if( RTE_SYSTEMRC_NO_ERROR == rc )
        {
            SAPDB_UInt index;
            SAPDB_UInt currentPid=0;
            do
            {
                buf[0]=0;
                readOneLine(context.cmdOutput,buf,FUSER_BUFFERSIZE_);
                if(buf[0])
                {
                    if( strstr(buf,"ermission")||strstr(buf,"denied"))  // these strings indicate technical problems...
                    {
                        return false;
                    }
                    index = 0;
                    SAPDB_Int4 state = WAITING_FOR_BLANK_;
                    while(index < FUSER_BUFFERSIZE_ && '\0' != buf[index])
                    {
                        if(isspace(buf[index])) // skip until the first space is found. The pids come after the first space
                        {
                            if(state == PARSING_PID_)
                            {
#ifndef THIS_CHARACTER_MUST_FOLLOW_A_PID
                                pidList.Insert(currentPid);
#endif
                            }
                            state = WAITING_FOR_PID_;
                            currentPid = 0;
                        }
                        else if(isdigit(buf[index]))
                        {
                            if(WAITING_FOR_PID_ == state)
                            {
                                state = PARSING_PID_;
                            }
                            if(PARSING_PID_ == state)
                            {
                                currentPid *= 10;
                                currentPid += (buf[index]-'0');
                            }
                        }
                        else
                        {
                            if(state == PARSING_PID_)
                            {
#ifdef THIS_CHARACTER_MUST_FOLLOW_A_PID
                                if( THIS_CHARACTER_MUST_FOLLOW_A_PID == buf[index])
#endif
                                {
                                    pidList.Insert(currentPid);
                                }
                                currentPid = 0;
                            }
                            state = WAITING_FOR_BLANK_;
                        }
                        index++;
                    }
                }
            }
            while(buf[0]);
#ifndef THIS_CHARACTER_MUST_FOLLOW_A_PID
            if(0 != currentPid)
            {
                pidList.Insert(currentPid);
            }
#endif
            RTESys_CloseCommandOutputPipe( context, exitStatus);
        }
    }
    return true;
}

//-----------------------------------------------------------

RTE_FuserAccess::fuserState RTE_FuserAccess::GetAccess(void)
{
    if ( notChecked == m_fuserState )
    {
        CheckFuserExistence();
    }
    return m_fuserState;
}

//-----------------------------------------------------------

void RTE_FuserAccess::CheckFuserExistence(void)
{
    // check if a fuser executable is accessible. It may be in the $PATH, it may be accessible
    // via /usr/sbin or not accessible at all. If it is accessible, calling it without arguments
    // produces something like "Usage:....." on stderr.
    m_fuserState = noAccess;
    FILE *p;
    SAPDB_Char buf[FUSER_BUFFERSIZE_];
    buf[0]=0;
    RTE_SystemRc rc;
    SAPDB_Int4 exitStatus;

    RTESys_CommandOutputPipeContext context;
    rc = RTESys_OpenCommandOutputPipe( context, "fuser" );
    if( RTE_SYSTEMRC_NO_ERROR == rc )
    {
        readOneLine(context.cmdOutput,buf,FUSER_BUFFERSIZE_);
        if(strstr(buf,"sage"))  // to match "usage" or "Usage"...
        {
            m_fuserState = inPath;
        }
        RTESys_CloseCommandOutputPipe( context, exitStatus);
    }
    if( m_fuserState != inPath )    // if it was not found in the $PATH, try to call it explicitly in /usr/sbin
    {
        buf[0]=0;
        RTE_SystemRc rc;
        SAPDB_Int4 exitStatus;

        RTESys_CommandOutputPipeContext context;
        rc = RTESys_OpenCommandOutputPipe( context, "/usr/sbin/fuser" );
        if( RTE_SYSTEMRC_NO_ERROR == rc )
        {
            readOneLine(context.cmdOutput,buf,FUSER_BUFFERSIZE_);
            if(strstr(buf,"sage"))
            {
                m_fuserState = inUsrSbin;
            }
            RTESys_CloseCommandOutputPipe( context, exitStatus);
        }
    }
    if( m_fuserState == noAccess )    // if it was not found in the $PATH, try to call it explicitly in /sbin
    {
        buf[0]=0;
        RTE_SystemRc rc;
        SAPDB_Int4 exitStatus;

        RTESys_CommandOutputPipeContext context;
        rc = RTESys_OpenCommandOutputPipe( context, "/sbin/fuser" );
        if( RTE_SYSTEMRC_NO_ERROR == rc )
        {
            readOneLine(context.cmdOutput,buf,FUSER_BUFFERSIZE_);
            if(strstr(buf,"sage"))
            {
                m_fuserState = inSbin;
            }
            RTESys_CloseCommandOutputPipe( context, exitStatus);
        }
    }
}

RTE_FuserAccess::fuserState RTE_FuserAccess::m_fuserState = notChecked;

//-----------------------------------------------------------



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
