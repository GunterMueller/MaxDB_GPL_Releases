/*!
  -----------------------------------------------------------------------------

  module: vcn14.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV, SteffenS, MarcW

  special area: Database Manager-API

  description:  Implementation of the API for Database Manager Client Applications

  version:      7.4.

  -----------------------------------------------------------------------------

  Copyright (c) 1998-2005 SAP AG

  -----------------------------------------------------------------------------


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



//
//   FreeBSD portions added by Kai Mosebach, 
//   For more informations see : http://www.komadev.de/sapdb
//
    
*/

/*
  -----------------------------------------------------------------------------
  private includes
  -----------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef FREEBSD
#include <malloc.h>
#endif

#include "heo03.h"
#include "heo13.h"
#include "heo02.h"
#include "hsp02.h"
#include "vsp004.h"
#include "gsp09.h"
#include "hsp77.h"
#include "heo01.h"
#include "gip00.h"

#include "hcn90c.h"
#include "hcn14.hpp"

#include "RunTime/Communication/RTEComm_URIBuilder.hpp"
#include "DBM/Srv/User/DBMSrvUsr_ChallengeResponseClient.hpp"
#include "SAPDBCommon/SAPDB_string.h"

#if !defined(_WIN32)
#include <unistd.h>    /* for close() */
#include <sys/types.h> /* for umask() */
#include <sys/stat.h>  /* for umask() */

#define stricmp  strcasecmp
#define strnicmp strncasecmp
#endif

/*
  -----------------------------------------------------------------------------
  private declarations
  -----------------------------------------------------------------------------
*/
typedef struct ControlSessionT {
    tsp00_Int4 reference;
    tsp00_Int4 packetSize;
    void*    packetData;
    char*    packetOutData; /* used for reading */
    char*    packetInData;  /* used for writing */
    tsp00_Int4 packetPos;
    tsp00_Int4 packetLen;
    bool     bUnicode;
} ControlSessionT;

#define C_COMPONENT    "DBM-Api"
#define XU_CRYPT_BASE  16
#define XU_MAX_USERS   32
#define XU_DBM_MARK    "1"
#define CTRL_ERR       "ERR"
#define RPM_ERR_HEAD   "ERR\n"
#define XU_SAP_USER    "SAPR3"
#define XU_SAP_PWD     "SAP"
#define XU_KEY_DEFAULT "DEFAULT"

#define BUFFER_LEN      1024
#define DBM_USERLOGON   "user_logon"
#define DBM_SYSTEMLOGON "user_system"
#define DBM_VERSION     "dbm_version"
#define DBM_UNICODE     "UNICODE"
#define DBM_ASSIGN      "="
#define DBM_YES         "= YES"

/*
  -----------------------------------------------------------------------------
  private prototypes
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_connectDBM (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    void                   ** sessionOut,
    Tools_DynamicUTF8String & oErrtext );

tsp00_Int4 cn14_connectDBMUsr (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    const char*               user,
    void                   ** sessionOut,
    Tools_DynamicUTF8String & oErrtext );

tsp00_Int4 cn14_replyAvailable (
    void                    * sessionParm,
    tsp00_ErrTextc            VAR_ARRAY_REF errtextC );

tsp00_Int4 cn14_cmdRequest (
    void                    * sessionParm,
    const void              * pData,
    tsp00_Int4                nLen,
    tsp00_ErrTextc            VAR_ARRAY_REF errtextC );

tsp00_Int4 cn14_replyReceive (
    void                    * sessionParm,
    void                    * pData,
    tsp00_Int4              * nLen,
    tsp00_ErrTextc            VAR_ARRAY_REF errtextC );

 tsp00_Int4 cn14_cmdExecute (
    void                         * sessionParm,
    const void                   * pInData,
    tsp00_Int4                     nInLen,
    void                         * pOutData,
    tsp00_Int4                   * nOutLen,
    bool                           bCheckForChallengeResponse,
    tsp00_ErrTextc   VAR_ARRAY_REF errtext );

 tsp00_Int4 cn14_dbmChallengeResponse (
    void                        * sessionParm,
    const void                  * pInData,
    tsp00_Int4                    nInLen,
    void                        * pOutData,
    tsp00_Int4                  * nOutLen,
    tsp00_ErrTextc  VAR_ARRAY_REF errtext );

tsp00_Int4 cn14_dbmLogon (
    void                    * sessionParm,
    const char*               user,
    Tools_DynamicUTF8String & oErrtext,
    const _TCHAR            * szCommand );

tsp00_Int4 cn14_dbmVersion 
	( void                    * sessionParm,
      Tools_DynamicUTF8String & oErrtext );    

tsp00_Int4 cn14_cmdWrite (
    void       * sessionParm,
    const void * data,
    tsp00_Int4     len);

tsp00_Int4 cn14_request (
    void            * sessionParm,
    tsp00_ErrTextc    VAR_ARRAY_REF errtextC );

tsp00_Int4 cn14_receive (
    void            * sessionParm,
    tsp00_ErrTextc    VAR_ARRAY_REF errtextC );

void cn14_errtextToC (
    tsp00_ErrTextc VAR_ARRAY_REF errtextC,
    tsp00_ErrText  VAR_ARRAY_REF errtextP );

tsp00_Int4 cn14_setErrtext (
    tsp00_ErrTextc & errtextC,
    tsp00_Int4       nRc );

tsp00_Int4 cn14_senduser (
    void                    * pSession,
    const char*               szServerNode,
    const char*               szDBName,
    Tools_DynamicUTF8String & oErrtext );    

void cn14_CopyError ( 
    const Tools_DynamicUTF8String & oSource, 
    tsp00_ErrTextc                & oTarget );

/*! @brief trim trailing whitespace */
template <int elemCount_TV>
void cn14_trimRight(
    tsp00_CString<elemCount_TV> VAR_ARRAY_REF cString);

/* UCS2 conversion macros */
#define SESSION_UNICODE(pSession) (pSession == NULL ? false : ((ControlSessionT *)pSession)->bUnicode)

#ifdef DBMUNICODETRACE

  void traceCode(const char * Message, const unsigned char * pBuffer, int nLength) {

    FILE * hFile = fopen("dbmuni.trc", "a");
    char   szBinary[200];
    char * pBinary = &szBinary[0];
    char   szText[200];
    char * pText = &szText[0];
    int i = 0;
    
    if (hFile != NULL) {
      fputs(Message, hFile);

      for (i = 0; i < nLength; ++ i) {
        sprintf(pBinary, "%02X ", pBuffer[i]);
        sprintf(pText,   " %c  ", ((pBuffer[i] > 31) ? pBuffer[i] : '.'));
        pBinary += 3;
        pText   += 3;
        if ((i % 25) == 24) {
          fputs(szText, hFile);
          fputs("\n", hFile);
          fputs(szBinary, hFile);
          fputs("\n", hFile);
          pBinary = &szBinary[0];
          pText = &szText[0];
        } /* end if */
      } /* end if */
      if (pBinary != &szBinary[0]) {
        fputs(szText, hFile);
        fputs("\n", hFile);
        fputs(szBinary, hFile);
        fputs("\n", hFile);
      } /* end if */
      fputs("\n", hFile);
      fclose(hFile);
    } /* end if */

  } /* end traceUCS2 */

#else 
  #define traceCode(a, b, c)
#endif

#ifdef DBMUNICODE

  #ifndef _WIN32
    #error "UCS2 enabling is Windows NT only"
  #endif

  #define UTF8_LEN(x) ((x) * 6)

  /* UCS2 to Intern Code */
  #define UCS2_TO_ACP(target, targetlen, source, sourcelen, ok)  \
    vcn14_UCS2_to_Intern(CP_ACP, target, targetlen, source, sourcelen, ok)

  #define UCS2_TO_UTF8(target, targetlen, source, sourcelen, ok)  \
    vcn14_UCS2_to_Intern(CP_UTF8, target, targetlen, source, sourcelen, ok)

  #define UCS2_TO_ACP_DYNAMIC(target, targetlen, source, sourcelen, ok)  \
    vcn14_UCS2_to_Intern_Dynamic(CP_ACP, target, targetlen, source, sourcelen, ok)

  #define UCS2_TO_UTF8_DYNAMIC(target, targetlen, source, sourcelen, ok)  \
    vcn14_UCS2_to_Intern_Dynamic(CP_UTF8, target, targetlen, source, sourcelen, ok)


  externC tsp00_Int4 vcn14_UCS2_to_Intern (
      UINT            targetcode, 
      char          * target,
      tsp00_Int4        targetlen,
      const char    * source,
      tsp00_Int4        sourcelen,
      bool          * ok) 
  {
    BOOL     bError  = FALSE;
    tsp00_Int4 nReturn = 0;

    traceCode("-- GUI to Server\nUCS2 before conversion:\n", (const char *) source, (sourcelen == -1) ? wcslen((const unsigned short *)source) * 2 : sourcelen * 2);
    nReturn = WideCharToMultiByte(targetcode,             
                                  0,                          
                                  (LPCWSTR) source,             
                                  sourcelen,                  
                                  target,                     
                                  targetlen,                  
                                  NULL,                       
                                  (targetcode == CP_ACP) ? &bError : NULL);                   
    traceCode("ASCII/UTF8 after conversion:\n", target, nReturn);
    *ok = *ok && (bError == FALSE) && (nReturn > 0);
    return nReturn;
  } /* end vcn14_UCS2_to_Intern */

  externC tsp00_Int4 vcn14_UCS2_to_Intern_Dynamic (
      UINT            targetcode, 
      char*&          target,
      tsp00_Int4&     targetlen,
      const char*     source,
      tsp00_Int4      sourcelen,
      bool*           ok) 
  {
    BOOL     bError  = FALSE;

    traceCode("-- GUI to Server (Dynamic)\nUCS2 before conversion:\n", (const char *) source, (sourcelen == -1) ? wcslen((const unsigned short *)source) * 2 : sourcelen * 2);
    targetlen = WideCharToMultiByte(targetcode,             
                                  0,                          
                                  (LPCWSTR) source,             
                                  sourcelen,                  
                                  NULL,                     
                                  targetlen,                  
                                  NULL,                       
                                  (targetcode == CP_ACP) ? &bError : NULL);                   

    target = new char[targetlen]; // byte for terminator is included
    if( target == NULL ) {
        *ok = false;
        return 0;
    }
    targetlen = WideCharToMultiByte(targetcode,             
                                  0,                          
                                  (LPCWSTR) source,             
                                  sourcelen,                  
                                  target,                     
                                  targetlen,                  
                                  NULL,                       
                                  (targetcode == CP_ACP) ? &bError : NULL);                   
    traceCode("ASCII/UTF8 (Dynamic) after conversion:\n", target, targetlen);
    *ok = *ok && (bError == FALSE) && (targetlen > 0);
    return targetlen;
  } /* end vcn14_UCS2_to_Intern_Dynamic */


  /* Intern Code to UCS2 */
  #define ACP_TO_UCS2(target, targetlen, source, sourcelen, ok)  \
    vcn14_Intern_to_UCS2(CP_ACP, target, targetlen, source, sourcelen, ok)

  #define UTF8_TO_UCS2(target, targetlen, source, sourcelen, ok) \
    vcn14_Intern_to_UCS2(CP_UTF8, target, targetlen, source, sourcelen, ok)

  externC tsp00_Int4 vcn14_Intern_to_UCS2 (
      UINT            sourcecode, 
      char          * target,
      tsp00_Int4        targetlen,
      const char    * source,
      tsp00_Int4        sourcelen,
      bool          * ok) 
  {
    tsp00_Int4 nReturn = 0;

    traceCode("-- Server to GUI\nASCII/UTF8 before conversion:\n", source, (sourcelen == -1) ? strlen(source) : sourcelen);
    nReturn =  MultiByteToWideChar(sourcecode,    
                                   0,
                                   source,              
                                   sourcelen,           
                                   (LPWSTR) target,     
                                   targetlen);
    traceCode("UCS2 after conversion:\n", (char *)  target, nReturn * 2);
    *ok = *ok && (nReturn > 0);
    return nReturn;
  } /* end  vcn14_Intern_to_UCS2 */

  /*
    -----------------------------------------------------------------------------
    public function cn14connectDBM (UCS2 Version)
    -----------------------------------------------------------------------------
   */
  externC tsp00_Int4 WINAPI_CN14 cn14connectDBM (
      const char*               servernodeU,
      const char*               dbnameU,
      const char*               dbrootU,
      void                   ** sessionOut,
      tsp00_ErrTextc            VAR_ARRAY_REF errtextU )
{
    if( servernodeU == NULL || dbnameU == NULL || dbrootU == NULL || errtextU == NULL) 
        return DBMAPI_NULLPARAM_CN14;
      
    tsp00_Int4 nRc  = DBMAPI_OK_CN14;

    char*             servernode(NULL);
    char*             dbname(NULL);
    char*             dbroot(NULL);
    tsp00_ErrTextc    errtext;
    bool              bOk = true;

    errtext.Init();

    /* Place here USC2 -> ASCII/UTF8 */
    tsp00_Int4 lenIntern(0);
    UCS2_TO_ACP_DYNAMIC(servernode, lenIntern, servernodeU, -1, &bOk);
    lenIntern = 0;
    UCS2_TO_ACP_DYNAMIC(dbname, lenIntern, dbnameU, -1, &bOk);
    lenIntern = 0;
    UCS2_TO_ACP_DYNAMIC(dbroot, lenIntern, dbrootU, -1, &bOk);

    if (!bOk) {
      nRc = cn14_setErrtext(errtext, DBMAPI_USC2_CN14);
    } else {
      Tools_DynamicUTF8String oErrtext;
      nRc = cn14_connectDBM ( servernode, dbname, dbroot, sessionOut, oErrtext);
      cn14_CopyError(oErrtext, errtext);
    } /* end if */

    if( servernode != NULL )
        delete[] servernode;
    if( dbname != NULL )
        delete[] dbname;
    if( dbroot != NULL )
        delete[] dbroot;

    /* Place here ASCII/UTF8 -> USC2 */
    if (SESSION_UNICODE(*sessionOut)) {
      UTF8_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } else {
      ACP_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } /* end if */

    return nRc;
  } /* end cn14connectDBM */

  /*
    -----------------------------------------------------------------------------
    public function cn14connectDBMUsr (UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14connectDBMUsr (
      const char*               servernodeU,
      const char*               dbnameU,
      const char*               dbrootU,
      const char*               userU,
      void                   ** sessionOut,
      tsp00_ErrTextc            VAR_ARRAY_REF errtextU )
  {
    tsp00_Int4 nRc  = DBMAPI_OK_CN14;

    if( servernodeU == NULL || dbnameU == NULL || dbrootU == NULL || errtextU == NULL) 
        return DBMAPI_NULLPARAM_CN14;

    char*             servernode(NULL);
    char*             dbname(NULL);
    char*             dbroot(NULL);
    char*             user(NULL);
    tsp00_ErrTextc    errtext;
    bool              bOk = true;

    errtext.Init();

    /* Place here USC2 -> ASCII/UTF8 */
    tsp00_Int4 lenIntern(0);
    if (SESSION_UNICODE(*sessionOut)) {
      UCS2_TO_UTF8_DYNAMIC(servernode, lenIntern, servernodeU, -1, &bOk);
      lenIntern = 0;
      UCS2_TO_UTF8_DYNAMIC(dbname, lenIntern, dbnameU, -1, &bOk);
      lenIntern = 0;
      UCS2_TO_UTF8_DYNAMIC(dbroot, lenIntern, dbrootU, -1, &bOk);
      lenIntern = 0;
      UCS2_TO_UTF8_DYNAMIC(user, lenIntern, userU, -1, &bOk);
    } else {
      UCS2_TO_ACP_DYNAMIC(servernode, lenIntern, servernodeU, -1, &bOk);
      lenIntern = 0;
      UCS2_TO_ACP_DYNAMIC(dbname, lenIntern, dbnameU, -1, &bOk);
      lenIntern = 0;
      UCS2_TO_ACP_DYNAMIC(dbroot, lenIntern, dbrootU, -1, &bOk);
      lenIntern = 0;
      UCS2_TO_ACP_DYNAMIC(user, lenIntern, userU, -1, &bOk);
    } /* end if */

    if (!bOk) {
      nRc = cn14_setErrtext(errtext, DBMAPI_USC2_CN14);
    } else {
      Tools_DynamicUTF8String oErrtext;
      nRc = cn14_connectDBMUsr (servernode, dbname, dbroot, user, sessionOut, oErrtext );
      cn14_CopyError(oErrtext, errtext);
    } /* end if */

    if( servernode != NULL )
        delete[] servernode;
    if( dbname != NULL )
        delete[] dbname;
    if( dbroot != NULL )
        delete[] dbroot;
    if( user != NULL )
        delete[] user;

    /* Place here ASCII/UTF8 -> USC2 */
    if (SESSION_UNICODE(*sessionOut)) {
      UTF8_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } else {
      ACP_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } /* end if */

    return nRc;
  } /* end cn14connectDBMUsr */

  /*
    -----------------------------------------------------------------------------
    public function cn14replyAvailable (UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14replyAvailable (
      void           * sessionParm,
      char*            errtextU )
  {
    if(errtextU == NULL)
        return DBMAPI_NULLPARAM_CN14;

    tsp00_Int4 nRc  = DBMAPI_OK_CN14;
    tsp00_ErrTextc    errtext;
    bool              bOk = true;

    errtext.Init();

    /* Place here USC2 -> ASCII/UTF8 */
    /* nothing todo */

    nRc = cn14_replyAvailable ( sessionParm, errtext);

    /* Place here ASCII/UTF8 -> USC2 */
    if (SESSION_UNICODE(sessionParm)) {
      UTF8_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } else {
      ACP_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } /* end if */

    return nRc;
  } /* end cn14replyAvailable */

  /*
    -----------------------------------------------------------------------------
    public function cn14cmdRequest (UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14cmdRequest (
      void           * sessionParm,
      const void     * pDataU,
      tsp00_Int4       nLen,
      char*            errtextU )
  {
    if(errtextU == NULL)
        return DBMAPI_NULLPARAM_CN14;
      
    tsp00_Int4 nRc  = DBMAPI_OK_CN14;

    tsp00_ErrTextc    errtext;
    char*             pData(NULL);
    bool              bOk   = true;

    errtext.Init();

    /* Place here USC2 -> ASCII/UTF8 */
    tsp00_Int4 lenIntern(0);
    if (SESSION_UNICODE(sessionParm)) {
      UCS2_TO_UTF8_DYNAMIC( pData, lenIntern, (const char*)pDataU, nLen, &bOk );
    } else {
      UCS2_TO_ACP_DYNAMIC( pData, lenIntern, (const char*)pDataU, nLen, &bOk );
    } /* end if */

    if (!bOk) {
      nRc = cn14_setErrtext(errtext, DBMAPI_USC2_CN14);
    } else {
      nRc = cn14_cmdRequest ( sessionParm, pData, lenIntern, errtext );
    } /* end if */


    if( pData != NULL )
        delete[] pData;

    /* Place here ASCII/UTF8 -> USC2 */
    if (SESSION_UNICODE(sessionParm)) {
      UTF8_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } else {
      ACP_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } /* end if */

    return nRc;
  } /* end cn14cmdRequest */

  /*
    -----------------------------------------------------------------------------
    public function cn14replyReceive (UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14replyReceive (
      void           * sessionParm,
      void           * pDataU,
      tsp00_Int4     * nLen,
      char*            errtextU )
  {
    if(errtextU == NULL)
        return DBMAPI_NULLPARAM_CN14;
      
    tsp00_Int4 nRc  = DBMAPI_OK_CN14;

    tsp00_ErrTextc    errtext;
    tsp00_Int4        nOrgLen = *nLen;
    char*             pData(new char[UTF8_LEN(*nLen)]); // upper bound
    bool              bOk   = true;

    errtext.Init();

    if (pData != NULL) {

      /* Place here USC2 -> ASCII/UTF8 */
      /* nothing todo */

      nRc = cn14_replyReceive ( sessionParm, pData, nLen, errtext );

      /* Place here ASCII/UTF8 -> USC2 */
      if (SESSION_UNICODE(sessionParm)) {
        *nLen = UTF8_TO_UCS2((char*)pDataU, nOrgLen, pData, *nLen, &bOk);
      } else {
        *nLen = ACP_TO_UCS2((char*)pDataU, nOrgLen, pData, *nLen, &bOk);
      } /* end if */

    } else {
      nRc = cn14_setErrtext(errtext, DBMAPI_MEMORY_CN14);
    } /* end if */

    /* Place here ASCII/UTF8 -> USC2 */
    if (SESSION_UNICODE(sessionParm)) {
      UTF8_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } else {
      ACP_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } /* end if */

    delete[] pData;

    return nRc;
  } /* end cn14replyReceive */

  /*
    -----------------------------------------------------------------------------
    public function cn14cmdExecute (UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14cmdExecute (
      void           * sessionParm,
      const void     * pInDataU,
      tsp00_Int4       nInLen,
      void           * pOutDataU,
      tsp00_Int4     * nOutLen,
      char*            errtextU )
  {
    if(errtextU == NULL)
        return DBMAPI_NULLPARAM_CN14;
      
      tsp00_Int4 nRc  = DBMAPI_OK_CN14;

    tsp00_ErrTextc    errtext;
    errtext.Init();
    tsp00_Int4          nOrgOutLen = *nOutLen;
    char*               pInData(NULL);
    char              * pOutData(new char[UTF8_LEN(*nOutLen)]);
    bool                bOk   = true;

    if ( pOutData != NULL) {

      /* Place here USC2 -> ASCII/UTF8 */
      tsp00_Int4 lenInternal(0);
      if (SESSION_UNICODE(sessionParm)) {
        UCS2_TO_UTF8_DYNAMIC(pInData, lenInternal, (const char*)pInDataU, nInLen, &bOk);
      } else {
        UCS2_TO_ACP(pInData, lenInternal, (const char*)pInDataU, nInLen, &bOk);
      } /* end if */

      if (!bOk) {
        nRc = cn14_setErrtext(errtext, DBMAPI_USC2_CN14);
      } else {
        nRc = cn14_cmdExecute(sessionParm, pInData, lenInternal, pOutData, nOutLen, true, errtext );
      } /* end if */

      if( pInData != NULL )
          delete[] pInData;

      /* Place here ASCII/UTF8 -> USC2 */
      if (SESSION_UNICODE(sessionParm)) {
        *nOutLen = UTF8_TO_UCS2((char*)pOutDataU, nOrgOutLen, pOutData, *nOutLen, &bOk);
      } else {
        *nOutLen = ACP_TO_UCS2((char*)pOutDataU, nOrgOutLen, pOutData, *nOutLen, &bOk);
      } /* end if */

    } else {
      nRc = cn14_setErrtext(errtext, DBMAPI_MEMORY_CN14);
    } /* end if */

    /* Place here ASCII/UTF8 -> USC2 */
    if (SESSION_UNICODE(sessionParm)) {
      UTF8_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } else {
      ACP_TO_UCS2(errtextU, errtext.size() * 2, errtext, strlen(errtext), &bOk);
    } /* end if */

    delete[] pOutData;

    return nRc;
  } /* end cn14cmdExecute */

#else /* #ifdef DBMUNICODE */

  /*
    -----------------------------------------------------------------------------
    public function cn14connectDBM (_non_ UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14connectDBM (
      const char*               servernode,
      const char*               dbname,
      const char*               dbroot,
      void                   ** sessionOut,
      tsp00_ErrTextc            VAR_ARRAY_REF errtext )
  {
    Tools_DynamicUTF8String oErrtext;
    tsp00_Int4 nRc = cn14connectDBM (servernode, dbname, dbroot, sessionOut, oErrtext);
    if (errtext != NULL) {
        cn14_CopyError(oErrtext, errtext);
    } // end if
    return nRc;
  } // end cn14connectDBM
  /*-----------------------------------------------------------------------------*/
  tsp00_Int4 cn14connectDBM (
      const char*               servernode,
      const char*               dbname,
      const char*               dbroot,
      void                   ** sessionOut,
      Tools_DynamicUTF8String & oErrtext )
  {
    if( servernode == NULL || dbname == NULL || dbroot == NULL) 
        return DBMAPI_NULLPARAM_CN14;

    tsp00_Int4 rc = cn14_connectDBM ( servernode, dbname, dbroot, sessionOut, oErrtext);
    return rc;
  } /* end cn14connectDBM */

  /*
    -----------------------------------------------------------------------------
    public function cn14connectDBMUsr (_non_ UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14connectDBMUsr (
      const char*               servernode,
      const char*               dbname,
      const char*               dbroot,
      const char*               user,
      void                   ** sessionOut,
      tsp00_ErrTextc            VAR_ARRAY_REF errtext )
  {
    Tools_DynamicUTF8String oErrtext;
    tsp00_Int4 nRc = cn14connectDBMUsr (servernode, dbname, dbroot, user, sessionOut, oErrtext);
    if (errtext != NULL) {
        cn14_CopyError(oErrtext, errtext);
    } // end if
    return nRc;
  } // end cn14connectDBM
  /*-----------------------------------------------------------------------------*/
 tsp00_Int4 cn14connectDBMUsr (
      const char*               servernode,
      const char*               dbname,
      const char*               dbroot,
      const char*               user,
      void                   ** sessionOut,
      Tools_DynamicUTF8String & oErrtext )
  {
    if( servernode == NULL || dbname == NULL || dbroot == NULL || user == NULL ) 
        return DBMAPI_NULLPARAM_CN14;
      
    tsp00_Int4 rc = cn14_connectDBMUsr (servernode, dbname, dbroot, user, sessionOut, oErrtext );
    return rc;
  } /* end cn14connectDBMUsr */

  /*
    -----------------------------------------------------------------------------
    public function cn14replyAvailable (_non_ UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14replyAvailable (
      void           * sessionParm,
      char*            errtext )
  {
    if(errtext == NULL) 
        return DBMAPI_NULLPARAM_CN14;

    tsp00_ErrTextc myErrText;
    myErrText.Init();
    tsp00_Int4 rc = cn14_replyAvailable ( sessionParm, myErrText);
    SAPDB_strcpy(errtext, myErrText.asCharp());
    return rc;
  } /* end cn14replyAvailable */

  /*
    -----------------------------------------------------------------------------
    public function cn14cmdRequest (_non_ UCS2 Version)
    -----------------------------------------------------------------------------
  */
 externC tsp00_Int4 WINAPI_CN14 cn14cmdRequest (
      void           * sessionParm,
      const void     * pData,
      tsp00_Int4       nLen,
      char*            errtext )
  {
    if(errtext == NULL) 
        return DBMAPI_NULLPARAM_CN14;

    tsp00_ErrTextc myErrText;
    myErrText.Init();
    tsp00_Int4 rc = cn14_cmdRequest ( sessionParm, pData, nLen, myErrText );
    SAPDB_strcpy(errtext, myErrText.asCharp());
    return rc;
  } /* end cn14cmdRequest */

  /*
    -----------------------------------------------------------------------------
    public function cn14replyReceive (_non_ UCS2 Version)
    -----------------------------------------------------------------------------
  */
 externC tsp00_Int4 WINAPI_CN14 cn14replyReceive (
      void           * sessionParm,
      void           * pData,
      tsp00_Int4     * nLen,
      char*          errtext )
  {
    if(errtext == NULL) 
        return DBMAPI_NULLPARAM_CN14;

    tsp00_ErrTextc myErrText;
    myErrText.Init();
    tsp00_Int4 rc = cn14_replyReceive ( sessionParm, pData, nLen, myErrText );
    SAPDB_strcpy(errtext, myErrText.asCharp());
    return rc;
  } /* end cn14replyReceive */

  /*
    -----------------------------------------------------------------------------
    public function cn14cmdExecute (_non_ UCS2 Version)
    -----------------------------------------------------------------------------
  */
  externC tsp00_Int4 WINAPI_CN14 cn14cmdExecute (
      void           * sessionParm,
      const void     * pInData,
      tsp00_Int4       nInLen,
      void           * pOutData,
      tsp00_Int4     * nOutLen,
      char*            errtext )
  {
    if(errtext == NULL) 
        return DBMAPI_NULLPARAM_CN14;

      
    tsp00_ErrTextc myErrText;
    myErrText.Init();
    tsp00_Int4 rc = cn14_cmdExecute(sessionParm, pInData, nInLen, pOutData, nOutLen, true, myErrText );
    SAPDB_strcpy(errtext, myErrText.asCharp());
    return rc;
  } /* end cn14cmdExecute */


    /*
      -----------------------------------------------------------------------------
      public function cn14ExecuteLoaderCmd (_non_ UCS2 Version)
      -----------------------------------------------------------------------------
    */
    externC tsp00_Int4 WINAPI_CN14 cn14ExecuteLoaderCmd(
        void           * sessionParm,
        const void     * pInData,
        tsp00_Int4       nInLen,
        void           * pOutData,
        tsp00_Int4     * nOutLen,
        char*            errtext )
    {
        if(errtext == NULL) 
            return DBMAPI_NULLPARAM_CN14;

        tsp00_Int4          rc      = DBMAPI_OK_CN14;
        ControlSessionT * session = (ControlSessionT*)sessionParm;


        /* Space that can be used in packet for a single command is:
           packetsize - replylen (1024 Bytes) - protocol_header_size (4 Bytes) */
        tsp00_Int4  _nMaxDataLen    = session->packetSize - RPM_REPLY_BUFFER_LEN_CN14 - RPM_PROTOCOL_SIZE_CN14;
        tsp00_Int4  _nDataLen       = _nMaxDataLen;
        
        tsp00_Int4  _nRemainderLen  = nInLen;
        char     *_pTmpDataBuffer = (char*)malloc(session->packetSize);
        char     *_pPtrToRealData = _pTmpDataBuffer + RPM_PROTOCOL_SIZE_CN14;
        

        /* The buffer sent to Loader server looks as follows:
            |-----------------------------------------------------------------------|
            |                   |                       |                           |
            |      protocol     |        data           | space for server reply    |
            |     (4 Bytes)     |  (nMaxDataLen Bytes)  |     (1024 Bytes)          |
            |-----------------------------------------------------------------------|
            ^                   ^
            ^                   ^
            ^                   This is where _pPtrToRealData points to
            ^
            this is where _pTmpDataBuffer points to
        */

        /* Set protocol header */
        memset(_pTmpDataBuffer, 0, RPM_PROTOCOL_SIZE_CN14);

        /* Process command (continued or not) */
        while ( (_nRemainderLen > (_nMaxDataLen) ) && (DBMAPI_OK_CN14 == rc) )
        {
            _nDataLen = _nMaxDataLen;

            /* Signal the server a continued command */
            _pTmpDataBuffer[0] = '\x01';

            /* Copy parts of command to message buffer */
            SAPDB_memcpy(_pPtrToRealData, &((char*)pInData)[nInLen - _nRemainderLen], _nDataLen);

            /* move to the last white space in string and adapt length */
            
            for (_nDataLen; (0 == isspace(_pPtrToRealData[_nDataLen - 1])); --_nDataLen)
                ;

            _nRemainderLen -= _nDataLen;

            /* Send command part to server */
            tsp00_ErrTextc myErrText;
            myErrText.Init();
            rc = cn14_cmdExecute(sessionParm,
                                 _pTmpDataBuffer,
                                 _nDataLen + RPM_PROTOCOL_SIZE_CN14,
                                 pOutData,
                                 nOutLen,
                                 false, 
                                 myErrText);
            SAPDB_strcpy(errtext, myErrText.asCharp());
        }

        /* Send last part of a continued command or a single (short) command to server and
           return answer to requesting client */
        if (DBMAPI_OK_CN14 == rc)
        {
            /* Signal the server that either last part of continued command or single command */
            _pTmpDataBuffer[0] = 0;

            /* Copy command to message buffer */
            SAPDB_memcpy(_pPtrToRealData, &((char*)pInData)[nInLen - _nRemainderLen], _nRemainderLen);

            /* Reset reply buffer length that might have been manipulated during while loop */
            if (NULL != nOutLen)
            {
                *nOutLen = RPM_REPLY_BUFFER_LEN_CN14;
            }

            /* Send command to server */
            tsp00_ErrTextc myErrText;
            myErrText.Init();
            rc = cn14_cmdExecute(sessionParm,
                                 _pTmpDataBuffer,
                                 _nRemainderLen + RPM_PROTOCOL_SIZE_CN14,
                                 pOutData,
                                 nOutLen,
                                 false,
                                 myErrText);
            SAPDB_strcpy(errtext, myErrText.asCharp());
        }

        /* Cleaning */
        _pPtrToRealData = NULL;   /* remove references */
        free(_pTmpDataBuffer);
        _pTmpDataBuffer = NULL;

        return rc;
    } /* end cn14ExecuteLoaderCmd */


    /*
      -----------------------------------------------------------------------------
      public function cn14connectRPM (_non_ UCS2 Version)
      -----------------------------------------------------------------------------
    */
    externC tsp00_Int4 cn14connectRPM_fixedSizeParameters (
            const tsp00_NodeIdc      VAR_ARRAY_REF servernode,
            const tsp00_DbNamec      VAR_ARRAY_REF dbname,
            const tsp00_VFilenamec   VAR_ARRAY_REF dbroot,
            char                    *pszData,
            void                   **sessionOut,
            tsp00_ErrTextc           VAR_ARRAY_REF errtext)
    {
      tsp00_NodeIdc myNode;
      myNode.rawAssign(servernode.asCharp());
      cn14_trimRight(myNode);
      tsp00_DbNamec myDbname;
      myDbname.rawAssign(dbname.asCharp());
      cn14_trimRight(myDbname);
      tsp00_VFilenamec myDbroot;
      myDbroot.rawAssign(dbroot.asCharp());
      cn14_trimRight(myDbroot);

      return cn14connectRPM(myNode.asCharp(), myDbname.asCharp(), myDbroot.asCharp(), pszData, sessionOut, errtext);
    }

    externC tsp00_Int4 cn14connectRPM (
            const char*              servernode,
            const char*              dbname,
            const char*              dbroot,
            char                    *pszData,
            void                   **sessionOut,
            tsp00_ErrTextc           VAR_ARRAY_REF errtext)
    {
        if( servernode == NULL || dbname == NULL || dbroot == NULL) 
            return DBMAPI_NULLPARAM_CN14;

        tsp00_Int4  rc = DBMAPI_OK_CN14;

#ifdef RPM_VERSION_INDEPENDENCE

        tsp01_RteError  Rte1Error;
        tsp00_Pathc     _szInstRoot;
        tsp00_DbNamec   _szDBName;

        /*
         *  Start standalone Loader server ALWAYS in independent program path or in given dbroot
         * 
         * That's new with version independence of the Loader
         */

        /* Ignore db name in any case */
        _szDBName[0] = 0;

        /* Check if dbroot is given */
        if (NULL == dbroot || 0 == dbroot[0])
        {
            sqlGetIndependentProgramsPath (_szInstRoot, TERM_WITHOUT_DELIMITER_EO01, &Rte1Error);
        }
        else
        {
            /* Coming here from the client the db name IS empty */
            size_t slen = _tcslen(dbroot);
            SAPDB_memcpy(_szInstRoot, dbroot, slen);
            _szInstRoot[slen] = 0;
        }

        /* Now, finally, connect */
        rc = cn14connect(servernode, _szDBName, _szInstRoot, DEFAULT_RPMPGM_CN14, sessionOut, errtext);
#else
        rc = cn14connect (servernode, dbname, dbroot, DEFAULT_RPMPGM_CN14, sessionOut, errtext);
#endif
        if (DBMAPI_OK_CN14 == rc)
        {
            tsp00_Int4 rc2 = DBMAPI_OK_CN14;
            tsp00_Int4 lDataLen;

#ifndef _WIN32
            const char *myUser;
            char       *cmdStr;
            tsp00_Int4 len = 0;

            myUser = sqlxusername();       /* get user name */

            cmdStr = (char *)malloc(strlen("set logowner ") + strlen(myUser) + 1);
            sprintf(cmdStr, "set logowner %s", (char*)myUser);
            len = strlen(cmdStr);
            rc2 = cn14_cmdExecute (*sessionOut, cmdStr, len, NULL, &lDataLen, false, errtext);
            free(cmdStr);
#endif
            if (NULL != pszData)
            {
                rc2 = cn14ExecuteLoaderCmd (*sessionOut, "set", 3, NULL, &lDataLen, errtext);
                if (DBMAPI_OK_CN14 == rc2)
                {
                    tsp00_Int4   lErrCode;
                    tsp00_Int4   lSqlCode;
                    tsp00_Int4   lSqlErrLen;
                    const char *pszMyReply = NULL;
                    cn14analyzeRpmAnswer(*sessionOut, &lErrCode, &pszMyReply, &lDataLen,
                                                      &lSqlCode, NULL,     &lSqlErrLen);
                    SAPDB_memcpy(pszData, pszMyReply, lDataLen);
                    pszData[lDataLen] = 0;
                }
            }
        }

        return rc;
    } /* end cn14connectRPM */


#endif /* #ifdef DBMUNICODE ... #else */

/*
  -----------------------------------------------------------------------------
  public function cn14encryptionMethodSSLAvailable
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 WINAPI_CN14 cn14encryptionMethodSSLAvailable( char* errtext ) {
    if( errtext == NULL ) 
        return DBMAPI_NULLPARAM_CN14;

    tsp00_Int4 nRc(DBMAPI_ENCRYPTION_SSL_AVAILABLE_NO);
    tsp00_ErrTextc myErrText;
    myErrText.Init();

    if( SqlSSLAvailable(myErrText) )
        nRc = DBMAPI_ENCRYPTION_SSL_AVAILABLE_YES;

#if defined(DBMUNICODE)
    bool okay(true);
    ACP_TO_UCS2(errtext, myErrText.size() * 2, myErrText, myErrText.length(), &okay);
#else
    SAPDB_strcpy(errtext, myErrText.asCharp());
#endif
    return nRc;
}

/*
  -----------------------------------------------------------------------------
  private function cn14_connectDBM
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_connectDBM (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    void                   ** sessionOut,
    Tools_DynamicUTF8String & oErrtext )
{
  	tsp00_Int4            nRc  = DBMAPI_OK_CN14;

  	// connect to server
    tsp00_ErrTextc errtext;
    errtext.Init();
  	nRc = cn14connect (servernode, dbname, dbroot, DEFAULT_DBMPGM_CN14, sessionOut, errtext);
    oErrtext.Assign(errtext.asCharp());
  
  	if (nRc == DBMAPI_OK_CN14) {
  		// connection established -> check version
    	nRc = cn14_dbmVersion(*sessionOut, oErrtext);
  	} // end if
  
  	if (nRc == DBMAPI_OK_CN14 && _tcslen(dbname) > 0) {
    	// try to send xuser data
    	nRc = cn14_senduser(*sessionOut, servernode, dbname, oErrtext );
    } // end if
  
    if (nRc != DBMAPI_OK_CN14) {
      	cn14release (sessionOut);
    } // end if

  	return nRc;
} /* end cn14_connectDBM */

/*
  -----------------------------------------------------------------------------
  private function cn14_connectDBMUsr
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_connectDBMUsr (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    const char*               user,
    void                   ** sessionOut,
    Tools_DynamicUTF8String & oErrtext )
{
  	tsp00_Int4            nRc  = DBMAPI_OK_CN14;

  	// connect to server
    tsp00_ErrTextc errtext;
    errtext.Init();
  	nRc = cn14connect (servernode, dbname, dbroot, DEFAULT_DBMPGM_CN14, sessionOut, errtext);
    oErrtext.Assign(errtext.asCharp());
  
  	if (nRc == DBMAPI_OK_CN14) {
  		// connection established -> check version
    	nRc = cn14_dbmVersion(*sessionOut, oErrtext);
  	} // end if
  
  	if (nRc == DBMAPI_OK_CN14 && _tcslen(user) > 0) {
  		// send user data to server
    	nRc = cn14_dbmLogon(*sessionOut, user, oErrtext, (strlen(dbname) == 0) ? DBM_SYSTEMLOGON : DBM_USERLOGON);
    } // end if
  
    if (nRc != DBMAPI_OK_CN14) {
      	cn14release (sessionOut);
    } // end if

  	return nRc;
} /* end cn14_connectDBMUsr */

/*
  -----------------------------------------------------------------------------
  public function cn14connectDBMTp
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14connectDBMTp (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    const char*               sysid,
    const char*               conn,
    const char*               profile,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext )
{
    Tools_DynamicUTF8String oErrtext;
    tsp00_Int4 nRc = cn14connectDBMTp (servernode, dbname, dbroot, sysid, conn, profile, sessionOut, oErrtext);
    if (errtext != NULL) {
        cn14_CopyError(oErrtext, errtext);
    } // end if
    return nRc;
} // end cn14connectDBM
/*-----------------------------------------------------------------------------*/
tsp00_Int4 cn14connectDBMTp (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    const char*               sysid,
    const char*               conn,
    const char*               profile,
    void                   ** sessionOut,
    Tools_DynamicUTF8String & oErrtext )
{
  if( servernode == NULL || dbname == NULL || dbroot == NULL || sysid == NULL || conn == NULL || profile == NULL) 
    return DBMAPI_NULLPARAM_CN14;

  tsp00_Int4         nRc   = DBMAPI_OK_CN14;
  tsp00_C64c         user;

  tsp00_ErrTextc errtext;
  errtext.Init();
  nRc = cn14GetTPUser(TP_USERMODE_DBM_CN14, sysid, conn, profile, user, errtext);
  oErrtext.Assign(errtext.asCharp());

  if (nRc == DBMAPI_OK_CN14) {
    nRc = cn14_connectDBMUsr (servernode, dbname, dbroot, user, sessionOut, oErrtext);
  } /* end if */

  return nRc;
} /* end cn14connectDBMTp */

/*
  -----------------------------------------------------------------------------
  public function cn14connect
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14connect (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    const char*               serverpgm,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtextC )
{
  if( servernode == NULL || dbname == NULL || dbroot == NULL || serverpgm == NULL) 
    return DBMAPI_NULLPARAM_CN14;
    
  tsp00_Int4              nRc          = DBMAPI_OK_CN14;
  static tsp00_CompName   szComponent;
  tsp01_CommErr         commErr;
  ControlSessionT     * session(NULL);

  // for empty dbroot use $INSTROOT if exists
  if ((SAPDB_strlen(dbroot) == 0) && (SAPDB_strlen(dbname) == 0) && (SAPDB_strlen(servernode) == 0)) {
    const char * pOwn      = getenv("OWN");
    if ((pOwn != NULL) && (SAPDB_strlen(pOwn) > 0)) {
      const char * pInstroot = getenv("INSTROOT");
      if (pInstroot != NULL)
        dbroot = pInstroot;
    } // end if
  } // end if

  // must do this to get rid of the const
  char* myServerNode(new char[SAPDB_strlen(servernode)+1]);
  if( myServerNode == NULL )
    return cn14_setErrtext(errtextC, DBMAPI_MEMORY_CN14);
  char* myDbname(new char[SAPDB_strlen(dbname)+1]);
  if( myDbname == NULL ) {
    delete[] myServerNode;
    return cn14_setErrtext(errtextC, DBMAPI_MEMORY_CN14);
  }
  char* myDbroot(new char[SAPDB_strlen(dbroot)+1]);
  if( myDbroot == NULL ) {
    delete[] myServerNode;
    delete[] myDbname;
    return cn14_setErrtext(errtextC, DBMAPI_MEMORY_CN14);
  }
  char* myServerpgm(new char[SAPDB_strlen(serverpgm)+1]);
  if( myServerpgm == NULL ) {
    delete[] myServerNode;
    delete[] myDbname;
    delete[] myDbroot;
    return cn14_setErrtext(errtextC, DBMAPI_MEMORY_CN14);
  }

  SAPDB_strcpy(myServerNode, servernode);
  SAPDB_strcpy(myDbname, dbname);
  SAPDB_strcpy(myDbroot, dbroot);
  SAPDB_strcpy(myServerpgm, serverpgm);

  /* init rte with pascal string */
  szComponent.SetBlank();
  _tcsncpy((_TCHAR *) szComponent, C_COMPONENT, _tcslen(C_COMPONENT));
  tsp00_Bool okay;
  sqlinit (szComponent, &okay);

    /* alloc memory for session structure */
    session = (ControlSessionT *) malloc (sizeof (ControlSessionT));;
    *sessionOut = session;

  if (session == NULL) {
    nRc = cn14_setErrtext(errtextC, DBMAPI_MEMORY_CN14);
  } else {

    session->packetPos     = 0;
    session->packetInData  = NULL;
    session->packetOutData = NULL;
    session->bUnicode      = false;
    memset((char*)errtextC, 0, errtextC.size());
    
    sqlx2connectc (0,
                   myServerNode,
                   myDbname,
                   myDbroot,
                   myServerpgm,
                   &session->reference,
                   &session->packetSize,
                   &session->packetData,
                   errtextC,
                   &commErr);

    delete[] myServerNode;
    delete[] myDbname;
    delete[] myDbroot;
    delete[] myServerpgm;

    if (commErr != commErrOk_esp01) {
      errtextC[errtextC.size() - 1] = 0;
      nRc = DBMAPI_COMMERR_CN14;
      free(session);
      *sessionOut = NULL;
    } /* end if */

  } /* end if */

  return nRc;
} /* end cn14connect */

/*
  -----------------------------------------------------------------------------
  public function cn14release
  -----------------------------------------------------------------------------
*/
externC void WINAPI_CN14 cn14release (
    void** sessionParm)
{
  ControlSessionT* session = (ControlSessionT*)*sessionParm;
  tsp00_Int4         nDummy;
  tsp00_ErrTextc   errtext;
  errtext.Init();

  if (session != NULL) {
    cn14_cmdExecute(session, "release", (tsp00_Int4) strlen("release") + 1, NULL, &nDummy, false, errtext);

    sqlarelease (session->reference);
    free (session);
    *sessionParm = NULL;
  } /* end if */

} /* end cn14release */

/*
  -----------------------------------------------------------------------------
  public function cn14cmdCancel
  -----------------------------------------------------------------------------
*/
externC void WINAPI_CN14 cn14cmdCancel (
    void* sessionParm)
{
  ControlSessionT* session = (ControlSessionT*)sessionParm;

  if (session != NULL) {
    sqlacancel (session->reference);
  } /* end if */

} /* cn14cmdCancel */

/*
  -----------------------------------------------------------------------------
  private function cn14_replyAvailable
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_replyAvailable (
    void           * sessionParm,
    tsp00_ErrTextc   VAR_ARRAY_REF errtextC )
{
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;
  ControlSessionT * session = (ControlSessionT*)sessionParm;
  tsp01_CommErr     commErr;
  tsp00_ErrText     errtextP;

  if ((session == NULL) || (session->packetInData != NULL)) {
    nRc = cn14_setErrtext(errtextC, DBMAPI_INVSESSION_CN14);
  } else {

    sqlareplyavailable (session->reference, errtextP, commErr);

    if (commErr != commErrOk_esp01) {
      cn14_errtextToC (errtextC, errtextP);
      nRc = DBMAPI_COMMERR_CN14;
    } /* end if */

  } /* end if */

  return nRc;
} /* end cn14_replyAvailable */

/*
  -----------------------------------------------------------------------------
  private function cn14_cmdRequest
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_cmdRequest (
    void           * sessionParm,
    const void     * pData,
    tsp00_Int4       nLen,
    tsp00_ErrTextc   VAR_ARRAY_REF errtextC )
{
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;

  nRc =  cn14_cmdWrite (sessionParm, pData, nLen);
  if (nRc != DBMAPI_OK_CN14) {
    nRc = cn14_setErrtext(errtextC, nRc);
  } else {
    nRc = cn14_request (sessionParm, errtextC);
  } /* end if */

  return nRc;
} /* end cn14_cmdRequest */

/*
  -----------------------------------------------------------------------------
  private function cn14_replyReceive
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_replyReceive (
    void           * sessionParm,
    void           * pData,
    tsp00_Int4     * nLen,
    tsp00_ErrTextc   VAR_ARRAY_REF errtextC )
{
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;

  nRc = cn14_receive (sessionParm, errtextC);
  if (nRc == DBMAPI_OK_CN14) {
    if (pData != NULL) {
      nRc =  cn14replyRead (sessionParm, pData, nLen);
      if (nRc != DBMAPI_OK_CN14) {
       nRc = cn14_setErrtext(errtextC, nRc);
      } /* end if */
    } /* end if */
  } /* end if */

  return nRc;
} /* end cn14_replyReceive */

/*
  -----------------------------------------------------------------------------
  private function cn14_cmdExecute
  -----------------------------------------------------------------------------
*/
 tsp00_Int4 cn14_cmdExecute (
    void                         * sessionParm,
    const void                   * pInData,
    tsp00_Int4                     nInLen,
    void                         * pOutData,
    tsp00_Int4                   * nOutLen,
    bool                           bCheckForChallengeResponse,
    tsp00_ErrTextc   VAR_ARRAY_REF errtext )
{
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;

  if (bCheckForChallengeResponse && (strnicmp(DBM_USERLOGON" ", (const char *) pInData, strlen(DBM_USERLOGON" ")) == 0)) {
    // we have a user_logon -> switch to challenge response
    nRc = cn14_dbmChallengeResponse(sessionParm, pInData, nInLen, pOutData, nOutLen, errtext);
  } else {
    nRc = cn14_cmdRequest(sessionParm, pInData, nInLen, errtext);
    if (nRc == DBMAPI_OK_CN14) {
      nRc = cn14_replyReceive(sessionParm, pOutData, nOutLen, errtext);
    } // end if
  } // end if


  return nRc;
} // end cn14_cmdExecute

/*
  -----------------------------------------------------------------------------
  private function cn14_dbmChallengeResponse
  -----------------------------------------------------------------------------
*/
 tsp00_Int4 cn14_dbmChallengeResponse (
    void                        * sessionParm,
    const void                  * pInData,
    tsp00_Int4                    nInLen,
    void                        * pOutData,
    tsp00_Int4                  * nOutLen,
    tsp00_ErrTextc  VAR_ARRAY_REF errtext )
{
//return cn14_cmdExecute (sessionParm,pInData,nInLen,pOutData,nOutLen,false,errtext );

  tsp00_Int4       nRc     = DBMAPI_OK_CN14;

  // extract user an password
  const char * pStart   = (const char *) pInData;
  const char * pCommand = (const char *) pInData;

  // skip leading spaces
  while (isspace(*pCommand) && ((pCommand-pStart < nInLen))) ++pCommand;
  // skip command
  while (!isspace(*pCommand) && ((pCommand-pStart < nInLen))) ++pCommand;
  // skip trailing space
  while (isspace(*pCommand) && ((pCommand-pStart < nInLen))) ++pCommand;

  // command has missing parameters
  if (!(pCommand-pStart < nInLen)) {
    return cn14_cmdExecute (sessionParm,pInData,nInLen,pOutData,nOutLen,false,errtext );
  } // end if

  DBMSrvUsr_ChallengeResponseClient oChallengeResponse(pCommand);

  pCommand = oChallengeResponse.GiveGetChallengeCommand();
  if (pCommand == NULL) {
    // no challenge response available try original command
    return cn14_cmdExecute (sessionParm,pInData,nInLen,pOutData,nOutLen,false,errtext );
  } else {
    nRc = cn14_cmdExecute (sessionParm,pCommand,(tsp00_Int4)strlen(pCommand),pOutData,nOutLen,false,errtext );
    if (nRc != DBMAPI_OK_CN14) {
      return nRc;
    } // end if
  } // end if

  tsp00_Int4        nReplyLen;
  const void     *  pPayLoad;
  tsp00_Int4        nErrorCode;
  if (cn14analyzeDbmAnswer(sessionParm, &pPayLoad, &nReplyLen, &nErrorCode, errtext) != DBMAPI_OK_CN14) {
    if (nErrorCode == -24977) {
      // command not available try original command
      return cn14_cmdExecute (sessionParm,pInData,nInLen,pOutData,nOutLen,false,errtext );
    } else {
      return nRc;
    } // end if
  } // end if

  oChallengeResponse.SetGetChallengeAnswer((const char *)pPayLoad, nReplyLen);

  // now we have the challenge in binary format
  pCommand = oChallengeResponse.GiveResponseCommand();
  if (pCommand == NULL) {
    nRc = cn14_setErrtext(errtext, DBMAPI_USR_FALSE_CN14);
  } else {
    nRc = cn14_cmdExecute (sessionParm,pCommand,(tsp00_Int4)strlen(pCommand),pOutData,nOutLen,false,errtext);
  } // end if

  return nRc;
} // end cn14_dbmChallengeResponse

/*
  -----------------------------------------------------------------------------
  private function cn14replyRead
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14replyRead (
    void     * sessionParm,
    void     * data,
    tsp00_Int4 * len )
{
  ControlSessionT* session = (ControlSessionT*)sessionParm;
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;

  if (session == NULL) {
    nRc = DBMAPI_INVSESSION_CN14;
  } else {

    if (*len > 0) {
      if (*len > (session->packetLen - session->packetPos)) {
          *len = session->packetLen - session->packetPos;
      } /* end if */

      if (*len <= 0) {
          nRc = DBMAPI_NOMOREDATA_CN14;
      } else {
        SAPDB_memcpy (data, session->packetOutData + session->packetPos, *len);
        session->packetPos = session->packetPos + *len;
      } /* end if */
    } /* end if */
  } /* end if */

  return nRc;
} /* end cn14replyRead */

/*
  -----------------------------------------------------------------------------
  public function cn14bytesAvailable
  -----------------------------------------------------------------------------
*/
externC int cn14bytesAvailable (
    void * sessionParm)
{
    ControlSessionT* session = (ControlSessionT*)sessionParm;
    int result;

    if (session->packetInData != NULL) {
        result = session->packetSize - session->packetPos;
    } else if (session->packetOutData != NULL) {
        result = session->packetLen - session->packetPos;
    } else {
        result = 0;
    } /* end if */
    return result;
} /* end cn14bytesAvailable */

/*
  -----------------------------------------------------------------------------
  public function cn14rawReadData
  -----------------------------------------------------------------------------
 */
externC const char * cn14rawReadData (
    void     * sessionParm,
    tsp00_Int4 * rc)
{
  const char * pReturn = NULL;

  ControlSessionT* session = (ControlSessionT*)sessionParm;
  if ((session == NULL) || (session->packetOutData == NULL)) {
    *rc = DBMAPI_INVSESSION_CN14;
  } else {
    *rc = DBMAPI_OK_CN14;
    pReturn = session->packetOutData + session->packetPos;
  } /* end if */

  return pReturn;
} /* end cn14rawReadData */

/*
  -----------------------------------------------------------------------------
  private function cn14packetstats
  -----------------------------------------------------------------------------
*/
externC void WINAPI_CN14 cn14packetstats (
    void *sessionParm,
    tsp00_Int4 *packetSize,
    tsp00_Int4 *packetLen,
    tsp00_Int4 *packetPos)
{
  ControlSessionT* session = (ControlSessionT*)sessionParm;

  if (session != NULL) {
    *packetSize = session->packetSize;
    *packetLen  = session->packetLen;
    *packetPos  = session->packetPos;
  } else {
    *packetSize = 0;
    *packetLen  = 0;
    *packetPos  = 0;
  } /* end if */

} /* end cn14packetstats */

/*
  -----------------------------------------------------------------------------
  private function cn14_dbmLogon
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_dbmLogon (
    void                    * sessionParm,
    const char*               pUser,
    Tools_DynamicUTF8String & oErrtext,
    const char              * szCommand )
{
    tsp00_Name        pwClear;
    tsp00_CryptPw     pwCrypt;
    tsp00_CryptName   cryptTemp;
    char              pData[csp9_ctrl_packet_size];
    tsp00_Int4        nReplyLen;
    const char      * pPassword = NULL;
    const void      * pPayLoad  = NULL;
    tsp00_Int4        nErrorCode;
    tsp00_ErrTextc    errtext;

    errtext.Init();

    pPassword = strchr(pUser, ',');
    if (pPassword == NULL) {
        cn14_setErrtext(errtext, DBMAPI_USR_FALSE_CN14);
        oErrtext.Assign(errtext.asCharp());
        return DBMAPI_USR_FALSE_CN14;
    } // end if

    pPassword++;

    pwClear.c2p(pPassword);
    s02applencrypt(pwClear, pwCrypt);
    SAPDB_memcpy(&cryptTemp[0], &pwCrypt[0], sizeof(cryptTemp));

    sprintf( pData , "%s %.*s,%08x%08x%08x%08x%08x%08x", szCommand, pPassword-pUser-1, pUser,
                                                         cryptTemp[0], cryptTemp[1], cryptTemp[2], cryptTemp[3], cryptTemp[4], cryptTemp[5]);

    tsp00_Int4 nRc = cn14_cmdExecute(sessionParm, pData, (tsp00_Int4) strlen(pData), NULL, NULL, true, errtext);
    if (nRc != DBMAPI_OK_CN14) {
        oErrtext.Assign(errtext.asCharp());
        return nRc;
    } // end if

    if (cn14analyzeDbmAnswer(sessionParm, &pPayLoad, &nReplyLen, &nErrorCode, oErrtext) != DBMAPI_OK_CN14) {
        return DBMAPI_USR_FALSE_CN14;
    } /* end if */
  
    return DBMAPI_OK_CN14;
} /* end cn14_dbmLogon */

/*
  -----------------------------------------------------------------------------
  private function cn14_dbmVersion
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_dbmVersion
	( void                    * sessionParm,
      Tools_DynamicUTF8String & oErrtext )
{
    ControlSessionT * session = (ControlSessionT*)sessionParm;
    const void      * pPayLoad;
    tsp00_Int4        nReplyLen;
    tsp00_Int4        nErrorCode;
    tsp00_ErrTextc    errtext;
    const char *      pUnicode;
    tsp00_Int4        nRc = DBMAPI_OK_CN14;

    errtext.Init();

    nRc = cn14_cmdExecute(session, DBM_VERSION, (tsp00_Int4) strlen(DBM_VERSION), NULL, NULL, false, errtext);
    oErrtext.Assign(errtext.asCharp());
    
    if (nRc == DBMAPI_OK_CN14) {
    	nRc = cn14analyzeDbmAnswer(session, &pPayLoad, &nReplyLen, &nErrorCode, oErrtext);
        if ( nRc == DBMAPI_OK_CN14) {
            /* read infos from payload */
            pUnicode = strstr((const char*)pPayLoad, DBM_UNICODE);
            if (pUnicode != NULL) {
                pUnicode = strstr(pUnicode, DBM_ASSIGN);
                if (pUnicode != NULL) {
                    session->bUnicode = (strncmp(pUnicode, DBM_YES, strlen(DBM_YES)) == 0);
                } /* end if */
            } /* end if */
        } // end if
    } /* end if */

    return nRc;
} /* end cn14_dbmVersion */

/*
  -----------------------------------------------------------------------------
  private function cn14_cmdWrite
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_cmdWrite (
    void       * sessionParm,
    const void * data,
    tsp00_Int4     len)
{
  ControlSessionT * session = (ControlSessionT*)sessionParm;
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;

  if (session == NULL) {
    nRc = DBMAPI_INVSESSION_CN14;
  } else {
    /* init request packet */
    if (session->packetInData == NULL) {
      session->packetInData = (char*)session->packetData;
      session->packetPos = 0;
    } /* end if */
    /* add packet data */
    if (len > (session->packetSize - session->packetPos)) {
       nRc = DBMAPI_TOSMALL_CN14;
    } else {
      SAPDB_memcpy (session->packetInData + session->packetPos, data, len);
      session->packetPos = session->packetPos + len;
    } /* end if */
  } /* end if */

  return nRc;
} /* end cn14_cmdWrite */

/*
  -----------------------------------------------------------------------------
  private function cn14_request
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_request (
    void            * sessionParm,
    tsp00_ErrTextc    VAR_ARRAY_REF errtextC )
{
  ControlSessionT* session = (ControlSessionT*)sessionParm;
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;
  tsp00_ErrText     errtextP;
  tsp01_CommErr     commErr;
  //int               packetLen;
  int               alignmod;

  if ((session == NULL) || (session->packetInData == NULL)) {
    nRc = cn14_setErrtext(errtextC, DBMAPI_INVSESSION_CN14);
  } else {

    alignmod = session->packetPos % 8;
    if (alignmod != 0) {
      memset (session->packetInData + session->packetPos, _T(' '), 8 - alignmod);
      session->packetPos += 8 - alignmod;
    } /* end if */

    sqlarequest (session->reference,
                 session->packetInData,
                 session->packetPos,
                 errtextP,
                 commErr);
    session->packetInData = NULL;
    session->packetPos = 0;

    if (commErr != commErrOk_esp01) {
      cn14_errtextToC (errtextC, errtextP);
      nRc = DBMAPI_COMMERR_CN14;
    } /* end if */

  } /* end if */

  return nRc;
} /* end cn14_request */

/*
  -----------------------------------------------------------------------------
  private function cn14_receive
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_receive (
    void            * sessionParm,
    tsp00_ErrTextc    VAR_ARRAY_REF errtextC )
{
  ControlSessionT*  session = (ControlSessionT*)sessionParm;
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;
  tsp01_CommErr     commErr;
  tsp00_ErrText     errtextP;

  if ((session == NULL) || (session->packetInData != NULL)) {
    nRc = cn14_setErrtext(errtextC, DBMAPI_INVSESSION_CN14);
  } else {

    sqlareceive (session->reference,
                 (void **) &session->packetOutData,
                 session->packetLen,
                 errtextP,
                 commErr);

    if (commErr != commErrOk_esp01) {
      cn14_errtextToC (errtextC, errtextP);
      nRc = DBMAPI_COMMERR_CN14;
    } /* end if */

  } /* end if */

  return nRc;
} /* end cn14_receive */

/*
  -----------------------------------------------------------------------------
  private function cn14_errtextToC
  -----------------------------------------------------------------------------
*/
void cn14_errtextToC (
    tsp00_ErrTextc VAR_ARRAY_REF errtextC,
    tsp00_ErrText  VAR_ARRAY_REF errtextP )
{
  errtextC.p2c(errtextP);
} /* end cn14_errtextToC */

/*
  -----------------------------------------------------------------------------
  private function cn14_setErrtext
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14_setErrtext (
    tsp00_ErrTextc & errtextC,
    tsp00_Int4       nRc )
{
  switch (nRc) {
    case DBMAPI_OK_CN14:
      errtextC.Init();
      break;
    case DBMAPI_USR_UNKNOWN_CN14:
      errtextC.rawAssign("user unknown");
      break;
    case DBMAPI_USR_FALSE_CN14:
      errtextC.rawAssign("wrong user/password");
      break;
    case DBMAPI_MEMORY_CN14:
      errtextC.rawAssign("out of memory");
      break;
    case DBMAPI_COMMERR_CN14:
      errtextC.rawAssign("communication error");
      break;
    case DBMAPI_TOSMALL_CN14:
      errtextC.rawAssign("packet too small");
      break;
    case DBMAPI_INVSESSION_CN14:
      errtextC.rawAssign("invalid session data");
      break;
    case DBMAPI_NOMOREDATA_CN14:
      errtextC.rawAssign("no more data available");
      break;
    case DBMAPI_XUSERKEY_CN14:
      errtextC.rawAssign("generated user key too long");
      break;
    case DBMAPI_NOT_OK_CN14:
      errtextC.rawAssign("some error occured");
      break;
    case DBMAPI_R3TP_CN14:
      errtextC.rawAssign("tp error: ");
      break;
    case DBMAPI_USC2_CN14:
      errtextC.rawAssign("UCS2 conversion error");
      break;
    case DBMAPI_URI_CN14:
      errtextC.rawAssign("URI calculation error");
      break;
    case DBMAPI_NULLPARAM_CN14:
      errtextC.rawAssign("input parameter with value NULL passed");
      break;
    default:
      errtextC.rawAssign("unknown error code");
      break;
  } /* end switch */

  return nRc;
} /* end cn14_setErrtext */

/*
  -----------------------------------------------------------------------------
  private function cn14_senduser
  -----------------------------------------------------------------------------
 */
tsp00_Int4 cn14_senduser (
    void                    * pSession,
    const char*               szServerNode,
    const char*               szDBName,
    Tools_DynamicUTF8String & oErrtext )
{
  tsp00_Int4          nRc     = DBMAPI_OK_CN14;
  tsp00_ErrText       szErrP;
  tsp4_xuser_record   recUser;
  char              * pUserkey;
  tsp00_Bool          bOk;
  tsp00_CryptName     cryptTemp;

  if ((strlen(szServerNode) + strlen(szDBName) + strlen(XU_DBM_MARK)) <= sizeof(recUser.xu_key)) {
    // prepare userkey
    recUser.xu_key.SetBlank();
    pUserkey = (char *) &recUser.xu_key[0];
    strncpy(pUserkey, XU_DBM_MARK, strlen(XU_DBM_MARK));
    pUserkey = pUserkey + strlen(XU_DBM_MARK);
    strncpy(pUserkey, szDBName, strlen(szDBName));
    pUserkey = pUserkey + strlen(szDBName);
    strncpy(pUserkey, szServerNode, strlen(szServerNode));

    // get user
    sqlgetuser (&recUser, NULL, szErrP, &bOk);
    recUser.xu_userUCS2.SetZero();

    if (bOk) {
      SAPDB_memcpy(cryptTemp, &recUser.xu_password, sizeof(cryptTemp));
      int nIndex = sizeof(recUser.xu_user) - 1;
      while (nIndex >= 0 && recUser.xu_user[nIndex] == ' ') {
        recUser.xu_user[nIndex] = '\0';
        nIndex--;
      }  // end while

      // create command
      char  pData[csp9_ctrl_packet_size];
      sprintf ( pData , "%s %s,%08x%08x%08x%08x%08x%08x", "user_logon",
                                                          recUser.xu_user.asCharp(),
                                                          cryptTemp[0],
                                                          cryptTemp[1],
                                                          cryptTemp[2],
                                                          cryptTemp[3],
                                                          cryptTemp[4],
                                                          cryptTemp[5]);

      tsp00_ErrTextc      errtext;
      errtext.Init();
      nRc = cn14_cmdExecute(pSession, pData,  (tsp00_Int4) strlen(pData), NULL, NULL, true, errtext);

      if (nRc == DBMAPI_OK_CN14) {
        tsp00_Int4          nReplyLen;
        const void        * pPayLoad = NULL;
        tsp00_Int4          nErrorCode;
        if (cn14analyzeDbmAnswer(pSession, &pPayLoad, &nReplyLen, &nErrorCode, oErrtext ) != DBMAPI_OK_CN14) {
            nRc = DBMAPI_USR_FALSE_CN14;
        } // end if
      } else {
          oErrtext.Assign(errtext.asCharp());
      } // end if
    } // end if
  } // end if

  return nRc;

} /* end cn14_senduser */

/*
  -----------------------------------------------------------------------------
  public function cn14saveUser
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14saveUser (
    const char*           szServerNode,
    const char*           szDBName,
    const _TCHAR *        szUser)
{
  if( szServerNode == NULL || szDBName == NULL || szUser == NULL) 
    return DBMAPI_NULLPARAM_CN14;
    
  tsp00_C18c szKey;
  int        nReturn = DBMAPI_USR_FALSE_CN14;

  if ((_tcslen(szServerNode) + _tcslen(szDBName) + _tcslen(XU_DBM_MARK)) <= (size_t)(szKey.size() - 1)) {
  
    /* prepare userkey */
    sprintf(szKey, "%s%s%s", XU_DBM_MARK, szDBName, szServerNode);

    nReturn = cn14saveUserByKey(szKey, szServerNode, szDBName, szUser, NULL);

  } /* end if */

  return nReturn;
} /* end cn14saveUser */

/*
  -----------------------------------------------------------------------------
  public function cn14saveUserByKey
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14saveUserByKey (
    const _TCHAR *        szKey,
    const char*           szServerNode,
    const char*           szDBName,
    const _TCHAR *        szUser,
    const _TCHAR *        szParams)
{
  if( szServerNode == NULL || szDBName == NULL) 
    return DBMAPI_NULLPARAM_CN14;
    
  int                 nReturn = DBMAPI_OK_CN14;
  tsp4_xuser_record   recUser;
  _TCHAR            * pUserkey;
  int                 nIndex;
  _TCHAR              szNumber1[2];
  _TCHAR              szNumber2[2];
  _TCHAR            * pEnd;
  tsp00_ErrText         szError;
  tsp00_Bool          bOk;
  const _TCHAR      * pPassword;
  int                 nUsrLen;
  tsp00_Name            szPassword;

  /* userrecord */
  memset(&recUser, _T('\0'), sizeof(recUser));

  /* search for password */
  pPassword = _tcschr (szUser, _T(','));
  if (pPassword != NULL) {
    nUsrLen = pPassword - szUser;
    ++pPassword;
  } else {
    nUsrLen = (int) strlen(szUser);
    pPassword = _T("");
  } /* end if */

  if (_tcslen(szKey) <= sizeof(recUser.xu_key)) {
  
    /* prepare userkey */
    memset(&recUser.xu_key[0], _T(' '), sizeof(recUser.xu_key));
    pUserkey = (char *) &recUser.xu_key[0];
    _tcsncpy(pUserkey, szKey, _tcslen(szKey));

    sqlxuopenuser (NULL, szError, &bOk);
    if (bOk) {

      /* get user */
      sqlgetuser (&recUser, NULL, szError, &bOk);
      memset(recUser.xu_userUCS2, 0, sizeof(recUser.xu_userUCS2));
      bOk = true;

      /* prepare password for save */
      memset(&recUser.xu_password, _T('\0'), sizeof(recUser.xu_password));
      if (_tcslen(pPassword) == (sizeof(recUser.xu_password) * 2)) {
        szNumber1[1] = _T('\0');
        szNumber2[1] = _T('\0');
        for (nIndex = 0; nIndex < sizeof(recUser.xu_password); nIndex++) {
          szNumber1[0] = pPassword[(2 * nIndex)];
          szNumber2[0] = pPassword[(2 * nIndex) + 1];
          recUser.xu_password[nIndex] = (teo00_Byte)(strtoul(szNumber1, &pEnd, XU_CRYPT_BASE) * XU_CRYPT_BASE + 
                                        strtoul(szNumber2, &pEnd, XU_CRYPT_BASE));
        } /* end for */
      } else {
        memset(&szPassword[0], _T(' '), sizeof(szPassword));
        _tcsncpy((_TCHAR *) szPassword, pPassword, _tcslen(pPassword));
        s02applencrypt(szPassword, recUser.xu_password);
      } /* end if */

      /* prepare user for save */
      memset(&recUser.xu_user[0], _T(' '), sizeof(recUser.xu_user));
      _tcsncpy((_TCHAR *) recUser.xu_user, szUser, nUsrLen);

      memset(&recUser.xu_servernode[0], _T(' '), sizeof(recUser.xu_servernode));
      _tcsncpy((_TCHAR *) recUser.xu_servernode, szServerNode, _tcslen(szServerNode));

      memset(&recUser.xu_serverdb[0], _T(' '), sizeof(recUser.xu_serverdb));
      _tcsncpy((_TCHAR *) recUser.xu_serverdb, szDBName, _tcslen(szDBName));

      if (szParams != NULL) {
        const _TCHAR * pParam = NULL;
        const _TCHAR * pValue = NULL;
        const _TCHAR * pEnd   = NULL;

        pParam = strstr(szParams, "CACHELIMIT");
        if (pParam != NULL) {
          pValue = strchr(pParam, '=');
          if (pValue != NULL) {
            ++pValue;
            pEnd = strchr(pValue, ';');
            if (pEnd != NULL) {
              recUser.xu_cachelimit = (pEnd == pValue) ? -1 : atoi(pValue);
            } /* end if */
          } /* end if */
        } /* end if */
        pParam = strstr(szParams, "SQLMODE");
        if (pParam != NULL) {
          pValue = strchr(pParam, '=');
          if (pValue != NULL) {
            ++pValue;
            pEnd = strchr(pValue, ';');
            if (pEnd != NULL) {
              memset(&recUser.xu_sqlmode[0], _T(' '), sizeof(recUser.xu_sqlmode));
              _tcsncpy((_TCHAR *) recUser.xu_sqlmode, pValue, pEnd - pValue);
              if (!(_tcsncmp((_TCHAR *) recUser.xu_sqlmode, "INTERNAL", 8) == 0 ||
                    _tcsncmp((_TCHAR *) recUser.xu_sqlmode, "ANSI    ", 8) == 0 ||
                    _tcsncmp((_TCHAR *) recUser.xu_sqlmode, "DB2     ", 8) == 0 ||
                    _tcsncmp((_TCHAR *) recUser.xu_sqlmode, "ORACLE  ", 8) == 0 ||
                    _tcsncmp((_TCHAR *) recUser.xu_sqlmode, "SAPR3   ", 8) == 0    )) {
                bOk = false;
              } /* end if */
            } /* end if */
          } /* end if */
        } /* end if */
        pParam = strstr(szParams, "TIMEOUT");
        if (pParam != NULL) {
          pValue = strchr(pParam, '=');
          if (pValue != NULL) {
            ++pValue;
            pEnd = strchr(pValue, ';');
            if (pEnd != NULL) {
              recUser.xu_timeout = (pEnd == pValue) ? -1 : atoi(pValue);
            } /* end if */
          } /* end if */
        } /* end if */
        pParam = strstr(szParams, "ISOLATION");
        if (pParam != NULL) {
          pValue = strchr(pParam, '=');
          if (pValue != NULL) {
            ++pValue;
            pEnd = strchr(pValue, ';');
            if (pEnd != NULL) {
              recUser.xu_isolation = (pEnd == pValue) ? -1 : atoi(pValue);
              switch (recUser.xu_isolation) {
                case 0: 
                case 1:
                case 2:
                case 3:
                case 10:
                case 15:
                case 20:
                case 30:
                  break;
                default:
                  bOk = false;
                  break;
              } /* end switch */
            } /* end if */
          } /* end if */
        } /* end if */
        pParam = strstr(szParams, "DBLOCALE");
        if (pParam != NULL) {
          pValue = strchr(pParam, '=');
          if (pValue != NULL) {
            ++pValue;
            pEnd = strchr(pValue, ';');
            if (pEnd != NULL) {
              memset(&recUser.xu_dblang[0], _T(' '), sizeof(recUser.xu_dblang));
              _tcsncpy((_TCHAR *) recUser.xu_dblang, pValue, pEnd - pValue);
            } /* end if */
          } /* end if */
        } /* end if */
      } /* end if */

      if (bOk) {
        sqlputuser (&recUser, NULL, szError, &bOk);
      } /* end if */
      if (!bOk) {
        nReturn = DBMAPI_XUSER_CN14;
      } /* end if */
      sqlxucloseuser (NULL, szError, &bOk);
    } else {
      nReturn = DBMAPI_XUSER_CN14;
    } /* end if */
  } else {
    nReturn = DBMAPI_XUSERKEY_CN14;
  } /* end if */


  return nReturn;
} /* end cn14saveUserByKey */

/*
  -----------------------------------------------------------------------------
  public function cn14checkUser
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14checkUser (
    const char* szServerNode,
    const char* szDBName,
    const _TCHAR *        szUser)
{
  if( szServerNode == NULL || szDBName == NULL) 
    return DBMAPI_NULLPARAM_CN14;
    
  tsp00_C18c szKey;
  int        nReturn = DBMAPI_USR_FALSE_CN14;

  if ((_tcslen(szServerNode) + _tcslen(szDBName) + _tcslen(XU_DBM_MARK)) <= (size_t)(szKey.size() - 1)) {
  
    /* prepare userkey */
    sprintf(szKey, "%s%s%s", XU_DBM_MARK, szDBName, szServerNode);

    nReturn = cn14checkUserByKey(szKey, szUser);

  } /* end if */

  return nReturn;
} /* end cn14checkUser */

/*
  -----------------------------------------------------------------------------
  public function cn14checkUserByKey
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14checkUserByKey (
    const _TCHAR *        szKey,
    const _TCHAR *        szUser)
{
  int                 nReturn = DBMAPI_USR_FALSE_CN14;
  tsp4_xuser_record   recUserEnv;
  tsp4_xuser_record   recUserIn;
  _TCHAR            * pUserkey;
  int                 nIndex;
  _TCHAR              szNumber1[2];
  _TCHAR              szNumber2[2];
  _TCHAR            * pEnd;
  tsp00_ErrText         szError;
  tsp00_Bool          bOk;
  const _TCHAR      * pPassword;
  int                 nUsrLen;
  tsp00_Name            szPassword;

  /* search for password */
  pPassword = _tcschr (szUser, _T(','));
  if (pPassword != NULL) {
    nUsrLen = pPassword - szUser;
    ++pPassword;
  } else {
    nUsrLen = (int) strlen (szUser);
    pPassword = _T("");
  } /* end if */

  /* prepare password for check */
  memset(&recUserIn.xu_password[0], _T('\0'), sizeof(recUserIn.xu_password));
  if (_tcslen(pPassword) == (sizeof(recUserIn.xu_password) * 2)) {
    szNumber1[1] = _T('\0');
    szNumber2[1] = _T('\0');
    for (nIndex = 0; nIndex < sizeof(recUserIn.xu_password); nIndex++) {
      szNumber1[0] = pPassword[(2 * nIndex)];
      szNumber2[0] = pPassword[(2 * nIndex) + 1];
      recUserIn.xu_password[nIndex] = (teo00_Byte)(strtoul(szNumber1, &pEnd, XU_CRYPT_BASE) * XU_CRYPT_BASE + 
                                      strtoul(szNumber2, &pEnd, XU_CRYPT_BASE));
    } /* end for */
  } else {
    memset(&szPassword[0], _T(' '), sizeof(szPassword));
    _tcsncpy((_TCHAR *) szPassword, pPassword, _tcslen(pPassword));
    s02applencrypt(szPassword, recUserIn.xu_password);
  } /* end if */

  /* prepare user for check */
  memset(&recUserIn.xu_user[0], _T(' '), sizeof(recUserIn.xu_user));
  _tcsncpy((_TCHAR *) recUserIn.xu_user, szUser, nUsrLen);

  if (_tcslen(szKey) <= sizeof(recUserEnv.xu_key)) {
  
    /* prepare userkey */
    memset(&recUserEnv.xu_key[0], _T(' '), sizeof(recUserEnv.xu_key));
    pUserkey = (char *) &recUserEnv.xu_key[0];
    _tcsncpy(pUserkey, szKey, _tcslen(szKey));

    /* get user */
    sqlgetuser (&recUserEnv, NULL, szError, &bOk);
    memset(recUserEnv.xu_userUCS2, 0, sizeof(recUserEnv.xu_userUCS2));

    if (bOk) {
      if (memcmp(&recUserIn.xu_user[0],     &recUserEnv.xu_user[0],     sizeof(recUserIn.xu_user)     ) == 0 && 
          memcmp(&recUserIn.xu_password[0], &recUserEnv.xu_password[0], sizeof(recUserIn.xu_password) ) == 0    ) {
        nReturn = DBMAPI_OK_CN14;
      } /* end if */

    } else {
      nReturn = DBMAPI_USR_UNKNOWN_CN14;
    } /* end if */
    
  } else {
    nReturn = DBMAPI_USR_UNKNOWN_CN14;
  } /* end if */

  return nReturn;
} /* end cn14checkUserByKey */

/*
  -----------------------------------------------------------------------------
  public function cn14checkDefaultUser
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14checkDefaultUser (
    const _TCHAR *        szUser)
{
  int                 nReturn = DBMAPI_USR_FALSE_CN14;
  tsp4_xuser_record   recUserEnv;
  tsp4_xuser_record   recUserIn;
  _TCHAR            * pUserkey;
  int                 nIndex;
  _TCHAR              szNumber1[2];
  _TCHAR              szNumber2[2];
  _TCHAR            * pEnd;
  tsp00_ErrText         szError;
  tsp00_Bool          bOk;
  const _TCHAR      * pPassword;
  int                 nUsrLen;
  tsp00_Name            szPassword;

  /* search for password */
  pPassword = _tcschr (szUser, _T(','));
  if (pPassword != NULL) {
    nUsrLen = pPassword - szUser;
    ++pPassword;
  } else {
    nUsrLen = (int) strlen (szUser);
    pPassword = _T("");
  } /* end if */

  /* prepare password for check */
  memset(&recUserIn.xu_password[0], _T('\0'), sizeof(recUserIn.xu_password));
  if (_tcslen(pPassword) == (sizeof(recUserIn.xu_password) * 2)) {
    szNumber1[1] = _T('\0');
    szNumber2[1] = _T('\0');
    for (nIndex = 0; nIndex < sizeof(recUserIn.xu_password); nIndex++) {
      szNumber1[0] = pPassword[(2 * nIndex)];
      szNumber2[0] = pPassword[(2 * nIndex) + 1];
      recUserIn.xu_password[nIndex] = (teo00_Byte)(strtoul(szNumber1, &pEnd, XU_CRYPT_BASE) * XU_CRYPT_BASE +
                                      strtoul(szNumber2, &pEnd, XU_CRYPT_BASE));
    } /* end for */
  } else {
    memset(&szPassword[0], _T(' '), sizeof(szPassword));
    _tcsncpy((_TCHAR *) szPassword, pPassword, _tcslen(pPassword));
    s02applencrypt(szPassword, recUserIn.xu_password);
  } /* end if */

  /* prepare user for check */
  memset(&recUserIn.xu_user[0], _T(' '), sizeof(recUserIn.xu_user));
  _tcsncpy((_TCHAR *) recUserIn.xu_user, szUser, nUsrLen);

  /* prepare userkey */
  memset(&recUserEnv.xu_key[0], _T(' '), sizeof(recUserEnv.xu_key));
  pUserkey = (char *) &recUserEnv.xu_key[0];
  _tcsncpy(pUserkey, XU_KEY_DEFAULT, _tcslen(XU_KEY_DEFAULT));

  /* open XUSer-File */
  sqlxuopenuser (NULL, szError, &bOk);

  if (bOk) {

    /* get user */
    sqlgetuser (&recUserEnv, NULL, szError, &bOk);
    memset(recUserEnv.xu_userUCS2, 0, sizeof(recUserEnv.xu_userUCS2));

    if (bOk) {
      if ( memcmp(&recUserIn.xu_user[0],     &recUserEnv.xu_user[0],     sizeof(recUserIn.xu_user)     ) == 0 && 
           memcmp(&recUserIn.xu_password[0], &recUserEnv.xu_password[0], sizeof(recUserIn.xu_password) ) == 0     ) {
        nReturn = DBMAPI_OK_CN14;
      } /* end if */
    } else {
      /* default unknown, then save also */
      /* write username to structure */
      memset(&recUserEnv.xu_user[0], _T(' '), sizeof(recUserIn.xu_user));
      _tcsncpy((_TCHAR *) &recUserEnv.xu_user[0],    XU_SAP_USER, _tcslen(XU_SAP_USER));
      /* write pwd to structure */
      memset(&szPassword[0], _T(' '), sizeof(szPassword));
      _tcsncpy((_TCHAR *) szPassword, XU_SAP_PWD, _tcslen(XU_SAP_PWD));
      s02applencrypt(szPassword, recUserEnv.xu_password);
      /* save */
      sqlputuser (&recUserEnv, NULL, szError, &bOk);
      if (bOk) {
        nReturn = DBMAPI_OK_CN14;
      } /* end if */
    } /* end if */

     /* close xuser-file */
    sqlxucloseuser (NULL, szError, &bOk);

  } /* end if */

  return nReturn;
} /* end cn14checkDefaultUser */

/*
  -----------------------------------------------------------------------------
  public function cn14deleteUser
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14deleteUser (
    const char*           szServerNode,
    const char*           szDBName)
{
  if( szServerNode == NULL || szDBName == NULL) 
    return DBMAPI_NULLPARAM_CN14;
    
  tsp00_C18c szKey;
  int        nReturn = DBMAPI_USR_FALSE_CN14;

  if ((_tcslen(szServerNode) + _tcslen(szDBName) + _tcslen(XU_DBM_MARK)) <= (size_t)(szKey.size() - 1)) {
  
    /* prepare userkey */
    sprintf(szKey, "%s%s%s", XU_DBM_MARK, szDBName, szServerNode);

    nReturn = cn14deleteUserByKey(szKey);

  } /* end if */

  return nReturn;
} /* end cn14deleteUser */

/*
  -----------------------------------------------------------------------------
  public function cn14deleteUserByKey
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14deleteUserByKey (
    const _TCHAR *        szKey)
{
  int                 nReturn = DBMAPI_OK_CN14;
  tsp4_xuser_record   recUsers[XU_MAX_USERS];
  tsp4_xuser_record   recUser;
  _TCHAR            * pUserkey;
  int                 nLoadIndex;
  int                 nSaveIndex;


  //char                szNumber1[2];
  //char                szNumber2[2];
  //char              * pEnd;
  tsp00_ErrText         szError;
  tsp00_Bool          bOk;
  //const char        * pPassword;
  //int                 nUsrLen;
  //tsp00_Name            szPassword;
  bool                bFound = false;

  /* userrecord */
  memset(&recUser, 0, sizeof(recUser));

  if (_tcslen(szKey) <= sizeof(recUser.xu_key)) {
  
    /* prepare userkey */
    memset(&recUser.xu_key[0], _T(' '), sizeof(recUser.xu_key));
    pUserkey = (char *) &recUser.xu_key[0];
    _tcsncpy(pUserkey, szKey, _tcslen(szKey));

    sqlxuopenuser (NULL, szError, &bOk);

    /* read */
    nLoadIndex = 0;
    while (bOk && (nLoadIndex < XU_MAX_USERS)) {
      sqlindexuser(nLoadIndex + 1, &recUsers[nLoadIndex], NULL, szError, &bOk);
      nLoadIndex++;
    } /* end while */

    /* clear the file */
    sqlclearuser(NULL);

    /* save */
    nLoadIndex = (bOk) ? nLoadIndex : nLoadIndex - 1;
    nSaveIndex = 0;
    bOk = true;
    while (bOk && (nSaveIndex < nLoadIndex)) {
      if (memcmp(&recUser.xu_key, &recUsers[nSaveIndex].xu_key, sizeof(recUser.xu_key)     ) != 0) {
        sqlputuser (&recUsers[nSaveIndex], NULL, szError, &bOk);
      } else {
        bFound = true;
      } /* end if */
      nSaveIndex++;
    } /* end while */

    sqlxucloseuser (NULL, szError, &bOk);

    if (!bOk) {
      nReturn = DBMAPI_XUSER_CN14;
    } else if (!bFound) {
      nReturn = DBMAPI_USR_UNKNOWN_CN14;
    } /* end if */

  } else {
    nReturn = DBMAPI_XUSERKEY_CN14;
  } /* end if */

  return nReturn;
} /* end cn14deleteUserByKey */

/*
  -----------------------------------------------------------------------------
  public function cn14listUsers
  -----------------------------------------------------------------------------
 */
externC long cn14listUsers (
    _TCHAR                * pBuffer,
    long                    nBuffer)
{
  tsp4_xuser_record   recUsers[XU_MAX_USERS];
  //_TCHAR            * pUserkey;
  int                 nLoadIndex;
  int                 nIndex;
  tsp00_ErrText         szError;
  tsp00_Bool          bOk;
  long                nUsed = 0;
  _TCHAR            * pCurrent = pBuffer;

  sqlxuopenuser (NULL, szError, &bOk);

  /* read */
  nLoadIndex = 0;
  while (bOk && (nLoadIndex < XU_MAX_USERS)) {
    sqlindexuser(nLoadIndex + 1, &recUsers[nLoadIndex], NULL, szError, &bOk);
    nLoadIndex++;
  } /* end while */
  nLoadIndex = (bOk) ? nLoadIndex : nLoadIndex - 1;

  /* clear the file */
  sqlxucloseuser (NULL, szError, &bOk);

  if (nLoadIndex > 0) {
    for (nIndex = 0; nIndex < nLoadIndex; ++nIndex) {
      if ((pCurrent + sizeof(recUsers[nIndex].xu_key) + /*sizeof(recUsers[nIndex].xu_user)*/ 18 + 2) <
          (pBuffer + nBuffer)  ) {
        _stprintf(pCurrent, _T("%.*s %.*s\n"),
                            sizeof(recUsers[nIndex].xu_key),
                            (_TCHAR *) &recUsers[nIndex].xu_key[0],
                            /* sizeof(recUsers[nIndex].xu_user)*/ 18,

                            (_TCHAR *) &recUsers[nIndex].xu_user[0]);
        pCurrent = pCurrent + _tcslen(pCurrent);
      } /* end if */


    } /* end for */
  } /* end if */

  return nLoadIndex;
} /* end cn14listUsers */

/*
  -----------------------------------------------------------------------------
  public function cn14GetTPUser
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14GetTPUser (
    const char              * pMode,
    const char*               sysid,
    const char*               conn,
    const char*               profile,
    tsp00_C64c                VAR_ARRAY_REF user,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext )
{
  if( sysid == NULL || conn == NULL || profile == NULL) 
    return DBMAPI_NULLPARAM_CN14;
    
  tsp00_Int4         nRc   = DBMAPI_OK_CN14;
  char             szCommand[BUFFER_LEN];
  char             szUserLine[BUFFER_LEN] = "";
  char             szLine[BUFFER_LEN] = "";
  FILE           * hFile = NULL;
  //tsp00_Int4       nExitCode;
  bool             bOk   = false;
#ifdef _WIN32
  char             szDummy[BUFFER_LEN];
#else
  int              tmpFd;
  mode_t           savedUmask;
#endif
  char         *   szTmpName   = NULL;
  char         *   szTmpDir    = NULL;
  char             szTmpFile[BUFFER_LEN] = "";
  
/*  FILE           * hTrace = fopen("dbmrfc1.log", "w"); */

#ifdef _WIN32
  szTmpName = tmpnam(szDummy);
  if (szTmpName == NULL) {
    SAPDB_strcpy(szUserLine, "can not create temporary file");
  } else {

    if ((szTmpDir = getenv("TMP")) == NULL) {
      sprintf(szTmpFile, "%s%s%s", ".", szTmpName, "cst");
    } else {
      char  szShortTmp[BUFFER_LEN] = "";
      GetShortPathName(szTmpDir, szShortTmp, BUFFER_LEN);
      sprintf(szTmpFile, "%s%s.%s", szShortTmp,  szTmpName, "cst");
    } /* end if */
#else
  /* this is not better than before if you think about , but uses mkstemp... */
  SAPDB_strcpy(&szTmpFile[0], "dbmtmp.XXXXXX");
  savedUmask = umask(066);
  tmpFd = mkstemp( &szTmpFile[0] ); /* tmpfile() does not work in use with other processes */
  umask(savedUmask);
  if ( tmpFd < 0 )
  {
    SAPDB_strcpy(szUserLine, "can not create temporary file");
  }
  else
  {
#endif
/*    if (hTrace != NULL) fprintf(hTrace, "tmpfile is %s (%s %d)\n", szTmpFile, __FILE__, __LINE__); */

#ifdef _WIN32
    if (getenv("SAPEXE") != NULL) {
      sprintf(szCommand, "set PATH=%%SAPEXE%%;%%PATH%% && tp pf=%s getlcconnectinfo %s lc-name=%s user-type=%s 1>%s 2>&1", profile, sysid, conn, pMode, szTmpFile);
    } else {
      sprintf(szCommand, "tp pf=%s getlcconnectinfo %s lc-name=%s user-type=%s 1>%s 2>&1", profile, sysid, conn, pMode, szTmpFile);
    } // end if
#else
    if (getenv("DIR_LIBRARY") != NULL) {
      sprintf(szCommand, LDLIBPATH_IP00"=$DIR_LIBRARY:$"LDLIBPATH_IP00" ; tp pf=%s getlcconnectinfo %s lc-name=%s user-type=%s 1>%s 2>&1", profile, sysid, conn, pMode, szTmpFile);
    } else {
      sprintf(szCommand, "tp pf=%s getlcconnectinfo %s lc-name=%s user-type=%s 1>%s 2>&1", profile, sysid, conn, pMode, szTmpFile);
    } // end if
#endif
/*    if (hTrace != NULL) fprintf(hTrace, "command is %s (%s %d)\n", szCommand, __FILE__, __LINE__); */

    if (system(szCommand) != -1) {
#ifdef _WIN32
      hFile = fopen(szTmpFile, "r");
#else
      hFile = fdopen(tmpFd, "r");
#endif
      if (hFile != NULL) {
        while (fgets ( szLine, BUFFER_LEN - 1, hFile ) != NULL) {
/*          if (hTrace != NULL) fprintf(hTrace, "szLine is %s (%s %d)\n", szLine, __FILE__, __LINE__); */
          SAPDB_strcpy(szUserLine, szLine);
          if ((strncmp(szUserLine, "LC-INFO=", strlen("LC-INFO=")) == 0)) {
            bOk = true;
            break; /* while */
          } /* end if */
        } /* end while */

        fclose(hFile);
        remove(szTmpFile);
      } else {
/*        if (hTrace != NULL) fprintf(hTrace, "errno is %d (%s %d)\n", errno, __FILE__, __LINE__); */
        SAPDB_strcpy(szUserLine, "can not open temporary file");
      } /* end if */
    } else {
#ifndef _WIN32
      close(tmpFd);
#endif
      SAPDB_strcpy(szUserLine, "can not execute tp");
    } /* end if */
     remove(szTmpFile);
  } /* end if */

  if (bOk) {
    _TCHAR * pUser;
    pUser = strchr(szUserLine, '=');
    if (pUser != NULL) {
      ++pUser;
      /* delete newline */
      pUser[_tcslen ( pUser ) - 1] = 0;
      user.rawAssign(pUser);
    } /* end if */

  } else {
    nRc = cn14_setErrtext(errtext, DBMAPI_R3TP_CN14);
    if (strlen(szUserLine) > 0) {
      szUserLine[errtext.size()  - errtext.length()] = 0;
      strcat(errtext, szUserLine);
    } else {
      szLine[errtext.size()  - errtext.length()] = 0;
      strcat(errtext, szLine);
    } /* end if */
  } /* end if */

/*  if (hTrace != NULL) fclose(hTrace); */

  return nRc;
} /* end cn14GetTPUser */

/*
  -----------------------------------------------------------------------------
  public function cn14analyzeDbmAnswer
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14analyzeDbmAnswer (
    void                       * sessionParm,
    const void                ** pPayLoad,
    tsp00_Int4                 * nPayLoad,
    tsp00_Int4                 * pErrorCode,
    tsp00_ErrTextc VAR_ARRAY_REF errtext )
{
    Tools_DynamicUTF8String oErrtext;
    tsp00_Int4 nRc = cn14analyzeDbmAnswer (sessionParm, pPayLoad, nPayLoad, pErrorCode, oErrtext);
    if (errtext != NULL) {
        cn14_CopyError(oErrtext, errtext);
    } // end if
    return nRc;
} // end cn14analyzeDbmAnswer
/*-----------------------------------------------------------------------------*/
tsp00_Int4 cn14analyzeDbmAnswer (
    void                       * sessionParm,
    const void                ** pPayLoad,
    tsp00_Int4                 * nPayLoad,
    tsp00_Int4                 * pErrorCode,
    Tools_DynamicUTF8String    & oErrtext )
{
  tsp00_Int4          nRc            = DBMAPI_OK_CN14;
  ControlSessionT * session        = (ControlSessionT*)sessionParm;
  const _TCHAR    * pCharData      = NULL;
  const _TCHAR    * pCharPayload   = NULL;
  const _TCHAR    * pCharError     = NULL;
  int               nCopy          = 0;

  if ((session == NULL) || (session->packetOutData == NULL)) {
    nRc = DBMAPI_INVSESSION_CN14;
  } else {
    *nPayLoad = session->packetLen;
    pCharData = (const _TCHAR *) session->packetOutData;
    nRc = cn14analyzeDbmData(pCharData, *nPayLoad, pPayLoad, nPayLoad, pErrorCode, oErrtext);
  } /* end if */

  return nRc;
} /* end cn14_analyzeDbmAnswer */

/*
  -----------------------------------------------------------------------------
  public function cn14analyzeDbmData
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14analyzeDbmData (
    const void                 * pData,
    tsp00_Int4                   nDataLen,
    const void                ** pPayLoad,
    tsp00_Int4                 * nPayLoad,
    tsp00_Int4                 * pErrorCode,
    tsp00_ErrTextc VAR_ARRAY_REF errtext )
{
    Tools_DynamicUTF8String oErrtext;
    tsp00_Int4 nRc = cn14analyzeDbmData (pData, nDataLen, pPayLoad, nPayLoad, pErrorCode, oErrtext);
    if (errtext != NULL) {
        cn14_CopyError(oErrtext, errtext);
    } // end if
    return nRc;
} // end cn14analyzeDbmData
/*-----------------------------------------------------------------------------*/
tsp00_Int4 cn14analyzeDbmData (
    const void                 * pData,
    tsp00_Int4                   nDataLen,
    const void                ** pPayLoad,
    tsp00_Int4                 * nPayLoad,
    tsp00_Int4                 * pErrorCode,
    Tools_DynamicUTF8String    & oErrtext )
{
    tsp00_Int4   nRc            = DBMAPI_OK_CN14;
    const char * pCharData      = NULL;
    const char * pCurrent       = NULL;

    *nPayLoad = nDataLen;
    pCharData = (const char*)pData;

    if (strncmp(pCharData, CTRL_ERR, strlen(CTRL_ERR)) == 0) {
        // we have a error
        nRc = DBMAPI_NOT_OK_CN14;
        pCurrent = strchr (pCharData, '\n');

        // here is the beginning of the error
        if (pCurrent != NULL) {
            pCurrent++;
            // assign it the error text
            oErrtext.Assign((Tools_DynamicUTF8String::ConstPointer) pCurrent, *nPayLoad - (pCurrent - pCharData));

            // store the errornumber
            *pErrorCode = atol(pCurrent);

            // find the payload
            pCurrent = strchr (pCurrent, '\n');
            if (pCurrent != NULL) {
                pCurrent++;
            } /* end if */

        } else {
            // no error information found
            *pErrorCode = 0;
        } /* end if */

    } else {
        // payload of successfull calll
        pCurrent = strchr (pCharData, '\n');
        if (pCurrent != NULL) {
            pCurrent++;
        } /* end if */
    } /* end if */

    // compute length of payload
    if (pCurrent != NULL) {
        *nPayLoad = *nPayLoad - (pCurrent - pCharData);
        *pPayLoad = pCurrent;
    } else {
        *nPayLoad = 0;
        *pPayLoad = NULL;
    } /* end if */

    return nRc;
} /* end cn14analyzeDbmData */

/*
  -----------------------------------------------------------------------------
  public function cn14IsUnicode
  -----------------------------------------------------------------------------
 */
externC bool cn14IsUnicode (
    void           * sessionParm )
{
  return SESSION_UNICODE(sessionParm);
} /* cn14IsUnicode */


/*
  -----------------------------------------------------------------------------
  public function cn14analyzeRpmAnswer
  -----------------------------------------------------------------------------
 */
externC tsp00_Int4 cn14analyzeRpmAnswer(void         *pRPMSession,
                     tsp00_Int4     *plErrCode,
                     const _TCHAR **ppszErrText,
                     tsp00_Int4     *plErrLen,
                     tsp00_Int4     *plSqlCode,
                     const _TCHAR **ppszSqlErrText,
                     tsp00_Int4     *plSqlErrLen)
{
    tsp00_Int4 rc = DBMAPI_OK_CN14;

    ControlSessionT *pSession = (ControlSessionT*)pRPMSession;

    const _TCHAR *pszData   = NULL;
    const _TCHAR *pszErr    = "";
    const _TCHAR *pszSqlErr = "";

    tsp00_Int4 lMyErrLen = pSession->packetLen;

    *plErrCode = 0;             /* Initialisiere die fehlernummern */
    *plSqlCode = 0;

    *plErrLen = 0;              /* Initialisiere die fehlerlaengen */
    *plSqlErrLen = 0;


    if ( (NULL == pSession) || (NULL == pSession->packetOutData) )
    {
        rc = DBMAPI_INVSESSION_CN14;
    }
    else
    {
        pszData = (const _TCHAR *)pSession->packetOutData;

        if (0 == _tcsncmp(pszData, RPM_ERR_HEAD, _tcslen(RPM_ERR_HEAD)))    /* Fehler zurueckgegeben */
        {
            rc = DBMAPI_NOT_OK_CN14;

            /* Ok, wir haben einen fehler. Das naechste muss die fehlernummer sein, die
               noch genau 5 byte lang ist und hinter dem header liegt */
            *plErrCode = atol(&pszData[_tcslen(RPM_ERR_HEAD)]);
            /* Wird hier 0 zurueckgegeben, dann ist die fehlernummer nicht interpretierbar.
               Ein fehlertext wird dann aber, falls vorhanden, trotzdem eingetragen. */

            /* suche nach der fehlernummer das naechste newline und danach steht der fehlertext */
            pszErr = _tcschr(&pszData[_tcslen(RPM_ERR_HEAD)], _T('\n') );
            if (NULL != pszErr)
            {
                ++pszErr;           /* newline gefunden --> Um eine position weiter bewegen */
            }
            else                    /* kein newline gefunden, fehler aber offensichtlich gesetzt */
            {
                pszErr = &pszData[_tcslen(RPM_ERR_HEAD)];
            }

            if (0 == _tcsnicmp(pszErr, "sql error", 9) )    /* sql fehler */
            {
                pszSqlErr = pszErr + 10;
                *plSqlCode = atol(pszSqlErr);
                if (0 != *plSqlCode)
                {
                    pszSqlErr = _tcschr(pszSqlErr, _T('=') );
                    if (NULL != pszSqlErr)
                    {
                        ++pszSqlErr;    /* Ueberspringe das gleichheitszeichen */
                        while (0 != isspace(*pszSqlErr))
                        {
                            ++pszSqlErr;
                        }
                        *plSqlErrLen = lMyErrLen - (pszSqlErr - pszData);
                    }
                    else
                    {
                        pszSqlErr = "";     /* fehlerstringlaenge ist bereits mit init gesetzt*/
                    }
                }
                /* SQL fehler, kein allgemeiner fehler, fehlerstringlaenge ist bereits mit init gesetzt*/
                pszErr = "";
            }
            else
            {/* kein sql fehler und pszErr ist bereits gesetzt.*/

                *plErrLen = lMyErrLen - (pszErr - pszData);
            }
        }
        else    /* kein fehler, sondern ok gefunden, setze ptr hinter das new line nach ok, */
        {       /* denn dort faengt der text an, wenn einer vorhanden ist */
            pszErr = _tcschr(pszData, _T('\n'));

            if (pszErr != NULL)
            {
                pszErr++;
            }
            *plErrLen = lMyErrLen - (pszErr - pszData);
        }

        /* Setze zu guter letzt die rueckgabeptr richtig */
        if (NULL != ppszErrText)
        {
            *ppszErrText    = pszErr;
        }
        if (NULL != ppszSqlErrText)
        {
            *ppszSqlErrText = pszSqlErr;
        }
    }

    return rc;
}
/* end cn14analyzeRpmAnswer */

void cn14_CopyError ( 
    const Tools_DynamicUTF8String & oSource, 
    tsp00_ErrTextc                & oTarget )
{
    Tools_DynamicUTF8String oTemp(oSource);

    if (oTemp.Length() > 7) {
        if ((oTemp[0] == '-') && (oTemp[1] == '2') && (oTemp[2] == '4') && (oTemp[6] == ',')) {
            oTemp = oTemp.SubStr(oTemp.Find(',')+1);
//                if (oTemp.Find(':')  != Tools_DynamicUTF8String::NPos) {
//                    oTemp = oTemp.SubStr(oTemp.Find(':')+2);
//                    if (oTemp.Find('\n') != Tools_DynamicUTF8String::NPos) {
//                        oTemp = oTemp.SubStr(0, oTemp.Find('\n'));
//                    } // end if
//                } // end if
        } // end if
    } // end if
    oTarget.rawAssign(oTemp.CharPtr());
} // end cn14_CopyError

template <int elemCount_TV>
void cn14_trimRight(tsp00_CString<elemCount_TV> VAR_ARRAY_REF cString) {

    // make index point to the additional C string terminator
    int index(cString.size());
        
    // find start of trailing whitespace
    while( index > 0 && (cString[index-1]==0 || isspace(cString[index-1])) )
        --index;
    
    cString[index] = '\0';
}

/* pascal compatibility */
externC tsp00_Int4 cn14connectDBM_fixedSizeParameters (
      const tsp00_NodeIdc       VAR_ARRAY_REF servernode,
      const tsp00_DbNamec       VAR_ARRAY_REF dbname,
      const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,
      void                   ** sessionOut,
      tsp00_ErrTextc            VAR_ARRAY_REF errtext )
{
      tsp00_NodeIdc myNode;
      myNode.rawAssign(servernode.asCharp());
      cn14_trimRight(myNode);
      tsp00_DbNamec myDbname;
      myDbname.rawAssign(dbname.asCharp());
      cn14_trimRight(myDbname);
      tsp00_VFilenamec myDbroot;
      myDbroot.rawAssign(dbroot.asCharp());
      cn14_trimRight(myDbroot);

    return cn14connectDBM ( myNode.asCharp(), myDbname.asCharp(), myDbroot.asCharp(), sessionOut, errtext );
}

externC tsp00_Int4 cn14connectDBMUsr_fixedSizeParameters (
      const tsp00_NodeIdc       VAR_ARRAY_REF servernode,
      const tsp00_DbNamec       VAR_ARRAY_REF dbname,
      const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,
      const tsp00_C64c          VAR_ARRAY_REF user,
      void                   ** sessionOut,
      tsp00_ErrTextc            VAR_ARRAY_REF errtext )
  {
      tsp00_NodeIdc myNode;
      myNode.rawAssign(servernode.asCharp());
      cn14_trimRight(myNode);
      tsp00_DbNamec myDbname;
      myDbname.rawAssign(dbname.asCharp());
      cn14_trimRight(myDbname);
      tsp00_VFilenamec myDbroot;
      myDbroot.rawAssign(dbroot.asCharp());
      cn14_trimRight(myDbroot);
      tsp00_C64c myUser;
      myUser.rawAssign(user.asCharp());
      cn14_trimRight(myUser);

    return cn14connectDBMUsr (myNode.asCharp(), myDbname.asCharp(), myDbroot.asCharp(), myUser.asCharp(), sessionOut, errtext );
  }

externC tsp00_Int4 cn14connectDBMTp_fixedSizeParameters (
  const tsp00_NodeIdc       VAR_ARRAY_REF servernode,
  const tsp00_DbNamec       VAR_ARRAY_REF dbname,
  const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,
  const tsp00_C8c           VAR_ARRAY_REF sysid,
  const tsp00_C30c          VAR_ARRAY_REF conn,
  const tsp00_Pathc         VAR_ARRAY_REF profile,
  void                   ** sessionOut,
  tsp00_ErrTextc            VAR_ARRAY_REF errtext )
{
    tsp00_NodeIdc myNode;
    myNode.rawAssign(servernode.asCharp());
    cn14_trimRight(myNode);
    tsp00_DbNamec myDbname;
    myDbname.rawAssign(dbname.asCharp());
    cn14_trimRight(myDbname);
    tsp00_VFilenamec myDbroot;
    myDbroot.rawAssign(dbroot.asCharp());
    cn14_trimRight(myDbroot);
    tsp00_C8c mySysid;
    mySysid.rawAssign(sysid.asCharp());
    cn14_trimRight(mySysid);
    tsp00_C30c myConn;
    myConn.rawAssign(conn.asCharp());
    cn14_trimRight(myConn);
    tsp00_Pathc myProfile;
    myProfile.rawAssign(profile.asCharp());
    cn14_trimRight(myProfile);

  return cn14connectDBMTp(myNode.asCharp(), myDbname.asCharp(), myDbroot.asCharp(), mySysid.asCharp(), myConn.asCharp(), myProfile.asCharp(), sessionOut, errtext);
}

externC tsp00_Int4 cn14connect_fixedSizeParameters (
    const tsp00_NodeIdc       VAR_ARRAY_REF servernode,
    const tsp00_DbNamec       VAR_ARRAY_REF dbname,
    const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,
    const tsp00_VFilenamec    VAR_ARRAY_REF serverpgm,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtextC )
{
    tsp00_NodeIdc myNode;
    myNode.rawAssign(servernode.asCharp());
    cn14_trimRight(myNode);
    tsp00_DbNamec myDbname;
    myDbname.rawAssign(dbname.asCharp());
    cn14_trimRight(myDbname);
    tsp00_VFilenamec myDbroot;
    myDbroot.rawAssign(dbroot.asCharp());
    cn14_trimRight(myDbroot);
    tsp00_VFilenamec myServerpgm;
    myServerpgm.rawAssign(serverpgm.asCharp());
    cn14_trimRight(myServerpgm);

  return cn14connect(myNode.asCharp(), myDbname.asCharp(), myDbroot.asCharp(), myServerpgm.asCharp(), sessionOut, errtextC);
}

externC tsp00_Int4 cn14saveUser_fixedSizeParameters (
    const tsp00_NodeIdc   VAR_ARRAY_REF szServerNode,
    const tsp00_DbNamec   VAR_ARRAY_REF szDBName,
    const _TCHAR *        szUser)
{
    tsp00_NodeIdc myNode;
    myNode.rawAssign(szServerNode.asCharp());
    cn14_trimRight(myNode);
    tsp00_DbNamec myDbname;
    myDbname.rawAssign(szDBName.asCharp());
    cn14_trimRight(myDbname);

  return cn14saveUser(myNode.asCharp(), myDbname.asCharp(), szUser);
}

externC tsp00_Int4 cn14saveUserByKey_fixedSizeParameters (
    const _TCHAR *        szKey,
    const tsp00_NodeIdc   VAR_ARRAY_REF szServerNode,
    const tsp00_DbNamec   VAR_ARRAY_REF szDBName,
    const _TCHAR *        szUser,
    const _TCHAR *        szParams)
{
    tsp00_NodeIdc myNode;
    myNode.rawAssign(szServerNode.asCharp());
    cn14_trimRight(myNode);
    tsp00_DbNamec myDbname;
    myDbname.rawAssign(szDBName.asCharp());
    cn14_trimRight(myDbname);

  return cn14saveUserByKey(szKey, myNode.asCharp(), myDbname.asCharp(), szUser, szParams);
}

externC tsp00_Int4 cn14checkUser_fixedSizeParameters (
    const tsp00_NodeIdc   VAR_ARRAY_REF szServerNode,
    const tsp00_DbNamec   VAR_ARRAY_REF szDBName,
    const _TCHAR *        szUser)
{
    tsp00_NodeIdc myNode;
    myNode.rawAssign(szServerNode.asCharp());
    cn14_trimRight(myNode);
    tsp00_DbNamec myDbname;
    myDbname.rawAssign(szDBName.asCharp());
    cn14_trimRight(myDbname);

  return cn14checkUser(myNode.asCharp(), myDbname.asCharp(), szUser);
}

externC tsp00_Int4 cn14deleteUser_fixedSizeParameters (
    const tsp00_NodeIdc   VAR_ARRAY_REF szServerNode,
    const tsp00_DbNamec   VAR_ARRAY_REF szDBName)
{
    tsp00_NodeIdc myNode;
    myNode.rawAssign(szServerNode.asCharp());
    cn14_trimRight(myNode);
    tsp00_DbNamec myDbname;
    myDbname.rawAssign(szDBName.asCharp());
    cn14_trimRight(myDbname);
    
  return cn14deleteUser(myNode.asCharp(), myDbname.asCharp());
}

// this returns just the string length without the terminator!
tsp00_Int4 cn14buildDBMURIImpl(
    const char*               servernode,
    const char*               dbname,
    char*                     outURI,
    tsp00_ErrTextc            VAR_ARRAY_REF outErr) {

    bool lengthCheckMode(outURI==NULL);
    RTEComm_URIBuilder::URIBuildRC buildrc(RTEComm_URIBuilder::Error);
    Msg_List buildErr;
    SAPDB_UInt4 space(0);
    SAPDB_UInt4 dummy(0);
    RTEComm_URIBuilder myURIBuilder;
    
    // calculate the URI
    buildrc = myURIBuilder.BuildDBMURI(
        (const SAPDB_UTF8*)dbname,
        servernode,
        buildErr,
        true);

    if( buildrc == RTEComm_URIBuilder::NoError ) {
        // no error
        const char* theURI(myURIBuilder.GetURI());
        if( !lengthCheckMode )
            SAPDB_strcpy(outURI, theURI);
        return SAPDB_strlen(theURI);
    }
    else {
        // handle the error
        buildErr.MessageWithInsertedArguments(0, NULL, space, false);
        char* messageWithInsertedTags(new char[++space]);
        if( messageWithInsertedTags != NULL) {
            // get the message
            buildErr.MessageWithInsertedArguments(space, messageWithInsertedTags, dummy, false);
            SAPDB_strncpy((char*)outErr, messageWithInsertedTags, outErr.size()-1);
            outErr[outErr.size()-1] = '\0';
            delete[] messageWithInsertedTags;
        } else {
            // no memory for the error message
            cn14_setErrtext(outErr, DBMAPI_MEMORY_CN14);
        }
        return DBMAPI_URI_CN14;
    }
}

externC tsp00_Int4 WINAPI_CN14 cn14buildDBMURIWinAPI(
    const char* servernode,
    const char* dbname,
    const int   useOutURI,
    char*       outURI,
    char*       outErr) {

    tsp00_Int4 rc(0);
    tsp00_ErrTextc myErrText;
    myErrText.Init();

    if( servernode == NULL || dbname == NULL ) {
        rc = cn14_setErrtext(myErrText, DBMAPI_NULLPARAM_CN14);
        SAPDB_strcpy(outErr, myErrText.asCharp());
        return rc;
    }

    return cn14buildDBMURI(
        servernode[0]==0?NULL:servernode,
        dbname[0]==0?NULL:dbname,
        useOutURI,
        outURI,
        outErr);
}

externC tsp00_Int4 cn14buildDBMURI(
    const char* servernode,
    const char* dbname,
    const int   useOutURI,
    char*       outURI,
    char*       outErr) {

    tsp00_Int4 rc(0);
    tsp00_ErrTextc myErrText;
    myErrText.Init();
    char* myOutURI(useOutURI==DBMAPI_USE_OUTURI_CN14_YES?outURI:NULL);

#if !defined(DBMUNICODE)
    rc = cn14buildDBMURIImpl( servernode, dbname, myOutURI, myErrText);
    SAPDB_strcpy(outErr, myErrText.asCharp());
    if( rc>0 )
        ++rc; // increment, because we want to return the size, not only the length
    return rc;
#else
    bool okay(true);
    char* servernodeIntern(NULL);
    tsp00_Int4 lenServernodeIntern(0);
    char* dbNameIntern(NULL);
    tsp00_Int4 lenDbNameIntern(0);
    
    // convert to USC2 -> internal encoding
    if( servernode != NULL ) {
        UCS2_TO_ACP_DYNAMIC(servernodeIntern, lenServernodeIntern, servernode, -1, &okay);
        if( !okay ) {
            rc = cn14_setErrtext(myErrText, DBMAPI_USC2_CN14);
            SAPDB_strcpy(outErr, myErrText.asCharp());
            return rc;
        }
    }

    if( dbname != NULL ) {
        UCS2_TO_ACP_DYNAMIC(dbNameIntern, lenDbNameIntern, dbname, -1, &okay);
        if( !okay ) {
            rc = cn14_setErrtext(myErrText, DBMAPI_USC2_CN14);
            SAPDB_strcpy(outErr, myErrText.asCharp());
            if( servernodeIntern != NULL )
                delete[] servernodeIntern;
            return rc;
        }
    }

    // now do it
    rc = cn14buildDBMURIImpl( servernodeIntern, dbNameIntern, myOutURI, myErrText);
    
    if( servernodeIntern != NULL )
        delete[] servernodeIntern;

    if( dbNameIntern != NULL )
        delete[] dbNameIntern;

    if( rc <= 0 ) {
        okay = true;
        ACP_TO_UCS2(outErr, SAPDB_strlen(myErrText.asCharp())*2, myErrText, SAPDB_strlen(myErrText.asCharp()), &okay);
        return DBMAPI_URI_CN14;
    }

    // convert internal encoding -> USC2
    if( myOutURI != NULL ) {
        // outURI now contains the path in internal encoding
        char* internalURI(new char[SAPDB_strlen(myOutURI)+1]);
        if( internalURI == NULL ) {
            rc = cn14_setErrtext(myErrText, DBMAPI_MEMORY_CN14);
            SAPDB_strcpy(outErr, myErrText.asCharp());
            return rc;
        }
        SAPDB_strcpy(internalURI, myOutURI);
        tsp00_Int4 ucs2Chars = ACP_TO_UCS2(myOutURI, SAPDB_strlen(internalURI)*2, internalURI, SAPDB_strlen(internalURI), &okay);
        delete [] internalURI;
        return ucs2Chars;
    }
    else {
        ++rc; // rc is always a string length here
              // increment, because we want to return the size, not only the length
        return rc; // upper bound
    }
#endif  //!defined(DBMUNICODE)
}
