/*!
  \file    Loader_FastloadCommand.cpp
  \author  SteffenS
  \brief   Implementation of FASTLOAD command

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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
*/
#include <stdio.h>
//time#include <time.h>

#include "Loader/Loader_Common.hpp"             // Table information structure tsp7_table_info  
#include "Loader/Loader_Types.hpp"             // Table information structure tsp7_table_info  

#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_ErrorMsgHandler.hpp"
#include "Loader/Loader_Loader.hpp"

#include "PacketInterface/PIn_SpecialParts.h"

#include "Interfaces/SQLDBC/SQLDBC.h"
#include "Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"  // unicode support

#include "Loader/Loader_StringConversion.hpp"
#include "Loader/Loader_PageBuilder.hpp"

#include "Loader/Loader_FastloadCommandSupport.hpp"


#include "PacketInterface/PIn_Packet.h"


#define VSP100_CPP
#include "gsp100.h"                                     // version string

using namespace SQLDBC;


/*
 *  The constructor
 */
Loader_FastloadCommand::Loader_FastloadCommand(SQLDBC_IRuntime* pLoaderRuntime, Loader_Connection* pConnection):
m_PacketInitBlock(NULL),
m_lReqPacketNo(0),
m_lRcvPacketNo(0),
m_PacketInitBlockLength(0),
m_StartPosInBuffer(0),
m_PagePartPayload(0),
m_BDINFO_Length(0),
m_pTableDescription(NULL),
m_ParamsSpecifiedCnt(0),
m_StampDefined(LOADER_FALSE),
m_PagesPerPacket(0),
m_RecordLength(0),
m_FixColumnsLength(0),
m_VarColumnsCount(0),
m_TableHasSyskey(LOADER_FALSE),
m_TableIsEmptyAtBeginLoad(LOADER_FALSE),
m_LastKeyLength(0),
m_UserDefault(NULL),
m_UserGroupDefault(NULL),
m_SYSDBADefault(NULL),
m_ParamBindingType(0),
m_BatchSize(0),
m_ParamList(NULL),
m_ParameterCount(0),
m_pPageBuilder(NULL),
m_RowStatusArray(NULL),
m_pError(NULL),
m_pSQLDBC_RT_Error(NULL),
m_bKeepPageBuilderObject(LOADER_FALSE)
//time,receive_duration(0)
//time,pagebuild_duration(0)
//time,overall_duration(0)
/*m_RunTime(0),
m_BindTime(0),
m_ExecuteTime(0),
m_Start(0),
m_End(0)*/
{
    m_pLoaderRuntime    = pLoaderRuntime;
    m_pConnection       = pConnection;

    m_ReqPackets[0]     = m_pConnection->m_PacketList[0];       // are not changable
    m_ReqPackets[1]     = m_pConnection->m_PacketList[1];
    m_RcvPackets[0]     = 0;                                    // will be assigned at receive time
    m_RcvPackets[1]     = 0;

    m_UserDefault       = m_pConnection->m_UserName;
    m_UserGroupDefault  = m_pConnection->m_pszUserGroupDefault;
    m_SYSDBADefault     = m_pConnection->m_SYSDBADefault;

    m_LastKey[0]        = 0;
    m_StampDefault[0]   = 0;

    m_pError            = new Loader_ErrorMsgHandler;
    m_pSQLDBC_RT_Error  = new SQLDBC_IRuntime::Error;

    m_pTableDescription = new Loader_TableDescription;
    memset(m_pTableDescription, 0, sizeof(Loader_TableDescription));

    //m_RunTime += getRunTime(m_Start);

//tracing    m_pLoaderRuntime->setLoaderTraceLevel(LOADER_TRACE_LEVEL_2);
    //if (NULL != m_pLoaderRuntime)
    //{
    //    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, Loader_FastloadCommand, m_pLoaderRuntime->m_pClientTrace);
    //    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "ptr to Loader Runtime not null" << endl;
    //}
#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "ptr to Loader Runtime: " << reinterpret_cast<LOADER_Int4>(m_pLoaderRuntime) << endl;
    
    if (NULL != m_pConnection)
    {
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "ptr to Connection Object not null" << endl;
    }
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "ptr to Connection Object: " << reinterpret_cast<LOADER_Int4>(m_pConnection) << endl;
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "ptr to 1st packet: " << reinterpret_cast<LOADER_Int4>(m_ReqPackets[0]) << endl;
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "ptr to 2nd packet: " << reinterpret_cast<LOADER_Int4>(m_ReqPackets[1]) << endl;
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "User default: " << m_UserDefault << endl;
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "User group default: " << m_UserGroupDefault << endl;
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Sysdba default: " << m_SYSDBADefault << endl;
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Error handler created" << endl;
#endif // LOADER_TRACING
}


/*
 *  The destructor
 */
Loader_FastloadCommand::~Loader_FastloadCommand()
{
    //setStartTime(m_Start);

    removeInternalObjects();
    //Loader_TableDescription* pTableDesc = static_cast<Loader_TableDescription*>(this->m_pTableDescription);
    //LOADER_Parameter*        pPL        = static_cast<LOADER_Parameter*>(m_ParamList);
    //if (NULL != pTableDesc)
    //{
    //    if (NULL != pTableDesc->pszTableName)
    //    {
    //        delete [] pTableDesc->pszTableName;
    //        pTableDesc->pszTableName = NULL;
    //    }

    //    LOADER_Int2 i = 0;
    //    for (i = 0; i < pTableDesc->lColumnCount; ++i)
    //    {
    //        if (NULL != pTableDesc->pszColumnNames[i])
    //        {
    //            delete [] pTableDesc->pszColumnNames[i];
    //            pTableDesc->pszColumnNames[i] = NULL;
    //        }

    //        if (NULL != pTableDesc->pColumnInfoArray[i].DefaultValue.pszStringData)
    //        {
    //            delete [] pTableDesc->pColumnInfoArray[i].DefaultValue.pszStringData;
    //        }
    //    }
    //    delete [] pTableDesc->pszColumnNames;
    //    delete [] pTableDesc->pColumnInfoArray;

    //    for (i = 0; i < this->m_ParameterCount; ++i)
    //    {
    //        if (NULL != pPL[i].LengthIndicator)
    //        {
    //            if (LOADER_DEFAULT_DATA == *pPL[i].LengthIndicator)
    //            {
    //                delete pPL[i].LengthIndicator;
    //                pPL[i].LengthIndicator = NULL;
    //            }
    //        }
    //    }

    //    pTableDesc = NULL;
    //    delete static_cast<Loader_TableDescription*>(m_pTableDescription);
    //}
    //if (NULL != m_ParamList)
    //{
    //    delete [] static_cast<LOADER_Parameter*>(m_ParamList);
    //    m_ParamList = NULL;
    //}

    //if (NULL != m_pPageBuilder)
    //{
    //    delete m_pPageBuilder;
    //    m_pPageBuilder = NULL;
    //}

    //if (NULL != m_pError)
    //{
    //    delete m_pError;
    //    m_pError = NULL;
    //}

    //if (NULL != m_pSQLDBC_RT_Error)
    //{
    //    delete m_pSQLDBC_RT_Error;
    //    m_pSQLDBC_RT_Error = NULL;
    //}

    //if (NULL != m_RowStatusArray)
    //{
    //    delete m_RowStatusArray;
    //    m_RowStatusArray = NULL;
    //}


    //if (NULL != m_PacketInitBlock)
    //{
    //    delete [] m_PacketInitBlock;
    //    m_PacketInitBlock = NULL;
    //}







    //m_RunTime += getRunTime(m_Start);
    //m_RunTime += m_BindTime + m_ExecuteTime;

    //m_pLoaderRuntime->setLoaderTraceLevel(LOADER_TRACE_LEVEL_2);
    //SAPDB_Char s[64];
    //sp77sprintf(s, 64, "%f", (double)m_RunTime/1000000);
    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "run time:" << s << endl;

    //sp77sprintf(s, 64, "%f", (double)m_BindTime/1000000);
    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "bind time:" << s << endl;

    //sp77sprintf(s, 64, "%f", (double)m_ExecuteTime/1000000);
    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "exe time:" << s << endl;


    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "bind called: " << m_BindCalled << endl;;
    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "execute called: " << m_ExecuteCalled << endl;
}
// Loader_FastloadCommand::~Loader_FastloadCommand()



void
Loader_FastloadCommand::removeInternalObjects()
{
    Loader_TableDescription* pTableDesc = static_cast<Loader_TableDescription*>(this->m_pTableDescription);
    LOADER_Parameter*        pPL        = static_cast<LOADER_Parameter*>(m_ParamList);
    if (NULL != pTableDesc)
    {
        if (NULL != pTableDesc->pszTableName)
        {
            delete [] pTableDesc->pszTableName;
            pTableDesc->pszTableName = NULL;
        }

        LOADER_Int2 i = 0;
        for (i = 0; i < pTableDesc->lColumnCount; ++i)
        {
            if (NULL != pTableDesc->pszColumnNames[i])
            {
                delete [] pTableDesc->pszColumnNames[i];
                pTableDesc->pszColumnNames[i] = NULL;
            }

            if (NULL != pTableDesc->pColumnInfoArray[i].DefaultValue.pszStringData)
            {
                delete [] pTableDesc->pColumnInfoArray[i].DefaultValue.pszStringData;
            }
        }
        delete [] pTableDesc->pszColumnNames;
        delete [] pTableDesc->pColumnInfoArray;

        for (i = 0; i < this->m_ParameterCount; ++i)
        {
            if (NULL != pPL[i].LengthIndicator)
            {
                if (LOADER_DEFAULT_DATA == *pPL[i].LengthIndicator)
                {
                    delete pPL[i].LengthIndicator;
                    pPL[i].LengthIndicator = NULL;
                }
            }
        }

        pTableDesc = NULL;
        delete static_cast<Loader_TableDescription*>(m_pTableDescription);
        m_pTableDescription = NULL;
    }
    if (NULL != m_ParamList)
    {
        delete [] static_cast<LOADER_Parameter*>(m_ParamList);
        m_ParamList = NULL;
    }

    if (NULL != m_pPageBuilder)
    {
        delete m_pPageBuilder;
        m_pPageBuilder = NULL;
    }

    if (NULL != m_pError)
    {
        delete m_pError;
        m_pError = NULL;
    }

    if (NULL != m_pSQLDBC_RT_Error)
    {
        delete m_pSQLDBC_RT_Error;
        m_pSQLDBC_RT_Error = NULL;
    }

    if (NULL != m_RowStatusArray)
    {
        delete m_RowStatusArray;
        m_RowStatusArray = NULL;
    }

    if (NULL != m_PacketInitBlock)
    {
        delete [] m_PacketInitBlock;
        m_PacketInitBlock = NULL;
    }
}
// Loader_FastloadCommand::removeInternalObjects()


void
Loader_FastloadCommand::initializeValues()
{
    removeInternalObjects();

    if (NULL == m_pError)
    {
        m_pError = new Loader_ErrorMsgHandler;
    }

    if (NULL == m_pSQLDBC_RT_Error)
    {
        m_pSQLDBC_RT_Error = new SQLDBC_IRuntime::Error;
    }

    if (NULL == m_pTableDescription)
    {
        m_pTableDescription = new Loader_TableDescription;
        memset(m_pTableDescription, 0, sizeof(Loader_TableDescription));
    }

    //m_PacketInitBlock = NULL;                     // done in removeInternalObjects()
    m_PacketInitBlockLength     = 0;
    m_lReqPacketNo              = 0;
    m_lRcvPacketNo              = 0;
    m_StartPosInBuffer          = 0;
    m_PagePartPayload           = 0;
    m_BDINFO_Length             = 0;

    m_ParamsSpecifiedCnt        = 0;
    m_StampDefined              = LOADER_FALSE;
    m_PagesPerPacket            = 0;
    m_RecordLength              = 0;
    m_FixColumnsLength          = 0;
    m_VarColumnsCount           = 0;
    m_TableHasSyskey            = LOADER_FALSE;
    m_TableIsEmptyAtBeginLoad   = LOADER_FALSE;
    m_LastKeyLength             = 0;
    //m_UserDefault                                 // once assigned in the constructor is does not need to be assigned again for the same connection
    //m_UserGroupDefault                            // once assigned in the constructor is does not need to be assigned again for the same connection
    //m_SYSDBADefault                               // once assigned in the constructor is does not need to be assigned again for the same connection
    m_ParamBindingType          = 0;
    m_BatchSize                 = 0;
    //m_ParamList         // done in removeInternalObjects()
    m_ParameterCount            = 0;
    //m_pPageBuilder                                // done in removeInternalObjects()
    //m_RowStatusArray                              // done in removeInternalObjects()
    m_bKeepPageBuilderObject    = LOADER_FALSE;
}
// Loader_FastloadCommand::initializeValues()


// Omitting either pszColumnNames OR lColumnNameCount is treated as an error; either both are specified or not
LOADER_Retcode
Loader_FastloadCommand::initialize(const LOADER_Char*           pszTableName,
                                   LOADER_Char**                pszColumnNames,
                                   LOADER_UInt4                 lColumnNameCount,
                                   const LOADER_StringEncoding  encoding)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, initialize, m_pLoaderRuntime->m_pClientTrace);
#endif

    // Do we have a connection?
    if (SAPDB_FALSE == this->m_pConnection->isConnected())
    {
        m_pError->setLoaderError(LOA_ERR_NO_CONNECTION);
        return LOADER_NOT_OK;
    }

    initializeValues();


    //setStartTime(m_Start);
    LOADER_Int4 rc = LOA_ERR_OK;

    Loader_TableDescription* pTableDesc = static_cast<Loader_TableDescription*>(this->m_pTableDescription);

    if ( ( (0 == lColumnNameCount) && (NULL != pszColumnNames) ) ||
         ( (0 != lColumnNameCount) && (NULL == pszColumnNames) ) )
    {
        m_pError->setLoaderError(LOA_ERR_COLUMN_SPEC_MISSING);
        return LOADER_NOT_OK;
    }

#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Column Name Count:" << lColumnNameCount << endl;
#endif

    if (NULL == pszTableName)
    {
        m_pError->setLoaderError(LOA_ERR_COLUMN_SPEC_MISSING);
        return LOADER_NOT_OK;
    }

    // Convert table name into UTF8 string
    rc = Loader_StrConvert(&pTableDesc->pszTableName, LOADER_STRINGENCODING_UTF8, pszTableName, -1, encoding);

#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Table Name:" << pTableDesc->pszTableName << endl;
#endif

    //*
    //*     Define count of specified parameters
    //*
    m_ParamsSpecifiedCnt = lColumnNameCount;
  
    if (LOA_ERR_OK == rc)
    {
        rc = getColumnInfosForFastload(pszColumnNames, lColumnNameCount, encoding);
    }
    
    if (LOA_ERR_OK == rc)
    {
        rc = checkForLongColumns();
    }

    if (LOA_ERR_OK == rc)
    {
        rc = getUserDefinedDefaultValues();
    }

    if (LOA_ERR_OK == rc)
    {
        getRecordParameters();
    }

    if (LOA_ERR_OK == rc)
    {
        rc = BeginLoad();
    }

    if (LOA_ERR_OK == rc)
    {
        //*
        //*     Build parameter list
        //*
        m_ParameterCount = pTableDesc->lColumnCount;
        if (LOADER_TRUE == m_TableHasSyskey)
        {
            --m_ParameterCount;
        }

        m_ParamList = new LOADER_Parameter[m_ParameterCount];
        memset(m_ParamList, 0, (sizeof(LOADER_Parameter)*m_ParameterCount));

        LOADER_SQLColumnInfo* pCI = NULL;

        // Fuer alle Spalten, die nicht vom Benutzer angegeben wurden (wenn keine Spalten angegeben
        // wurden, wird das behandelt, als waeren alle Spalten angegeben worden, d.h. fuer alle Spalten
        // muessen vom Benutzer Daten geliefert werden), wird der LengthIndicator DEFAULT eingetragen. Diese
        // Spalten erlauben NULL oder den DEFAULT und muessen daher nicht zwingend angegeben werden.
        LOADER_Int2 colidx   = (LOADER_FALSE == m_TableHasSyskey) ? 0 : 1;
        LOADER_Int2 paramidx = 0;
        for (colidx, paramidx; colidx < pTableDesc->lColumnCount; ++colidx, ++paramidx)
        {
            pCI = &pTableDesc->pColumnInfoArray[colidx];

            if ( (LOADER_SQLDEFAULT_NONE != pCI->DefaultType) && (LOADER_SQLDEFAULT_OTHER != pCI->DefaultType) )
            {
                if (LOADER_SQLDEFAULT_USER == pCI->DefaultType)
                {
                    pCI->DefaultValue.lStringLength = strlen(m_UserDefault);
                    pCI->DefaultValue.pszStringData = new LOADER_Char[pCI->DefaultValue.lStringLength + sizeof(LOADER_Char)];
                    SAPDB_memcpy(pCI->DefaultValue.pszStringData, m_UserDefault, pCI->DefaultValue.lStringLength);
                    pCI->DefaultValue.pszStringData[pCI->DefaultValue.lStringLength] = 0;
                }
                else if (LOADER_SQLDEFAULT_USERGROUP == pCI->DefaultType)
                {
                    pCI->DefaultValue.lStringLength = strlen(m_UserGroupDefault);
                    pCI->DefaultValue.pszStringData = new LOADER_Char[pCI->DefaultValue.lStringLength + sizeof(LOADER_Char)];
                    SAPDB_memcpy(pCI->DefaultValue.pszStringData, m_UserGroupDefault, pCI->DefaultValue.lStringLength);
                    pCI->DefaultValue.pszStringData[pCI->DefaultValue.lStringLength] = 0;
                }
                else if (LOADER_SQLDEFAULT_STAMP == pCI->DefaultType)
                {
                    pCI->DefaultValue.lStringLength = sizeof(m_StampDefault);
                    pCI->DefaultValue.pszStringData = new LOADER_Char[pCI->DefaultValue.lStringLength + sizeof(LOADER_Char)];
                    SAPDB_memcpy(pCI->DefaultValue.pszStringData, m_StampDefault, pCI->DefaultValue.lStringLength);
                }
                else if (LOADER_SQLDEFAULT_SYSDBA == pCI->DefaultType)
                {
                    pCI->DefaultValue.lStringLength = strlen(m_SYSDBADefault);
                    pCI->DefaultValue.pszStringData = new LOADER_Char[pCI->DefaultValue.lStringLength + sizeof(LOADER_Char)];
                    SAPDB_memcpy(pCI->DefaultValue.pszStringData, m_SYSDBADefault, pCI->DefaultValue.lStringLength);
                    pCI->DefaultValue.pszStringData[pCI->DefaultValue.lStringLength] = 0;
                }
            }

            if ( (0 == pCI->StmtColumnNo) && 
                 ( (LOADER_COLMODE_OPTIONAL == pCI->ColumnMode) || (LOADER_SQLDEFAULT_NONE != pCI->DefaultType) ) )
            {
                static_cast<LOADER_Parameter*>(m_ParamList)[paramidx].LengthIndicator = new LOADER_Length;
                *(static_cast<LOADER_Parameter*>(m_ParamList)[paramidx].LengthIndicator) = LOADER_DEFAULT_DATA;
            }
        }   // end for (colidx, paramidx; colidx < pTableDesc->lColumnCount; ++colidx, ++paramidx)
#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "built parameter list" << endl;
#endif
    }

    // Set value for m_PagesPerPacket
    //  tls00_PktBlock contains packet-, segment- and part header size. Because each packet
    //  consists of 2 parts the part header has to be substracted again.
    if (LOA_ERR_OK == rc)
    {
        m_PagesPerPacket = (m_pConnection->m_PacketSize
                            - (sizeof(tsp1_packet_header) + sizeof(tsp1_segment_header) + sizeof(tsp1_part_header)) // =sizeof(tls00_PktBlock)
                            - sizeof(tsp1_part_header)
                            - m_BDINFO_Length)/m_pConnection->m_PageSize;
#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "no pages per packet: " << m_PagesPerPacket << endl;
#endif

        configurePageBuilder();

        m_lSndPcktCnt = 0;
    }

    //m_RunTime += getRunTime(m_Start);

    // Initialize packet for pages
    InitPagePacket(0);

    return (LOA_ERR_OK != rc) ? LOADER_NOT_OK : LOADER_OK;
}
// initialize()


LOADER_Retcode
Loader_FastloadCommand::getColumnCount(LOADER_UInt4& lColumnCount)
{
    //setStartTime(m_Start);
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getColumnCount, m_pLoaderRuntime->m_pClientTrace);
#endif

    if (NULL == m_ParamList)
    {
        // No initialize done - return error
        m_pError->setLoaderError(LOA_ERR_CMD_NOT_INITIALIZED);
        return LOADER_NOT_OK;
    }
    lColumnCount = m_ParameterCount;

    //m_RunTime += getRunTime(m_Start);

    return LOADER_OK;
}
// getColumnCount()


LOADER_Retcode
Loader_FastloadCommand::getColumnNames(LOADER_Char** ppszNameBuffer, const LOADER_UInt4 lBufferElementLength, const LOADER_StringEncoding encoding)
{
    //setStartTime(m_Start);

#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getColumnNames, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_ErrorCode rc = LOA_ERR_OK;

    if (NULL == m_ParamList)
    {
        // No initialize done - return error
        m_pError->setLoaderError(LOA_ERR_CMD_NOT_INITIALIZED);
        return LOADER_NOT_OK;
    }

    LOADER_UInt4 _len       = 0;
    LOADER_UInt4 _destlen   = 0;
    LOADER_Int4  _bufidx    = 0;
    LOADER_Int4  _paramidx  = 0;
    Tools_DynamicUTF8String szUTF8String;

    if (LOADER_TRUE == this->m_TableHasSyskey)
    {
        ++_paramidx;
    }

    Loader_TableDescription* pTableDesc = static_cast<Loader_TableDescription*>(m_pTableDescription);
    for (_paramidx; _paramidx < pTableDesc->lColumnCount; ++_paramidx, ++_bufidx)
    {
        _len         = strlen(pTableDesc->pszColumnNames[_paramidx]);      // we're sure it's UTF8
        _destlen     = lBufferElementLength;
        szUTF8String = reinterpret_cast<SAPDB_UTF8*>(pTableDesc->pszColumnNames[_paramidx]);
        
        rc = Loader_StrConvertFromUTF8(reinterpret_cast<LOADER_Byte*>(ppszNameBuffer[_bufidx]),
                                       _destlen,
                                       szUTF8String,
                                       encoding,
                                       m_pszErrText);
    }

    //m_RunTime += getRunTime(m_Start);

    return LOADER_OK;
}
// getColumnNames()


LOADER_UInt4
Loader_FastloadCommand::getCountofRecordsOnPage()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getCountofRecordsOnPage, m_pLoaderRuntime->m_pClientTrace);
#endif

    return (m_pConnection->m_PageSize - sizeof(tbd_node_header) - sizeof(tgg00_PageCheckTrailer))/(m_RecordLength + POINTERSIZE_BD00);
}
// getCountofRecordsOnPage()


LOADER_UInt4
Loader_FastloadCommand::getPreferredBatchSize()
{
    if (0 != m_RecordLength)
    {
        return m_PagesPerPacket * getCountofRecordsOnPage();
    }
    return 0;
}
//getPreferredBatchSize()


LOADER_Retcode
Loader_FastloadCommand::setBindingType(LOADER_UInt4 Size, LOADER_Bool bFixedLength)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, setBindingType, m_pLoaderRuntime->m_pClientTrace);
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "binding type: " << Size << endl;
#endif

    m_ParamBindingType = Size;
    m_ParamLengthIsFix = bFixedLength;

    return LOADER_OK;
}
// setBindingType()


LOADER_Retcode
Loader_FastloadCommand::SetBatchSize(LOADER_UInt4 ArraySize)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, SetBatchSize, m_pLoaderRuntime->m_pClientTrace);
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "batch size: " << ArraySize << endl;
#endif
    
    m_BatchSize = ArraySize;

    if (NULL != m_RowStatusArray)
    {
        delete m_RowStatusArray;
    }
    m_RowStatusArray = new LOADER_Int4[m_BatchSize];

    return LOADER_OK;
}
// SetBatchSize()


const LOADER_Int4*
Loader_FastloadCommand::getRowStatus() const
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getRowStatus, m_pLoaderRuntime->m_pClientTrace);
#endif

    return m_RowStatusArray;
}
// getRowStatus()



const LOADER_Char*
Loader_FastloadCommand::getErrorText() const
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getErrorText, m_pLoaderRuntime->m_pClientTrace);
#endif

    return m_pError->getErrorText();
}


LOADER_Int4
Loader_FastloadCommand::getErrorCode()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getErrorCode, m_pLoaderRuntime->m_pClientTrace);
#endif

    return m_pError->getErrorCode();
}

/*
 *  function: bindParameter
 */
LOADER_Retcode
Loader_FastloadCommand::bindParameter(const LOADER_UInt2 ParameterNumber,
                                      const LOADER_UInt2 ParameterType,
                                      void*              paramAddr,
                                      LOADER_Length*     LengthIndicator,
                                      LOADER_Length      Size)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, bindParameter, m_pLoaderRuntime->m_pClientTrace);
#endif

    //setStartTime(m_Start);
    //++m_BindCalled;

    LOADER_Retcode rc = LOADER_OK;

    if (NULL == m_ParamList)
    {
        // No initialize done - return error
        m_pError->setLoaderError(LOA_ERR_CMD_NOT_INITIALIZED);
        rc = LOADER_NOT_OK;
    }
    else
    {
        LOADER_Parameter* _pParamList = static_cast<LOADER_Parameter*>(m_ParamList);

        // Check for right parameter number
        LOADER_UInt2 _allowedparamcnt = static_cast<Loader_TableDescription*>(m_pTableDescription)->lColumnCount;
        if (LOADER_TRUE == m_TableHasSyskey)
        {
            --_allowedparamcnt;
        }
        if ( (0 == ParameterNumber) || (ParameterNumber > _allowedparamcnt) )
        {
            m_pError->setLoaderError(LOA_ERR_INVALID_PARAMETERINDEX, ParameterNumber);
            rc = LOADER_NOT_OK;
        }

        if (LOADER_OK == rc)
        {
#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "param no: " << ParameterNumber << endl;
#endif

            // Anzahl Parameter ist immer die angegebene Anzahl der Spalten; hat eine Tabelle z.B. einen
            // Syskey, dann wird dieser NICHT zu den Parametern gerechnet; die Zaehlung beginnt immer
            // bei 0
            LOADER_UInt2 _paramno = ParameterNumber - 1;

            if ( (LOADER_HostType_MIN == ParameterType) ||
                 (LOADER_HostType_MAX <  ParameterType) )
            {
                m_pError->setLoaderError(LOA_ERR_INVALID_HOSTTYPE,
                                         LOADER_HostTypeToString(static_cast<LOADER_HostType>(ParameterType)),
                                         ParameterNumber);
                rc = LOADER_NOT_OK;
            }
            else
            {
                _pParamList[_paramno].HostType = static_cast<LOADER_HostType>(ParameterType);
            }

            if (LOADER_OK == rc)
            {
                if (NULL == paramAddr) {
                    m_pError->setLoaderError(LOA_ERR_NULL_PARAMETERADDR, ParameterNumber);
                    rc = LOADER_NOT_OK;
                }
                else {
                    _pParamList[_paramno].pszData = static_cast<LOADER_Byte*>(paramAddr);
                }
            }


            if (LOADER_OK == rc)
            {
                if (NULL == LengthIndicator) {
                    m_pError->setLoaderError(LOA_ERR_INVALID_LENGTHINDICATOR, ParameterNumber);
                    rc = LOADER_NOT_OK;
                }
                else
                {
                    _pParamList[_paramno].LengthIndicator = LengthIndicator;     // Attention: Address value
#ifdef LOADER_TRACING
                    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "param length (1st value): " << *LengthIndicator << endl;
#endif
                }
            }

            if (LOADER_OK == rc)
            {
                if (0 > Size) {
                    m_pError->setLoaderError(LOA_ERR_NEGATIVE_BUFFERLEN, ParameterNumber);
                    rc = LOADER_NOT_OK;
                }
                else {
                    _pParamList[_paramno].BytesLength = Size;              //Length of the data buffer in bytes.
#ifdef LOADER_TRACING
                    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "param size: " << Size << endl;
#endif
                }
            }
        }
    }   // end else of if (NULL == m_ParamList)

    //m_BindTime += getRunTime(m_Start);

    return rc;
}
// Loader_FastloadCommand::bindParameter()


LOADER_Retcode
Loader_FastloadCommand::clearParameters()
{
    //setStartTime(m_Start);

#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, clearParameters, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Parameter* _pParamList = static_cast<LOADER_Parameter*>(m_ParamList);
    for (LOADER_Int2 j = 0; j < static_cast<Loader_TableDescription*>(m_pTableDescription)->lColumnCount; ++j)
    {
        _pParamList[j].HostType     = LOADER_HostType_PARAMETER_NOTSET;
        _pParamList[j].pszData      = NULL;
        _pParamList[j].BytesLength  = 0;
        _pParamList[j].PosIndicator = NULL;
        _pParamList[j].AddrBound    = LOADER_FALSE;
        _pParamList[j].Terminate    = LOADER_FALSE;

        if (NULL != _pParamList[j].LengthIndicator)
        {
            if (LOADER_DEFAULT_DATA != *_pParamList[j].LengthIndicator)
            {
                _pParamList[j].LengthIndicator = NULL;
            }
        }
    }

    //m_RunTime += getRunTime(m_Start);

    return LOADER_OK;
}
// Loader_FastloadCommand::clearParameters()


LOADER_Retcode
Loader_FastloadCommand::Execute()
{
    //setStartTime(m_Start);
    //++m_ExecuteCalled;

#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, Execute, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Retcode rc = LOADER_OK;

//time    clock_t start_all, finish_all;
//time    start_all = clock();


    m_pError->clearError();

    // Check for first time of execution
    // - do some checks
    // - set packet status
    if (LOA_PACKET_NEEDDATA == m_pPageBuilder->m_PacketStatus)
    {
        m_pPageBuilder->m_PacketStatus = LOA_PACKET_INPROCESS;
        m_pPageBuilder->initializeExecutionData();
    }
    else if (LOA_PACKET_INITIAL == m_pPageBuilder->m_PacketStatus)
    {
        //*
        //*     Some checks
        //*
        if (NULL == m_ParamList)        // cmd initialized
        {
            m_pError->setLoaderError(LOA_ERR_CMD_NOT_INITIALIZED);
        }

        if (0 == m_BatchSize)           // Wrong batch size?
        {
            m_pError->setLoaderError(LOA_ERR_INVALID_ROWARRAYSIZE);
        }

        // Are all parameters bound?
        LOADER_Parameter* _pParamList = static_cast<LOADER_Parameter*>(m_ParamList);
        for (LOADER_Int2 i = 0; i < m_ParameterCount; ++i)
        {
            // Parameter is not bound if there is no hosttype set and it has been
            // specified during initialize. All parameters not specified during initialize
            // get the length indicator set to LOADER_DEFAULT_DATA.
            if ( (LOADER_HostType_PARAMETER_NOTSET == _pParamList[i].HostType) &&
                 (NULL                             == _pParamList[i].LengthIndicator) )
            {
                m_pError->setLoaderError(LOA_ERR_PARAMETER_NOT_SET, i+1);
            }
        }
        
        if (LOADER_TRUE == *m_pError)  // error
        {
            return LOADER_NOT_OK;
        }
        
        // Coming here first time
        m_lReqPacketNo = 0;
        m_lRcvPacketNo = 0;

        m_pPageBuilder->initializeExecutionData();
    }

    // Initialize row status array
    memset(m_RowStatusArray, LOADER_OK, m_BatchSize*sizeof(LOADER_Int4));

    LOADER_Int4 rcFILL             = LOA_ERR_OK;
    LOADER_Int4 rcPROCESS          = LOA_ERR_OK;

    while ( (LOA_ERR_OK == rcFILL) &&                   // Do it as long as there is data
            (LOA_ERR_OK == rcPROCESS) &&
            (LOA_PACKET_NEEDDATA != m_pPageBuilder->m_PacketStatus) )
    {
        if (LOA_PACKET_INPROCESS != m_pPageBuilder->m_PacketStatus)
        {
            m_StartPosInBuffer = preparePagePacket(m_lReqPacketNo);
        }
        m_pPageBuilder->setPageBuffer(&(static_cast<LOADER_Byte*>(m_ReqPackets[m_lReqPacketNo]))[m_StartPosInBuffer]);
        
//time        clock_t start, finish;
//time        start = clock();
        // Fill pages with data
        rcFILL = m_pPageBuilder->AddData(static_cast<LOADER_Parameter*>(m_ParamList));

//time        finish = clock();
//time        pagebuild_duration += (double)(finish - start) / CLOCKS_PER_SEC;

        if (m_lReqPacketNo != m_lRcvPacketNo)
        {
            rcPROCESS = receivePageAnswer(m_lRcvPacketNo);

            // Adapt received packet number in any case; this prevents also from
            // receiving a packet in case of error (where we don't send one again)
            m_lRcvPacketNo = (m_lRcvPacketNo + 1) % 2;
        }

        if ( (LOA_PACKET_NEEDDATA != m_pPageBuilder->m_PacketStatus) && (LOA_ERR_OK == rcPROCESS) )
        {

            // Even in case of error a page might be partially filled - send this page, too!


            // In case there is no data page build (error while building first page to send)
            // we can't send any page because the kernel crashes over an empty page; this
            // may ONLY happen in case of errors but should always be checked

            
            // Is there any data built or do we have to stop on error while building the page?
            if ( (m_pPageBuilder->m_Page_PageNo > 0) ||
                 ( (0 == m_pPageBuilder->m_Page_PageNo) && (0 != m_pPageBuilder->m_Page_nRecCount) ) )
            {
                rcPROCESS = sendPageRequest(m_lReqPacketNo);
                if (LOA_ERR_OK == rcPROCESS)
                {
#ifdef LOADER_TRACING
                    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "s" << endl;
#endif
                    m_lReqPacketNo = (m_lReqPacketNo + 1) % 2;
                    ++m_lSndPcktCnt;
                }
            }
        }   // end if ( (LOA_PACKET_NEEDDATA != m_pPageBuilder->m_PacketStatus) && (LOADER_OK == rc) )

        // In case the error reported during filling the page is a key sequence error we know that the data is
        // ok - try to overcome this error by inserting the record using sql INSERT
        if (LOA_ERR_KEY_ORDER == rcFILL)
        {
            LOADER_Int4 rcKEY = handleKeySequenceError();
            if (LOA_ERR_OK == rcKEY)
            {
                rcFILL = LOA_ERR_OK;
            }
        }
    }   // end while ( (LOA_ERR_OK == rcFILL) && ...

    if ((LOA_ERR_OK != rcFILL) || (LOA_ERR_OK != rcPROCESS) )
    {
        rc = LOADER_NOT_OK;        
    }

    //m_ExecuteTime += getRunTime(m_Start);


//time    finish_all = clock();
//time    overall_duration += (double)(finish_all - start_all) / CLOCKS_PER_SEC;
    return rc;
}
// Loader_FastloadCommand::Execute()


LOADER_Retcode
Loader_FastloadCommand::End(LOADER_Bool bRollback)
{
    //setStartTime(m_Start);

#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, End, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Retcode  rc     = LOADER_OK;
    LOADER_Int4     rcLOAD = LOA_ERR_OK;

    // It may happen that the first page touched (for a single page) is not completely filled
    // and no data is left. In this case the counter m_Page_PageNo is still 0 but the
    // status m_Page_Status has been set to 1.
    if ( (LOA_PACKET_NEEDDATA == m_pPageBuilder->m_PacketStatus) &&
         ( (m_pPageBuilder->m_Page_PageNo > 0) || (1 == m_pPageBuilder->m_Page_Status) ) &&
         LOADER_FALSE == bRollback )
    {
#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "still data to send before ENDLOAD" << endl;
#endif

        // Is there still an open, not completely filled page?
        if (1 == m_pPageBuilder->m_Page_Status)
        {
            m_pPageBuilder->finalizePage();
#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "still open page found -> finalized" << endl;
#endif
        }

        // There seems not to be more data; user wishes to end the command
        // We therefor have to send the already built packet first
        rcLOAD = sendPageRequest(m_lReqPacketNo);
#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "page request sent" << endl;
#endif
        if (LOA_ERR_OK == rcLOAD)
        {
            m_lReqPacketNo = (m_lReqPacketNo + 1) % 2;
            ++m_lSndPcktCnt;

            // Get last answer packet
            rcLOAD = receivePageAnswer(m_lRcvPacketNo);
#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "received answer to page request" << endl;
#endif
 
            // Adapt received packet number in any case; this prevents also from
            // receiving a packet in case of error (where we don't send one again)
            m_lRcvPacketNo = (m_lRcvPacketNo + 1) % 2;
        }
    }
    else
    {
#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "no data to send" << endl;
#endif

        if ( (0 != m_lSndPcktCnt) && (m_lReqPacketNo != m_lRcvPacketNo) )
        {
#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "answer to receive" << endl;
#endif

            // Get last answer
            rcLOAD = receivePageAnswer(m_lRcvPacketNo);
#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "received answer to page request" << endl;
#endif

            // Adapt received packet number in any case; this prevents also from
            // receiving a packet in case of error (where we don't send one again)
            m_lRcvPacketNo = (m_lRcvPacketNo + 1) % 2;
        }
    }

    if (LOA_ERR_OK == rcLOAD)
    {
        if (LOADER_TRUE == bRollback)
        {
            this->m_pConnection->rollback();
#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "transaction rolled back" << endl;
#endif
        }
        else
        {
            rcLOAD = EndLoad();
        }
    }

    if ( (NULL != m_pPageBuilder) && (LOADER_FALSE == m_bKeepPageBuilderObject) )
    {
        delete m_pPageBuilder;
        m_pPageBuilder = NULL;
#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "page builder obj destroyed" << endl;
#endif
    }

//TODO clearparameters und andere aufrufen

    //m_RunTime += getRunTime(m_Start);
//time    printf( "receive: %2.2f seconds\n", receive_duration);
//time    printf( "page build: %2.2f seconds\n", pagebuild_duration);
//time    printf( "all: %2.2f seconds\n", overall_duration);

    if (LOA_ERR_OK != rcLOAD)
    {
        rc = LOADER_NOT_OK;
    }

    return rc;
}
// End()


//////////////////private functions
LOADER_Int4
Loader_FastloadCommand::getColumnNamesOfTable(LOADER_Char**& ppszColumnNames, LOADER_UInt4& lColumnCount)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getColumnNamesOfTable, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4  rc = LOA_ERR_OK;
        
    //*
    //*     Get columnnames
    //*
    LOADER_Char pszCmd[256];
    sp77sprintf(pszCmd, 256, "SELECT COLUMNNAME FROM DOMAIN.COLUMNS WHERE TABLENAME = '%s' ORDER BY POS",
                             static_cast<Loader_TableDescription*>(m_pTableDescription)->pszTableName);

    SQLDBC_Statement* pStmt = m_pConnection->m_pSQLDBCConnection->createStatement();
    SQLDBC_Retcode rcSQLDBC = pStmt->execute(pszCmd);
    if (SQLDBC_OK == rcSQLDBC)
    {
        SQLDBC_Length     length = 0;
        SQLDBC_ResultSet* rs     = pStmt->getResultSet();
        
        // We need to use auxiliary strings to temporarily save the names because we have to fetch
        // all names to get the right count of names if select-fetch-optimization is either not available
        // or is switched off. To be sure we always assume it isn't available.
        LOADER_String AuxString[1024];
        LOADER_Int4   _lAuxCounter  = 0;
        LOADER_Int4   _lAuxLength   = 0;

        memset(AuxString, 0, sizeof(AuxString));

        while (SQLDBC_OK == rs->next())
        {
            AuxString[_lAuxCounter].pszStringData = new LOADER_Char[LOADER_KNL_ID_SIZE*4];
            rcSQLDBC = rs->getObject(1, SQLDBC_HOSTTYPE_UTF8, AuxString[_lAuxCounter].pszStringData, &length, 256, LOADER_TRUE);
            if (SQLDBC_OK == rcSQLDBC)
            {
                // Trim values
                _lAuxLength = static_cast<LOADER_Int4>(length);
                AuxString[_lAuxCounter].lStringLength = length;
                Loader_StrTrimRight(reinterpret_cast<LOADER_Byte*>(AuxString[_lAuxCounter].pszStringData),
                                    _lAuxLength,
                                    LOADER_BLANK);
                AuxString[_lAuxCounter].lStringLength = _lAuxLength;
                ++_lAuxCounter;
            }
            else
            {
//TODOTODO fehlermeldung
                rc = LOA_ERR_INTERNAL;
                break;
            }
        }

        // Now assign the right column names
        if (LOA_ERR_OK == rc)
        {
            lColumnCount     = _lAuxCounter;
            ppszColumnNames  = new LOADER_Char*[lColumnCount];
            for (LOADER_UInt4 j = 0; j < lColumnCount; ++j)
            {
                ppszColumnNames[j] = new LOADER_Char[AuxString[j].lStringLength + 1];
                SAPDB_memcpy(ppszColumnNames[j], AuxString[j].pszStringData, AuxString[j].lStringLength);
                ppszColumnNames[j][AuxString[j].lStringLength] = 0;
            }
        }

        // Delete auxiliary strings
        for (LOADER_Int4 j = 0; j < _lAuxCounter; ++j)
        {
            if (NULL != AuxString[j].pszStringData)
            {
                delete AuxString[j].pszStringData;
                AuxString[j].pszStringData = NULL;
            }
        }
    }
    else
    {
        m_pError->setLoaderError(LOA_ERR_SQL_CMD_EXECUTION_FAILED, pszCmd, pStmt->error().getErrorText());
        rc = LOA_ERR_SQL_CMD_EXECUTION_FAILED;
    }

    // Release the stmt
    m_pConnection->m_pSQLDBCConnection->releaseStatement(pStmt);

    return rc;
}
// getColumnNamesOfTable()


LOADER_Int4
Loader_FastloadCommand::executeSQLStatement(LOADER_Int4 lPacketNo, const LOADER_Char* pszCmd, LOADER_Bool bWithInfo)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, executeSQLStatement, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;

#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Executing stmt:" << "\"" << pszCmd << "\"" << endl;
#endif

    // Get a request packet
    PIn_RequestPacket packet(reinterpret_cast<tsp1_packet*>(m_ReqPackets[lPacketNo]),
                             m_pConnection->m_PacketSize,
                             m_pConnection->m_DBIsUnicode);

    packet.InitVersion("LOA");

    PIn_RequestSegment segment = packet.AddSegment(sp1m_dbs, sp1sm_internal);

    segment.SetProducer(sp1pr_internal_cmd);                // Producer is always set to internal for internal commands

    if (LOADER_TRUE == bWithInfo)
    {
        segment.SetWithInfo();
    }

    segment.AddCommandOption(sp1co_scrollable_cursor_on);   // Append command option to scrollable cursor

    PIn_Part part = segment.AddPart(sp1pk_command);


    // Make cmd to send either ASCII or UNICODE!
    //BuildCommandString();
    Tools_DynamicUTF8String  _SqlStmtUTF8(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pszCmd));
    LOADER_StringEncoding    _DestCode   = LOADER_STRINGENCODING_ASCII;
    LOADER_UInt4             _lStmtLen   = _SqlStmtUTF8.BasisSize();
    LOADER_Byte*             _pszSqlStmt = NULL;

    if (LOADER_TRUE == m_pConnection->m_DBIsUnicode)       // Check for encoding of order interface to use
    {
        _lStmtLen *= 4;
        _DestCode = LOADER_STRINGENCODING_UCS2NATIVE;      // in case of commands the UCS2 encoded string must be in native swap
    }

    _pszSqlStmt = new LOADER_Byte[_lStmtLen];

    rc = Loader_StrConvertFromUTF8(_pszSqlStmt, _lStmtLen, _SqlStmtUTF8, _DestCode, m_pszErrText);

    part.AddArgument(_pszSqlStmt, _lStmtLen);
    segment.ClosePart(part);
    packet.CloseSegment(segment);

    if (NULL != _pszSqlStmt)
    {
        delete [] _pszSqlStmt;
        _pszSqlStmt = NULL;
    }

    rc = processSQLPacket(lPacketNo, pszCmd);
    return rc;
}
// executeSQLStatement()


//*
//  -----------------------------------------------------------------------------
//  function:     executeSQLStatementWData
//  -----------------------------------------------------------------------------
//*
LOADER_Int4
Loader_FastloadCommand::executeSQLStatementWData(LOADER_Int4    lPacketNo,
                                                 const char*    pszCmd,
                                                 //LOADER_OIPartInfo* pPartInfo,
                                                 void*          pPartInfo,
                                                 LOADER_Bool    bWithInfo)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, executeSQLStatementWData, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;

    PIn_RequestPacket packet(reinterpret_cast<tsp1_packet*>(m_ReqPackets[lPacketNo]),
                             m_pConnection->m_PacketSize,
                             m_pConnection->m_DBIsUnicode);

    packet.InitVersion("LOA");

    PIn_RequestSegment segment = packet.AddSegment(sp1m_dbs, sp1sm_internal);

    // Producer is always set to internal for internal commands
    segment.SetProducer(sp1pr_internal_cmd);

    if (LOADER_TRUE == bWithInfo)
    {
        segment.SetWithInfo();
    }

    PIn_Part part = segment.AddPart(sp1pk_command);

    Tools_DynamicUTF8String    _SqlStmtUTF8(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pszCmd));
    LOADER_Byte*                _pszMySqlStmt = NULL;
    LOADER_UInt4                _lStmtLen     = _SqlStmtUTF8.BasisSize();    // Initialize to not unicode db
    LOADER_StringEncoding      _DestCode     = LOADER_STRINGENCODING_ASCII;

    if (LOADER_TRUE == m_pConnection->m_DBIsUnicode)       // Check for encoding of order interface to use
    {
        _lStmtLen *= 4;
        _DestCode = LOADER_STRINGENCODING_UCS2NATIVE;      // in case of commands the UCS2 encoded string must be in native swap
    }
    _pszMySqlStmt = new LOADER_Byte[_lStmtLen];

    rc = Loader_StrConvertFromUTF8(_pszMySqlStmt,
                                   _lStmtLen,
                                   _SqlStmtUTF8,
                                   _DestCode,
                                   m_pszErrText);


    part.AddArgument(_pszMySqlStmt, _lStmtLen);
    segment.ClosePart(part);

    // Add data parts
    for (LOADER_Int4 i = 0; i < static_cast<LOADER_OIPartInfo*>(pPartInfo)->Count; ++i)
    {
        segment.AddPart(static_cast<tsp1_part_kind_Enum>((static_cast<LOADER_OIPartInfo*>(pPartInfo))->pPartKind[i]), part);
        part.AddArgument(static_cast<LOADER_OIPartInfo*>(pPartInfo)->pData[i].pszStringData,
                         static_cast<LOADER_OIPartInfo*>(pPartInfo)->pData[i].lStringLength);
        segment.ClosePart(part);
    }

    packet.CloseSegment(segment);

    rc = processSQLPacket(lPacketNo, pszCmd);

    if (NULL != _pszMySqlStmt)
    {
        delete _pszMySqlStmt;
        _pszMySqlStmt = NULL;
    }

    return rc;
}
// executeSQLStatementWData()


/*
 *      Function: analyzeSQLError
 */
LOADER_Int4
Loader_FastloadCommand::analyzeSQLError(LOADER_Int4 lPacketNo, const LOADER_Char* pszCmd)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, analyzeSQLError, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;
    
    PIn_ReplyPacket         _ReplyPacket(REINTERPRET_CAST(tsp1_packet*, m_RcvPackets[lPacketNo]));
    const PIn_ReplySegment  _ReplySegment = _ReplyPacket.GetFirstSegment();
    LOADER_Int4             _ecode        = _ReplySegment.ErrorCode();
    if (0 != _ecode)
    {
        const LOADER_Int4 _maxlen = 256;
        LOADER_Char       _pszErrorText[_maxlen];
        
        PIn_Part _Part = _ReplySegment.FindPart(sp1pk_errortext);
        if (LOADER_FALSE == _Part.IsValid())
        {
            /*sp77sprintf(ErrText, lMaxBufLen, "SQL error %i (error position: %d)\n", STATIC_CAST(int, ErrCode),
                                                                                    ErrorPos);*/
            sp77sprintf(_pszErrorText, _maxlen, "SQL error %i", _ecode);
        }
        else
        {
            PIn_ErrorTextPart _ErrTextPart(_Part);
            const LOADER_Byte* _pszSQLErrorText  = _ErrTextPart.ErrorText();
            LOADER_Int4        _lSQLErrorTextLen = _ErrTextPart.ErrorLen();
            LOADER_Int4        _pos              = 0;

            // We write everything in utf8
            /*if ( (csp_unicode_swap == SqlPacket->sp1_header.sp1h_mess_code) || 
                 (csp_unicode      == SqlPacket->sp1_header.sp1h_mess_code)  )*/
            if (true == m_pConnection->m_DBIsUnicode)
            {
                _pos = sp77sprintfUnicode(sp77encodingUTF8, _pszErrorText, _maxlen,
                                                                           "SQL error %i = %.*S", _ecode,
                                                                                                  _lSQLErrorTextLen / 2,
                                                                                                  _pszSQLErrorText);
            }
            else
            {
                _pos = sp77sprintfUnicode(sp77encodingUTF8, _pszErrorText, _maxlen,
                                                                           "SQL error %i = %.*s", _ecode,
                                                                                                  _lSQLErrorTextLen,
                                                                                                  _pszSQLErrorText);
            }
        }
        m_pError->setLoaderErrorWSQL(LOA_ERR_INTERNAL_SQL_FAILED, _ecode, _pszErrorText, pszCmd, _pszErrorText);
        rc = LOA_ERR_INTERNAL_SQL_FAILED;
    }

    return rc;
}
//  analyzeSQLError()


/*
 *      Function: getColumnInfosForFastload
 */
// Fills m_ParamsSpecifiedCnt if no column names are given in initialize()
LOADER_Int4
Loader_FastloadCommand::getColumnInfosForFastload(LOADER_Char** ppszColumnNames,
                                                  LOADER_UInt4 lColumnNameCount,
                                                  const LOADER_StringEncoding encoding)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getColumnInfosForFastload, m_pLoaderRuntime->m_pClientTrace);
#endif
    LOADER_Int4 rc = LOA_ERR_OK;

    // Build a temporary array of column names because they might be not in order or there
    // is no primkey and table has a syskey column
    LOADER_UInt4   i = 0;
    LOADER_Char**  _pszTmpColNames;

    if (NULL == ppszColumnNames)             // lColumnNameCount = 0
    {
        // Fills field count and column names
        rc = getColumnNamesOfTable(_pszTmpColNames, m_ParamsSpecifiedCnt);
    }
    else                                    // lColumnNameCount > 0
    {
        _pszTmpColNames = new LOADER_Char*[m_ParamsSpecifiedCnt];
        for (i = 0; i < m_ParamsSpecifiedCnt; ++i)
        {
            rc = Loader_StrConvert(&_pszTmpColNames[i], LOADER_STRINGENCODING_UTF8, ppszColumnNames[i], -1, encoding);
        }
    }

    //*
    //*     Hole Spalteninformationen um FASTLOAD ausfuehren zu koennen
    //*
    //*     Build the command to send
    //* Column names are always decorated with "

    Loader_TableDescription* pTableDesc = static_cast<Loader_TableDescription*>(m_pTableDescription);
    if (LOA_ERR_OK == rc)
    {
        LOADER_UInt4 _lCmdLen = (2 + m_ParamsSpecifiedCnt) * (LOADER_KNL_ID_SIZE + 4);
        char *_pszCmd = new char[_lCmdLen];

        i = 0;
        sp77sprintf(_pszCmd, _lCmdLen, "LOAD \"%s\" (\"%s", pTableDesc->pszTableName, _pszTmpColNames[i]);
        for (i = 1; i < m_ParamsSpecifiedCnt; ++i)                  // all other column names
        {
            strcat(_pszCmd, "\",\"");
            strcat(_pszCmd, _pszTmpColNames[i]);
        }
        strcat(_pszCmd, "\")");                                     // last closing parenthesis

        // One specific error that may happen here is: -7050 Fastload with indexed table not allowed
        rc = executeSQLStatement(0, _pszCmd);
        delete [] _pszCmd;

        // Delete temp col name array
        // Wenn keine Spaltennamen uebergeben wurden, muessen diese temp neu angelegt und dann auch
        // wieder geloescht werden. Ansonsten reicht es, die Zeiger zu loeschen.
        for (i = 0; i < m_ParamsSpecifiedCnt; ++i)
        {
            delete [] _pszTmpColNames[i];
            _pszTmpColNames[i] = NULL;
        }
        delete [] _pszTmpColNames;

        if (LOA_ERR_OK != rc)
        {
            return rc;
        }
    }


    // Using always the packet no 0
    PIn_ReplyPacket         ReplyPacket(reinterpret_cast<tsp1_packet*>(m_RcvPackets[0]));
    const PIn_ReplySegment  ReplySegment = ReplyPacket.GetFirstSegment();
    PIn_Part                DataPart     = ReplySegment.FindPart(sp1pk_data);

    if (LOADER_FALSE == DataPart.IsValid())
    {
        m_pError->setLoaderError(LOA_ERR_UNKNOWN_PART, static_cast<LOADER_Int4>(sp1pk_data));
        rc = LOA_ERR_UNKNOWN_PART;
    }
    else    // Found the data part
    {
        //*
        //*     Read table description (in part of kind sp1pk_data)
        //*
        LOADER_Int4         i           = 0;
        const LOADER_Byte*  pPartData   = DataPart.GetReadData();
        tsp00_Int4          _lKnlIDLen  = LOADER_KNL_ID_SIZE;

        pPartData += _lKnlIDLen;            // skip Table-Authid as it is of no interest
        
//TODO we could check if the names are equal
        pPartData += _lKnlIDLen;            // Table name: can be skipped as we have the name already
        
        // Table-Head - is 8 Byte long; copy them into structure

        // Next 8 Bytes must be interpreted as follows
        // field count      - 2 Bytes
        // kernel swap      - 1 Byte
        // alignment        - 1 Byte
        // Kernel loc       - 2 Bytes
        // constraint cnt   - 2 Bytes

        // We don't like bus errors if assigning odd addressed to ints!
        SAPDB_memcpy(&pTableDesc->lColumnCount, pPartData, sizeof(LOADER_Int2));
        pPartData += sizeof(LOADER_Int2);

        pTableDesc->KernelSwap = STATIC_CAST(LOADER_SwapKind, *pPartData);
        pPartData += sizeof(LOADER_Int2);

        SAPDB_memcpy(&pTableDesc->lKernelLoc, pPartData, sizeof(LOADER_Int2));
        pPartData += sizeof(LOADER_Int2);

        SAPDB_memcpy(&pTableDesc->lConstraintCount, pPartData, sizeof(LOADER_Int2));
        pPartData += sizeof(LOADER_Int2);

        // Table-Fields
        pTableDesc->pColumnInfoArray = new LOADER_SQLColumnInfo[pTableDesc->lColumnCount];
        memset(pTableDesc->pColumnInfoArray, 0, sizeof(LOADER_SQLColumnInfo) * pTableDesc->lColumnCount);

#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Column Count: " << pTableDesc->lColumnCount << endl;
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Kernel swap: " << pTableDesc->KernelSwap << endl;
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Constraint Count: " << pTableDesc->lConstraintCount << endl;
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "stmt_col_no" << "\tdata_type"
                                                                    << "\tfraction" << "\tsql_len" << "\tio_len"
                                                                    << "\tdef_col_no" << "\tcol_mode" << "\tlen_type"
                                                                    << "\tdefault" << endl;
#endif
        for (i = 0; i < pTableDesc->lColumnCount; ++i)
        {
            SAPDB_memcpy(&pTableDesc->pColumnInfoArray[i].StmtColumnNo, pPartData, sizeof(LOADER_Int2));
            pPartData += sizeof(LOADER_Int2);

            // column basics
            pTableDesc->pColumnInfoArray[i].ColumnBasics.SQLDataType = STATIC_CAST(LOADER_SQLType, *pPartData);
            ++pPartData;
            pTableDesc->pColumnInfoArray[i].ColumnBasics.SQLFraction = *pPartData;
            ++pPartData;
            SAPDB_memcpy(&pTableDesc->pColumnInfoArray[i].ColumnBasics.SQLLength, pPartData, sizeof(LOADER_Int2));
            pPartData += sizeof(LOADER_Int2);
            SAPDB_memcpy(&pTableDesc->pColumnInfoArray[i].ColumnBasics.SQLInOutLength, pPartData, sizeof(LOADER_Int2));
            pPartData += sizeof(LOADER_Int2);

            SAPDB_memcpy(&pTableDesc->pColumnInfoArray[i].DefinitionColumnNo, pPartData, sizeof(LOADER_Int2));
            pPartData += sizeof(LOADER_Int2);

            pTableDesc->pColumnInfoArray[i].ColumnMode = STATIC_CAST(LOADER_SQLColumnMode, *pPartData);
            ++pPartData;
            pTableDesc->pColumnInfoArray[i].LengthType = STATIC_CAST(LOADER_SQLColumnLengthType, *pPartData);
            ++pPartData;
            pTableDesc->pColumnInfoArray[i].DefaultType = STATIC_CAST(LOADER_SQLDefault, *pPartData);
            pPartData += sizeof(LOADER_Int4);        // skip 3 byte filler, too

#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << pTableDesc->pColumnInfoArray[i].StmtColumnNo
                                                                        << "\t\t" << pTableDesc->pColumnInfoArray[i].ColumnBasics.SQLDataType
                                                                        << "\t\t\t" << static_cast<LOADER_Int2>(pTableDesc->pColumnInfoArray[i].ColumnBasics.SQLFraction)
                                                                        << "\t\t" << pTableDesc->pColumnInfoArray[i].ColumnBasics.SQLLength
                                                                        << "\t\t" << pTableDesc->pColumnInfoArray[i].ColumnBasics.SQLInOutLength
                                                                        << "\t" << pTableDesc->pColumnInfoArray[i].DefinitionColumnNo
                                                                        << "\t\t\t" << pTableDesc->pColumnInfoArray[i].ColumnMode
                                                                        << "\t\t" << pTableDesc->pColumnInfoArray[i].LengthType
                                                                        << "\t\t\t" << pTableDesc->pColumnInfoArray[i].DefaultType
                                                                        << endl;
#endif
        }

        // After that we're on the right place to read the column names
        char          _pszTmpName[LOADER_KNL_ID_SIZE + sizeof(char)];
        LOADER_UInt4  _lNameLen   = 0;
        char          _clen       = 0;

        pTableDesc->pszColumnNames = new char*[pTableDesc->lColumnCount];

        for (i = 0; i < pTableDesc->lColumnCount; ++i)
        {        
            _clen = *pPartData;
            ++pPartData;

            _lNameLen = _clen;     // Implicit conversion from char to int4

            // temporarily save the column name
            SAPDB_memcpy(_pszTmpName, pPartData, _lNameLen);
            _pszTmpName[_lNameLen] = 0;
            pPartData += _lNameLen;

            // Return names in UTF8
            if (false == m_pConnection->m_DBIsUnicode)
            {
                pTableDesc->pszColumnNames[i] = new char[_lNameLen + sizeof(char)];
                sp77sprintfUnicode(sp77encodingUTF8, pTableDesc->pszColumnNames[i], (_lNameLen + sizeof(char)),
                                   "%=.*S",
                                   sp77encodingAscii, _lNameLen, _pszTmpName);
            }
            else
            {
                pTableDesc->pszColumnNames[i] = new char[2*_lNameLen + sizeof(char)];

                //Column names are in this case ALWAYS delivered in big endian order (not swapped)
                sp77sprintfUnicode(sp77encodingUTF8, pTableDesc->pszColumnNames[i], (2*_lNameLen + sizeof(char)),
                                   "%=.*S",
                                   sp77encodingUCS2, _lNameLen/2, _pszTmpName);
            }

        }   // end for (tsp00_Int2 i = 0; i < FieldCount; ++i)
    }

    return rc;
}
// getColumnInfosForFastload()


/*
 *      Function: getUserDefinedDefaultValues()
 */
LOADER_Int4
Loader_FastloadCommand::getUserDefinedDefaultValues()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getDefaultValues, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;

    Loader_TableDescription*  pTableDesc  = static_cast<Loader_TableDescription*>(m_pTableDescription);
    LOADER_UInt2*             _lFieldNo   = new LOADER_UInt2[pTableDesc->lColumnCount];
    LOADER_Int4               i           = 0;
    LOADER_Int4               _lDefCount  = 0;
    LOADER_SQLColumnInfo*     pFI         = NULL;

    for (i=0, _lDefCount=0; i < pTableDesc->lColumnCount; ++i)
    {
        pFI = &pTableDesc->pColumnInfoArray[i];

        if (LOADER_SQLDEFAULT_OTHER == pFI->DefaultType)
        {
            _lFieldNo[_lDefCount] = i;
            ++_lDefCount;
        }
        else if (LOADER_SQLDEFAULT_STAMP == pFI->DefaultType)
        {
            m_StampDefined = LOADER_TRUE;
        }
    }

    //* User defined defaults?
    if (_lDefCount > 0)
    {

        LOADER_Int2          _lColNo     = 0;
        LOADER_Int4          _kversion   = m_pConnection->getKernelVersion();

        const LOADER_UInt4   max_cmd_len = 4*sizeof(tsp00_KnlIdentifier);
        LOADER_Char          szCmd[max_cmd_len];
        sp77sprintf(szCmd, max_cmd_len, "DESCRIBE DEFAULT \"%s\"", pTableDesc->pszTableName);

        LOADER_OIPartInfo pPI;

        pPI.Count = 1;
        pPI.pPartKind = new LOADER_Int4[pPI.Count];
        pPI.pPartKind[0] = sp1pk_data;
        pPI.pData = new LOADER_String[pPI.Count];

        const LOADER_Byte* _pDefault = NULL;
        LOADER_Int2        _lDefaultLength = 0;

//TODO alle hereinkommenden Identifier sollten nach UTF8 umgewandelt sein
         for (i=0; ( (i < _lDefCount) && (0 == rc) ); ++i)
         {
             // Define number of column to get the default for
             _lColNo = pTableDesc->pColumnInfoArray[_lFieldNo[i]].DefinitionColumnNo;
             pPI.pData[0].pszStringData = REINTERPRET_CAST(LOADER_Char*, &_lColNo);
             pPI.pData[0].lStringLength = sizeof(LOADER_Int2);
             
             // For defining the default we send "DESCRIBE DEFAULT <table name>" plus column no
             // of column for which the default is defined
             rc = executeSQLStatementWData(0, szCmd, &pPI);
             if (LOA_ERR_OK == rc)
             {
                 //PIn_ReplyPacket        ReplyPacket(reinterpret_cast<tsp1_packet*>(m_pConnection->m_PacketList[0]));
                 PIn_ReplyPacket        ReplyPacket(reinterpret_cast<tsp1_packet*>(m_RcvPackets[0]));
                 const PIn_ReplySegment ReplySegment = ReplyPacket.GetFirstSegment();
                 PIn_Part               DataPart     = ReplySegment.FindPart(sp1pk_data);
                 if (LOADER_FALSE == DataPart.IsValid())
                 {
                     m_pError->setLoaderError(LOA_ERR_UNKNOWN_PART, static_cast<LOADER_Int4>(sp1pk_data));
                     rc = LOA_ERR_UNKNOWN_PART;
                 }
                 else
                 {
                     // Found the right part; do whatever has to be done
                     _pDefault = DataPart.GetReadData();

                     // what does the value in answer packet look like
                     //  kernel version 7.2.
                     //      ------------------------------------------------------------
                     //      | value length | default value with leading defined byte   |
                     //      ------------------------------------------------------------
                     //         1 byte           length byte
                     //
                     //  kernel version 7.3.
                     //      ------------------------------------------------------------
                     //      | value length | default value with leading defined byte   |
                     //      ------------------------------------------------------------
                     //         2 byte           length byte
                     

                     // Define length of value and move pointer to correct place
                     if (_kversion < 70300)
                     {
                         _lDefaultLength =  _pDefault[0];       // implicit cast to tsp00_Int2
                         _pDefault       += 1;
                     }
                     else
                     {
                         // assuming that kernel delivers 2 byte length value in swap of Loader
                         SAPDB_memcpy(&_lDefaultLength, _pDefault, sizeof(LOADER_Int2));
                         _pDefault       += sizeof(LOADER_Int2);
                     }
                
                     pTableDesc->pColumnInfoArray[_lFieldNo[i]].DefaultValue.lStringLength = _lDefaultLength;
                     pTableDesc->pColumnInfoArray[_lFieldNo[i]].DefaultValue.pszStringData =
                         new LOADER_Char[_lDefaultLength + sizeof(LOADER_Char)];
                     SAPDB_memcpy(pTableDesc->pColumnInfoArray[_lFieldNo[i]].DefaultValue.pszStringData,
                                  _pDefault,
                                  _lDefaultLength);
                     pTableDesc->pColumnInfoArray[_lFieldNo[i]].DefaultValue.pszStringData[_lDefaultLength] = 0;

#ifdef LOADER_TRACING
                    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "User defined default" << i << ": "
                                                                                << pTableDesc->pColumnInfoArray[_lFieldNo[i]].DefaultValue.pszStringData
                                                                                << endl;
#endif
                 }  // end else of if (LOADER_FALSE == DataPart.IsValid())
             }  // end if (LOA_ERR_OK == rc)
         }   // end for (i=0; ( (i < _lDefCount) && (0 == rc) ); ++i)


         // Some cleaning
         for (i = 0; i < pPI.Count; ++i)
         {
             delete [] pPI.pPartKind;
             pPI.pPartKind = NULL;

             delete [] pPI.pData;
             pPI.pData = NULL;
         }

    }   // end if (_lDefCount > 0)

    delete [] _lFieldNo;

    return rc;
}
// getUserDefinedDefaultValues()


/*
 *      Function: checkForLongColumns
 */
LOADER_Int4
Loader_FastloadCommand::checkForLongColumns()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, checkForLongColumns, m_pLoaderRuntime->m_pClientTrace);
#endif

    Loader_TableDescription* pTD = static_cast<Loader_TableDescription*>(m_pTableDescription);
    for (LOADER_Int4 i = 0; i < pTD->lColumnCount; ++i)
    {
        if (1 == LOADER_MAP_LONGDATATYPE[pTD->pColumnInfoArray[i].ColumnBasics.SQLDataType])
        {
#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "LONG column: " << i << endl;
#endif
            m_pError->setLoaderError(LOA_ERR_LONG_COL_NOT_ALLOWED);
            return LOA_ERR_LONG_COL_NOT_ALLOWED;
        }
    }

#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "No LONG column found" << endl;
#endif
    return LOA_ERR_OK;
}
// checkForLongColumns()()


/*
 *      Function: getRecordParameters
 */
void
Loader_FastloadCommand::getRecordParameters()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, getRecordParameters, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int2 i = 0;
    LOADER_SQLColumnInfo*    pCI        = NULL;
    Loader_TableDescription* pTableDesc = static_cast<Loader_TableDescription*>(m_pTableDescription);

    //*
    //*     Record laenge bestimmen
    //*
    m_RecordLength = 8;     // 8 byte header in every record

    for (i=0; i < pTableDesc->lColumnCount; ++i)
    {
        pCI = &pTableDesc->pColumnInfoArray[i];
        m_RecordLength += pCI->ColumnBasics.SQLInOutLength;

        // depending on length type of column some bytes need to be added
        // the following length types exist: sp7_fix_len, sp7_var_len, sp7_var_len_long
        switch (pCI->LengthType)
        {
            case LOADER_COLLENTYPE_VAR:
            {
                // Syskey or last column of a user defined key are variable long ===> skip these key columns
                if (pCI->ColumnMode > LOADER_COLMODE_KEY)
                {
                    ++m_RecordLength;           // add the length byte to the record length
                    ++m_VarColumnsCount;        // increase count of variable long columns
                }
                break;
            }
            case LOADER_COLLENTYPE_VAR_LONG:
            {
                m_RecordLength += 2;            // add length bytes to the record length
                break;
            }
            default:                            // corresponds to LOADER_COLLENTYPE_FIX
            {
                // Internal (db) length of fix long columns. All columns except the last column of
                // a user defined key are fix long, too. Skip these columns.
                if (LOADER_COLMODE_KEY != pCI->ColumnMode)
                {
                    m_FixColumnsLength += pCI->ColumnBasics.SQLInOutLength;
                }
                // nothing to add to record length
                break;
            }
        } //switch (pCI->LengthType)

    } //for(i=0; i < m_TableDescription.lColumnCount; ++i)

#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "record length: " << m_RecordLength << "\"" << endl;
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "fix columns length: " << m_FixColumnsLength << "\"" << endl;
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "var column count: " << m_VarColumnsCount << "\"" << endl;
#endif

    //*
    //*     Check for syskey! Syskey is in any case the first column delivered by the kernel
    //*     in the previous function.
    //*
    if (LOADER_COLMODE_SYSKEY == pTableDesc->pColumnInfoArray[0].ColumnMode)
    {
        m_TableHasSyskey = LOADER_TRUE;
#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "has syskey" << endl;
#endif
    }

}
//getRecordParameters()


/*
 *      Function: BeginLoad
 */
LOADER_Int4
Loader_FastloadCommand::BeginLoad()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, BeginLoad, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;
    
    //*
    //*     Build command
    //*
    LOADER_Char _szCmd[128];
    _szCmd[0] = 0;

    if (false == m_StampDefined)
    {
        sp77sprintf(_szCmd, 128, "BEGINLOAD \"%s\"", static_cast<Loader_TableDescription*>(m_pTableDescription)->pszTableName);
    }
    else
    { // Table has default stamp columns
        sp77sprintf(_szCmd, 128, "BEGINLOAD \"%s\" WITH STAMP", static_cast<Loader_TableDescription*>(m_pTableDescription)->pszTableName);
    }

    rc = executeSQLStatement(0, _szCmd);
    if (LOA_ERR_OK == rc)
    {
        PIn_ReplyPacket   ReplyPacket   = PIn_ReplyPacket(REINTERPRET_CAST(tsp1_packet*, m_RcvPackets[0]));
        PIn_ReplySegment  ReplySegment  = ReplyPacket.GetFirstSegment();
        PIn_Part          Part;

        //*
        //*     Part of part kind KEY
        //*
        //* The keys are always separated be the def_byte (blank for character and 0 for numeric
        //* values) because key columns are not allowed to contain NULL.
        //*
        Part = ReplySegment.FindPart(sp1pk_key);
        if (LOADER_FALSE == Part.IsValid())
        {
            m_pError->setLoaderError(LOA_ERR_UNKNOWN_PART, static_cast<LOADER_Int4>(sp1pk_key));
            rc = LOA_ERR_UNKNOWN_PART;
        }
        else
        {
            PIn_KeyPart KeyPart(Part);
            KeyPart.GetKey(REINTERPRET_CAST(LOADER_Byte*, m_LastKey), m_LastKeyLength);
            if (0 == m_LastKeyLength)
            {
                m_TableIsEmptyAtBeginLoad = LOADER_TRUE;
#ifdef LOADER_TRACING
                SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "table empty at begin load" << endl;
#endif
            }
        }

        //*
        //*     Part of part kind SURROGATE
        //*
        //* In case the table contains at least one column defined as CHAR(8) BYTE DEFAULT STAMP
        //* we need to get the last system wide given stamp value to start from
        //*
        //    ------------------------------------------------------------------------------------------
        //    | 1 byte stamp value length | stamp value, usually 9 bytes long (including defined byte) |
        //    ------------------------------------------------------------------------------------------
        //
        if ( (LOA_ERR_OK == rc) && (LOADER_TRUE == m_StampDefined) )
        {
            Part = ReplySegment.FindPart(sp1pk_surrogate);
            if (LOADER_FALSE == Part.IsValid())
            {
                m_pError->setLoaderError(LOA_ERR_UNKNOWN_PART, static_cast<LOADER_Int4>(sp1pk_surrogate));
                rc = LOA_ERR_UNKNOWN_PART;
            }
            else
            {
                LOADER_UInt4 _stamplen = 0;
                PIn_SurrogatePart SurrogatePart(Part);
                SurrogatePart.GetSurrogate(reinterpret_cast<LOADER_Byte*>(m_StampDefault), _stamplen);

#ifdef LOADER_TRACING
                SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "table has stamp" << endl;
#endif
            }
        }

        //*
        //*     Part of part kind BDINFO
        //*
        if (LOA_ERR_OK == rc)
        {
            Part = ReplySegment.FindPart(sp1pk_bdinfo);
            if (LOADER_FALSE == Part.IsValid())
            {
                m_pError->setLoaderError(LOA_ERR_UNKNOWN_PART, static_cast<LOADER_Int4>(sp1pk_bdinfo));
                rc = LOA_ERR_UNKNOWN_PART;
            }
            else
            {
                PIn_BDInfoPart BDInfoPart(Part);
                BDInfoPart.GetBDInfo(m_BDInfo);
                m_BDINFO_Length = BDInfoPart.Length();

#ifdef LOADER_TRACING
                SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "got first BDINFO" << endl;
#endif
            }
        }
    }   // end else of if (LOADER_OK != rc)

    return rc;
}
// BeginLoad()


/*
 *      Function: EndLoad
 */
LOADER_Int4
Loader_FastloadCommand::EndLoad()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, EndLoad, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;

    LOADER_Char _szCmd[128];
    _szCmd[0] = 0;

    sp77sprintf(_szCmd, 128, "ENDLOAD \"%s\"", static_cast<Loader_TableDescription*>(m_pTableDescription)->pszTableName);

    LOADER_OIPartInfo PartInfo;

    PartInfo.Count = 3;
    PartInfo.pPartKind = new LOADER_Int4[PartInfo.Count];
    PartInfo.pPartKind[0] = sp1pk_data;
    PartInfo.pPartKind[1] = sp1pk_bdinfo;
    PartInfo.pPartKind[2] = sp1pk_surrogate;
    PartInfo.pData = new LOADER_String[PartInfo.Count];

    //*
    //*     data part
    //* Enthaelt die anzahl geladener pages und records.
    //*
    PartInfo.pData[0].lStringLength = mxsp_resnum + mxsp_resnum;

    PartInfo.pData[0].pszStringData = new LOADER_Char[PartInfo.pData[0].lStringLength];
    memset(PartInfo.pData[0].pszStringData, 0, PartInfo.pData[0].lStringLength);
    if (true == m_TableIsEmptyAtBeginLoad)
    {
        PartInfo.pData[0].pszStringData[0]           = '1';
        PartInfo.pData[0].pszStringData[mxsp_resnum] = '1';

#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "table was empty at beginload" << endl;
#endif
    }
    else
    {
        PartInfo.pData[0].pszStringData[0]           = '\x00';
        PartInfo.pData[0].pszStringData[mxsp_resnum] = '\x00';
#ifdef LOADER_TRACING
        SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "table was not empty at beginload" << endl;
#endif
    }

    IFR_Retcode rcIFR = IFRUtil_VDNNumber::uint4ToNumber(m_pPageBuilder->getPageCount(),
                                                         reinterpret_cast<LOADER_Byte*>(&PartInfo.pData[0].pszStringData[1]),
                                                         csp_resnum_deflen);

#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "pages loaded: " << m_pPageBuilder->getPageCount() << endl;
#endif

    // The number of records is submitted to kernel only for one reason:
    // the optimizer can use the number for updating statistics; thus the user is not
    // necessarily forced to run 'UPDATE STATISTICS' after successful upload of
    // data; BUT the optimizer can only process values up to MAX_INT4_SP00 - that means
    // we have to limit the record count here

    LOADER_UInt4 _reccount = static_cast<LOADER_UInt4>(m_pPageBuilder->getRecordCount());
    if (_reccount > MAX_INT4_SP00)
    {
        _reccount = MAX_INT4_SP00;
    }
    rcIFR = IFRUtil_VDNNumber::uint4ToNumber(_reccount,
                                             reinterpret_cast<LOADER_Byte*>(&PartInfo.pData[0].pszStringData[mxsp_resnum+1]),
                                             csp_resnum_deflen);

#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "records loaded: " << _reccount << endl;
#endif

    //*     bdinfo part
    PartInfo.pData[1].lStringLength = m_BDINFO_Length;
    PartInfo.pData[1].pszStringData = new LOADER_Char[m_BDINFO_Length];
    SAPDB_memcpy(PartInfo.pData[1].pszStringData, this->m_BDInfo, m_BDINFO_Length);

    //*
    //*      surrogate part
    //* Contains either last inserted syskey or stamp value. In case the table has syskey
    //* column as well as stamp column(s) the stamp is sent.
    //*
    if ( (LOADER_TRUE == m_TableHasSyskey) || (LOADER_TRUE == m_StampDefined) )
    {
        // Length of buffer to admit is length of syskey/stamp (= 9 bytes including def byte)
        PartInfo.pData[2].pszStringData = new LOADER_Char[sizeof(m_StampDefault)];

        if (LOADER_TRUE == m_StampDefined)
        {
            PartInfo.pData[2].lStringLength = sizeof(m_StampDefault);
            SAPDB_memcpy(PartInfo.pData[2].pszStringData, m_StampDefault, PartInfo.pData[2].lStringLength);
        }
        else
        {
            PartInfo.pData[2].lStringLength = m_LastKeyLength;
            SAPDB_memcpy(PartInfo.pData[2].pszStringData, m_LastKey, PartInfo.pData[2].lStringLength);
        }
    }
    else
    {
        PartInfo.Count = 2;
        PartInfo.pData[2].pszStringData = NULL;
    }


    rc = executeSQLStatementWData(0, _szCmd, &PartInfo);

#ifdef LOADER_TRACING
    SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "sent ENDLOAD stmt" << endl;
#endif


    for (LOADER_Int4 i=0; i < PartInfo.Count; ++i)
    {
        delete [] PartInfo.pData[i].pszStringData;
        PartInfo.pData[i].pszStringData = NULL;
    }
    delete [] PartInfo.pPartKind;
    delete [] PartInfo.pData;

    return rc;
}    
// EndLoad()


void
Loader_FastloadCommand::configurePageBuilder()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, configurePageBuilder, m_pLoaderRuntime->m_pClientTrace);
#endif

    m_pPageBuilder = new Loader_PageBuilder(static_cast<Loader_TableDescription*>(m_pTableDescription),
                                            m_pConnection->m_DBIsUnicode,
                                            m_pConnection->m_SQLMode,
                                            this);

    m_pPageBuilder->setRecordLength(m_RecordLength);
    
    m_pPageBuilder->setLastInsertedKey(m_LastKey, m_LastKeyLength);

    m_pPageBuilder->setPagesInformation(m_pConnection->m_PageSize, m_PagesPerPacket);

    m_pPageBuilder->setKernelDateTimeFormat(m_pConnection->m_DateTimeFormat);
}
// configurePageBuilder()


LOADER_Int4
Loader_FastloadCommand::preparePagePacket(LOADER_Int4 lPacketNo)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, preparePagePacket, m_pLoaderRuntime->m_pClientTrace);
#endif
    SAPDB_memcpy(m_ReqPackets[lPacketNo], m_PacketInitBlock, m_PacketInitBlockLength);
    return m_PacketInitBlockLength;
}
// preparePagePacket()


LOADER_Int4
Loader_FastloadCommand::sendPageRequest(LOADER_Int4 lPacketNo)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, sendPageRequest, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;

    // Get pointer to Packet to send
    tsp1_packet* _pPacket = static_cast<tsp1_packet*>(m_ReqPackets[lPacketNo]);

    //*
    //* Build the packet to send
    //*

    // Get segment
    tsp1_segment *_pSegment = reinterpret_cast<tsp1_segment*>(&_pPacket->sp1_varpart()[0]);

    // compute part header offset
    // Actually the value _pSegment->sp1s_segm_offset() should be added, too. But at this point
    // it is surely 0.
    LOADER_Int4 PartOffset = _pSegment->sp1s_segm_len() - sizeof(tsp1_part_header);

    // Assign pointer to segment pointer in the packet
    tsp1_part* _pPart = reinterpret_cast<tsp1_part*>(&_pPacket->sp1_varpart()[PartOffset]);

    // Add part ;-)
    LOADER_UInt4 _len = (m_pPageBuilder->m_Page_PageNo == m_PagesPerPacket) ?
                        m_PagePartPayload :
                        m_pPageBuilder->m_Page_PageNo * m_pConnection->m_PageSize;

    _pPart->sp1p_buf_len()  += _len;

    _pPart->sp1p_arg_count() = 1;

    // Close part and segment and packet
    LOADER_Int4 _alignedlen = ALIGN_EO00(_len , packetAlignment_PI);

    _pSegment->sp1s_segm_len()             += _alignedlen;
    _pPacket->sp1_header.sp1h_varpart_len  += _alignedlen;


    //*
    //*     add bdinfo part
    //*
    PartOffset = _pSegment->sp1s_segm_len();
    _pPart = (tsp1_part*) &_pPacket->sp1_varpart()[PartOffset];      // ATTENTION: Reusing part pointer

    // Assign part specific header values
    _pPart->sp1p_part_kind().becomes(sp1pk_bdinfo);
    _pPart->sp1p_attributes().clear();
    _pPart->sp1p_segm_offset()     = _pSegment->sp1s_segm_offset();
    _pPart->sp1p_arg_count()       = 1;
    _pSegment->sp1s_no_of_parts() += 1;

    // *** Put bdinfo in ***
    SAPDB_memcpy(&_pPart->sp1p_buf()[0], m_BDInfo, m_BDINFO_Length);

    _alignedlen                            = ALIGN_EO00 (m_BDINFO_Length , packetAlignment_PI);
    LOADER_Int4  _partLen                  = sizeof(tsp1_part_header) + _alignedlen;
    _pPart->sp1p_buf_len()                 = _alignedlen;
    _pPart->sp1p_buf_size()                = _pPacket->sp1_header.sp1h_varpart_size - PartOffset - _partLen;
    _pSegment->sp1s_segm_len()            += _partLen;
    _pPacket->sp1_header.sp1h_varpart_len += _partLen;

    LOADER_Int4 _packetlen = _pPacket->sp1_header.sp1h_varpart_len + sizeof (tsp1_packet_header);

    //*
    //* Eventually send the packet
    //*
    LOADER_Bool rcBOOL = m_pLoaderRuntime->request(m_pConnection->m_ConnectionID,
                                                   m_ReqPackets[lPacketNo],
                                                   _packetlen,
                                                   *(reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error)));
    if (LOADER_FALSE == rcBOOL)
    {
        LOADER_UInt4 _len = strlen((reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errortext);
        m_pError->setLoaderError(LOA_ERR_DB_COMMUNICATION_FAILURE, "request", (reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errorcode,
                                                                               _len,
                                                                               (reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errortext);
        rc = LOA_ERR_DB_COMMUNICATION_FAILURE;
    }

    return rc;
}
// sendPageRequest()


LOADER_Int4
Loader_FastloadCommand::receivePageAnswer(LOADER_Int4 lPacketNo)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, receivePageAnswer, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;

    LOADER_Int4 _packetlen = 0;

//time    clock_t start, finish;
//time    start = clock();

    LOADER_Bool rcBOOL = m_pLoaderRuntime->receive(m_pConnection->m_ConnectionID,
                                                   &m_RcvPackets[lPacketNo],
                                                   _packetlen,
                                                   *(reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error)));

//time    finish = clock();
//time    receive_duration += (double)(finish - start) / CLOCKS_PER_SEC;
    
    if (LOADER_FALSE == rcBOOL)
    {
        LOADER_UInt4 _len = strlen((reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errortext);
        m_pError->setLoaderError(LOA_ERR_DB_COMMUNICATION_FAILURE, "receive", (reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errorcode,
                                                                              _len,
                                                                              (reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errortext);
        rc = LOA_ERR_DB_COMMUNICATION_FAILURE;
    }
    else
    {
        rc = analyzeSQLError(lPacketNo, "send packet");
        if (LOA_ERR_OK == rc)
        {
            PIn_ReplyPacket         ReplyPacket(REINTERPRET_CAST(tsp1_packet*, m_RcvPackets[lPacketNo]));
            const PIn_ReplySegment  ReplySegment = ReplyPacket.GetFirstSegment();
            PIn_Part                Part         = ReplySegment.FindPart(sp1pk_bdinfo);
            if (LOADER_FALSE == Part.IsValid())
            {
                m_pError->setLoaderError(LOA_ERR_UNKNOWN_PART, static_cast<LOADER_Int4>(sp1pk_bdinfo));
                rc = LOA_ERR_UNKNOWN_PART;
            }
            else
            {
                PIn_BDInfoPart BDInfoPart(Part);
                BDInfoPart.GetBDInfo(m_BDInfo);
                m_BDINFO_Length = BDInfoPart.Length();
            }
        }
    }

    return rc;
}
// receivePageAnswer()


LOADER_Int4
Loader_FastloadCommand::processSQLPacket(LOADER_Int4 lPacketNo, const LOADER_Char* pszCmd)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, processSQLPacket, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;

    PIn_RequestPacket       packet(reinterpret_cast<tsp1_packet*>(m_ReqPackets[lPacketNo]));
    LOADER_Int4             _packetlen = packet.Length();
    LOADER_Bool             rcBOOL = LOADER_FALSE;

    rcBOOL = m_pLoaderRuntime->request(m_pConnection->m_ConnectionID,
                                       m_ReqPackets[lPacketNo],
                                       _packetlen,
                                       *(reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error)));
    if (LOADER_FALSE == rcBOOL)
    {
        LOADER_UInt4 _len = strlen((reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errortext);
        m_pError->setLoaderError(LOA_ERR_DB_COMMUNICATION_FAILURE, "request", (reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errorcode,
                                                                              _len,
                                                                              (reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errortext);
        rc = LOA_ERR_DB_COMMUNICATION_FAILURE;
    }
    else
    {
        rcBOOL = m_pLoaderRuntime->receive(m_pConnection->m_ConnectionID,
                                           &m_RcvPackets[lPacketNo],
                                           _packetlen,
                                           *(reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error)));
        if (LOADER_TRUE == rcBOOL)
        {
            rc = analyzeSQLError(lPacketNo, pszCmd);
        }
        else
        {
            LOADER_UInt4 _len = strlen((reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errortext);
            m_pError->setLoaderError(LOA_ERR_DB_COMMUNICATION_FAILURE, "receive", (reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errorcode,
                                                                                  _len,
                                                                                  (reinterpret_cast<SQLDBC_IRuntime::Error*>(m_pSQLDBC_RT_Error))->errortext);
            rc = LOA_ERR_DB_COMMUNICATION_FAILURE;
        }
    }

    return rc;
}
// processSQLPacket()


// This function MUST be called AFTER calling BeginLoad because it uses the value m_BDINFO_Length
void
Loader_FastloadCommand::InitPagePacket(LOADER_Int4 lPacketNo)
{
    tsp1_packet*   pPagePacket     = static_cast<tsp1_packet*>(m_ReqPackets[lPacketNo]);
    tsp1_segment*  pPagePacketSegm = NULL;
    tsp1_part*     pPagePacketPart = NULL;


    memset(pPagePacket, 0, sizeof (tsp1_packet_header));             //???
    
    pPagePacket->sp1_header.sp1h_varpart_size = m_pConnection->m_PacketSize - sizeof (tsp1_packet_header);

    if  (LOADER_FALSE == m_pConnection->m_DBIsUnicode )
    {
        pPagePacket->sp1_header.sp1h_mess_code    = csp_ascii;
    }
    else
    {
        if (LOADER_SWAPKIND_SWAPPED == LOADER_SWAP_TYPE)
        {
            pPagePacket->sp1_header.sp1h_mess_code = csp_unicode_swap;
        }
        else
        {
            pPagePacket->sp1_header.sp1h_mess_code = csp_unicode;
        }
    } 


    pPagePacket->sp1_header.sp1h_mess_swap.becomes(tsp00_SwapKind_Enum(LOADER_SWAP_TYPE));

    LOADER_Char szVersion[32];
    sprintf(szVersion, "%d%02d%02d", MAJOR_VERSION_NO_SP100, MINOR_VERSION_NO_SP100, CORRECTION_LEVEL_SP100);

    SAPDB_memcpy(pPagePacket->sp1_header.sp1h_appl_version, szVersion, 5);
    SAPDB_memcpy(pPagePacket->sp1_header.sp1h_application, "LOA", 3);


    //*
    //*     Init segment
    //*
    // segment OFFSET is 0: this is the first segment

    // Assign segment specific header values
    pPagePacketSegm                     = (tsp1_segment*) &pPagePacket->sp1_varpart()[0];
    pPagePacketSegm->sp1s_segm_len()    = sizeof (tsp1_segment_header);
    pPagePacketSegm->sp1s_segm_offset() = 0;
    pPagePacketSegm->sp1s_no_of_parts() = 0;
    pPagePacketSegm->sp1s_own_index()   = 1;
    pPagePacketSegm->sp1s_segm_kind().becomes(sp1sk_cmd);       // always a command and not a result

    // Set default values
    pPagePacketSegm->sp1c_mess_type().becomes(sp1m_load);
    pPagePacketSegm->sp1c_sqlmode().becomes(sp1sm_session_sqlmode);
    pPagePacketSegm->sp1c_producer().becomes(sp1pr_internal_cmd);
    
    // Default for commit immediately is TRUE.
    // This can be changed with the user command AUTOCOMMIT ON/OFF (takes effect in ls04FastInit)
    pPagePacketSegm->sp1c_commit_immediately() = false;
    pPagePacketSegm->sp1c_ignore_costwarning() = false;
    pPagePacketSegm->sp1c_prepare()            = false;
    pPagePacketSegm->sp1c_with_info()          = false;
    pPagePacketSegm->sp1c_mass_cmd()           = false;
    pPagePacketSegm->sp1c_parsing_again()      = false;
    pPagePacketSegm->sp1c_command_options().clear();
    pPagePacketSegm->sp1c_filler1()            = 0;
    memset(pPagePacketSegm->sp1c_filler2(), 0, 8);
    memset(pPagePacketSegm->sp1c_filler3(), 0, 8);

    // Adjust packet informations for the segment
    pPagePacket->sp1_header.sp1h_varpart_len += pPagePacketSegm->sp1s_segm_len();
    pPagePacket->sp1_header.sp1h_no_of_segm  += 1;    

    // begin page part
    LOADER_Int4 PartOffset = pPagePacketSegm->sp1s_segm_offset() + pPagePacketSegm->sp1s_segm_len();      // behind bdinfo part

    pPagePacketPart = (tsp1_part*) &pPagePacket->sp1_varpart()[PartOffset];

    // Assign part specific header values
    pPagePacketPart->sp1p_part_kind().becomes(sp1pk_page);
    pPagePacketPart->sp1p_segm_offset()    = pPagePacketSegm->sp1s_segm_offset();
    pPagePacketPart->sp1p_buf_len()        = 0;
    pPagePacketPart->sp1p_buf_size()       = pPagePacket->sp1_header.sp1h_varpart_size - PartOffset - sizeof (tsp1_part_header);

    // Set DEFAULT values
    pPagePacketSegm->sp1s_segm_len()     += sizeof(tsp1_part_header);
    pPagePacketSegm->sp1s_no_of_parts()  += 1;
    pPagePacket->sp1_header.sp1h_varpart_len += sizeof(tsp1_part_header);


    m_PacketInitBlockLength = static_cast<tsp1_packet*>(m_ReqPackets[lPacketNo])->sp1_header.sp1h_varpart_len + sizeof (tsp1_packet_header);
    m_PacketInitBlock = new LOADER_Byte[m_PacketInitBlockLength];

    SAPDB_memcpy(m_PacketInitBlock, m_ReqPackets[lPacketNo], m_PacketInitBlockLength);

    // Compute payload for page data in case of a completely filled packed
    m_PagePartPayload = m_PagesPerPacket * m_pConnection->m_PageSize;
}
// InitPagePacket()

/*
LOADER_ErrorCode
Loader_FastloadCommand::processPagePacket(LOADER_Int4 lPacketNo)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;


    if ( (m_pPageBuilder->m_Page_PageNo > 0) || ( (0 == m_pPageBuilder->m_Page_PageNo) && (0 != m_pPageBuilder->m_Page_nRecCount) ) )
    {
        rc = sendPageRequest(m_lReqPacketNo);
        if (LOA_ERR_OK == rc)
        {
            rc = receivePageAnswer(m_lRcvPacketNo);
        }
    }
    return rc;
}
*/



LOADER_Int4
Loader_FastloadCommand::handleKeySequenceError()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, handleKeySequenceError, m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_Int4 rc = LOA_ERR_OK;

    // Finish FASTLOAD command properly: run End(); (make sure to not destroy the PageBuilder object while finishing)
    m_bKeepPageBuilderObject = LOADER_TRUE;
    rc = End();
    m_bKeepPageBuilderObject = LOADER_FALSE;

    // Run SET NOLOG OFF
    if (LOA_ERR_OK == rc)
    {
        LOADER_Char _szCmd[128];
        _szCmd[0] = 0;

        sp77sprintf(_szCmd, 128, "SET NOLOG OFF");
        rc = executeSQLStatement(0, _szCmd);
    }

    // Insert record that was not in proper key sequence order using SQLDBC connection
    if (LOA_ERR_OK == rc)
    {
        rc = insertAfterKeySequenceError();
    }

    // Start FASTLOAD command newly
    if (LOA_ERR_OK == rc)
    {
        rc = BeginLoad();
    }

    // Reset erroneous status array member
    if (LOA_ERR_OK == rc)
    {
        //LOADER_UInt4 lErrRow = this->m_pPageBuilder->m_Rowidx - 1
        m_RowStatusArray[this->m_pPageBuilder->m_Rowidx - 1] = LOADER_OK;            
    }

    // Reset packet and (if necessary) page status
    if (LOA_ERR_OK == rc)
    {
        if (LOA_PACKET_NEEDDATA == m_pPageBuilder->m_PacketStatus)
        {
            m_pPageBuilder->initializePageMetaData();
        }

        m_pPageBuilder->m_PacketStatus = LOA_PACKET_FILLED;
    }

    return rc;
}
// handleKeySequenceError()


LOADER_Int4
Loader_FastloadCommand::insertAfterKeySequenceError()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_FastloadCommand, insertAfterKeySequenceError, m_pLoaderRuntime->m_pClientTrace);
#endif

    // Attention: This function will only be executed for tables that have NO SYSKEY!
    LOADER_Int4 rc = LOA_ERR_OK;

    //*
    //*     Prepare INSERT statement
    //*
    SQLDBC_PreparedStatement*  _pStmt      = m_pConnection->m_pSQLDBCConnection->createPreparedStatement();
    Loader_TableDescription*   _pTableDesc = static_cast<Loader_TableDescription*>(this->m_pTableDescription);

    LOADER_UInt4  i         = 0;
    LOADER_Int4   _len      = 64*(m_ParamsSpecifiedCnt + 1) + 3 * m_ParamsSpecifiedCnt + 1024;      // col names + tab name + place holder + rest
    LOADER_Char*  _pszCmd   = new LOADER_Char[_len];
    LOADER_Int4   _pos      = 0;

    // Build statement
    _pszCmd[0] = 0;
    _pos = sp77sprintf(_pszCmd + _pos, _len - _pos, "INSERT INTO %s (", _pTableDesc->pszTableName);
    for (i = 0; i < m_ParamsSpecifiedCnt; ++i)
    {
        _pos += sp77sprintf(_pszCmd + _pos, _len - _pos, "%s,", _pTableDesc->pszColumnNames[this->m_pPageBuilder->m_plStmtNoIndex[i]]);
    }
    _pszCmd[_pos-1] = ')';              // substitute last comma with ')'

    _pos += sp77sprintf(_pszCmd + _pos, _len - _pos, " values (", _pTableDesc->pszTableName);
    for (i = 0; i < m_ParamsSpecifiedCnt; ++i)
    {
        _pos += sp77sprintf(_pszCmd + _pos, _len - _pos, "?,");
    }
    _pszCmd[_pos-1] = ')';              // substitute last comma with ')'

    // Prepare statement
    rc = _pStmt->prepare(_pszCmd);
    rc = _pStmt->setBatchSize(1);
    rc = _pStmt->setBindingType(m_ParamBindingType);

    // Bind parameters
    LOADER_Int2        _parameterIdx    = 0;
    LOADER_Parameter*  _pPL             = static_cast<LOADER_Parameter*>(m_ParamList);
    LOADER_UInt4       _lErrorneousRow  = this->m_pPageBuilder->m_Rowidx - 1;
    LOADER_UInt4       _lDistance       = 0;
    if (m_ParamBindingType > 0)        // Row wise binding
    {
        _lDistance = _lErrorneousRow * m_ParamBindingType;
    }

    // Prepared Statement works with SQLDBC_Length
    SQLDBC_Length* _pLenIndicator = new SQLDBC_Length[m_ParameterCount];

    for (_parameterIdx = 0; (LOADER_OK == rc) && (_parameterIdx < m_ParameterCount); ++_parameterIdx, ++_pPL)      // Loop over fields
    {
        if (0 == m_ParamBindingType)    // Column wise binding
        {
            _pLenIndicator[_parameterIdx] = _pPL->LengthIndicator[_lErrorneousRow];
            _pStmt->bindParameter(_parameterIdx + 1,
                                  (SQLDBC_HostType)_pPL->HostType,
                                  _pPL->pszData + _lErrorneousRow*_pPL->BytesLength,
                                  &_pLenIndicator[_parameterIdx],
                                  _pPL->BytesLength,
                                  (LOADER_NTS == _pLenIndicator[_parameterIdx]));
        }
        else                            // Row wise binding
        {
            if (LOADER_FALSE == this->m_ParamLengthIsFix)
            {
                LOADER_Byte*  pAux = reinterpret_cast<LOADER_Byte*>(_pPL->LengthIndicator) + _lDistance;
                _pLenIndicator[_parameterIdx] = *(reinterpret_cast<SQLDBC_Length*>(pAux));
            }
            else
            {
                _pLenIndicator[_parameterIdx] = *_pPL->LengthIndicator;
            }

            _pStmt->bindParameter(_parameterIdx + 1,
                                  (SQLDBC_HostType)_pPL->HostType,
                                  _pPL->pszData + _lDistance,
                                  &_pLenIndicator[_parameterIdx],
                                  _pPL->BytesLength,
                                  (LOADER_NTS == _pLenIndicator[_parameterIdx]));
        }
    }   // end for (_parameterIdx = 0; (LOADER_OK == rc) && (_parameterIdx < m_ParameterCount); ++_parameterIdx)

    // Execute statement
    rc = _pStmt->execute();
    if (SQLDBC_OK != rc)
    {
        LOADER_Int _lErrCode = _pStmt->error().getErrorCode();
        const LOADER_Char *_pErrTxt = _pStmt->error().getErrorText();

        m_pError->insertLoaderError(LOA_ERR_RECOVER_KEY_ORDER_VIOLATION, _lErrCode, _pErrTxt);
        rc = LOA_ERR_RECOVER_KEY_ORDER_VIOLATION;
    }

    if (NULL != _pLenIndicator)
    {
        delete [] _pLenIndicator;
        _pLenIndicator = NULL;
    }

    // Commit insert
    if (SAPDB_FALSE == m_pConnection->m_pSQLDBCConnection->getAutoCommit())
    {
        m_pConnection->m_pSQLDBCConnection->commit();
    }

    // Release the statement object
    m_pConnection->m_pSQLDBCConnection->releaseStatement(_pStmt);

    if (NULL != _pszCmd)
    {
        delete [] _pszCmd;
        _pszCmd = NULL;
    }

    return rc;
}
// insertAfterKeySequenceError()