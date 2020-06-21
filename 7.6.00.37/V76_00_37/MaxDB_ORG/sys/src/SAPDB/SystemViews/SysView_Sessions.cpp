/****************************************************************************/
/*!

  @file        SysView_Sessions.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "Sessions" view class.

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

#include "ggg00.h"
#include "hgg01_3.h" // vtrace
#include "hgg03.h"
#include "hsp78.h"
#include "geo003.h"

#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_Sessions.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_SessionIterator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "Container/Container_List.hpp"

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "Messages/Msg_List.hpp"

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

void SysView_Sessions::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Sessions::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED ,SV_SESSIONID, 10);
    m_Table->AppendCol (ITOCT_CHAR ,SV_OWN, 3);
	m_Table->AppendCol (ITOCT_CHAR ,SV_SESSIONTYPE, 4);
	m_Table->AppendCol (ITOCT_CHAR ,SV_ENCRYPTION, 3);
	m_Table->AppendCol (ITOCT_TIMESTAMP ,SV_STARTDATE, 0);
	m_Table->AppendCol (ITOCT_CHAR ,SV_APPLICATIONNODE, 64);
	m_Table->AppendCol (ITOCT_FIXED ,SV_APPLICATIONPROCESS, 10);
	m_Table->AppendCol (ITOCT_CHAR ,SV_APPLICATIONTYPE, 3);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_USERNAME, 32);
	m_Table->AppendCol (ITOCT_FIXED ,SV_PEERPROCESSID, 10);
 	m_Table->AppendCol (ITOCT_CHAR ,SV_CONNECTSTATE, 32);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKID, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_OMSHEAPUSEDSIZE, 20);
	m_Table->AppendCol (ITOCT_CHAR ,SV_TRACE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CATALOGCACHEUSEDSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDTEMPORARYSIZE, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_PAGINGFILEUSEDSIZE, 10);
	m_Table->AppendCol (ITOCT_CHAR ,SV_SQLMODE, 8);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ISOLATIONLEVEL, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TIMEOUT, 10);
	m_Table->AppendCol (ITOCT_CHAR ,SV_DATEFORMAT, 50);
	m_Table->AppendCol (ITOCT_CHAR ,SV_DEFAULTCODE, 8);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Sessions::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Sessions::GetColCount", SysView_Trace, 5);

    return SV_CC_SESSIONS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Sessions::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Sessions::EstimateRows", SysView_Trace, 5);

    return SV_ER_SESSIONS;
}

/*---------------------------------------------------------------------------*/

/*!
  @brief collects rows for SYSDD.SESSIONS
 */
class SysView_ResultCollector
{
public :
    /*!
       @brief constructor
     */
    SysView_ResultCollector (SQLMan_Context& Context, SysView_Sessions& SessionView);
    /*!
       @brief adds one row to the collection
     */
    void Append (SQLMan_SessionIterator& sessionIt);
    /*!
       @brief inserts all collected rows into the result set
     */
    void Flush();
    /*!
       @brief destrcutor, inserts all remaining rows into the result set 
     */
    ~SysView_ResultCollector();
private :
    
    struct SessionsRow
    {
        SAPDB_Int4                m_sessionNo;
        bool                      m_own;
        RTETask_ITaskClientConnection::ConnectionType               m_SessionType;  
        RTETask_ITaskClientConnection::EncryptionType               m_Encryption;
        SAPDB_Bool                m_applOk;
        bool                      m_vtrace;
        tsp00_SqlMode_Enum        m_sqlmode;
        SAPDB_Int4                m_sessionStartDate;
        SAPDB_Int4                m_sessionStartTime;
        tsp00_NodeId              m_applNode;
        tsp00_Int4                m_applPid;
        tak_appl_type             m_appl_type;
        SQLMan_Identifier         m_currUser;
        tsp00_TaskId              m_taskId;
        SAPDB_Int4                m_catalogCacheSize;
        SAPDB_Int4                m_tempSize;
        SAPDB_Int4                m_parsFileSize;
        SAPDB_UInt1               m_isoLevel;
        SAPDB_Int2                m_userTimeout;
        tgg00_DateTimeFormat_Enum m_dtFormat;
        SAPDB_Int2                m_dfLength;
        char                      m_dateFormat[50];
        SAPDB_UInt1               m_defaultCode;
    };

    typedef Container_List<SessionsRow> SessionRowList;

    SQLMan_Context&   m_context;
    SysView_Sessions& m_sessionView;
    SessionRowList    m_rowList;
};

/*---------------------------------------------------------------------------*/

SysView_ResultCollector::SysView_ResultCollector (SQLMan_Context& Context, SysView_Sessions& SessionView)
: m_context     (Context)
, m_sessionView (SessionView)
, m_rowList     (Context.GetAllocator())  
{
}

/*---------------------------------------------------------------------------*/

void SysView_ResultCollector::Append (
    SQLMan_SessionIterator& sessionIt)
{
    if (m_context.IsOk())
    {
        SessionsRow      row;
        tgg91_SessionNo  sessionNo;
        sessionIt.GetSessionId( sessionNo );
        row.m_sessionNo = 256 * (256 * (256 * sessionNo[0] + sessionNo[1]) + sessionNo[2]) + sessionNo[3];
        row.m_own = (sessionIt.IsSameSession(m_context));
        row.m_SessionType = sessionIt.GetSessionType ();
        row.m_Encryption = sessionIt.GetEncryptionType ();
        row.m_vtrace = (g01vtrace.vtrAny_gg00 || (g01vtrace.vtrSession_gg00 == sessionNo));

        row.m_sqlmode = sessionIt.GetSqlMode();

        row.m_sessionStartDate = sessionIt.GetStartDate();
        row.m_sessionStartTime = sessionIt.GetStartTime();
        
        row.m_applOk = sessionIt.GetApplicationInfo(row.m_applNode, row.m_applPid);
        row.m_appl_type = sessionIt.GetApplicationType();
        sessionIt.GetSessionUserName(row.m_currUser);
        row.m_taskId    = sessionIt.GetTaskId();
        int kbFactor = PAGE_MXSP00 / 1024;
        row.m_catalogCacheSize = sessionIt.GetCatalogCacheSize() * kbFactor;
        row.m_tempSize         = sessionIt.GetTempPageCount(m_context) * kbFactor;

        Msg_List errMsg;
        FileDir_FileNo parseFileNo(sessionIt.GetParseFileNo());
        FileDir_Generic parseFile;
        sessionIt.GetFile(parseFileNo, parseFile, errMsg);
        if ( parseFile.IsAssigned() )
        {
            row.m_parsFileSize = (parseFile.GetLeafPageCount(m_context.TransContext().trTaskId_gg00)
                + parseFile.GetIndexPageCount(m_context.TransContext().trTaskId_gg00))
                * kbFactor;
        }
        else
        {
            row.m_parsFileSize = -1;
        }
        row.m_isoLevel    = sessionIt.GetIsolationLevel();
        row.m_userTimeout = sessionIt.GetUserTimeout();
        if (sqlm_oracle == sessionIt.GetSqlMode())
        {
            SQLMan_NlsParams nlsParams;
            sessionIt.GetNlsParams(nlsParams);
            row.m_dfLength = nlsParams.df_length;
            s78df_clear(g03dictionary.dict[1],
                (tsp00_MoveObjPtr) &nlsParams.date_format[0], nlsParams.df_length,
                (tsp00_MoveObjPtr) &row.m_dateFormat[0], sizeof(row.m_dateFormat));
        }
        else
        {
            row.m_dfLength = -1;
            row.m_dtFormat = sessionIt.GetDateTimeFormat();
        }

        row.m_defaultCode = sessionIt.GetDefaultCode ();

        if (!m_rowList.InsertEnd(row).IsValid())
        {
            Msg_List errMsg;
            m_context.SetNoMoreMemoryError(errMsg);
        }
    }
}

/*---------------------------------------------------------------------------*/

void SysView_ResultCollector::Flush()
{
    if (m_context.IsOk())
    {
        SessionRowList::Iterator iter = m_rowList.Begin();
        SessionRowList::Iterator end  = m_rowList.End();
        while (iter != end)
        {
            m_sessionView.AddColumn(iter->m_sessionNo);
            m_sessionView.m_Table->MoveToCol (ITOVT_YESNO, &iter->m_own, 0);

            switch (iter->m_SessionType)
            {
            case RTETask_ITaskClientConnection::OLTP:
            case RTETask_ITaskClientConnection::LiveCache:
                m_sessionView.AddColumn (SAPDB_ToString (iter->m_SessionType));
                break;
            default:
                m_sessionView.AddColumn ();
            }

            switch (iter->m_Encryption)
            {
            case RTETask_ITaskClientConnection::NoEncryption:
            case RTETask_ITaskClientConnection::NISSL:
                m_sessionView.AddColumn (SAPDB_ToString (iter->m_Encryption));
                break;
            default:
                m_sessionView.AddColumn ();
            }

            if (iter->m_sessionStartDate > 0)
            {
                char tsString[TIMESTAMP_MXSP00+1];
                memset (tsString, '0', TIMESTAMP_MXSP00);
                tsString[TIMESTAMP_MXSP00] = 0;
                SAPDB_Int8 ts = ((SAPDB_Int8) iter->m_sessionStartDate * 1000000) + iter->m_sessionStartTime;
                SAPDB_ToStringClass str = ToStr(ts);
                SAPDB_MemCopyNoCheck (tsString, (char*) str, strlen(str));
                m_sessionView.AddColumn(tsString);
            }
            else
            {
                m_sessionView.AddColumn();
            }
            if (iter->m_applOk)
            {
                m_sessionView.AddColumn((SAPDB_Char*)&iter->m_applNode, sizeof(iter->m_applNode));
                m_sessionView.AddColumn(iter->m_applPid);
            }
            else
            {
                m_sessionView.AddColumn();
                m_sessionView.AddColumn();
            }

            switch (iter->m_appl_type)
            {
            case at_cpc :
                m_sessionView.AddColumn (csp_comp_cpc);
                break;
            case at_cpp :
                m_sessionView.AddColumn (csp_comp_cpp);
                break;
            case at_db_manager :
                m_sessionView.AddColumn (csp_comp_db_manager);
                break;
            case at_gpc :
                m_sessionView.AddColumn (csp_comp_gpc );
                break;
            case at_jdbc :
                m_sessionView.AddColumn (csp_comp_jdbc);
                break;
            case at_load :
                m_sessionView.AddColumn (csp_comp_load);
                break;
            case at_odbc :
                m_sessionView.AddColumn (csp_comp_odbc);
                break;
            case at_util :
                m_sessionView.AddColumn (csp_comp_util);
                break;
            case at_xci :
                m_sessionView.AddColumn (csp_comp_xci);
                break;
            default :
                m_sessionView.AddColumn("UNKNOWN");
            }

            m_sessionView.m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &iter->m_currUser[0], 0);
            m_sessionView.AddColumn();
            m_sessionView.AddColumn();
            m_sessionView.AddColumn(iter->m_taskId);
            m_sessionView.AddColumn();
            m_sessionView.m_Table->MoveToCol (ITOVT_YESNO, &iter->m_vtrace, 0);
            m_sessionView.AddColumn(iter->m_catalogCacheSize);
            if ( iter->m_tempSize >= 0 )
            {
                m_sessionView.AddColumn(iter->m_tempSize);
            }
            else
            {
                m_sessionView.AddColumn();
            }
            if (iter->m_parsFileSize >= 0)
            {
                m_sessionView.AddColumn(iter->m_parsFileSize);
            }
            else
            {
                m_sessionView.AddColumn();
            }
            m_sessionView.m_Table->MoveToCol (ITOVT_SQLMODE, &iter->m_sqlmode, 0);
            m_sessionView.AddColumn(iter->m_isoLevel);
            m_sessionView.AddColumn(iter->m_userTimeout);
            if (iter->m_dfLength >= 0)
            {
                m_sessionView.m_Table->MoveToCol (ITOVT_CHARPTR,  (SAPDB_Char *) &iter->m_dateFormat[0], sizeof(iter->m_dateFormat));
            }
            else
            {
                m_sessionView.m_Table->MoveToCol (ITOVT_DATETIMEFORMAT, &iter->m_dtFormat, 0);
            }

            switch (iter->m_defaultCode)
            {
            case csp_ascii:
                m_sessionView.AddColumn ("ASCII");
                break;
            case csp_unicode:
                m_sessionView.AddColumn ("UNICODE");
                break;
            default:
                m_sessionView.AddColumn ();
            }


            ++iter;
        }
    }
    m_rowList.Delete();
}

/*---------------------------------------------------------------------------*/

SysView_ResultCollector::~SysView_ResultCollector()
{
    this->Flush();
}

/*---------------------------------------------------------------------------*/

void SysView_Sessions::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Sessions::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();
    SQLMan_SessionIterator sessionIterator;
    SysView_ResultCollector collector (m_Context, *this);
    do
    {
       if (sessionIterator.Next())
       {
           collector.Append (sessionIterator);

           if (sessionIterator.Unlock())
           {
               collector.Flush();
           }
       }
       else
       {
           break;
       }
    }
    while (m_Context.IsOk());
    collector.Flush();
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
