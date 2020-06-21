/*!
  @file           Proc_ServerSessionFactory.hpp
  @author         DanielD
  @brief

\if EMIT_LICENCE

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



\endif
*/


#ifndef PROC_SERVERSESSIONFACTORY_HPP
#define PROC_SERVERSESSIONFACTORY_HPP

#include "SAPDBCommon/SAPDB_Types.h"
#include "DBProc/Proc_IFactory.hpp"
#include "DBProc/Proc_PointerList.hpp"

class Proc_ServerControl;

/*!
  @class          Proc_ServerSessionFactory

 */

class Proc_ServerSessionFactory : public Proc_IFactory
{
public:
    Proc_ServerSessionFactory (const char * langidParm, char serverKind, const char * commandlineParm, int sessionsPerServer = 1);
    virtual ~Proc_ServerSessionFactory ();
    virtual Proc_ISession * createSession (SAPDBErr_MessageList * messageList);
    virtual void stopController (Proc_ServerControl * serverControl);
    virtual void detachController (Proc_ServerControl * serverControl);
    virtual void dumpOn (SAPDBTrace_Stream & printer, int indent = 0) const;
    virtual void collectAllServerControls (Proc_ControllerVector & allControls) const;
    virtual const char * getLanguageId () const;
    virtual const char * getCommandTemplate () const;

private:
    Proc_ServerControl * createController (SAPDBErr_MessageList * messageList);
    Proc_ServerControl * findSuitableController ();
    SAPDB_UInt4 nextId ();

private:
    const char * languageid;
    const char * commandTemplate;

    Proc_PointerList serverControls;
    int sessionsPerServer;
    SAPDB_UInt4 idGenerator;
    char serverKind;
};




#endif
