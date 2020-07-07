/*! 
  -----------------------------------------------------------------------------
  module: hls20.h
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

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

#ifndef HLS20
#define HLS20

#include "gls00.h"
#include "hls30.h"
#include "hls31_odbc_Result.h"

#define FILE_EXTENSION_SIZE_LS20      4

#define DBFUNC_NAME_OFFSET_LS20 3

// We expect not more that 2 packets for communication with the db
#define PACKET_COUNT_LS20    2

/*!
  -----------------------------------------------------------------------------
  Chapter:  public functions
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  function:     ImportPackageDataRecord
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
int
ImportPackageDataRecord( MetaDataDef *pMetaDataDef, tls00_DBInfo * DBInfo, tsp00_Addr ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20ImportTableRows
  -----------------------------------------------------------------------------
  description:  Executes  TABLELOAD TABLE RECORDS = 
                          DATALOAD  TABLE RECORDS .

  arguments:    pDBInfo     [IN]  - structure that contains all info on the
                                    running session and the database connected to.
                pTransformationRTE [IN]  - structure, holding information for executing
                                    DATALOAD for a single table
                ErrText    [OUT]  -  error text

  returnvalue:  errOK_els00       - no error
                errors from loading single table

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20ImportTableRows(MetaDataDef*&             pMetaDataDef,
                    tls00_TransformationRTE*  pTransformationRTE,
                    tsp00_Addr                ErrText) ; 


/*!
  -----------------------------------------------------------------------------
  function:     ls20InitDataSourceStruct
  -----------------------------------------------------------------------------
  description:  Initializes structure tls00_DataSource used to hold values read
                from stream (file, tape or pipe). Used for DATALOAD and DATAUPDATE
                commands.

  arguments:    pDataSource      [IN/OUT] - structure to initialize
                pFileFormat      [IN]     - contains info on code type of data file
                lFieldCount,     [IN]     - count of column to fill during load/update
                pMultColSpec     [IN]     - column structure 
                lSetColumnIndex  [IN/OUT] - index indicating the start of SET columns
                                             in a DATAUPDATE command; defaults to 0
  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls20InitDataSourceStruct(tls00_DataSource  *pDataSource,
                         tls00_FileFormat  *pFileFormat,
                         tsp00_Int4         lFieldCount,
                         tls00_MultColSpec *pMultColSpec,
                         tsp00_Int4         lSetColumnIndex);


/*!
  -----------------------------------------------------------------------------
  function:     ls20ConvertValue2InternEncoding
  -----------------------------------------------------------------------------
  description:  Converts data to ASCII encoding for further processing if needed.
                Conversion is for instance needed for time, date and timestamp
                data.

  arguments:    RawData     [IN/OUT]  - Data to convert
                DataType    [IN]      - DB data type of column
                CodeType    [IN]      - External encoding of data
                pszErrText  [OUT]     - Error text
  
  returnvalue:  errOK_els00                 - No error
                errInternal_els98           - Error converting between encodings
                errConvertingEncoding_els98 - Error converting between encodings
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls20ConvertValue2InternEncoding(tls00_String        &RawData,
                                tsp00_DataType_Enum  DataType,
                                tls00_CodeType       CodeType,
                                tsp00_Addr           pszErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls20ReadData
  -----------------------------------------------------------------------------
  description:  Reads the data from a data file according to given parameters.

  arguments:    pBuffer        [OUT] - buffer for read data
                pszErrText     [OUT] - error text
  
  returnvalue:  errOK_els00         - in case of NO errors
                errFileRead_ls98    - error reading file
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20ReadData(tls00_VFile*      pInfile,
             tls00_FileFormat* pFileFormat,
             tls00_Pattern*    pPattern,
             tls00_MetaData*   pMeta,
             tls00_Buffer2*    pBuffer,
             SAPDB_Bool*       pbTabLoaded,
             SAPDB_Bool        bFirstRecord,
             tsp00_Addr        pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_ConstructRTEStructure
  -----------------------------------------------------------------------------
  description:  initializes Transformation Runtime structure

  arguments:    pTransformationCmd
                pTransformationRTE [IN] - structure to construct
  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls20ConstructRTEStructure(tls00_TransformationRTE*&  pTransformationRTE,
						  tls00_DBInfo*              pDBInfo,
					      tls00_TransformationCmd*   pTransformationCmd);

void
ls20DestructRTEStructure(tls00_DBInfo*              pDBInfo,
						 tls00_TransformationCmd*   pTransformationCmd,
						 tls00_TransformationRTE*   pTransformationRTE);

/*!
  -----------------------------------------------------------------------------
  EndChapter:  public functions
  -----------------------------------------------------------------------------
*/

#endif /*HLS20*/
