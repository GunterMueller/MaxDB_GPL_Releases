/*!========================================================================

  @ingroup      Sec
  @file         RTESec_MCF.h
  @author       ChristophB

  @brief        

  @since        2004-08-18  13:43
  @sa           


  =======================================================================

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
  ----------------------------------------------------------------------------*/

#ifndef RTESEC_MCF64_H
#define RTESEC_MCF64_H RTESEC_MCF64_H

/*===========================================================================*
 *  INCLUDES                                                               *
 *===========================================================================*/
#include "gsp01.h"
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/

externC int MCFEnc (void *, void *, int);
externC int MCFDec (void *, void *, int);

#endif

