/*!
  @file           veo43.cpp
  @author         JoergM
  @special area   c_eo43xshow
  @brief          class interface to access X_CONS show output
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(UNICODE)
#undef UNICODE
#endif /* UNICODE */
/*
 * Sorry no UNICODE variant possible yet...
 */

#include "heo01.h"
#include "geo43.h"
#include <string.h>
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32)
#include <sys/stat.h>
#if defined(UNICODE)
#define POPEN_CALL _wpopen
#define PCLOSE_CALL _wpclose
#define FGETS_CALL  fgetws
#define STAT_CALL   _wstat
#else
#define POPEN_CALL _popen
#define PCLOSE_CALL _pclose
#define FGETS_CALL  fgets
#define STAT_CALL   _stat
#endif /* UNICODE */
#else
#define POPEN_CALL  popen
#define PCLOSE_CALL pclose
#define FGETS_CALL  fgets
#endif

/* space is needed to separate from database name */
#ifdef _WIN32
  #define X_CONS_EXE         _T("cons.exe")
#else
  #define X_CONS_EXE         _T("cons")
#endif
/* spaces are needed to separate from database name and argument */
#define SHOW_CMD _T(" show ")

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

ceo43_CmdPipe::ceo43_CmdPipe()
:m_CommandPipe_eo43((FILE *)0)
,m_OpenedCommand_eo43((TCHAR *)0) 
{}

void ceo43_CmdPipe::eo43OpenCommand(const TCHAR *command, tsp00_Bool & openOk )
{
    tsp00_Int4 lastExitCode = 0;

    eo43CloseCommand( lastExitCode );

    size_t cmdLen = _tcslen(command);

    if ( cmdLen > 0 )
    {
        m_OpenedCommand_eo43 = new TCHAR[cmdLen+sizeof(TCHAR)];
        SAPDB_MemCopyNoCheck(m_OpenedCommand_eo43, command, cmdLen+sizeof(TCHAR));
#if defined(_WIN32)
        // WIN32 returns a valid pipe even if program does not exist
        TCHAR *programExecutableStart;
        TCHAR *programExecutableEnd;

        programExecutableStart = m_OpenedCommand_eo43;
        while ( memcmp(programExecutableStart, _T("\""), sizeof(TCHAR)) == 0 ) 
        {
            ++programExecutableStart;
            if ( memcmp(programExecutableStart, _T(""), sizeof(TCHAR)) == 0 )
                break;
        } // end while
        programExecutableEnd = programExecutableStart;
        do
        {
            ++programExecutableEnd;
            if ( memcmp(programExecutableEnd, _T("\""), sizeof(TCHAR)) == 0 )
                break;
        } while ( memcmp(programExecutableEnd, _T(""), sizeof(TCHAR)) != 0 );

//        if ( memcmp(programExecutableEnd, _T("\""), sizeof(TCHAR)) == 0 ) /* PTS 1110806 */
//        {
            TCHAR separator;
            // i do not copy the program executable in a different buffer, but reuse 
            // the original
            separator = *programExecutableEnd;
            SAPDB_MemFillNoCheck(programExecutableEnd, 0, sizeof(TCHAR));
            struct _stat statBuf;
            // stat call does not allow quotes
            if ( STAT_CALL(programExecutableStart, &statBuf) == 0
              && (statBuf.st_mode & _S_IEXEC) == _S_IEXEC )
            {
                *programExecutableEnd = separator;
                m_CommandPipe_eo43 = POPEN_CALL(m_OpenedCommand_eo43, _T("r") );
            }
            *programExecutableEnd = separator;
//        }
#else
        m_CommandPipe_eo43 = POPEN_CALL(m_OpenedCommand_eo43, _T("r") );
#endif /* _WIN32 */
    }

    openOk = true;
}

void ceo43_CmdPipe::eo43CloseCommand( tsp00_Int4 & ExitCode )
{
    if ( m_CommandPipe_eo43 )
    {
        tsp00_Int4 returnedSize = 0;
        TCHAR      buffer[202];

        do 
        {
            if ( FGETS_CALL ( buffer, 200, m_CommandPipe_eo43 ) == NULL ) 
            {
                break;
            } 
            else 
            {
                returnedSize = (tsp00_Int4)_tcslen ( buffer );
            } // end if
        } while (returnedSize > 0);

        m_LastExitCode_eo43 = PCLOSE_CALL(m_CommandPipe_eo43);
        m_CommandPipe_eo43  = (FILE *)0;
    }
    ExitCode = m_LastExitCode_eo43;

    if ( m_OpenedCommand_eo43 )
    {
        delete [] m_OpenedCommand_eo43;
        m_OpenedCommand_eo43 = 0;
    }
}

ceo43_CmdPipe::~ceo43_CmdPipe()
{
    tsp00_Int4 lastExitCode = 0;

    eo43CloseCommand( lastExitCode );
}

tsp00_Int4
ceo43_CmdPipe::eo43NextLine(
TCHAR           *buffer,
const tsp00_Int4 sizeInBytes,
bool           & NoNewline,
tsp00_Int4     & ExitCode)
{
    tsp00_Int4 returnedSize = 0;

    if ( 0 == m_CommandPipe_eo43 )
    {
        if ( 0 == m_OpenedCommand_eo43 )
        {
            NoNewline = true;
            ExitCode = 1;
        }
        else
        {
            TCHAR *ptr = buffer;
            tsp00_Int4 len;

            // in case of trouble the caller should get information what command failed
            len = (tsp00_Int4)_tcslen(_T("command '"));
            SAPDB_MemCopyNoCheck(ptr, _T("command '"), len);
            ptr += len;

            SAPDB_MemCopyNoCheck(ptr, m_OpenedCommand_eo43, _tcslen(m_OpenedCommand_eo43));
            ptr += _tcslen(m_OpenedCommand_eo43);

            len = (tsp00_Int4)_tcslen(_T("' cannot start\n"));
            SAPDB_MemCopyNoCheck(ptr, _T("' cannot start\n"), len);

            ptr += len;
            SAPDB_MemFillNoCheck(ptr, 0, sizeof(TCHAR)); /* PTS 1110806 */

            NoNewline = false;
            ExitCode = 0;

            delete [] m_OpenedCommand_eo43;
            m_OpenedCommand_eo43 = 0;

            returnedSize = (tsp00_Int4)_tcslen(buffer);
        }
    }
    else 
    {
        // We have a command and a buffer...
        if ( sizeInBytes > 0 )
        {
            if ( FGETS_CALL ( buffer, sizeInBytes - 2, m_CommandPipe_eo43 ) != NULL )
            {
                returnedSize = (tsp00_Int4)_tcslen ( buffer ); /* point to newline */
                NoNewline = ( memcmp( &buffer[returnedSize - 1], _T("\n"), sizeof(TCHAR)) != 0 );
            }
        }

        // last line read leads to close of command
        if ( returnedSize == 0 )
        {
            eo43CloseCommand( ExitCode );
        }
    }

    // terminate buffer only if allowed
    if ( returnedSize < sizeInBytes )
    {
        SAPDB_MemFillNoCheck(&buffer[returnedSize], 0, sizeof(TCHAR));
    }

    return returnedSize;
}

ceo43_xshow::ceo43_xshow( const TCHAR *showCommand, tsp00_DbNamec dbName, tsp00_Bool & openOk )
{
    size_t binpathLen;
    size_t execLen;
    size_t cmdLen;
    size_t dbnameLen;
    size_t argLen;
    tsp00_Pathc    binPath;
    tsp01_RteError rteError;

    openOk = 0;

    if (!sqlGetDbrootPgmPath(binPath, true, &rteError)) {
      return;
    } // end if

    binpathLen = _tcslen(binPath);
    execLen = _tcslen(X_CONS_EXE);
    dbnameLen  = _tcslen(dbName);
    cmdLen  = _tcslen(SHOW_CMD);

    if ( showCommand )
    {
        argLen = _tcslen(showCommand);
    }

                               // add two ""    add separating space           add zero termination
#if defined(_WIN32)
    m_Command_eo43 = new TCHAR[ binpathLen + 2 + execLen + 1 + dbnameLen + cmdLen + argLen + 1];
#else
    m_Command_eo43 = new TCHAR[ binpathLen + execLen + 1 + dbnameLen + cmdLen + argLen + 1];
#endif
    
    if ( m_Command_eo43 )
    {
        TCHAR *pCmd = m_Command_eo43;
#if defined(_WIN32)
        SAPDB_MemCopyNoCheck(pCmd, _T("\""), sizeof(TCHAR)); // PTS 1110673
        ++pCmd;
#endif
        SAPDB_MemCopyNoCheck(pCmd, (const char *) binPath, binpathLen * sizeof(TCHAR));
        pCmd += binpathLen;
        SAPDB_MemCopyNoCheck(pCmd, X_CONS_EXE, execLen * sizeof(TCHAR));
        pCmd += execLen;
#if defined(_WIN32)
        SAPDB_MemCopyNoCheck(pCmd, _T("\""), sizeof(TCHAR));
        ++pCmd;
#endif
        SAPDB_MemCopyNoCheck(pCmd, _T(" "), sizeof(TCHAR));
        ++pCmd; // separating space

        SAPDB_MemCopyNoCheck(pCmd, (const char *) dbName, dbnameLen * sizeof(TCHAR));
        pCmd += dbnameLen;

        SAPDB_MemCopyNoCheck(pCmd, SHOW_CMD, cmdLen * sizeof(TCHAR));
        pCmd += cmdLen;

        SAPDB_MemCopyNoCheck(pCmd, showCommand, argLen * sizeof(TCHAR) );
        pCmd += argLen;

        SAPDB_MemFillNoCheck(pCmd, 0, sizeof(TCHAR)); // zero termination

        eo43OpenCommand(m_Command_eo43, openOk);
    }
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

#ifdef TEST

#include <stdlib.h>
/*#include <unistd.h>*/
#include <iostream.h>

externC tsp00_Bool 
sqlGetDbrootPath ( tsp00_Pathc  VAR_ARRAY_REF   Path,
                   int                          TerminateWithDelimiter,
                   tsp01_RteError              *RteError ) ;
extern "C" tsp00_Bool sqlGetDbrootPgmPath(tsp00_Pathc &binPath, int terminate, tsp01_RteError *rteError)
{
    SAPDB_MemFillNoCheck(rteError, 0, sizeof(tsp01_RteError));
    strcpy(binPath.asCharp(), "D:/SAPDevelop/V74/develop/u sr/pgm/");
    return true;
}

extern "C" void sp36_BuildSet (
                 unsigned char * data,
                 int             maxVal,
                 int             argc,
                 va_list         argptr)
{
    return;
}


void Usage(char *msg)
{
    if ( msg )
    {
        cerr << msg << endl;
    }
    cerr << "Syntax: show_api dbname xcons_show_cmd" << endl;
    cerr << "Usage: Execute an x_cons show command and display result" << endl;
    cerr << "Options: none" << endl;
    exit(1);
}

int
main(int argc, char *argv[])
{
#define BUFFER_SIZE_IN_BYTES 80
    char buffer[BUFFER_SIZE_IN_BYTES];
    int bufferSize = BUFFER_SIZE_IN_BYTES;

    bool       NoNewline;
    int        lineCount;
    tsp00_Int4 bytes;
    tsp00_DbNamec dbName;

    if ( argc < 3 )
    {
        Usage("Missing command");
    }

    if ( argv[1][0] == '-' )
    {
        switch ( argv[1][1] )
        {
        case '?':
        case 'h':
        case 'H':
            Usage(NULL);
            break;
        default:
            break;
        }
    }

    dbName.rawAssign(argv[1]);

    tsp00_Bool openOk;
    tsp00_Int4 ExitCode;

    ceo43_xshow showCommand(argv[2], dbName, openOk);

    if ( !openOk )
    {
        cerr << "showCommand " << argv[2] << " failed" << endl;
    }

    NoNewline = 0;
    lineCount = 0;

    do
    {
        if ( !NoNewline )
        {
            lineCount++;
            bytes = showCommand.eo43NextLine(buffer, bufferSize, NoNewline, ExitCode);
            if ( bytes > 0 )
            {
                cout << "Line " << lineCount << ": " << buffer;
            }
        }
        else
        {
            bytes = showCommand.eo43NextLine(buffer, bufferSize, NoNewline, ExitCode);
            if ( bytes > 0 )
            {
                cout << buffer;
            }
        }
    } while ( bytes != 0 );

    cout << "x_cons terminated. Exitcode :" << (int)ExitCode << endl;

    return 0;
}
#endif /* TEST */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
