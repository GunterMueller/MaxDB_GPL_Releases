/*! 
  -----------------------------------------------------------------------------
 
  module: hls26.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Definition of exported functions for TABLEEXTRACT
                
  version:      7.5.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1999-2005 SAP AG-2003
 
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
#ifndef HLS26
#define HLS26

// includes
#include "gls00.h"
#include "hls31_odbc_Result.h"

// defines
#define MAX_MEDIUM_SIZE_IN_BYTES_LS26    (1024*1024*1024)    // 1 GB in Byte so far !
/*!
  -----------------------------------------------------------------------------
 
  function:     ExportPackageData

  -----------------------------------------------------------------------------

  description:  main function for table extract execution
  
                This function does the first things that are necessary to extract
                a table. It then starts the data export.

  arguments:    *TECmd   [IN]  - table extract cmd structure;  contains all
                                  necessary information for execution.
                *DBInfo  [IN]  - structure with all information on the
                                  running session and the db connected to.
                *ErrText [OUT] - error text

  returnvalue:  errOK_els00    - everything went fine
                errors of the called functions, error lyrics can be found in log file.
  -----------------------------------------------------------------------------
*/

tsp00_Int4
ExportPackageData(MetaDataDef  *pMetaDataDef, 
					 tls00_DBInfo *pDBInfo, 
					 tsp00_Addr   ErrText); 

/*
  -----------------------------------------------------------------------------
  function: ls26ExportPackageTables
  -----------------------------------------------------------------------------

  description:  Main function for extracting tables.

  arguments:    pTabExtrRec  [IN] - tableextract record structure
                ErrText      [IN] - error text

  returnvalue:  errOK_els00                    - no error
                errors extracting tables
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26ExportPackageTables(MetaDataDef       *&pMetaDataDef,
                         tls00_TextractRec *pTabExtrRec,
                         tsp00_Addr         ErrText);

/*!
  -----------------------------------------------------------------------------

  function:     ls26UpdateTransformationPackage

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
ls26UpdateTransformationPackage( tls00_DBInfo      *pDBInfo,
                                 MetaDataDef       *&pDBDef,
                                 tls00_TransformationCmd  *pTransformationCmd,
                                 tls00_TextractRec *pTabExtrRec,
                                 tsp00_Addr        ErrText);

#endif /*HLS26*/