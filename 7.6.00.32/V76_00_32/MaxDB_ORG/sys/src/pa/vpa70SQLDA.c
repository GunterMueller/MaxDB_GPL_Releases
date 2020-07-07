/*!**********************************************************************

  module: vpa70SQLDA.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC related sqlda functions

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa70SQLDA.h"


#ifndef SQLDASIZE
#define SQLDASIZE(n) (sizeof(sqldatype) + (n-sqlnmax)*sizeof(sqlvartype))
#endif

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static tsp00_Int2 pa70MapToVarType(tsp00_Int2 coltype);

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/


/* apmasda() - API miscellaneous function, allocate SQLDA */
API_RETCODE pa70AllocSQLDA( API_HANDLE *phand,
                            UWORD       num_elements )
{
    API_HANDLE hand;
    DWORD sqlda_size;
    API_RETCODE retcode;
    sqldatype FAR * sqlda_ptr;
    UCHAR FAR * sqlda_lit_ptr;
    UCHAR sqlda_lit [9];
  
    API_TRACE(API_TR_ENTRY,"pa70AllocSQLDA",0);
    API_TRACE(API_TR_PTR,"phand",&phand);
    API_TRACE(API_TR_UWORD,"num_elements",&num_elements);
  
    API_STRCPY (sqlda_lit, "SQLDA   ");
    sqlda_size = SQLDASIZE(num_elements);
    hand = apdallo( (DWORD) sqlda_size);
    if (hand == 0) {
        retcode = API_NOT_OK;
    }
    else {
        sqlda_ptr = (sqldatype FAR *) apdlock (hand);
        API_ASSERT_PTR(sqlda_ptr);
        sqlda_lit_ptr = (UCHAR FAR *) sqlda_lit;
        API_MEMCPY((UCHAR FAR *) sqlda_ptr -> sqldaid, sqlda_lit_ptr,
                   sizeof(sqlda_ptr -> sqldaid));
        sqlda_ptr -> sqlmax = num_elements;
        sqlda_ptr -> sqld = 0;
        apdunlk (hand);
        retcode = API_OK;
    }

    *phand = hand;

    API_TRACE(API_TR_EXIT,"pa70AllocSQLDA",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
    API_TRACE(API_TR_HANDLE,"*phand",phand);

    return(retcode);
} /* pa70AllocSQLDA */



/* apmfsda() - API miscellaneous function, free SQLDA */
API_RETCODE pa70FreeSQLDA( API_HANDLE hand )
{
    API_RETCODE retcode;
    API_TRACE(API_TR_MSG,"pa70FreeSQLDA",0);

    apdfree(hand);
    
    retcode = API_OK;
  
    return(retcode);
} /* pa70FreeSQLDA */


UWORD pa70NumCol( sqldatype *sqlda_ptr )
{
    UWORD cnt=0;
    SWORD i;
    
    API_TRACE(API_TR_ENTRY, "pa70NumCol", 0);
    API_TRACE(API_TR_UWORD, "sqln", &sqlda_ptr->sqln);
    for (i=0; i < sqlda_ptr->sqln;i++) {
        if ( sqlda_ptr->sqlvar[i].colio == CPR_INFO_OUTPUT || 
             sqlda_ptr->sqlvar[i].colio == CPR_INFO_INOUT)
            cnt++;
    }
    API_TRACE(API_TR_UWORD, "cnt", &cnt);
    API_TRACE(API_TR_EXIT, "pa70NumCol", 0);
    
    return (cnt);
} /* pa70NumCol */


UWORD pa70NumParam( sqldatype *sqlda_ptr )
{
   UWORD cnt=0;
   SWORD i;
   
   API_TRACE(API_TR_ENTRY, "pa70NumParam", 0);
   API_TRACE(API_TR_UWORD, "sqln", &sqlda_ptr->sqln);
   for (i=0; i < sqlda_ptr->sqln;i++) {
#ifdef PA07PASSTHROUGH     
      if (sqlda_ptr->sqlvar[i].colio == CPR_INFO_INPUT || 
	 sqlda_ptr->sqlvar[i].colio == CPR_INFO_INOUT)
#endif	    
	 cnt++;
   }
   API_TRACE(API_TR_UWORD, "cnt", &cnt);
   API_TRACE(API_TR_EXIT, "pa70NumParam", 0);
   
   return (cnt);
} /* pa70NumParam */



API_RETCODE pa70patch( sqldatype *sqlda_ptr,
                       UWORD      number_cols,
                       UWORD      special,
                       UDWORD     odbcVersion )
{
    UWORD i;
    API_RETCODE api_retcode;
    sqlvartype * sqlvar_ptr;

    API_TRACE(API_TR_ENTRY,"pa70patch",0);
    API_TRACE(API_TR_UWORD,"special", &special);
    api_retcode = API_OK;

    sqlvar_ptr = &sqlda_ptr -> sqlvar [0];     
    for (i=1; i <= number_cols; i++) {
        API_TRACE(API_TR_UWORD,"col",&i);
        switch (special) {
        case (API_SPEC_SQLTABLES): {
            switch (i) {
            case (TATABLE_NAME): {}
            case (TATABLE_OWNER): {}
            case (TAABLE_TYPE): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                break;
            }
            case (TATABLE_QUALIFIER): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (TAREMARKS): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = 254;
                break;
            }
            } /* switch col */
            break;
        }
        case (API_SPEC_SQLCOLUMNS): {
            switch (i) {       
            case (COTABLE_QUALIFIER): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (COTABLE_OWNER): {}
            case (COTABLE_NAME): {}
            case (COCOLUMN_NAME): {}
            case (COTYPE_NAME): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                if (i == COTYPE_NAME) {
                    sqlvar_ptr->collength = API_MAX_KEYWORD_LEN;
                }
                break;
            }
            case (COREMARKS): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = 254;
                break;
            }
            case (COSQL_DATA_TYPE): {
                if (odbcVersion == SQL_OV_ODBC3) {
                    sqlvar_ptr->coltype = dsmallint;
                };
                break;
            }
            case (CODATA_TYPE): {}
            case (COSCALE): {}
            case (CONULLABLE): {}
            case (CORADIX): {
                sqlvar_ptr->coltype = dsmallint;
                break;
            }
            case (COPRECISION): {}
            case (COLENGTH): {
                sqlvar_ptr->coltype = dinteger;
                break;
            }
            } /* switch col */
            break;
        }
        case (API_SPEC_SQLSTATISTICS): {
            switch (i) {
            case (STTABLE_OWNER): {}
            case (STTABLE_NAME): {}
            case (STCOLUMN_NAME): {}
            case (STINDEX_QUALIFIER): {}
            case (STINDEX_NAME): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                break;
            }
            case (STTABLE_QUALIFIER): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (STFILTER_CONDITION): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = 128;
                break;
            }
            case (STNON_UNIQUE): {}
            case (STSEQ_IN_INDEX): {}
            case (STTYPE): {
                sqlvar_ptr->coltype = dsmallint;
                break;
            }          
            case (STCARDINALITY): {}
            case (STPAGES): {
                sqlvar_ptr->coltype = dinteger;
                break;
            }          
            } /* switch col */
            break;
        }
        case (API_SPEC_SQLSPECIALCOLUMNS): {
            switch (i) {
            case (SPSCOPE): {}
            case (SPDATA_TYPE): {}
            case (SPSCALE): {}
            case (SPPSEUDO_COLUMN): {
                sqlvar_ptr->coltype = dsmallint;
                break;
            }
            case (SPPRECISION): {}
            case (SPLENGTH): {
                sqlvar_ptr->coltype = dinteger;
                break;
            }
            case (SPCOLUMN_NAME): {}
            case (SPTYPE_NAME): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = 128;
                break;
            }
            }
            break;
        }
        case (API_SPEC_SQLPRIMARYKEYS): {
            switch (i) {       
            case (PRTABLE_QUALIFIER): {}
            case (PRPK_NAME): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (PRTABLE_OWNER): {}
            case (PRTABLE_NAME): {}
            case (PRCOLUMN_NAME): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                break;
            }
            case (PRKEY_SEQ): {
                sqlvar_ptr->coltype = dsmallint;
                break;
            }
            } /* switch col */
            break;
        }       
        case (API_SPEC_SQLFOREIGNKEYS): {
            switch (i) {          
            case (PKTABLE_QUALIFIER): {}
            case (FKTABLE_QUALIFIER): {}
            case (FOPK_NAME): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (PKTABLE_OWNER): {}
            case (PKTABLE_NAME): {}
            case (PKCOLUMN_NAME): {}
            case (FKTABLE_OWNER): {}
            case (FKTABLE_NAME): {}
            case (FKCOLUMN_NAME): {}
            case (FOFK_NAME): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                break;
            }
            case (FOKEY_SEQ): {}
            case (FODELETE_RULE): {}
            case (FOUPDATE_RULE): {
                sqlvar_ptr->coltype = dsmallint;
                break;
            }
            } /* switch col */
            break;
        }
        case (API_SPEC_SQLPROCEDURES): {
            switch (i) {          
            case (PROCEDURE_QUALIFIER): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (PROCEDURE_OWNER): {}
            case (PROCEDURE_NAME): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                break;
            }
            case (PROCNUM_INPUT_PARAMS): {}
            case (PROCNUM_OUTPUT_PARAMS): {}
            case (PROCNUM_RESULT_PARAMS): {}
            case (PROCEDURE_TYPE): {   
                sqlvar_ptr->coltype = dsmallint;
                break;
            }
            case (PROCREMARKS): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = 254;
                break;
            }
            } /* switch col */
            break;
        }
        case (API_SPEC_SQLPROCEDURESCOLUMNS): {
            switch (i) {          
            case (PROCCOL_QUALIFIER): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (PROCCOL_OWNER): {}
            case (PROCCOL_NAME): {}
            case (PCOLCOLUMN_NAME): {}
            case (PCOLTYPE_NAME): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                break;
            }
            case (PCOLSQL_DATA_TYPE): {
                if (odbcVersion == SQL_OV_ODBC3) {
                    sqlvar_ptr->coltype = dsmallint;
                };
                break;
            }
            case (PCOLCOLUMN_TYPE): {}
            case (PCOLDATA_TYPE): {}   
            case (PCOLSCALE): {}
            case (PCOLRADIX): {}
            case (PCOLNULLABLE): {   
                sqlvar_ptr->coltype = dsmallint;
                break;
            }
            case (PCOLPRECISION): {}
            case (PCOLLENGTH): {   
                sqlvar_ptr->coltype = dinteger;
                break;
            }
            case (PCOLREMARKS): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = 254;
                break;
            }
            } /* switch col */
            break;
        }
        case (API_SPEC_SQLCOLUMNPRIVILEGES): {
            switch (i) {          
            case (CPRTABLE_QUALIFIER): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (CPRTABLE_OWNER): {}
            case (CPRTABLE_NAME): {}
            case (CPRCOLUMN_NAME): {}
            case (CPRGRANTOR): {}
            case (CPRGRANTEE): {}
            case (CPRPRIVILEGE): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                break;
            }
            case (CPRIS_GRANTABLE): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                sqlvar_ptr->collength = 3;
                break;
            }
            } /* switch col */
            break;
        }
        case (API_SPEC_SQLTABLEPRIVILEGES): {
            switch (i) {          
            case (TPRTABLE_QUALIFIER): {
                sqlvar_ptr->coltype = dvarchara;
                sqlvar_ptr->collength = API_TABLE_NAME_MAX;
                break;
            }
            case (TPRTABLE_OWNER): {}
            case (TPRTABLE_NAME): {}
            case (TPRGRANTOR): {}
            case (TPRGRANTEE): {}
            case (TPRPRIVILEGE): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                break;
            }
            case (TPRIS_GRANTABLE): {
                sqlvar_ptr->coltype = pa70MapToVarType(sqlvar_ptr->coltype);
                sqlvar_ptr->collength = 3;
                break;
            }
            } /* switch col */
            break;
        }
        default: {
            break;
        }
        } /* switch special */  
        API_TRACE(API_TR_SWORD, "coltype", &sqlvar_ptr->coltype);
        sqlvar_ptr++;
    } /* of for */
    API_TRACE(API_TR_EXIT,"pa70patch",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);

    return(api_retcode);
} /* pa70patch */

static tsp00_Int2 pa70MapToVarType(tsp00_Int2 coltype)
{
  return ((coltype == dcha) ? dvarchara : (coltype == dunicode) ? dvarcharuni : coltype);
}

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/
