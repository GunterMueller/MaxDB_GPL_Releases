/*!
  @file           veo31.c
  @author         JoergM
  @special area   RTE | Debugging Processes
  @brief          Enables Debugging
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

#include "gsp00.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#undef MOD__
#define MOD__ "veo31.c:"

#define SAPDB_Debug_Env_eo31 "SAPDB_Debug_Env" /* file used for dynamic debugging */

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

static tsp00_Pathc eo31_SAPDB_Debug_Env = SAPDB_Debug_Env_eo31; /* file used for dynamic debugging */
static tsp00_Int4 eo31_DebugEnabledDefault = 1;
static const tsp00_Int4 *eo31_IsDebugEnabled = &eo31_DebugEnabledDefault;
/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static  int  eo31_GetDebugEnv(const char *Variable,
			      char *ResultBuffer,
			      const size_t MaxLength);

static int eo31_SetDebugProfile(const char *FileName);

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
 *       on BSD44                  "xterm -e gdb path_to_exe %d"
 *       on WIN32        only the entry must be made
 *
 
 * This function is i.e. called inside of vabort(), sqlDebugBreak()
 */
#undef MF__
#define MF__ MOD__"eo31CallDebugger"
externC int eo31CallDebugger()
{
#define DEBUG_COMMAND_BUFFER_MXEO31 300

static char  DebugCall[DEBUG_COMMAND_BUFFER_MXEO31];
static char  DebugCmd[DEBUG_COMMAND_BUFFER_MXEO31-12];

    /*
     * Check Environment and check length of string found (need space for pid too)!
     */
    if ( eo31_GetDebugEnv("SAPDB_DEBUG_CMD", DebugCmd, DEBUG_COMMAND_BUFFER_MXEO31-12 ) )
    {
#if WIN32
        DebugBreak();
#else      
        sprintf( DebugCall, DebugCmd, getpid() );
        system(DebugCall);
#endif
        /* stop at least the current thread... Debugger should have taken over then... */
        return 1;
    }
    else
    {
        return 0;
    }
}

static int eo31_SetDebugProfile(const char *FileName)
{
    size_t CheckLength;
    /* Variable must be set */
    if ( FileName == NULL )
    {
        return 0;
    }

    /* Variable must be non empty string */
    CheckLength = strlen(FileName);
    if ( CheckLength == 0 )
    {
        return 0;
    }
    
    if ( CheckLength > sizeof(eo31_SAPDB_Debug_Env)-1 ) {
      return 0;
    }

    strcpy(eo31_SAPDB_Debug_Env, FileName);
    return 1;
}

/*
 * File to allow dynamically specified debug information
 */
#undef MF__
#define MF__ MOD__"eo31_GetDebugEnv"
static int eo31_GetDebugEnv(const char *Variable, char *ResultBuffer, const size_t MaxLength)
{
    size_t CheckIndex;
    int Found;
    FILE *Fd;
#define FILE_CONTENT_MXEO31 8192
    char FileContent[FILE_CONTENT_MXEO31];
    char *VariablePtr;
    int FileSize;
    size_t CheckLength;

    /* Variable must be set */
    if ( Variable == NULL )
    {
        return 0;
    }

    /* Variable must be non empty string */
    CheckLength = strlen(Variable);
    if ( CheckLength == 0 )
    {
        return 0;
    }

    /* File must exist */
    Fd = fopen(eo31_SAPDB_Debug_Env, "r");

    if ( Fd == NULL ) {
      return 0;
    }

    /* Filesize is strongly limited */
    FileSize = (int)fread( FileContent, sizeof(FileContent[0]), FILE_CONTENT_MXEO31, Fd );
    
    if ( FileSize < 0 )
    {
        fclose(Fd);
        return 0;
    }

    if ( FileSize == FILE_CONTENT_MXEO31 )
    {
        FileContent[FileSize-1] = 0;
    }

    VariablePtr = FileContent;

    /* Search Variable in Memory buffer. Set Found if Found */
    for ( Found = 0; *VariablePtr != '\0' && Found == 0; )
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
            Found = 1;
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
    
    fclose(Fd);
    return Found;  
}


#undef MF__
#define MF__ MOD__"sqlSetDebug"
externC void sqlSetDebug(char *FileName, tsp00_Int4 *Enabled)
{
  if (FileName) {
    eo31_SetDebugProfile(FileName);
  }
  if (Enabled) {
    eo31_IsDebugEnabled = Enabled;
  }
  if (!FileName && !Enabled) {
    eo31_IsDebugEnabled = &eo31_DebugEnabledDefault;
    eo31_SetDebugProfile(SAPDB_Debug_Env_eo31);
  }
  return;
}

#undef MF__
#define MF__ MOD__"sqlDebugBreak"
externC void sqlDebugBreak()
{
  if (eo31_IsDebugEnabled && *eo31_IsDebugEnabled == 1) {
    eo31CallDebugger();
  }
}
