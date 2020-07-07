/*!*********************************************************************

    @file         Proc_IServerControl.hpp

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

#ifndef PROC_ISERVERCONTROL_HPP
#define PROC_ISERVERCONTROL_HPP

#include "SAPDBCommon/SAPDB_Types.h"

class Proc_ServerSessionIterator;

class SAPDBTrace_Stream;
/*!

  @class Proc_IServerControl

 */
class Proc_IServerControl
{
public:
    virtual void stop (bool detachFromFactory = true) = 0;
    virtual const char * kind () const = 0;
    virtual SAPDB_UInt4 id () const = 0;
    virtual int sessionCount () const = 0;
    virtual const char * cmdline () const = 0;
    virtual void initSessionIterator (Proc_ServerSessionIterator * iterator) = 0;
    virtual void dumpOn (SAPDBTrace_Stream & printer, int indent = 0) const = 0;
};


#endif
