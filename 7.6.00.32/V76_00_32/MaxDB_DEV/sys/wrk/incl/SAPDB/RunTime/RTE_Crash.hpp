/*!***************************************************************************

  module      : RTE_Crash.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE
  description : Crash of Kernel

  Issue famous last words

  last changed: 2001-01-11  17:32
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef RTE_CRASH_HPP
#define RTE_CRASH_HPP

class Msg_List;

/*!
  @brief Crash kernel with last words output

  Let kernel crash writing last words into knldiag, after that write stack
  backtrace and resume tracewriter.

  @param msgList [in] The famous last words
  @return will never return...
 */
void RTE_Crash( const Msg_List &msgList );

#endif  /* RTE_CRASH_HPP */
