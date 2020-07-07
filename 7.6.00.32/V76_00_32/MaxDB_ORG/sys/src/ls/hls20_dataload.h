/*! 
  -----------------------------------------------------------------------------
  module: hls20_dataload.h
  -----------------------------------------------------------------------------
 
  responsible:  d033893

  special area: SAP DB LOADER
 
  description:  dataload
                
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

#ifndef HLS20_DATALOAD
#define HLS20_DATALOAD

#include "gls00.h"
#include "hls31_odbc_Result.h"

// defines

#define FILE_EXTENSION_SIZE_LS20     4

/*!
  -----------------------------------------------------------------------------
  function:     ls20ImportPackageDataRecord
  -----------------------------------------------------------------------------

  description:  dataload main function

  arguments:    DloadCmd    [IN]    - structure with user information for
                                      processing dataload command
                DBInfo      [IN]    - structure with info on running session and
                                      database connected to
                ErrText     [OUT]   - error text
  
  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ImportPackageDataRecord(MetaDataDef  *pMetaDataDef,
				        tls00_DBInfo *pDBInfo, 
				        tsp00_Addr    ErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls20ImportPackageTables
  -----------------------------------------------------------------------------

  description:  Executes TABLELOAD ALL RECORDS  =
                         DATALOAD  ALL RECORDS  .

  arguments:    pTransformationCmd [IN]  - dataload command structure
                pTransformationRTE [IN]  - structure, holding information for executing
                                    DATALOAD for a single table
                ErrText [OUT]     - error text

  returnvalue:  errOK_els00       - no error
                errors from loading single table

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20ImportPackageTables(MetaDataDef*&             pMetaDataDef,
                        tls00_TransformationRTE*  pTransformationRTE,
                        tsp00_Addr                ErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls20UpdateTransformationPackage
  -----------------------------------------------------------------------------

  description:  Writes restart entry to restart table
                Used only for dataload all/user/table

  arguments:    pTransformationRTE  [IN]  - contains restart entry to write
                pszErrText   [OUT] - error text

  returnvalue:  errOK_els00                     - no error
                errors from writing to file

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20UpdateTransformationPackage( tls00_DBInfo*			   pDBInfo,
							     MetaDataDef*&			   pMetaDataDef,
					             tls00_TransformationCmd*  pTransformationCmd,
				                 tls00_TransformationRTE*  pTransformationRTE,
                                 tsp00_Addr                ErrText);

 
#endif // HLS20_DATALOAD
