/*!
    \file    ToolsMeMa_New.hpp
    \author  TiloH
    \ingroup infrastructure for tools group
    \brief   a substitute for new operator, that does not throw any exceptions

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

#if !defined(TOOLSMEMA_NEW)
#define TOOLSMEMA_NEW

#include <new.h>

/*! \brief a macro supplying the exception free operator new on our different
    platforms */

#if defined(WIN32)
#define ToolsMeMa_New new
#else
#if defined (HPUX)
#define ToolsMeMa_New new(nothrow)
#else
#define ToolsMeMa_New new(std::nothrow)
#endif
#endif

#endif
