/*!
  -----------------------------------------------------------------------------
  module: vls26_TETable.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of TABLEEXTRACT for a single table

  version:      7.6.
  -----------------------------------------------------------------------------
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
#include "hls01.h"
#include "hls04.h"
#include "hls041.h"
#include "hls03.h"
#include "hls05.h"
#include "hls07.h"
#include "hls13.h"
#include "hls15.h"
#include "hls30.h"
#include "hls98msg.h"
#include "hls99.h"
#include "gbd00.h"

#include "hls31_odbc_Result.h"
#include "hls26.h"
#include "hls26_TETable.h"


static SAPDB_Char szLocalCmdString[lStmtLength_ls26];
static SAPDB_Char szLocalErrString[MAX_REPLY_LENGTH_LS00];

/*
  -----------------------------------------------------------------------------
  BeginChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls26WriteMetaHeader
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26WriteMetaHeader(tls00_DBInfo             *pDBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_TextractRec        *pTabExtrRec,
                    tsp00_Addr               ErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls26_SaveTableDescription
  -----------------------------------------------------------------------------

  description:  Get table description from kernel as binary structure
                (tsp7_table_description) and write structure to data file.

  arguments:    pDBInfo        [IN]   - contains all information on the running
                                         session and the database connected to
                pTabExtrRec    [IN]   - structure holding information for
                                         executing TABLEEXTRACT for a single table
                TableName      [IN]   - table name as UTF8 string
                pszTableDesc   [OUT]  - pointer to table descriptions structure
                ErrText        [OUT]  - error text

  returnvalue:  errOK_els00        - no error
                errFileWrite_els98 - error writing data
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_SaveTableDescription(tls00_DBInfo            *pDBInfo,
                          tls00_TextractRec       *pTabExtrRec,
                          Tools_DynamicUTF8String &TableName,
                          tsp00_Addr              &pszTableDesc,
                          tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls26_SaveIndexDescription
  -----------------------------------------------------------------------------

  description:  Get index description from kernel as binary structure
                (tsp7_index_description) and write structure to data file.

  arguments:    pDBInfo        [IN]   - contains all information on the running
                                         session and the database connected to
                pTabExtrRec    [IN]   - structure holding information for
                                         executing TABLEEXTRACT for a single table
                TableName      [IN]   - table name as UTF8 string
                ErrText        [OUT]  - error text

  returnvalue:  errOK_els00        - no error
                errFileWrite_els98 - error writing data
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_SaveIndexDescription(tls00_DBInfo            *pDBInfo,
                          tls00_TextractRec       *pTabExtrRec,
                          Tools_DynamicUTF8String &TableName,
                          tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteTableIndexDescToFile
  -----------------------------------------------------------------------------

  description:  Writes table/index description to data file.
                If description structure length exceeds single page size more
                than one page is written.

  arguments:    pTERec          [IN]   - tableextract structure
                DescriptionType [IN]   - table or index description ?
                pDesc           [IN]   - description itself
                lDescLength     [IN]   - description length
                ErrText         [OUT]  - error text

  returnvalue:  errOK_els00        - no error
                errFileWrite_els98 - error writing data
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteTableIndexDescToFile(tls00_TextractRec *TERec,
                               tls00_TEPageType   DescriptionType,
                               tsp00_Addr         pDesc,
                               tsp00_Int4         lDescLength,
                               tsp00_Addr         pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls26_WriteConstraintInfo
  -----------------------------------------------------------------------------

  description:  Save constraint definitions to data file. The definitions have
                been retrieved querying the system catalog.

  attention:    Prerequisite is that single constraint descriptions may not
                exceed a length of 8kB.

  arguments:    pDBInfo        [IN]   - contains all information on the running
                                         session and the database connected to
                pTableDef [IN]   - table info object that contains the list of
                                         constraint definitions
                pTabExtrRec    [IN]   - structure holding information for
                                         executing TABLEEXTRACT for a single table
                ErrText        [OUT]   - error text

  returnvalue:  errOK_els00        - no error
                errFileWrite_els98 - error writing data
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteConstraintInfo(tls00_DBInfo       *pDBInfo,
                         TableDef           *pTableDef,
                         tls00_TextractRec  *pTabExtrRec,
                         tsp00_Addr          pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls26_WriteDefaultInfo
  -----------------------------------------------------------------------------

  description:  Save default definitions to data file. The definitions have
                been retrieved querying the system catalog.

  arguments:    DBInfo         [IN]   - contains all information on the running
                                         session and the database connected to
                pTableDef [IN]   - table info object that contains the list of
                                         constraint definitions
                pTabExtrRec    [IN]   - structure holding information for
                                         executing TABLEEXTRACT for a single table
                ErrText        [OUT]  - error text

  returnvalue:  errOK_els00        - no error
                errFileWrite_els98 - error writing data
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteDefaultInfo(tls00_DBInfo       *pDBInfo,
                      TableDef           *pTableDef,
                      tls00_TextractRec  *pTabExtrRec,
                      tsp00_Addr          ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteShortStringPages
  -----------------------------------------------------------------------------

 description:   Extracts short string data pages.
                Runs command 'BEGIN SAVE LONG <table name>' and receives
                initial bdinfo. This bdinfo is a parameter to function
                ls26_PageExtractLoop.
                Last mentioned function finally gets the pages.

  arguments:    pDBInfo     [IN]   - contains all information on the running
                                      session and the database connected to
                TableName   [IN]   - table name UTF8 encoded
                pTabExtrRec [IN]   - structure holding information for
                                      executing TABLEEXTRACT for a single table
                ErrText     [OUT]  - error text

  returnvalue:  errOK_els00          - no error
                errInternSQL_els98   - command 'BEGIN...' failed
                errInternal_els98    - unknown part

                error of called functions are forwarded
                --> errInternal_els98, errFileWrite_els98, errFinishPart_els98,
                    errDbComm_els98
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteShortStringPages(tls00_DBInfo             *pDBInfo,
                           Tools_DynamicUTF8String  &TableName,
                           tls00_TextractRec        *pTabExtrRec,
                           tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls26_WritePrimaryDataPages
  -----------------------------------------------------------------------------

 description:   extracts primary data pages.
                Runs command 'BEGIN SAVE TABLE <table name>' and receives
                initial bdinfo. This bdinfo is a parameter to function
                ls26_PageExtractLoop.
                Last mentioned function finally gets the pages.

  arguments:    pDBInfo     [IN]   - contains all information on the running
                                      session and the database connected to
                TableName   [IN]   - table name UTF8 encoded
                pTabExtrRec [IN]   - structure holding information for
                                      executing TABLEEXTRACT for a single table
                ErrText     [OUT]  - error text

  returnvalue:  errOK_els00          - no error
                errInternSQL_els98   - command 'BEGIN...' failed
                errInternal_els98    - unknown part

                error of called functions are forwarded
                --> errInternal_els98, errFileWrite_els98, errFinishPart_els98,
                    errDbComm_els98

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WritePrimaryDataPages(tls00_DBInfo             *pDBInfo,
                           Tools_DynamicUTF8String  &TableName,
                           tls00_TextractRec        *pTabExtrRec,
                           tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteLongStringPages
  -----------------------------------------------------------------------------

  description:  Extracts long string data pages.
                Runs command 'BEGIN SAVE COLUMN' and receives
                initial bdinfo. This bdinfo is a parameter to function
                ls26_PageExtractLoop.
                Last mentioned function finally gets the pages.

  arguments:    pDBInfo     [IN]   - contains all information on the running
                                      session and the database connected to
                pTabExtrRec [IN]   - structure holding information for
                                      executing TABLEEXTRACT for a single table
                pszErrText  [OUT]  - error text

  returnvalue:  errOK_els00          - no error
                errInternSQL_els98   - command 'BEGIN...' failed
                errInternal_els98    - unknown part

                error of called functions are forwarded
                --> errInternal_els98, errFileWrite_els98, errFinishPart_els98,
                    errDbComm_els98

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteLongStringPages(tls00_DBInfo      *pDBInfo,
                          tls00_TextractRec *pTabExtrRec,
                          tsp00_Addr         pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls26_WDataHeaderPage
  -----------------------------------------------------------------------------

  description:  Writes special header page for data pages of type short string
                and primary data.
                The page contains the structure tls26_DescPageHeader at the
                beginning and at the end.
                Header structure tls26_DescPageHeader at those header pages
                contains only values for
                dphPageNo_ls26, dphType_ls26 and dphPagType_ls26.

  arguments:    pTabExtrRec        [IN]   - pointer; structure containing info for
                                        tableextract
                pszErrText    [OUT]  - error text

  returnvalue:  errOK_els00          - No error
                errFileWrite_els98   - error writing page to file
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WDataHeaderPage(tls00_TextractRec *pTabExtrRec, tsp00_Addr pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteMetaTrailer
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26_WriteMetaTrailer(tls00_DBInfo             *pDBInfo,
                      Tools_DynamicUTF8String  &OwnerName,
                      Tools_DynamicUTF8String  &TableName,
                      tls00_TextractRec        *pTabExtrRec,
                      tsp00_Addr                ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls26_PageExtractLoop
  -----------------------------------------------------------------------------

 description:  Gets data pages from kernel and saves them in data file.
                Main routine to extract data.

  arguments:    pDBInfo     [IN]   - Contains all information on the running
                                      session and the database connected to
                pTabExtrRec      [IN]   - Structure holding information for
                                      executing TABLEEXTRACT for a single table
                pszErrText  [OUT]  - Error text

  returnvalue:  errOK_els00        - No error
                Errors of called functions are forwarded
                --> errInternal_els98, errFileWrite_els98, errFinishPart_els98,
                    errDbComm_els98
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_PageExtractLoop(tls00_DBInfo      *pDBInfo,
                     tls00_TextractRec *pTabExtrRec,
                     tsp00_Addr         pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls26_Analyse

  -----------------------------------------------------------------------------

  description:  Extrahiert die datenseiten aus einem part des paketes. Passt dabei
                die Seitennummer und den seitentyp an.
                Bestimmt beim ersten aufruf fuer einen seitentyp die maximal
                moegliche anzahl von seiten im paket. Wird diese dann spaeter bei
                der bearbeitung der daten fuer diesen seitentyp unterschritten, ist
                das das signal, dass das letzte paket bearbeitet wird.

  arguments:    tsp1_packet_ptr [IN]   - zeiger; zu bearbeitendes paket
                pTabExtrRec          [IN]   - zeiger; Struktur mit daten fuer tableextract
                pszErrText      [OUT]  - Fehlertext

  returnvalue:  errOK_els00            - everything went fine
                STOP_ANALYSE_LS26      - ende der daten eines seitentyps erreicht
                errInternal_els98      - interner fehler: enthaelt eigentlichen fehlercode
                                         und zeile sowie dateinamen
                errFileWrite_els98     - fehler beim schreiben der daten in extraktionsdatei

                Die funktion vermerkt den fehler bereits im logfile und im
                rueckgabestring.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_Analyse(tsp1_packet_ptr    pSqlPacket,
             tls00_TextractRec *pTabExtrRec,
             tsp00_Addr         pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls26_SendBdinfoPckt

  -----------------------------------------------------------------------------

  description:  Sendet ein bdinfo an den kern

  arguments:    pDBInfo         [IN]   - zeiger; contains all
                                          information on the running session and the
                                          database connected to.
                lPcktNo         [IN]   - Nummer des zu nutzenden paketes (0 oder 1)
                pszRawChars     [IN]   - zeiger; zeigt auf zu sendende daten (z.b.
                                          bdinfo)
                lRawCharsLength [IN]   - laenge der zu sendenden daten
                pszErrText      [OUT]  - Fehlertext

  returnvalue:  errOK_els00            - everything went fine
                errFinishPart_els98    - der zu sendende part konnte nicht erstellt werden
                errDbComm_els98        - fehler beim senden des paketes

                Die funktion vermerkt den fehler bereits im logfile und im
                rueckgabestring.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_SendBdinfoPckt(tls00_DBInfo *pDBInfo,
                    tsp00_Int4    lPcktNo,
                    tls00_BDInfo* pBDInfo,
                    tsp00_Addr    pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls26_WLongDataHeaderPage

  -----------------------------------------------------------------------------

  description:  Writes special header page for data pages of type long string
  .
                The page contains the structure tls26_DescPageHeader at the
                beginning and at the end.
                Header structure tls26_DescPageHeader contains only values for
                dphPageNo_ls26, dphType_ls26, dphPagType_ls26 and dphSpecialID_ls26.
                The latter contains the key length.

                Additionally to the header the header page contains the key of the
                record for which the long column was extracted for.

  arguments:    pTabExtrRec        [IN]   - pointer; structure containing info for
                                        tableextract
                lColumnNo     [IN]   - 2 byte integer, number of column in single
                                        record extracted
                lRecordKeyLen [IN]   - length of key of single record
                pszRecordKey  [IN]   - key of single record
                pszErrText    [OUT]  - error text

  returnvalue:  errOK_els00          - No error
                errFileWrite_els98   - error writing page to file
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WLongDataHeaderPage(tls00_TextractRec *pTabExtrRec,
                         tsp00_Int2         lColumnNo,
                         tsp00_Int4         lRecordKeyLen,         // nur fuer long-werte von interesse, fuer alle anderen aufrufer auf 0 setzen
                         tsp00_Addr         pszRecordKey,
                         tsp00_Addr         pszErrText);

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  BeginChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls26ExportTablePages
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26ExportTablePages(MetaDataDef       *&pMetaDataDef ,
                      tls00_TextractRec *pTabExtrRec,
                      tsp00_Addr        ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26ExportTablePages");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    tsp00_Int4       rc     = errOK_els00;

    TransformationDef       * pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd * pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_DBInfo            * pDBInfo            = pMetaDataDef->getConnection();
    //*
    //*  update transformation table and write metadata header
    //*
    if ( errOK_els00 == rc )
    {
         rc = ls26UpdateTransformationPackage(pDBInfo, pMetaDataDef , pTransformationCmd, pTabExtrRec, ErrText);
         if (errOK_els00 != rc )
         {
             pMsgObj->ls98Msg(layTableExtract_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
             return errInternal_els98;
         }
         ls26WriteMetaHeader(pDBInfo, pTransformationCmd, pTabExtrRec, ErrText );
    }
    //*
    //*     Convert ownername and tablename (if given) to UTF8 strings
    //*
    Tools_DynamicUTF8String  ownername;
    Tools_DynamicUTF8String  tablename;
    Tools_DynamicUTF8String  schemaname;

    if (0 != pTransformationCmd->trTable_ls00.tsTabOwner_ls00.length()) {
        ownername.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,pTransformationCmd->trTable_ls00.tsTabOwner_ls00.asCharp()));
    }
    tablename.Assign    (REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp()));
    schemaname         = REINTERPRET_CAST(SAPDB_UTF8*,                          pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp());

    if ( 0 != _tcscmp         (pDBInfo->dbiSession_ls00.siSchemaName_ls00, pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp()) && false == pTransformationCmd->trInternal_ls00 )
         rc = ls15SwitchSchema(pDBInfo, schemaname, ErrText);

    //*
    //*     Extract table data and necessary meta data
    //*
    if ( (errOK_els00 == rc) && (false == pTransformationCmd->trExclude_ls00) )
    {
        //*
        //*     Get list of contraint and default definitons
        //* This has to be done before running ls26_SaveTableDescription() which runs a
        //* 'DESCRIBE TABLE'; this command locks the table definition until a commit has been
        //* issued and no information can be retrieved from the db
        //*
        TableDef *pTableDef = pMetaDataDef->getRelationalModel()->getTableDef();
        if (errOK_els00 == rc)
        {
            rc = pTableDef->Init(schemaname, tablename);
            if (errOK_els00 == rc)
                rc = pTableDef->readConstraintDef();

            if (errOK_els00 == rc)
                rc = pTableDef->readDefaultDef();

        }

        // Needed to decide whether to save short LONG column pages or not
        bool bShortColumnFileAvailable = false;

        //*
        //*  Save table description
        //*
        if (errOK_els00 == rc)        {
            tsp00_Addr pszTabDesc                = NULL;        // holds the table description structure

            rc = ls26_SaveTableDescription(pDBInfo, pTabExtrRec, tablename, pszTabDesc, ErrText);
            if (errOK_els00 == rc)
            {
                tsp7_table_description *TabDescStruct_sp07 = REINTERPRET_CAST(tsp7_table_description*, pszTabDesc);

                // Get counts of contraints and defaults
                pTabExtrRec->terConstraintCnt_ls00 = pTableDef->constraintCount();
                pTabExtrRec->terDefaultCnt_ls00    = pTableDef->defaultCount();

                // Check for LONG columns in general and short LONG columns in particular
                int i = 0;
                tsp7_ffield_info *pFieldInfo = NULL;

                for (i; i < TabDescStruct_sp07->sp7t_field_cnt(); ++i)
                {
                    pFieldInfo = &TabDescStruct_sp07->sp7t_fields()[i];

                    if (1 == szMap_LongDataType_ls00[pFieldInfo->sp7fi_data_type])
                    {
                        // Long columns in table;
                        ++pTabExtrRec->terLongColCnt_ls00;

                        // Check for short column file in db -> available only if at least
                        // one long column is of data type dstra, dstre, dstrb, dstrdb or dstruni
                        if ((pFieldInfo->sp7fi_data_type < dlonga) || (pFieldInfo->sp7fi_data_type == dstruni) )
                        {
                            bShortColumnFileAvailable = true;
                        }
                    }
                }   // end for (i; i < TabDescStruct_sp07->sp7t_field_cnt(); ++i)
            }
        }

        //*
        //*  Save index description
        //*
        if (errOK_els00 == rc)
            rc = ls26_SaveIndexDescription(pDBInfo, pTabExtrRec, tablename, ErrText);

        //*
        //*  Save constraint description
        //*
        if ( (errOK_els00 == rc) && (0 != pTabExtrRec->terConstraintCnt_ls00) )
        {
            pTabExtrRec->terPageTypeExtracting_ls00 = tptConstraint_els00;
            rc = ls26_WriteConstraintInfo(pDBInfo, pTableDef, pTabExtrRec, ErrText);
        }

        //*
        //*   Save default description
        //*
        if ( (errOK_els00 == rc) && (0 != pTabExtrRec->terDefaultCnt_ls00) )
        {
            pTabExtrRec->terPageTypeExtracting_ls00 = tptDefault_els00;
            rc = ls26_WriteDefaultInfo(pDBInfo, pTableDef, pTabExtrRec, ErrText);
        }

        //*
        //*   Save short LONG pages
        //*
        if ( (errOK_els00 == rc ) && (true == bShortColumnFileAvailable))
        {
            pTabExtrRec->terPageTypeExtracting_ls00 = tptShortString_els00;
            rc = ls26_WriteShortStringPages(pDBInfo, tablename, pTabExtrRec, ErrText);
            if (errOK_els00 != rc)
            {
                pMsgObj->ls98Msg(layTableExtract_els98, ErrText, errTEExtractDataOfCertainTable_els98,
                                                                 "SHORT LONG",
                                                                 tablename.StrPtr());
            }
        }

        //*
        //*   Save primary data pages
        //*
        if (errOK_els00 == rc)
        {
            pTabExtrRec->terPageTypeExtracting_ls00 = tptData_els00;
            rc = ls26_WritePrimaryDataPages(pDBInfo, tablename, pTabExtrRec, ErrText);
            if (errOK_els00 != rc)
                pMsgObj->ls98Msg(layTableExtract_els98, ErrText, errTEExtractDataOfCertainTable_els98,
                                                                 "PRIMARY",
                                                                 tablename.StrPtr());
        }


        //*
        //*   Save long LONG column pages
        //*
        if ( (errOK_els00 == rc ) && (0 != pTabExtrRec->terLongColCnt_ls00) )
        {
            pTabExtrRec->terPageTypeExtracting_ls00 = tptLongString_els00;

            rc = ls26_WriteLongStringPages(pDBInfo, pTabExtrRec, ErrText);
            if (errOK_els00 != rc)
                pMsgObj->ls98Msg(layTableExtract_els98, ErrText, errTEExtractDataOfCertainTable_els98,
                                                                 "LONG",
                                                                 tablename.StrPtr());
        }


        //*
        //*   Write trailer page into data file
        //*
        if (errOK_els00 == rc)
        {
            pTabExtrRec->terPageTypeExtracting_ls00 = tptEOT_els00;
            rc = ls26_WDataHeaderPage(pTabExtrRec, ErrText);
        }


        // Release the table lock if
        // - the WHOLE table is extracted without errors 
        // - user canceled execution
        if ( (errOK_els00 == rc) || (errCancelRequest_els98 == rc) )
        {
            char szErrText[256];
            tsp00_Int4 rcCommit = ls15Commit(pDBInfo, szErrText);
            // error is of no interest here
            if (errOK_els00 == rc)
            {   // Mark table extracted only if no errors occured
                pTabExtrRec->terTabExtracted_ls00  = true;
            }
        }
    } // if ( false == pTransformationCmd->trExclude_ls00 )

    //*
    //*  update transformation table and write metadata trailer
    //*
    if ( errOK_els00 == rc )
    {
        ls26_WriteMetaTrailer(pDBInfo, ownername, tablename, pTabExtrRec, ErrText);

        rc = ls26UpdateTransformationPackage(pDBInfo, pMetaDataDef , pTransformationCmd, pTabExtrRec, ErrText);
        if (errOK_els00 != rc )
        {
            pMsgObj->ls98Msg(layTableExtract_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
            return errInternal_els98;
        }
    }
    schemaname.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));

    if ( 0     != _tcscmp(pDBInfo->dbiSession_ls00.siSchemaName_ls00, pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp()) &&
         false == pTransformationCmd->trInternal_ls00)
    {
        ls15SwitchSchema(pDBInfo, schemaname, ErrText);
    }

    if ( false == pTransformationCmd->trInternal_ls00 && errOK_els00 == rc )
        pMsgObj->ls98PrintMessage(layTableExtract_els98, msgTESuccessful_els98);

    return rc;
}
// ls26ExportTablePages()


/*
  -----------------------------------------------------------------------------
  function:     ls26WriteMetaHeader
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26WriteMetaHeader(tls00_DBInfo      *pDBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_TextractRec *pTabExtrRec,
                    tsp00_Addr         ErrText )
{
    ROUTINE_DBG_MEO00 ("ls26WriteMetaHeader");

    tsp00_Int4 rc = errOK_els00;

    LoaderMessages  *MsgInstance = LoaderMessages::Instance();
    // header to write
    //-----------------------------------------------------------------------------------------------------------
    // repserver | code | header |            |         |      |      | page size    | count of        | more than
    // swap at   | type | length | identifier | version | date | time | of extracted | pages in        | one table
    // extract   |      |        |            |         |      |      | db           | file (blockung) | in file ?
    //-----------------------------------------------------------------------------------------------------------
    // 1 byte    |  1   |  4     |      64    | 40      | 8    | 8    |    4         |  4              |   1

    SAPDB_Bool bMultipleTables = SAPDB_FALSE;

    tsp00_Int4 lBufPos = 0;

    // headerlength
    size_t len = pTabExtrRec->terPageSize_ls00;

    tsp00_Addr pszBuffer = pTabExtrRec->terpPageBuffer_ls00;

    if ( NULL != pszBuffer )
    {
        memset(pszBuffer, 0, pTabExtrRec->terPageSize_ls00 );

        // swap type of source database
        pszBuffer[lBufPos] = STATIC_CAST(unsigned char, ls07DefineSwap(NULL));
        lBufPos += sizeof(unsigned char);

        // encoding type of source database
        pszBuffer[lBufPos] = STATIC_CAST(unsigned char, pDBInfo->dbiEncoding_ls00);
        lBufPos += sizeof(unsigned char);

        // header length of header
        SAPDB_memcpy(&pszBuffer[lBufPos], &len, sizeof(tsp00_Int4));
        lBufPos += sizeof(tsp00_Int4);

        // identifier
        tsp00_C64c pszId;
        pszId.Init();
        pszId.rawAssign(HEADER_IDENTIFIER_LS00);
        SAPDB_memcpy(&pszBuffer[lBufPos], pszId.asCharp(), pszId.length());
        lBufPos += pszId.size();

        // version
        tsp00_Versionc pszVersion;
        sp100_GetVersionString(COMP_NAME_REPL_MAN_SP100, s100buildnumber, pszVersion);
        SAPDB_memcpy(&pszBuffer[lBufPos], pszVersion.asCharp(), pszVersion.length());
        lBufPos += pszVersion.size();

        tsp00_Date tmpDate;
        tsp00_Time tmpTime;
        sqldattime(tmpDate, tmpTime);
        tsp00_Datec pszDate;
        tsp00_Timec pszTime;
        pszDate.p2c(tmpDate);
        pszTime.p2c(tmpTime);
        // date
        SAPDB_memcpy(&pszBuffer[lBufPos], pszDate.asCharp(), pszDate.length());
        lBufPos += pszDate.size();
        // time
        SAPDB_memcpy(&pszBuffer[lBufPos], pszTime.asCharp(), pszTime.length());
        lBufPos += pszTime.size();
        // pagesize
        SAPDB_memcpy(&pszBuffer[lBufPos], REINTERPRET_CAST(char*, &pDBInfo->dbiPageSize_ls00), sizeof(pDBInfo->dbiPageSize_ls00));
        lBufPos += sizeof(pDBInfo->dbiPageSize_ls00);

        SAPDB_memcpy(&pszBuffer[lBufPos], REINTERPRET_CAST(char*, &pTabExtrRec->terCount), sizeof(pTabExtrRec->terCount));
        lBufPos += sizeof(pTabExtrRec->terCount);

        // multiple tables
        SAPDB_memcpy(&pszBuffer[lBufPos], REINTERPRET_CAST(char*, &bMultipleTables), sizeof(bMultipleTables));
        lBufPos += sizeof(bMultipleTables);

        // write command to buffer
        SAPDB_memcpy(&pszBuffer[lBufPos], pTransformationCmd->trFullUserInput_ls00, _tcslen(pTransformationCmd->trFullUserInput_ls00));
        lBufPos += _tcslen(pTransformationCmd->trFullUserInput_ls00);

        // write structure to file; updates file pos
        rc = ls30VFWriteLn(*pTabExtrRec->terOutFile_ls00, pszBuffer, len, ErrText); // error is already logged by ls30VFWriteLn
    }

    return rc;

} // ls26WriteMetaHeader()



/*
  -----------------------------------------------------------------------------
  EndChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/



/*
  -----------------------------------------------------------------------------
  BeginChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls26_SaveTableDescription
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_SaveTableDescription(tls00_DBInfo            *pDBInfo,
                          tls00_TextractRec       *pTabExtrRec,
                          Tools_DynamicUTF8String &TableName,
                          tsp00_Addr              &pszTableDesc,
                          tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_SaveTableDescription");

    tsp00_Int4  rc = errOK_els00;

    tsp00_Int4 lDescLen = 0;

    rc = ls13GetTableDescription(pDBInfo, 0, TableName, lDescLen, pszTableDesc, pszErrText);
    if (errOK_els00 == rc)
    {
        rc = ls26_WriteTableIndexDescToFile(pTabExtrRec, tptTableDescription_els00, pszTableDesc, lDescLen, pszErrText);
    }
    return rc;
}
// ls26_SaveTableDescription()


/*
  -----------------------------------------------------------------------------
  function:     ls26_SaveIndexDescription
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_SaveIndexDescription(tls00_DBInfo            *pDBInfo,
                          tls00_TextractRec       *pTabExtrRec,
                          Tools_DynamicUTF8String &TableName,
                          tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_SaveIndexDescription");

    tsp00_Int4  rc = errOK_els00;

    tsp00_Addr pszDesc = NULL;
    tsp00_Int4 lDescLen = 0;
    szLocalErrString[0] = 0;

    rc = ls13GetIndexDescription(pDBInfo, 0, TableName, lDescLen, pszDesc, szLocalErrString);
    if (errOK_els00 == rc)
    {
        rc = ls26_WriteTableIndexDescToFile(pTabExtrRec, tptIndexDescription_els00, pszDesc, lDescLen, pszErrText);
    }

    return rc;
}
// ls26_SaveIndexDescription()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteTableIndexDescToFile
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteTableIndexDescToFile(tls00_TextractRec *pTERec,
                               tls00_TEPageType   DescriptionType,
                               tsp00_Addr         pDesc,
                               tsp00_Int4         lDescLength,
                               tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WriteTableIndexDescToFile");

    tsp00_Int4  rc        = errOK_els00;
    _TCHAR     *pszBuffer = pTERec->terpPageBuffer_ls00;

    tls26_DescPageHeader Header;
    Header.dphPageNo_ls26     = pTERec->terPageCount_ls00;
    Header.dphType_ls26       = MY_PAGE_LS00;
    Header.dphPageType_ls26   = DescriptionType;
    Header.dphFillerInt_ls26  = lDescLength;

    tsp00_Int4 lHeaderLength  = sizeof(tls26_DescPageHeader);
    tsp00_Int4 lMaxDataLength = pTERec->terPageSize_ls00 - lHeaderLength;
    tsp00_Int4 lWriteLength   = lDescLength;

    while (lWriteLength > 0)
    {
        Header.dphPageNo_ls26 += 1;
        if (lWriteLength <= lMaxDataLength)
        {
            Header.dphSpecialID_ls26 = lWriteLength;
            Header.dphMore_ls26      = 0;
        }
        else
        {
            Header.dphSpecialID_ls26 = lMaxDataLength;
            Header.dphMore_ls26      = 1;
        }

        // Fill buffer
        SAPDB_memcpy(pszBuffer, REINTERPRET_CAST(_TCHAR*, &Header), lHeaderLength);
        SAPDB_memcpy(&pszBuffer[lHeaderLength], &pDesc[lDescLength - lWriteLength], Header.dphSpecialID_ls26);

        // Flush buffer to file
        rc = ls30VFWriteLn(*pTERec->terOutFile_ls00, pszBuffer, pTERec->terPageSize_ls00, pszErrText);
        if (errOK_els00 != rc)
        {
            break;
        }

        lWriteLength -= Header.dphSpecialID_ls26;
    }   // end while (lWriteLength > 0)

    if (errOK_els00 == rc)
    {
        pTERec->terPageCount_ls00 = Header.dphPageNo_ls26;
    }
    return rc;
}
// ls26_WriteTableIndexDescToFile()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteConstraintInfo
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteConstraintInfo(tls00_DBInfo       *pDBInfo,
                         TableDef           *pTableDef,
                         tls00_TextractRec  *pTabExtrRec,
                         tsp00_Addr          pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WriteConstraintInfo");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR *pszBuffer = pTabExtrRec->terpPageBuffer_ls00;

    tls26_DescPageHeader Header;
    Header.dphPageNo_ls26   = pTabExtrRec->terPageCount_ls00;
    Header.dphType_ls26     = MY_PAGE_LS00;
    Header.dphPageType_ls26 = pTabExtrRec->terPageTypeExtracting_ls00;

    tsp00_Int4 lHeaderLength  = sizeof(tls26_DescPageHeader);
    tsp00_Int4 lMaxDataLength = pTabExtrRec->terPageSize_ls00 - lHeaderLength;   // max. length of data on single page
    tsp00_Int4 lBufLength     = lMaxDataLength;
    tsp00_Int4 lmyOffset      = lHeaderLength;                                   // offset in buffer


    TRACE_PRNF_MLS99(("ls26_WriteConstraintInfo", "Overall number of constraints: %d.\n",
                                                  pTabExtrRec->terConstraintCnt_ls00));

    // Column Constraint
    tsp00_Int2               lConstraintCountOnPage = 0;
    StudioTD_ConstraintDef  *pConstraint            = NULL;
    Tools_DynamicUTF8String  ConstraintName;
    Tools_DynamicUTF8String  ConstraintDefinition;
    tsp00_Int4               ConstraintNameLen        = 0;
    tsp00_Int4               ConstraintDefinitionLen  = 0;
    tsp00_Int4               lenlen                   = sizeof(tsp00_Int4);

    for (tsp00_Int4 i = 0; i < pTabExtrRec->terConstraintCnt_ls00; ++i)
    {
        pConstraint             = pTableDef->constraint(i);        // Get constraint from list

        ConstraintName          = pConstraint->name();
        ConstraintNameLen       = ConstraintName.BasisSize();
        ConstraintDefinition    = pConstraint->definition();
        ConstraintDefinitionLen = ConstraintDefinition.BasisSize();

        // Flush the buffer with filled constraint definitions to file if the buffer length is
        // exceeded when writing the next constraint info to buffer
        if ( (ConstraintDefinitionLen + ConstraintNameLen + lenlen + lenlen) > lBufLength )
        {
            // before writing the new value we must save the buffer to the file
            Header.dphPageNo_ls26    += 1;
            Header.dphSpecialID_ls26  =  lConstraintCountOnPage;

            if (i < (pTabExtrRec->terConstraintCnt_ls00 - 1) )
            {
                Header.dphMore_ls26 = 1;
            }
            else
            {
                Header.dphMore_ls26 = 0;
            }
            SAPDB_memcpy(&pszBuffer[0], REINTERPRET_CAST(_TCHAR*, &Header), lHeaderLength);

            TRACE_PRNF_MLS99(("ls26_WriteConstraintInfo", "page no: %d.\n", Header.dphPageNo_ls26));
            TRACE_PRNF_MLS99(("ls26_WriteConstraintInfo", "constraint cnt on page: %d.\n", Header.dphSpecialID_ls26));

            rc = ls30VFWriteLn(*pTabExtrRec->terOutFile_ls00, pszBuffer, pTabExtrRec->terPageSize_ls00, pszErrText);
            if (errOK_els00 != rc)
            {
                break;
            }

            // Reinitialize
            lmyOffset              = lHeaderLength;
            lBufLength             = lMaxDataLength;
            lConstraintCountOnPage = 0;

        }   // end if ( (lConstraintLen  + lLengthLen) > lBufLength )

        ++lConstraintCountOnPage;

        // Put length of constraints name into page (4 Bytes)
        SAPDB_memcpy(&pszBuffer[lmyOffset], REINTERPRET_CAST(char*, &ConstraintNameLen), lenlen);
        lBufLength -= lenlen;
        lmyOffset  += lenlen;

        SAPDB_memcpy(&pszBuffer[lmyOffset], REINTERPRET_CAST(char*, &ConstraintDefinitionLen), lenlen);
        lBufLength -= lenlen;
        lmyOffset  += lenlen;

        TRACE_PRNF_MLS99(("ls26_WriteConstraintInfo", "constraint name len = %d, constraint def len = %d\n",
                                                      ConstraintNameLen, ConstraintDefinitionLen));

        // write constraint name to buffer
        SAPDB_memcpy(&pszBuffer[lmyOffset], ConstraintName.StrPtr(), ConstraintNameLen);
        lBufLength -= ConstraintNameLen;
        lmyOffset  += ConstraintNameLen;

        // write constraint name to buffer
        SAPDB_memcpy(&pszBuffer[lmyOffset], ConstraintDefinition.StrPtr(), ConstraintDefinitionLen);
        lBufLength -= ConstraintDefinitionLen;
        lmyOffset  += ConstraintDefinitionLen;

    }   // end for (i = 1, lConstraintCountOnPage; i <= pTabExtrRec->terConstraintCnt_ls00; ++i, ++lConstraintCountOnPage)

    //*     Last writing of buffer to file
    //* only if there is still one that was not saved so far
    if ( (lmyOffset > lHeaderLength) && (errOK_els00 == rc) )
    {
        Header.dphPageNo_ls26    += 1;
        Header.dphSpecialID_ls26  = lConstraintCountOnPage;
        Header.dphMore_ls26       = 0;                       // definitely last contraint page

        SAPDB_memcpy(&pszBuffer[0], REINTERPRET_CAST(_TCHAR*, &Header), lHeaderLength);

        TRACE_PRNF_MLS99(("ls26_WriteConstraintInfo", "page no: %d.\n", Header.dphPageNo_ls26));
        TRACE_PRNF_MLS99(("ls26_WriteConstraintInfo", "constraint cnt on page: %d.\n", Header.dphSpecialID_ls26));

        rc = ls30VFWriteLn(*pTabExtrRec->terOutFile_ls00, pszBuffer, pTabExtrRec->terPageSize_ls00, pszErrText);
    }

    if (errOK_els00 == rc)
    {
        pTabExtrRec->terPageCount_ls00 = Header.dphPageNo_ls26;
    }

    return rc;
}
// ls26_WriteConstraintInfo()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteDefaultInfo
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteDefaultInfo(tls00_DBInfo      *DBInfo,
                      TableDef      *pTableDef,
                      tls00_TextractRec *TERec,
                      tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WriteDefaultInfo");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR *pszBuffer = TERec->terpPageBuffer_ls00;

    tls26_DescPageHeader Header;
    Header.dphPageNo_ls26   = TERec->terPageCount_ls00;
    Header.dphType_ls26     = MY_PAGE_LS00;
    Header.dphPageType_ls26 = TERec->terPageTypeExtracting_ls00;

    tsp00_Int4   lHeaderLength   = sizeof(tls26_DescPageHeader);
    tsp00_Int4   lMaxDataLength  = TERec->terPageSize_ls00 - lHeaderLength;
    tsp00_Uint4  lBufLength      = lMaxDataLength;
    tsp00_Int4   lmyOffset       = lHeaderLength;


    tsp00_Int2          lDefaultCountOnPage = 0;
    tsp00_Uint4         lDefaultCount       = pTableDef->defaultCount();
    tsp00_Uint4         lColCount           = pTableDef->columnCount();
    StudioTD_ColumnDef *pCol                = NULL;

    TRACE_PRNF_MLS99(("ls26_WriteDefaultInfo", "Overall number of defaults: %d.\n", lDefaultCount));


    // defaults are stored in a page as follows:
    //
    //------------------------------------------
    //| default 1 | default 2 | ... | default N|
    //------------------------------------------
    //
    // where each default is stored as follows:
    //
    //-------------------------------------------------------------------------------
    //|     default type     | no of col default | default len | default definition   |
    //| function or user def |  is defined for   |             |                      |
    //|        1 Byte        |     2 Byte        |    2 Byte   |  default len Byte    |
    //-------------------------------------------------------------------------------
    //
    //

    Tools_DynamicUTF8String Default;

    // fix size value of single default definition in page
    tsp00_Uint4             dlen   = sizeof(char) + sizeof(tsp00_Int2) + sizeof(tsp00_Int2);

    // variable size value of single default definition in page - single default value len
    tsp00_Uint2             sd_len = 0;

    //*
    //*     Go through all col defs and check for not null default values to save
    //*
    for (tsp00_Uint2 i = 0, lColNo = 0; i < lColCount; ++i)
    {
        pCol    = pTableDef->column(i);
        Default = pCol->defaultValue();

        if (SAPDB_FALSE == Default.Empty())
        {
            sd_len = Default.BasisSize();

            if ((dlen + sd_len) > lBufLength)
            {
                // write buffer to file before saving the next value
                Header.dphPageNo_ls26    += 1;
                Header.dphSpecialID_ls26  = lDefaultCountOnPage;
                Header.dphFillerInt_ls26  = lDefaultCount;

                if (i < lDefaultCount)
                {
                    Header.dphMore_ls26 = 1;
                }
                else
                {
                    Header.dphMore_ls26 = 0;
                }
                SAPDB_memcpy(&pszBuffer[0], REINTERPRET_CAST(_TCHAR*, &Header), lHeaderLength);

                TRACE_PRNF_MLS99(("ls26_WriteDefaultInfo", "page no: %d.\n", Header.dphPageNo_ls26));
                TRACE_PRNF_MLS99(("ls26_WriteDefaultInfo", "default cnt on page: %d.\n", lDefaultCountOnPage));


                rc = ls30VFWriteLn(*TERec->terOutFile_ls00, pszBuffer, TERec->terPageSize_ls00, pszErrText);
                if (errOK_els00 != rc)
                {
                    break;
                }

                //* reinitialize
                lmyOffset       = lHeaderLength;
                lBufLength      = lMaxDataLength;
                lDefaultCountOnPage = 0;
            }

            // Increase the default count on page only after checking for buffer flush
            ++lDefaultCountOnPage;

            // Save type of default
            pszBuffer[lmyOffset] = STATIC_CAST(char, pCol->IsDefaultUserDefinedValue());
            lBufLength -= sizeof(char);
            lmyOffset  += sizeof(char);

            // Save column number of default
            lColNo      = pCol->ColPos();
            SAPDB_memcpy(&pszBuffer[lmyOffset], REINTERPRET_CAST(char*, &lColNo), sizeof(tsp00_Uint2));
            lBufLength -= sizeof(tsp00_Uint2);
            lmyOffset  += sizeof(tsp00_Uint2);

            // Save length of default value
            SAPDB_memcpy(&pszBuffer[lmyOffset], REINTERPRET_CAST(char*, &sd_len), sizeof(tsp00_Uint2));
            lBufLength -= sizeof(tsp00_Uint2);
            lmyOffset  += sizeof(tsp00_Uint2);

            // Save default value
            SAPDB_memcpy(&pszBuffer[lmyOffset], Default.StrPtr(), sd_len);
            lBufLength -= sd_len;
            lmyOffset  += sd_len;

        }   // end if (SAPDB_FALSE == Default.Empty())

    }   // end for (i, lDefaultCountOnPage; i < TERec->terDefaultCnt_ls00; ++i, ++lDefaultCountOnPage)

    //*     Last time writing the buffer
    //* only if there is still one thats not yet saved
    if ( (lmyOffset > lHeaderLength) && (errOK_els00 == rc) )
    {
        Header.dphPageNo_ls26   += 1;
        Header.dphSpecialID_ls26 =  lDefaultCountOnPage;
        Header.dphFillerInt_ls26 = lDefaultCount;       // overall count of defaults for this table
        Header.dphMore_ls26      = 0;                   // surely the last default page

        SAPDB_memcpy(&pszBuffer[0], REINTERPRET_CAST(_TCHAR*, &Header), lHeaderLength);

        TRACE_PRNF_MLS99(("ls26_WriteDefaultInfo", "page no: %d.\n", Header.dphPageNo_ls26));
        TRACE_PRNF_MLS99(("ls26_WriteDefaultInfo", "default cnt on page: %d.\n", Header.dphSpecialID_ls26));

        rc = ls30VFWriteLn(*TERec->terOutFile_ls00, pszBuffer, TERec->terPageSize_ls00, pszErrText);
    }

    if (errOK_els00 == rc)
    {
        TERec->terPageCount_ls00 = Header.dphPageNo_ls26;
    }

    return rc;
}
// ls26_WriteDefaultInfo()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteShortStringPages
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteShortStringPages(tls00_DBInfo            *pDBInfo,
                           Tools_DynamicUTF8String &TableName,
                           tls00_TextractRec       *pTabExtrRec,
                           tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WriteShortStringPages");

    tsp00_Int4 rc = errOK_els00;

    szLocalCmdString[0] = 0;
    sp77sprintf(szLocalCmdString, lStmtLength_ls26, SAVE_LONG_STMT_LS26, TableName.StrPtr());

    tsp00_Int4 lErrPos;
    szLocalErrString[0] = 0;

    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szLocalCmdString, szLocalErrString, lErrPos);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternSQL_els98,
                                                                                szLocalCmdString, szLocalErrString);
        rc = errInternSQL_els98;
    }
    else
    {
        tsp1_packet* pSqlPacket = pDBInfo->dbiPktRcvList_ls00[0];
        //tsp1_part   *SqlPart   = NULL;

        rc = ls04GetBdinfo(pSqlPacket, &pTabExtrRec->terBdInfo_ls00);
        if (errOK_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                    __FILE__,
                                                                                    __LINE__,
                                                                                    errUnknownPart_els98);
            rc = errInternal_els98;
        }
        else
        {
            //*
            //*     Write intro page for short string data to data file
            //*
            rc = ls26_WDataHeaderPage(pTabExtrRec, pszErrText);
            if (errOK_els00 == rc)
            {
                //*
                //*     Get short string data pages
                //*
                Loader_RTStatistics* pStatObj = Loader_RTStatistics::Instance();
                pStatObj->SetCounter(0);

                rc = ls26_PageExtractLoop(pDBInfo, pTabExtrRec, pszErrText);

                pStatObj->SetPagesCounter(Loader_RTStatistics::SHORTLONGDATA, pStatObj->GetCounter());
                // Function returns error text, too
            }
        }
    }
    return rc;
}
// ls26_WriteShortStringPages()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WritePrimaryDataPages
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WritePrimaryDataPages(tls00_DBInfo            *pDBInfo,
                           Tools_DynamicUTF8String &TableName,
                           tls00_TextractRec       *pTabExtrRec,
                           tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WritePrimaryDataPages");

    tsp00_Int4 rc = errOK_els00;

    szLocalCmdString[0] = 0;

    sp77sprintf(szLocalCmdString, lStmtLength_ls26, SAVE_TABLE_STMT_LS26, TableName.StrPtr());

    tsp00_Int4 lErrPos;
    szLocalErrString[0] = 0;

    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szLocalCmdString, szLocalErrString, lErrPos);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternSQL_els98,
                                                                                szLocalCmdString, szLocalErrString);
        rc = errInternSQL_els98;
    }
    else
    {
        tsp1_packet* pSqlPacket = pDBInfo->dbiPktRcvList_ls00[0];
        //tsp1_part   *SqlPart   = NULL;

        rc = ls04GetBdinfo(pSqlPacket, &pTabExtrRec->terBdInfo_ls00);
        if (errOK_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                    __FILE__, __LINE__,
                                                                                    errUnknownPart_els98);
            rc = errInternal_els98;
        }
        else
        {
            // Store BDINFO for later reuse
            SAPDB_memcpy(pTabExtrRec->terLongBdInfo_ls00.szbdiData_ls00,
                         pTabExtrRec->terBdInfo_ls00.szbdiData_ls00,
                         pTabExtrRec->terBdInfo_ls00.bdiLength_ls00);
            pTabExtrRec->terLongBdInfo_ls00.bdiLength_ls00 = pTabExtrRec->terBdInfo_ls00.bdiLength_ls00;

            //*
            //*     Write intro page for primary data to data file
            //*
            rc = ls26_WDataHeaderPage(pTabExtrRec, pszErrText);
            if (errOK_els00 == rc)
            {
                //*
                //*     Get primary data pages
                //*
                Loader_RTStatistics* pStatObj = Loader_RTStatistics::Instance();
                pStatObj->SetCounter(0);

                rc = ls26_PageExtractLoop(pDBInfo, pTabExtrRec, pszErrText);

                pStatObj->SetPagesCounter(Loader_RTStatistics::PRIMARYDATA, pStatObj->GetCounter());
            }
        }
    }

    return rc;
}
// ls26_WritePrimaryDataPages()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteLongStringPages
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteLongStringPages(tls00_DBInfo      *pDBInfo,
                          tls00_TextractRec *pTabExtrRec,
                          tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WriteLongStringPages");

    tsp00_Int4 rc = errOK_els00;

    tsp00_Int4 lErrorPos = 0;
    szLocalCmdString[0] = 0;
    szLocalErrString[0] = 0;

    // Create command (because it doesn't contain identifiers it is already UTF8 encoded)
    _tcscpy(szLocalCmdString, SAVE_COLUMN_STMT_LS26);

    tls00_Parts4InternalSql myStruct;

    myStruct.pfiCount_ls00 = 3;
    myStruct.pfiPartKind_ls00    = new tsp1_part_kind_Enum[myStruct.pfiCount_ls00];
    myStruct.pfiPartKind_ls00[0] = sp1pk_command;
    myStruct.pfiPartKind_ls00[1] = sp1pk_bdinfo;
    myStruct.pfiPartKind_ls00[2] = sp1pk_data;


    myStruct.pfiData_ls00        = new tls00_String[myStruct.pfiCount_ls00];

    // cmd part
    myStruct.pfiData_ls00[0].strAddr_ls00 = szLocalCmdString;
    myStruct.pfiData_ls00[0].strLeng_ls00 = STATIC_CAST(tsp00_Int4, _tcslen(szLocalCmdString));

    // bdinfo part
    myStruct.pfiData_ls00[1].strAddr_ls00 = new _TCHAR[sizeof(pTabExtrRec->terLongBdInfo_ls00.szbdiData_ls00)];
    myStruct.pfiData_ls00[1].strLeng_ls00 = pTabExtrRec->terLongBdInfo_ls00.bdiLength_ls00;
    SAPDB_memcpy(myStruct.pfiData_ls00[1].strAddr_ls00, pTabExtrRec->terLongBdInfo_ls00.szbdiData_ls00, pTabExtrRec->terLongBdInfo_ls00.bdiLength_ls00);


    // data part
    // this part needs to be reinitialized for every long column to be extracted; to simplify
    // work we'll put a pointer to this part

    // folgendes aussehen
    //--------------------------------------------------------------------------------------
    //          |          |              |            |
    // FilePos  | long col | current long | key length | key
    //          |   cnt    |  col number  |            |
    //--------------------------------------------------------------------------------------
    //  8 byte      2 byte       2 byte      2 byte       (key length) byte

    tsp00_Int4    lNumPartLen = FILE_POS_MXGG00 + 3 * INT2_MXSP00;      // this length is used later on

    tls00_String *pMyData     = &myStruct.pfiData_ls00[2];
    pMyData->strLeng_ls00     = lNumPartLen + PERM_KEY_MXSP00;
    pMyData->strAddr_ls00     = new _TCHAR[myStruct.pfiData_ls00[2].strLeng_ls00];

    // first 8 byte are initially of no interest and are skipped therefore
    SAPDB_memcpy(&pMyData->strAddr_ls00[FILE_POS_MXGG00],
                 REINTERPRET_CAST(char*, &pTabExtrRec->terLongColCnt_ls00),
                 INT2_MXSP00);

    // next 10 bytes are initialized with 0
    // - first 2 bytes represent long_col_no (must initially be 0)
    // - next 8 byte are length representations in a record (record header) (and must initially also be 0)
    memset(&pMyData->strAddr_ls00[FILE_POS_MXGG00 + INT2_MXSP00], 0, (cgg_rec_key_offset + 2*INT2_MXSP00));

    while (1)
    {
        rc = ls13ExecIntSQLMultParts(pDBInfo, 0, myStruct, szLocalErrString, lErrorPos);
        if (errOK_els00 != rc)
        {
            if (100 == rc)      // no more LONGs to extract found by kernel
            {
                rc = errOK_els00;
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternSQL_els98,
                                                                                        szLocalCmdString, szLocalErrString);
                rc = errInternSQL_els98;
            }
            break;
        }

        tsp1_packet* pSqlPacket = pDBInfo->dbiPktRcvList_ls00[0];
        tsp1_part*   pSqlPart   = NULL;

        rc = ls04GetBdinfo(pSqlPacket, &pTabExtrRec->terBdInfo_ls00);
        if (errOK_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                    __FILE__, __LINE__,
                                                                                    errUnknownPart_els98);
            rc = errInternal_els98;
            break;
        }

        if ( false == ls04GetPart(pSqlPacket, sp1pk_key, pSqlPart) )
        {
            LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                    __FILE__, __LINE__,
                                                                                    errUnknownPart_els98);
            rc = errInternal_els98;
            break;
        }
        else
        {
            // right data here to run next ls13ExecIntSQLMultParts
            myStruct.pfiData_ls00[2].strLeng_ls00 = ls04GetPartLength(pSqlPart);
            ls04BufFromPartEx(pSqlPart, 0, pMyData->strLeng_ls00, pMyData->strAddr_ls00, pMyData->strLeng_ls00);
        }


        //*
        //*     Write intro page for long string data to data file
        //*
        //* Make colno and key length usable
        //*
        tsp00_Int2 lColNo = *REINTERPRET_CAST(tsp00_Int2*, &pMyData->strAddr_ls00[FILE_POS_MXGG00 + INT2_MXSP00]);
        tsp00_Int4 lKeyLen = *REINTERPRET_CAST(tsp00_Int2*, &pMyData->strAddr_ls00[FILE_POS_MXGG00 + INT2_MXSP00 + INT2_MXSP00]);

        rc = ls26_WLongDataHeaderPage(pTabExtrRec, lColNo, lKeyLen, &pMyData->strAddr_ls00[lNumPartLen], pszErrText);
        if (errOK_els00 != rc)
        {
            break;
        }

        //*
        //*     Get long string data pages
        //*
        Loader_RTStatistics* pStatObj = Loader_RTStatistics::Instance();
        pStatObj->SetCounter(0);

        rc = ls26_PageExtractLoop(pDBInfo, pTabExtrRec, pszErrText);

        pStatObj->IncreasePagesCounter(Loader_RTStatistics::LONGLONGDATA, pStatObj->GetCounter());

        if (errOK_els00 != rc)
        {
            break;
        }
    }   // end while (1)


    if (NULL != myStruct.pfiPartKind_ls00)
    {
        delete [] myStruct.pfiPartKind_ls00;
        myStruct.pfiPartKind_ls00 = NULL;
    }

    if (NULL != myStruct.pfiData_ls00[1].strAddr_ls00)
    {
        delete [] myStruct.pfiData_ls00[1].strAddr_ls00;
        myStruct.pfiData_ls00[1].strAddr_ls00 = NULL;
    }
    if (NULL != myStruct.pfiData_ls00[2].strAddr_ls00)
    {
        delete [] myStruct.pfiData_ls00[2].strAddr_ls00;
        myStruct.pfiData_ls00[2].strAddr_ls00 = NULL;
    }

    if (NULL != myStruct.pfiData_ls00)
    {
        delete [] myStruct.pfiData_ls00;
        myStruct.pfiData_ls00 = NULL;
    }

    return rc;
}
// ls26_WriteLongStringPages()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WLongDataHeaderPage
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WLongDataHeaderPage(tls00_TextractRec *pTabExtrRec,
                         tsp00_Int2         lColumnNo,
                         tsp00_Int4         lRecordKeyLen,         // nur fuer long-werte von interesse, fuer alle anderen aufrufer auf 0 setzen
                         tsp00_Addr         pszRecordKey,
                         tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WLongDataHeaderPage");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR *pszBuffer = pTabExtrRec->terpPageBuffer_ls00;        // der enthaelt eigentlich schon die laenge einer datenseite

    ++pTabExtrRec->terPageCount_ls00;

    tls26_DescPageHeader Header;
    Header.dphPageNo_ls26    = pTabExtrRec->terPageCount_ls00;
    Header.dphType_ls26      = MY_PAGE_LS00;
    Header.dphPageType_ls26  = pTabExtrRec->terPageTypeExtracting_ls00;;
    Header.dphSpecialID_ls26 = lRecordKeyLen;
    Header.dphMore_ls26      = 0;                           // actually of no use here
    Header.dphFillerInt_ls26 = lColumnNo;

    // write header to the beginning of page
    SAPDB_memcpy(pszBuffer, REINTERPRET_CAST(_TCHAR*, &Header), sizeof(tls26_DescPageHeader));

    // write record key to page (located behind header)
    SAPDB_memcpy(&pszBuffer[sizeof(tls26_DescPageHeader)], pszRecordKey, lRecordKeyLen);

    // write same header again to the END of page
    SAPDB_memcpy(&pszBuffer[pTabExtrRec->terPageSize_ls00 - sizeof(tls26_DescPageHeader)],
                 REINTERPRET_CAST(_TCHAR*, &Header),
                 sizeof(tls26_DescPageHeader));

    // write page to file
    rc = ls30VFWriteLn(*pTabExtrRec->terOutFile_ls00, pszBuffer, pTabExtrRec->terPageSize_ls00, pszErrText);

    return rc;
}
// ls26_WLongDataHeaderPage()


/*
  -----------------------------------------------------------------------------

  function:     ls26_WDataHeaderPage
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WDataHeaderPage(tls00_TextractRec *pTabExtrRec, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WDataHeaderPage");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR *pszBuffer = pTabExtrRec->terpPageBuffer_ls00;        // der enthaelt eigentlich schon die laenge einer datenseite

    ++pTabExtrRec->terPageCount_ls00;

    tls26_DescPageHeader Header;
    Header.dphPageNo_ls26    = pTabExtrRec->terPageCount_ls00;
    Header.dphType_ls26      = MY_PAGE_LS00;
    Header.dphPageType_ls26  = pTabExtrRec->terPageTypeExtracting_ls00;
    Header.dphSpecialID_ls26 = 0;                           // actually of no use here
    Header.dphMore_ls26      = 0;                           // actually of no use heredphFillerInt_ls26
    Header.dphFillerInt_ls26 = 0;                           // actually of no use here

    // Write header to beginning of page
    SAPDB_memcpy(pszBuffer, REINTERPRET_CAST(_TCHAR*, &Header), sizeof(tls26_DescPageHeader));

    TRACE_PRNF_MLS99(("ls26_WDataHeaderPage", "page size: %d.\n", pTabExtrRec->terPageSize_ls00));
    TRACE_BUFF_MLS99("ls26_WDataHeaderPage", "HEADER", sizeof(tls26_DescPageHeader), REINTERPRET_CAST(_TCHAR*, &Header));

    // Write same header again to the end of the page
    SAPDB_memcpy(&pszBuffer[pTabExtrRec->terPageSize_ls00 - sizeof(tls26_DescPageHeader)],
                 REINTERPRET_CAST(_TCHAR*, &Header),
                 sizeof(tls26_DescPageHeader));

    TRACE_BUFF_MLS99("ls26_WDataHeaderPage", "FOOTER", sizeof(tls26_DescPageHeader), REINTERPRET_CAST(_TCHAR*, &Header));

    // Write header page to file
    rc = ls30VFWriteLn(*pTabExtrRec->terOutFile_ls00, pszBuffer, pTabExtrRec->terPageSize_ls00, pszErrText);

    return rc;
}
// ls26_WDataHeaderPage()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteMetaTrailer
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26_WriteMetaTrailer(tls00_DBInfo             *pDBInfo,
                      Tools_DynamicUTF8String  &OwnerName,
                      Tools_DynamicUTF8String  &TableName,
                      tls00_TextractRec        *pTabExtrRec,
                      tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WriteMetaTrailer");

    tsp00_Int4 rc = errOK_els00;

    if ( NULL != pTabExtrRec->terpPageBuffer_ls00 )
    {
        memset(pTabExtrRec->terpPageBuffer_ls00, 0, pTabExtrRec->terPageSize_ls00 );

        // trailerlength
        size_t len = pTabExtrRec->terPageSize_ls00;

        // transformation statement
        Tools_DynamicUTF8String  transformation("LOAD TABLE TABLE ");

        transformation += "\"";
        transformation += OwnerName;
        transformation += "\".\"";
        transformation += TableName;
        transformation += "\"  DATA INSTREAM ";

        // datafile
        transformation += "'";
        transformation += REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, 
                                           pTabExtrRec->terOutFile_ls00->vfFileName_ls00);
        transformation += "' PAGES ";

        // <HEADER_BLOCK>:<HEADER_POS> (number strings have same encodings in ASCII and UTF8)
        _TCHAR HEADER_BLOCK[64];
        _TCHAR HEADER_POS  [64];
        HEADER_BLOCK[0] = 0;
        HEADER_POS  [0] = 0;
        _itot(STATIC_CAST(tsp00_Int4, pTabExtrRec->terHeaderFilePos_ls00.lnu2GB_Blocks_ls00), HEADER_BLOCK, 10);
        _itot(STATIC_CAST(tsp00_Int4, pTabExtrRec->terHeaderFilePos_ls00.lnuRemainder_ls00), HEADER_POS, 10);
        // <TRAILER_BLOCK>:<TRAILER_POS>
        _TCHAR TRAILER_BLOCK [64];
        _TCHAR TRAILER_POS   [64];
        TRAILER_BLOCK   [0] = 0;
        TRAILER_POS     [0] = 0;
        _itot(STATIC_CAST(tsp00_Int4, pTabExtrRec->terOutFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00), TRAILER_BLOCK, 10);
        _itot(STATIC_CAST(tsp00_Int4, pTabExtrRec->terOutFile_ls00->vfFilePos_ls00.lnuRemainder_ls00 ), TRAILER_POS  , 10);

        transformation += " START ";
        transformation += HEADER_BLOCK;
        transformation += ":";
        transformation += HEADER_POS;
        transformation += " ";
        transformation += TRAILER_BLOCK;
        transformation += ":";
        transformation += TRAILER_POS;

        // transformation statement
        SAPDB_memcpy( pTabExtrRec->terpPageBuffer_ls00, transformation.StrPtr(), transformation.BasisSize() );

        rc = ls30VFWriteLn( *pTabExtrRec->terOutFile_ls00, pTabExtrRec->terpPageBuffer_ls00, len , ErrText);
    }

    return rc;

}// ls26_WriteMetaTrailer()


/*
  -----------------------------------------------------------------------------
  function:     ls26_SendBdinfoPckt
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_SendBdinfoPckt(tls00_DBInfo *pDBInfo,
                    tsp00_Int4    lPcktNo,
                    tls00_BDInfo* pBDInfo,
                    tsp00_Addr    pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_SendBdinfoPckt");

    tsp00_Int4 rc = errOK_els00;

    tsp1_packet  *SqlPacket = pDBInfo->dbiPktSndList_ls00[lPcktNo];
    tsp1_segment *SqlSegm   = NULL;
    tsp1_part    *SqlPart   = NULL;

    //*
    //*     Initialize/fill packet for statement
    //* Setzt autocommit immer auf false fuer interne kommandos.
    //* Der sql mode wird in FastInit IMMER auf INTERNAL fuer interne kommandos gesetzt
    //*
    ls04FastInit(SqlPacket, &SqlSegm, &SqlPart, pDBInfo);    // sollte keine fehler produzieren
    
    ls04SetProducer(SqlSegm, sp1pr_internal_cmd);

    ls04SetMsgType(SqlSegm, sp1m_unload);

    ls04SetPartKind(SqlPart, sp1pk_bdinfo);

    // Eigentlich muesste fuer die folgende funktion der fehlercode abgefragt werden. Der kann
    // allerdings nur beinhalten, dass der puffer zu klein ist. Da das hier aber das einzige
    // ist, was wir eintragen, reicht der puffer in jedem fall aus.
    ls04BufToPart(SqlPart, pBDInfo->szbdiData_ls00, pBDInfo->bdiLength_ls00);

    rc = ls04FinishPart (SqlPacket, SqlPart);
    if (errOK_els00 == rc)
    {
        // Sende das paket
        rc = ls03SQLRequest (pDBInfo, lPcktNo, pszErrText);
        if (errOK_els00 == rc)
        {
            TRACE_PRNF_MLS99(("ls26_SendBdinfoPckt", "Sent PacketNo: %i.\n------------------------\n", lPcktNo));
        }
    }
    else
    {
        LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc));
        rc = errFinishPart_els98;
    }

    return rc;
}
//  ls26_SendBdinfoPckt()


/*
  -----------------------------------------------------------------------------
   function:   ls26_PageExtractLoop
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_PageExtractLoop(tls00_DBInfo      *pDBInfo,
                     tls00_TextractRec *pTabExtrRec,
                     tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_PageExtractLoop");
    
    LoaderMessages* pMsgObj = LoaderMessages::Instance();

    tsp00_Int4  rc = errOK_els00;

    tsp00_Int4 lSndPcktNo = 0;
    tsp00_Int4 lRcvPcktNo = 0;

    tsp00_Int4 nSndPcktCnt = 0;

    bool bCancelMsgWritten = false;

    //*  Main part of TABLEEXTRACT
    rc = ls26_SendBdinfoPckt(pDBInfo, lSndPcktNo, &pTabExtrRec->terBdInfo_ls00, pszErrText);
    if (errOK_els00 == rc)
    {
        nSndPcktCnt += 1;
        lSndPcktNo   = (lSndPcktNo + 1) % 2;

        tsp1_packet *SqlAuswertPacket = NULL;
        tsp1_part   *SqlAuswertPart   = NULL;

        tsp00_Int4 lLoopCount = 0;

        while (1)
        {
            if (lSndPcktNo != lRcvPcktNo)   // Check for receive-packet to get
            {
                //*     Get receive-SQL-Packet
                //  If successfully adapt number of packet to receive
                rc = ls13ReceiveBdinfoPckt(pDBInfo, lRcvPcktNo, &pTabExtrRec->terBdInfo_ls00, pszErrText);
                if (errOK_els00 != rc)
                {
                    pMsgObj->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                        __FILE__, __LINE__, STATIC_CAST(ErrorNumbers, rc));
                    rc = errInternal_els98;
                    break;
                }
            }
            // Write message in case of cancel request
            if (errCancelRequest_els98 == (rc = ls01CheckForCancel(pDBInfo->dbiSession_ls00.sipCancelByte_ls00) ))
            {
                pMsgObj->ls98Msg(layTableExtract_els98, pszErrText, errCancelRequest_els98);
                bCancelMsgWritten = true;
                break;
            }

            //*     send request-SQL-Packet
            //  If successfully adapt number of packet to send
            rc = ls26_SendBdinfoPckt(pDBInfo, lSndPcktNo, &pTabExtrRec->terBdInfo_ls00, pszErrText);
            if (errOK_els00 != rc)
            {
                pMsgObj->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                    __FILE__, __LINE__, STATIC_CAST(ErrorNumbers, rc));
                rc = errInternal_els98;
            }
            else
            {
                nSndPcktCnt += 1;
                lSndPcktNo   = (lSndPcktNo + 1) % 2;
            }

            //  Werte ein SQL-Packet aus
            SqlAuswertPacket = pDBInfo->dbiPktRcvList_ls00[lRcvPcktNo];
            SqlAuswertPart   = NULL;

            lRcvPcktNo = (lRcvPcktNo + 1) % 2;      // Setzen der naechsten empfangspaketnummer !

            rc = ls26_Analyse(SqlAuswertPacket, pTabExtrRec, pszErrText);
            if (errOK_els00 != rc)
            {
                if (STOP_ANALYSE_LS26 == rc)
                {
                    rc = errOK_els00;
                }
                break;
            }

            ++lLoopCount;
            TRACE_PRNF_MLS99(("ls26_PageExtractLoop", "LoopCount: %d\n", lLoopCount));
        }   // while loop
    }

    //*     letztes SQL-Packet
    //* bei diesem interessiert uns das resultat nicht mehr, da es sich um ein ueberbleibsel handelt
    if (errOK_els00 == rc)
    {
        tsp00_Int4 rc2 = ls13ReceiveBdinfoPckt(pDBInfo, lRcvPcktNo, &pTabExtrRec->terBdInfo_ls00, pszErrText);
        if (errOK_els00 != rc2)
        {
            LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                    __FILE__,
                                                                                    __LINE__,
                                                                                    STATIC_CAST(ErrorNumbers, rc2));
            rc = errInternal_els98;
        }
    }
    // Write message in case of cancel request (may be the cancel request
    // arrives only when receiving the last packet; in all other cases a msg is already written)
    if (errCancelRequest_els98 == ls01CheckForCancel(pDBInfo->dbiSession_ls00.sipCancelByte_ls00))
    {
        if (false == bCancelMsgWritten) {
            pMsgObj->ls98Msg(layTableExtract_els98, pszErrText, errCancelRequest_els98);
        }
        if (errOK_els00 == rc) {
            rc = errCancelRequest_els98;
        }
    }

    return rc;
}
//  ls26_PageExtractLoop()


/*
  -----------------------------------------------------------------------------
  function:     ls26_Analyse
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_Analyse(tsp1_packet_ptr    pSqlPacket,
             tls00_TextractRec *pTabExtrRec,
             tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_Analyse");

    tsp00_Int4  rc = errOK_els00;

    tsp00_Int4 lPageCnt = 0;
    tsp1_part *pSqlPart = NULL;
    if ( false == ls04GetPart(pSqlPacket, sp1pk_page, pSqlPart) )
    {
        LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                __FILE__,
                                                                                __LINE__,
                                                                                errUnknownPart_els98);
        rc = errInternal_els98;
    }
    else
    {
        tsp00_Int4 len = ls04GetPartLength(pSqlPart);
        if (0 == len)
        {
            rc = STOP_ANALYSE_LS26;
        }
        else
        {
            lPageCnt = len/pTabExtrRec->terPageSize_ls00;

            tsp00_Int4 lPageOffset  = 0;
            tsp00_Int4 lPageNo      = pTabExtrRec->terPageCount_ls00;
            tsp00_Addr pszBuffer    = NULL;
            tsp00_Addr pBufStartPos = NULL;

            int i = 0;
            for (i; i < lPageCnt; ++i)
            {
                lPageNo += 1;
                lPageOffset = i*pTabExtrRec->terPageSize_ls00;
                ls04BufPtrFromPart(pSqlPart, lPageOffset, pszBuffer);

                if (0 == i)
                {
                    pBufStartPos = pszBuffer;
                }

                // Redeclare buffer to db page and put neccessary information in;
                // to flag thats a db page (and not a loader page) page type is set to 0
                (REINTERPRET_CAST(tbd_node*, pszBuffer))->nd_id()  = lPageNo;
                (REINTERPRET_CAST(tbd_node*, pszBuffer))->nd_pt().becomes(ptNil_egg00);
            }

            rc = ls30VFWriteLn(*pTabExtrRec->terOutFile_ls00, pBufStartPos, len, pszErrText);
            if (errOK_els00 == rc)
            {
                pTabExtrRec->terPageCount_ls00 = lPageNo;
                Loader_RTStatistics::Instance()->IncreaseCounter(lPageCnt);
                if (lPageCnt < pTabExtrRec->terPgPerPckt_ls00)
                {
                    rc = STOP_ANALYSE_LS26;
                }
            }
        }
    }   // end of else of if ( false == ls04GetPart(pSqlPacket, sp1pk_page, pSqlPart) )

    return rc;
}
// ls26_Analyse()


/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*===========================================================================*
 *     END
 *===========================================================================*/
