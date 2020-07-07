/* @lastChanged: "1998-07-10  16:03"
 * @filename:    vco20.cpp
 * @purpose:     "System Stored Procedures"
 * @release:     7.2.0.0  
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"



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

/****************************************************************************/
/* DbpInstallObj.cpp : Implementation of CDbpInstall                            */
/****************************************************************************/

#if defined(WIN32)
//#include <string>  FF 19-Sep-2001 not needed any more ( Warnings on IA64 )
#else
#include <stdlib.h>
#endif
#include "heo02x.h"
#include "lcbasetypes.h"      /* no check */
#include "livecachetypes.h"   /* no check */
#include "vco20.h"            /* no check */
#include "hco20.h"         
#include "geo573.h"

long wstringlen(DbpTypeUnicode *p) 
{
    long i;
    for ( i=0; *(p+i); i=i+2 );
    return i/2;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

#define MX_KNL_IDENTIFIER   64
#define STR_IN(P) SqlCol(P, SqlCol::VSTRING, (/*IA64*/long)strlen(P))
/* PTS 1119957 */
#define STR_UNI_IN(P) SqlCol(P, SqlCol::VSTRING_UNICODE, wstringlen(P))
#define STR_OUT(P) SqlCol(&P[0], SqlCol::VSTRING, (/*IA64*/long)sizeof(P))
/* PTS 1119957 */
#define STR_UNI_OUT(P) SqlCol(&P[0], SqlCol::VSTRING_UNICODE, (/*IA64*/long)(sizeof(P)/2))

void truncateBlanks (CPath& Path) {
  int len = sizeof(Path) - 1;
  while (' ' == Path[len]) {
    len--;
  }
  len++;
  if (len < sizeof(Path)) {
    Path[len] = 0;
  }
}

/****************************************************************************/
/* Method Register is the very first Method called after the COM-Objekt     */
/* has been created.                                                        */
/* The CALLBACK Interface (IKernelSink) has been activated and may be used  */
/* (within the "constructor" of the COM-object IKernelSink hasn't been      */
/*  activated yet !)                                                        */

STDMETHODIMP CDbpInstall::Register()
{
  HRESULT hr	= S_OK;
  try {
    
    co90CheckVersion ((IUnknown**)&this->m_pIUnknown, 
                      ILIVECACHESINK_VERSION, 
                      LIVECACHETYPES_VERSION, LIVECACHEOMS_VERSION, 
                      LIVECACHESQL_VERSION );
/* *** TODO ***                                                            */
/* Register all persistent objects used by DB-Procedures here              */
/* This Routine is called, before the first DB-Procedure gets control.     */
  }
  catch (DbpError e) {
    omsExceptionHandler (e);
  }
	return hr;
}

/****************************************************************************/
/* IDbpInstall method implementation  (to be inserted here)                   */
/*-------------------------------------------------------------------------------*/

STDMETHODIMP CDbpInstall::COPY_AND_REG_DLL(CPath * DllName, DllHandle * Handle, int regDll)
{
	enum { bufSize = 4096 };
    
 
  FILE  *dll = NULL;
  try {
    CPath         Path;
    int           bufCnt = 0;
    unsigned char buf[bufSize];
    unsigned char *p;
    char          *InstRoot;

    truncateBlanks(*DllName);
    InstRoot = getenv("INSTROOT");
    if (!InstRoot) {
      throw DbpError (-2, "INSTROOT not set");
    }
    if (strlen(InstRoot) + 5 + strlen ((char*) DllName) >= sizeof(Path)) {
      throw DbpError (-3, "Path too long");
    }
    strcpy ((char*) &Path[0], InstRoot);
    strcat ((char*) &Path[0], "\\sap\\");
    strcat ((char*) &Path[0], (char*) DllName);

    OmsIStream<DllHandle, unsigned char> inStream(this, *Handle);
    if( NULL == (dll  = fopen( REINTERPRET_CAST(char*,Path), "wb" )) ) {
      throw DbpError (-1, "could not open output file\n" );
    }
    while (p = inStream.omsRead()) {
      if (bufCnt < bufSize) {
        buf[bufCnt] = *p;
        bufCnt++;
      }
      else {
        fwrite( buf, 1, bufCnt, dll);
        buf[0] = *p;
        bufCnt = 1;
      }
    }
    if (bufCnt > 0) {
      fwrite( buf, 1, bufCnt, dll);
    }
    fclose (dll);
    
    if (regDll) {
      REG_DLL (&Path);
    }
  }
  catch (DbpError e)
  {
    if (dll) {
      fclose (dll);
    }
    omsExceptionHandler(e);
  }
  return S_OK;
}

/*-------------------------------------------------------------------------------*/

STDMETHODIMP CDbpInstall::ACTIVATE_DLL(CProgId * pProgId)
{
  try {
    char stmt[256];
    truncateBlanks(*pProgId);
    sqlSetMode(SqlOpt::SQLMODE_INTERNAL);         
    SQL sel = sql ("Select OWNER, DBPROCNAME from domain.dbprocedures where Prog_id = ?");
    (sel << STR_IN((char*)pProgId)).sqlExecute();
    if (100 != sqlCode()) {
      int cnt = 0;
      int resCnt = sqlResultCount();
      SQL fetch  = sql ("Fetch Into ?, ?");
      char owner[MX_KNL_IDENTIFIER + 1];
      char dbproc[MX_KNL_IDENTIFIER + 1];
      fetch << STR_OUT(owner) << STR_OUT(dbproc);
      do {
        fetch.sqlExecute();
        if (0 == sqlCode()) {
          cnt++;
          strcpy(&stmt[0],"CREATE OR REPLACE DBPROC ");
          strcat(&stmt[0], dbproc);
          strcat(&stmt[0], " AS '");
          strcat(&stmt[0], (char*)pProgId);
          strcat(&stmt[0], "' EXECUTE INPROC");
          if ( cnt == resCnt ) {
            strcat(&stmt[0], " COMMIT");
          }
          sql (&stmt[0])();
        }
        else {
          break;
        }
      }
      while (0 == sqlCode());
    }
  }
  catch (DbpError e)
  {
    omsExceptionHandler(e);
  }
  
  return S_OK;
}

/*-------------------------------------------------------------------------------*/

STDMETHODIMP CDbpInstall::REG_DLL(CPath * Path)
{
  tsp00_ErrText       ErrText;
  int               ErrTextLen = sizeof(ErrText);

  typedef HRESULT (WINAPI *REGISTERSERVER) (void);
  
  truncateBlanks(*Path);

  try {
    HINSTANCE hInst = (HINSTANCE)sqlLoadLibrary(REINTERPRET_CAST(char*,Path), (char*) &ErrText[0], ErrTextLen);
    if ( NULL == hInst ) 
    {
      throw DbpError (-2, "could not load dll");
    }
    REGISTERSERVER r = (REGISTERSERVER)sqlGetProcAddress ( hInst ,"DllRegisterServer", (char*) &ErrText[0], ErrTextLen) ;
    
    if ( NULL == r )
    {  
      throw DbpError (-3, "could not get proc address"); 
    }
    
    HRESULT hr = r ( );
    if (0 != hr) {
      throw DbpError (-4, "could not register"); 
    }
    
    sqlFreeLibrary(hInst, (char*) &ErrText[0], ErrTextLen);
  }
  catch (DbpError e)
  {
    omsExceptionHandler(e);
  }
  return S_OK;
}

/*-------------------------------------------------------------------------------*/
/* PTS 1103927 */

STDMETHODIMP CDbpInstall::FORCE_AND_WAIT_FOR_CHECKPOINT() 
{
  /* PTS 1110790 char emsg[256]; */                       /* PTS 1110814 FF */
  SqlHandle::sqlHandler orgHandler = sqlSetHandler(NULL); /* PTS 1110814 FF don't use exception handling */
  try
  {
    sqlSetMode(SqlOpt::SQLMODE_INTERNAL);
    SQL forceSavepoint = sql("force savepoint no open transaction");
    forceSavepoint.sqlExecute();
    if (  (0 != sqlCode()) && ( 100 != sqlCode()) ) goto DBErrorExit; /* PTS 1110814 FF */
  }
  catch (DbpError e)
  {
    dbpTrace("force checkpoint failed (unexpected error : %d", e.dbpError());
    sqlExceptionHandler(e);
  }
  sqlSetHandler(orgHandler);      /* PTS 1110814 FF */
  return S_OK;

  /* DBErrorExist has been coded for PTS 1110814 FF */
DBErrorExit:
  sqlSetHandler(orgHandler); 
  dbpTrace("force checkpoint failed (unexpected error : %d", sqlCode());
  STATIC_CAST(SqlHandle*,this)->sqlRaiseError(sqlCode(),sqlErrorMsg());
  return S_OK;
}

/*-------------------------------------------------------------------------------*/

/* PTS 1102637, T.A. 1999-05-06 */

STDMETHODIMP CDbpInstall::FORCE_CHECKPOINT(OmsTypeInt4 *chptId) {
  *chptId = 0;	// PTS 1110086
  this->FORCE_AND_WAIT_FOR_CHECKPOINT();
  return S_OK;
}


/*-------------------------------------------------------------------------------*/

STDMETHODIMP CDbpInstall::WAIT_FOR_CHECKPOINT(OmsTypeInt4 chptId) {
  return S_OK; // PTS 1110086
}
/* END PTS */
/*-------------------------------------------------------------------------------*/

STDMETHODIMP CDbpInstall::SYSDDLTRIGGER()
{
  return S_OK;
}

/*-------------------------------------------------------------------------------*/

static int co20_hex2int( char x )
{
  unsigned int i = 0;
    if (('0' <= x) && ( x <= '9'))
      i = x - '0';
    if (('A' <= x) && ( x <= 'F'))
      i = (x - 'A') + 10;
    if (('a' <= x) && ( x <= 'f'))
      i = (x - 'a') + 10;
  return i;
}

static void co20GUID_FromString(const char* vStrClsID, GUID& guid)
{
  int i;

  guid.Data1 = 0;
  for ( i=0; i<4; i++ ) 
  {
    guid.Data1 = ( guid.Data1 * 256) + ( co20_hex2int( vStrClsID[(i*2)] ) * 16 ) +
      co20_hex2int( vStrClsID[(i*2)+1] );
  }

  guid.Data2 = 0;
  for ( i=0; i<2; i++ ) 
  {
    guid.Data2 = ( guid.Data2 * 256) + ( co20_hex2int( vStrClsID[(i*2)+9] ) * 16 ) +
      co20_hex2int( vStrClsID[(i*2)+10] );
  }

  guid.Data3 = 0;
  for ( i=0; i<2; i++ ) 
  {
    guid.Data3 = ( guid.Data3 * 256) + ( co20_hex2int( vStrClsID[(i*2)+14] ) * 16 ) +
      co20_hex2int( vStrClsID[(i*2)+15] );
  }

  for ( i=0; i<2; i++ ) 
  {
    guid.Data4[i] = ( co20_hex2int( vStrClsID[(i*2)+19] ) * 16 ) +
      co20_hex2int( vStrClsID[(i*2)+20] );
  }
  
  for ( i=2; i<8; i++ ) 
  {
    guid.Data4[i] = ( co20_hex2int( vStrClsID[(i*2)+20] ) * 16 ) + 
      co20_hex2int( vStrClsID[(i*2)+21] );
  }
};


STDMETHODIMP CDbpInstall::DROP_SCHEMAS()
{
    dbpOpMsg ("DROP_SCHEMAS called");
    bool error = false;
    SqlHandle::sqlHandler orgHandler = sqlSetHandler(NULL);
    sqlSetMode(SqlOpt::SQLMODE_INTERNAL);

    /* BEGIN PTS 1133706 FF logic completely rewritten */
    sql ("DELETE FROM SYSCATALOG.CLASSCONTAINERS WHERE 1 = 1")();
    if (0 == sqlCode() || (100 == sqlCode()))
    {
        dbpOpMsg ("%d containers dropped", sqlResultCount());

        sql ("DELETE FROM SYSCATALOG.AUTHORIZATION WHERE SCHEMAID > 1 AND SCHEMAID <> 2147483647")();
        if (0 == sqlCode() || (100 == sqlCode()))
        {
            dbpOpMsg ("%d schemas dropped successfully", sqlResultCount());
        }
        else 
        {
            dbpOpError ("unexpected error in DELETE FROM SYSCATALOG.SCHEMAS : %d", sqlCode());
            error = true;
        }
    }
    else
    {
        dbpOpError ("unexpected error in DELETE FROM SYSCATALOG.CLASSCONTAINERS : %d", sqlCode());
        error = true;
    }
    /* END PTS 1133706 FF logic completely rewritten */

    sqlSetHandler(orgHandler);
    if (error)
    {
        DbpError e(-1, "DROP SCHEMAS FAILED, see knldiag");
        omsExceptionHandler(e);
    }
    return S_OK;
}


STDMETHODIMP CDbpInstall::OMS_CLEANUP_AFTER_INIT()
{
  dbpOpMsg ("OMS_CLEANUP_AFTER_INIT called");

  try{
    omsCleanUpAfterInitialization();
  }
  catch (DbpError& e){
    dbpOpError("Cleaning of global oms-structures failed with error=%d", e.dbpError());
    omsExceptionHandler(e);
  }
  return S_OK;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
