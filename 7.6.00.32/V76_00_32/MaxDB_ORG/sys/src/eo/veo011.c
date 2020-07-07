/*!
  @file           veo011.c
  @author         JoergM
  @special area   "Client C++ RTE: Checks"
  @brief          Helper routines for C++ RTE Checks
  @first created  000-06-06  9:33
  @see            http://www.bea.sap-ag.de/Entwicklung/C-Style/commonDeclarations.html"

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

#include <stdio.h>
#if defined (_WIN32)
#include <windows.h>
#endif

#include "gsp00_0.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

static const char* AssertMessages_c [] =
{
    "Assertion failed",
    "Precondition failed",
    "Postcondition failed",
    "Not implemented",
    "Unexpected switch value"
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static void eo01_MessageAbort (char* msg, int msglen);
static void eo01_Abort ();

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*----------------------------------------*/

void
sqlk_assert (                        /* ALLIN */
             int            kind,
             int            lineno,
             const char    *filename,
             const char    *condition)
{
    char buf [300];
    int  bytesWritten = 0;

    bytesWritten += sprintf (buf, "%s: '%s'\n%d: %s\n",
                             AssertMessages_c [kind], condition, lineno,
                             filename);
    eo01_MessageAbort (buf, bytesWritten);
}

/*----------------------------------------*/

teo00_Longint
sqlk_rangeviolation (               /* ALLIN */
                     teo00_Longint     val,
                     teo00_Longint     minVal,
                     teo00_Longint     maxVal,
                     int               lineno,
                     const char       *fileName)
{
    char buf [300];
    int  bytesWritten = 0;

    if (val < minVal)
    {
        bytesWritten = sprintf (buf, "Range violation: value %ld < bound %ld\n",
                                val, minVal);
    }
    else if (val > maxVal)
    {
        bytesWritten = sprintf (buf, "Range violation: value %ld > bound %ld\n",
                                val, maxVal);
    }
    else
    {
        return val;
    }
    bytesWritten += sprintf (buf + bytesWritten, "(%d): %s\n", lineno, fileName);
    eo01_MessageAbort (buf, bytesWritten);
    return val;
}

/*----------------------------------------*/

#if defined (_WIN32)
static void
eo01_MessageAbort (
                  char* msg,
                  int msglen)
{
    HANDLE consoleStdout;
    int    msgboxResult;
    DWORD  numWrite;

    consoleStdout = GetStdHandle (STD_OUTPUT_HANDLE);
    if (consoleStdout == INVALID_HANDLE_VALUE)
    {
        /* GUI */
        strcat (msg, "\nAbort?");
        msgboxResult = MessageBoxEx (NULL, msg, "", MB_YESNO | MB_ICONERROR, 0);
        if (msgboxResult != IDNO)
        {
            eo01_Abort ();
        }
    }
    else
    {
        /* console */
        WriteFile (consoleStdout, msg, msglen, &numWrite, 0);
        eo01_Abort ();
    }
}

#else

/*----------------------------------------*/

static void
   eo01_MessageAbort (
                     char* msg,
                     int msglen)
{
    fwrite (msg, msglen, 1, stderr);
    eo01_Abort ();
}

#endif

/*----------------------------------------*/

static void
   eo01_Abort ()
{
    int *nowhere = NULL;

    *nowhere = 0;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
