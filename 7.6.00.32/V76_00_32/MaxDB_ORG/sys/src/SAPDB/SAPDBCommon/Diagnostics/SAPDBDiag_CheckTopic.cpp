/*!*********************************************************************

  module:       SAPDBDiag_CheckTopic.cpp

  ----------------------------------------------------------------------

  responsible:  DanielD

  special area: SAPDB Tracing

  description:  allows to switch groups of conditional statements

  ----------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp"

/*! function: constructor

    description: creates a new SAPDBDiag_CheckTopic.
    arguments:   switchKey [in] the string to be used by
                    <Method>SwitchConditional</>
 */
SAPDBDiag_CheckTopic::SAPDBDiag_CheckTopic (
    const SAPDB_Char * switchKey,
    const SAPDB_Char * switchDescription)
: SAPDBDiag_Topic (switchKey, switchDescription)
{
}

/*----------------------------------------*/

/*! Function: ChecksLevel

    returns: true if the local check level is smaller or equal
        <Param>requestedLevel</>
 */
bool SAPDBDiag_CheckTopic::ChecksLevel (
    int requestedLevel) const
{
    return requestedLevel <= this->level;
}

/*----------------------------------------*/

/*! Function: MultiSwitch

    description: parses a line with check instructions

        The line has the format "topic [level] [topic [level] ...]",
        a topic key followed by an optional numeric trace level.
        The topic '*' is a wild card, switching all topics.
 */
void SAPDBDiag_CheckTopic::MultiSwitch (
    SAPDBDiag_CheckTopic * const topics [],
    const SAPDB_Char * line,
    int                lineLen)
{
    SAPDBDiag_Topic::MultiSwitch (
        reinterpret_cast<SAPDBDiag_Topic *const *>(topics), line, lineLen);
}

/*----------------------------------------*/

const SAPDB_Char * SAPDBDiag_CheckTopic::GetTopicKind () const
{
    return "Check";
}

/*----------------------------------------*/


