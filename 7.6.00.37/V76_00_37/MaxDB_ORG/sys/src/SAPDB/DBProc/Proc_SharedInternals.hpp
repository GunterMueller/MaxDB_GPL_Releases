/*!*********************************************************************

    @file         Proc_SharedInternals.hpp

    @author       DanielD

    @brief        shared definitions for Proc_*.cpp

    Should never be included in a .hpp.

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

#ifndef PROC_SHAREDINTERNALS_HPP
#define PROC_SHAREDINTERNALS_HPP

#include "Container/Container_Vector.hpp"

class Proc_IServerControl;

/*!

  @class Proc_ControllerVector

 */
class Proc_ControllerVector : public Container_Vector<Proc_IServerControl *>
{
public:
    Proc_ControllerVector (SAPDBMem_IRawAllocator & allocator)
    : Container_Vector<Proc_IServerControl *> (allocator) {};
};


#endif

