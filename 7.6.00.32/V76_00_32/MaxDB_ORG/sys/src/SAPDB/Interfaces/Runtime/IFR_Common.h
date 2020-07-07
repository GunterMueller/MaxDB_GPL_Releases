/*!
  @file           IFR_Common.h
  @author         BurkhardD
  @ingroup        IFR_Common
  @brief          Common Base Header for Interface Runtime
  @see            

\if EMIT_LICENCE

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



\endif
*/
/*!
  @defgroup       IFR_Runtime    Interface Runtime
*/
/*!
  @defgroup       IFR_Common     Common definitions
  @ingroup        IFR_Runtime
*/
/*!
  @defgroup       IFR_Connection Database connection
  @ingroup        IFR_Runtime
*/
/*!
  @defgroup       IFR_Statement  Executing SQL commands
  @ingroup        IFR_Runtime
*/
/*!
  @defgroup       IFR_Fetch      Fetching data
  @ingroup        IFR_Runtime
*/
/*!
  @defgroup       IFR_DataConv   Data conversion
  @ingroup        IFR_Runtime
*/
/*!
  @defgroup       IFR_Packet     Operation with the Order Interface
  @ingroup        IFR_Runtime
*/
/*!
  @defgroup       IFR_Trace      Tracing and logging
  @ingroup        IFR_Runtime
*/
/*!
  @defgroup       IFR_Util       Utilities
  @ingroup        IFR_Runtime
*/
#ifndef IFR_COMMON_H
#define IFR_COMMON_H

#define INCLUDED_FROM_IFR_COMMON
#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/Util/IFRUtil_New.h"
#include "Interfaces/Runtime/IFR_Trace.h"
#include "Interfaces/Runtime/IFR_TraceContext.h"
#undef INCLUDED_FROM_IFR_COMMON

#endif // IFR_COMMON_H
