/*



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
#ifndef VPA70SQLDA_H
#define VPA70SQLDA_H
/*!**********************************************************************

  module: vpa70SQLDA.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC related sqlda functions

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:           Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"


enum TAB_SQLTABLES {TATABLE_QUALIFIER=1, TATABLE_OWNER, 
                    TATABLE_NAME, TAABLE_TYPE, TAREMARKS};


enum TAB_SQLCOLUMNS {COTABLE_QUALIFIER=1, COTABLE_OWNER, COTABLE_NAME, 
	    COCOLUMN_NAME, CODATA_TYPE, COTYPE_NAME, COPRECISION, COLENGTH, 
	    COSCALE, CORADIX, CONULLABLE, COREMARKS, COPOS, COCODETYPE};

#define COSQL_DATA_TYPE  (14)

enum TAB_SQLSTAT {STTABLE_QUALIFIER=1, STTABLE_OWNER, STTABLE_NAME,
                  STNON_UNIQUE, STINDEX_QUALIFIER, STINDEX_NAME,
                  STTYPE, STSEQ_IN_INDEX, STCOLUMN_NAME, 
                  STCOLLATION, STCARDINALITY, STPAGES, STFILTER_CONDITION };

enum TAB_SQLSPEC { SPSCOPE=1, SPCOLUMN_NAME, SPDATA_TYPE, SPTYPE_NAME, 
                   SPPRECISION, SPLENGTH, SPSCALE, SPPSEUDO_COLUMN,
                   SPCODETYPE};

enum TAB_SQLPRIM { PRTABLE_QUALIFIER=1, PRTABLE_OWNER, PRTABLE_NAME,
                   PRCOLUMN_NAME, PRKEY_SEQ, PRPK_NAME };
	
enum TAB_SQLFOREIGN { PKTABLE_QUALIFIER=1, PKTABLE_OWNER, PKTABLE_NAME,
                      PKCOLUMN_NAME, FKTABLE_QUALIFIER, FKTABLE_OWNER,
                      FKTABLE_NAME, FKCOLUMN_NAME, FOKEY_SEQ,
                      FOUPDATE_RULE, FODELETE_RULE, FOFK_NAME, FOPK_NAME};


enum TAB_SQLTABLEPRIVILEGES { TPRTABLE_QUALIFIER=1, TPRTABLE_OWNER, 
                              TPRTABLE_NAME, TPRGRANTOR, TPRGRANTEE,
                              TPRPRIVILEGE, TPRIS_GRANTABLE}; 

enum TAB_SQLCOLUMNPRIVILEGES { CPRTABLE_QUALIFIER=1, CPRTABLE_OWNER, 
                               CPRTABLE_NAME, CPRCOLUMN_NAME, CPRGRANTOR,
                               CPRGRANTEE, 
                               CPRPRIVILEGE, CPRIS_GRANTABLE}; 


enum TAB_SQLTYPES {TYTYPE_NAME=1, TYDATA_TYPE, TYPRECISION, TYLITERAL_PREFIX,
                   TYLITERAL_SUFFIX, TYCREATE_PARAMS, TYNULLABLE,
                   TYCASE_SENSITIVE, TYSEARCHABLE, TYUNSIGNED_ATTRIBUTE,
                   TYMONEY, TYAUTO_INCREMENT, 
                   TYLOCAL_TYPE_NAME, TYMINIMUM_SCALE, TYMAXIMUM_SCALE};
	
enum TAB_SQLPROCEDURE { PROCEDURE_QUALIFIER=1, PROCEDURE_OWNER, 
                        PROCEDURE_NAME, PROCNUM_INPUT_PARAMS,
                        PROCNUM_OUTPUT_PARAMS, 
                        PROCNUM_RESULT_PARAMS, PROCREMARKS, PROCEDURE_TYPE};

enum TAB_SQLPROCEDURECOL { PROCCOL_QUALIFIER=1, PROCCOL_OWNER, PROCCOL_NAME, 
                           PCOLCOLUMN_NAME, PCOLCOLUMN_TYPE, PCOLDATA_TYPE,
                           PCOLTYPE_NAME, 
                           PCOLPRECISION, PCOLLENGTH, PCOLSCALE, PCOLRADIX,
                           PCOLNULLABLE, PCOLREMARKS, PCOLCODETYPE};

#define PCOLSQL_DATA_TYPE  (15)


/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa70AllocSQLDA

  description: Allocate sqlda array.

  arguments:
    phand        [out]  Buffer to return the new array handle
    num_elements [in]   The requested size of the array.
    
  return value: API_OK, API_NOT_OK
  */

API_RETCODE pa70AllocSQLDA( API_HANDLE *phand,
                            UWORD       num_elements );


/*!**********************************************************************
  Function: pa70FreeSQLDA

  description: Free memory for the sqlda array.

  arguments:
      hand  [in]  SQLDA handle
      
  return value: API_OK
  */

API_RETCODE pa70FreeSQLDA( API_HANDLE hand );


/*!**********************************************************************
  Function: pa70NumCol

  description: Return the number of columns in the sqlda structure.

  arguments:
    sqlda_ptr [in]  Pointer to sqlda-structure
    
  return value: The number of columns.
  */

UWORD pa70NumCol( sqldatype *sqlda_ptr );


/*!**********************************************************************
  Function: pa70NumParam

  description: Return the number of parameters in the sqlda structure.

  arguments:
    sqlda_ptr [in]  Pointer to sqlda-structure
      
  return value: The number of parameters.
  */

UWORD pa70NumParam( sqldatype *sqlda_ptr );


/*!**********************************************************************
  Function: pa70Patch

  description: Patch the column type and length for catalog functions.

  arguments:
    sqlda_ptr   [in]  Pointer to sqlda-structure
    number_cols [in]  Number of columns.
    special     [in]  Indicate special case (API_SPEC_...)
    odbcVersion [in]  SQL_OV_ODBC3 or SQL_OV_ODBC2. This is needed because
                      columns for catalog functions are different in
                      ODBC 3.x.
  return value: The number of parameters.
  */

API_RETCODE pa70patch( sqldatype *sqlda_ptr,
                       UWORD      number_cols,
                       UWORD      special,
                       UDWORD     odbcVersion );

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif
