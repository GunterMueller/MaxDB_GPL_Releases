/*!*********************************************************************

    module:       Proc_ServerControl.cpp

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

#include "DBProc/Proc_ServerControl.hpp"

#include <string.h>
#include <stdio.h>

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp"
#include "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
#include "RunTime/ProcServer/ProcRTE_Runtime.hpp"
#include "RunTime/ProcServer/ProcRTE_ServerControl.hpp"
#include "DBProc/Proc_ServerSession.hpp"
#include "DBProc/Proc_ServerSessionFactory.hpp"
#include "DBProc/Proc_ServerSessionIterator.hpp"
#include "DBProc/Proc_FactoryDirectory.hpp"
#include "SQLManager/DBProc/DBProc_Messages.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"

Proc_ServerControl::Proc_ServerControl (
    Proc_ServerSessionFactory * factory_init,
    SAPDB_UInt4  controllerId_init,
    char         serverKind,
    const char * commandLine,
    int          sessionsAllowed_init,
    SAPDBErr_MessageList * messageList)
: sessions (ProcRTE_Runtime::allocator (), sessionsAllowed_init)
{
    this->factory = factory_init;
    this->controllerId = controllerId_init;
    this->rteServerControl =
        ProcRTE_ServerControl::createServerControl (
            serverKind == 'P', commandLine,
            (SAPDB_Int4) strlen (commandLine), messageList);
    this->sessionsAllowed = sessionsAllowed_init;
}

/*----------------------------------------*/

Proc_ServerControl::~Proc_ServerControl ()
{
    this->stop ();
}

/*----------------------------------------*/

void Proc_ServerControl::stop (
    bool detachFromFactory)
{
    if (this->rteServerControl != NULL) {
        this->rteServerControl->simpleCmd ("stop");
        ProcRTE_Runtime::destroy (this->rteServerControl);
        this->rteServerControl = NULL;
        if (detachFromFactory && (this->factory != NULL)) {
            this->factory->detachController (this);
        }
    }
    this->factory = NULL;
}

/*----------------------------------------*/

void Proc_ServerControl::
dumpOn (
    SAPDBTrace_Stream & printer,
    int indent) const
{
    printer << SAPDB_Indent (indent) << "Proc_ServerControl ("
        << SAPDB_ToString (this) << ")" << FlushLine;
    printer << SAPDB_Indent (indent + 1) << "factory = "
        << SAPDB_ToString (this->factory) << FlushLine;
    printer << SAPDB_Indent (indent = 1) << "rteServerControl = "
        << SAPDB_ToString (this->rteServerControl) << FlushLine;
    /*
     * dump session objects
     */
    Proc_ListIterator iterator (&this->sessions);
    Proc_ISession * current;
    while (iterator.next ((void**)&current)) {
        current->dumpOn (printer, indent + 1);
    }
}


/*----------------------------------------*/

Proc_ISession * Proc_ServerControl::createSession (
    SAPDBErr_MessageList * messageList)
{
    /*
     * create Proc_ServerSession
     */
    Proc_ServerSession * result
        = PROCRTE_RUNTIME_NEW (Proc_ServerSession) (this);
    if (result == NULL) {
        *messageList = SAPDBErr_MessageList (DBPROC_OUT_OF_MEMORY,
            ToStr (sizeof (Proc_ServerSession)));
        return NULL;
    }
    /*
     * wait for connect
     */
    SAPDBErr_MessageList msgList;
    RTETask_ITaskClientConnection * connection = this->rteServerControl->createSession (messageList);
    if (connection == NULL) {
        ProcRTE_Runtime::free (result);
        return NULL;
    }
    result->finishConnectHandshake (connection);
    /*
     * keep pointer
     */
    this->sessions.addPointer (result);
    SAPDBTRACE_IF (DBProc_Trace, 8, this->sessions.trace (DBProc_Trace, "added session"));
    /*
     * return
     */
    return result;
}

/*----------------------------------------*/

void Proc_ServerControl::releaseSession (
    Proc_ISession * sessionParam)
{
    SAPDBTRACE_WRITELN (DBProc_Trace, 8, "Proc_ServerControl::releaseSession ("
        << ToStr (sessionParam) << ")");
    if (!this->sessions.hasPointer (sessionParam)) {
        this->sessions.trace (DBProc_Trace, "missing object for release ");
#if defined (SAPDB_SLOW)
        SAPDBTRACE_WRITELN (DBProc_Trace, 1, "control " << SAPDB_ToString (this)
            << " has no session " << SAPDB_ToString (sessionParam));
        Proc_FactoryDirectory::dump (5);
#endif
    }
    SAPDBERR_PRECONDITION(this->sessions.hasPointer (sessionParam));
    ProcRTE_Runtime::destroy (sessionParam);
    this->sessions.clearPointer (sessionParam);
    if (this->sessionsAllowed == 1) {
        this->stop ();
    }
}

/*----------------------------------------*/

bool Proc_ServerControl::
hasFreeSlot() const
{
    return this->sessions.hasFreeSlot ();
}

/*----------------------------------------*/

bool Proc_ServerControl::
isValid () const
{
    return this->rteServerControl != NULL;
}

/*----------------------------------------*/

const char * Proc_ServerControl::
kind () const
{
    return this->factory->getLanguageId ();
}

/*----------------------------------------*/

SAPDB_UInt4 Proc_ServerControl::
id () const
{
    return this->controllerId;
}

/*----------------------------------------*/

int Proc_ServerControl::
sessionCount () const
{
    return this->sessions.slotsUsed ();
}

/*----------------------------------------*/

const char * Proc_ServerControl::
cmdline () const
{
    return this->factory->getCommandTemplate ();
}

/*----------------------------------------*/

void Proc_ServerControl::
initSessionIterator (
    Proc_ServerSessionIterator * iterator)
{
    iterator->init (&this->sessions);
}

/*----------------------------------------*/

