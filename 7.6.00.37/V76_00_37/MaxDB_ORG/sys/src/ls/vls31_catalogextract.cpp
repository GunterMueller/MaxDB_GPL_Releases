/*
  -----------------------------------------------------------------------------
  module: vls31_catalogextract.cpp
  -----------------------------------------------------------------------------

  responsible:  d033893 and SteffenS

  special area: SAP DB LOADER

  description:  Implementation of the CATALOGEXTRACT/CATALOGLOAD functionality

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

#include "gls00.h"
#include "hls03.h" 
#include "hls15.h"
#include "hls30.h"
#include "hls99.h"
#include "hls31_odbc_Result.h"
#include "hls31_catalogextract.h"

/* -----------------------------------------------------------------------------
   ExportCatalogSchemaMap
/-------------------------------------------------------------------------------*/
SAPDB_Int4
ExportCatalogSchemaMap(MetaDataDef *&pMetaDataDef,
                       tls00_DBInfo *pDBInfo,
                       tsp00_Addr    ErrText)
{
    ROUTINE_DBG_MEO00 ("ExportCatalogSchemaMap");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Long rc = errOK_els00;

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 26);

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) ) {
         TransformationDef       * pTransformationDef  = pMetaDataDef->getTransformationModel();
         tls00_TransformationCmd * pTransformationCmd  = pTransformationDef->getTransformationCmd();
         Tools_DynamicUTF8String   schemaMapName;
         schemaMapName.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp()));
         if ( pTransformationDef->existsSchemaMap(schemaMapName, ErrText))
              pTransformationDef->ExportSchemaMap(ErrText);
         else {
             pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errSchemaMapNotExists_els98, schemaMapName.StrPtr());
             rc = errSchemaMapNotExists_els98 ;
         }
    }
    return rc;
}// ExportCatalogSchemaMap

/* -----------------------------------------------------------------------------
   ImportCatalogSchemaMap
/-------------------------------------------------------------------------------*/
SAPDB_Int4
ImportCatalogSchemaMap(MetaDataDef *&pMetaDataDef,
                       tls00_DBInfo *pDBInfo,
                       tsp00_Addr    ErrText)
{
    ROUTINE_DBG_MEO00 ("ImportCatalogSchemaMap");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Long rc = errOK_els00;

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 26);

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) ){
        TransformationDef       * pTransformationDef  = pMetaDataDef->getTransformationModel();
        tls00_TransformationCmd * pTransformationCmd  = pTransformationDef->getTransformationCmd();
        Tools_DynamicUTF8String   schemaMapName;
        schemaMapName.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp()));
        if (!pTransformationDef->existsSchemaMap(schemaMapName, ErrText ))
             rc = pTransformationDef->ImportSchemaMap( ErrText);
        else {
             pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errSchemaMapExists_els98, schemaMapName.StrPtr());
             rc = errSchemaMapExists_els98 ;
        }
    }
    return rc;
}// ImportCatalogSchemaMap

/* -----------------------------------------------------------------------------
   UseCatalogSchemaMap
/-------------------------------------------------------------------------------*/
SAPDB_Int4
UseCatalogSchemaMap(MetaDataDef *&pMetaDataDef,
                    tls00_DBInfo *pDBInfo,
                    tsp00_Addr    ErrText)
{
    ROUTINE_DBG_MEO00 ("UseCatalogSchemaMap");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Long rc = errOK_els00;

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 26);
    Tools_DynamicUTF8String      schemaMapName;

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) ) {
         TransformationDef       * pTransformationDef  = pMetaDataDef->getTransformationModel();
         tls00_TransformationCmd * pTransformationCmd  = pTransformationDef->getTransformationCmd();
         schemaMapName.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp()));
         if ( pTransformationDef   ->existsSchemaMap(schemaMapName,ErrText))
              rc=pTransformationDef->switchSchemaMap(schemaMapName,ErrText);
         else {
              pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errSchemaMapNotExists_els98, schemaMapName.StrPtr());
              rc = errSchemaMapNotExists_els98 ;
         }
    }
    return rc;
}// UseCatalogSchemaMap

/* -----------------------------------------------------------------------------
   TransportPackageCatalog
/-------------------------------------------------------------------------------*/
SAPDB_Int4
TransportPackageCatalog(MetaDataDef  *&pMetaDataDef,
                        tls00_DBInfo  *pDBInfo,
                        tsp00_Addr     ErrText)
{
    ROUTINE_DBG_MEO00 ("TransportPackageCatalog");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    SAPDB_Long rc = errOK_els00;

    TransformationDef       * pTransformationDef  = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd * pTransformationCmd  = pTransformationDef->getTransformationCmd();
    pTransformationCmd->trCatalogFile_ls00.fsCurrentStream_ls00 = true;

    StudioOAL_String  ErrorText;
    StudioTD_String   username;
    StudioTD_String   schemaname;
    tls00_FileInfo    streamInfo;
    // interpret <catalogextract statement>::=
    streamInfo.fileSpec = pTransformationCmd->trCatalogFile_ls00;

    if ( errOK_els00 == rc  )
    {
        //*
        //*  initialize stream
        //*
        streamInfo.vFile.vfFileType_ls00   = streamInfo.fileSpec.fsDeviceType_ls00;

        if ( dtypeTape_ls00 == streamInfo.fileSpec.fsDeviceType_ls00 && rc == errOK_els00)
        {
            streamInfo.vFile.vfBackupTool_ls00 = pDBInfo->dbiSession_ls00.siBackupTool_ls00;
            streamInfo.vFile.vfNodeName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                     pDBInfo->dbiSession_ls00.si_pDBNode_ls00));
            streamInfo.vFile.vfDBName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                   pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp()));
        }
        //*
        //* export catalog ddl, xml
        //*
        if ( tsExport_ls00 == pTransformationCmd->trTransSpec_ls00 )
        {
            //*
            //*  import transformationpackage 
            //*
            if ( false == pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXmlNoTable_ls00  )
                 rc = pTransformationDef->ImportPackage(pMetaDataDef, ErrText);
            else
            {
                StudioOAL_String  transformationpackage;
                transformationpackage  = "Import    PACKAGE ";
                transformationpackage += "x'";
                transformationpackage += pTransformationCmd->trPackageid_ls00;
                transformationpackage += "'";
                pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgCommandExecuting_els98,transformationpackage.StrPtr());
            }
            if (errOK_els00 == rc)
            {
                if (true == pTransformationCmd->trCatalogFile_ls00.fsAppend_ls00)
                {
                    ls30VFInitialize(streamInfo.vFile, pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00, 
                                 sp5vf_text, sp5vf_append, streamInfo.fileSpec.fsFormat_ls00.ffoCodeType_ls00);
                }
                else
                {
                    ls30VFInitialize(streamInfo.vFile, pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00, 
                                 sp5vf_text, sp5vf_write, streamInfo.fileSpec.fsFormat_ls00.ffoCodeType_ls00);
                }
                rc = ls30VFOpen (streamInfo.vFile,ErrText);
            }
            if ( rc != errOK_els00 )
                return rc;

            pMetaDataDef->setMedium( streamInfo );
            //*
            //*  execute transformationpackage
            //*
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgExecSchemaPackage_els98);

            if ( ptTable_els00  < pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
                 false         == pTransformationDef->ExistPackageData(pTransformationCmd->trPackageid_ls00) &&
                 false         == pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXmlNoTable_ls00)
            {
                if ( pDBInfo->dbiSession_ls00.siConfigurationType_ls00 == ctInclude_els00 ) { 
                     pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumTabToTransform_els98    ,pTransformationDef->m_nTableCount);
                }
                if ( pDBInfo->dbiSession_ls00.siConfigurationType_ls00 == ctExclude_els00 ) {
                     pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumTabToTransform_els98    ,pTransformationDef->m_nTableCount+pTransformationDef->m_nTableData);
                     pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumViewToTransform_els98   ,pTransformationDef->m_nViewCount);
                     pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumSynonymToTransform_els98,pTransformationDef->m_nSynonymCount);
                }
            }

            username  .Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
            schemaname.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
            //*
            //*  update  transformationpackage
            //*
            RelationalDef* pRelationalDef = pMetaDataDef->getRelationalModel();
            pTransformationDef->m_currentStream = stCatalog_els00;

            if (errOK_els00 == rc)
            {
                if (ptAll_els00    == pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    rc = pRelationalDef->ExportPackageDb(username, ErrText);
                }
                else if (ptUser_els00   == pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    rc = pRelationalDef->ExportPackageUser(username, ErrText);
                }
                else if (ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    rc = pRelationalDef->ExportPackageSchema(schemaname, ErrText);
                }
                else if (ptTable_els00  == pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    rc = pRelationalDef->ExportPackageTable();
                }
            }

            SAPDB_Int4 rcFile = ls30VFClose(streamInfo.vFile,ErrText);
            if ( (errOK_els00 == rc) && (errOK_els00 != rcFile) )
                 rc = rcFile;
            //*
            //*  storagedevice, servernode, serverdb, username, codetype, codepage, transformation, part, streamformat
            //*
            if ( (dtypeTape_ls00 == pTransformationCmd->trCatalogFile_ls00.fsDeviceType_ls00 ) &&
                 (ptTable_els00   < pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )   )
            {
                pTransformationCmd->trCatalogFile_ls00.fsBackupId_ls00.bBackupIdName_ls00 =
                    CONST_CAST(SAPDB_Char*, streamInfo.vFile.vfBackupId_ls00.CharPtr());
                rc = pTransformationDef->updateCatalogCmd();
            }
            //*
            //*  export transformationpackage
            //*
            if ( errOK_els00 == rc && NULL == pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )
                 rc = pTransformationDef->ExportPackage( pMetaDataDef, ErrText );
        }//if ( tsExport == pTransformationCmd->trTransSpec_ls00  && rc == errOK_els00 )

        //*
        //* import catalog ddl
        //*
        else if ( tsImport_ls00 == pTransformationCmd->trTransSpec_ls00  )
        {
            if ( dtypeTape_ls00 == pTransformationCmd->trCatalogFile_ls00.fsDeviceType_ls00 )
            {
                streamInfo.vFile.vfBackupId_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                        streamInfo.fileSpec.fsBackupId_ls00.bBackupIdName_ls00));
            }

            ls30VFInitialize(streamInfo.vFile, pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00,
                             sp5vf_text, sp5vf_read, streamInfo.fileSpec.fsFormat_ls00.ffoCodeType_ls00);
            rc = ls30VFOpen (streamInfo.vFile, ErrText);
            if (errOK_els00 == rc)
            {
                pMetaDataDef->setMedium( streamInfo );

                username.Assign(REINTERPRET_CAST  (Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
                schemaname.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
                //*
                //*  execute transformationpackage
                //*
                RelationalDef* pRelationalDef = pMetaDataDef->getRelationalModel();
                pTransformationDef->m_currentStream = stCatalog_els00;

                pMsgInstance->ls98PrintMessage(layCatalogLoad_els98, msgExecSchemaPackage_els98);

                if (ptAll_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    rc = pRelationalDef->ImportPackageDb(username);
                }
                else if (ptUser_els00   == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
                    rc = pRelationalDef->ImportPackageUser(username);
                }
                else if (ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
                    rc = pRelationalDef->ImportPackageSchema(schemaname);
                }
                else if (ptTable_els00  == pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    rc = pRelationalDef->ImportPackageTable(schemaname);
                }

                SAPDB_Int4 rcFile = ls30VFClose(streamInfo.vFile,ErrText);
                if ( (errOK_els00 == rc) && (errOK_els00 != rcFile) )
                    rc = rcFile;

                if (errOK_els00 == rc)
                {
                    //*
                    //*  import transformationpackage 
                    //*
                    rc = pTransformationDef->ImportPackage ( pMetaDataDef, ErrText );
                }
                if (errOK_els00 != rc)
                {
                    ErrorHndl *pError = pMetaDataDef->getError();
                    pMsgInstance->ls98MsgToString(layTableExtract_els98, ErrText, STATIC_CAST(ErrorNumbers, rc), pError->getErrorText().StrPtr());
                }
                if ( ptTable_els00  < pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
                     false         == pTransformationDef->ExistPackageData(pTransformationCmd->trPackageid_ls00) &&
                     false         == pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXmlNoTable_ls00  )
                {
                    Loader_RTStatistics::Instance()->SetCatalogObjTransformed(pTransformationDef->m_nTableCount);
                    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumTabTransformed_els98, pTransformationDef->m_nTableCount);
                }
                //*
                //*  export  transformationpackage
                //*
                if ( errOK_els00 == rc && NULL == pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )
                     rc = pTransformationDef->ExportPackage( pMetaDataDef, ErrText );
            }
        } // if ( pTransformationCmd->trTransSpec_ls00 == tsImport )
    }
    pTransformationCmd->trCatalogFile_ls00.fsCurrentStream_ls00 = false;
    return rc;
}// TransportPackageCatalog

/*===========================================================================*
 *     END
 *===========================================================================*/
