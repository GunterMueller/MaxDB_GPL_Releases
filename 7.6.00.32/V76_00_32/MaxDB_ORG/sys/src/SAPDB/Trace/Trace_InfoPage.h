/*!**************************************************************************

  module       : Trace_InfoPage.h

  -------------------------------------------------------------------------

  responsible : JoegM

  special area: Handling of trace entries

  description : Function to create a trace info page from C

  last changed: 2000-12-18  20:48
  see also    : example.html ...
  first created:2000-12-18  20:44

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#ifndef TRACE_INFOPAGE_H
#define TRACE_INFOPAGE_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
  Function: Trace_InfoPageSetup
  Description: Setup Trace_InfoPage using KGS information

  Called during kernel initialisation. Setup first page of information
  contained the trace buffer sizes of each UKT used.
 */
externC void Trace_InfoPageSetup();

#endif  /* TRACE_INFOPAGE_H */
