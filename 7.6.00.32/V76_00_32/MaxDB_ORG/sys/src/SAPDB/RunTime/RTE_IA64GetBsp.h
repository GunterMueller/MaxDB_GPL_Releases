/*!***************************************************************************

  module      : RTE_IA64GetBsp.h

  -------------------------------------------------------------------------

  responsible : Robin

  special area: RTE
  description : Get the Backing Store out of the depths of the IA64 processor...

  As this pointer is stored in a special register, the implementation is
  found in an assembly language module

  last changed: 2004-03-01  17:32
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright by SAP AG, 2004

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



*****************************************************************************/


#ifndef RTE_IA64GETBSP_H
#define RTE_IA64GETBSP_H

/*!
  Function: RTE_IA64GetBsp
  Description: Return the current value of the IA64 backing store pointer 

  Return value: the current value of the IA64 backing store pointer 
 */
extern "C" void *RTE_IA64GetBsp(void);

#endif // RTE_IA64GETBSP_H