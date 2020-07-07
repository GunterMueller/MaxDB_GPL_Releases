/*! 
  -----------------------------------------------------------------------------
 
  module: hls27_DEExec.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Functions for dataextract execution
                
  version:      7.6.
  -----------------------------------------------------------------------------
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

*/
#ifndef HLS27_DEEXEC
#define HLS27_DEEXEC

#include "gls00.h"
#include "hls31_odbc_Result.h"

#define CURSOR_FOR_SEL_LS27               _T("DECLARE %s CURSOR FOR SELECT %s")
#define CURSOR_FOR_SEL_STAR_LS27          _T("DECLARE %s CURSOR FOR SELECT * from \"%s\".\"%s\"")
#define DESCRIBE_CURSOR_FOR_SEL_STAR_LS27 _T("DESCRIBE %s ")
#define CURSOR_FOR_SEL_STAR_ORDER_LS27    _T("DECLARE %s CURSOR FOR SELECT * from \"%s\".\"%s\" ORDER BY %s")
#define PACKETCOUNT_BEFORE_COMMIT_LS27     2

/*!
  -----------------------------------------------------------------------------
  Chapter:  public functions
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------

  function:     ls27ExtractTableColumns

  -----------------------------------------------------------------------------
  
  description:  

  arguments:    pTransformationCmd  [IN]  - extract command structure
                pDatExtrRec  [IN]  - structure with all info on the running
                                     session and the database connected to
                ErrText      [OUT] - error text

                
  returnvalue:  errOK_els00          everything went fine
                errors

  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27ExportTableRows (MetaDataDef        *&pMetaDataDef,
                     tls00_DextractRec  *pDatExtrRec,
                     tsp00_Addr         ErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls27ExtractRowColumns

  -----------------------------------------------------------------------------
*/
tsp00_Int4 
ls27ExtractRowColumns (tls00_DBInfo            *pDBInfo,
                       MetaDataDef             *&pMetaDataDef,
                       tls00_DextractRec       *pDatExtrRec,
                       tsp00_Addr               ErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls27WriteMetaHeader

  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27WriteMetaHeader(tls00_DBInfo            *pDBInfo,
                    tls00_TransformationCmd *pTransformationCmd,
                    tls00_DextractRec       *pDatExtrRec,
                    tsp00_Addr              ErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls27WriteMetaTrailer

  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27WriteMetaTrailer(TransformationDef       *&pTransformationDef,
                     tls00_VFile              &VFile,
                     tls00_TransformationCmd *pTransformationCmd,
                     tls00_DextractRec       *pDatExtrRec,
                     tsp00_Addr              ErrText );

#endif //HLS27_DEEXEC