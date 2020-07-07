/*!*********************************************************************

    @file         Proc_ISession.hpp

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

#ifndef PROC_ISESSION_HPP
#define PROC_ISESSION_HPP

class SQLMan_Context;
class DBProc_DBProcDescriptor;
class Proc_IServerControl;
class Proc_ParameterCopy;
class SAPDBTrace_Stream;

/*!

  @class Proc

 */
class Proc
{
public:
    enum Error {ok, communicationFailed, internalSQLFailed,
        internalError, udeReturnedError};
private:
    Proc () {};
};

/*!

  @class Proc_ISession

 */
class Proc_ISession
{
public:
    virtual ~Proc_ISession ();
    virtual Proc::Error callDBProc (SQLMan_Context      * context,
                             DBProc_DBProcDescriptor    * procDescriptor,
                             Proc_ParameterCopy         * parameterMap) = 0;
    virtual void release () {};
    virtual Proc_IServerControl * getController () = 0;
    virtual void dumpOn (SAPDBTrace_Stream & printer, int indent = 0) const = 0;
protected:
    Proc_ISession () {};
};


/*
 * INLINE METHODS
 */

inline Proc_ISession::
~Proc_ISession ()
{
    this->release ();
};

#endif

