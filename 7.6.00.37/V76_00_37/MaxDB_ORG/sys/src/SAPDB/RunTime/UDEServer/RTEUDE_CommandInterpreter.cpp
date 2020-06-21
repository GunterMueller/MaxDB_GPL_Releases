/*!*********************************************************************

    @file         RTEUDE_CommandInterpreter.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#include "RunTime/UDEServer/RTEUDE_CommandInterpreter.hpp"

#include <string.h>
#include <ctype.h>

#include "RunTime/UDEServer/RTEUDE_IPC.hpp"

/*----------------------------------------*/

RTEUDE_CommandInterpreter::
RTEUDE_CommandInterpreter (RTEUDE_CommandLookup * lookup_init)
{
    RTEUDE_CommandLookup * current = lookup_init;
    if ((current->cmdName != NULL)
        && (strlen (current->cmdName) != current->cmdLength))
    {
        while (current->cmdName != NULL) {
            current->cmdLength = strlen (current->cmdName);
            ++current;
        }
    }
    this->lookup = lookup_init;
}

/*----------------------------------------*/

RTEUDE_CommandInterpreter::
~RTEUDE_CommandInterpreter ()
{
}

/*----------------------------------------*/

static RTEUDE_CommandLookup *
findFunction (
    RTEUDE_CommandLookup * lookupTable,
    const char * cmd,
    int          cmdLen)
{
    RTEUDE_CommandLookup * current = lookupTable;
    while (current->cmdName != NULL) {
        if ((cmdLen == current->cmdLength)
            && (memcmp (cmd, current->cmdName, cmdLen) == 0))
        {
            return current;
        }
        ++current;
    }
    return NULL;
}

/*----------------------------------------*/

static const char * unknownCommandC = "unknown command";

/*----------------------------------------*/

bool RTEUDE_CommandInterpreter::
run (
    void       * context,
    RTEUDE_IPC * ipc)
{
    char buf [255];
    int cmdLength;
    RTEUDE_CommandReply reply;

    reply.stopLoop = false;
    while (!reply.stopLoop) {
        /*
         * read next command
         */
        bool readOK = ipc->receive (buf, sizeof (buf) - 1, cmdLength);
        if (!readOK) {
            return false;
        }
        buf [cmdLength] = '\0';
        /*
         * parse and execute
         */
        bool executeOK = this->runOne (context, buf, cmdLength, reply);
        if (reply.sendReply) {
            ipc->send (reply.buf, reply.length);
        }
    }
    return true;
}

/*----------------------------------------*/

bool RTEUDE_CommandInterpreter::
runOne (
    void * context,
    const char * buf,
    int buflen,
    RTEUDE_CommandReply & reply)
{
    /*
     * find command in table
     */
    RTEUDE_TokenIterator tokeni (buf, buflen);
    const char * cmd;
    int          cmdlen;
    if (!tokeni.next (cmd, cmdlen)) {
        reply.set ("");
        return false;
    }
    RTEUDE_CommandLookup * cmdObj = findFunction (this->lookup,
        cmd, cmdlen);
    if (cmdObj == NULL) {
        reply.set (unknownCommandC);
        return false;
    }
    /*
     * execute command handler
     */
    bool executeOK = cmdObj->cmdFunction (context, tokeni, reply);
    return executeOK;
}

/*----------------------------------------*/

RTEUDE_CommandReply::
RTEUDE_CommandReply ()
{
    this->stopLoop = false;
    this->length = 0;
}

/*----------------------------------------*/

bool RTEUDE_CommandReply::
set (
    const char * text,
    int textlen)
{
    if (textlen == -1) {
        textlen = strlen (text);
    }
    if (textlen > sizeof (this->buf)) {
        textlen = sizeof (this->buf);
    }
    memcpy (this->buf, text, textlen);
    this->length = textlen;
    return true;
}

/*----------------------------------------*/

bool RTEUDE_CommandReply::
error (
    const char * text,
    int textlen)
{
    this->set (text, textlen);
    return false;
}

/*----------------------------------------*/

RTEUDE_TokenIterator::
RTEUDE_TokenIterator (
    const char * cmdline,
    int len)
{
    this->readpos = cmdline;
    this->endpos = cmdline + len;
}

/*----------------------------------------*/

RTEUDE_TokenIterator::
~RTEUDE_TokenIterator ()
{
}

/*----------------------------------------*/

static const char *
skipSpace (
    const char * pos,
    const char * endpos)
{
    while ((pos < endpos) && isspace (*pos)) {
        ++pos;
    }
    if (pos >= endpos) {
        return NULL;
    }
    return pos;
}

/*----------------------------------------*/

static const char *
skipToken (
    const char * pos,
    const char * endpos)
{
    while ((pos < endpos) && !isspace (*pos)) {
        ++pos;
    }
    return pos;
}

/*----------------------------------------*/

static const char *
skipToQuote (
    const char * pos,
    const char * endpos,
    char quote)
{
    while ((pos < endpos) && (*pos != quote)) {
        ++pos;
    }
    if (pos >= endpos) {
        return NULL;
    }
    return pos;
}

/*----------------------------------------*/

bool RTEUDE_TokenIterator::
next (
    const char *& token,
    int & tokenLen)
{
    const char * result = ::skipSpace (this->readpos, this->endpos);
    if (result == NULL) {
        this->readpos = this->endpos;
        return false;
    }
    const char * eoToken;
    switch (*result) {
        case '\'':
        case '"':
            eoToken = skipToQuote (result + 1, this->endpos, *result);
            ++result;
            if (eoToken == NULL) {
                // not closed, but accept anyway
                this->readpos = this->endpos;
                token = result;
                tokenLen = this->endpos - result;
            }
            else {
                this->readpos = eoToken + 1;
                token = result;
                tokenLen = eoToken - result;
            }
            break;
        default:
            eoToken = skipToken (result, this->endpos);
            this->readpos = eoToken;
            token = result;
            tokenLen = eoToken - result;
            break;
    }
    return true;
}

/*----------------------------------------*/

void RTEUDE_TokenIterator::
skipSpace ()
{
    while ((this->readpos < this->endpos) && isspace (*this->readpos)) {
        ++this->readpos;
    }
}

/*----------------------------------------*/

const char * RTEUDE_TokenIterator::
current () const
{
    return this->readpos;
}

/*----------------------------------------*/

int RTEUDE_TokenIterator::
length () const
{
    return this->endpos - this->readpos;
}

/*----------------------------------------*/

