/*!
  @file           SQLMan_Tracing
  @author         ThomasA
  @special area   Catalog
  @brief          Catalog Session Cache
  @see            example.html ...
  @first created  000-03-09  18:21

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef SQLMAN_TRACING_HPP
#define SQLMAN_TRACING_HPP

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "hta01.h"
#include "hta01_1.h"

extern SAPDBTrace_Topic SQLMan_Trace;   //!< sql manager tracing
extern SAPDBTrace_Topic Catalog_Trace;  //!< catalog tracing 
extern SAPDBTrace_Topic DBProc_Trace;   //!< db-procedure traceing

#ifdef SAPDB_SLOW
#define SQLMAN_TRACE_BUFFER(TOPIC,LVL,MSG,PTR,LEN) {\
  if (TOPIC.TracesLevel(LVL)) {                     \
      {                                             \
		  Kernel_VTrace() << MSG;                   \
      }                                             \
	  t01moveobj (td_always, PTR, 1, LEN);  } }
#else
#define SQLMAN_TRACE_BUFFER(TOPIC,LVL,MSG,PTR,LEN)
#endif

#ifdef SAPDB_SLOW
#define SQLMAN_TRACE_STACK_ENTRY(TOPIC,LVL,MSG,ST,IDX) {\
  if (TOPIC.TracesLevel(LVL)) {                         \
      {                                                 \
		  Kernel_VTrace() << MSG;                       \
      }                                                 \
	  t01stackentry (td_always, ST, IDX);  } }
#else
#define SQLMAN_TRACE_STACK_ENTRY(TOPIC,LVL,MSG,ST,IDX)
#endif

#endif