/*!***************************************************************************

  module:       SAPDB_Config.hpp

  responsible : Stefan Baier

  special area: Common configuration file

  description:  Contains special defines used in SAPDB.

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#ifndef SAPDB_CONFIG_HPP
#define SAPDB_CONFIG_HPP

#define SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING

#if !defined(ASSERTION_EXCEPTION_CODE) && \
    !defined(ASSERTION_CRASH_CODE)     && \
    !defined(ASSERTION_DEBUG_CODE)     && \
    !defined(ASSERTION_QUICK_CODE)     && \
    !defined(ASSERTION_FAST_CODE)

    #if defined(SAPDB_SLOW)
        #define  ASSERTION_CRASH_CODE
    #elif defined(SAPDB_QUICK)
        #define  ASSERTION_CRASH_CODE
    #elif defined(SAPDB_FAST)
        #define  ASSERTION_FAST_CODE
    #endif

#endif

#endif /* SAPDB_CONFIG_HPP */

