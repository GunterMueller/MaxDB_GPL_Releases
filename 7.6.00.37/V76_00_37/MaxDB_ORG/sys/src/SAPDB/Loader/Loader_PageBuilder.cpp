/*!
  \file    Loader_PageBuilder.cpp
  \author  SteffenS
  \brief   Implementation of filling a DB page

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

#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_ErrorMsgHandler.hpp"
#include "Loader/Loader_Loader.hpp"
#include "Loader/Loader_PageBuilder.hpp"
#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_Common.hpp"
#include "Loader/Loader_VDNNumber.hpp"

#include "Loader/Loader_StringConversion.hpp"
#include "Loader/Loader_SpecialConstants.hpp"
#include "gbd00.h"      // database bd layer (data page layout)
#include "vin00.h"      // date and time conversion
#include "hsp30.h"      // comparison routine for key fields: s30cmp

// temporarily tracing
//#include "Interfaces/SQLDBC/SQLDBC_ClientTrace.h"
#include "Loader/Loader_ClientRuntime.hpp"

//externPascal tin_global_in_vars i01g;
extern "C" tin_global_in_vars i01g;


static LOADER_Char const* LOADER_AncientDateTimeErrText[13] =
                                             {"Invalid length for date or time field",  //17404
                                              "Double field definition or delimiter",   //17405
                                              "No delimiter allowed here",              //17406
                                              "Invalid order of time fields",           //17407
                                              "Mask too long",                          //17408
                                              "Month table not available",              //17409
                                              "Date mask incomplete",                   //17410
                                              "Input string is too long",               //17411
                                              "Input string is incomplete",             //17412
                                              "Invalid time",                           //17413
                                              "Invalid date",                           //17414
                                              "Specify month by : Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec", //17415
                                              "Specify one standard for all DATE/TIME parameters"};                 //17416


static const LOADER_Char szPAGEBUILDER_DefByteArray[39] =
{DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE_ASCII,     DEFINED_BYTE_EBCDIC,
 DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE_ASCII,     DEFINED_BYTE_EBCDIC,
 DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE_ASCII,     DEFINED_BYTE_ASCII,
 DEFINED_BYTE,          DEFINED_BYTE_ASCII, DEFINED_BYTE,           DEFINED_BYTE,
 DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE,           DEFINED_BYTE_ASCII,
 DEFINED_BYTE_EBCDIC,   DEFINED_BYTE,       DEFINED_BYTE,           DEFINED_BYTE,
 DEFINED_BYTE_UNICODE,  DEFINED_BYTE,       DEFINED_BYTE,           DEFINED_BYTE,
 DEFINED_BYTE,          DEFINED_BYTE,       DEFINED_BYTE,           DEFINED_BYTE_ASCII,
 DEFINED_BYTE_EBCDIC,   DEFINED_BYTE,       DEFINED_BYTE_UNICODE,   DEFINED_BYTE_UNICODE,
 DEFINED_BYTE_UNICODE,  DEFINED_BYTE,       DEFINED_BYTE};


//  These constants are used to build the record for a data page 
static const LOADER_Int4 BUF_START_POS          = 0;
static const LOADER_Int4 KEYLEN_OFFSET          = 2;
static const LOADER_Int4 VARCOL_OFFSET          = 4;
static const LOADER_Int4 VARCOL_COUNT_OFFSET    = 6;
static const LOADER_Int4 HEADER_OFFSET          = 8;
static const LOADER_Int4 LENGTH_OFFSET          = 8;


Loader_PageBuilder::Loader_PageBuilder(Loader_TableDescription* pTableDescription,
                                       LOADER_Bool               bUnicodeDB,
                                       LOADER_SQLMode           SqlMode,
                                       Loader_FastloadCommand*  pFLCmdObj) :
m_PacketStatus(LOA_PACKET_INITIAL),
m_ParameterCount(0),
m_ColumnCount(0),
m_KernelSwap(LOADER_SWAPKIND_NORMAL),
m_pColumnDataArray(NULL),
m_PageSize(8192),
m_PagesPerBuffer(0),
m_RecordLength(0),
m_FixColumnsLength(0),
m_TableHasSyskey(LOADER_FALSE),
m_LastKeyLength(0),
m_SQLMode(LOADER_SQLMODE_INTERNAL),
m_KernelDateTimeFormat(LOADER_DateTimeFormat_Internal),
m_pDBEncoding(sp77encodingAscii),
m_Distance(0),
m_Page_Status(0),
m_PageCount(0),
m_RowCount(0),
m_pDataPage(NULL),
m_pDataBuffer(NULL),
m_pSpecialPagePtr(NULL),
m_Rowidx(0),
m_DataTruncated(LOADER_FALSE),
m_lErrDataIndex(0),
m_lErrColNameIndex(0),
m_SpecialDateTimeError(0)
//time,duration(0)
{
    i01g = new tin_global_in_record;
    i01g->i20.code_type = csp_ascii;
    SAPDB_memcpy(i01g->i32.transtables[0].asCharp(), LOADER_Latin1_ToLowerMap, 256);
    SAPDB_memcpy(i01g->i32.transtables[1].asCharp(), LOADER_lLatin1_ToUpperMap, 256);

    // Assign the FastloadCommand object
    m_pTheCreator = pFLCmdObj;
    
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_PageBuilder, Loader_PageBuilder, m_pTheCreator->m_pLoaderRuntime->m_pClientTrace);
#endif

    memset(m_LastKey, 0, 1024);

    m_ColumnCount = pTableDescription->lColumnCount;
    m_KernelSwap  = pTableDescription->KernelSwap;

    if (LOADER_COLMODE_SYSKEY == pTableDescription->pColumnInfoArray[0].ColumnMode)
    {
        m_TableHasSyskey = LOADER_TRUE;
        m_ParameterCount = m_ColumnCount - 1;
    }
    else
    {
        m_ParameterCount = m_ColumnCount;
    }


    // Allocate m_pColumnDataArray
    // Structure can only be initialized when running AddData because values must be ordered in parameter order
    m_pColumnDataArray = new Loader_ColumnData[m_ParameterCount];
    memset(m_pColumnDataArray, 0, (m_ParameterCount * sizeof(Loader_ColumnData)));

    // Das ist der Index, der dir Reihenfolge festlegt, in der die Daten in den
    // Datensatz auf der Seite geschrieben werden; der Index wird erstellt bezogen
    // auf m_ColumnCount!
    m_plFillIndex = new LOADER_UInt4[m_ColumnCount];

    // Because the syskey can't be specified by the user we use m_ParameterCount here
    m_plStmtNoIndex = new LOADER_UInt4[m_ParameterCount];

    //*
    //*     Assign columninfoarray members to appropriate members of columndataarray
    //*
    LOADER_Int2 k = 0;
    LOADER_Int2 m = 0;

    // First assign those that have not been specified by the user
    for (k = 0, m = m_pTheCreator->m_ParamsSpecifiedCnt; k < m_ColumnCount; ++k)
    {
        if ( (0 == pTableDescription->pColumnInfoArray[k].StmtColumnNo) &&
             (LOADER_COLMODE_SYSKEY != pTableDescription->pColumnInfoArray[k].ColumnMode) )
        {
            SAPDB_memcpy(&m_pColumnDataArray[m].SQLColumnInfo, &pTableDescription->pColumnInfoArray[k], sizeof(LOADER_SQLColumnInfo));

            // special handling for default values
            if (NULL != pTableDescription->pColumnInfoArray[k].DefaultValue.pszStringData)
            {
                m_pColumnDataArray[m].SQLColumnInfo.DefaultValue.lStringLength = pTableDescription->pColumnInfoArray[k].DefaultValue.lStringLength;
                m_pColumnDataArray[m].SQLColumnInfo.DefaultValue.pszStringData = new LOADER_Char[m_pColumnDataArray[m].SQLColumnInfo.DefaultValue.lStringLength + sizeof(LOADER_Char)];
                SAPDB_memcpy(m_pColumnDataArray[m].SQLColumnInfo.DefaultValue.pszStringData,
                             pTableDescription->pColumnInfoArray[k].DefaultValue.pszStringData,
                             m_pColumnDataArray[m].SQLColumnInfo.DefaultValue.lStringLength);
                m_pColumnDataArray[m].SQLColumnInfo.DefaultValue.pszStringData[m_pColumnDataArray[m].SQLColumnInfo.DefaultValue.lStringLength] = 0;
            }

            m_plFillIndex[k] = m;

            // Allocate memory for the data buffer depending on the data type of that member
//TODOTODO UNICODE
            m_pColumnDataArray[m].lDataBufferLength = m_pColumnDataArray[m].SQLColumnInfo.ColumnBasics.SQLInOutLength;
            m_pColumnDataArray[m].pDataBuffer       = new LOADER_Byte[m_pColumnDataArray[m].lDataBufferLength];

            // Assign define byte - prerequisite is that SQLDataType is already assigned
            m_pColumnDataArray[m].cDefByte = GetDefinedByte(m_pColumnDataArray[m].SQLColumnInfo.ColumnBasics.SQLDataType);

            // Assign fill byte - prerequisite is that SQLDataType is already assigned
            m_pColumnDataArray[m].cFillByte = GetFillByte(m_pColumnDataArray[m].SQLColumnInfo.ColumnBasics.SQLDataType);

            // Conversion to Unicode necessary when writing to page at the end?
            if (1 == LOADER_MAP_UNICODEDATATYPE[m_pColumnDataArray[m].SQLColumnInfo.ColumnBasics.SQLDataType]) 
            {
                m_pColumnDataArray[m].bUnicodeConversion = 1;
            }


            if (LOADER_COLLENTYPE_VAR == m_pColumnDataArray[m].SQLColumnInfo.LengthType)
            {
                m_pColumnDataArray[m].LengthInfoByteCount = sizeof(LOADER_Char);
            }
            else if (LOADER_COLLENTYPE_VAR_LONG == m_pColumnDataArray[m].SQLColumnInfo.LengthType)
            {
                m_pColumnDataArray[m].LengthInfoByteCount = 2*sizeof(LOADER_Char);
            }


            if (0 == LOADER_MAP_SAP_VARCHAR_HOSTTYPE[m_pColumnDataArray[m].HostType])     // special handling for special dbsl data type
            {
                m_pColumnDataArray[m].DataOffset = sizeof(LOADER_Char);
            }
            else if (1 == LOADER_MAP_SAP_VARCHAR_HOSTTYPE[m_pColumnDataArray[m].HostType])
            {
                m_pColumnDataArray[m].DataOffset = sizeof(LOADER_Char) + sizeof(LOADER_Int2);
            }
            ++m;
        }
    }   // end for (k = 0, m = m_pTheCreator->m_ParamsSpecifiedCnt; k < m_ColumnCount; ++k)

    // Second assign those specified by the user
    for (k = 0, m = 0; k < m_ParameterCount; ++k)
    {
//TODO define databufferlength abhaengig vom sqltype!
        for (m = 0; m < m_ColumnCount; ++m)
        {
            if (k == pTableDescription->pColumnInfoArray[m].StmtColumnNo - 1)
            {
                SAPDB_memcpy(&m_pColumnDataArray[k].SQLColumnInfo, &pTableDescription->pColumnInfoArray[m], sizeof(LOADER_SQLColumnInfo));
                // special handling for default values
                if (NULL != pTableDescription->pColumnInfoArray[m].DefaultValue.pszStringData)
                {
                    m_pColumnDataArray[k].SQLColumnInfo.DefaultValue.lStringLength = pTableDescription->pColumnInfoArray[m].DefaultValue.lStringLength;
                    m_pColumnDataArray[k].SQLColumnInfo.DefaultValue.pszStringData = new LOADER_Char[m_pColumnDataArray[k].SQLColumnInfo.DefaultValue.lStringLength + sizeof(LOADER_Char)];
                    SAPDB_memcpy(m_pColumnDataArray[k].SQLColumnInfo.DefaultValue.pszStringData,
                                 pTableDescription->pColumnInfoArray[m].DefaultValue.pszStringData,
                                 m_pColumnDataArray[k].SQLColumnInfo.DefaultValue.lStringLength);
                    m_pColumnDataArray[k].SQLColumnInfo.DefaultValue.pszStringData[m_pColumnDataArray[k].SQLColumnInfo.DefaultValue.lStringLength] = 0;
                }

                m_plFillIndex[m] = k;
                m_plStmtNoIndex[k] = m;

                // Allocate memory for the data buffer depending on the data type of that member
//TODOTODO UNICODE
                m_pColumnDataArray[k].lDataBufferLength = m_pColumnDataArray[k].SQLColumnInfo.ColumnBasics.SQLInOutLength;
                m_pColumnDataArray[k].pDataBuffer = new LOADER_Byte[m_pColumnDataArray[k].lDataBufferLength];

                // Assign define byte - prerequisite is that SQLDataType is already assigned
                m_pColumnDataArray[k].cDefByte = GetDefinedByte(m_pColumnDataArray[k].SQLColumnInfo.ColumnBasics.SQLDataType);

                // Assign fill byte - prerequisite is that SQLDataType is already assigned
                m_pColumnDataArray[k].cFillByte = GetFillByte(m_pColumnDataArray[k].SQLColumnInfo.ColumnBasics.SQLDataType);

                // Conversion to Unicode necessary when writing to page at the end?
                if (1 == LOADER_MAP_UNICODEDATATYPE[m_pColumnDataArray[k].SQLColumnInfo.ColumnBasics.SQLDataType]) 
                {
                    m_pColumnDataArray[k].bUnicodeConversion = 1;
                }

                if (LOADER_COLLENTYPE_VAR == m_pColumnDataArray[k].SQLColumnInfo.LengthType)
                {
                    m_pColumnDataArray[k].LengthInfoByteCount = sizeof(LOADER_Char);
                }
                else if (LOADER_COLLENTYPE_VAR_LONG == m_pColumnDataArray[k].SQLColumnInfo.LengthType)
                {
                    m_pColumnDataArray[k].LengthInfoByteCount = 2*sizeof(LOADER_Char);
                }

                if (0 == LOADER_MAP_SAP_VARCHAR_HOSTTYPE[m_pColumnDataArray[k].HostType])     // special handling for special dbsl data type
                {
                    m_pColumnDataArray[k].DataOffset = sizeof(LOADER_Char);
                }
                else if (1 == LOADER_MAP_SAP_VARCHAR_HOSTTYPE[m_pColumnDataArray[k].HostType])
                {
                    m_pColumnDataArray[k].DataOffset = sizeof(LOADER_Char) + sizeof(LOADER_Int2);
                }

                break;
            }
        }
    }   // end for (k = 0, m = 0; k < m_ParameterCount; ++k)

    setSQLMode(SqlMode);

    memset(&m_DateTimeMask, 0, sizeof(m_DateTimeMask));

    setDBEncoding(bUnicodeDB);

    initializePageMetaData();

    // Initialize special values that are fixed
    Loader_Swap2BIntToChar(m_pTheCreator->m_VarColumnsCount, m_szVarColumnsCount, (LOADER_SWAP_TYPE != m_KernelSwap));

    m_FixColumnsLength = m_pTheCreator->m_FixColumnsLength;
    Loader_Swap2BIntToChar(m_FixColumnsLength, m_szFixColumnsLength, (LOADER_SWAP_TYPE != m_KernelSwap));
}
// Loader_PageBuilder()


Loader_PageBuilder::~Loader_PageBuilder()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_PageBuilder, ~Loader_PageBuilder, m_pTheCreator->m_pLoaderRuntime->m_pClientTrace);
#endif

//time    printf( "add: %2.2f seconds\n", duration);

    if (NULL != m_plFillIndex)
    {
        delete [] m_plFillIndex;
    }

    if (NULL != m_plStmtNoIndex)
    {
        delete [] m_plStmtNoIndex;
    }

    if (NULL != m_pColumnDataArray)
    {
        for (LOADER_Int4 i = 0; i < m_ParameterCount; ++i)
        {
            if (NULL != m_pColumnDataArray[i].pDataBuffer)
            {
                delete [] m_pColumnDataArray[i].pDataBuffer;
                m_pColumnDataArray[i].pDataBuffer = NULL;
            }

            m_pColumnDataArray[i].pPtrToData = NULL;
            if (NULL != m_pColumnDataArray[i].SQLColumnInfo.DefaultValue.pszStringData)
            {
                delete [] m_pColumnDataArray[i].SQLColumnInfo.DefaultValue.pszStringData;
            }
        }
        delete [] m_pColumnDataArray;
        m_pColumnDataArray = NULL;
    }

    m_pTheCreator = NULL;

    delete i01g;
}
// ~Loader_PageBuilder()


void
Loader_PageBuilder::initializePageMetaData()
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_PageBuilder, initializePageMetaData, m_pTheCreator->m_pLoaderRuntime->m_pClientTrace);
#endif

    m_Page_ListTop      = 0;
    m_Page_Bottom       = 0;
    m_Page_Space        = 0;
    m_Page_PageNo       = 0;
    m_Page_nRecCount    = 0;
    m_Page_RecordLength = 0;
    m_Page_Status       = 0;
}
// initializePageMetaData()


void
Loader_PageBuilder::setKernelDateTimeFormat(LOADER_DateTimeFormat DateTimeFormat)
{    
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_PageBuilder, setKernelDateTimeFormat, m_pTheCreator->m_pLoaderRuntime->m_pClientTrace);
#endif

    m_KernelDateTimeFormat = DateTimeFormat;

    tin_date_mask _SpecialMask;
    memset(&_SpecialMask, 0, sizeof(_SpecialMask));

    // Build date and time masks for later use
    switch (DateTimeFormat)
    {
        // valid for all cases: no need to assign tlen a value; this is done by i41dparse
        case LOADER_DateTimeFormat_Internal:
        {
            _SpecialMask.dm_type.becomes(dm_int);
            _SpecialMask.mlen    = mxin_key_int;      // mxin_key_int=3

            _SpecialMask.msk.SetBlank();
            _SpecialMask.msk.c2p("INTERNAL");
            break;
        }
        case LOADER_DateTimeFormat_Iso:
        {
            _SpecialMask.dm_type.becomes(dm_iso);
            _SpecialMask.mlen    = mxin_key_iso;      // mxin_key_iso=3

            _SpecialMask.msk.SetBlank();
            _SpecialMask.msk.c2p("ISO");
            break;
        }
        case LOADER_DateTimeFormat_Usa:
        {
            _SpecialMask.dm_type.becomes(dm_usa);
            _SpecialMask.mlen    = mxin_key_usa;      // mxin_key_usa=3

            _SpecialMask.msk.SetBlank();
            _SpecialMask.msk.c2p("USA");
            break;
        }
        case LOADER_DateTimeFormat_Eur:
        {
            _SpecialMask.dm_type.becomes(dm_eur);
            _SpecialMask.mlen    = mxin_key_eur;      // mxin_key_eur=3

            _SpecialMask.msk.SetBlank();
            _SpecialMask.msk.c2p("EUR");
            break;
        }
        case LOADER_DateTimeFormat_Jis:
        {
            _SpecialMask.dm_type.becomes(dm_jis);
            _SpecialMask.mlen    = mxin_key_jis;      // mxin_key_jis=3

            _SpecialMask.msk.SetBlank();
            _SpecialMask.msk.c2p("JIS");
            break;
        }
        default:
        {
            break;      // Error!
        }
    }

    LOADER_Int4  lError = 0;
    LOADER_UInt2 lErrorPos;

    // We parse only 1 masks because all date, time, or timestamp values must have the same format
    SAPDB_memcpy(&m_DateTimeMask, &_SpecialMask, sizeof(_SpecialMask));
    i41dparse(m_DateTimeMask, lError, lErrorPos);

//TODO Fehler?!

    //if (0 != lError)
    //{
    //    {
    //        pszErrText = LOADER_AncientDateTimeErrText[lError - 17404];
    //    }

}
// setKernelDateTimeFormat()


/*
 *      function: setDBEncoding
 */
void
Loader_PageBuilder::setDBEncoding(LOADER_Bool bUnicode)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_PageBuilder, setDBEncoding, m_pTheCreator->m_pLoaderRuntime->m_pClientTrace);
#endif

    if (LOADER_TRUE == bUnicode)
    {
        if (LOADER_SWAPKIND_NORMAL == m_KernelSwap)
        {
            m_pDBEncoding = sp77encodingUCS2;
        }
        else
        {
            m_pDBEncoding = sp77encodingUCS2Swapped;
        }
    }
    else
    {
        m_pDBEncoding = sp77encodingAscii;
    }
}
// setDBEncoding()


LOADER_ErrorCode
Loader_PageBuilder::AddData(LOADER_Parameter* ptrToData)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_PageBuilder, AddData, m_pTheCreator->m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_ErrorCode rc = LOA_ERR_OK;

    // The following both control filling of pages
    if (LOA_PACKET_FILLED == m_PacketStatus)
    {
        m_Page_PageNo         = 0;
    }
    m_PacketStatus = LOA_PACKET_INPROCESS;
    

    // Is structure already initialized? If not do it here for the first and last time.
    // Some values like ptr to real data must be initialized for every run

    // There is at least one value in m_pColumnDataArray?
    if (LOADER_HostType_PARAMETER_NOTSET == m_pColumnDataArray[0].HostType)
    {
        for (LOADER_Int2 k = 0; k < m_ParameterCount; ++k)
        {
            m_pColumnDataArray[k].HostType = ptrToData[k].HostType;
        }
    }

    LOADER_Int2   paramidx = 0;
    LOADER_Byte*  pAux     = NULL;

    while ( (LOA_ERR_OK == rc) && (m_Page_PageNo < m_PagesPerBuffer) && (LOA_PACKET_INPROCESS == m_PacketStatus) )
    {
        // Initialize buffer and values
        m_pDataPage       = reinterpret_cast<tbd_node*>(&m_pGlobalDataBuffer[m_Page_PageNo*m_PageSize]);
        m_pSpecialPagePtr = &m_pGlobalDataBuffer[m_Page_PageNo*m_PageSize];

        if (0 != m_Page_Status)     // Page is not a virgin any more
        {
            m_pDataBuffer    = m_pSpecialPagePtr + m_Page_Bottom;
        }
        else                        // Page is still a virgin = new page
        {
            memset(m_pDataPage, 0, sizeof(tbd_node_header));                        // Initialize page header

            m_Page_Bottom    = sizeof(tbd_node_header);
            m_Page_ListTop   = m_PageSize - sizeof(tgg00_PageCheckTrailer);

            // Space for page ptr of first record has already to be substracted at this point to get the real available space
            m_Page_Space     = m_Page_ListTop - m_Page_Bottom - POINTERSIZE_BD00;   // corresponds to 100% Usage
            m_pDataBuffer    = m_pSpecialPagePtr + sizeof(tbd_node_header);
            m_Page_nRecCount = 0;
        }


        //*
        //*     Fill single page
        //*
        //* When assigning the length we cast it down to Int4 (on 64-Bit machines)
        //*
        while ( (m_Page_Space > m_MinSpaceRequiredForSingleRecord) && (LOA_ERR_OK == rc) && (m_Rowidx < m_pTheCreator->m_BatchSize) )
        {
            if (m_pTheCreator->m_ParamBindingType > 0)        // row wise binding
            {
                m_Distance = m_Rowidx * m_pTheCreator->m_ParamBindingType;
            }

// Parameterindex und stmtcolno stimmen überein; ausser im Fall, dass ein SYSKEY existiert!!!

            for (paramidx = 0; (LOADER_OK == rc) && (paramidx < m_ParameterCount); ++paramidx)      // Loop over fields
            {
                m_pColumnDataArray[paramidx].PtrUsed_ls00 = LOADER_TRUE;

                if (0 == m_pTheCreator->m_ParamBindingType)
                {   // Column wise binding
                    m_pColumnDataArray[paramidx].pPtrToData  = ptrToData[paramidx].pszData + m_Rowidx * ptrToData[paramidx].BytesLength;
                    m_pColumnDataArray[paramidx].lDataLength = static_cast<LOADER_Int4>(ptrToData[paramidx].LengthIndicator[m_Rowidx]);
                }
                else
                {   // Row wise binding
                    m_pColumnDataArray[paramidx].pPtrToData  = ptrToData[paramidx].pszData + m_Distance;
                    
                    if (LOADER_FALSE == m_pTheCreator->m_ParamLengthIsFix)
                    {
                        pAux = reinterpret_cast<LOADER_Byte*>(ptrToData[paramidx].LengthIndicator) + m_Distance;
                        m_pColumnDataArray[paramidx].lDataLength = *(reinterpret_cast<LOADER_Int4*>(pAux));
                    }
                    else
                    {
                        // In case of dbsl the length is fix and given only once for the first bunch of data; this is
                        // flagged by m_ParamLengthIsFix
                        m_pColumnDataArray[paramidx].lDataLength = static_cast<LOADER_Int4>(*ptrToData[paramidx].LengthIndicator);
                    }
#ifdef LOADER_TRACING                    
                    SQLDBC_APPL_LEVEL_2_TRACE(m_pTheCreator->m_pLoaderRuntime->m_pClientTrace) << "data[ " << m_Rowidx << "," << paramidx << "]="
                                                                                               << m_pColumnDataArray[paramidx].pPtrToData << endl;
#endif
                }

                if (LOADER_NTS == m_pColumnDataArray[paramidx].lDataLength)
                {
                    if ( (LOADER_HostType_UCS2         != m_pColumnDataArray[paramidx].HostType) &&
                         (LOADER_HostType_UCS2_SWAPPED != m_pColumnDataArray[paramidx].HostType) )
                    {
                        m_pColumnDataArray[paramidx].lDataLength = LOADER_Strnlen(reinterpret_cast<LOADER_Char*>(m_pColumnDataArray[paramidx].pPtrToData),
                                                                                  static_cast<LOADER_Int4>(ptrToData[paramidx].BytesLength));
                    }
                    else
                    {
                        m_pColumnDataArray[paramidx].lDataLength = LOADER_StrnlenUCS2(reinterpret_cast<LOADER_Char*>(m_pColumnDataArray[paramidx].pPtrToData),
                                                                                      static_cast<LOADER_Int4>(ptrToData[paramidx].BytesLength));
                    }
                }

            }   // end for (paramidx = 0; (LOADER_OK == rc) && (paramidx < m_ParameterCount); ++paramidx)      // Loop over fields

            m_Page_RecordLength = buildRecord();

            // Check return value and set row status array value
            if (m_Page_RecordLength >= 0)   // Check for data truncation and mark the row status array with a warning
            {
                if (LOADER_TRUE == m_DataTruncated)
                {
                    m_pTheCreator->m_RowStatusArray[m_Rowidx] = LOADER_DATA_TRUNC;
                    m_DataTruncated = LOADER_FALSE;
                }
            }
            else                            // Simply an error
            {
                rc = static_cast<LOADER_ErrorCode>(m_Page_RecordLength);
                m_pTheCreator->m_RowStatusArray[m_Rowidx] = LOADER_NOT_OK;
            }

            if (LOA_ERR_OK == rc)
            {
#ifdef LOADER_TRACING
                SQLDBC_APPL_LEVEL_2_TRACE(m_pTheCreator->m_pLoaderRuntime->m_pClientTrace) << "+";
#endif

                m_Page_ListTop -= POINTERSIZE_BD00;    // update positions

                //  Write pointer to the end of the data list
                Loader_Swap4BIntToChar(m_Page_Bottom + 1,
                                       m_pSpecialPagePtr + m_Page_ListTop,
                                       LOADER_SWAP_TYPE,
                                       m_KernelSwap);

                m_Page_Space  -= POINTERSIZE_BD00;      // update available space
                m_Page_Bottom += m_Page_RecordLength;
                m_Page_Space  -= m_Page_RecordLength;
                if ( 0 != (m_Page_RecordLength%2) )     // 2 byte alignment of record
                {
                    m_Page_Bottom += 1;
                    m_Page_Space  -= 1;
                }
                m_Page_nRecCount += 1;                  // Update number of records written to table

                m_pDataBuffer = m_pSpecialPagePtr + m_Page_Bottom;
            }

            ++m_Rowidx;
        }   // end while ( (m_Page_Space > m_RecordLength) && ...

        // Update page header values even in case of errors. If we encounter errors while
        // filling the page there might have been SOME records that were error free that can
        // be send to the kernel.
        // But sending completely empty pages crashes the kernel upon inserting so we should
        // be careful here.
        if (0 != m_Page_nRecCount)    // No record -> no page to build
        {
            if (LOA_ERR_OK == rc)
            {
                if (m_Page_Space <= m_MinSpaceRequiredForSingleRecord)
                {
                    finalizePage();                 // Update record header values
                    m_Page_Status = 0;
                }
                else
                {
                    m_Page_Status = 1;
                }
            }
            else
            {
                if (m_Page_nRecCount > 1)           // send valid records even in case of an error
                {
                    finalizePage();                 // Update record header values
                    m_Page_Status = 0;
                }
                else
                {
                    m_Page_Status = 1;
                }
            }
        }   // end if ( (ErrCode != STOP_NODATA_LS22) && (0 != nPageRecCount) ) // No record -> no page to build

        if (m_Rowidx == m_pTheCreator->m_BatchSize)
        {
            if (m_Page_PageNo < m_PagesPerBuffer)
            {
                m_PacketStatus = LOA_PACKET_NEEDDATA;
            }
            else
            {
                m_PacketStatus = LOA_PACKET_FILLED;
            }
        }
    }   // end while ( (LOA_ERR_OK == rc) && (m_Page_PageNo < m_PagesPerBuffer) )

    // Set final status of packet filling
//TODOTODO Was, wenn ein Fehler bei der X-ten Seite aufgetreten ist, aber die anderen vorher noch an den Kern
//         geschickt werden muessen???
    if (LOA_ERR_OK == rc)
    {
        if (m_Page_PageNo == m_PagesPerBuffer)
        {
            m_PacketStatus = LOA_PACKET_FILLED;
            
//            m_PageCount += m_Page_PageNo;
#ifdef LOADER_TRACING
            SQLDBC_APPL_LEVEL_2_TRACE(m_pTheCreator->m_pLoaderRuntime->m_pClientTrace) << "pc=" << m_PageCount << endl;
#endif
        }
    }
    return rc;
}
// AddData()


/*
  -----------------------------------------------------------------------------
  function:     getNILConstants
  -----------------------------------------------------------------------------
*/
inline LOADER_ErrorCode
Loader_PageBuilder::getNILConstants(Loader_ColumnData* pColumnData)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_PageBuilder, getNILConstants, m_pTheCreator->m_pLoaderRuntime->m_pClientTrace);
#endif

    // Coming here we know the parameter length is set to LOADER_NULL_DATA - check if this is possible

    // We're checking here ColumnMode that may have the following values:
    // mod_syskey, mod_key, mod_mandatory, mod_optional and mod_dropped
    // mod_syskey and mod_key:  those columns are key columns; should not appear at this place
    // mod_mandatory:           Not NULL column; But because we insert in case of not null
    //                          with default the default value we have to check here for
    //                          a default
    // mod_optional:            column allows NULL-values
    // mod_dropped:             shouldn't appear at this place
    if (pColumnData->SQLColumnInfo.ColumnMode <= LOADER_COLMODE_MANDATORY)
    {
        if (LOADER_SQLDEFAULT_NONE == pColumnData->SQLColumnInfo.DefaultType) {
            return LOA_ERR_NULL_NOT_ALLOWED;
        }
        else {
            pColumnData->lDataLength = LOADER_DEFAULT_DATA;        // NULL not allowed but default defined
        }
    }
    // nothing to do for LOADER_COLMODE_OPTIONAL

    return LOA_ERR_OK;
}
// getNILConstants()


/*
  -----------------------------------------------------------------------------
  function:     buildDefault
  -----------------------------------------------------------------------------
  // Error codes: LOA_ERR_MISSING_DATA_NN_COL, LOA_ERR_UNKNOWN_DEFAULT
*/
LOADER_ErrorCode
Loader_PageBuilder::buildDefault(LOADER_Byte*        pszDestination,
                                 LOADER_Int4&        lDestLen,
                                 Loader_ColumnData*  pFieldData,     //--> Indicator whether NULL or DEFAULT
                                 LOADER_Char&        bDefaultIsUnicode)
{
#ifdef LOADER_TRACING
    DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_PageBuilder, buildDefault, m_pTheCreator->m_pLoaderRuntime->m_pClientTrace);
#endif

    LOADER_ErrorCode rc = LOA_ERR_OK;

    if (LOADER_DEFAULT_DATA == pFieldData->lDataLength)
    {
        if (0 == pFieldData->SQLColumnInfo.StmtColumnNo)      // column not specified in command; check if null/default is allowed
        {
            if ( (LOADER_COLMODE_MANDATORY == pFieldData->SQLColumnInfo.ColumnMode) &&
                 (LOADER_SQLDEFAULT_NONE   == pFieldData->SQLColumnInfo.DefaultType) )
            {   
                // if column is not null defined (mandatory) but no default -> reject
                rc = LOA_ERR_MISSING_DATA_NN_COL;
            }
        }
    }

    // If a null value must be inserted we need to make sure that no default is inserted instead
    if (LOA_ERR_OK == rc)
    {
        lDestLen = Loader_SpecialConstants::InsertDefaultValue(pFieldData, pszDestination, 0);
        if (-1 != lDestLen)
        {
            if ( (LOADER_SQLDEFAULT_OTHER == pFieldData->SQLColumnInfo.DefaultType) ||
                 (UNDEFINED_BYTE          == *pszDestination) )
            {   // In case of user defined default (defined as 'create table ... col1 char(10) not null DEFAULT 'HALLO' ...')
                // the value is already in right form and length for unicode!; same applies if the value is the NULL value
                bDefaultIsUnicode = LOADER_TRUE;
            }
        }
        else
        {
            rc = LOA_ERR_UNKNOWN_DEFAULT;
        }
    }
    return rc;
}
// buildDefault()


/*
  -----------------------------------------------------------------------------
  function:     addValueToPageBuffer

  // Error codes:   LOA_ERR_MISSING_DATA_NN_COL,
                    LOA_ERR_UNKNOWN_DEFAULT
                    LOA_ERR_INTERNAL
                    LOA_ERR_CONVERTING_ENCODING
  -----------------------------------------------------------------------------
*/
LOADER_ErrorCode
Loader_PageBuilder::addValueToPageBuffer(LOADER_Byte*        pszDestination,
                                         LOADER_Int4&        lDestLen,
                                         Loader_ColumnData*  pFieldData)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    lDestLen = 0;                                           // making sure the length is initialized
    LOADER_Char bDefaultIsUnicode = 0;                      // flags if the value has to be processed in case of unicode (not if
                                                            // default type is other_default)

    // Define whether to use the pointer to data or the data buffer in pFieldData
    LOADER_Byte* _pData = (LOADER_TRUE == pFieldData->PtrUsed_ls00) ? pFieldData->pPtrToData
                                                                    : pFieldData->pDataBuffer;


    //if ( (LOADER_DEFAULT_DATA == pFieldData->lDataLength) || (LOADER_NULL_DATA == pFieldData->lDataLength) )
    
    if (pFieldData->lDataLength >= 0)       // This should be the most likely case
    {
        // first byte to insert is defined byte
        *pszDestination = pFieldData->cDefByte;

        lDestLen = pFieldData->DataOffset;       // Coming here the length IS still set to 0 from the very beginning of this function

        //*
        //*     INSERT USER DELIVERED VALUE
        //*
        if (0 != pFieldData->lDataLength)        // char values might be empty (consist entirely of blanks)
        {
            SAPDB_memcpy(&pszDestination[lDestLen], _pData, pFieldData->lDataLength);
            lDestLen += pFieldData->lDataLength;
        }
        //else      empty values consist only of a def byte on a database page if it is variable long
    }
    else
    {
        //*
        //*     INSERT DEFAULT OR NULL VALUE
        //* function delivers length of inserted default value INCLUDING the defined byte
        //*
        rc = buildDefault(pszDestination, lDestLen, pFieldData, bDefaultIsUnicode);
        if (LOA_ERR_OK != rc)
        {
            return rc;
        }
    }

    //*
    //*     UNICODE CONVERSION
    //*
    //* Necessary 
    //*  - if data type is 'dunicode' (all numbers are non unicode stored in the db,
    //*    and the user defined default value comes as right unicode value) or the value is the NULL value
    //*     AND
    //*  - if external encoding type is either UTF8 or ASCII
    //*  - if value is not the empty value
    //* In case of external encoding type UCS2 we have only to check the swap type and
    //* swap the char data to the right type.
    //*
    if ( (lDestLen > sizeof(LOADER_Char)) && (1 == pFieldData->bUnicodeConversion) && (LOADER_FALSE == bDefaultIsUnicode) )
    {
        rc = convertValues2UnicodeOnPage(pszDestination, lDestLen, pFieldData);
    }

    //*
    //*     LENGTH ADJUSTMENT
    //*
    //* Adjust column values depending on the length type - variable (trimming) or fix long (filling)
    //*
    if (LOA_ERR_OK == rc)
    {
        if (LOADER_COLLENTYPE_FIX == pFieldData->SQLColumnInfo.LengthType)
        {
            Loader_StrInsertFillBytes(pszDestination, lDestLen, pFieldData);

            lDestLen = pFieldData->SQLColumnInfo.ColumnBasics.SQLInOutLength;
        }
        else
        {
            //*
            //*     Trim variable long values; applies to numbers and to values in columns with COLUMNCOMPRESSION = YES
            //* the only case where number values may be trimmed is when a number column is
            //* the last (or only) primary key column which is always variable long
            //*
            if (lDestLen > 1)       // more than the required defined byte in the data?
            {
                if (0 == pFieldData->bUnicodeConversion)
                {
                    Loader_StrTrimRight(pszDestination, lDestLen, pFieldData->cFillByte);
                }
                else
                {
                    Loader_StrTrimRight(pszDestination, lDestLen, sp77encodingUCS2, pFieldData->cFillByte);
                }

                if (0 == lDestLen)          // In case the value consists entirely of DEF_BYTES the length is here
                {                           // 'adjusted' to 0. This must be corrected the way that at least the
                    lDestLen = 1;           // def_byte of the values 'survives'
                }
            }
        }
    }

    // Write length to the beginning of the value for special dbls data type
    if (3 == pFieldData->DataOffset)
    {
        LOADER_Int2 _len = static_cast<LOADER_Int2>(lDestLen - pFieldData->DataOffset);
        Loader_Swap2BIntToChar(_len, &pszDestination[sizeof(LOADER_Char)], (LOADER_SWAP_TYPE != m_KernelSwap));
    }

    return rc;
}
// addValueToPageBuffer()


/*
  -----------------------------------------------------------------------------
  function:     buildRecord

  // Error codes:   LOA_ERR_NULL_NOT_ALLOWED
                    LOA_ERR_INVALID_HEX
                    LOA_ERR_INCOMPATIBLE_CONST_LENGTH
                    LOA_ERR_SYSKEY_EXCEEDED
                    LOA_ERR_MISSING_DATA_NN_COL
                    LOA_ERR_UNKNOWN_DEFAULT
                    LOA_ERR_INTERNAL             // At converting from src_encoding to tgt_encoding
                    LOA_ERR_CONVERTING_ENCODING
                    LOA_ERR_KEY_LENGTH
                    LOA_ERR_KEY_ORDER
                    LOA_ERR_NUMBER_OVERFLOW
                    LOA_ERR_EMPTY_NOT_ALLOWED
                    LOA_ERR_EMPTY_NOT_ALLOWED
                    LOA_ERR_EMPTY_NOT_ALLOWED
  -----------------------------------------------------------------------------
*/
LOADER_Int4
Loader_PageBuilder::buildRecord()
{
    LOADER_ErrorCode  rc      = LOA_ERR_OK;

    LOADER_Int4  lTrimLen   = 0;

    // Record length variables
    // We work here with some constant position offset in the record. Those offsets are defined in
    // the header file.
    LOADER_Int4  plBufLen   = LENGTH_OFFSET;   // Datensatz startet mit laengenbytes --> ueberspring diese
    LOADER_Int4  lRecordLen = LENGTH_OFFSET;
    LOADER_Int4  lKeyLen    = 0;
    LOADER_Int2  i          = 0;

    for (i; ( (i < m_ParameterCount) && (LOA_ERR_OK == rc) ); ++i)
    {
        Loader_ColumnData&   ColDataArrayElem = m_pColumnDataArray[i];
        //*
        //*     Specified value in data file is NOT EMPTY.
        //*
        if ( (LOADER_EMPTY_DATA != ColDataArrayElem.lDataLength) && (0 != ColDataArrayElem.lDataLength) )
        {
            if (ColDataArrayElem.lDataLength > 0)    // Only if values have a 'real' length
            {
                //*
                //*     Trim values right before transformation - remove trailing blanks
                //*
                if ( (1 == LOADER_MAP_CHAR_HOSTTYPE[ColDataArrayElem.HostType]) &&
                     !( (LOADER_COLLENTYPE_FIX == ColDataArrayElem.SQLColumnInfo.LengthType) &&
                        (ColDataArrayElem.lDataLength <= ColDataArrayElem.SQLColumnInfo.ColumnBasics.SQLLength) ) )
                {
                    lTrimLen  = ColDataArrayElem.lDataLength;
                    Loader_StrTrimRight(ColDataArrayElem.pPtrToData, lTrimLen, ColDataArrayElem.HostType);
                    ColDataArrayElem.lDataLength = lTrimLen;
                }
            }
            else // the only possible length here beside 0 or a length > 0 is LOADER_NULL_DATA:  if (LOADER_NULL_DATA == ColDataArrayElem.lDataLength)
            {
                //*
                //*     Check for NULL value representation
                //* - NULL value representation may only be indicated if there are columns specified
                //*   in the FASTLOAD command
                //*
                rc = getNILConstants(&ColDataArrayElem);
                if (LOA_ERR_OK == rc)           // The only possible error code here is: LOA_ERR_NULL_NOT_ALLOWED
                {
                    // In case the NULL-check went without problems we have anyway 2 cases to distinguish:
                    // 1. there is a null-value --> continue with the main for loop
                    // 2. there is a real value
                    if (ColDataArrayElem.lDataLength < 0)
                    {
                        continue;
                    }
                }
            }

            //*
            //*     Check for hex values
            //* - HEX value indication is possible only if there are columns specified in the
            //*   FASTLOAD command (example FASTLOAD TABLE <name> <col name> 1 HEX);
            //*   but HEX values are requested also if the column to be loaded is of sql blob type
            //*   which may be the case if no column is specified in the FASTLOAD command, too
            //*
            if (LOA_ERR_OK == rc)
            {
                rc = checkForHexValues(i);
            }
            //*
            //*     Check for
            //*  - special constants like date, time, timestamp, boolean
            //*  - NUMBER values (convert to internal db number representation)
            //*  - special CHAR values (only dbsl)
            //*
            if (LOA_ERR_OK == rc)
            {
                rc = checkForSpecialConstants(i);
            }
        }
        else    // of if (LOADER_EMPTY_DATA != ColDataArrayElem.lDataLength) ...
        {
            //*
            //*     Handle empty data
            //*

            // NULL-value check may reveal that the read value does not represent the NULL value;
            // in this case the length remains unchanged (INSERT_EMPTY_VALUE)

            // Check if column is specified in the FASTLOAD command; if not the
            // value to be inserted is signaled by setting the length to LOADER_DEFAULT_DATA
//            rc = DoingForEmptyData(i);


            if (0 == ColDataArrayElem.SQLColumnInfo.StmtColumnNo)
            {
                ColDataArrayElem.lDataLength = LOADER_DEFAULT_DATA;
            }
            else
            {
                // nothing read and null condition not fullfilled; check if data is of data type char
                if ( (1 == LOADER_MAP_CHARDATATYPE[ColDataArrayElem.SQLColumnInfo.ColumnBasics.SQLDataType]) ||
                        (1 == LOADER_MAP_BLOBDATATYPE[ColDataArrayElem.SQLColumnInfo.ColumnBasics.SQLDataType]) )
                {
                    if (LOADER_SQLMODE_ORACLE == m_SQLMode)
                    {
                        // if sqlmode = oracle an empty field means to insert NULL; check if it is defined
                        if (LOADER_COLMODE_OPTIONAL == ColDataArrayElem.SQLColumnInfo.ColumnMode)
                        {
                            ColDataArrayElem.lDataLength = LOADER_NULL_DATA;
                        }
                        else
                        {
                            rc = LOA_ERR_NULL_NOT_ALLOWED;
                        }
                    }
                    else
                    {
                        // Ok, empty field means to insert BLANKs; we'll set the length to 0 and
                        // when building the page the value is either filled to the needed length
                        // (fixed long) or nothing else to do (variable long)
                        ColDataArrayElem.lDataLength = 0;
                    }
                }
                else
                {
                    rc = LOA_ERR_EMPTY_NOT_ALLOWED;
                }
            }   // end else of if (0 == m_pColumnDataArray[i].SQLColumnInfo.StmtColumnNo)




        }   // end else of if (LOADER_EMPTY_DATA != ColDataArrayElem.lDataLength)





        //*
        //*     Check the value length. 
        //* Length of numeric values is checked twice because the first time check is done
        //* when converting to internal number format; shouldn't be a problem
        //*
        if ( (ColDataArrayElem.lDataLength >= 0) && (LOA_ERR_OK == rc) )
        {
            // The only possible error code here is: LOA_ERR_INCOMPATIBLE_CONST_LENGTH
            rc = checkValueLength(ColDataArrayElem.lDataLength, ColDataArrayElem.SQLColumnInfo.ColumnBasics);
        }
    }   // end for (i=0; ( (errOK_els00 == rc) && (i < pFLRec->flrDataSource_ls00.lReadFieldsCnt_ls00) ); ++i)


    if (LOA_ERR_OK == rc)       // No problems so far?!
    {
        LOADER_Int4   lValLen       = 0;
        LOADER_Bool   bSwapLength   = (LOADER_SWAP_TYPE != m_KernelSwap);
        LOADER_UInt4* pFillIndex    = m_plFillIndex;
        LOADER_UInt4* pFillIndexEnd = m_plFillIndex + m_ColumnCount;

        //*
        //*     Fill the db record buffer.
        //*
        //* Recordlayout 7.2 and higher
        //* |------------------------------------------------------------------------------------------ ... ---------- ... -----
        //* | record |  key   | var col| var col|  fix   |  fix   |  var   |  fix   |  fix   |  var   |     | var    |
        //* | length | length | offset | count  | key 1  | key 2  |  key   | col 1  | col 2  | col 1  |     |longchar|
        //* |        |        |        |        |        |        |        |        |        |        |     | col 1  |
        //* |   2    |   2    |   2    |   2    |        |        |        |        |        |        |     |        |
        //* | bytes  | bytes  | bytes  | bytes  |        |        |        |        |        |        |     |        |
        //* |------------------------------------------------------------------------------------------ ... ---------- ... -----
        //*
        //*          <-- KEYLEN_OFFSET_LS23 = 2 bytes
        //*                   <--   VARCOL_OFFSET_LS23 = 4 bytes
        //*                            <--  VARCOL_COUNT_OFFSET_LS23 = 6 bytes
        //*                                     <-- HEADER_OFFSET_LS23 = 8 bytes

        //*
        //*     BEGIN KEY VALUES
        //*
        rc = buildKey(pFillIndex, plBufLen, lKeyLen);
        if (LOA_ERR_OK == rc)
        {
            lRecordLen += lKeyLen;      // computing record length first time
        }
        //*
        //*     END KEY VALUES
        //*

        //*
        //*     BEGIN FIX LONG COLUMNS
        //*
        if ( (0 != m_FixColumnsLength) && (LOA_ERR_OK == rc) )
        {
            for (pFillIndex; pFillIndex < pFillIndexEnd; ++pFillIndex)
            {
                Loader_ColumnData&   ColDataArrayElem = m_pColumnDataArray[*pFillIndex];

                if (LOADER_COLLENTYPE_FIX == ColDataArrayElem.SQLColumnInfo.LengthType) 
                {
                    rc = addValueToPageBuffer(&m_pDataBuffer[plBufLen], lValLen, &ColDataArrayElem);
                    if (LOA_ERR_OK == rc) {
                        plBufLen += lValLen;
                    }
                    else
                        break;
                }
                else
                    break;
            }

            if (LOA_ERR_OK == rc)   // Adjust record length: m_FixColumnsLength contains def-byte, too
            {
                lRecordLen += m_FixColumnsLength;
            }
        }   // end if ( (LOA_ERR_OK == rc) && (0 != m_FixColumnsLength) )


        //*
        //*     begin VARIABLE LONG COLUMNS
        //*
        // coming here only variable long columns may remain; we process normal long (<= 255 bytes) and very long (> 255 bytes)
        // variable long columns with the same procedure
        if ( (pFillIndex < pFillIndexEnd) && (LOA_ERR_OK == rc) )
        {
            LOADER_Byte* pszMemo = NULL;    // To memorize the point where to write the col length

            for (pFillIndex; pFillIndex < pFillIndexEnd; ++pFillIndex)
            {
                Loader_ColumnData&   ColDataArrayElem = m_pColumnDataArray[*pFillIndex];

                // Skip (but memorize) position for length information of that column
                pszMemo = &m_pDataBuffer[plBufLen];
                
                // Adjust lengths: 1 byte for long var and 2 bytes for very long var
                plBufLen   += ColDataArrayElem.LengthInfoByteCount;
                lRecordLen += ColDataArrayElem.LengthInfoByteCount;
                
                rc = addValueToPageBuffer(&m_pDataBuffer[plBufLen], lValLen, &ColDataArrayElem);
                if (LOA_ERR_OK == rc)
                {
                    plBufLen   += lValLen;
                    lRecordLen += lValLen;

                    // write value length to buffer
                    if (LOADER_COLLENTYPE_VAR == ColDataArrayElem.SQLColumnInfo.LengthType)
                    {
                        *pszMemo = static_cast<LOADER_Byte>(lValLen);       // maximum length for this column type is 255
                    }
                    else
                    {
                        Loader_Swap2BIntToChar(static_cast<LOADER_Int2>(lValLen), pszMemo, bSwapLength);
                    }
                }   // end if (errOK_els00 == rc)
                else
                    break;
            
            }   // end for (j; j < pTI->tdFieldCount_ls00; ++j)
        }

        //*
        //*     end VARIABLE LONG COLUMNS
        //*
        
        //*
        //*     Write record header
        //*

        if (LOA_ERR_OK == rc)
        {
            // key length
            Loader_Swap2BIntToChar(static_cast<LOADER_Int2>(lKeyLen), &m_pDataBuffer[KEYLEN_OFFSET], bSwapLength);

            // Offset of variable long columns; overall length of fix long columns corresponds to
            // the offset of variable long columns
            m_pDataBuffer[VARCOL_OFFSET]   = m_szFixColumnsLength[0];
            m_pDataBuffer[VARCOL_OFFSET+1] = m_szFixColumnsLength[1];

            // Count of variable long columns (length <= 255 bytes);
            // Even if there are none of those this needs to be mentioned
            m_pDataBuffer[VARCOL_COUNT_OFFSET]   = m_szVarColumnsCount[0];
            m_pDataBuffer[VARCOL_COUNT_OFFSET+1] = m_szVarColumnsCount[1];

            // record length
            Loader_Swap2BIntToChar(static_cast<LOADER_Int2>(lRecordLen), &m_pDataBuffer[BUF_START_POS], bSwapLength);
        }

        if (LOA_ERR_OK != rc)
        {
            lRecordLen = rc;
            m_lErrDataIndex = *pFillIndex-1;//j-1;
            m_lErrColNameIndex = *pFillIndex-1;//j-1;
        }
    }   // end else of if (LOA_ERR_OK != rc)
    else
    {
        m_lErrDataIndex    = i-1;       // as we do not break upon error we have to substract 1
        m_lErrColNameIndex = i;
        if (LOADER_FALSE == m_TableHasSyskey)
        {
            --m_lErrColNameIndex;
        }

        // The return value!
        lRecordLen = rc;
    }
    
    if (LOA_ERR_OK != rc)
    {
        buildErrorString(rc, lKeyLen);
    }

    return lRecordLen;  // We're done.
}
// buildRecord()


LOADER_ErrorCode
Loader_PageBuilder::convertToVDNNumber(Loader_ColumnData&  ColumnData)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    if (1 == LOADER_MAP_NUMBER_HOSTTYPE[ColumnData.HostType])
    {
        switch (ColumnData.HostType)
        {
            case LOADER_HostType_UINT1:
            {
                rc = Loader_VDNNumber::uint1ToNumber(*reinterpret_cast<LOADER_UInt1*>(ColumnData.pPtrToData),
                                                     ColumnData.pDataBuffer,
                                                     ColumnData.SQLColumnInfo.ColumnBasics.SQLLength);
                break;
            }
            case LOADER_HostType_INT1:
            {
                rc = Loader_VDNNumber::int1ToNumber(*reinterpret_cast<LOADER_Int1*>(ColumnData.pPtrToData),
                                                    ColumnData.pDataBuffer,
                                                    ColumnData.SQLColumnInfo.ColumnBasics.SQLLength);
                break;
            }

            case LOADER_HostType_UINT2:
            {
                rc = Loader_VDNNumber::uint2ToNumber(*reinterpret_cast<LOADER_UInt2*>(ColumnData.pPtrToData),
                                                     ColumnData.pDataBuffer,
                                                     ColumnData.SQLColumnInfo.ColumnBasics.SQLLength);
                break;
            }
            case LOADER_HostType_INT2:
            {
                rc = Loader_VDNNumber::int2ToNumber(*reinterpret_cast<LOADER_Int2*>(ColumnData.pPtrToData),
                                                    ColumnData.pDataBuffer,
                                                    ColumnData.SQLColumnInfo.ColumnBasics.SQLLength);
                break;
            }

            case LOADER_HostType_UINT4:
            {
                rc = Loader_VDNNumber::uint4ToNumber(*reinterpret_cast<LOADER_UInt4*>(ColumnData.pPtrToData),
                                                     ColumnData.pDataBuffer,
                                                     ColumnData.SQLColumnInfo.ColumnBasics.SQLLength);
                break;
            }
            case LOADER_HostType_INT4:
            {
                rc = Loader_VDNNumber::int4ToNumber(*reinterpret_cast<LOADER_Int4*>(ColumnData.pPtrToData),
                                                    ColumnData.pDataBuffer,
                                                    ColumnData.SQLColumnInfo.ColumnBasics.SQLLength);
                break;
            }

            case LOADER_HostType_UINT8:
            {
                rc = Loader_VDNNumber::uint8ToNumber(*reinterpret_cast<LOADER_UInt8*>(ColumnData.pPtrToData),
                                                     ColumnData.pDataBuffer,
                                                     ColumnData.SQLColumnInfo.ColumnBasics.SQLLength);
                break;
            }
            case LOADER_HostType_INT8:
            {
                rc = Loader_VDNNumber::int8ToNumber(*reinterpret_cast<LOADER_Int8*>(ColumnData.pPtrToData),
                                                    ColumnData.pDataBuffer,
                                                    ColumnData.SQLColumnInfo.ColumnBasics.SQLLength);
                break;
            }
            case LOADER_HostType_DOUBLE:        // Attention: double fall through
            case LOADER_HostType_FLOAT:
            {
                LOADER_Int4 precision = -1;

                if(LOADER_SQLTYPE_FLOAT != ColumnData.SQLColumnInfo.ColumnBasics.SQLDataType)
                {
                    precision = ColumnData.SQLColumnInfo.ColumnBasics.SQLFraction;
                }
                rc = Loader_VDNNumber::doubleToNumber(*reinterpret_cast<double*>(ColumnData.pPtrToData),
                                                    ColumnData.pDataBuffer,
                                                    ColumnData.SQLColumnInfo.ColumnBasics.SQLLength,
                                                    precision);
                break;
            }
            case LOADER_HostType_DECIMAL:
            {
                LOADER_Int4 _decimal_length     = LOADER_DECIMAL_DIGITS(ColumnData.lDataLength);
                LOADER_Int4 _decimal_precision  = LOADER_DECIMAL_FRACTION(ColumnData.lDataLength);
                LOADER_Int4 _result_precision   = -1;
                if(LOADER_SQLTYPE_FLOAT != ColumnData.SQLColumnInfo.ColumnBasics.SQLDataType)
                {
                    _result_precision = ColumnData.SQLColumnInfo.ColumnBasics.SQLFraction;
                }
                rc = Loader_VDNNumber::decimalToNumber(ColumnData.pPtrToData,
                                                       _decimal_length,
                                                       _decimal_precision,
                                                       ColumnData.pDataBuffer,
                                                       ColumnData.SQLColumnInfo.ColumnBasics.SQLLength,
                                                       _result_precision);
                break;
            }
            // default?
        }
    }
    else if (1 == LOADER_MAP_CHAR_HOSTTYPE[ColumnData.HostType])
    {
        LOADER_StringEncoding  encoding  = LOA_MAP_HOSTTYPE_TO_ENCODING[ColumnData.HostType];
        LOADER_Bool            bNAN      = LOADER_FALSE;
        LOADER_Bool            bFixed    = LOADER_FALSE;
        LOADER_Int4            precision = -1;
        if(LOADER_SQLTYPE_FLOAT != ColumnData.SQLColumnInfo.ColumnBasics.SQLDataType)
        {
            precision = ColumnData.SQLColumnInfo.ColumnBasics.SQLFraction;
            bFixed    = LOADER_TRUE;
        }

        rc = Loader_VDNNumber::stringToNumber(ColumnData.pPtrToData,
                                              ColumnData.lDataLength,
                                              LOA_MAP_HOSTTYPE_TO_ENCODING[ColumnData.HostType],
                                              bNAN,
                                              ColumnData.pDataBuffer,
                                              bFixed,
                                              ColumnData.SQLColumnInfo.ColumnBasics.SQLLength,
                                              precision);
    }
    else
    {
        rc = LOA_ERR_INVALID_HOSTTYPE;  // invalid hosttype!
    }

    // Assign right length value
    ColumnData.lDataLength = ColumnData.SQLColumnInfo.ColumnBasics.SQLInOutLength - 1;

    // Set flag signaling that the data is stored in pDataBuffer member
    ColumnData.PtrUsed_ls00 = LOADER_FALSE;


    // Check for right range in case of integer values
    if ( (LOA_ERR_OK == rc) &&
         ( (LOADER_SQLTYPE_SMALLINT == ColumnData.SQLColumnInfo.ColumnBasics.SQLDataType) ||
           (LOADER_SQLTYPE_INTEGER  == ColumnData.SQLColumnInfo.ColumnBasics.SQLDataType) ) )
    {
        rc = Loader_VDNNumber::checkIntRange(ColumnData.pDataBuffer,
                                             ColumnData.lDataLength,
                                             ColumnData.SQLColumnInfo.ColumnBasics.SQLDataType);
    }
    return rc;
}
// convertToVDNNumber()



LOADER_ErrorCode
Loader_PageBuilder::convert2InternalEncoding(Loader_ColumnData* pColumnData)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    //*
    //*     FIRST TRANSFORMATION of delivered value: bring it into 'internal'
    //*     encoding (ascii so far) to process it if the external encoding
    //*     is UNICODE (i.e. UCS2 or UTF8).
    //*
    //* Transformation to 'internal' encoding for values of type
    //*  - date, time, timestamp (value must be brought into the right format and
    //*    transformation routines may only handle ascii DATA)
    //*  - boolean - delivered as external user defined representation
    //*  - numeric - delivered as readable value
    //* No transformation for values of all other types because
    //*  - char/varchar - transformed later with regards to the external representation
    //*  - numeric in numerical format, for instance FLOAT or INTEGER

    if (1 == LOADER_MAP_CHAR_HOSTTYPE[pColumnData->HostType])
    {
        if (LOADER_HostType_ASCII == pColumnData->HostType)
        {
            return rc;
        }

        LOADER_UInt4 _len = pColumnData->lDataLength;

        if (LOADER_HostType_UTF8 == pColumnData->HostType)
        {
            LOADER_Char szErrText[1024];
            szErrText[0] = 0;

            // Incoming source string is already UTF8
            Tools_DynamicUTF8String Source;
            Source.AssignRaw(STATIC_CAST(const LOADER_UTF8*, pColumnData->pPtrToData), STATIC_CAST(LOADER_UInt4, _len));

            // From this point on we use the data buffer not longer the buffer the ptr points to
            rc = Loader_StrConvertFromUTF8(pColumnData->pDataBuffer,
                                           _len,
                                           Source,
                                           LOADER_STRINGENCODING_ASCII,
                                           szErrText);
        }
        else if ( (LOADER_HostType_UCS2_SWAPPED == pColumnData->HostType) || (LOADER_HostType_UCS2 == pColumnData->HostType) )
        {
            rc = Loader_StrConvertUCS2ToASCII(pColumnData->pDataBuffer,
                                              _len,
                                              pColumnData->pPtrToData,
                                              pColumnData->lDataLength,
                                              LOA_MAP_HOSTTYPE_TO_ENCODING[pColumnData->HostType]);
        }
        if (LOA_ERR_OK == rc)
        {
            pColumnData->lDataLength = _len;
            pColumnData->PtrUsed_ls00 = LOADER_FALSE;
        }
    }

    return rc;
}
// convert2InternalEncoding()


LOADER_ErrorCode
Loader_PageBuilder::convertValues2UnicodeOnPage(LOADER_Byte*        pszDestination,
                                                LOADER_Int4&        lDestLen,
                                                Loader_ColumnData*  pFieldData)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    if ( (LOADER_HostType_UCS2_SWAPPED == pFieldData->HostType) || (LOADER_HostType_UCS2 == pFieldData->HostType) )
    {
        // Data in UCS2 is ALWAYS saved in little endian order on page!
        if (LOADER_HostType_UCS2_SWAPPED == pFieldData->HostType)
        {
            Loader_SwapUCS2StringInPlace((pszDestination + pFieldData->DataOffset), lDestLen - pFieldData->DataOffset);
        }
    }
    else
    {
        LOADER_Byte szTempBuffer[PAGE_MXSP00]; // a single value may not be bigger than a whole db page

        SAPDB_memcpy(szTempBuffer, pszDestination + pFieldData->DataOffset, lDestLen - pFieldData->DataOffset);   // skip defined byte

        LOADER_UInt4 lTmpSrcLen  = pFieldData->SQLColumnInfo.ColumnBasics.SQLInOutLength;

        rc = Loader_StrConvertToUCS2NotSwapped((pszDestination + pFieldData->DataOffset),
                                                lTmpSrcLen,
                                                szTempBuffer,
                                                lDestLen - pFieldData->DataOffset,
                                                LOA_MAP_HOSTTYPE_TO_ENCODING[pFieldData->HostType]);
        if (0 == rc)
        {
            lDestLen = lTmpSrcLen + pFieldData->DataOffset;
        }
    }
    return rc;
}


void
Loader_PageBuilder::buildErrorString(LOADER_ErrorCode rc, const LOADER_Int4 lKeyLength)
{
    // Build the error message - well, in case ...
    LOADER_Byte* p = (LOADER_TRUE == m_pColumnDataArray[m_lErrDataIndex].PtrUsed_ls00) ?
                        m_pColumnDataArray[m_lErrDataIndex].pPtrToData : m_pColumnDataArray[m_lErrDataIndex].pDataBuffer;

    switch (rc)
    {
        // 4 times fall through!
        case LOA_ERR_NULL_NOT_ALLOWED:
        case LOA_ERR_INVALID_HEX:
        case LOA_ERR_MISSING_DATA_NN_COL:
        case LOA_ERR_UNKNOWN_DEFAULT:
        {
            m_pTheCreator->m_pError->setLoaderError(rc, static_cast<Loader_TableDescription*>(m_pTheCreator->m_pTableDescription)->pszColumnNames[m_lErrColNameIndex]);
            break;
        }
        case LOA_ERR_INCOMPATIBLE_CONST_LENGTH:
        {
            m_pTheCreator->m_pError->setLoaderError(rc,
                                                    m_pColumnDataArray[m_lErrDataIndex].lDataLength,
                                                    p,
                                                    m_pColumnDataArray[m_lErrDataIndex].lDataLength,
                                                    static_cast<Loader_TableDescription*>(m_pTheCreator->m_pTableDescription)->pszColumnNames[m_lErrColNameIndex]);
            break;
        }
        case LOA_ERR_WRONG_DATE_TIME:
        {
            m_pTheCreator->m_pError->setLoaderError(rc,
                                                    m_lErrColNameIndex,
                                                    m_pColumnDataArray[m_lErrDataIndex].lDataLength,
                                                    p,
                                                    LOADER_AncientDateTimeErrText[m_SpecialDateTimeError - 17404]);
            m_SpecialDateTimeError = 0;
            break;
        }
        case LOA_ERR_NUMBER_OVERFLOW:
        {
            m_pTheCreator->m_pError->setLoaderError(rc, m_lErrColNameIndex);
            break;
        }
        case LOA_ERR_INVALID_HOSTTYPE:
        {
            m_pTheCreator->m_pError->setLoaderError(rc,
                                                    LOADER_SQLTypeToString(m_pColumnDataArray[m_lErrDataIndex].SQLColumnInfo.ColumnBasics.SQLDataType),
                                                    m_lErrColNameIndex);
            break;
        }
        case LOA_ERR_EMPTY_NOT_ALLOWED:
        {
            m_pTheCreator->m_pError->setLoaderError(rc,
                                                    LOADER_HostTypeToString(m_pColumnDataArray[m_lErrDataIndex].HostType),
                                                    m_lErrColNameIndex);
            break;
        }
        case LOA_ERR_KEY_ORDER:
        {
            LOADER_Int4   i   = 0;
            SAPDB_Int4    _pos = 0;
            SAPDB_Int4    _posbin = 0;
            LOADER_Int4   _len = (m_LastKeyLength >= lKeyLength) ? m_LastKeyLength : lKeyLength;
            LOADER_Char*  pszLastKey = new SAPDB_Char[_len + 2];
            LOADER_Char*  pszNewKey = new SAPDB_Char[_len + 2];
            LOADER_Int4   _binlen = 2 * _len;
            LOADER_Char*  pszLastKeyBin = new SAPDB_Char[_binlen + 2];
            LOADER_Char*  pszNewKeyBin = new SAPDB_Char[_binlen + 2];
            pszLastKey[0] = 0;
            pszNewKey[0]  = 0;
            pszLastKeyBin[0] = 0;
            pszNewKeyBin[0] = 0;

            _len += 2;          //sp77sprintf checks for available space to add the trailing 0
            _binlen += 2;

            for (i = 0, _pos = 0, _posbin = 0; i < m_LastKeyLength; ++i)
            {
                if (0 == isprint(m_LastKey[i]))
                {
                    _pos += sp77sprintf(pszLastKey + _pos, _len - _pos, ".");
                }
                else
                {
                    _pos += sp77sprintf(pszLastKey + _pos, _len - _pos, "%c",  m_LastKey[i]);
                }

                if (0 == m_LastKey[i])
                {
                    _posbin += sp77sprintf(pszLastKeyBin + _posbin, _binlen - _posbin, "00");
                }
                else
                {
                    _posbin += sp77sprintf(pszLastKeyBin + _posbin, _binlen - _posbin, "%X",  m_LastKey[i]);
                }
            }
            
            for (i = 0, _pos = 0, _posbin = 0; i < lKeyLength; ++i)
            {
                if (0 == isprint(m_pDataBuffer[HEADER_OFFSET + i]))
                {
                    _pos += sp77sprintf(pszNewKey + _pos, _len - _pos, ".");
                }
                else
                {
                    _pos += sp77sprintf(pszNewKey + _pos, _len - _pos, "%c",  m_pDataBuffer[HEADER_OFFSET + i]);
                }

                if (0 == m_pDataBuffer[HEADER_OFFSET + i])
                {
                    _posbin += sp77sprintf(pszNewKeyBin + _posbin, _binlen - _posbin, "00");
                }
                else
                {
                    _posbin += sp77sprintf(pszNewKeyBin + _posbin, _binlen - _posbin, "%X",  m_pDataBuffer[HEADER_OFFSET + i]);
                }
            }

            m_pTheCreator->m_pError->setLoaderError(rc, pszLastKey, pszLastKeyBin, pszNewKey, pszNewKeyBin);

            delete [] pszLastKey;
            delete [] pszLastKeyBin;
            delete [] pszNewKey;
            delete [] pszNewKeyBin;
            break;
        }
        // five time fall through!
        case LOA_ERR_SYSKEY_EXCEEDED:
        case LOA_ERR_KEY_LENGTH:
        case LOA_ERR_INTERNAL:             // At converting from src_encoding to tgt_encoding
        case LOA_ERR_CONVERTING_ENCODING:
        {
            m_pTheCreator->m_pError->setLoaderError(rc);
            break;
        }
    }   // end switch (rc)
}
// buildErrorString()


LOADER_ErrorCode
Loader_PageBuilder::checkForHexValues(LOADER_Int2 i)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    if ( ( (LOADER_TRUE == m_pColumnDataArray[i].ExtHexRepresentation) ||
           ( (1     == LOADER_MAP_BLOBDATATYPE[m_pColumnDataArray[i].SQLColumnInfo.ColumnBasics.SQLDataType]) &&
             (0     == LOADER_MAP_LONGDATATYPE[m_pColumnDataArray[i].SQLColumnInfo.ColumnBasics.SQLDataType]) &&
             (LOADER_HostType_BINARY != m_pColumnDataArray[i].HostType) ) ) )
    {                                                       // -->  umwandeln in byte
        LOADER_Int4  _lnewlen = m_pColumnDataArray[i].lDataBufferLength;

        rc = Loader_X2C(reinterpret_cast<LOADER_Char*>(m_pColumnDataArray[i].pPtrToData),
                        m_pColumnDataArray[i].lDataLength,
                        m_pColumnDataArray[i].pDataBuffer,
                        _lnewlen);
        if (LOA_ERR_OK == rc)           // The only possible error code here is: LOA_ERR_INVALID_HEX
        {
            m_pColumnDataArray->PtrUsed_ls00  = LOADER_FALSE;
            m_pColumnDataArray[i].lDataLength = _lnewlen;
        }
    }
    return rc;
}
// checkForHexValues()


LOADER_ErrorCode
Loader_PageBuilder::checkForSpecialConstants(LOADER_Int2 i)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    if (1 == LOADER_MAP_NUMBERDATATYPE[m_pColumnDataArray[i].SQLColumnInfo.ColumnBasics.SQLDataType])
    {
        //*
        //*     Number conversion - conversion routine takes encoding into account
        //*
        if (LOA_ERR_OK == rc)
        {
            rc = convertToVDNNumber(m_pColumnDataArray[i]);
            if (LOA_ERR_OK != rc)
            {
                if (LOA_ERR_NUMBER_TRUNCATED == rc)
                {
                    m_DataTruncated = LOADER_TRUE;
                    rc = LOA_ERR_OK;
                }
            }
        }
    }
    else if (1 == LOADER_MAP_SPECCONSTDATATYPE[m_pColumnDataArray[i].SQLColumnInfo.ColumnBasics.SQLDataType])
    {
        if (LOADER_SQLTYPE_BOOLEAN == m_pColumnDataArray[i].SQLColumnInfo.ColumnBasics.SQLDataType)
        {
            rc = Loader_SpecialConstants::convertToBoolean(&m_pColumnDataArray[i]);
        }
        else // date, time, timestamp
        {
            rc = convert2InternalEncoding(&m_pColumnDataArray[i]);
            if (LOA_ERR_OK == rc)
            {
                m_SpecialDateTimeError = Loader_SpecialConstants::convertDateTimeValues(&m_pColumnDataArray[i],
                                                                                        &m_DateTimeMask,
                                                                                        m_KernelDateTimeFormat);
                if (0 != m_SpecialDateTimeError)
                {
                    rc = LOA_ERR_WRONG_DATE_TIME;
                }
            }
        }
    }

    return rc;
}
// checkForSpecialConstants()


LOADER_ErrorCode
Loader_PageBuilder::DoingForEmptyData(LOADER_Int2 i)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    // NULL-value check may reveal that the read value does not represent the NULL value;
    // in this case the length remains unchanged (INSERT_EMPTY_VALUE)

    // Check if column is specified in the FASTLOAD command; if not the
    // value to be inserted is signaled by setting the length to LOADER_DEFAULT_DATA
    if (0 == m_pColumnDataArray[i].SQLColumnInfo.StmtColumnNo)
    {
        m_pColumnDataArray[i].lDataLength = LOADER_DEFAULT_DATA;
    }
    else
    {
        // nothing read and null condition not fullfilled; check if data is of data type char
        if ( (1 == LOADER_MAP_CHARDATATYPE[m_pColumnDataArray[i].SQLColumnInfo.ColumnBasics.SQLDataType]) ||
                (1 == LOADER_MAP_BLOBDATATYPE[m_pColumnDataArray[i].SQLColumnInfo.ColumnBasics.SQLDataType]) )
        {
            if (LOADER_SQLMODE_ORACLE == m_SQLMode)
            {
                // if sqlmode = oracle an empty field means to insert NULL; check if it is defined
                if (LOADER_COLMODE_OPTIONAL == m_pColumnDataArray[i].SQLColumnInfo.ColumnMode)
                {
                    m_pColumnDataArray[i].lDataLength = LOADER_NULL_DATA;
                }
                else
                {
                    rc = LOA_ERR_NULL_NOT_ALLOWED;
                }
            }
            else
            {
                // Ok, empty field means to insert BLANKs; we'll set the length to 0 and
                // when building the page the value is either filled to the needed length
                // (fixed long) or nothing else to do (variable long)
                m_pColumnDataArray[i].lDataLength = 0;
            }
        }
        else
        {
            rc = LOA_ERR_EMPTY_NOT_ALLOWED;
        }
    }   // end else of if (0 == m_pColumnDataArray[i].SQLColumnInfo.StmtColumnNo)

    return rc;
}
// DoingForEmptyData()


LOADER_ErrorCode
Loader_PageBuilder::buildKey(LOADER_UInt4*& plFillIndex, LOADER_Int4& plBufLen, LOADER_Int4& lKeyLen)//buildKey(LOADER_Int4& j, LOADER_Int4& plBufLen, LOADER_Int4& lKeyLen)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    LOADER_Int4 lValLen    = 0;

    if (LOADER_TRUE == m_TableHasSyskey)
    {
        //*
        //*     SYSKEY COLUMN
        //*
        //  Increase m_LastKey and write value to buffer, no further checks necessary
        //  m_LastKeyLength is first time filled in setLastInsertedKey()

        // The only possible error code here is: LOA_ERR_SYSKEY_EXCEEDED
        rc = Loader_SpecialConstants::increaseSysKey(&m_LastKey[3]);
        if (LOA_ERR_OK == rc)
        {
            SAPDB_memcpy(&m_pDataBuffer[plBufLen], m_LastKey, m_LastKeyLength);
            plBufLen += m_LastKeyLength;    // mv bufpointer behind copied value
            lKeyLen  += m_LastKeyLength;
            ++plFillIndex;
        }
    }
    else
    {
        //*
        //*     USER DEFINED KEY COLUMNS
        //*
        //  All key values must be specified in fastload command or none of all is specified.
        //  Data for key columns must be supplied by the user

        //  Keylayout
        //  |-------------------...--------------------------------------- ...
        //  | def  |  fixed  || ... || def  | fixed   || def  |  var    ||  other values
        //  | byte |key value|| ... || byte |key value|| byte |key value||
        //  |-------------------...--------------------------------------- ...
        //
        //  Key values have no length specification in front

        for (plFillIndex; ; ++plFillIndex)
        {
            // This for-loop is left when the last key column is processed using a break because
            // the last key column is always a column of length type LOADER_COLLENTYPE_VAR or on error (of course)
            rc = addValueToPageBuffer(&m_pDataBuffer[plBufLen], lValLen, &m_pColumnDataArray[*plFillIndex]);
            if (LOA_ERR_OK == rc)
            {
                plBufLen += lValLen;
                lKeyLen  += lValLen;
                if (LOADER_COLLENTYPE_FIX != m_pColumnDataArray[*plFillIndex].SQLColumnInfo.LengthType)
                {
                    //*
                    //*     Variable long key value --> the LAST KEY VALUE OF A SINGLE RECORD
                    //*
                    ++plFillIndex;
                    break;      // It's the last key value. So leave here.
                }
            }   // end if (rc == rc)
            else
                break;
        } // end for (j = 0; ( (errOK_els00 == rc) && (j < pTI->tdFieldCount_ls00) ); ++j)
        
        
        if (LOA_ERR_OK == rc)
        {
            //*
            //*     CHECK KEY LENGTH
            //*
            if (lKeyLen >= PERM_KEY_MXSP00)
            {
                rc = LOA_ERR_KEY_LENGTH;
            }
        
            //*
            //*     CHECK KEY ORDER
            //*
            //*  Write key into key-member of fastload structure.
            //*
            tsp00_LcompResult   CompVal;
            CompVal.becomes(l_undef);

            s30cmp(&m_pDataBuffer[HEADER_OFFSET], 1, lKeyLen, m_LastKey, 1, m_LastKeyLength, CompVal);
            if (l_greater == CompVal)
            {
                // key value starts BEHIND record header (BUF_START_POS_LS23 + HEADER_OFFSET_LS23)
                SAPDB_memcpy(m_LastKey, &m_pDataBuffer[HEADER_OFFSET], lKeyLen);
                m_LastKeyLength = lKeyLen;
            }
            else    // new key is less or result is undefined
            {
                rc = LOA_ERR_KEY_ORDER;
            }
        }   // end if (errOK_els00 == rc)

    } // end if (true == pFLRec->flrIsSyskey_ls00)

    return rc;
}
// buildKey()


inline LOADER_Int4
Loader_PageBuilder::LOADER_Strnlen(LOADER_Char* p, LOADER_Int4 sz)
{
    register LOADER_Char *pf = (LOADER_Char *) memchr(p, 0, sz);
    if(pf) {
        return pf - p;
    } else {
        return sz;
    }
}

/**
 * Length in bytes for UCS2.
 */
inline LOADER_Int4
Loader_PageBuilder::LOADER_StrnlenUCS2(LOADER_Char* p, LOADER_Int4 sz)
{
    register LOADER_Char *pp = p;
    LOADER_Int4 all = sz;
    while(sz >= 2) {
        if(*p == 0) {
            ++p;
            if(*p == 0) {
                return p - pp - 1;
            }
             ++p;
        } else {
            p += 2;
        }
        sz -= 2;
    }
    return all;
}