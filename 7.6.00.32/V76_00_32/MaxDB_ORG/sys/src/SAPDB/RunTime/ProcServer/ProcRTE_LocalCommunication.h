/*!*********************************************************************

    @file         ProcRTE_LocalCommunication.h
    
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

#ifndef PROCRTE_LOCALCOMMUNICATION_HPP
#define PROCRTE_LOCALCOMMUNICATION_HPP

#include "gsp00.h"
#if defined (_WIN32) /*&if $OSSPEC == WIN32 */
#include "gos003.h"
#endif               /*&endif               */

/*!
  @brief install call
 */
externC void
procRTE_installConstructor ();

#endif
