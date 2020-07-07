/*!**********************************************************************

  module: vpa60Stmt.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC statement handles (datstructures,functions)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------





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

#include "vpa30.h"
#include "vpa40DBC.h"
#include "vpa50Env.h"
#include "vpa60Stmt.h"
#include "vpa70SQLDA.h"
#include "vpa06.h"
#include "vpa09.h"

#include "gpr03.h"

/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/

/* return a unique id for each statement-handle. This id is used to
 * generate a cursor name.
 */
VOID pa60_GetStatementId( UDWORD *id );

API_RETCODE pa60apmastmt( SQLHDBC hdbc, SQLHSTMT *phstmt );

API_RETCODE pa60apmfstmt ( SQLHSTMT hstmt);

SQLRETURN pa60_GetGroupByColumns( PTR        valuePtr,
                                  SDWORD     bufferLen,
                                  SDWORD    *strLenPtr,
                                  UWORD     *sqlStatePtr ); 

API_RETCODE pa60CloseCursor( tpa40DBC  *dbc_block_ptr,
                             tpa60Stmt *stmt_block_ptr );

VOID pa60_InitSetPosSpecial( tpa60SetPosSpecial *setPosSpecial );

VOID pa60_FreeSetPosSpecial( tpa60SetPosSpecial *setPosSpecial );

SQLRETURN pa60_DropOneStmt( SQLHSTMT hstmt );

UDWORD pa60_ComputePosition (UDWORD           long_pos,
                             tpa21DescRecord *impl_rec_ptr,
                             SWORD            fCType, 
                             UWORD           *terminatorSize);
tsp00_Int pa60_ComputeKernelPosition (UDWORD           long_pos,
                                      tpa21DescRecord *impl_rec_ptr);

int pa60_isConnected (tpa40DBC *dbc_block_ptr);

/*!**********************************************************************

  Chapter: ODBC functions

************************************************************************/

#ifndef SAPDB_FAST
#ifndef _UNICODE_ODBC
UCHAR FAR *PA60_FN_SQLGETSTMTATTR = { (UCHAR*) "SQLGetStmtAttr" };
UCHAR FAR *PA60_FN_SQLFREESTMT    = { (UCHAR*) "SQLFreeStmt" };
UCHAR FAR *PA60_FN_SQLSETSTMTATTR = { (UCHAR*) "SQLSetStmtAttr" };
#else
extern UCHAR FAR *PA60_FN_SQLGETSTMTATTR;
extern UCHAR FAR *PA60_FN_SQLFREESTMT;
extern UCHAR FAR *PA60_FN_SQLSETSTMTATTR;
#endif
#endif


#ifndef _UNICODE_ODBC
ODBC_PROC(SQLFreeStmt, ( SQLHSTMT   hstmt,
                        UWORD   fOption),
          (hstmt, fOption))
{
    SQLRETURN   retcode;
    API_RETCODE api_retcode;
    UWORD       state;
    UWORD       number_cols;
    UWORD       number_parms;
    UWORD       i;
    tpa60Stmt  *stmt_block_ptr;
    SQLHDBC     hdbc;
    tpa40DBC   *dbc_block_ptr;
    SQLHENV     henv;
    tpa50Env   *env_block_ptr;
    tpa21DescRecord *ard_rec_ptr;
    tpa21DescRecord *apd_rec_ptr;
    
    API_TRACE( API_TR_ENTRY, PA60_FN_SQLFREESTMT, 0);
    API_TRACE( API_TR_HANDLE, "hstmt", &hstmt);
    API_TRACE( API_TR_UWORD, "fOption", &fOption);

    api_retcode = apmstfc(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, 
                          SQL_API_SQLFREESTMT);
    if (api_retcode != API_OK) {
        retcode = SQL_INVALID_HANDLE;
        goto no_lock_exit;
    }
   
    if (pa60VerifyStmt( hstmt ) != API_OK) { /* hstmt valid? */
        retcode = SQL_INVALID_HANDLE;
        goto no_lock_exit;
    }
   
    apmlocp(&hstmt, &stmt_block_ptr, &hdbc, &dbc_block_ptr, 
            &henv, &env_block_ptr);
    state = stmt_block_ptr->state;
    number_cols = stmt_block_ptr -> number_cols;
    number_parms = stmt_block_ptr -> number_parms;
    if (stmt_block_ptr -> hstmt_setpos && fOption != SQL_DROP) {
        CALL_ODBC(SQLFreeStmt, (stmt_block_ptr->hstmt_setpos, fOption));      
    }; /* if */
    if (stmt_block_ptr -> hstmt_extfetch && fOption != SQL_DROP) {
        CALL_ODBC(SQLFreeStmt, (stmt_block_ptr->hstmt_extfetch, fOption));
    }; /* if */

    if (fOption == SQL_DROP) {
        /* no PA09ENTERASYNCFUNCTION for DROP (PTS 1105984) */
        retcode = pa60DropStmt( hstmt );
    } else {
        PA09ENTERASYNCFUNCTION(dbc_block_ptr, stmt_block_ptr);
        switch (fOption) {
        case (SQL_CLOSE): {} 
            switch (state) {
            case (API_STMT_POSITIONED):
                {
                    stmt_block_ptr->state = API_STMT_EXECUTED;
                }
            case (API_STMT_EXECUTED):
                {
                    if (number_cols > 0) {
                        pa60CloseCursor( dbc_block_ptr, stmt_block_ptr );
                        stmt_block_ptr->fetch_pos = 0;
                        stmt_block_ptr->rowset_pos = 0;
                    }     
                    if (stmt_block_ptr -> hrowstat != API_NULL_HANDLE) {
                        apdfree(stmt_block_ptr -> hrowstat);
                        stmt_block_ptr -> hrowstat = API_NULL_HANDLE;
                    }
                    stmt_block_ptr->state = API_STMT_PREPARED;	       
                }
            case (API_STMT_PREPARED):
                {
                }
            }; /* switch */
            stmt_block_ptr -> number_rows = 0;
            stmt_block_ptr -> reconnect_status = API_FALSE;
            stmt_block_ptr -> dbc_special.special = API_SPEC_NOT;
            stmt_block_ptr -> need_data = FALSE;
            stmt_block_ptr -> setPosNeedData = FALSE;
            pa60_FreeSetPosSpecial( &stmt_block_ptr -> setPosSpecial );
            retcode = SQL_SUCCESS;
            break;
        case (SQL_UNBIND): { /* remove the binds */
            for (i=0; i <= number_cols; i++) {
                ard_rec_ptr = pa20GetRecord( stmt_block_ptr->ardPtr, i );
                API_ASSERT_PTR( ard_rec_ptr );
                ard_rec_ptr -> bound = API_FALSE;
            }
            stmt_block_ptr->ardPtr -> count = 0;
            stmt_block_ptr->ardPtr->header.bindType = SQL_BIND_TYPE_DEFAULT;
            stmt_block_ptr->ird.count = 0;
            pa60_FreeSetPosSpecial( &stmt_block_ptr -> setPosSpecial );
            retcode = SQL_SUCCESS;
            break;
        }
        case (SQL_RESET_PARAMS): { /* remove the binds */
            for (i=1; i <= number_parms; i++) {
                apd_rec_ptr = pa20GetRecord( stmt_block_ptr->apdPtr, i );
                API_ASSERT_PTR( apd_rec_ptr );
                apd_rec_ptr -> bound = API_FALSE;
            }
            stmt_block_ptr->apdPtr -> count = 0;
            /* SQLParamOptions */
            stmt_block_ptr -> ipd.header.arraySize = 1;
            stmt_block_ptr -> ipd.header.previousArraySize = 1;
            retcode = SQL_SUCCESS;
            break;
        }
        default: {/* bad code */
            retcode = SQL_ERROR;
            pa60PutError( hstmt, API_ODBC_S1092,  NULL);
            break;
        }
        } /* of switch */
        PA09LEAVEASYNCFUNCTION();
    }; /* else */      
   
no_lock_exit:

    API_TRACE(API_TR_EXIT,PA60_FN_SQLFREESTMT,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    /*PA30UNPROTECTTHREAD();*/
    return (retcode);

} /* SQLFreeStmt */
#endif  /* of _UNICODE_ODBC */

#ifdef _UNICODE_ODBC
ODBC_PROC(SQLGetStmtAttrW, (SQLHSTMT      hstmt,
                            SQLINTEGER    attr,
                            SQLPOINTER    valuePtr,
                            SQLINTEGER    bufferLen,
                            SQLINTEGER   *strLenPtr ),
          (hstmt, attr, valuePtr, bufferLen, strLenPtr))
#else
ODBC_PROC(SQLGetStmtAttr, (SQLHSTMT      hstmt,
                           SQLINTEGER    attr,
                           SQLPOINTER    valuePtr,
                           SQLINTEGER    bufferLen,
                           SQLINTEGER   *strLenPtr ),
          (hstmt, attr, valuePtr, bufferLen, strLenPtr))
#endif
{
    SQLRETURN  retcode = SQL_ERROR;
    tpa60Stmt *stmtPtr;
    UWORD      sqlState = API_ODBC_00000; /* success */
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii; 
#endif
                                           
    API_TRACE( API_TR_ENTRY, PA60_FN_SQLGETSTMTATTR, 0);
    API_TRACE( API_TR_HANDLE, "hstmt", &hstmt);
    API_TRACE( API_TR_STMTATTR, "attr", &attr);
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr);
    API_TRACE( API_TR_SDWORD, "bufferLen", &bufferLen);
    API_TRACE( API_TR_PTR, "strLenPtr", &strLenPtr);

    if (pa60VerifyStmt( hstmt ) != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        retcode = SQL_SUCCESS;   
        stmtPtr = (tpa60Stmt*) hstmt;
        API_ASSERT_PTR(stmtPtr);
        if (valuePtr!=NULL) {
            switch(attr) {
            case (SQL_ROWSET_SIZE): {
                *((UDWORD*)  valuePtr) = stmtPtr->ardPtr->header.arraySize;
                break;
            }
            case (SQL_GET_BOOKMARK): {
                if (stmtPtr->stmtopt.use_bookmarks == SQL_UB_ON) { /* cmp. beneath comment */
                    *((UDWORD*)  valuePtr) = (SQLUINTEGER) stmtPtr->fetch_pos;
                } else {
                    sqlState = API_ODBC_S1011; /* invalid option */
                    retcode = SQL_ERROR;
                }
                break;
            }
            case (SQL_ROW_NUMBER): { /* in ODBC reference, SQLSetStmtAttr
                                        this is explicitely SQLUINTEGER  */
                *((UDWORD FAR *)  valuePtr) = (SQLUINTEGER) stmtPtr->fetch_pos;
                break;
            }
            /* ------ *** ODBC 3.0 * ---------------------------------- */
            case SQL_ATTR_APP_PARAM_DESC:
                *((tpa20Desc**) valuePtr) = pa60GetAPD( stmtPtr );
                break;
            case SQL_ATTR_APP_ROW_DESC:
                *((tpa20Desc**) valuePtr) = pa60GetARD( stmtPtr );
                break;
            case SQL_ATTR_IMP_PARAM_DESC:
                *((tpa20Desc**) valuePtr) = &stmtPtr->ipd;
                break;
            case SQL_ATTR_IMP_ROW_DESC:
                *((tpa20Desc**) valuePtr) = &stmtPtr->ird;
                break;
                /* descriptor attributes */
            case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
                retcode = pa20GetDescField( pa60GetAPD(stmtPtr),
                                            0,
                                            SQL_DESC_BIND_OFFSET_PTR,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;                
            case SQL_ATTR_ROW_BIND_OFFSET_PTR:
                retcode = pa20GetDescField( pa60GetARD(stmtPtr),
                                            0,
                                            SQL_DESC_BIND_OFFSET_PTR,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_PARAM_BIND_TYPE:
                retcode = pa20GetDescField( pa60GetAPD(stmtPtr),
                                            0,
                                            SQL_DESC_BIND_TYPE,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;                
            case SQL_BIND_TYPE:
                retcode = pa20GetDescField( pa60GetARD(stmtPtr),
                                            0,
                                            SQL_DESC_BIND_TYPE,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_PARAM_OPERATION_PTR:
                retcode = pa20GetDescField( pa60GetAPD(stmtPtr),
                                            0,
                                            SQL_DESC_ARRAY_STATUS_PTR,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_PARAM_STATUS_PTR:
                retcode = pa20GetDescField( &stmtPtr->ipd,
                                            0,
                                            SQL_DESC_ARRAY_STATUS_PTR,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_PARAMS_PROCESSED_PTR:
                retcode = pa20GetDescField( &stmtPtr->ipd,
                                            0,
                                            SQL_DESC_ROWS_PROCESSED_PTR,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_PARAMSET_SIZE:
                retcode = pa20GetDescField( pa60GetAPD(stmtPtr),
                                            0,
                                            SQL_DESC_ARRAY_SIZE,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_ROW_ARRAY_SIZE:
                retcode = pa20GetDescField( pa60GetARD(stmtPtr),
                                            0,
                                            SQL_DESC_ARRAY_SIZE,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_ROW_OPERATION_PTR:
                retcode = pa20GetDescField( pa60GetARD(stmtPtr),
                                            0,
                                            SQL_DESC_ARRAY_STATUS_PTR,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_ROW_STATUS_PTR:
                retcode = pa20GetDescField( &stmtPtr->ird,
                                            0,
                                            SQL_DESC_ARRAY_STATUS_PTR,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
            case SQL_ATTR_ROWS_FETCHED_PTR:
                retcode = pa20GetDescField( &stmtPtr->ird,
                                            0,
                                            SQL_DESC_ROWS_PROCESSED_PTR,
                                            valuePtr,
                                            0,
                                            strLenPtr,
                                            &sqlState,
                                            nativeEncoding);
                break;
                /* SAP DB special attributes */
            case SQL_ATTR_GROUP_BY_COLUMNS:
                retcode = pa60_GetGroupByColumns( valuePtr,
                                                  bufferLen,
                                                  strLenPtr,
                                                  &sqlState );
                break;
            default:
                retcode = pa61GetStmtAttr( &stmtPtr->stmtopt,
                                           attr,
                                           valuePtr,
                                           bufferLen,
                                           strLenPtr,
                                           &sqlState );
                if (attr == SQL_ATTR_CURSOR_TYPE) {
                    API_TRACE( API_TR_CURSORTYPE, "cursorType", valuePtr);
                };
            }; /* switch */
        }; /* if (valuePtr!=NULL) */
    }; /* else */

    if (sqlState!=API_ODBC_00000) {
        pa60PutError( hstmt, sqlState, NULL );
    }; /* if */

    API_TRACE(API_TR_EXIT, PA60_FN_SQLGETSTMTATTR, 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    
    return retcode;   
} /* SQLGetStmtAttr */


/* ODBC 3.0 */
#ifdef _UNICODE_ODBC
ODBC_PROC(SQLSetStmtAttrW, (SQLHSTMT    hstmt,
                            SQLINTEGER  attr,
                            SQLPOINTER  valuePtr,
                            SQLINTEGER  strLen ),
          (hstmt, attr, valuePtr, strLen))
#else
ODBC_PROC(SQLSetStmtAttr, (SQLHSTMT    hstmt,
                           SQLINTEGER  attr,
                           SQLPOINTER  valuePtr,
                           SQLINTEGER  strLen ),
          (hstmt, attr, valuePtr, strLen))
#endif
{
    SQLRETURN  retcode = SQL_SUCCESS;
    tpa60Stmt *stmtPtr;
    UWORD      sqlState = API_ODBC_00000; /* success */
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii; 
#endif
    
    API_TRACE( API_TR_ENTRY, PA60_FN_SQLSETSTMTATTR, 0);
    API_TRACE( API_TR_HANDLE, "hstmt", &hstmt);
    API_TRACE( API_TR_STMTATTR, "attr", &attr);
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr);
    API_TRACE( API_TR_SDWORD, "strLen", &strLen);

    if (pa60VerifyStmt(hstmt)!=API_OK) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        stmtPtr = (tpa60Stmt*) hstmt;
        pa60ResetError( (SQLHSTMT) stmtPtr );
        switch (attr) {
        case (SQL_ROWSET_SIZE):
            {
                if ((UDWORD) VALUE_CAST (valuePtr) > 0) {
                    stmtPtr->ardPtr->header.arraySize = (UDWORD) VALUE_CAST (valuePtr);
                } else {
                    sqlState = API_ODBC_S1009;
                }
                break;
            }
            /* descriptors */
            /* if valuePtr is an IPD or IRD the DM produces an error
             * so we don´t have to catch this error here */
        case SQL_ATTR_APP_PARAM_DESC:
            stmtPtr->apdPtr->parentStmt = SQL_NULL_HSTMT;
            if (valuePtr != SQL_NULL_HDESC) {
                stmtPtr->apdPtr = (tpa20Desc*) valuePtr;
            } else {
                /* revert to default descriptor */
                stmtPtr->apdPtr = &stmtPtr->defaultApd;
            }; /* else */
            break;
        case SQL_ATTR_APP_ROW_DESC:
            stmtPtr->ardPtr->parentStmt = SQL_NULL_HSTMT;
            if (valuePtr != SQL_NULL_HDESC) {
                stmtPtr->ardPtr = (tpa20Desc*) valuePtr;
            } else {
                /* revert to default descriptor */
                stmtPtr->ardPtr = &stmtPtr->defaultArd;
            }; /* else */
            break;
        case SQL_ATTR_IMP_PARAM_DESC:
        case SQL_ATTR_IMP_ROW_DESC:
            retcode = SQL_ERROR;
            sqlState = API_ODBC_HY017; /* Invalid use of an automatically
                                        * allocated descriptor handle */
            break;
            /* descriptor attributes */
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
           retcode = pa20SetDescField( pa60GetAPD(stmtPtr),
                                       0,
                                       SQL_DESC_BIND_OFFSET_PTR,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
           break;
        case SQL_ATTR_ROW_BIND_OFFSET_PTR:
           retcode = pa20SetDescField( pa60GetARD(stmtPtr),
                                       0,
                                       SQL_DESC_BIND_OFFSET_PTR,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
            break;
        case SQL_BIND_TYPE:
            retcode = pa20SetDescField( pa60GetARD(stmtPtr),
                                        0,
                                        SQL_DESC_BIND_TYPE,
                                        valuePtr,
                                        0,
                                        &sqlState,
                                        nativeEncoding);

            break;
        case SQL_ATTR_PARAM_BIND_TYPE:
            retcode = pa20SetDescField( pa60GetAPD(stmtPtr),
                                        0,
                                        SQL_DESC_BIND_TYPE,
                                        valuePtr,
                                        0,
                                        &sqlState,
                                        nativeEncoding);

        case SQL_ATTR_PARAM_OPERATION_PTR:
           retcode = pa20SetDescField( pa60GetAPD(stmtPtr),
                                       0,
                                       SQL_DESC_ARRAY_STATUS_PTR,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
           break;
        case SQL_ATTR_PARAM_STATUS_PTR:
           retcode = pa20SetDescField( &stmtPtr->ipd,
                                       0,
                                       SQL_DESC_ARRAY_STATUS_PTR,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
           break;
        case SQL_ATTR_PARAMS_PROCESSED_PTR:
           retcode = pa20SetDescField( &stmtPtr->ipd,
                                       0,
                                       SQL_DESC_ROWS_PROCESSED_PTR,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
           break;
        case SQL_ATTR_PARAMSET_SIZE:
           retcode = pa20SetDescField( pa60GetAPD(stmtPtr),
                                       0,
                                       SQL_DESC_ARRAY_SIZE,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
           break;
        case SQL_ATTR_ROW_ARRAY_SIZE:
           retcode = pa20SetDescField( pa60GetARD(stmtPtr),
                                       0,
                                       SQL_DESC_ARRAY_SIZE,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
           break;
        case SQL_ATTR_ROW_OPERATION_PTR:
           retcode = pa20SetDescField( pa60GetARD(stmtPtr),
                                       0,
                                       SQL_DESC_ARRAY_STATUS_PTR,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);

           break;
        case SQL_ATTR_ROW_STATUS_PTR:
           retcode = pa20SetDescField( &stmtPtr->ird,
                                       0,
                                       SQL_DESC_ARRAY_STATUS_PTR,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
           break;
        case SQL_ATTR_ROWS_FETCHED_PTR:
           retcode = pa20SetDescField( &stmtPtr->ird,
                                       0,
                                       SQL_DESC_ROWS_PROCESSED_PTR,
                                       valuePtr,
                                       0,
                                       &sqlState,
                                       nativeEncoding);
           break;
        default:
            /* statement attributes */
            retcode = pa61SetStmtAttr( (SQLHSTMT) stmtPtr,
                                       stmtPtr->state,
                                       attr,
                                       valuePtr,
                                       strLen,
                                       &stmtPtr->stmtopt,
                                       &sqlState );
            break;
        }; /* switch */
    }; /* else */

    if (sqlState!=API_ODBC_00000) {
        pa60PutError( hstmt, sqlState, NULL );
    }; /* if */
    
    API_TRACE(API_TR_EXIT, PA60_FN_SQLSETSTMTATTR, 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    
    return retcode;   
} /* SQLSetStmtAttr */

#ifndef _UNICODE_ODBC


/*!**********************************************************************

  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

UCHAR FAR *PA60_FN_ALLOCSTMT = { (UCHAR*) "pa60AllocStmt" };


SQLRETURN pa60AllocStmt( SQLHDBC hdbc, SQLHSTMT *phstmt)
{
    SQLRETURN  retcode;
    UWORD      state;
    tpa60Stmt *stmtPtr;
    
    PA30PROTECTTHREAD();
    API_TRACE(API_TR_ENTRY,PA60_FN_ALLOCSTMT,0);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);
    API_TRACE(API_TR_PTR,"phstmt",&phstmt);

    if (pa40VerifyDBC(hdbc) != API_OK) { /* hdbc valid? */
        retcode = SQL_INVALID_HANDLE;
        if (phstmt != NULL) { /* non-NULL pointer? */
            *phstmt = SQL_NULL_HSTMT;
        }
    }
    else {
        state = ((tpa40DBC*) hdbc)->state;
        apmstfc(SQL_NULL_HENV,hdbc,SQL_NULL_HSTMT, SQL_API_SQLALLOCSTMT);
        if (phstmt == NULL) { /* NULL pointer? */
            retcode = SQL_ERROR;
            pa40PutError( hdbc, API_ODBC_S1009, NULL ); /* invaild argument
                                                         * value */
        } 
        else {
            if (state != API_DBC_CONNECTED) { /* connected? */
                retcode = SQL_ERROR;
                pa40PutError( hdbc, API_ODBC_S1009, NULL ); /* connection
                                                             * not open */
            } 
            else {
                /* allocate ok? */
                if (pa60apmastmt( hdbc, phstmt ) != API_OK) {
                    retcode = SQL_ERROR;
                    *phstmt = SQL_NULL_HSTMT;
                    pa40PutError( hdbc, API_ODBC_S1001, NULL ); /* memory
                                                                 * allocation
                                                                 * failure */
                }
                else {
                    stmtPtr = *((tpa60Stmt**) phstmt);
                    stmtPtr->diagArea = apdallo( sizeof(stmtPtr->diagArea));
                    if (stmtPtr->diagArea == NULL) {
                        retcode = SQL_ERROR;
                        pa40PutError( hdbc, API_ODBC_S1001, NULL );
                        /* memory allocation failure */
                    } else {
                        pa30InitDiagArea( stmtPtr->diagArea );
                        retcode = SQL_SUCCESS;
                    }; /* else */
                }
            }; /* else */
        }; /* else */
    }; /* else */
	
    API_TRACE(API_TR_EXIT,PA60_FN_ALLOCSTMT,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    API_TRACE(API_TR_HANDLE,"*phstmt",phstmt);
	
    PA30UNPROTECTTHREAD();
    
    return(retcode);
} /* pa60AllocStmt */


API_RETCODE pa60AllocSetPosSpecial( tpa60SetPosSpecial *setPosSpecial,
                                    tsp00_Int4          numCols )
{
    if ( setPosSpecial->arraySize < numCols ) {
        pa60_FreeSetPosSpecial( setPosSpecial );
        setPosSpecial->ignoredArray = apdallo( numCols * sizeof( tsp00_Bool ));
        setPosSpecial->arraySize    = numCols;
        pa60ReinitSetPosSpecial( setPosSpecial );
    };
    
    return ((setPosSpecial->ignoredArray != NULL) ? API_OK : API_NOT_OK);
} /* pa60AllocSetPosSpecial */


API_RETCODE pa60RefreshSetPosSpecial( tpa60SetPosSpecial *setPosSpecial,
                                      tpa20Desc          *ardPtr,
                                      tsp00_Int4          rowsetPos )
{
    UWORD            iCol = 0;
    tpa21DescRecord *ardRecPtr = NULL;
    SQLLEN          *octetLengthPtr = NULL;

    for (iCol = 1; iCol <= ardPtr->count; iCol++) {
        ardRecPtr = pa20GetRecord( ardPtr, iCol );
        API_ASSERT_PTR( ardRecPtr );
        octetLengthPtr = PA20_OCTETLENGTH_PTR(ardRecPtr);
        if (octetLengthPtr != NULL) {
            octetLengthPtr += rowsetPos-1;
            /* skip columns with length SQL_IGNORE PTS 1104910.
             * In pa20PutDesc these are also skipped. So the parameters
             * do match. */
            if (*octetLengthPtr != SQL_IGNORE) {
                setPosSpecial->ignoredArray[iCol-1] = false;
            } else {
                setPosSpecial->ignoredArray[iCol-1] = true;
            };
        } else {
            setPosSpecial->ignoredArray[iCol-1] = true;
        };    
    }; /* for */

    return API_OK;
} /* pa60RefreshSetPosSpecial */


VOID pa60ReinitSetPosSpecial( tpa60SetPosSpecial *setPosSpecial )
{
    tsp00_Int4       iCol = 0;

    for (iCol = 1; iCol <= setPosSpecial->arraySize; iCol++) {
        setPosSpecial->ignoredArray[iCol-1] = false;
    };
} /* pa60ReinitSetPosSpecial */

tsp00_Bool pa60StmtChanged( tpa60SetPosSpecial *setPosSpecial,
                            tpa20Desc          *ardPtr,
                            tsp00_Int4          rowsetPos )
{
    UWORD            iCol = 0;
    tpa21DescRecord *ardRecPtr = NULL;
    SQLLEN          *octetLengthPtr = NULL;

    for (iCol = 1; iCol <= ardPtr->count; iCol++) {
        ardRecPtr = pa20GetRecord( ardPtr, iCol );
        API_ASSERT_PTR( ardRecPtr );
        octetLengthPtr = PA20_OCTETLENGTH_PTR(ardRecPtr);
        if (octetLengthPtr != NULL) {
            octetLengthPtr += rowsetPos-1;
            if ((*octetLengthPtr == SQL_IGNORE) !=
                (setPosSpecial->ignoredArray[iCol-1])){
                return true;
            };
        };    
    }; /* for */

    return false;
} /* pa60StmtChanged */


VOID pa60DropFetchParseIds( tpa60Stmt *stmt_block_ptr )
{
    tpa40DBC *dbc_block_ptr = (tpa40DBC*) stmt_block_ptr->parent_hdbc;

    API_TRACE( API_TR_ENTRY, "pa60DropFetchParseIds", 0 );

    API_ASSERT( pa40VerifyDBC( dbc_block_ptr ) == API_OK );
    
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129839 */
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1131844 */
    if (!pa60_isConnected (dbc_block_ptr))
      return;

    apedrprs( &dbc_block_ptr->esqblk,
              stmt_block_ptr -> fetch_first_parsid,
              dbc_block_ptr,
              stmt_block_ptr );
    apedrprs( &dbc_block_ptr->esqblk,
              stmt_block_ptr -> fetch_last_parsid,
              dbc_block_ptr,
              stmt_block_ptr );
    apedrprs( &dbc_block_ptr->esqblk,
              stmt_block_ptr -> fetch_absolute_parsid,
              dbc_block_ptr,
              stmt_block_ptr );
    apedrprs( &dbc_block_ptr->esqblk,
              stmt_block_ptr -> fetch_next_parsid,
              dbc_block_ptr,
              stmt_block_ptr );
    apedrprs( &dbc_block_ptr->esqblk,
              stmt_block_ptr -> fetch_prior_parsid,
              dbc_block_ptr,
              stmt_block_ptr );
    apedrprs( &dbc_block_ptr->esqblk,
              stmt_block_ptr -> fetch_relative_parsid,
              dbc_block_ptr,
              stmt_block_ptr );

    API_TRACE( API_TR_EXIT, "pa60DropFetchParseIds", 0 );
} /* pa60DropFetchParseIds */


VOID pa60DropParseIds( tpa60Stmt *stmt_block_ptr )
{
    tpa40DBC *dbc_block_ptr = (tpa40DBC*) stmt_block_ptr->parent_hdbc;

    API_TRACE( API_TR_ENTRY, "pa60DropParseIds", 0 );

    API_ASSERT( pa40VerifyDBC( dbc_block_ptr ) == API_OK );
    
    pa60DropFetchParseIds( stmt_block_ptr );

    if (!pa60_isConnected (dbc_block_ptr))
      return;

    apedrprs( &dbc_block_ptr->esqblk,
              stmt_block_ptr -> close_parsid,
              dbc_block_ptr,
              stmt_block_ptr );
    apedrprs( &dbc_block_ptr->esqblk,
              stmt_block_ptr -> stmt_parsid,
              dbc_block_ptr,
              stmt_block_ptr );

    API_TRACE( API_TR_EXIT, "pa60DropParseIds", 0 );
} /* pa60DropParseIds */

SQLRETURN pa60DropStmt( SQLHSTMT hstmt )
{
    SQLRETURN retcode;
    
    /* first drop stmts from SQLSetPos and SQLExtendedFetch */
    retcode = pa60DropChildStmts( hstmt );
    if (retcode == SQL_SUCCESS) {
        retcode = pa60_DropOneStmt( hstmt );
    }; /* if */

    return retcode;
} /* pa60DropStmt */

SQLRETURN pa60DropChildStmts( SQLHSTMT hstmt )
{
    SQLRETURN    retcode = SQL_SUCCESS;
    API_RETCODE  api_retcode;
    tpa60Stmt   *stmt_block_ptr;
    SQLHDBC      hdbc;
    tpa40DBC    *dbc_block_ptr;
    SQLHENV      henv;
    tpa50Env    *env_block_ptr;
    
    /* we have to check invalid handle before apmstfc because
     * apmstfc crashes with invalid handle */
    if (pa60VerifyStmt( hstmt ) != API_OK) {
        retcode =SQL_INVALID_HANDLE;
    } else {
        api_retcode = apmstfc(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, 
                              SQL_API_SQLFREESTMT);
        if (api_retcode != API_OK) {
            retcode = SQL_INVALID_HANDLE;
        } else {            
            apmlocp(&hstmt, &stmt_block_ptr, &hdbc, &dbc_block_ptr, 
                    &henv, &env_block_ptr);
            
            if (stmt_block_ptr -> hstmt_setpos) {
                pa60_DropOneStmt( stmt_block_ptr->hstmt_setpos );
                stmt_block_ptr->hstmt_setpos = API_NULL_HANDLE;
            }; /* if */
            if (stmt_block_ptr -> hstmt_extfetch) {
                pa60_DropOneStmt( stmt_block_ptr->hstmt_extfetch );
                stmt_block_ptr->hstmt_extfetch = API_NULL_HANDLE;
            }; /* if */
        }; /* else */
    }; /* else */

    return retcode;
} /* pa60DropChildStmts */


SQLRETURN pa60_DropOneStmt( SQLHSTMT hstmt )
{
    SQLRETURN    retcode;
    API_RETCODE  api_retcode;
    UWORD        state;
    UWORD        number_cols;
    UWORD        number_parms;
    tpa60Stmt   *stmt_block_ptr;
    SQLHDBC      hdbc;
    tpa40DBC    *dbc_block_ptr;
    SQLHENV      henv;
    tpa50Env    *env_block_ptr;

    API_TRACE( API_TR_ENTRY, "pa60_DropOneStmt", 0 );
    API_TRACE( API_TR_HANDLE, "hstmt", &hstmt );

    /* we have to check invalid handle before apmstfc because
     * apmstfc crashes with invalid handle */
    if (pa60VerifyStmt( hstmt ) != API_OK) {
        retcode =SQL_INVALID_HANDLE;
    } else {
        api_retcode = apmstfc(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, 
                              SQL_API_SQLFREESTMT);
        if (api_retcode != API_OK) {
            retcode = SQL_INVALID_HANDLE;
        } else {            
            apmlocp(&hstmt, &stmt_block_ptr, &hdbc, &dbc_block_ptr, 
                    &henv, &env_block_ptr);
            
            state = stmt_block_ptr->state;
            PA09ENTERASYNCFUNCTION(dbc_block_ptr, stmt_block_ptr);
            number_cols = stmt_block_ptr -> number_cols;
            number_parms = stmt_block_ptr -> number_parms;
            /* the diagArea is shared by hstmt and hstmt_setpos
             * so it is only freed once for the parent stmt */
            if ( stmt_block_ptr->hstmt_parent == SQL_NULL_HSTMT
                 && stmt_block_ptr->diagArea != NULL) {
                pa30FreeDiagArea( stmt_block_ptr->diagArea );
                pa30DeleteDiagArea( stmt_block_ptr->diagArea );
                apdfree( stmt_block_ptr->diagArea );
                stmt_block_ptr->diagArea = NULL;
            }; /* if */
            
            switch (state) {
            case (API_STMT_POSITIONED):
                {
                }
            case (API_STMT_EXECUTED):
                {
                    if (number_cols > 0) {
                        pa60CloseCursor( dbc_block_ptr, stmt_block_ptr );
                        stmt_block_ptr->fetch_pos = 0;
                        stmt_block_ptr->rowset_pos = 0;
                    }     
                    if (stmt_block_ptr -> hrowstat != API_NULL_HANDLE) {
                        apdfree(stmt_block_ptr -> hrowstat);
                        stmt_block_ptr -> hrowstat = API_NULL_HANDLE;
                    }
                    state = API_STMT_PREPARED;	       
                }
            case (API_STMT_PREPARED):
                {
                    stmt_block_ptr -> number_cols = 0;
                    stmt_block_ptr -> number_parms = 0;
                    state = API_STMT_ALLOCATED;	       
                }
            }; /* switch */
            pa60DropParseIds( stmt_block_ptr );
            stmt_block_ptr -> number_rows = 0;
            stmt_block_ptr -> reconnect_status = API_FALSE;
            /* ADIS 1001285 */
            pa09TerminateThread(hstmt);
            PA09LEAVEASYNCFUNCTION();
            pa09WaitEndOfThread(hstmt);
            if (pa60apmfstmt(hstmt) == API_OK) { /* free ok? */
                retcode = SQL_SUCCESS;
            }
            else {
                retcode = SQL_ERROR;
                pa60PutError( hstmt, API_ODBC_S1000, NULL);
            }
        }; /* else */
    }; /* else */
    
    API_TRACE( API_TR_EXIT, "pa60_DropOneStmt", 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    return retcode;    
} /* pa60_DropOneStmt */

API_RETCODE pa60GetFetchParseId( tpa60Stmt  *stmt,
                                 UCHAR     **parseid,
                                 UWORD       fetchtype)
{
    API_RETCODE apiRetcode = API_OK;
    
    switch (fetchtype) {
    case SQL_FETCH_FIRST:
        *parseid = stmt->fetch_first_parsid;
        break;
    case SQL_FETCH_LAST:
        *parseid = stmt->fetch_last_parsid;
        break;
    case SQL_FETCH_ABSOLUTE:
        *parseid = stmt->fetch_absolute_parsid;
        break;
    case SQL_FETCH_NEXT:
        *parseid = stmt->fetch_next_parsid;
        break;
    case SQL_FETCH_PRIOR:
        *parseid = stmt->fetch_prior_parsid;
        break;
    case SQL_FETCH_RELATIVE:
        *parseid = stmt->fetch_relative_parsid;
        break;
    default:
        apiRetcode = API_NOT_OK;
        break;
    }; /* switch */

    return apiRetcode;
} /* pa60GetFetchParseId */


SQLHDBC pa60GetParentDBC( tpa60Stmt *stmt )
{
    SQLHDBC parentDBC;
    
    API_TRACE( API_TR_ENTRY, "pa60GetParentDBC", 0);
    API_TRACE( API_TR_PTR, "stmt", &stmt );

    parentDBC = stmt->parent_hdbc;

    API_TRACE( API_TR_EXIT, "pa60GetParentDBC", 0);
    API_TRACE( API_TR_PTR, "parentDBC", &parentDBC);
    
    return parentDBC;
} /* pa60GetParentDBC */


SQLHENV pa60GetParentEnv( tpa60Stmt *stmt )
{
    SQLHENV    parentEnv;
    tpa40DBC  *dbc;
    
    API_TRACE( API_TR_ENTRY, "pa60GetParentEnv", 0);
    API_TRACE( API_TR_PTR, "stmt", &stmt );

    dbc = (tpa40DBC*) stmt->parent_hdbc;
    parentEnv = (SQLHENV) pa40GetParentEnv( dbc );

    API_TRACE( API_TR_EXIT, "pa60GetParentEnv", 0);
    API_TRACE( API_TR_PTR, "parentEnv", &parentEnv);
    
    return parentEnv;
} /* pa60GetParentEnv */




API_RETCODE pa60MoveLong( tpa40DBC   *dbc_block_ptr,
                          tpa60Stmt  *stmt_block_ptr,
                          tpa20Desc  *appl_desc_ptr,
                          tpa20Desc  *impl_desc_ptr,
                          UWORD       icol,
                          UWORD       number_cols )
{
    API_RETCODE      api_retcode=API_OK;
    tpa62DBCSpecial *dbc_special_ptr;  
    SWORD            fCType;
    PTR              rgbValue;
    SQLLEN           cbValueMax;
    SQLLEN          *pcbValue=NULL;
    SQLLEN           cbValue = 0;
    SQLLEN           oldcbValue;
    SWORD            col;
    SWORD            i;
    SWORD            get_next;
    SWORD            rbcolno;
    tpa21DescRecord *appl_rec_ptr;
    tpa21DescRecord *impl_rec_ptr;
    
    API_TRACE(API_TR_ENTRY, "pa60MoveLong", 0);
    API_TRACE(API_TR_PTR, "dbc_block_ptr", &dbc_block_ptr);
    API_TRACE(API_TR_PTR, "stmt_block_ptr", &stmt_block_ptr);
    API_TRACE(API_TR_PTR, "appl_desc_ptr", &appl_desc_ptr);
    API_TRACE(API_TR_PTR, "impl_desc_ptr", &impl_desc_ptr);
    API_TRACE(API_TR_UWORD, "icol", &icol);
    API_TRACE(API_TR_UWORD, "number_cols", &number_cols);

    API_ASSERT_PTR( appl_desc_ptr );
    API_ASSERT_PTR( impl_desc_ptr );
    API_ASSERT( appl_desc_ptr->descType == API_DESC_TYPE_AD );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );
    
    dbc_special_ptr = &stmt_block_ptr -> dbc_special;
    get_next = FALSE;   
    if (icol == 0) {
        icol = 1;
    }; /* if */
    
    for ( i=icol; i <= (SWORD) number_cols; i++ ) {

        appl_rec_ptr = pa20GetRecord( appl_desc_ptr, i );
        API_ASSERT_PTR( appl_rec_ptr );
        impl_rec_ptr = pa20GetRecord( impl_desc_ptr, i );
        API_ASSERT_PTR( impl_rec_ptr );

        api_retcode = API_OK;
        oldcbValue = 0;
        if ( appl_rec_ptr -> bound != API_TRUE
             || impl_rec_ptr -> esqIO == CPR_INFO_INPUT) 
            continue;
        if (!apgislong( impl_rec_ptr-> type ))
            continue;
        if (apgisindi( impl_rec_ptr-> optimizeNullPtr )) {
            api_retcode = pa20SMap( impl_desc_ptr,
                                    i,
                                    dbc_special_ptr, 
                                    appl_rec_ptr->conciseType,
                                    0, /* scale dummy */
                                    PA20_DATA_PTR(appl_rec_ptr),
                                    appl_rec_ptr->length,
                                    PA20_OCTETLENGTH_PTR(appl_rec_ptr));
            continue;
        }
        API_TRACE(API_TR_SWORD, "col", &i );
        /*
        API_TRACE(API_TR_SDWORD, "moved_len", &attr_ptr -> moved_len);
        API_TRACE(API_TR_SWORD, "last_data", &attr_ptr -> last_data);
        */
        fCType     = appl_rec_ptr -> conciseType;
        rgbValue   = PA20_DATA_PTR(appl_rec_ptr);
        cbValueMax = appl_rec_ptr -> length;
        /* ADIS 1001662
        if ( appl_rec_ptr -> octetLengthPtr != NULL )
            pcbValue = appl_rec_ptr -> octetLengthPtr;
        else
        */
            pcbValue = &cbValue;

        *pcbValue = 0;
	rbcolno = icol;

        API_TRACE(API_TR_SWORD, "rbcolno", &rbcolno);
            api_retcode = pa20SMap( impl_desc_ptr,
                                    i,
                                    dbc_special_ptr, 
                                    fCType,
                                    0, /* scale dummy */
                                    rgbValue,
                                    cbValueMax,
                                    pcbValue);
            oldcbValue = *pcbValue;
            API_TRACE(API_TR_SDWORD, "oldcbValue", &oldcbValue);		 
        if ( api_retcode == API_OK
             && impl_rec_ptr -> lastData != API_TRUE
             || get_next) {
            SQLLEN cbV;
            UCHAR FAR *rgbV;	    	 
            SQLLEN cbVM;
            cbV = *pcbValue;
            rgbV = rgbValue;
            rgbV += cbV;
            cbVM = cbValueMax - cbV;
            *pcbValue = 0;
            get_next = FALSE;
            while (api_retcode == API_OK) {
                /*
                API_TRACE(API_TR_SDWORD, "moved_len", &attr_ptr -> moved_len);
                */
                API_TRACE(API_TR_SDWORD, "*pcbV", &cbV);
                API_TRACE(API_TR_SDWORD, "cbVM", &cbVM);		 
                col = (SWORD)i;
                api_retcode = apegetl( &dbc_block_ptr->esqblk,
                                       dbc_block_ptr,
                                       stmt_block_ptr,
                                       impl_desc_ptr,
                                       &col );
                if (PA_ENV_SQLCODE != PA_OK)
                    api_retcode = API_NOT_OK;
                if (api_retcode == API_OK ) {			      
                    impl_rec_ptr -> movedLen = 0;
                    api_retcode = pa20SMap( impl_desc_ptr,
                                            i,
                                            dbc_special_ptr,
                                            fCType,
                                            0, /* scale dummy */
                                            rgbV,
                                            cbVM,
                                            &cbV );
                    if (cbV != SQL_NO_TOTAL) {
                        *pcbValue += cbV;
                    } else {
                        *pcbValue = SQL_NO_TOTAL;
                    }; /* else */
                    cbVM -= cbV;
                    rgbV += cbV;
                    cbV = 0;
                    API_TRACE(API_TR_SDWORD, "pcbValue", pcbValue);
                    if (oldcbValue > 0) {
                        /* set from restbuffer of last from apmsmap */
                        if (api_retcode == API_OK)
                            *pcbValue += oldcbValue;
                        oldcbValue = 0;
                    }
                    if (col <= 0) {
                        if (api_retcode == API_OK) {
                            *pcbValue += cbV;
                            get_next = TRUE;
                        }
                        break;
                    }; /* if */
                }; /* if */
            }; /* while */
        }; /* if */
        API_TRACE(API_TR_SDWORD, "*pcbValue", pcbValue);
        if ( appl_rec_ptr->octetLengthPtr ) {
            /* ADIS 1001662 */
            if ( api_retcode != API_NOT_OK
                 && api_retcode != API_NO_DATA_FOUND) {
                *PA20_OCTETLENGTH_PTR(appl_rec_ptr) = *pcbValue;
            }; /* if */
            if (api_retcode == API_TRUNCATE) {
                if (*PA20_OCTETLENGTH_PTR(appl_rec_ptr) == SQL_NO_TOTAL) {
                    API_TRACE( API_TR_SDWORD,
                               "dbc_block_ptr->lvctrunc",
                               &dbc_block_ptr->lvctrunc);
                    switch ( dbc_block_ptr->lvctrunc ) {
                    case (API_LONG_MAX): {
                        *pcbValue = SAPDB_MAX_INT4;    /* PTS 1119281 */
                        break;
                    }
                    case (API_VALUEMAX): {
                        *pcbValue = cbValueMax;
                        break;
                    }
                    case (API_VALUEMAX_PLUSONE): {
                        *pcbValue = cbValueMax+1;
                        break;
                    }
                    case (API_SQL_NO_TOTAL): {
                        break;
                    }
                    default: {
                        *pcbValue = dbc_block_ptr->lvctrunc;
                        break;
                    }
                    }; /* switch */
                }; /* if */
            }; /* if */
        }; /* if */
    }; /* for */
    
    API_TRACE(API_TR_SDWORD, "*pcbValue", pcbValue);
    API_TRACE(API_TR_EXIT, "pa60MoveLong", 0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
  
    return(api_retcode);
} /* pa60MoveLong */


void pa60_GetSpIndex( struct SQLLD *sqlldp,
                      SWORD         icol,
                      SWORD        *spIndexP )
{
    SWORD i;
    SWORD pcIndex = -99;
    for ( i = 1; i <= sqlldp->ldmaxo; i++ ) {
        if ( sqlldp->ldpc[i-1].ldcolno == icol ) {
            pcIndex = i;
            break;
        }; /* if */
    }; /* for */    
    for ( i = 1; i <= sqlldp->ldmaxo; i++ ) {
        if ( sqlldp->ldsp[i-1].ld.n.ld_valind == pcIndex ) {
            *spIndexP = i;
            break;
        }; /* if */
    }; /* for */
} /* pa60_GetSpIndex */


/* Used by SQLGetData if special stmt-attr SQLATTR_READ_LONG_POS is <> 0
 */
API_RETCODE pa60MoveLongPos( tpa40DBC   *dbc_block_ptr,
                             tpa60Stmt  *stmt_block_ptr,
                             tpa20Desc  *appl_desc_ptr,
                             tpa20Desc  *impl_desc_ptr,
                             UWORD       icol,
                             UDWORD      long_pos )
{
    API_RETCODE api_retcode = API_OK;
    tpa62DBCSpecial *dbc_special_ptr;  
    SWORD   fCType;
    PTR     rgbValue;
    SQLLEN  cbValueMax;
    SQLLEN *pcbValue=NULL;
    SQLLEN  cbValue = 0;
    SQLLEN  oldcbValue;
    SWORD   col;
    SWORD   rbcolno;
    SWORD   odbcSqlType;
    UDWORD  odbcSqlLen;
    UWORD   odbcSqlScale;
    UDWORD  sql_len;
    SWORD   keepMovedLen;
    tpa21DescRecord *appl_rec_ptr;
    tpa21DescRecord *impl_rec_ptr;
    UWORD   terminatorSize;
    UDWORD  long_pos2;

    API_TRACE(API_TR_ENTRY, "pa60MoveLongPos", 0);
    API_TRACE(API_TR_PTR, "dbc_block_ptr", &dbc_block_ptr);
    API_TRACE(API_TR_PTR, "stmt_block_ptr", &stmt_block_ptr);
    API_TRACE(API_TR_PTR, "appl_desc_ptr", &appl_desc_ptr);
    API_TRACE(API_TR_PTR, "impl_desc_ptr", &impl_desc_ptr);
    API_TRACE(API_TR_UWORD, "icol", &icol);
    API_TRACE(API_TR_UDWORD, "long_pos", &long_pos);

    API_ASSERT_PTR( appl_desc_ptr );
    API_ASSERT_PTR( impl_desc_ptr );
    API_ASSERT( appl_desc_ptr->descType == API_DESC_TYPE_AD );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );
    
    dbc_special_ptr = &stmt_block_ptr -> dbc_special;

    appl_rec_ptr = pa20GetRecord( appl_desc_ptr, icol );
    API_ASSERT_PTR( appl_rec_ptr );
    impl_rec_ptr = pa20GetRecord( impl_desc_ptr, icol );
    API_ASSERT_PTR( impl_rec_ptr );

    api_retcode = API_OK;
    oldcbValue = 0;
    if ( appl_rec_ptr -> bound != API_TRUE
         || impl_rec_ptr -> esqIO == CPR_INFO_INPUT) 
        goto exit;
    if (!apgislong( impl_rec_ptr-> type ))
        goto exit;
    if (apgisindi( impl_rec_ptr-> optimizeNullPtr )) {
        api_retcode = pa20SMap( impl_desc_ptr,
                                icol,
                                dbc_special_ptr, 
                                appl_rec_ptr->conciseType,
                                0, /* scale dummy */
                                PA20_DATA_PTR( appl_rec_ptr ),
                                appl_rec_ptr->length,
                                PA20_OCTETLENGTH_PTR( appl_rec_ptr ) );
        goto exit;
    }; /* if */
    fCType     = appl_rec_ptr -> conciseType;
    rgbValue   = PA20_DATA_PTR( appl_rec_ptr );
    cbValueMax = appl_rec_ptr -> length;

    pcbValue = &cbValue;
    *pcbValue = 0;

    api_retcode = apgstys( impl_rec_ptr->esqType,
                           impl_rec_ptr->esqLength,
                           impl_rec_ptr->esqScale,
                           &odbcSqlType,
                           &odbcSqlLen,
                           &odbcSqlScale);
    
    if (apgislong( odbcSqlType )) {
        sql_len = apggvstrl (impl_rec_ptr->optimizePtr);
    } else {
        api_retcode = API_NOT_OK; /* no long value */
        goto exit;
    }; /* else */
    
    rbcolno = icol;

    keepMovedLen = API_FALSE;
    col = (SWORD) icol;

    /* recompute long_pos due to encoding of client/host
       http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
    long_pos2 = pa60_ComputePosition (long_pos, impl_rec_ptr, fCType, &terminatorSize);

    if (long_pos2 <= sql_len) {
        /* our start position is inside the buffer impl_rec_ptr->optimizePtr */
        impl_rec_ptr->movedLen += long_pos2 - terminatorSize;
        if (rbcolno >= icol || rbcolno == 0) {
            api_retcode = pa20SMap( impl_desc_ptr,
                                    icol,
                                    dbc_special_ptr, 
                                    fCType,
                                    0, /* scale dummy */
                                    rgbValue,
                                    cbValueMax,
                                    pcbValue);
            /* impl_rec_ptr->movedLen = 0; */
            oldcbValue = *pcbValue;
            API_TRACE(API_TR_SDWORD, "oldcbValue", &oldcbValue);		 
        }
    } else {
        /* we have to send position with next getval */
        SWORD spIndex;
        
        pa60_GetSpIndex( stmt_block_ptr->rasqlldp,
                       icol,
                       &spIndex );
	if (long_pos > (UDWORD) stmt_block_ptr->rasqlldp->ldsp[spIndex-1].ld.o.ld_maxlen)
	  stmt_block_ptr->rasqlldp->ldsp[spIndex-1].ld.o.ld_intern_pos =
            (tsp00_Int4) 0;
	else
	  stmt_block_ptr->rasqlldp->ldsp[spIndex-1].ld.o.ld_intern_pos =
            pa60_ComputeKernelPosition (long_pos, impl_rec_ptr);
    }; /* else */
    
    if ( api_retcode == API_OK
         && impl_rec_ptr -> lastData != API_TRUE
         && rbcolno == icol ) {
        SQLLEN cbVM;
        SQLLEN cbV    = *pcbValue;
        UCHAR *rgbV   = rgbValue;
        rgbV         += cbV;
        cbVM          = cbValueMax - cbV;
        *pcbValue     = 0;
        while ( api_retcode == API_OK && !impl_rec_ptr->lastData
                && col == icol) {
            /*
              API_TRACE(API_TR_SDWORD, "moved_len", &attr_ptr -> moved_len);
            */
            API_TRACE(API_TR_SDWORD, "*pcbV", &cbV);
            API_TRACE(API_TR_SDWORD, "cbVM", &cbVM);		 
                api_retcode = apegetl( &dbc_block_ptr->esqblk,
                                       dbc_block_ptr,
                                       stmt_block_ptr,
                                       impl_desc_ptr,
                                       &col );
                if (PA_ENV_SQLCODE != PA_OK)
                    api_retcode = API_NOT_OK;
                if (api_retcode == API_OK ) {
                    if (keepMovedLen) {
                        keepMovedLen = API_FALSE; /* keep it only the first
                                                   * time */
                    } else {
                        impl_rec_ptr -> movedLen = 0;
                    }; /* else */
                    api_retcode = pa20SMap( impl_desc_ptr,
                                            icol,
                                            dbc_special_ptr,
                                            fCType,
                                            0, /* scale dummy */
                                            rgbV,
                                            cbVM,
                                            &cbV );
                    if (cbV != SQL_NO_TOTAL) {
                        *pcbValue += cbV;
                    } else {
                        *pcbValue = SQL_NO_TOTAL;
                    }; /* else */
                    cbVM -= cbV;
                    rgbV += cbV;
                    cbV = 0;
                    API_TRACE(API_TR_SDWORD, "pcbValue", pcbValue);
                    if (oldcbValue > 0) {
                        /* set from restbuffer of last from apmsmap */
                        if (api_retcode == API_OK)
                            *pcbValue += oldcbValue;
                        oldcbValue = 0;
                    }
                }; /* if */
        }; /* while */
    }; /* if */
    if (*pcbValue == 0) {
        api_retcode = API_NO_DATA_FOUND;
    }; /* if */
    API_TRACE(API_TR_SDWORD, "*pcbValue", pcbValue);
    if ( appl_rec_ptr->octetLengthPtr ) {
        /* ADIS 1001662 */
        if ( api_retcode != API_NOT_OK
             && api_retcode != API_NO_DATA_FOUND) {
            *(PA20_OCTETLENGTH_PTR(appl_rec_ptr)) = *pcbValue;
        }; /* if */
        if (api_retcode == API_TRUNCATE) {
            if (*PA20_OCTETLENGTH_PTR(appl_rec_ptr) == SQL_NO_TOTAL) {
                API_TRACE( API_TR_SDWORD,
                           "dbc_block_ptr->lvctrunc",
                           &dbc_block_ptr->lvctrunc);
                switch ( dbc_block_ptr->lvctrunc ) {
                case (API_LONG_MAX): {
                    *pcbValue = SAPDB_MAX_INT4;	   
                    break;
                }
                case (API_VALUEMAX): {
                    *pcbValue = cbValueMax;
                    break;
                }
                case (API_VALUEMAX_PLUSONE): {
                    *pcbValue = cbValueMax+1;
                    break;
                }
                case (API_SQL_NO_TOTAL): {
                    break;
                }
                default: {
                    *pcbValue = dbc_block_ptr->lvctrunc;
                    break;
                }
                }; /* switch */
            }; /* if */
        }; /* if */
    }; /* if */

 exit:
    
    API_TRACE(API_TR_SDWORD, "*pcbValue", pcbValue);
    API_TRACE(API_TR_EXIT, "pa60MoveLongPos", 0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    
    return(api_retcode);
} /* pa60MoveLongPos */


API_RETCODE pa60Param( tpa20Desc *appl_desc_ptr,
                       tpa20Desc *impl_desc_ptr,
                       tpa60Stmt *stmt_block_ptr,
                       PTR       *prgbValue,
                       BOOLEAN    nolong )
{
    API_RETCODE api_retcode;
    SWORD param, number_parms;
    BOOLEAN found=FALSE;

    tpa21DescRecord *appl_rec_ptr = NULL;
    tpa21DescRecord *impl_rec_ptr;
    
    API_TRACE(API_TR_ENTRY,"pa60Param",0);
    API_TRACE(API_TR_PTR,"appl_desc_ptr", &appl_desc_ptr);
    API_TRACE(API_TR_PTR,"impl_desc_ptr", &impl_desc_ptr);
    API_TRACE(API_TR_PTR,"stmt_block_ptr", &stmt_block_ptr);
    API_TRACE(API_TR_PTR,"prgbValue", &prgbValue);
    API_TRACE(API_TR_UCHAR, "nolong", &nolong);

    API_ASSERT_PTR( appl_desc_ptr );
    API_ASSERT_PTR( impl_desc_ptr );
    
    api_retcode = API_OK;   
   
    param = stmt_block_ptr->next_param;    
    if (param == -1)
        param = 0;      
    else 
        param++;
    number_parms = stmt_block_ptr->number_parms;
    
    API_TRACE(API_TR_SWORD, "param", &param);
    API_TRACE(API_TR_SWORD, "number_parms", &number_parms);
    
    for (; param < (SWORD) number_parms; param++ ) {
        appl_rec_ptr = pa20GetRecord( appl_desc_ptr, (UWORD) (param+1) );
        API_ASSERT_PTR( appl_rec_ptr );
        impl_rec_ptr = pa20GetRecord( impl_desc_ptr, (UWORD) (param+1) );
        API_ASSERT_PTR( impl_rec_ptr );
        
        API_TRACE(API_TR_UWORD, "param", &param);
        if (impl_rec_ptr->esqIO == CPR_INFO_OUTPUT)
            continue;
        if (appl_rec_ptr -> bound != API_TRUE) {
            continue;
        }
        API_TRACE(API_TR_UWORD, "bound param", &param);
        if (appl_rec_ptr -> octetLengthPtr != NULL) {
            API_TRACE( API_TR_SDWORD, "*octetLengthPtr",
                       *PA20_OCTETLENGTH_PTR(appl_rec_ptr) );
            if (*PA20_OCTETLENGTH_PTR(appl_rec_ptr) == SQL_DATA_AT_EXEC ||
                *PA20_OCTETLENGTH_PTR(appl_rec_ptr)
                <= SQL_LEN_DATA_AT_EXEC_OFFSET) {
                if (impl_rec_ptr -> isNull == API_TRUE)
                    continue;
                if (nolong) {
                    if ( apgislong( impl_rec_ptr -> type))
                        continue;
                }	
                else {	
                    if ( !apgislong( impl_rec_ptr -> type))
                        continue;
                }	
                found = TRUE;
                break;
            }	
        }	 
    } /* of for loop */   
    if (!found)
        stmt_block_ptr->next_param = -1;
    else {
        if (prgbValue != NULL) {
            *prgbValue = appl_rec_ptr->dataPtr;
        }
        stmt_block_ptr->next_param = param;
    }
   
    API_TRACE(API_TR_SWORD,"next_param", &stmt_block_ptr->next_param);
    API_TRACE(API_TR_EXIT,"pa60Param",0);
    API_TRACE(API_TR_PTR, "prgbValue", prgbValue);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
  
    return(api_retcode);
} /* pa60Param */


API_RETCODE pa60PutError( SQLHSTMT          hstmt,
                          UWORD          sqlState,
                          tpa41ESQBlock *esqblkPtr)
{
    API_RETCODE apiRetcode = API_OK;
    tpa60Stmt  *stmtPtr    = NULL;
    SDWORD      nativeErr  = 0;
    
    API_TRACE( API_TR_ENTRY, "pa60PutError", 0);
    API_TRACE( API_TR_UWORD, "sqlState", &sqlState );
    API_TRACE( API_TR_PTR, "esqblkPtr", &esqblkPtr );
    
    stmtPtr    = (tpa60Stmt*) hstmt;
    apiRetcode = pa30PutError( stmtPtr->diagArea, sqlState, esqblkPtr );
    if (esqblkPtr != NULL) {
        nativeErr = esqblkPtr->esq_sql_code;
        if (nativeErr == -708 || nativeErr == -807 || nativeErr == -813 ) {
            /* connection is broken. Set connect attr connectionDead. */
            ((tpa40DBC*) stmtPtr->parent_hdbc)->connectionDead = API_TRUE;
        }; /* if */
    }; /* if */
    
    API_TRACE( API_TR_EXIT, "pa60PutError", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

    return apiRetcode;
} /* pa60PutError */


SQLRETURN pa60PutESQError( SQLHSTMT hstmt, API_RETCODE  api_retcode)
{
    SQLRETURN    retcode;
    API_RETCODE  ret = API_OK;
    
    API_TRACE(API_TR_ENTRY, "pa60PutESQError", 0);
    switch (api_retcode) {          /* ODBC book page 280         */
    case (API_OK) : {
        retcode = SQL_SUCCESS;
        break;
    }
     /* SQL_C_BIN -> SQL_WCHAR, odd byte number
        http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
    case (API_TRUNCATE_22001): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_22001, NULL);
        break;
    }
    case (API_TRUNCATE): {
        retcode = SQL_SUCCESS_WITH_INFO;
        ret = pa60PutError( hstmt, API_ODBC_01004, NULL);
        break;
    }
    case (API_DATA_LOSS): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_22003, NULL);
        break;
    }
    case (API_FEW_BOUNDS): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_07001, NULL);
        break;
    }
    case (API_NOT_NUMBER): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_22005, NULL);
        break;
    }
    case (API_NOT_DATE): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_22008, NULL);
        break;
    }
    case (API_NEED_DATA): {
        retcode = SQL_NEED_DATA;
        break;
    }
    case (API_NO_DATA_FOUND): {
        retcode = SQL_NO_DATA_FOUND;
        break;
    }
    case (API_INVALID): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_07006, NULL);
        break;
    }
    case (API_INVALID_LENGTH): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_S1090, NULL);
        break;
    }
    case (API_CONSTRAINT_VIOLATION): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_23000, NULL);
        break;
    }
    case (API_TIME_OUT): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_S1T00, NULL);
        break;
    }
    case (API_NO_INDICATOR): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_22002, NULL);
        break;
    }
    case (API_INVALID_CHAR): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_22008, NULL);
        break;
    }
    case (API_FRACTIONAL_TRUNC): {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_01S07, NULL);
        break;
    }

    default: {
        retcode = SQL_ERROR;
        ret = pa60PutError( hstmt, API_ODBC_S1000, NULL);
        break;
    }
    }
    if (ret != API_OK) {
        retcode = SQL_ERROR;
    }
    API_TRACE(API_TR_EXIT, "pa60PutESQError", 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    return (retcode);
} /* pa60PutESQError */


API_RETCODE pa60ResetError( SQLHSTMT hstmt )
{
    API_RETCODE apiRetcode = API_OK;
    tpa60Stmt    *stmtPtr;
    
    API_TRACE( API_TR_ENTRY, "pa60ResetError", 0);
    
    stmtPtr = (tpa60Stmt*) hstmt;
    if (stmtPtr->diagArea!=NULL) {
        apiRetcode = pa30FreeDiagArea( stmtPtr->diagArea );
        stmtPtr->errorIndex = 0;
    }; /* if */
    
    API_TRACE( API_TR_EXIT, "pa60ResetError", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

    return apiRetcode;
} /* pa60ResetError */


void pa60SetDefaultDesc( SQLHSTMT hstmt, tpa20Desc *appl_desc )
{
    tpa60Stmt *stmtPtr;
    
    API_TRACE( API_TR_ENTRY, "pa60SetDefaultAPD", 0 );
    API_TRACE( API_TR_HANDLE, "hstmt", &hstmt );

    API_ASSERT( pa60VerifyStmt( hstmt ) == API_OK );
    
    stmtPtr = (tpa60Stmt*) hstmt;
    if ( stmtPtr->apdPtr == appl_desc ) {
        stmtPtr->apdPtr = &stmtPtr->defaultApd;
    } else if ( stmtPtr->ardPtr == appl_desc ) {
        stmtPtr->ardPtr = &stmtPtr->defaultArd;
    } else {
        API_ASSERT( 0 /* appl_desc is not associated with hstmt */ );
    }; /* else */
    
    API_TRACE( API_TR_EXIT, "pa60SetDefaultAPD", 0 );
} /* pa60SetDefaultDesc */


API_RETCODE pa60VerifyStmt( SQLHSTMT hstmt )
{
    tpa60Stmt FAR * stmtPtr;
    API_RETCODE apiRetcode = API_OK;

    API_TRACE(API_TR_ENTRY,"pa60VerifyStmt",0);
    API_TRACE(API_TR_HANDLE,"hstmt",&hstmt);

    stmtPtr = (tpa60Stmt*) hstmt;
    if (stmtPtr == NULL) {
        apiRetcode = API_NOT_OK;
    } else {
        if (stmtPtr -> type != API_STMT_TYPE) {
            apiRetcode = API_NOT_OK;
        }; /* if */
    }; /* else */

    API_TRACE( API_TR_EXIT, "pa60VerifyStmt", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

    return (apiRetcode);
} /* pa60VerifyStmt */

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

/* apmastmt() - API miscellaneous function, allocate statement */

API_RETCODE pa60apmastmt( SQLHDBC hdbc, SQLHSTMT *phstmt )
{
  SQLHSTMT     hstmt;
  SQLHSTMT     next_hstmt;
  API_HANDLE   input_sqlda_handle;
  API_HANDLE   output_sqlda_handle;
  tpr05_String   *stmt_str;
  API_RETCODE  retcode;
  tpa40DBC    *dbc_block_ptr;
  tpa60Stmt   *stmt_block_ptr;
  /*  UCHAR       *cursor_ptr; */
  UWORD        sqlStateDummy;
  UCHAR        cursorStr[API_ODBC_CURSOR_LEN+1];
  
  API_TRACE(API_TR_ENTRY,"pa60apmastmt",0);
  API_TRACE(API_TR_PTR,"phstmt",&phstmt);

  hstmt = (SQLHSTMT) apdallo( (DWORD) sizeof(tpa60Stmt));
  if (pa70AllocSQLDA( &input_sqlda_handle, (UWORD) API_SQLDA_NUM) != API_OK ) {
    input_sqlda_handle = 0;
  }
  if (pa70AllocSQLDA( &output_sqlda_handle, (UWORD) API_SQLDA_NUM) != API_OK) {
    output_sqlda_handle = 0;
  }

  stmt_str = pr05IfCom_String_NewDynString (300, NULL);   /* default size */

  if ( hstmt == 0
       || input_sqlda_handle == 0
       || output_sqlda_handle == 0
       || stmt_str == API_NULL_HANDLE ) {
      hstmt = SQL_NULL_HSTMT;
      retcode =  API_NOT_OK;
  }
  else {
    retcode = API_OK;
    stmt_block_ptr = (tpa60Stmt*) hstmt;
    API_ASSERT_PTR( stmt_block_ptr );
    dbc_block_ptr = (tpa40DBC*) hdbc;
    stmt_block_ptr -> type = API_STMT_TYPE;
    stmt_block_ptr -> state = API_STMT_ALLOCATED;
    stmt_block_ptr -> reconnect_status = API_FALSE;
    stmt_block_ptr -> dbc_special.special = API_SPEC_NOT;
    stmt_block_ptr -> dbc_special.blocksize = dbc_block_ptr -> blocksize;
#define API_DBKEMPTY (-1)
    stmt_block_ptr -> dbc_special.kernelversion = (UWORD)API_DBKEMPTY;
    stmt_block_ptr -> stmt_type = API_ATTRIB_INIT;
    stmt_block_ptr -> need_data = FALSE;
    stmt_block_ptr -> no_result_set  = FALSE;
    stmt_block_ptr -> row_not_found  = FALSE;
    stmt_block_ptr -> errorIndex     = 0;
    stmt_block_ptr -> setPosNeedData = FALSE;

    /* SQLSetStmtOption / SQLGetStmtOption */
    API_MEMCPY( &stmt_block_ptr->stmtopt,
                &dbc_block_ptr->constmtopt, sizeof(stmt_block_ptr->stmtopt));
    if (stmt_block_ptr->stmtopt.async_enable == SQL_ASYNC_ENABLE_ON) {
        pa61SetStmtAttr( (SQLHSTMT) stmt_block_ptr,
                         API_STMT_ALLOCATED,
                         SQL_ASYNC_ENABLE,
                         (PTR) SQL_ASYNC_ENABLE_ON,
                         0,
                         &stmt_block_ptr->stmtopt,
                         &sqlStateDummy );
    }; /* if */

    /* 3.0/3.5 */
    stmt_block_ptr->stmtopt.cursorScrollable  = SQL_NONSCROLLABLE;
    stmt_block_ptr->stmtopt.cursorSensitivity = SQL_UNSPECIFIED;
    stmt_block_ptr->stmtopt.fetchBookmarkPtr  = NULL;
    stmt_block_ptr->stmtopt.metadataId        = SQL_FALSE;

    /* initialize descriptors */
    pa20InitApplDesc( (SQLHDESC) &stmt_block_ptr->defaultApd,
                      hdbc,
                      (SQLHSTMT) stmt_block_ptr );
    pa20InitApplDesc( (SQLHDESC) &stmt_block_ptr->defaultArd,
                      hdbc,
                      (SQLHSTMT) stmt_block_ptr );
    pa20InitImplDesc( (SQLHDESC) &stmt_block_ptr->ipd,
                      hdbc,
                      (SQLHSTMT) stmt_block_ptr,
                      API_DESC_TYPE_IPD );
    pa20InitImplDesc( (SQLHDESC) &stmt_block_ptr->ird,
                      hdbc,
                      (SQLHSTMT) stmt_block_ptr,
                      API_DESC_TYPE_IRD );
    stmt_block_ptr->apdPtr = &stmt_block_ptr->defaultApd;
    stmt_block_ptr->ardPtr = &stmt_block_ptr->defaultArd;

    pa20AllocRecords( (SQLHDESC) stmt_block_ptr->apdPtr,
                      (UWORD) API_BIND_NUM );
    pa20AllocRecords( (SQLHDESC) &stmt_block_ptr->ipd,
                      (UWORD) API_BIND_NUM );
    pa20AllocRecords( (SQLHDESC) stmt_block_ptr->ardPtr,
                      (UWORD) API_BIND_NUM );
    pa20AllocRecords( (SQLHDESC) &stmt_block_ptr->ird,
                      (UWORD) API_BIND_NUM );
    
    /* SQLParamOptions */
    stmt_block_ptr -> ipd.header.arraySize = 1;
    stmt_block_ptr -> ipd.header.previousArraySize = 1;
    
    /* SQLSetPos */
    stmt_block_ptr -> setpos_irow = 0;
    pa60_InitSetPosSpecial( &stmt_block_ptr -> setPosSpecial );
    
    stmt_block_ptr -> parent_hdbc = hdbc;
    stmt_block_ptr -> next_hstmt = dbc_block_ptr -> child_hstmt;
    next_hstmt = dbc_block_ptr -> child_hstmt;
    stmt_block_ptr -> prev_hstmt = 0;
    dbc_block_ptr -> child_hstmt = hstmt;
    pa40GetStmtId( dbc_block_ptr, &stmt_block_ptr -> statement_id );
    /* allocate tpr05_String, same encoding as in connection */
    if (dbc_block_ptr->unicode_catalog) {
      stmt_block_ptr->cursor_name
        = pr05IfCom_String_NewDynString (sizeof(SQLWCHAR)*(API_ODBC_CURSOR_LEN+1),
                                         sp77encodingUCS2Native);
      stmt_block_ptr->table_name
        = pr05IfCom_String_NewDynString (sizeof(SQLWCHAR)*(API_TABLE_NAME_MAX*3+sizeof("''.''.''")),
                                         sp77encodingUCS2Native);
    }
    else {
      stmt_block_ptr->cursor_name
        = pr05IfCom_String_NewDynString (API_ODBC_CURSOR_LEN+1, sp77encodingAscii);

      stmt_block_ptr->table_name
        = pr05IfCom_String_NewDynString (API_TABLE_NAME_MAX*3+sizeof("''.''.''"), sp77encodingAscii);
    }

    if (!pa06UniqueName ((UCHAR*) "SQL_CURSOR_0000",
                         cursorStr, sizeof(cursorStr),
                         stmt_block_ptr->statement_id))
        retcode = API_NOT_OK;

    pr05IfCom_String_strcatP (stmt_block_ptr->cursor_name, (char*) cursorStr,
                              (tsp00_Uint4) API_STRLEN (cursorStr),
                              sp77encodingAscii);

    /*
    cursor_ptr = stmt_block_ptr -> cursor_name;
    {
       WORD len;
       if (dbc_block_ptr->unicode_catalog == TRUE)
           len = (sizeof(stmt_block_ptr -> cursor_name)-1) / 2;
       else
           len = (sizeof(stmt_block_ptr -> cursor_name)-1);
       if (!pa06UniqueName((UCHAR*) "SQL_CURSOR_0000", cursor_ptr, len,
                           stmt_block_ptr -> statement_id ))
           retcode = API_NOT_OK;
    }
    */
    stmt_block_ptr -> number_rows = 0;
    stmt_block_ptr -> fetch_pos = 0;
    stmt_block_ptr -> number_cols = 0;
    stmt_block_ptr -> col_size = 0;
    stmt_block_ptr -> parm_size = 0;
    stmt_block_ptr -> col_handle = 0;
    stmt_block_ptr -> parm_handle = 0;
    stmt_block_ptr -> input_sqlda_handle = input_sqlda_handle;
    stmt_block_ptr -> output_sqlda_handle = output_sqlda_handle;
    stmt_block_ptr -> rasqlldp = NULL;
    stmt_block_ptr -> stmt_str_handle = stmt_str;
    /*    stmt_block_ptr -> allocated_stmt_len = 1; */
    stmt_block_ptr -> hstmt_parent = SQL_NULL_HSTMT;
    stmt_block_ptr -> hstmt_setpos = SQL_NULL_HSTMT;
    stmt_block_ptr -> hstmt_extfetch = SQL_NULL_HSTMT;
    stmt_block_ptr -> hrowstat = API_NULL_HANDLE;
    stmt_block_ptr -> stmt_function_code = csp1_nil_fc;
    stmt_block_ptr -> async.hLocalValues = API_NULL_HANDLE;
    stmt_block_ptr -> diagArea = NULL;
    stmt_block_ptr -> fetchRetcode = SQL_SUCCESS;
    
    pa06InitNullParseId( stmt_block_ptr->stmt_parsid );
    pa06InitNullParseId( stmt_block_ptr->close_parsid );
    pa06InitNullParseId( stmt_block_ptr->fetch_first_parsid );
    pa06InitNullParseId( stmt_block_ptr->fetch_last_parsid );
    pa06InitNullParseId( stmt_block_ptr->fetch_next_parsid );
    pa06InitNullParseId( stmt_block_ptr->fetch_prior_parsid );
    pa06InitNullParseId( stmt_block_ptr->fetch_relative_parsid );
    pa06InitNullParseId( stmt_block_ptr->fetch_absolute_parsid );
    
    if (next_hstmt !=0) {
       stmt_block_ptr = (tpa60Stmt FAR *) apdlock (next_hstmt);
       API_ASSERT_PTR(stmt_block_ptr);
       stmt_block_ptr -> prev_hstmt = hstmt;
    }
  }
  
  *phstmt = hstmt;

  API_TRACE(API_TR_EXIT,"pa60apmastmt",0);
  API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
  API_TRACE(API_TR_HANDLE,"*phstmt",phstmt);

  return(retcode);
} /* pa60apmastmt */


/* pa60apmfstmt() - API miscellaneous function, free statement */

API_RETCODE pa60apmfstmt ( SQLHSTMT hstmt)
{    
    API_RETCODE  retcode = API_OK;
    tpa40DBC    *dbc_block_ptr;
    tpa60Stmt   *stmt_block_ptr;
    SQLHSTMT     next_hstmt;
    SQLHSTMT     prev_hstmt;
    SQLHENV      parent_hdbc;

    API_TRACE(API_TR_ENTRY,"pa60apmfstmt",0);
    API_TRACE(API_TR_HANDLE,"hstmt",&hstmt);
    stmt_block_ptr = (tpa60Stmt FAR *) apdlock(hstmt);
    API_ASSERT_PTR(stmt_block_ptr);

    pr05IfCom_String_DeleteString (stmt_block_ptr->cursor_name);
    pr05IfCom_String_DeleteString (stmt_block_ptr->table_name);
    stmt_block_ptr->cursor_name = stmt_block_ptr->table_name = NULL;
    
    /* these may be explicit allocated descriptors. The parentStmt gets
     * invalid */
    stmt_block_ptr -> ardPtr -> parentStmt = SQL_NULL_HSTMT;
    stmt_block_ptr -> apdPtr -> parentStmt = SQL_NULL_HSTMT;
    /* free records etc in the descriptors */
    if (pa20DeleteDesc( &stmt_block_ptr->defaultArd ) != API_OK) {
        retcode = API_NOT_OK;
    }; /* if */
    if (pa20DeleteDesc( &stmt_block_ptr->defaultApd ) != API_OK) {
        retcode = API_NOT_OK;
    }; /* if */  
    if (pa20DeleteDesc( &stmt_block_ptr->ipd ) != API_OK) {
        retcode = API_NOT_OK;
    }; /* if */  
    if (pa20DeleteDesc( &stmt_block_ptr->ird ) != API_OK) {
        retcode = API_NOT_OK;
    }; /* if */
  
    if (stmt_block_ptr->stmt_str_handle != API_NULL_HANDLE) {
        pr05IfCom_String_DeleteString (stmt_block_ptr->stmt_str_handle);
        stmt_block_ptr->stmt_str_handle = API_NULL_HANDLE;
        /*        stmt_block_ptr->allocated_stmt_len = 0; */
    }
  
    if (stmt_block_ptr->input_sqlda_handle != API_NULL_HANDLE) {
        pa70FreeSQLDA( stmt_block_ptr->input_sqlda_handle );
        stmt_block_ptr->input_sqlda_handle = API_NULL_HANDLE;
    }
  
    if (stmt_block_ptr->output_sqlda_handle != API_NULL_HANDLE) {
        pa70FreeSQLDA( stmt_block_ptr->output_sqlda_handle );
        stmt_block_ptr->output_sqlda_handle = API_NULL_HANDLE;
    }
    if (stmt_block_ptr->rasqlldp) {
        p03freeld(&stmt_block_ptr->rasqlldp);
        API_TRACE(API_TR_PTR, "rasqlldp", &stmt_block_ptr->rasqlldp);
    }
    if (stmt_block_ptr->col_handle != API_NULL_HANDLE) {
        apdfree(stmt_block_ptr->col_handle);
        stmt_block_ptr->col_handle = API_NULL_HANDLE;
    }
  
    if (stmt_block_ptr -> parm_handle != API_NULL_HANDLE) {
        apdfree(stmt_block_ptr->parm_handle);
        stmt_block_ptr->parm_handle = API_NULL_HANDLE;
    }
  
    if (stmt_block_ptr -> hrowstat != API_NULL_HANDLE) {
        apdfree(stmt_block_ptr -> hrowstat);
        stmt_block_ptr -> hrowstat = API_NULL_HANDLE;
    }

    pa60_FreeSetPosSpecial( &stmt_block_ptr -> setPosSpecial );
    
    next_hstmt = stmt_block_ptr -> next_hstmt;
    prev_hstmt = stmt_block_ptr -> prev_hstmt;
    parent_hdbc = stmt_block_ptr -> parent_hdbc;
  
    /* set invalid type. So next operation on this freed handle will return
     * SQL_INVALID_HANDLE (or crashes) */
    stmt_block_ptr -> type = API_INVALID_HANDLE_TYPE;
    apdfree(stmt_block_ptr);
  
    if (prev_hstmt == 0) {           /* change link in parent      */
        dbc_block_ptr = (tpa40DBC FAR *) apdlock(parent_hdbc);
        API_ASSERT_PTR(dbc_block_ptr);
        dbc_block_ptr -> child_hstmt = next_hstmt;
    }
    else {                          /* change link in prev        */
        stmt_block_ptr = (tpa60Stmt FAR *) apdlock(prev_hstmt);
        API_ASSERT_PTR(stmt_block_ptr);
        stmt_block_ptr -> next_hstmt = next_hstmt;
    }
    if (next_hstmt != 0) {           /* change link in next        */
        stmt_block_ptr = (tpa60Stmt FAR *) apdlock(next_hstmt);
        API_ASSERT_PTR(stmt_block_ptr);
        stmt_block_ptr -> prev_hstmt = prev_hstmt;
    }

    /* run over all hstmt-children of the dbc_block and remove the
       just freed hstmt */
    next_hstmt = ((tpa40DBC *) parent_hdbc)->child_hstmt;
    for(;next_hstmt != SQL_NULL_HSTMT;) {
        stmt_block_ptr = (tpa60Stmt*) next_hstmt;

        if (stmt_block_ptr->hstmt_parent == hstmt)
            stmt_block_ptr->hstmt_parent = SQL_NULL_HSTMT;
        if (stmt_block_ptr->hstmt_setpos == hstmt)
            stmt_block_ptr->hstmt_setpos = SQL_NULL_HSTMT;
        if (stmt_block_ptr->hstmt_extfetch == hstmt)
            stmt_block_ptr->hstmt_extfetch = SQL_NULL_HSTMT;
        
        next_hstmt = stmt_block_ptr->next_hstmt;
    }

  
    API_TRACE(API_TR_EXIT,"pa60apmfstmt",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);

    return(API_OK);

} /* pa60apmfstmt */


tpa20Desc* pa60GetAPD( tpa60Stmt* stmt )
{
    return ((stmt->apdPtr==NULL) ? &stmt->defaultApd : stmt->apdPtr );
} /* pa60GetAPD */


tpa20Desc* pa60GetARD( tpa60Stmt* stmt )
{
    return ((stmt->ardPtr==NULL) ? &stmt->defaultArd : stmt->ardPtr );
} /* pa60GetAPD */


API_RETCODE pa60CloseCursor( tpa40DBC  *dbc_block_ptr,
                             tpa60Stmt *stmt_block_ptr )
{
  /* in Ora mode, no close cursor is necessary, if the last fetch
     found no rows. this is not taken into account here. */
    apeclos( &dbc_block_ptr->esqblk,
             stmt_block_ptr -> cursor_name,
             dbc_block_ptr,
             stmt_block_ptr);

    return TRUE;
} /* pa60CloseCursor */

RETCODE pa60_GetGroupByColumns( PTR        valuePtr,
                                SDWORD     bufferLen,
                                SDWORD    *strLenPtr,
                                UWORD     *sqlStatePtr )
{
    strcpy( valuePtr, "HALLOOOO" );
    *strLenPtr = (UWORD) strlen( valuePtr );

    return SQL_SUCCESS;
} /* pa60_GetGroupByColumns */


VOID pa60_InitSetPosSpecial( tpa60SetPosSpecial *setPosSpecial )
{
    setPosSpecial->ignoredArray = NULL;
    setPosSpecial->arraySize    = 0;
} /* pa60_InitSetPosSpecial */
                               

VOID pa60_FreeSetPosSpecial( tpa60SetPosSpecial *setPosSpecial )
{
    if (setPosSpecial->ignoredArray != NULL) {
        apdfree( setPosSpecial->ignoredArray );
        setPosSpecial->ignoredArray = NULL;
        setPosSpecial->arraySize = 0;
    };
} /* pa60_FreeSetPosSpecial */

/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
UDWORD pa60_ComputePosition (UDWORD           long_pos,
                             tpa21DescRecord *impl_rec_ptr,
                             SWORD            fCType, 
                             UWORD           *terminatorSize)
{
  int clientIsUni, dbIsUni;
  SWORD  sql_type;       /* type of implementation buffer */
  UDWORD sql_len;
  UWORD  sql_scale;

  *terminatorSize = 1;

  apgstys (impl_rec_ptr->esqType, impl_rec_ptr->esqLength, impl_rec_ptr->esqScale,
           &sql_type, &sql_len, &sql_scale);

  if (sql_type == SQL_WCHAR  ||  sql_type == SQL_WVARCHAR  ||
      sql_type == SQL_WLONGVARCHAR)
    dbIsUni = TRUE;
  else
    dbIsUni = FALSE;

  if (fCType == SQL_WCHAR  ||  fCType == SQL_WVARCHAR  ||
      fCType == SQL_WLONGVARCHAR)
    clientIsUni = TRUE;
  else
    clientIsUni = FALSE;

  /* following table shows, what is calculated:

     client    db      long_pos     termSize
      UCS2     UCS2       * 2          2
      UCS2     ASC        =            1
      ASC      UCS2       * 2          2
      ASC      ASC        =            1
  */
  if (clientIsUni) {
    if (dbIsUni) {
      *terminatorSize = sizeof (SQLWCHAR);
      long_pos *= sizeof (SQLWCHAR);
    } else {
      *terminatorSize = 1;
    }
  } else {
    if (dbIsUni) {
      *terminatorSize = sizeof (SQLWCHAR);
      long_pos *= sizeof (SQLWCHAR);
    } else
      *terminatorSize = 1;
  }

  return long_pos;
}

/* computes position into kernel position. I.e. for SQL_CHAR nothing is done,
   for SQL_WCHAR, the buffer is double and shifted (pos starts at 1 (Pascal))
   http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
tsp00_Int pa60_ComputeKernelPosition (UDWORD           long_pos,
                                      tpa21DescRecord *impl_rec_ptr)
{
  SWORD  real_sql_type;       /* type of column in kernel */
  UDWORD sql_len;
  UWORD  sql_scale;

  apgstys (impl_rec_ptr->realEsqType, impl_rec_ptr->esqLength, impl_rec_ptr->esqScale,
           &real_sql_type, &sql_len, &sql_scale);

  if (real_sql_type == SQL_WCHAR  ||  real_sql_type == SQL_WVARCHAR  ||
      real_sql_type == SQL_WLONGVARCHAR)
    long_pos = 2*long_pos - 1;

  return (tsp00_Int4) long_pos;
}

int pa60_isConnected (tpa40DBC *dbc_block_ptr)
{
  if (dbc_block_ptr->esqblk.esq_sql_code == -807 || 
      dbc_block_ptr->esqblk.esq_sql_code == 700  ||
      dbc_block_ptr->esqblk.esq_sql_code == -813 ||
      dbc_block_ptr->esqblk.esq_sql_code == 800  ||
      dbc_block_ptr->esqblk.esq_sql_code == -708)
    return FALSE;

  return TRUE;
}

#endif  /* of _UNICODE_ODBC */


/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
