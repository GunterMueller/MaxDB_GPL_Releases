/*!
  @file           veo51.cpp
  @author         Daniel Dittmar
  @special area   RTE | Basic Declarations
  @brief          assertion support routines for kernel
  @see            http://www.bea.sap-ag.de/Entwicklung/C-Style/commonDeclarations.html

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


#include <stdio.h>
#if !(defined SUN || defined SOLARIS)
#include <memory.h>
#endif /* SUN||SOLARIS */

#include "gsp00_0.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

externC void vabort (teo00_Bool     writeCrashDump);

externC void vopmsg (teo00_Int2    prio,
                    teo00_Int2    msgtype,
                    teo00_Int4    npar1,
                    teo00_Int4    npar2,
                    const char  * spar,
                    const char  * msgline);

static void eo51_ShowMessage (char* msg, int msgLen);


static const char* AssertMessages_c [] =
{
    "Assertion failed",
    "Precondition failed",
    "Postcondition failed",
    "Not implemented",
    "Unexpected switch value"
};

/*----------------------------------------*/

externC void
sqlk_assert (                        // ALLIN
             int            kind,
             int            lineno,
             const char    *filename,
             const char    *condition)
{
    ROUTINE_DBG_MSP00 ("sqlk_assert");
    char buf [300];
    int  bytesWritten = 0;

    bytesWritten += sprintf (buf, "%s: '%s'\n%d: %s\n",
                             AssertMessages_c [kind], condition, lineno,
                             filename);
    eo51_ShowMessage (buf, bytesWritten);
    vabort (true);
}

/*----------------------------------------*/

externC teo00_Longint
sqlk_rangeviolation (               // ALLIN
                     teo00_Longint     val,
                     teo00_Longint     minVal,
                     teo00_Longint     maxVal,
                     int               lineno,
                     const char       *fileName)
{
    ROUTINE_DBG_MSP00 ("sqlk_rangeviolation");
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
    eo51_ShowMessage (buf, bytesWritten);
    vabort (true);
    // never reached, but explicit return pleases compiler
    return val;
}

/*----------------------------------------*/

static char blankstr_c [] =
      "                                                                ";
const int   maxPartLen_c = 64;
const char  lineContinuation_c = '\\';

static void
   eo51_ShowMessage (
                     char* msg,
                     int msgLen)
{
    ROUTINE_DBG_MSP00 ("eo51_ShowMessage");
    const char *msgEnd;
    const char *msgPos;
    size_t      partLen;
    const char *nlPos;
    char        buf [maxPartLen_c];

    msgPos = msg;
    msgEnd = msg + msgLen;
    while (msgPos < msgEnd)
    {
        nlPos = (const char*) memchr (msgPos, '\n', msgEnd - msgPos);
        if (nlPos != NULL)
        {
            // write a logical line
            partLen = nlPos - msgPos + 1;
            SAPDB_MemCopyNoCheck(buf, msgPos, partLen);
            buf [partLen - 1] = ' ';    // overwrite nl
        }
        else
        {
            if ((msgEnd - msgPos) <= sizeof (buf))
            {
                // write rest of msg
                partLen = msgEnd - msgPos;
                SAPDB_MemCopyNoCheck (buf, msgPos, partLen);
            }
            else
            {
                // break msg into physical line
                partLen = sizeof (buf) - 1;    // leave room for continuation
                SAPDB_MemCopyNoCheck (buf, msgPos, partLen);
                buf [partLen] = lineContinuation_c;
            }
        }
        if ((partLen < sizeof (buf))
                && (buf [sizeof (buf) - 1] != lineContinuation_c))
        {
            SAPDB_MemFillNoCheck (buf + partLen, ' ', sizeof (buf) - partLen);
        }
        vopmsg (1, 699, 0, 0, blankstr_c, buf);
        msgPos += partLen;
    }
}

