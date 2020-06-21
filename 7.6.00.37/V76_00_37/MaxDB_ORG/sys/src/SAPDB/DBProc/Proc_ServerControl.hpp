/*!
  @file           Proc_ServerControl.hpp
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


#ifndef PROC_SERVERCONTROL_HPP
#define PROC_SERVERCONTROL_HPP

#include "DBProc/Proc_IServerControl.hpp"
#include "DBProc/Proc_PointerList.hpp"

class Proc_ISession;
class Proc_ServerSession;
class ProcRTE_ServerControl;
class Proc_ServerSessionFactory;
class SAPDBErr_MessageList;

/*!
  @class          Proc_ServerControl

 */

class Proc_ServerControl : public Proc_IServerControl
{
public:
    virtual ~Proc_ServerControl ();
    virtual void stop (bool detachFromFactory = true);
    virtual void dumpOn (SAPDBTrace_Stream & printer, int indent = 0) const;
    virtual Proc_ISession * createSession (SAPDBErr_MessageList * messageList);
    virtual void releaseSession (Proc_ISession * session);
    virtual bool hasFreeSlot() const;
    virtual bool isValid () const;
    virtual const char * kind () const;
    virtual SAPDB_UInt4 id () const;
    virtual int sessionCount () const;
    virtual const char * cmdline () const;
    virtual void initSessionIterator (Proc_ServerSessionIterator * iterator);

protected:
    Proc_ServerControl (Proc_ServerSessionFactory * factory,
        SAPDB_UInt4  controllerId,
        char         serverKind,
        const char * commandLine,
        int          sessionsAllowed,
        SAPDBErr_MessageList * messageList);

private:
    Proc_ServerSessionFactory * factory;
    SAPDB_UInt4  controllerId;
    ProcRTE_ServerControl * rteServerControl;
    Proc_PointerList     sessions;
    int                  sessionsAllowed;
    friend class Proc_ServerSessionFactory;
};



#endif
