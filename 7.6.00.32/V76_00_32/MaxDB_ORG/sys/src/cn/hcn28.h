/*!
  -----------------------------------------------------------------------------

  module: hcn28.h

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBMServer Parameter File Migration

  description:  DBMServer Parameter File Migration - Specification

  version:      7.4.*

  -----------------------------------------------------------------------------

  Copyright (c) 2001-2005 SAP AG

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



  -----------------------------------------------------------------------------
*/

#ifndef _HCN28_H_
#define _HCN28_H_

/* 
  -------------------------------------------------------------------------
  includes
  -------------------------------------------------------------------------
 */
#include "gcn00.h"
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"

/*!
  -------------------------------------------------------------------------
  function:     cn28MigrateParamFile
  -------------------------------------------------------------------------
  description:  migrates the param file

  arguments:    szDbName    [IN]    - database name
                replyData   [INOUT] - buffer for answer in case of errors
                replyLen    [INOUT] - len of answer in case of errors
  -------------------------------------------------------------------------
 */
tcn00_Error cn28MigrateParamFile 
      ( tsp00_DbNamec          szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax);

#endif /* _HCN28_H_ */
