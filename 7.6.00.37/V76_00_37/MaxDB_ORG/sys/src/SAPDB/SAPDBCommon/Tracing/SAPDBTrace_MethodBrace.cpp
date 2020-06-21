/*!*********************************************************************

  module:       SAPDBTrace_MethodBrace.cpp

  ----------------------------------------------------------------------

  responsible:  DanielD

  special area: SAPDB Tracing

  description:  classes to trace method entry and exit

  use #define SAPDB_STACKCHK to enable STACKCHK output PTS 1110953

  ----------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG




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




  *********************************************************************/

#include "SAPDBCommon/Tracing/SAPDBTrace_MethodBrace.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#ifdef SAPDB_STACKCHK
#include "RunTime/RTE_CurrentStack.hpp"
#endif /* SAPDB_STACKCHK */

static const char * tagOpenC = "{";
static const char * endtagOpenC = "{/";
static const char * tagCloseC = "}";
static const char * endLine = "\n";

SAPDBTrace_MethodBrace::SAPDBTrace_MethodBrace (
    const char * nameP,
    SAPDBTrace_Topic & traceTopicP,
    int traceLevel)
{
#ifdef SAPDB_STACKCHK
    SAPDB_UInt4 currentStackUsage = RTE_CurrentStackUsage();
#endif /* SAPDB_STACKCHK */

    if (traceTopicP.TracesLevel (traceLevel)) {

        this->traceTopic = &traceTopicP;
        this->methodName = nameP;
        SAPDBTrace_Stream stream (&traceTopicP.GetTracePrinter ());
        stream  << tagOpenC << nameP << tagCloseC;
#ifdef SAPDB_STACKCHK
        stream << " stack: " << currentStackUsage;
#endif /* SAPDB_STACKCHK */
        stream  << endLine;

    }
    else {
        this->traceTopic = NULL;
        this->methodName = NULL;
    }
}

/*----------------------------------------*/

SAPDBTrace_MethodBrace::SAPDBTrace_MethodBrace (
    const char * nameP,
    const void * thisPtr,
    SAPDBTrace_Topic & traceTopicP,
    int traceLevel)
{
#ifdef SAPDB_STACKCHK
    SAPDB_UInt4 currentStackUsage = RTE_CurrentStackUsage();
#endif /* SAPDB_STACKCHK */

    if (traceTopicP.TracesLevel (traceLevel)) {

        this->traceTopic = &traceTopicP;
        this->methodName = nameP;
        SAPDBTrace_Stream stream (&traceTopicP.GetTracePrinter ());
        stream << tagOpenC << nameP << " @ " << ToStr(thisPtr) << tagCloseC;
#ifdef SAPDB_STACKCHK
        stream << " stack: " << currentStackUsage;
#endif /* SAPDB_STACKCHK */
        stream  << endLine;

    }
    else {
        this->traceTopic = NULL;
        this->methodName = NULL;
    }
}

/*----------------------------------------*/

SAPDBTrace_MethodBrace::~SAPDBTrace_MethodBrace ()
{
    if (this->traceTopic != NULL) {
        SAPDBTrace_Stream stream (&this->traceTopic->GetTracePrinter ());
        stream << endtagOpenC << this->methodName << tagCloseC << endLine;
    }
}

/*----------------------------------------*/

