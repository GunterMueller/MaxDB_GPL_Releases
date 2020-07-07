/*!========================================================================
  @ingroup      Sec
  @file         RTESec_SAPSSL.cpp
  @author       StefanP

  @brief        Secure Socket Layer (SSL) Interface
                The Secure Socket Layer (SSL) Interface is based on the 
                SAP Socket library (ssslib) and can only be used together 
                with the adapted SAP NI Interface which is implemented 
                in the SAPNI.
                We always work with a server certificate and only a
                anonymous client certificate.
                Prerequisites:
                On the client an d server side the SAP sapcrypto library
                has to be installed in the independent libary path. Also
                the server certificate SAPSSLS.pse and the anonymous client
                certificate SAPSSLA.pse have to be installed in the security
                path (<independant data path>/sec).

  @since        2006-01-11  13:26
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

  \endif
 ============================================================================*/


/*===========================================================================*/
#if !defined STUB_MODULE /* IMPLEMENTATION_MODULE */
/*===========================================================================*/
/*--------------------------------------------------------------------*/
/* SAP includes                                                       */
/*--------------------------------------------------------------------*/
#define SAPwithUNICODE 

#ifdef WIN32
  #define SAPonNT
#endif

#ifdef _IBMR2
  #define SAPonRS6000
#endif

#if PA11 || PA20W
  #define SAPonHPPA
#endif

#if defined(OSF1)
  #define SAPonALPHA
#endif

#if defined (HP_IA64)
  #define SAPonHPPA
  #define SAPonHPIA64
#endif

#if (defined(SVR4) && defined(SUN)) || defined(SOLARIS)
  #define SAPonSUN
#endif

#if defined(LINUX)
#define SAPonLIN
#endif

#if defined(FREEBSD)
#define SAPonFREEBSD
#endif

//UNICODE
#include "sap/700/saptype.h"	/* nocheck */
#include "sap/700/saptypeb.h"	/* nocheck */
#include "sap/700/saptypec.h"	/* nocheck */
#include "sap/700/sapuc.h"		/* nocheck */
#include "sap/700/sapuc2.h"		/* nocheck */

//NI
#include "sap/700/err.h"		/* nocheck */
#include "sap/700/nixx.h"		/* nocheck */
#include "sap/700/dptrace.h"	/* nocheck */

//SSL
#include "sap/700/ssslxx.h"     /* nocheck */
#include "sap/700/sapwrap.h"    /* nocheck */
#include "sap/700/dlxx.h"       /* nocheck */
#include "sap/700/cdkapi.h"     /* nocheck */

#include    "heo47.h"                                   /* nocheck */
#include    "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"  /* nocheck */
#include    "RunTime/Security/RTESec_SSL.h"             /* nocheck */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* externC is defined in SAP-Includes (sapuc.h) too   */
//#undef externC
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*===========================================================================*/
#endif /* IMPLEMENTATION_MODULE */
/*===========================================================================*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/Security/RTESec_SAPSSL.h"
#include    "RunTime/System/RTESys_SharedLib.h"
#include    "heo46.h"
#include    "geo007_2.h"      


/*===========================================================================*/
#if !defined STUB_MODULE /* IMPLEMENTATION_MODULE */
/*===========================================================================*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    SAP Unicode is realized as UTF16. So a single unicode character may need  
    a four byte representation. In case of unicode the datatype SAP_UC is
    a short, i.e sizeof (SAP_UC) == 2.
    Therefore in case of unicode the following define is not quite exact. But we
    assume that we have not to deal with characters that need a four byte 
    representation.
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */ 

#define LENGTH_OF_UNICODE_CHARACTER     sizeof (SAP_UC)
//`cU´ can not be used under UNIX because a special precompiler from Walldorf 
//is necessary to resolve it 

#define FILE_OPEN_FLAG                  "wb"

#if defined (_WIN32)
#define SSL_DELIMETER                   "\\"
#else
#define SSL_DELIMETER                   "/"
#endif

#define TMP_PK_CONTAINER                "TmpPKContainer.pse"
#define DUMMY_DN                        "CN=*.xyz.sap.corp, O=SAP-AG, C=DE"


#define STRINGU_CONVERT_ON_STACK (_string, _stringU, _errText)                                      \
    {                                                                                               \
        _stringU = (SAP_UC *) alloca ((strlen (_string) + 1) * LENGTH_OF_UNICODE_CHARACTER);        \
        if (NULL == _stringU)                                                                       \
        {                                                                                           \
            ;                                                                                       \    
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \    
            A7sToUcs (_stringU, _string);                                                           \
        }                                                                                           \
    }

//            strcpy (_errText.asCharp (), ERRMSG_SSL_MEM, string);
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/
externC FILE *tf ; /* SAP global trace/error file descriptor */

SAP_UC *                pPSEFileNameU = NULL; //Server or Client certificate file name
SAP_UC *                pathSecurityU = NULL; //SECUDIR path
SAP_UC *                pathSSLLibraryU = NULL; //SSL Library Path
SAPDB_Bool              rteSSL_IsClient = true;
SSSL_ROLE_T             role = SSSL_ROLE_CLIENT;
SAPDB_Bool              is_SNC_CDK_initialized = false;
SAPDB_Bool              is_SAP_SLL_initialized = false;



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/
static void *   DupSSLHandle ( void *handleSSL );
static void     RTESec_NiSSLBuildErrorString (tsp00_ErrTextc &      errText,
                                              SAPDB_Char const *    ErrorMsgHeader,
                                              SAPRETURN             SapRc);
static void
RTESec_SSLSNCBuildErrorString (tsp00_ErrTextc &     errText,
                               SAPDB_Char const *   functionName,
                               RC                   rcSNC);

static SAPDB_Bool
RTESec_SAPSSLInitLibraryNameU (SAPDB_Char *              pathSSLLibrary,
                              tsp00_ErrTextc &          errText );

static SAPDB_Bool
RTESec_SAPSSLInitPSENameU     (SAPDB_Char const * const   pPSEName,
                              tsp00_ErrTextc &           errText );

static SAPDB_Bool
RTESec_SAPSSLInitSecudirNameU (SAPDB_Char *              pathSecurity,
                               tsp00_ErrTextc &          errText );

#endif /* IMPLEMENTATION_MODULE */


/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*===========================================================================*/
#if !defined STUB_MODULE /* IMPLEMENTATION_MODULE */

/*===========================================================================*/
externC int  RTESec_SAPSSLIsInitialized ()
{
    return is_SAP_SLL_initialized;
}

/*===========================================================================*/

externC tsp01_CommErr_Enum  RTESec_SAPSSLInit (SAPDB_Bool         isClient,
                                               SAPDB_Char *       pathSSLLibrary,
                                               SAPDB_Char *       pathSecurity,  
                                               tsp00_ErrTextc &   errText)
{
    sapssl_init_desc        init_params;
    SAPRETURN               rc;
    tsp01_CommErr_Enum      commState = commErrOk_esp01;

    SapSSLSetTraceFile(tf);

    memset (&init_params, 0, sizeofR(init_params));

    init_params.struct_size      = sizeofR(init_params);

    //Setting Library path
    if (!RTESec_SAPSSLInitLibraryNameU (pathSSLLibrary, errText))
    {
        return commErrNotOk_esp01;
    }

    init_params.ssl_lib = pathSSLLibraryU;

    //Setting Secudir path
    if (!RTESec_SAPSSLInitSecudirNameU (pathSecurity, errText))
    {
        return commErrNotOk_esp01;
    }

    init_params.secudir = pathSecurityU;

    //We do not use client certificate
    if (!isClient) 
    { /* SERVER */
        rteSSL_IsClient = false;
        if (!RTESec_SAPSSLInitPSENameU (SERVER_PSE_FILE_NAME, errText))
        {
            return commErrNotOk_esp01;
        }

        init_params.sapssls_filename = pPSEFileNameU;
        init_params.usage_flags      = SSSL_UF_SERVER;
        role                         = SSSL_ROLE_SERVER;
    } 
    else 
    { /* CLIENT */
        if (!RTESec_SAPSSLInitPSENameU (ANONYMOUS_PSE_FILE_NAME, errText))
        {
            return commErrNotOk_esp01;
        }

        init_params.sapssla_filename  = pPSEFileNameU;
        init_params.usage_flags       = SSSL_UF_ANON_CLIENT;
    }

    rc = SapSSLInit (FALSE, &init_params, 0);
    if ( rc!=SAP_O_K ) 
    {
        RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_INIT_ERROR, rc);
        ErrTrace(tf);
        return commErrNotOk_esp01;
    }

    is_SAP_SLL_initialized = true;

    return commErrOk_esp01;
}

/*===========================================================================*/

externC tsp01_CommErr_Enum  RTESec_SAPSSLStartSession (void *              pHandleNI,
                                                       SAPDB_Char *        pHostname,
                                                       SAPDB_Bool          ignoreHostnameInServerCert,
                                                       void **             ppHandleSSL,
                                                       tsp00_ErrTextc &    errText)
{
    tsp01_CommErr_Enum  commState = commErrOk_esp01;
    SSSL_HDL            tmpSSLHandle   = SSSL_INVALID_HDL;
    SAPRETURN	        rc          = SAP_O_K;
    

    rc = SapSSLSessionInit (&tmpSSLHandle, role, SSSL_AUTH_NO_CLIENT_CERT);
    if (rc!=SAP_O_K)
    {
        RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_SESSION_INIT_ERROR, rc);
        ErrTrace(tf);
        return commErrNotOk_esp01;
    }


    *ppHandleSSL = DupSSLHandle (&tmpSSLHandle);
    commState = (NULL != *ppHandleSSL ? commErrOk_esp01 : commErrNotOk_esp01);
    if (commErrOk_esp01 != commState)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_DUP_SSL_HDL);
        return commState;
    }

    rc = SapSSLSetSessionCredential (*((SSSL_HDL*)*ppHandleSSL), pPSEFileNameU);
    if (rc!=SAP_O_K)
    {
        RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_CREDENTIAL_ERROR, rc);
        ErrTrace(tf);
        return commErrNotOk_esp01;
    }

    /* This call sets non-blocking on the socket as well */
    rc = SapSSLSetNiHdl (*((SSSL_HDL*)*ppHandleSSL), *((NI_HDL *)(pHandleNI)));
    if (rc!=SAP_O_K)
    {
        RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_SET_NIHDL_ERROR, rc);
        ErrTrace(tf);
        return commErrNotOk_esp01;
    }


    if (rteSSL_IsClient)
    {
        SAP_UC *            pHostnameU;

        pHostnameU = (SAP_UC *) alloca ((strlen (pHostname) + 1) * LENGTH_OF_UNICODE_CHARACTER);
        if (NULL == pHostnameU)
        {
            strcpy (errText.asCharp (), ERRMSG_SSL_MEM_HOSTNAME_NAME);
            return commErrNotOk_esp01;
        }

        A7sToUcs (pHostnameU, pHostname);

        rc = SapSSLSetTargetHostname (*((SSSL_HDL*)*ppHandleSSL), pHostnameU);
        if (rc!=SAP_O_K)
        {
            RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_SET_HOSTNAME, rc);
            ErrTrace(tf);
            return commErrNotOk_esp01;
        }
    }

    rc = SapSSLSessionStart(*((SSSL_HDL*)*ppHandleSSL));
    if (SAP_O_K != rc) 
    {
        if (!ignoreHostnameInServerCert || SSSLERR_SERVER_CERT_MISMATCH != rc) 
        {
            RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_SESSION_START, rc);
            ErrTrace(tf);
            return commErrNotOk_esp01;
        }
    }


/*
    SAP_RAW  * cert_blob;
    SAP_INT    cert_blob_len;
    SAP_UC   * subject_dn;
    SAP_UC   * issuer_dn;
    SAP_UC   * cipher;

    rc = SapSSLGetPeerInfo(*((SSSL_HDL*)*ppHandleSSL), &cert_blob, &cert_blob_len, &subject_dn, &issuer_dn, &cipher );
    if (SAP_O_K != rc)
    {
        RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_GET_PEER_INFO, rc);
        ErrTrace(tf);
        return commErrNotOk_esp01;
    }
    else
    {
        if (!RTESec_WriteCertToFile (CAPTURED_SERVER_CERT_FILENAME, cert_blob, cert_blob_len, errText))
        {
            //Fehlerbehandlung
            return commErrNotOk_esp01;
        }
    }

    {
        sssl_cert_array_desc   * cert_array;
        SAP_INT                  cert_array_len;
        SAP_RAW                * cipher_id;
        SAP_INT                  cipher_id_len;
        SAP_INT                  keysize;

        rc = SapSSLGetPeerInfo2( sssl, FALSE, &cert_array, &cert_array_len, &cipher_id, &cipher_id_len, &keysize );
    }
*/

    return commErrOk_esp01;
}


/*===========================================================================*/

externC tsp00_Uint          RTESec_SAPSSLSend (void *                        pHandleSend,
                                               char *                        pData,
                                               int                           length,
                                               tsp00_ErrTextc &              errText)

{
    tsp00_Uint              commState = commErrOk_esp01;
    SAPRETURN               rcSAP;
    SSSL_HDL                handleSSL = *((SSSL_HDL *)pHandleSend);
    SAP_INT                 bytesSend;

    for (bytesSend = 0;
        length > 0 && commState == commErrOk_esp01 ;
        pData += bytesSend, length -= bytesSend )
    {
	    rcSAP = SapSSLWrite(handleSSL, (SAP_RAW *)pData, length, NI_BLOCK, &bytesSend);
        if ( rcSAP != SAP_O_K)
        {
            RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_SEND, rcSAP);
            commState = commErrNotOk_esp01 ;
            ErrTrace(tf);
        }
    }

    return commState;
}

/*===========================================================================*/

externC tsp00_Uint          RTESec_SAPSSLReceive (void *                        pHandleReceive,
                                                  char *                        pData,
                                                  tsp00_Uint                    bytesToReceive,
                                                  tsp00_Uint *                  bytesReceived,
                                                  tsp00_ErrTextc &              errText )
{
    tsp00_Uint              commState = commErrOk_esp01;
    SAPRETURN               rcSAP;
    SSSL_HDL                handleSSL = *((SSSL_HDL *)pHandleReceive);
    SAP_INT                 bytesRead;


    rcSAP = SapSSLRead(handleSSL, (SAP_RAW *)pData, bytesToReceive, NI_BLOCK, &bytesRead);

    if (SAP_O_K != rcSAP)
    {
        RTESec_NiSSLBuildErrorString (errText, ERRMSG_SSL_RECEIVE, rcSAP);
        commState = commErrNotOk_esp01 ;
        ErrTrace(tf);
    }
    else
    {
        *bytesReceived = bytesRead;
    }

    return commState;
}

/*===========================================================================*/

externC void  RTESec_SAPSSLSessionClose (void *  pHandleSSL)
{
   if ( NULL != pHandleSSL)
   {
       if (SSSL_INVALID_HDL != *((SSSL_HDL *)(pHandleSSL))) 
       {
          SapSSLSessionDone ((SSSL_HDL *)pHandleSSL);  
       }

       free (pHandleSSL) ;
       pHandleSSL = NULL; 
   }
}

/*===========================================================================*/

externC void  RTESec_SAPSSLClose ()
{
    if (is_SAP_SLL_initialized)
    {
        SapSSLDone ();  
    }

    if (NULL != pPSEFileNameU)
    {
        free (pPSEFileNameU);
        pPSEFileNameU = NULL;
    }

    if (NULL != pathSSLLibraryU)
    {
        free (pathSSLLibraryU);
        pathSSLLibraryU = NULL;
    }

    if (NULL != pathSecurityU)
    {
        free (pathSecurityU);
        pathSecurityU = NULL;
    }

    is_SAP_SLL_initialized = false;
//Unload SNC CDK
}


/*===========================================================================*/

/* 
    SSL Configuration Utility from SNC

    SNC - Secure Network Communication
    CDK - Configuration Development Kit
*/


externC int  RTESec_SAPSSLUtilIsInitialized ()
{
    return is_SNC_CDK_initialized;
}

/*===========================================================================*/

externC tsp01_CommErr_Enum  RTESec_SAPSSLUtilInit     (SAPDB_Bool         isClient,
                                                       SAPDB_Char *       pathSSLLibrary,
                                                       SAPDB_Char *       pathSecurity,  
                                                       tsp00_ErrTextc &   errText)
{
    /* Load further function pointer of the SAPCRYPTOLIB.
       This is done by using the SNC Configuration Development Kit */

    RC                      rcSNC;

    if (!RTESec_SAPSSLInitLibraryNameU (pathSSLLibrary, errText))
    {
        return commErrNotOk_esp01;
    }

    if (!isClient) 
    { /* SERVER */
        rteSSL_IsClient = false;
        if (!RTESec_SAPSSLInitPSENameU (SERVER_PSE_FILE_NAME, errText))
        {
            return commErrNotOk_esp01;
        }
    } 
    else 
    { /* CLIENT */
        if (!RTESec_SAPSSLInitPSENameU (ANONYMOUS_PSE_FILE_NAME, errText))
        {
            return commErrNotOk_esp01;
        }
    }

    if (!RTESec_SAPSSLInitSecudirNameU (pathSecurity, errText))
    {
        return commErrNotOk_esp01;
    }

    rcSNC = CDKLoadSAPCryptModule (pathSSLLibraryU);
    if (0 != rcSNC)
    {
        RTESec_SSLSNCBuildErrorString (errText, "CDKLoadSAPCryptModule", rcSNC);
        return commErrNotOk_esp01;
    }

    is_SNC_CDK_initialized = true;

    //Set Secudir
    rcSNC = CDKSetSecuDir (pathSecurityU);
    if (0 != rcSNC)
    {
        RTESec_SSLSNCBuildErrorString (errText, "CDKSetSecuDir", rcSNC);
        return commErrNotOk_esp01;
    }


    return commErrOk_esp01;
}

/*===========================================================================*/

externC tsp01_CommErr_Enum   RTESec_SAPSSLExportOwnCertificate (SAPDB_Byte **     ppCertBuffer,
                                                                SAPDB_UInt4 &     lenCert,       
                                                                tsp00_ErrTextc &  errText )
{
    RC                  rc;
    tsp01_CommErr_Enum  commError = commErrOk_esp01;

    //Certificate is actually  a SAP_UC string
    rc = CDKExportMyCert (pPSEFileNameU, (SAP_CHAR **)ppCertBuffer);
    if (0 != rc)
    {
        RTESec_SSLSNCBuildErrorString (errText, "CDKExportMyCert", rc);
        lenCert = 0;
        commError = commErrNotOk_esp01;
    }
    else
    {
        lenCert = ((SAPDB_UInt4)(strlenU ((SAP_UC *)(*ppCertBuffer))) + 1) * LENGTH_OF_UNICODE_CHARACTER;
    }

    return commError;
}

/*===========================================================================*/

externC void   RTESec_SAPSSLAuxFreeString (SAPDB_Byte **     ppBuf)
{
    CdkAuxFreeString ((void **) ppBuf);
}

/*===========================================================================*/

externC void  RTESec_SAPSSLUtilClose ()
{

    if (is_SNC_CDK_initialized)
    {
        CDKReleaseSAPCryptModule ();
    }

    if (NULL != pPSEFileNameU)
    {
        free (pPSEFileNameU);
        pPSEFileNameU = NULL;
    }

    if (NULL != pathSSLLibraryU)
    {
        free (pathSSLLibraryU);
        pathSSLLibraryU = NULL;
    }

    if (NULL != pathSecurityU)
    {
        free (pathSecurityU);
        pathSecurityU = NULL;
    }

    is_SNC_CDK_initialized = false;
}

/*===========================================================================*/
/*
externC void  RTESec_SAPSSLPrintCertificate (SAPDB_Byte const *   pCertificate)
{
    SAP_UC * pCertificateU = (SAP_UC *) pCertificate;
//    SAP_UC   formatString[2];

//    A7sToUcs (formatString, "%s\n");

    printfU (cU("%s\n"), pCertificate);
//    printfU (formatString, pCertificateU);
}
*/
/*===========================================================================*/

externC tsp01_CommErr_Enum  RTESec_SAPSSLWriteCertificateToFile (SAPDB_Byte const *            pCertificate, 
                                                                 SAPDB_UInt4                   bytesToWrite,
                                                                 tsp00_ErrTextc &              errText )
{
    FILE *              fp = NULL;
    size_t              written;
    SAP_UC *            pCertFileNameU;
    SAP_UC *            pCertFilePathU;
    SAP_UC *            openFlagU;
    tsp01_CommErr_Enum  commState = commErrOk_esp01;
    SAP_UC              delimeterU[2];


    if (NULL == pCertificate) 
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_NO_CERT);
        return commErrNotOk_esp01;
    }

    //Create Path to Certificate File 
    pCertFileNameU = (SAP_UC *) alloca ((strlen (CAPTURED_SERVER_CERT_FILENAME) + 1) * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == pCertFileNameU)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_MEM_CERT_FILE_NAME);
        return commErrNotOk_esp01;
    }

    A7sToUcs (pCertFileNameU, CAPTURED_SERVER_CERT_FILENAME);
    pCertFilePathU = (SAP_UC *) alloca ((strlenU (pathSecurityU) + strlenU (pCertFileNameU) + 2) * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == pCertFilePathU)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_MEM_CERT_FILE_NAME);
        return commErrNotOk_esp01;
    }
    
    strcpyU (pCertFilePathU, pathSecurityU);
    A7sToUcs (delimeterU, SSL_DELIMETER);
    strcatU (pCertFilePathU, delimeterU);
    strcatU (pCertFilePathU, pCertFileNameU);
    
    openFlagU = (SAP_UC *) alloca ((strlen (FILE_OPEN_FLAG) + 1) * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == openFlagU)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_MEM_OPEN_FLAG);
        return commErrNotOk_esp01;
    }

    A7sToUcs (openFlagU, FILE_OPEN_FLAG);


    fp = fopenU (pCertFilePathU, openFlagU);
    if ( fp==NULL ) 
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_OPEN_CERT_FILE);
    } 
    else 
    {
        written = fwriteR( (SAP_RAW *)pCertificate, 1, (size_t) bytesToWrite, fp );
        if ( written != bytesToWrite ) 
        {
            strcpy (errText.asCharp (), ERRMSG_SSL_WRITE_CERT_FILE);
            commState = commErrNotOk_esp01;
        } 

        fclose(fp);  
    }

    return commState;
} 

/*===========================================================================*/

externC tsp01_CommErr_Enum  RTESec_SAPSSLImportCertificate (SAPDB_Byte const *   pCertificate, 
                                                            tsp00_ErrTextc &     errText )
{
    RC                      rc;
    tsp01_CommErr_Enum      commState = commErrOk_esp01;

    rc = CDKAdd2PK(pPSEFileNameU, (SAP_CHAR *)pCertificate);
    if (0 != rc)
    {
        RTESec_SSLSNCBuildErrorString (errText, "CDKAdd2PK", rc);
        commState = commErrNotOk_esp01;
    }

    return commState;
}

/*===========================================================================*/

externC tsp01_CommErr_Enum  RTESec_SAPSSLShowCertificate (SAPDB_Byte const *   pCertificate, 
                                                          tsp00_ErrTextc &     errText )
{
    RC                      rc;
    SAP_CHAR *              trustedCertList;
    SAP_UC *                pTmpPKContainerFilenameU;
    const SAP_A7 *          pSecurityPath;
    SAPDB_Char *            pTmpPKContainerPath;
    SAP_UC *                dummyDN_U;
    SAP_UC *                formatStringU;

    //The SAPCRYPTOLIB does not provide a interface to show the DER encoded PK of a certificate.
    //Therefore we have to perform the following stupid workaround:

    pTmpPKContainerFilenameU = (SAP_UC *) alloca ((strlen (TMP_PK_CONTAINER) + 1) * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == pTmpPKContainerFilenameU)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_MEM_CERTIFICATE_NAME);
        return commErrNotOk_esp01;
    }

    A7sToUcs (pTmpPKContainerFilenameU, TMP_PK_CONTAINER);

    //Try to delete temporary PK container if it exists of a former run
    pSecurityPath = UcsToA7sVola(pathSecurityU);

    pTmpPKContainerPath = (SAPDB_Char *) alloca (strlen (pSecurityPath) + strlen (TMP_PK_CONTAINER) + 2);
    if (NULL == pTmpPKContainerPath)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_ALLOC);
        return commErrNotOk_esp01;
    }

    strcpy (pTmpPKContainerPath, pSecurityPath);
    strcat (pTmpPKContainerPath, SSL_DELIMETER);
    strcat (pTmpPKContainerPath, TMP_PK_CONTAINER);

#if defined(_WIN32)
    DeleteFile (pTmpPKContainerPath);
#else
    unlink (pTmpPKContainerPath);
#endif /* WIN32 */

    dummyDN_U = (SAP_UC *) alloca ((strlen (DUMMY_DN) + 1) * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == dummyDN_U)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_ALLOC);
        return commErrNotOk_esp01;
    }

    A7sToUcs (dummyDN_U, DUMMY_DN);

    rc = CDKGetPSE (pTmpPKContainerFilenameU, NULL, NULL, dummyDN_U);
    if (0 != rc)
    {
        RTESec_SSLSNCBuildErrorString (errText, "CDKGetPSE", rc);
        return commErrNotOk_esp01;
    }

    rc = CDKAdd2PK (pTmpPKContainerFilenameU, (SAP_CHAR *)pCertificate);
    if (0 != rc)
    {
        RTESec_SSLSNCBuildErrorString (errText, "CDKAdd2PK", rc);
        return commErrNotOk_esp01;
    }

    rc = CDKListTrustedCerts (pTmpPKContainerFilenameU, &trustedCertList);
    if (0 != rc)
    {
        RTESec_SSLSNCBuildErrorString (errText, "CDKListTrustedCerts", rc);
        return commErrNotOk_esp01;
    }

/*
        Example for using makro:
        STRINGU_CONVERT_ON_STACK ("%s\n", formatStringU, errText)  
        if (NULL = formatStringU)
        {
            return commErrNotOk_esp01;
        }
*/
    formatStringU = (SAP_UC *) alloca ((strlen ("%s\n") + 1) * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == formatStringU)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_ALLOC);
        return commErrNotOk_esp01;
    }

    A7sToUcs (formatStringU, "%s\n");

    printfU (formatStringU, trustedCertList);

    //Remove temporary PK container
#if defined(_WIN32)
    DeleteFile (pTmpPKContainerPath);
#else
    unlink (pTmpPKContainerPath);
#endif /* WIN32 */

    CdkAuxFreeString ((void **) &trustedCertList);

    return commErrOk_esp01;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static void *   DupSSLHandle ( void *handleSSL )
{
  void *dupHandleSSL = malloc (sizeof(SSSL_HDL)) ;

  if (NULL != dupHandleSSL)
    SAPDB_MemCopyNoCheck (dupHandleSSL, handleSSL, sizeof(SSSL_HDL)) ;
    
  return (dupHandleSSL) ;
}

/*===========================================================================*/

static void
RTESec_NiSSLBuildErrorString (tsp00_ErrTextc &  errText,
                              SAPDB_Char const *ErrorMsgHeader,
                              SAPRETURN         SapRc)
{
  const SAP_A7 * SSLErrorString ;
  int  ErrorHeaderLen  = (int)strlen(ErrorMsgHeader) ;
  int  ErrorTrailerLen = errText.size() - ErrorHeaderLen;

  SSLErrorString = UcsToA7sVola(SapSSLErrorName (SapRc));
  
  errText.rawAssign (ErrorMsgHeader);

  if ( ErrorTrailerLen > 0 )
  {
    memset (errText.asCharp () + ErrorHeaderLen, ',', 1);
    strncpy(errText.asCharp () + ErrorHeaderLen + 1 , (SAPDB_Char *) SSLErrorString, ErrorTrailerLen - 1) ;
    memset (errText.asCharp () + errText.size(), '\0', 1);
  }
}

/*===========================================================================*/

static void
RTESec_SSLSNCBuildErrorString (tsp00_ErrTextc &     errText,
                               SAPDB_Char const *   functionName,
                               RC                   rcSNC)
{
    SAPDB_Char  rcStringBuffer[40] = ": ";
    int         ErrorHeaderLen  = (int)strlen(functionName) ;
    int         ErrorTrailerLen;


  memset (rcStringBuffer + strlen (rcStringBuffer), '\0', sizeof (rcStringBuffer) - strlen (rcStringBuffer)); 

/* Error codes from sapwrap.h  */
  switch (rcSNC)
  {
  case WRONG_SIGN_ALG:
      strcat (rcStringBuffer, "WRONG_SIGN_ALG");
      break;
  case MALFORMED_DN:
      strcat (rcStringBuffer, "MALFORMED_DN");
      break;
  case DERIVE_ABS_PATH:
      strcat (rcStringBuffer, "DERIVE_ABS_PATH");
      break;
  case PARAM_ERROR:
      strcat (rcStringBuffer, "PARAM_ERROR");
      break;
  case PSE_CREATION_FAILED:
      strcat (rcStringBuffer, "PSE_CREATION_FAILED");
      break;
  case SET_AF_OPTIONS:
      strcat (rcStringBuffer, "SET_AF_OPTIONS");
      break;
  case CREATE_CERT_FAILED:
      strcat (rcStringBuffer, "CREATE_CERT_FAILED");
      break;
  case WRITE_CERT2PSE:
      strcat (rcStringBuffer, "WRITE_CERT2PSE");
      break;
  case CREATE_PKROOT_FAILED:
      strcat (rcStringBuffer, "CREATE_PKROOT_FAILED");
      break;
  case WRITE_PKROOT2PSE:
      strcat (rcStringBuffer, "WRITE_PKROOT2PSE");
      break;
  case OPEN_PSE_FAILED:
      strcat (rcStringBuffer, "OPEN_PSE_FAILED");
      break;
  case CREATE_CERT_REQ:
      strcat (rcStringBuffer, "CREATE_CERT_REQ");
      break;
  case ENCODE_CERT_REQ:
      strcat (rcStringBuffer, "ENCODE_CERT_REQ");
      break;
  case MEM_ERROR:
      strcat (rcStringBuffer, "MEM_ERROR");
      break;
  case FAILED_KEY_GEN:
      strcat (rcStringBuffer, "FAILED_KEY_GEN");
      break;
  case BAD_FORMAT:
      strcat (rcStringBuffer, "BAD_FORMAT");
      break;
  case DECODE_CERT:
      strcat (rcStringBuffer, "DECODE_CERT");
      break;
  case INSTALL_CERT:
      strcat (rcStringBuffer, "INSTALL_CERT");
      break;
  case MY_NAME:
      strcat (rcStringBuffer, "MY_NAME");
      break;
  case ADD_CREDENTIALS:
      strcat (rcStringBuffer, "ADD_CREDENTIALS");
      break;
  case NO_CREDENTIALS:
      strcat (rcStringBuffer, "NO_CREDENTIALS");
      break;
  case DEL_CREDENTIALS:
      strcat (rcStringBuffer, "DEL_CREDENTIALS");
      break;
  case SINGLE_LOGIN:
      strcat (rcStringBuffer, "SINGLE_LOGIN");
      break;
  case GET_CERT:
      strcat (rcStringBuffer, "GET_CERT");
      break;
  case GET_POLICY:
      strcat (rcStringBuffer, "GET_POLICY");
      break;
  case NEW_PIN:
      strcat (rcStringBuffer, "NEW_PIN");
      break;
  case FUNC_NOT_LOADED:
      strcat (rcStringBuffer, "FUNC_NOT_LOADED");
      break;
  case NOT_FOUND:
      strcat (rcStringBuffer, "NOT_FOUND");
      break;
  case DELETE_PK:
      strcat (rcStringBuffer, "DELETE_PK");
      break;
  case ADD_PK:
      strcat (rcStringBuffer, "ADD_PK");
      break;
  case ENCODE_CERT:
      strcat (rcStringBuffer, "ENCODE_CERT");
      break;
  case FILE_OPEN:
      strcat (rcStringBuffer, "FILE_OPEN");
      break;
  case NO_TICKET_FOUND:
      strcat (rcStringBuffer, "NO_TICKET_FOUND");
      break;
  case SET_SECUDIR_FAILED:
      strcat (rcStringBuffer, "SET_SECUDIR_FAILED");
      break;
  case FILE_WRITE:
      strcat (rcStringBuffer, "FILE_WRITE");
      break;
  case SECUDIR_NOT_EXIST:
      strcat (rcStringBuffer, "SECUDIR_NOT_EXIST");
      break;
  case USER_NOT_FOUND:
      strcat (rcStringBuffer, "USER_NOT_FOUND");
      break;
#if defined (LINUX)|| (defined (_WIN32) && !defined(AMD64))
  case DECODE_PRIVATE_KEY:
      strcat (rcStringBuffer, "DECODE_PRIVATE_KEY");
      break;
  case MISSING_PRIVATE_KEY:
      strcat (rcStringBuffer, "MISSING_PRIVATE_KEY");
      break;
#endif
  case MISSING_PIN:
      strcat (rcStringBuffer, "MISSING_PIN");
      break;
  case MISSING_PSE:
      strcat (rcStringBuffer, "MISSING_PSE");
      break;
  case PKCS10_REQUEST:
      strcat (rcStringBuffer, "PKCS10_REQUEST");
      break;
  case PKCS10_RESPONSE:
      strcat (rcStringBuffer, "PKCS10_RESPONSE");
      break;
  case BUF_TOO_SMALL:
      strcat (rcStringBuffer, "BUF_TOO_SMALL");
      break;
  case MISSING_FILE:
      strcat (rcStringBuffer, "MISSING_FILE");
      break;
  case CERTIFICATE:
      strcat (rcStringBuffer, "CERTIFICATE");
      break;
  case PKLIST:
      strcat (rcStringBuffer, "PKLIST");
      break;
#if defined (LINUX)|| (defined (_WIN32) && !defined(AMD64))
  case PKCS12_CONTAINER:
      strcat (rcStringBuffer, "PKCS12_CONTAINER");
      break;
  case FILE_ALREADY_EXISTS:
      strcat (rcStringBuffer, "FILE_ALREADY_EXISTS");
      break;
  case MISSING_CA_CERTS:
      strcat (rcStringBuffer, "MISSING_CA_CERTS");
      break;
  case CERT_VERIFY_FAILURE:
      strcat (rcStringBuffer, "CERT_VERIFY_FAILURE");
      break;
#endif
  default:
      sql47_itoa (rcSNC, rcStringBuffer + strlen (rcStringBuffer), 
                    sizeof (rcStringBuffer) - strlen (rcStringBuffer) - 1);

  }

  errText.rawAssign (functionName);

  ErrorTrailerLen = errText.size() - ErrorHeaderLen;
  if ( ErrorTrailerLen > 0 )
  {
      strncpy (errText.asCharp () + ErrorHeaderLen, rcStringBuffer, ErrorTrailerLen);
      memset (errText.asCharp () + errText.size(), '\0', 1);

/*
    strcpy (errText.asCharp () + ErrorHeaderLen, RCSTRING);
    sql47_itoa (rcSNC, errText.asCharp () + ErrorHeaderLen + strlen (RCSTRING), ErrorTrailerLen);
*/
  }
}


/*===========================================================================*/

static SAPDB_Bool
RTESec_SAPSSLInitLibraryNameU (SAPDB_Char *              pathSSLLibrary,
                              tsp00_ErrTextc &          errText )
{
    if (NULL != pathSSLLibraryU)
    {
        return true;
    }

    //Setting path and name for the underlying SSL librarry
    pathSSLLibraryU = (SAP_UC *) malloc ((strlen (pathSSLLibrary) + strlen (SSL_LIBRARY_NAME) + 1) 
                                        * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == pathSSLLibraryU)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_MEM_LIBRARY_PATH);
        return false;
    }

    A7sToUcs (pathSSLLibraryU, pathSSLLibrary);
    A7sToUcs (pathSSLLibraryU + strlenU (pathSSLLibraryU), SSL_LIBRARY_NAME);

    return true;
}

/*===========================================================================*/

static SAPDB_Bool
RTESec_SAPSSLInitSecudirNameU (SAPDB_Char *              pathSecurity,
                               tsp00_ErrTextc &          errText )
{
    if (NULL != pathSecurityU)
    {
        return true;
    }

    pathSecurityU = (SAP_UC *) malloc ((strlen (pathSecurity) + strlen (SECURITY_DIRECTORY) + 1) 
                                        * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == pathSecurityU)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_MEM_SECURITY_PATH);
        return false;
    }

    A7sToUcs (pathSecurityU, pathSecurity);
    A7sToUcs (pathSecurityU + strlenU (pathSecurityU), SECURITY_DIRECTORY);

    return true;
}

/*===========================================================================*/

static SAPDB_Bool
RTESec_SAPSSLInitPSENameU     (SAPDB_Char const * const   pPSEName,
                               tsp00_ErrTextc &           errText )
{
    if (NULL != pPSEFileNameU)
    {
        return true;
    }

    pPSEFileNameU = (SAP_UC *) malloc ((strlen (pPSEName) + 1) * LENGTH_OF_UNICODE_CHARACTER);
    if (NULL == pPSEFileNameU)
    {
        strcpy (errText.asCharp (), ERRMSG_SSL_MEM_CERTIFICATE_NAME);
        return false;
    }

    A7sToUcs (pPSEFileNameU, pPSEName);
    return true;
}


/*===========================================================================*/
#else /* STUB_MODULE */
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/
externC RTE_SharedLibHandle hSAPNI;

typedef int (*RTESec_SAPSSLIsInitialized_t)();
static RTESec_SAPSSLIsInitialized_t fpRTESec_SAPSSLIsInitialized = 0;

typedef tsp01_CommErr_Enum (*RTESec_SAPSSLInit_t)(SAPDB_Bool , SAPDB_Char *, SAPDB_Char *, tsp00_ErrTextc &);
static RTESec_SAPSSLInit_t fpRTESec_SAPSSLInit = 0;

typedef tsp01_CommErr_Enum (*RTESec_SAPSSLStartSession_t)(void *, SAPDB_Char *, SAPDB_Bool, void **, tsp00_ErrTextc &);
static RTESec_SAPSSLStartSession_t fpRTESec_SAPSSLStartSession = 0;

typedef tsp00_Uint (*RTESec_SAPSSLSend_t)(void *, char *, int, tsp00_ErrTextc &);
static RTESec_SAPSSLSend_t fpRTESec_SAPSSLSend = 0;

typedef tsp00_Uint (*RTESec_SAPSSLReceive_t)( void *, char *, tsp00_Uint, tsp00_Uint *, tsp00_ErrTextc &);
static RTESec_SAPSSLReceive_t fpRTESec_SAPSSLReceive = 0;

typedef void (*RTESec_SAPSSLSessionClose_t)(void *);
static RTESec_SAPSSLSessionClose_t fpRTESec_SAPSSLSessionClose = 0;

typedef void (*RTESec_SAPSSLClose_t)();
static RTESec_SAPSSLClose_t fpRTESec_SAPSSLClose = 0;

typedef int (*RTESec_SAPSSLUtilIsInitialized_t)();
static RTESec_SAPSSLUtilIsInitialized_t fpRTESec_SAPSSLUtilIsInitialized = 0;

typedef tsp01_CommErr_Enum (*RTESec_SAPSSLUtilInit_t)(SAPDB_Bool, SAPDB_Char *, SAPDB_Char *, tsp00_ErrTextc &);
static RTESec_SAPSSLUtilInit_t fpRTESec_SAPSSLUtilInit = 0;

typedef tsp01_CommErr_Enum (*RTESec_SAPSSLExportOwnCertificate_t)(SAPDB_Byte **, SAPDB_UInt4 &, tsp00_ErrTextc &);
static RTESec_SAPSSLExportOwnCertificate_t fpRTESec_SAPSSLExportOwnCertificate = 0;

typedef void (*RTESec_SAPSSLAuxFreeString_t)(SAPDB_Byte **);
static RTESec_SAPSSLAuxFreeString_t fpRTESec_SAPSSLAuxFreeString = 0;

typedef void (*RTESec_SAPSSLUtilClose_t)();
static RTESec_SAPSSLUtilClose_t fpRTESec_SAPSSLUtilClose = 0;

typedef tsp01_CommErr_Enum (*RTESec_SAPSSLWriteCertificateToFile_t)(SAPDB_Byte const *, SAPDB_UInt4, tsp00_ErrTextc &);
static RTESec_SAPSSLWriteCertificateToFile_t fpRTESec_SAPSSLWriteCertificateToFile = 0;

typedef tsp01_CommErr_Enum (*RTESec_SAPSSLImportCertificate_t)(SAPDB_Byte const *, tsp00_ErrTextc &);
static RTESec_SAPSSLImportCertificate_t fpRTESec_SAPSSLImportCertificate = 0;

typedef tsp01_CommErr_Enum (*RTESec_SAPSSLShowCertificate_t)(SAPDB_Byte const *, tsp00_ErrTextc &);
static RTESec_SAPSSLShowCertificate_t fpRTESec_SAPSSLShowCertificate = 0;


externC SAPDB_Bool RTESec_LoadSSLFunctions (tsp00_ErrTextc &    errText)
{
    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLIsInitialized",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLIsInitialized ))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLInit",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLInit ))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLStartSession",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLStartSession ))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLSend",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLSend ))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLReceive",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLReceive ))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLSessionClose",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLSessionClose ))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLClose",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLClose ))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLUtilIsInitialized",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLUtilIsInitialized ))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLUtilInit",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLUtilInit))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLExportOwnCertificate",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLExportOwnCertificate))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLAuxFreeString",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLAuxFreeString))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLUtilClose",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLUtilClose))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLWriteCertificateToFile",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLWriteCertificateToFile))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLImportCertificate",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLImportCertificate))
    {
        return false;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "RTESec_SAPSSLShowCertificate",
                                                 errText,
                                                 errText.size() + 1,
                                                 (RTE_SharedLibProcAddress &)fpRTESec_SAPSSLShowCertificate))
    {
        return false;
    }

    
    return true;
}

externC void RTESec_UnLoadSSLFunctions ()
{
    fpRTESec_SAPSSLIsInitialized = 0;
    fpRTESec_SAPSSLInit = 0;
    fpRTESec_SAPSSLStartSession = 0;
    fpRTESec_SAPSSLSend = 0;
    fpRTESec_SAPSSLReceive = 0;
    fpRTESec_SAPSSLSessionClose = 0;
    fpRTESec_SAPSSLClose = 0;
    fpRTESec_SAPSSLUtilIsInitialized = 0;
    fpRTESec_SAPSSLUtilInit = 0;
    fpRTESec_SAPSSLExportOwnCertificate = 0;
    fpRTESec_SAPSSLAuxFreeString = 0;
    fpRTESec_SAPSSLUtilClose = 0;
    fpRTESec_SAPSSLWriteCertificateToFile = 0;
    fpRTESec_SAPSSLImportCertificate = 0;
    fpRTESec_SAPSSLShowCertificate = 0;
}

externC int  RTESec_SAPSSLIsInitialized ()
{
    if (0 != fpRTESec_SAPSSLIsInitialized)
    {
        return fpRTESec_SAPSSLIsInitialized ();
    }
    else
    {
        return false;
    }
}

externC tsp01_CommErr_Enum  RTESec_SAPSSLInit (SAPDB_Bool         isClient,
                                               SAPDB_Char *       pathSSLLibrary,
                                               SAPDB_Char *       pathSecurity, 
                                               tsp00_ErrTextc &   errText)
{
    if (RTESec_SAPSSLIsInitialized ())
    {
        return commErrOk_esp01;
    }

    if (0 != fpRTESec_SAPSSLInit)
    {
        return (fpRTESec_SAPSSLInit (isClient, pathSSLLibrary, pathSecurity, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp01_CommErr_Enum  RTESec_SAPSSLStartSession (void *              pHandleNI,
                                                       SAPDB_Char *        pHostname,
                                                       SAPDB_Bool          ignoreHostnameInServerCert,
                                                       void **             ppHandleSSL,
                                                       tsp00_ErrTextc &    errText)
{
    if (0 != fpRTESec_SAPSSLStartSession)
    {
        return (fpRTESec_SAPSSLStartSession (pHandleNI, pHostname, ignoreHostnameInServerCert, ppHandleSSL, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp00_Uint          RTESec_SAPSSLSend (void *                        pHandleSend,
                                               char *                        pData,
                                               int                           length,
                                               tsp00_ErrTextc &              errText)
{
    if (0 != fpRTESec_SAPSSLSend)
    {
        return (fpRTESec_SAPSSLSend (pHandleSend, pData, length, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp00_Uint          RTESec_SAPSSLReceive (void *                        pHandleReceive,
                                                  char *                        pData,
                                                  tsp00_Uint                    bytesToReceive,
                                                  tsp00_Uint *                  bytesReceived,
                                                  tsp00_ErrTextc &              errText )
{
    if (0 != fpRTESec_SAPSSLReceive)
    {
        return (fpRTESec_SAPSSLReceive (pHandleReceive, pData, bytesToReceive, bytesReceived, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC void RTESec_SAPSSLSessionClose (void *  pHandleSSL)
{
    if (0 != fpRTESec_SAPSSLSessionClose)
    {
        fpRTESec_SAPSSLSessionClose (pHandleSSL);
    }

    return;
}

externC void  RTESec_SAPSSLClose ()
{
    if (0 != fpRTESec_SAPSSLClose)
    {
        fpRTESec_SAPSSLClose ();
    }

    return;
}

externC int  RTESec_SAPSSLUtilIsInitialized ()
{
    if (0 != fpRTESec_SAPSSLUtilIsInitialized)
    {
        return fpRTESec_SAPSSLUtilIsInitialized ();
    }
    else
    {
        return false;
    }
}

externC tsp01_CommErr_Enum  RTESec_SAPSSLUtilInit (SAPDB_Bool         isClient,
                                                   SAPDB_Char *       pathSSLLibrary,
                                                   SAPDB_Char *       pathSecurity,  
                                                   tsp00_ErrTextc &   errText)
{
    if (RTESec_SAPSSLUtilIsInitialized ())
    {
        return commErrOk_esp01;
    }

    if (0 != fpRTESec_SAPSSLUtilInit)
    {
        return (fpRTESec_SAPSSLUtilInit (isClient, pathSSLLibrary, pathSecurity, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp01_CommErr_Enum  RTESec_SAPSSLExportOwnCertificate (SAPDB_Byte **                  ppCertBuffer,
                                                               SAPDB_UInt4 &                   lenCert,
                                                               tsp00_ErrTextc &               errText )
{
    if (0 != fpRTESec_SAPSSLExportOwnCertificate)
    {
        return (fpRTESec_SAPSSLExportOwnCertificate (ppCertBuffer, lenCert, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC void  RTESec_SAPSSLAuxFreeString    (SAPDB_Byte **  ppBuf)
{
    if (0 != fpRTESec_SAPSSLAuxFreeString)
    {
        fpRTESec_SAPSSLAuxFreeString (ppBuf);
    }

    return;
}

externC void  RTESec_SAPSSLUtilClose    ()
{
    if (0 != fpRTESec_SAPSSLUtilClose)
    {
        fpRTESec_SAPSSLUtilClose ();
    }

    return;
}

externC tsp01_CommErr_Enum  RTESec_SAPSSLWriteCertificateToFile (SAPDB_Byte const *             pCertificate,
                                                                 SAPDB_UInt4                    bytesToWrite,
                                                                 tsp00_ErrTextc &               errText)
{
    if (0 != fpRTESec_SAPSSLWriteCertificateToFile)
    {
        return (fpRTESec_SAPSSLWriteCertificateToFile (pCertificate, bytesToWrite, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp01_CommErr_Enum  RTESec_SAPSSLImportCertificate (SAPDB_Byte const *             pCertificate,
                                                            tsp00_ErrTextc &               errText)
{
    if (0 != fpRTESec_SAPSSLImportCertificate)
    {
        return (fpRTESec_SAPSSLImportCertificate (pCertificate, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp01_CommErr_Enum  RTESec_SAPSSLShowCertificate (SAPDB_Byte const *             pCertificate,
                                                          tsp00_ErrTextc &               errText)
{
    if (0 != fpRTESec_SAPSSLShowCertificate)
    {
        return (fpRTESec_SAPSSLShowCertificate (pCertificate, errText));
    }
    else
    {
        eo46BuildErrorStringRC ( errText, (char *)ERRMSG_SSL_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

#endif /* STUB_MODULE */

