/*!
  -----------------------------------------------------------------------------
  module: vls27_dataextract.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of functions for dataextract

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/

#include "gls00.h"
#include "hsp77.h"
#include "hls05.h"
#include "hls07.h"
#include "hls15.h"
#include "hls27_DEExec.h"
#include "hls27_dataextract.h"
#include "hls31_catalogextract.h"
#include "hls30.h"
#include "hls98msg.h"
#include "hls99.h"


/*
  -----------------------------------------------------------------------------
  Chapter: Local functions declaration
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------

  function:     ls27_InitDatExtrRec  - local function

  -----------------------------------------------------------------------------

  description:  initialize structure used for execution of table extract


  arguments:    pDBInfo      [IN]   - structure with all information on the running
                                      session and the db connected to.
                pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pDatExtrRec  [IN]   - dataextract record structure that must be initialized.

  returnvalue:  errOK_els00, errNoMoreMemory_els00
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_InitDatExtrRec(tls00_DBInfo             *pDBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_DextractRec        *&pDatExtrRec,
                    tsp00_Addr               ErrText );

/*
  -----------------------------------------------------------------------------
  function: ls27_FreeDatExtrRec
  -----------------------------------------------------------------------------

  description:  destroys the tableextract structure built in ls27_InitDatExtrRec


  arguments:    pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pDatExtrRec  [IN]   - dataextract record structure that must be destroyed.

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls27_FreeDatExtrRec(tls00_TransformationCmd  *pTransformationCmd,
                    tls00_DextractRec        *pDatExtrRec);


/*
  -----------------------------------------------------------------------------
  function:     ls27_InitStreams  - local function
  -----------------------------------------------------------------------------

  description:


  arguments:    pDBInfo      [IN]   - structure with all information on the running
                                      session and the db connected to.
                pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pDatExtrRec  [IN]   - dataextract record structure that must be initialized.

  returnvalue:

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_InitStreams( tls00_DBInfo             *pDBInfo,
                  tls00_TransformationCmd  *pTransformationCmd,
                  tls00_DextractRec        *pDatExtrRec,
                  tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls27_CloseStreams       - local function

  -----------------------------------------------------------------------------

  description:


  arguments:    pDBInfo      [IN]   - structure with all information on the running
                                      session and the db connected to.
                pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pDatExtrRec  [IN]   - dataextract record structure that must be initialized.

  returnvalue:

  -----------------------------------------------------------------------------
*/

static void
ls27_CloseStreams(tls00_TransformationCmd  *pTransformationCmd,
                  tls00_DextractRec        *pDatExtrRec,
                  tsp00_Addr               ErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls27_FreeStreams       - local function

  -----------------------------------------------------------------------------

  description:


  arguments:    pDBInfo      [IN]   - structure with all information on the running
                                      session and the db connected to.
                pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pDatExtrRec  [IN]   - dataextract record structure that must be initialized.

  returnvalue:

  -----------------------------------------------------------------------------
*/

static void
ls27_FreeStreams(tls00_TransformationCmd  *pTransformationCmd,
                 tls00_DextractRec        *pDatExtrRec);

/*
  -----------------------------------------------------------------------------
  EndChapter  : Local functions declaration
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  BeginChapter: Local functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls27_InitDatExtrRec  - local function
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_InitDatExtrRec( tls00_DBInfo             *pDBInfo,
                     tls00_TransformationCmd  *pTransformationCmd,
                     tls00_DextractRec        *&pDatExtrRec,
                     tsp00_Addr               ErrText )
{
    ROUTINE_DBG_MEO00 ("ls27_InitDatExtrRec");

    tsp00_Int4 rc     = errOK_els00;

    pDatExtrRec = new tls00_DextractRec;
    if (NULL == pDatExtrRec )
        rc = errNoMoreMemory_els98;
    else
    {
        memset(pDatExtrRec, 0, sizeof(tls00_DextractRec));

        //*
        //*     Initialize common values
        //*
        pDatExtrRec->derPartSpec_ls00 = pTransformationCmd->trPartSpec_ls00;
        // Loader sessioninfo
        pDatExtrRec->derDefStruct_ls00.pdUserDefault      = pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp();
        pDatExtrRec->derDefStruct_ls00.pdUserGroupDefault = pDBInfo->dbiSession_ls00.siUserGroup_ls00.asCharp();
        memset(pDatExtrRec->derDefStruct_ls00.pdOracleUid_ls00, BLANK_LS00, sizeof(tsp00_Number));
        pDatExtrRec->derSwapKind_ls00                     = pDBInfo->dbiOwnSwap_ls00;
        // rejected = siMaxAdmissibleErrors - derLRejected_ls00
        pDatExtrRec->derLRejected_ls00           = pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors;
   
        pDatExtrRec->derpStartPos_ls00[0]        = 0;

        pDatExtrRec->derMediumSize_ls00          = MAX_MEDIUM_SIZE_IN_BYTES_LS27;    // 1 GB in Byte so far !
        pDatExtrRec->derMediumLongIntBlocks_ls00 = 0;

        //create and initialize mapping file and outfile
        if (pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00)
        {
            pDatExtrRec->derRestore_ls00      = pTransformationCmd->trRestore_ls00;
            pDatExtrRec->lNameCount_ls00      = (pDBInfo->dbiPktSize_ls00 + 4*1024)/(sizeof(tsp00_KnlIdentifierc) * 2);
        }// db|user
        else
        {
            pDatExtrRec->lNameCount_ls00  = 1;

            // table
            pDatExtrRec->derTabEmpty_ls00 = false;
            memset(&pDatExtrRec->derDataSource_ls00, 0, sizeof(tls00_DataSource));

            pDatExtrRec->derTableInfo_ls00.tdKernelSwap_ls00.becomes(sw_dummy);
            pDatExtrRec->derFillValue_ls00           = ls07GetFillByte(dcha);

        }   // end else of if (pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00)

        if (errOK_els00 == rc)
             rc = ls27_InitStreams( pDBInfo, pTransformationCmd, pDatExtrRec, ErrText);
    }
    return rc;
}
// ls27_InitDatExtrRec()


/*
  -----------------------------------------------------------------------------
  function: ls27_FreeDatExtrRec
  -----------------------------------------------------------------------------
*/
static void
ls27_FreeDatExtrRec(tls00_TransformationCmd  *pTransformationCmd,
                    tls00_DextractRec        *pDatExtrRec)
{
    ROUTINE_DBG_MEO00 ("ls27_FreeDatExtrRec");

    if ( NULL != pDatExtrRec )
    {
        char ErrText[256];

        ls27_CloseStreams(pTransformationCmd, pDatExtrRec, ErrText);

        if ( (dtypeTape_ls00 != pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00) &&
             (ptTable_els00  == pDatExtrRec->derPartSpec_ls00.psPartType_ls00) )
        {
            pTransformationCmd->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00 =
                CONST_CAST(SAPDB_Char*, pDatExtrRec->derDataFile_ls00->vfBackupId_ls00.CharPtr());
        }
        ls27_FreeStreams(pTransformationCmd, pDatExtrRec);

        delete pDatExtrRec;
        pDatExtrRec = NULL;
    }   // if ( NULL != pDatExtrRec )
}
// ls27_FreeDatExtrRec()

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  BeginChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
   function:     ExportPackageDataRecord
  -----------------------------------------------------------------------------
*/
int ExportPackageDataRecord(MetaDataDef *pMetaDataDef, tls00_DBInfo *pDBInfo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ExportPackageDataRecord");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance(); 

    tsp00_Longint rc = errOK_els00;

    TransformationDef       *pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();
	RelationalDef           *pRelationalDef     = pMetaDataDef->getRelationalModel();
    //*
    //*  db|user|table records, formatted, compressed
    //*
    if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 && errOK_els00 == rc )
    {
        pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = true;
        //*
        //*  import transformationpackage 
        //*
        rc = pTransformationDef->ImportPackage(pMetaDataDef,ErrText);

        if ( errOK_els00 == rc )
        {
            tls00_DextractRec *pDatExtrRec  = NULL;
            rc = ls27_InitDatExtrRec(pDBInfo, pTransformationCmd, pDatExtrRec, ErrText);
            if ( errOK_els00 == rc )
            {
                //*
                //*  execute transformationpackage to transform data
                //*
                if ( ptTable_els00  <= pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
                    SAPDB_FALSE    == pTransformationCmd->trInternal_ls00 && pTransformationDef->m_nTableCount != 0)
                {
                    pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgExecDataPackage_els98);
                    pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgNumTabToTransform_els98, pTransformationDef->m_nTableCount);
                }
                //*
                //*  update  transformationpackage
                //*
                if( ptAll_els00    == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                    ptUser_els00   == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                    ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
                {
                    rc = ls27ExportPackageTables( pMetaDataDef, pDatExtrRec, ErrText );
                }
                if (ptTable_els00  == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
					ptColumn_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
                {
                    rc = ls27ExportTableRows( pMetaDataDef, pDatExtrRec, ErrText );
                }
            }
            if ( NULL != pDatExtrRec )
            {
                ls27_FreeDatExtrRec(pTransformationCmd, pDatExtrRec);
                pDatExtrRec = NULL;
            }
        }
        pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = false;
		//*
	    //*  execute transformationpackage to transform catalog
	    //*
		if ( pRelationalDef->SplitPackage())
		{
		     StudioOAL_String catalogfilename(pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00);
             catalogfilename += "0001";
             delete [] pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00;
             pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 = new char [catalogfilename.BasisSize()+1];
             sp77sprintf(pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00, PATH_MXSP00, "%s", CONST_CAST(SAPDB_Char*, catalogfilename.CharPtr()) );
			 rc = TransportPackageCatalog( pMetaDataDef, pDBInfo, ErrText);
		}
		//*
        //*  export  transformationpackage
        //*
        if (errOK_els00 == rc && pTransformationDef->m_nTableCount != 0 )
            rc = pTransformationDef->ExportPackage(pMetaDataDef,ErrText);
    }// if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )
    return STATIC_CAST(tsp00_Int4, rc);
}
//  ExportPackageDataRecord()

/*
  -----------------------------------------------------------------------------
  function:     ls27ExportPackageTables
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27ExportPackageTables(MetaDataDef*&       pMetaDataDef,
                        tls00_DextractRec*  pDatExtrRec,
                        tsp00_Addr          ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27ExportPackageTables");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    tsp00_Int4      rc           = errOK_els00;

    TransformationDef*       pTransformationDef  = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd* pTransformationCmd  = pTransformationDef->getTransformationCmd();
    tls00_DBInfo*            pDBInfo             = pMetaDataDef->getConnection();
    Loader_RTStatistics*     pStatObj            = Loader_RTStatistics::Instance();

    // Define count of excluded tables
    pStatObj->SetTablesExcluded(pTransformationDef->m_nTableData);

    StudioOAL_String  select;
    _TCHAR            szselect[2048];
    szselect[0] = 0;

    sp77sprintf(szselect, 2048, pTransformationDef->Statement(sel_Transformation_708).CharPtr(), pTransformationCmd->trPackageid_ls00.StrPtr());
    select.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, szselect));

    if ( !pTransformationDef->executeSQL( select ) )
    {
        StudioOAL_String NativeErrorAsString  = pTransformationDef->getNativeErrorAsString();
        SQLRETURN        ReturnCode = pTransformationDef->getReturnCode();

        pMsgInstance->ls98PrintMessage(layDataExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        rc = ReturnCode;
    }

    StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
    if (errOK_els00 == rc)
    {
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
        currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
        currentSchema = sessionSchema;
    }
    pTransformationDef->m_currentStream = stData_els00;
    /*-------------------------------------------------------------------------------*
     * Read/Write the CATALOG definition of the specified object from/to resources   *
     *-------------------------------------------------------------------------------*/
    while ( pTransformationDef->initTransformationStep() && errOK_els00 == rc )
    {
        rc = pTransformationDef->switchUserSchema( currentUser, currentSchema, false, ErrText);
        
        currentUser   = pTransformationDef->m_owner ;
        currentSchema = pTransformationDef->m_schemaname ;
        
		if (errOK_els00 == rc)
        {
            rc = pTransformationDef->initTransformationCmd(srFctColExtract_els00);
            if (errOK_els00 == rc)
            {
                pDatExtrRec->derTableSwitched_ls00 = true;
                rc = ls27ExportTableRows( pMetaDataDef, pDatExtrRec, ErrText);
                if (errOK_els00 == rc)
                {
                    pStatObj->IncreaseCounter_TablesTranformed();
                    ++pDatExtrRec->derNumTabInFile_ls00;
                }
                else
                {
                    if ( (errInternal_els98 != rc) && (errCancelRequest_els98 != rc) )
                    {
                        pStatObj->IncreaseCounter_TablesFailed();
                        if (pStatObj->GetTablesFailed() <= pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors)
                        {
                            ErrText[0] = 0;
                            rc = errOK_els00;
                        }
                    }
                }
            }//pTransformationDef->initTransformationCmd
        }
    }// while pTransformationDef->initTransformationStep

    if (errOK_els00 == rc)
    {
        pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgNumTabTransformed_els98, pStatObj->GetTablesTranformed());
    }
    //*
    //* switch user back to session user
    //*
    if ( pTransformationCmd->trPartSpec_ls00.psPartType_ls00 >= ptUser_els00 )
        rc = pTransformationDef->switchUser( currentUser, sessionUser, ErrText);

    if ( NULL           != pTransformationDef->getTransformationCmd() &&
         dtypeTape_ls00 == pTransformationDef->getTransformationCmd()->trDataFile_ls00.fsDeviceType_ls00 )
    {
         ls27_CloseStreams(pTransformationDef->getTransformationCmd(), pDatExtrRec, ErrText);
         pTransformationCmd->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00 =
             CONST_CAST(SAPDB_Char*, pDatExtrRec->derDataFile_ls00->vfBackupId_ls00.CharPtr());
    }

    pTransformationDef->freeTransformationCmd (srFctColExtract_els00);

    return rc;
}
// ls27ExportPackageTables()

/*
  -----------------------------------------------------------------------------
  function:     ls27UpdateTransformationPackage
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27UpdateTransformationPackage(MetaDataDef             *&pMetaDataDef,
                                tls00_TransformationCmd  *pTransformationCmd,
                                tls00_DextractRec        *pDatExtrRec,
                                tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27UpdateTransformationPackage");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    tsp00_Int4 rc       = errOK_els00;
    SAPDB_Bool rcBOOL   = SAPDB_FALSE;

	if( pTransformationCmd->trPartSpec_ls00.psPartType_ls00 >= ptTable_els00 /*&&
		pTransformationCmd->trInternal_ls00 == false*/)
    {
        TransformationDef * pTransformationDef = pMetaDataDef->getTransformationModel();
		StudioOAL_String    NativeErrorAsString;
        SQLRETURN           ReturnCode = 0;
        //*
        //* performance
        //*
        extern tls00_Performance performance;
        SAPDB_UInt8  start, finish, duration;
        //*
        //*  storagedevice, servernode, serverdb, username, codetype, codepage, transformation, part, streamformat
        //*
        _TCHAR RECORD_EXTRACTED[256];
        _TCHAR HEADER_BLOCK    [256];
        _TCHAR HEADER_POS      [256];
        _TCHAR DATA_BLOCK      [256];
        _TCHAR DATA_POS        [256];
        _TCHAR TRAILER_BLOCK   [256];
        _TCHAR TRAILER_POS     [256];
        RECORD_EXTRACTED[0] = 0;
        HEADER_BLOCK    [0] = 0;
        HEADER_POS      [0] = 0;
        DATA_BLOCK      [0] = 0;
        DATA_POS        [0] = 0;
        TRAILER_BLOCK   [0] = 0;
        TRAILER_POS     [0] = 0;
        if (true == pDatExtrRec->derTableSwitched_ls00)
        {
             // TIMESTAMP BEGIN
             pDatExtrRec->derTableSwitched_ls00 =  false;
             if ( !pTransformationDef->updateBegin() )
             {
                  ReturnCode          = pTransformationDef->getReturnCode();
                  NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                  pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());

                  return ReturnCode;
             }
             // <DATASTREAMNAME>
             pTransformationDef->m_datastreamname = pDatExtrRec->derDataFile_ls00->vfName_ls00;
             if ( !pTransformationDef->updateDataStream() )
             {
                  ReturnCode          = pTransformationDef->getReturnCode();
                  NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                  pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                  return ReturnCode;
             }
             // <HEADER_BLOCK>:<HEADER_POS>
             _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derDataFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00),HEADER_BLOCK, 10);
             _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derDataFile_ls00->vfFilePos_ls00.lnuRemainder_ls00) ,HEADER_POS, 10);
			 pTransformationDef->m_headerpos_block = HEADER_BLOCK;
			 pTransformationDef->m_headerpos_byte  = HEADER_POS;

             if ( !pTransformationDef->updateHeader() )
             {
                 ReturnCode          = pTransformationDef->getReturnCode();
                 NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                 pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                 return ReturnCode;
             }
             pDatExtrRec->derHeaderFilePos_ls00 = pDatExtrRec->derDataFile_ls00->vfFilePos_ls00;
        }
        // RECORD_EXTRACTED
        pDatExtrRec->derLastCommLine_ls00  = pDatExtrRec->derExtractedRows_ls00;
        _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derLoadedRows_ls00 + pDatExtrRec->derLastCommLine_ls00), RECORD_EXTRACTED, 10);
        // <DATA_BLOCK>:<DATA_POS>
        _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derDataFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00), DATA_BLOCK, 10);
        _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derDataFile_ls00->vfFilePos_ls00.lnuRemainder_ls00) , DATA_POS, 10);
		pTransformationDef->m_rows_exported = RECORD_EXTRACTED;
		pTransformationDef->m_datapos_block = DATA_BLOCK;
		pTransformationDef->m_datapos_byte  = DATA_POS;
		// Export
		start =  RTESys_MicroSecTimer();
        if ( !pTransformationDef->updateExportData() )
        {
            ReturnCode          = pTransformationDef->getReturnCode();
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            return ReturnCode;
        }
        finish = RTESys_MicroSecTimer();
		duration = finish - start;
        performance.perLog_ls00 += duration;
        
        if ( true == pDatExtrRec->derTabEmpty_ls00     ||
             true == pDatExtrRec->derTabExtracted_ls00 ||
             true == pTransformationCmd->trExclude_ls00 )
        {
            // RECORD_EXTRACTED
            pDatExtrRec->derLastCommLine_ls00  = pDatExtrRec->derExtractedRows_ls00;
            _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derLoadedRows_ls00 + pDatExtrRec->derLastCommLine_ls00), RECORD_EXTRACTED, 10);
            // <DATA_BLOCK>:<DATA_POS>
            _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derDataFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00), DATA_BLOCK, 10);
            _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derDataFile_ls00->vfFilePos_ls00.lnuRemainder_ls00) , DATA_POS, 10);
            pTransformationDef->m_rows_exported = RECORD_EXTRACTED;
            pTransformationDef->m_datapos_block = DATA_BLOCK;
            pTransformationDef->m_datapos_byte  = DATA_POS;
            if ( !pTransformationDef->updateExportData() )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
            // <TRAILER_BLOCK>:<TRAILER_POS>
            _itot(STATIC_CAST(tsp00_Int4, pTransformationDef->m_trailerpos.lnu2GB_Blocks_ls00),TRAILER_BLOCK,10);
            _itot(STATIC_CAST(tsp00_Int4, pTransformationDef->m_trailerpos.lnuRemainder_ls00) ,TRAILER_POS,10);
            pTransformationDef->m_trailerpos_block = TRAILER_BLOCK;
            pTransformationDef->m_trailerpos_byte  = TRAILER_POS;

            rcBOOL = pTransformationDef->updateTrailer();
            if (SAPDB_TRUE == rcBOOL)
            {   // RESTART
                rcBOOL = pTransformationDef->updateRestart();
            }
            if (SAPDB_TRUE == rcBOOL)
            {   // TIMESTAMP END
                rcBOOL = pTransformationDef->updateEnd();
            }
            if (SAPDB_TRUE == rcBOOL)
            {   // SYSTEM TIME
                rcBOOL = pTransformationDef->updatePerformance();
            }
            if (SAPDB_FALSE == rcBOOL)
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
        }//if ( pDatExtrRec->derTabEmpty_ls00 == true )

        if ( !pTransformationDef->update("COMMIT WORK") )
        {
            ReturnCode          = pTransformationDef->getReturnCode();
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            return ReturnCode;
        }
    }
    return rc;
}//ls27UpdateTransformationPackage

/*
  -----------------------------------------------------------------------------
  function:     ls27_InitStreams
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_InitStreams( tls00_DBInfo             *pDBInfo,
                  tls00_TransformationCmd  *pTransformationCmd,
                  tls00_DextractRec        *pDatExtrRec,
                  tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_InitStreams");

    tsp00_Int4 rc = errOK_els00;

    // mapping file, datafile, commandfile
    tls00_VFile *pDataFile = NULL;

    // switch tableempty
    if (false == pDatExtrRec->derTabEmpty_ls00  )
    {
        //*
        //*  Record
        //*
        if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )
        {
            pDatExtrRec->derDataFile_ls00 = new tls00_VFile;
            memset(pDatExtrRec->derDataFile_ls00, 0, sizeof(tls00_VFile));
            pDataFile = pDatExtrRec->derDataFile_ls00;
            //*
            //*     initialize DATA streams depending on stream type - FILE, PIPE, TAPE
            //*
            if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
            {
                tsp05_RteFileMode tmpFileMode;
                tmpFileMode.becomes(sp5vf_append);
                // switch part
                if ( (pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00)     &&
                    (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) )
                {
                    // switch restart
                    if (SAPDB_FALSE == pTransformationCmd->trRestart_ls00.rstRestart_ls00)
                        tmpFileMode.becomes(sp5vf_write);
                    //*
                    //* DATA file
                    //*
                    pDataFile->vfFileName_ls00 = new _TCHAR[PATH_MXSP00];
                    pDataFile->vfFileName_ls00[0] = 0;
                    sp77sprintf(pDataFile->vfFileName_ls00, PATH_MXSP00, "%s%0*d", pTransformationCmd->trDataFile_ls00.fsFileName_ls00,FILE_EXTENSION_SIZE_LS27,1);
                    ls30VFInitialize(*pDataFile, pDataFile->vfFileName_ls00, sp5vf_binary, tmpFileMode);
                }
                else
                {
                    // switch streamformat
                    if ((  true  == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00)  ||
                        ( (true  == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00) &&
                        (  false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00)    )  )
                    {
                        if (true == pTransformationCmd->trDataFile_ls00.fsAppend_ls00) {
                            ls30VFInitialize(*pDataFile,pTransformationCmd->trDataFile_ls00.fsFileName_ls00,
                                              sp5vf_text,sp5vf_append, pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00);
                        }
                        else {
                            ls30VFInitialize(*pDataFile,pTransformationCmd->trDataFile_ls00.fsFileName_ls00,
                                              sp5vf_text,sp5vf_write, pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00);
                        }
                        
                        
                    }
                    else
                    {
                        if ( (false       == pTransformationCmd->trCmdFile_ls00.fsAppend_ls00) &&
                            (SAPDB_FALSE == pTransformationCmd->trRestart_ls00.rstRestart_ls00) )
                        {
                            tmpFileMode.becomes(sp5vf_write);
                        }
                        ls30VFInitialize(*pDataFile, pTransformationCmd->trDataFile_ls00.fsFileName_ls00, sp5vf_binary, tmpFileMode);
                    }
                }// if ( (pTransformationCmd->trPartSpec_ls00.psPartType_ls00 <= ptTable_els00) ...

                //*
                //*   streamname
                //*
                if ( NULL != pDataFile->vfFileName_ls00 )
                {
                    _TCHAR* pszDest = strrchr(pDataFile->vfFileName_ls00, SLASH_LS00);
                    if (NULL == pszDest)
                    {
                        pszDest = strrchr(pDataFile->vfFileName_ls00, BACKSLASH_LS00);
                        if (NULL == pszDest)
                            pszDest = pDataFile->vfFileName_ls00;
                    }
                    pszDest = (pszDest == pDataFile->vfFileName_ls00) ? pszDest : pszDest + 1;
                    pDatExtrRec->derDataFile_ls00->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                                       pszDest));
                }

            }// end if  FILE
            else
            {// stream types PIPE and TAPE

                if (dtypePipe_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
                {
                    pDataFile->vfFileType_ls00 = dtypePipe_ls00;
                }
                if (dtypeTape_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
                {
                    pDataFile->vfFileType_ls00   = dtypeTape_ls00;
                    pDataFile->vfBackupTool_ls00 = pDBInfo->dbiSession_ls00.siBackupTool_ls00;
                    pDataFile->vfNodeName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                       pDBInfo->dbiSession_ls00.si_pDBNode_ls00));
                    pDataFile->vfDBName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                     pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp()));
                }
                // switch streamformat
                if ( (  true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00)  ||
                      ( (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00) &&
                      (false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00)  )  )
                {
                    ls30VFInitialize(*pDataFile,
                                     pTransformationCmd->trDataFile_ls00.fsFileName_ls00,
                                     sp5vf_text,
                                     sp5vf_write,
                                     pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00);
                }
                else
                {
                    ls30VFInitialize(*pDataFile,
                                     pTransformationCmd->trDataFile_ls00.fsFileName_ls00, sp5vf_binary, sp5vf_write);
                }
            }
        } // RECORD

        //*
        //*  Transformation
        //*
        if ( ( (resUnknown_els00 != pTransformationCmd->trRestore_ls00) || 
               (pTransformationCmd->trPackageFile_ls00.fsFileName_ls00  != NULL  && 
                pTransformationCmd->trPartSpec_ls00.psPartType_ls00     == ptTable_els00) ) &&
             ( (false == pDatExtrRec->derTabEmpty_ls00          ) || 
               (pTransformationCmd->trRestore_ls00 < resDataUpdt_els00) ) )
        {
            if ( NULL == pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 )
            {
                pDatExtrRec->derCmdFile_ls00 = new tls00_VFile;
                memset(pDatExtrRec->derCmdFile_ls00, 0, sizeof(tls00_VFile));

                if (true == pTransformationCmd->trCmdFile_ls00.fsAppend_ls00)
                {
                    ls30VFInitialize(*pDatExtrRec->derCmdFile_ls00, pTransformationCmd->trCmdFile_ls00.fsFileName_ls00,
                                    sp5vf_text, sp5vf_append);
                }
                else
                {
                    ls30VFInitialize(*pDatExtrRec->derCmdFile_ls00, pTransformationCmd->trCmdFile_ls00.fsFileName_ls00,
                                    sp5vf_text, sp5vf_write);
                }

                // Set right stream type
                pDatExtrRec->derCmdFile_ls00->vfFileType_ls00 = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;

                rc = ls30VFOpen(*pDatExtrRec->derCmdFile_ls00, ErrText);
            }
            if ( NULL != pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 &&
                 true == pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoDtl_ls00 )
            {
                pDatExtrRec->derCmdFile_ls00 = new tls00_VFile;
                memset(pDatExtrRec->derCmdFile_ls00, 0, sizeof(tls00_VFile));

                if (NULL != getenv ("maxdbloaderpackagename"))
                {
                    StudioOAL_String packagefilename(getenv ("maxdbloaderpackagename"));
                    packagefilename += ".IMPORT";
                    pDatExtrRec->derCmdFile_ls00->vfFileName_ls00 = new char [packagefilename.BasisSize()+1];
                    sp77sprintf(pDatExtrRec->derCmdFile_ls00->vfFileName_ls00, PATH_MXSP00, "%s", CONST_CAST(SAPDB_Char*, packagefilename.CharPtr()) );

                    if (true == pTransformationCmd->trPackageFile_ls00.fsAppend_ls00) {
                        ls30VFInitialize(*pDatExtrRec->derCmdFile_ls00, pDatExtrRec->derCmdFile_ls00->vfFileName_ls00, sp5vf_text, sp5vf_append);
                    }
                    else {
                        ls30VFInitialize(*pDatExtrRec->derCmdFile_ls00, pDatExtrRec->derCmdFile_ls00->vfFileName_ls00, sp5vf_text, sp5vf_write);
                    }
                }
                if (NULL == getenv ("maxdbloaderpackagename"))
                {
                    if (true == pTransformationCmd->trPackageFile_ls00.fsAppend_ls00) {
                        ls30VFInitialize(*pDatExtrRec->derCmdFile_ls00, pTransformationCmd->trPackageFile_ls00.fsFileName_ls00, sp5vf_text, sp5vf_append);
                    }
                    else {
                        ls30VFInitialize(*pDatExtrRec->derCmdFile_ls00, pTransformationCmd->trPackageFile_ls00.fsFileName_ls00, sp5vf_text, sp5vf_write);
                    }
                }

                // Set right stream type
                pDatExtrRec->derCmdFile_ls00->vfFileType_ls00 = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;

                rc = ls30VFOpen(*pDatExtrRec->derCmdFile_ls00, ErrText);
            }
        }
    }   // end if (false == pDatExtrRec->derTabEmpty_ls00)
    else
    {
        if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 &&
             true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 )
        {
            // Allocate file structure (datafile for MetaDataHeader)
            pDatExtrRec->derDataFile_ls00 = new tls00_VFile;
            memset(pDatExtrRec->derDataFile_ls00, 0, sizeof(tls00_VFile));

            pDataFile = pDatExtrRec->derDataFile_ls00;

            //*
            //*     Initialize DATA file
            //*
            if (true == pTransformationCmd->trCmdFile_ls00.fsAppend_ls00)
            {
                ls30VFInitialize(*pDataFile, pTransformationCmd->trDataFile_ls00.fsFileName_ls00,
                                 sp5vf_binary, sp5vf_append);
            }
            else
            {
                if ( SAPDB_FALSE == pTransformationCmd->trRestart_ls00.rstRestart_ls00 )
                {
                    ls30VFInitialize(*pDataFile, pTransformationCmd->trDataFile_ls00.fsFileName_ls00,
                                     sp5vf_binary, sp5vf_write);
                }
                else
                {
                    ls30VFInitialize(*pDataFile, pTransformationCmd->trDataFile_ls00.fsFileName_ls00,
                                     sp5vf_binary, sp5vf_append);
                }
                //*
                //*     open DATA file if any
                //*
                if (errOK_els00 == rc && NULL != pDataFile )
                {
                    rc = ls30VFOpen(*pDataFile, ErrText);
                    pDataFile->vfFileType_ls00 = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;
                }
            }
        }   // end if ( true == pTransformationCmd->trDataFile_ls00->fsFormat_ls00->ffoCompact_ls00 )

    }   // end else of if (false == pDatExtrRec->derTabEmpty_ls00)

    if ( NULL           != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 &&
         dtypeFile_ls00 != pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
    {
        pDatExtrRec->derDataFile_ls00->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                           pDatExtrRec->derDataFile_ls00->vfFileName_ls00));
    }

    return rc;
}
// ls27_InitStreams()


/*
  -----------------------------------------------------------------------------
  function:     ls27_CloseStreams
  -----------------------------------------------------------------------------
*/
static void
ls27_CloseStreams(tls00_TransformationCmd  *pTransformationCmd,
                  tls00_DextractRec        *pDatExtrRec,
                  tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_CloseStreams");

    tsp00_Int4 rc = errOK_els00;

    if ( (true == pDatExtrRec->derTabEmpty_ls00) &&
         (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) )
    {
        //*
        //* DATA file
        //*
        if (NULL != pDatExtrRec->derDataFile_ls00)
            ls30VFClose(*pDatExtrRec->derDataFile_ls00, ErrText);
    }

    if (false == pDatExtrRec->derTabEmpty_ls00)
    {
        //*
        //* DATA file
        //*
        if (NULL != pDatExtrRec->derDataFile_ls00)
            ls30VFClose(*pDatExtrRec->derDataFile_ls00, ErrText);
        //*
        //* long file
        //*
        if ( ( 0     != pDatExtrRec->derLongColCnt_ls00) &&
             ( false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) )
        {
            tls00_Column *pCol = NULL;

            for (int i = 0; i < pTransformationCmd->trColSpec_ls00.mcsCount_ls00; ++i)
            {
                pCol = pTransformationCmd->trColSpec_ls00.mcsColumn_ls00[i];

                // Literals may not be of type LONG
                if (NULL != pCol->colLongInfo.loiFileStruct_ls00)
                    if (false == pCol->colIsLit_ls00)
                        ls30VFClose(*pCol->colLongInfo.loiFileStruct_ls00, ErrText);
            }   // end for (i; i < pTransformationCmd->trColSpec_ls00.mcsCount_ls00; ++i)
        }
    }

    // command file
    if (NULL != pDatExtrRec->derCmdFile_ls00)
        ls30VFClose(*pDatExtrRec->derCmdFile_ls00, ErrText);

}

/*
  -----------------------------------------------------------------------------
  function:     ls27_FreeStreams
  -----------------------------------------------------------------------------
*/
static void
ls27_FreeStreams(tls00_TransformationCmd  *pTransformationCmd,
                 tls00_DextractRec        *pDatExtrRec)
{
    ROUTINE_DBG_MEO00 ("ls27_FreeStreams");

    if ( (true == pDatExtrRec->derTabEmpty_ls00) &&
         (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) )
    {
        //*
        //* DATA file
        //*
        if (NULL != pDatExtrRec->derDataFile_ls00)
        {
            if ( NULL != pDatExtrRec->derDataFile_ls00->vfFileName_ls00 )
            {
                 delete [] pDatExtrRec->derDataFile_ls00->vfFileName_ls00;
                 pDatExtrRec->derDataFile_ls00->vfFileName_ls00 = NULL;
            }
            delete pDatExtrRec->derDataFile_ls00;
            pDatExtrRec->derDataFile_ls00 = NULL;
        }
    }

    if (false == pDatExtrRec->derTabEmpty_ls00)
    {
        //*
        //* DATA file
        //*
        if (NULL != pDatExtrRec->derDataFile_ls00)
        {
            if ( NULL != pDatExtrRec->derDataFile_ls00->vfFileName_ls00 )
            {
                 // delete [] pDatExtrRec->derDataFile_ls00->vfFileName_ls00;
                 pDatExtrRec->derDataFile_ls00->vfFileName_ls00 = NULL;
            }
            delete pDatExtrRec->derDataFile_ls00;
            pDatExtrRec->derDataFile_ls00 = NULL;
        }
        //*
        //* long file
        //*
        if ( ( 0     != pDatExtrRec->derLongColCnt_ls00) &&
             ( false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) )
        {
            tls00_Column *pCol = NULL;

            for (int i = 0; i < pTransformationCmd->trColSpec_ls00.mcsCount_ls00; ++i)
            {
                pCol = pTransformationCmd->trColSpec_ls00.mcsColumn_ls00[i];

                // Literals may not be of type LONG
                if (NULL != pCol->colLongInfo.loiFileStruct_ls00)
                {
                    if (false == pCol->colIsLit_ls00)
                    {
                        delete pCol->colLongInfo.loiFileStruct_ls00;
                        pCol->colLongInfo.loiFileStruct_ls00 = NULL;
                    }
                }
            }   // end for (i; i < pTransformationCmd->trColSpec_ls00.mcsCount_ls00; ++i)
        }
    }
    // command file
    if (NULL != pDatExtrRec->derCmdFile_ls00)
    {
        if ( NULL != pDatExtrRec->derCmdFile_ls00->vfFileName_ls00 && NULL != getenv("maxdbloadersourcepackagename") )
        {
            delete []  pDatExtrRec->derCmdFile_ls00->vfFileName_ls00;
            pDatExtrRec->derCmdFile_ls00->vfFileName_ls00 = NULL;
             
        }
        delete pDatExtrRec->derCmdFile_ls00;
        pDatExtrRec->derCmdFile_ls00 = NULL;
    }

    return;
}
// ls27_FreeStreams()

/*===========================================================================*
 *     END
 *===========================================================================*/


