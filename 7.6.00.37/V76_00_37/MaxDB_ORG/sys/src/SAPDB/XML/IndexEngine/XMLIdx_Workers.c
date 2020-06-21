/*!**********************************************************************

  module: XMLIdx_Workers.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Index Engine

  description:  Worker management

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
#include "SAPDBCommon/SAPDB_string.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/IndexEngine/XMLIdx_Workers.h"
#include "SAPDB/XML/Semaphore/XMLSem_Semaphore.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_ErrorCodes.h"

#include "SAPDB/XML/Common/XMLCom_Common.h"
#include "SAPDB/XML/Lock/XMLLock_Lock.h"
#include "SAPDB/XML/Memory/XMLMem_Memory.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

void* Workers_Worker( void *userData );

SAPDB_Bool Workers_InitWork( XMLIdx_Work        hWork,
                             XMLSP_SessionPool  hSessionPoolIndexStore,
                             XMLSP_SessionPool  hSessionPoolDocStore,
                             XMLSP_Session      hSessionDocStore,
                             XMLId_Id           docId,
                             XMLDC_DocClass     docClassList,
                             XMLId_Id           docClassId,
                             XMLLib_Function    funcSetIndexValue,
                             XMLError_Error     hError );

SAPDB_Bool Workers_CopyUserData( void*            *newUserData,
                                 void*             userData,
                                 size_t            userDataSize,
                                 XMLError_Error    hError );

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

static XMLLock_Lock      hWorksLock = NULL;
static XMLSem_Semaphore  hWorksSemaphore = NULL;
static teo07_Thread	    *hWorkersThreadList = NULL;
static SAPDB_UInt4       workersCount = 0;

static XMLIdx_Work       lastWork = NULL;
static XMLIdx_Work       listOfWorks = NULL;

static SAPDB_Bool        stopWork = SAPDB_FALSE;

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool Workers_Create( XMLError_Error hError )
{

    if (!Lock_Create(&hWorksLock)) {
        return SAPDB_FALSE;
    }

    if (!Semaphore_Create(&hWorksSemaphore)) {
        Lock_Destroy(hWorksLock);
        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_Destroy()
{

    /* Terminate threads of worker pool */

    /* Destroy lock handle */
    Lock_Destroy(hWorksLock);

    /* Destroy semaphore handle */
    Semaphore_Destroy(hWorksSemaphore);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_Start( SAPDB_UInt4 maxWorkers )
{

    unsigned long		i;
    tsp00_ErrTextc		errorText;
    teo07_ThreadErr		error;
    tsp00_Bool			ok;

    /* Allocat list of thread ids */
    XMLMem_Allocat(maxWorkers * sizeof(teo07_Thread), &hWorkersThreadList, &ok);
    if (!ok) {
        return SAPDB_FALSE;
    };


    /* Create thread pool */
    for (i = 0; i < maxWorkers; i++) {
        sqlbeginthread(XMLCOM_THREAD_STACK_SIZE,
                       Workers_Worker, NULL, THR_CREATE_SUSPENDED_EO07, 
                      &hWorkersThreadList[i],
                       errorText, &error);

        if (error != THR_OK_EO07) {
            XMLMem_Free((SAPDB_UInt1*) hWorkersThreadList);
            return SAPDB_FALSE;
        }
    }

    /* store max workers */
    workersCount = maxWorkers;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_Stop()
{

    stopWork = SAPDB_TRUE;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_ThreadAlive( SAPDB_UInt4   *threadCount )
{

    unsigned long	i;
    tsp00_ErrTextc	errorText;
    teo07_ThreadErr	error;

    /* Init */
    *threadCount = 0;

    /* Checks */
    if (!hWorkersThreadList)
        return SAPDB_FALSE;

    /* Check thread */
    for(i = 0; i < workersCount; i++) {
        if(sqlthreadalive(hWorkersThreadList[i], errorText, &error)) {
            (*threadCount)++;
        }
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_Resume( XMLError_Error  hError )
{

    unsigned long	i;
    tsp00_ErrTextc	errorText;
    teo07_ThreadErr	error;

    if (!hWorkersThreadList)
        return SAPDB_FALSE;

    for(i = 0; i < workersCount; i++) {
        sqlresumethread(hWorkersThreadList[i], errorText, &error);
        if (error != THR_OK_EO07) {
            XMLMem_Free((SAPDB_UInt1*) hWorkersThreadList);

            return SAPDB_FALSE;
        }
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_CreateWork( XMLIdx_Work                  *hNewWork,
                               XMLSP_SessionPool             hSessionPoolIndexStore,
                               XMLSP_SessionPool             hSessionPoolDocStore,
                               XMLSP_Session                 hSessionDocStore,
                               XMLId_Id                      docId,
                               XMLDC_DocClass                docClassList,
                               XMLDC_DocClassId              docClassId,
                               XMLLib_Function               funcSetIndexValue,
                               XMLError_Error                hError )
{

    tsp00_Bool  ok;

    XMLMem_Allocat(sizeof(struct st_xmlidx_work), hNewWork, &ok);
    if (ok) {
        if (!Workers_InitWork(*hNewWork, hSessionPoolIndexStore, hSessionPoolDocStore, hSessionDocStore, docId, docClassList, docClassId, funcSetIndexValue, hError)) {
            *hNewWork = NULL;

            return SAPDB_FALSE;
        }
    } else {
        *hNewWork = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_DestroyWork( XMLIdx_Work hWork )
{

    XMLMem_Free((SAPDB_UInt1*) hWork);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_PutWorkIntoQueue( XMLIdx_Work    hWork,
                                     XMLError_Error hError )
{

    /* Acquire List controll */
    Lock_Begin(hWorksLock);

    hWork->next = NULL;

    if (lastWork) {
        lastWork->next = hWork;
        lastWork = hWork;
    } else {
        listOfWorks = hWork;
        lastWork = hWork;
    }

    Semaphore_End(hWorksSemaphore);

    /* Give up List controll */
    Lock_End(hWorksLock);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Workers_GetWorkFromQueue( XMLIdx_Work    *hWork,
                                     XMLError_Error  hError )
{

    XMLIdx_Work hCurrentWork;

    /* Wait for work */
    Semaphore_Begin(hWorksSemaphore);

    /* Acquire List controll */
    Lock_Begin(hWorksLock);

    if (listOfWorks) {
        hCurrentWork = listOfWorks;
        listOfWorks = listOfWorks->next;
        hCurrentWork->next = NULL; 

        if (!listOfWorks) {
            lastWork = NULL;
        }
    } else {
        /*	There wasn't any work, are we shutting down ? */
        hCurrentWork = NULL;
    }

    *hWork = hCurrentWork;

    /* Give up List controll */
    Lock_End(hWorksLock);

    return SAPDB_TRUE;

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

void* Workers_Worker( void *userData )
{

    struct st_xmlerror_error    error;
    XMLError_Error              hError = &error;
    XMLIdx_Work                 hWork = NULL;
    XMLId_IdString              docIdString;
    XMLId_IdString              docClassIdString;

    /* Inits */
    Error_Init(hError, "WORKER");

    do {
        /* Get next work from queue */
        if (!Workers_GetWorkFromQueue(&hWork, hError)) {
	        /* Error handling */
            Error_Log(hError);

            break;
        } else if (hWork->stopWork == SAPDB_TRUE) {
            Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_STOP_WORKER,
                                            XMLIDX_ERR_TEXT_STOP_WORKER);
            Error_Log(hError);

            Workers_DestroyWork(hWork);

            break;
        }

        /* Get indexing store session */
        if (SP_GetSession(hWork->hSessionPoolIndexStore, &hWork->hSessionIndexStore, hError)) {
            /* Process indexing */
            if (!Indexing_Process(hWork, hError)) {
                /* Error handling */
                SP_CancelTransaction(hWork->hSessionIndexStore, hError);
                SP_CancelTransaction(hWork->hSessionDocStore, hError);
                Error_Log(hError);
            } else {
                /* Commit doc store */
                SP_EndTransaction(hWork->hSessionIndexStore, hError);
                SP_EndTransaction(hWork->hSessionDocStore, hError);

#ifdef DEBUG
                Id_AsString(hWork->docId, docIdString);
                Id_AsString(hWork->docClassId, docClassIdString);
                Error_SetFormatted(XMLERROR_POS, hError, XMLERROR_ERR_CODE_UNDEFINED, "Document indexed: docId=x'%s'  docClassId=x'%s'", docIdString, docClassIdString);
                Error_Log(hError);
#endif
            }

            /* Put sessions back to session pool */
            SP_PutSession(hWork->hSessionPoolIndexStore, hWork->hSessionIndexStore, hError);
            SP_PutSession(hWork->hSessionPoolDocStore, hWork->hSessionDocStore, hError);

            /* Destroy work */
            Workers_DestroyWork(hWork);
        } else {
            /* Error handling */
            SP_CancelTransaction(hWork->hSessionDocStore, hError);
            SP_PutSession(hWork->hSessionPoolDocStore, hWork->hSessionDocStore, hError);

            Workers_DestroyWork(hWork);

            Error_Log(hError);
        }
    } while (1);

    sqlendthread(0);

    return (void*)0;

}

/*===========================================================================*/

SAPDB_Bool Workers_InitWork( XMLIdx_Work        hWork,
                             XMLSP_SessionPool  hSessionPoolIndexStore,
                             XMLSP_SessionPool  hSessionPoolDocStore,
                             XMLSP_Session      hSessionDocStore,
                             XMLId_Id           docId,
                             XMLDC_DocClass     docClassList,
                             XMLId_Id           docClassId,
                             XMLLib_Function    funcSetIndexValue,
                             XMLError_Error     hError )
{

    hWork->hSessionPoolIndexStore = hSessionPoolIndexStore;
    hWork->hSessionIndexStore = NULL;

    hWork->hSessionPoolDocStore = hSessionPoolDocStore;
    hWork->hSessionDocStore = hSessionDocStore;

    SAPDB_memcpy(hWork->docId, docId, XMLID_MAX_ID_LEN);
    SAPDB_memcpy(hWork->docClassId, docClassId, XMLID_MAX_ID_LEN);

    hWork->hDocClassList = docClassList;

    hWork->next = NULL;

    hWork->funcSetIndexValue = funcSetIndexValue;

    hWork->stopWork = SAPDB_FALSE;

    return SAPDB_TRUE;

}

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
