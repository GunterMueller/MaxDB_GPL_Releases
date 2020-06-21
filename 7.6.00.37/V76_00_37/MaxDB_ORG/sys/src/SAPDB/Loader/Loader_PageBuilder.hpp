/*!
  \file    Loader_PageBuilder.hpp
  \author  SteffenS
  \brief   builds pages

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

#ifndef LOADER_PAGEBUILDER_HPP
#define LOADER_PAGEBUILDER_HPP

#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_Common.hpp"
#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_Loader.hpp"
#include "Loader/Loader_StringConversion.hpp"
#include "gbd00.h"      // database bd layer (data page layout)




class Loader_PageBuilder
{
public:

    /*!
       @brief constructor
     */
    Loader_PageBuilder(Loader_TableDescription* pTableDescription, LOADER_Bool bUnicodeDB, LOADER_SQLMode SqlMode,
                       Loader_FastloadCommand* pFLCmdObj);

    ~Loader_PageBuilder();


    // Das sind feste werte, die nur einmal gesetzt werden muessen nachdem das Objekt konstruiert wurde
    void setColumnInformation(LOADER_Int2 lColumnCount,  LOADER_SwapKind KernelSwap, LOADER_SQLColumnInfo* pColumnInfo);

    inline void setLastInsertedKey(const LOADER_Char* pszLastKey, LOADER_Int4 lKeyLength)
    {
        if (lKeyLength <= 1024)
        {
            memcpy(m_LastKey, pszLastKey, lKeyLength);
            m_LastKeyLength = lKeyLength;
        }
        else
        {
            memcpy(m_LastKey, pszLastKey, 1024);
            m_LastKeyLength = 1024;
        }
    }


    inline void
    setRecordLength(LOADER_UInt4 lLength)
    {
        m_RecordLength = lLength;
        m_MinSpaceRequiredForSingleRecord = m_RecordLength + POINTERSIZE_BD00;
        if (0 != (m_MinSpaceRequiredForSingleRecord%2))
            m_MinSpaceRequiredForSingleRecord += 1;
    }

    inline void
    setSyskeyInformation(LOADER_Bool bTableHasSyskey)
    {
        m_TableHasSyskey = bTableHasSyskey;
    }

    inline void
    setPagesInformation(LOADER_UInt4 lPageSize, LOADER_UInt4 lPagesPerPacket)
    {
        m_PageSize       = lPageSize;
        m_PagesPerBuffer = lPagesPerPacket;
    }

    void setKernelDateTimeFormat(LOADER_DateTimeFormat DateTimeFormat);

    inline void
    setSQLMode(LOADER_SQLMode SqlMode)
    {
        m_SQLMode = SqlMode;
    }


    void setDefaults(const LOADER_Char* pszUserDefault, const LOADER_Char* pszUserGroupDefault,
                     const LOADER_Char* pszStampDefault, const LOADER_Char* pszSysdbaDefault);


    // Werte, die beim Abschluss eines Fastload-Kommandos gebraucht werden
//    void getLastInsertedKey(const LOADER_Char* pszLastKey, LOADER_UInt4& lKeyLength) const;


    // Dann gibt es Werte, die muessen immer wieder neu gesetzt werden, z.B. wenn ein Puffer gewechselt wird

    // Bei Aufruf dieser Funktion werden alle internen Zaehler, die die parameter betreffen, zurueckgesetzt
    inline void setPageBuffer(LOADER_Byte* pPageBuffer)
    {
        m_pGlobalDataBuffer = pPageBuffer;
    }


    void setDBEncoding(LOADER_Bool bUnicode);


    inline void initializeExecutionData()
    {
        m_Rowidx = 0;
    }


    // statistics
    inline LOADER_UInt4
    getPageCount()
    {
        return this->m_PageCount;
    }

    inline LOADER_UInt8
    getRecordCount()
    {
        return this->m_RowCount;
    }


public:             // public status members
    Loader_FLPacketStatus          m_PacketStatus;

    LOADER_ErrorCode    AddData(LOADER_Parameter* ptrToData);


private:            // functions
    void                initializePageMetaData();
    LOADER_Int4         buildRecord();
    inline LOADER_ErrorCode    getNILConstants(Loader_ColumnData* pColumnData);

    LOADER_ErrorCode    buildDefault(LOADER_Byte*        pszDestination,
                                     LOADER_Int4&        lDestLen,
                                     Loader_ColumnData*  pFieldData,
                                     LOADER_Char&        bDefaultIsUnicode);

    LOADER_ErrorCode    convertToVDNNumber(Loader_ColumnData&  pData);

    LOADER_ErrorCode    addValueToPageBuffer(LOADER_Byte*        pszDestination,
                                             LOADER_Int4&        lDestLen,
                                             Loader_ColumnData*  pFieldData);

    inline void finalizePage()
    {
        m_pDataPage->nd_record_cnt() = Loader_Swap4BInt(m_Page_nRecCount, LOADER_SWAP_TYPE, m_KernelSwap);
        m_pDataPage->nd_bottom()     = Loader_Swap4BInt(m_Page_Bottom + 1, LOADER_SWAP_TYPE, m_KernelSwap);
        m_pDataPage->nd_sorted()     = LOADER_TRUE;

        m_Page_PageNo  += 1;
        m_PageCount    += 1;
        m_RowCount     += m_Page_nRecCount;
    }
    
    LOADER_ErrorCode    convert2InternalEncoding(Loader_ColumnData* pColumnData);

    inline LOADER_ErrorCode
    checkValueLength(const LOADER_Int4 lValueLength, LOADER_SQLColumnBasic & ColumnInfo)
    {
        return ((lValueLength > (ColumnInfo.SQLInOutLength - 1)) ?
                LOA_ERR_INCOMPATIBLE_CONST_LENGTH : LOA_ERR_OK);
    }

    LOADER_ErrorCode    convertValues2UnicodeOnPage(LOADER_Byte*        pszDestination,
                                                    LOADER_Int4&        lDestLen,
                                                    Loader_ColumnData*  pFieldData);

    void                buildErrorString(LOADER_ErrorCode rc, const LOADER_Int4 lKeyLength);

    LOADER_ErrorCode    checkForHexValues(LOADER_Int2 i);

    LOADER_ErrorCode    checkForSpecialConstants(LOADER_Int2 i);

    LOADER_ErrorCode    DoingForEmptyData(LOADER_Int2 i);

    LOADER_ErrorCode    buildKey(LOADER_UInt4*& plFillIndex, LOADER_Int4& plBufLen, LOADER_Int4& lKeyLen);

    inline LOADER_Int4  LOADER_Strnlen(LOADER_Char* p, LOADER_Int4 sz);

    inline LOADER_Int4  LOADER_StrnlenUCS2(LOADER_Char* p, LOADER_Int4 sz);



private:            // member variables
    friend class Loader_FastloadCommand;



    // THE CREATOR
    Loader_FastloadCommand*     m_pTheCreator;

    LOADER_Int2                 m_ParameterCount;
    LOADER_Int2                 m_ColumnCount;          // Includes SYSKEY column, too.
    LOADER_SwapKind             m_KernelSwap;

    Loader_ColumnData*          m_pColumnDataArray;

    LOADER_Byte*                m_pGlobalDataBuffer;

    LOADER_UInt4                m_PageSize;
    LOADER_UInt4                m_PagesPerBuffer;           // number of possible pages per packet
    LOADER_UInt4                m_RecordLength;             // length of one record on a page in a table
    LOADER_UInt4                m_MinSpaceRequiredForSingleRecord;      // this is the space a single record needs on a page including the space for pageptr
                                                                        // and 2 Byte alignment
    LOADER_UInt4                m_FixColumnsLength;         // length of all fix columns together
    LOADER_Byte                 m_szFixColumnsLength[2];
    LOADER_Byte                 m_szVarColumnsCount[2];     // The only point where we need is is when to write the column count into the record as 2 Byte char

    LOADER_Bool                 m_TableHasSyskey;

    LOADER_Byte                 m_LastKey[1024];            // not null terminated string
    LOADER_Int4                 m_LastKeyLength;


    LOADER_UInt4*               m_plFillIndex;
    LOADER_UInt4*               m_plStmtNoIndex;            // indexing the columns in the same order as they were specified by Loader_FastloadCommand::initialize

    LOADER_SQLMode              m_SQLMode;
    LOADER_DateTimeFormat       m_KernelDateTimeFormat;


    // Parameter data
    tsp77encoding const *       m_pDBEncoding;


    LOADER_UInt4                m_Distance;                 // internal variable to compute the distance between values for row wise binding


    // Interne Variablen zur Bearbeitung der Parameterdaten
    LOADER_UInt2                m_PageCount;        // overall number of pages sent to kernel
    LOADER_UInt8                m_RowCount;         // overall number of rows processed


    tbd_node*                   m_pDataPage;        // seitenstruktur; wird uber puffer gelegt
    LOADER_Byte*                m_pDataBuffer;      // ist paket als pagepuffer

    LOADER_Byte*                m_pSpecialPagePtr;

    // Value valid until array size records are processed
    LOADER_UInt4                m_Rowidx;  // row index

    // Metadata necessary to fill a DB page
    LOADER_Int4                 m_Page_ListTop;
    LOADER_UInt4                m_Page_Bottom;
    LOADER_UInt4                m_Page_Space;
    LOADER_UInt4                m_Page_PageNo;
    LOADER_Int4                 m_Page_nRecCount;
    LOADER_Int4                 m_Page_RecordLength;

    LOADER_Int4                 m_Page_Status;      // 0 = initial; better: there is no page initialize -> start a new
                                                    // 1 = a page is in process of filling with data
                                                    // 2 = full --> gleiches wie initial nur dass pageno 1 weiter gezaehlt?

    // Error handling: this class delivers the error code and the column number for which the error happened if
    //                 at all it happened for a special column; if not the error code must be enough; it does not
    //                 construct error messages, this must be done by the caller

    LOADER_Int4                 m_lErrDataIndex;
    LOADER_Int4                 m_lErrColNameIndex;

    LOADER_Bool                 m_DataTruncated;

    LOADER_DateTimeMask         m_DateTimeMask;
    LOADER_Int4                 m_SpecialDateTimeError;

//time    double duration;
};

//*
//*     All following prototypes (i41...) are for compatibility reasons with the respective
//*     pascal source vin41 where no prototypes exist.
//*
externPascal void i41dparse(tin_date_mask&  DateStruct,
                            LOADER_Int4&    lError,
                            SAPDB_UInt2&    lErrorPos);

externPascal void i41tparse(tin_date_mask&  TimeStruct,
                            LOADER_Int4&    lError,
                            SAPDB_UInt2&    lErrorPos);

externPascal void i41tsparse(tin_date_mask& TimeStampStruct,
                             LOADER_Int4&   lError,
                             SAPDB_UInt2&   lErrorPos);

#endif //LOADER_PAGEBUILDER_HPP