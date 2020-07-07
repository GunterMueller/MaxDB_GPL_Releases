/*!**********************************************************************

  module: hwd33hconn.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Connection handle

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG



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




***********************************************************************/

#ifndef HWD33HCONN_H
#define HWD33HCONN_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd05wases.h"
#include "hwd26err.h"
#include "hwd31conn.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: WA_DbConnectionP

  description:
  
************************************************************************/


sapdbwa_DbConnectionP wd33CreateConnHandle( twd31DbConnP	hconn,
											sapdbwa_UInt4	userDllId );


void wd33DestroyConnHandle( sapdbwa_DbConnectionP	hconn );

twd31DbConnP wd33GetConnection( sapdbwa_DbConnectionP	hconn );

/*!**********************************************************************

  EndChapter: WADbConnectionP

************************************************************************/

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
