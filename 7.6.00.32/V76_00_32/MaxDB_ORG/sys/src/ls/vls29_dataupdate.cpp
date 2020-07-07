/*!
  -----------------------------------------------------------------------------
  module: vls29_dataupdate.cpp
  -----------------------------------------------------------------------------

  responsible:  d033893

  special area: SAP DB LOADER

  description:  dataupdate

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
#include "hls20.h"
#include "hls29_dataupdate.h"
#include "hls99.h"
#include "hls98msg.h"


/*
  -----------------------------------------------------------------------------
  function:     UpdatePackageDataRecord
  -----------------------------------------------------------------------------
*/
tsp00_Int4
UpdatePackageDataRecord(MetaDataDef *pMetaDataDef, tls00_DBInfo *pDBInfo, tsp00_Addr pszErrText)
{ 
    ROUTINE_DBG_MEO00 ("UpdatePackageDataRecord");

    tsp00_Longint  rc = errOK_els00;
    tls00_TransformationRTE*  pTransformationRTE  = NULL;

    // interpret <dataupdate statement>::=
    // UPDATE DATA TABLE <table_spec> <acc_column_spec_mlt> <set_column_spec_mlt> <infile_spec> <longfile_spec_mlt>;
    rc = ls20ImportTableRows(pMetaDataDef, pTransformationRTE, pszErrText);

    return STATIC_CAST(tsp00_Int4, rc);
}
//UpdatePackageDataRecord()
