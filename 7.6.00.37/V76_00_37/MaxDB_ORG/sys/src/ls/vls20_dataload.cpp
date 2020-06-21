/*!
  -----------------------------------------------------------------------------
  module: vls20_dataload.cpp yours
  -----------------------------------------------------------------------------

  responsible:  d033893 and SteffenS

  special area: SAP DB LOADER

  description:  Implementation of functions for dataload

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
#include "hls04.h"
#include "hls041.h"
#include "hls05.h"
#include "hls11.h"
#include "hls15.h"
#include "hls20_dataload.h"
#include "hls20.h"
#include "hls21.h"
#include "hls30.h"
#include "hls31_catalogextract.h"
#include "hls99.h"
#include "hls98msg.h"

#include "hsp77.h"


/*
  -----------------------------------------------------------------------------
  BeginChapter: Local functions declaration
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------

  function:     ls20_InitDatLoadRec

  -----------------------------------------------------------------------------

  description:  Creates and initializes DATA load record structure

  arguments:    pTransformationRTE [IN] - structure with information on processing the
                                   dataload command
                DBInfo      [IN] - structure with information on the running
                                   session and the database connected to.
  returnvalue:  No

  -----------------------------------------------------------------------------
*/

static tsp00_Int4
ls20_InitDatLoadRec( tls00_DBInfo            *pDBInfo,
                     tls00_TransformationCmd *pTransformationCmd,
                     tls00_TransformationRTE *&pTransformationRTE,
                     tsp00_Addr              ErrText  );

/*
  -----------------------------------------------------------------------------
  function:     ls20_FreeDatLoadRec
  -----------------------------------------------------------------------------

  description:

  arguments:    pTransformationCmd [IN] -  structure with user information for
                                    processing dataload command.
                pTransformationRTE  [IN] - structure with information on processing the
                                    dataload command
  returnvalue:  No

  -----------------------------------------------------------------------------
*/
static void
ls20_FreeDatLoadRec(tls00_TransformationCmd*   pTransformationCmd,
                    tls00_TransformationRTE*&  pTransformationRTE);

/*
  -----------------------------------------------------------------------------

  function: ls20_InitStreams

  -----------------------------------------------------------------------------

  description:

  arguments:

  returnvalue:

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_InitStreams ( tls00_DBInfo            *pDBInfo,
                   tls00_TransformationCmd *pTransformationCmd,
                   tls00_TransformationRTE          *&pTransformationRTE,
                   tsp00_Addr              ErrText      );

/*
  -----------------------------------------------------------------------------

  function:     ls20_SwitchStreams - local function

  -----------------------------------------------------------------------------

  description:


  arguments:    pDBInfo      [IN]   - structure with all information on the running
                                      session and the db connected to.
                pTransformationCmd  [IN]   - structure with information for dataload supplied by the user
                pTransformationRTE  [IN]   - dataload record structure that must be initialized.

  returnvalue:

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_SwitchStreams( TransformationDef  *&pTransformationDef,
                    tls00_TransformationRTE     *pTransformationRTE,
                    tsp00_Addr         ErrText);
/*
  -----------------------------------------------------------------------------
  function: ls20_FreeStreams
  -----------------------------------------------------------------------------

  description:

  arguments:

  returnvalue:
  -----------------------------------------------------------------------------
*/
static void
ls20_FreeStreams(tls00_TransformationCmd*  pTransformationCmd,
                 tls00_TransformationRTE*  pTransformationRTE);

/*
  -----------------------------------------------------------------------------
  Local functions: implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls20_InitDatLoadRec
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_InitDatLoadRec(tls00_DBInfo             *pDBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_TransformationRTE           *&pTransformationRTE,
                    tsp00_Addr               ErrText )
{
    ROUTINE_DBG_MEO00 ("ls20_InitDatLoadRec");

    tsp00_Int4 rc = errOK_els00;

    pTransformationRTE = new tls00_TransformationRTE;
    if (NULL == pTransformationRTE)
    {
        rc = errNoMoreMemory_els98;
    }
    else
        memset(pTransformationRTE, 0, sizeof(tls00_TransformationRTE));

    pTransformationRTE->rte_Cnt_RejectedRecords_ls00 = 0;
    pTransformationRTE->rte_SwapKind_ls00  = pDBInfo->dbiOwnSwap_ls00;
    pTransformationRTE->rte_PartSpec_ls00  = pTransformationCmd->trPartSpec_ls00;

    if ( true == pTransformationCmd->trRestart_ls00.rstRestart_ls00 )
         pTransformationRTE->rte_bRestart_ls00 = true;

    if ( rc == errOK_els00 )
         rc = ls20_InitStreams(pDBInfo, pTransformationCmd, pTransformationRTE, ErrText);

    return rc;
}// ls20_InitDatLoadRec


/*
  -----------------------------------------------------------------------------
  function:     ls20_FreeDatLoadRec
  -----------------------------------------------------------------------------
*/
static void
ls20_FreeDatLoadRec(tls00_TransformationCmd*   pTransformationCmd,
                    tls00_TransformationRTE*&  pTransformationRTE)
{
    ROUTINE_DBG_MEO00 ("ls20_FreeDatLoadRec");

    if ( pTransformationRTE != NULL )
    {
        ls20_FreeStreams(pTransformationCmd, pTransformationRTE);

        delete pTransformationRTE;
        pTransformationRTE = NULL;
    }
}
//  ls20_FreeDatLoadRec()

/*
  -----------------------------------------------------------------------------
  BeginChapter:  Global functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ImportPackageDataRecord
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ImportPackageDataRecord( MetaDataDef *pMetaDataDef, tls00_DBInfo *pDBInfo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ImportPackageDataRecord");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    tsp00_Longint    rc      = errOK_els00;

    TransformationDef       *pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();
    RelationalDef           *pRelationalDef     = pMetaDataDef->getRelationalModel();
    //*
    //*  db|user|table records, formatted, compressed
    //*
    if ( (errOK_els00 == rc) && (NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00) )
    {
        //*
        //* record in pages
        //*
        if ( true  == pTransformationCmd->trUsage_ls00.uFastMode_ls00 )
        {
            pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = true;
            //*
            //*  import transformationpackage 
            //*
            rc = pTransformationDef->ImportPackage(pMetaDataDef, ErrText);

            if ( errOK_els00 == rc )
            {
                //*
                //*  execute transformationpackage to transform data
                //*
                if (ptTable_els00 <= pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
                {
                    pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgExecDataPackage_els98);
                    pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgNumTabToTransform_els98,1);
                }
                //*
                //*  update  transformationpackage
                //*
                rc = ls21ImportTableRowPages(pMetaDataDef, pDBInfo, ErrText);
                //*
                //*  export  transformationpackage
                //*
                if ( errOK_els00 <= rc || -25362 == rc )
                     rc = pTransformationDef->ExportPackage( pMetaDataDef, ErrText);
            }
            pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = false;
        }
        //*
        //* record in rows
        //*
        else
        {
            pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = true;
            //*
            //*  import transformationpackage 
            //*
            rc = pTransformationDef->ImportPackage(pMetaDataDef, ErrText);

            if ( errOK_els00 == rc )
            {
                tls00_TransformationRTE   *pTransformationRTE  = NULL;
                rc = ls20_InitDatLoadRec(pDBInfo, pTransformationCmd, pTransformationRTE, ErrText);

                if (errOK_els00 == rc)
                {
                    //*
                    //*  execute transformationpackage to transform data
                    //*
                    if (ptTable_els00 <= pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
                    {
                        pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgExecDataPackage_els98);
                        pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgNumTabToTransform_els98, pTransformationDef->m_nTableCount);
                    }
                    //*
                    //*  update  transformationpackage
                    //*
                    if (ptAll_els00    == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                        ptUser_els00   == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                        ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
                    {
                        rc = ls20ImportPackageTables(pMetaDataDef, pTransformationRTE, ErrText);
                    }
                    else
                    if (ptTable_els00  == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                        ptColumn_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
                    {
                        rc = ls20ImportTableRows (pMetaDataDef, pTransformationRTE, ErrText);
                    }
                }
                if ( NULL != pTransformationRTE )
                {
                    ls20_FreeDatLoadRec(pTransformationCmd, pTransformationRTE);
                }
            }
            pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = false;
            //*
            //*  execute transformationpackage to transform catalog
            //*
            if ( pRelationalDef->SplitPackage())
                 rc = TransportPackageCatalog( pMetaDataDef, pDBInfo, ErrText);

            //*
            //*  export  transformationpackage
            //*
            if ( errOK_els00 <= rc || -25362 == rc )
                 rc = pTransformationDef->ExportPackage( pMetaDataDef, ErrText);

        }
    }// if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )

    return STATIC_CAST(tsp00_Int4, rc);
}
//  ImportPackageDataRecord()

/*
  -----------------------------------------------------------------------------
  function:     ls20ImportPackageTables
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20ImportPackageTables(MetaDataDef              *&pMetaDataDef,
                        tls00_TransformationRTE  *pTransformationRTE,
                        tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20ImportPackageTables");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    tsp00_Int4      rc           = errOK_els00;

    TransformationDef*        pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd*  pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_DBInfo*             pDBInfo            = pMetaDataDef->getConnection();
    Loader_RTStatistics*      pRTStatistics      = Loader_RTStatistics::Instance();

    StudioOAL_String  SchemaMapName,SchemaMap,select;
    _TCHAR            szselect[2048];
    szselect[0] = 0;

    if ( pTransformationDef->isSchemaMap() == true )
    {
        if (pDBInfo->dbiSession_ls00.siSchemaMapName_ls00[0] != 0 ) {
            SchemaMapName = REINTERPRET_CAST(SAPDB_UTF8*, pDBInfo->dbiSession_ls00.siSchemaMapName_ls00);
            SchemaMap     = _T("AND SYSLOADER.MAPSCHEMANAMES.MAPPINGNAME = \'");
            SchemaMap    += SchemaMapName;
            SchemaMap    += "\'";
            sp77sprintf(szselect, 2048,  pTransformationDef->Statement(sel_Transformation_724).CharPtr(), _T("") ,
            pTransformationCmd->trPackageid_ls00.StrPtr(), SchemaMap.StrPtr());
         } else {
            SchemaMapName = _T("AND SYSLOADER.TRANSFORMATIONMODEL.PACKAGEGUID = SYSLOADER.MAPSCHEMANAMES.PACKAGEGUID ");
            sp77sprintf(szselect, 2048,  pTransformationDef->Statement(sel_Transformation_724).CharPtr(), SchemaMapName.StrPtr(),
            pTransformationCmd->trPackageid_ls00.StrPtr(), _T(""));
         }
    }
    else
         sp77sprintf(szselect, 2048,  pTransformationDef->Statement(sel_Transformation_708).CharPtr(), pTransformationCmd->trPackageid_ls00.StrPtr());

    select.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, szselect));

    if ( !pTransformationDef->executeSQL( select ) )
    {
        StudioOAL_String NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
        pMsgInstance->ls98PrintMessage(layDataLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        rc = pTransformationDef->getReturnCode();
    }

    StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
    if (errOK_els00 == rc)
    {
        sessionUser.Assign(REINTERPRET_CAST  (Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
        currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
        currentSchema = sessionSchema;
    }

	pTransformationDef->m_currentStream = stData_els00;
    /*-------------------------------------------------------------------------------*
     * Read/Write the CATALOG definition of the specified object from/to resources   *
     *-------------------------------------------------------------------------------*/
    while ( pTransformationDef->initTransformationStep()  && errOK_els00 == rc )
    {
        rc = pTransformationDef->switchUserSchema( currentUser, currentSchema, false, ErrText);
        
        currentUser   = pTransformationDef->m_owner ;
        currentSchema = pTransformationDef->m_schemaname ;

        if (errOK_els00 == rc)
        {
             rc = pTransformationDef->initTransformationCmd(srFctColLoad_els00);
           
             if (errOK_els00 == rc)
             {
                 pTransformationRTE->rte_TableSwitched_ls00 = true;
           
                 if (errOK_els00 == rc)
                 {
                     rc = ls20_SwitchStreams(pTransformationDef, pTransformationRTE, ErrText);
                     if (errOK_els00 == rc)
                     {
                         rc = ls20ImportTableRows(pMetaDataDef, pTransformationRTE, ErrText);
                         if (errOK_els00 == rc)
                         {
                             if (false == pTransformationDef->getTransformationCmd()->trExclude_ls00)
                             {
                                pRTStatistics->IncreaseCounter_TablesTranformed();
                             }
                             ErrText[0] = 0;
                         }
                         else
                         {
                             if ( (errInternal_els98 != rc) && (errCancelRequest_els98 != rc) && (errTableToLoadNotCurrentTable_els98 != rc) )
                             {
                                 if ( EOF_LS00 == rc )
                                 {
                                     pMsgInstance->ls98Msg(layDataLoad_els98, ErrText, msgEmptyDataFile_els98, pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00);
                                 }
                                 // Here the 'real' loading MUST be separated from its preparation to adequately react on return codes; furthermore
                                 // - we must log failures on single tables and summarize those in a single message that must be returned to the client.
                                 // - the loader should jump to the next table DATA in infile if the max error count is not yet reached.
                    
                                 // Rejected lines are counted while inserting DATA in vls20.cpp
                                 if (pTransformationRTE->rte_Cnt_RejectedRecords_ls00 < pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors)
                                 {
                                     rc = errOK_els00;
                                     ErrText[0] = 0;
                                 }
                                 else
                                 {
                                     pMsgInstance->ls98Msg(layDataLoad_els98, ErrText, errMaxErrorCountReached_els98);
                                     rc = errMaxErrorCountReached_els98;
                                 }
                             }
                         }
                     }// ls20_SwitchStreams
                 }
             }// pTransformationDef->initTransformationCmd
             pTransformationDef->freeTransformationCmd (srFctColLoad_els00);
        }
    }// while pTransformationDef->initTransformationStep

    if (errOK_els00 == rc)
    {
        pMsgInstance->ls98PrintMessage(layDataLoad_els98, msgNumTabExcludedExport_els98,
                                                          pRTStatistics->GetTablesExcluded());

        pMsgInstance->ls98PrintMessage(layDataLoad_els98, msgNumTabTransformed_els98,
                                                          (pRTStatistics->GetTablesTranformed() -
                                                            pRTStatistics->GetTablesExcluded()));
    }
    //*
    //* switch user back to session user
    //*
    tsp00_Int4 rc2 = pTransformationDef->switchUser( currentUser, sessionUser, ErrText);

    if ( errOK_els00 == rc && errOK_els00 != rc2 )
         rc = rc2;

    return rc;
}
// ls20ImportPackageTables()


/*
  -----------------------------------------------------------------------------
  function:     ls20UpdateTransformationPackage
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls20UpdateTransformationPackage( tls00_DBInfo            *pDBInfo,
                                 MetaDataDef             *&pMetaDataDef,
                                 tls00_TransformationCmd *pTransformationCmd,
                                 tls00_TransformationRTE *pTransformationRTE,
                                 tsp00_Addr        ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20UpdateTransformationPackage");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    tsp00_Int4 rc = errOK_els00;

    if ( pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00 >= ptTable_els00 && NULL == getenv ("maxdbloaderserver"))
    {
         TransformationDef* pTransformationDef = pMetaDataDef->getTransformationModel();
         SAPDB_Int4       ErrorPos   = 0;
         SQLRETURN        ReturnCode = 0;
         StudioOAL_String NativeErrorAsString;
         //*
         //* Performance
         //*
         extern tls00_Performance performance;
         SAPDB_UInt8 start, finish;
         SAPDB_UInt8 duration;
         //*
         //*  update transformation table columns:
         //*  taskid, storagedevice, servernode, serverdb, username, codetype, codepage, transformation, part, streamformat
         //*
        _TCHAR RECORD_LOADED   [256];
        _TCHAR HEADER_BLOCK    [256];
        _TCHAR HEADER_POS      [256];
        _TCHAR DATA_BLOCK      [256];
        _TCHAR DATA_POS        [256];
        _TCHAR TRAILER_BLOCK   [256];
        _TCHAR TRAILER_POS     [256];
        RECORD_LOADED   [0] = 0;
        HEADER_BLOCK    [0] = 0;
        HEADER_POS      [0] = 0;
        DATA_BLOCK      [0] = 0;
        DATA_POS        [0] = 0;
        TRAILER_BLOCK   [0] = 0;
        TRAILER_POS     [0] = 0;
        //*
        //*  datastreamheaderpos,datastreamheaderblock,datastreamdatapos
        //*
        if( pTransformationRTE->rte_TableSwitched_ls00 == true  )
        {
            pTransformationRTE->rte_TableSwitched_ls00  = false;
            // <EXCLUDE>

            // TIMESTAMP BEGIN
            if ( !pTransformationDef->updateBegin() )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layDataLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
            // <DATASTREAMNAME>
            pTransformationDef->m_datastreamname = pTransformationRTE->rte_pDataFile_ls00->vfName_ls00;
            if ( !pTransformationDef->updateDataStream( ) )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
            // <HEADER_BLOCK>:<HEADER_POS>
            _itot(STATIC_CAST(tsp00_Int4, pTransformationRTE->rte_pDataFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00),HEADER_BLOCK,10);
            _itot(STATIC_CAST(tsp00_Int4, pTransformationRTE->rte_pDataFile_ls00->vfFilePos_ls00.lnuRemainder_ls00 ),HEADER_POS,10);
			pTransformationDef->m_headerpos_block = HEADER_BLOCK;
			pTransformationDef->m_headerpos_byte  = HEADER_POS;

            if ( !pTransformationDef->updateHeader() )
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layDataLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
        }// if ( pTransformationRTE->rte_TableSwitched_ls00 == true  )

        // RECORD_LOADED
        _itot(STATIC_CAST(tsp00_Int4, pTransformationRTE->rte_LastCommLine_ls00),RECORD_LOADED,10);
        // <DATA_BLOCK>:<DATA_POS>
        _itot(STATIC_CAST(tsp00_Int4, pTransformationRTE->rte_pDataFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00),DATA_BLOCK,10);
        _itot(STATIC_CAST(tsp00_Int4, pTransformationRTE->rte_pDataFile_ls00->vfFilePos_ls00.lnuRemainder_ls00 ),DATA_POS,10);
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
        finish = RTESys_MicroSecTimer();
        duration = finish - start;
        performance.perLog_ls00 += duration;
        //pTransformationRTE->rte_LastCommLine_ls00  = pTransformationRTE->dlrInsertedRows_ls00;

        if (pTransformationRTE->rte_TabEmpty_ls00  == true ||
            pTransformationRTE->rte_TabLoaded_ls00 == true ||
            pTransformationCmd->trExclude_ls00     == true    )
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
            // <TRAILER_BLOCK>:<TRAILER_POS>
            _itot(STATIC_CAST(tsp00_Int4, pTransformationDef->m_trailerpos.lnu2GB_Blocks_ls00),TRAILER_BLOCK,10);
            _itot(STATIC_CAST(tsp00_Int4, pTransformationDef->m_trailerpos.lnuRemainder_ls00 ),TRAILER_POS  ,10);
			pTransformationDef->m_trailerpos_block = TRAILER_BLOCK;
			pTransformationDef->m_trailerpos_byte  = TRAILER_POS;
            if ( !pTransformationDef->updateTrailer() )
            {
                 ReturnCode          = pTransformationDef->getReturnCode();
                 NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                 pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                 return ReturnCode;
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
        }// if ( pTransformationRTE->rte_TabEmpty_ls00   == true )
    }// end if (pTransformationRTE->rte_PartSpec_ls00.psPartType_ls00 > ptTable_els00)
    return rc;

}//ls20UpdateTransformationPackage

/*
  -----------------------------------------------------------------------------
  function:     ls20_InitStreams
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_InitStreams(tls00_DBInfo*            pDBInfo,
                 tls00_TransformationCmd* pTransformationCmd,
                 tls00_TransformationRTE   *&      pTransformationRTE,
                 tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_InitStreams");

    tsp00_Int4 rc = errOK_els00;
    //*
    //*  initialize DATA stream
    //*
    tls00_FileSpec *pFileSpec  = &pTransformationCmd->trDataFile_ls00;
    tls00_VFile    *pVDataFile = NULL;

    if ( NULL == pTransformationRTE->rte_pDataFile_ls00 )
    {
        pTransformationRTE->rte_pDataFile_ls00  = new tls00_VFile;
        memset(pTransformationRTE->rte_pDataFile_ls00, 0, sizeof(tls00_VFile));
        pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00     = new _TCHAR[PATH_MXSP00];
        pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00[0]  = 0;
        pTransformationRTE->rte_pDataFile_ls00->vfFileType_ls00     = pFileSpec->fsDeviceType_ls00;
        pTransformationRTE->rte_pDataFile_ls00->vfFileEncoding_ls00 = pFileSpec->fsFormat_ls00.ffoCodeType_ls00;
    }

    pVDataFile = pTransformationRTE->rte_pDataFile_ls00;

    //*
    //* medium
    //*
    if ( dtypeTape_ls00 == pFileSpec->fsDeviceType_ls00 )
    {
        _tcscpy( pVDataFile->vfFileName_ls00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00 );

        pVDataFile->vfFileType_ls00   = dtypeTape_ls00;
        pVDataFile->vfBackupTool_ls00 = pDBInfo->dbiSession_ls00.siBackupTool_ls00;
        pVDataFile->vfNodeName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                            pDBInfo->dbiSession_ls00.si_pDBNode_ls00));
        pVDataFile->vfDBName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                          pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp()));
        pVDataFile->vfBackupId_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                            pFileSpec->fsBackupId_ls00.bBackupIdName_ls00));

        pVDataFile->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pFileSpec->fsFileName_ls00));
    }
    else
    {
        if ( ptTable_els00 < pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
        {
            pTransformationRTE->rte_MetaData_ls00.pmdMetaFile_ls00 = pVDataFile;
            pTransformationRTE->rte_bRestart_ls00 = pTransformationCmd->trRestart_ls00.rstRestart_ls00;
        }
        if ( dtypeFile_ls00 == pFileSpec->fsDeviceType_ls00 )
        {
            //*
            //* part
            //*
            if ( ptTable_els00 < pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
            {
                sp77sprintf(pVDataFile->vfFileName_ls00, PATH_MXSP00, "%s%0*d", pFileSpec->fsFileName_ls00,
                                                                                FILE_EXTENSION_SIZE_LS20, 1);
            }
            else
            {
                _tcscpy(pVDataFile->vfFileName_ls00, pFileSpec->fsFileName_ls00);
            }

            //*
            //* streamname
            //*
            _TCHAR *pszStreamName = strrchr(pVDataFile->vfFileName_ls00, SLASH_LS00);
            if (NULL == pszStreamName)
            {
                pszStreamName = strrchr(pVDataFile->vfFileName_ls00, BACKSLASH_LS00);
                if (NULL == pszStreamName)
                    pszStreamName = pVDataFile->vfFileName_ls00;
            }
            pszStreamName = (pszStreamName == pVDataFile->vfFileName_ls00 ) ? pszStreamName : pszStreamName + 1;
            pVDataFile->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pszStreamName));
            //*
            //* streampath
            //*
            pVDataFile->vfPathName_ls00.AssignRaw(REINTERPRET_CAST(unsigned char*, pVDataFile->vfFileName_ls00),
                                                  _tcslen(pVDataFile->vfFileName_ls00)-_tcslen(pszStreamName));
        }
        //*
        //* medium
        //*
        else if ( dtypePipe_ls00 == pFileSpec->fsDeviceType_ls00 )
        {
            _tcscpy(pVDataFile->vfFileName_ls00, pFileSpec->fsFileName_ls00);
            pVDataFile->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                            pFileSpec->fsFileName_ls00));
        }

        //*
        //*     Now initialize runtime data file structure and open data file
        //*
        tsp05_RteDataKind_Param FileDataType = sp5vf_binary;
        if ( (ptTable_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) &&
             (false == pFileSpec->fsFormat_ls00.ffoBinary_ls00) && (false == pFileSpec->fsFormat_ls00.ffoCompact_ls00) )
        {
            FileDataType = sp5vf_text;
        }
        ls30VFInitialize(*pVDataFile,
                         pVDataFile->vfFileName_ls00,
                         FileDataType, sp5vf_read,
                         pFileSpec->fsFormat_ls00.ffoCodeType_ls00);
        //*
        //* open stream
        //*
        rc = ls30VFOpen(*pTransformationRTE->rte_pDataFile_ls00, ErrText);
        if ( (errOK_els00 == rc) && (ctUnknown_els00 == pFileSpec->fsFormat_ls00.ffoCodeType_ls00) )
        {
            pFileSpec->fsFormat_ls00.ffoCodeType_ls00 = pVDataFile->vfFileEncoding_ls00;
        }
    }   // end else of if ( dtypeTape_ls00 == pFileSpec->fsDeviceType_ls00 )

    return rc;

}
// ls20_InitStreams()


/*
  -----------------------------------------------------------------------------
  function:     ls20_SwitchStreams
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls20_SwitchStreams(TransformationDef  *&pTransformationDef,
                   tls00_TransformationRTE     *pTransformationRTE,
                   tsp00_Addr          ErrText)
{
    ROUTINE_DBG_MEO00 ("ls20_SwitchStreams");

    tsp00_Int4 rc = errOK_els00;

    tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_DBInfo            *pDBInfo            = pTransformationDef->getMetaData()->getConnection();
    //*
    //* medium
    //*
    if ( dtypeTape_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
    {
        if (0 != pTransformationDef->m_datastreamebid.Compare("?") &&
            0 != pTransformationDef->m_datastreamebid.Compare( pTransformationRTE->rte_pDataFile_ls00->vfBackupId_ls00 ))
        {
            pTransformationRTE->rte_pDataFile_ls00->vfFileType_ls00   = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;
            pTransformationRTE->rte_pDataFile_ls00->vfBackupTool_ls00 = pDBInfo->dbiSession_ls00.siBackupTool_ls00;
            pTransformationRTE->rte_pDataFile_ls00->vfNodeName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                                            pDBInfo->dbiSession_ls00.si_pDBNode_ls00));
            pTransformationRTE->rte_pDataFile_ls00->vfDBName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                                          pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp()));
            pTransformationRTE->rte_pDataFile_ls00->vfBackupId_ls00   = pTransformationDef->m_datastreamebid;
            //*
            //* initialize stream
            //*
            ls30VFInitialize(*pTransformationRTE->rte_pDataFile_ls00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00 ,sp5vf_binary, sp5vf_read);
            //*
            //* open stream
            //*
            rc = ls30VFOpen(*pTransformationRTE->rte_pDataFile_ls00, ErrText);
        }
    }
    //*
    //* medium
    //*
    if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
    {
        if (false == pTransformationCmd->trInternal_ls00 &&
            UNDEFINED_LS00 != pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00  )
        {
            //*
            //*  seek to right position
            //*
            if (  pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnu2GB_Blocks_ls00 !=  pTransformationRTE->rte_pDataFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00 ||
                  pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00  !=  pTransformationRTE->rte_pDataFile_ls00->vfFilePos_ls00.lnuRemainder_ls00     )
            {
                tls00_LargeNumber lFilePos;
                lFilePos.lnu2GB_Blocks_ls00 =   pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnu2GB_Blocks_ls00 ;
                lFilePos.lnuRemainder_ls00  =   pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00  ;
                rc = ls30VFMoveToStartPosition(*pTransformationRTE->rte_pDataFile_ls00, &lFilePos, ErrText);
            }

        }
        if (true  == pTransformationCmd->trInternal_ls00 )
        {
            if ( 0   != pTransformationDef->m_datastreamname.Compare("?") &&
                 0   != pTransformationDef->m_datastreamname.Compare(pTransformationRTE->rte_pDataFile_ls00->vfName_ls00 ))
            {
                //*
                //* close old file if open
                //*
                ls30VFClose(*pTransformationRTE->rte_pDataFile_ls00, ErrText);
                //*
                //* concatenate file name
                //*
                tsp00_Addr pszFileName = NULL;
                tsp00_Addr pPos = NULL;
                pPos = strrchr(pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00, BACKSLASH_LS00);
                if (NULL == pPos)
                    pPos = strrchr(pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00, SLASH_LS00);
                if (NULL == pPos)
                    pPos = pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00;
                else
                    pPos = &pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00[pPos - pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00 + 1];

                size_t _len = _tcslen(pTransformationDef->m_datastreamname.CharPtr());
                SAPDB_memcpy(pPos,pTransformationDef->m_datastreamname.CharPtr(), _len);
                pPos[_len] = 0;

                //*
                //* open new file
                //*
                rc = ls30VFOpen(*pTransformationRTE->rte_pDataFile_ls00, ErrText);

                //*
                //*  read current filename
                //*
                if (errOK_els00 == rc)
                    pTransformationRTE->rte_pDataFile_ls00->vfName_ls00 = pTransformationDef->m_datastreamname;
            }
            //*
            //*  seek to right position
            //*
            if (  errOK_els00 == rc &&
                (pTransformationDef->m_headerpos.lnu2GB_Blocks_ls00 !=  pTransformationRTE->rte_pDataFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00 ||
                 pTransformationDef->m_headerpos.lnuRemainder_ls00  !=  pTransformationRTE->rte_pDataFile_ls00->vfFilePos_ls00.lnuRemainder_ls00     ))
            {
                tls00_LargeNumber lFilePos;
                lFilePos.lnu2GB_Blocks_ls00 =   pTransformationDef->m_headerpos.lnu2GB_Blocks_ls00 ;
                lFilePos.lnuRemainder_ls00  =   pTransformationDef->m_headerpos.lnuRemainder_ls00  ;
                rc = ls30VFMoveToStartPosition(*pTransformationRTE->rte_pDataFile_ls00, &lFilePos, ErrText);
            }
            //*
            //*  seek to right restart position
            //*
            if ( (errOK_els00 == rc) && (true == pTransformationRTE->rte_bRestart_ls00) )
            {
                rc = ls30VFSeekLong(*pTransformationRTE->rte_pDataFile_ls00, &pTransformationDef->m_headerpos, ErrText, sp5vf_seek_begin);
                //*
                //* reset restart flag
                //*
                pTransformationRTE->rte_bRestart_ls00  = false;
            }
        }
    }
    return rc;

}//ls20_SwitchStreams


/*
  -----------------------------------------------------------------------------
  function:     ls20_FreeStreams
  -----------------------------------------------------------------------------
*/
static void
ls20_FreeStreams( tls00_TransformationCmd*  pTransformationCmd,
                  tls00_TransformationRTE*  pTransformationRTE)
{
    ROUTINE_DBG_MEO00 ("ls20_FreeStreams");

    //*
    //* close datafile
    //*
    if (NULL != pTransformationRTE->rte_pDataFile_ls00)
    {
        // Error is of no interest here
        char szErrText[256];
        ls30VFClose(*pTransformationRTE->rte_pDataFile_ls00, szErrText);

        //*
        //* delete infile structure
        //*
        if (NULL != pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00)
        {
            delete [] pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00;
            pTransformationRTE->rte_pDataFile_ls00->vfFileName_ls00 = NULL;
        }

        delete pTransformationRTE->rte_pDataFile_ls00;
        pTransformationRTE->rte_pDataFile_ls00 = NULL;
    }
    //*
    //* delete meta DATA
    //*
    if (NULL != pTransformationRTE->rte_MetaData_ls00.pmdMetaFile_ls00)
    {
        if (NULL != pTransformationRTE->rte_MetaData_ls00.pmdLengthArray_ls00)
        {
            delete [] pTransformationRTE->rte_MetaData_ls00.pmdLengthArray_ls00;
            pTransformationRTE->rte_MetaData_ls00.pmdLengthArray_ls00 = NULL;
        }
    }
}
//ls20_FreeStreams()


/*===========================================================================*
 *     END
 *===========================================================================*/
