/*******************************************************************************

  module:       sdbinfo.h

  ------------------------------------------------------------------------------

  responsible:  Alexander Ringhof (D038248 - alexander.ringhof@sap.com)

  special area: DBM Server: exec_sdbinfo

  description:  header module, determination of db host system parameters

  see also:     none

  ------------------------------------------------------------------------------
                         Copyright (c) 2002-2004 SAP AG

  modifications:
  -------------------------------------------------------------------------
  01   who:
       what:
       when:
  -------------------------------------------------------------------------
\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
*******************************************************************************/

/* INCLUDE SECTION */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef WIN32 
#include <windows.h>
// #include <iostream.h> PTS 1129537
#endif

/* DEFINE SECTION */
#define MXLINELEN 255
#define MXPATHLEN 95
#define MXFILELEN 41
#define MXTEXTLEN 1023

#define BUFSIZE 1024
