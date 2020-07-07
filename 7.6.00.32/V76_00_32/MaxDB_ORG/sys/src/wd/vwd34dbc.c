/*!**********************************************************************

  module: vwd34dbc.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Persistent ODBC connections

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1999-2005 SAP AG




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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <time.h>

#include "vpa00sqlod32.h"
#include "gwd00.h"
#include "hwd26err.h"
#include "hwd27excl.h"
#include "hwd31conn.h"
#include "hwd34dbc.h"
#include "hwd38data.h"

#include "hsp81.h"
#include "hsp83.h"

#include "SAPDBCommon/SAPDB_string.h"

#define DBFS_TABLE_CONTAINER_OLD_SCHEMA "SYSDBA.WA_MY_CONTAINER"            /* Used with old versions < 7.6  */
#define DBFS_TABLE_INODE_OLD_SCHEMA     "SYSDBA.WA_MY_INODE"                /* Used with new versions < 7.6  */
#define DBFS_TABLE_CONTAINER            "SYSDBFILESYSTEM.MY_CONTAINER"      /* Used with new versions >= 7.6 */
#define DBFS_TABLE_INODE                "SYSDBFILESYSTEM.MY_INODE"          /* Used with new versions >= 7.6 */

sapdbwa_Bool wd34_CreateString( char **newStr, const char *sourceStr );

sapdbwa_Bool wd34_SetConnectFromConnStr( twd34ConnectParam *connectParam,
                                         char              *connStr );

sapdbwa_Bool wd34_GetStringParam( sapdbwa_DBCP         dbc,
                                unsigned char *buf,
                                sapdbwa_UInt4    bufLen,
                                char          *stringValue );

void wd34FreeConnection( SQLHDBC henv, SQLHDBC hdbc );

sapdbwa_Bool wd34_ConnIsAlive( SQLHDBC hdbc );

sapdbwa_Bool wd34_ConnBrokenError( sapdbwa_Int4 natoveErr );

/*********************
 * attrListItem
 *********************/
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
typedef struct st_attr_list_item  attrListItem;
struct st_attr_list_item
{
    SQLINTEGER    attribute;
    SQLPOINTER    value;
    SQLINTEGER    stringLength;
	attrListItem *next;
};

/*********************
 * twd34DBCAttr
 *********************/

/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
typedef struct st_wa_dbc_attr
{
    SQLPOINTER     sqlMode;
    SQLPOINTER     autocommit;
    SQLPOINTER     isolation;
    SQLPOINTER     sessionTimeout;
	attrListItem  *extendedAttrList;
} twd34DBCAttr;

void wd34InitAttr( twd34DBCAttr *attr );

sapdbwa_Bool wd34EqualAttr( twd34DBCAttr *attr1,
                            twd34DBCAttr *attr2 );

sapdbwa_Bool wd34EqualAttrList( attrListItem *attrList1,
                                attrListItem *attrList2 );

sapdbwa_Bool wd34SetConnectAttr( SQLHDBC       hdbc,
                                 twd34DBCAttr *attr,
                                 twd26ErrP     err );

sapdbwa_Bool SetAttrList( SQLHDBC       hdbc,
                          attrListItem *attrList,
                          twd26ErrP     err       );

sapdbwa_Bool GetAttrFromAttrList( attrListItem   *attrList,
								  SQLINTEGER      attr,
							      attrListItem  **foundAttr );

sapdbwa_Bool NewAttrListItem( attrListItem **newItem,
							  SQLINTEGER     attr,
							  SQLPOINTER     value,
                              SQLINTEGER     stringLength );

sapdbwa_Bool CopyAttributes(       twd34DBCAttr *destAttr,
                             const twd34DBCAttr *srcAttr );

sapdbwa_Bool ClearAttrList( attrListItem *attrList );

sapdbwa_UInt4 AttrListSize( attrListItem  *attrList );

/*********************
 * twd34PoolElemP
 *********************/

typedef struct st_pool_elem *twd34PoolElemP;

struct st_pool_elem
{
    /* ODBC handles */
    SQLHENV         henv;
    SQLHDBC         hdbc;

    /* connect attributes */
    twd34DBCAttr    attr;

    /* connect parameters */
    twd34ConnectParam  connectParam;

    /* used for timeout detection */
    time_t             lastAccessTime;

    sapdbwa_Bool         connAlive;
    
    /* user data */
    twd38UserDataP	userDataList;

    /* reference count for not exclusive connections */
    sapdbwa_Int4      refCount;
    
    /* links to other queue elements */
    twd34PoolElemP  prevElem;
    twd34PoolElemP  nextElem;

    sapdbwa_DBCExclType   exclusive;

    sapdbwa_Bool    isOldDbfsSchema;
    char            dbfsTableContainer[64];
    char            dbfsTableInode[64];
}; /* struct st_pool_elem */


twd34PoolElemP wd34CreatePoolElem( const twd34DBCAttr *attr );

void wd34DestroyPoolElem( twd34PoolElemP  poolElem );


sapdbwa_Bool wd34AllocConnect( twd34PoolElemP poolElem, twd26ErrP err );


sapdbwa_Bool wd34ElemMatches( const twd34PoolElemP  poolElem,
                            const twd34DBCAttr   *attr,
                            const char           *datasource,
                            const char           *driver,
                            const char           *serverNode,
                            const char           *serverDb,
                            const char           *user,
                            const char           *password,
                            SAPDB_Bool            ignoreUser,
                            const char           *sqltrace );

sapdbwa_Bool wd34_Reconnect( twd34PoolElemP  poolElem, twd26ErrP err );

/*********************
 * twd34DBCQueue
 *********************/

typedef struct st_wa_dbc_queue {
    twd34PoolElemP  firstEntry;
    twd34PoolElemP  lastEntry;
} twd34DBCQueue; /* struct st_wa_dbc_queue */

void wd34CheckTimeout( twd34DBCQueue    *dbcQueue,
                       time_t            timeOut );

void wd34DestroyElements( twd34DBCQueue    *dbcQueue );

void wd34AppendElem( twd34DBCQueue    *dbcQueue,
                     twd34PoolElemP    poolElem );

void wd34RemoveElem( twd34DBCQueue    *dbcQueue,
                     twd34PoolElemP    poolElem );

twd34PoolElemP wd34FindMatchingDBC( const twd34DBCQueue *dbcQueue,
                                    const twd34DBCAttr  *attr,
                                    const char          *datasource,
                                    const char          *driver,
                                    const char          *serverNode,
                                    const char          *serverDb,
                                    const char          *user,
                                    const char          *password,
                                    SAPDB_Bool           ignoreUser,
                                    const char          *sqltrace );

                                     
void wd34InitQueue( twd34DBCQueue  *dbcQueue );

/*!**********************************************************************

  Chapter: sapdbwa_DBCP

************************************************************************/

/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
void wd34InitAttr( twd34DBCAttr *attr )
{

    if (!attr) {
        return;
    }

    attr->sqlMode          = (SQLPOINTER) 1; /* internal */
    attr->autocommit       = (SQLPOINTER) SQL_AUTOCOMMIT_ON;
    attr->isolation        = (SQLPOINTER) SQL_TXN_READ_COMMITTED;
    attr->sessionTimeout   = (SQLPOINTER) SQL_DEFAULT_SESSION_TIMEOUT;

	attr->extendedAttrList = NULL; 
} /* wd34InitAttr */


sapdbwa_Bool wd34EqualAttr( twd34DBCAttr *attr1,
                            twd34DBCAttr *attr2 )
{
    sapdbwa_Bool equal = (attr1->sqlMode == attr2->sqlMode)
                      && (attr1->autocommit == attr2->autocommit)
                      && (attr1->isolation == attr2->isolation)
                      && (attr1->sessionTimeout == attr2->sessionTimeout)
					  && (wd34EqualAttrList( attr1->extendedAttrList, attr2->extendedAttrList ));

    return equal;
} /* wd34EqualAttr */

sapdbwa_Bool wd34EqualAttrList( attrListItem *attrList1,
                                attrListItem *attrList2 )
{

	attrListItem	*currentAttr = NULL;

	/* Initial check	*/
	if ( !attrList1 && attrList2 || attrList1 && !attrList2 || AttrListSize( attrList1 ) != AttrListSize( attrList2 )) {
		return sapdbwa_False;
	}

	/* Compare attribute lists	*/
	currentAttr = attrList1;
	while( currentAttr ) {
		/* Is current attribute in second attribute list?	*/
		if ( !GetAttrFromAttrList( attrList2, currentAttr->attribute, NULL )) {
			return sapdbwa_False;
		}

		currentAttr = currentAttr->next;
	}

	/* Whole list is compared and is equal	*/
    return sapdbwa_True;
}


/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool wd34SetOneAttr( SQLHDBC        hdbc,
							 SQLINTEGER     attr,
                             SQLPOINTER     value,
                             SQLINTEGER     stringLength,
                             twd26ErrP      err    )
{
    SQLRETURN   sqlRetcode = SQL_SUCCESS;
    sapdbwa_Bool  ok = true;

    sqlRetcode = SQLSetConnectAttr( hdbc, attr, value, stringLength );
    if (!wd00Success( sqlRetcode )) {
        ok = false;
        wd26SetOdbcError( err,
                          sqlRetcode,
                          SQL_NULL_HENV,
                          hdbc,
                          SQL_NULL_HSTMT );        
    }; /* if */

    return ok;
} /* wd34SetOneAttr */


sapdbwa_Bool wd34SetConnectAttr( SQLHDBC       hdbc,
                                 twd34DBCAttr *attr,
                                 twd26ErrP     err   )
{
    sapdbwa_Bool  ok = true;

    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
    ok = wd34SetOneAttr( hdbc, SQL_SQLMODE, attr->sqlMode, SQL_IS_INTEGER, err )
      && wd34SetOneAttr( hdbc, SQL_ATTR_AUTOCOMMIT, attr->autocommit, SQL_IS_INTEGER, err )
      && wd34SetOneAttr( hdbc, SQL_TXN_ISOLATION, attr->isolation, SQL_IS_INTEGER, err )
      && wd34SetOneAttr( hdbc, SQL_ATTR_SESSION_TIMEOUT, attr->sessionTimeout, SQL_IS_INTEGER, err )
	  && SetAttrList( hdbc, attr->extendedAttrList, err );

    return ok;
} /* wd34SetConnectAttr */


sapdbwa_Bool SetAttrList( SQLHDBC       hdbc,
                          attrListItem *attrList,
                          twd26ErrP     err       )
{
    attrListItem	*currentAttr = NULL;

	currentAttr = attrList;
	while( currentAttr ) {
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
		if ( !wd34SetOneAttr( hdbc, currentAttr->attribute, currentAttr->value, currentAttr->stringLength, err )) {
			return sapdbwa_False;
		}

		currentAttr = currentAttr->next;
	}
		
    return sapdbwa_True;
}


struct st_wa_dbc
{
    /* pointer to actual used poolElem, NULL if not connected */
    twd34PoolElemP  poolElem;

    /* if connect failes we keep our poolElem. So the HDBC is still
     * accessible to retrieve some Information with SQLGetDiagField
     * (PTS 1106096) */
    twd34PoolElemP  cachedPoolElem;
    
    /* connect attributes */
    twd34DBCAttr    attr;
    
    twd26ErrP       err;
}; /* struct st_wa_dbc */

/* helps us detecting leaks */
sapdbwa_Int4   wd34DbcCount = 0;

sapdbwa_DBCP WINAPI sapdbwa_CreateDBC()
{
    sapdbwa_Bool allocatOK = true;
    sapdbwa_DBCP newDBC = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_dbc ),
                   (sapdbwa_UInt1**) &newDBC,
                   &allocatOK );
    if (allocatOK) {
        newDBC->err = wd26CreateErr();
        if (newDBC->err != NULL) {
            newDBC->poolElem        = NULL;
            newDBC->cachedPoolElem  = NULL;
            wd34InitAttr( &newDBC->attr );
            wd34DbcCount++;
        } else {
            /* memory allocation failure */
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newDBC );
            newDBC = NULL;
        }; /* else */
    }; /* if */

    return newDBC;
} /* sapdbwa_CreateDBC */

void WINAPI sapdbwa_DestroyDBC( sapdbwa_DBCP dbc )
{
    /* wd34DBCDisconnect( ... */
    wd26DestroyErr( dbc->err );
    if (dbc->cachedPoolElem != NULL) {
        wd34DestroyPoolElem( dbc->cachedPoolElem );
    };

    /* Reset attributes */
    ClearAttrList(dbc->attr.extendedAttrList);
    wd34InitAttr(&dbc->attr);

    wd34DbcCount--;
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) dbc );
} /* sapdbwa_DestroyDBC */


sapdbwa_Bool WINAPI sapdbwa_CheckDBC( sapdbwa_DBCP      dbc,
                           sapdbwa_Int4   nativeErr,
                           const char  *sqlState,
                           sapdbwa_Bool      *connectionDown )
{
    sapdbwa_Bool retcode = true;

    if ((sqlState != NULL
        && (strcmp( sqlState, "S1T00" ) == 0
            || strcmp( sqlState, "HYT00" ) == 0))
        || wd34_ConnBrokenError( nativeErr )) {
        *connectionDown = true;
        /* PTS: 1110486 MaO */
        retcode = wd38FreeAllUserDatas( &(dbc->poolElem->userDataList) );
        /*
        retcode = wd34_Reconnect( dbc->poolElem, dbc->err );
        */
    } else {
        *connectionDown = false;
    }; /* else */

    return retcode;
} /* sapdbwa_CheckDBC */


SQLHDBC WINAPI sapdbwa_DBCHandle( sapdbwa_DBCP dbc )
{
    if (dbc->poolElem != NULL) {
        return dbc->poolElem->hdbc;
    } else if (dbc->cachedPoolElem != NULL) {
        return dbc->cachedPoolElem->hdbc;
    } else {
        return SQL_NULL_HDBC;
    };
} /* sapdbwa_DBCHandle */


SQLHENV WINAPI sapdbwa_DBCEnv( sapdbwa_DBCP dbc )
{
    if (dbc->poolElem != NULL) {
        return dbc->poolElem->henv;
    } else if (dbc->cachedPoolElem != NULL) {
        return dbc->cachedPoolElem->henv;
    } else {
        return SQL_NULL_HENV;
    };
} /* sapdbwa_DBCEnv */

/*===========================================================================*/

sapdbwa_Bool WINAPI sapdbwa_GetSchemaInfo( sapdbwa_DBCP dbc )
{
    if (dbc->poolElem != NULL) {
        return dbc->poolElem->isOldDbfsSchema;
    } else {
        return sapdbwa_False;
    };
}

/*===========================================================================*/

const char* WINAPI sapdbwa_GetTableContainer( sapdbwa_DBCP dbc )
{
    if (dbc->poolElem != NULL) {
        return dbc->poolElem->dbfsTableContainer;
    } else {
        return NULL;
    };
}

/*===========================================================================*/

const char* WINAPI sapdbwa_GetTableInode( sapdbwa_DBCP dbc )
{
    if (dbc->poolElem != NULL) {
        return dbc->poolElem->dbfsTableInode;
    } else {
        return NULL;
    };
}

/*===========================================================================*/


/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool WINAPI sapdbwa_DBCSetAttr( sapdbwa_DBCP  dbc,
									    SQLINTEGER    attr,
										SQLPOINTER    value )
{
    sapdbwa_Bool setAttrOK = true;
        
    switch (attr) {
    case SQL_SQLMODE:
        dbc->attr.sqlMode = value;
        break;
    case SQL_ATTR_AUTOCOMMIT:
        dbc->attr.autocommit = value;
        break;
    case SQL_TXN_ISOLATION:
        dbc->attr.isolation = value;
        break;
    case SQL_ATTR_SESSION_TIMEOUT:
        dbc->attr.sessionTimeout = value;
        break;
    default:
        wd26SetErr( dbc->err, ERR_INVALID_CONNECT_ATTR_WD26, NULL, NULL );
        setAttrOK = false;
        break;
    }; /* switch */

    return setAttrOK;
} /* sapdbwa_DBCSetAttr */


/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool WINAPI sapdbwa_DBCGetAttr( sapdbwa_DBCP   dbc,
									    SQLINTEGER     attr,
										SQLPOINTER    *pvalue )
{
    sapdbwa_Bool getAttrOK = true;
        
    switch (attr) {
    case SQL_SQLMODE:
        *pvalue = dbc->attr.sqlMode;
        break;
    case SQL_ATTR_AUTOCOMMIT:
        *pvalue = dbc->attr.autocommit;
        break;
    case SQL_TXN_ISOLATION:
        *pvalue = dbc->attr.isolation;
        break;
    case SQL_ATTR_SESSION_TIMEOUT:
        *pvalue = dbc->attr.sessionTimeout;
        break;        
    default:
        wd26SetErr( dbc->err, ERR_INVALID_CONNECT_ATTR_WD26, NULL, NULL );
        getAttrOK = false;
        break;
    }; /* switch */

    return getAttrOK;
} /* sapdbwa_DBCGetAttr */


/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool GetAttrFromAttrList( attrListItem   *attrList,
								  SQLINTEGER      attr,
							      attrListItem  **foundAttr )

{

	attrListItem	*currentAttr = NULL;

	currentAttr = attrList;
	while( currentAttr ) {
		/* Attribute found?	*/
		if( currentAttr->attribute == attr ) {
			if ( foundAttr ) {
				*foundAttr = currentAttr;
			}

			return sapdbwa_True;
		}

		currentAttr = currentAttr->next;
	}

	return sapdbwa_False;

}


/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool NewAttrListItem( attrListItem  **newItem,
							  SQLINTEGER      attr,
							  SQLPOINTER      value,
                              SQLINTEGER      stringLength )

{
	
    sapdbwa_Bool  ok = sapdbwa_True;
	attrListItem *currentAttr = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_attr_list_item ),
                        (sapdbwa_UInt1**) newItem, &ok );
    if ( ok ) {
		/* Init new attribute	*/
		currentAttr = *newItem;
		currentAttr->attribute = attr;
		currentAttr->value = value;
		currentAttr->stringLength = stringLength;
		currentAttr->next = NULL;
    } else {
        /* memory allocation failure */
        *newItem = NULL;
		return sapdbwa_False;
    };

    return sapdbwa_True;

}

sapdbwa_Bool CopyAttributes(       twd34DBCAttr *destAttr,
                             const twd34DBCAttr *srcAttr )
{

	attrListItem	*currentSrcAttr = srcAttr->extendedAttrList;
	attrListItem	*currentDestAttr = NULL;
	attrListItem	*newAttrItem = NULL;

    /* Copy standard attributes */
    destAttr->sqlMode = srcAttr->sqlMode;
    destAttr->autocommit = srcAttr->autocommit;
    destAttr->isolation = srcAttr->isolation;
    destAttr->sessionTimeout = srcAttr->sessionTimeout;
	destAttr->extendedAttrList = NULL;

    /* Copy extended list */
	while(currentSrcAttr) {
		/* Create new attr item	*/
        if (!NewAttrListItem(&newAttrItem,
                              currentSrcAttr->attribute,
                              currentSrcAttr->value,
                              currentSrcAttr->stringLength)) {
            ClearAttrList(destAttr->extendedAttrList);
            return sapdbwa_False;
        }

        if (!currentDestAttr) {
            /* Set first attr */
            destAttr->extendedAttrList = newAttrItem;
            currentDestAttr = destAttr->extendedAttrList;
        } else {
            /* Add next attr */
            currentDestAttr->next = newAttrItem;
            currentDestAttr = newAttrItem;
        }

		currentSrcAttr = currentSrcAttr->next;
	}

	return sapdbwa_True;

}

sapdbwa_UInt4 AttrListSize( attrListItem  *attrList )
{

	sapdbwa_UInt4  listSize = 0;
	attrListItem  *currentAttr = NULL;

	currentAttr = attrList;
	while( currentAttr ) {
		currentAttr = currentAttr->next;
		listSize++;
	}

	return listSize;

}


/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool WINAPI sapdbwa_DBCAddAttr( sapdbwa_DBCP  dbc,
									    SQLINTEGER    attr,
										SQLPOINTER    value,
                                        SQLINTEGER    stringLength)
{
	
	attrListItem	*newAttr = NULL;
	attrListItem	*currentAttr = NULL;

	/* Is attribute already there?	*/
	if ( GetAttrFromAttrList( dbc->attr.extendedAttrList, attr, &currentAttr )) {
		/* Update attribute value	*/
		currentAttr->value = value;
		currentAttr->stringLength = stringLength;
	} else {
		/* Create new attr list item	*/
		if ( !NewAttrListItem( &newAttr, attr, value, stringLength )) {
			return sapdbwa_False;
		}

		/* Add new attribute at the end of the list	*/
		if( !dbc->attr.extendedAttrList ) {
			dbc->attr.extendedAttrList = newAttr;
		} else {
			currentAttr = dbc->attr.extendedAttrList;
			while( currentAttr ) {
				if( currentAttr->next ) {
					currentAttr = currentAttr->next;
				} else {
					currentAttr->next = newAttr;
					currentAttr = NULL;
				}
			}
		}
	}

	return sapdbwa_True;

}


/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool WINAPI sapdbwa_DBCSelectAttr( sapdbwa_DBCP   dbc,
									       SQLINTEGER     attr,
									       SQLPOINTER    *pvalue,
                                           SQLINTEGER    *stringLength )
{

	attrListItem	*currentAttr = NULL;

	if ( GetAttrFromAttrList( dbc->attr.extendedAttrList, attr, &currentAttr )) {
		if ( currentAttr ) {
			*pvalue = currentAttr->value;
			*stringLength = currentAttr->stringLength;

			return sapdbwa_True;
		}
	}

	return sapdbwa_False;

}


sapdbwa_Bool WINAPI sapdbwa_DBCClearAttrList( sapdbwa_DBCP dbc )
{

    return ClearAttrList(dbc->attr.extendedAttrList);

}

sapdbwa_Bool ClearAttrList( attrListItem *attrList )
{

	attrListItem	*currentAttr = NULL;
	attrListItem	*nextAttr = NULL;

	currentAttr = attrList;
	while( currentAttr ) {
		nextAttr = currentAttr->next;

		/* Free attr item	*/
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) currentAttr );

		currentAttr = nextAttr;
	}

	return sapdbwa_True;

}

sapdbwa_ErrP WINAPI sapdbwa_DBCGetErr( sapdbwa_DBCP dbc )
{
    return dbc->err;
} /* sapdbwa_DBCGetErrorMsg */


sapdbwa_Bool WINAPI sapdbwa_DBCGetUser( sapdbwa_DBCP        dbc,
                                 unsigned char *buf,
                                 sapdbwa_UInt4    bufLen )
{
    if (dbc->poolElem != NULL) {
        /* connected */
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->poolElem->connectParam.user );
    } else if (dbc->cachedPoolElem != NULL) {
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->cachedPoolElem->connectParam.user );
    } else {
        /* not connected */
        wd26SetErr( dbc->err, ERR_NOT_CONNECTED_WD26, NULL, NULL );
        
        return false;
    };
} /* sapdbwa_GetUser */


sapdbwa_Bool WINAPI sapdbwa_DBCGetPassword( sapdbwa_DBCP        dbc,
                                     unsigned char *buf,
                                     sapdbwa_UInt4    bufLen )
{
    if (dbc->poolElem != NULL) {
        /* connected */
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->poolElem->connectParam.password );
    } else if (dbc->cachedPoolElem != NULL) {
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->cachedPoolElem->connectParam.password );
    } else {
        /* not connected */
        wd26SetErr( dbc->err, ERR_NOT_CONNECTED_WD26, NULL, NULL );
        
        return false;
    };
} /* sapdbwa_GetPassword */


sapdbwa_Bool WINAPI sapdbwa_DBCGetServerDb( sapdbwa_DBCP        dbc,
                                     unsigned char *buf,
                                     sapdbwa_UInt4    bufLen  )
{
    if (dbc->poolElem != NULL) {
        /* connected */
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->poolElem->connectParam.serverDb );
    } else if (dbc->cachedPoolElem != NULL) {
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->cachedPoolElem->connectParam.serverDb );
    } else {
        /* not connected */
        wd26SetErr( dbc->err, ERR_NOT_CONNECTED_WD26, NULL, NULL );
        
        return false;
    };
} /* sapdbwa_GetServerDb */


sapdbwa_Bool WINAPI sapdbwa_DBCGetServerNode( sapdbwa_DBCP        dbc,
                                       unsigned char *buf,
                                       sapdbwa_UInt4    bufLen  )
{
    if (dbc->poolElem != NULL) {
        /* connected */
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->poolElem->connectParam.serverNode );
    } else if (dbc->cachedPoolElem != NULL) {
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->cachedPoolElem->connectParam.serverNode );
    } else {
        /* not connected */
        wd26SetErr( dbc->err, ERR_NOT_CONNECTED_WD26, NULL, NULL );
        
        return false;
    };
} /* sapdbwa_GetServerNode */



sapdbwa_Bool WINAPI sapdbwa_DBCGetDatasource( sapdbwa_DBCP         dbc,
                                       unsigned char  *buf,
                                       sapdbwa_UInt4     bufLen )
{
    if (dbc->poolElem != NULL) {
        /* connected */
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->poolElem->connectParam.datasource );
    } else if (dbc->cachedPoolElem != NULL) {
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->cachedPoolElem->connectParam.datasource );
    } else {
        /* not connected */
        wd26SetErr( dbc->err, ERR_NOT_CONNECTED_WD26, NULL, NULL );
        
        return false;
    };
} /* sapdbwa_GetDatasource */


sapdbwa_Bool WINAPI sapdbwa_DBCGetDriver( sapdbwa_DBCP         dbc,
                              unsigned char *buf,
                              sapdbwa_UInt4        bufLen )
{
    if (dbc->poolElem != NULL) {
        /* connected */
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->poolElem->connectParam.driver );
    } else if (dbc->cachedPoolElem != NULL) {
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->cachedPoolElem->connectParam.driver );
    } else {
        /* not connected */
        wd26SetErr( dbc->err, ERR_NOT_CONNECTED_WD26, NULL, NULL );
        
        return false;
    };
} /* sapdbwa_GetDriver */


sapdbwa_Bool WINAPI sapdbwa_DBCGetSqlTrace( sapdbwa_DBCP dbc, unsigned char *buf, sapdbwa_UInt4 bufLen )
{
    if (dbc->poolElem != NULL) {
        /* connected */
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->poolElem->connectParam.sqltrace );
    } else if (dbc->cachedPoolElem != NULL) {
        return wd34_GetStringParam( dbc, buf, bufLen,
                                    dbc->cachedPoolElem->connectParam.sqltrace );
    } else {
        /* not connected */
        wd26SetErr( dbc->err, ERR_NOT_CONNECTED_WD26, NULL, NULL );
        
        return false;
    };
} /* sapdbwa_GetSqlTrace */

/*!**********************************************************************

  EndChapter: sapdbwa_DBCP

************************************************************************/

/*!**********************************************************************

  Chapter: sapdbwa_DBCPoolP

************************************************************************/

/*********************
 * twd34PoolElemP
 *********************/

void wd34InitConnectParam( twd34ConnectParam *connectParam )
{
    connectParam->paramSet   = false;
    connectParam->datasource = NULL;
    connectParam->driver     = NULL;
    connectParam->serverNode = NULL;
    connectParam->serverDb   = NULL;
    connectParam->user       = NULL;
    connectParam->password   = NULL;
    connectParam->sqltrace   = NULL;
} /* wd34InitConnectParam */


void wd34FreeConnectParam( twd34ConnectParam *connectParam )
{
    if (connectParam->paramSet) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) connectParam->datasource );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) connectParam->driver );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) connectParam->serverNode );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) connectParam->serverDb );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) connectParam->user );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) connectParam->password );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) connectParam->sqltrace );
        wd34InitConnectParam( connectParam );
    }; /* if */    
} /* wd34FreeConnectParam */


twd34PoolElemP wd34CreatePoolElem( const twd34DBCAttr *attr )
{
    sapdbwa_Bool     allocatOK = true;
    twd34PoolElemP newPoolElem = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_pool_elem ),
                   (sapdbwa_UInt1**) &newPoolElem,
                   &allocatOK );
    if (allocatOK) {
        newPoolElem->userDataList	= NULL;
        newPoolElem->henv = SQL_NULL_HENV;
        newPoolElem->hdbc = SQL_NULL_HDBC;

        if (CopyAttributes(&newPoolElem->attr, attr) == sapdbwa_False) {
            sapdbwa_SQLFREE((sapdbwa_UInt1*) newPoolElem );
            return NULL;
        }
        SAPDB_memcpy( &newPoolElem->attr,
                attr,
                sizeof(struct st_wa_dbc_attr) );
        wd34InitConnectParam( &newPoolElem->connectParam );
        time( &newPoolElem->lastAccessTime );
        newPoolElem->connAlive  = false;
        newPoolElem->refCount   = 0;
        newPoolElem->prevElem   = NULL;
        newPoolElem->nextElem   = NULL;
        newPoolElem->exclusive  = sapdbwa_DBC_SHARED;
    }; /* if */

    return newPoolElem;
} /* wd34CreatePoolElem */


void wd34DestroyPoolElem( twd34PoolElemP  poolElem )
{
    wd38FreeAllUserDatas( &(poolElem->userDataList) );
    poolElem->userDataList = NULL;
    wd34FreeConnectParam( &poolElem->connectParam );
    wd34FreeConnection( poolElem->henv, poolElem->hdbc );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) poolElem );
} /* wd34DestroyPoolElem */


sapdbwa_Bool wd34SetConnect( twd34ConnectParam *connectParam,
                           const char        *datasource,
                           const char        *driver,
                           const char        *serverNode,
                           const char        *serverDb,
                           const char        *user,
                           const char        *password,
                           const char        *sqltrace )
{
    sapdbwa_Bool ok = true;
    
    sapdbwa_ASSERT( !connectParam->paramSet );
    ok = wd34_CreateString( &connectParam->datasource, datasource )
        && wd34_CreateString( &connectParam->driver, driver )
        && wd34_CreateString( &connectParam->serverNode, serverNode)
        && wd34_CreateString( &connectParam->serverDb, serverDb)
        && wd34_CreateString( &connectParam->user, user)
        && wd34_CreateString( &connectParam->password, password )
        && wd34_CreateString( &connectParam->sqltrace, sqltrace );
    if (ok) {
        connectParam->paramSet = true;
    }; /* if */
    
    return ok;
} /* wd34SetConnect */


sapdbwa_Bool wd34AllocConnect( twd34PoolElemP poolElem, twd26ErrP err )
{
    SQLRETURN  sqlRetcode = SQL_SUCCESS;
    sapdbwa_Bool allocOK = true;
    
    sqlRetcode = SQLAllocEnv( &poolElem->henv );
    if (wd00Success(sqlRetcode)) {
        sqlRetcode = SQLAllocConnect( poolElem->henv, &poolElem->hdbc );
        if (wd00Success(sqlRetcode)) {
            /* set connect attributes */
            if (!wd34SetConnectAttr( poolElem->hdbc, &poolElem->attr, err )) {
                allocOK = false;
                SQLFreeEnv( poolElem->henv );
                poolElem->henv = SQL_NULL_HENV;
                SQLFreeConnect( poolElem->hdbc );
                poolElem->hdbc = SQL_NULL_HDBC;
            }; /* if */
        } else {
            wd26SetOdbcError( err,
                              sqlRetcode,
                              poolElem->henv,
                              poolElem->hdbc,
                              SQL_NULL_HSTMT );
            poolElem->hdbc = SQL_NULL_HDBC;
            allocOK = false;
        }; /* else */
    } else {
        wd26SetOdbcError( err,
                          sqlRetcode,
                          poolElem->henv,
                          SQL_NULL_HDBC,
                          SQL_NULL_HSTMT );
        poolElem->henv = SQL_NULL_HENV;
        allocOK = false;
    }; /* else */

    return allocOK;
} /* wd34AllocConnect */


sapdbwa_Bool wd34ElemMatches( const twd34PoolElemP  poolElem,
                              const twd34DBCAttr   *attr,
                              const char           *datasource,
                              const char           *driver,
                              const char           *serverNode,
                              const char           *serverDb,
                              const char           *user,
                              const char           *password,
                              SAPDB_Bool            ignoreUser,
                              const char           *sqltrace )
{
    sapdbwa_Bool elemMatches =
        (strcmp( user, poolElem->connectParam.user ) == 0 || ignoreUser == SAPDB_TRUE)
        && wd34EqualAttr( (twd34DBCAttr*)attr, (twd34DBCAttr*) &poolElem->attr )
        && (strcmp( datasource, poolElem->connectParam.datasource ) == 0)
        && (strcmp( driver, poolElem->connectParam.driver ) == 0)
        && (stricmp( serverNode, poolElem->connectParam.serverNode ) == 0)
        && (stricmp( serverDb, poolElem->connectParam.serverDb ) == 0)
        && (strcmp( sqltrace, poolElem->connectParam.sqltrace ) == 0)
        && (strcmp( password, poolElem->connectParam.password ) == 0 || ignoreUser == SAPDB_TRUE);

    return elemMatches;
} /* wd34ElemMatches */


sapdbwa_Bool wd34OdbcConnect( twd34PoolElemP  poolElem,
                              sapdbwa_UInt2   driverCompletion,
                              SQLHWND         windowHandle,
                              twd26ErrP       err )
{
    sapdbwa_Bool             connectOK = true;
    SQLRETURN                retcode = SQL_SUCCESS;
    char                     tracefile[MAX_FILENAME_LEN_WD00];
    char                     connStr[2048];
    char                     outConnStr[2048];
    tsp81_UCS2Char           connStrUCS2[4096];
    tsp81_UCS2Char           outConnStrUCS2[4096];
    sapdbwa_Int2             outLen;
    tsp00_Byte const        *srcAt = NULL;
    tsp81_UCS2Char          *destAt = NULL;
	tsp81_UCS2Char const    *srcAtUcs2 = NULL;
    tsp00_Byte		        *destAtUcs2 = NULL;
    SQLHSTMT                 hStmt;
    SQLRETURN                sqlRC;

    /* append id to sqltrace-filename to prevent
     * 'trace file open'-error */
    if (poolElem->connectParam.sqltrace != NULL
        && strlen(poolElem->connectParam.sqltrace) != 0) {
        sp77sprintf(tracefile, MAX_FILENAME_LEN_WD00, "%s%p.log", poolElem->connectParam.sqltrace, poolElem->hdbc);
    } else {
        tracefile[0] = '\0';
    }; /* else */
    wd31MakeConnectString( poolElem->connectParam.datasource,
                           poolElem->connectParam.driver,
                           poolElem->connectParam.serverNode,
                           poolElem->connectParam.serverDb,
                           poolElem->connectParam.user,
                           poolElem->connectParam.password,
                           tracefile,
                           connStr, sizeof( connStr ));
    /* connect to sap db */
    /*
    retcode = SQLDriverConnect( poolElem->hdbc, windowHandle,
                                (UCHAR*) connStr, SQL_NTS,
                                (UCHAR*) outConnStr, sizeof(outConnStr),
                                &outLen, driverCompletion );
    */
    sp83UTF8ConvertToUCS2((const tsp00_Byte*) connStr,
                          (const tsp00_Byte*)(connStr+strlen(connStr)+1),
                          &srcAt, connStrUCS2, connStrUCS2+4096+1, &destAt, WD_IS_UCS2_SWAPED);

    retcode = SQLDriverConnectW (poolElem->hdbc, windowHandle,
                                 (SQLWCHAR*)connStrUCS2, SQL_NTS,
                                 (SQLWCHAR*)outConnStrUCS2, sizeof(outConnStrUCS2),
                                 &outLen, driverCompletion);

	memset(outConnStr,0,sizeof(outConnStr));
	sp83UTF8ConvertFromUCS2((const tsp81_UCS2Char*)outConnStrUCS2,  
							(const tsp81_UCS2Char*)&outConnStrUCS2[sp81UCS2strlen(outConnStrUCS2)],
							&srcAtUcs2, WD_IS_UCS2_SWAPED,
                            (tsp00_Byte*)outConnStr, (tsp00_Byte*)(outConnStr+sizeof(outConnStr)),
                            &destAtUcs2);

    if (wd00Success( retcode )) {
        poolElem->connAlive = true;
        if (driverCompletion != SQL_DRIVER_NOPROMPT) {
            /* get complete connect data from outConnStr */
            wd34_SetConnectFromConnStr( &poolElem->connectParam,
                                        outConnStr );
        }; /* if */
    } else {
        /* connect failed */
        poolElem->connAlive = false;        
        wd26SetOdbcError( err,
                          retcode,
                          SQL_NULL_HENV,
                          poolElem->hdbc,
                          SQL_NULL_HSTMT );
        connectOK = false;
    }; /* else */

    /* Try to find dbfs schema version */
    /* The table names differ for version < 7.6 */
    poolElem->isOldDbfsSchema = sapdbwa_True;
    sp77sprintf(poolElem->dbfsTableContainer, 64, "%s", DBFS_TABLE_CONTAINER_OLD_SCHEMA);
    sp77sprintf(poolElem->dbfsTableInode, 64, "%s", DBFS_TABLE_INODE_OLD_SCHEMA);

	if (poolElem->attr.sqlMode == (SQLPOINTER)1) {	/* internal mode */
		sqlRC = SQLAllocStmt(poolElem->hdbc, &hStmt);
		if ( sqlRC != 0 ) {
			/* Disconnect */
			poolElem->connAlive = false;        
			SQLDisconnect(poolElem->hdbc);

			return false;
		}

		sqlRC = SQLPrepare(hStmt, (SQLCHAR*)"SELECT 1 FROM TABLES WHERE TABLENAME = 'WA_MY_INODE' AND TABLETYPE = 'VIEW'", SQL_NTS);
		if ( sqlRC != 0 ) {
			/* Disconnect */
			SQLFreeStmt(hStmt, SQL_DROP);

			poolElem->connAlive = false;        
			SQLDisconnect(poolElem->hdbc);

			return false;
		}

		sqlRC = SQLExecute(hStmt);
		if ( sqlRC != 0 ) {
			/* Disconnect */
			SQLFreeStmt(hStmt, SQL_DROP);

			poolElem->connAlive = false;        
			SQLDisconnect(poolElem->hdbc);

			return false;
		}

		/* Fetch rowset */
		sqlRC = SQLFetch(hStmt);
		if ( sqlRC != 0 ) {
			if (sqlRC == SQL_NO_DATA) {
				/* Set DBFS table names */
				/* The table names differ for version < 7.6 */
				poolElem->isOldDbfsSchema = sapdbwa_False;
				sp77sprintf(poolElem->dbfsTableContainer, 64, "%s", DBFS_TABLE_CONTAINER);
				sp77sprintf(poolElem->dbfsTableInode, 64, "%s", DBFS_TABLE_INODE);
			} else {
				/* Disconnect */
				SQLFreeStmt(hStmt, SQL_DROP);

				poolElem->connAlive = false;        
				SQLDisconnect(poolElem->hdbc);

				return false;
			}
		}

		/* Free statment handle */
		SQLFreeStmt(hStmt, SQL_DROP);
	}

    return connectOK;
} /* wd34OdbcConnect */


sapdbwa_Bool wd34_Reconnect( twd34PoolElemP  poolElem, twd26ErrP err )
{
    sapdbwa_Bool reconnectOk = true;
    
	/* free user data before diconnected, so stmt handles
	 * are not dropped twice. */
	wd38FreeAllUserDatas( &(poolElem->userDataList) );
    /* usually this will fail, else we won´t make a reconnect */
    SQLDisconnect( poolElem->hdbc );
    /* reconnect without connect box. */
    reconnectOk = wd34OdbcConnect( poolElem,
                                   SQL_DRIVER_NOPROMPT,
                                   NULL,
                                   err );

    return reconnectOk;
} /* wd34_Reconnect */


/*********************
 * twd34DBCQueue
 *********************/

void wd34CheckTimeout( twd34DBCQueue    *dbcQueue,
                       time_t            timeOut )
{
    twd34PoolElemP actElem = NULL;
    twd34PoolElemP tmpElem = NULL;    
    time_t         actualTime = 0;

    time( &actualTime );
    for (actElem = dbcQueue->firstEntry;
         actElem != NULL; ) {
        tmpElem = actElem->nextElem;
        if (actualTime - actElem->lastAccessTime > timeOut ) {
            wd34RemoveElem( dbcQueue, actElem );
            wd34DestroyPoolElem( actElem );
        }; /* if */
        actElem = tmpElem;
    }; /* for */
} /* wd34CheckTimeout */


void wd34DestroyElements( twd34DBCQueue    *dbcQueue )
{
    twd34PoolElemP actElem = NULL;
    twd34PoolElemP tmpElem = NULL;
    
    for (actElem = dbcQueue->firstEntry;
         actElem != NULL; ) {
        tmpElem = actElem->nextElem;
        wd34DestroyPoolElem( actElem );
        actElem = tmpElem;
    }; /* for */

    dbcQueue->firstEntry = NULL;
    dbcQueue->lastEntry = NULL;
} /* wd34DestroyElements */


void wd34AppendElem( twd34DBCQueue     *dbcQueue,
                     twd34PoolElemP     poolElem )
{
    twd34PoolElemP tmpElem = NULL;
    
    if (dbcQueue->lastEntry == NULL) {
        sapdbwa_ASSERT( dbcQueue->firstEntry == NULL );
        poolElem->nextElem = NULL;
        poolElem->prevElem = NULL;
        dbcQueue->lastEntry = poolElem;
        dbcQueue->firstEntry = poolElem;
    } else {
        dbcQueue->lastEntry->nextElem = poolElem;
        poolElem->nextElem = NULL;
        poolElem->prevElem = dbcQueue->lastEntry;
        dbcQueue->lastEntry = poolElem;
    }; /* else */
} /* wd34AppendElem */


void wd34RemoveElem( twd34DBCQueue    *dbcQueue,
                     twd34PoolElemP    poolElem )
{
    if (poolElem->prevElem != NULL) {
        poolElem->prevElem->nextElem = poolElem->nextElem;
    } else {
        /* poolElem is first entry */
        dbcQueue->firstEntry = poolElem->nextElem;
    }; /* else */

    if (poolElem->nextElem != NULL) {
        poolElem->nextElem->prevElem = poolElem->prevElem;
    } else {
        /* poolElem is last entry */
        dbcQueue->lastEntry = poolElem->prevElem;
    }; /* else */

    poolElem->nextElem = NULL;
    poolElem->prevElem = NULL;
} /* wd34RemoveElem */


twd34PoolElemP wd34FindMatchingDBC( const twd34DBCQueue *dbcQueue,
                                    const twd34DBCAttr  *attr,
                                    const char          *datasource,
                                    const char          *driver,
                                    const char          *serverNode,
                                    const char          *serverDb,
                                    const char          *user,
                                    const char          *password,
                                    SAPDB_Bool           ignoreUser,
                                    const char          *sqltrace )
{
    twd34PoolElemP actElem = NULL;
    twd34PoolElemP matchingElem = NULL;
    
    for ( actElem = dbcQueue->firstEntry;
          actElem != NULL && matchingElem == NULL;
          actElem = actElem->nextElem ) {
        /* elems from freeList have always SHARED set.
         * elems from activeList are only used if they were specified as
         * shared. */
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
        if ( (actElem->exclusive == sapdbwa_DBC_SHARED
             || (actElem->exclusive == sapdbwa_DBC_EXCL_AUTOCOMMIT_OFF
                 && attr->autocommit == (SQLPOINTER) SQL_AUTOCOMMIT_ON))
             && wd34ElemMatches( actElem, attr,
                                 datasource, driver,
                                 serverNode, serverDb,
                                 user, password, ignoreUser, sqltrace )) {
            matchingElem = actElem;
        }; /* if */
    }; /* for */

    return matchingElem;
} /* wd34FindMatchingDBC */

                                     
void wd34InitQueue( twd34DBCQueue  *dbcQueue )
{
    dbcQueue->firstEntry = NULL;
    dbcQueue->lastEntry  = NULL;
} /* wd34InitQueue */

/*********************
 * sapdbwa_DBCPool
 *********************/

struct st_wa_dbc_pool
{
    twd27ExclP         poolExcl; /* guarantee exclusive access to the pool */
    sapdbwa_Int4         timeOut;   /* time in seconds after which open db
                                   * connections are closed implicitely */
    
    struct st_wa_dbc_queue activeDBC; /* list of active database connections */
    struct st_wa_dbc_queue freeDBC;   /* list of available database connections */
}; /* struct st_wa_dbc_pool */


sapdbwa_DBCPoolP WINAPI sapdbwa_CreateDBCPool( sapdbwa_Int4  timeOut )
{
    sapdbwa_Bool allocatOK = true;
    sapdbwa_DBCPoolP newDBCPool = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_dbc_pool ),
                   (sapdbwa_UInt1**) &newDBCPool,
                   &allocatOK );
    if (allocatOK) {
        if (!wd27InitExcl( &newDBCPool->poolExcl )) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newDBCPool );
            return NULL;
        } else {
            newDBCPool->timeOut   = timeOut;
            wd34InitQueue( &newDBCPool->activeDBC );
            wd34InitQueue( &newDBCPool->freeDBC );
        }; /* else */
    }; /* if */

    return newDBCPool;
} /* WACreateDBCPool */


void WINAPI sapdbwa_DestroyDBCPool( sapdbwa_DBCPoolP  dbcPool )
{
    wd27BegExcl( dbcPool->poolExcl );
    /* destroy free dbc list */
    wd34DestroyElements( &dbcPool->freeDBC );
    /* destroy active dbc list (we hope nobody is using one of this dbc's) */
    wd34DestroyElements( &dbcPool->activeDBC );

    wd27EndExcl( dbcPool->poolExcl );
    wd27ExitExcl( dbcPool->poolExcl );
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) dbcPool );
} /* WADestroyDBCPool */


sapdbwa_Bool wd34Connect( sapdbwa_DBCPoolP       dbcPool,
                          sapdbwa_DBCP           dbc,
                          const char            *datasource,
                          const char            *driver,
                          const char            *serverNode,
                          const char            *serverDb,
                          const char            *user,
                          const char            *password,
                          const char            *sqltrace,
                          sapdbwa_UInt2          driverCompletion,
                          SQLHWND                windowHandle,
                          sapdbwa_DBCExclType    exclusive,
                          SAPDB_Bool             ignoreUser,
                          twd26ErrP              err )
{
    twd34PoolElemP freeElem = NULL;
    sapdbwa_Bool     connectOK = true;
    
    wd27BegExcl( dbcPool->poolExcl );
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
    if (exclusive == sapdbwa_DBC_SHARED
        || (exclusive == sapdbwa_DBC_EXCL_AUTOCOMMIT_OFF
            && dbc->attr.autocommit == (SQLPOINTER) SQL_AUTOCOMMIT_ON)) {
        /* first show in list of active connections */
        freeElem = wd34FindMatchingDBC( &dbcPool->activeDBC,
                                        &dbc->attr,
                                        datasource, driver,
                                        serverNode, serverDb,
                                        user, password,
                                        ignoreUser,
                                        sqltrace );
        if (freeElem != NULL) {
            /* check if connection is alive */
            freeElem->connAlive = wd34_ConnIsAlive( freeElem->hdbc );
            if (!freeElem->connAlive) {
                /* try to reconnect
                 * !!! other "users" of this shared connection do not notice that
                 * the connection was reconnected (this is bad for autocommit off
                 * connections) !!! */
                connectOK = wd34_Reconnect( freeElem, err );
            }; /* if */
            if (connectOK) {
                /* increase reference count */
                /* assert : freeElem->exclusive == sapdbwa_DBC_SHARED */
                freeElem->refCount++;
                dbc->poolElem = freeElem;
            } else {
                /* don't serve broken connections */
                freeElem = NULL;
                /* we must not destroy the broken connection because others use it */
            };
        };
    }; /* if */
    if (freeElem == NULL) {
        freeElem = wd34FindMatchingDBC( &dbcPool->freeDBC,
                                        &dbc->attr,
                                        datasource, driver,
                                        serverNode, serverDb,
                                        user, password,
                                        ignoreUser,
                                        sqltrace );
        if (freeElem) {
            wd34RemoveElem( &dbcPool->freeDBC, freeElem );
            /* check if connection is alive */
            freeElem->connAlive = wd34_ConnIsAlive( freeElem->hdbc );
            if (!freeElem->connAlive) {
                /* try to reconnect */
                connectOK = wd34_Reconnect( freeElem, err );
            }; /* if */
            if (connectOK) {
                /* use existing connection */
                wd34AppendElem( &dbcPool->activeDBC, freeElem );
                freeElem->refCount = 1;
                freeElem->exclusive = exclusive;
                dbc->poolElem = freeElem;
            } else {
                /* forget broken connection */
                wd34DestroyPoolElem( freeElem );
                freeElem = NULL;
            }; /* else */
        };
    }; /* else */
    if (freeElem == NULL) {
        /* open new connection */
        /* copy attributes from dbc */
        twd34PoolElemP newPoolElem = wd34CreatePoolElem( &dbc->attr );
        if (newPoolElem != NULL) {
            /* alloc hdbc and set connect attributes */
            if (wd34AllocConnect( newPoolElem, err )) {
                wd34SetConnect( &newPoolElem->connectParam,
                                datasource, driver,
                                serverNode, serverDb,
                                user, password,
                                sqltrace );
                connectOK = wd34OdbcConnect( newPoolElem,
                                             driverCompletion,
                                             windowHandle,
                                             err );
            } else {
                connectOK = false;
            }; /* else */
            if (connectOK) {
                dbc->poolElem = newPoolElem;

                wd34AppendElem( &dbcPool->activeDBC, newPoolElem );
                newPoolElem->refCount = 1;
                newPoolElem->exclusive = exclusive;
            } else {
                /* keep old poolElem (PTS 1106096) */
                if (dbc->cachedPoolElem != NULL) {
                    wd34DestroyPoolElem( dbc->cachedPoolElem );
                };
                dbc->cachedPoolElem = newPoolElem;
            }; /* else */
        } else {
            /* memory allocation failure */
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL);
            connectOK = false;
        }; /* else */
    }; /* else */
    /* timeOut sapdbwa_DBC_NO_TIMEOUT means no timeout */
    if (dbcPool->timeOut != sapdbwa_DBC_NO_TIMEOUT) {
        wd34CheckTimeout( &dbcPool->freeDBC, dbcPool->timeOut );
    }; /* if */
    wd27EndExcl( dbcPool->poolExcl );

    return connectOK;
} /* wd34Connect */

/*===========================================================================*/
    
sapdbwa_Bool WINAPI sapdbwa_DBCConnect( sapdbwa_DBCPoolP     dbcPool,
                                        sapdbwa_DBCP         dbc,
                                        const char          *datasource,
                                        const char          *driver,
                                        const char          *serverNode,
                                        const char          *serverDb,
                                        const char          *user,
                                        const char          *password,
                                        const char          *sqltrace,
                                        sapdbwa_UInt2        driverCompletion,
                                        SQLHWND              windowHandle,
                                        sapdbwa_DBCExclType  exclusive )
{

    return wd34Connect( dbcPool, dbc, datasource, driver,
                        serverNode, serverDb,
                        user, password,
                        sqltrace, driverCompletion,
                        windowHandle,
                        exclusive, SAPDB_FALSE, dbc->err );

}

/*===========================================================================*/
    
sapdbwa_Bool WINAPI sapdbwa_DBCConnectWithServerDB( sapdbwa_DBCPoolP     dbcPool,
                                                    sapdbwa_DBCP         dbc,
                                                    const char          *datasource,
                                                    const char          *driver,
                                                    const char          *serverNode,
                                                    const char          *serverDb,
                                                    const char          *user,
                                                    const char          *password,
                                                    const char          *sqltrace,
                                                    sapdbwa_UInt2        driverCompletion,
                                                    SQLHWND              windowHandle,
                                                    sapdbwa_DBCExclType  exclusive )
{

    return wd34Connect( dbcPool, dbc, datasource, driver,
                        serverNode, serverDb,
                        user, password,
                        sqltrace, driverCompletion,
                        windowHandle,
                        exclusive, SAPDB_TRUE, dbc->err );

}

/*===========================================================================*/

sapdbwa_Bool WINAPI sapdbwa_DBCDisconnect( sapdbwa_DBCPoolP  dbcPool,
                               sapdbwa_DBCP      dbc )
{
    sapdbwa_Bool     disconnectOK = true;
        
    wd27BegExcl( dbcPool->poolExcl );
    if (dbc->poolElem) {
        dbc->poolElem->refCount--;
        if (dbc->poolElem->refCount == 0) {
            /* connection is not in use anymore */
            wd34RemoveElem( &dbcPool->activeDBC, dbc->poolElem );
            if (dbc->poolElem->connAlive) {
                wd34AppendElem( &dbcPool->freeDBC, dbc->poolElem );
                time( &dbc->poolElem->lastAccessTime );
            } else {
                /* don't store connections that are broken */
                wd34DestroyPoolElem( dbc->poolElem );
            }; /* else */
            /* set to shared. Else findMatchingElem would not use this */
            dbc->poolElem->exclusive = sapdbwa_DBC_SHARED;
            dbc->poolElem = NULL;
        };
    } else {
        /* dbc is not active */
        wd26SetErr( dbc->err, ERR_DBC_NOT_ACTIVE_WD26, NULL, NULL );
        disconnectOK = false;
    }; /* else */
    wd27EndExcl( dbcPool->poolExcl );
    
    return disconnectOK;
} /* sapdbwa_DBCDisconnect */


sapdbwa_Bool WINAPI WAForgetDBC( sapdbwa_DBCPoolP  dbcPool,
                           sapdbwa_DBCP      dbc )
{
    sapdbwa_Bool      forgetOK = true;

    if (dbc->poolElem != NULL) {
        wd34RemoveElem( &dbcPool->activeDBC, dbc->poolElem );
        wd34DestroyPoolElem( dbc->poolElem );
        dbc->poolElem = NULL;
    } else {
        /* dbc is not active */
        wd26SetErr( dbc->err, ERR_DBC_NOT_ACTIVE_WD26, NULL, NULL );
        forgetOK = false;
    }; /* else */

    return forgetOK;
} /* WAForgetDBC */


void* WINAPI sapdbwa_DBCGetUserData( sapdbwa_DBCP conn )
{
    /* id 0 is reserved in vwd106pool.c */
    return wd38GetUserData( conn->poolElem->userDataList, 0 );
} /* wd34GetUserData */


sapdbwa_Bool WINAPI sapdbwa_DBCSetUserData( sapdbwa_DBCP           conn,
                                     void             *data,
                                     sapdbwa_Destructor     destructor )
{
    /* id 0 is reserved in vwd106pool.c */
    return wd38NewUserData( &(conn->poolElem->userDataList), 0, data, destructor );
} /* wd34SetUserData */


void* WINAPI wd34GetUserData( sapdbwa_DBCP conn, sapdbwa_UInt4 id )
{
    return wd38GetUserData( conn->poolElem->userDataList, id );
} /* wd34GetUserData */


sapdbwa_Bool WINAPI wd34SetUserData( sapdbwa_DBCP           conn,
                                   sapdbwa_UInt4       id,
                                   void             *data,
                                   sapdbwa_Destructor     destructor )
{
    return wd38NewUserData( &(conn->poolElem->userDataList), id , data, destructor );
} /* wd34SetUserData */


void wd34DestroyUserData( sapdbwa_DBCPoolP dbcPool )
{
    twd34PoolElemP actElem = NULL;
    
    for ( actElem = dbcPool->freeDBC.firstEntry;
          actElem != NULL;
          actElem = actElem->nextElem ) {
        if (actElem->userDataList != NULL) {
            wd38FreeAllUserDatas( &actElem->userDataList );
            actElem->userDataList = NULL;
        }; /* if */
    }; /* for */
} /* wd34DestroyUserData */

/*!**********************************************************************

  EndChapter: sapdbwa_DBCPool

************************************************************************/

void wd34FreeConnection( SQLHDBC henv, SQLHDBC hdbc )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;

    if (hdbc != SQL_NULL_HDBC) {
        sqlRetcode = SQLDisconnect( hdbc );
        sqlRetcode = SQLFreeConnect( hdbc );
    };
    if (henv != SQL_NULL_HENV) {
        sqlRetcode = SQLFreeEnv( henv );
    };
} /* wd34FreeConnection */


sapdbwa_Bool wd34_SetConnectFromConnStr( twd34ConnectParam *connectParam,
                                         char              *connStr )
{
    char *serverDb = NULL;
    char *serverNode = NULL;
    char *user = NULL;
    char *password = NULL;
    char *pos = NULL;
    char *semPos = NULL; /* position of semicolon */

	sapdbwa_UInt2 nUserTagLen = 0;
	
	/* Assume connect string key words are upper-case MaO */
    serverDb = strstr( connStr, ";SERVERDB=" );
    serverNode = strstr( connStr, ";SERVERNODE=" );
    user = strstr( connStr, ";UID=" );
	/* in a new version  odbc changed output of connect string	*/
    /* see PTS Error 1108230									*/
	if(user == NULL){
		user = strstr( connStr, "UID=" );
		nUserTagLen = strlen("UID=");
	}
	else
		nUserTagLen = strlen(";UID=");	

    password = strstr( connStr, ";PWD=" );
    if (serverDb != NULL) {
        serverDb += strlen(";SERVERDB=");
        semPos = strchr( serverDb, ';' );
        if (semPos != NULL) {
            *semPos = '\0';
        };
        wd34_CreateString( &connectParam->serverDb, serverDb );
    };
    if (serverNode != NULL) {
        serverNode += strlen(";SERVERNODE=");
        semPos = strchr( serverNode, ';' );
        if (semPos != NULL) {
            *semPos = '\0';
        };
        wd34_CreateString( &connectParam->serverNode, serverNode );
    };
    if (user != NULL) {
        user += nUserTagLen;
        semPos = strchr( user, ';' );
        if (semPos != NULL) {
            *semPos = '\0';
        };
        wd34_CreateString( &connectParam->user, user );
    };
    if (password != NULL) {
        password += strlen(";PWD=");
        semPos = strchr( password, ';' );
        if (semPos != NULL) {
            *semPos = '\0';
        };
        wd34_CreateString( &connectParam->password, password );
    };

    return true;
}


sapdbwa_Bool wd34_CreateString( char **newStr, const char *sourceStr )
{
    sapdbwa_Bool  allocOK = true;
    sapdbwa_UInt4 strLen = (sourceStr != NULL) ? strlen( sourceStr )+1 : 1;

    /* free old content */
    if (*newStr != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) *newStr );
    };
    /* allocate new str */
    sapdbwa_SQLALLOCAT( strLen, (sapdbwa_UInt1**) newStr, &allocOK );
    if (allocOK) {
        if (sourceStr != NULL) {
            strcpy( *newStr, sourceStr );
        } else {
            newStr[0] = '\0';
        }; /* else */
    }; /* if */

    return allocOK;
} /* wd34_CreateString */


sapdbwa_Bool wd34_GetStringParam( sapdbwa_DBCP   dbc,
                                  unsigned char *buf,
                                  sapdbwa_UInt4  bufLen,
                                  char          *stringValue )
{
    sapdbwa_Bool bufferTooSmall;

    if (!buf || bufLen < 1) {
        return sapdbwa_False;
    }
    
    if (!stringValue) {
        buf[0] = '\0';
        return sapdbwa_True;
    }

    if (strlen(stringValue) > bufLen-1 ) {
        bufferTooSmall = sapdbwa_True;
    } else {
        bufferTooSmall = sapdbwa_False;
    }
        
    strncpy( (char*) buf, stringValue, bufLen-1 );
    buf[bufLen-1] = '\0';
    if (bufferTooSmall == sapdbwa_True) {
        wd26SetErr( dbc->err, ERR_BUFFER_TOO_SMALL_WD26, NULL, NULL );
    };
        
    if (!bufferTooSmall) {
        return (sapdbwa_True);
    } else {
        return (sapdbwa_False);
    }

} /* wd34_GetStringParam */

sapdbwa_Bool WINAPI wd34IsPoolElemNull(  sapdbwa_DBCP  conn )
{

    if (conn->poolElem == NULL ) {
        return sapdbwa_True;
    } else {
        return sapdbwa_False;
    }

}

sapdbwa_Bool wd34_ConnIsAlive( SQLHDBC hdbc )
{
    sapdbwa_Bool  connAlive = true;
    SQLHSTMT    hstmt = SQL_NULL_HSTMT;
    SQLRETURN   retcode = SQL_SUCCESS;
    UCHAR       messageText[ODBC_ERROR_TEXT_LEN_WD00];
    UCHAR       sqlstate[10];
    SWORD       errLen = 0;
    SDWORD      nativeOdbcErr = 0;
    
    retcode = SQLAllocStmt( hdbc, &hstmt );
    if (retcode == SQL_SUCCESS) {
        retcode = SQLPrepare( hstmt, (UCHAR*) "SELECT * from dual", SQL_NTS );
        if (retcode != SQL_SUCCESS) {
            retcode = SQLError( SQL_NULL_HENV, hdbc, hstmt,
                                sqlstate,
                                &nativeOdbcErr,
                                messageText,
                                sizeof( messageText ),
                                &errLen );
            if (wd34_ConnBrokenError( nativeOdbcErr )) {
                connAlive = false;
            };
        };
        SQLFreeStmt( hstmt, SQL_DROP );
    } else {
        /* we cannot check connection */
        connAlive = false;
    };

    return connAlive;
} /* wd34_ConnIsAlive */


sapdbwa_Bool wd34_ConnBrokenError( sapdbwa_Int4 nativeErr )
{
    return (nativeErr == 700 || nativeErr == 800
            || nativeErr == -708 || nativeErr == -807 || nativeErr == -813 );
} /* wd34_ConnBrokenError */
