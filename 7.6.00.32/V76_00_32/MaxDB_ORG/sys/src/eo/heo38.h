/* @lastChanged: "1998-09-04  9:41"

  * @filename:   heo38k.h
  * @purpose:    "COM"
  * @release:    7.2.0.0
  * @see:        "-.-"
  *
  * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

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



\endif

*/


#ifndef HEO38_H
#define HEO38_H

#include "gsp00.h"
#include "Oms/OMS_Defines.h"

externC long sql38CoGetClassObject(CLSID* rclsid, REFIID riid, LPVOID FAR* ppv);

externC void eo38CTraceBack( tsp00_Addr hd ); /*PTS 1106505 FF */

externC int eo38_Eval_Exception ( tsp_dcom_dispatch_parms * parms );

#endif
