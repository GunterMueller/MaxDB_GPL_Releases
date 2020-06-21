/*!**********************************************************************

  module: vpa60StmtAttr.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC statement attributes

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

#include "vpa61StmtAttr.h"
#include "vpa09.h"

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/


VOID pa61InitStmtAttr( tpa61StmtAttr *stmtAttrPtr )
{
        stmtAttrPtr->sqlmode = CPR_KIND_EMPTY;
        stmtAttrPtr->query_timeout = 0;
        stmtAttrPtr->max_length = 0;
        stmtAttrPtr->max_select = SQL_MAX_ROWS_DEFAULT;
        stmtAttrPtr->noscan = FALSE;
        stmtAttrPtr->async_enable = SQL_ASYNC_ENABLE_DEFAULT;
        stmtAttrPtr->passthrough = FALSE;      
        stmtAttrPtr->reconnect_warning = FALSE;
        stmtAttrPtr->cursor_type = SQL_CURSOR_TYPE_DEFAULT;
        stmtAttrPtr->concurrency = SQL_CONCUR_DEFAULT;         
        stmtAttrPtr->keyset_size = 0;         
        stmtAttrPtr->retrieve_data = SQL_RD_ON;         
        stmtAttrPtr->use_bookmarks = SQL_UB_OFF;         
        stmtAttrPtr->message_code = sp1m_nil;
        stmtAttrPtr->enableAutoIPD = SQL_FALSE;
        stmtAttrPtr->data_caching = API_DATA_CACHING_DEFAULT;
        stmtAttrPtr->optimize_cursor = SQL_FALSE;
        stmtAttrPtr->read_long_pos = 0;
} /* pa61InitStmtAttr */


RETCODE pa61GetStmtAttr ( tpa61StmtAttr *stmtAttrPtr,
                          SDWORD         attr,
                          PTR            valuePtr,
                          SDWORD         bufferLen,
                          SDWORD        *strLenPtr,
                          UWORD         *sqlStatePtr )
{
    RETCODE    retcode = SQL_SUCCESS;

    API_TRACE( API_TR_ENTRY, "pa6GetStmtAttr", 0 );
    API_TRACE( API_TR_PTR, "stmtAttrPtr", &stmtAttrPtr );
    API_TRACE( API_TR_SDWORD, "attr", &attr );
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SDWORD, "bufferLen", &bufferLen );
    API_TRACE( API_TR_PTR, "strLenPtr", &strLenPtr );
    API_TRACE( API_TR_PTR, "sqlStatePtr", &sqlStatePtr );

    API_ASSERT_PTR( valuePtr );
    API_ASSERT_PTR( sqlStatePtr );
    
    switch (attr) {
    case SQL_QUERY_TIMEOUT: {
        API_MEMCPY( valuePtr,
                    &stmtAttrPtr->query_timeout,
                    sizeof(UDWORD));
        break;
    }
    case SQL_MAX_LENGTH: {
        API_MEMCPY( valuePtr,
                    &stmtAttrPtr->max_length,
                    sizeof(UDWORD));
        break;
    }
    /* moved to desc header field
    case SQL_BIND_TYPE: {
        API_MEMCPY( valuePtr,
                    &stmtAttrPtr->bind_type,
                    sizeof(UDWORD));
        break;
    }
    */
    case SQL_MAX_ROWS: {
        API_MEMCPY( valuePtr,
                    &stmtAttrPtr->max_select,
                    sizeof(UDWORD));
        break;
    }
    case SQL_NOSCAN: {
        API_MEMCPY( valuePtr,
                    &stmtAttrPtr->noscan,
                    sizeof(UDWORD)); 
        break;
    }
    case SQL_ASYNC_ENABLE: {
        API_MEMCPY( valuePtr,
                    &stmtAttrPtr->async_enable,
                    sizeof(UDWORD)); 
        break;
    }
    case SQL_PASSTHROUGH: {
        API_MEMCPY( valuePtr,
                    &stmtAttrPtr->passthrough,
                    sizeof(UDWORD)); 
        break;
    }
    /* ------ *** ODBC 2.0 * ---------------------------------- */	
    case (SQL_CONCURRENCY): {
        *((UDWORD FAR *)  valuePtr) = stmtAttrPtr->concurrency;
        break;
    }	
    case (SQL_CURSOR_TYPE): {
        *((UDWORD FAR *)  valuePtr) = stmtAttrPtr->cursor_type;
        break;
    }	
    case (SQL_KEYSET_SIZE): {
        *((UDWORD FAR *)  valuePtr) = stmtAttrPtr->keyset_size;
        break;
    }
    case (SQL_RETRIEVE_DATA): {
        *((UDWORD FAR *)  valuePtr) = stmtAttrPtr->retrieve_data;
        break;
    }
    case (SQL_SIMULATE_CURSOR): {
        *((UDWORD FAR *)  valuePtr) = SQL_SC_NON_UNIQUE;
        break;
    }		
    case (SQL_USE_BOOKMARKS): {
        *((UDWORD FAR *)  valuePtr) = stmtAttrPtr->use_bookmarks;
        break;
    }
    case SQL_MESSAGE_CODE: {
        API_MEMCPY( valuePtr,
                    &stmtAttrPtr->message_code,
                    sizeof(UWORD)); 
        break;
    }
    /* ------ *** ODBC 3.0 * ---------------------------------- */
    /* statement attributes */
    case (SQL_ATTR_CURSOR_SENSITIVITY):
        *((UDWORD*) valuePtr) = stmtAttrPtr->cursorSensitivity;
        break;
    case (SQL_ATTR_ENABLE_AUTO_IPD):
        *((UDWORD*) valuePtr) = stmtAttrPtr->enableAutoIPD;
        break;
    case (SQL_ATTR_FETCH_BOOKMARK_PTR):
        *((PTR*) valuePtr) = stmtAttrPtr->fetchBookmarkPtr;
        break;
    case (SQL_ATTR_METADATA_ID):
        *((UDWORD*) valuePtr) = stmtAttrPtr->metadataId;
        break;
        /* ----driver defined-------------------------------------- */
    case (SQL_SQLMODE): {
        *((SWORD*)  valuePtr) = stmtAttrPtr->sqlmode;
        break;
    }
    case (SQL_ATTR_DATA_CACHING): {
        *((UDWORD*)  valuePtr) = stmtAttrPtr->data_caching;
        break;
    }
    case (SQL_ATTR_OPTIMIZE_CURSOR): {
        *((UDWORD*)  valuePtr) = stmtAttrPtr->optimize_cursor;
        break;
    }
    case (SQL_ATTR_READ_LONG_POS): {
        *((UDWORD*)  valuePtr) = stmtAttrPtr->read_long_pos;
        break;
    }
    default: {
        *sqlStatePtr = API_ODBC_S1092; /* invalid option */
        retcode = SQL_ERROR;
        break;
    }
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa61GetStmtAttr", 0 );
    API_TRACE( API_TR_RETCODE, "retcode",&retcode );
    
    return retcode;
} /* pa61GetStmtAttr */


RETCODE pa61SetStmtAttr( SQLHSTMT          hstmt,
                         UWORD          stmtState,
                         SDWORD         attr,
                         PTR            valuePtr,
                         SDWORD         strLen,
                         tpa61StmtAttr *stmtAttr,
                         UWORD         *sqlStatePtr )
{
    RETCODE retcode;
    
    API_TRACE(API_TR_ENTRY, "pa61SetStmtAttr", 0);
    API_TRACE(API_TR_UWORD, "stmtState", &stmtState);

    API_ASSERT_PTR( sqlStatePtr );
    API_ASSERT_PTR( stmtAttr );

    retcode = SQL_SUCCESS;
    if (stmtState != API_STMT_ALLOCATED && stmtState != API_STMT_PREPARED) {
        switch (attr) {
        case (SQL_CURSOR_TYPE): {}
        case (SQL_SIMULATE_CURSOR): {}
        case (SQL_USE_BOOKMARKS): {}
        case (SQL_CONCURRENCY): {}
        case (SQL_ATTR_OPTIMIZE_CURSOR): 
            {
                if (stmtState == API_STMT_EXECUTED)
                    *sqlStatePtr = API_ODBC_S1011;    
                else
                    *sqlStatePtr = API_ODBC_24000;
                break;
            }
        }; /* switch */
    }; /* if */
    if (retcode == SQL_SUCCESS) {
        switch (attr) {
        case SQL_QUERY_TIMEOUT:
            {
                stmtAttr->query_timeout = (UDWORD) VALUE_CAST (valuePtr);
                break;
            }
        case SQL_MAX_LENGTH:
            {
                stmtAttr->max_length = (UDWORD) VALUE_CAST (valuePtr);
                break;
            }
            /* moved to desc header field
        case SQL_BIND_TYPE:
            {
                stmtAttr->bind_type = (UDWORD) valuePtr;
                break;
            }
            */
        case SQL_MAX_ROWS:
            {
                stmtAttr->max_select = (UDWORD) VALUE_CAST (valuePtr);
                break;
            }
        case SQL_NOSCAN:
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (TRUE): {}  /* don't scan escape sequences */
                case (FALSE): {  /* scan escape sequences */
                    stmtAttr->noscan = (UDWORD) VALUE_CAST (valuePtr);
                    break;
                }
                default:
                    {
                        *sqlStatePtr = API_ODBC_S1009; /* invalid argument */
                        break;
                    }
                }; /* switch */
                break;
            }
        case SQL_ASYNC_ENABLE:
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_ASYNC_ENABLE_ON):
                    {
                        SWORD rtc = pa09CreateThread(hstmt);
                        if (rtc == TRUE)
                            stmtAttr->async_enable = (UDWORD) VALUE_CAST (valuePtr);
                        else
                            *sqlStatePtr = API_ODBC_S1C00;
                        break;
                    }
                case (SQL_ASYNC_ENABLE_OFF):
                    {
                        if (pa09TerminateThread(hstmt) == TRUE)
                            stmtAttr->async_enable = (UDWORD) VALUE_CAST (valuePtr);
                        else
                            *sqlStatePtr = API_ODBC_S1C00; 
                        break;
                    }
                default:
                    {
                        *sqlStatePtr = API_ODBC_S1009; /* invalid argument */
                        break; 
                    }
                }; /* switch */
            break;
            }
        case (SQL_PASSTHROUGH):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (TRUE): {}  /* don't scan escape sequences */
                case (FALSE):
                    {  /* scan escape sequences */
                        stmtAttr->passthrough = (UDWORD) VALUE_CAST (valuePtr);
                        break;
                    }
                default:
                    {
                        *sqlStatePtr = API_ODBC_S1009; /* invalid argument */
                        break;
                    }
                }; /* switch */
                break;
            }	
        case (SQL_MESSAGE_CODE):
            {
                stmtAttr->message_code = (UWORD) VALUE_CAST (valuePtr);
                break;
            }
        /* --- *** ODBC 2.0 * ----------------------------------------- */
        case (SQL_CONCURRENCY):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_CONCUR_READ_ONLY) : {}
                case (SQL_CONCUR_LOCK) : {}
                case (SQL_CONCUR_ROWVER) : {}
                case (SQL_CONCUR_VALUES) : {
                    stmtAttr->concurrency = (UDWORD) VALUE_CAST (valuePtr);
                    break;
                }
                default:
                    {
                        *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                        break; 
                    }
                }
                break;                 
            }
        case (SQL_CURSOR_TYPE):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_CURSOR_FORWARD_ONLY) : {}
                case (SQL_CURSOR_KEYSET_DRIVEN) : {}
                case (SQL_CURSOR_DYNAMIC) : {}
                case (SQL_CURSOR_STATIC) : {
                    stmtAttr->cursor_type = (UDWORD) VALUE_CAST (valuePtr);                    break;
                }
                default:
                    {
                        *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                        break; 
                    }
                }; /* switch */
                break; 
            }
        case (SQL_KEYSET_SIZE):
            {
                stmtAttr->keyset_size = (UDWORD) VALUE_CAST (valuePtr);
                break;
            }
        case (SQL_RETRIEVE_DATA):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_RD_ON): {}
                case (SQL_RD_OFF):
                    {
                        stmtAttr->retrieve_data = (UDWORD) VALUE_CAST (valuePtr);
                        break;
                    }
                default:
                    {
                        *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                        break; 
                    }
                }; /* switch */
                break;  
            }
        case (SQL_SIMULATE_CURSOR):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_SC_NON_UNIQUE) :
                    {
                        break;
                    }
                case (SQL_SC_TRY_UNIQUE) : {}
                case (SQL_SC_UNIQUE) :
                    {
                        *sqlStatePtr = API_ODBC_01S02;/* option value
                                                       * changed */
                        break;
                    }
                default:
                    {
                        *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                        break;
                    }
                }; /* switch */
                break;                     
            }
        case (SQL_USE_BOOKMARKS):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_UB_ON):
                case (SQL_UB_OFF):
                    stmtAttr->use_bookmarks = (UDWORD) VALUE_CAST (valuePtr);
                    break;
                default:
                    *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                    break;
                }; /* switch */
                break;          
            }
        /* -- ODBC 3.0 ------------------------------------------------ */
        case (SQL_ATTR_CURSOR_SCROLLABLE):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_SCROLLABLE):
                case (SQL_NONSCROLLABLE):
                    stmtAttr->cursorScrollable = (UDWORD) VALUE_CAST (valuePtr);
                    break;
                default:
                    *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                    break;
                }; /* switch */
                break;
            }
        case (SQL_ATTR_CURSOR_SENSITIVITY):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_UNSPECIFIED):
                case (SQL_INSENSITIVE):
                case (SQL_SENSITIVE):
                    stmtAttr->cursorSensitivity = (UDWORD) VALUE_CAST (valuePtr);
                    break;
                default:
                    *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                    break;
                }; /* switch */
                break;
            }
        case (SQL_ATTR_ENABLE_AUTO_IPD):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_TRUE):
                case (SQL_FALSE):
                    stmtAttr->enableAutoIPD = (UDWORD) VALUE_CAST (valuePtr);
                    break;
                default:
                    *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                    break;
                }; /* switch */
                break;
            }
        case (SQL_ATTR_FETCH_BOOKMARK_PTR):
            {
                stmtAttr->fetchBookmarkPtr = valuePtr;
                break;
            }
        case (SQL_ATTR_METADATA_ID):
            {
                switch ((UDWORD) VALUE_CAST (valuePtr)) {
                case (SQL_TRUE):
                    stmtAttr->metadataId = (UDWORD) VALUE_CAST (valuePtr);
                    break;
                case (SQL_FALSE):
                    *sqlStatePtr = API_ODBC_01S02; /* option value changed */
                default:
                    *sqlStatePtr = API_ODBC_S1C00; /* invalid argument */
                    break;
                }; /* switch */
                break;
            }                    
        /* ---driver defined------------------------------------------- */
            /* no more in use
        case (SQL_CANCELCALLBACK):
            {
                stmtAttr->lpCancelProc = (PACANCELPROC*) valuePtr;
                break;
            }
            */
        case (SQL_SQLMODE):
            {
                switch ( (SWORD) VALUE_CAST (valuePtr)) {
                case (CPR_KIND_INTERNAL): {}
                case (CPR_KIND_ORACLE): {}
                case (CPR_KIND_ANSI): {}
                case (CPR_KIND_DB2): {}
                case (CPR_KIND_SAPR3):
                    {  /* scan escape sequences */
                        stmtAttr->sqlmode = (SWORD) VALUE_CAST (valuePtr);
                        break;
                    }
                default:
                    {
                        *sqlStatePtr = API_ODBC_S1009; /* invalid argument */
                        break;
                    }
                }; /* switch */
                break;
            }
        case (SQL_ATTR_DATA_CACHING):
            {
                if ( (UDWORD) VALUE_CAST (valuePtr) > 0 ) {
                    stmtAttr->data_caching = (UDWORD) VALUE_CAST (valuePtr);
                } else {
                    *sqlStatePtr = API_ODBC_S1009; /* invalid argument
                                                    * value */
                }; /* else */
                break;
            }
        case (SQL_ATTR_OPTIMIZE_CURSOR):
            {
                if ( (UDWORD) VALUE_CAST (valuePtr) == SQL_TRUE
                     || (UDWORD) VALUE_CAST (valuePtr) == SQL_FALSE ) {
                    stmtAttr->optimize_cursor = (UDWORD) VALUE_CAST (valuePtr);
                } else {
                    *sqlStatePtr = API_ODBC_S1009; /* invalid argument
                                                    * value */
                }; /* else */
                break;
            }
        case (SQL_ATTR_READ_LONG_POS):
            {
                stmtAttr->read_long_pos = (UDWORD) VALUE_CAST (valuePtr);
                break;
            };
        default:
            {
                *sqlStatePtr = API_ODBC_S1092;     /* option out of range */
                break;
            }
        }; /* switch */
    }; /* if (retcode==SUCCESS) */

    if (*sqlStatePtr != API_ODBC_00000) { /* != SUCCESS */
        if (*sqlStatePtr == API_ODBC_01S02) {
            /* option value changed */
            retcode = SQL_SUCCESS_WITH_INFO;
        } else {
            retcode = SQL_ERROR;
        }; /* else */
    }; /* if */
    
    API_TRACE(API_TR_EXIT, "pa61SetStmtAttr", 0);
    API_TRACE(API_TR_RETCODE, "retcode", &retcode);
    
    return(retcode);
} /* pa61SetStmtAttr */

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/
