/*!**********************************************************************

  module: vpa21DescRec.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC-Descriptor-Records - Datastructures and functions

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

#include "vpa21DescRec.h"
#include "vpa20Desc.h"

/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/

/* returns TRUE, if sqlmode is ora and length is 0 for variable length data
   cmp: insert into ... values ('', x'') in ora mode  PTS 1122478
   http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122478 */
int pa21_isOraNULL (UWORD sqlmode, tpa21DescRecord *appl_desc_rec_ptr)
{
  return  sqlmode == CPR_KIND_ORACLE  &&  *PA20_OCTETLENGTH_PTR(appl_desc_rec_ptr) == 0
            &&  (appl_desc_rec_ptr->type == SQL_C_CHAR  ||  appl_desc_rec_ptr->type == SQL_C_BINARY);
}                  


/*!**********************************************************************

  Chapter: exported functions

************************************************************************/


API_RETCODE pa21GetLongAddr( UCHAR           **varaddr,
                             tpa21DescRecord  *impl_rec_ptr )
{
    API_RETCODE retcode = API_NOT_OK;
  
    API_TRACE(API_TR_ENTRY, "pa21GetLongAddr", 0);

    API_ASSERT_PTR( impl_rec_ptr );
    API_ASSERT_PTR( impl_rec_ptr->longHndl );
  
    if ( impl_rec_ptr->longHndl != NULL ) {
        *varaddr = apdlock( impl_rec_ptr->longHndl );
        if (varaddr)  
            retcode = API_OK;
    }; /* if */

    API_TRACE(API_TR_EXIT, "pa21GetLongAddr", 0);
    API_TRACE(API_TR_PTR, "varaddr", varaddr);
    API_TRACE(API_TR_API_RETCODE, "retcode", &retcode);

    return (retcode);
} /* pa21GetLongAddr */


void pa21InitDescRecord( tpa21DescRecord *recPtr,
                         DescPtr          parentDesc )
{
    API_ASSERT_PTR( recPtr );
    API_ASSERT_PTR( parentDesc );
    recPtr->parentDesc      = parentDesc;
    recPtr->conciseType     = SQL_C_DEFAULT;
    recPtr->dataPtr         = NULL;
    recPtr->datetimeIntervalCode = 0;
    recPtr->datetimeIntervalPrecision = 0;
    recPtr->indicatorPtr    = NULL;    
    recPtr->length          = 0;
    recPtr->name            = NULL;
    recPtr->nullable        = 0;
    recPtr->octetLengthPtr  = NULL;
    recPtr->parameterType   = SQL_PARAM_INPUT; /* only used by IPD´s */
    recPtr->precision       = 0;
    recPtr->type            = SQL_C_DEFAULT;
    recPtr->conciseType     = SQL_C_DEFAULT;
    recPtr->sqlType         = SQL_UNKNOWN_TYPE;
    recPtr->numPrecRadix    = 0;
    
    /* internal fields */
    recPtr->longHndl        = API_NULL_HANDLE;    
    recPtr->lastData        = 0;
    recPtr->optimizePtr     = NULL;
    recPtr->movedLen        = 0;
    recPtr->optimizeNullPtr = NULL;
    recPtr->bound           = API_FALSE;
    recPtr->alreadyRead     = 0;
} /* pa21InitDescRecord */

/* put fields from user fields to sqlda areas */
API_RETCODE pa21PutRec( tpa21DescRecord *appl_desc_rec_ptr,
                        tpa21DescRecord *impl_desc_rec_ptr,
                        UWORD            sqlmode )
{
    API_RETCODE conv_retcode = API_OK;
    UDWORD      column_definition;
    tsp00_Int4  indicator = 0;

    API_TRACE( API_TR_ENTRY, "pa21PutRec", 0);
    API_TRACE( API_TR_PTR, "appl_desc_rec_ptr", &appl_desc_rec_ptr);
    API_TRACE( API_TR_PTR, "impl_desc_rec_ptr", &impl_desc_rec_ptr);

    API_ASSERT_PTR( appl_desc_rec_ptr );
    API_ASSERT_PTR( impl_desc_rec_ptr );
    
    conv_retcode = API_OK;
    
    API_TRACE( API_TR_SDWORD, "octetLengthPtr",
               PA20_OCTETLENGTH_PTR(appl_desc_rec_ptr) );
    impl_desc_rec_ptr -> isNull = API_FALSE;
    /* API_NEED_DATA for all data types admitted (cmp o3lparam). It's like MSSQL.
       Actually SQL_NEED_DATA doesn't make a lot sense for types like SQL_C_LONG etc.
       but doesn't harm, too.
       However, if an uninitialized value is passed here, SQL_NEED_DATA will be
       reported in SQLExecute, if the pointer contains a negative value by accident. */
    if (PA20_OCTETLENGTH_PTR(appl_desc_rec_ptr) != NULL) {
        if (*PA20_OCTETLENGTH_PTR(appl_desc_rec_ptr) == SQL_DATA_AT_EXEC ||
            *PA20_OCTETLENGTH_PTR(appl_desc_rec_ptr) <= SQL_LEN_DATA_AT_EXEC_OFFSET) {
            conv_retcode = API_NEED_DATA;
            API_TRACE( API_TR_MSG, "need data", 0);
        }
        else {
            conv_retcode = API_OK;
            if (impl_desc_rec_ptr->defaultValue &&
            	*PA20_OCTETLENGTH_PTR(appl_desc_rec_ptr) == SQL_DEFAULT_PARAM)
            	indicator = (tsp00_Int4) CPR_DEFAULT_PARAM;   /* PTS 1118562 */
            else
            if (impl_desc_rec_ptr -> nullable == SQL_NULLABLE)        {
                API_TRACE(API_TR_SWORD, "indicator", &indicator);
                /* is it NULL? */
                /* special handling for ora mode, PTS 1122478
                   http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122478
                   http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122725 
                   http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
                if (appl_desc_rec_ptr->indicatorPtr != NULL  &&
                    (*PA20_INDICATOR_PTR(appl_desc_rec_ptr) == SQL_NULL_DATA
                    || pa21_isOraNULL (sqlmode, appl_desc_rec_ptr))) {
                    indicator = (tsp00_Int4) SQL_NULL_DATA;
                    impl_desc_rec_ptr -> isNull = API_TRUE;
                }
            } 
            else 
                if (appl_desc_rec_ptr->indicatorPtr != NULL  &&
                    (*PA20_INDICATOR_PTR(appl_desc_rec_ptr) == SQL_NULL_DATA
                    || pa21_isOraNULL (sqlmode, appl_desc_rec_ptr))) {
                    conv_retcode = API_CONSTRAINT_VIOLATION;
                }		
        }
    }
    /* real data?             */
    if(impl_desc_rec_ptr -> optimizeNullPtr)
        API_MEMCPY( impl_desc_rec_ptr -> optimizeNullPtr,
                    &indicator,
                    sizeof(indicator));
    if ( impl_desc_rec_ptr -> isNull == API_FALSE
         && conv_retcode == API_OK) { 
        column_definition = impl_desc_rec_ptr -> esqLength;
        conv_retcode = apgctoe( impl_desc_rec_ptr -> esqType,
                                (UCHAR*) impl_desc_rec_ptr -> optimizePtr,
                                column_definition,
                                impl_desc_rec_ptr -> esqScale,
                                &(impl_desc_rec_ptr -> movedLen),
                                appl_desc_rec_ptr -> conciseType,
                                (UCHAR*) PA20_DATA_PTR( appl_desc_rec_ptr ),
                                appl_desc_rec_ptr -> precision,
                                appl_desc_rec_ptr -> scale,
                                PA20_OCTETLENGTH_PTR(appl_desc_rec_ptr),
                                impl_desc_rec_ptr -> conciseType );
        impl_desc_rec_ptr -> convRetcode = conv_retcode;
    } /* of if isNull */
        
    if ( conv_retcode == API_TRUNCATE
         && apgislong( impl_desc_rec_ptr -> type))
        conv_retcode = API_OK;

    API_TRACE(API_TR_EXIT,"pa21PutRec",0);
    API_TRACE(API_TR_API_RETCODE,"conv_retcode",&conv_retcode);
    
    return (conv_retcode);
} /* pa21PutRec */


VOID pa21SetCType( tpa21DescRecord *applRecPtr, SWORD cType )
{
    API_ASSERT_PTR( applRecPtr );

    switch (cType) {
    case SQL_C_ABAPHANDLE:
        applRecPtr->conciseType = cType;
        applRecPtr->type = cType;
        break;
    case SQL_C_CHAR:
        applRecPtr->conciseType = cType;
        applRecPtr->type = cType;
        /* implcitely set length and prec (see ODBC reference 3.0 p. 934) */
        applRecPtr->length = 1;
        applRecPtr->precision = 0;
        break;
    case SQL_DECIMAL:
        applRecPtr->conciseType = cType;
        applRecPtr->type = cType;
        applRecPtr->scale = 0;
        applRecPtr->precision = API_SQL_DECIMAL_PRECISION;
        break;
    case SQL_NUMERIC:
        applRecPtr->conciseType = cType;
        applRecPtr->type = cType;
        applRecPtr->scale = 0;
        applRecPtr->precision = API_SQL_NUMERIC_PRECISION;
        break;
    case SQL_FLOAT:
        applRecPtr->conciseType = cType;
        applRecPtr->type = cType;
        applRecPtr->precision = API_SQL_FLOAT_PRECISION;
        break;
    case SQL_C_INTERVAL_MONTH:
    case SQL_C_INTERVAL_YEAR:
    case SQL_C_INTERVAL_YEAR_TO_MONTH:
    case SQL_C_INTERVAL_DAY:
    case SQL_C_INTERVAL_HOUR:
    case SQL_C_INTERVAL_MINUTE:
    case SQL_C_INTERVAL_SECOND:
    case SQL_C_INTERVAL_DAY_TO_HOUR:
    case SQL_C_INTERVAL_DAY_TO_MINUTE:
    case SQL_C_INTERVAL_DAY_TO_SECOND:
    case SQL_C_INTERVAL_HOUR_TO_MINUTE:
    case SQL_C_INTERVAL_HOUR_TO_SECOND:
    case SQL_C_INTERVAL_MINUTE_TO_SECOND:
        /* we dont support these */
        break;
        /* always use ->type
         * difference between type and conciseType is handled in
         * pa20GetType, pa20GetDateTimeIntervallCode
    case SQL_C_DATE:
    case SQL_C_TYPE_DATE:
        applRecPtr->conciseType = cType;
        applRecPtr->type = SQL_DATETIME;
        applRecPtr->datetimeIntervalCode = SQL_CODE_DATE;
        break;
    case SQL_C_TIME:
    case SQL_C_TYPE_TIME:
        applRecPtr->conciseType = cType;
        applRecPtr->type = SQL_DATETIME;
        applRecPtr->datetimeIntervalCode = SQL_CODE_TIME;
        break;
    case SQL_C_TIMESTAMP:
    case SQL_C_TYPE_TIMESTAMP:
        applRecPtr->conciseType = cType;
        applRecPtr->type = SQL_DATETIME;
        applRecPtr->datetimeIntervalCode = SQL_CODE_TIMESTAMP;
        break;
        */
    default:
        applRecPtr->conciseType = cType;
        applRecPtr->type = cType;
    }; /* switch */
} /* pa21SetCType */


/* set the internal hostvartype use ODBC Types */
API_RETCODE pa21SetHostVarType( tpa21DescRecord  *impl_rec_ptr,
                                 SWORD           *hostvartype,
                                 UDWORD          *collength)
{
    API_RETCODE api_retcode;
    SWORD sql_type;
  
    API_TRACE( API_TR_ENTRY,"pa21SetHostVarType", 0);
    API_TRACE( API_TR_SWORD,"type", &impl_rec_ptr->type);
  
    api_retcode = API_OK;
    sql_type = impl_rec_ptr->type;
    *collength = impl_rec_ptr -> esqLength;
    if (apgisvstr(sql_type)) 
        *collength += API_VSTRING_OFFSET;
    switch (sql_type) {
    case (SQL_ABAPHANDLE): {
      *hostvartype = (SWORD) CPR_VABAPHANDLE;
      break;
    }
    case (SQL_CHAR): {}
    case (SQL_VARCHAR): {}                 /* PTS 1121248 */
    case (SQL_LONGVARCHAR): {
        *hostvartype = (SWORD) ((apgisvstr(sql_type)) ?
                                CPR_VSTRING4 : CPR_VCHARC);
        break;
    }
    case (SQL_UNICODE): {}
    case (SQL_UNICODE_VARCHAR): {}
    case (SQL_UNICODE_LONGVARCHAR): {
        *hostvartype = (SWORD) ((apgisvstr(sql_type)) ?
                                CPR_VSTRINGUNICODE4 : CPR_VUNICODEC);
        break;
    }
    case (SQL_TYPE_DATE): {}
    case (SQL_TYPE_TIME): {}
    case (SQL_TYPE_TIMESTAMP): {}
    case (SQL_DATE): {}
    case (SQL_TIME): {}
    case (SQL_TIMESTAMP): {
        *hostvartype = (SWORD) ((apgisvstr(sql_type)) ?
                                CPR_VSTRING4 : CPR_VCHAR);
        break;
    }
    case (SQL_VARBINARY): {}
    case (SQL_BINARY): {}
    case (SQL_LONGVARBINARY): {
        *hostvartype = (SWORD) ((apgisvstr(sql_type)) ?
                                CPR_VSTRING4 : CPR_VBCHAR);
        break;
    }
    case (SQL_SMALLINT): {
        *hostvartype = CPR_VINT2;
        break;
    }
    case (SQL_INTEGER): {
        *hostvartype = CPR_VINT4;
        break;
    }
    case (SQL_REAL): {
        *hostvartype = CPR_VREAL4;
        *collength = API_REAL_PREC;
        break;
    }
    case (SQL_FLOAT): {}
    case (SQL_DOUBLE): {
        *hostvartype = CPR_VREAL8;
        *collength = API_FLOAT_PREC;
        break;
    }
    case (SQL_NUMERIC): {}
    case (SQL_DECIMAL): {	 
        *hostvartype = CPR_VCHARC;
        break;
    }
    case (SQL_BIT): {	 
        *hostvartype = CPR_VCHARC;
        break;
    }
    default: {
        API_TRACE(API_TR_SWORD,"** unknown ** sql_type", &sql_type);
        api_retcode = API_NOT_OK;
        break;
    }
    } /* of switch */
    API_TRACE(API_TR_SWORD,"hostvartype", hostvartype);
    API_TRACE(API_TR_SDWORD,"collength", collength);
    API_TRACE(API_TR_EXIT,"pa21SetHostVarType", 0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode", &api_retcode);

    return(api_retcode);

} /* pa21SetHostVarType */


VOID pa21SetSQLType( tpa21DescRecord *implRecPtr, SWORD sqlType )
{
    API_ASSERT_PTR( implRecPtr );

    switch (sqlType) {
    case SQL_CHAR:
    case SQL_VARCHAR:
        implRecPtr->conciseType = sqlType;
        implRecPtr->sqlType = sqlType;
        /* implcitely set length and prec (see ODBC reference 3.0 p. 934) */
        implRecPtr->length = 1;
        implRecPtr->precision = 0;
        break;
    case SQL_DECIMAL:
        implRecPtr->conciseType = sqlType;
        implRecPtr->sqlType = sqlType;
        implRecPtr->scale = 0;
        implRecPtr->precision = API_SQL_DECIMAL_PRECISION;
        break;
    case SQL_NUMERIC:
        implRecPtr->conciseType = sqlType;
        implRecPtr->sqlType = sqlType;
        implRecPtr->scale = 0;
        implRecPtr->precision = API_SQL_NUMERIC_PRECISION;
        break;
    case SQL_FLOAT:
        implRecPtr->conciseType = sqlType;
        implRecPtr->sqlType = sqlType;
        implRecPtr->precision = API_SQL_FLOAT_PRECISION;
        break;
    case SQL_INTERVAL_MONTH:
    case SQL_INTERVAL_YEAR:
    case SQL_INTERVAL_YEAR_TO_MONTH:
    case SQL_INTERVAL_DAY:
    case SQL_INTERVAL_HOUR:
    case SQL_INTERVAL_MINUTE:
    case SQL_INTERVAL_SECOND:
    case SQL_INTERVAL_DAY_TO_HOUR:
    case SQL_INTERVAL_DAY_TO_MINUTE:
    case SQL_INTERVAL_DAY_TO_SECOND:
    case SQL_INTERVAL_HOUR_TO_MINUTE:
    case SQL_INTERVAL_HOUR_TO_SECOND:
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        /* interval types are not supported */
        break;
        /*
    case SQL_DATE:
        implRecPtr->conciseType = sqlType;
        implRecPtr->sqlType = SQL_DATETIME;
        implRecPtr->datetimeIntervalCode = SQL_CODE_DATE;
        break;
    case SQL_TIME:
        implRecPtr->conciseType = sqlType;
        implRecPtr->sqlType = SQL_DATETIME;
        implRecPtr->datetimeIntervalCode = SQL_CODE_TIME;
        break;
    case SQL_TIMESTAMP:
        implRecPtr->conciseType = sqlType;
        implRecPtr->sqlType = SQL_DATETIME;
        implRecPtr->datetimeIntervalCode = SQL_CODE_TIMESTAMP;
        break;
        */
    default:
        implRecPtr->conciseType = sqlType;
        implRecPtr->sqlType = sqlType;
    }; /* switch */
} /* pa21SetSQLType */


/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/

