/*!
  @file           RTE_ClientTraceTopics.cpp
  @author         StefanP
  @brief          Trace Topics for Client Applications
  @see            

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
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

/*===========================================================================*
 *  GLOBALES                                                                 *
 *===========================================================================*/


/*! global memory trace object */
SAPDBTrace_Topic Memory_Trace               ("MEMORY", "Trace for memory management");
/*! global messages trace object */
SAPDBTrace_Topic Messages_Trace             ("MESSAGES","Trace for generated messages");
/*! global runtime trace object */
SAPDBTrace_Topic Runtime_Trace              ("RUNTIME", "Trace for runtime environment");
/*! global synchronisation trace object */
SAPDBTrace_Topic Sync_Trace                 ("SYNCHRONISATION", "Trace for synchronisation mechanismen");
/*! global interprocess communication trace object */
SAPDBTrace_Topic IPC_Trace                  ("IPC", "Trace for interprocess communication");
/*! global client kernel communication trace object */
SAPDBTrace_Topic CKC_Trace                  ("CLIENTKERNELCOM", "Trace for client kernel communication");
/*! global system trace object */
SAPDBTrace_Topic System_Trace               ("SYSTEM", "Trace for system access");
/*! global communication trace object */
SAPDBTrace_Topic Communication_Trace        ("COMMUNICATION", "Trace for communication");


