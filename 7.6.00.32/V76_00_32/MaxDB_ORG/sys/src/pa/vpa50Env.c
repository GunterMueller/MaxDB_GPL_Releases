/*!**********************************************************************

  module: vpa50Env.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC-environment handles (datastructures,functions)

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

#include "vpa50Env.h"
#include "vpa06.h"
#include "vpa09.h"

/*!**********************************************************************

  constants and prototypes for internal functions

************************************************************************/

API_RETCODE pa50apmaenv( SQLHENV *phenv );

API_RETCODE pa50apmfenv( SQLHENV henv );

#ifndef _UNICODE_ODBC
extern tsp81_CodePage   pa04gCodePage;
#endif


/*!**********************************************************************

  Chapter: ODBC functions

************************************************************************/

#ifndef SAPDB_FAST
#ifndef _UNICODE_ODBC
UCHAR *PA50_FN_SQLDRIVERS     = { (UCHAR*) "SQLDrivers" };
UCHAR *PA50_FN_SQLDATASOURCES = { (UCHAR*) "SQLDataSources" };
UCHAR *PA50_FN_SQLGETENVATTR  = { (UCHAR*) "SQLGetEnvAttr" };
UCHAR *PA50_FN_SQLSETENVATTR  = { (UCHAR*) "SQLSetEnvAttr" };
#else
extern UCHAR *PA50_FN_SQLDRIVERS;
extern UCHAR *PA50_FN_SQLDATASOURCES;
extern UCHAR *PA50_FN_SQLGETENVATTR;
extern UCHAR *PA50_FN_SQLSETENVATTR;
#endif
#endif


#ifdef _UNICODE_ODBC
ODBC_PROC(SQLDriversW,
	  (SQLHENV         henv,
           SQLUSMALLINT    fDirection,
           SQLWCHAR       *szDriverDesc,
           SQLSMALLINT     cbDriverDescMax,
           SQLSMALLINT    *pcbDriverDesc,
           SQLWCHAR       *szDriverAttributes,
           SQLSMALLINT     cbDrvrAttrMax,
           SQLSMALLINT    *pcbDrvrAttr), 
          (henv, fDirection, szDriverDesc, cbDriverDescMax, pcbDriverDesc, szDriverAttributes, cbDrvrAttrMax, pcbDrvrAttr))
#else
ODBC_PROC(SQLDrivers,
	  (SQLHENV         henv,
           SQLUSMALLINT    fDirection,
           SQLCHAR        *szDriverDesc,
           SQLSMALLINT     cbDriverDescMax,
           SQLSMALLINT    *pcbDriverDesc,
           SQLCHAR        *szDriverAttributes,
           SQLSMALLINT     cbDrvrAttrMax,
           SQLSMALLINT    *pcbDrvrAttr), 
          (henv, fDirection, szDriverDesc, cbDriverDescMax, pcbDriverDesc, szDriverAttributes, cbDrvrAttrMax, pcbDrvrAttr))
#endif
{

  SQLRETURN   retcode;
  API_RETCODE api_retcode;

  API_TRACE(API_TR_ENTRY, PA50_FN_SQLDRIVERS, 0);
  API_TRACE(API_TR_HANDLE,"henv",&henv);
  API_TRACE(API_TR_UWORD,"fDirection",&fDirection);
  API_TRACE(API_TR_PTR,"szDriverDesc",&szDriverDesc);
  API_TRACE(API_TR_SWORD,"cbDriverDescMax",&cbDriverDescMax);
  API_TRACE(API_TR_PTR,"pcbDriverDesc",&pcbDriverDesc);
  API_TRACE(API_TR_PTR,"szDriverAttributes",&szDriverAttributes);
  API_TRACE(API_TR_SWORD,"cbDrvrAttrMax",&cbDrvrAttrMax);
  API_TRACE(API_TR_PTR,"pcbDrvrAttr",&pcbDrvrAttr);

  api_retcode = apmstfc (henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, SQL_API_SQLDRIVERS);
  if (api_retcode != API_OK) {
     retcode = SQL_INVALID_HANDLE;
  }
  else {
      pa50PutError( henv, API_ODBC_S1C00 ); /* drive not capable */
      retcode = SQL_ERROR;
  }
  API_TRACE(API_TR_EXIT, PA50_FN_SQLDATASOURCES,0);
  API_TRACE(API_TR_RETCODE,"retcode",&retcode);

  return (retcode);
} /* SQLDrivers */


#ifdef _UNICODE_ODBC
ODBC_PROC(SQLDataSourcesW,
	  (SQLHENV       henv,
	   SQLUSMALLINT  fDirection,
	   SQLWCHAR     *szDSN,
	   SQLSMALLINT   cbDSNMax,
	   SQLSMALLINT  *pcbDSN,
	   SQLWCHAR     *szDescription,
	   SQLSMALLINT   cbDescriptionMax,
	   SQLSMALLINT  *pcbDescription),
	  (henv, fDirection, szDSN, cbDSNMax, pcbDSN, szDescription, cbDescriptionMax, pcbDescription))
#else
ODBC_PROC(SQLDataSources,
	  (SQLHENV       henv,
	   SQLUSMALLINT  fDirection,
	   SQLCHAR      *szDSN,
	   SQLSMALLINT   cbDSNMax,
	   SQLSMALLINT  *pcbDSN,
	   SQLCHAR      *szDescription,
	   SQLSMALLINT   cbDescriptionMax,
	   SQLSMALLINT  *pcbDescription),
	  (henv, fDirection, szDSN, cbDSNMax, pcbDSN, szDescription, cbDescriptionMax, pcbDescription))
#endif
{

  SQLRETURN   retcode;
  API_RETCODE api_retcode;

  API_TRACE(API_TR_ENTRY, PA50_FN_SQLDATASOURCES, 0);
  API_TRACE(API_TR_HANDLE,"henv",&henv);
  API_TRACE(API_TR_UWORD,"fDirection",&fDirection);
  API_TRACE(API_TR_PTR,"szDSN",&szDSN);
  API_TRACE(API_TR_SWORD,"cbDSNMax",&cbDSNMax);
  API_TRACE(API_TR_PTR,"pcbDSN",&pcbDSN);
  API_TRACE(API_TR_PTR,"szDescription",&szDescription);
  API_TRACE(API_TR_SWORD,"cbDescriptionMax",&cbDescriptionMax);
  API_TRACE(API_TR_PTR,"pcbDescription",&pcbDescription);

  api_retcode = apmstfc(henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, 
	   SQL_API_SQLDATASOURCES);
  if (api_retcode != API_OK) {
     retcode = SQL_INVALID_HANDLE;
  }
  else {
      pa50PutError( henv, API_ODBC_S1C00 ); /* drive not capable */
      retcode = SQL_ERROR;
  }
  API_TRACE(API_TR_EXIT, PA50_FN_SQLDATASOURCES,0);
  API_TRACE(API_TR_RETCODE,"retcode",&retcode);


  return (retcode);
} /* SQLDataSources */


#ifndef _UNICODE_ODBC
/* ODBC 3.0 */
ODBC_PROC(SQLGetEnvAttr, ( SQLHENV henv,
                           SDWORD  attr,
                           PTR     valuePtr,
                           SDWORD  len,
                           SDWORD *lenPtr ),
          ( henv, attr, valuePtr, len, lenPtr ))
{
    RETCODE retcode = SQL_ERROR;
    UWORD sqlState = API_ODBC_00000; /* Sucess */
    tpa50Env *env_blockPtr = (tpa50Env*) apdlock(henv);
    UDWORD *int_valuePtr = NULL;

    API_TRACE( API_TR_ENTRY, PA50_FN_SQLGETENVATTR, 0);
    API_TRACE( API_TR_HANDLE,"henv", &henv);
    API_TRACE( API_TR_SWORD, "attr", &attr);
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SWORD, "len", &len);
    API_TRACE( API_TR_PTR, "lenPtr", &lenPtr );

    
    if ( pa50VerifyEnv( henv ) != API_OK) {             /* henv valid? */
        retcode = SQL_INVALID_HANDLE;
    } else {
        pa50ResetError( henv );
        if (valuePtr) {
            int_valuePtr = (UDWORD*) valuePtr;
            switch( attr ) {
            case SQL_ATTR_CONNECTION_POOLING :
                {
                    *int_valuePtr = env_blockPtr->connection_pooling;
                    break;
                };
            case SQL_ATTR_CP_MATCH :
                {
                    *int_valuePtr = env_blockPtr->cp_match;
                    break;
                };
            case SQL_ATTR_ODBC_VERSION :
                {
                    *int_valuePtr = env_blockPtr->odbc_version;
                    break;
                };
            case SQL_ATTR_OUTPUT_NTS :
                {
                    *int_valuePtr = env_blockPtr->output_nts;
                    break;
                };
            default :
                {
                    sqlState = API_ODBC_S1092; /* Option type out of range */
                    retcode = SQL_ERROR;
                    break;
                };
            }; /* switch */
        } else {
            /* undocumented */
            sqlState = API_ODBC_HY009; /* Invalid use of null
                                          retcode = SQL_ERROR;        * pointer             */
        
        }; /* else */
    }; /* else */
    
    if (sqlState!=API_ODBC_00000) {
        pa50PutError( henv, sqlState );
    }; /* if */
    
    API_TRACE( API_TR_EXIT, PA50_FN_SQLGETENVATTR, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    return retcode;
} /* SQLGetEnvAttr */


/* ODBC 3.0
 * SQLSetEnvAttr
 *
 * Preconditions:
 * - henv is allocated
 * - no connection handle is allocated in henv
 */
ODBC_PROC(SQLSetEnvAttr, ( SQLHENV    henv,
                           SDWORD     attr,
                           SQLPOINTER valuePtr,
                           SDWORD     len ),
          ( henv, attr, valuePtr, len ))
{
    RETCODE retcode = SQL_SUCCESS;
    UWORD sqlState = API_ODBC_00000; /* Sucess */
    tpa50Env *envBlockPtr = (tpa50Env*) apdlock(henv);
    UDWORD int_value = 0;
    
    API_TRACE( API_TR_ENTRY, PA50_FN_SQLSETENVATTR, 0);
    API_TRACE( API_TR_HANDLE, "henv", &henv);
    API_TRACE( API_TR_ENVATTR, "attr", &attr);
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SWORD, "len", &len);
    
    if ( pa50VerifyEnv( henv ) != API_OK) {             /* henv valid? */
        retcode = SQL_INVALID_HANDLE;
    } else {
        pa50ResetError( henv );
        if ( envBlockPtr->child_hdbc != SQL_NULL_HDBC) {
            /* there already exists a connection handle */
            sqlState = API_ODBC_S1010; /* Function sequence error */
            retcode = SQL_ERROR;
        } else if (valuePtr==NULL) {
            sqlState = API_ODBC_S1092; /* Invalid attribute value */
            retcode = SQL_ERROR;
        } else {
            int_value = (UDWORD) VALUE_CAST (valuePtr);
            switch( attr ) {
            case SQL_ATTR_CONNECTION_POOLING :
                {
                    if ( int_value == SQL_CP_OFF
                         || int_value == SQL_CP_ONE_PER_DRIVER
                         || int_value == SQL_CP_ONE_PER_HENV) {
                        envBlockPtr->connection_pooling = int_value;
                    } else {
                        sqlState = API_ODBC_S1092; /* Invalid attribute
                                                    * value */
                        retcode = SQL_ERROR;
                    }; /* else */
                    break;
                };
            case SQL_ATTR_CP_MATCH :
                {
                    if (int_value == SQL_CP_STRICT_MATCH
                        || int_value == SQL_CP_RELAXED_MATCH) {
                        envBlockPtr->cp_match = int_value;
                    } else {
                        sqlState = API_ODBC_S1092; /* Invalid attribute
                                                    * value */
                        retcode = SQL_ERROR;
                    }; /* else */
                    break;
                };
            case SQL_ATTR_ODBC_VERSION :
                {
                    if (int_value == SQL_OV_ODBC2
                        || int_value == SQL_OV_ODBC3) {
                        envBlockPtr->odbc_version = int_value;
                    } else {
                        sqlState = API_ODBC_S1092; /* Invalid attribute
                                                    * value */
                        retcode = SQL_ERROR;
                    }; /* else */
                    break;
                };
            case SQL_ATTR_OUTPUT_NTS :
                {
                    if (int_value == SQL_TRUE) {
                        envBlockPtr->output_nts = int_value;
                    } else if (int_value == SQL_FALSE) {
                        sqlState = API_ODBC_HYC00; /* Optional Feature not
                                                    * implemented */
                        retcode = SQL_ERROR;
                    } else {
                        sqlState = API_ODBC_S1092; /* Invalid attribute
                                                    * value */
                        retcode = SQL_ERROR;
                    }; /* else */
                    break;
                };
            default :
                {
                    sqlState = API_ODBC_S1092; /* Option type out of range */
                    retcode = SQL_ERROR;
                    break;
                };
            }; /* switch */
        }; /* else */
    }; /* else */

    if (sqlState!=API_ODBC_00000) {
        pa50PutError( henv, sqlState );
    }; /* if */

    API_TRACE( API_TR_EXIT, PA50_FN_SQLSETENVATTR, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    return retcode;
} /* SQLSetEnvAttr */

/*!**********************************************************************

  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

UCHAR *PA50_FN_ALLOCENV  = { (UCHAR*) "pa50AllocEnv" };
UCHAR *PA50_FN_FREEENV   = { (UCHAR*) "pa50FreeEnv" };

#ifdef WIN
extern int InitWSA(UCHAR FAR *database);
extern HANDLE s_hModule;
#endif

extern api_global_tab pa07global[];

/* vpa01c (keyword tab from ASCII to UCS2) */
void pa01BuildKeywordW ();

SQLRETURN pa50AllocEnv( SQLHENV *phenv )
{
  SQLRETURN retcode;
#ifdef WIN   
   HWND hwnd;
   HTASK hCurr;
   SWORD i;
#endif   

  static int build_keyword_tab = 0;

  if (build_keyword_tab == 0) {
      build_keyword_tab = 1;
      pa01BuildKeywordW ();
  }
  
  PA30PROTECTTHREAD();
#ifndef WIN
  if(cross_check == 0) {
     API_TRACE_INIT(88);
  }
#endif
  cross_check++;

  API_TRACE( API_TR_ENTRY, PA50_FN_ALLOCENV, 0);
  API_TRACE( API_TR_PTR,"phenv", &phenv);

  if (phenv == NULL) { /* null pointer */
    retcode = SQL_ERROR;
  }
  else {
      /*
      if (pa10_henv)
       *phenv = pa10_henv;
    else
    */
    if (pa50apmaenv(phenv) != API_OK) { /* env allocate ok? */
      *phenv = SQL_NULL_HENV;
      retcode = SQL_ERROR;
      goto error_exit;
    }
    apmstfc(*phenv, SQL_NULL_HDBC, SQL_NULL_HSTMT, SQL_API_SQLALLOCENV);
    pa50ResetError( *phenv );
    retcode = SQL_SUCCESS;
#ifdef WIN       
       hCurr = GetCurrentTask();
       API_TRACE(API_TR_HWND, "hCurr", &hCurr);
       hwnd = 0;
       /* first search for an used task */
       for (i=0; i < MAX_TASK; i++) {
	  if (pa07global[i].hTask == hCurr) {
	     hwnd = pa07global[i].hWin;
	     pa07global[i].used_count++;
	     pa07global[i].init_calls++;
	     break;
	  }	       
       }
       /* second search for an empty task */
       API_TRACE(API_TR_HWND,"hwnd", &hwnd);
       if (!hwnd) {
	 char szBuf[MAX_PATH];      
	 GetModuleFileName(NULL, szBuf, sizeof(szBuf));      
	 hwnd = CreateWindow("SQLODBC",szBuf,WS_OVERLAPPEDWINDOW,
			     CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_DESKTOP,0, s_hModule, NULL);
	 API_TRACE(API_TR_HWND,"hwnd", &hwnd);	 
	 for (i=0; i < MAX_TASK; i++) {
	   if (pa07global[i].hTask == 0) {
	     pa07global[i].hTask = hCurr;
	     pa07global[i].used_count = 1;
	     pa07global[i].init_calls = 1;
	     pa07global[i].hWin = hwnd;
	     break;
	   }	
	 }	
	  if (i == MAX_TASK) {
	     API_TRACE(API_TR_MSG, "task limit overflow", 0);
	     retcode = SQL_ERROR;
	     goto error_exit;
	  }
#ifndef WIN32	  
	  if (e07setglobal ( (WORD)RTE_GLOBAL_HMAIN , (DWORD)hwnd ) == 0) {
	     API_TRACE(API_TR_MSG,"e07setglobal faild", 0);
	     retcode = SQL_ERROR;
	     goto error_exit;
	  }
          if (e07setglobal ( RTE_GLOBAL_HOOK , (DWORD)lpODBCHook ) == 0) {
	     API_TRACE(API_TR_MSG,"e07setglobal faild", 0);
	     retcode = SQL_ERROR;
	     goto error_exit;
	  }
#endif	  
       }
#endif	    
  }
  /* pa10_henv = *phenv; */
error_exit:
  API_TRACE(API_TR_EXIT,PA50_FN_ALLOCENV,0);
  API_TRACE(API_TR_RETCODE,"retcode",&retcode);
  API_TRACE(API_TR_HANDLE,"*phenv",phenv);

  PA30UNPROTECTTHREAD();
  
  return(retcode);
} /* pa50AllocEnv */


SQLRETURN pa50FreeEnv( SQLHENV henv )
{
    SQLRETURN    retcode;
    API_RETCODE  api_retcode;
    tpa50Env    *env_block_ptr;
    SQLHDBC      hdbc;
#ifdef WIN  
    HWND hwnd;
    HANDLE hCurr;
    SWORD i;
#endif
    
    PA30PROTECTTHREAD();
    API_TRACE( API_TR_ENTRY, PA50_FN_FREEENV,0);
    API_TRACE( API_TR_HANDLE, "henv",&henv);

    if ( pa50VerifyEnv( henv ) != API_OK) { /* henv valid? */
        retcode = SQL_INVALID_HANDLE;
    } else {
        api_retcode = apmstfc(henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, 
                              SQL_API_SQLFREEENV);
        if (api_retcode != API_OK) {
            retcode = SQL_INVALID_HANDLE;
        } else {
            env_block_ptr = (tpa50Env FAR *) apdlock(henv);
            API_ASSERT_PTR(env_block_ptr);
            hdbc = env_block_ptr -> child_hdbc;
            apdunlk(henv);
            if (cross_check > 1)
                retcode = SQL_SUCCESS;
            else
                if (hdbc != 0) { /* remaining children dbcs? */
                    retcode = SQL_ERROR;
                    pa50PutError( henv, API_ODBC_S1010 );
                }
                else {
                    if (pa50apmfenv(henv) == API_OK) {
                        /*	 pa10_henv = SQL_NULL_HENV;  */
#ifdef WIN	
                        hwnd = 0;
                        hCurr = GetCurrentTask();
                        for (i=0; i < MAX_TASK; i++) {
                            if (pa07global[i].hTask == hCurr) {
                                hwnd = pa07global[i].hWin;
                                pa07global[i].used_count--;
                                break;
                            }	       
                        }
                        if (i == MAX_TASK) {
                            retcode = SQL_ERROR;
                            /* henv doesnt exist anymore
                            pa50PutError(henv, API_ODBC_S1000 ); */
                        }
                        else {
                            api_global_tab FAR *gp;
                            API_TRACE(API_TR_HANDLE, "hwnd", &hwnd);
                            gp = &pa07global[i];
#ifndef WIN32	    
                            if (gp->used_count == 0) {
                                for (i=0; i < gp->init_calls; i++)
		  
                                    sqlfinish(API_FALSE);
                                gp->init_calls = 0;
                            }
#endif	       
                            gp->hTask = 0;	       
                            if (hwnd) {
                                gp->hWin = 0;	       
                                if(!DestroyWindow(hwnd))
                                    retcode = SQL_ERROR;
                            }
                        }
#endif	    	
                        retcode = SQL_SUCCESS;
                    }
                    else {
                        retcode = SQL_ERROR;
                        /* henv doesnt exist anymore
                        pa50PutError( henv, API_ODBC_S1000 ); */
                    }
                }
        }
    }
    if (retcode == SQL_SUCCESS) {
        cross_check--;		  /* keep use count current */
        if (cross_check == 0) {
            API_TRACE(API_TR_MSG, "sqlfinish", 0);
            sqlfinish(API_FALSE);
        }
    }

    API_TRACE( API_TR_EXIT, PA50_FN_FREEENV, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);

    PA30UNPROTECTTHREAD();
    return (retcode);
} /* pa50FreeEnv */


API_RETCODE pa50PutError( SQLHENV henv, UWORD sqlState )
{
    API_RETCODE   apiRetcode = API_OK;
    tpa50Env     *envPtr;
    tpa31DiagRec *newDiagRec;
    
    API_TRACE( API_TR_ENTRY, "pa50PutError", 0);

    envPtr    = (tpa50Env*) henv;
    newDiagRec = pa30AddDiagRec( &envPtr->diagArea );
    if (newDiagRec!=NULL) {
        pa31InitDiagRec( newDiagRec, sqlState, 0, (UCHAR*) "", 0, 0 );
    } else {
        apiRetcode = API_NOT_OK;
    }; /* else */
    
    API_TRACE( API_TR_EXIT, "pa50PutError", 0);
    API_TRACE( API_TR_RETCODE, "apiRetcode", &apiRetcode);

    return apiRetcode;
} /* pa50PutError */


API_RETCODE pa50ResetError( SQLHENV henv )
{
    API_RETCODE apiRetcode = API_OK;
    tpa50Env *envPtr;
    
    API_TRACE( API_TR_ENTRY, "pa50ResetError", 0);

    envPtr = (tpa50Env*) henv;
    pa30FreeDiagArea( &envPtr->diagArea );
    envPtr->errorIndex = 0;
    
    API_TRACE( API_TR_EXIT, "pa50ResetError", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

    return apiRetcode;
} /* pa50ResetError */


API_RETCODE pa50VerifyEnv( SQLHENV henv )
{
  tpa50Env FAR * envPtr;
  API_RETCODE apiRetcode = API_OK;

  API_TRACE( API_TR_ENTRY, "pa50VerifyEnv", 0);
  API_TRACE( API_TR_HANDLE, "henv",&henv);

  envPtr = (tpa50Env*) henv;
  if (envPtr == NULL) {
      apiRetcode = API_NOT_OK;
  } else {
      if (envPtr->type != API_ENV_TYPE) {
          apiRetcode = API_NOT_OK;
      }; /* if */
  }; /* else */

  API_TRACE( API_TR_EXIT, "pa50VerifyEnv", 0);
  API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

  return (apiRetcode);
} /* pa50VerifyEnv */

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

/* pa50apmaenv() - API miscellaneous function, allocate environment */

API_RETCODE pa50apmaenv ( SQLHENV *phenv )
{
   SQLHENV      henv;
   API_RETCODE  retcode=API_OK;
   tpa50Env    *env_block_ptr;
   tsp81_CodePage *defaultCodePage = sp81GetCodePage ();

   API_TRACE(API_TR_ENTRY,"pa50apmaenv",0);
   API_TRACE(API_TR_PTR,"phenv",&phenv);


   /* set default codepage: in windows, get from system,
      in non-windows, use pa04gCodePage */
#ifdef WIN
   if (defaultCodePage == NULL) {
     defaultCodePage = apdallo (sizeof (tsp81_CodePage));
     if (defaultCodePage != NULL) {
       char buf[256];
       int i;
       unsigned int acp = GetACP();   /* get default codepage from system */

       /* get codepage from user */
       i = GetLocaleInfo( LOCALE_USER_DEFAULT,
                          LOCALE_IDEFAULTANSICODEPAGE,
                          buf, sizeof(buf));
       /* if no error: codepage number is returned as string */
       if (i > 0)
         acp = atoi (buf);

       for (i=0; i<256; i++)
         buf[i] = i;

       MultiByteToWideChar (acp, MB_PRECOMPOSED, buf, 256,
                            defaultCodePage->map, 256);
       API_SPRINTF (defaultCodePage->name, "WINDOWS-%d", acp);

       sp81SetCodePage (defaultCodePage);
     }
     else
       sp81SetCodePage (&pa04gCodePage);
   }
#else
   if (defaultCodePage == NULL)
     sp81SetCodePage (&pa04gCodePage);
#endif

   henv = (SQLHENV) apdallo( (DWORD) sizeof(tpa50Env));
   if (henv == 0) {
      henv = SQL_NULL_HENV;
      retcode = API_NOT_OK;
   }
   else {
      env_block_ptr = (tpa50Env*) henv;
      API_ASSERT_PTR(env_block_ptr);
#ifdef WIN32
#ifdef MEMCHECK      
      _CrtMemCheckpoint( &env_block_ptr->startMemState );
#endif      
#endif
      env_block_ptr -> type       = API_ENV_TYPE;
      env_block_ptr -> child_hdbc = 0;
      
      /* ODBC 3.0 Attributes */
      env_block_ptr -> connection_pooling = SQL_CP_OFF;
      env_block_ptr -> cp_match           = SQL_CP_STRICT_MATCH;
      env_block_ptr -> odbc_version       = SQL_OV_ODBC2;
      env_block_ptr -> output_nts         = SQL_TRUE;
      pa30InitDiagArea( &env_block_ptr->diagArea );
      env_block_ptr -> errorIndex         = 0;
      env_block_ptr -> envContainer       = pr01EnvNewCont ();
   }
   *phenv = henv;

   API_TRACE(API_TR_EXIT,"pa50apmaenv",0);
   API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
   API_TRACE(API_TR_HANDLE,"*phenv",phenv);

   return(retcode);
} /* apmaenv */


/* pa50apmfenv() - API miscellaneous function, free environment */

API_RETCODE pa50apmfenv( SQLHENV henv)
{
   tpa50Env    *env_block_ptr;
   API_RETCODE  retcode;
    
   API_TRACE(API_TR_MSG,"pa50apmfenv",0);
   env_block_ptr = (tpa50Env FAR *) apdlock (henv);
   API_ASSERT_PTR(env_block_ptr);

   pa30DeleteDiagArea( &env_block_ptr->diagArea );

   pr01EnvDeleteCont (env_block_ptr->envContainer);
   
#ifdef WIN32
#ifdef MEMCHECK   
   { /* Memory leak checking */
       _CrtMemState endMemState;
       _CrtMemState diffMemState;
       _CrtMemCheckpoint( &endMemState );
       if (_CrtMemDifference( &diffMemState, 
                              &env_block_ptr->startMemState, 
                              &endMemState ) != 0) {
           _CrtMemDumpStatistics( &diffMemState );
           _CrtMemDumpAllObjectsSince( &diffMemState );
       }; /* if */
       _CrtDumpMemoryLeaks();
   } /* Memory leak checking */
#endif   
#endif

   /* set invalid type. So next operation on this freed handle will return
    * SQL_INVALID_HANDLE (or crashes) */    
   env_block_ptr -> type = API_INVALID_HANDLE_TYPE;   
   apdfree((void*) henv);

   retcode = API_OK;

   return(retcode);
} /* pa50apmfenv */

#endif  /* of  _UNICODE_ODBC */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
