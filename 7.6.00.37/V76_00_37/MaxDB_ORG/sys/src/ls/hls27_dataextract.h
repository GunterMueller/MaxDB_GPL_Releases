/*! 
  -----------------------------------------------------------------------------
 
  module: hls27_dataextract.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Functions for dataextract
                
  version:      7.5.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1999-2005 SAP AG-2001
 
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

#ifndef HLS27_DATAEXTRACT
#define HLS27_DATAEXTRACT

// includes
#include "gls00.h"
#include "hls31_odbc_Result.h"

// defines

#define SEL_DB_VERSION_LS27         _T("SELECT KERNEL FROM VERSIONS")

#define MAX_MEDIUM_SIZE_IN_BYTES_LS27  (1024*1024*1024)    // 1 GB in Byte so far !

#define FILE_EXTENSION_SIZE_LS27     4
#define FILECOUNT_MXLS27             9999

/*!
  -----------------------------------------------------------------------------
 
  function:     ExportPackageDataRecord

  -----------------------------------------------------------------------------

  description:  Dataextract main function

  description:  

  arguments:    pTransformationCmd  [IN]  - extract command structure
                pDatExtrRec  [IN]  - structure with all info on the running
                                     session and the database connected to
                ErrText      [OUT] - error text

                
  returnvalue:  errOK_els00           everything went fine
                errors

  -----------------------------------------------------------------------------
*/
tsp00_Int4
ExportPackageDataRecord(MetaDataDef *pMetaDataDef, tls00_DBInfo *DBInfo, tsp00_Addr ErrText); 

/*!
  -----------------------------------------------------------------------------
  function:     ls27ExportPackageTables
  -----------------------------------------------------------------------------

  description:  Executes TABLEEXTRACT ALL RECORDS  =
                         DATAEXTRACT  ALL RECORDS  .

  arguments:    pDatExtrRec [IN]  - structure, holding information for executing
                                    DATAEXTRACT for a single table
                ErrText    [OUT]  - error text

  returnvalue:  errOK_els00       - no error
                errors from extracting a single table

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27ExportPackageTables( MetaDataDef      *&pMetaDataDef,
                         tls00_DextractRec *pDatExtrRec,
                         tsp00_Addr         ErrText);

/*!
  -----------------------------------------------------------------------------

  function:     ls27UpdateTransformationPackage

  -----------------------------------------------------------------------------

  description:  Writes restart entry to restart table
                Used only for dataextract all/user/table

  arguments:    tls00_DextractRec  [IN]  - contains restart entry to write
                pszErrText         [OUT] - error text

  returnvalue:  errOK_els00                     - no error
                errors from writing to file

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27UpdateTransformationPackage( MetaDataDef              *&pMetaDataDef,
					             tls00_TransformationCmd  *pTransformationCmd,
				                 tls00_DextractRec        *pDatExtrRec,
                                 tsp00_Addr               ErrText);

#endif //HLS27_DATAEXTRACT