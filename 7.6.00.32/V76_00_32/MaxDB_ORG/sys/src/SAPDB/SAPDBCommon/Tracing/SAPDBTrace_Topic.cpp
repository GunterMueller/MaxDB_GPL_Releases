/*!*********************************************************************

  module:       SAPDBTrace_Topic.cpp

  ----------------------------------------------------------------------

  responsible:  DanielD

  special area: SAPDB Tracing

  description:  allows to switch groups of trace statements

  ----------------------------------------------------------------------

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




  *********************************************************************/

#include <string.h>
#include <ctype.h>

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"

enum WellKnownLevel {MinTrace = 0, StdTrace = 5, MaxTrace = 10};

class SAPDBTrace_NullPrinter : public SAPDBTrace_LinePrinter
{
public:
    /* function: PrintLn
       description: writes a line to the output medium
     */
    virtual void PrintLn (const SAPDB_Char *, int) {}
};

static SAPDBTrace_NullPrinter nullPrinter;

/*! function: constructor

    description: creates a new SAPDBTrace_Topic.
    arguments:   switchKey [in] the string to be used by
                    <Method>SwitchConditional</>
 */
SAPDBTrace_Topic::SAPDBTrace_Topic (
    const SAPDB_Char * switchKey,
    const SAPDB_Char * switchDescription)
: SAPDBDiag_Topic (switchKey, switchDescription)
{
    this->linePrinter = &nullPrinter;
}

/*----------------------------------------*/

/*! Function: TracesLevel

    returns: true if the local trace level is smaller or equal
        <Param>requestedLevel</>
 */
bool SAPDBTrace_Topic::TracesLevel (
    int requestedLevel) const
{
    return requestedLevel <= this->level;
}

/*----------------------------------------*/

/*! function: SetTracePrinter
    description: changes the global trace printer for this topic
    arguments: printer [in] the new trace printer
 */
void SAPDBTrace_Topic::SetTracePrinter (
    SAPDBTrace_LinePrinter * printer)
{
    this->linePrinter = printer;
}

/*----------------------------------------*/

/*! Function: GetTracePrinter

    returns: the shared trace printer
 */
SAPDBTrace_LinePrinter & SAPDBTrace_Topic::GetTracePrinter () const
{
    SAPDBERR_ASSERT_STATE(this->linePrinter != 0);
    return *this->linePrinter;
}

/*----------------------------------------*/

/*! Function: InitGroup

    description: sets the trace stream for a group of topics.
 */
void SAPDBTrace_Topic::InitGroup (
    SAPDBTrace_Topic * const topics [],
    SAPDBTrace_LinePrinter * printer)
{
    for (int i = 0; topics [i] != NULL; ++i) {
        topics [i]->SetTracePrinter (printer);
    }
}

/*----------------------------------------*/

/*! Function: MultiSwitch

    description: parses a line with trace instructions

        The line has the format "topic level [topic level ...]",
        a topic key followed by a numeric trace level.
 */
void SAPDBTrace_Topic::MultiSwitch (
    SAPDBTrace_Topic * const topics [],
    const SAPDB_Char * line,
    int                lineLen)
{
    SAPDBDiag_Topic::MultiSwitch (
        reinterpret_cast<SAPDBDiag_Topic *const *>(topics), line, lineLen);
}

/*----------------------------------------*/

const SAPDB_Char * SAPDBTrace_Topic::GetTopicKind () const
{
    return "Vtrace";
}

/*----------------------------------------*/

