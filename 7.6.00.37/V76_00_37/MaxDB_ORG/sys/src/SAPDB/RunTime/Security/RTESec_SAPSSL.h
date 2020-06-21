/*!========================================================================

  @ingroup      Sec
  @file         RTESec_SAPSSL.h
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

  @since        2005-08-24  15:34
  @sa           


  =======================================================================

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
  ----------------------------------------------------------------------------*/


#ifndef RTESEC_SAPSSL_H
#define RTESEC_SAPSSL_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gsp01.h"
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(WIN32)
#  define SAPDB_DLL_EXTENSION ".dll"
#elif (defined HPUX && !defined HP_IA64)
#  define SAPDB_DLL_EXTENSION ".sl"
#else
#  define SAPDB_DLL_EXTENSION ".so"
#endif


#if defined (_WIN32) 
#define  SSL_LIBRARY_NAME		        "sapcrypto" SAPDB_DLL_EXTENSION
#elif defined _IBMR2
#define  SSL_LIBRARY_NAME		        "libsapcrypto.o"
#else
#define  SSL_LIBRARY_NAME		        "libsapcrypto" SAPDB_DLL_EXTENSION
#endif

#define  SERVER_PSE_FILE_NAME		    "SDBSSLS.pse"
#define  ANONYMOUS_PSE_FILE_NAME		"SDBSSLA.pse"

#define  RTE_SSL_LICENSE_FILE                   "ticket"
#define  SECURITY_DIRECTORY             "sec"

#define  CAPTURED_SERVER_CERT_FILENAME	"servercert.der"

#define MAX_CERTIFICATE_LENGTH          8192 /*Length of the Public Key*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/


/*!
    @brief      Unloading SSL Interface functions   
*/
externC void                RTESec_UnLoadSSLFunctions     ();

/*!
    @brief      Check if SAP SSL Interface is initialized 
    @return true if initalized, false else
*/
externC int                 RTESec_SAPSSLIsInitialized    ();

/*!
    @brief      Loading SSL Interface functions   

    @return     true/false  Loading of SSL functions succeeded/failed
*/
externC SAPDB_Bool          RTESec_LoadSSLFunctions      (tsp00_ErrTextc VAR_ARRAY_REF    errText);

/*!
    @brief      Initialize the SSL interface
                This function has only to be called once for a process. It has to be called after 
                the NI interface was initialized.

    @param      isClient [in]           - True: The caller is the client of the wanted SSL connection
    @param      pathSSLLibrary [in]     - Path to the SAP SSL library excluding the DLL name.
                                          The path has to be terminated with a delimiter.
    @param      pathSecurity [in]       - Path to the security directory (location of the certifates).
                                          The path has to be terminated with a delimiter.
    @param      errText  [out]          - Error text

    @return     commErrOk_esp01, commErrNotOk_esp01   
*/

externC tsp01_CommErr_Enum  RTESec_SAPSSLInit      (SAPDB_Bool                      isClient,
                                                    SAPDB_Char *                    pathSSLLibrary,
                                                    SAPDB_Char *                    pathSecurity,  
                                                    tsp00_ErrTextc VAR_ARRAY_REF    errText);
/*!
    @brief      Start a SSL session on an established NI connection
                Has to be called on the client side after the NI connect as well on the server side
                after the NI accept.
                Prerequisites:
                On the client and server side the SAP sapcrypto library
                has to be installed in the independent libary path. Also
                the server certificate SAPSSLS.pse and the anonymous client
                certificate SAPSSLA.pse have to be installed in the security
                path (<independant data path>/sec).

    @param      pHandleNI [in]          - Pointer to the NI handle
    @param      pHostname [in]          - Client: A normal Hostname or IP address (without port specification (:<port) 
                                                   or an SAP Router String of the remote side as it was used 
                                                   for the NI connect.
                                          Server: NULL (Not used)
    @param      ignoreHostnameInServerCert [in] - true: Ignore Hostname in Server certificate (not relevant on server side)
    @param      ppHandleSSL [out]       - Pointer to a pointer of the SSL handle
    @param      errText [out]           - Error text


    @return     commErrOk_esp01, commErrNotOk_esp01   
*/

externC tsp01_CommErr_Enum  RTESec_SAPSSLStartSession     (void *                          pHandleNI,
                                                           SAPDB_Char *                    pHostname,
                                                           SAPDB_Bool                      ignoreHostnameInServerCert,
                                                           void **                         ppHandleSSL,
                                                           tsp00_ErrTextc VAR_ARRAY_REF    errText);
/*!
    @brief      Send data over the SSL connection   

    @param      pHandleSend [in]        - Pointer to the SSL Handle
    @param      pData [in]              - Pointer to the data to be send
    @param      length [in]             - Data length
    @param      errText [out]           - Error text

    @return     commErrOk_esp01, commErrNotOk_esp01   
*/

externC tsp00_Uint          RTESec_SAPSSLSend      (void *                          pHandleSend,
                                                    char *                          pData,
                                                    int                             length,
                                                    tsp00_ErrTextc VAR_ARRAY_REF    errText);
/*!
    @brief      Receive data over the SSL connection   

    @param      pHandleReceive [in]     - Pointer to the SSL Handle
    @param      pData [out]             - Pointer to data buffer 
    @param      bytesToReceive [in]     - Bytes to be read
    @param      bytesReceived [out]     - Number of byted received
    @param      errText [out]           - Error text

    @return     commErrOk_esp01, commErrNotOk_esp01   
*/

externC tsp00_Uint          RTESec_SAPSSLReceive       (void *                          pHandleReceive,
                                                        char *                          pData,
                                                        tsp00_Uint                      bytesToReceive,
                                                        tsp00_Uint *                    bytesReceived,
                                                        tsp00_ErrTextc VAR_ARRAY_REF    errText );
/*!
    @brief       Close SAPSSL session

    @param       pHandleSSL [in]        -      
*/

externC void                RTESec_SAPSSLSessionClose  (void *                          pHandleSSL);

/*!
    @brief       Close SAPSSL
*/
externC void                RTESec_SAPSSLClose         ();

/*!
    @brief       Check if SAPSSL Util is initialized

    @return true or false
*/
externC int                 RTESec_SAPSSLUtilIsInitialized ();

/*!
    @brief       Initialize SAPSSL Utility
*/
externC tsp01_CommErr_Enum  RTESec_SAPSSLUtilInit      (SAPDB_Bool                      isClient,
                                                        SAPDB_Char *                    pathSSLLibrary,
                                                        SAPDB_Char *                    pathSecurity,  
                                                        tsp00_ErrTextc VAR_ARRAY_REF    errText);

/*!
    @brief       Export own certificate
*/
externC tsp01_CommErr_Enum  RTESec_SAPSSLExportOwnCertificate (SAPDB_Byte **                   ppCertBuffer,
                                                                SAPDB_UInt4 VAR_VALUE_REF       lenCert,
                                                                tsp00_ErrTextc VAR_ARRAY_REF    errText );

/*!
    @brief       Free an internal used string
*/
externC void                RTESec_SAPSSLAuxFreeString (SAPDB_Byte **                  ppBuf);

/*!
    @brief       Close SAP SSL utility
 */
externC void                RTESec_SAPSSLUtilClose          ();

/*externC void              RTESec_SAPSSLPrintCertificate   (SAPDB_Byte *                   pCertificate);*/

/*!
    @brief       Write certificates to file
*/
externC tsp01_CommErr_Enum  RTESec_SAPSSLWriteCertificateToFile (SAPDB_Byte const *             pCertificate, 
                                                                 SAPDB_UInt4                    bytesToWrite,
                                                                 tsp00_ErrTextc VAR_ARRAY_REF   errText );

/*!
    @brief       Import certificate
*/
externC tsp01_CommErr_Enum  RTESec_SAPSSLImportCertificate  (SAPDB_Byte const *             pCertificate, 
                                                             tsp00_ErrTextc VAR_ARRAY_REF   errText );

/*!
    @brief       Show certificate
*/
externC tsp01_CommErr_Enum  RTESec_SAPSSLShowCertificate    (SAPDB_Byte const *             pCertificate, 
                                                             tsp00_ErrTextc VAR_ARRAY_REF   errText );


#endif  /* RTESEC_SAPSSL_H */

