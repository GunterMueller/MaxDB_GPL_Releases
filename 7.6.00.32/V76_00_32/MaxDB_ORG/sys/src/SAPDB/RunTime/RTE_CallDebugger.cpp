/*!**************************************************************************

  module       : RTE_CallDebugger.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Debugging
  description : Debugger calling function


  last changed: 2000-07-25  17:47
  see also    : example.html ...
  first created:2000-07-25  16:30

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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

#   FreeBSD portions added by Kai Mosebach, 
#   For more informations see : http://www.komadev.de/sapdb
#

*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_CallDebugger.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define DEFAULT_SAPDB_DEBUG_ENV "SAPDB_Debug_Env"

#define DEBUG_COMMAND_BUFFER    300
#define FILE_CONTENT_BUFFER     8192

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/* Default for file used for dynamic debugging */
static const SAPDB_UTF8 * Default_DebugEnvFilename 
        = (const SAPDB_UTF8 *)DEFAULT_SAPDB_DEBUG_ENV;
static const SAPDB_UTF8 * Current_DebugEnvFilename 
        = (const SAPDB_UTF8 *)0;
static SAPDB_Bool DebugEnabledDefault
        = true;
static SAPDB_Bool *IsDebugEnabled 
        = &DebugEnabledDefault;

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

#if !defined(WIN32)

static SAPDB_Bool GetDebugEnv( const char *Variable,
			char *ResultBuffer,
			const size_t MaxLength );

#endif

static SAPDB_Bool SetDebugProfile( const SAPDB_UTF8 *FileName );


/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*
 * Supplying a 'SAPDB_DEBUG_CMD' will have the effect, that a call to this
 * function leads in executing a command prepared using this environment value
 * as format string for sprintf() with argument current pid (returned by getpid)
 *
 * i.e.: on AIX   SAPDB_DEBUG_CMD ="xterm -e dbx -a %d"
 *       on HP                     "xterm -e dde -ui line -attach %d path_to_exe"
 *       on SUN                    "xterm -e dbx path_to_exe %d"
 *       on SOLARIS                "xterm -e dbx path_to_exe %d"
 *       on DEC                    "xterm -e ladebug -pid %d path_to_exe"
 *                              or "xterm -e dbx path_to_exe %d"
 *       on SNI                    "xterm -e dbx %d"
 *       on LINUX                  "xterm -e gdb path_to_exe %d"
 *       on FREEBSD                "xterm -e gdb path_to_exe %d"
 *       on WIN32 NOT NEEDED (DebugBreak() must be enabled with NT own interfaces)
 * 
 * This function is i.e. called inside of vabort(), sqlDebugBreak()
 */
/* see RTE_CallDebugger.h */
void RTE_CallDebugger()
{
#if defined(WIN32)
  DebugBreak();
#else      

  static char  DebugCall[DEBUG_COMMAND_BUFFER];
  static char  DebugCmd[DEBUG_COMMAND_BUFFER-12];

  /*
   * Check Environment and check length of string found (need space for pid too)!
   */
  if ( GetDebugEnv( "SAPDB_DEBUG_CMD", DebugCmd, DEBUG_COMMAND_BUFFER-12 ) )
  {
    sprintf( DebugCall, DebugCmd, getpid() );
    system(DebugCall);
  }
#endif
}

/*---------------------------------------------------------------------------*/
/* see RTE_CallDebugger.h */
void RTE_SetupDebugger(const SAPDB_UTF8 *FileName, SAPDB_Bool *Enabled)
{
  if (FileName != (const SAPDB_UTF8 *)0) {
    SetDebugProfile(FileName);
  }
  if (Enabled) {
    IsDebugEnabled = Enabled;
  }
  if (!FileName && !Enabled) {
    IsDebugEnabled = &DebugEnabledDefault;
    SetDebugProfile(Default_DebugEnvFilename);
  }
  return;
}

/*---------------------------------------------------------------------------*/
/* see RTE_CallDebugger.h */
void RTE_DebugBreak()
{
  if (IsDebugEnabled && *IsDebugEnabled) {
    RTE_CallDebugger();
  }
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

/*
  Function: SetDebugProfile
  Description: Set new Debug Environment File
  Copies the given Filename into a static variable.

  Arguments: FileName [in] Pointer to the Filename
  Return value: true for success, false else
 */
static SAPDB_Bool SetDebugProfile(const SAPDB_UTF8 *FileName)
{
  size_t CheckLength;
  const SAPDB_UTF8 *newFilename;

  /* Variable must be set */
  if ( FileName == (const SAPDB_UTF8 *)0 )
  {
    return false;
  }

  /* Variable must be non empty string */
  CheckLength = strlen((const SAPDB_Char *)FileName);
  if ( CheckLength == 0 )
  {
    return false;
  }

  /* Default must not be allocated */
  if ( FileName != Default_DebugEnvFilename )
  {
    if ( Current_DebugEnvFilename == (const SAPDB_UTF8 *)0
    || Current_DebugEnvFilename == Default_DebugEnvFilename )
    {
      newFilename = (SAPDB_UTF8 *)RTEMem_Allocator::Instance().Allocate(CheckLength+1);
    }
    else
    {
      if ( strlen((const SAPDB_Char *)Current_DebugEnvFilename) < CheckLength )
      {
        newFilename = (SAPDB_UTF8 *)RTEMem_Allocator::Instance().Allocate(CheckLength+1);
      }
      else
      {
        newFilename = Current_DebugEnvFilename;
      }
    }

    if ( !newFilename ) {
      return false;
    }

    if ( newFilename != Current_DebugEnvFilename
      && Current_DebugEnvFilename != (const SAPDB_UTF8 *)0
      && Current_DebugEnvFilename != Default_DebugEnvFilename )
    {
      RTEMem_Allocator::Instance().Deallocate((void *)Current_DebugEnvFilename);
    }

    strcpy((SAPDB_Char *)newFilename, (SAPDB_Char *)FileName);
  }
  else
  {
    newFilename = Default_DebugEnvFilename;
  }

  Current_DebugEnvFilename = newFilename;

  return true;
}

#if !defined(WIN32)

/*---------------------------------------------------------------------------*/
/*
  Function: GetDebugEnv
  Description: Read Variable from Debug Environment File
  Function gets debug information for specified variable from file
  Arguments: Variable [in] Pointer to the Variable name
             ResultBuffer [out] Place where to store result
             MaxLength [in] Maximum buffer size in bytes
  Return value: false if variable not found, true if ResultBuffer filled
 */
static SAPDB_Bool GetDebugEnv( const char *Variable, char *ResultBuffer, const size_t MaxLength )
{
  static SAPDB_Char FileContent[FILE_CONTENT_BUFFER];
  static size_t CheckIndex;
  static SAPDB_Bool Found;
  static int Fd;
  static SAPDB_Char *VariablePtr;
  static size_t FileSize;
  static size_t CheckLength;

  /* Variable must be set */
  if ( Variable == NULL )
  {
    return false;
  }

  /* Variable must be non empty string */
  CheckLength = strlen((const SAPDB_Char *)Variable);
  if ( CheckLength == 0 )
  {
    return false;
  }

  if ( Current_DebugEnvFilename == (const SAPDB_UTF8 *)0 )
  {
    Current_DebugEnvFilename = Default_DebugEnvFilename;
  }

  /* File must exist */
  Fd = open((SAPDB_Char *)Current_DebugEnvFilename, O_RDONLY);

  if ( Fd < 0 ) {
    return false;
  }

  /* Filesize is strongly limited */
  FileSize = read( Fd, &FileContent[0], FILE_CONTENT_BUFFER);
  
  close(Fd);

  if ( FileSize <= 0 )
  {
    return false;
  }

  if ( FileSize == FILE_CONTENT_BUFFER )
  {
    FileContent[FileSize-1] = 0;
  }

  VariablePtr = FileContent;

  /* Search Variable in Memory buffer. Set Found if Found */
  for ( Found = false; *VariablePtr != '\0' && Found == false; )
  {
        /* Skip Whitespace */
    while ( *VariablePtr == '\t'
         || *VariablePtr == ' '
         || *VariablePtr == '\n' )
         ++VariablePtr;

        /* Skip Comment */
    if ( *VariablePtr == '#' )
    {
      while ( *VariablePtr != '\0'
           && *VariablePtr != '\n' )
          ++VariablePtr;
      continue;
    }

        /* Comparison ignoring case */
    for ( CheckIndex = 0; CheckIndex < CheckLength; CheckIndex++ )
    {
      if ( toupper(Variable[CheckIndex]) != VariablePtr[CheckIndex] )
          break;
    }

    if ( CheckIndex == CheckLength )
    {
      Found = true;
      /* Allow ResultBuffer not to be specified ... */
      if ( ResultBuffer != NULL
        && MaxLength > 0 )
      {
        VariablePtr += CheckIndex;

            /* Skip Whitespace */
        while ( *VariablePtr != '\0'
             && *VariablePtr != '\n'
             && ( *VariablePtr == '\t' || *VariablePtr == ' ' ) )
                ++VariablePtr;

        /* Allow equal sign 
           and do NOT SKIP whitespace after equal sign.
           This allows to define string beginning with white spaces
         */
        if ( *VariablePtr == '=' )
        {
           ++VariablePtr;
        }

        for ( CheckIndex = 0; *VariablePtr != '\n' 
                           && CheckIndex < MaxLength; CheckIndex++ )
        {
          /* Copy DOES NOT ignore case */
          ResultBuffer[CheckIndex] = *VariablePtr++;
        }
        ResultBuffer[CheckIndex] = 0;
      }
    }
    else
    {
      /* Skip this Variable */
      while ( *VariablePtr != '\0'
           && *VariablePtr != '\n' )
          ++VariablePtr;
      continue;
    }
  } 
  
  return Found;  
}

#endif /* !WIN32 */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
