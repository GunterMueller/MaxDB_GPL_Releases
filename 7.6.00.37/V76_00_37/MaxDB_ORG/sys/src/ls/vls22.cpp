/*! 
  -----------------------------------------------------------------------------
  module: vls22.cpp
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Fastload - filling db pages
                
  version:      7.6. 
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include "gls00.h"
#include "hls03.h"
#include "hls04.h"
#include "hls07.h"
#include "hls13.h"
#include "hls16_condition.h"
#include "hls18.h"
#include "hls20.h"
#include "hls22.h"
#include "hls23.h"
#include "hls30.h"
#include "hls98msg.h"
#include "hls99.h"
#include "gbd00.h"      // database bd layer (data page layout)
#include "ggg00.h"      // database bd layer (data page layout)

#define _INTERNAL_FILE_ID_   _T("ls22.cpp")

/*!
  -----------------------------------------------------------------------------
  Chapter: local functions definition
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  function:     ls22_PageLoadLoop
  -----------------------------------------------------------------------------
  description:  Main loop of fastload.
                Fills data pages, sends those pages and receives answers from db.
  
  arguments:    pTransformationCmd   [IN]   - fastload command structure
                FloadRec   [IN]   - fastload record structure
                DBInfo     [IN]   - dbinfo structure
                VFile      [IN]   - vfile structure
                pbRollback [OUT]  - indicates to roll back or commit inserted rows
                                     up to erroneous row in case of error
                ErrText    [OUT]  - error text

  returnvalue:  errOK_els00      - data successfully processed
                errors from db system

                Logs the error in log file.
  -----------------------------------------------------------------------------
*/
static int
ls22_PageLoadLoop(TransformationDef *pTransformationDef,
                  tls00_FloadRec*  pFloadRec,
                  tls00_DBInfo*    pDBInfo,
                  tls00_VFile*     pVFile,
                  SAPDB_Bool*      pbRollback,
                  tsp00_Addr       pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls22_FillPacket
  -----------------------------------------------------------------------------

  description:  Fills a packet with data from infile.
  
  arguments:    pTransformationCmd   [IN]   - fastload structure built by the parser from
                                     given command
                FloadRec   [IN]   - fastload structure built by the server
                DBInfo     [IN]   - dbinfo structure
                lPcktNo    [IN]   - number of request packet to fill
                VFile      [IN]   - data file structure
                pPcktStats [OUT]  - count of records and pages in packet
                ErrText    [OUT]  - error text

  returnvalue:  errOK_els00             - data loaded
                errors when loading data failed

                Logs the error in log file.
  -----------------------------------------------------------------------------
*/
int ls22_FillPacket(tls00_TransformationCmd*  pTransformationCmd,
                    tls00_FloadRec*           FloadRec,
                    tls00_DBInfo*             DBInfo,
                    tsp00_Int4                lPcktNo,
                    tls00_VFile*              VFile,
                    tls22_Statistics*         pPcktStats,
                    tsp00_Addr                ErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls22_ReceiveBdinfoPckt
  -----------------------------------------------------------------------------

  description:  Receives the answer packet for a sent request with data.
                Handles errors that could only be found by the kernel (like integrity
                violation). Handling includes repairing the pages and resending them.
                The function adapts the send statistics.
  
  arguments:    pDBInfo           [IN]      - DBinfo structure
                pFloadRec         [IN]      - Fastload record structure
                lRcvPcktNo        [IN]      - No of packet to receive
                pPacketStatistics [IN/OUT]  - Count of sent records, pages ... in that
                                               particular packet
                pSentValues       [IN/OUT]  - Count of sent records, pages ... at all
                                               until this packet had been send;
                                               adapted here
                pszErrText        [OUT]     - Error text

  returnvalue:  errOK_els00                    - data successfully processed
                errFL_IntegrityViolation_els98 - integrity violation errors exceed
                                                  MAXERRORCOUNT
                errors from db system when sending/receiving packet
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls22_ReceiveBdinfoPckt(tls00_DBInfo*      pDBInfo,
                       tls00_FloadRec*    pFloadRec,
                       SAPDB_Int4         lRcvPcktNo,
                       tls22_Statistics*  pPacketStatistics,
                       tls22_Statistics*  pSentValues,
                       tsp00_Addr         pszErrText);


/*!
  -----------------------------------------------------------------------------
   function:     ls22_RepairPacket
  -----------------------------------------------------------------------------
  description:  Removes invalid records from all pages in packet in case of integrity
                violation. This kind of error is reported by the kernel in a
                receive packet. The receive packet then contains the error and
                all data pages send in a request.
                The function adapts the send statistics of this particular packet.
  
  arguments:    pDBInfo            [IN]      - DBinfo structure
                pFloadRec          [IN]      - Fastload record structure
                lPcktNo            [IN]      - No of packet to receive
                pPacketStatistics  [IN/OUT]  - Count of sent records, pages ... in that
                                                particular packet;
                                                adapted here
                szErrText          [OUT]  - error text

  returnvalue:  errOK_els00            - data successfully processed
                STOP_NODATA_LS22       - no valid data in packet found; nothing resend
                errors from db system  - errors resending the packet

                Logs the error in log file.
  -----------------------------------------------------------------------------
*/
static int
ls22_RepairPacket(tls00_DBInfo*             pDBInfo,
                  tls00_FloadRec*           pFloadRec,
                  tsp00_Int4                lPcktNo,
                  tls22_Statistics*         pPacketStatistics,
                  Tools_DynamicUTF8String  &szErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls22_RepairPage
  -----------------------------------------------------------------------------
  description:  Removes invalid records from a single page in case of integrity
                violation. This kind of error is reported by the kernel in a
                receive packet. The receive packet then contains the error and
                all data pages send in a request.
                Logs erroneous infile line numbers.
                Adapts the send statistics of the packet containing for the
                processed page.
  
  arguments:    pDataPage             [IN]   - Page to process
                pFloadRec             [IN]   - fastload record structure
                pPacketStatistics     [IN]   - Count of sent records, pages ... in that
                                                particular packet which contains the
                                                processed page
                lLineNoBeforeFilling  [IN]   - Line no in infile before sending this
                                                packet to the kernel - necessary for
                                                correct error reporting
                szErrText             [OUT]  - Contains the erroneous line numbers

  returnvalue:  SAPDB_TRUE      - invalid records removed; there are valid records
                                  left on that page
                SAPDB_FALSE     - no valid records on that page at all after repair
  -----------------------------------------------------------------------------
*/
static SAPDB_Bool
ls22_RepairPage(tbd_node*                 pDataPage,
                tls00_FloadRec*           pFloadRec,
                tls22_Statistics*         pPacketStatistics,
                tsp00_Int8&               lLineNoBeforeFilling,
                Tools_DynamicUTF8String  &szErrText);



/*!
  -----------------------------------------------------------------------------
  End Chapter: local functions definition
  -----------------------------------------------------------------------------
*/


/*!
  -----------------------------------------------------------------------------
  Chapter: global functions
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls20UpdateTransformationPackage
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls22UpdateTransformationPackage( tls00_DBInfo      *pDBInfo,
                                 TransformationDef *pTransformationDef,
                                 tls00_FloadRec*   pFloadRec,
                                 tsp00_Addr        ErrText)
{

	ROUTINE_DBG_MEO00 ("ls22UpdateTransformationPackage");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    tsp00_Int4 rc = errOK_els00;
	tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();

    if ( pTransformationCmd->trPartSpec_ls00.psPartType_ls00 >= ptTable_els00 )
    {
         SAPDB_Int4       ErrorPos   = 0;
         SQLRETURN        ReturnCode = 0;
         StudioOAL_String NativeErrorAsString;
         //*
         //*  performance
         //*
         extern tls00_Performance performance;
         SAPDB_UInt8 start, finish;
		 SAPDB_UInt8 duration;
         //*
         //*  update transformation table columns:
         //*  taskid, storagedevice, servernode, serverdb, username, codetype, codepage, transformation, part, streamformat
         //*
        _TCHAR RECORD_LOADED[256];
        _TCHAR DATA_BLOCK   [256];
        _TCHAR DATA_POS     [256];
        RECORD_LOADED       [0]=0;
        DATA_BLOCK          [0]=0;
        DATA_POS            [0]=0;
        //*
        //*  datastreamheaderpos,datastreamheaderblock,datastreamdatapos
        //*
        if (pFloadRec->flrTableSwitched_ls00  == true )
        {
            pFloadRec->flrTableSwitched_ls00  = false;
            // TIMESTAMP BEGIN
            if ( !pTransformationDef->updateBegin() )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layDataLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
			// <CATALOGSTREAMNAME>
			if ( !pTransformationDef->updateCatalogCmd())
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
			// <DATASTREAM>
			if ( !pTransformationDef->updateDataCmd())
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
            // <DATASTREAMNAME>
            pTransformationDef->m_datastreamname = pFloadRec->flrpInfile_ls00->vfFileName_ls00;

			if ( !pTransformationDef->updateDataStream() )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
        }// if (pFloadRec->flrTableSwitched_ls00  == true )

        // RECORD_LOADED
        _itot(STATIC_CAST(tsp00_Int4, pFloadRec->flrRecordCount_ls00),RECORD_LOADED,10);
        // <DATA_BLOCK>:<DATA_POS>
        _itot(STATIC_CAST(tsp00_Int4, pFloadRec->flrpInfile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00),DATA_BLOCK,10);
        _itot(STATIC_CAST(tsp00_Int4, pFloadRec->flrpInfile_ls00->vfFilePos_ls00.lnuRemainder_ls00 ),DATA_POS,10);
        pTransformationDef->m_rows_imported = RECORD_LOADED;
		pTransformationDef->m_datapos_block = DATA_BLOCK;
		pTransformationDef->m_datapos_byte  = DATA_POS;
        start = RTESys_MicroSecTimer();
        // Import
        if ( !pTransformationDef->updateImportData() )
        {
            ReturnCode          = pTransformationDef->getReturnCode();
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layDataLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            return ReturnCode;
        }
        finish   = RTESys_MicroSecTimer();
		duration = finish - start;
        performance.perLog_ls00 += duration;

        if (//pFloadRec->flrTabEmpty_ls00      == true ||
            pFloadRec->flrTabLoaded_ls00       == true ||
            pTransformationCmd->trExclude_ls00 == true    )
        {
            // <EXCLUDE>
            if ( pTransformationCmd->trExclude_ls00 == true )
            {
                if ( !pTransformationDef->updateExclude() )
                {
                    ReturnCode          = pTransformationDef->getReturnCode();
                    NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                    pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    return ReturnCode;
                }
            }
            // RESTART
            if ( !pTransformationDef->updateRestart() )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layDataLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
            // TIMESTAMP END
            if ( !pTransformationDef->updateEnd() )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layDataLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
            // SYSTEM TIME
            if ( !pTransformationDef->updatePerformance() )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
       }// if (pFloadRec->flrTabEmpty_ls00 == true)
    }

    return rc;

}//ls22UpdateTransformationPackage

/*
  -----------------------------------------------------------------------------
  function:     ls22LoadPages
  -----------------------------------------------------------------------------
*/
int ls22LoadPages(TransformationDef *pTransformationDef,
                  tls00_FloadRec*   FloadRec,
                  tls00_DBInfo*     DBInfo,
                  SAPDB_Bool*       pbRollback,
                  tsp00_Addr        ErrText)
{
    ROUTINE_DBG_MEO00 ("ls22LoadPages");

    tsp00_Int4 rc = errOK_els00;
    
	tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_FileSpec *pInFileSpec = &pTransformationCmd->trDataFile_ls00;
    tls00_VFile    *pVFile      = FloadRec->flrpInfile_ls00;

    tsp00_Int4 lByteCount = ls18GetRowLength(pTransformationCmd->trColSpec_ls00,
                                             FloadRec->flrTableInfo_ls00,
                                             pInFileSpec->fsFormat_ls00.ffoFormatted_ls00);

    if (true == pInFileSpec->fsFormat_ls00.ffoBinary_ls00)
    {
        if (0 == pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00)       // NOHEADER not found in command
        {
            rc = ls30VFReadHeader(*pVFile, pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00, ErrText);
            if (errOK_els00 != rc)
            {
                if (errNoDatafileHeader_ls98 == rc)
                {
                    pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00 = lByteCount;
                    LoaderMessages::Instance()->ls98PrintMessage(layFastload_els98,
                                                                 errNoDatafileHeader_ls98,
                                                                 pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00);
                    rc = errOK_els00;
                }
                // In case of file read error we stop at this point;
                // if only no header log it and calculate length
            }
        }
        // else is done because value is already set
    }
    else
    {
        pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00 = lByteCount;
    }

    //*
    //*     Check for loading a part of the file only
    //*
    if (errOK_els00 == rc)
    {
        if ( (UNDEFINED_LS00 != pInFileSpec->fsExtract_ls00.feStart_ls00.lnuRemainder_ls00) &&
             (0              != pInFileSpec->fsExtract_ls00.feStart_ls00.lnuRemainder_ls00) )   // in case of 0 we're already on the right pos
        {
            tls00_LargeNumber lFilePos;
            lFilePos.lnu2GB_Blocks_ls00 = 0;

            if (false == pInFileSpec->fsFormat_ls00.ffoBinary_ls00)  // text file
            {
                lFilePos.lnuRemainder_ls00 = pInFileSpec->fsExtract_ls00.feStart_ls00.lnuRemainder_ls00;
            }
            else
            {
                lFilePos.lnuRemainder_ls00 = pInFileSpec->fsFormat_ls00.ffoBytesPerLine_ls00 * (pInFileSpec->fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 - 1);
            }
            rc = ls30VFMoveToStartPosition(*pVFile, &lFilePos, ErrText);
        }
    }


    //*
    //*     Main working routine for loading fast
    //*
    //* We need to catch the case when the user specified a START position in infile and
    //* a count of rows to load but this count is 0.
    //*
    if (errOK_els00 == rc)
    {
        if ( (UNDEFINED_LS00 == pInFileSpec->fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00) ||
             (0               < pInFileSpec->fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00) )
        {
            rc = ls22_PageLoadLoop(pTransformationDef, FloadRec, DBInfo, pVFile, pbRollback, ErrText);
        }
        else if (0 == pInFileSpec->fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00)
        {
            FloadRec->flrLastCommLine_ls00 = 0;
        }
    }

    return rc;
}
//  ls22LoadPages()

/*!
  -----------------------------------------------------------------------------
  EndChapter: global functions
  -----------------------------------------------------------------------------
*/


/*---------------------------------------------------------------------------*/


/*!
  -----------------------------------------------------------------------------
  Chapter: local functions
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls22_PageLoadLoop
  -----------------------------------------------------------------------------
*/
static int
ls22_PageLoadLoop(TransformationDef *pTransformationDef,
                  tls00_FloadRec*    pFloadRec,
                  tls00_DBInfo*      pDBInfo,
                  tls00_VFile*       pVFile,
                  SAPDB_Bool*        pbRollback,      // meaningful in case of error only; indicates rolling back or committing
                  tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls22_PageLoadLoop");
    LoaderMessages* pMsgObj = LoaderMessages::Instance();

	tls00_TransformationCmd * pTransformationCmd = pTransformationDef->getTransformationCmd();

    tsp00_Int4  rc                 = errOK_els00;
    tsp00_Int4  rcFill             = errOK_els00;
    SAPDB_Int4  rcCheck            = errOK_els00;
    tsp00_Int4  lSndPcktNo         = 0;
    tsp00_Int4  lRcvPcktNo         = 0;
    tsp00_Int4  nSndPcktCnt        = 0;

    SAPDB_Int4  _lAdmissibleErrNo  = pFloadRec->flrLRejected_ls00;

    // Count of sent records, pages and last sent key for single packets
    tls22_Statistics PcktStats[2];
    PcktStats[0].lPageCount_ls22   = 0;
    PcktStats[0].lRecordCount_ls22 = 0;
    PcktStats[1].lPageCount_ls22   = 0;
    PcktStats[1].lRecordCount_ls22 = 0;

    // Count of sent records, pages and last sent key for the whole FASTLOAD process of a single table
    tls22_Statistics SentValues;
    SentValues.lLastKeyLength_ls22 = 0;
    SentValues.lPageCount_ls22     = 0;
    SentValues.lRecordCount_ls22   = 0;

    //*  MAIN part of fastload
    while ( (errOK_els00 == rcFill) && (errOK_els00 == rc) )
    {
        //*
        //*     Fill SQL-Packet (by building pages and filling them into the packet)
        //*
        rcFill = ls22_FillPacket(pTransformationCmd, pFloadRec, pDBInfo, lSndPcktNo, pVFile, &PcktStats[lSndPcktNo], pszErrText);
        if (lSndPcktNo != lRcvPcktNo)
        {
            //*
            //*     Get answer packet
            //*
            rc = ls22_ReceiveBdinfoPckt(pDBInfo, pFloadRec, lRcvPcktNo, &PcktStats[lRcvPcktNo], &SentValues, pszErrText);
            if (errOK_els00 == rc)
            {
                // Reset values
                PcktStats[lRcvPcktNo].lPageCount_ls22   = 0;
                PcktStats[lRcvPcktNo].lRecordCount_ls22 = 0;
            }
            // Adapt received packet number in any case; this prevents also from
            // receiving a packet in case of error (where we don't send one again)
            lRcvPcktNo = (lRcvPcktNo + 1) % 2;
        }
        
        //*
        //*     Send packet
        //* even in case of error if at least one record could be placed on a db page
        //*
        if ( (rcFill != STOP_NODATA_LS22) && (errOK_els00 == rc) )
        {
            // In case there is no data page build (error while building first page to send)
            // we can't send any page because the kernel crashes over an empty page; this
            // may ONLY happen in case of errors but should always be checked
            if (PcktStats[lSndPcktNo].lPageCount_ls22 > 0)
            {
                rc = ls13SendBdinfoPckt(pDBInfo, lSndPcktNo, &pFloadRec->flrBdInfo_ls00, pszErrText);
                if (errOK_els00 == rc)
                {
                    nSndPcktCnt += 1;

                    // Adapt pFloadRec statistic values
                    pFloadRec->flrRecordCount_ls00 += PcktStats[lSndPcktNo].lRecordCount_ls22;
                    pFloadRec->flrPageCount_ls00   += PcktStats[lSndPcktNo].lPageCount_ls22;

                    //*
                    //*  update transformation table
                    //*
                    if ( errOK_els00 == rc )
                         rc = ls22UpdateTransformationPackage(pDBInfo,pTransformationDef,pFloadRec,pszErrText); 

                    lSndPcktNo = (lSndPcktNo + 1) % 2;
                }
            }
        }   // end if ( (rcFill != STOP_NODATA_LS22) && (errOK_els00 == rc) )
    }   // end while ( (errOK_els00 == rcFill) && (errOK_els00 == rc) )

    // Create an error message if something happened during sending/receiving the packages
    if ( (errOK_els00 != rc) && (errFL_IntegrityViolation_els98 != rc) )
    {
        pMsgObj->ls98Msg(layFastload_els98, pszErrText, errInternal_els98,_INTERNAL_FILE_ID_,
                                                                          __LINE__,
                                                                          STATIC_CAST(ErrorNumbers, rc));
        rc = errInternal_els98;
    }

    //*
    //*     Check in case of no (more) data found in data file if there was any and
    //*     adapt the return code respectively
    //*
    if (STOP_NODATA_LS22 == rcFill)
    {                                           
        if (0 == pFloadRec->flrRecordCount_ls00)
        {   // Nothing sent - error?
            if (pFloadRec->flrLRejected_ls00 != _lAdmissibleErrNo)      // There has been data, but erroneous
            {
                char* pszTabName = NULL;
                if (NULL != pTransformationCmd)
                {
                    pszTabName = pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp();
                }

                if (NULL != pszTabName)
                {
                    pMsgObj->ls98Msg(layFastload_els98, pszErrText, errDLNoDataLoaded_els98, pszTabName);
                }
                else
                {
                    pMsgObj->ls98Msg(layFastload_els98, pszErrText, errDLNoDataLoaded_els98,
                                                                    pFloadRec->flrTableInfo_ls00.tdTablename_ls00.asCharp());
                }
                rc = errDLNoDataLoaded_els98;
            }
            else                                                        // No data at all in data file
            {
                pMsgObj->ls98Msg(layFastload_els98, pszErrText, msgEmptyDataFile_els98, pVFile->vfFileName_ls00);
                rc = msgEmptyDataFile_els98;
            }
        }
        else
        {
            rc = rcFill = errOK_els00;   // Ok, already data sent; end with ok
        }
    }

    //*
    //*      last packet to receive
    //* Because the communication to the db is synchronous we MUST receive a packet
    //*
    if ( (0 != nSndPcktCnt) && (lSndPcktNo != lRcvPcktNo) )
    {
        tsp00_Int4 rc2 = ls22_ReceiveBdinfoPckt(pDBInfo,
                                                pFloadRec,
                                                lRcvPcktNo,
                                                &PcktStats[lRcvPcktNo],
                                                &SentValues,
                                                pszErrText);
        if ( (errOK_els00 != rc2) && (errFL_IntegrityViolation_els98 != rc) && (errOK_els00 == rc) )
        {
            pMsgObj->ls98Msg(layFastload_els98, pszErrText, errInternal_els98, _INTERNAL_FILE_ID_,
                                                                               __LINE__,
                                                                               STATIC_CAST(ErrorNumbers, rc2));
            rc = errInternal_els98;
        }
    }   // end if ( (0 != nSndPcktCnt) && (lSndPcktNo != lRcvPcktNo) )


    if (errOK_els00 == rc)
    {
        if ( (_lAdmissibleErrNo - pFloadRec->flrLRejected_ls00) && (_lAdmissibleErrNo > 1) )
        {
            char szLogfileName[256];
            szLogfileName[0] = '\0';
            pMsgObj->ls98GetLogFileName(szLogfileName);
            pMsgObj->ls98MsgToString(layFastload_els98, pszErrText, msgErrNo_LT_MaxAdmissibleErrNo_els98, szLogfileName);
        }
        if ( (errOK_els00 != rcFill) && (STOP_LS22 != rcFill) )
        {
            *pbRollback = SAPDB_FALSE;
            rc = rcFill;
        }
    }
    else
    {
        if (errFL_IntegrityViolation_els98 == rc)
        {
            *pbRollback = SAPDB_FALSE;
        }
    }

    if ( (errOK_els00 == rc) || (SAPDB_FALSE == *pbRollback) )
    {
        pFloadRec->flrLastCommLine_ls00  = SentValues.lRecordCount_ls22;
        pFloadRec->flrRecordCount_ls00   = SentValues.lRecordCount_ls22;
        pFloadRec->flrPageCount_ls00     = SentValues.lPageCount_ls22;
        if (pFloadRec->flrRecordCount_ls00 > 0) {
            pFloadRec->flrLenKeyAcp_ls00     = SentValues.lLastKeyLength_ls22;
            SAPDB_memcpy(pFloadRec->flrLstKeyAcp_ls00, SentValues.LastKey_ls22, pFloadRec->flrLenKeyAcp_ls00);
        }
    }

    return rc;
}
//  ls22_PageLoadLoop()


/*
  -----------------------------------------------------------------------------
   function:     ls22_FillPacket
  -----------------------------------------------------------------------------
*/
int ls22_FillPacket(tls00_TransformationCmd*  pTransformationCmd,
                    tls00_FloadRec*    pFloadRec,
                    tls00_DBInfo*      pDBInfo,
                    tsp00_Int4         lPcktNo,
                    tls00_VFile*       pVFile,
                    tls22_Statistics   *pPcktStats,
                    tsp00_Addr         ErrText)
{
   ROUTINE_DBG_MEO00 ("ls22_FillPacket");

    tsp00_Int4   ErrCode       = errOK_els00;
    tsp00_Int4   rcREAD        = errOK_els00;

    tsp1_packet *SqlPacket     = NULL;
    tsp1_part   *SqlPart_page  = NULL;
    tsp00_Int4   ListTop       = 0;
    tsp00_Int4   Bottom        = 0;
    tsp00_Int4   Space         = 0;
    tsp00_Int4   PageNo        = 0;
    tsp00_Int4   nPageRecCount = 0;
    tsp00_Int4   RecordLen     = 0;
    tsp00_Addr   Buffer        = NULL;

    tsp00_SwapKind_Enum SSwap   = pFloadRec->flrSwapKind_ls00;
    tsp00_SwapKind_Enum DSwap   = pFloadRec->flrTableInfo_ls00.tdKernelSwap_ls00;

    tls00_FileSpec *pInFileSpec = &pTransformationCmd->trDataFile_ls00;
    tls00_String   DataRecord;
    DataRecord.strAddr_ls00 = STATIC_CAST(char*, pFloadRec->flrDataSource_ls00.dasInputBuffer.pBufPtr_ls00);

    tls00_String   *RawData     = pFloadRec->flrDataSource_ls00.dasConvertedData;

    tbd_node       pDataPage;
    tls00_PageInfo PGI        = pFloadRec->flrPageInfo_ls00;
    SAPDB_Bool     bLimitReached = SAPDB_FALSE;


    //*
    //*     Prepare packet to send
    //*
    ls13Prepare_FL_TL_Packet(pDBInfo, lPcktNo, SqlPacket, SqlPart_page);

    //*
    //*     2 while loops to fill the data pages
    //*
    //* - 1st loop goes over the count of pages to fill
    //* - 2nd loop is used to fill a single page
    //*
    while ( (errOK_els00 == ErrCode) && (PageNo < pFloadRec->flrPgPerPckt_ls00) )
    {
        pDataPage.nd_full_header().SetZero();               // Initialize page header

        Bottom        = PGI.pgiDataStartPos;
        ListTop       = PGI.pgiDataEndPos;
        Space         = PGI.pgiDataSpace;
        Buffer        = STATIC_CAST(char*, pDataPage.nd_body());  // Offset is 0 for nd_body() !!!
        nPageRecCount = 0;
        bLimitReached = SAPDB_FALSE;

        //*
        //*     Read data from file and insert into page
        //*
        while ( (Space > pFloadRec->flrRecordLen_ls00) && (errOK_els00 == ErrCode) && (SAPDB_FALSE == bLimitReached) )
        {
            rcREAD = ls20ReadData(pVFile,
                                  &pTransformationCmd->trDataFile_ls00.fsFormat_ls00,
                                  &pFloadRec->flrPattern_ls00,
                                  NULL,
                                  &pFloadRec->flrDataSource_ls00.dasInputBuffer,
                                  &pFloadRec->flrTabLoaded_ls00,
                                  (0== pFloadRec->flrRecordCount_ls00),
                                  ErrText);
            if (rcREAD < 0)
            {
                // following values may happen:
                // - STOP_LAST_DATA_LS00    --> process read record and then stop processing (only for RECORDS format of data file)
                // - STOP_NOMORE_DATA_LS00  --> nothing more to process - get outa here
                // - error code             --> nothing more to process - get outa here
                if (STOP_NOMORE_DATA_LS00 != rcREAD)
                {   // In case of error we stop immediately.
                    ErrCode = rcREAD;
                }
                else// if (STOP_NOMORE_DATA_LS00 == rcREAD)
                {   // In case of EOF ErrCode remains OK and the started page will be finished.

                    if (0 == nPageRecCount )
                    {   // eof while reading first data record
                        ErrCode = STOP_NODATA_LS22;
                    }
                    else
                    {
                        ErrCode = STOP_LS22;
                    }
                }
                break;
            }

            //*
            //*     Check for condition in command
            //*
            if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00)
            {
                if ( false == ls16CheckCondition(*pTransformationCmd->trTable_ls00.tsCondSpec_ls00,
                                                 DataRecord,
                                                 pInFileSpec->fsFormat_ls00) )
                {
                    continue;
                }
            }

            //*
            //*     Insert record into data page
            //*
            RecordLen = ls23BuildRecord(Buffer, pTransformationCmd, pFloadRec, pDBInfo->dbiSession_ls00.siSQLMode_ls00, ErrText);
            
            ls18RecordInit(pFloadRec->flrDataSource_ls00.lReadFieldsCnt_ls00,
                           RawData,
                           STATIC_CAST(char*, pFloadRec->flrDataSource_ls00.dasInputBuffer.pBufPtr_ls00),
                           pFloadRec->flrDataSource_ls00.dasInputBuffer.lBufPos_ls00);

            if (0 > RecordLen)      // Error while processing record
            {
                if (sp5vf_text == pVFile->vfDataType_ls00)
                {
                    tsp00_Int8 ErrorLine = (UNDEFINED_LS00 == pInFileSpec->fsExtract_ls00.feStart_ls00.lnuRemainder_ls00) ?
                                            pVFile->vfFileLineNum_ls00 :
                                            pVFile->vfFileLineNum_ls00 + pInFileSpec->fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 - 1;
                    LoaderMessages::Instance()->ls98PrintMessage(layGRec_els98, errAtRow2_els98, ErrorLine);
                }
                else
                {
                    tsp00_Int8 ErrorPos = pVFile->vfFilePos_ls00.lnu2GB_Blocks_ls00*MAX_INT4_SP00 +
                                           pVFile->vfFilePos_ls00.lnuRemainder_ls00;
                    LoaderMessages::Instance()->ls98PrintMessage(layGRec_els98, errAtPos2_els98, ErrorPos);
                }

                pFloadRec->flrLRejected_ls00 -= 1;
                if (pFloadRec->flrLRejected_ls00 <= 0)
                {
                    ErrCode = RecordLen;
                    break;
                }
                else
                {
                    continue;
                }
            }

            ListTop -= POINTERSIZE_BD00;    // update positions

            //  Write pointer to the end of the data list
            ls07Swap4BIntToChar(Bottom + 1, SSwap, DSwap,
                                REINTERPRET_CAST( char*, &pDataPage.nd_body()[ListTop - PGI.pgiDataStartPos]) );

            Space  -= POINTERSIZE_BD00;     // update available space
        
            Bottom += RecordLen;
            Space  -= RecordLen;
            if ( 0 != (RecordLen%2) )       // 2 byte alignment of record (only the record pointer is changed
            {                               // not the record length)
                Bottom += 1;
                Space  -= 1;
            }
            nPageRecCount += 1;             // Update number of records written to table

            //*
            //*     Check for user specified filling level for a single database page (in rows)
            //*
            if (true == pTransformationCmd->trUsage_ls00.uUsingRows_ls00)
            {
                if (nPageRecCount == pTransformationCmd->trUsage_ls00.uUsageValue_ls00)
                {
                    bLimitReached = SAPDB_TRUE;
                }
            }

            //*
            //*     Check for user specified START position in data file
            //*
            //* If the user specified a START position he may have defined a count of
            //* rows to insert. This is checked here as a stop option.
            //*
            if (UNDEFINED_LS00 != pInFileSpec->fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00)
            {
                if (pInFileSpec->fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00 == 
                            (pFloadRec->flrRecordCount_ls00 + nPageRecCount + pPcktStats->lRecordCount_ls22) )
                {
                    ErrCode       = STOP_LS22;
                    bLimitReached = SAPDB_TRUE;
                }
            }

            // Reassign Buffer to the next free byte in nd_body() - ATTENTION: Initial offset of nd_body
            // is 0, so we have to substact the bytes for the header from the new offset all the time
            Buffer = REINTERPRET_CAST( char*, &pDataPage.nd_body()[Bottom - PGI.pgiDataStartPos] );
        
        }   // end while ( (Space > pFloadRec->flrRecordLen_ls00) && (errOK_els00 == ErrCode) )

        TRACE_PRNF_MLS99(("ls22_FillPacket", "%d rows at page.\n", nPageRecCount));


        // Update page header values even in case of errors. If we encounter errors while
        // filling the page there might have been SOME records that were error free that can
        // be send to the kernel.
        // But sending completely empty pages crashes the kernel upon inserting so we should
        // be careful here.
        if ( (ErrCode != STOP_NODATA_LS22) && (0 != nPageRecCount) )    // No record -> no page to build
        {
            //*
            //*     update record header values
            //*
            pDataPage.nd_record_cnt() = ls07Swap4BInt(nPageRecCount, SSwap, DSwap);
            pDataPage.nd_bottom()     = ls07Swap4BInt(Bottom + 1, SSwap, DSwap);
            pDataPage.nd_sorted()     = true;

            //*
            //*     Write filled page to part
            //*
            ls04BufToPart(SqlPart_page, REINTERPRET_CAST(char*, &pDataPage), pDBInfo->dbiPageSize_ls00);
            PageNo += 1;

            pPcktStats->lRecordCount_ls22 += nPageRecCount; // adapt count of inserted records

            pPcktStats->lPageCount_ls22   += 1;             // adapt count of sent pages
        }   // end if ( (ErrCode != STOP_NODATA_LS22) && (0 != nPageRecCount) ) // No record -> no page to build

        //TRACE_BUFF_MLS99 ("ls22_FillPacket", "PAGE HEADER", sizeof(tbd_node_header), pDataPage.nd_full_header());
        //TRACE_BUFF_MLS99 ("ls22_FillPacket", "PAGE BODY", PGI.pgiDataEndPos - PGI.pgiDataStartPos, pDataPage.nd_body());
    }   // end while ( (EOF_LS00 != pVFile->vfFilePos_ls00) && ...

    
    // Finish page part of the packet
    if (0 != PageNo)            // no EOF without data
    {
        tsp00_Int4 rc2 = ls04FinishPart(SqlPacket, SqlPart_page);
        if (errOK_els00 != rc2)
        {
            ErrCode = rc2;
        }
        else
        {
            if (STOP_NODATA_LS22 == ErrCode)
            {
                ErrCode = STOP_LS22;
            }
        }

        // Adapt key
        pPcktStats->lLastKeyLength_ls22 = pFloadRec->flrLenKeyAcp_ls00;
        SAPDB_memcpy(pPcktStats->LastKey_ls22, pFloadRec->flrLstKeyAcp_ls00, pFloadRec->flrLenKeyAcp_ls00);
    }

    return ErrCode;
}
// ls22_FillPacket()


/*
  -----------------------------------------------------------------------------
  function:     ls22_ReceiveBdinfoPckt
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls22_ReceiveBdinfoPckt(tls00_DBInfo*      pDBInfo,
                       tls00_FloadRec*    pFloadRec,
                       SAPDB_Int4         lRcvPcktNo,
                       tls22_Statistics*  pPacketStatistics,
                       tls22_Statistics*  pSentValues,
                       tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls22_ReceiveBdinfoPckt");

    SAPDB_Int4 rc      = errOK_els00;
    SAPDB_Int4 rcCheck = errOK_els00;

    // Receive an answer packet from kernel
    rc = ls13ReceiveBdinfoPckt(pDBInfo, lRcvPcktNo, &pFloadRec->flrBdInfo_ls00, pszErrText);
    if (errOK_els00 != rc)
    {
        if (rc > 0)
        {
            // Positive return value -> sql error from kernel
            // This can only be one of the following:
            //   300 - integrity violation
            //   320 - view violation
            //   350 - referential integrity violated
            //   360 - foreign key integrity violated

            Tools_DynamicUTF8String _szErrText_UTF8;
            
            // We have to resend the valid records
            rcCheck = ls22_RepairPacket(pDBInfo, pFloadRec, lRcvPcktNo, pPacketStatistics, _szErrText_UTF8);

            // Log sql error to log file
            LoaderMessages::Instance()->ls98PrintMessage(layFastload_els98, errFL_IntegrityViolation_els98,
                                                                             rc, _szErrText_UTF8.StrPtr());

            // Valid return values for rcCheck are:
            //  errOK_els00      -> found valid records on pages that should be send
            //  STOP_NODATA_LS22 -> no valid records found on pages; nothing to send
            // besides those return codes the error counter pFloadRec->flrlRejected_ls00 might turn to 0 thus
            // indicating that the max allowed error no is exceeded; this means to stop processing after resending
            if (errOK_els00 == rcCheck)
            {
                // Resend valid records
                // This has to be done synchronously; so send and receive here
                SAPDB_Int4 rcRedo = ls13SendBdinfoPckt(pDBInfo, lRcvPcktNo, &pFloadRec->flrBdInfo_ls00, pszErrText);
                if (errOK_els00 == rcRedo)
                {
                    rcRedo = ls13ReceiveBdinfoPckt(pDBInfo, lRcvPcktNo, &pFloadRec->flrBdInfo_ls00, pszErrText);
                }

                if (errOK_els00 != rcRedo)
                {
                    rc = rcRedo;
                }
            }   // if (errOK_els00 == rcCheck)
        }   // end if (rc > 0)
        // else: errors in ls13ReceiveBdinfoPckt are reported by the function itself

    }   // end if (errOK_els00 != rc)

    if (rc >= errOK_els00)
    {
        // We have to check the return value of ls22_RepairPacket here too
        // to decide whether to update these statistics or not
        if (errOK_els00 == rcCheck)     // This is actually neccessary only if rc > 0 but we have to check here anyway
        {
            pSentValues->lLastKeyLength_ls22  = pPacketStatistics->lLastKeyLength_ls22;
            SAPDB_memcpy(pSentValues->LastKey_ls22, pPacketStatistics->LastKey_ls22, pPacketStatistics->lLastKeyLength_ls22);

            pSentValues->lPageCount_ls22    += pPacketStatistics->lPageCount_ls22;
            pSentValues->lRecordCount_ls22  += pPacketStatistics->lRecordCount_ls22;
        }

        if (rc > 0)
        {
            if (pFloadRec->flrLRejected_ls00 <= 0)
            {
                // report this error now as we exceed the maximum number of admissible errors
                LoaderMessages::Instance()->ls98MsgToString(layFastload_els98, pszErrText, errFL_IntegrityViolation_els98, rc, "");
                rc = errFL_IntegrityViolation_els98;
            }
            else
            {
                rc = errOK_els00;
            }
        }
    }
    return rc;
}
// ls22_ReceiveBdinfoPckt()


/*
  -----------------------------------------------------------------------------
  function:     ls22_RepairPacket
  -----------------------------------------------------------------------------
*/
static int
ls22_RepairPacket(tls00_DBInfo*             pDBInfo,
                  tls00_FloadRec*           pFloadRec,
                  tsp00_Int4                lPcktNo,
                  tls22_Statistics*         pPacketStatistics,
                  Tools_DynamicUTF8String  &szErrText)
{
    ROUTINE_DBG_MEO00 ("ls22_RepairPacket");

    tsp00_Int4 rc = errOK_els00;

    //*
    //*     Constraint violation is indicated by a negative key length of the corresponding record
    //*
    //* This is true for new communication handling only (7.6 and higher, rudimentary in 7.5).
    //* In case the old handling is still in use the kernel cannot return the invalid pages as
    //* the return packet is too small.
    //* So the kernel returns the error code and a part of part kind page. But this part is empty
    //* and thus has a length of 0 Bytes.

    tsp1_packet* _pReceivePacket  = pDBInfo->dbiPktRcvList_ls00[lPcktNo];
    tsp1_packet* _pSendPacket     = pDBInfo->dbiPktSndList_ls00[lPcktNo];
    tsp1_part*   _pSqlPart        = NULL;

    if (true == ls04GetPart(_pReceivePacket, sp1pk_page, _pSqlPart))
    {
        tbd_node*  _pDataPage = NULL;
        char*      _pBuffer = NULL;


        // What the heck is LineNoBeforePageFilling?
        // Assuming the Loader sent 100 records with this packet to the kernel (and the kernel found some
        // among them that do not fulfill some integrity constraints). Assuming furthermore the Loader has
        // already sent 300 valid records before sending this packet.
        // Then we now have a value plSentRecordCount = 400. To define the correct line numbers of the
        // erroneous records we have to 'reset' the line no to the line no of the first record
        // in this packet.
        SAPDB_Int8 LineNoBeforePageFilling = pFloadRec->flrRecordCount_ls00 - pPacketStatistics->lRecordCount_ls22;


        // Get pages from packet
        SAPDB_Int4 _lPartLen = ls04GetPartLength(_pSqlPart);
        if (0 == _lPartLen)
        {
            // In this case we can only adapt the admissible error count and the packet statistics
            // and generate an error message
            char _szLineNo[128];
            _szLineNo[0] = 0;
            sp77sprintf(_szLineNo, 128, "%qd-%qd", LineNoBeforePageFilling+1,
                                                   LineNoBeforePageFilling+pPacketStatistics->lRecordCount_ls22);
            szErrText = "; possible erroneous records in infile: ";
            szErrText += _szLineNo;

            pFloadRec->flrLRejected_ls00         -= STATIC_CAST(SAPDB_Int4, pPacketStatistics->lRecordCount_ls22);
            pPacketStatistics->lPageCount_ls22    = 0;
            pPacketStatistics->lRecordCount_ls22  = 0;

        }
        else
        {
            _pBuffer = new char[_lPartLen];
            ls04BufFromPartEx(_pSqlPart, 0, _lPartLen, _pBuffer, _lPartLen);

            // Count of pages in received answer packet
            SAPDB_Int4 _lPageCount = _lPartLen/PAGE_MXSP00;

            //*
            //* Prepare send packet (may be there are some valid records among the invalid)
            //*
            ls13Prepare_FL_TL_Packet(pDBInfo, lPcktNo, _pSendPacket, _pSqlPart);

            // LineNoBeforePageFilling and pPacketStatistics will be changed on every call of ls22_RepairPage
            SAPDB_Bool rcValid = SAPDB_FALSE;
            szErrText = "; erroneous records in infile:";

            for (SAPDB_Int4 j = 0; ( (j < _lPageCount) && (pFloadRec->flrLRejected_ls00 > 0) ); ++j)
            {
                _pDataPage = REINTERPRET_CAST(tbd_node*, _pBuffer + j*PAGE_MXSP00);

                rcValid = ls22_RepairPage(_pDataPage,
                                        pFloadRec,
                                        pPacketStatistics,
                                        LineNoBeforePageFilling,
                                        szErrText);
                if (SAPDB_TRUE == rcValid)
                {   // Move page into send packet for resending valid records only if ther are valid records
                    ls04BufToPart(_pSqlPart, _pBuffer + j*PAGE_MXSP00, PAGE_MXSP00);
                }
            }   // end for (j = 0; j < _lPageCount; ++j)

            // Clean up error text
            if (false == szErrText.Empty())
            {
                szErrText[szErrText.Size() - 1] = 0;
            }

            // Assuming the smallest possible record the following count of records can be found on a single page:
            // create table <name> (col1 char(2), primary key (col1))
            // space for data on a page (assuming 8 kB page size): 8096 Byte (header = 80 Byte, footer = 16 Byte)
            // record consists of: 8 Byte header
            //                     1 Byte defined byte
            //                     2 Byte data
            //                     1 Byte alignment (records are 2 Byte aligned)
            //                     2 Byte entry in pointer list
            // --> 8096/14 = 578 records

            if (pPacketStatistics->lPageCount_ls22 > 0)
            {
                tsp00_Int4 anyrc = ls04FinishPart(_pSendPacket, _pSqlPart);
            }
            
        }   // end else of if (0 == _lPartLen)

        if (NULL != _pBuffer)
        {
            delete [] _pBuffer;
            _pBuffer = NULL;
        }

        if (0 == pPacketStatistics->lPageCount_ls22)
        {
            rc = STOP_NODATA_LS22;      // There is nothing to send; but it shouldn't be treated as an error yet
        }
    }   // end if (true == ls04GetPart(_pReceivePacket, sp1pk_page, _pSqlPart))

    return rc;
}
// ls22_RepairPacket()


/*
  -----------------------------------------------------------------------------
  function:     ls22_RepairPage
  -----------------------------------------------------------------------------
*/
static SAPDB_Bool
ls22_RepairPage(tbd_node*                 pDataPage,
                tls00_FloadRec*           pFloadRec,
                tls22_Statistics*         pPacketStatistics,
                tsp00_Int8&               lLineNoBeforeFilling,
                Tools_DynamicUTF8String&  szErrText)
{
    ROUTINE_DBG_MEO00 ("ls22_RepairPage");

    tsp00_SwapKind_Enum SSwap   = pFloadRec->flrTableInfo_ls00.tdKernelSwap_ls00;
    tsp00_SwapKind_Enum DSwap   = pFloadRec->flrSwapKind_ls00;

#if COMPILEMODE_MSP00 >= SLOW_MSP00                 // Only for tracing
    SAPDB_Int2 _lTxtPos = 0;
    SAPDB_Int2 _lRecLen = 0;
    char       _szTrcText[4096];
    _szTrcText[0] = 0;
    _lTxtPos = sp77sprintf(_szTrcText, 1024, "KeyLen|PtrListEntry|RecordLen"NEW_LINE_LS00"");
#endif

    SAPDB_Int4         i                  = 0;

    // Record count on single page starting repair
    SAPDB_Int4         _lRecordCount      = ls07Swap4BInt(pDataPage->nd_record_cnt(), SSwap, DSwap);
    
    tbd_pointer_list&  _PtrList             = pDataPage->nd_pointer_list();
    char*              _pPageBody           = pDataPage->nd_body().asCharp();

    const SAPDB_Int4   _lMax                    = MAX_POINTERINDEX_BD00 - 1;        // maximum pointer index (1024) as c-index
    SAPDB_Int4         _lValidRecordsOnPage     = 0;
    SAPDB_Int2         _lKeyLen                 = 0;        // Length of key of a single record on a page
    SAPDB_Int4         _lPtrListEntry           = 0;
    SAPDB_Int4         _lPtrListEntry_New       = 0;
    SAPDB_Int4         _lPtrListEntry_Retain    = 0;

    SAPDB_Int4         _lInvalidRecords         = 0;        // Count of records marked as invalid -> negative key lengths
    SAPDB_Int4         _lValidRecords           = 0;        // Count of valid records             -> positive key lengths
    SAPDB_Int4         _lPosOf1stValidRecord    = 0;
    SAPDB_Int4         _lBottom                 = 0;        // Last free valid position before copying

    SAPDB_Bool         _bValidCopied            = SAPDB_TRUE;

    // Error reporting
    tsp00_Int8         _row = 0;
    char               _szLineNo[32];
    _szLineNo[0] = 0;

    //*
    //*     Swap all pointer list entries for more comfortable processing
    //*     and invert the list (actually the list must be read from the
    //*     back on the page)
    //*
    SAPDB_Int4* _plMyPtrList = new SAPDB_Int4[_lRecordCount];
    for (SAPDB_Int4 m = 0; m < _lRecordCount; ++m)
    {
        if (SSwap != DSwap)
        {
            _plMyPtrList[m] = ls07Swap4BInt(_PtrList[_lMax - m], SSwap, DSwap);
        }
        else
        {
            _plMyPtrList[m] = _PtrList[_lMax - m];
        }
    }

    //*
    //*     Iterate through all records on page to find and remove invalid records
    //*
    for (i = 0; ( (i < _lRecordCount) && (pFloadRec->flrLRejected_ls00 > 0) ); ++i)
    {
        // Get pointer to single record on page (this is simply an offset)
        _lPtrListEntry = _plMyPtrList[i];

        // Get key length of single record
        SAPDB_memcpy(&_lKeyLen, &_pPageBody[_lPtrListEntry - BODY_BEG_BD00] + KEYLEN_OFFSET_LS23, sizeof(tsp00_Int2));
        ls07Swap2BCharInPlace(REINTERPRET_CAST(char*, &_lKeyLen), SSwap, DSwap);

        if (_lKeyLen > 0)
        {   // Valid record (in a number of other valid records preceding/following) found

#if COMPILEMODE_MSP00 >= SLOW_MSP00
            // Trace key length, list pointer entry, and record length of valid records before repairing
            SAPDB_memcpy(&_lRecLen, &_pPageBody[_lPtrListEntry - BODY_BEG_BD00], sizeof(tsp00_Int2));
            ls07Swap2BCharInPlace(REINTERPRET_CAST(char*, &_lRecLen), SSwap, DSwap);
            _lTxtPos += sp77sprintf(_szTrcText + _lTxtPos, 4096 - _lTxtPos, "%d|%d|%d"NEW_LINE_LS00"",
                                                                            _lKeyLen, _lPtrListEntry, _lRecLen);
#endif      
            
            // Count of all valid records on a single page (needed for resending the page to the kernel)
            ++_lValidRecordsOnPage;
            
            // Count successively valid records for later copying them to the last free valid position
            // on page (thus removing invalid records)
            ++_lValidRecords;

            if (_lInvalidRecords > 0)       
            {   // Found valid record is the first in a number of valid records --> reset number of invalid records
                _lInvalidRecords = 0;
                _bValidCopied = SAPDB_FALSE;
            }
        }
        else                                
        {   // Invalid record (in a number of other invalid records preceding/following) found
            // _lKeyLen < 0; 0 is an impossible key length because a record has either an internal primkey
            // or a user defined one

            _row = lLineNoBeforeFilling + i + 1;             // Log erroneous records number
            sp77sprintf(_szLineNo, 32, " %qd,", _row);
            szErrText += _szLineNo;

            if (_lValidRecords > 0)
            {   // Found invalid record is the first in a number of invalid records 
                //  --> copy valid records preceding this invalid one to the last free valid position;
                //      but we need to copy only if this is not the first invalid record on this page at all
                //
                if (_lValidRecords < i)
                {
                    _bValidCopied = SAPDB_TRUE;

                    _lBottom               = _plMyPtrList[_lValidRecordsOnPage - _lValidRecords];
                    _lPosOf1stValidRecord  = _plMyPtrList[i - _lValidRecords];

                    // _lPtrListEntry is the position of the first invalid record in a number of invalid records
                    //  and thus defines the distance to copy
                    SAPDB_memcpy(&_pPageBody[_lBottom - BODY_BEG_BD00],
                                 &_pPageBody[_lPosOf1stValidRecord - BODY_BEG_BD00],
                                 _lPtrListEntry - _lPosOf1stValidRecord);

                    // Adapt pointer list values for copied valid records
                    _lPtrListEntry_Retain = _plMyPtrList[_lValidRecordsOnPage - _lValidRecords];
                    for (int k = (_lValidRecords - 1); k >= 0; --k)
                    {
                        _lPtrListEntry_New = _lPtrListEntry_Retain + _plMyPtrList[i - k] - _lPosOf1stValidRecord;

                        // At this point we MUST use the real pointer list (reading from back)
                        _PtrList[_lMax - (_lValidRecordsOnPage - k)] = ls07Swap4BInt(_lPtrListEntry_New, DSwap, SSwap);
                    }
                }   // end if (_lValidRecords < i)
                _lValidRecords = 0;
            }   // end if (_lValidRecords > 0)

            ++_lInvalidRecords;
            --pFloadRec->flrLRejected_ls00;     // Adapt max allowed error 
        
        }   // end else of if (_lKeyLen > 0)
    }   // end for (i = 0; i < _lRecordCount; ++i)


    // If there are invalid records on page we have to adapt the statistics and some page values
    if (_lRecordCount != _lValidRecordsOnPage)
    {
        if (0 == _lValidRecordsOnPage)
        {
            // Adapt packet statistics
            --pPacketStatistics->lPageCount_ls22;
            TRACE_PRNF_MLS99(("ls22_RepairPage", "No valid records on page"));
        }
        else //if (_lValidRecordsOnPage > 0)
        {
            //*
            //*     Copy rest if needed
            //*
            if ( (SAPDB_FALSE == _bValidCopied) && (_lValidRecords > 0) )
            {   // Found invalid record is the first in a number of invalid records 
                //  --> copy valid records preceding this invalid one to the last free valid position;
                //      but we need to copy only if this is not the first invalid record on this page at all
                //

                // pos to copy from is last free pos on page and this is hold in nd_bottom()
                _lPtrListEntry = ls07Swap4BInt(pDataPage->nd_bottom(), SSwap, DSwap);

                _lBottom                 = _plMyPtrList[_lValidRecordsOnPage - _lValidRecords];
                _lPosOf1stValidRecord    = _plMyPtrList[_lRecordCount - _lValidRecords];

                // _lPtrListEntry is the position of the first invalid record in a number of invalid records
                //  and thus defines the distance to copy
                SAPDB_memcpy(&_pPageBody[_lBottom - BODY_BEG_BD00],
                             &_pPageBody[_lPosOf1stValidRecord - BODY_BEG_BD00],
                             _lPtrListEntry - _lPosOf1stValidRecord);

                // Adapt pointer list values for copied valid records
                _lPtrListEntry_Retain = _plMyPtrList[_lValidRecordsOnPage - _lValidRecords];
                for (int k = (_lValidRecords - 1); k > 0; --k)
                {
                    _lPtrListEntry_New = _lPtrListEntry_Retain + _plMyPtrList[i - k] - _lPosOf1stValidRecord;

                    // Here we need to use the 'right' pointer list
                    _PtrList[_lMax - (_lValidRecordsOnPage - k)] = ls07Swap4BInt(_lPtrListEntry_New, DSwap, SSwap);
                }
            }   // end if (_lValidRecords > 0)


            // As we send the page to the kernel again if there are valid records on
            // - adapt record count value
            // - get key of last valid record on page
            // - adapt bottom value

            // We have to use here DSwap as source swap and SSwap as destination swap when writing TO page!

            // record count
            pDataPage->nd_record_cnt() = ls07Swap4BInt(_lValidRecordsOnPage, DSwap, SSwap);

            // Define pointer list entry of last valid record on repaired page
            _lPtrListEntry = ls07Swap4BInt(_PtrList[_lMax - (_lValidRecordsOnPage - 1)], SSwap, DSwap);
            
            // key length and key
            SAPDB_memcpy(&pPacketStatistics->lLastKeyLength_ls22,
                         &_pPageBody[_lPtrListEntry - BODY_BEG_BD00] + KEYLEN_OFFSET_LS23,
                         sizeof(tsp00_Int2));
            ls07Swap2BCharInPlace(REINTERPRET_CAST(char*, &pPacketStatistics->lLastKeyLength_ls22), SSwap, DSwap);

            SAPDB_memcpy(pPacketStatistics->LastKey_ls22,
                         &_pPageBody[_lPtrListEntry - BODY_BEG_BD00] + HEADER_OFFSET_LS23,
                         pPacketStatistics->lLastKeyLength_ls22);

            // Set bottom value
            // The used algorithm assures that there is always one more pointer list entry written than
            // there are records on the page -> this can be used for bottom value
            _lPtrListEntry = ls07Swap4BInt(_PtrList[_lMax - _lValidRecordsOnPage], SSwap, DSwap);
            pDataPage->nd_bottom() = ls07Swap4BInt(_lPtrListEntry, DSwap, SSwap);

            TRACE_PRNF_MLS99(("ls22_RepairPage", "%s", _szTrcText));
        
        }   // end else of if (0 == _lValidRecordsOnPage)

        // Adapt packet statistics
        pPacketStatistics->lRecordCount_ls22 -= (_lRecordCount - _lValidRecordsOnPage);
    }

    lLineNoBeforeFilling += _lRecordCount;      // Adapt line no for NEXT page!!

    if (NULL != _plMyPtrList)
    {
        delete [] _plMyPtrList;
        _plMyPtrList = NULL;
    }

    return (_lValidRecordsOnPage > 0);
}
// ls22_RepairPage()

/*!
  -----------------------------------------------------------------------------
  EndChapter: local functions
  -----------------------------------------------------------------------------
*/
