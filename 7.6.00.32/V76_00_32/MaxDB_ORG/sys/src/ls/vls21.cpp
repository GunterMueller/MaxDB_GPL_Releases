/* 
  -----------------------------------------------------------------------------
  module: vls21.cpp
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Loader
 
  description:  FASTLOAD execution
                
  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

*/
#include <assert.h>

#include "gls00.h"
#include "hls03.h"
#include "hls04.h"
#include "hls041.h"
#include "hls05.h"
#include "hls07.h"
#include "hls13.h"
#include "hls15.h"
#include "hls16_condition.h"
#include "hls18.h"
#include "hls21.h"
#include "hls22.h"          // fastload-laden der seiten
#include "hls30.h"
#include "hls99.h"          // trace-ausgaben
#include "hls98msg.h"       // fehlermeldungen/logdatei

#include "hsp30.h"
#include "gbd00.h"      // bd-schicht der datenbank (datenseiten layout)
#include "ggg00.h"      // bd-schicht der datenbank (datenseiten layout)

#define _INTERNAL_FILE_ID_    _T("vls21.cpp")

/* Lokale Funktionen: ******************************************************************/

/*
  -----------------------------------------------------------------------------

  function:     ls21_InitFastLoadRec

  -----------------------------------------------------------------------------

  description:  Creates and initializes fast load record structure.

  arguments:    pFLRec   [IN]    - fast load record structure
                pDBInfo  [IN]    - db session structure

  returnvalue:  errOK_els00           - in case of NO errors
                errNoMoreMemory_els98 - not enough memory to create structure
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls21_InitFastLoadRec(tls00_FloadRec *&pFLRec,
                    tls00_DBInfo   *pDBInfo);


/*
  -----------------------------------------------------------------------------

  function:     ls21_FreeFastLoadRec

  -----------------------------------------------------------------------------

  description:  Destroys fast load record structure.

  arguments:    pFLRec   [IN]    - fast load record structure

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls21_FreeFastLoadRec(tls00_FloadRec *&         FloadRec,
                     tls00_TransformationCmd*  pTransformationCmd);


/*
  -----------------------------------------------------------------------------
  function:     ls21_ExtractFieldInfos
  -----------------------------------------------------------------------------
  description:  Extracts the field information for a table out of a delivered
                receive packet.

                PREREQUISITE is a packet size of 32K. For smaller sizes the 
                behavior is undefined, e.g. only a part of the information (from
                the 1st part) is extracted.

  arguments:    SqlPacket  [IN]    - receive packet
                TableInfo  [OUT]   - that will be filled with info on every column
                Errtext    [OUT]   - error text

  returnvalue:  errOK_els00         - no error
                errUnknownPart_ls98 - part of part kind data not found
  -----------------------------------------------------------------------------
*/
static int
ls21_ExtractFieldInfos(tsp1_packet              *SqlPacket,
                       tls00_TableDescription & TableInfo,
                       tsp00_Addr               ErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls21_GetFieldInfo
  -----------------------------------------------------------------------------
  description:  Gets all information about the table to load (fastload). Issues the
                special command 'LOAD TABLE <tablename> (<colname1>, ..., <colnameN>).

                In case there are column name specified in the column structure
                those column names will be used here to get the information. In
                case there were no column names specified at all this function first
                retrieves the column names and then gets the needed information. It
                refreshes the value Columns->mcsCount_ls00, too.        

  arguments:    DBInfo      [IN]    - structure with all info on the running
                                       session and the database connected to
                TableName   [IN]    - fully qualified table name
                pColumns    [OUT]   - structure that will be filled with column
                                       names and some other info on the columns.
                TableInfo   [OUT]   - structure that will be filled with additional
                                       information on every column
                errtext     [OUT]   - error text

  returnvalue:  errOK_els00         - in case of NO errors
                some errors
                
                Error messages will be logged by this function into the log file.
  -----------------------------------------------------------------------------
*/
static int
ls21_GetFieldInfo(MetaDataDef*            pMetaDataDef,
                  SAPDB_Char*             pszSchemaName,
                  SAPDB_Char*             pszTableName,
                  tls00_MultColSpec      *pColumns,
                  tls00_TableDescription &TableInfo,
                  tsp00_Addr              ErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls21_GetDefaultValue - local function
  -----------------------------------------------------------------------------
  description:  Get all default values for all column names delivered by the caller.

                ATTENTION:
                Allocates memory for default-value-member of FloadRec which
                MUST be freed in the calling function.
                Fuer das array wird keine groesse mitgegeben. Dafuer muss das ende
                des array mit einer -1 gekennzeichnet sein. Das bedeutet, dass das
                array genau einen wert groesser sein muss, als defaultwerte bestimmt
                werden sollen.

  arguments:    DBInfo          [IN]    - db session structure
                FloadRec        [IN]    - fastload record structure
                pszTableName    [IN]    - fully qualified table name.
                ColNum[]        [IN]    - Array of column numbers to get defaults for
                                           end of array must be marked with an extra
                                           value of -1
                TableInfo       [OUT]   - Table description strucuter
                Errtext         [OUT]   - error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors
                
                Error messages will be logged into return string and log file.
  -----------------------------------------------------------------------------
*/
static int
ls21_GetDefaultValue(tls00_DBInfo   *pDBInfo,
                     tls00_FloadRec &FloadRec,
                     tsp00_Addr      pszTableName,
                     tsp00_Int4      ColNum[],
                     tsp00_Int2      lDefaultCount,
                     tsp00_Addr      ErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls21_InitFastLoadRec
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls21_InitFastLoadRec(tls00_FloadRec *&pFLRec,
                    tls00_DBInfo   *pDBInfo)
{
    ROUTINE_DBG_MEO00 ("ls21_InitFastLoadRec");

    tsp00_Int4 rc = errOK_els00;

    pFLRec = new tls00_FloadRec;
    if (NULL == pFLRec)
    {
        rc = errNoMoreMemory_els98;
    }
    else
    {
        memset(pFLRec, 0, sizeof(tls00_FloadRec));      // set all to 0

		// table empty|full
		pFLRec->flrTabEmpty_ls00      = false;  
		pFLRec->flrTabLoaded_ls00     = false;
		pFLRec->flrTableSwitched_ls00 = true;

        memset(pFLRec->flrDefStruct_ls00.pdOracleUid_ls00, BLANK_LS00, sizeof(tsp00_Number));

        // Initialize position values with database constants.
        pFLRec->flrPageInfo_ls00.pgiDataStartPos = sizeof(tbd_node_header);
        pFLRec->flrPageInfo_ls00.pgiDataEndPos   = pDBInfo->dbiPageSize_ls00 - sizeof(tgg00_PageCheckTrailer);

        // Initializing number of rejected lines to maximum. During execution this value
        // will be decremented by 1 for every error.
        // For correct output of number of rejected lines we need to subtract
        // siMaxAdmissibleErrors - flrLRejected_ls00.
        pFLRec->flrLRejected_ls00 = pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors;

        pFLRec->flrSwapKind_ls00 = pDBInfo->dbiOwnSwap_ls00;   //  Swap behavior

        pFLRec->flrpInfile_ls00 = new tls00_VFile;
        memset(pFLRec->flrpInfile_ls00, 0, sizeof(tls00_VFile));
    }
    return rc;
}
//  ls21_InitFastLoadRec()

/*
  -----------------------------------------------------------------------------
  function:     ls21_FreeFastLoadRec
  -----------------------------------------------------------------------------
*/
static void
ls21_FreeFastLoadRec(tls00_FloadRec*&          FloadRec,
                     tls00_TransformationCmd*  pTransformationCmd)
{
    ROUTINE_DBG_MEO00 ("ls21_FreeFastLoadRec");

    tls00_TableDescription  &T = FloadRec->flrTableInfo_ls00;
    int i = 0;

    // We have to delete only as much values as we have allocated.
    for (i=0; i < T.tdFieldCount_ls00; ++i)
    {
        if (NULL != FloadRec->flrDefValue_ls00[i])
        {
            if (NULL != FloadRec->flrDefValue_ls00[i]->strAddr_ls00)
            {
                delete [] FloadRec->flrDefValue_ls00[i]->strAddr_ls00;
                FloadRec->flrDefValue_ls00[i]->strAddr_ls00 = NULL;
            }
        }
    
        if (NULL != T.tdFieldsArray_ls00[i])
        {
            delete [] T.tdFieldsArray_ls00[i];
            T.tdFieldsArray_ls00[i] = NULL;
        }

        if (NULL != T.tdColName_ls00[i])
        {
            delete T.tdColName_ls00[i];
            T.tdColName_ls00[i] = NULL;
        }
    }

    // clean up data source structure
    if (NULL != FloadRec->flrDataSource_ls00.dasInputBuffer.pBufPtr_ls00)
    {
        delete [] STATIC_CAST(char*, FloadRec->flrDataSource_ls00.dasInputBuffer.pBufPtr_ls00);
        FloadRec->flrDataSource_ls00.dasInputBuffer.pBufPtr_ls00 = NULL;
    }

    // Delete converted data buffer
    ls18RecordDestructor(T.tdFieldCount_ls00, FloadRec->flrDataSource_ls00.dasConvertedData);

    if (NULL != FloadRec->flrDataSource_ls00.lFillIndex_ls00)
    {
        delete [] FloadRec->flrDataSource_ls00.lFillIndex_ls00;
        FloadRec->flrDataSource_ls00.lFillIndex_ls00 = NULL;
    }
    if (NULL != FloadRec->flrDataSource_ls00.plDataToParamIndex_ls00)
    {
        delete [] FloadRec->flrDataSource_ls00.plDataToParamIndex_ls00;
        FloadRec->flrDataSource_ls00.plDataToParamIndex_ls00 = NULL;
    }
    if (NULL != FloadRec->flrDataSource_ls00.plDataToColIndex_ls00)
    {
        delete [] FloadRec->flrDataSource_ls00.plDataToColIndex_ls00;
        FloadRec->flrDataSource_ls00.plDataToColIndex_ls00 = NULL;
    }
    if (NULL != FloadRec->flrDataSource_ls00.lReadPos_ls00)
    {
        delete [] FloadRec->flrDataSource_ls00.lReadPos_ls00;
        FloadRec->flrDataSource_ls00.lReadPos_ls00 = NULL;
    }

    ls18DestructPattern(&FloadRec->flrPattern_ls00);

    // Close file; return value is of no interest
    SAPDB_Char _errTxt[256];
    ls30VFClose(*FloadRec->flrpInfile_ls00, _errTxt);
    if (NULL != FloadRec->flrpInfile_ls00)
    {
        delete FloadRec->flrpInfile_ls00;
        FloadRec->flrpInfile_ls00 = NULL;
    }

    if (NULL != FloadRec->flrNullValueCounter_ls00.nvcColumnAllowsNull_ls00)
    {
        delete [] FloadRec->flrNullValueCounter_ls00.nvcColumnAllowsNull_ls00;
        FloadRec->flrNullValueCounter_ls00.nvcColumnAllowsNull_ls00 = NULL;
    }

    // Delete NULL value counter aux structures
    if (NULL != FloadRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00)
    {
        if (NULL != FloadRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00)
        {
            if (NULL != FloadRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00->conCompVal_ls00)
            {
                if (FloadRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00->conCompVal_ls00 !=
                    pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoNullSpec_ls00)
                {
                    delete [] FloadRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00->conCompVal_ls00;
                }
                FloadRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00->conCompVal_ls00 = NULL;
            }
            delete FloadRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00;
            FloadRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00 = NULL;
        }
        delete [] FloadRec->flrNullValueCounter_ls00.nvcColumnAllowsNull_ls00;
        FloadRec->flrNullValueCounter_ls00.nvcColumnAllowsNull_ls00 = NULL;
    }


    delete FloadRec;
    FloadRec = NULL;
}
//  ls21_FreeFastLoadRec()


/*
  -----------------------------------------------------------------------------
  function:     ls21_ExtractFieldInfos
  -----------------------------------------------------------------------------
*/
static int
ls21_ExtractFieldInfos(tsp1_packet              *SqlPacket,
                       tls00_TableDescription & TableInfo,
                       tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls21_ExtractFieldInfos");

    tsp00_Int4 rc = errOK_els00;
    //*
    //*     Read table description (in part of kind sp1pk_data)
    //*
    tsp1_part   *pPart = NULL;
    tsp00_Int4  Offset = 0;
    int         i      = 0;
    
    if ( false == ls04GetPart(SqlPacket, sp1pk_data, pPart) )
    {
        LoaderMessages::Instance()->ls98Msg(layFastload_els98, ErrText,
                                                                errUnknownPart_els98);
        rc = errUnknownPart_els98;
    }
    else
    {
        tsp00_KnlIdentifierc myTempID;
        tsp00_Int4           lLen     = sizeof(tsp00_KnlIdentifier);

        // Table-Authid
        ls04BufFromPartEx(pPart, Offset, lLen, myTempID.asCharp(), lLen);
        
        // make authid readable (ascii/utf8) if db is unicode
        if (true == ls04IsDBUnicode(SqlPacket))
        {
            sp77sprintfUnicode(sp77encodingAscii, TableInfo.tdAuthid_ls00.asCharp(), sizeof(tsp00_KnlIdentifierc),
                               "%=.*S",
                               sp77encodingUCS2, lLen/2, myTempID.asCharp());
        }
        else
        {
            SAPDB_memcpy(TableInfo.tdAuthid_ls00.asCharp(), myTempID.asCharp(), lLen);
            TableInfo.tdAuthid_ls00[lLen] = 0;
        }

        // Table name
        Offset += lLen;
        ls04BufFromPartEx(pPart, Offset, lLen, myTempID.asCharp(), lLen);
        
        // make table name readable (ascii/utf8) if db is unicode
        if (true == ls04IsDBUnicode(SqlPacket))
        {
            sp77sprintfUnicode(sp77encodingAscii, TableInfo.tdTablename_ls00.asCharp(), sizeof(tsp00_KnlIdentifierc),
                               "%=.*S",
                               sp77encodingUCS2, lLen/2, myTempID.asCharp());
        }
        else
        {
            SAPDB_memcpy(TableInfo.tdTablename_ls00.asCharp(), myTempID.asCharp(), lLen);
            TableInfo.tdTablename_ls00[lLen] = 0;
        }

        // Table-Head
        Offset += lLen;
        ls04BufFromPartEx(pPart, Offset, 8, REINTERPRET_CAST(char*, &(TableInfo.tdFieldCount_ls00)), 8);



        Offset += 8;                                    // Table-Fields
        for (i = 0; i < TableInfo.tdFieldCount_ls00; ++i)
        {
            TableInfo.tdFieldsArray_ls00[i] = new tls00_FieldInfo;
        
            ls04BufFromPartEx(pPart,
                              Offset,
                              sizeof(tls00_FieldInfo),
                              REINTERPRET_CAST(char*, &(TableInfo.tdFieldsArray_ls00[i])[0]),
                              sizeof(tls00_FieldInfo));

            Offset += sizeof (tls00_FieldInfo);
        }


        // After that we're on the right place to read the column names
        for (i = 0; i < TableInfo.tdFieldCount_ls00; ++i)
        {
            if (NULL == TableInfo.tdColName_ls00[i])
            {
                TableInfo.tdColName_ls00[i] = new tsp00_KnlIdentifierc;
            }
        }

        if (true == ls04IsDBUnicode(SqlPacket))
        {
            ls04GetColumnNamesRaw(pPart, Offset, TableInfo.tdColName_ls00, TableInfo.tdFieldCount_ls00, ctUCS2_els00);
        }
        else
        {
            ls04GetColumnNamesRaw(pPart, Offset, TableInfo.tdColName_ls00, TableInfo.tdFieldCount_ls00, ctAscii_els00);
        }
    }

    return rc;
}
// ls21_ExtractFieldInfos


/*
  -----------------------------------------------------------------------------
  function:     ls21_GetFieldInfo
  -----------------------------------------------------------------------------
*/
static int
ls21_GetFieldInfo(MetaDataDef*            pMetaDataDef,
                  SAPDB_Char*             pszSchemaName,
                  SAPDB_Char*             pszTableName,
                  tls00_MultColSpec*      pColumns,
                  tls00_TableDescription &TableInfo,
                  tsp00_Addr              ErrText)
{
    ROUTINE_DBG_MEO00 ("ls21_GetFieldInfo");

    tsp00_Int4 rc = errOK_els00;

    tls00_DBInfo* pDBInfo = pMetaDataDef->getConnection();
    
    //  No column names in statement -> get'em specially. By doing this we define the
    //  count of columns to fill, too. This value = Columns.mcsCount_ls00.
    if (0 == pColumns->mcsCount_ls00)
    {
        rc = ls13GetColumnNames(pMetaDataDef, pszSchemaName, pszTableName, &TableInfo, ErrText);
        if (errOK_els00 != rc)
        {
            return rc;
        }
    }
    else
    {
        TableInfo.tdFieldCount_ls00 = pColumns->mcsCount_ls00;
        for (int j = 0; j < TableInfo.tdFieldCount_ls00; ++j)
        {
            TableInfo.tdColName_ls00[j] = new tsp00_KnlIdentifierc;
            _tcscpy(TableInfo.tdColName_ls00[j]->asCharp(), pColumns->mcsColumn_ls00[j]->colName_ls00.asCharp());
        }
    }
        
    //*     Build the command to send
    //* Column names are always decorated with "
    char *pszCmd = new char[(2 + TableInfo.tdFieldCount_ls00) * (sizeof(tsp00_KnlIdentifier) + 4)];
        
    pszCmd[0] = '\0';
    sprintf(pszCmd, "LOAD \"%s\".\"%s\" (\"%s", pszSchemaName, pszTableName, TableInfo.tdColName_ls00[0]->asCharp());
    int i = 1;
    for (i; i < TableInfo.tdFieldCount_ls00; i++)                  // all other column names
    {
        strcat(pszCmd, "\",\"");
        strcat(pszCmd, TableInfo.tdColName_ls00[i]->asCharp());
    }
    strcat(pszCmd, "\")");                                     // last closing parenthesis
    
    tsp00_Int4  ErrorPos = 0;
    rc = ls13ExecIntSQLStmt(pDBInfo, 0, pszCmd, ErrText, ErrorPos);

    delete [] pszCmd;
    pszCmd = NULL;

    if (errOK_els00 == rc)
    {
        rc = ls21_ExtractFieldInfos(pDBInfo->dbiPktRcvList_ls00[0], TableInfo, ErrText);
        if (errOK_els00 == rc)
        {
            TRACE_TAB2_MLS99 ("ls21_GetFieldInfo", &TableInfo);
        }
    }

    return rc;
} 
// ls21_GetFieldInfo()


/*
  -----------------------------------------------------------------------------
  function:     ls21_GetDefaultValue
  -----------------------------------------------------------------------------
*/
static int
ls21_GetDefaultValue(tls00_DBInfo   *pDBInfo,
                     tls00_FloadRec &FloadRec,
                     tsp00_Addr      pszTableName,
                     tsp00_Int4      ColNum[],
                     tsp00_Int2      lDefaultCount,
                     tsp00_Addr      ErrText)
{
    ROUTINE_DBG_MEO00 ("ls21_GetDefaultValue");

    tsp00_Longint rc = errOK_els00;

    tls00_TableDescription *pTD     = &FloadRec.flrTableInfo_ls00;
    tls00_String           *pVal    = NULL;
    tsp00_Int2             lColNo   = 0;
    tsp00_Int2             lDefLen  = 0;
    tsp00_Addr             pDefault = NULL;

    Tools_DynamicUTF8String TableNameUTF8;
    rc = ls05ConvertToUTF8(TableNameUTF8,
                           REINTERPRET_CAST(unsigned char*, pszTableName), _tcslen(pszTableName),
                           ctAscii_els00, ErrText);

    int i = 0;
    for (i; i < lDefaultCount; ++i) 
    {
        // Define number of column to get the default for
        lColNo = pTD->tdFieldsArray_ls00[ColNum[i]]->fiDefColNo_ls00;

        rc = ls13GetDefaultValue(pDBInfo, TableNameUTF8, lColNo, pDefault, lDefLen, ErrText);
        if (errOK_els00 == rc)
        {
            FloadRec.flrDefValue_ls00[ColNum[i]] = new tls00_String;
            pVal = FloadRec.flrDefValue_ls00[ColNum[i]];

            pVal->strLeng_ls00 = lDefLen;
            pVal->strAddr_ls00 = new _TCHAR[lDefLen + 1];

            SAPDB_memcpy(pVal->strAddr_ls00, pDefault, lDefLen);
            pVal->strAddr_ls00[lDefLen] = DEF_BYTE_LS00; // Abschliessen mit char(0)

            TRACE_PRNF_MLS99 (("ls21_GetDefaultValue", "Table: %s, column no: %i, default: '%s'",
                                                       pszTableName,
                                                       ColNum[i],
                                                       pVal->strAddr_ls00));
        }
    }
    return STATIC_CAST(tsp00_Int4, rc);
}
//  ls21_GetDefaultValue()



/********************************************************************************/  
/* FUNCTION ls21_ExecBeginLoad  - lokale funktion                               */
/*==============================================================================*/
/* PURPOSE                                                                      */
/*  Beginnt den fastload-lauf mit einem speziellen kommando.                    */
/*  Ermittelt die folgenden werte fuer die zu ladende tabelle                   */
/*  - letzter akzeptierter schluesselwert                                       */
/*  - letzter akzeptierter stamp wert                                           */
/*  - bdinfo                                                                    */
/*  Setzt commit-flag auf false fuer diese ausfuehrung                          */
/*==============================================================================*/
/* PARAMETER        IN/OUT      DESCRIPTION                                     */
/*  *DBInfo         X           pointer to db session structure                 */
/*  *FloadRec       X           pointer to fastload structure                   */
/*  pszTableName    X           pointer, full qualified table name              */
/*  ErrText            X        status message                                  */
/*==============================================================================*/
/* RETURN CODE          DESCRIPTION                                             */
/*  errOK_els00          success in case                                        */
/*  fehler bei der zusammenstellung des pakets und bei der kommunikation mit    */
/*  der datenbank. Die fehlertexte werden im logfile eingetragen.               */
/********************************************************************************/
int ls21_ExecBeginLoad(tls00_DBInfo   *pDBInfo,
                       tls00_FloadRec *pFLRec,
                       tsp00_Addr      pszTableName,
                       tsp00_Addr      ErrText)
{
    ROUTINE_DBG_MEO00 ("ls21_ExecBeginLoad");

    LoaderMessages *pMsgObj = LoaderMessages::Instance(); // Für error reports
    tsp00_Int4       rc      = errOK_els00;
    
    //*
    //*     Build command
    //*
    _TCHAR szCmd[lStmtLength_ls21];
    szCmd[0] = 0;
    if (false == pFLRec->flrStampDef_ls00)
    {
        sp77sprintf(szCmd, lStmtLength_ls21, BEGINLOAD_CMD_LS21, pszTableName);
    }
    else
    { // Table has default stamp columns
        sp77sprintf(szCmd, lStmtLength_ls21, BEGINLOAD_W_STAMP_CMD_LS21, pszTableName);
    }

    tsp00_Int4 ErrPos = 0;
    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, ErrText, ErrPos, false, sp1m_dbs, false);
    if (errOK_els00 != rc)
    {
        pMsgObj->ls98PrintMessage(layFastload_els98, errSimpleSQL_els98, ErrText);
    }
    else
    {
        tsp1_packet *pSqlPacket = pDBInfo->dbiPktRcvList_ls00[0];        // Packet zuweisen
        tsp1_part   *pSqlPart   = NULL;

        //*
        //*     Part of part kind KEY
        //*
        //* The keys are always separated be the def_byte (blank for character and 0 for numeric
        //* values) because key columns are not allowed to contain NULL.
        //*
        if ( false == ls04GetPart(pSqlPacket, sp1pk_key, pSqlPart) )
        {
            TRACE_PRNF_MLS99 (("ls21_BeginLoad", "Unknown part sp1k_key"));
            rc = errUnknownPart_els98;
        }
        else
        {
            //*
            //*     Key - last key accepted on the table to process
            //* The data in part of part kind key is stored as follows where the key length
            //* is always given in big endian order. In case the table has a user defined key
            //* and is empty the part contains only the length of the key which is 0.
            //
            //      -------------------------------------------------
            //      | 2 byte key length | key, usually 9 bytes long |
            //      -------------------------------------------------
            //
            ls04BufFromPartEx(pSqlPart,
                              0,
                              sizeof(pFLRec->flrLenKeyAcp_ls00),
                              REINTERPRET_CAST(char*, &pFLRec->flrLenKeyAcp_ls00),
                              sizeof(pFLRec->flrLenKeyAcp_ls00));

            if (0 != pFLRec->flrLenKeyAcp_ls00)
            {
                if (sw_normal != pFLRec->flrSwapKind_ls00)
                {
                    pFLRec->flrLenKeyAcp_ls00 = ls07Swap2BInt(pFLRec->flrLenKeyAcp_ls00, sw_normal, pFLRec->flrSwapKind_ls00);
                }
                ls04BufFromPartEx(pSqlPart,
                                  2,
                                  pFLRec->flrLenKeyAcp_ls00,
                                  pFLRec->flrLstKeyAcp_ls00,
                                  sizeof(pFLRec->flrLstKeyAcp_ls00));
            }
            else
            {  // table has user defined key but is still empty
                pFLRec->flrTabEmpty_ls00 = true;
            }
        }   // end if ( false == ls04GetPart(pSqlPacket, sp1pk_key, pSqlPart) )


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
        if ( (errOK_els00 == rc) && (true == pFLRec->flrStampDef_ls00) )
        {
            if ( false == ls04GetPart(pSqlPacket, sp1pk_surrogate, pSqlPart) )
            {
                TRACE_PRNF_MLS99 (("ls21_BeginLoad", "Unknown part sp1pk_surrogate."));
                rc = errUnknownPart_els98;
            }
            else
            {
                // first get the length
                char lStampLen;
                ls04BufFromPartEx(pSqlPart, 0, sizeof(char), &lStampLen, sizeof(char));
                // then get the stamp
                ls04BufFromPartEx(pSqlPart, 1, lStampLen,
                                  pFLRec->flrDefStruct_ls00.pdStamp_ls00, sizeof(pFLRec->flrDefStruct_ls00.pdStamp_ls00));
            }
        }

        //*
        //*     Part of part kind BDINFO
        //*
        rc = ls04GetBdinfo(pSqlPacket, &pFLRec->flrBdInfo_ls00);
    }   // end if (errOK_els00 != rc)

    if (errUnknownPart_els98 == rc)
    {
        pMsgObj->ls98Msg(layFastload_els98, ErrText, errInternal_els98, _INTERNAL_FILE_ID_, __LINE__, errUnknownPart_els98);
        rc = errInternal_els98;
    }
                
    return rc;
}
// ls21_ExecBeginLoad()


/********************************************************************************/  
/* FUNCTION ls21_ExecEndLoad  - lokale funktion                                 */
/*==============================================================================*/
/* PURPOSE                                                                      */
/*  Beendet den fastload-lauf mit einem speziellen kommando.                    */
/*  Uebergibt an den kern die folgenden informationen:                          */
/*  - letztes bdinfo                                                            */
/*  - letzter verwendeter syskey- und stamp-wert                                */
/*  - anzahl geladener seiten und datensaetze                                   */
/*==============================================================================*/
/* PARAMETER        IN/OUT      DESCRIPTION                                     */
/*  *DBInfo         X           pointer to db session structure                 */
/*  *FloadRec       X           pointer to fastload structure                   */
/*  pszTableName    X           pointer, full qualified table name              */
/*  ErrText            X        status message                                  */
/*==============================================================================*/
/* RETURN CODE          DESCRIPTION                                             */
/*  errOK_els00          success in case                                        */
/*  fehler bei der zusammenstellung des pakets und bei der kommunikation mit    */
/*  der datenbank. Die fehlertexte werden im logfile eingetragen.               */
/********************************************************************************/
int
ls21_ExecEndLoad(tls00_DBInfo   *DBInfo,
                 tls00_FloadRec *pFLRec,
                 tsp00_Addr      pszTableName,
                 tsp00_Addr      ErrText)
{
    ROUTINE_DBG_MEO00 ("ls21_ExecEndLoad");

    tsp00_Int4 rc        = errOK_els00;
    tsp00_Addr pszBuffer = NULL;

    LoaderMessages *pMsgObj = LoaderMessages::Instance(); // Für error reports
    
    tls00_Parts4InternalSql S;
    
    S.pfiCount_ls00 = 4;
    S.pfiPartKind_ls00 = new tsp1_part_kind_Enum[S.pfiCount_ls00];
    S.pfiPartKind_ls00[0] = sp1pk_command;
    S.pfiPartKind_ls00[1] = sp1pk_bdinfo;
    S.pfiPartKind_ls00[2] = sp1pk_data;
    S.pfiPartKind_ls00[3] = sp1pk_surrogate;
    S.pfiData_ls00 = new tls00_String[S.pfiCount_ls00];


    //*     kommando part
    _TCHAR szCmd[lStmtLength_ls21];
    szCmd[0] = 0;
    sp77sprintf(szCmd, lStmtLength_ls21, ENDLOAD_CMD_LS21, pszTableName);
    S.pfiData_ls00[0].strAddr_ls00 = szCmd;
    S.pfiData_ls00[0].strLeng_ls00 = STATIC_CAST( tsp00_Int4, _tcslen(szCmd) );

    //*     bdinfo part
    S.pfiData_ls00[1].strAddr_ls00 = new _TCHAR[pFLRec->flrBdInfo_ls00.bdiLength_ls00];
    S.pfiData_ls00[1].strLeng_ls00 = pFLRec->flrBdInfo_ls00.bdiLength_ls00;
    SAPDB_memcpy(S.pfiData_ls00[1].strAddr_ls00, pFLRec->flrBdInfo_ls00.szbdiData_ls00, pFLRec->flrBdInfo_ls00.bdiLength_ls00);
    //*
    //*     data part
    //* Enthaelt die anzahl geladener pages und records.
    //*
    S.pfiData_ls00[2].strLeng_ls00 = mxsp_resnum + mxsp_resnum;
    S.pfiData_ls00[2].strAddr_ls00 = new _TCHAR[S.pfiData_ls00[2].strLeng_ls00];
    pszBuffer = S.pfiData_ls00[2].strAddr_ls00;
    if (true == pFLRec->flrTabEmpty_ls00)
    {
        pszBuffer[0]           = '1';
        pszBuffer[mxsp_resnum] = '1';
    }
    else
    {
        pszBuffer[0]           = '\x00';
        pszBuffer[mxsp_resnum] = '\x00';
    }

    ls07IntToNumber(pFLRec->flrPageCount_ls00, &pszBuffer[1]);

    // The number of records is submitted to kernel only for one reason:
    // the optimizer can use the number for updating statistics; thus the user is not
    // necessarily forced to run 'UPDATE STATISTICS' after successful upload of
    // data; BUT the optimizer can only process values up to MAX_INT4_SP00 - that means
    // we have to limit the record count here

    tsp00_Int4 reccount = MAX_INT4_SP00;
    if (pFLRec->flrRecordCount_ls00 < MAX_INT4_SP00)
    {
        reccount = STATIC_CAST(tsp00_Int4, pFLRec->flrRecordCount_ls00);
    }
    ls07IntToNumber(reccount, &pszBuffer[mxsp_resnum+1]);

    //*
    //*      surrogate part
    //* Contains either last inserted syskey or stamp value. In case the table has syskey
    //* column as well as stamp column(s) the stamp is sent.
    //*
    if ( (true == pFLRec->flrIsSyskey_ls00) || (true == pFLRec->flrStampDef_ls00) )
    {
        // Length of buffer to admit is length of syskey/stamp (= 9 bytes including def byte)
        S.pfiData_ls00[3].strAddr_ls00 = new _TCHAR[sizeof(pFLRec->flrDefStruct_ls00.pdStamp_ls00)];
        pszBuffer = S.pfiData_ls00[3].strAddr_ls00;
        if (true == pFLRec->flrStampDef_ls00)
        {
            S.pfiData_ls00[3].strLeng_ls00 = sizeof(pFLRec->flrDefStruct_ls00.pdStamp_ls00);
            SAPDB_memcpy(pszBuffer, pFLRec->flrDefStruct_ls00.pdStamp_ls00.asCharp(), S.pfiData_ls00[3].strLeng_ls00);
        }
        else
        {
            S.pfiData_ls00[3].strLeng_ls00 = pFLRec->flrLenKeyAcp_ls00;
            SAPDB_memcpy(pszBuffer, pFLRec->flrLstKeyAcp_ls00, S.pfiData_ls00[3].strLeng_ls00);
        }
    }
    else
    {
        S.pfiCount_ls00 = 3;
        S.pfiData_ls00[3].strAddr_ls00 = NULL;
    }


    tsp00_Int4 lErrorPos = 0;
    _TCHAR szErr[MAX_REPLY_LENGTH_LS00];
    szErr[0] = 0;
    rc = ls13ExecIntSQLMultParts(DBInfo, 0, S, szErr, lErrorPos);
    if (errOK_els00 != rc)
    {
        pMsgObj->ls98Msg(layFastload_els98, ErrText, errInternSQL_els98,
                                                     S.pfiData_ls00[0].strAddr_ls00,
                                                     szErr);
        rc = errInternSQL_els98;
    }

    int i = 1;      // fuer das endload-kommando wurde kein speicher auf dem heap angelegt!
    for (i; i < S.pfiCount_ls00; ++i)
    {
        delete [] S.pfiData_ls00[i].strAddr_ls00;
        S.pfiData_ls00[i].strAddr_ls00 = NULL;
    }
    delete [] S.pfiPartKind_ls00;
    delete [] S.pfiData_ls00;

    TRACE_PRNF_MLS99 (("ls21_ExecEndLoad", "Pages loaded\t\t: %d", pFLRec->flrPageCount_ls00));

#if COMPILEMODE_MSP00 >= SLOW_MSP00
    char crc[32];
    crc[0] = 0;
    sp77sprintf(crc, 32, "%qd", pFLRec->flrRecordCount_ls00);
    TRACE_PRNF_MLS99 (("ls21_ExecEndLoad", "Records loaded\t: %s", crc));
#endif

    return rc;
}
//  ls21_ExecEndLoad()



/* Globale Funktionen: **********************************************************/


/*
  -----------------------------------------------------------------------------
  function:     ls21ImportTableRowPages 
  -----------------------------------------------------------------------------
  description:  Processes a fastload command.

  arguments:    pTransformationCmd     [IN]  - fastload command structure with user info
                pDBInfo    [IN]  - structure with info on db and running session
                ErrText [OUT] - error text

  returnvalue:  errOK_els00 - no error
                errors from processing 
  -----------------------------------------------------------------------------
*/
int
ls21ImportTableRowPages(MetaDataDef *pMetaDataDef, tls00_DBInfo *pDBInfo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls21ImportTableRowPages");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // Für error reports
    tsp00_Int4       rc     = errOK_els00;

    TransformationDef       *pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();

    Tools_DynamicUTF8String  tableschema,sessionschema;
    sessionschema = REINTERPRET_CAST(SAPDB_UTF8*, pDBInfo->dbiSession_ls00.siSchemaName_ls00);
    tableschema   = REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp());
    if ( 0 != _tcscmp(pDBInfo->dbiSession_ls00.siSchemaName_ls00, pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp()) )
         rc = ls15SwitchSchema(pDBInfo, tableschema, ErrText);
    if (errOK_els00 != rc)
    {
        return rc;
    }
    //*
    //*     Initialize fastload-record structure
    //*
    tls00_FloadRec *pFLRec = NULL;
    rc = ls21_InitFastLoadRec(pFLRec, pDBInfo);
    if (errOK_els00 != rc)
    {
        return rc;
    }

    tls00_MultColSpec      *pMultCol  = &pTransformationCmd->trColSpec_ls00;
    tls00_TableDescription &TableInfo = pFLRec->flrTableInfo_ls00;
    tls00_TableSpec        &TableSpec = pTransformationCmd->trTable_ls00;
    tls00_FieldInfo        *pFI       = NULL;
    tls00_Column           *pCol      = NULL;

    int i = 0;
    int j = 0;
    int k = 0;

    // Initialize and open data file
    if (false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00)
    {
        ls30VFInitialize(*pFLRec->flrpInfile_ls00,
                         pTransformationCmd->trDataFile_ls00.fsFileName_ls00,
                         sp5vf_text,
                         sp5vf_read,
                         pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00);
    }
    else
    {
        ls30VFInitialize(*pFLRec->flrpInfile_ls00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00);
    }

    // Open file and set encoding type if necessary
    rc = ls30VFOpen(*pFLRec->flrpInfile_ls00, ErrText);
    if (errOK_els00 != rc)
    {
        ls21_FreeFastLoadRec(pFLRec, pTransformationCmd);
        return rc;
    }
    else
    {
        if (ctUnknown_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
        {
            pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = pFLRec->flrpInfile_ls00->vfFileEncoding_ls00;
        }
    }

    //*
    //*     Check file encoding and database encoding.
    //*     Not supported is loading UNICODE data into ASCII databases.
    //*
    rc = STATIC_CAST(tsp00_Int4, ls05AreEncodingsCompatible(pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00,
                                                            ls00IsDBUnicode(pDBInfo) ? ctUCS2_els00 : ctAscii_els00));
    if (errOK_els00 != rc)
    {
        pMsgObj->ls98Msg(layFastload_els98, ErrText, errIncompatibleEncodings_els98);
        ls21_FreeFastLoadRec(pFLRec, pTransformationCmd);
        return rc;
    }

    //*
    //*     Get detailed info on the table to process from kernel
    //*
    if (errOK_els00 == rc)
    {
        char szErr[MAX_REPLY_LENGTH_LS00];
        szErr[0] = 0;
        rc = ls21_GetFieldInfo(pMetaDataDef, TableSpec.tsTabSchema_ls00, TableSpec.tsTabName_ls00, pMultCol, TableInfo, szErr);
        if (errOK_els00 != rc)
        {
            pMsgObj->ls98Msg(layFastload_els98, ErrText, errSimpleSQL_els98, szErr);

            ls21_FreeFastLoadRec(pFLRec, pTransformationCmd);

            // Rollback because we have run the locking FASTLOAD command already; return value is of no interest
            SAPDB_Char _errTxt[256];
            ls15Rollback(pDBInfo, _errTxt);

            return rc;
        }
    }

    //*
    //*     Pruefen auf
    //* - user definierte defaults
    //* - default STAMP spalten
    //* - LONG spalten (-> in diesem fall wird abgelehnt, die tabelle zu laden!!!)
    //*

    // Der array lDefCount wird im folgenden mit den indizes der tabellenspalten
    // gefuellt, fuer die der benutzer einen default bei definition der tabelle festgelegt hat.
    tsp00_Int4  lDefaultFieldNo[csp_max_fields];

    for (i=0, j=0; i < TableInfo.tdFieldCount_ls00; ++i)
    {
        pFI = TableInfo.tdFieldsArray_ls00[i];

        if (sp7_other_default == pFI->fiDefault_ls00)
        {
            lDefaultFieldNo[j] = i;
            ++j;
        }
        else if (sp7_default_stamp == pFI->fiDefault_ls00)
        {
            pFLRec->flrStampDef_ls00 = true;
        }

        //*     LONG spalte (LONG ASCII; LONG EBCDIC; LONG BYTE; LONG RAW; LONG UNICODE) ?
        //* Stop working. We do not process long values so far
        if (1 == szMap_LongDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00])
        {
            ls21_FreeFastLoadRec(pFLRec, pTransformationCmd);

            pMsgObj->ls98Msg(layFastload_els98, ErrText, errLongColIllegal_els98);
            
            // Rollback because we have run the locking FASTLOAD command already; return value is of no interest
            SAPDB_Char _errTxt[256];
            ls15Rollback(pDBInfo, _errTxt);
            
            return errLongColIllegal_els98;
        }
    }
    //*     Gabs benutzerdefinierte defaults ? Dann hole diese.
    if (j > 0)
    {
        rc = ls21_GetDefaultValue (pDBInfo, *pFLRec, TableSpec.tsTabName_ls00, lDefaultFieldNo, j, ErrText);
        if (errOK_els00 != rc)
        {
            ls21_FreeFastLoadRec(pFLRec, pTransformationCmd);
            
            // Rollback because we have run the locking FASTLOAD command already; return value is of no interest
            SAPDB_Char _errTxt[256];
            ls15Rollback(pDBInfo, _errTxt);
            
            return rc;
        }
    }

    //*
    //*     Record laenge bestimmen
    //*
    pFLRec->flrRecordLen_ls00 = ls18GetRecordLength(TableInfo, false);
    
    //*
    //*     Count of var long columns and length of fix long columns
    for (i=0; i < TableInfo.tdFieldCount_ls00; ++i)
    {
        pFI = TableInfo.tdFieldsArray_ls00[i];

        switch (pFI->fiLengthType_ls00)
        {
            case sp7_fix_len:
            {   // internal (db) length of fix long columns. Alle benutzerdefinierten schluesselspalten
                // bis auf die letzte sind ebenfalls fix lang und duerfen hier nicht mit
                // eingehen.
                if (mod_key != pFI->fiColMode_ls00)
                {
                    pFLRec->flrFixColLen_ls00 += pFI->fiBasics_ls00.fbInOutLength_ls00;
                }
                break;
            }
            case  sp7_var_len:
            {
                //      anzahl var langer spalten
                //  syskey bzw. die letzte spalte eines schluessels ist ebenfalls
                //  variabel lang ===> ueberspringe diese spalte !!!
                if ( (mod_key != pFI->fiColMode_ls00) && (mod_syskey != pFI->fiColMode_ls00) )
                {
                    pFLRec->flrVarColCnt_ls00 += 1;
                }
                break;
            }
            default:    // steht fuer sp7_var_len_long: keine aktionen hier
            {
                break;
            }
        }
    }


    //*     Assign some user related special default values.
    pFLRec->flrDefStruct_ls00.pdUserDefault       = pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp();
    pFLRec->flrDefStruct_ls00.pdUserGroupDefault  = pDBInfo->dbiSession_ls00.siUserGroup_ls00.asCharp();

    //*
    //*     Check for syskey! Syskey is in any case the first column delivered by the kernel
    //*     in the previous function.
    //*
    if (0 == TableInfo.tdFieldsArray_ls00[0]->fiColMode_ls00)
    {
        pFLRec->flrIsSyskey_ls00 = true;
        TRACE_PRNF_MLS99 (("ls21ImportTableRowPages", "Table with syskey."));
    }

    //*
    //*     NULL value representation and fraction value
    //* - Convert NULL value representation if necessary;
    //* - Check for needs of transformation of NULL value representation for DEFAULT NULL
    //*   specification (mainly in case data file encoding is UCS2 in any form)
    //*
    //* Den fraction-wert anpassen. Wenn er auf UNDEFINED_LS00 steht, dann
    //* die fraction des db-systems uebernehmen.
    //*

    tsp00_Int2 Index = 0;
    for (i=0; i < TableInfo.tdFieldCount_ls00; ++i)
    {
        Index = TableInfo.tdFieldsArray_ls00[i]->fiStmtColNo_ls00;
        if ( (0 != Index) && (0 != pMultCol->mcsCount_ls00) )
        {
            pCol = pMultCol->mcsColumn_ls00[Index - 1];
            pFI  = TableInfo.tdFieldsArray_ls00[i];

            if (NULL != pCol->colNullCond_ls00)
            {
                rc = ls16ConvertCondition(*pCol->colNullCond_ls00,
                                          pDBInfo->dbiOwnSwap_ls00,
                                          pTransformationCmd->trDataFile_ls00.fsFormat_ls00,
                                          ErrText);
                if (errOK_els00 != rc)
                {
                    ls21_FreeFastLoadRec(pFLRec, pTransformationCmd);

                    // Rollback because we have run the locking FASTLOAD command already; return value is of no interest
                    SAPDB_Char _errTxt[256];
                    ls15Rollback(pDBInfo, _errTxt);

                    return rc;
                }
            }

            if (UNDEFINED_LS00 == pCol->colFormat_ls00.ffoFrac_ls00)
            {
                pCol->colFormat_ls00.ffoFrac_ls00 = pFI->fiBasics_ls00.fbFraction_ls00;
            }
        }
    }   // end for (i=0; i < TableInfo.tdFieldCount_ls00; ++i)


    tls00_DataSource *pDataSrc = &pFLRec->flrDataSource_ls00;


    // Here we have always TableInfo.tdFieldCount_ls00 data fields to deal with (reads, assignments, defaults)
    // Input line buffer used to read data from data file
    pDataSrc->dasInputBuffer.lBufLen_ls00 = 6*pDBInfo->dbiPageSize_ls00;
    pDataSrc->dasInputBuffer.pBufPtr_ls00 = new char[pDataSrc->dasInputBuffer.lBufLen_ls00];
    pDataSrc->dasInputBuffer.lBufPos_ls00 = 0;

    rc = ls18RecordConstructor(TableInfo.tdFieldCount_ls00, pDataSrc->dasConvertedData);

    pDataSrc->lFillIndex_ls00     = new tsp00_Uint4[TableInfo.tdFieldCount_ls00];
    pDataSrc->lReadFieldsCnt_ls00 = TableInfo.tdFieldCount_ls00;


    //*
    //*     Define number of values to read from file
    //*
    //* Count of assignments:
    //*  - if columns were specified in the cmd they may be specified with
    //*    assignments like 'col1 DATE'; those data can not be read
    //*  - if no columns were specified  in the cmd the count of
    //*    assignments is 0, too
    //*
    //* Count of not specified columns that needs to be loaded anyway
    //*  - even if several columns (or all) are not specified in the command
    //*    they need to be handled specially -> load defaults; those data can
    //*    not be read, too; a very special column of this type is the SYSTEMKEY
    tsp00_Int4 lAssignCnt = 0;

    if (0 != pMultCol->mcsCount_ls00)
    {
        for (i=0; i < pMultCol->mcsCount_ls00; ++i)
        {
            pCol = pMultCol->mcsColumn_ls00[i];
            if ( (0 == pCol->colFPos_ls00.fpoStart_ls00) &&
                 (asUnknown_els00 != pCol->colAssign_ls00.fasAsgnTyp_ls00) )
            {
                --pDataSrc->lReadFieldsCnt_ls00;
                ++lAssignCnt;
            }
        }
    }
    // In case user specified no column in the FASTLOAD command the server builds
    // the command internally containing all columns of the table except of the column
    // SYSTEMKEY; this column can never be given in the command and therefore we have
    // to check for columns not specified outside of the if branch that defines
    // the case of having columns defined
    for (i=0; i < TableInfo.tdFieldCount_ls00; ++i)
    {
        if (0 == TableInfo.tdFieldsArray_ls00[i]->fiStmtColNo_ls00)
        {
            --pDataSrc->lReadFieldsCnt_ls00;
        }
    }

    // tdFieldCount will always be gt ReadFieldsCount because the first specifies the count
    // of columns the table has and the latter defines the columns the user has data for;
    // if there is a difference that means that not all columns of the table are specified in
    // the FASTLOAD command; because the specification defines that columns that are not
    // given in the command have to be loaded with the default value we initialize the length of
    // those data values corresponding to not given columns to INSERT_DEFAULT
    tsp00_Uint4 lDif = STATIC_CAST(tsp00_Uint4, TableInfo.tdFieldCount_ls00) - pDataSrc->lReadFieldsCnt_ls00;
    while (lDif > 0)
    {
        pDataSrc->dasConvertedData[TableInfo.tdFieldCount_ls00 - lDif].strLeng_ls00 = INSERT_DEFAULT;
        --lDif;
    }

    //*
    //*     Initialize read position structure
    //* We need only as many read pos as we have data fields to read
    //*
    pDataSrc->lReadPos_ls00           = new tsp00_Uint4[pDataSrc->lReadFieldsCnt_ls00];
    pDataSrc->plDataToParamIndex_ls00 = new tsp00_Uint4[pDataSrc->lReadFieldsCnt_ls00];
    pDataSrc->plDataToColIndex_ls00   = new tsp00_Uint4[pDataSrc->lReadFieldsCnt_ls00];

    //*
    //*     Fill index structures
    //*
    if (0 != pMultCol->mcsCount_ls00)
    {
        for (i=0, j=0; i < pMultCol->mcsCount_ls00; ++i)
        {
            pCol = pMultCol->mcsColumn_ls00[i];
            if (0 != pCol->colFPos_ls00.fpoStart_ls00)
            {
                pDataSrc->lReadPos_ls00[j]         = pCol->colFPos_ls00.fpoStart_ls00;
                pDataSrc->plDataToColIndex_ls00[j] = i;

                for (k=0; k < TableInfo.tdFieldCount_ls00; ++k)
                {
                    if ( (0 != TableInfo.tdFieldsArray_ls00[k]->fiStmtColNo_ls00) &&
                         (TableInfo.tdFieldsArray_ls00[k]->fiStmtColNo_ls00 == (i+1)) )
                    {
                        pDataSrc->plDataToParamIndex_ls00[j] = k;
                        break;
                    }
                }
                ++j;
            }
        }
    }
    else
    {
        tsp00_Int2 lStmtNo = 0;
        i = 0;
        if (true == pFLRec->flrIsSyskey_ls00)
        {
            i = 1;
        }
        for (i, j=0; i < TableInfo.tdFieldCount_ls00; ++i, ++j)
        {
            lStmtNo = TableInfo.tdFieldsArray_ls00[i]->fiStmtColNo_ls00;

            pDataSrc->lReadPos_ls00[lStmtNo - 1]         = lStmtNo;
            pDataSrc->plDataToParamIndex_ls00[lStmtNo - 1] = i;
        }
    }


    j = pDataSrc->lReadFieldsCnt_ls00 + lAssignCnt;     // index start for not specified columns
    k = pDataSrc->lReadFieldsCnt_ls00;                  // index start for assigments

    if (0 != pMultCol->mcsCount_ls00)
    {
        for (i = 0; i < TableInfo.tdFieldCount_ls00; ++i)
        {
            if (0 == TableInfo.tdFieldsArray_ls00[i]->fiStmtColNo_ls00)
            {
                pDataSrc->lFillIndex_ls00[i] = j;
                ++j;
            }
            else
            {
                pCol = pMultCol->mcsColumn_ls00[TableInfo.tdFieldsArray_ls00[i]->fiStmtColNo_ls00 - 1];
                if ( (0 == pCol->colFPos_ls00.fpoStart_ls00) &&         // special value: DATE, TIME, ..., USER,...
                     (asUnknown_els00 != pCol->colAssign_ls00.fasAsgnTyp_ls00) )
                {
                    // Assignment specified in load command
                    pDataSrc->lFillIndex_ls00[i] = k;
                    pDataSrc->dasConvertedData[k].strLeng_ls00 = INSERT_ASSIGNED;
                    ++k;
                }
                else
                {
                    // Real position in load command
                    tsp00_Uint4 v = 0;
                    for (; v < pDataSrc->lReadFieldsCnt_ls00; ++v)
                    {
                        if (pDataSrc->plDataToColIndex_ls00[v] == STATIC_CAST(tsp00_Uint4, TableInfo.tdFieldsArray_ls00[i]->fiStmtColNo_ls00 - 1) )
                        {
                            pDataSrc->lFillIndex_ls00[i] = v;
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        for (i=0; i < TableInfo.tdFieldCount_ls00; ++i)
        {
            if (0 == TableInfo.tdFieldsArray_ls00[i]->fiStmtColNo_ls00)
            {
                pDataSrc->lFillIndex_ls00[i] = j;
                ++j;
            }
            else
            {
                pDataSrc->lFillIndex_ls00[i] = TableInfo.tdFieldsArray_ls00[i]->fiStmtColNo_ls00 - 1;
            }
        }
    }
    
    if ( (false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) &&
         (0     == pMultCol->mcsCount_ls00) )
    {
        pFLRec->flrNullValueCounter_ls00.nvcCount_ls00 = SAPDB_TRUE;

        // Allocate auxiliary condition structure
        pFLRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00 = new tls00_CondSpec;
        memset(pFLRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00, 0, sizeof(tls00_CondSpec));
        pFLRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosDefNull_ls00 = true;
        pFLRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00 = new tls00_Condition;
        memset(pFLRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00->cosCond_ls00, 0, sizeof(tls00_Condition));

        rc = ls16ConvertCondition(*pFLRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00,
                                  pDBInfo->dbiOwnSwap_ls00,
                                  pTransformationCmd->trDataFile_ls00.fsFormat_ls00,
                                  ErrText);


        // Check if we have to check for every value of every column that allows NULLs if we
        // have to insert found values that correspond to null value specification as NULLs or
        // as true value; we check all values if no columns were specified in the command and the format of
        // the data file is either CSV or FWD.
        // In case the user did not specify a column in the load command no Column-Structure is allocated so
        // far and we have to do it here.

        // adapt column count
        SAPDB_Int4 _colCount = TableInfo.tdFieldCount_ls00;
        SAPDB_Int4 _idx      = 0;
        if (true == pFLRec->flrIsSyskey_ls00)
        {
            --_colCount;
            ++_idx;
        }
        pFLRec->flrNullValueCounter_ls00.nvcColumnAllowsNull_ls00 = new SAPDB_Bool[_colCount];

        if (errOK_els00 == rc)
        {
            for (_idx; _idx < TableInfo.tdFieldCount_ls00; ++_idx)
            {
                pFI = TableInfo.tdFieldsArray_ls00[_idx];


                // We expect colNullCond_ls00 to be NULL here!
                if ( (mod_optional == pFI->fiColMode_ls00) ||
                     ( (mod_optional != pFI->fiColMode_ls00)&& (sp7_no_default != pFI->fiDefault_ls00)) )
                {
                    pFLRec->flrNullValueCounter_ls00.nvcColumnAllowsNull_ls00[pFI->fiStmtColNo_ls00-1] = SAPDB_TRUE;
                }
            }
        }
    }   // if ( (false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) &&


    //*
    //*     Fill pattern structure
    //*
    rc = ls18ConstructPattern(&pFLRec->flrPattern_ls00,
                              pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoSeperat_ls00,
                              pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoDelimit_ls00,
                              pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00,
                              ErrText);

    //*     Some traces
    TRACE_PRNF_MLS99 (("ls21ImportTableRowPages", "Computed internal length of fix long columns: %d",
                                           pFLRec->flrFixColLen_ls00));
    TRACE_PRNF_MLS99 (("ls21ImportTableRowPages", "Count of variable long columns: %d",
                                           pFLRec->flrVarColCnt_ls00));
    TRACE_PRNF_MLS99 (("ls21ImportTableRowPages", "Computed internal record length: %d",
                                           pFLRec->flrRecordLen_ls00));

    //*
    //*     Zu guter letzt ueberpruefe die usageangabe auf ausfuehrbarkeit
    //*
    tls00_PageInfo  *PGI = &pFLRec->flrPageInfo_ls00;
    tsp00_Int4  tmpSpace = PGI->pgiDataEndPos - PGI->pgiDataStartPos;   // corresponds to 100% usage of one page

    TRACE_PRNF_MLS99 (("ls21ImportTableRowPages", "Available page space at full usage: %d", tmpSpace));

    if (true == pTransformationCmd->trUsage_ls00.uUsingRows_ls00)
    {
        // fuer jeden einzufuegenden datensatz wird auf der page ein pointer angelegt, der auf den
        // anfang dieses datensatzes verweist; der platzbedarf fuer diesen pointer (POINTERSIZE_BD00)
        // muss in die berechnung mit eingehen
        PGI->pgiDataSpace = pTransformationCmd->trUsage_ls00.uUsageValue_ls00 * (pFLRec->flrRecordLen_ls00 + POINTERSIZE_BD00);

        TRACE_PRNF_MLS99 (("ls21ImportTableRowPages", "Available page space if %d rows per page: %d.",
                                               pTransformationCmd->trUsage_ls00.uUsageValue_ls00,
                                               PGI->pgiDataSpace));
        if (PGI->pgiDataSpace > tmpSpace)
        {
            // Issue a warning and adapt space value
            pMsgObj->ls98PrintMessage(layFastload_els98, wrnUsageIgnoredRows_els98, pTransformationCmd->trUsage_ls00.uUsageValue_ls00);
            pTransformationCmd->trUsage_ls00.uUsageValue_ls00 = tmpSpace/(pFLRec->flrRecordLen_ls00 + POINTERSIZE_BD00);
            PGI->pgiDataSpace = tmpSpace;
        }
    }
    else
    {
        // der platzbedarf fuer den pointer auf den ersten datensatz muss an dieser stelle bereits
        // abgezogen werden, um den wirklich vorhandenen platz zu bestimmen

        PGI->pgiDataSpace = ( (tmpSpace * pTransformationCmd->trUsage_ls00.uUsageValue_ls00) / 100 ) -  POINTERSIZE_BD00;

        // zusaetzlich muessen wir pruefen, ob bereits der erste satz diese grenze ueberschreiten wuerde;
        // fuer 100% usage wird das eigentlich vom kern abgefangen, da ein record nicht laenger gewaehlt
        // werden kann, als in eine seite mit allen zusatzinformationen passt.
        // fuer einen usage-wert zwischen 50% und 100% kann aber der erste satz bereits den fuellgrad
        // ueberschreiten. In diesem fall wird eine warnung ausgegeben und der fuellgrad schweigend ignoriert.

        TRACE_PRNF_MLS99 (("ls21ImportTableRowPages", "Available page space at %d%% usage: %d",
                                               pTransformationCmd->trUsage_ls00.uUsageValue_ls00,
                                               PGI->pgiDataSpace));

        if (pFLRec->flrRecordLen_ls00 > PGI->pgiDataSpace)
        {
            // Issue a warning and adapt space value
            pMsgObj->ls98PrintMessage(layFastload_els98, wrnUsageIgnoredPercent_els98, pTransformationCmd->trUsage_ls00.uUsageValue_ls00);
            PGI->pgiDataSpace = tmpSpace -  POINTERSIZE_BD00;
        }
    }
    // recordlength is implicitely checked while building the page in ls22-module: every value
    // must fit into the length given by the database

	//*
    //*  update transformation table
    //*
    if ( errOK_els00 == rc )
         rc = ls22UpdateTransformationPackage(pDBInfo,pTransformationDef,pFLRec,ErrText);

	//*
    //*  execute fastload
    //*
    tsp00_Int8 _lRejectedRecords = 0;
    tsp00_Int8 _lInsertedRecords = 0;

    if (errOK_els00 == (rc = ls21_ExecBeginLoad(pDBInfo, pFLRec, TableSpec.tsFullTabName_ls00, ErrText)))
    {
        SAPDB_Bool _bRollback = SAPDB_TRUE;


        // Calculate count of pages fitting into a single packet - here because we have here the length of bdinfo only;
        // tls00_PktBlock contains packet-, segment- and part header size. Because each packet
        // consists of 2 parts the part header has to be substracted again.
        pFLRec->flrPgPerPckt_ls00 = STATIC_CAST( tsp00_Int2, (pDBInfo->dbiPktSize_ls00 - sizeof(tls00_PktBlock)   
                                                              - sizeof(tsp1_part_header)
                                                              - pFLRec->flrBdInfo_ls00.bdiLength_ls00) / pDBInfo->dbiPageSize_ls00 );        

        rc = ls22LoadPages(pTransformationDef, pFLRec, pDBInfo, &_bRollback, ErrText);
        if ( (errOK_els00 == rc) ||
             ( (errOK_els00 != rc) && (SAPDB_FALSE == _bRollback) ) )
        {
            //*
            //*     This is the final statement that signals the kernel that all data was sent
            //*
            tsp00_Int4 rc2 = ls21_ExecEndLoad(pDBInfo, pFLRec, TableSpec.tsFullTabName_ls00, ErrText);
            if (errOK_els00 == rc2)
            {
                pMsgObj->ls98PrintMessage(layFastload_els98, msgLastCommInputLine2_els98, pFLRec->flrLastCommLine_ls00);

                _lInsertedRecords = pFLRec->flrRecordCount_ls00;
                _lRejectedRecords = pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors - pFLRec->flrLRejected_ls00;
            }
            else
            {
                _lInsertedRecords = 0;
                _lRejectedRecords = pFLRec->flrRecordCount_ls00;
                if (errOK_els00 == rc)
                {
                    rc = rc2;
                }
            }
        }
        else
        {
            //*
            //*     Error processing FASTLOAD command; all submitted data will be rolled back
            //*
            tsp00_Int4 rc2 = ls15Rollback(pDBInfo, ErrText);

            pMsgObj->ls98PrintMessage(layFastload_els98, msgLastCommInputLine2_els98, pFLRec->flrLastCommLine_ls00);

            _lInsertedRecords = 0;
            _lRejectedRecords = pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors - pFLRec->flrLRejected_ls00;
        }
        pMsgObj->ls98PrintMessage(layFastload_els98, msgInsertedRejectedRows2_els98, _lInsertedRecords,
                                                                                     _lRejectedRecords);

        SAPDB_Char _szAux[64];
        _szAux[0] = 0;
        sp77sprintf(_szAux, 64, " (= %d pages)", pFLRec->flrPageCount_ls00);
        pMsgObj->ls98AppendStatusToString(layFastload_els98, ErrText, statInsertedRejectedRows_els98,
                                                                      _lInsertedRecords,
                                                                      _szAux,
                                                                      _lRejectedRecords);
        if (SAPDB_TRUE == pFLRec->flrNullValueCounter_ls00.nvcCount_ls00)
        {
            if (0 != pFLRec->flrNullValueCounter_ls00.nvcCounter_ls00)
            {
                pMsgObj->ls98PrintMessage(layFastload_els98, msgNULLValuesInserted_els98, pFLRec->flrNullValueCounter_ls00.nvcCounter_ls00,
                                                                                          pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoNullSpec_ls00);
                pMsgObj->ls98AppendStatusToString(layDataLoad_els98, ErrText, statNULLValuesInserted_els98,
                                                                              pFLRec->flrNullValueCounter_ls00.nvcCounter_ls00,
                                                                              pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoNullSpec_ls00);
            }
        }
    }   // end if (errOK_els00 == (rc = ls21_ExecBeginLoad(pDBInfo, pFLRec, TableSpec.tsFullTabName_ls00, ErrText)))

	//*
    //*  update transformation table
    //*
    if ( errOK_els00 == rc )
         rc = ls22UpdateTransformationPackage(pDBInfo,pTransformationDef,pFLRec,ErrText);

    if ( errOK_els00 == rc && 0 != _tcscmp(pDBInfo->dbiSession_ls00.siSchemaName_ls00, pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp()) )
         rc = ls15SwitchSchema(pDBInfo, sessionschema, ErrText);

    ls21_FreeFastLoadRec(pFLRec, pTransformationCmd);

    return rc;
}
//  ls21ImportTableRowPages()
