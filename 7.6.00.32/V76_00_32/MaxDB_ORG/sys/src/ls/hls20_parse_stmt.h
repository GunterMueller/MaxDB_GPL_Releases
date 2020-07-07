/*
  -----------------------------------------------------------------------------
  module: vls20_parse_stmt.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  

  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

*/

#include "gls00.h"


/*!
  -----------------------------------------------------------------------------
  function:     ls20ParseInsertStmt
  -----------------------------------------------------------------------------
  description:  Prepare INSERT Statement as mass command.
                Extraxt table information out of the answer packet

  arguments:    MetaDataDef         [IN]  - structure with info on running session and
                                            database connected to
                pTransformationRTE  [IN]  - structure with information on
                                            processing the dataload command
                pszErrText          [OUT] - error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20ParseInsertStmt(MetaDataDef*&             pMetaDataDef,
                    tls00_TransformationRTE*  pTransformationRTE,
                    tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20ParseUpdateStmt
  -----------------------------------------------------------------------------
  description:  Prepare UPDATE Statement as mass command.
                Extraxt table information out of the answer packet

  arguments:    MetaDataDef         [IN]  - structure with info on running session and
                                            database connected to
                pTransformationRTE  [IN]  - structure with information on
                                            processing the dataload command
                pszErrText          [OUT] - error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls20ParseUpdateStmt(MetaDataDef*&              pMetaDataDef,
                    tls00_TransformationRTE*&  pTransformationRTE,
                    tsp00_Addr                 pszErrText);
