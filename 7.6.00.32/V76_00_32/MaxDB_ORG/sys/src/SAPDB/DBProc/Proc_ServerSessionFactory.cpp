/*!*********************************************************************

    module:       Proc_ServerSessionFactory.cpp

    --------------------------------------------------------------------

    responsible:  DanielD

    special area:

    description:

    --------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




***********************************************************************/

#include "DBProc/Proc_ServerSessionFactory.hpp"

#include <stddef.h>
#include <string.h>

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include "RunTime/ProcServer/ProcRTE_Runtime.hpp"
#include "DBProc/Proc_ServerControl.hpp"
#include "SQLManager/DBProc/DBProc_Messages.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "DBProc/Proc_SharedInternals.hpp"

Proc_ServerSessionFactory::
Proc_ServerSessionFactory (
    const char * langidParm,
    char         serverKind_init,
    const char * commandLine,
    int          sessionsPerServer_init)
: serverControls (
    ProcRTE_Runtime::allocator (),
    (sessionsPerServer == Proc_PointerList::noLimitC) ? 1 : Proc_PointerList::noLimitC)
    // unlimited sessions per UDE server: one UDE server handles all
    // limited sessions per UDE server: potentially unlimited number of UDE servers
{
    SAPDBTRACE_WRITELN (DBProc_Trace, 5, "Proc_ServerSessionFactory (\""
        << langidParm << "\", \"" << commandLine << "\") ->" << ToStr (this));
    int commandLen = strlen (commandLine) + 1;
    int langidLen = strlen (langidParm) + 1;
    this->commandTemplate = ProcRTE_Runtime::alloc (commandLen + langidLen);
    memcpy ((void *) this->commandTemplate, commandLine, commandLen);
    this->languageid = this->commandTemplate + commandLen;
    memcpy ((void *) this->languageid, langidParm, langidLen);
    this->sessionsPerServer = sessionsPerServer_init;
    this->idGenerator = 0;
    this->serverKind = serverKind_init;
}

/*----------------------------------------*/

Proc_ServerSessionFactory::
~Proc_ServerSessionFactory ()
{
    if (this->commandTemplate != 0) {
        ProcRTE_Runtime::free ((void*) this->commandTemplate);
    }

    Proc_ListIterator iterator (&this->serverControls);
    Proc_ServerControl * current;

    while (iterator.next ((void**) &current)) {
        current->stop (false);
        ProcRTE_Runtime::destroy (current);
    }
}

/*----------------------------------------*/

Proc_ServerControl * Proc_ServerSessionFactory::
createController (
    SAPDBErr_MessageList * messageList)
{
    Proc_ServerControl * result;

    result = PROCRTE_RUNTIME_NEW (Proc_ServerControl) (
                    this, this->nextId (), serverKind,
                    this->commandTemplate, this->sessionsPerServer, messageList);
#if SAPDB_SLOW
    if (result == NULL) {
        SAPDBTRACE_WRITELN (DBProc_Trace, 5, "creating controller failed");
    }
    else {
        SAPDBTRACE_WRITELN (DBProc_Trace, 5, "created " << this->languageid << " controller: " << ToStr (result));
    }
#endif
    /*
     * check for errors
     */
    if (result == NULL) {
        *messageList = SAPDBErr_MessageList (
            DBPROC_OUT_OF_MEMORY, ToStr (sizeof (Proc_ServerControl)));
        return NULL;
    }
    if (!result->isValid ()) {
        return NULL;
    }
    /*
     * keep reference
     */
    this->serverControls.addPointer (result);
    SAPDBTRACE_IF (DBProc_Trace, 8, this->serverControls.trace (DBProc_Trace, "added controller"));
    return result;
}

/*----------------------------------------*/

Proc_ServerControl * Proc_ServerSessionFactory::
findSuitableController ()
{
    Proc_ListIterator iterator (&this->serverControls);
    Proc_ServerControl * current;

    while (iterator.next ((void**) &current)) {
        if (current->hasFreeSlot ()) {
            return current;
        }
    }
    return NULL;
}

/*----------------------------------------*/

SAPDB_UInt4 Proc_ServerSessionFactory::
nextId ()
{
    ++this->idGenerator;
    return this->idGenerator;
}

/*----------------------------------------*/

Proc_ISession * Proc_ServerSessionFactory::
createSession (
    SAPDBErr_MessageList * messageList)
{

    SAPDBTRACE_METHOD_DEBUG ("Proc_ServerSessionFactory::createSession", DBProc_Trace, 5);
    Proc_ServerControl * controller;

    /*
     * search for a controller with a free slot
     */
    controller = this->findSuitableController ();
#if SAPDB_SLOW
    if (controller != NULL) {
        SAPDBTRACE_WRITELN (DBProc_Trace, 6, "using existing session "
            << ToStr (controller));
    }
#endif
    if (controller == NULL) {
        controller = this->createController (messageList);
    }
    if (controller == NULL) {
        return NULL;
    }
    /*
     * allocate session object
     */
    Proc_ISession * result = controller->createSession (messageList);
    SAPDBTRACE_WRITELN (DBProc_Trace, 5, "created new "
        << this->languageid << " session " << ToStr (result));
    return result;
}

/*----------------------------------------*/

void Proc_ServerSessionFactory::
stopController (
    Proc_ServerControl * serverControl)
{
    ProcRTE_Runtime::destroy (serverControl);
    this->detachController (serverControl);
}

/*----------------------------------------*/

void Proc_ServerSessionFactory::
detachController (
    Proc_ServerControl * serverControl)
{
    this->serverControls.clearPointer (serverControl);
}

/*----------------------------------------*/

int realStringLength (
    const char * text)
{
    const char * end = text + strlen (text) - 1;
    while ((end > text) && isspace (*end)) {
        --end;
    }
    return end - text + 1;
}

/*----------------------------------------*/

void Proc_ServerSessionFactory::
dumpOn (
    SAPDBTrace_Stream & printer,
    int indent) const
{
    printer << SAPDB_Indent (indent) << "Proc_ServerSessionFactory ("
        << SAPDB_ToString (this) << "):" << FlushLine;
    printer << SAPDB_Indent (indent + 1) << "langid = " << this->languageid << FlushLine;
    printer << SAPDB_Indent (indent + 1) << "sessionsPerServer = " << this->sessionsPerServer << FlushLine;
    int cmdLen = realStringLength (this->commandTemplate);
    printer << SAPDB_Indent (indent + 1) << "commandLine = '"
        << ToStr (this->commandTemplate, cmdLen, cmdLen) << "'" << FlushLine;

    Proc_ListIterator iterator (&this->serverControls);
    Proc_ServerControl * current;
    bool hasController = false;

    while (iterator.next ((void**) &current)) {
        current->dumpOn (printer, indent + 1);
        hasController = true;
    }
    if (!hasController) {
        printer << SAPDB_Indent (2) << "no controller found\n";
    }
}

/*----------------------------------------*/

void Proc_ServerSessionFactory::
collectAllServerControls (
    Proc_ControllerVector & allControls) const
{
    Proc_ListIterator iter (&this->serverControls);
    Proc_IServerControl * control;

    while (iter.next ((void**) &control)) {
        allControls.InsertEnd (control);
    }
}

/*----------------------------------------*/

const char * Proc_ServerSessionFactory::
getLanguageId () const
{
    return this->languageid;
}

/*----------------------------------------*/

const char * Proc_ServerSessionFactory::
getCommandTemplate () const
{
    return this->commandTemplate;
}

/*----------------------------------------*/


