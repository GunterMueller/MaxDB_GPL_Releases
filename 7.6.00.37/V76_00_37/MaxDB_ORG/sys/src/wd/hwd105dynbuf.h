/*!**********************************************************************

  module: hwd105dynbuf.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Dynamic buffer

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




************************************************************************/

#ifndef HWD105DYNBUF_H
#define HWD105DYNBUF_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gsp00.h"
#include "hpa102odcompr.h"
#include "hwd01wadef.h"

/*!**********************************************************************

  Chapter: twd105DynBufP

  description:
  
************************************************************************/

typedef struct st_dyn_buf *twd105DynBufP;

twd105DynBufP wd105CreateDynBuf( sapdbwa_Int4 initialSize );

void wd105DestroyDynBuf( twd105DynBufP dynBuf );

sapdbwa_Bool wd105AddToBuf( twd105DynBufP  dynBuf,
                          char          *buf,
                          sapdbwa_UInt4    nBytes );

sapdbwa_Bool wd105FreeBuf( twd105DynBufP dynBuf );

sapdbwa_UInt4 wd105GetTotalSize( twd105DynBufP dynBuf );

SQLRETURN wd105PutDataCompressed( twd105DynBufP   dynBuf,
                                  tpa102_ODCompr  odcompr,
                                  sapdbwa_Int4     *comprErr );

/*!**********************************************************************

  EndChapter: twd105DynBufP

************************************************************************/

#endif
