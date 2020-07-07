/* @lastChanged: "1998-01-28 12:00"

  * @filename:   vta99.cpp
  * @purpose:    "TA_knltrace"
  * @release:    7.1.0.0
  * @see:        ""
  *
  * @Copyright (c) 1998-2005 SAP AG"


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/* ========================================================================== */
/* ==========           REPLACEMENTS BY THE PREPROCESSOR            ========= */
/* ========================================================================== */


#define IsStart_c	true
#define NOT			!


/* ========================================================================== */
/* ==========          DECLARATION OF TYPES AND FUNCTIONS           ========= */
/* ========================================================================== */


#include "hsp77.h"
#include "hta99.h"

#include "gsp00_0.h" // COMPILEMODE_MSP00

#if COMPILEMODE_MSP00 >= SLOW_MSP00	
#include "hta01.h"
#endif



/* ========================================================================== */
/* ==========               DEFINITION OF METHODS                   ========= */
/* ========================================================================== */


tta99_TraceClass::tta99_TraceClass (const char *Param)

{
#if COMPILEMODE_MSP00 >= SLOW_MSP00	
    this->RoutineName = Param;
    if (this->RoutineName != NULL)
    {
        t01cpp_trace (this->RoutineName, IsStart_c);
    }
#endif
}


/*----------------------------------------------------------------------------*/


tta99_TraceClass::~tta99_TraceClass ()

{
#if COMPILEMODE_MSP00 >= SLOW_MSP00	
	if (this->RoutineName != NULL)
		
    {
        t01cpp_trace (this->RoutineName, NOT IsStart_c);
    }
#endif
}

/*----------------------------------------------------------------------------*/

void tta99_TraceClass::trace (
    tgg00_Debug_Enum debug,
    const char *format,
    ...)
{
#if COMPILEMODE_MSP00 >= SLOW_MSP00
    char        buf [500];
    int         resultLen;
    char       *writePos = buf;
    char       *writeEnd;
    va_list     args;
    char       *nlPos;
    int         lineLen;

    if (!t01trace (debug))
    {
        return;
    }
    va_start (args, format);
    resultLen = sp77vsprintf (buf, sizeof (buf), format, args);
    if (resultLen > 0)
    {
        writeEnd = buf + resultLen;
        while (writePos < writeEnd)
        {
            nlPos = (char*) memchr (writePos, '\n', writeEnd - writePos);
            if (nlPos != NULL)
                lineLen = (int) (nlPos - writePos);
            else
                lineLen = (int) (writeEnd - writePos);
            t01bufline (debug, *(tsp00_Line*) writePos, lineLen);
            writePos += lineLen + 1;
        }
    }
    va_end (args);
#endif
}

//---------------------------------------------------------------------------

cta99_BasisErrorTrace::~cta99_BasisErrorTrace ()
{
#if COMPILEMODE_MSP00 >= SLOW_MSP00
    t01basis_error (m_Debug, "ReturnError ", m_BasisError);
#endif
}

//---------------------------------------------------------------------------

cta99_IsOkTrace::~cta99_IsOkTrace ()
{
#if COMPILEMODE_MSP00 >= SLOW_MSP00
    t01bool (m_Debug, "ReturnIsOk  ", m_IsOk);
#endif
}

