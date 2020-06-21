/*!========================================================================
  @ingroup      Sec
  @file         RTESec_SSL.cpp
  @author       StefanP

  @brief        

  @since        2006-02-15  15:51
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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "heo01.h"
#include "heo46.h"
#include "geo007_2.h"
#include "geo40.h"
#include "RunTime/Communication/RTEComm_ParseURI.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/Security/RTESec_SAPSSL.h"
#include "RunTime/Security/RTESec_SSL.h"
#include "SAPDBCommon/SAPDB_sprintf.h"

#if !defined (_WIN32)                   /*&if $OSSPEC!=WIN32*/
#include "RunTime/RTE_saveUNIXcalls.h"  /*nocheck*/
#endif                                  /*&endif*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define OPTION_SERVER_IGNORE    (SAPDB_UTF8*)"IgnoreHostnameInServerCert"

#if defined(_WIN32)
/*--- NT    */
#define     SEPERATOR                   "\\"
#else
/*--- UNIX  */
#define     SEPERATOR                   "/"
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/
#if defined (_WIN32)
externC HANDLE       hModule;
#endif

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/
static SAPDB_Bool RTESec_GetSAPSSLLibraryPath  (tsp00_Pathc    &   pathSAPSSL,
                                                 tsp00_ErrTextc &  errText);

static SAPDB_Bool RTESec_GetSAPSecurityPath (SAPDB_Bool         isClient,
                                             tsp00_Pathc & pathSecurity, 
                                             tsp00_ErrTextc &   errText);
static SAPDB_Bool RTESec_CheckLicenceFile   (RTE_Path            secPath, 
                                             tsp00_ErrTextc &    errText);

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC tsp01_CommErr_Enum  RTESec_IsSSLConnection (SAPDB_Char *         pszServerNode,
                                                    SAPDB_Bool &         isSSL,
                                                    SAPDB_Bool &         isNISSL,
                                                    SAPDB_Bool &         ignoreHostnameInServerCert,
                                                    tsp00_ErrTextc &     errText)
{
    SAPDBErr_MessageList        messageList;
    RTEComm_ParseURI            uri;
    RTEComm_URIUtils::URIRet    rcURI = RTEComm_URIUtils::NoError;
//    SAPDB_Char *                tmpConnectURI;

/*
    tmpConnectURI = (SAPDB_Char *) alloca (strlen (pszServerNode) + strlen (URI_DATABASE_PATH) + strlen (pszServerDB) + 1);
    if (NULL == tmpConnectURI)
    {
        eo46BuildErrorStringRC ( errText, (char *)(ERRMSG_COM_NO_MORE_MEMORY_FOR_URI), 0 );
        return commErrNotOk_esp01;
    }

    strcpy (tmpConnectURI, pszServerNode);
    strcat (tmpConnectURI, URI_DATABASE_PATH);
    strcat (tmpConnectURI, pszServerDB);
*/
    errText.Init();
    ignoreHostnameInServerCert = false;
    isSSL = false;
    isNISSL = false;
    rcURI = uri.Parse (pszServerNode, messageList);
    //Error may be ignored here
    if (RTEComm_URIUtils::NoError == rcURI)
    {
        if (isNISSL = uri.IsNISSLLocation ())
        {
            isSSL = true;
            //This string copy is ok. because the resulting string is shorter than the original string
            strcpy (pszServerNode, (SAPDB_Char *)uri.NILocation ()->SAPRouter ());
        }
        else if (isSSL = uri.IsSSLLocation ())
        {
            //This string copy is ok. because the resulting string is shorter than the original string
            strcpy (pszServerNode, (SAPDB_Char *)uri.IPLocation ()->Hostname ());
            if (NULL != uri.IPLocation ()->PortNo ())
            {
                strcat (pszServerNode, ":");
                strcat (pszServerNode, (SAPDB_Char *)uri.IPLocation ()->PortNo ());
            }
        }

        if (isSSL )
        {

#if SDB_LC_STATE == DEV
            //Makes testing more pleasant on DEV
            ignoreHostnameInServerCert = true;
#else
            SAPDB_UTF8 const *       pTypeString = NULL;

            if (uri.FindOption (OPTION_SERVER_IGNORE, pTypeString))
            {
                if (NULL == pTypeString ||
                    0 == strcmp (reinterpret_cast <SAPDB_Char const *> (pTypeString), 
                                 "1"))
                {
                    ignoreHostnameInServerCert = true;
                }
            }
#endif
        }
    }

    return commErrOk_esp01;
}

/*------------------------------*/

externC tsp01_CommErr_Enum  RTESec_SSLInit (tsp00_Pathc &       niTraceFile, 
                                            tsp00_ErrTextc &    errText)
{
    tsp00_Pathc         pathSAPSSL;
    tsp00_Pathc         pathSecurity;
    tsp01_CommErr_Enum  commError = commErrOk_esp01;
    SAPDB_Bool          isClient = true;


    errText.Init();

    commError = (tsp01_CommErr_Enum)eo40NiInit(niTraceFile, 0, errText);
    if (commErrOk_esp01 != commError)
    {
        tsp00_ErrTextc  tmpErrText;
        eo40NiFinish (tmpErrText);
        return commError;
    }

    pathSAPSSL.Init ();
    pathSecurity.Init ();

    if (RTESec_GetSAPSSLLibraryPath (pathSAPSSL, errText))
    {
#if defined(SERVER)
        isClient = false;
#endif
        if (RTESec_GetSAPSecurityPath (isClient, pathSecurity, errText))
        {
            if (!RTESec_CheckLicenceFile (pathSecurity.asCharp (), errText))
            {
                commError = commErrNotOk_esp01;
            }

            if (commErrOk_esp01 == commError)
            {
                commError = RTESec_SAPSSLInit(isClient, pathSAPSSL, pathSecurity, errText);
                if (commErrOk_esp01 != commError)
                {
                    RTESec_SAPSSLClose ();
                }
            }
        }
        else
        {
            commError = commErrNotOk_esp01;
        }
    }
    else
    {
        commError = commErrNotOk_esp01;
    }

    return commError;
}

/*------------------------------*/

externC tsp01_CommErr_Enum  RTESec_SSLUtilInit (tsp00_Pathc &       niTraceFile, 
                                                tsp00_ErrTextc &    errText)
{
    tsp00_Pathc         pathSAPSSL;
    tsp00_Pathc         pathSecurity;
    tsp01_CommErr_Enum  commError = commErrOk_esp01;
    SAPDB_Bool          isClient = true;


    errText.Init();

    commError = (tsp01_CommErr_Enum)eo40NiInit(niTraceFile, 0, errText);
    if (commErrOk_esp01 != commError)
    {
        tsp00_ErrTextc  tmpErrText;
        eo40NiFinish (tmpErrText);
        return commError;
    }

    pathSAPSSL.Init ();
    pathSecurity.Init ();

    if (RTESec_GetSAPSSLLibraryPath (pathSAPSSL, errText))
    {
#if defined(SERVER)
        isClient = false;
#endif
        if (RTESec_GetSAPSecurityPath (isClient, pathSecurity, errText))
        {
            commError = RTESec_SAPSSLUtilInit(isClient, pathSAPSSL, pathSecurity, errText);
            if (commErrOk_esp01 != commError)
            {
                RTESec_SAPSSLUtilClose ();
            }
        }
        else
        {
            commError = commErrNotOk_esp01;
        }
    }
    else
    {
        commError = commErrNotOk_esp01;
    }

    return commError;
}

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static SAPDB_Bool RTESec_GetSAPSSLLibraryPath (tsp00_Pathc &        pathSAPSSL,
                                                tsp00_ErrTextc &     errText) 
{
    tsp01_RteError          rteError;

    eo46_rte_error_init ( &rteError ) ;
    errText.Init();

//Setting path for the underlying SSL librarry

//On Windows try first the path of the executable or the loaded module (DLL) respectively
#if defined (_WIN32)
    DWORD                   dwLen;

    if ( hModule != INVALID_HANDLE_VALUE )
    {
        dwLen = GetModuleFileName((HMODULE)hModule, pathSAPSSL, sizeof(pathSAPSSL) - 1);
    }
    else
    {
        dwLen = GetModuleFileName(NULL, pathSAPSSL, sizeof(pathSAPSSL) - 1);
    }

    if (dwLen >= 0) 
    {
        char *Ptr;

        Ptr = strrchr ((char *) pathSAPSSL, '\\');
        if (Ptr != 0) 
        {
            
            Ptr++;
            *Ptr = '\0';
            if (sizeof (pathSAPSSL) >  strlen (pathSAPSSL) + strlen (SSL_LIBRARY_NAME))
            {
                tsp00_Pathc tmpPathSSL;
                strcpy (tmpPathSSL, pathSAPSSL);
                strcat (tmpPathSSL, SSL_LIBRARY_NAME );
            
                WIN32_FIND_DATA     fileData;
                if (INVALID_HANDLE_VALUE != FindFirstFile ( tmpPathSSL, &fileData))
                {
                    return (true);
                }
            }
        }
    }
#endif

    if (!sqlGetIndependentLibPath(pathSAPSSL, TERM_WITH_DELIMITER_EO01, &rteError)) 
    {
/*
        SAPDB_Char   tmpBuffer[20] = "";
        //Converting tsp01_RteError to tsp00_ErrTextc
        if (0 < rteError.OsErrCode)
        {
            SAPDB_sprintf (tmpBuffer, sizeof (tmpBuffer), " , rc = %d", rteError.OsErrCode);
        }

        strncpy (errText.asCharp (), rteError.RteErrText, errText.size() - strlen (tmpBuffer));
        strcat (errText.asCharp (), tmpBuffer);
*/
        eo46BuildErrorStringRC ( errText, (char *)(ERRMSG_SSL_LIBRARY_NOT_FOUND), 0 );
        return false;
    }

    return true;
}

/*------------------------------*/

static SAPDB_Bool RTESec_GetSAPSecurityPath (SAPDB_Bool         isClient,
                                             tsp00_Pathc &      pathSecurity,
                                             tsp00_ErrTextc &   errText)
{
    tsp01_RteError          rteError;

    eo46_rte_error_init ( &rteError ) ;
    errText.Init();

#if defined (_WIN32)
    if (isClient)
    {
//On Windows try first the path of the executable or the loaded module (DLL) respectively
        DWORD                   dwLen;

        if ( hModule != INVALID_HANDLE_VALUE )
        {
            dwLen = GetModuleFileName((HMODULE)hModule, pathSecurity, sizeof(pathSecurity) - 1);
        }
        else
        {
            dwLen = GetModuleFileName(NULL, pathSecurity, sizeof(pathSecurity) - 1);
        }

        if (dwLen >= 0) 
        {
            char *Ptr;

            Ptr = strrchr ((char *) pathSecurity, '\\');
            if (Ptr != 0) 
            {

                Ptr++;
                *Ptr = '\0';
                if (sizeof (pathSecurity) >  strlen (pathSecurity) + strlen (SECURITY_DIRECTORY) + 1 + strlen (ANONYMOUS_PSE_FILE_NAME))
                {
                    tsp00_Pathc tmpPathSSL;
                    strcpy (tmpPathSSL, pathSecurity);
                    strcat (tmpPathSSL, SECURITY_DIRECTORY "\\");
                    strcat (tmpPathSSL, ANONYMOUS_PSE_FILE_NAME );

                    WIN32_FIND_DATA     fileData;
                    if (INVALID_HANDLE_VALUE != FindFirstFile ( tmpPathSSL, &fileData))
                    {
                        return (true);
                    }
                }
            }
        }
    }
#endif

    if (!sqlGetIndependentDataPath(pathSecurity, TERM_WITH_DELIMITER_EO01, &rteError))
    {
/*
        SAPDB_Char   tmpBuffer[20] = "";
        //Converting tsp01_RteError to tsp00_ErrTextc
        if (0 < rteError.OsErrCode)
        {
            SAPDB_sprintf (tmpBuffer, sizeof (tmpBuffer), " , rc = %d", rteError.OsErrCode);
        }

        strncpy (errText.asCharp (), rteError.RteErrText, errText.size() - strlen (tmpBuffer));
        strcat (errText.asCharp (), tmpBuffer);
*/
        eo46BuildErrorStringRC ( errText, (char *)(ERRMSG_SSL_SECURITY_PATH_NOT_FOUND), 0 );
        return false;
    }

    return true;
}

/*------------------------------*/

static SAPDB_Bool RTESec_CheckLicenceFile (RTE_Path            secPath, 
                                           tsp00_ErrTextc &    errText)
{
    RTE_FileHandle      handle;
    SAPDB_Char *        filePath;
    SAPDB_UInt4         lengthFilePath;

    lengthFilePath = strlen (secPath) + strlen (SECURITY_DIRECTORY) + strlen (SEPERATOR) + strlen (RTE_SSL_LICENSE_FILE) + 1;
    filePath = (SAPDB_Char *) alloca (lengthFilePath);
    if (NULL == filePath)
    {
        eo46BuildErrorStringRC ( errText, (char *)(ERRMSG_SSL_MEM_LICENCE_NAME), 0 );
        return false;
    }

    memset (filePath, '\0', lengthFilePath);
    strcpy (filePath, secPath);
    strcat (filePath, SECURITY_DIRECTORY SEPERATOR RTE_SSL_LICENSE_FILE);

#if defined (_WIN32)
    handle = CreateFile (filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
    handle = RTE_save_open (filePath, O_RDONLY);
#endif
    if (RTE_UNDEF_HANDLE == handle) 
    {
        eo46BuildErrorStringRC ( errText, (char *)(ERRMSG_SSL_LICENCE_FILE_NOT_FOUND), 0 );
        return false;
    }

#if defined (_WIN32)
    CloseHandle (handle);
#else
    RTE_save_close (handle);
#endif

    return true;
}
