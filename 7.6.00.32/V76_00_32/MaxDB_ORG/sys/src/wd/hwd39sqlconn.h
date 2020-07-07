/*!**********************************************************************

  module: hwd39sqlconn.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  C-Wrapper for connect/disconnect with SQL-Class

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 1999-2005 SAP AG



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

#ifndef HWD39SQLCONN_H
#define HWD39SQLCONN_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct st_sqlconn *twd39SqlConnP;

void wd39SqlInit();

void wd39SqlFinish();

void *wd39SqlSession( twd39SqlConnP conn );
    
tsp00_Bool wd39AllocSqlConn( twd39SqlConnP *pconn, twd26ErrP err );

tsp00_Bool wd39FreeSqlConn( twd39SqlConnP conn );

tsp00_Bool wd39SqlConnect( twd39SqlConnP  conn,
                           const char    *host,
                           const char    *serverdb,
                           const char    *user,
                           const char    *password,
                           SAPDB_Int4     isolation,
                           twd26ErrP      err );


tsp00_Bool wd39SqlDisconnect( twd39SqlConnP conn, twd26ErrP err );

#if defined(__cplusplus)
}
#endif

#endif
