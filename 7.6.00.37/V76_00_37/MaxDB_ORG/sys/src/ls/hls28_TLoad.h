/*! 
  -----------------------------------------------------------------------------
 
  module: hls28_TLoad.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Definition of exported functions for TABLELOAD
                
  version:      7.5.

  -----------------------------------------------------------------------------
 
  Copyright (c) 2000-2005 SAP AG-2001
 
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
#ifndef HLS28_TLOAD
#define HLS28_TLOAD

#include "gls00.h"
#include "hls51.h"

// defines

// restart entry looks like :
// "<USER NAME> (=tsp00_KnlIdentifierc)"."<TABLE NAME> (=tsp00_KnlIdentifier" <BEGIN BLOCKS>:<BEGIN POS>-<END BLOCKS>:<END POS>
// blocks and pos are 10 digit (max) numbers
const tsp00_Int4 RestartEntryLen_ls28 = 2 * sizeof(tsp00_KnlIdentifier) + 64;

/*!
  -----------------------------------------------------------------------------
 
  function:     ImportPackageData 

  -----------------------------------------------------------------------------

  description:  start the execution of table load
  
                This function does the first things that are necessary to load
                a table that was extracted using TABLE EXTRACT.
                It then starts the execution of data load.

  arguments:    pDatLoadCmd[IN]  - pointer; tableload command structure that
                                    contains all necessary information for execution.
                DBInfo     [IN]  - pointer; structure that contains all information
                                    on the running session and the database
                                    connected to.
                ErrText    [OUT] - pointer; buffer taking the error description
                                    in case of errors.

  returnvalue:  errOK_els00     - everything went fine
                errors of the called functions, error texts can be found in log file.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ImportPackageData(MetaDataDef *pMetaDataDef, tls00_DBInfo *DBInfo, tsp00_Addr  ErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls28ImportPackageTables
  -----------------------------------------------------------------------------

  description:  Executes TABLELOAD ALL command.

  arguments:    pTabLoadRec     [IN]  - structure, holding information for executing
                                        TABLELOAD for a single table
                ErrText [OUT] - error text

  returnvalue:  errOK_els00                     - no error
                errors from loading single table

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls28ImportPackageTables(MetaDataDef    *&pMetaDataDef,
                      tls00_TloadRec *pTabLoadRec,
                      tsp00_Addr      ErrText);

tsp00_Int4
ls28UpdateTransformationPackage( MetaDataDef             *&pMetaDataDef,
					             tls00_TransformationCmd *pDatLoadCmd,
					             tls00_TloadRec          *pTabLoadRec, 
								 ReadBuffer              &readBuffer,
					             tsp00_Addr              ErrText);


void
ls28FreeTransformationPackage( MetaDataDef *&pMetaDataDef );

#endif


/*===========================================================================*
 *     END
 *===========================================================================*/
