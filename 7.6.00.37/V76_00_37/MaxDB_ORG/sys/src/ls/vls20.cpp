/*
  -----------------------------------------------------------------------------
  module: vls20.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of the DATALOAD functionality

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

#include <time.h>

#include "gls00.h"
#include "hls03.h"
#include "hls04.h"
#include "hls041.h"
#include "hls05.h"
#include "hls07.h"
#include "hls08_long.h"
#include "hls10.h"
#include "hls11.h"
#include "hls15.h"
#include "hls16_condition.h"
#include "hls18.h"
#include "hls20.h"
#include "hls20_dataload.h"
#include "hls20_parse_stmt.h"
#include "hls24.h"
#include "hls25.h"
#include "hls30.h"
#include "hls99.h"
#include "hls98msg.h"

/*!
  -----------------------------------------------------------------------------
  Chapter: Local functions
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
  function:     ls20_ParseStmt
  -----------------------------------------------------------------------------
  description:  Prepare INSERT Statement as mass command.
                Extraxt table information out of the answer packet

  arguments:    MetaDataDef         [IN]  - structure with info on running session and
                                            database connected to
                pTransformationRTE  [IN]  - structure with information on
                                            processing the dataload command
                pszErrText          [OUT] - error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_ParseStmt(MetaDataDef*&             pMetaDataDef,
               tls00_TransformationRTE*  pTransformationRTE,
               tsp00_Addr                pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_InitializeOIPacket
  -----------------------------------------------------------------------------
  description:  Initializes buffer that serves as data part buffer for
                the order interface packet.

  arguments:    pDBInfo            [IN]  - Structure with info on running session and
                                            database connected to
                pTransformationRTE [IN|OUT] 
  
  returnvalue:  errOK_els00             - no error
                errWriteBuf2Part_els98  - error writing parse id or result count to packet
                errInitPart_els98       - error initilizing parts
                errFinishPart_els98     - error finishing parts

                Function returns only error codes but no error text.
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_InitializeOIPacket(tls00_DBInfo*  pDBInfo, tls00_TransformationRTE*  pTransformationRTE);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_ValueToOIBuffer
  -----------------------------------------------------------------------------

  description:  Writes single DATA to DATA part buffer in the right format.
                Data has be given in the right database format.
   
  arguments:    pInputData          [IN]     - Data to write to buffer
                ParameterInfo       [IN]     - Info describing DATA
                pColumn             [IN]     - Structure describing column
                DataFileEncoding    [IN]     - Data file encoding
                bUnicodeOI          [IN]     - Database UNICODE or ASCII
                pOIValueBuffer      [IN/OUT] - Buffer to put the DATA in
                pszErrText          [OUT]    - Error text
  
  returnvalue:  errOK_els00                 - No error
                errIncompatibleConst_els98  - Error inserting assigned value
                errUnknownAssignment_els98  - Error inserting assigned value
                errInternal_els98           - Error converting between encodings
                errConvertingEncoding_els98 - Error converting between encodings
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_ValueToOIBuffer(tls00_String        &pInputData,
                     tls00_ParamInfo     &ParameterInfo,
                     tls00_Column        *pColumn,
                     tls00_CodeType       DataFileEncoding,
                     tsp81_CodePage      *pCodePageTable,
                     bool                 bUnicodeOI,
                     unsigned char       *pOIValueBuffer,
                     tsp00_Addr           pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20_ReadMetaHeader
  -----------------------------------------------------------------------------
  Function requires the file opened in binary mode!
*/
static SAPDB_Int4
ls20_ReadMetaHeader(tls00_DBInfo*             pDBInfo,
                    tls00_TransformationCmd*  pTransformationCmd,
                    tls00_TransformationRTE*  pTransformationRTE,
                    tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20_ReadMetaTrailer
  -----------------------------------------------------------------------------
  Function requires the file opened in binary mode!
*/
static SAPDB_Int4
ls20_ReadMetaTrailer(tls00_DBInfo*             pDBInfo,
                     TransformationDef*&       pTransformationDef,
                     tls00_TransformationCmd*  pTransformationCmd,
                     tls00_TransformationRTE*  pTransformationRTE,
                     Tools_DynamicUTF8String&  DatLoadStmt,
                     tsp00_Addr                ErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls20_CheckHeader
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_CheckHeader(tls00_DBInfo            *pDBInfo,
                 tls00_TransformationCmd *pDatLoadCmd,
                 Tools_DynamicUTF8String  DatExtrStmt,
                 tsp00_Addr               ErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_TransformRowColumns
  -----------------------------------------------------------------------------
  description:  Build a database record

                This function builds one database record and writes it to a buffer.

  arguments:    pTransformationCmd    [IN]    - structure with user information for
                                       processing dataload command.
                pTransformationRTE    [IN]    - structure with information on
                                       processing the dataload command
                Errtext     [OUT]   - error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_TransformRowColumns(tls00_TransformationCmd  *pTransformationCmd,
                         tls00_TransformationRTE     *pTransformationRTE,
                         tsp1_sqlmode_Enum  SQLMode,
                         bool               bUnicodeOI,
                         tsp00_Addr         ErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls20_TransformTableRows
  -----------------------------------------------------------------------------
  description:  Build as many database records as fit into one packet

                This function builds one database record and writes it to a buffer.

  arguments:    DBInfo      [IN]    - structure with info on running session and
                                       database connected to
                pTransformationCmd [IN]    - structure with user information for
                                       processing dataload command
                pTransformationRTE [IN]    - structure with information on
                                       processing the dataload command
                ErrText     [OUT]   - error text
                RowCount    [OUT]   - Count of records stored in pszRecord
                bPutVal     [OUT]   - flags the need of using putval - oi-packets
                                       to process remaining long values

  returnvalue:  errOK_els00         - in case of NO errors
                error reading file
                error in processing DATA uebersteigt maxerrorcount
                  some other errors - in case of errors
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_TransformTableRows(tls00_DBInfo*              DBInfo,
                        tls00_TransformationCmd*   pTransformationCmd,
                        tls00_TransformationRTE*   pTransformationRTE,
                        tls00_AlternatingBuffers*  pDataPartCache,
                        bool                       bRecoverLong,
                        tsp00_Addr                 ErrText,
                        bool                      &bPutVal);


/*
  -----------------------------------------------------------------------------
  function:     ls20_DL_Commit
  -----------------------------------------------------------------------------

  description:  

  arguments:    

  returnvalue:  errOK_els00        - Success
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_DL_Commit(tls00_DBInfo*             pDBInfo,
               SAPDB_Int4                lPcktNo,
               MetaDataDef*&             pMetaDataDef,
               tls00_TransformationCmd*  pTransformationCmd,
               tls00_TransformationRTE*  pTransformationRTE,
               SAPDB_Int4                lTransactionSize,      // defines number of rows to be loaded before commit
               SAPDB_Int4&               lInsertedRowsSinceLastCommit,
               tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20_FillPacketBuffer
  -----------------------------------------------------------------------------

  description:  

  arguments:    

  returnvalue:  errOK_els00        - Success
                STOP_NO_DATA_LS00  - No data row to send found; not an error rather
                                      a hint for the caller
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_FillPacketBuffer(tls00_DBInfo*             pDBInfo,
                      tls00_TransformationCmd*  pTransformationCmd,
                      tls00_TransformationRTE*  pTransformationRTE,
                      tls00_AlternatingBuffers* pDataPartCache,
                      tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20_ReceivePacket
  -----------------------------------------------------------------------------

  description:  

  arguments:    

  returnvalue:  errOK_els00        - Success
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_ReceivePacket(tls00_DBInfo*              pDBInfo,
                   tls00_TransformationCmd*   pTransformationCmd,
                   tls00_TransformationRTE*   pTransformationRTE,
                   tls00_AlternatingBuffers*  pAltBuffers,
                   SAPDB_Int4 &               lErrorPos,
                   tsp00_Addr                 pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_Recover
  -----------------------------------------------------------------------------
  description:  Rebuilds the packet buffer after an error: Removes the inserted
                records (records before erroneous record) and the erroneous itself.
                Moves the valid records to the beginning of the packet buffer.
                Resets FreeSpace in packet and dpc_lRowCount_ls00 and bPutVal.

  arguments:    pAlternatingBuffers  [IN]     - structure with information for processing
                                                 data part buffer
                lLongColCnt          [IN]     - Count of LONG columns in table
                pTD                  [IN]     - Info on table to load
                lErrorPos            [IN]     - Number of erroneous record in buffer
                bPutVal              [IN/OUT] - Indicating if the last record has longs
                                                 that needs to be send using putval-packets;
                                                 in case this is the erroneous record the
                                                 function sets bPutVal to false
  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls20_Recover(tls00_AlternatingBuffers* pAlternatingBuffers,
             tsp00_Int4                lLongColCnt,
             tls00_TableDescription*   pTD,
             SAPDB_Int4                lErrorPos,
             bool                    & bPutVal);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_MoveDataWithinPacket
  -----------------------------------------------------------------------------
  description:  

  arguments:    

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls20_MoveDataWithinPacket(tls00_OI_DataPartCache *  pDataPartCache,
                          SAPDB_Int4                lLongColCnt,
                          SAPDB_Int4                plLongIndex[],
                          tls00_TableDescription*   pTD,
                          SAPDB_Int4                lErrorPos);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_TransformColumnValue
  -----------------------------------------------------------------------------
  description:  Transforms external data to internal db format.

  arguments:    pRawValue     [IN]  - Data to transform
                pColumn       [IN]  - Structure describing column
                pInputBuffer  [IN]  - Input buffer
                pParamInfo    [IN]  - Info describing data
                pFileSpec     [ÍN]  - Structure describes file 
                SwapKind      [OUT] - Swap kind of load server
                SQLMode       [IN]  - sql mode of session; important to decide
                                       wether an empty value must be inserted
                                       as NULL (sql mode = oracle) or as empty
                                       value (sql mode = internal)
                pszErrText    [OUT] - Error text
  
  returnvalue:  errOK_els00         - in case of NO errors
                errors from called functions;
                function logs and returns the error text to caller
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls20_TransformColumnValue(tls00_String        *pRawValue,
                          tls00_Column        *pColumn,
                          tls00_Buffer2       *pInputBuffer,
                          tls00_ParamInfo     *pParamInfo,
                          tls00_FileSpec      *pFileSpec,
                          tsp00_SwapKind_Enum  SwapKind,
                          tsp1_sqlmode_Enum    SQLMode,
                          tls00_Pattern*       pPattern,
                          tsp00_Addr           pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_BuildRecord
  -----------------------------------------------------------------------------
  description:  Puts transformed data together to a record that can be processed
                by the database kernel to insert or update values.

  arguments:    pTransformationCmd [IN]   - user information for processing command
                pTransformationRTE [IN]   - runtime information for processing command
                pDataPartBuffer    [OUT]  - Buffer to store built record
                bUnicodeOI         [IN]   - DB UNICODE or ASCII?
                pszErrText         [OUT]  - Error text
  
  returnvalue:  errOK_els00         - in case of NO errors
                Errors converting data to UNICODE in case of UNICODE DB
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls20_BuildRecord(tls00_TransformationCmd*  pTransformationCmd,
                 tls00_TransformationRTE*  pTransformationRTE,
                 tls00_Buffer*             pDataPartBuffer,
                 bool                      bUnicodeOI,
                 tsp00_Addr                pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_ReadDataLength
  -----------------------------------------------------------------------------
  description:  Analyze length information written in meta data fields in case
                of loading data in file format RECORDS. Fills length members
                pmdLengthArray_ls00 and mdRecordLength_ls00 in structure of type
                tls00_MetaData (defined in gls00.h).
                Computing the record length NULL-values and LONG-values are
                not counted.

  arguments:    pMeta      [OUT]  - meta data structure to fill
                DSwap      [IN]   - swap behavior of destination db/machine
                pszErrText [OUT]  - error text
  
  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls20_ReadDataLength(tls00_MetaData *pMetaDataDef, tsp00_SwapKind_Enum DSwap, tsp00_Addr pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls20_LoadRowColumns
  -----------------------------------------------------------------------------
  description:  Loop for building mass insert requests, sending them to the kernel
                checking the kernel answer, sending putvals, ...


  arguments:    DBInfo      [IN]    - structure with info on running session and
                                       database connected to
                pTransformationCmd [IN]    - structure with user information for
                                       processing dataload command
                pTransformationRTE [IN]    - structure with information on
                                       processing the dataload command
                ErrText     [OUT]   - error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors   - in case of errors
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_LoadRowColumns(tls00_DBInfo*             DBInfo,
                    MetaDataDef*&             pMetaDataDef,
                    tls00_TransformationCmd*  pTransformationCmd,
                    tls00_TransformationRTE*  pTransformationRTE,
                    tsp00_Addr                ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20_InitializeDataStream
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_InitializeDataStream(tls00_TransformationCmd*  pTransformationCmd,
						  tls00_TransformationRTE*  pTransformationRTE,
						  tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20_InitializeStructuresBeforeParsing
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_InitializeStructuresBeforeParsing(tls00_DBInfo*             pDBInfo,
						               tls00_TransformationCmd*  pTransformationCmd,
									   tls00_TransformationRTE*  pTransformationRTE,
									   tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20_InitializeStructuresAfterParsing
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_InitializeStructuresAfterParsing(tls00_DBInfo*             pDBInfo,
									  tls00_TransformationCmd*  pTransformationCmd,
									  tls00_TransformationRTE*  pTransformationRTE,
									  tsp00_Addr                pszErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls20_ParseAgainErrorHandler
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_ParseAgainErrorHandler(MetaDataDef*&             pMetaDataDef,
                            tls00_TransformationRTE*  pTransformationRTE,
                            SAPDB_Int4                lErrorPos,
                            tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls20_ComputeErroneousRowNo
  -----------------------------------------------------------------------------
*/
inline SAPDB_Int4
ls20_ComputeErroneousRowNo(tls00_TransformationCmd*  pTransformationCmd,
                           const SAPDB_Int4          lProcessedRow)
{
    
    if (UNDEFINED_LS00 == pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00)
    {
        return lProcessedRow;
    }
    else
    {
        return (lProcessedRow + pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 - 1);
    }
}

/*!
  -----------------------------------------------------------------------------
  EndChapter:   Local function declaration
  -----------------------------------------------------------------------------
 */

/*
  -----------------------------------------------------------------------------
  BeginChapter: Global function implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls20ImportTableRows
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20ImportTableRows(MetaDataDef*&             pMetaDataDef,
                    tls00_TransformationRTE*  pTransformationRTE,
                    tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20ImportTableRows");

    tsp00_Int4      rc      = errOK_els00;
    LoaderMessages *pMsgObj = LoaderMessages::Instance();

    TransformationDef       *pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_DBInfo            *pDBInfo            = pMetaDataDef->getConnection();
    SAPDB_Bool               bFormatSwitched    = SAPDB_FALSE;

    //*
    //*  Create transformation RunTime structure
    //*
    ls20ConstructRTEStructure(pTransformationRTE, pDBInfo, pTransformationCmd);

    //*
    //*  Update transformation table and read metadata header
    //*
    rc = ls20UpdateTransformationPackage(pDBInfo, pMetaDataDef, pTransformationCmd, pTransformationRTE, ErrText);
    if (errOK_els00 != rc )
    {
        pMsgObj->ls98Msg(layDataExtract_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
        return errInternal_els98;
    }
    
    if (true  == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
    {
        // The data file must have been opened binary
        rc = ls20_ReadMetaHeader(pDBInfo, pTransformationCmd, pTransformationRTE, ErrText);
    }

    if ( (errOK_els00 == rc) && (SAPDB_FALSE == pTransformationCmd->trExclude_ls00) )
    {
        rc = ls20_InitializeDataStream(pTransformationCmd, pTransformationRTE, ErrText);
    }

    if (errOK_els00 == rc)
    {
        rc = ls20_InitializeStructuresBeforeParsing(pDBInfo, pTransformationCmd, pTransformationRTE, ErrText);
    }

    // Parse INSERT/UPDATE statement as mass cmd
    if (errOK_els00 == rc)
    {
        rc = ls20_ParseStmt(pMetaDataDef, pTransformationRTE, ErrText);
    }

    if (errOK_els00 == rc)
    {
        rc = ls20_InitializeStructuresAfterParsing(pDBInfo, pTransformationCmd, pTransformationRTE, ErrText);
    }
    
    //*
    //*   load DATA record values
    //*
    tsp00_Longint lRejectedRows = pTransformationRTE->rte_Cnt_RejectedRecords_ls00;

    if ( errOK_els00 == rc &&
         false == pTransformationRTE->rte_TabEmpty_ls00 &&      // Wird in ReadMetaHeader gesetzt
         false == pTransformationCmd->trExclude_ls00   )        // Wird in ReadMetaHeader gesetzt
    {
        if (ctUnknown_els00 != pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
        {
            rc = ls05AreEncodingsCompatible(pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00,
                                            ls00IsDBUnicode(pDBInfo) ? ctUCS2_els00 : ctAscii_els00);
            if (errOK_els00 != rc)
            {
                // Unicode file and ascii DB do not match
                pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errIncompatibleEncodings_els98);
                rc = errIncompatibleEncodings_els98;
            }
        }

        //*
        //*  run dataload
        //*
        // Here starts the REAL dataload only
        // To indicate errors before dataloading we use a 'hack' - set rejected to 1 to prevent from
        // continuing if max admissible errors is 1
        if (errOK_els00 != rc)
        {
            pTransformationRTE->rte_Cnt_RejectedRecords_ls00 = 1;
        }

        //*
        //*  Switch DB date and time format
        //* User may have run the statement 'SET FORMAT <name>'. If the format set is different from
        //* INTERNAL the Loader must switch the format to INTERNAL before sending the data and
        //* switch the format again to the user requested value after sending the data.
        //* The Loader does only process date and time data delivered by the kernel in format INTERNAL.
        //*
        if ((errOK_els00 == rc) && (dtInternal_els00 != pDBInfo->dbiSession_ls00.siFormat_ls00))
        {
            rc = ls15SwitchFormat(pDBInfo, dtInternal_els00, ErrText);
            if (errOK_els00 == rc)
            {
                bFormatSwitched = SAPDB_TRUE;
            }
        }

        //*
        //*  run dataload
        //*
        if (errOK_els00 == rc)
        {
            rc = ls20_LoadRowColumns(pDBInfo, pMetaDataDef, pTransformationCmd, pTransformationRTE, ErrText);

            if (errOK_els00 == rc)
            {
                if ( 0 == pTransformationRTE->rte_Cnt_ProcessedRecords_ls00 && ptTable_els00 == pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00 )
                {
                      pMsgObj->ls98Msg(layDataLoad_els98, ErrText, msgEmptyDataFile_els98, pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00);
                      rc = msgEmptyDataFile_els98;
                }
            }
        }

        //*
        //*  Switch DB date and time format back to user requested value
        //*
        if (SAPDB_TRUE == bFormatSwitched)
        {
            rc = ls15SwitchFormat(pDBInfo, pDBInfo->dbiSession_ls00.siFormat_ls00, ErrText);
        }

        //*
        //* write log file
        //*
        if ( (errOK_els00 == rc) && (false  == pTransformationCmd->trInternal_ls00) )
        {
            char* _ptmp = pTransformationCmd->trTable_ls00.tsTabOwner_ls00.asCharp();
            if (0      == pTransformationCmd->trTable_ls00.tsTabOwner_ls00.length ())
            {
                _ptmp = pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp();
            }
            if (false         == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 && 
                ptTable_els00  < pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
            {
                pMsgObj->ls98PrintMessage(layDataLoad_els98,msgDLSuccessful_els98, _ptmp,
                                          pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());
            }
            else if ( ptTable_els00  < pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
            {
                pMsgObj->ls98PrintMessage(layDataLoad_els98,msgTL0Successful_els98,_ptmp, 
                                          pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp() );
            }
        }

        if (rc != errFileOpen_els98)
        {
            // adapt rte_LastCommLine_ls00 for the case of a defined start position
            if (UNDEFINED_LS00 != pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00)
            {
                // problems with all|user ?
                pTransformationRTE->rte_LastCommLine_ls00 += pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 - 1;
            }

            if ( false == pTransformationCmd->trInternal_ls00 )
            {
                if (0 != pTransformationRTE->rte_Cnt_CommittedRecords_ls00) {
                    if ( true  == pDBInfo->dbiSession_ls00.siAutoCommit_ls00 )
                        pMsgObj->ls98PrintMessage(layDataLoad_els98, msgLastCommInputLine_els98,
                                                                     pTransformationRTE->rte_LastCommLine_ls00);
                    else
                        pMsgObj->ls98PrintMessage(layDataLoad_els98, msgLastInsInputLine_els98,
                                                                     pTransformationRTE->rte_LastCommLine_ls00);
                }

                if (tsImport_ls00 == pTransformationCmd->trTransSpec_ls00) {
                    pMsgObj->ls98PrintMessage(layDataLoad_els98, msgInsertedRejectedRows_els98,
                                                                 pTransformationRTE->rte_Cnt_CommittedRecords_ls00,
                                                                 pTransformationRTE->rte_Cnt_RejectedRecords_ls00);
                }
                else if (tsUpdate_ls00 == pTransformationCmd->trTransSpec_ls00) {
                    pMsgObj->ls98PrintMessage(layDataUpdate_els98, msgUpdatedInvalidRows_els98,
                                                                   pTransformationRTE->rte_Cnt_CommittedRecords_ls00,
                                                                   pTransformationRTE->rte_Cnt_RejectedRecords_ls00);
                }

            }//if ( false == pTransformationCmd->trInternal_ls00 )

        }
    } //false == pTransformationRTE->rte_TabEmpty_ls00

    //*
    //*  update transformation table and read metadata trailer
    //*
    if ( errOK_els00 == rc  )
    {
         if  (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
         {
            Tools_DynamicUTF8String  DatLoadStmt;

            rc = ls20_ReadMetaTrailer(pDBInfo, pTransformationDef, pTransformationCmd, pTransformationRTE, DatLoadStmt, ErrText);
         }
         if (errOK_els00 == rc)
         {
             rc = ls20UpdateTransformationPackage(pDBInfo, pMetaDataDef, pTransformationCmd, pTransformationRTE, ErrText);
             if (errOK_els00 == rc )
             {
                 if( true == pTransformationRTE->rte_TabEmpty_ls00 && false == pTransformationCmd->trInternal_ls00  )
                      pMsgObj->ls98PrintMessage(layDataLoad_els98, msgInsertedRejectedRows_els98,
                                                                   pTransformationRTE->rte_Cnt_CommittedRecords_ls00,
                                                                   pTransformationRTE->rte_Cnt_RejectedRecords_ls00, ""NEW_LINE"// *");
             }
             else
             {
                 pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
                 rc = errInternal_els98;
             }
         }
    }

    // Check if any record of a this single table could be inserted
    if ( (errOK_els00 == rc)                      &&
         (false == pTransformationRTE->rte_TabEmpty_ls00) && 
         (false == pTransformationCmd->trExclude_ls00   ) &&
         (pTransformationRTE->rte_Cnt_ProcessedRecords_ls00 == (pTransformationRTE->rte_Cnt_RejectedRecords_ls00 - lRejectedRows) ) )
    {
        pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errDLNoDataLoaded_els98,
                                                     pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());
        rc = errDLNoDataLoaded_els98;
    }

    // Write some status message to the return string in any case - error or not
    if ( false == pTransformationCmd->trInternal_ls00 )
    {
        if (tsImport_ls00 == pTransformationCmd->trTransSpec_ls00)
        {
            pMsgObj->ls98AppendStatusToString(layDataLoad_els98, ErrText, statInsertedRejectedRows_els98,
                                                                          STATIC_CAST(SAPDB_Int8, pTransformationRTE->rte_Cnt_CommittedRecords_ls00),
                                                                          " ",
                                                                          STATIC_CAST(SAPDB_Int8, pTransformationRTE->rte_Cnt_RejectedRecords_ls00));
        }
        else if (tsUpdate_ls00 == pTransformationCmd->trTransSpec_ls00)
        {
            pMsgObj->ls98AppendStatusToString(layDataLoad_els98, ErrText, statUpdatedInvalidRows_els98,
                                                                          STATIC_CAST(SAPDB_Int8, pTransformationRTE->rte_Cnt_CommittedRecords_ls00),
                                                                          STATIC_CAST(SAPDB_Int8, pTransformationRTE->rte_Cnt_RejectedRecords_ls00));
        }

        if (SAPDB_TRUE == pTransformationRTE->rte_NullValueCounter_ls00.nvcCount_ls00)
        {
            if (0 != pTransformationRTE->rte_NullValueCounter_ls00.nvcCounter_ls00)
            {
                pMsgObj->ls98PrintMessage(layDataLoad_els98, msgNULLValuesInserted_els98, pTransformationRTE->rte_NullValueCounter_ls00.nvcCounter_ls00,
                                                                                          pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoNullSpec_ls00);
                pMsgObj->ls98AppendStatusToString(layDataLoad_els98, ErrText, statNULLValuesInserted_els98,
                                                                              pTransformationRTE->rte_NullValueCounter_ls00.nvcCounter_ls00,
                                                                              pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoNullSpec_ls00);
            }
        }
    }

    //*
    //*   free dataload record structure
    //*
    ls20DestructRTEStructure(pDBInfo, pTransformationCmd, pTransformationRTE);

    return rc;
}
// ls20ImportTableRows()




/*!
  -----------------------------------------------------------------------------
  function:     ls20_LoadRowColumns
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_LoadRowColumns(tls00_DBInfo             *pDBInfo,
                    MetaDataDef              *&pMetaDataDef,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_TransformationRTE  *pTransformationRTE,
                    tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_LoadRowColumns");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();

    tsp00_Int4    rc       = errOK_els00;
    tsp00_Int4    rcINSERT = errOK_els00;
    
    tls00_String  pInsertErrorText;
    pInsertErrorText.strAddr_ls00 = NULL;

    // define max count of rows to send before sending commit
    SAPDB_Int4  lCommitCount        = 0;
    tsp00_Int4  lRowCntBeforeCommit = (0 == pDBInfo->dbiSession_ls00.siTASize_ls00) ?
                                       (10*pTransformationRTE->rte_MaxRecordsInPacket_ls00) :
                                       pDBInfo->dbiSession_ls00.siTASize_ls00;

    // Define max count of rows in a single packet
    if ( (0 != pDBInfo->dbiSession_ls00.siTASize_ls00) &&
         (pDBInfo->dbiSession_ls00.siTASize_ls00 < pTransformationRTE->rte_MaxRecordsInPacket_ls00) )
    {
        pTransformationRTE->rte_MaxRecordsInPacket_ls00 = pDBInfo->dbiSession_ls00.siTASize_ls00;
    }

    bool                       bPutVal[2];
    bool                       bRecoverLong    = false;
    bool                       bParseAgain     = false;
    tsp00_Int4                 tmpRowCnt       = 0;
    char                       szErr[MAX_REPLY_LENGTH_LS00];
    szErr[0] = 0;
    bPutVal[0] = false;
    bPutVal[1] = false;

    tls00_AlternatingBuffers*  pDataPartCache = &pTransformationRTE->rte_OI_DataPart_Cache_ls00;

    // for easier code reading!
    SAPDB_Int4&                lSndPcktNo  = pDataPartCache->ab_lSndPcktNo_ls00;
    SAPDB_Int4&                lRcvPcktNo  = pDataPartCache->ab_lRcvPcktNo_ls00;
    SAPDB_Int4                 lErrorPos   = 0;

    //*
    //*   main loop to load DATA
    //*
    while ( errOK_els00 == rc && errOK_els00 == rcINSERT )
    {
        if ( (false == bPutVal[lRcvPcktNo]) && (false == bParseAgain) )       // Fill a new packet only if there are no LONGs that need special processing
        {
            //*
            //* Insert DATA into buffer
            //*
            rcINSERT = ls20_TransformTableRows(pDBInfo, pTransformationCmd, pTransformationRTE,
                                               pDataPartCache, bRecoverLong, ErrText, bPutVal[lSndPcktNo]);
            if ( (errOK_els00 != rcINSERT) && (STOP_NOMORE_DATA_LS00 != rcINSERT) && (STOP_NO_DATA_LS00 != rcINSERT))
            {
                rc = rcINSERT;

                // Keep the error message for the case it is overwritten when receiving an remaining packet
                pInsertErrorText.strLeng_ls00 = _tcslen(ErrText);
                pInsertErrorText.strAddr_ls00 = new char[pInsertErrorText.strLeng_ls00 + sizeof(char)];
                SAPDB_memcpy(pInsertErrorText.strAddr_ls00, ErrText, pInsertErrorText.strLeng_ls00);
                pInsertErrorText.strAddr_ls00[pInsertErrorText.strLeng_ls00] = 0;
            }
            bRecoverLong = false;
        }
        else if (true == bParseAgain)
        {
            bParseAgain = false;
        }

        //*
        //*     Get the answer packet
        //*
        if (lSndPcktNo != lRcvPcktNo)
        {
            lErrorPos = 0;                  // initialize to impossible error position
            rc = ls20_ReceivePacket(pDBInfo, pTransformationCmd, pTransformationRTE, pDataPartCache, lErrorPos, ErrText);

            //*
            //*     Process remaining LONGs using putval
            //* RowCount is set to 0 if there was no error in the DATA
            //*
            //* Longs have to be processed after a RECEIVE; to do so we have stopped filling new packets
            //* and waited for the receive packet; we call the LONG handler if no error has been reported in the receive
            //*
            if ( (errOK_els00 == rc) && (true == bPutVal[lRcvPcktNo]) )
            {
                tmpRowCnt = pDataPartCache->ab_pCache_ls00[lRcvPcktNo].dpc_lRowCount_ls00;    // This value is MISUSED in ls08ExecPutVal
                rc = ls08ExecPutVal(pDBInfo,
                                    &pTransformationCmd->trColSpec_ls00,
                                    pTransformationRTE,
                                    pDataPartCache,
                                    ErrText);
                if (errOK_els00 != rc)
                {
//TODOTODO: Possible to put this together with the similar part from ls20_Receive into a function ls20_LogErrorAndAdaptCOunter
//          or write a ls20_PostReceiveActionHandler?
                    ++pTransformationRTE->rte_Cnt_RejectedRecords_ls00;
                    --pTransformationRTE->rte_Cnt_CommittedRecords_ls00;
                    pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errAtRow_els98,
                                                                 ls20_ComputeErroneousRowNo(
                                                                  pTransformationCmd,
                                                                  pDataPartCache->ab_pCache_ls00[lRcvPcktNo].dpc_RecordNumbers_ls00[tmpRowCnt - 1]));

                    if (pTransformationRTE->rte_Cnt_RejectedRecords_ls00 >= pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors)
                    {
                        rc = msgTableNotImport_els98;
                    }
                    else
                    {
                        rc = errOK_els00;
                        ErrText[0] = 0;
                    }
                }
            }

            //*
            //*     Handle Error in sent packet
            //* Check if user specified number of errors allowed and if we already exceed this number
            //*
            
            // In case the error position (lErrRecordNo) is not adapted but an error reported
            // the error happened sending and/or receiving the packet and we stop at this point
            else if ( (errOK_els00 != rc) && (0 != lErrorPos) )
            {
                if (dberrParseAgain_els98 == rc)
                {
                    // In case a reparse does not fix the problem we stop processing of the command in
                    // any case, even if MAXERRORCOUNT has been set
                    rc = ls20_ParseAgainErrorHandler(pMetaDataDef, pTransformationRTE, lErrorPos, ErrText);
                    if (errOK_els00 == rc)
                    {
                        bParseAgain = true;

                        // We have to resend the packet in any case
                        if (STOP_NO_DATA_LS00 == rcINSERT)
                        {
                            rcINSERT = errOK_els00;
                        }
                    }
                }
                else
                {
                    if (pTransformationRTE->rte_Cnt_RejectedRecords_ls00 < pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors)
                    {
                        // Initializes RowCount, too
                        ls20_Recover(pDataPartCache,
                                     pTransformationRTE->rte_LongColCnt_ls00,
                                     &pTransformationRTE->rte_TableInfo_ls00,
                                     lErrorPos,
                                     bPutVal[lRcvPcktNo]);

                        rc         = errOK_els00;
                        ErrText[0] = 0;

                        // If we have some place in the packet we must recover longs first
                        if (true == bPutVal[lRcvPcktNo])
                        {
                            bRecoverLong = true;
                        }

                        // We have to send the rest of the packet that was recovered even if there is no
                        // more data to read (indicated by rcINSERT == STOP_NO_DATA_LS00);
                        // in addition we have to check if there are remaining rows that have to be send:
                        // (ls20RecoverEx adapts the count of rows)
                        if ( (STOP_NO_DATA_LS00 == rcINSERT) && (pDataPartCache->ab_pCache_ls00[lSndPcktNo].dpc_lRowCount_ls00 > 0) )
                        {
                            rcINSERT = STOP_NOMORE_DATA_LS00;
                        }
                    }
                }   // end of else of if (dberrParseAgain_els98 != rc)
            }   // end else if ( (errOK_els00 != rc) && (0 != lErrorPos) )

            // commit in any case and do not pay any attention to the return value
            ls20_DL_Commit(pDBInfo, lRcvPcktNo, pMetaDataDef, pTransformationCmd, pTransformationRTE,
                           lRowCntBeforeCommit, lCommitCount, ErrText);

            if (true == bParseAgain)
            {
                lSndPcktNo = lRcvPcktNo;        // we have to resend the data of the currently sent packet
            }
            else
            {
                if (false == bPutVal[lRcvPcktNo])
                {
                    lRcvPcktNo = (lRcvPcktNo + 1) % 2;
                }
                else
                {
                    bPutVal[lRcvPcktNo] = false;    // reinitialize
                    lSndPcktNo = lRcvPcktNo = 0;    // reinitialize; we start freshly as if we didn't do anything so far
                    continue;                       // Make sure we start with filling the packet buffer
                }
            }
        }   // end if (lSndPcktNo != lRcvPcktNo)


        //*
        //*     Send packet to kernel
        //*
        if ( (errOK_els00 == rc) && ( (errOK_els00 == rcINSERT) || (STOP_NOMORE_DATA_LS00 == rcINSERT) ) )
        {
            rc = ls20_FillPacketBuffer(pDBInfo, pTransformationCmd, pTransformationRTE, pDataPartCache, ErrText);
            if (errOK_els00 == rc)
            {
                rc = ls03SQLRequest(pDBInfo, lSndPcktNo, szErr);
                if (errOK_els00 == rc) {
                    lSndPcktNo = (lSndPcktNo + 1) % 2;  //  successful sent: recalculate packet number
                }
            }
            else
            {
                if  (STOP_NO_DATA_LS00 == rc) {
                    rc = errOK_els00;       // come from 'parse again' error recovery and have no more data to send
                }
            }

            if (STOP_NOMORE_DATA_LS00 == rcINSERT) {    // Stop processing if no errors occurred
                rcINSERT = errOK_els00;
            }
        }
    }   // end while ( errOK_els00 == rc && errOK_els00 == rcINSERT )

    // send again an commit (even if errors occurred): setting lRowCntBeforeCommit = 0 we force a commit
    ls20_DL_Commit(pDBInfo, lRcvPcktNo, pMetaDataDef, pTransformationCmd, pTransformationRTE, 0, lCommitCount, ErrText);

    // Check if we have overwritten rcINSERT
    if ( (errOK_els00 == rc) &&
         (errOK_els00 != rcINSERT) && (STOP_NOMORE_DATA_LS00 != rcINSERT) && (STOP_NO_DATA_LS00 != rcINSERT) )
    {
        rc = rcINSERT;
        
        SAPDB_memcpy(ErrText, pInsertErrorText.strAddr_ls00, pInsertErrorText.strLeng_ls00);
        ErrText[pInsertErrorText.strLeng_ls00] = 0;
    }
    if (NULL != pInsertErrorText.strAddr_ls00)
    {
        delete [] pInsertErrorText.strAddr_ls00;
    }

    return rc;
}
// ls20_LoadRowColumns()


/*
  -----------------------------------------------------------------------------
  function:     ls20_ReadMetaHeader
  -----------------------------------------------------------------------------
*/
// Fills/compares code type of data file
static SAPDB_Int4
ls20_ReadMetaHeader(tls00_DBInfo*             pDBInfo,
                    tls00_TransformationCmd*  pDatLoadCmd,
                    tls00_TransformationRTE*  pTransformationRTE,
                    tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20ReadMetaHeader");

    tsp00_Int4 rc = errOK_els00;

    tls00_VFile* pVFile = pTransformationRTE->rte_pDataFile_ls00;

    tls30_FileMetaDataHeader   FileMetaDataHeader;
    tls00_MetaData            *pMeta              = &pTransformationRTE->rte_MetaData_ls00;
    tsp00_SwapKind_Enum       *pSSwap             = &pMeta->mdSwapKind_ls00;
    tsp00_Uint4              *&pLongPos           = pMeta->pmdLongFieldPos_ls00;
    Tools_DynamicUTF8String    DatExtrStmt;

    // First read length of buffer from file
    tsp00_Versionc Version;
    SAPDB_Int4 rlen = 0;

    tsp00_Int4 buflen = Version.size() + sizeof(char) + sizeof(tsp00_Int4);

    tsp00_Addr pszBuffer = new _TCHAR[buflen];
    memset(pszBuffer, 0, buflen);
    tsp00_Addr pszKeepBufPtr = pszBuffer;

    rlen = ls30VFReadLnEx(*pVFile, pszBuffer, pszErrText, buflen);
    if (rlen <= 0)         // In case of error length corresponds to error number
    {
        rc = rlen;
    }
    else
    {
        bool bCP_Avail = false;     // The version defines if we can expect a code page mapping table
                                    // for mapping ASCII to UCS2
        // Read version info
        SAPDB_memcpy(REINTERPRET_CAST(char*, &Version), pszBuffer, Version.size());
        pszBuffer += Version.size();

        // Check version
        if ( ('7' == Version[sizeof(tsp100_CompName)]) && (Version[sizeof(tsp100_CompName) + 2]) >= '4' )
        {
            bCP_Avail = true;
        }

        tsp00_SwapKind_Enum DSwap = SERVER_SWAPPING_LS00;

        *pSSwap = STATIC_CAST(tsp00_SwapKind_Enum, *pszBuffer);

        TRACE_PRNF_MLS99(("ls20_ReadMetaHeader", "SSwap = %d", *pSSwap) );

        pszBuffer += sizeof(unsigned char);

        // Define header length for buffer to read; we have to take into account the fact
        // that the header is aligned to full kB written -> so we need to read at least
        // the full kB
        tsp00_Int4 hlen = 0;
        SAPDB_memcpy(&hlen, pszBuffer, sizeof(hlen));

        hlen = ls07Swap4BInt(hlen, *pSSwap, DSwap);

        hlen -= buflen;             // header length contains the length of preheader info, too

        if (hlen <= 0)      // Can't be
        {
            LoaderMessages::Instance()->ls98Msg(layDataLoad_els98, pszErrText, errInternal_els98,
                                                                               __FILE__, __LINE__,
                                                                               errCheckHeader_els98);
        }

        tsp00_Addr pszHeader = new _TCHAR[hlen];
        memset(pszHeader, 0, hlen);
        tsp00_Addr pszKeepHeaderPtr = pszHeader;

        rlen = ls30VFReadLnEx(*pVFile, pszHeader, pszErrText, hlen);
        if (rlen <= 0)  // EOF_LS00 = -1
        {
            rc = STATIC_CAST(tsp00_Int4, rlen);
            return rc;
        }
        else
        {
            if (true == bCP_Avail)
            {
                //*
                //*     We can expect a code page (at least)
                //*

                // First read the code type before reading the code page
                tls00_CodeType CodeType = STATIC_CAST(tls00_CodeType, *pszHeader);
                pszHeader += sizeof(char);

                if (ctUnknown_els00 == pDatLoadCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                {
                    if (ctUCS2Native_els00 == CodeType)
                    {
                        if (sw_full_swapped == *pSSwap)
                        {
                            pDatLoadCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUCS2Swapped_els00;
                        }
                        else
                        {
                            pDatLoadCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUCS2_els00;
                        }
                    }
                    else
                    {
                        pDatLoadCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = CodeType;
                    }
                }


                // Check if default code page was used upon extract (= ISO-8859-1)
                bool bDefaultCPUsed = (0 != *pszHeader);
                pszHeader += sizeof(char);

                // Now read the code page information
                tsp81_CodePage *pCP = pTransformationRTE->rte_DataSource_ls00.pdasCodePage_ls00;


                // There will be a code page name and a mapping table only in case the
                // code type of the file is ASCII and the code page is not ISO-8859-1
                if (ctAscii_els00 == CodeType)
                {
                    if (false == bDefaultCPUsed)
                    {
                        // Default code page was NOT used; so there should be code page
                        // information be found

                        // Code page name
                        pCP->name.Init();
                        pCP->name.rawAssign(pszHeader);
                        pszHeader += sizeof(tsp00_KnlIdentifierc);

                        // Code page UCS2 mapping
                        SAPDB_memcpy(pCP->map, pszHeader, sizeof(pCP->map));

                        if (*pSSwap != DSwap)
                            ls07SwapUCS2StringInPlace(REINTERPRET_CAST(char*, pCP->map), MAX_UINT1_SP00 + 1);

                        // Try to ensure there is really some code page mapping
                        if ( (0 == pCP->map[0]) && (1 == pCP->map[1]) )
                            pszHeader += sizeof(pCP->map);
                    }
                    else
                    {
                        // Default code page was used upon extract; we must make sure that our
                        // code page struct that's used for loading contains the default code
                        // page
                        pCP->name.Init();
                        pCP->name.rawAssign(pszDefaultCodePageName_ls00);

                        SAPDB_memcpy(pCP->map, lDefaultCodePageMap_ls00, 256*sizeof(tsp00_Uint2));
                    }
                }
            }

            SAPDB_memcpy(&FileMetaDataHeader.mdRecordLength_ls30, pszHeader, sizeof(FileMetaDataHeader.mdRecordLength_ls30));
            FileMetaDataHeader.mdRecordLength_ls30 = ls07Swap4BInt(FileMetaDataHeader.mdRecordLength_ls30, *pSSwap, DSwap);
            pszHeader += sizeof(tsp00_Uint4);

            SAPDB_memcpy(&FileMetaDataHeader.mdFieldCount_ls30, pszHeader, sizeof(FileMetaDataHeader.mdFieldCount_ls30));
            FileMetaDataHeader.mdFieldCount_ls30 = ls07Swap4BInt(FileMetaDataHeader.mdFieldCount_ls30, *pSSwap, DSwap);
            pszHeader += sizeof(tsp00_Uint4);

            SAPDB_memcpy(&FileMetaDataHeader.mdLongCount_ls30, pszHeader, sizeof(FileMetaDataHeader.mdLongCount_ls30));
            FileMetaDataHeader.mdLongCount_ls30 = ls07Swap4BInt(FileMetaDataHeader.mdLongCount_ls30, *pSSwap, DSwap);
            pszHeader += sizeof(tsp00_Uint4);

            SAPDB_memcpy(&FileMetaDataHeader.mdMetaData_ls30, pszHeader, sizeof(tls00_FileFormat));
            pszHeader += sizeof(tls00_FileFormat);

            SAPDB_memcpy(&FileMetaDataHeader.mdTableEmpty_ls30, pszHeader, sizeof(FileMetaDataHeader.mdTableEmpty_ls30));
            FileMetaDataHeader.mdTableEmpty_ls30 = ls07Swap4BInt(FileMetaDataHeader.mdTableEmpty_ls30, *pSSwap, DSwap);
            pszHeader += sizeof(tsp00_Uint4);

            if (0 != FileMetaDataHeader.mdLongCount_ls30)
            {
                // read long positions from DATA record behind read header
                pLongPos = new tsp00_Uint4[FileMetaDataHeader.mdLongCount_ls30];

                for (tsp00_Uint4 i = 0; i < FileMetaDataHeader.mdLongCount_ls30; ++i)
                {
                    SAPDB_memcpy(&pLongPos[i], pszHeader, sizeof(pLongPos[i]));
                    pLongPos[i] = ls07Swap4BInt(pLongPos[i], *pSSwap, DSwap);
                    pszHeader += sizeof(tsp00_Uint4);
                }
            }

            // read command from buffer
            DatExtrStmt.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pszHeader));

            if (NULL != pszKeepHeaderPtr)
            {
                delete [] pszKeepHeaderPtr;
                pszKeepHeaderPtr = NULL;
                pszHeader        = NULL;
            }
        }

        if (pszKeepBufPtr != NULL)
        {
            delete [] pszKeepBufPtr;
            pszKeepBufPtr = NULL;
            pszBuffer     = NULL;
        }
    }

    if (errOK_els00 == rc)
    {
        //*
        //* parse owner and tablename from command string in DATA File header (source) and compare those
        //* with owner and tablename to command string from CATALOG table
        //* This is done only for tables extracted with part type USER, SCHEMA or DB. It must
        //* be allowed to load the data of a single source table A into a target table B if the
        //* columns match (detected while parsing the INSERT statement).
        //*
        if (pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00 > ptTable_els00)
        {
            rc = ls20_CheckHeader(pDBInfo, pDatLoadCmd, DatExtrStmt, pszErrText);
        }
        if (errOK_els00 == rc)
        {
            pMeta->mdFieldCount_ls00     = FileMetaDataHeader.mdFieldCount_ls30;
            pMeta->mdLongFieldCount_ls00 = FileMetaDataHeader.mdLongCount_ls30;
            pMeta->mdRecordLength_ls00   = FileMetaDataHeader.mdRecordLength_ls30;

            pMeta->mdLengthArrayLen_ls00 = pMeta->mdFieldCount_ls00 + pMeta->mdLongFieldCount_ls00;
            pMeta->pmdLengthArray_ls00   = new tsp00_Int2[pMeta->mdLengthArrayLen_ls00];
            memset(pMeta->pmdLengthArray_ls00, 0, (sizeof(tsp00_Int2) * pMeta->mdLengthArrayLen_ls00));
            pMeta->pmdMetaFile_ls00      = pTransformationRTE->rte_pDataFile_ls00;

            FileMetaDataHeader.mdTableEmpty_ls30  == 1 ? pTransformationRTE->rte_TabEmpty_ls00 = true :
                                                         pTransformationRTE->rte_TabEmpty_ls00 = false;

            if ( true == pDatLoadCmd->trExclude_ls00 )
            {
                // These tables excluded are those that were already excluded during export !!!
                Loader_RTStatistics::Instance()->IncreaseCounter_TablesExcluded();
            }
        }
        else
            ;//!!ErrorHandling
    }

    return rc;
}
//  ls20_ReadMetaHeader()


/*
  -----------------------------------------------------------------------------
  function:     ls20_ReadMetaTrailer
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_ReadMetaTrailer(tls00_DBInfo*             pDBInfo,
                     TransformationDef*&       pTransformationDef,
                     tls00_TransformationCmd*  pTransformationCmd,
                     tls00_TransformationRTE*  pTransformationRTE,
                     Tools_DynamicUTF8String&  DatLoadStmt,
                     tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_ReadMetaTrailer");

    tsp00_Int4 rc = errOK_els00;

    tls00_VFile* pVFile = pTransformationRTE->rte_pDataFile_ls00;

    // read table end marker
    tsp00_Longint rlen    = 0;
    tsp00_Int4    buflen  = pTransformationRTE->rte_MetaData_ls00.mdLongFieldCount_ls00 * sizeof(tsp00_Int4) +
                           (pTransformationRTE->rte_MetaData_ls00.mdFieldCount_ls00 -
                            pTransformationRTE->rte_MetaData_ls00.mdLongFieldCount_ls00)* sizeof(tsp00_Int2);

    if( true == pTransformationRTE->rte_TabEmpty_ls00 || 
		true == pTransformationCmd->trExclude_ls00       ) 
	{
         tsp00_Addr pszNoMoreData;
         pszNoMoreData = new _TCHAR[buflen];
         memset(pszNoMoreData, 0, buflen);
         rlen = ls30VFReadLnEx(*pVFile, pszNoMoreData, ErrText, buflen);
         if ( pszNoMoreData != NULL )
         {
             delete [] pszNoMoreData;
             pszNoMoreData = NULL;
         }
		 // does not work for pipes !
		 //rc = ls30VFSeekLong(*pVFile, buflen, ErrText);
	}
    // trailer length table end | command | alignment
    tsp00_Int4 trailerlen = sizeof(tsp00_Int4);
    tsp00_Int4 hlen       = 0;

    rlen = ls30VFReadLnEx(*pVFile, REINTERPRET_CAST(char*, &hlen), ErrText, trailerlen);
    if (rlen <= 0)
    {
        rc = STATIC_CAST(tsp00_Int4, rlen);
    }
    else
    {
        hlen  = ls07Swap4BInt(hlen, pTransformationRTE->rte_MetaData_ls00.mdSwapKind_ls00, pTransformationRTE->rte_SwapKind_ls00);
        hlen -= (trailerlen + buflen);
        if (hlen <= 0)      // Can't be; shouldn't be
        {
            rc = errCheckTrailer_els98;
        }
        else
        {
            tsp00_Addr pszTrailer;
            pszTrailer = new _TCHAR[hlen];
            memset(pszTrailer, 0, hlen);

            // <TRAILER_BLOCK>:<TRAILER_POS>
            //pTransformationDef->m_trailerpos.lnu2GB_Blocks_ls00= VFile.vfFilePos_ls00.lnu2GB_Blocks_ls00;
            //pTransformationDef->m_trailerpos.lnuRemainder_ls00 = VFile.vfFilePos_ls00.lnuRemainder_ls00;

            rlen = ls30VFReadLnEx(*pVFile, pszTrailer, ErrText, hlen);
            if (rlen <= 0)
            {
                rc = STATIC_CAST(tsp00_Int4, rlen);
            }
            else
            {
                // transformation statement
                DatLoadStmt.AssignRaw(REINTERPRET_CAST(unsigned char*,pszTrailer), _tcslen(pszTrailer));
            }

            if (NULL != pszTrailer)
            {
                delete [] pszTrailer;
                pszTrailer = NULL;
            }
        }   // end else of if (hlen <= 0)
    }   // end else of if (rlen <= 0)

    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layDataLoad_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc);
        rc = errInternal_els98;
    }
    return rc;
}
// ls20_ReadMetaTrailer()


/*
  -----------------------------------------------------------------------------
  function:     ls20InitDataSourceStruct
  -----------------------------------------------------------------------------
*/
void
ls20InitDataSourceStruct(tls00_DataSource  *pDataSource,
                         tls00_FileFormat  *pFileFormat,
                         tsp00_Int4         lFieldCount,        // tabledescription->fieldcount_ls00
                         tls00_MultColSpec *pMultColSpec,
                         tsp00_Int4         lSetColumnIndex)    // applies only to DATAUPDATE command; defaults to 0
{
    ROUTINE_DBG_MEO00 ("ls20InitDataSourceStruct");

    tls00_Column *pCol = NULL;

    pDataSource->dasCodeType_ls00 = pFileFormat->ffoCodeType_ls00;

    pDataSource->dasInputBuffer.pBufPtr_ls00 = new char[pDataSource->dasInputBuffer.lBufLen_ls00];
    pDataSource->dasInputBuffer.lBufPos_ls00 = 0;

    ls18RecordConstructor(lFieldCount, pDataSource->dasConvertedData);

    pDataSource->lReadFieldsCnt_ls00 = lFieldCount;

    // We need only as much read positions as we have DATA fields to read
    pDataSource->lReadPos_ls00           = new tsp00_Uint4[lFieldCount];
    pDataSource->plDataToColIndex_ls00   = new tsp00_Uint4[lFieldCount];
    pDataSource->plDataToParamIndex_ls00 = new tsp00_Uint4[lFieldCount];

    tsp00_Int4 lSetColCount = lFieldCount - lSetColumnIndex;

    for (int j = 0, k=0; j < pMultColSpec->mcsCount_ls00; ++j)
    {
        pCol = pMultColSpec->mcsColumn_ls00[j];
        if (asUnknown_els00 == pCol->colAssign_ls00.fasAsgnTyp_ls00)
        {
            pDataSource->plDataToColIndex_ls00[k] = j;

            pDataSource->lReadPos_ls00[k] = (0 == pCol->colFPos_ls00.fpoStart_ls00) ?
                                            j + 1 :
                                            pCol->colFPos_ls00.fpoStart_ls00;
            if (0 != lSetColumnIndex)
            {
                if (k < lSetColumnIndex)
                {
                    pDataSource->plDataToParamIndex_ls00[k] = k + lSetColCount;
                }
                else
                {
                    pDataSource->plDataToParamIndex_ls00[k] = k - lSetColumnIndex;
                }
            }
            else
            {
                pDataSource->plDataToParamIndex_ls00[k] = k;
            }
            ++k;
        }
        else if (asSeqno_els00 == pCol->colAssign_ls00.fasAsgnTyp_ls00)     // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129727
        {
            pDataSource->plDataToColIndex_ls00[k] = j;
            
            // Hack: we don't want to read anything from data file for this type of assignment but we can't
            // make sure not to do so; so read always the first position as it MUST exist if a data file is given
            pDataSource->lReadPos_ls00[k] = 1;

            pDataSource->dasConvertedData[j].strLeng_ls00 = INSERT_ASSIGNED;
            if (0 != lSetColumnIndex)
            {
                if (k < lSetColumnIndex)
                {
                    pDataSource->plDataToParamIndex_ls00[k] = k + lSetColCount;
                }
                else
                {
                    pDataSource->plDataToParamIndex_ls00[k] = k - lSetColumnIndex;
                }
            }
            else
            {
                pDataSource->plDataToParamIndex_ls00[k] = k;
            }
            ++k;
        }
    }   // end for (int j = 0, k=0; j < pMultColSpec->mcsCount_ls00; ++j)

    return;
}
// ls20InitDataSourceStruct()


/*
  -----------------------------------------------------------------------------
  function:     ls20_InitializeOIPacket
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls20_InitializeOIPacket(tls00_DBInfo*  pDBInfo, tls00_TransformationRTE*  pTransformationRTE)
{
    ROUTINE_DBG_MEO00 ("ls20_InitializeOIPacket");

    tsp00_Int4 rc = errOK_els00;

    tsp1_packet  *pSqlPacket = pDBInfo->dbiPktSndList_ls00[0];
    tsp1_segment *pSqlSegm   = NULL;
    tsp1_part    *pSqlPart   = NULL;

    if (NULL != pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00.strAddr_ls00)
    {
        // Seems we got a "-8 = Parse again" error; build the packet header newly
        delete [] pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00.strAddr_ls00;
        pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00.strAddr_ls00 = NULL;
    }

    //*
    //*     Generate Execute Command
    //*
    //* 1.part: parse id
    //* 2.part: daten
    //* 3.part: result count
    //*
    rc = ls04FastInit(pSqlPacket, &pSqlSegm, &pSqlPart, pDBInfo);
    ls04SetMsgType(pSqlSegm, sp1m_execute);                      // Message Type EXECUTE'
    ls04SetMCFlag(pSqlSegm, true);                               // Mass insert
    pSqlPart->sp1p_part_kind().becomes(sp1pk_parsid);


    // Insert parse id into parse id part (do not write trailing NULL)
    rc = ls04BufToPart(pSqlPart, pTransformationRTE->rte_ParseId_ls00, MAX_PARSEID_LENGTH_LS00);
    if (errOK_els00 == rc)
    {
        rc = ls04FinishPart(pSqlPacket, pSqlPart);
    }

    // insert resultcount part
    if (errOK_els00 == rc)
    {
        rc = ls04InitPart(pSqlPacket, pSqlSegm, &pSqlPart, sp1pk_resultcount);
        if (errOK_els00 == rc)
        {
            rc = ls04BufToPart(pSqlPart,
                            REINTERPRET_CAST(tsp00_Addr, pTransformationRTE->rte_ResultCount_ls00),
                            sizeof(csp_rescnt_unknown));
            if (errOK_els00 == rc)
            {
                rc = ls04FinishPart(pSqlPacket, pSqlPart);
            }
        }
    }

    // insert DATA part
    if (errOK_els00 == rc)
    {
        pSqlPart = NULL;
        rc = ls04InitPart(pSqlPacket, pSqlSegm, &pSqlPart, sp1pk_data);
        if (errOK_els00 != rc)
        {
            // Set part attribute for the first time (always: partattr = firstpacket)
            ls04SetPartAttribute(pSqlPart, sp1pa_first_packet);
        }
    }


    tls00_String* pOIPacketInitBlock = &pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00;

    // The length of the packet header blocks computes as:
    // sizeof(packet_header) + sizeof(segment_header) + 
    // sizeof(part_header_of_parseid_part) + len_of_parseid_part +
    // sizeof(part_header_of_resultcount_part) + len_of_resultcount_part +
    // sizeof(part_header_of_data_part
    // or a bit simpler
    // length of a packet - available space (sp1p_buf_size) in data part
    pOIPacketInitBlock->strLeng_ls00 = pDBInfo->dbiPktSize_ls00 - pSqlPart->sp1p_buf_size();;

    //*
    //*     Allocate space for initial block in order interface packet that may be reused
    //*     for every single DATA packet sent to the kernel while inserting DATA for one single table
    //*
    pOIPacketInitBlock->strAddr_ls00 = new char[pOIPacketInitBlock->strLeng_ls00];
    SAPDB_memcpy(pOIPacketInitBlock->strAddr_ls00,
                    REINTERPRET_CAST(char*, &pSqlPacket->sp1_header),
                    pOIPacketInitBlock->strLeng_ls00);

    // Initialize working buffer with the size of the DATA part
    tls00_OI_DataPartCache* pCache = NULL;
    for (SAPDB_Int4 i = 0; i < pDBInfo->dbiPktCount_ls00; ++i)
    {
        pCache = &pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00[i];
        if (NULL == pCache->dpc_Buffer_ls00.pBufPtr_ls00)
        {
            pCache->dpc_Buffer_ls00.lBufSize_ls00 = pSqlPart->sp1p_buf_size();
            pCache->dpc_Buffer_ls00.lBufLen_ls00  = 0;
            pCache->dpc_Buffer_ls00.pBufPtr_ls00  = new unsigned char[pCache->dpc_Buffer_ls00.lBufSize_ls00];
        }
    }

    return rc;
}
// ls20_InitializeOIPacket()


/*
  -----------------------------------------------------------------------------
  function:     ls20ReadData
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20ReadData(tls00_VFile*      pInfile,
             tls00_FileFormat* pFileFormat,
             tls00_Pattern*    pPattern,
             tls00_MetaData*   pMeta,
             tls00_Buffer2*    pBuffer,
             SAPDB_Bool*       pbTabLoaded,
             SAPDB_Bool        bFirstRecord,
             tsp00_Addr        pszErrText)

{
    ROUTINE_DBG_MEO00 ("ls20ReadData");

    tsp00_Int4 rc = errOK_els00;

    tsp00_Int4          rlen   = 0;
    tsp00_SwapKind_Enum DSwap  = ls07DefineSwap(NULL);

    // In case the table data is completely read return
    if (true == *pbTabLoaded)
    {
        return STOP_NOMORE_DATA_LS00;
    }

    if (true == pFileFormat->ffoCompact_ls00)
    {
        // In case of file format RECORDS the DATA file has the following structure where
        // the long DATA is only optional if the table has long columns
        //|------------------------------------------------------------------------------------ ...
        //| meta data1 | data1 | [long data1] | meta data2 | data2 | [long data2] | meta data3 | ...
        //|------------------------------------------------------------------------------------...
        //
        // To minimize the file access we use the following strategy for tables with no
        // long columns:
        // During first read of the DATA (indicated by recordcount = 0) we read the first
        // meta data1 and compute the length of the corresponding data1. Then we read
        // immediately afterwards this data1 because we know the length and the next meta data2 at once.
        // The length of the meta DATA is fix for the whole file. The meta data2 is kept
        // then until this function is entered next time. Then we compute the length of data2
        // and read data2 and the next meta data3 at once and so on.
        //
        // For tables with long columns we first read the meta data1 and compute the length
        // of the data1. Then we read only data1 and process it. Thus in this case meta DATA
        // and DATA is read separately.


        // check for last DATA record; in this case do not read meta DATA of next record either;
        // bLastTimeRead can only be defined for 'dataload all/user records' because here we have
        // the information about the number of rows to be inserted

        // compute meta DATA length
        rlen = pMeta->mdLengthArrayLen_ls00 * sizeof(tsp00_Uint2);

        if ( (true == bFirstRecord) || (pMeta->mdLongFieldCount_ls00 > 0) )
        {
            // Check if we read at the right position
            if ( (false == bFirstRecord) &&
                 ( (pMeta->pmdMetaFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00 != pMeta->mdPosOfNextLengthArray_ls00.lnu2GB_Blocks_ls00) ||
                   (pMeta->pmdMetaFile_ls00->vfFilePos_ls00.lnuRemainder_ls00 != pMeta->mdPosOfNextLengthArray_ls00.lnuRemainder_ls00) ) )
            {
                ls30VFSeekLong(*pMeta->pmdMetaFile_ls00, &pMeta->mdPosOfNextLengthArray_ls00, pszErrText);
            }

            // read meta information
            tsp00_Int4 len = ls30VFReadLnEx(*pMeta->pmdMetaFile_ls00, REINTERPRET_CAST(_TCHAR*, pMeta->pmdLengthArray_ls00), pszErrText, rlen);  //pMeta->mdLengthArrayLen_ls00 * sizeof(tsp00_Uint2));

            len >= 0 ?  rc = errOK_els00 : rc = len ;
        }
        else
        {
            // save meta information
            SAPDB_memcpy(pMeta->pmdLengthArray_ls00, &(STATIC_CAST(char*, pBuffer->pBufPtr_ls00)[pMeta->mdRecordLength_ls00]), rlen);
        }

        if (errOK_els00 == rc)
        {
            ls20_ReadDataLength(pMeta, DSwap, pszErrText);

            if ( pMeta->pmdLengthArray_ls00[0] == STOP_NOMORE_DATA_LS00  )
            {
                if ( errOK_els00 == rc )
                {
                    *pbTabLoaded  = true;
                    rc  = STOP_NOMORE_DATA_LS00;
                }
            }
            else
            {
                if  (pMeta->mdLongFieldCount_ls00 > 0)
                {
                    rlen  = pMeta->mdRecordLength_ls00;
                }
                else
                {
                    rlen += pMeta->mdRecordLength_ls00;
                }
            }
        }
    } // if (true == pFileFormat->ffoCompact_ls00)
    else
    {
        rlen = pFileFormat->ffoBytesPerLine_ls00;
    }


    // Read the real DATA. In case of compact format we read the meta information of
    // the next record at this time, too.
    if ( (errOK_els00 == rc) || (STOP_LAST_DATA_LS00 == rc) )
    {
        SAPDB_Bool bNLInData = SAPDB_FALSE;
        SAPDB_Int4 len = 0;

        pBuffer->lBufPos_ls00 = 0;      // initialize
        
        do
        {
            len = ls30VFReadLnEx(*pInfile,       // read one row in DATA file
                                 &(STATIC_CAST(char*, pBuffer->pBufPtr_ls00)[pBuffer->lBufPos_ls00]),
                                 pszErrText,
                                 rlen,
                                 pBuffer->lBufLen_ls00 - pBuffer->lBufPos_ls00);
            if (0 > len)
            {
                if ( EOF_LS00 == len )
                {
                    rc  = STOP_NOMORE_DATA_LS00;    // InputLen = EOF_LS00 (= -1)
				    *pbTabLoaded  = true;
                }
                else
                {
                    rc = len;                       // InputLen = errFileRead_els98 in case of read error
                }
            }
            else
            {
                if (true == pFileFormat->ffoCompact_ls00)
                {
                    if (len < rlen)
                    {
                        // Coming here we could not read the DATA and the next meta DATA as
                        // requested. This can only mean that the file is fully read. We format
                        // the length array buffer area to 0 to force a EOF_LS00 answer when
                        // executing the read the next time.
                        memset(&(STATIC_CAST(char*, pBuffer->pBufPtr_ls00)[len]),
                               0,
                               pMeta->mdLengthArrayLen_ls00 * sizeof(tsp00_Uint2));
                        rc = STOP_LAST_DATA_LS00;
                    }
                }
                else
                {
                    pBuffer->lBufPos_ls00 += len;

                    // It's not the proprietary format: so try to find out if there is some
                    // newline in the DATA. If so read the next line and do the same until
                    // we find the closing newline character.
                    // Exclude reading binary and the FORMATTED format. In the latter case we
                    // can not with absolute certainty decide
                    // whether the row is simply that short or the DATA contains new line characters.
                    if ( (true == pFileFormat->ffoCompress_ls00) &&
                         (false == pFileFormat->ffoBinary_ls00)    &&
                         (0     != *pFileFormat->ffoDelimit_ls00)   )
                    {
                        // The given pBuffer has a initial length of 32 kByte. We have to check this
                        // to prevent buffer overflows
                        bNLInData = ls18CheckForNLInData(pBuffer, pPattern, pFileFormat);
                    }
                }
            }   // end else of if (0 > len)
        }
        while ( (SAPDB_TRUE == bNLInData) && (errOK_els00 == rc) );
    }   // end if (errOK_els00 == rc)

    return rc;
}
// ls20ReadData()


/*
  -----------------------------------------------------------------------------
  function:     ls20_ReadDataLength
  -----------------------------------------------------------------------------
*/
static void
ls20_ReadDataLength(tls00_MetaData *pMeta, tsp00_SwapKind_Enum DSwap, tsp00_Addr pszErrText)
{
    //  This function is used to transform the length of DATA fields in case of
    //  file format RECORDS to the right swapped values and to compute the record length.
    //  The DATA is structured as follows
    //|------------------------------------------------------------------------------------ ...
    //| meta data1 | data1 | [long data1] | meta data2 | data2 | [long data2] | meta data3 | ...
    //|------------------------------------------------------------------------------------...
    //
    // meta data1 contains the length of all data1 and long data1 fields. Every DATA field
    // is characterized by a 2 byte integer length whereas every long DATA field is
    // characterized by a 4 byte integer. NULL values are indicated by a length of -12.
    // All length fields are signed integers.
    // The length values are swapped in the current machine swap.
    //
    // This function computes the record length, too. Because of negative length of NULL values
    // this length is not added to the record length. Additionally the length of long
    // values is also not added to the record length because longs are handled separately.
    ROUTINE_DBG_MEO00 ("ls20_ReadDataLength");

    pMeta->mdRecordLength_ls00 = 0;
    if (pMeta->mdLongFieldCount_ls00 > 0)
    {
        // First part (easy): set position to file position AFTER the length array
        // because this has already been read coming here
        pMeta->mdPosOfNextLengthArray_ls00.lnu2GB_Blocks_ls00 = pMeta->pmdMetaFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00;
        pMeta->mdPosOfNextLengthArray_ls00.lnuRemainder_ls00  = pMeta->pmdMetaFile_ls00->vfFilePos_ls00.lnuRemainder_ls00;
    }

    tsp00_Uint4 i       = 0;
    tsp00_Uint4 j       = 0;
    tsp00_Int4  LongLen = 0;
    bool        bLong   = false;
    for (i,j; i < pMeta->mdFieldCount_ls00; ++i)
    {
        if (j < pMeta->mdLongFieldCount_ls00)
        {
            if ((i+1) == pMeta->pmdLongFieldPos_ls00[j])
            {
                SAPDB_memcpy(&LongLen, &(pMeta->pmdLengthArray_ls00[i+j]), sizeof(tsp00_Int4));
                if (DSwap != pMeta->mdSwapKind_ls00)
                {
                    ls07Swap4BCharInPlace(REINTERPRET_CAST(tsp00_Addr, &LongLen), pMeta->mdSwapKind_ls00, DSwap);
                    SAPDB_memcpy(&(pMeta->pmdLengthArray_ls00[i+j]), &LongLen, sizeof(tsp00_Int4));
                }
                TRACE_PRNF_MLS99(("ls20_ReadDataLength", "Long value length: %d", LongLen));

                // Second part: add length of this long to position of next record
                if (INSERT_NULL != LongLen)
                {
                    ls07AddLargeNumbers(pMeta->mdPosOfNextLengthArray_ls00, LongLen);
                }
                bLong = true;
            }
        }

        if (false == bLong)
        {
            if (DSwap != pMeta->mdSwapKind_ls00)
            {
                pMeta->pmdLengthArray_ls00[i+j] = ls07Swap2BInt(pMeta->pmdLengthArray_ls00[i+j],
                                                                pMeta->mdSwapKind_ls00, DSwap);
            }
            if (INSERT_NULL != pMeta->pmdLengthArray_ls00[i+j])     // NULL-value; do not add length to record length
            {
                pMeta->mdRecordLength_ls00 += pMeta->pmdLengthArray_ls00[i+j];
            }
            TRACE_PRNF_MLS99(("ls20_ReadDataLength", "vallen=%d", pMeta->pmdLengthArray_ls00[i+j]) );
        }
        else
        {
            bLong = false;
            ++j;
        }
    } //while (i < pMeta->mdLengthArrayLen_ls00);

    // Third part: add length of this record without LONGs (these have already been added) to position of next record
    ls07AddLargeNumbers(pMeta->mdPosOfNextLengthArray_ls00, pMeta->mdRecordLength_ls00);

    return;
}
// ls20_ReadDataLength()


/*
  -----------------------------------------------------------------------------
  function:     ls20_BuildRecord
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls20_BuildRecord(tls00_TransformationCmd*  pTransformationCmd,
                 tls00_TransformationRTE*  pTransformationRTE,
                 tls00_Buffer*             pDataPartBuffer,
                 bool                      bUnicodeOI,
                 tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_BuildRecord");

    tls00_DataSource*        pDataSource    = &pTransformationRTE->rte_DataSource_ls00;
    tls00_TableDescription*  pTableDesc     = &pTransformationRTE->rte_TableInfo_ls00;
    tls00_LongExecInfo*      pLongExecInfo  = &pTransformationRTE->rte_LongExecInfo_ls00;
    

    tsp00_Longint     rc         = errOK_els00;
    tsp00_Int4        lResult    = 0;
    unsigned char*    pColValue  = NULL;
    tls00_Column*     pColumn    = NULL;
    tls00_ParamInfo*  pPI        = NULL;
    tls00_String*     RawData    = pDataSource->dasConvertedData;
    unsigned char*    pszBuffer  = &(STATIC_CAST(unsigned char*, pDataPartBuffer->pBufPtr_ls00)[pDataPartBuffer->lBufLen_ls00]);

    for(tsp00_Int4 i=0; (i < pTableDesc->tdFieldCount_ls00) && (rc >= 0); ++i)
    {
        pColumn = pTransformationCmd->trColSpec_ls00.mcsColumn_ls00[pDataSource->plDataToColIndex_ls00[i]];

        // Even if loop index i corresponds exactly plDataToParamIndex_ls00[i] for DATALOAD
        // commands we need it here as we use this function for DATAUPDATE also; for DATAUPDATE
        // both indexes are not the same
        pPI       = pTableDesc->tdParamArray_ls00[pDataSource->plDataToParamIndex_ls00[i]];

        // Points to undef-byte before bufpos for value to build
        pColValue = pszBuffer + (pPI->piBufpos_ls00 - 1);

        lResult = ls20_ValueToOIBuffer(RawData[i],
                                       *pPI,
                                       pColumn,
                                       pDataSource->dasCodeType_ls00,
                                       pDataSource->pdasCodePage_ls00,
                                       bUnicodeOI,
                                       pColValue,
                                       pszErrText);
        if (lResult >= 0)
        {
            // Additional check/work for LONGs: if NOT NULL increase counter
            if ( (1           == szMap_LongDataType_ls00[pPI->piBasicInfo_ls00.fbDataType_ls00]) &&
                 (INSERT_NULL != RawData[i].strLeng_ls00) )
            {
                pLongExecInfo->dliNNLongs_ls00[pLongExecInfo->dliCntNNLongs_ls00] = i;
                ++pLongExecInfo->dliCntNNLongs_ls00;
            }
        }
        else    // OOOPs, error --> adapt return value, too!
        {
            LoaderMessages *pMsgObj = LoaderMessages::Instance();

            pMsgObj->ls98Msg(layDataLoad_els98, pszErrText, errAtRow_els98, ls20_ComputeErroneousRowNo(
                                                                             pTransformationCmd,
                                                                             pTransformationRTE->rte_Cnt_ProcessedRecords_ls00));
            if (lResult == errInvalidNumber_els98)     // This type of error can be generated only for sequence number assignments!
            {
                SAPDB_Char _pAux[16];
                _itot(pColumn->colAssign_ls00.fasSeqNo_ls00, _pAux, 10);

                pMsgObj->ls98Msg(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers, lResult), _tcslen(_pAux), _pAux);
            }
            else
            {
                pMsgObj->ls98Msg(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers, lResult),
                                                                pTableDesc->tdColName_ls00[i]->asCharp());
            }
            rc = lResult;
        }
    }   // end for(tsp00_Int4 i=0; (i < pTableDesc->tdFieldCount_ls00) && (rc >= 0); ++i)

    return rc;
}
// ls20_BuildRecord()


/*!
  -----------------------------------------------------------------------------
  function:     ls20_ValueToOIBuffer
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_ValueToOIBuffer(tls00_String        &pInputData,            // = RawData
                     tls00_ParamInfo     &ParameterInfo,
                     tls00_Column        *pColumn,
                     tls00_CodeType       DataFileEncoding,
                     tsp81_CodePage      *pCodePageTable,
                     bool                 bUnicodeOI,
                     unsigned char       *pOIValueBuffer,
                     tsp00_Addr           pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_ValueToOIBuffer");

    tsp00_Longint  rc       = 0;
    tsp00_Int4     lResult  = 0;
    tsp00_Longuint lDestLen = 0;

    tls00_FieldBasics   BasicInfo = ParameterInfo.piBasicInfo_ls00;

    switch(pInputData.strLeng_ls00)
    {
        case INSERT_ASSIGNED:           // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129727
        {
            *pOIValueBuffer = ls07GetDefByte(BasicInfo.fbDataType_ls00);     // Insert defined-byte
            lResult = ls24InsertAssignedVal(pColumn, BasicInfo, reinterpret_cast<SAPDB_Char*>(&pOIValueBuffer[sizeof(SAPDB_Char)]), 0);
            break;
        }
        case INSERT_DEFAULT:
        {
            *pOIValueBuffer = csp_default_byte;
            break;
        }
        case INSERT_NULL:
        {
            *pOIValueBuffer = csp_undef_byte;
            break;
        }
        case INSERT_EMPTY_VALUE:
        {
            *pOIValueBuffer = ls07GetDefByte(BasicInfo.fbDataType_ls00);     // Insert defined-byte
            if (1 == szMap_LongDataType_ls00[BasicInfo.fbDataType_ls00])
            {
                // Initialize Longdescriptor to vm_nodata
                tsp00_LongDescriptor LongDescriptor;
                ls08InitLONGDescriptor(&LongDescriptor);
                SAPDB_memcpy((pOIValueBuffer + 1), &LongDescriptor, LongDescSize_ls00);
            }
            else
            {
                // length parameter counts the defined byte
                ls05InsertFillBytes(pOIValueBuffer,
                                    lResult + sizeof(char),
                                    BasicInfo.fbDataType_ls00,
                                    BasicInfo.fbInOutLength_ls00,
                                    bUnicodeOI,
                                    SERVER_SWAPPING_LS00);
            }
            break;
        }
        default:
        {
            *pOIValueBuffer = ls07GetDefByte(BasicInfo.fbDataType_ls00);     // Insert defined-byte

            if (1 == szMap_LongDataType_ls00[BasicInfo.fbDataType_ls00])
            {
                // Initialize Longdescriptor to vm_nodata
                tsp00_LongDescriptor LongDescriptor;
                ls08InitLONGDescriptor(&LongDescriptor);
                SAPDB_memcpy((pOIValueBuffer + 1), &LongDescriptor, LongDescSize_ls00);
            }
            else
            {
                if ( (true == bUnicodeOI) && (true == bUnicodeOverOI_ls00[BasicInfo.fbDataType_ls00]) )
                {
                    // coming here only char DATA types, blobs or date, time and timestamp arrive
                    // char must not be converted if the DataFileEncoding is already UCS2
                    //
                    // DATE, TIME and TIMESTAMP must always be converted because the internal routines
                    // to convert the representation to ISO, ... work only with ascii DATA;
                    // But this conversion needs only code page ISO8859-1 - ever. So don't give any
                    // code page info to the conversion routine.
                    if (1 == szMap_SpecConstDataType_ls00[BasicInfo.fbDataType_ls00])
                    {
                        lDestLen = BasicInfo.fbInOutLength_ls00;

                        rc = ls05ConvertToUCS2Simple(pOIValueBuffer + 1,
                                                     lDestLen,
                                                     REINTERPRET_CAST(unsigned char*, pInputData.strAddr_ls00),
                                                     pInputData.strLeng_ls00,
                                                     ctAscii_els00,
                                                     pszErrText);
                        if (errOK_els00 == rc)
                        {
                            lResult = STATIC_CAST(tsp00_Int4, lDestLen);
                        }
                    }
                    else
                    {
                        // char DATA already in UCS2 must not be converted again, but may be swapped;
                        // char DATA UTF8 or ASCII encoded must be converted here
                        if ( false == ls00IsUCS2(&DataFileEncoding) )
                        {
                            lDestLen = BasicInfo.fbInOutLength_ls00;

                            rc = ls05ConvertToUCS2(pOIValueBuffer + 1,
                                                   lDestLen,
                                                   REINTERPRET_CAST(unsigned char*, pInputData.strAddr_ls00),
                                                   pInputData.strLeng_ls00,
                                                   DataFileEncoding,
                                                   BasicInfo.fbDataType_ls00,
                                                   pszErrText,
                                                   pCodePageTable);
                            if (errOK_els00 == rc)
                            {
                                lResult = STATIC_CAST(tsp00_Int4, lDestLen);
                            }
                        }
                        else
                        {
                            SAPDB_memcpy(pOIValueBuffer + 1, pInputData.strAddr_ls00, pInputData.strLeng_ls00);
                            lResult = pInputData.strLeng_ls00;
                            
                            if ( ( (sw_full_swapped == SERVER_SWAPPING_LS00) && (ctUCS2_els00 == DataFileEncoding) )
                                 ||
                                 ( (sw_normal == SERVER_SWAPPING_LS00)       && (ctUCS2Swapped_els00 == DataFileEncoding) ) )
                            {
                                ls07SwapUCS2StringInPlace(REINTERPRET_CAST(char*, (pOIValueBuffer + 1)), pInputData.strLeng_ls00);
                            }
                        }
                    }

                }   // end if ( (true == bUnicodeOI) && (true == bUnicodeOverOI_ls00[BasicInfo.fbDataType_ls00]) )
                else
                {
                    SAPDB_memcpy(pOIValueBuffer + 1, pInputData.strAddr_ls00, pInputData.strLeng_ls00);
                    lResult = pInputData.strLeng_ls00;
                }

                if (lResult >= 0)
                {
                    // length parameter counts the defined byte
                    ls05InsertFillBytes(pOIValueBuffer,
                                        lResult + sizeof(char),
                                        BasicInfo.fbDataType_ls00,
                                        BasicInfo.fbInOutLength_ls00,
                                        bUnicodeOI,
                                        SERVER_SWAPPING_LS00);
                }
            }
            break;
        }
    } // End OF SWITCH RAWDATA TYPE

    return lResult;
}
// ls20_ValueToOIBuffer()


/*!
  -----------------------------------------------------------------------------
  function:     ls20ConvertValue2InternEncoding
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls20ConvertValue2InternEncoding(tls00_String        &RawData,
                                tsp00_DataType_Enum  DataType,
                                tls00_CodeType       CodeType,
                                tsp00_Addr           pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20ConvertValue2InternEncoding");

    tsp00_Longint rc = 0;

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
    //*  - blobs   - delivered in readable hex notation
    //*  - long    - DATA file contains meta information on real long values located
    //*              in long files; this meta information has to be converted; the real
    //*              longs are gotten later when processing the descriptor
    //* No transformation for values of all other types because
    //*  - char/varchar - transformed later with regards to the external representation
    bool bConversionNecessary = true;
    if (1 == szMap_CharDataType_ls00[DataType])
    {
         if (0 == szMap_BlobDataType_ls00[DataType])
         {
             bConversionNecessary = false;
         }
         else
         {
             if (ctUTF8_els00 == CodeType)
             {
                 // In case of UTF8 BLOBs (i.e. readable hex notation) does not need to
                 // be converted because it is in any case US7ASCII
                 bConversionNecessary = false;
             }
         }
    }   // end if (1 == szMap_LongDataType_ls00[DataType])


    if (true == bConversionNecessary)
    {
        if (ctUTF8_els00 == CodeType)
        {
            tsp00_Longuint len = RawData.strLeng_ls00;

            rc = ls05ConvertFromUTF8(REINTERPRET_CAST(unsigned char*, RawData.strAddr_ls00),
                                     len,
                                     REINTERPRET_CAST(unsigned char*, RawData.strAddr_ls00),
                                     RawData.strLeng_ls00,
                                     ctAscii_els00,
                                     pszErrText);
            if (errOK_els00 == rc)
            {
                RawData.strLeng_ls00 = STATIC_CAST(tsp00_Int4, len);
            }
        }
        else if (true == ls00IsUCS2(&CodeType))
        {
            tsp00_Longint lConversionBufLen = RawData.strLeng_ls00;

            rc = ls05ConvertFromUCS2Simple(REINTERPRET_CAST(unsigned char*, RawData.strAddr_ls00),
                                           REINTERPRET_CAST(unsigned char*, RawData.strAddr_ls00),
                                           lConversionBufLen,
                                           RawData.strLeng_ls00,
                                           ctAscii_els00,
                                           CodeType,
                                           pszErrText);
            if (errOK_els00 == rc)
            {
                RawData.strLeng_ls00 = STATIC_CAST(tsp00_Int4, lConversionBufLen);
            }
        }
    }   // end if (true == bConversionNecessary)

    return rc;
}
// ls20ConvertValue2InternEncoding()

/*
  -----------------------------------------------------------------------------
  function:     ls20_TransformColumnValue
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls20_TransformColumnValue(tls00_String        *pRawValue,
                          tls00_Column        *pColumn,
                          tls00_Buffer2       *pInputBuffer,
                          tls00_ParamInfo     *pParamInfo,
                          tls00_FileSpec      *pFileSpec,
                          tsp00_SwapKind_Enum  SwapKind,
                          tsp1_sqlmode_Enum    SQLMode,
                          tls00_Pattern*       pPattern,
                          tsp00_Addr           pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_TransformColumnValue");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // error reporting obj
    tsp00_Longint    rc      = errOK_els00;

    tls00_FileFormat *pFileFormat = &pFileSpec->fsFormat_ls00;
    tls00_String     tmpStr;

    if ( (INSERT_EMPTY_VALUE != pRawValue->strLeng_ls00) &&
         (INSERT_NULL        != pRawValue->strLeng_ls00) &&
         (INSERT_ASSIGNED    != pRawValue->strLeng_ls00) )
    {
        if ( (true         == pFileFormat->ffoCompress_ls00) &&
             (ioChar_els00 == pColumn->colFormat_ls00.ffoFormat_ls00) &&
             (1            == szMap_CharDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) &&
             (1            != szMap_BlobDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00])  )
        {
            ls18UndoubleDelimitersInCharData(pRawValue, pPattern, pFileFormat->ffoCodeType_ls00);
        }

        //*
        //*     Trim values right - remove trailing blanks
        //* Done for LONG DATA that is of external DATA type CHAR in case of format COMPRESSED and FORMATTED
        //* these values in the DATA file are simply positions specs and can be trimmed
        //*
        if ( (false == pFileFormat->ffoCompact_ls00) && (ioChar_els00 == pColumn->colFormat_ls00.ffoFormat_ls00) )
        {
            if ( (true == pFileFormat->ffoFormatted_ls00) ||    //http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1118955
                 ( (true == pFileFormat->ffoCompress_ls00)   &&
                   (1    == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) ) )
            {
                tsp00_Longint len  = pRawValue->strLeng_ls00;
                // Handle byte values in files with FORMATTED BINARY differently from not binary
                // formatted files
                if ( (true == pFileFormat->ffoFormatted_ls00) && (true == pFileFormat->ffoBinary_ls00) &&
                     (1    == szMap_BlobDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) &&
                     (0    == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) &&
                     (false == pColumn->colFormat_ls00.ffoIsHex_ls00) )
                {
                    ls05StrTrimRight(pRawValue->strAddr_ls00, len, '\0', pFileFormat->ffoCodeType_ls00);
                }
                else
                {
                    ls05StrTrimRight(pRawValue->strAddr_ls00, len, BLANK_LS00, pFileFormat->ffoCodeType_ls00);
                }

                // In case of format FORMATTED trimming prevents from errors where the DATA field exceeds
                // the allowed column length but only because it is padded with blanks;
                // but if the DATA consists entirely of blanks we'll retain it here
                if (len > 0)        // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124555
                {
                    pRawValue->strLeng_ls00 = STATIC_CAST(tsp00_Int4, len);
                }
            }
        }   // end if ( (false == pFileFormat->ffoCompact_ls00) && (ioChar_els00 == pColumn->colFormat_ls00.ffoFormat_ls00) )

        // Check for NULL DEFINITION
        // NULL condition may be checked only for not COMPACT formats. In case of COMPACT format
        // the NULL value is already given with the read DATA - here the length has the
        // value -12 = INSERT_NULL
        // A NULL condition is specified in the load command if the null condition value is not NULL itself
        if ( (errOK_els00 == rc) && (NULL != pColumn->colNullCond_ls00) && (false == pFileFormat->ffoCompact_ls00) )
        {
            tmpStr.strAddr_ls00 = STATIC_CAST(char*, pInputBuffer->pBufPtr_ls00);
            tmpStr.strLeng_ls00 = pInputBuffer->lBufPos_ls00;
            rc = ls24GetNILConstants(*pRawValue,
                                     tmpStr,
                                     pParamInfo->piMode_ls00,
                                     *pColumn->colNullCond_ls00,
                                     *pFileFormat);
            if (errNullNotAllowed_els98 == rc)
            {
                pMsgObj->ls98MsgToString(layDataLoad_els98, pszErrText,
                                                            STATIC_CAST(ErrorNumbers,rc),
                                                            pColumn->colName_ls00.asCharp());
            }
            // In case the NULL-check went without problems we have 2 cases to distinguish:
            // 1. there is a null-value --> leave the function (pRawValue->strLeng_ls00 is set to INSERT_NULL)
            // 2. there is a real value --> continue processing value
        }

        if ( (errOK_els00 == rc) && (INSERT_NULL != pRawValue->strLeng_ls00) )
        {
            //*
            //*     Convert values to internal encoding (ASCII) for further processing
            //*
            if ( (ctAscii_els00 != pFileFormat->ffoCodeType_ls00 )         &&
                (ioChar_els00  == pColumn->colFormat_ls00.ffoFormat_ls00) &&
                ((true          != pFileFormat->ffoCompact_ls00  )         ||
                ((true          == pFileFormat->ffoCompact_ls00  )         &&
                (0             == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00] )) ))
            {
                rc = ls20ConvertValue2InternEncoding(*pRawValue,
                                                     pParamInfo->piBasicInfo_ls00.fbDataType_ls00,
                                                     pFileFormat->ffoCodeType_ls00,
                                                     pszErrText);
            }

            //* Check for:
            //*  - hex representation of DATA -> convert it; exclude LONG specs in DATA file because
            //*    those are only meta DATA to the real long values in the long files which are
            //*    handled later
            //*  - NULL value representation
            //*  - LONGs
            //*  - specials like DATE, TIME, TIMESTAMP, BOOLEAN
            //*  - NUMBER (convert to internal db format)

            // (DATA in HEX form) or (BLOB DATA type and file format is not binary)?

            if (errOK_els00 == rc)
            {
                if ( ( (true == pColumn->colFormat_ls00.ffoIsHex_ls00) &&
                    (0 == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) )
                    ||
                    ( (1 == szMap_BlobDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) &&
                    (0 == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) &&
                    (false == pFileFormat->ffoBinary_ls00) ) )
                {                                                       // -->  convert to byte
                    tsp00_Addr  pNewValue = NULL;
                    tsp00_Int4  lNewValueLen = 0;

                    rc = ls07ConvertHexToByte(pRawValue->strAddr_ls00, pRawValue->strLeng_ls00,
                                            &pNewValue,              lNewValueLen);
                    if (errOK_els00 != rc)
                    {
                        pMsgObj->ls98MsgToString(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers,rc),
                                                                                pRawValue->strLeng_ls00,
                                                                                pRawValue->strAddr_ls00);
                    }
                    else
                    {
                        pRawValue->strAddr_ls00 = pNewValue;
                        pRawValue->strLeng_ls00 = lNewValueLen;
                    }
                }
            }

            // Process LONG value?
            if (1 == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00])
            {
                TRACE_PRNF_MLS99(("ls20_TransformColumnValue", "Long value found"));

                // There need to be DATA
                if ( (pRawValue->strLeng_ls00 >= 0) && (false == pFileFormat->ffoCompact_ls00) )
                {
                    rc = ls08ReadLongFileInfo(pColumn, pParamInfo, pRawValue, pszErrText);
                    if (errOK_els00 != rc)
                    {
                        if( (errFileClose_els98 != rc) && (errFileOpen_els98 != rc) && (errFileRead_els98 != rc) )
                        {
                            pMsgObj->ls98MsgToString(layDataLoad_els98, pszErrText, errLongfileSpecMissing_els98);
                        }
                    }
                }
            }
            else
            {
                if (1 == szMap_NumberDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00])
                {                                                       // --> convert to internal db format
                    rc = ls25PrepareNumbers(&pRawValue->strAddr_ls00,
                                            pRawValue->strLeng_ls00,
                                            *pColumn,
                                            pParamInfo->piBasicInfo_ls00,
                                            SwapKind,
                                            *pFileFormat);
                    if (errOK_els00 != rc)
                    {
                        if (errNumberTruncated_els98 != rc)
                        {
                            pMsgObj->ls98MsgToString(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                                    pRawValue->strLeng_ls00,
                                                                                    pRawValue->strAddr_ls00);
                        }
                    }
                }
                else if (1 == szMap_SpecConstDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00])
                {

                    rc = ls24SpecialFormat(*pRawValue, pParamInfo->piBasicInfo_ls00, *pFileFormat);
                    if (errOK_els00 != rc)
                    {
                        pMsgObj->ls98MsgToString(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                                pRawValue->strLeng_ls00,
                                                                                pRawValue->strAddr_ls00);
                    }
                }
            }   // end else branch if (1 == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00])

        }   // end if ( (errOK_els00 == rc) && (INSERT_NULL != pRawValue->strLeng_ls00) )

    }   // end if ( (INSERT_EMPTY_VALUE != pRawValue->strLeng_ls00) && ...
    else if (INSERT_EMPTY_VALUE == pRawValue->strLeng_ls00)
    {
        // Coming here there is either no DATA found in DATA file for specified column or the value is
        // the empty value;
        // check if column has null condition (even an empty string may be defined
        // to represent the NULL value)
        if (NULL != pColumn->colNullCond_ls00)
        {
            tmpStr.strAddr_ls00 = STATIC_CAST(char*, pInputBuffer->pBufPtr_ls00);
            tmpStr.strLeng_ls00 = pInputBuffer->lBufPos_ls00;
            rc = ls24GetNILConstants(*pRawValue,
                                     tmpStr,
                                     pParamInfo->piMode_ls00,
                                     *pColumn->colNullCond_ls00,
                                     *pFileFormat);
        }

        // NULL-value check may reveal that the DATA is not the NULL value;
        // in this case the length remains unchanged (INSERT_EMPTY_VALUE)
        if (INSERT_EMPTY_VALUE == pRawValue->strLeng_ls00)
        {
            // nothing read but no null condition specified; check if DATA is of DATA type char
            // as this is the only data type that allows empty values
            if ( (1 == szMap_CharDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) ||
                 (1 == szMap_BlobDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) ||
                 (1 == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]))
            {
                if (sp1sm_oracle == SQLMode)
                {
                    // if sqlmode = oracle an empty field means to insert NULL; check if it is defined
                    if ( true == pParamInfo->piMode_ls00.includes(sp1ot_optional) )
                    {
                        pRawValue->strLeng_ls00 = INSERT_NULL;
                    }
                    else
                    {
                        rc = errNullNotAllowed_els98;
                    }
                }
                // we're done here with the emptyness here; the rest is done in ls20_ValueToOIBuffer
            }
            else
            {
                rc = errMissingData_els98;
            }
        }

        if (errOK_els00 != rc)
        {
            pMsgObj->ls98MsgToString(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                    pColumn->colName_ls00.asCharp());
        }
    }   // end else of if (INSERT_DEFAULT != pRawValue->strLeng_ls00)


    //*
    //* Check value length
    //* Length of number values will be checked twice because it is first time check at conversion
    //* --> doesn't matter
    //*
    if ( (errOK_els00 == rc) &&
         (pRawValue->strLeng_ls00 >= 0) &&          // Exclude the case where the value is empty or NULL
         (0 == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) &&
         (0 == szMap_NumberDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) )
    {
        tsp00_Uint4 _len = pRawValue->strLeng_ls00;         // this is the length in Bytes but we need
                                                            // the length in characters

        if ( (1 == szMap_CharDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) &&
             (ctAscii_els00 != pFileFormat->ffoCodeType_ls00) )
        {
            if (ctUTF8_els00 != pFileFormat->ffoCodeType_ls00)
            {
                _len /= 2;
            }
            else
            {
                unsigned int charLength       = 0;
                unsigned int byteLength       = 0;
                int          isTerminated     = 0;
                int          isCorrupted      = 0;
                int          isExhausted      = 0;

                sp77encodingUTF8->stringInfo(pRawValue->strAddr_ls00,
                                             _len,
                                             1,
                                             &_len,
                                             &byteLength,
                                             &isTerminated,
                                             &isCorrupted,
                                             &isExhausted);
            }
        }
        rc = ls18CheckValueLength(_len, pParamInfo->piBasicInfo_ls00);
        if (rc != errOK_els00)
        {
            pMsgObj->ls98MsgToString(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                    pColumn->colName_ls00.asCharp());
        }
    }   // end if if ( (errOK_els00 == rc) && (0 == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) )

    return rc;
}
// ls20_TransformColumnValue()


/*
  -----------------------------------------------------------------------------
  function:     ls20_MoveDataWithinPacket
  -----------------------------------------------------------------------------
*/
void
ls20_MoveDataWithinPacket(tls00_OI_DataPartCache *  pDataPartCache,
                          SAPDB_Int4                lLongColCnt,
                          SAPDB_Int4                plLongIndex[],
                          tls00_TableDescription*   pTD,
                          SAPDB_Int4                lErrorPos)
{
    ROUTINE_DBG_MEO00 ("ls20_MoveDataWithinPacket");

    unsigned char * _pszBufPtr = STATIC_CAST(unsigned char*, pDataPartCache->dpc_Buffer_ls00.pBufPtr_ls00);
    SAPDB_Int4 &    _lRowCnt   = pDataPartCache->dpc_lRowCount_ls00;

    SAPDB_Int4 i = 0;
    SAPDB_Int4 j = 0;

    //*
    //*     Move valid records in buffer to the beginning of the buffer overwriting
    //*     the already inserted and the erroneous one
    //*
    tsp00_Int4 lMoveDistance = pDataPartCache->dpc_RecordSpace_ls00[lErrorPos - 1];

    SAPDB_memmove(&_pszBufPtr[0],
                  &_pszBufPtr[lMoveDistance],
                  (pDataPartCache->dpc_RecordSpace_ls00[_lRowCnt-1] - pDataPartCache->dpc_RecordSpace_ls00[lErrorPos - 1]) );

    pDataPartCache->dpc_Buffer_ls00.lBufSize_ls00 += lMoveDistance;                // adapt free
    pDataPartCache->dpc_Buffer_ls00.lBufLen_ls00  -= lMoveDistance;                // and used space in packet buffer

    //*
    //*     Adapt long descriptors in every single record and
    //*     adapt values for dlrRecordSpace_ls00 and dlrRecordNumbers_ls00
    //*
    //* Recalculate valpos (absolute value) and use the length of the long for computing the
    //* beginning of the next record in buffer. If the longs are all NULL the record length is
    //* the length to move the pointer to match the next record.
    //* In case the long descriptor indicates no long value in the buffer (ld_valmode() = vm_nodata)
    //* treat them as if they were NULL (the long length is not set).
    //*
    tsp00_Int4            lLongLen  = 0;
    tsp00_LongDescriptor  LongDesc;
    unsigned char         cLongColDefByte;
    unsigned char        *pszRecord = &_pszBufPtr[0];   // initializing pszRecord to first record in packet buffer
                                                        // this ptr will be wandering through the buffer
    _lRowCnt = _lRowCnt - lErrorPos;
    for (i = 0; i < _lRowCnt; ++i)
    {
        for (j = 0, lLongLen = 0; j < lLongColCnt; ++j)
        {
            // We need to subtract 1 from piBufPos_ls00 because Bufpos starts counting at 1!
            cLongColDefByte = *(pszRecord + pTD->tdParamArray_ls00[plLongIndex[j]]->piBufpos_ls00 - 1);

            // We need to copy the long descriptor before manipulating it to be
            // sure not to cause a crash when casting it to an odd memory address
            SAPDB_memcpy(&LongDesc, pszRecord + pTD->tdParamArray_ls00[plLongIndex[j]]->piBufpos_ls00, sizeof(tsp00_LongDescriptor));
            if ( (csp_undef_byte != cLongColDefByte) &&
                 (vm_nodata      != LongDesc.ld_valmode()) )
            {
                LongDesc.ld_valpos() -= lMoveDistance;            // adapt valpos - absolute value
                lLongLen             += LongDesc.ld_vallen();   // add len of THIS longvalue to lLongLen

                // We need to move the descriptor back to its place
                SAPDB_memcpy(pszRecord + pTD->tdParamArray_ls00[plLongIndex[j]]->piBufpos_ls00, &LongDesc, sizeof(tsp00_LongDescriptor));
            }
        }   // end for (j = 0, lLongLen = 0; j < lLongColCnt; ++j)

        pszRecord += lLongLen + pDataPartCache->dpc_RecordLength_ls00;

        pDataPartCache->dpc_RecordNumbers_ls00[i] = pDataPartCache->dpc_RecordNumbers_ls00[i + lErrorPos];
        pDataPartCache->dpc_RecordSpace_ls00[i]   = pDataPartCache->dpc_RecordSpace_ls00[i + lErrorPos] - lMoveDistance;
    }

    return;
}
// ls20_MoveDataWithinPacket()


/*
  -----------------------------------------------------------------------------
  function:     ls20_Recover
  -----------------------------------------------------------------------------
*/
void
ls20_Recover(tls00_AlternatingBuffers* pAlternatingBuffers,
             tsp00_Int4                lLongColCnt,
             tls00_TableDescription*   pTD,
             SAPDB_Int4                lErrorPos,
             bool                     &bPutVal)
{
    ROUTINE_DBG_MEO00 ("ls20_Recover");

    tsp00_Int4 rc = errOK_els00;

    // The 1st packet is the one that was received in the caller funtion
    // The 2nd packet is the one thats has been prepared (if ever) for sending by the caller function

    tls00_OI_DataPartCache *_pCache_1     = &pAlternatingBuffers->ab_pCache_ls00[pAlternatingBuffers->ab_lRcvPcktNo_ls00];
    tls00_Buffer           *_pCacheBuf_1  = &_pCache_1->dpc_Buffer_ls00;
    unsigned char          *_pszBufPtr    = STATIC_CAST(unsigned char*, _pCacheBuf_1->pBufPtr_ls00);

    SAPDB_Int4 & RowCount = _pCache_1->dpc_lRowCount_ls00;

    if (_pCache_1->dpc_lRowCount_ls00 > lErrorPos)
    {
        SAPDB_Int4 i = 0;
        SAPDB_Int4 j = 0;

//TODOTODO DAS SOLLTE IN EINE ALLGEMEINE STRUKTUR; UM ES NICHT IMMER WIEDER AUSFUEHREN ZU MUESSEN ???       
        //*
        //*     Special treatment for long columns: adapt positions
        //*
        SAPDB_Int4 *_plLongIndex = NULL;
        if (0 != lLongColCnt)
        {
            //*
            //*     Get the indexes of longs in param array
            //*
            _plLongIndex = new tsp00_Int4[lLongColCnt];
            for (i = 0, j = 0; i < pTD->tdFieldCount_ls00; ++i)
            {   //*     LONG column ?
                if (1 == szMap_LongDataType_ls00[pTD->tdParamArray_ls00[i]->piBasicInfo_ls00.fbDataType_ls00])
                {
                    _plLongIndex[j] = i;
                    ++j;
                }
            }
        }

        //*
        //*     Move data in 1st (receive) packet
        //* Means: move valid records in buffer to the beginning of the buffer overwriting
        //*        the already inserted and the erroneous one
        //* Changes value of _pCache_1->dpc_lRowCount_ls00 !!!
        //*
        ls20_MoveDataWithinPacket(_pCache_1, lLongColCnt, _plLongIndex, pTD, lErrorPos);

        //*
        //*     Move data from second packet into first one
        //* Means: move valid records in buffer to the beginning of the buffer overwriting
        //*        the already inserted and the erroneous one
        //*
        //* In ls20_MoveDataWithinPacket the values lBufLen and lBufSize are adapted to valid values that
        //* can be used here;
        //* There are some points that have to be considered:
        //*  - there might be no (filled) 2nd packet (if putval for the first packet was set)
        //*  - the records in the 2nd packet could occupy more space than there is free space in packet 1 after
        //*    moving data in the 1st packet
        //*  - in case putval is set for the 2nd packet (rowcount > 0 then for the 2nd packet) the last record
        //*    (the one for which putval is set) MUST not be copied to the first packet to not force putval for the first packet
        //*  - in case there are LONG values among the data and the 2nd packet is filled and bPutVal is not
        //*    set for the first packet: we will only copy as many records as fit into the first packet with complete
        //*    LONGs to prevent the need to send addtional putval packets
        //*
        tls00_OI_DataPartCache *_pCache_2 = &pAlternatingBuffers->ab_pCache_ls00[pAlternatingBuffers->ab_lSndPcktNo_ls00];
        
        if (_pCache_2->dpc_lRowCount_ls00 > 0)
        {
            // Initialize the bufptr to the first record in the 2nd packet
            tls00_Buffer*   _pCacheBuf_2    = &_pCache_2->dpc_Buffer_ls00;
            unsigned char*  _pszNextBufPtr  = STATIC_CAST(unsigned char*, _pCacheBuf_2->pBufPtr_ls00);
            SAPDB_Int4      _lCnt_RowsToAdd = _pCache_2->dpc_lRowCount_ls00;

            if (true == bPutVal)
            {
                --_lCnt_RowsToAdd;
            }

            // Prevent setting bPutVal for the first packet: compute count of records to copy from
            // 2nd to 1st packet
            while (_pCache_2->dpc_RecordSpace_ls00[_lCnt_RowsToAdd-1] > _pCacheBuf_1->lBufSize_ls00)
            {
                --_lCnt_RowsToAdd;
            }
            SAPDB_Int4 lMoveDistance = _pCache_2->dpc_RecordSpace_ls00[_lCnt_RowsToAdd-1];

            //*
            //* Copy records from 2nd to 1st packet
            //*
            SAPDB_memcpy(&_pszBufPtr[_pCacheBuf_1->lBufLen_ls00], &_pszNextBufPtr[0], lMoveDistance);

            //*
            //* Adapt Longdescriptor values (if there are LONGs among the data) and space values in recover structure
            //*
            SAPDB_Int4            _lLongLen  = 0;
            SAPDB_Int4            k          = 0;
            tsp00_LongDescriptor  LongDesc;
            unsigned char         cLongColDefByte;
            // initialize pszRecord to first record in packet buffer that was copied from 2nd packet;
            // this ptr will be wandering through the buffer
            unsigned char        *pszRecord = &_pszBufPtr[_pCacheBuf_1->lBufLen_ls00];

            for (i = _pCache_1->dpc_lRowCount_ls00, j = 0; i < _lCnt_RowsToAdd + _pCache_1->dpc_lRowCount_ls00; ++i, ++j)
            {
                for (k = 0, _lLongLen = 0; k < lLongColCnt; ++k)
                {
                    // We need to subtract 1 from piBufPos_ls00 because Bufpos starts counting at 1!
                    cLongColDefByte = *(pszRecord + pTD->tdParamArray_ls00[_plLongIndex[k]]->piBufpos_ls00 - 1);

                    // We need to copy the long descriptor before manipulating it to be
                    // sure not to cause a crash when casting it to an odd memory address
                    SAPDB_memcpy(&LongDesc, pszRecord + pTD->tdParamArray_ls00[_plLongIndex[k]]->piBufpos_ls00, sizeof(tsp00_LongDescriptor));
                    if ( (csp_undef_byte != cLongColDefByte) && (vm_nodata != LongDesc.ld_valmode()) )
                    {
                        LongDesc.ld_valpos() -= lMoveDistance;              // adapt valpos - absolute value
                        _lLongLen            += LongDesc.ld_vallen();       // add len of THIS longvalue to lLongLen

                        // We need to move the descriptor back to its place
                        SAPDB_memcpy(pszRecord + pTD->tdParamArray_ls00[_plLongIndex[k]]->piBufpos_ls00, &LongDesc, sizeof(tsp00_LongDescriptor));
                    }
                }   // end for (k = 0, lLongLen = 0; k < lLongColCnt; ++k)

                pszRecord += _lLongLen + _pCache_1->dpc_RecordLength_ls00;

                _pCache_1->dpc_RecordNumbers_ls00[i] = _pCache_2->dpc_RecordNumbers_ls00[j];
                _pCache_1->dpc_RecordSpace_ls00[i]   = _pCache_2->dpc_RecordSpace_ls00[j] + _pCacheBuf_1->lBufLen_ls00;
            }

            _pCacheBuf_1->lBufSize_ls00   -= lMoveDistance;                 // adapt free
            _pCacheBuf_1->lBufLen_ls00    += lMoveDistance;                 // and used space in first packet buffer
            _pCache_1->dpc_lRowCount_ls00 += _lCnt_RowsToAdd;               // adapt row count

            //*
            //*     Move data in 2nd (send) packet
            //* Means: move valid remaining records in buffer to the beginning of the buffer
            //* Changes value of _pCache_2->dpc_lRowCount_ls00 !!!
            //*
            //* the function is called as if an error occured at record _lCnt_RowsToAdd in the 2nd packet
            //*
            if (_lCnt_RowsToAdd != _pCache_2->dpc_lRowCount_ls00)
            {
                ls20_MoveDataWithinPacket(_pCache_2, lLongColCnt, _plLongIndex, pTD, _lCnt_RowsToAdd);
            }
        }   // end if (_pCache_2->dpc_lRowCount_ls00 > 0)

        // Reset packet numbers
        pAlternatingBuffers->ab_lRcvPcktNo_ls00 = (pAlternatingBuffers->ab_lRcvPcktNo_ls00 + 1) % 2;
        pAlternatingBuffers->ab_lSndPcktNo_ls00 = (pAlternatingBuffers->ab_lSndPcktNo_ls00 + 1) % 2;

        if (NULL != _plLongIndex)
        {
            delete [] _plLongIndex;
            _plLongIndex = NULL;
        }
    }
    else        //if (_pCache_1->dpc_lRowCount_ls00 > lErrorPos)
    {
        // Ok, all rows of the 1st packet are inserted -> reset row counter to 0 and adapt free space in packet buffer
        _pCacheBuf_1->lBufSize_ls00    = _pCacheBuf_1->lBufLen_ls00 + _pCacheBuf_1->lBufSize_ls00;
        _pCacheBuf_1->lBufLen_ls00     = 0;
        bPutVal                        = false;
        _pCache_1->dpc_lRowCount_ls00  = 0;
    }
    return;
}
// ls20_Recover()

/*
  -----------------------------------------------------------------------------
   End Chapter Global functions: implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
   Begin Chapter Local functions: implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls20_CheckHeader
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_CheckHeader(tls00_DBInfo            *pDBInfo,
                 tls00_TransformationCmd *pDatLoadCmd,
                 Tools_DynamicUTF8String  DatExtrStmt,
                 tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_CheckHeader");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    tsp00_Int4       rc      = errOK_els00;

    tls00_Command *pSvrCmd = new tls00_Command;
    memset (pSvrCmd, DEF_BYTE_LS00, sizeof(tls00_Command));

    // initialize the request string
    pSvrCmd->cmdReqDat_ls00 = CONST_CAST(SAPDB_Char*, DatExtrStmt.CharPtr());

    // initialize the answer string
    pSvrCmd->cmdRpyDat_ls00 = new char[MAX_REPLY_LENGTH_LS00];
    memset(pSvrCmd->cmdRpyDat_ls00, DEF_BYTE_LS00, MAX_REPLY_LENGTH_LS00);

    // initialize logging behaviour
    pSvrCmd->cmdLogging_ls00  = SAPDB_FALSE;
    pSvrCmd->cmdInternal_ls00 = SAPDB_TRUE;

    // Build a parse tree to be able to compare table names in the command that
    // generated the DATA (the one parsed now) and the command to load the
    // DATA (parameter to this function)
    rc = ls11ParseLoadCommand(pDBInfo, pSvrCmd);

    // Parsed command MUST be a dataextract command
    tls00_TransformationCmd *pDatExtrCmd = REINTERPRET_CAST(tls00_TransformationCmd*, pSvrCmd->cmdScnDat_ls00);

    if (pSvrCmd->cmdScanRC_ls00 != srFctSqlStmt_els00)
    {
        Tools_DynamicUTF8String DatExtrName;
        Tools_DynamicUTF8String DatLoadName;

        if ( true == pDatExtrCmd->trExclude_ls00 )
        {
            pDatLoadCmd->trExclude_ls00 = pDatExtrCmd->trExclude_ls00;
        }

        DatExtrName.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                            pDatExtrCmd->trPartSpec_ls00.psTabName_ls00.asCharp()));
        DatLoadName.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                            pDatLoadCmd->trPartSpec_ls00.psTabName_ls00.asCharp()));


        if (0 != DatExtrName.Compare( DatLoadName ))
        {
            pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errTableToLoadNotCurrentTable_els98,
                                                         DatLoadName.StrPtr(),
                                                         DatExtrName.StrPtr());
            rc = errTableToLoadNotCurrentTable_els98;
        }
    }
    else
    {
        pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errCheckHeader_els98);
        rc = errCheckHeader_els98;
    }

    if (NULL != pDatExtrCmd)
    {
        ls10FreeTransformation(srFctColExtract_els00, pSvrCmd->cmdScnDat_ls00 );

        if (NULL != pSvrCmd->cmdRpyDat_ls00)
        {
            delete [] pSvrCmd->cmdRpyDat_ls00;
            pSvrCmd->cmdRpyDat_ls00 = NULL;
        }
        if (NULL != pSvrCmd)
        {
            delete pSvrCmd;
            pSvrCmd = NULL;
        }
    }

    return rc;
}// ls20_CheckHeader


/*
  -----------------------------------------------------------------------------
  function:     ls20ConstructRTEStructure
  -----------------------------------------------------------------------------
*/
void
ls20ConstructRTEStructure(tls00_TransformationRTE*&  pTransformationRTE,
                          tls00_DBInfo*              pDBInfo,
                          tls00_TransformationCmd*   pTransformationCmd)
{
    if (NULL == pTransformationRTE)
    {
        pTransformationRTE = new tls00_TransformationRTE;
        memset(pTransformationRTE, 0, sizeof(tls00_TransformationRTE));
    };

    pTransformationRTE->rte_ResultCount_ls00[0] = 0xff;
    pTransformationRTE->rte_PartAttr_ls00.becomes(sp1pa_first_packet);

    pTransformationRTE->rte_SwapKind_ls00.becomes(SERVER_SWAPPING_LS00);


    //*
    //* performance
    //*
    extern tls00_Performance performance;
	memset(&performance,0,sizeof(tls00_Performance));

    // table empty|full
    pTransformationRTE->rte_TabEmpty_ls00  = false;
    pTransformationRTE->rte_TabLoaded_ls00 = false;

    pTransformationRTE->rte_Cnt_ProcessedRecords_ls00 = 0;
    pTransformationRTE->rte_LongColCnt_ls00           = 0;
    //*
    //* db|user
    //*
    pTransformationRTE->rte_TableSwitched_ls00 = true;

    // Init code type and code page
    tls00_DataSource *pDataSource = &pTransformationRTE->rte_DataSource_ls00;

    pDataSource->dasCodeType_ls00  = pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00;
    pDataSource->pdasCodePage_ls00 = new tsp81_CodePage;

    // We have to take into account the encodings of DATA file AND long files;
    // so we'll copy the code page in any case
    SAPDB_memcpy(&pDataSource->pdasCodePage_ls00->map, pDBInfo->dbiSession_ls00.siCodePage_ls00.map, 512);

    // Data file structure: only entered for DATAUPDATE as the file for DATALOAD is initialized
    // in vls20_dataload.cpp:ls20_InitStreams
    if (NULL == pTransformationRTE->rte_pDataFile_ls00)
    {
        pTransformationRTE->rte_pDataFile_ls00 = new tls00_VFile;
        memset(pTransformationRTE->rte_pDataFile_ls00, 0, sizeof(tls00_VFile));

        pTransformationRTE->rte_pDataFile_ls00->vfFileType_ls00 = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;
        if ( dtypeTape_ls00 == pTransformationRTE->rte_pDataFile_ls00->vfFileType_ls00 )
        {
            pTransformationRTE->rte_pDataFile_ls00->vfBackupTool_ls00 = pDBInfo->dbiSession_ls00.siBackupTool_ls00;
            pTransformationRTE->rte_pDataFile_ls00->vfBackupId_ls00.Assign(
                REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pTransformationCmd->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00));
        }
    }

    // Allocate space for data part handling (free space management in order interface buffer)
    if (NULL == pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00)
    {
        tls00_OI_DataPartCache* pCache = NULL;
        
        pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00 = new tls00_OI_DataPartCache[pDBInfo->dbiPktCount_ls00];
        for (SAPDB_Int4 i = 0; i < pDBInfo->dbiPktCount_ls00; ++i)
        {
            pCache = &pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00[i];
            memset(pCache, 0, sizeof(tls00_OI_DataPartCache));
            memset(&pCache->dpc_Buffer_ls00, 0, sizeof(tls00_Buffer));
            //pTransformationRTE->rte_pOI_DataPart_Cache_ls00 = new tls00_DataPartHandling;
            //memset(pTransformationRTE->rte_pOI_DataPart_Cache_ls00, 0, sizeof(tls00_DataPartHandling));
            //memset(&pTransformationRTE->rte_pOI_DataPart_Cache_ls00->dphBuffer_ls00, 0, sizeof(tls00_Buffer));
            
        }
    }
}
// ls20ConstructRTEStructure()


/*
  -----------------------------------------------------------------------------
  function:     ls20DestructRTEStructure
  -----------------------------------------------------------------------------
*/
void
ls20DestructRTEStructure(tls00_DBInfo*              pDBInfo,
                         tls00_TransformationCmd*   pTransformationCmd,
                         tls00_TransformationRTE*   pTransformationRTE)
{
    ROUTINE_DBG_MEO00 ("ls20DestructRTEStructure");

    //*
    //* performance
    //*
    extern tls00_Performance performance;
    memset(&performance,0,sizeof(tls00_Performance));

    pTransformationRTE->rte_Cnt_CommittedRecords_ls00 = 0;
    // pTransformationRTE->rte_Cnt_RejectedRecords_ls00 MUST not be updated here; it's used to count overall
    // rejected lines if processing TL ALL/USER
    pTransformationRTE->rte_LastCommLine_ls00 = 0;
    pTransformationRTE->rte_LongColCnt_ls00   = 0;

    tls00_TableDescription* pTI  = &pTransformationRTE->rte_TableInfo_ls00;
    SAPDB_Int4              i    = 0;

    // We have to delete only as much values as we have allocated.
    for (i=0; i < pTI->tdFieldCount_ls00; ++i)
    {
        // Delete paraminfo
        if (NULL != pTI->tdParamArray_ls00[i])
        {
            delete pTI->tdParamArray_ls00[i];
            pTI->tdParamArray_ls00[i] = NULL;
        }

        // Delete column names
        if (NULL != pTI->tdColName_ls00[i])
        {
            delete pTI->tdColName_ls00[i];
            pTI->tdColName_ls00[i] = NULL;
        }

    }

    // allocated buffer for DATA
    tls00_OI_DataPartCache* pCache = NULL;
    for (i = 0; i < pDBInfo->dbiPktCount_ls00; ++i)
    {
        pCache = &pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00[i];
        if (NULL != pCache)
        {
            if (NULL != pCache->dpc_Buffer_ls00.pBufPtr_ls00)
            {
                delete [] (STATIC_CAST(unsigned char*, pCache->dpc_Buffer_ls00.pBufPtr_ls00));
                pCache->dpc_Buffer_ls00.pBufPtr_ls00 = NULL;
            }
            if (NULL != pCache->dpc_RecordNumbers_ls00)
            {
                delete [] pCache->dpc_RecordNumbers_ls00;
                pCache->dpc_RecordNumbers_ls00 = NULL;
            }
            if (NULL != pCache->dpc_RecordSpace_ls00)
            {
                delete [] pCache->dpc_RecordSpace_ls00;
                pCache->dpc_RecordSpace_ls00 = NULL;
            }
        }
    }

    // Delete the structure itself
    if (NULL != pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00)
    {
        delete [] pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00;
        pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00 = NULL;
    }

    // Delete reusable packet header byte stream
    if (NULL != pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00.strAddr_ls00)
    {
        delete [] pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00.strAddr_ls00;
        pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00.strAddr_ls00 = NULL;
    }

    if (NULL != pTransformationRTE->rte_pStmtParsed_ls00)
    {
        delete [] pTransformationRTE->rte_pStmtParsed_ls00;
        pTransformationRTE->rte_pStmtParsed_ls00 = NULL;
    }


    tls00_DataSource *pDataSource = &pTransformationRTE->rte_DataSource_ls00;

    // Delete code page structure
    if (NULL != pDataSource->pdasCodePage_ls00)
    {
        delete pDataSource->pdasCodePage_ls00;
        pDataSource->pdasCodePage_ls00 = NULL;
    }

    // Delete input buffer
    if (NULL != pDataSource->dasInputBuffer.pBufPtr_ls00)
    {
        delete [] STATIC_CAST(unsigned char*, pDataSource->dasInputBuffer.pBufPtr_ls00);
        pDataSource->dasInputBuffer.pBufPtr_ls00 = NULL;
    }

    // Delete converted DATA buffer
    ls18RecordDestructor(pTransformationRTE->rte_TableInfo_ls00.tdFieldCount_ls00, pDataSource->dasConvertedData);

    // Delete remaining members of struct tls00_DataSource
    if (NULL != pDataSource->lReadPos_ls00)
    {
        delete [] pDataSource->lReadPos_ls00;
        pDataSource->lReadPos_ls00 = NULL;
    }
    if (NULL != pDataSource->plDataToColIndex_ls00)
    {
        delete [] pDataSource->plDataToColIndex_ls00;
        pDataSource->plDataToColIndex_ls00 = NULL;
    }
    if (NULL != pDataSource->plDataToParamIndex_ls00)
    {
        delete [] pDataSource->plDataToParamIndex_ls00;
        pDataSource->plDataToParamIndex_ls00 = NULL;
    }


    // delete pattern matching structure
    ls18DestructPattern(&pTransformationRTE->rte_Pattern_ls00);

    // - close LONG DATA files still open only if the file format is not RECORDS
    // - delete conversion buffers (if allocated)
    tls00_MultColSpec &MultCol = pTransformationCmd->trColSpec_ls00;

    if (0 != pTransformationRTE->rte_LongColCnt_ls00)
    {
        char szErr[256];
        szErr[0] = 0;

        tls00_Column *pCol = NULL;

        for (i = 0; i < MultCol.mcsCount_ls00; ++i)
        {
            pCol = MultCol.mcsColumn_ls00[i];
            if (true == pCol->colLongInfo.loiIsLong_ls00)
            {
                if ( (NULL  != pCol->colLongInfo.loiFileStruct_ls00) &&
                     (false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) )
                {
                    ls30VFClose(*pCol->colLongInfo.loiFileStruct_ls00, szErr);

                    pCol->colLongInfo.loiFileStruct_ls00->vfFileName_ls00 = NULL;

                    if (NULL != pCol->colLongInfo.loiFileStruct_ls00->vfMagicBytesCache_ls00.pBufPtr_ls00)
                    {
                        delete [] STATIC_CAST(unsigned char*, pCol->colLongInfo.loiFileStruct_ls00->vfMagicBytesCache_ls00.pBufPtr_ls00);
                        pCol->colLongInfo.loiFileStruct_ls00->vfMagicBytesCache_ls00.pBufPtr_ls00 = NULL;
                    }
                    if ( pCol->colLongInfo.loiFileStruct_ls00 != NULL )
                    {
                        delete pCol->colLongInfo.loiFileStruct_ls00;
                        pCol->colLongInfo.loiFileStruct_ls00 = NULL;
                    }
                    if( pCol->colLongInfo.loiLongFileName_ls00 != NULL)
                    {
                        delete [] pCol->colLongInfo.loiLongFileName_ls00;
                        pCol->colLongInfo.loiLongFileName_ls00 = NULL;
                    }
                }
            }
        }

        if (NULL != pTransformationRTE->rte_LongExecInfo_ls00.dliConversionBuffer_ls00.pBufPtr_ls00)
        {
            delete [] (STATIC_CAST(unsigned char*, pTransformationRTE->rte_LongExecInfo_ls00.dliConversionBuffer_ls00.pBufPtr_ls00));
            pTransformationRTE->rte_LongExecInfo_ls00.dliConversionBuffer_ls00.pBufPtr_ls00 = NULL;
            pTransformationRTE->rte_LongExecInfo_ls00.dliConversionBuffer_ls00.lBufLen_ls00 = 0;
        }

        if (NULL != pTransformationRTE->rte_LongExecInfo_ls00.dliNNLongs_ls00)
        {
            delete [] pTransformationRTE->rte_LongExecInfo_ls00.dliNNLongs_ls00;
            pTransformationRTE->rte_LongExecInfo_ls00.dliNNLongs_ls00 = NULL;
        }

        if (NULL != pTransformationRTE->rte_pLongDescriptorContainer_ls00)
        {
            delete [] pTransformationRTE->rte_pLongDescriptorContainer_ls00;
            pTransformationRTE->rte_pLongDescriptorContainer_ls00 = NULL;
        }
    }   // end if (0 != pTransformationRTE->rte_LongColCnt_ls00)

    //*
    //* ONLY for DATAUPDATE: close data file and delete data file structure and
    //*                      structure TransformationRTE itself
    if (tsUpdate_ls00 == pTransformationCmd->trTransSpec_ls00)
    {
        char szErrText[256];
        if (NULL != pTransformationRTE->rte_pDataFile_ls00)
        {
            ls30VFClose(*pTransformationRTE->rte_pDataFile_ls00, szErrText);

            //delete [] pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00;
            pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00 = NULL;

            delete pTransformationRTE->rte_pDataFile_ls00;
            pTransformationRTE->rte_pDataFile_ls00  = NULL;
        }

        delete pTransformationRTE;
    }
}
//  ls20DestructRTEStructure()


/*
  -----------------------------------------------------------------------------
  function:     ls20_ParseStmt
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_ParseStmt(MetaDataDef*&             pMetaDataDef,
               tls00_TransformationRTE*  pTransformationRTE,
               tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_ParseStmt");

    SAPDB_Int4  rc = errOK_els00;


    tls00_TransformationCmd *pTransformationCmd = pMetaDataDef->getTransformationModel()->getTransformationCmd();


    if (tsImport_ls00 == pTransformationCmd->trTransSpec_ls00)
    {
        return ls20ParseInsertStmt(pMetaDataDef, pTransformationRTE, pszErrText);
    }
    else if (tsUpdate_ls00 == pTransformationCmd->trTransSpec_ls00)
    {
        return ls20ParseUpdateStmt(pMetaDataDef, pTransformationRTE, pszErrText);
    }
    return rc;
}
// ls20_ParseStmt()




/*!
  -----------------------------------------------------------------------------
  function:     ls20_TransformRowColumns
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_TransformRowColumns(tls00_TransformationCmd*  pTransformationCmd,
                         tls00_TransformationRTE*  pTransformationRTE,
                         tsp1_sqlmode_Enum         SQLMode,
                         bool                      bUnicodeOI,
                         tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_TransformRowColumns");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // error msg obj
    tsp00_Longint    rc      = errOK_els00;

    tls00_ParamInfo  *pPI     = NULL;
    tls00_FileFormat  Format  = pTransformationCmd->trDataFile_ls00.fsFormat_ls00;
    tls00_Column     *pCol    = NULL;
    tls00_String     *RawData = pTransformationRTE->rte_DataSource_ls00.dasConvertedData;

    tsp00_Addr pszInputLine = STATIC_CAST(char*, pTransformationRTE->rte_DataSource_ls00.dasInputBuffer.pBufPtr_ls00);
    tsp00_Int4 InputLen     = pTransformationRTE->rte_DataSource_ls00.dasInputBuffer.lBufPos_ls00;

    //*
    //*     Read DATA
    //*
    tsp00_Int4 lFailIndex = 0;
    if (true == Format.ffoFormatted_ls00)           // read formatted DATA
    {
        rc = ls18GetFormattedValueEx(pszInputLine,
                                     InputLen,
                                     &pTransformationCmd->trColSpec_ls00,
                                     pTransformationRTE->rte_DataSource_ls00.lReadFieldsCnt_ls00,
                                     pTransformationRTE->rte_DataSource_ls00.plDataToColIndex_ls00,
                                     RawData,
                                     lFailIndex);
    }
    else if (true == Format.ffoCompress_ls00)
    {
        rc = ls18GetCSVData(pszInputLine,
                            InputLen,
                            &pTransformationRTE->rte_Pattern_ls00,
                            pTransformationRTE->rte_DataSource_ls00.lReadFieldsCnt_ls00,
                            pTransformationRTE->rte_DataSource_ls00.lReadPos_ls00,
                            pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00,
                            RawData,
                            lFailIndex);


/* This is actually the right place for this function; still in ls20_TransformColumnValue
        if (errOK_els00 == rc)
        {
            for (SAPDB_UInt i = 0; i < pTransformationRTE->rte_DataSource_ls00.lReadFieldsCnt_ls00; ++i)
            {
                pPI  = pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[i];
                pCol = pTransformationCmd->trColSpec_ls00.mcsColumn_ls00[pTransformationRTE->rte_DataSource_ls00.plDataToColIndex_ls00[i]];
                if ( (ioChar_els00 == pCol->colFormat_ls00.ffoFormat_ls00) &&
                     (1            == szMap_CharDataType_ls00[pPI->piBasicInfo_ls00.fbDataType_ls00]) &&
                     (1            != szMap_BlobDataType_ls00[pPI->piBasicInfo_ls00.fbDataType_ls00]) &&
                     (INSERT_EMPTY_VALUE != RawData[i].strLeng_ls00) && (INSERT_NULL != RawData[i].strLeng_ls00) )
                {
                    ls18UndoubleDelimitersInCharData(&RawData[i], &pTransformationRTE->rte_Pattern_ls00, Format.ffoCodeType_ls00);
                }
            }

        }
*/
    }
    else if (true == Format.ffoCompact_ls00)
    {
        rc = ls18GetCompactValue(pszInputLine,
                                 &pTransformationRTE->rte_MetaData_ls00,
                                 pTransformationRTE->rte_DataSource_ls00.lReadFieldsCnt_ls00,
                                 pTransformationRTE->rte_DataSource_ls00.lReadPos_ls00,
                                 RawData,
                                 &pTransformationCmd->trColSpec_ls00,
                                 lFailIndex);
    }
    if (errOK_els00 != rc)
    {
        if (ptTable_els00 == pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00)
        {
            pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errAtRow_els98,
                                                         ls20_ComputeErroneousRowNo(
                                                          pTransformationCmd,
                                                           pTransformationRTE->rte_Cnt_ProcessedRecords_ls00));
        }
        else
        {
            pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errAtRowWithTabname_els98,
                                                         ls20_ComputeErroneousRowNo(
                                                          pTransformationCmd,
                                                          pTransformationRTE->rte_Cnt_ProcessedRecords_ls00),
                                                         pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp());
        }
        if (errMissingData_els98 == rc)
        {
            pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errMissingData_els98,
                                                         pTransformationRTE->rte_TableInfo_ls00.tdColName_ls00[lFailIndex]->asCharp());
        }
    }

    tsp00_Longint len = 0;
    tsp00_Uint4   j   = 0;
    for (j=0; ( (errOK_els00 == rc) && (j < pTransformationRTE->rte_DataSource_ls00.lReadFieldsCnt_ls00) ) ; ++j)
    {
        // Even if loop index j corresponds exactly plDataToParamIndex_ls00[j] for DATALOAD
        // commands we need it here as we use this function for DATAUPDATE also; for DATAUPDATE
        // both indexes are not the same
        pPI  = pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[pTransformationRTE->rte_DataSource_ls00.plDataToParamIndex_ls00[j]];
        pCol = pTransformationCmd->trColSpec_ls00.mcsColumn_ls00[pTransformationRTE->rte_DataSource_ls00.plDataToColIndex_ls00[j]];

        rc = ls20_TransformColumnValue(&RawData[j],
                                       pCol,
                                       &pTransformationRTE->rte_DataSource_ls00.dasInputBuffer,
                                       pPI,
                                       &pTransformationCmd->trDataFile_ls00,
                                       pTransformationRTE->rte_SwapKind_ls00,
                                       SQLMode,
                                       &pTransformationRTE->rte_Pattern_ls00,
                                       ErrText);
        if (errOK_els00 == rc)
        {
            if ( (INSERT_NULL == RawData[j].strLeng_ls00) && (SAPDB_TRUE == pTransformationRTE->rte_NullValueCounter_ls00.nvcCount_ls00) )
            {
                ++pTransformationRTE->rte_NullValueCounter_ls00.nvcCounter_ls00;
            }
        }
        else
        {
            if (ptTable_els00 == pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00)
            {
                pMsgObj->ls98PrintMessage(layDataLoad_els98, errAtRow_els98,
                                                             ls20_ComputeErroneousRowNo(
                                                              pTransformationCmd,
                                                              pTransformationRTE->rte_Cnt_ProcessedRecords_ls00));
            }
            else
            {
                pMsgObj->ls98PrintMessage(layDataLoad_els98, errAtRowWithTabname_els98,
                                                             ls20_ComputeErroneousRowNo(
                                                              pTransformationCmd,
                                                              pTransformationRTE->rte_Cnt_ProcessedRecords_ls00),
                                                             pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp());
            }
            pMsgObj->ls98EFromRcToFile(layDataLoad_els98, ErrText, STATIC_CAST(ErrorNumbers, rc));
        }
    }   //  end for (j=0; ( (errOK_els00 == rc) && (j < pTransformationRTE->rte_DataSource_ls00.lReadFieldsCnt_ls00) ) ; ++j)

    return STATIC_CAST(tsp00_Int4, rc);
}
//  ls20_TransformRowColumns()


/*
  -----------------------------------------------------------------------------
  function:     ls20_TransformTableRows
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_TransformTableRows(tls00_DBInfo*              pDBInfo,
                        tls00_TransformationCmd*   pTransformationCmd,
                        tls00_TransformationRTE*   pTransformationRTE,
                        tls00_AlternatingBuffers*  pAlternatingBuffer,
                        bool                       bRecoverLong,
                        tsp00_Addr                 ErrText,
                        bool                      &bPutVal)
{
    ROUTINE_DBG_MEO00 ("ls20_TransformTableRows");

    tsp00_Longint rc     = errOK_els00;
    tsp00_Int4    rcREAD = errOK_els00;

    pTransformationRTE->rte_PartAttr_ls00.becomes(sp1pa_next_packet);

    tls00_OI_DataPartCache*  _pOI_PartCache  = &pAlternatingBuffer->ab_pCache_ls00[pAlternatingBuffer->ab_lSndPcktNo_ls00];
    tls00_Buffer*            pDataPartBuffer = &_pOI_PartCache->dpc_Buffer_ls00;
    SAPDB_Int4               iRow            = _pOI_PartCache->dpc_lRowCount_ls00;
    tls00_String             DataRecord;     // char buffer for reading row in file
    DataRecord.strAddr_ls00 = STATIC_CAST(char*, pTransformationRTE->rte_DataSource_ls00.dasInputBuffer.pBufPtr_ls00);

    bool bUnicodeOI = ls00UseUnicodeOI(pDBInfo);

    //*
    //*     Compute count of rows to be placed in the packet
    //* The count of rows that can be send depends on 2 values:
    //* 1. overall count of rows to insert set by customer using START option in DATALOAD command
    //* 2. count of rows that fit into 1 order interface packet
    //*
    SAPDB_Int4 _lMaxRecords = pTransformationRTE->rte_MaxRecordsInPacket_ls00;

    // Check if count of rows to insert is already reached
    if (UNDEFINED_LS00 != pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00)
    {
        SAPDB_Int4 _lDiff =
            STATIC_CAST(SAPDB_Int4, (pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00 -
                                     pTransformationRTE->rte_Cnt_ProcessedRecords_ls00) );
        if (_lDiff > 0)
        {
            if (_lDiff < _lMaxRecords)
            {
                _lMaxRecords = _lDiff;
            }
        }
        else
        {
            rcREAD = STOP_NOMORE_DATA_LS00;
        }
    }

    // Check if there are some long values to insert before continuing (may happen if some
    // records could not be inserted and bPutVal was set to true)
    if (true == bRecoverLong)
    {
        // LONGs? Insert them here. Last inserted 'normal' record is the one for
        // which we have to import the LONG
        rc = ls08InsertLongs(&pTransformationCmd->trColSpec_ls00,
                             pTransformationRTE,
                             _pOI_PartCache,
                             bPutVal,
                             bUnicodeOI,
                             ErrText);
        if (errOK_els00 == rc)
        {
            // save used space value for THIS record;
            // (iRow - 1) because iRow gives the number of rows handled up to this point; this must
            // be subtracted by 1 to come to the right index
            _pOI_PartCache->dpc_RecordSpace_ls00[iRow-1] = STATIC_CAST(tsp00_Int4, pDataPartBuffer->lBufLen_ls00);
        }
        else
        {   // Error --> undo inserted record by restoring used space of previous record

            // At this point we might have added already some part of the LONG restoring to the packet;
            // We need to remove all: the record and the long DATA
            SAPDB_Int4 _lSpaceDif = _pOI_PartCache->dpc_RecordSpace_ls00[iRow-1] - _pOI_PartCache->dpc_RecordSpace_ls00[iRow-2];
            pDataPartBuffer->lBufSize_ls00 += _lSpaceDif;
            pDataPartBuffer->lBufLen_ls00  -= _lSpaceDif;
        }

//TODOTODO Was ist, wenn hier ein Fehler auftritt aber der Benutzer eine bestimmte Fehleranzahl toleriert?
    }

    // Get the records for one request
    for (   iRow;
         ( (iRow < _lMaxRecords)                                                      &&
           (pDataPartBuffer->lBufSize_ls00 >= _pOI_PartCache->dpc_RecordLength_ls00)  &&
           (errOK_els00 == rcREAD) && (errOK_els00 == rc)                             &&
           (false == bPutVal) );
            ++iRow)
    {
        //*
        //*   process single record
        //*

        // get DATA from file
        rcREAD = ls20ReadData(pTransformationRTE->rte_pDataFile_ls00,
                              &pTransformationCmd->trDataFile_ls00.fsFormat_ls00,
                              &pTransformationRTE->rte_Pattern_ls00,
                              &pTransformationRTE->rte_MetaData_ls00,
                              &pTransformationRTE->rte_DataSource_ls00.dasInputBuffer,
                              &pTransformationRTE->rte_TabLoaded_ls00,
                              ( 0 == pTransformationRTE->rte_Cnt_ProcessedRecords_ls00),
                              ErrText);
        if (rcREAD < 0)
        {
            // following values may happen:
            // - STOP_LAST_DATA_LS00    --> process read record and then stop processing
            // - STOP_NOMORE_DATA_LS00  --> nothing more to process - get outa here
            // - error code             --> nothing more to process - get outa here
            if ( (STOP_NOMORE_DATA_LS00 != rcREAD) && (STOP_LAST_DATA_LS00 != rcREAD) )
            {
                rc = rcREAD;
            }
            else if (STOP_NOMORE_DATA_LS00 == rcREAD)
            {
                --iRow;
            }

            pTransformationRTE->rte_PartAttr_ls00.becomes(sp1pa_last_packet);  // Last DATA in any case
        }
        //*
        //*   check for load condition for table
        //*
        if ( (errOK_els00 == rc) && (STOP_NOMORE_DATA_LS00 != rcREAD) )
        {
            if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00)
            {
                DataRecord.strLeng_ls00 = pTransformationRTE->rte_DataSource_ls00.dasInputBuffer.lBufPos_ls00;

                if ( false == ls16CheckCondition(*pTransformationCmd->trTable_ls00.tsCondSpec_ls00,
                                                 DataRecord,
                                                 pTransformationCmd->trDataFile_ls00.fsFormat_ls00) )
                {
                    --iRow;
                    continue;
                }
            }
        }

        if ( (errOK_els00 == rc) && (STOP_NOMORE_DATA_LS00 != rcREAD) )
        {
            pTransformationRTE->rte_Cnt_ProcessedRecords_ls00++;

            //  insert record into RawData structure
            rc = ls20_TransformRowColumns(pTransformationCmd,
                                          pTransformationRTE,
                                          pDBInfo->dbiSession_ls00.siSQLMode_ls00,
                                          bUnicodeOI,
                                          ErrText);
        }

        if ( (errOK_els00 == rc) && (STOP_NOMORE_DATA_LS00 != rcREAD) )
        {
            //*
            //* Save build record number for later error messages
            //*
            _pOI_PartCache->dpc_RecordNumbers_ls00[iRow] = pTransformationRTE->rte_Cnt_ProcessedRecords_ls00;

            //*
            //* set LONG value descriptions
            //*
            pTransformationRTE->rte_LongExecInfo_ls00.dliCntNNLongs_ls00      = 0;
            pTransformationRTE->rte_LongExecInfo_ls00.dliFinishedNNLongs_ls00 = 0;

            rc = ls20_BuildRecord(pTransformationCmd,
                                  pTransformationRTE,
                                  &_pOI_PartCache->dpc_Buffer_ls00,
                                  bUnicodeOI,
                                  ErrText);
            if (errOK_els00 == rc)
            {
                // adapt free and used space in buffer
                pDataPartBuffer->lBufSize_ls00 -= _pOI_PartCache->dpc_RecordLength_ls00;
                pDataPartBuffer->lBufLen_ls00  += _pOI_PartCache->dpc_RecordLength_ls00;

                if (0 != pTransformationRTE->rte_LongExecInfo_ls00.dliCntNNLongs_ls00)
                {
                    // LONGs? Insert them here. Last inserted 'normal' record is the one for
                    // which we have to import the LONG
                    rc = ls08InsertLongs(&pTransformationCmd->trColSpec_ls00,
                                         pTransformationRTE,
                                         _pOI_PartCache,
                                         bPutVal,
                                         bUnicodeOI,
                                         ErrText);
                    if (errOK_els00 == rc)
                    {
                        // save used space value for THIS record
                        _pOI_PartCache->dpc_RecordSpace_ls00[iRow] = STATIC_CAST(tsp00_Int4, pDataPartBuffer->lBufLen_ls00);
                    }
                    else
                    {   // Error --> undo inserted record by restoring used space of previous record
                        pDataPartBuffer->lBufSize_ls00 += _pOI_PartCache->dpc_RecordLength_ls00;
                        pDataPartBuffer->lBufLen_ls00  -= _pOI_PartCache->dpc_RecordLength_ls00;
                    }
                }
                else
                {
                    // save used space value for THIS record
                    _pOI_PartCache->dpc_RecordSpace_ls00[iRow] = STATIC_CAST(tsp00_Int4, pDataPartBuffer->lBufLen_ls00);
                }
            }
        }

        // Following if handles errors from GetRecord and BuildRecord. Both functions log the error
        // so we have to adapt the error count only here.
        if (errOK_els00 != rc)
        {
            ++pTransformationRTE->rte_Cnt_RejectedRecords_ls00;
            if(pTransformationRTE->rte_Cnt_RejectedRecords_ls00 < pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors)
            {
                rc = errOK_els00;       // reset the error code to read more DATA;
            }
            --iRow;
        }

        ls18RecordInit(pTransformationRTE->rte_DataSource_ls00.lReadFieldsCnt_ls00,
                       pTransformationRTE->rte_DataSource_ls00.dasConvertedData,
                       STATIC_CAST(char*, pTransformationRTE->rte_DataSource_ls00.dasInputBuffer.pBufPtr_ls00),
                       pTransformationRTE->rte_DataSource_ls00.dasInputBuffer.lBufLen_ls00);

    } // END OF FOR count_of_rows

    _pOI_PartCache->dpc_lRowCount_ls00 = iRow;

    if (errOK_els00 == rc) 
    {
        if (STOP_NOMORE_DATA_LS00 == rcREAD)
        {
            rc = rcREAD;
        }
        
        if ( (0 == iRow) && ( (errOK_els00 == rc) || (STOP_NOMORE_DATA_LS00 == rc) ) )
        {
            rc = STOP_NO_DATA_LS00;
        }        
    }

    return STATIC_CAST(tsp00_Int4, rc);
}
//  ls20_TransformTableRows()


/*!
  -----------------------------------------------------------------------------
  function:     ls20_DL_Commit
  -----------------------------------------------------------------------------
*/
// UPDATES COUNT OF COMMITTED ROWS
static SAPDB_Int4
ls20_DL_Commit(tls00_DBInfo*             pDBInfo,
               SAPDB_Int4                lPcktNo,
               MetaDataDef*&             pMetaDataDef,
               tls00_TransformationCmd*  pTransformationCmd,
               tls00_TransformationRTE*  pTransformationRTE,
               SAPDB_Int4                lTransactionSize,      // defines number of rows to be loaded before commit
               SAPDB_Int4&               lInsertedRowsSinceLastCommit,
               tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_DL_Commit");

    tsp00_Int4  rc = errOK_els00;
    
    //if ( ( (pTransformationRTE->rte_Cnt_CommittedRecords_ls00 - lInsertedRowsSinceLastCommit) >= lTransactionSize ) ||
    //     (true == pTransformationRTE->rte_TabLoaded_ls00) )
    if ( (pTransformationRTE->rte_Cnt_CommittedRecords_ls00 - lInsertedRowsSinceLastCommit) >= lTransactionSize )
    {
        rc = ls15RSCommit(pDBInfo, pszErrText, lPcktNo);
        if (errOK_els00 == rc)
        {
            //*
            //*     Update the value of the last line of the data file that was committed to the DB
            //*
            pTransformationRTE->rte_LastCommLine_ls00 = 
                pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_LastSuccessful_ls00;
            //*
            //*     The following commit assures restart ability because we have one
            //*     single transaction for updating the meta DATA and for inserting the DATA;
            //*     for internal commands the autocommit is always off
            //*
            rc = ls20UpdateTransformationPackage(pDBInfo, pMetaDataDef, pTransformationCmd, pTransformationRTE, pszErrText);

            lInsertedRowsSinceLastCommit = pTransformationRTE->rte_Cnt_CommittedRecords_ls00;

            TRACE_PRNF_MLS99(("ls20_DL_Commit", "Commit after %d rows", pTransformationRTE->rte_LastCommLine_ls00) );
        }
    }
    return rc;
}
// ls20_DL_Commit()


/*
  -----------------------------------------------------------------------------
  function:     ls20_FillPacketBuffer
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_FillPacketBuffer(tls00_DBInfo*             pDBInfo,
                      tls00_TransformationCmd*  pTransformationCmd,
                      tls00_TransformationRTE*  pTransformationRTE,
                      tls00_AlternatingBuffers* pDataPartCache,
                      tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_FillPacketBuffer");

    tsp00_Int4  rc = errOK_els00;

    tsp1_packet*  pSqlPacket = NULL;
    tsp1_part*    pDataPart  = NULL;

    SAPDB_Int4 lRowCount = pDataPartCache->ab_pCache_ls00[pDataPartCache->ab_lSndPcktNo_ls00].dpc_lRowCount_ls00;
    if (0 == lRowCount)
    {
        rc = STOP_NO_DATA_LS00;
    }
    else
    {

    pSqlPacket = pDBInfo->dbiPktSndList_ls00[pDataPartCache->ab_lSndPcktNo_ls00];

    SAPDB_memcpy(REINTERPRET_CAST(char*, &pSqlPacket->sp1_header),
                 pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00.strAddr_ls00,
                 pTransformationRTE->rte_OI_Reuse_PacketHeader_ls00.strLeng_ls00);

    // Get pointer to DATA part to work with
    ls04GetPart(pSqlPacket, sp1pk_data, pDataPart);

    // Set part attribute to current value (either nextpacket or lastpacket)
    ls04SetPartAttribute(pDataPart, pTransformationRTE->rte_PartAttr_ls00);

    pDataPart->sp1p_arg_count() = lRowCount;                  // insert count of records in packet

    rc = ls04BufToPart(pDataPart,
                       REINTERPRET_CAST(char*, pDataPartCache->ab_pCache_ls00[pDataPartCache->ab_lSndPcktNo_ls00].dpc_Buffer_ls00.pBufPtr_ls00),
                       STATIC_CAST(tsp00_Int4, pDataPartCache->ab_pCache_ls00[pDataPartCache->ab_lSndPcktNo_ls00].dpc_Buffer_ls00.lBufLen_ls00));
    if (errOK_els00 == rc)
    {
        rc = ls04FinishPart (pSqlPacket, pDataPart);          // finish packet
    }
    
    }

    return rc;
}
// ls20_FillPacketBuffer()


/*
  -----------------------------------------------------------------------------
  function:     ls20_ReceivePacket
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_ReceivePacket(tls00_DBInfo*              pDBInfo,
                   tls00_TransformationCmd*   pTransformationCmd,
                   tls00_TransformationRTE*   pTransformationRTE,
                   tls00_AlternatingBuffers*  pAltBuffers,
                   SAPDB_Int4 &               lErrorPos,
                   tsp00_Addr                 pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_ReceivePacket");
    
    SAPDB_Int4 rc = errOK_els00;
    
    char                    _szErr[MAX_REPLY_LENGTH_LS00];

    SAPDB_Int4&             _lRcvPcktNo  = pAltBuffers->ab_lRcvPcktNo_ls00;
    tls00_OI_DataPartCache* _pDPCache    = &pAltBuffers->ab_pCache_ls00[_lRcvPcktNo];
    SAPDB_Int4&             _lRowCnt     = pAltBuffers->ab_pCache_ls00[_lRcvPcktNo].dpc_lRowCount_ls00;
    pszErrText[0] = 0;

    //*
    //*     Get the receive packet
    //*
    rc = ls03SQLReceive(pDBInfo, _lRcvPcktNo, pszErrText);
    if (errOK_els00 == rc)
    {
        // Analyse error status of sent packet
        rc = ls04RcFromPacket(pDBInfo->dbiPktRcvList_ls00[_lRcvPcktNo], _szErr, lErrorPos);
        if (errOK_els00 != rc)
        {
            pDBInfo->dbiPktRc_ls00 = rc;                // Set return code in info structure
        }
    }

    //*
    //*     Handle errors in packet
    //*
    if (errOK_els00 == rc)
    {
        // reporting
        pTransformationRTE->rte_Cnt_CommittedRecords_ls00 += _lRowCnt;
        pAltBuffers->ab_LastSuccessful_ls00 = (0 == _lRowCnt) ? _lRowCnt :
                                                                _pDPCache->dpc_RecordNumbers_ls00[_lRowCnt-1];
  
        // reinitializing
        _pDPCache->dpc_Buffer_ls00.lBufSize_ls00 = _pDPCache->dpc_Buffer_ls00.lBufLen_ls00 +
                                                       _pDPCache->dpc_Buffer_ls00.lBufSize_ls00;
        _pDPCache->dpc_Buffer_ls00.lBufLen_ls00  = 0;
        _lRowCnt = 0;
    }
    else
    {
        // In case the error position (lErrRecordNo) is not adapted but an error reported
        // the error happened sending and/or receiving the packet and we stop at this point
//TODOTODO                      if (-1 == lErrorPos)
        if (0 != lErrorPos )
        {
            LoaderMessages *pMsgObj = LoaderMessages::Instance();

            // If it is not parse again set error code and write message
            if (dberrParseAgain_els98 != rc)
            {
                if (ptTable_els00 == pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00)
                {
                    pMsgObj->ls98PrintMessage(layDataLoad_els98, errAtRow_els98,
                                                                 ls20_ComputeErroneousRowNo(
                                                                  pTransformationCmd,
                                                                  _pDPCache->dpc_RecordNumbers_ls00[lErrorPos - 1]));
                }
                else
                {
                    pMsgObj->ls98PrintMessage(layDataLoad_els98, errAtRowWithTabname_els98,
                                                                 ls20_ComputeErroneousRowNo(
                                                                  pTransformationCmd,
                                                                  _pDPCache->dpc_RecordNumbers_ls00[lErrorPos - 1]),
                                                                 pTransformationCmd->trTable_ls00.tsFullTabName_ls00);
                }
                pMsgObj->ls98Msg(layDataLoad_els98, pszErrText, errSimpleSQL_els98, _szErr);

                rc = errSimpleSQL_els98;

                ++pTransformationRTE->rte_Cnt_RejectedRecords_ls00;     // increase error counter
                pTransformationRTE->rte_Cnt_CommittedRecords_ls00 += lErrorPos - 1;

                // reporting
                pAltBuffers->ab_LastSuccessful_ls00 = _pDPCache->dpc_RecordNumbers_ls00[lErrorPos - 2];
            }   // end if (dberrParseAgain_els98 != rc)
        }
    }
    return rc;
}
// ls20_ReceivePacket()


/*
  -----------------------------------------------------------------------------
  function:     ls20_InitializeDataStream
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_InitializeDataStream(tls00_TransformationCmd*  pTransformationCmd,
                          tls00_TransformationRTE*  pTransformationRTE,
                          tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_InitializeDataStream");
    
    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    SAPDB_Int4      rc      = errOK_els00;

    tls00_FileSpec *pInFileSpec = &pTransformationCmd->trDataFile_ls00;

    //*
    //*     Initialize and open stream if not yet done
    //*
    if (0 == pTransformationRTE->rte_pDataFile_ls00->vfFileHndl_ls00)
    {
        // this branch will mainly be used by DATAUPDATE as the file is already opened coming
        // here in DATALOAD
        if (false == pInFileSpec->fsFormat_ls00.ffoBinary_ls00)
        {
            ls30VFInitialize(*pTransformationRTE->rte_pDataFile_ls00,
                             pInFileSpec->fsFileName_ls00,
                             sp5vf_text,
                             sp5vf_read,
                             pInFileSpec->fsFormat_ls00.ffoCodeType_ls00);
        }
        else
        {
            ls30VFInitialize(*pTransformationRTE->rte_pDataFile_ls00, pInFileSpec->fsFileName_ls00);
        }

        rc = ls30VFOpen(*pTransformationRTE->rte_pDataFile_ls00, pszErrText);

        if (ctUnknown_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
        {
            pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = pTransformationRTE->rte_pDataFile_ls00->vfFileEncoding_ls00;
        }
    }

    // Find out the maximum characters to read from a file for a single record; in case
    // of binary files (and not compact) a header might contain this information: check it
    // If there is no header we do nothing here; in this case and in case of not binary files
    // the count of characters to read is calculated after parsing the command
    // in ls20_InitializeStructuresAfterParsing()
    // To do this the data file must already be open
    if ( (errOK_els00 == rc) && (true == pInFileSpec->fsFormat_ls00.ffoBinary_ls00) )
    {
        // Try to read a header; it contains information about the record length
        // of one single record in the file;
        if (0 == pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00)
        {
            // command option "NOHEADER <size of record>" not specified in command -> try to read the header
            rc = ls30VFReadHeader(*pTransformationRTE->rte_pDataFile_ls00,
                                  pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00,
                                  pszErrText);
            if (errOK_els00 != rc)
            {
                if (errNoDatafileHeader_ls98 == rc)
                {
                    pMsgObj->ls98PrintMessage(layDataLoad_els98, errNoDatafileHeader_ls98,
                                                                 pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00);
                    rc = errOK_els00;
                }
                // if file read error, stop execution;
                // if only no header log it and compute length
            }
        }
    }

    // check for loading a part of the file only and move to start position
    if ( (errOK_els00 == rc) &&
         (false          == pInFileSpec->fsFormat_ls00.ffoCompact_ls00) &&
         (UNDEFINED_LS00 != pInFileSpec->fsExtract_ls00.feStart_ls00.lnuRemainder_ls00) )
    {
        if (ptTable_els00 == pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00)
        {
            tls00_LargeNumber lFilePos;
            lFilePos.lnu2GB_Blocks_ls00 = 0;        // Is of no interest here and ignored by ls30VFMoveToStartPosition

            if (false == pInFileSpec->fsFormat_ls00.ffoBinary_ls00)  // text file
            {
                lFilePos = pInFileSpec->fsExtract_ls00.feStart_ls00;
            }
            else
            {
                lFilePos.lnuRemainder_ls00 = pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00 *
                                            (pInFileSpec->fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 - 1);
            }

            // PTS 1113205
            rc = ls30VFMoveToStartPosition(*pTransformationRTE->rte_pDataFile_ls00, &lFilePos, pszErrText);
        }
    }

    return rc;
}
// ls20_InitializeDataStream()


/*
  -----------------------------------------------------------------------------
  function:     ls20_InitializeStructuresBeforeParsing
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls20_InitializeStructuresBeforeParsing(tls00_DBInfo*             pDBInfo,
                                       tls00_TransformationCmd*  pTransformationCmd,
                                       tls00_TransformationRTE*  pTransformationRTE,
                                       tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_InitializeStructuresBeforeParsing");

    SAPDB_Int4 rc = errOK_els00;

    //*
    //*     Check for table condition and convert
    //*
    if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00)
    {
        rc = ls16ConvertCondition(*pTransformationCmd->trTable_ls00.tsCondSpec_ls00,
                                  pDBInfo->dbiOwnSwap_ls00,
                                  pTransformationCmd->trDataFile_ls00.fsFormat_ls00,
                                  pszErrText);
    } 
    
    //*
    //*     Check for NULL value representation convert
    //*
    tls00_Column*  pCol = NULL;
    for (SAPDB_Int4 i = 0; ( (errOK_els00 == rc) && (i < pTransformationCmd->trColSpec_ls00.mcsCount_ls00) ); ++i)
    {
        pCol = pTransformationCmd->trColSpec_ls00.mcsColumn_ls00[i];
        //* Check for needs of transformation of NULL value representation for DEFAULT NULL
        //* specification (mainly in case DATA file encoding is UCS2 in any form).
        if (NULL != pCol->colNullCond_ls00)
        {
            rc = ls16ConvertCondition(*pCol->colNullCond_ls00,
                                      pDBInfo->dbiOwnSwap_ls00,
                                      pTransformationCmd->trDataFile_ls00.fsFormat_ls00,
                                      pszErrText);
        }
    }

    // Did the user specify columns in the command? If not flag it and check after parsing
    // if we have to count null values
    // This must be done for IMPORT/UPDATE TABLE commands only in case of file format CSV and FWD
    if ( (false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) &&
         (0     == pTransformationCmd->trColSpec_ls00.mcsCount_ls00) )
    {
        pTransformationRTE->rte_NullValueCounter_ls00.nvcCount_ls00 = SAPDB_TRUE;
    }

    if (errOK_els00 == rc)
    {
        // Fill pattern structure only if the file format is COMPRESSED
        if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00)
        {
            rc = ls18ConstructPattern(&pTransformationRTE->rte_Pattern_ls00,
                                      pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoSeperat_ls00,
                                      pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoDelimit_ls00,
                                      pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00,
                                      pszErrText);
        }
    }

    return rc;
}
// ls20_InitializeStructuresBeforeParsing()


/*
  -----------------------------------------------------------------------------
  function:     ls20_InitializeStructuresAfterParsing
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_InitializeStructuresAfterParsing(tls00_DBInfo*             pDBInfo,
                                      tls00_TransformationCmd*  pTransformationCmd,
                                      tls00_TransformationRTE*  pTransformationRTE,
                                      tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_InitializeStructuresAfterParsing");

    SAPDB_Int4 rc = errOK_els00;

    rc = ls20_InitializeOIPacket(pDBInfo, pTransformationRTE);

    if ( (errOK_els00 == rc) && (false == pTransformationCmd->trExclude_ls00) )
    {
        tls00_TableDescription*  pTI            = &pTransformationRTE->rte_TableInfo_ls00;
        tls00_OI_DataPartCache*  pOI_DataCache  = NULL;
        tls00_FileSpec*          pInFileSpec    = &pTransformationCmd->trDataFile_ls00;
        tls00_MultColSpec&       MultCol        = pTransformationCmd->trColSpec_ls00;
        tls00_Column*            pCol           = NULL;
        tls00_ParamInfo_LONG*    pLCI           = NULL;
        tls00_FieldBasics*       pBI            = NULL;
 
        //*
        //*     assign table name
        //*
        pTI->tdTablename_ls00.Init();
        pTI->tdTablename_ls00.rawAssign(pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());

        ls08InitLongFileInfo(&pTransformationCmd->trColSpec_ls00, &pTransformationCmd->trColLongSpec_ls00);

        // Do it with the first packet buffer; we have at least one packet buffer
        pOI_DataCache = &pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00[0];
        //*
        //*     Define record length
        pOI_DataCache->dpc_RecordLength_ls00 = ls18GetRecordLength(*pTI, true);

        //*
        //*     Compute max records to send in one packet
        //* Parts resultcount und parseid need to fit into packet too!
        //* Prior to this computation function ls20_InitializeOIPacket MUST have been run
        //*
        pTransformationRTE->rte_MaxRecordsInPacket_ls00 = pOI_DataCache->dpc_Buffer_ls00.lBufSize_ls00 /
                                                           pOI_DataCache->dpc_RecordLength_ls00;
        if (0 == pTransformationRTE->rte_MaxRecordsInPacket_ls00)
        {
            pTransformationRTE->rte_MaxRecordsInPacket_ls00 = 1;
        }

        TRACE_PRNF_MLS99(("ls20_InitializeStructuresAfterParsing", "Max count of rows per packet: %d",
                                                                  pTransformationRTE->rte_MaxRecordsInPacket_ls00) );
        
        pOI_DataCache->dpc_RecordNumbers_ls00 = new tsp00_Int4[pTransformationRTE->rte_MaxRecordsInPacket_ls00];
        pOI_DataCache->dpc_RecordSpace_ls00   = new tsp00_Int4[pTransformationRTE->rte_MaxRecordsInPacket_ls00];

        // Now do it for the other packets
        for (SAPDB_Int4 j = 1; j < pDBInfo->dbiPktCount_ls00; ++j)
        {
            pOI_DataCache = &pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00[j];

            pOI_DataCache->dpc_RecordLength_ls00 = pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00[0].dpc_RecordLength_ls00;
            pOI_DataCache->dpc_RecordNumbers_ls00 = new tsp00_Int4[pTransformationRTE->rte_MaxRecordsInPacket_ls00];
            pOI_DataCache->dpc_RecordSpace_ls00   = new tsp00_Int4[pTransformationRTE->rte_MaxRecordsInPacket_ls00];
        }

        //*
        //* Get the length of a single record to read from data file
        //*
        if ( true == pInFileSpec->fsFormat_ls00.ffoBinary_ls00 )        // formatted format
        {
            // In case of binary data file a header has already been read in ls20_InitializeDataStream
            if (0 == pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00)       // NOHEADER not found in command
            {
                pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00 = ls18GetRowLength(pTransformationCmd->trColSpec_ls00,
                                                                                   pTransformationRTE->rte_TableInfo_ls00,
                                                                                   pInFileSpec->fsFormat_ls00.ffoBinary_ls00);
            }   // else-zweig is done because the right value is set
        }
        else
        {
            pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00 = ls18GetRowLength(pTransformationCmd->trColSpec_ls00,
                                                                               pTransformationRTE->rte_TableInfo_ls00,
                                                                               pInFileSpec->fsFormat_ls00.ffoFormatted_ls00);
        }

        // Now allocate space and initialize DATA source structure
        if (errOK_els00 == rc)
        {
            // Initialization - a hack
            pTransformationRTE->rte_DataSource_ls00.dasInputBuffer.lBufLen_ls00 = 6 * pDBInfo->dbiPageSize_ls00;

            ls20InitDataSourceStruct(&pTransformationRTE->rte_DataSource_ls00,
                                     &pTransformationCmd->trDataFile_ls00.fsFormat_ls00,
                                     pTI->tdFieldCount_ls00,
                                     &pTransformationCmd->trColSpec_ls00,
                                     (tsUpdate_ls00 == pTransformationCmd->trTransSpec_ls00 ? 
                                      pTransformationCmd->trSetColIndex_ls00 : 0));
        }

        // Check if we have to check for every value of every column that allows NULLs if we
        // have to insert found values that correspond to null value specification as NULLs or
        // as true value; we check all values if no columns were specified in the command and the format of
        // the data file is either CSV or FWD - this is flagged already in ls20_InitializeStructuresBeforeParsing
        // and can be used here.
        // In case the user did not specify a column in the load command the Column-Structure is allocated in
        // the parse function. We have to allocate the null-condition members of every Column-Structure here if
        // that column allows NULLs.
        if ( (errOK_els00 == rc) && (SAPDB_TRUE == pTransformationRTE->rte_NullValueCounter_ls00.nvcCount_ls00) )
        {
            tls00_TableDescription *pTI = &pTransformationRTE->rte_TableInfo_ls00;

            for (SAPDB_Int4 k = 0; k < MultCol.mcsCount_ls00; ++k)
            {
                tls00_ParamInfo* pPI = pTI->tdParamArray_ls00[pTransformationRTE->rte_DataSource_ls00.plDataToParamIndex_ls00[k]];
                pCol = MultCol.mcsColumn_ls00[pTransformationRTE->rte_DataSource_ls00.plDataToColIndex_ls00[k]];

                // We expect colNullCond_ls00 to be NULL here!
                if ( (true == pPI->piMode_ls00.includes(sp1ot_optional)) || (true == pPI->piMode_ls00.includes(sp1ot_default)) )
                {
                pCol->colNullCond_ls00 = new tls00_CondSpec;
                memset(pCol->colNullCond_ls00, 0, sizeof(tls00_CondSpec));
                pCol->colNullCond_ls00->cosDefNull_ls00 = true;
                pCol->colNullCond_ls00->cosCond_ls00 = new tls00_Condition;
                memset(pCol->colNullCond_ls00->cosCond_ls00, 0, sizeof(tls00_Condition));


                rc = ls16ConvertCondition(*pCol->colNullCond_ls00,
                                          pDBInfo->dbiOwnSwap_ls00,
                                          pTransformationCmd->trDataFile_ls00.fsFormat_ls00,
                                          pszErrText);
                }
            }
        }

        for (SAPDB_Int4 i = 0; ( (errOK_els00 == rc) && (i < pTI->tdFieldCount_ls00) ); ++i)
        {
            // Even if loop index i corresponds exactly plDataToParamIndex_ls00[i] for DATALOAD
            // commands we need it here as we use this function for DATAUPDATE also; for DATAUPDATE
            // both indexes are not the same
            pBI  = &pTI->tdParamArray_ls00[pTransformationRTE->rte_DataSource_ls00.plDataToParamIndex_ls00[i]]->piBasicInfo_ls00;
            pCol = MultCol.mcsColumn_ls00[pTransformationRTE->rte_DataSource_ls00.plDataToColIndex_ls00[i]];

            if ( (errOK_els00 == rc) && (UNDEFINED_LS00 == pCol->colFormat_ls00.ffoFrac_ls00) )
            {
                pCol->colFormat_ls00.ffoFrac_ls00 = pBI->fbFraction_ls00;
            }

            //*
            //*     LONG columns
            //*
            if ( (errOK_els00 == rc) && (1 == szMap_LongDataType_ls00[pBI->fbDataType_ls00]) )
            {
                pLCI = &pCol->colLongInfo;

                pLCI->loiIsLong_ls00 = true;

                if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
                {
                    pLCI->loiFileStruct_ls00 = pTransformationRTE->rte_pDataFile_ls00;
                    // Define file encoding
                    pLCI->loiFileStruct_ls00->vfFileEncoding_ls00 = pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00;

                    // This is done because the file in format RECORDS is opened as binary file but it
                    // is not marked as encoded at the very beginning. Because the data file stores 'normal'
                    // data and LONG data we have to ensure that no magic bytes will be requested when
                    // processing LONGs
                    ls30VFEmptyMagicBytesCache(pLCI->loiFileStruct_ls00, true);
                }
                else //if (false == pDatLoadCmd->dloInfile_ls00.fsFormat_ls00.ffoCompact_ls00)
                {
//TODOTODO Besser in init_and_open_Longfile machen!??!?!??!
                    if (NULL == pLCI->loiFileStruct_ls00)
                    {
                        pCol->colLongInfo.loiFileStruct_ls00 = new tls00_VFile;
                        memset(pCol->colLongInfo.loiFileStruct_ls00, 0, sizeof(tls00_VFile));
                    }
                    else
                    {
                        // Check compatibility of external code type to db data type
                        rc = ls08CheckLONG_DT_Compatibility(pLCI->loiFileStruct_ls00->vfFileEncoding_ls00, pBI->fbDataType_ls00);
                        if (errOK_els00 != rc)
                        {
                            // error: data types do not match (function returns only ok or errIncompatibleDataTypes_els98)
                            LoaderMessages::Instance()->ls98Msg(layDataLoad_els98, pszErrText,
                                                                                   errIncompatibleDataTypes_els98,
                                                                                   pTI->tdColName_ls00[i]->asCharp());
                            break;
                        }
                    }
                }   // end else of if (true == pDatLoadCmd->dloInfile_ls00.fsFormat_ls00.ffoCompact_ls00)

                ++pTransformationRTE->rte_LongColCnt_ls00;
            } // end if ( (errOK_els00 == rc) && (1 == szMap_LongDataType_ls00[pBI->fbDataType_ls00]) )
        }   // end for (i; ( (errOK_els00 == rc) && (i < TableInfo.tdFieldCount_ls00) ); ++i)

        // Initialize variables used for LONG value processing
        //  - vector of long descriptors; used for handling LONGs in putval packets
        //  - array of indexes of NOT NULL LONG values in every single record
        //  - conversion buffer used for conversion of LONGs between different encodings and hex representation
        if ( (errOK_els00 == rc) && (0 != pTransformationRTE->rte_LongColCnt_ls00) )
        {
            pTransformationRTE->rte_pLongDescriptorContainer_ls00 = new tsp00_LongDescriptor[pTransformationRTE->rte_LongColCnt_ls00];
            pTransformationRTE->rte_LongExecInfo_ls00.dliNNLongs_ls00 = new tsp00_Int2[pTransformationRTE->rte_LongColCnt_ls00];

            pTransformationRTE->rte_LongExecInfo_ls00.dliConversionBuffer_ls00.lBufLen_ls00 = 8*pDBInfo->dbiPktSize_ls00;
            pTransformationRTE->rte_LongExecInfo_ls00.dliConversionBuffer_ls00.pBufPtr_ls00 =
                new unsigned char[pTransformationRTE->rte_LongExecInfo_ls00.dliConversionBuffer_ls00.lBufLen_ls00];
        }
    
    }   // end if ( (errOK_els00 == rc) && (false == pTransformationCmd->trExclude_ls00) )

    return rc;
}
// ls20_InitializeStructuresAfterParsing()


/*
  -----------------------------------------------------------------------------
  function:     ls20_ParseAgainErrorHandler
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls20_ParseAgainErrorHandler(MetaDataDef*&             pMetaDataDef,
                            tls00_TransformationRTE*  pTransformationRTE,
                            SAPDB_Int4                lErrorPos,
                            tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_ParseAgainErrorHandler");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    SAPDB_Int4 rc = errOK_els00;

    tls00_TransformationCmd *pTransformationCmd = pMetaDataDef->getTransformationModel()->getTransformationCmd();
    tls00_DBInfo            *pDBInfo            = pMetaDataDef->getConnection();


    tls00_OI_DataPartCache* pCurrentCache =
        &pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_pCache_ls00[pTransformationRTE->rte_OI_DataPart_Cache_ls00.ab_lRcvPcktNo_ls00];

    tls00_TransformationRTE* pRTE = new tls00_TransformationRTE;
    memset(pRTE, 0, sizeof(tls00_TransformationRTE));
    pRTE->rte_pStmtParsed_ls00 = pTransformationRTE->rte_pStmtParsed_ls00;

    char szErr[MAX_REPLY_LENGTH_LS00];
    szErr[0] = 0;
    rc = ls20_ParseStmt(pMetaDataDef, pRTE, szErr);
    if (errOK_els00 != rc)
    {
        // szErr is filled by called function; but as ls20_ParseStmt already logs
        // the error message to the logfile we adapt it here for a new message to be printed
        szErr[0] = 0;
        sp77sprintf(szErr, MAX_REPLY_LENGTH_LS00, "SQL error %d", rc);
        rc = errParseAgainFailed_els98;
    }
    else
    {
        tls00_TableDescription* pTIOld = &pTransformationRTE->rte_TableInfo_ls00;
        tls00_TableDescription* pTINew = &pRTE->rte_TableInfo_ls00;
        if (pTIOld->tdFieldCount_ls00 == pTINew->tdFieldCount_ls00)
        {
            SAPDB_Int4 j = 0;
            for (j; j < pTIOld->tdFieldCount_ls00; ++j)
            {
                if (0 != memcmp(pTIOld->tdParamArray_ls00[j], pTINew->tdParamArray_ls00[j], sizeof(tls00_ParamInfo)))
                {
                    sp77sprintf(szErr, MAX_REPLY_LENGTH_LS00, "Definition of column %s has changed", pTIOld->tdColName_ls00[j]->asCharp());
                    rc = errParseAgainFailed_els98;
                    break;
                }
            }
            if (j == pTIOld->tdFieldCount_ls00)     // in case j < fieldcount it's an error
            {
                SAPDB_memcpy(pTransformationRTE->rte_ParseId_ls00, pRTE->rte_ParseId_ls00, MAX_PARSEID_LENGTH_LS00);
                rc = ls20_InitializeOIPacket(pDBInfo, pTransformationRTE);
            }
        }
    }

    if (errOK_els00 != rc)
    {
        char szHint[64];
        szHint[0] = 0;
        sp77sprintf(szHint, 64, "START %d MAXROWS", pTransformationRTE->rte_Cnt_CommittedRecords_ls00 + 1);

        pMsgObj->ls98Msg(layDataLoad_els98, pszErrText, errParseAgainFailed_els98,
                                                        pTransformationRTE->rte_TableInfo_ls00.tdTablename_ls00.asCharp(),
                                                        "load",
                                                        szErr,
                                                        szHint);


        if (ptTable_els00 == pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00)
        {
            pMsgObj->ls98PrintMessage(layDataLoad_els98, errAtRow_els98,
                                                         ls20_ComputeErroneousRowNo(
                                                          pTransformationCmd,
                                                          pCurrentCache->dpc_RecordNumbers_ls00[lErrorPos - 1]));
        }
        else
        {
            pMsgObj->ls98PrintMessage(layDataLoad_els98, errAtRowWithTabname_els98,
                                                         ls20_ComputeErroneousRowNo(
                                                          pTransformationCmd,
                                                          pCurrentCache->dpc_RecordNumbers_ls00[lErrorPos - 1]),
                                                         pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp());
        }
    }

    return rc;
}
// ls20_ParseAgainErrorHandler()

/*===========================================================================*
 *     END
 *===========================================================================*/
