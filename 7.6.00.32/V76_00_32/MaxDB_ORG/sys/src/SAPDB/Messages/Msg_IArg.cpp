/*!
  @file    Msg_IArg.cpp
  @author  JoergM
  @ingroup Common
  @brief   Message argument interface class

  const member variable initializations
  
  -------------------------------------------------------------------------

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
 */

#include "Messages/Msg_IArg.hpp"
#include "Messages/Msg_IOptArg.hpp"

SAPDB_UTF8  const * const Msg_IArg::UndefinedValue = MSG_IARG_UNDEFINED_VALUE;

SAPDB_Char  const * const Msg_IArg::UnknownTag     = MSG_IARG_UNKNOWN_TAG;

SAPDB_UInt4 const         Msg_IArg::MaxTagLength   = MSG_IARG_TAG_LIMIT;

SAPDB_UTF8  const * const Msg_IOptArg::UndefinedValue = MSG_IARG_UNDEFINED_VALUE;

SAPDB_Char  const * const Msg_IOptArg::UnknownTag     = MSG_IARG_UNKNOWN_TAG;

SAPDB_UInt4 const         Msg_IOptArg::MaxTagLength   = MSG_IARG_TAG_LIMIT;
