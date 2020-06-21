/*!*********************************************************************

    @file         Proc_FactoryDirectory.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "DBProc/Proc_FactoryDirectory.hpp"

#include <string.h>
#include "DBProc/Proc_IFactory.hpp"
#include "DBProc/Proc_SystemProcs.hpp"
#include "DBProc/Proc_UDEDirectory.hpp"
#include "RunTime/ProcServer/ProcRTE_Runtime.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "DBProc/Proc_SharedInternals.hpp"


/*----------------------------------------*/

void dumpFactory (
    void * factory_p,
    void * printer_p)
{
    Proc_IFactory * factory = static_cast<Proc_IFactory *> (factory_p);
    SAPDBTrace_Stream * printer = static_cast<SAPDBTrace_Stream *> (printer_p);
    factory->dumpOn (*printer, 1);
}

/*----------------------------------------*/

class Proc_UDEFactoryDirectory
    : public Proc_UDEDirectory
{
public:
    Proc_UDEFactoryDirectory (SAPDBMem_IRawAllocator * allocator)
        : Proc_UDEDirectory (allocator)
    {
    };

    /*----------------------------------------*/

    Proc_IFactory * findFactory (
        Catalog_DBProcObject::Language langid,
        const char * langstring)
    {
        return (Proc_IFactory*) this->findEntry (langid, langstring);
    }

    /*----------------------------------------*/

    void addFactory (
        Catalog_DBProcObject::Language langid,
        const char * langstring,
        Proc_IFactory * newfactory)
    {
        this->addEntry (langid, langstring, (void*) newfactory);
    }

    /*----------------------------------------*/

    void dumpOn (
        SAPDBTrace_Stream & printer,
        int indent = 0)
    {
        printer << SAPDB_Indent (indent) << "UDE Factories:\n";
        this->withAllDo (dumpFactory, &printer);
    }
};

static Proc_UDEFactoryDirectory * factoryDir = NULL;

/*----------------------------------------*/

static void
initDirectory ()
{
    SAPDBMem_IRawAllocator & allocator = RTEMem_Allocator::Instance ();
    factoryDir = new (allocator) Proc_UDEFactoryDirectory (&allocator);
    factoryDir->addFactory (Catalog_DBProcObject::System,
        "System", Proc_SystemProcs::factory ());
}

/*----------------------------------------*/

Proc_IFactory * Proc_FactoryDirectory::
findFactory (
    DBProc_DBProcDescriptor::Language   langEnum,
    const char                  * langString)
{
    if (factoryDir == NULL) {
        initDirectory ();
    }
    return factoryDir->findFactory (langEnum, langString);
}

/*----------------------------------------*/

bool Proc_FactoryDirectory::
addFactory (
    DBProc_DBProcDescriptor::Language   langEnum,
    const char                  * langString,
    Proc_IFactory               * newFactory)
{
    if (factoryDir == NULL) {
        initDirectory ();
    }
    factoryDir->addFactory (langEnum, langString, newFactory);
    return true;
}

/*----------------------------------------*/

void Proc_FactoryDirectory::
dump (
    SAPDBTrace_Topic & topic,
    int level)
{
    if (topic.TracesLevel (level)) {
        SAPDBTrace_Stream printer (&topic.GetTracePrinter ());
        if (factoryDir == NULL) {
            printer << "no UDE Factories initialized\n";
        }
        else {
            factoryDir->dumpOn (printer, 1);
        }
    }
}

/*----------------------------------------*/

void Proc_FactoryDirectory::
dump (
    int level)
{
    Proc_FactoryDirectory::dump (DBProc_Trace, level);
}

/*----------------------------------------*/

void Proc_FactoryDirectory::
collectAllServerControls (
    Proc_ControllerVector & allControls)
{
    if (factoryDir == NULL) {
        return;
    }
    Proc_UDEDirectory::Iterator iter (factoryDir);
    Proc_IFactory * factory;

    while (iter.next ((void**) &factory)) {
        factory->collectAllServerControls (allControls);
    }
}
