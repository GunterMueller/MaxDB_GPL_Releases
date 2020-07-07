/*!*********************************************************************

  module:       SAPDBDiag_Topic.cpp

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

#include "SAPDBCommon/Diagnostics/SAPDBDiag_Topic.hpp"

enum WellKnownLevel {MinTrace = 0, StdTrace = 5, MaxTrace = 10};

/*! function: constructor

    description: creates a new SAPDBDiag_Topic.
    arguments:   switchKey [in] the string to be used by
                    <Method>SwitchConditional</>
                 switchDescription [in] the description of the switch
 */
SAPDBDiag_Topic::SAPDBDiag_Topic (
    const SAPDB_Char * switchKey,
    const SAPDB_Char * switchDescription)
{
    this->topicKey = switchKey;
    this->level = MinTrace;
    this->topicDescription = switchDescription;
}

/*----------------------------------------*/

/*! Function: SwitchLevel

    description:
    returns: the old trace level
 */
int SAPDBDiag_Topic::SetTraceLevel (
    int newLevel)
{
    SAPDBERR_ASSERT_ARGUMENT (MinTrace <= newLevel);
    SAPDBERR_ASSERT_ARGUMENT (newLevel <= MaxTrace);
    int result = this->level;

    this->level = newLevel;
    return result;
}

/*----------------------------------------*/

/*! Function: Matches

    returns: true if <Param>testKey</> match the internal key
 */
bool SAPDBDiag_Topic::Matches (
    const SAPDB_Char * testKey,
    int testKeyLen) const
{
    bool result;
    int keyLen = static_cast<int> (strlen (this->topicKey));

    if (keyLen != testKeyLen) {
        return false;
    }
    if (memcmp (this->topicKey, testKey, keyLen) == 0) {
        result = true;
    }
    else {
        result = false;
    }
    return result;
}

/*----------------------------------------*/

static void skipSpace (
    const SAPDB_Char *& current,
    const SAPDB_Char *  eoLine)
{
    while ((current < eoLine) && isspace (*current)) {
        ++current;
    }
}

/*----------------------------------------*/

static int parseTraceKey (
    const SAPDB_Char *& current,
    const SAPDB_Char *  eoLine)
{
    int result = 0;

    while ((current < eoLine) && !isspace (*current)) {
        ++current;
        ++result;
    }
    return result;
}

/*----------------------------------------*/

static int parseTraceLevel (
    const SAPDB_Char *& current,
    const SAPDB_Char *  eoLine)
{
    int result = 0;
    int digitCount = 0;

    skipSpace (current,eoLine);
    if (current >= eoLine) {
        return StdTrace;
    }
    while ((current < eoLine) && isdigit (*current)) {
        result *= 10;
        result += *current - '0';
        ++current;
        ++digitCount;
    }
    if (digitCount == 0) {
        result = StdTrace;
    }
    return result;
}

/*----------------------------------------*/

static void local_strncpy (
    SAPDB_Char * target,
    int targetSize,
    const SAPDB_Char * source)
{
    int sourceLen = static_cast<int> (strlen (source));
    if (sourceLen > targetSize - 1) {
        sourceLen = targetSize - 1;
    }
    memcpy (target, source, sourceLen);
    target [sourceLen] = '\0';
}

/*----------------------------------------*/

const SAPDB_Char * SAPDBDiag_Topic::GetName () const
{
    return this->topicKey;
}

/*----------------------------------------*/

void SAPDBDiag_Topic::GetInfo (
    SAPDB_Char * kindOut,
    int          maxKindLen,
    SAPDB_Char * topicKeyOut,
    int          maxTopicKey,
    int        & levelOut,
    SAPDB_Char * topicDescOut,
    int          maxTopicDesc) const
{
    local_strncpy (kindOut, maxKindLen, this->GetTopicKind());
    local_strncpy (topicKeyOut, maxTopicKey, this->topicKey);
    levelOut = this->level;
    local_strncpy (topicDescOut, maxTopicDesc, this->topicDescription);
}

/*----------------------------------------*/

/*! Function: MultiSwitch

    description: parses a line with trace instructions

        The line has the format "topic [level] [topic [level] ...]",
        a topic key followed by an optional numeric trace level.
        The topic '*' is a wild card, switching all topics.
 */
void SAPDBDiag_Topic::MultiSwitch (
    SAPDBDiag_Topic * const topics [],
    const SAPDB_Char * line,
    int                lineLen)
{
    if (lineLen == -1) {
        lineLen = static_cast<int> (strlen (line));
    }
    const SAPDB_Char * eoLine = line + lineLen;
    const SAPDB_Char * current = line;
    const SAPDB_Char * key;
    int                keyLen;
    int                newLevel;

    skipSpace (current, eoLine);
    while (current < eoLine) {
        // parse trace key
        key = current;
        keyLen = parseTraceKey (current,eoLine);
        // parse integer
        newLevel = parseTraceLevel (current,eoLine);
        // search for matching topic
        if ((keyLen == 1) && (key [0] == '*')) {
            /*
             * wildcard * switches all
             */
            for (int i = 0; topics [i] != NULL; ++i) {
                topics [i]->SetTraceLevel(newLevel);
            }
        }
        else {
            /*
             * search for first matching topic
             */
            for (int i = 0; topics [i] != NULL; ++i) {
                if (topics [i]->Matches(key, keyLen)) {
                    topics [i]->SetTraceLevel(newLevel);
                    break;
                }
            }
        }
        skipSpace (current, eoLine);
    }
}

/*----------------------------------------*/

void SAPDBDiag_Topic::SingleSwitch (
    SAPDBDiag_Topic  * const topics [],
    const SAPDB_Char * topicName,
    int                newLevel)
{
    int namelen = strlen (topicName);
    for (int i = 0; topics [i] != NULL; ++i) {
        if (topics [i]->Matches(topicName, namelen)) {
            topics [i]->SetTraceLevel(newLevel);
            break;
        }
    }
}

/*----------------------------------------*/

