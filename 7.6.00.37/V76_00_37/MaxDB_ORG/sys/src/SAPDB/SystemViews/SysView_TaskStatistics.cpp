/****************************************************************************/
/*!

  @file        SysView_TaskStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "TaskStatistics" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "RunTime/RTE_Types.h"  // just to get windows.h right...

#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_TaskStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"

#define KERNEL_LZU
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_Task.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_TaskStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatistics::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKID, 10);
	m_Table->AppendCol (ITOCT_CHAR ,SV_TASKTYPE, 32);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKGROUPID, 10);
	m_Table->AppendCol (ITOCT_CHAR ,SV_TASKSTATE, 16);
	m_Table->AppendCol (ITOCT_CHAR ,SV_RUNNABLE, 3);
	m_Table->AppendCol (ITOCT_CHAR ,SV_ACTIVE, 3);
	m_Table->AppendCol (ITOCT_CHAR ,SV_EXTENDEDTASKSTATE, 40);
	m_Table->AppendCol (ITOCT_CHAR ,SV_CANCELFLAG, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_REGIONID, 10);
	m_Table->AppendCol (ITOCT_CHAR  ,SV_READERWRITERLOCK, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITINGFORTASK, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITINGFORROOT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITINGFORPAGE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_APPLICATIONPROCESS, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_COMMUNICATIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_BYTESRECEIVED, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_BYTESREPLIED, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DISPATCHCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKRESCEDULECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_REGIONACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SUSPENDCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SUSPENDRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SUSPENDABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SLEEPCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SLEEPRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SLEEPABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_STATEMENTEXECUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_STATEMENTWAITRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_STATEMENTWAITABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFREADCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFREADPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFREADABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFWRITECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFWRITEPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFWRITEABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITEPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITERELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITEABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USABLESTACKSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDSTACKSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXUSEDSTACK, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_HOPCOUNT, 20);
	m_Table->AppendCol (ITOCT_CHAR ,SV_TASKGROUPBLOCKED, 3);
	m_Table->AppendCol (ITOCT_CHAR ,SV_OMSCALLACTIVE, 3);
	m_Table->AppendCol (ITOCT_CHAR ,SV_OMSPROCEDURENAME, 64);
	m_Table->AppendCol (ITOCT_FIXED ,SV_OMSCALLBACKCOUNT, 20);
	m_Table->AppendCol (ITOCT_CHAR ,SV_OMSCALLBACKMETHOD, 32);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_TASKSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatistics::EstimateRows", SysView_Trace, 5);

    return SV_ER_TASKSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_TaskStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatistics::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented

    /*
    SAPDB_UInt8             uint8val = 0;
    SAPDB_Int4              int4val  = 0;
    SAPDB_Int4              pid=1;
    RTETask_Task            *pTaskObject;
    SAPDB_Char              address[20];

    m_Table->GetCatalogTable();

    while(pTaskObject = RTETask_Task::ByTaskID(pid))
    {
        m_Table->MoveToCol (ITOVT_INT4 ,    &pid,                   0);                                         //SV_TASKID

        m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>(pTaskObject->TypeAsString()),          (SAPDB_Int)min(strlen((pTaskObject->TypeAsString())),32));             //SV_TASKTYPE

        m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_TASKGROUPID

        m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>(pTaskObject->StateAsString()),         (SAPDB_Int)min(strlen(pTaskObject->StateAsString()),16));            //SV_TASKSTATE

        m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>("SV_RUNNABLE"),          (SAPDB_Int)min(strlen("SV_RUNNABLE"),3));              //SV_RUNNABLE
	    m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>("SV_ACTIVE"),            (SAPDB_Int)min(strlen("SV_ACTIVE"),3));                //SV_ACTIVE
	    m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>("SV_EXTENDEDTASKSTATE"), (SAPDB_Int)min(strlen("SV_EXTENDEDTASKSTATE"),40));    //SV_EXTENDEDTASKSTATE
	    m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>("SV_CANCELFLAG"),        (SAPDB_Int)min(strlen("SV_CANCELFLAG"),3));            //SV_CANCELFLAG
	    
        int4val = pTaskObject->LegacyTaskCtrl().InRegionID();
        m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_REGIONID

        SAPDB_sprintf(address,sizeof(address),"0x%0p",pTaskObject->RWRegion());
        m_Table->MoveToCol (ITOVT_CHARPTR, address,(SAPDB_Int)strlen((SAPDB_Char *)address));                   //SV_READERWRITERLOCK

        m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_WAITINGFORTASK
	    m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_WAITINGFORROOT
	    m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_WAITINGFORPAGE
	    m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_APPLICATIONPROCESS
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_COMMUNICATIONCOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_BYTESRECEIVED
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_BYTESREPLIED
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_DISPATCHCOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_TASKRESCEDULECOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_REGIONACCESSCOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SUSPENDCOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SUSPENDRELATIVETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SUSPENDABSOLUTETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_WAITCOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_WAITRELATIVETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_WAITABSOLUTETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SLEEPCOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SLEEPRELATIVETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SLEEPABSOLUTETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_STATEMENTEXECUTETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_STATEMENTWAITRELATIVETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_STATEMENTWAITABSOLUTETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SELFREADCOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SELFREADPAGECOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SELFREADABSOLUTETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SELFWRITECOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SELFWRITEPAGECOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_SELFWRITEABSOLUTETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_READCOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_READPAGECOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_READRELATIVETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_READABSOLUTETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_WRITECOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_WRITEPAGECOUNT
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_WRITERELATIVETIME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_WRITEABSOLUTETIME
	    m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_USABLESTACKSIZE
	    m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_USEDSTACKSIZE
	    m_Table->MoveToCol (ITOVT_INT4 ,    &int4val,               0);                                         //SV_MAXUSEDSTACK
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_HOPCOUNT
	    m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>("SV_TASKGROUPBLOCKED"), (SAPDB_Int)min(strlen("SV_TASKGROUPBLOCKED"),3));       //SV_TASKGROUPBLOCKED
	    m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>("SV_OMSCALLACTIVE"), (SAPDB_Int)min(strlen("SV_OMSCALLACTIVE"),3));             //SV_OMSCALLACTIVE
	    m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>("SV_OMSPROCEDURENAME"), (SAPDB_Int)min(strlen("SV_OMSPROCEDURENAME"),64));      //SV_OMSPROCEDURENAME
	    m_Table->MoveToCol (ITOVT_INT8 ,    &uint8val,              0);                                         //SV_OMSCALLBACKCOUNT
	    m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>("SV_OMSCALLBACKMETHOD"), (SAPDB_Int)min(strlen("SV_OMSCALLBACKMETHOD"),32));    //SV_OMSCALLBACKMETHOD

        pid++;
    }
    */
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_TaskStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatistics::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
