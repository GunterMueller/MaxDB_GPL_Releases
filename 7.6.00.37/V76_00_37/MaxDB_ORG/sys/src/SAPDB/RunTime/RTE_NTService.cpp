/*!---------------------------------------------------------------------
  @file           RTE_NTService.cpp
  @author         RobinW, RaymondR
  @brief          Routine to create, change and delete an entry for a
                  database in the Windows service registry
  @see

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo002.h"
#include "heo11.h"
#include "geo007_1.h"
#include "gos44.h"  // for sql44c_get_file_info()
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_SpeedInfo.hpp"
#include "RunTime/RTE_DBRegister.hpp"
#include "heo01.h" // for eo01_IsEnvVarDoNotUseIndepPathSet()
#include "hsp100.h"// for sp100_CompareVersionIDs()

#include "RunTime/RTE_NTService.hpp"

#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"


#if defined CLUSTER_SERVER_VERSION
#  include          "gos212.h"   /* nocheck */
#endif


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

struct SERVERDB_PARAM_REC
{
    BOOL                  fTapeCompressEnabled;
    ULONG                 ulWaitSec;
    ULONG                 ulTermTimeout;
    DWORD                 ValUseIndepPath;
    DWORD                 AWEReservedMemory;
    PATHNAME              szDBRoot;
    PATHNAME              szAEDebug;
    tsp100_VersionID0     RTEVersionID;
    BOOL                  fAlwaysCheckConn;
};

/*------------------------------*/

SAPDB_Char *RTE_NTService::versionFromParam ( SAPDB_Char  *KernelVersion )
{

    DBGPAS;

    // --- display no error messages
    sql60_disable_message_output ();

    RTEConf_Parameter       config(m_dbName,0,RTECONF_MAXNAMELENGTH,RTECONF_MAXSTRINGLENGTH);
    SAPDB_Bool              fileExist;
    SAPDBErr_MessageList    errList;
    SAPDB_Bool              returnValue;
    if(returnValue = config.Read(fileExist, errList))
    {
        if(!config.GetValue((RTEConf_Parameter::Name) PAN_PARAM_KERNELVERSION, (RTEConf_Parameter::String)KernelVersion, errList))
            returnValue = false;
    }
    sql60_enable_message_output();

    return (returnValue ? KernelVersion : NULL);
}

/*------------------------------*/

APIRET RTE_NTService::getAccountName ( SAPDB_Char * pszAccountName,SAPDB_Int ulMaxAccountNameLen )
{
    APIRET              rc  = NO_ERROR;
    PATHNAME            szKey;
    REG_ENTRY_REC       RegistryEntries[1];

    DBGIN;

    pszAccountName[0] ='\0';

    strcpy ( szKey, m_serviceName );

    RegistryEntries[0].pszValueName = REG_VN_OBJECT_NAME;
    RegistryEntries[0].pValue       = pszAccountName;
    RegistryEntries[0].ulValueSize  = ulMaxAccountNameLen;
    RegistryEntries[0].ulValueType  = REG_SZ;

    rc = sql50_reg_get_service_values ( m_NodeName, szKey,
                                        1, RegistryEntries );



    DBGOUT;
    return ( rc );
}

/*------------------------------*/

APIRET RTE_NTService::readParameters ( SERVERDB_PARAM_REC *Params )
{
    APIRET              rc  = NO_ERROR;
    PATHNAME            szKey;
    REG_ENTRY_REC       RegistryEntries[8];
    ULONG               ulValues  = 0;


    DBGIN;

    strcpy ( szKey, m_serviceName );
    strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );

    RegistryEntries[ulValues].pszValueName = REG_VN_DBROOT;
    RegistryEntries[ulValues].pValue       = Params->szDBRoot;
    RegistryEntries[ulValues].ulValueSize  = sizeof(Params->szDBRoot);
    RegistryEntries[ulValues].ulValueType  = REG_SZ;
    ulValues++;

    RegistryEntries[ulValues].pszValueName = REG_VN_VERSION_ID;
    RegistryEntries[ulValues].pValue       = &(Params->RTEVersionID);
    RegistryEntries[ulValues].ulValueSize  = sizeof(Params->RTEVersionID);
    RegistryEntries[ulValues].ulValueType  = REG_BINARY;
    ulValues++;

    RegistryEntries[ulValues].pszValueName = REG_VN_TAPE_COMPRESS;
    RegistryEntries[ulValues].pValue       = &Params->fTapeCompressEnabled;
    RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
    RegistryEntries[ulValues].ulValueType  = REG_DWORD;
    ulValues++;

    RegistryEntries[ulValues].pszValueName = REG_VN_TAPE_LOAD_RETRY_TIME;
    RegistryEntries[ulValues].pValue       = &Params->ulWaitSec;
    RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
    RegistryEntries[ulValues].ulValueType  = REG_DWORD;
    ulValues++;

    RegistryEntries[ulValues].pszValueName = REG_VN_TERMINATION_TIMEOUT;
    RegistryEntries[ulValues].pValue       = &Params->ulTermTimeout;
    RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
    RegistryEntries[ulValues].ulValueType  = REG_DWORD;
    ulValues++;

    RegistryEntries[ulValues].pszValueName = REG_VN_AEDEBUG;
    RegistryEntries[ulValues].pValue       = Params->szAEDebug;
    RegistryEntries[ulValues].ulValueSize  = sizeof(Params->szAEDebug);
    RegistryEntries[ulValues].ulValueType  = REG_SZ;
    ulValues++;

    RegistryEntries[ulValues].pszValueName = REG_VN_ALWAYS_CHECK_CONN;
    RegistryEntries[ulValues].pValue       = &Params->fAlwaysCheckConn;
    RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
    RegistryEntries[ulValues].ulValueType  = REG_DWORD;
    ulValues++;

    // --- check array size
    SAPDBERR_ASSERT_ARGUMENT( sizeof (RegistryEntries)/sizeof(RegistryEntries[0]) >= ulValues );

    rc = sql50_reg_get_service_values ( m_NodeName, szKey,
                                      ulValues, RegistryEntries );


    // --- Optional values. Ignoring return code from 'sql50_reg_get_service_values'
    ulValues = 0;
    RegistryEntries[ulValues].pszValueName = REG_VN_USE_INDEP_PATH;
    RegistryEntries[ulValues].pValue       = &Params->ValUseIndepPath;
    RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
    RegistryEntries[ulValues].ulValueType  = REG_DWORD;
    ulValues++;

    RegistryEntries[ulValues].pszValueName = REG_VN_AWE_RESERVED_MEMORY;
    RegistryEntries[ulValues].pValue       = &Params->AWEReservedMemory;
    RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
    RegistryEntries[ulValues].ulValueType  = REG_DWORD;
    ulValues++;

    sql50_reg_get_service_values ( m_NodeName, szKey, ulValues, RegistryEntries );

    DBGOUT;
    return ( rc );
}

/*------------------------------*/

APIRET RTE_NTService::alterParameter ( SERVERDB_PARAM_REC *Params,SAPDB_Char *pszParameter,SAPDB_Char *pszValue )
{
    APIRET              rc  = NO_ERROR;
    SAPDB_Long          lValue = 0;
    SAPDB_Char          szValue[30];

    DBGIN;
    // --- set current DBROOT
    strcpy ( Params->szDBRoot, m_dbRoot);

    if ( pszParameter == NULL )
    {
        DBGOUT;
        return ( NO_ERROR );
    }
    // --- skip blanks
    for ( ;; pszValue++ )
    {
        if ( *pszValue != ' ' )
            break;
    }

    if ( !stricmp ( REG_VN_AEDEBUG, pszParameter ) )                   // --- Debugger
    {
        if ( sizeof(Params->szAEDebug) <= strlen (pszValue) )
        {
            rc = ERROR_INVALID_PARAMETER;
            MSGCD ((ERR_VALUE_TOO_LONG, pszParameter ));
        }
        else
        {
            strcpy ( Params->szAEDebug, pszValue );
        }
    }
    else if ( !stricmp ( REG_VN_TAPE_COMPRESS, pszParameter ) )        // --- TapeCompression
    {
        lValue = atol( pszValue );

        if ( stricmp ( pszValue, _ltoa ( (LONG)(lValue & 0x1), szValue, 10)) )
        {
            rc = ERROR_INVALID_PARAMETER;
            MSGCD ((ERR_INVALID_VALUE, pszParameter, pszValue ));
        }
        else
        {
            Params->fTapeCompressEnabled = (BOOL)lValue;
        }
    }
    else if ( !stricmp ( REG_VN_ALWAYS_CHECK_CONN, pszParameter ) )   // --- AlwaysCheckConn
    {
        lValue = atol( pszValue );

        if ( stricmp ( pszValue, _ltoa ( (LONG)(lValue & 0x1), szValue, 10)) )
        {
            rc = ERROR_INVALID_PARAMETER;
            MSGCD ((ERR_INVALID_VALUE, pszParameter, pszValue ));
        }
        else
        {
            Params->fAlwaysCheckConn = (BOOL)lValue;
        }
    }
    else if ( !stricmp ( REG_VN_TERMINATION_TIMEOUT, pszParameter ) )      // --- TerminationTimeout
    {
        lValue = atol( pszValue );

        if ( stricmp ( pszValue, _ltoa ( (LONG)(ULONG)lValue, szValue, 10)) )
        {
            rc = ERROR_INVALID_PARAMETER;
            MSGCD ((ERR_INVALID_VALUE, pszParameter, pszValue ));
        }
        else
        {
            Params->ulTermTimeout = (ULONG)lValue;
        }
    }
    else if ( !stricmp ( REG_VN_TAPE_LOAD_RETRY_TIME, pszParameter ) ) // --- TWTermTimeout
    {
        lValue = atol( pszValue );

        if ( stricmp ( pszValue, _ltoa ( (LONG)(ULONG)lValue, szValue, 10)) )
        {
            rc = ERROR_INVALID_PARAMETER;
            MSGCD ((ERR_INVALID_VALUE, pszParameter, pszValue ));
        }
        else
        {
            Params->ulWaitSec = (ULONG)lValue;
        }
    }
    else if ( !stricmp ( REG_VN_AWE_RESERVED_MEMORY, pszParameter ) ) // --- AWEReservedPhysMemory
    {
        lValue = atol( pszValue );

        if ( stricmp ( pszValue, _ltoa ( (LONG)(ULONG)lValue, szValue, 10)) )
        {
            rc = ERROR_INVALID_PARAMETER;
            MSGCD ((ERR_INVALID_VALUE, pszParameter, pszValue ));
        }
        else
        {
            Params->AWEReservedMemory = (ULONG)lValue;
        }
    }
    else
    {
        MSGCD ((ERR_INVALID_PARAMETER, pszParameter ));
        rc = ERROR_INVALID_PARAMETER;
    }
    DBGOUT;
    return ( rc );
}

/*------------------------------*/

APIRET RTE_NTService::updateParameters( SERVERDB_PARAM_REC *Params )
{
    APIRET              rc  = NO_ERROR;
    PATHNAME            szKey;
    REG_ENTRY_REC       RegistryEntries[12];
    SAPDB_ULong         ValUseIndepPath;
    SAPDB_UInt          values  = 0;
    tsp100_VersionID0   RTEVersionID;
    SAPDB_Char          szEventSource[40];
    PATHNAME            szMsgFile;
    PATHNAME            szCategoryFile;
    tsp00_Versionc      RTEVersionString;

    DBGIN;

    strcpy ( szKey, m_serviceName );
    strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );

    // --- set the RTE version string
    if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
        sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, RTEVersionString);
    else
        sp100_GetVersionString(COMP_NAME_RTE_WIN9X_SP100, s100buildnumber, RTEVersionString );

    RegistryEntries[values].pszValueName = REG_VN_VERSION;
    RegistryEntries[values].pValue       = RTEVersionString;
    RegistryEntries[values].ulValueSize  = (ULONG)strlen(RTEVersionString);
    RegistryEntries[values].ulValueType  = REG_SZ;
    values++;

    sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &RTEVersionID );
    RegistryEntries[values].pszValueName = REG_VN_VERSION_ID;
    RegistryEntries[values].pValue       = &RTEVersionID;
    RegistryEntries[values].ulValueSize  = sizeof(RTEVersionID);
    RegistryEntries[values].ulValueType  = REG_BINARY;
    values++;

    RegistryEntries[values].pszValueName = REG_VN_DBROOT;
    RegistryEntries[values].pValue       = Params->szDBRoot;
    RegistryEntries[values].ulValueSize  = (ULONG)strlen(Params->szDBRoot);
    RegistryEntries[values].ulValueType  = REG_SZ;
    values++;

    RegistryEntries[values].pszValueName = REG_VN_TAPE_COMPRESS;
    RegistryEntries[values].pValue       = &Params->fTapeCompressEnabled;
    RegistryEntries[values].ulValueSize  = sizeof(DWORD);
    RegistryEntries[values].ulValueType  = REG_DWORD;
    values++;

    RegistryEntries[values].pszValueName = REG_VN_TAPE_LOAD_RETRY_TIME;
    RegistryEntries[values].pValue       = &Params->ulWaitSec;
    RegistryEntries[values].ulValueSize  = sizeof(DWORD);
    RegistryEntries[values].ulValueType  = REG_DWORD;
    values++;

    RegistryEntries[values].pszValueName = REG_VN_TERMINATION_TIMEOUT;
    RegistryEntries[values].pValue       = &Params->ulTermTimeout;
    RegistryEntries[values].ulValueSize  = sizeof(DWORD);
    RegistryEntries[values].ulValueType  = REG_DWORD;
    values++;

    RegistryEntries[values].pszValueName = REG_VN_AEDEBUG;
    RegistryEntries[values].pValue       = Params->szAEDebug;
    RegistryEntries[values].ulValueSize  = (ULONG)strlen(Params->szAEDebug);
    RegistryEntries[values].ulValueType  = REG_SZ;
    values++;

    RegistryEntries[values].pszValueName = REG_VN_ALWAYS_CHECK_CONN;
    RegistryEntries[values].pValue       = &Params->fAlwaysCheckConn;
    RegistryEntries[values].ulValueSize  = sizeof(DWORD);
    RegistryEntries[values].ulValueType  = REG_DWORD;
    values++;

    ValUseIndepPath = eo01_IsEnvVarDoNotUseIndepPathSet() ? 0 : 1 ;
    RegistryEntries[values].pszValueName = REG_VN_USE_INDEP_PATH;
    RegistryEntries[values].pValue       = &ValUseIndepPath;
    RegistryEntries[values].ulValueSize  = sizeof(DWORD);
    RegistryEntries[values].ulValueType  = REG_DWORD;
    values++;

    if ( Params->AWEReservedMemory != 0 )
    {
        RegistryEntries[values].pszValueName = REG_VN_AWE_RESERVED_MEMORY;
        RegistryEntries[values].pValue       = &Params->AWEReservedMemory;
        RegistryEntries[values].ulValueSize  = sizeof(DWORD);
        RegistryEntries[values].ulValueType  = REG_DWORD;
        values++;
    }

    SAPDB_Char *        pszKernelVersion = NULL;
    SAPDB_Char KernelVersion[RTECONF_MAXSTRINGLENGTH];
    pszKernelVersion = versionFromParam ( &KernelVersion[0] );

    if ( pszKernelVersion != NULL )
    {
        RegistryEntries[values].pszValueName = REG_VN_KERNEL_VERSION;
        RegistryEntries[values].pValue       = pszKernelVersion;
        RegistryEntries[values].ulValueSize  = (ULONG)strlen(pszKernelVersion);
        RegistryEntries[values].ulValueType  = REG_SZ;
        values++;
    }
    else
    {
        sql50_reg_del_service_value ( m_NodeName, szKey, REG_VN_KERNEL_VERSION );
    }

    // --- check array size
    SAPDBERR_ASSERT_ARGUMENT( sizeof (RegistryEntries)/sizeof(RegistryEntries[0]) >= values );

    rc = sql50_reg_put_service_values ( m_NodeName, szKey, values,
                                        RegistryEntries );

    if ( rc == NO_ERROR )
    {
        strcpy ( szEventSource, SERVERDB_EVENT_SOURCE_NAME );
        strcat( szEventSource, m_dbName );

        szMsgFile[0] = '\0';
        sql01c_build_pgm_path ( Params->szDBRoot, szMsgFile ) ;
        strcat ( szMsgFile, REG_MSG_FILE );

        szCategoryFile[0] = '\0';
        strcpy ( szCategoryFile,  Params->szDBRoot );
        strcat ( szCategoryFile, "\\" );
        strcat ( szCategoryFile, DB_FAST_KERN_FILE_NAME );


        rc = sql60_install_event_source ( m_NodeName,
                                          szEventSource,
                                          szMsgFile,
                                          szCategoryFile,
                                          NTKERN_CATEGORY_LAST);
    }

    DBGOUT;
    return ( rc );
}

/*------------------------------*/

/*------------------------------*/

SAPDB_Bool RTE_NTService::makeErrListFromApiRet(APIRET rc,SAPDBErr_MessageList &errList)
{
    switch (rc)
    {
    case ERROR_INVALID_PARAMETER:
        break;
    case NO_ERROR:
        return true;
    case ERROR_SERVICE_DATABASE_LOCKED:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVICE_DB_LOCKED);
        break;
    case ERROR_INVALID_SERVICE_ACCOUNT:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVICE_ACCOUNT_INVALID );
        break;
    case ERROR_SERVICE_EXISTS:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_DBREG_SERVICE_ALREADY_INST, m_serviceDisplayString);
        break;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_DBREG_SERVICE_NOT_INST_CORR);
        break;
    case ERROR_SERVICE_DOES_NOT_EXIST:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_DBREG_SERVICE_NOT_INSTALLED, m_serviceDisplayString);
        break;
    case ERROR_SERVICE_MARKED_FOR_DELETE:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_DBREG_SERVICE_DELETE_MARKED);
        break;
    case RPC_S_SERVER_UNAVAILABLE:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVER_NODE_UNKNOWN, m_NodeName);
        break;
    case ERROR_ACCESS_DENIED:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CREAT_SERVICE_NO_ACCESS);
        break;
    default:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVICE_ERROR, SAPDB_ToString((SAPDB_ULong)rc));
    }
    return false;
}

/*------------------------------*/

SAPDB_Bool RTE_NTService::UpdateServiceEntry (  
                                          RTE_StartOption        Start,
                                          SAPDB_Char             *pszAccountName,
                                          SAPDB_Char             *pszPassword,
                                          SAPDB_Char             *pszParameter,
                                          SAPDB_Char             *pszValue,
                                          SAPDBErr_MessageList   &errList)
{
    SAPDB_Bool            serviceEntryAlreadyExists = false;
    SC_HANDLE             hScM, hS;
    SC_LOCK               ScLock      = NULL;
    APIRET                rc          = NO_ERROR;
    PSECURITY_DESCRIPTOR  pSD         = NULL;
    SERVERDB_PARAM_REC    Params      = { TRUE,
                                          DEFAULT_TAPE_LOAD_RETRY_TIME,
                                          DEFAULT_TERMINATION_TIMEOUT,
                                          -1,
                                          0,
                                          "",
                                          DR_WATSON_EXENAME,
                                          FALSE };

    DBGIN;


    SAPDB_UInt            ulTmp       = 0;
    SAPDB_Int             lAtt        = 0;
    rc = sql44c_get_file_info ( m_executableNameWithPath, &lAtt, &ulTmp, &ulTmp );

    if ( rc == ERROR_ACCESS_DENIED )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_ACCESS_DENIED_ON_XX, m_executableNameWithPath);
        return false;
    }
    else
    {
        rc = NO_ERROR;
    }

    if (hScM = OpenSCManager( m_NodeName, NULL, SC_MANAGER_ALL_ACCESS ))
    {
        SAPDB_Char            szAccountName[80];
        if ((pszAccountName != (SAPDB_Char *)RTE_SERVICE_ENTRY_UNCHANGED) && (pszAccountName != NULL ) && (strrchr ( pszAccountName, '\\' ) == NULL))
        {
            szAccountName[sizeof(szAccountName) - 1];
            strcpy (szAccountName, ".\\");
            strncat(szAccountName, pszAccountName, sizeof(szAccountName) - 3 );
            pszAccountName = szAccountName;
        }

        //
        // --- does the service exist?
        //
        if (hS = OpenService(hScM, m_serviceName, SERVICE_START))
        {
            CloseServiceHandle(hS);
            serviceEntryAlreadyExists = true;
        }

        DWORD serviceType;
        DWORD startType;

        if(serviceEntryAlreadyExists)
        {
            startType = SERVICE_NO_CHANGE;
            if(RTE_StartUnchanged != Start)
            {
                SAPDB_Bool currentAutoStartFlag;
                GetAutoStartFlag(currentAutoStartFlag);
                if(currentAutoStartFlag)
                {
                    if(RTE_StartOnDemand == Start)
                    {
                        startType = SERVICE_DEMAND_START;
                    }
                }
                else
                {
                    if(RTE_StartAutomatic == Start)
                    {
                        startType = SERVICE_AUTO_START;
                    }
                }
            }
            SAPDB_Char szCurrAccountName[80];
            serviceType = SERVICE_NO_CHANGE;
            if((SAPDB_Char *)RTE_SERVICE_ENTRY_UNCHANGED == pszAccountName)
            {
                pszAccountName = NULL;
                pszPassword    = NULL;
            }
            else
            {
                getAccountName ( szCurrAccountName,sizeof(szCurrAccountName) );
                if (( pszAccountName == NULL ) || ( pszAccountName[0] == '\0' ))
                {
                    if(0 != stricmp("LocalSystem",szCurrAccountName))
                    {
                        serviceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
                    }
                }
                else
                {
                    if(0 != stricmp(szCurrAccountName, pszAccountName))
                    {
                        serviceType = SERVICE_WIN32_OWN_PROCESS;
                    }
                }
            }
            SAPDB_Bool changeDBRoot = stricmp(m_dbRoot, Params.szDBRoot) != 0 ;

            // from now on: (serviceType == SERVICE_NO_CHANGE && pszAccountName == NULL) -> do not change account
            //              (serviceType != SERVICE_NO_CHANGE && pszAccountName == NULL) -> change account to local system
            //              (serviceType != SERVICE_NO_CHANGE && pszAccountName != NULL) -> change account to another user

            SAPDB_UInt4 UseIndepPath = eo01_IsEnvVarDoNotUseIndepPathSet() ? 0 : 1 ;
            readParameters ( &Params );

            tsp100_VersionResult Result = sp100_CompareVersionIDs(&Params.RTEVersionID);

            if ( 
                  changeDBRoot                                              ||
                ( pszParameter                          != NULL )           ||
                ( Result                                != IsEqual_esp100 ) ||
                ( Params.ValUseIndepPath                != UseIndepPath )   ||
                ( SERVICE_NO_CHANGE                     != serviceType)     ||
                ( SERVICE_NO_CHANGE                     != startType ))
            {
                if ( pszParameter == NULL )
                {
                    errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_DBREG_UPDATING_SERVICE, m_serviceDisplayString );
                }

                rc = alterParameter ( &Params, pszParameter, pszValue );

                if ( rc == NO_ERROR )
                {
                    rc = updateParameters( &Params);
                }

                if ( rc == NO_ERROR )
                {
                    if ((ScLock = LockServiceDatabase (hScM)) == NULL)
                    {
                        rc = GetLastError();
                    }
                    else
                    {
                        if ( hS = OpenService(hScM, m_serviceName, SERVICE_CHANGE_CONFIG) )
                        {
                            if( !pszAccountName && (serviceType != SERVICE_NO_CHANGE) ) // change to LocalSystem? (doesn't work using ChangeServiceConfig()...
                            {
                                REG_ENTRY_REC       RegistryEntries[1];
                                RegistryEntries[0].pszValueName = "ObjectName";
                                RegistryEntries[0].pValue       = "LocalSystem";
                                RegistryEntries[0].ulValueSize  = (ULONG)strlen("LocalSystem");
                                RegistryEntries[0].ulValueType  = REG_SZ;
                                rc = sql50_reg_put_service_values(m_NodeName,m_serviceName,1,RegistryEntries);
                            }
                            if( (serviceType != SERVICE_NO_CHANGE) || (startType != SERVICE_NO_CHANGE) || changeDBRoot )
                            {
                                if (ChangeServiceConfig ( hS,
                                    serviceType,
                                    startType,
                                    SERVICE_NO_CHANGE,
                                    m_executableNameWithPath, NULL, NULL, NULL,
                                    pszAccountName, pszPassword,
                                    m_serviceDisplayString ) == FALSE )
                                {
                                    rc = GetLastError();
                                }
                            }

                            if ( !CloseServiceHandle(hS) && rc == NO_ERROR )
                            {
                                rc = GetLastError();
                            }
                        }
                        UnlockServiceDatabase (ScLock);
                    }
                }
                //
                // --- set log on as a service right
                //
                if (( rc             == NO_ERROR ) &&
                    ( pszAccountName != NULL )     &&
                    ( pszPassword    != NULL ))
                {
                    rc = sql49c_set_service_logon_right ( m_NodeName, pszAccountName );

                    if ( rc == ERROR_ALREADY_INITIALIZED )
                    {
                        rc = NO_ERROR;
                    }
                    else if ( rc == NO_ERROR )
                    {
                        MSGCD ((INFO_LOGON_AS_SERV_RIGHT_GRNT,
                        (pszAccountName[0] == '.' && pszAccountName[1] == '\\') ?
                        pszAccountName + 2 : pszAccountName));
                    }
                }

            }
        }

        //
        //
        // ---- CREATE SERVICE
        //
        //
        else 
        {
            if(RTE_StartOnDemand == Start)
            {
                startType = SERVICE_DEMAND_START;
            }
            else
            {
                startType = SERVICE_AUTO_START;
            }
            if ( pszAccountName == (SAPDB_Char *)RTE_SERVICE_ENTRY_UNCHANGED )
            {
                pszAccountName  = NULL;
                pszPassword     = NULL;
            }
            if (( pszAccountName == NULL ) || ( pszAccountName[0] == '\0' ))
            {
                serviceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
            }
            else
            {
                serviceType = SERVICE_WIN32_OWN_PROCESS;
            }
            if ((ScLock = LockServiceDatabase (hScM)) == NULL)
            {
                rc = GetLastError();
            }

            if ( rc == NO_ERROR )
            {
                if ((hS = CreateService(hScM, m_serviceName, m_serviceDisplayString,
                    SERVICE_ALL_ACCESS, serviceType,
                    startType,
                    SERVICE_ERROR_NORMAL,
                    m_executableNameWithPath, NULL, NULL, NULL,
                    pszAccountName, pszPassword )) == NULL)
                {
                    rc = GetLastError();
                }
                else
                {
/*  TODO: enter a service description. This description is disabled in the windows service manager 
                    SERVICE_DESCRIPTION sd;
                    sd.lpDescription = (LPTSTR)"Provides core database functionality";
                    rc = ChangeServiceConfig2(hS,SERVICE_CONFIG_DESCRIPTION,(LPVOID)&sd);

                    if(0 == rc)
                    {
                        rc = GetLastError();
                    }
*/
                    rc = sql49c_alloc_and_init_service_SD ( &pSD, hS );

                    if ( (rc == NO_ERROR ) && ( pSD != NULL ))
                    {
                        if (!SetServiceObjectSecurity( hS, DACL_SECURITY_INFORMATION, pSD ))
                        {
                            rc = GetLastError ();
                        }
                    }

                    if (!CloseServiceHandle(hS) && rc == NO_ERROR)
                    {
                        rc = GetLastError();
                    }

                    if ( rc == NO_ERROR )
                    {
                        rc = alterParameter ( &Params, pszParameter, pszValue );

                        if ( rc == NO_ERROR )
                        {
                            rc = updateParameters( &Params);
                        }
                    }

                    //
                    // --- set log on as a service right
                    //
                    if (( rc == NO_ERROR ) && ( pszAccountName != NULL ) && ( pszPassword != NULL ))
                    {
                        rc = sql49c_set_service_logon_right ( m_NodeName, pszAccountName );

                        if ( rc == ERROR_ALREADY_INITIALIZED )
                        {
                            rc = NO_ERROR;
                        }
                        else if ( rc == NO_ERROR )
                        {
                            MSGCD ((INFO_LOGON_AS_SERV_RIGHT_GRNT,
                            (pszAccountName[0] == '.' && pszAccountName[1] == '\\') ?
                            pszAccountName + 2 : pszAccountName));
                        }
                    }

                    if (( rc != NO_ERROR ) && ( hS = OpenService(hScM, m_serviceName, DELETE) ))
                    {
                        DeleteService(hS);
                        CloseServiceHandle(hS);
                    }
                }
            }

            if ( ScLock != NULL )
            {
                UnlockServiceDatabase (ScLock);
            }
#if defined CLUSTER_SERVER_VERSION
            if ( m_speed == speedFast )
            {
                os212_AddSERVERDBClusterResource ( m_dbName, "service entry" );
            }
#endif
        }

        if (!CloseServiceHandle(hScM) && rc == NO_ERROR)
        {
            rc = GetLastError();
        }
    }
    else
    {
        rc = GetLastError();
    }

    return makeErrListFromApiRet(rc,errList);
}

/*------------------------------*/

SAPDB_Bool RTE_NTService::RemoveServiceEntry (   
                                          SAPDBErr_MessageList   &errList)
{
    SC_HANDLE             hScM, hS;
    SC_LOCK               ScLock      = NULL;
    APIRET                rc          = NO_ERROR;
    DBGIN;

    if (hScM = OpenSCManager( m_NodeName, NULL, SC_MANAGER_ALL_ACCESS ))
    {
        //
        //
        // ---- REMOVE SERVICE
        //
        //
        if ((ScLock = LockServiceDatabase (hScM)) == NULL)
        {
            rc = GetLastError();
        }
        else
        {
            if ( hS = OpenService(hScM, m_serviceName, DELETE) )
            {
                if ( !DeleteService(hS) )
                {
                    rc = GetLastError();
                }
                else
                {
                    C40C szEventSource;
                    strcpy ( szEventSource, SERVERDB_EVENT_SOURCE_NAME );
                    strcat( szEventSource, m_dbName );

                    sql60_remove_event_source ( m_NodeName,
                        szEventSource );
                }

                if ( !CloseServiceHandle(hS) && rc == NO_ERROR )
                {
                    rc = GetLastError();
                }
            }
            else
            {
                rc = GetLastError();
            }
        }
        UnlockServiceDatabase (ScLock);

#if defined CLUSTER_SERVER_VERSION
        if ( m_speed == speedFast )
        {
            os212_RemoveSERVERDBClusterResource ( m_dbName, 0 );
        }
#endif

        if (!CloseServiceHandle(hScM) && rc == NO_ERROR)
        {
            rc = GetLastError();
        }
    }
    else
    {
        rc = GetLastError();
    }

    return makeErrListFromApiRet(rc,errList);
}

/*------------------------------*/

RTE_NTService::RTE_NTService(
        const SAPDB_Char *dbName,
        const SAPDB_Char *dbRoot,
        const SAPDB_Char *PgmName,  
        const SAPDB_Char *NodeName,
        RTE_SpeedInfo &speed)
        :
        m_speed(speed),
        m_errorInConstructor(NO_ERROR),
        m_executableNameWithPath(NULL),
        m_serviceName(NULL),
        m_serviceDisplayString(NULL),
        m_NodeName(NULL)
{
    SAPDB_sprintf(m_dbRoot , sizeof(RTE_Path),"%s",dbRoot);
    
    SAPDB_sprintf(m_dbName , sizeof(RTE_DBName),"%s",dbName);
    CharUpperBuff(m_dbName, (DWORD)strlen(m_dbName));

    SAPDB_UInt sizeNeededForExecutableNameWithPath;
    if(speedTest == speed && NULL != PgmName)
    {
        sizeNeededForExecutableNameWithPath = (SAPDB_UInt)(strlen(DB_KERN_PATH_NAME) + strlen(PgmName) + 1);
    }
    else
    {
        sizeNeededForExecutableNameWithPath = (SAPDB_UInt)(strlen(dbRoot) + 1 + strlen(speed.ExecutableName()) + 1);
    }
    m_executableNameWithPath = (SAPDB_Char *)(RTEMem_Allocator::Instance().Allocate(sizeNeededForExecutableNameWithPath));
    if(speedTest == speed && NULL != PgmName)
    {
        SAPDB_sprintf(m_executableNameWithPath,sizeNeededForExecutableNameWithPath,"%s%s",DB_KERN_PATH_NAME,PgmName);
    }
    else
    {
        if ( dbRoot[strlen(dbRoot) - 1] == '\\' )
        {
            SAPDB_sprintf(m_executableNameWithPath,sizeNeededForExecutableNameWithPath,"%s%s",dbRoot,speed.ExecutableName());
        }
        else
        {
            SAPDB_sprintf(m_executableNameWithPath,sizeNeededForExecutableNameWithPath,"%s\\%s",dbRoot,speed.ExecutableName());
        }
    }

    if(NULL != NodeName)
    {
        SAPDB_UInt sizeNeededForNodeName = (SAPDB_UInt)strlen(NodeName) + 1;
        m_NodeName = (SAPDB_Char *)(RTEMem_Allocator::Instance().Allocate(sizeNeededForNodeName));
        SAPDB_sprintf(m_NodeName,sizeNeededForNodeName,"%s",NodeName);
    }

    if (m_hScM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT))
    {
        SAPDB_UInt sizeNeededForSpeedName = 0;
        if(speedTest == speed)
        {
            sizeNeededForSpeedName = 20;
        }
        else if(!(speedFast == speed))
        {
            sizeNeededForSpeedName = (SAPDB_UInt)(strlen(" (") + strlen((SAPDB_Char *)speed.Name()) + strlen(")"));
        }
        SAPDB_UInt sizeNeededForServiceName = (SAPDB_UInt)(strlen(SERVICE_ID) + strlen(dbName) + 1) + sizeNeededForSpeedName;
        SAPDB_UInt sizeNeededForServiceDisplayString = (SAPDB_UInt)(strlen(SERVICE_DISPLAY_STR) + strlen(dbName) + 1) + sizeNeededForSpeedName;

        m_serviceName
            = (SAPDB_Char *)(RTEMem_Allocator::Instance().Allocate(sizeNeededForServiceName));

        m_serviceDisplayString
            = (SAPDB_Char *)(RTEMem_Allocator::Instance().Allocate(sizeNeededForServiceDisplayString));

        if(NULL == m_serviceName)
        {
            m_errorInConstructor = ERROR_NOT_ENOUGH_MEMORY;
        }
        else
        {
            SAPDB_sprintf(m_serviceName,sizeNeededForServiceName,"%s%s",SERVICE_ID,dbName);
            SAPDB_sprintf(m_serviceDisplayString,sizeNeededForServiceDisplayString,"%s%s",SERVICE_DISPLAY_STR,dbName);
            if(!(speedFast == speed))
            {
                SAPDB_sprintf(m_serviceName,sizeNeededForServiceName,"%s (%s)",m_serviceName,speed.Name());
                if(speedTest == speed)
                {
                    SAPDB_Char *pszPgmNamePos;
                    pszPgmNamePos = strrchr ( m_executableNameWithPath, '\\' );
                    pszPgmNamePos = pszPgmNamePos != NULL ? pszPgmNamePos + 1 : m_executableNameWithPath;
                    SAPDB_sprintf(m_serviceDisplayString,sizeNeededForServiceDisplayString,"%s (%.20s)",m_serviceDisplayString,pszPgmNamePos );
                }
                else
                {
                    SAPDB_sprintf(m_serviceDisplayString,sizeNeededForServiceDisplayString,"%s (%s)",m_serviceDisplayString,speed.Name());
                }
            }
        }
    }
    else
    {
        m_errorInConstructor = GetLastError();
    }
};

/*------------------------------*/

RTE_NTService::~RTE_NTService()
{
    if(NULL != m_hScM)
    {
        CloseServiceHandle(m_hScM);
    }
    if(NULL != m_serviceName)
    {
        RTEMem_Allocator::Instance().Deallocate(m_serviceName);
    }
    if(NULL != m_executableNameWithPath)
    {
        RTEMem_Allocator::Instance().Deallocate(m_executableNameWithPath);
    }
    if(NULL != m_serviceDisplayString)
    {
        RTEMem_Allocator::Instance().Deallocate(m_serviceDisplayString);
    }
    if(NULL != m_NodeName)
    {
        RTEMem_Allocator::Instance().Deallocate(m_NodeName);
    }
};

/*------------------------------*/

APIRET RTE_NTService::GetExecutableName(RTE_Path &executable)
{
    if(m_errorInConstructor)
    {
        return m_errorInConstructor;
    }
    APIRET rc = NO_ERROR;
    SC_HANDLE serviceHandle;
    if(!(serviceHandle = OpenService(m_hScM, m_serviceName,  SERVICE_QUERY_CONFIG )))// need QUERY access
    {
        return GetLastError();
    }
    // first call to QueryServiceConfig() with a zero size memory area, just to see how big the memory area
    // has to be...
    DWORD sizeNeeded;
    QueryServiceConfig(serviceHandle,NULL,0,&sizeNeeded);
    if(ERROR_INSUFFICIENT_BUFFER == (rc = GetLastError()))
    // as we have passed a zero size memory area to QueryServiceConfig(), ERROR_INSUFFICIENT_BUFFER is just
    // the expected result...
    {
        QUERY_SERVICE_CONFIG *serviceConfig = (QUERY_SERVICE_CONFIG *)alloca(sizeNeeded);
        if(NULL == serviceConfig)
        {
            rc = ERROR_NOT_ENOUGH_MEMORY;
        }
        if(!QueryServiceConfig(serviceHandle,serviceConfig,sizeNeeded,&sizeNeeded))
        {
            rc = GetLastError();
        }
        SAPDB_sprintf(executable,sizeof(RTE_Path),((QUERY_SERVICE_CONFIG *)serviceConfig)->lpBinaryPathName);
    }
    CloseServiceHandle(serviceHandle);
    return rc;
};

/*------------------------------*/

APIRET RTE_NTService::GetAutoStartFlag(SAPDB_Bool &autoStart)
{
    if(m_errorInConstructor)
    {
        return m_errorInConstructor;
    }
    APIRET rc = NO_ERROR;
    SC_HANDLE serviceHandle;
    if(!(serviceHandle = OpenService(m_hScM, m_serviceName,  SERVICE_QUERY_CONFIG )))// need QUERY access
    {
        return GetLastError();
    }
    // first call to QueryServiceConfig() with a zero size memory area, just to see how big the memory area
    // has to be...
    DWORD sizeNeeded;
    QueryServiceConfig(serviceHandle,NULL,0,&sizeNeeded);
    if(ERROR_INSUFFICIENT_BUFFER == (rc = GetLastError()))
    // as we have passed a zero size memory area to QueryServiceConfig(), ERROR_INSUFFICIENT_BUFFER is just
    // the expected result...
    {
        QUERY_SERVICE_CONFIG *serviceConfig = (QUERY_SERVICE_CONFIG *)alloca(sizeNeeded);
        if(NULL == serviceConfig)
        {
            rc = ERROR_NOT_ENOUGH_MEMORY;
        }
        else if(!QueryServiceConfig(serviceHandle,serviceConfig,sizeNeeded,&sizeNeeded))
        {
            rc = GetLastError();
        }
        else
        {
            autoStart = (SERVICE_AUTO_START == ((QUERY_SERVICE_CONFIG *)serviceConfig)->dwStartType);
        }
    }
    CloseServiceHandle(serviceHandle);
    return rc;
};

/*------------------------------*/

APIRET RTE_NTService::Start(SAPDB_Char *options)
{
    if(m_errorInConstructor)
    {
        return m_errorInConstructor;
    }
    APIRET rc;
    SC_HANDLE serviceHandle;
    if (!(serviceHandle = OpenService(m_hScM, m_serviceName, SERVICE_START )))
    {
        return GetLastError();
    }
    for(SAPDB_Int4 timeout=0 ; timeout < 60 ; timeout++)
    {
        const CHAR *pszArg[1] = { options };
        ULONG      ulArgs = 1;
        if(StartService(serviceHandle, ulArgs, &pszArg[0]))
        {
            rc = NO_ERROR;
            break;
        }
        else
        {
            rc = GetLastError();
            if(ERROR_SERVICE_DATABASE_LOCKED == rc)
            {
                SLEEP(1000);
            }
            else
            {
                break;
            }
        }
    }
    CloseServiceHandle(serviceHandle);
    return rc;
};

/*------------------------------*/

APIRET RTE_NTService::Stop(void)
{
    if(m_errorInConstructor)
    {
        return m_errorInConstructor;
    }
    SC_HANDLE serviceHandle;
    APIRET rc = NO_ERROR;
    if (!(serviceHandle = OpenService(m_hScM, m_serviceName, SERVICE_STOP )))
    {
        return GetLastError();
    }
    SERVICE_STATUS ssServiceStatus;
    if (!ControlService(serviceHandle, SERVICE_CONTROL_STOP, &ssServiceStatus))
    {
        rc = GetLastError();
    }
    CloseServiceHandle(serviceHandle);
    return rc;
};

/*------------------------------*/

APIRET RTE_NTService::GetStatus(DWORD &dwCurrentState)
{
    if(m_errorInConstructor)
    {
        return m_errorInConstructor;
    }
    APIRET rc = NO_ERROR;
    SC_HANDLE serviceHandle;
    if(!(serviceHandle = OpenService(m_hScM, m_serviceName,  SERVICE_QUERY_STATUS   )))
    {
        return GetLastError();
    }
    SERVICE_STATUS ssServiceStatus;
    if ( !QueryServiceStatus( serviceHandle, &ssServiceStatus ))
    {
        rc = GetLastError();
    }
    else
    {
        dwCurrentState = ssServiceStatus.dwCurrentState;
    }
    CloseServiceHandle( serviceHandle );
    return rc;
};

