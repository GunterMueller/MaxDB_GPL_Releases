/*! 
  -----------------------------------------------------------------------------
 
  module: hls30_catalogextract.h
 
  -----------------------------------------------------------------------------

  responsible:  d033893
  
  special area: SAP DB LOADER

  description:  catalogextract

  version:      7.6.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1997-2005 SAP AG-2000
 
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

#ifndef HLS31
#define HLS31

// Save Catalog

// Catalog Management
#include "gls00.h"

/*!
  -----------------------------------------------------------------------------

  function:     ExportCatalogSchemaMap

  -----------------------------------------------------------------------------

  description:  ExportCatalogSchemaMap main function

  arguments:    pDBInfo     [IN]    - A pointer to a structure that contains all 
                                      information on the running session and the
                                      database connected to.
				pLoaderCmd[IN]    
				                    - A pointer to a structure that contains all
                                      information on the catalogextract/catalogextract
									  command givenby the user
                errText     [OUT]   - A pointer to a buffer where a description is
                                      available in case of errors
  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors

  -----------------------------------------------------------------------------
*/

SAPDB_Int4 ExportCatalogSchemaMap ( MetaDataDef   *&pMetaDataDef,
							        tls00_DBInfo  *pDBInfo,
							        tsp00_Addr    ErrText); 


/*!
  -----------------------------------------------------------------------------

  function:     ImportCatalogSchemaMap

  -----------------------------------------------------------------------------

  description:  ImportCatalogSchemaMap main function

  arguments:    pDBInfo     [IN]    - A pointer to a structure that contains all 
                                      information on the running session and the
                                      database connected to.
				pLoaderCmd[IN]    
				                    - A pointer to a structure that contains all
                                      information on the catalogextract/catalogextract
									  command givenby the user
                errText     [OUT]   - A pointer to a buffer where a description is
                                      available in case of errors
  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors

  -----------------------------------------------------------------------------
*/

SAPDB_Int4 ImportCatalogSchemaMap ( MetaDataDef   *&pMetaDataDef,
							        tls00_DBInfo  *pDBInfo,
							        tsp00_Addr    ErrText); 

SAPDB_Int4 UseCatalogSchemaMap    ( MetaDataDef   *&pMetaDataDef,
							        tls00_DBInfo  *pDBInfo,
							        tsp00_Addr    ErrText); 

/*!
  -----------------------------------------------------------------------------

  function:     TransportPackageCatalog

  -----------------------------------------------------------------------------

  description:  CatalogExtract main function

  arguments:    pDBInfo     [IN]    - A pointer to a structure that contains all 
                                      information on the running session and the
                                      database connected to.
				pLoaderCmd[IN]    
				                    - A pointer to a structure that contains all
                                      information on the catalogextract/catalogextract
									  command givenby the user
                errText     [OUT]   - A pointer to a buffer where a description is
                                      available in case of errors
  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors

  -----------------------------------------------------------------------------
*/

SAPDB_Int4 TransportPackageCatalog ( MetaDataDef   *&pMetaDataDef,
							         tls00_DBInfo  *pDBInfo,
								     tsp00_Addr    ErrText); 


#endif /*HLS31*/

/*===========================================================================*
 *     END
 *===========================================================================*/  