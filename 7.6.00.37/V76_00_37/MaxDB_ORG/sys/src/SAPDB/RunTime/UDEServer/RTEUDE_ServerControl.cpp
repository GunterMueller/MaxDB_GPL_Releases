/*!========================================================================

  @file         RTEUDE_ServerControl.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        User Defined Externsion (UDE) Server handling

                Class implements the kernel UDE server handling
                like starting, stoping, create a connection
                to the server and so on.

  @since        2003-11-06  17:05
  @sa           

  ==========================================================================

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "RunTime/RTE_Types.hpp"
#include    "RunTime/UDEServer/RTEUDE_ServerControl.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32)
#  define DEBUGGER                       "devenv /debugexe "
#endif /* _WIN32 */


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

#if defined(_WIN32) && defined (DEBUGGER)

   static bool IsProgrammRunningInDebugger();

   typedef WINBASEAPI BOOL (WINAPI tIsDbgPresent)( VOID );

#endif /* _WIN32 */


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTEUDE_ServerControl::ConnRet
    RTEUDE_ServerControl::Start ( RTETask_ITask            &task,
                                  SAPDB_Char * const       pExecutable,
                                  SAPDBErr_MessageList     &messageList )
{
    ConnRet        retRC = NoError;

    RTETask_Task* pTask = static_cast<RTETask_Task*>(&task);

    SAPDB_Char const * pSessionURIString = pTask->ClientConnections().SessionURIString();

#   if defined(_WIN32)
     STARTUPINFO             StartupInfo;
     SAPDB_Char *            pExtension;
     SAPDB_Char *            pDebugger   = "";

     GetStartupInfo (&StartupInfo);
     memset ( &m_ProcInfo, 0, sizeof(m_ProcInfo) );

     if ( 0 == _tcsicmp ( pExecutable + _tcslen(pExecutable) - 4, ".exe") )
         pExtension = "";
     else
         pExtension = ".exe";

#    if defined DEBUGGER
      if ( IsProgrammRunningInDebugger() )
      {
          SAPDB_Char  debuggerEnv[512];

          SleepEx(1000, true);

          if ( MessageBox( HWND_DESKTOP, "Open debugger for client proccess?",
              (char *)"Debugger",
              MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ) == IDYES )
          {
              DWORD Len = GetEnvironmentVariable("_DEBUGGER", debuggerEnv, sizeof(debuggerEnv));

              if ((Len < (sizeof(debuggerEnv) - 1)) && ( Len != 0 ))
              {
                  debuggerEnv[Len]     = ' ';
                  debuggerEnv[Len + 1] = '\0';
                  pDebugger = debuggerEnv;
              }
              else
                  pDebugger = DEBUGGER;
          }
      }
#    endif

     SAPDB_Char* pCommand = 
         reinterpret_cast<SAPDB_Char*> (alloca( strlen (pDebugger)    +
                                                strlen (pExecutable)  +
                                                strlen (pExtension)   +
                                                strlen (pSessionURIString) + 2 ));
     strcpy ( pCommand, pDebugger );
     strcat ( pCommand, pExecutable );
     strcat ( pCommand, pExtension );
     strcat ( pCommand, " " );
     strcat ( pCommand, pSessionURIString );

     StartupInfo.lpTitle = pCommand;
     StartupInfo.dwFlags = 0;

     if ( CreateProcess ( 0, pCommand, 0, 0, TRUE, CREATE_NO_WINDOW,
                          0, 0, &StartupInfo, &m_ProcInfo) == false )
     {
         retRC = Error;
     }
     else
         m_ClientStarted = true;

#   else /* UNIX */

     int                      argCount = 0;

     char const	*             arguments[3];
     arguments [argCount++]   = pExecutable;
     arguments [argCount++]   = pSessionURIString;
     arguments [argCount++]   = NULL;

     m_SonPID = fork ();

     if ( 0 > m_SonPID )
         retRC = Error;
     else if ( 0 == m_SonPID )
         execv ( pExecutable, (char**)arguments );
     else
         m_ClientStarted = true;
#   endif

    return retRC;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEUDE_ServerControl::isAlive () const
{
    SAPDB_Bool   alive = false;

    if ( m_ClientStarted )
    {
#       if defined(_WIN32)
         if( WaitForSingleObject( m_ProcInfo.hProcess, 0) != WAIT_TIMEOUT )
             alive = true;
#       else
         if ( kill ( m_SonPID, 0 ) == 0 )
             alive = true;
#       endif
    }

    return alive;
}

/*---------------------------------------------------------------------------*/

RTETask_ITaskClientConnection * const
    RTEUDE_ServerControl::AcceptClientConnect( RTETask_ITask &task )
{
    RTETask_Task* pTask = static_cast<RTETask_Task*>(&task);

    return pTask->AcceptClientConnect();
}

/*---------------------------------------------------------------------------*/

RTEUDE_ServerControl::~RTEUDE_ServerControl()
{
    if ( m_ClientStarted )
    {
#   if defined(_WIN32)
         CloseHandle ( m_ProcInfo.hProcess );
         CloseHandle ( m_ProcInfo.hThread );
#   endif 
    }

}

/*---------------------------------------------------------------------------*/

#if defined(_WIN32)
# if defined (DEBUGGER)
  static bool IsProgrammRunningInDebugger ()
  {
    bool                  DbgPresent = false;
    tIsDbgPresent *       IsDbgPresent;
    HINSTANCE             Knl32;

    Knl32 = LoadLibrary("KERNEL32.DLL");

    if ( Knl32 != NULL )
    {
      IsDbgPresent  = (tIsDbgPresent *) GetProcAddress(Knl32, "IsDebuggerPresent");

      if ( IsDbgPresent != NULL )
        DbgPresent = (TRUE == IsDbgPresent());

      FreeLibrary( Knl32 );
    }

    return DbgPresent;
  };
# endif
#endif /* _WIN32 */



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/