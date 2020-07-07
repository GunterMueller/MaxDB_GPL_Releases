/*!**********************************************************************

  module: XMLIdx_Engine.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Index Engine

  description:  Engine

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



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





************************************************************************/

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include "heo07.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/IndexEngine/XMLIdx_Engine.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_Workers.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_ErrorCodes.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_DocStore.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_Indexing.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Common/XMLCom_Common.h"
#include "SAPDB/XML/Error/XMLError_Error.h"
#include "SAPDB/XML/DocClass/XMLDC_DocClass.h"
#include "SAPDB/XML/Library/XMLLib_Library.h"
#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"
#include "SAPDB/XML/Settings/XMLSet_Settings.h"
#include "SAPDB/XML/Memory/XMLMem_Memory.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

#ifdef WIN32
    #define Engine_Sleep(a)     Sleep(a)
#else
    #define Engine_Sleep(a)     sleep(a)
#endif
/****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

typedef struct st_xmlidx_engine                 *XMLIdx_Engine;

struct st_xmlidx_engine {
    XMLIdx_ServiceDescription   hServiceDescription;
};

struct st_xmlidx_service_description {
    XMLId_Id                    serviceId;
    XMLSet_ServiceNameString    serviceName;

    XMLLib_Library              hLib;
    XMLLib_LibraryNameString    libPath;

    XMLSP_SessionPool           hSessionPoolIndexStore;
    XMLSP_SessionPool           hSessionPoolDocStore;
    XMLSP_Session               hSessionRegistry;

    XMLIMAPI_Handle             hIMApi;

    XMLDC_DocClass              hDocClassList;
    XMLLib_Function             funcSetIndexValue;
};

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

void* Engine_Listen( void *userData );

SAPDB_Bool  Engine_CreateHandle( XMLIdx_Engine *hNewEngine,
                                 XMLError_Error hError );

SAPDB_Bool  Engine_InitHandle( XMLIdx_Engine  hNewEngine,
                               XMLError_Error hError );

SAPDB_Bool  Engine_DestroyHandle( XMLIdx_Engine hEngine );

SAPDB_Bool  Engine_CreateServiceDescription( XMLIdx_ServiceDescription *hServiceDescription,
                                             XMLSet_ServiceName         serviceName,
                                             XMLSP_DBServerNode         serverNode,
                                             XMLSP_DBServerName         serverName,
                                             XMLSP_DBUser               user,
                                             XMLSP_DBPassword           password,
                                             XMLSP_SessionPoolSize      maxThreads,
                                             XMLSP_DBTraceFilename      traceFilename,
                                             XMLLib_LibraryName         indexingLibName,
                                             XMLError_Error             hError );

SAPDB_Bool  Engine_InitServiceDescription( XMLIdx_ServiceDescription  hIndexingServiceDescription,
                                           XMLSet_ServiceName         serviceName,
                                           XMLSP_DBServerNode         serverNode,
                                           XMLSP_DBServerName         serverName,
                                           XMLSP_DBUser               user,
                                           XMLSP_DBPassword           password,
                                           XMLSP_SessionPoolSize      maxThreads,
                                           XMLSP_DBTraceFilename      traceFilename,
                                           XMLLib_LibraryName         indexingLibName,
                                           XMLError_Error             hError );

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

static teo07_Thread     engineThread = NULL;
static teo07_Thread     listenerThread = NULL;

static SAPDB_Bool       stopEngine = SAPDB_FALSE;
static SAPDB_Bool       engineStopped = SAPDB_FALSE;

/*---------------------------------------------------------------------------*/

static XMLSet_Settings  hSettings;

/*---------------------------------------------------------------------------*/

static XMLIdx_Engine    hEngine = NULL;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool  Engine_Create( char *exeDir,
                           char *iniFile,
                           char *sectionName,
                           char *errorOutText )
{

    tsp00_ErrTextc              threadErrText;
    teo07_ThreadErr             threadErr;

    char                        usedIniFile[XMLCOM_MAX_FILENAME_LEN+1];

    struct st_xmlerror_error    error;
    XMLError_Error              hError = &error;
    XMLError_LogFilenameString  logFilenameString;
    XMLError_LogFilename        logFilename = logFilenameString;

    /* set default log filename */
    if (exeDir && strlen(exeDir) > 0) {
        if (exeDir[strlen(exeDir)-1] == XMLCOM_DIRPATH_SEP) {
            sp77sprintf(logFilename, XMLERROR_MAX_LOG_FILENAME_LEN, "%ssapdbxie.log", exeDir);
        } else {
            sp77sprintf(logFilename, XMLERROR_MAX_LOG_FILENAME_LEN, "%s%csapdbxie.log", exeDir, XMLSET_DIRPATH_SEP);
        }
    } else {
        logFilename = NULL;
    }
    Error_SetLogFilename(logFilename);
    Error_Init(hError, "START");

    /* Init threading environment           */
    /* Also important for usage of mutex    */
    sqlinit_multithreading(&engineThread, threadErrText, &threadErr);
    if (threadErr != THR_OK_EO07) {
        Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_CANNOT_INIT_THREADING,
                                        XMLIDX_ERR_TEXT_CANNOT_INIT_THREADING);
        Error_Log(hError);

        strcpy(errorOutText, "Cannot initialize threading");

        return SAPDB_FALSE;
    }

    /* Init error */
    /* We can do this only if we made a sqlinit_multithreading -> use of mutex */
    if (!Error_Create()) {
        Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_CANNOT_INIT_THREADING,
                                        XMLIDX_ERR_TEXT_CANNOT_INIT_THREADING);
        Error_Log(hError);

        strcpy(errorOutText, "Cannot initialize threading");

        return SAPDB_FALSE;
    }

    /* Init id management */
    Id_Init();

    /* Read settings from registry */
    if (!Settings_Create(&hSettings, exeDir, iniFile, sectionName, usedIniFile, hError)) {
        Error_Log(hError);
        Error_Destroy();

        strcpy(errorOutText, "Cannot read settings");

        return SAPDB_FALSE;
    }

    /* Set log filename */
    Error_SetLogFilename(Settings_GetLogFilename(hSettings));

    /* Init worker threads */
    if (!Workers_Create(hError)) {
        Error_Log(hError);
        Error_Destroy();

        strcpy(errorOutText, "Cannot init worker threads");

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool  Engine_Destroy()
{

	SAPDB_Int4		status;
	tsp00_ErrTextc	errtext;
	teo07_ThreadErr	err;

	if (!listenerThread) {
		return SAPDB_TRUE;
	}

    /* Stop engine */
    stopEngine = SAPDB_TRUE;
    engineStopped = SAPDB_FALSE;

    /* Wait until engine has been stopped */
    do {
        Engine_Sleep(1000);
    } while (engineStopped == SAPDB_FALSE);

    /* Terminate listener */
    /*
    sqljointhread(listenerThread, &status, errtext, &err );
    

	if (err != THR_OK_EO07) {
        * Log errText *
	}

	listenerThread = NULL;
    */

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool  Engine_Start()
{

    tsp00_ErrTextc  errtext;
    teo07_ThreadErr err;

    /* Already running? */
    if (listenerThread) {
        return SAPDB_TRUE;
    }

    /* Start workers */
    if (!Workers_Start(Settings_GetMaxThreads(hSettings))) {
        return SAPDB_FALSE;
    }

    /* Start listener */
    sqlbeginthread(XMLCOM_THREAD_STACK_SIZE, &Engine_Listen, NULL,
                   THR_CREATE_SUSPENDED_EO07, &listenerThread, errtext, &err);

    if (err == THR_OK_EO07) {
        sqlresumethread(listenerThread, errtext, &err);
        if (err == THR_OK_EO07) {
            return SAPDB_TRUE;
        } else {
            return SAPDB_FALSE;
        }
    } else {
        return SAPDB_FALSE;
    }

}

/*===========================================================================*/

SAPDB_Bool  Engine_Wait( SAPDB_Bool  debug )
{

	tsp00_Int4		 status;
	tsp00_ErrTextc	 errtext;
	teo07_ThreadErr	 err;

	if (!listenerThread) {
		return SAPDB_FALSE;
	}

#ifdef WIN32
	if ( debug == SAPDB_FALSE ) {
		/* Report the status SERVICE_RUNNING	*/
		Service_IsRunning();
	}
#endif

    sqljointhread(listenerThread, &status, errtext, &err);
	if (err != THR_OK_EO07) {
        /* Log errText */
	}

	listenerThread = NULL;

	return SAPDB_TRUE;

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

void* Engine_Listen( void *userData )
{

    struct st_xmlerror_error	error;
    struct st_xmlerror_error	lastError;
    XMLError_Error				hError = &error;
    XMLError_Error				hLastError = &lastError;
    XMLIdx_Work					hWork = NULL;
    SAPDB_UInt4				    workersCount = 0;
    XMLSP_Session				hSessionCurrentDoc = NULL;
    XMLId_Id					docId;
    XMLDC_DocClassIdString		docClassId;

    /* Inits */
    Error_Init(hError, "ENGINE");
    Error_Init(hLastError, "ENGINE");

    /* Init doc class management */
    if (!DocClass_Init()) {
        return (void *)0;
    }

    /* Create engine handle */
    if (!Engine_CreateHandle(&hEngine, hError)) {
        Error_Log(hError);
        DocClass_Exit();

        return 0;
    }

    /* Get service description */
    if (!Engine_CreateServiceDescription(&hEngine->hServiceDescription,
                                          Settings_GetIndexingServiceName(hSettings),
                                          Settings_GetRegistryNode(hSettings),
                                          Settings_GetRegistryName(hSettings),
                                          Settings_GetRegistryUser(hSettings),
                                          Settings_GetRegistryPassword(hSettings),
                                          Settings_GetMaxThreads(hSettings),
                                          Settings_GetRegistryTraceFilename(hSettings),
                                          Settings_GetIndexingLibFile(hSettings),
                                          hError)) {
        Error_Log(hError);
        Engine_DestroyHandle(hEngine);

        DocClass_Exit();

		return 0;
    }

    /* Start workers */
    if (!Workers_Resume(hError)) {
        Error_Log(hError);
        Engine_DestroyHandle(hEngine);

        DocClass_Exit();

        return (void *)0;
    }

    /* Poll infinite */
    while(stopEngine != SAPDB_TRUE) {
        /* Can we use last session again? */
        if (!hSessionCurrentDoc) {
            /* Get doc store session */
            if (!SP_GetSession(hEngine->hServiceDescription->hSessionPoolDocStore,
                              &hSessionCurrentDoc, hError)) {
                Error_Log(hError);

                break;
            }
        }

        /* Initialize indexing process */
        SP_StartTransaction(hSessionCurrentDoc, hError);

        /* Look for next work */
        DocStore_SelectNextDoc(hSessionCurrentDoc,
                             docId,
                             docClassId,
                             hError);
        if (hError->code != XMLIDX_ERR_CODE_UNDEFINED && hError->code != XMLIDX_ERR_CODE_NO_DOC_TO_INDEX) {
	        /* Error handling */
            if (hLastError->code != hError->code) {
                hLastError->code = hError->code;

                Error_Log(hError);
            }

            /* Cancel indexing process */
            SP_CancelTransaction(hSessionCurrentDoc, hError);

            /* Nothing to parse */
            hError->code = XMLIDX_ERR_CODE_NO_DOC_TO_INDEX;
	    }

        /* Is there something to do? */
        if(hError->code == XMLIDX_ERR_CODE_NO_DOC_TO_INDEX) {
            /* Wait, to give another process a chance */
            Engine_Sleep(1000);
        } else {
            /* Insert work into worker queue */
             if (!Workers_CreateWork(&hWork,
                                      hEngine->hServiceDescription->hSessionPoolIndexStore,
                                      hEngine->hServiceDescription->hSessionPoolDocStore,
                                      hSessionCurrentDoc,
                                      docId,
                                      hEngine->hServiceDescription->hDocClassList,
                                      docClassId,
                                      hEngine->hServiceDescription->funcSetIndexValue,
                                      hError)) {
	            /* Error handling */
                Error_Log(hError);

                /* Cancel indexing process */
                SP_CancelTransaction(hSessionCurrentDoc, hError);

                /* Put session back to session pool */
                SP_PutSession(hEngine->hServiceDescription->hSessionPoolDocStore,
                              hSessionCurrentDoc, hError);

	            break;
             }

             if (!Workers_PutWorkIntoQueue(hWork, hError)) {
	            /* Error handling */
                Error_Log(hError);

                /* Cancel indexing process */
                SP_CancelTransaction(hSessionCurrentDoc, hError);

                /* Put session back to session pool */
                SP_PutSession(hEngine->hServiceDescription->hSessionPoolDocStore,
                              hSessionCurrentDoc, hError);

	            break;
             }

             /* REMARK: Indexing process will be comitted by worker thread */
             /*         Current session will be gave back by worker thread */

             /* Current session will be used by new worker thread,  */
             /* so we can not use it anymore                        */
             hSessionCurrentDoc = NULL;
        }
    }

    /* Stop workers */
    Workers_Stop();

    /* Wait until all workers has been terminated */
    do {
        Workers_ThreadAlive(&workersCount);
    } while (workersCount > 0);

    /* Close and drop registry handles */
    XMLIMAPI_destroyHandle(hEngine->hServiceDescription->hIMApi);
    SP_CloseSession(hEngine->hServiceDescription->hSessionRegistry, hError);

    /* Destroy engine handle */
    Engine_DestroyHandle(hEngine);

    /* Exit doc class management */
    DocClass_Exit();

    /* Destroy error */
    Error_Destroy();

    /* Destroy settings */
    Settings_Destroy(hSettings);

    engineStopped = SAPDB_TRUE;

    return (void*)0;

}

/*===========================================================================*/

SAPDB_Bool  Engine_CreateServiceDescription( XMLIdx_ServiceDescription *hServiceDescription,
                                             XMLSet_ServiceName         serviceName,
                                             XMLSP_DBServerNode         serverNode,
                                             XMLSP_DBServerName         serverName,
                                             XMLSP_DBUser               user,
                                             XMLSP_DBPassword           password,
                                             XMLSP_SessionPoolSize      maxThreads,
                                             XMLSP_DBTraceFilename      traceFilename,
                                             XMLLib_LibraryName         indexingLibName,
                                             XMLError_Error             hError )
{

    SAPDB_Bool  ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_xmlidx_service_description), (SAPDB_UInt1**) hServiceDescription, &ok );
	if (!ok) {
        *hServiceDescription = NULL;

        return SAPDB_FALSE;
	}

	/* Init */
    if (!Engine_InitServiceDescription(*hServiceDescription, serviceName,
                                        serverNode, serverName, user, password, maxThreads, traceFilename,
                                        indexingLibName, hError )) {

        XMLMem_Free(*hServiceDescription);
        *hServiceDescription = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool  Engine_InitServiceDescription( XMLIdx_ServiceDescription  hIndexingServiceDescription,
                                           XMLSet_ServiceName         serviceName,
                                           XMLSP_DBServerNode         serverNode,
                                           XMLSP_DBServerName         serverName,
                                           XMLSP_DBUser               user,
                                           XMLSP_DBPassword           password,
                                           XMLSP_SessionPoolSize      maxThreads,
                                           XMLSP_DBTraceFilename      traceFilename,
                                           XMLLib_LibraryName         indexingLibName,
                                           XMLError_Error             hError )
{

    XMLIMAPI_IndexingService    serviceDescription;
    XMLIMAPI_ErrorItem          imAPIErrorItem;
    XMLIMAPI_ErrorText          imApiErrorText;

    /* Set service name */
    Com_StrMaxCopy(hIndexingServiceDescription->serviceName, serviceName, XMLSET_MAX_SERVICE_NAME_LEN);

    /* Open session to registry */
	if (!SP_OpenSession(&hIndexingServiceDescription->hSessionRegistry, serverNode, serverName, user, password, traceFilename, hError)) {
        Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_CAN_NOT_CONNECT_TO_REG_DB, XMLIDX_ERR_TEXT_CAN_NOT_CONNECT_TO_REG_DB);

        return SAPDB_FALSE;
	}

    /* Create xml modelling api handle */
    if (!XMLIMAPI_createHandle(&hIndexingServiceDescription->hIMApi,
                                SP_GetHEnv(hIndexingServiceDescription->hSessionRegistry),
                                SP_GetHDBC(hIndexingServiceDescription->hSessionRegistry))) {
        Session_Close(hIndexingServiceDescription->hSessionRegistry);

        return SAPDB_FALSE;
    }

    /* Get service description from registry */
    if (!XMLIMAPI_ServiceGetIndexingService(hIndexingServiceDescription->hIMApi,
                                            hIndexingServiceDescription->serviceName,
                                            &serviceDescription)) {
        if (XMLIMAPI_GetLastError(hIndexingServiceDescription->hIMApi, &imAPIErrorItem)) {
            XMLIMAPI_GetErrorText(imAPIErrorItem, &imApiErrorText);

            Error_Set(XMLERROR_POS, hError,
                      XMLIDX_ERR_CODE_CAN_NOT_GET_INDEXING_DESCRIPTION, imApiErrorText);
        }

        XMLIMAPI_destroyHandle(hIndexingServiceDescription->hIMApi);
        Session_Close(hIndexingServiceDescription->hSessionRegistry);

        return SAPDB_FALSE;
    }
                
    /* Set service id */
    strcpy(hIndexingServiceDescription->serviceId, serviceDescription.ServiceID);

    /* Load library for given indexing service */
    if (!Library_Load(&hIndexingServiceDescription->hLib,
					  indexingLibName,
                      "SAPDBXML_Init",
                      "SAPDBXML_Exit",
					  hError)) {
        XMLIMAPI_destroyHandle(hIndexingServiceDescription->hIMApi);
        Session_Close(hIndexingServiceDescription->hSessionRegistry);

        return SAPDB_FALSE;
    };

    /* Get function to set index values */
    if (!Library_GetFunction(hIndexingServiceDescription->hLib,
                            "SAPDBXML_SetIndexValue",
                            &hIndexingServiceDescription->funcSetIndexValue,
                             hError)) {
        Library_Unload(hIndexingServiceDescription->hLib);
        XMLIMAPI_destroyHandle(hIndexingServiceDescription->hIMApi);
        Session_Close(hIndexingServiceDescription->hSessionRegistry);

        return SAPDB_FALSE;
    };

    /* Create session pool for doc store */
    if (!SP_Create(&hIndexingServiceDescription->hSessionPoolDocStore,
                   serviceDescription.DocStore.DbNode,
                   serviceDescription.DocStore.DbName,
                   serviceDescription.DocStore.DbUser,
                   serviceDescription.DocStore.DbPassword,
                   serviceDescription.DocStore.TraceFile, maxThreads, hError)) {
        /* Error handling */
        Library_Unload(hIndexingServiceDescription->hLib);
        XMLIMAPI_destroyHandle(hIndexingServiceDescription->hIMApi);
        Session_Close(hIndexingServiceDescription->hSessionRegistry);

        return SAPDB_FALSE;
    }

    /* Create session pool for index store */
    if (!SP_Create(&hIndexingServiceDescription->hSessionPoolIndexStore,
                   serviceDescription.IdxStore.DbNode,
                   serviceDescription.IdxStore.DbName,
                   serviceDescription.IdxStore.DbUser,
                   serviceDescription.IdxStore.DbPassword,
                   serviceDescription.IdxStore.TraceFile, maxThreads, hError)) {
        /* Error handling */
        Library_Unload(hIndexingServiceDescription->hLib);
        XMLIMAPI_destroyHandle(hIndexingServiceDescription->hIMApi);
        Session_Close(hIndexingServiceDescription->hSessionRegistry);

        return SAPDB_FALSE;
    }

    /* Get doc class list */
    if (!DocClass_CreateDocClassList(&hIndexingServiceDescription->hDocClassList,
                                      hIndexingServiceDescription->hIMApi,
                                      Indexing_SetIndexValue,
                                      hError)) {
        /* Error handling */
        Library_Unload(hIndexingServiceDescription->hLib);
        SP_Destroy(hIndexingServiceDescription->hSessionPoolDocStore);
        XMLIMAPI_destroyHandle(hIndexingServiceDescription->hIMApi);
        Session_Close(hIndexingServiceDescription->hSessionRegistry);

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool  Engine_DestroyServiceDescription( XMLIdx_ServiceDescription  hIndexingServiceDescription )
{

    if (!hIndexingServiceDescription) {
        return SAPDB_TRUE;
    }

    /* Destroy session pools */
    SP_Destroy(hIndexingServiceDescription->hSessionPoolDocStore);

    /* Destroy session pools */
    SP_Destroy(hIndexingServiceDescription->hSessionPoolIndexStore);

    /* Unload libraries */
    Library_Unload(hIndexingServiceDescription->hLib);

    /* Destroy service description itself */
    XMLMem_Free(hIndexingServiceDescription);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool  Engine_CreateHandle( XMLIdx_Engine *hNewEngine,
                                 XMLError_Error hError )
{

    SAPDB_Bool  ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_xmlidx_engine), (SAPDB_UInt1**) hNewEngine, &ok );
	if (!ok) {
        *hNewEngine = NULL;

        return SAPDB_FALSE;
	}

	/* Init */
    if (!Engine_InitHandle(*hNewEngine, hError)) {
        XMLMem_Free(*hNewEngine);
        *hNewEngine = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool  Engine_InitHandle( XMLIdx_Engine  hNewEngine,
                               XMLError_Error hError )
{

    hNewEngine->hServiceDescription = NULL;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool  Engine_DestroyHandle( XMLIdx_Engine hEngine )
{

    /* Destroy service description */
    Engine_DestroyServiceDescription(hEngine->hServiceDescription);

    /* Destroy engine itself */
    XMLMem_Free(hEngine);

    return SAPDB_TRUE;

}

/***********************************************************************

	End

 ***********************************************************************/
