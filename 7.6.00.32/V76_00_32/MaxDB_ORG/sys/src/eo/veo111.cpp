/*!
  @file           veo111.cpp
  @author         RaymondR
  @special area   
  @brief          
  @see            

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


#include "geo110.h"
#include "geo111.h"
#include "geo200.h"
#include "geo81.h"
#include "geo007_1.h"
#include "gsp00.h"


#if defined(_WIN32)
# include <windows.h>
# include <crtdbg.h>

#else  /* UNIX */

# include <assert.h>
# include <stdlib.h>
# include <unistd.h>

# if defined(_DEBUG)
#  define _ASSERT      assert
# else
#  define _ASSERT(_assertion)
# endif

#endif /* _WIN32/UNIX */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


#define   OPTION_CHARACTER_PIPE_DESC      _T( 'P')
#define   OPTION_STRING_PIPE_DESC         _T("-P")

// --- need by t_EvalChildArgs
//
#define   OPTION_ID_HELP                 1
#define   OPTION_ID_PIPE_DESC            2


#if defined(_WIN32)
#  define DEBUGGER                       "devenv /debugexe "
#endif /* _WIN32 */


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/





/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           class t_EvalChildArgs                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

class t_EvalChildArgs : public teo81_EvalProgArgs
{
public:
  typedef struct
  {
    teo110_PipeDesc::teo110_ASCIIPipeDesc     ASCIIPipeDesc;
  } t_Args;

public:
  t_EvalChildArgs                          ( int                        argc,
                                             TCHAR**                    argv );
  ~t_EvalChildArgs                         () {;};

  bool       ParseArguments                ( t_Args const **            Args )
                                             {
                                               *Args = &m_Args;
                                               return eo80_ParseArguments();
                                             };

private:
  bool       eo80_vNextValidOption         ( const int                  OptionIndex,
                                             const TCHAR*               OptionArgument );
  bool       eo80_vParseArgumentsFinished  ( const _TCHAR* const*       ArgvRemaining,
                                             int                        ArgsRemaining,
                                             int                        ArgsParsed,
                                             bool                       BreakOptionFound );
private:
  t_Args     m_Args;
};


#if defined(_WIN32)
# if defined (DEBUGGER)
   typedef WINBASEAPI BOOL (WINAPI tIsDbgPresent)( VOID );
# endif
#endif /* _WIN32 */

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static teo81_EvalProgArgs::teo81_OptionDesc Options[] =
       {{ OPTION_ID_HELP,       _T('h'),                     _T("help")    ,   true,  true,  _T("[LONG]"),                  _T("help") },
        { OPTION_ID_HELP,       _T('?'),                     _T("?")       ,   true,  true,  _T("[LONG]"),                  _T("help") },
        { OPTION_ID_PIPE_DESC,  OPTION_CHARACTER_PIPE_DESC,  _T("PIPEDESC"),   true,  false, _T("<ASCII Pipe Descriptor>"), _T("ASCII Pipe Descriptor") },
       };


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static bool eo111_IsDebuggerPresent ();


/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class teo111_ChildComm                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

teo111_ChildComm::teo111_ChildComm ( int                  argc,
                                     TCHAR**              argv,
                                     bool                 &Ok )
{
  ROUTINE_DBG_MEO60 (_T("teo111_ChildComm::teo111_ChildComm"));
  t_EvalChildArgs::t_Args const *   Args;
  t_EvalChildArgs                   EvalArgs( argc, argv );

  Ok = EvalArgs.ParseArguments( &Args );

  if ( Ok )
  {
    teo110_PipeDesc      PipeDesc;

    PipeDesc.eo110_ASCIIToPipeDesc( Args->ASCIIPipeDesc );

    eo110_Initialize( PipeDesc );
  }
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class teo111_ParentComm                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

bool teo111_ParentComm::teo111_Connect ( _TCHAR const * const      Executable,
                                         teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo111_ParentComm::teo111_Connect"));

  teo110_PipeDesc                          ChildPipeDesc;
  teo110_PipeDesc::teo110_ASCIIPipeDesc    ASCIIChildPipeDesc;
  bool                                     Ok;

  Ok = eo110_Create ( ChildPipeDesc, true, pEventList );

  if ( Ok == true )
  {
    ChildPipeDesc.eo110_PipeDescToASCII  ( ASCIIChildPipeDesc );

    Ok = eo111_StartClient( Executable, &ASCIIChildPipeDesc, pEventList );


    // --- We are using the communication pipes for IPC,
    //     the parent process has to close the child pipe
    //     ( eo110_ReleaseChildPipeHandles ).
    if ( Ok )
      eo110_ReleaseChildPipeHandles();
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

teo111_ParentComm::~teo111_ParentComm()
{
  ROUTINE_DBG_MEO60 (_T("teo111_ParentComm::~teo111_ParentComm"));

# if defined(_WIN32)

   if ( m_ClientStarted )
   {
     CloseHandle (m_ProcInfo.hProcess);
     CloseHandle (m_ProcInfo.hThread);
   }

# else  /* UNIX */

# endif /* _WIN32/UNIX */

  return;
}

/*---------------------------------------------------------------------------*/

bool teo111_ParentComm::eo111_StartClient ( _TCHAR const * const                           Executable,
                                            teo110_PipeDesc::teo110_ASCIIPipeDesc const *  ASCIIPipeDesc,
                                            teo200_EventList * const                       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo111_ParentComm::eo111_StartClient"));

  teo001_SystemRc                          rc = NO_ERROR_EO001;

# if defined(_WIN32) /*------- _WIN32 -------------------*/

   _TCHAR                  CmdLine[1024];
   STARTUPINFO             StartupInfo;
   BOOL                    Ok;
   _TCHAR *                Extension;
   _TCHAR *                Debugger   = _T("");
   _TCHAR                  DebuggerEnv[256];

   GetStartupInfo (&StartupInfo);
   memset ( &m_ProcInfo, 0, sizeof(m_ProcInfo) );

   if ( NULL == _tcsicmp ( Executable + _tcslen(Executable) - 4, ".exe") )
     Extension = _T("");
   else
     Extension = _T(".exe");

#  if defined DEBUGGER
    if ( eo111_IsDebuggerPresent() )
    {
      SleepEx(1000, true);
      if ( MessageBox( HWND_DESKTOP, "Open debugger for client proccess?",
                       (char *)"Debugger",
                       MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ) == IDYES )
      {
        DWORD Len = GetEnvironmentVariable("_DEBUGGER", DebuggerEnv, sizeof(DebuggerEnv));

        if ((Len < (sizeof(DebuggerEnv) - 1)) && ( Len != 0 ))
        {
          DebuggerEnv[Len]     = ' ';
          DebuggerEnv[Len + 1] = '\0';
          Debugger = DebuggerEnv;
        }
        else
          Debugger = DEBUGGER;
      }
    }
#  endif

   _stprintf ( CmdLine, "%s\"%s%s\" %s %s",
               Debugger,
               Executable,
               Extension,
               OPTION_STRING_PIPE_DESC,
               ASCIIPipeDesc );

   StartupInfo.lpTitle = CmdLine;
   StartupInfo.dwFlags = 0;

   Ok = CreateProcess ( 0, CmdLine, 0, 0, TRUE, CREATE_NO_WINDOW,
                        0, 0, &StartupInfo, &m_ProcInfo);

   if ( false == Ok )
      rc = GetLastError();

# else  /*--------------------- UNIX --------------------*/

   int                      Result;
   char const	*             Arguments[5];
   int                      ArgCount = 0;
   pid_t                    SonPID;

   Arguments [ArgCount++]   = Executable;

   Arguments [ArgCount++]   = OPTION_STRING_PIPE_DESC;
   Arguments [ArgCount++]   = (char*) ASCIIPipeDesc;

   Arguments [ArgCount++]   = NULL;

  // --- check array size
  _ASSERT( (sizeof(Arguments)/sizeof(Arguments[0])) >= ArgCount );

	SonPID = fork ();

	if ( 0 > SonPID )
  {
    rc = errno;
  }
	else if ( 0 == SonPID )
	{
    execv (Executable, (char**)Arguments);
  }

#endif /*----------------- _WIN32/UNIX -----------------*/

  if ( NO_ERROR_EO001 != rc )
  {
    m_ClientStarted = false;

    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_START_PROCCESS, Executable, rc );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
  }
  else
    m_ClientStarted = true;

  return NO_ERROR_EO001 == rc;
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           class t_EvalChildArgs                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

t_EvalChildArgs::t_EvalChildArgs    ( int                        argc,
                                      TCHAR**                    argv )
                : teo81_EvalProgArgs( argc, argv, Options, NUM_OF_OPTIONS_EO80(Options),
                                      OPTION_ID_HELP )
{
  ROUTINE_DBG_MEO60 (_T("t_EvalChildArgs::t_EvalChildArgs"));

  memset ( &m_Args, 0, sizeof(m_Args) );
};

/*---------------------------------------------------------------------------*/

bool t_EvalChildArgs::eo80_vNextValidOption ( const int            OptionIndex,
                                              const TCHAR*         OptionArgument )
{
  ROUTINE_DBG_MEO60 (_T("t_EvalChildArgs::eo80_vNextValidOption"));
  bool Ok = true;

  switch (OptionIndex)
  {
  case OPTION_ID_PIPE_DESC:
      _tcsncpy( m_Args.ASCIIPipeDesc, OptionArgument, sizeof(m_Args.ASCIIPipeDesc) - 1 );
      m_Args.ASCIIPipeDesc[sizeof(m_Args.ASCIIPipeDesc) - 1] = _T('\0');
      break;

  default:
    Ok = false;
    break;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool t_EvalChildArgs::eo80_vParseArgumentsFinished  ( const _TCHAR* const* ArgvRemaining,
                                                      int                  ArgsRemaining,
                                                      int                  ArgsParsed,
                                                      bool                 BreakOptionFound )
{
  bool Ok = true;

  if (( 0        <  ArgsRemaining )         ||
      ( 0        == ArgsParsed )            ||
      ( _T('\0') == m_Args.ASCIIPipeDesc[0] ))
  {
    eo80_vHelp(NULL);
    Ok = false;
  }

  return Ok;
};


/*---------------------------------------------------------------------------*/

#if defined(_WIN32)
# if defined (DEBUGGER)
  static bool eo111_IsDebuggerPresent ()
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