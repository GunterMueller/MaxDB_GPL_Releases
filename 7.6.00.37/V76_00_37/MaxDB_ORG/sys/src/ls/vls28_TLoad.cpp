/*!
  -----------------------------------------------------------------------------
  module: vls28_TLoad.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of TABLELOAD

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
#include "hls05.h"
#include "hls07.h"
#include "hls10.h"
#include "hls11.h"
#include "hls13.h"
#include "hls15.h"
#include "hls20_dataload.h"
#include "hls31_catalogextract.h"
#include "hls26_TETable.h"
#include "hls28_TLTable.h"
#include "hls28_TLoad.h"
#include "hls30.h"
#include "hls51.h"

#include "hls98msg.h"
#include "hls99.h"

#include <time.h>

/*
  -----------------------------------------------------------------------------
  BeginChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  function:     ls28_InitTabLoadRec
  -----------------------------------------------------------------------------

  description:  Creates and initializes the structure used for
                execution of table load.
                Initializes and opens mapping file, DATA file and restart file.

  arguments:    pDBInfo            [IN]     - structure that contains all info on the
                                              running session and the database connected to.
                pTransformationCmd [IN]     - tableload command structure
                pTableLoadRec      [IN|OUT] - tableload structure that must be initialized.
                ErrText            [OUT]    - error text

  returnvalue:  errOK_els00, errNoMoreMemory_els00
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_InitTabLoadRec(tls00_DBInfo             *pDBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_TloadRec           *&pTableLoadRec,
                    tsp00_Addr               ErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls28_FreeTabLoadRec
  -----------------------------------------------------------------------------

  description:  Deletes all allocated space and the tableload structure.

  arguments:    pTableLoadRec     [IN|OUT] - tableload structure that must be initialized.
                pTransformationCmd     [IN]     - tableload command structure

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls28_FreeTabLoadRec(tls00_TloadRec          *pTableLoadRec,
                    tls00_TransformationCmd *pTransformationCmd,
                    tsp00_Addr              ErrText);

/*
  -----------------------------------------------------------------------------

  function: ls28_InitStreams

  -----------------------------------------------------------------------------

  description:

  arguments:    pTabLoadRec     [IN]  - structure, holding information for executing
                                        TABLELOAD for a single table
                pTransformationCmd     [IN]  - tableload command structure
                ErrText        [OUT]  - error text

  returnvalue:

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_InitStreams ( tls00_DBInfo            *pDBInfo,
                   tls00_TransformationCmd *pTransformationCmd,
                   tls00_TloadRec          *&pTabLoadRec,
                   tsp00_Addr              ErrText      );

/*
  -----------------------------------------------------------------------------

  function: ls28_SwitchStreams

  -----------------------------------------------------------------------------

  description:

  arguments:    pDBInfo         [IN]  - structure that contains all info on the
                                        running session and the database connected to.
                pTabLoadRec     [IN]  - structure, holding information for executing
                                        TABLELOAD for a single table
                pTransformationCmd     [IN]  - tableload command structure
                ErrText         [OUT] - error text

  returnvalue:

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_SwitchStreams(TransformationDef *&pTransformationDef,
                   tls00_TloadRec    *pTabLoadRec,
                   ReadBuffer        *&pReadBuffer,
                   tsp00_Addr         pszErrText);

/*
  -----------------------------------------------------------------------------

  function: ls28_FreeStreams

  -----------------------------------------------------------------------------

  description:

  arguments:    pTabLoadRec     [IN]  - structure, holding information for executing
                                        TABLELOAD for a single table
                pTransformationCmd     [IN]  - tableload command structure
                ErrText         [OUT] - error text

  returnvalue:

  -----------------------------------------------------------------------------
*/
static void
ls28_FreeStreams ( tls00_TransformationCmd *pTransformationCmd,
                   tls00_TloadRec          *pTabLoadRec,
                   tsp00_Addr              ErrText      );

/*!
  -----------------------------------------------------------------------------
  function:     ls28_GetTargetKernelSwap
  -----------------------------------------------------------------------------

  description:  Gets the swap of kernel from target db.

  arguments:    pDBInfo     [IN]  - structure that contains all info on the
                                    running session and the database connected to.
                pTabLoadRec [IN]  - structure, holding information for executing
                                    TABLELOAD for a single table
                ErrText     [OUT] - error text

  returnvalue:  errOK_els00              - no error

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetTargetKernelSwap(tls00_DBInfo   *pDBInfo,
                         tls00_TloadRec *pTabLoadRec,
                         tsp00_Addr      ErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls28ReadDataFileHeader
  -----------------------------------------------------------------------------

  description:  Reads header of newly opened DATA file (infile) and checks if the
                DATA file was created by TABLEEXTRACT and checks some flags.
                Fills some values in pTabLoadRec.

  arguments:    pTabLoadRec     [IN/OUT]  - structure, holding information for executing
                                             TABLELOAD for a single table
                pPartSpec       [IN]      - structure holding info on the kind of TL
                                            (USER/ALL)
                ErrText          [OUT]     - error text

  returnvalue:  errOK_els00                     - no error
                errTLMultTabFileSingleTab_els98 - DATA file with multiple table DATA
                                                  but TL command for single table
                errTLSingleTabFileMultTab_els98 - DATA file with DATA of one single
                                                  table but TL command for multiple
                                                  tables

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28ReadDataFileHeader(tls00_DBInfo            *pDBInfo,
                       tls00_TloadRec          *pTabLoadRec,
                       tls00_TransformationCmd *pTransformationCmd,
                       tsp00_Addr              ErrText);

/*!
  -----------------------------------------------------------------------------

  function:     ls28_ReadDataFileHeader
  -----------------------------------------------------------------------------

  description:  Reads the header of a DATA file generated using TABLEEXTRACT and
                checks for corruption.
                Sets some necessary values in ppTabLoadRec: tlrSwapFromTE_ls00, tlrCount_ls00

                Does not check code type, version, date, time and if more than one
                table are in file.

  arguments:    pTabLoadRec [IN]   - pointer; structure, holding information for
                                      executing TABLELOAD for a single table
                ErrText  [OUT]     - pointer; error text

  returnvalue:  errOK_els00              - no error
                errWrongTEFileType_els98 - wrong identifier in header (file was
                                           probably not generated using TE or is
                                           corrupted
                errWrongTEPageSize_els98 - page sizes of source and target system
                                           do not match
                some error from reading the file
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ReadDataFileHeader(tls00_DBInfo *pDBInfo,
                        tls00_TloadRec *pTabLoadRec,
                        tsp00_Addr      ErrText);



/*!
  -----------------------------------------------------------------------------
  function:     ls28_HandleRestart
  -----------------------------------------------------------------------------

  description:  Restart handler. In case of restart the function reads the restart
                file and finds the name of table in mapping file restart begins
                with.

  arguments:    pRestartStruct  [IN/OUT] - structure with info for restart
                pMapFile        [IN]     - mapping file
                ErrText         [OUT]    - error text

  returnvalue:  errOK_els00                     - no error
                errTLRestartFileOpen_els98      - failure open restart file
                errTLRestartEntryNotFound_els98 - restart entry not found in
                                                  mapping file

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_HandleRestart(tls00_Restart *pRestartStruct,
                   tls00_VFile   *pMapFile,
                   tsp00_Addr     ErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls28_ImportTablePages
  -----------------------------------------------------------------------------

  description:  Executes TABLELOAD TABLE <table name> command.

  arguments:    DBInfo      [IN]  - structure that contains all info on the
                                    running session and the database connected to.
                pTabLoadRec [IN]  - structure, holding information for executing
                                    TABLELOAD for a single table
                ErrText     [OUT] - error text

  returnvalue:  errOK_els00                     - no error
                errors from loading single table

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ImportTablePages(MetaDataDef      *&pMetaDataDef,
                    tls00_TloadRec   *pTabLoadRec,
                    tsp00_Addr        ErrText);

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  BeginChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls28_InitTabLoadRec
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_InitTabLoadRec(tls00_DBInfo     *pDBInfo,
                    tls00_TransformationCmd *pTransformationCmd,
                    tls00_TloadRec   *&pTabLoadRec,
                    tsp00_Addr        ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_InitTabLoadRec");

    tsp00_Int4 rc = errOK_els00;

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    pTabLoadRec = new tls00_TloadRec;

    if (NULL == pTabLoadRec)
    {
        rc = errNoMoreMemory_els98;
    }
    else
    {
        memset(pTabLoadRec, 0, sizeof(tls00_TloadRec));

        if ( true == pTransformationCmd->trRestart_ls00.rstRestart_ls00 )
             pTabLoadRec->tlrRestart_ls00.rstRestart_ls00 = true;

        //  Number of possible pages per packet to send
        //  tls00_PktBlock contains packet-, segment- and part header size. Because each packet
        //  consists of 2 parts the part header has to be substracted again.
        pTabLoadRec->tlrPgPerPckt_ls00 = STATIC_CAST( tsp00_Int2, (pDBInfo->dbiPktSize_ls00 - sizeof(tls00_PktBlock)
                                                                - sizeof(tsp1_part_header)
                                                                - sizeof(pTabLoadRec->tlrBdInfo_ls00.szbdiData_ls00)) / pDBInfo->dbiPageSize_ls00 );

        // In case the page size allows only one page per packet we need to stop processing;
        // otherwise the kernel would core
        if (1 == pTabLoadRec->tlrPgPerPckt_ls00)
        {
            pMsgInstance->ls98Msg(layTableLoad_els98, ErrText, errTLPacketSizeTooSmall_els98, (pDBInfo->dbiPageSize_ls00/1024));
            rc = errTLPacketSizeTooSmall_els98;
        }
        else
        {
            pTabLoadRec->tlrPageSize_ls00  = pDBInfo->dbiPageSize_ls00;
            pTabLoadRec->tlrSwapFromDb_ls00.becomes(sw_dummy);
            pTabLoadRec->tlrSwapFromTE_ls00.becomes(sw_dummy);
            pTabLoadRec->tlrPartSpec_ls00  = pTransformationCmd->trPartSpec_ls00;
            pTabLoadRec->tlrPageTypeExtracting_ls00 = tptUnknown_els00;
            rc = ls28_InitStreams(pDBInfo,pTransformationCmd, pTabLoadRec, ErrText);
        }
    }
    //*
    //*  define target kernel swap
    //*
    if (errOK_els00 == rc)
         rc = ls28_GetTargetKernelSwap(pDBInfo, pTabLoadRec, ErrText);

    return rc;
} //ls28_InitTabLoadRec

/*
  -----------------------------------------------------------------------------
  function:     ls28_InitStreams
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_InitStreams(tls00_DBInfo            *pDBInfo,
                 tls00_TransformationCmd *pTransformationCmd,
                 tls00_TloadRec          *&pTabLoadRec,
                 tsp00_Addr              ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_InitStreams");

    tsp00_Int4  rc = errOK_els00;

    //*
    //*  initialize DATA stream
    //*
    if ( NULL == pTabLoadRec->tlrInfile_ls00 )
    {
         pTabLoadRec->tlrInfile_ls00 = new tls00_VFile;
         pTabLoadRec->tlrInfile_ls00->vfFileName_ls00 = new _TCHAR[PATH_MXSP00];
         pTabLoadRec->tlrInfile_ls00->vfFileName_ls00[0] = 0;
         pTabLoadRec->tlrMappingfile_ls00 = new tls00_VFile;
         pTabLoadRec->tlrMappingfile_ls00->vfFileName_ls00 = new _TCHAR[PATH_MXSP00];
         pTabLoadRec->tlrMappingfile_ls00->vfFileName_ls00[0] = 0;
    }
    //*
    //* medium
    //*
    if ( dtypeTape_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
    {
         pTabLoadRec->tlrInfile_ls00->vfFileType_ls00 = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;

        _tcscpy(pTabLoadRec->tlrInfile_ls00->vfFileName_ls00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00 );

        pTabLoadRec->tlrInfile_ls00->vfBackupTool_ls00 = pDBInfo->dbiSession_ls00.siBackupTool_ls00;
        pTabLoadRec->tlrInfile_ls00->vfNodeName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                             pDBInfo->dbiSession_ls00.si_pDBNode_ls00));
        pTabLoadRec->tlrInfile_ls00->vfDBName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                           pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp()));
        pTabLoadRec->tlrInfile_ls00->vfBackupId_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                             pTransformationCmd->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00));
    }
    //*
    //* medium
    //*
    else if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
    {
         _TCHAR sztransformationstream[PATH_MXSP00];
         sztransformationstream[0] = 0;
         pTabLoadRec->tlrInfile_ls00->vfFileType_ls00 = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;
         //*
         //* part
         //*
         if ( ptTable_els00 <  pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
              sp77sprintf(sztransformationstream, PATH_MXSP00, "%s%0*d", pTransformationCmd->trDataFile_ls00.fsFileName_ls00, FILE_EXTENSION_SIZE_LS26, 1);
         if ( ptTable_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
              sp77sprintf(sztransformationstream, PATH_MXSP00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00 );

         _tcscpy(pTabLoadRec->tlrInfile_ls00->vfFileName_ls00, sztransformationstream );
         sztransformationstream[0] = 0;
         sp77sprintf(sztransformationstream, PATH_MXSP00, "%s%0*d", pTransformationCmd->trDataFile_ls00.fsFileName_ls00, FILE_EXTENSION_SIZE_LS26, 0);
         _tcscpy(pTabLoadRec->tlrMappingfile_ls00->vfFileName_ls00, sztransformationstream );
         ls30VFInitialize(*pTabLoadRec->tlrInfile_ls00, pTabLoadRec->tlrInfile_ls00->vfFileName_ls00,sp5vf_binary, sp5vf_read);
         //*
         //* open stream
         //*
         rc = ls30VFOpen(*pTabLoadRec->tlrInfile_ls00, ErrText);
         //*
         //*  read pagefile header
         //*
         if (errOK_els00 == rc)
             rc = ls28ReadDataFileHeader(pDBInfo, pTabLoadRec, pTransformationCmd, ErrText);
    }
    //*
    //* medium
    //*
    else if (dtypePipe_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
    {
         _tcscpy(pTabLoadRec->tlrInfile_ls00->vfFileName_ls00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00 );
         pTabLoadRec->tlrInfile_ls00->vfFileType_ls00 = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;
         ls30VFInitialize(*pTabLoadRec->tlrInfile_ls00, pTabLoadRec->tlrInfile_ls00->vfFileName_ls00, sp5vf_binary, sp5vf_read );
         //*
         //* open stream
         //*
         rc = ls30VFOpen(*pTabLoadRec->tlrInfile_ls00, ErrText);
         //*
         //*  read pagefile header
         //*
         if (errOK_els00 == rc)
             rc = ls28ReadDataFileHeader(pDBInfo, pTabLoadRec, pTransformationCmd, ErrText);
    }

    //*
    //* streamname
    //*
    _TCHAR *    sztransformationstreamname = strrchr(pTabLoadRec->tlrInfile_ls00->vfFileName_ls00, SLASH_LS00);
    if (NULL == sztransformationstreamname)
    {
        sztransformationstreamname = strrchr(pTabLoadRec->tlrInfile_ls00->vfFileName_ls00, BACKSLASH_LS00);
        if (NULL == sztransformationstreamname)
            sztransformationstreamname = pTabLoadRec->tlrInfile_ls00->vfFileName_ls00;
    }
    sztransformationstreamname = (sztransformationstreamname == pTabLoadRec->tlrInfile_ls00->vfFileName_ls00 )  ? sztransformationstreamname : sztransformationstreamname + 1;
    //*
    //* streampath
    //*
    if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
        pTabLoadRec->tlrInfile_ls00->vfPathName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                             pTabLoadRec->tlrInfile_ls00->vfFileName_ls00));

    pTabLoadRec->tlrInfile_ls00->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                     sztransformationstreamname));

    return rc;

}// ls28_InitStreams

/*
  -----------------------------------------------------------------------------
  function:     ls28_SwitchStreams
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_SwitchStreams(TransformationDef  *&pTransformationDef,
                   tls00_TloadRec     *pTabLoadRec,
                   ReadBuffer         *&pReadBuffer,
                   tsp00_Addr          pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_SwitchStreams");

    tsp00_Int4 rc = errOK_els00;

    tls00_TransformationCmd* pTransformationCmd  = pTransformationDef->getTransformationCmd();
    tls00_DBInfo*            pDBInfo             = pTransformationDef->getMetaData()->getConnection();
    tls00_VFile*             pInfile             = pTabLoadRec->tlrInfile_ls00;
    SAPDB_Bool               bNewFileOpened      = SAPDB_FALSE;
    //*
    //* medium
    //*
    if ( dtypeTape_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
    {
        if ( 0   != pTransformationDef->m_datastreamebid.Compare("?") &&
             0   != pTransformationDef->m_datastreamebid.Compare( pInfile->vfBackupId_ls00 ))
        {
            pInfile->vfFileType_ls00   = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;
            pInfile->vfBackupTool_ls00 = pDBInfo->dbiSession_ls00.siBackupTool_ls00;
            pInfile->vfNodeName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                             pDBInfo->dbiSession_ls00.si_pDBNode_ls00));
            pInfile->vfDBName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                           pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp()));
            pInfile->vfBackupId_ls00   = pTransformationDef->m_datastreamebid;
            //*
            //* initialize stream
            //*
            ls30VFInitialize(*pInfile, pTransformationCmd->trDataFile_ls00.fsFileName_ls00, sp5vf_binary, sp5vf_read);
            //*
            //* open stream
            //*
            rc = ls30VFOpen(*pInfile, pszErrText);
            //*
            //*  read pagefile header
            //*
            if (errOK_els00 == rc)
            {
                rc = ls28ReadDataFileHeader(pDBInfo, pTabLoadRec, pTransformationCmd, pszErrText);
                bNewFileOpened = SAPDB_TRUE;
            }
        }
    }
    //*
    //* medium
    //*
    else if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
    {
        if (false == pTransformationCmd->trInternal_ls00)
        {
            if (UNDEFINED_LS00 != pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00)
            {
                //*
                //*  seek to right position
                //*
                if (pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnu2GB_Blocks_ls00 !=  pInfile->vfFilePos_ls00.lnu2GB_Blocks_ls00 ||
                    pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00  !=  pInfile->vfFilePos_ls00.lnuRemainder_ls00     )
                {
                    tls00_LargeNumber lFilePos;
                    lFilePos.lnu2GB_Blocks_ls00 = pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnu2GB_Blocks_ls00 ;
                    lFilePos.lnuRemainder_ls00  = pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00  ;
                    rc = ls30VFMoveToStartPosition(*pInfile, &lFilePos, pszErrText);
                }
            }
        }
        else
        {
            if ( false == pTransformationDef->m_datastreamname.Empty() &&
                 0     != pTransformationDef->m_datastreamname.Compare("?") &&
                 0     != pTransformationDef->m_datastreamname.Compare(pInfile->vfName_ls00 ))
            {
                //*
                //* close old file if open
                //*
                ls30VFClose(*pInfile, pszErrText);
                //*
                //* concatenate file name
                //*
                tsp00_Addr pszFileName = NULL;
                tsp00_Addr pPos = NULL;
                pPos = strrchr(pInfile->vfFileName_ls00, BACKSLASH_LS00);
                // unix file?
                if (NULL == pPos)
                    pPos = strrchr(pInfile->vfFileName_ls00, SLASH_LS00);
                if (NULL == pPos)
                    pPos = pInfile->vfFileName_ls00;
                else
                    pPos = &pInfile->vfFileName_ls00[pPos - pInfile->vfFileName_ls00 + 1];

                size_t _len = _tcslen(CONST_CAST(SAPDB_Char*, pTransformationDef->m_datastreamname.CharPtr()));
                SAPDB_memcpy(pPos,
                             CONST_CAST(SAPDB_Char*, pTransformationDef->m_datastreamname.CharPtr()),
                             _len);
                pPos[_len] = 0;

                //*
                //* open new file
                //*
                rc = ls30VFOpen(*pInfile, pszErrText);
                if (errOK_els00 == rc)
                {
                    //*
                    //*  read current filename
                    //*
                    pInfile->vfName_ls00 = pTransformationDef->m_datastreamname;
                    //*
                    //*  read pagefileheader
                    //*
                    rc = ls28ReadDataFileHeader(pDBInfo, pTabLoadRec, pTransformationCmd, pszErrText);

                    bNewFileOpened = SAPDB_TRUE;
                }
            }
            //*
            //*  seek to right position
            //*
            if (errOK_els00 == rc)
            {
                if (true == pTabLoadRec->tlrRestart_ls00.rstRestart_ls00)
                {
                    //*
                    //* reset restart flag
                    //*
                    pTabLoadRec->tlrRestart_ls00.rstRestart_ls00 = false;
                }
                //*
                //* Set buffer to new file pos
                //*
                pReadBuffer->ls51SetFilePos(pTransformationDef->m_headerpos);
            }
        }   // end if (true  == pTransformationCmd->trInternal_ls00 )
    }   // end else if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )


    //*
    //*     Delete ReadBuffer object which handled the closed file and
    //*     attach NEW ReadBuffer object to newly opened file
    //*
    if (SAPDB_TRUE == bNewFileOpened)
    {
        if (NULL != pReadBuffer)
        {
            delete pReadBuffer;
        }
        pReadBuffer = new ReadBuffer(pInfile, pTabLoadRec->tlrPageSize_ls00, pTabLoadRec->tlrPgPerPckt_ls00);
    }
    return rc;
}
// ls28_SwitchStreams()

/*
  -----------------------------------------------------------------------------
  function:     ls28_FreeStreams
  -----------------------------------------------------------------------------
*/
static void
ls28_FreeStreams( tls00_TransformationCmd *pTransformationCmd,
                  tls00_TloadRec          *pTabLoadRec,
                  tsp00_Addr              ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_FreeStreams");

    //*
    //* mappingfile
    //*
    if (NULL != pTabLoadRec->tlrMappingfile_ls00)
    {
        if ( NULL != pTabLoadRec->tlrMappingfile_ls00->vfFileName_ls00 )
        {
            delete [] pTabLoadRec->tlrMappingfile_ls00->vfFileName_ls00;
            pTabLoadRec->tlrMappingfile_ls00->vfFileName_ls00 = NULL;
        }
        delete pTabLoadRec->tlrMappingfile_ls00;
        pTabLoadRec->tlrMappingfile_ls00 = NULL;
    }
    //*
    //* datafile
    //*
    if (NULL != pTabLoadRec->tlrInfile_ls00)
    {
        if (0 != pTabLoadRec->tlrInfile_ls00->vfFileHndl_ls00)
            ls30VFClose(*pTabLoadRec->tlrInfile_ls00,ErrText );

        if (pTabLoadRec->tlrInfile_ls00->vfFileName_ls00 != pTransformationCmd->trDataFile_ls00.fsFileName_ls00)
        {
            delete [] pTabLoadRec->tlrInfile_ls00->vfFileName_ls00;
            pTabLoadRec->tlrInfile_ls00->vfFileName_ls00 = NULL;
        }
        delete pTabLoadRec->tlrInfile_ls00;
        pTabLoadRec->tlrInfile_ls00 = NULL;
    }

}// ls28_FreeStreams

/*
  -----------------------------------------------------------------------------
  function:     ls28_GetTargetKernelSwap
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetTargetKernelSwap(tls00_DBInfo *pDBInfo, tls00_TloadRec *pTabLoadRec, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_GetTargetKernelSwap");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR szCmd[256];
    szCmd[0] = 0;
    _tcscpy(szCmd, "CREATE TABLE LOA_SWAP (COL1 INT)");

    tsp00_Int4 lErrPos;
    _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];

    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, ErrText, errInternSQL_els98,
                                                                             szCmd, szErr);
        rc = errInternSQL_els98;
    }
    else
    {
        _TCHAR szTabName[128] = {'L','O','A','_','S','W','A','P'};

        tsp00_Addr pszTD = NULL;
        tsp00_Int4 lLen  = 0;

        rc = ls13GetTableDescription(pDBInfo, 0, szTabName, lLen, pszTD, ErrText);
        if (errOK_els00 == rc)
        {
            pTabLoadRec->tlrTargetSwap_ls00 = (REINTERPRET_CAST(tsp7_table_description*, pszTD))->sp7t_kernel_swap();

            szCmd[0] = 0;
            _tcscpy(szCmd, "DROP TABLE LOA_SWAP");
            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos);
        }
        if (errOK_els00 == rc)
        {
             szCmd[0] = 0;
             _tcscpy(szCmd, "COMMIT");
             rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos);
        }
    }

    szCmd[0] = 0;
    _tcscpy(szCmd, "COMMIT");
    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos);

    TRACE_PRNF_MLS99(("ls28_GetTargetKernelSwap", "Target DB kernel swap = %d.\n", STATIC_CAST(int, pTabLoadRec->tlrTargetSwap_ls00)));

    return rc;
}
// ls28_GetTargetKernelSwap()

/*
  -----------------------------------------------------------------------------
  function: ls28_FreeTabLoadRec
  -----------------------------------------------------------------------------
*/
static void
ls28_FreeTabLoadRec( tls00_TloadRec *pTabLoadRec, tls00_TransformationCmd *pTransformationCmd,tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_FreeTabLoadRec");

    if ( pTabLoadRec != NULL )
    {
        ls28_FreeStreams(pTransformationCmd,pTabLoadRec,ErrText);

        if (NULL != pTabLoadRec->tlrRestart_ls00.rstRestartEntry_ls00)
        {
            delete [] pTabLoadRec->tlrRestart_ls00.rstRestartEntry_ls00;
            pTabLoadRec->tlrRestart_ls00.rstRestartEntry_ls00 = NULL;
        }

        delete pTabLoadRec;
        pTabLoadRec = NULL;
    }

    return;
}
// ls28_FreeTabLoadRec()

/*
  -----------------------------------------------------------------------------
  function: ls28ReadDataFileHeader
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28ReadDataFileHeader(tls00_DBInfo      *pDBInfo,
                       tls00_TloadRec    *pTabLoadRec,
                       tls00_TransformationCmd  *pTransformationCmd,
                       tsp00_Addr         ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28ReadDataFileHeader");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    tsp00_Int4       rc      = errOK_els00;

    rc = ls28_ReadDataFileHeader(pDBInfo, pTabLoadRec, ErrText);        // read file header (1024 byte)
    if (errOK_els00 == rc)
    {
        // check for right encoding types - ascii may only be loaded to ascii db and
        // ucs2 may  only be loaded to ucs2 db
        if (true == ls00IsDBUnicode(pDBInfo))
        {
            if (ctAscii_els00 == pTabLoadRec->tlrSourceEncoding_ls00)
            {
                pMsgObj->ls98Msg(layTableLoad_els98, ErrText, errTLSrcAndTgtEncodingDifferent_els98,"ASCII", "UNICODE");
                rc = errTLSrcAndTgtEncodingDifferent_els98;

            }

            // Here we need to check for different swap of the lserver while
            // extracting and loading
            if ( tsp00_SwapKind_Enum(pDBInfo->dbiOwnSwap_ls00) != tsp00_SwapKind_Enum(pTabLoadRec->tlrSwapFromTE_ls00) )
            {
                pMsgObj->ls98Msg(layTableLoad_els98, ErrText, errTLSrcAndTgtSwapDifferent_els98);
                rc = errTLSrcAndTgtSwapDifferent_els98;
            }
        }
        else
        {
            if ( (ctUCS2_els00        == pTabLoadRec->tlrSourceEncoding_ls00) ||
                 (ctUCS2Swapped_els00 == pTabLoadRec->tlrSourceEncoding_ls00) ||
                 (ctUCS2Native_els00  == pTabLoadRec->tlrSourceEncoding_ls00)  )
            {
                pMsgObj->ls98Msg(layTableLoad_els98, ErrText, errTLSrcAndTgtEncodingDifferent_els98,"UNICODE", "ASCII");
                rc = errTLSrcAndTgtEncodingDifferent_els98;
            }
        }

        // Trace swap behavior of machine repman run on during TABLEEXTRACT
        TRACE_PRNF_MLS99(("ls28ReadDataFileHeader", "Swap at TE = %d\n", STATIC_CAST(int, pTabLoadRec->tlrSwapFromTE_ls00)));

        // Check in header DATA for multiple table flag and reject if not set because we cannot
        // process multiple table DATA files with a single table DATA file
        if (errOK_els00 == rc)
        {
            if (pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00)
            {
                if (false == pTabLoadRec->tlrMultTabInFile_ls00)
                {
                    pMsgObj->ls98Msg(layTableLoad_els98, ErrText, errTLSingleTabFileMultTab_els98);
                    rc = errTLSingleTabFileMultTab_els98;
                }
            }
            else
            {
                if (( pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnu2GB_Blocks_ls00 == UNDEFINED_LS00 ||
                      pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00  == UNDEFINED_LS00 ) &&
                      true == pTabLoadRec->tlrMultTabInFile_ls00 )
                {
                    pMsgObj->ls98Msg(layTableLoad_els98, ErrText, errTLMultTabFileSingleTab_els98,
                                                                  pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp());
                    rc = errTLMultTabFileSingleTab_els98;
                }
            }
        }
    }
    return rc;

}
// ls28ReadDataFileHeader()

/*
  -----------------------------------------------------------------------------
  function:     ls28_ReadDataFileHeader
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ReadDataFileHeader(tls00_DBInfo *pDBInfo, tls00_TloadRec *pTabLoadRec, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28ReadDataFileHeader");

    LoaderMessages  *MsgInstance = LoaderMessages::Instance();    // Für error reports
    tsp00_Int4        rc          = errOK_els00;
    _TCHAR           *pszBuffer   = NULL;

    // Header at extract
    //---------------------------------------------------------------------------------------------------------------
    // loader    | code |            | loader  |      |      | page size    | count of        | more than | kernel  |
    // swap at   | type | identifier | version | date | time | of extracted | pages in        | one table | version |
    // extract   |      |            |         |      |      | db           | file (blockung) | in file ? |         |
    //---------------------------------------------------------------------------------------------------------------
    // 1 byte    |  1   |      64    | 40      | 8    | 8    |    4         |  4              |   1       | 40      |
    //
    // In earlier versions (7.3) the last version entry may not be present. This will be
    // handled here, too.


    int lReadLen = 0;
    lReadLen = ls30VFReadLn(*pTabLoadRec->tlrInfile_ls00, &pszBuffer, ErrText, TE_FILE_HEADER_LENGTH_LS00);
    if (lReadLen < 0)
    {
        // there seems to be an error
        rc = lReadLen;
    }
    else
    {
        size_t lBufPos = 0;

        pTabLoadRec->tlrSwapFromTE_ls00.becomes(tsp00_SwapKind::fromInt(STATIC_CAST(int, pszBuffer[lBufPos])));
        lBufPos += sizeof(unsigned char);


        // Get encoding type of source database
        pTabLoadRec->tlrSourceEncoding_ls00 = STATIC_CAST(tls00_CodeType, pszBuffer[lBufPos]);
        lBufPos += sizeof(unsigned char);

        // Check if file created by Load Server
        int result = memcmp(&pszBuffer[lBufPos], HEADER_IDENTIFIER_LS00, sizeof(HEADER_IDENTIFIER_LS00));
        if (0 != result)
        {
            MsgInstance->ls98Msg(layTableLoad_els98, ErrText, errWrongTEFileType_els98,
                                                              pTabLoadRec->tlrInfile_ls00->vfFileName_ls00,
                                                              "wrong identifier");
            rc = errWrongTEFileType_els98;
        }
        else
        {
            lBufPos += 64;

            //*
            //*     Check loader versions
            //*

            // Target loader version (the one running ;-) )
            tsp100_VersionID1 LoaderTgtVersion;
            sp100_GetVersionID(VersionIDType0_esp100, s100buildnumberEx, &LoaderTgtVersion);

            // Source loader version (logged in DATA file)
            tsp100_VersionID1 LoaderSrcVersion;

            LoaderSrcVersion.MajorVersion_sp100 = pszBuffer[lBufPos + BEG_OF_1ST_VERS_SUBSTR_SP100]   - '0';
            LoaderSrcVersion.MinorVersion_sp100 = pszBuffer[lBufPos + BEG_OF_1ST_VERS_SUBSTR_SP100+2] - '0';
            LoaderSrcVersion.CorrLevel_sp100    = pszBuffer[lBufPos + BEG_OF_1ST_VERS_SUBSTR_SP100+4] - '0';

            lBufPos += VERSION_MXSP00;

            // Skip date and time
            lBufPos += DATE_MXSP00 + TIME_MXSP00;

            // Get page size of source db
            tsp00_Int4 lExtractedPageSize = ls07Swap4BHexToLong(&pszBuffer[lBufPos],
                                                                pTabLoadRec->tlrSwapFromTE_ls00,
                                                                ls07DefineSwap(NULL));
            lBufPos += sizeof(tsp00_Int4);

            // Get count
            pTabLoadRec->tlrCount_ls00 = ls07Swap4BHexToLong(&pszBuffer[lBufPos],
                                                        pTabLoadRec->tlrSwapFromTE_ls00,
                                                        ls07DefineSwap(NULL));
            lBufPos += sizeof(tsp00_Int4);

            // Get info if only one or more tables expected in DATA file
            pTabLoadRec->tlrMultTabInFile_ls00 = (pszBuffer[lBufPos] != 0);
            lBufPos += sizeof(char);


            // Source Kernel version
            // supposing there is no kernel version logged the version information is simply
            // set to 0 because the extract initializes the header buffer with zeroes
/* Check disabled as long as the binary page layout is compatible between different versions
            tsp100_VersionID1 KernelSrcVersion;

            KernelSrcVersion.MajorVersion_sp100 = pszBuffer[lBufPos + BEG_OF_1ST_VERS_SUBSTR_SP100]   - '0';
            KernelSrcVersion.MinorVersion_sp100 = pszBuffer[lBufPos + BEG_OF_1ST_VERS_SUBSTR_SP100+2] - '0';
            KernelSrcVersion.CorrLevel_sp100    = pszBuffer[lBufPos + BEG_OF_1ST_VERS_SUBSTR_SP100+4] - '0';
*/
            lBufPos += VERSION_MXSP00;


            //*
            //*     Check different parameters
            //*

            // page size of source and target system must match
            if (lExtractedPageSize != pTabLoadRec->tlrPageSize_ls00)
            {
                MsgInstance->ls98Msg(layTableLoad_els98, ErrText, errWrongTEPageSize_els98,
                                                                  lExtractedPageSize,
                                                                  pTabLoadRec->tlrPageSize_ls00);
                rc = errWrongTEPageSize_els98;
            }


            if ( (errOK_els00 == rc)  &&
                 (IsEqual_esp100 != sp100_Compare2VersionIDs(VersionIDType0_esp100, &LoaderSrcVersion, &LoaderTgtVersion)) )
            {
                MsgInstance->ls98Msg(layTableLoad_els98, ErrText, errTLWrongLoaderVersion_els98,
                                                                  STATIC_CAST(tsp00_Int4, LoaderSrcVersion.MajorVersion_sp100),
                                                                  STATIC_CAST(tsp00_Int4, LoaderSrcVersion.MinorVersion_sp100),
                                                                  STATIC_CAST(tsp00_Int4, LoaderSrcVersion.CorrLevel_sp100),
                                                                  STATIC_CAST(tsp00_Int4, LoaderTgtVersion.MajorVersion_sp100),
                                                                  STATIC_CAST(tsp00_Int4, LoaderTgtVersion.MinorVersion_sp100),
                                                                  STATIC_CAST(tsp00_Int4, LoaderTgtVersion.CorrLevel_sp100));
                rc = errTLWrongLoaderVersion_els98;
            }

            //*
            //*     Check kernel versions
            //*
/* Check disabled as long as the binary page layout is compatible between different versions
            if (errOK_els00 == rc)
            {
                // First check if there is something to compare
                if ( (6 == KernelSrcVersion.MajorVersion_sp100) ||
                     (7 == KernelSrcVersion.MajorVersion_sp100) ||
                     (8 == KernelSrcVersion.MajorVersion_sp100)  )
                {
                    // Well, seems to be not completely useless - try to compare
                    if (IsEqual_esp100 != sp100_Compare2VersionIDs(VersionIDType0_esp100, &KernelSrcVersion, &pDBInfo->dbiDBVersion_ls00) )
                    {
                        MsgInstance->ls98Msg(layTableLoad_els98, ErrText,
                                                                 errTLWrongKernelVersion_els98,
                                                                 STATIC_CAST(tsp00_Int4, KernelSrcVersion.MajorVersion_sp100),
                                                                 STATIC_CAST(tsp00_Int4, KernelSrcVersion.MinorVersion_sp100),
                                                                 STATIC_CAST(tsp00_Int4, KernelSrcVersion.CorrLevel_sp100),
                                                                 STATIC_CAST(tsp00_Int4, pDBInfo->dbiDBVersion_ls00.MajorVersion_sp100),
                                                                 STATIC_CAST(tsp00_Int4, pDBInfo->dbiDBVersion_ls00.MinorVersion_sp100),
                                                                 STATIC_CAST(tsp00_Int4, pDBInfo->dbiDBVersion_ls00.CorrLevel_sp100));
                        rc = errTLWrongKernelVersion_els98;
                    }
                }
            }
*/
        }   // end else of if (0 != result)
    }   // end if (lReadLen < 0)

    return rc;
}
// ls28_ReadDataFileHeader()

/*
  -----------------------------------------------------------------------------
  function:     ls28_HandleRestart
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_HandleRestart(tls00_Restart *pRestartStruct,
                   tls00_VFile   *pMapFile,
                   tsp00_Addr     ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_HandleRestart");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();    // error report object

    tsp00_Int4 rc = errOK_els00;

    bool bRestartEntryFound = false;

    // read restart entry from restart file
    tsp00_Int4 lReadLen = ls30VFReadLnEx(*pRestartStruct->rstRestartFile_ls00,
                                         pRestartStruct->rstRestartEntry_ls00,
                                         ErrText,
                                         RestartEntryLen_ls28);
    if (lReadLen > 0)
    {
        pRestartStruct->rstRestartEntry_ls00[lReadLen] = 0;     // set trailing 0


        _TCHAR pszData[2 * PATH_MXSP00];                            // pointer holding read DATA
        _TCHAR *pszPos              = NULL;                         // pointer for searching the string

        size_t        lRestartEntryLen = _tcslen(pRestartStruct->rstRestartEntry_ls00);
        tsp00_Longint lRDFNameLen      = 2 * PATH_MXSP00;

        pRestartStruct->rstRestartDataFileName_ls00 = new _TCHAR[lRDFNameLen];

        // Read DATA for first time and then until restart entry is found
        tsp00_Int4 lByteCount = ls30VFReadLnEx(*pMapFile, pszData, ErrText, 256);
        while ( (EOF_LS00 != lByteCount) && (lByteCount > 0) )
        {
            // check for either file name (line start with '[') or for the restart entry
            if ('[' == pszData[0])
            {
                pszPos = strchr(pszData, ']');

                tsp00_Longint len = STATIC_CAST(tsp00_Longint, (&pszPos[0] - &pszData[1]));
                if (len > lRDFNameLen)
                {
                    lRDFNameLen = len;
                    delete pRestartStruct->rstRestartDataFileName_ls00;
                    pRestartStruct->rstRestartDataFileName_ls00 = new _TCHAR[len + 1];
                }
                SAPDB_memcpy(pRestartStruct->rstRestartDataFileName_ls00, &pszData[1], len);
                pRestartStruct->rstRestartDataFileName_ls00[len] = 0;
            }
            else
            {
                if (0 == memcmp(pszData, pRestartStruct->rstRestartEntry_ls00, lRestartEntryLen))
                {
                    // When the restart entry is located in mapping file we need to get the end
                    // position because the restart entry always specifies the
                    // last successfully loaded table;
                    // position specification looks like this
                    // <BEGIN BLOCKS>:<BEGIN POS>-<END BLOCKS>:<END POS>
                    pszPos = strchr(pszData, '-');          // find backwards the position separating dash
                    if (NULL != pszPos)
                    {
                        pRestartStruct->rstRestartPos_ls00.lnu2GB_Blocks_ls00 = atoi(pszPos+1);

                        pszPos = strchr(pszPos, ':');       // find separating colon
                        if (NULL != pszPos)
                        {
                            pRestartStruct->rstRestartPos_ls00.lnuRemainder_ls00 = atoi(pszPos+1);
                            bRestartEntryFound = true;
                        }
                    }
                    break;
                }
            }

            lByteCount = ls30VFReadLnEx(*pMapFile, pszData, ErrText, 256);
        }

        if ( (lByteCount < 0) && (lByteCount != EOF_LS00) )
        {
            // Error reading mapping file; the error is already logged and we return
            pMsgInstance->ls98Msg(layTableLoad_els98, ErrText, errTLRestartFileOpen_els98,
                                                             pMapFile->vfFileName_ls00);
            rc = errTLRestartFileOpen_els98;
        }
        else
        {
            if (false == bRestartEntryFound)
            {
                pMsgInstance->ls98Msg(layTableLoad_els98, ErrText, errTLRestartEntryNotFound_els98,
                                                                 pRestartStruct->rstRestartEntry_ls00,
                                                                 pMapFile->vfFileName_ls00);
                rc = errTLRestartEntryNotFound_els98;
            }
        }
    }
    else
    {
        if (EOF_LS00 != lReadLen)
        {
            pMsgInstance->ls98Msg(layTableLoad_els98, ErrText, errTLRestartFileOpen_els98,
                                                             pRestartStruct->rstRestartFile_ls00->vfFileName_ls00);
            rc = errTLRestartFileOpen_els98;
        }
    }   // end if (lReadLen > 0)

    return rc;
}
// ls28_HandleRestart()


/*
  -----------------------------------------------------------------------------
  function:     ls28_ImportTablePages
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ImportTablePages(MetaDataDef      *&pMetaDataDef,
                      tls00_TloadRec   *pTabLoadRec,
                      tsp00_Addr        ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_ImportTablePages");

    tsp00_Longint rc = errOK_els00;

    TransformationDef       * pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd * pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_DBInfo            * pDBInfo            = pMetaDataDef->getConnection();
    //*
    //*   initialize read buffer class
    //*
    ReadBuffer *pReadBuffer = NULL;
    pReadBuffer = new ReadBuffer(pTabLoadRec->tlrInfile_ls00, pTabLoadRec->tlrPageSize_ls00, pTabLoadRec->tlrPgPerPckt_ls00);
    rc = ls28_SwitchStreams(pTransformationDef, pTabLoadRec, pReadBuffer, ErrText);
    if (errOK_els00 == rc )
    {
        rc = ls28ImportTablePages(pMetaDataDef, pTabLoadRec, *pReadBuffer, ErrText);
    }
    if (errOK_els00 == rc)
    {
        LoaderMessages::Instance()->ls98PrintMessage(layTableLoad_els98, msgTLSuccessful_els98);
    }
    if (NULL != pReadBuffer)
    {
        pReadBuffer->ls51ResetFilePos();

        delete pReadBuffer;
        pReadBuffer = NULL;
    }
    return STATIC_CAST(tsp00_Int4, rc);
}
// ls28_ImportTablePages()

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

tsp00_Int4 ImportPackageData( MetaDataDef *pMetaDataDef , tls00_DBInfo *pDBInfo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ImportPackageData"); 
    LoaderMessages*  pMsgInstance = LoaderMessages::Instance();

    tsp00_Int4  rc  = errOK_els00;

    TransformationDef*        pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd*  pTransformationCmd = pTransformationDef->getTransformationCmd();
	RelationalDef*            pRelationalDef     = pMetaDataDef->getRelationalModel();
    tls00_TloadRec*           pTabLoadRec        = NULL;
    Loader_RTStatistics*      pStatObj           = Loader_RTStatistics::Instance();

    // Initialize statistics counter
    pStatObj->Init();


    //*
	//*  execute transformationpackage to map catalogschemas
	//*
    if ( ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
         0  == pTransformationCmd->trSchemaMap_ls00.msCount_ls00 &&
         0  != pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00[0])
    {
        Tools_DynamicUTF8String schemaMapName(REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp()));
        if ( pTransformationDef->existsSchemaMap(schemaMapName,ErrText) )
        {
             rc  = pTransformationDef->switchSchemaMap(schemaMapName,ErrText);
        }
        else
        {
             pMsgInstance->ls98Msg(layTableLoad_els98, ErrText, errSchemaMapNotExists_els98, schemaMapName.StrPtr());
             rc = errSchemaMapNotExists_els98;
        }
    }
    if ( (errOK_els00 == rc) &&
         (ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00) &&
         (0 != pTransformationCmd->trSchemaMap_ls00.msCount_ls00) )
    {
         rc = pTransformationDef->ImportSchemaMap( ErrText);
         if ( errOK_els00 == rc ) {
              rc = pTransformationDef->switchSchemaMap("",ErrText);
         }
    }
    //*
	//*  execute transformationpackage to transform catalog
	//*
    if ( errOK_els00 == rc && NULL != pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 )
        rc = TransportPackageCatalog( pMetaDataDef, pDBInfo, ErrText);

    //*
    //*  db|user|table records, compressed, formattted, pages
    //*
    if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 && 
         ( errOK_els00 == rc || pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors > 1 ) )
    {
        //*
        //*   db|user|table records, compressed, formattted
        //*
        if ( false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00 ) {
             rc = ImportPackageDataRecord (pMetaDataDef, pDBInfo, ErrText );
        }
        //*
        //*   db|user|table pages
        //*
        else 
        if ( true  == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00 )
        {
            pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = true;
			//*
            //*  import transformationpackage 
            //*
            rc = pTransformationDef->ImportPackage( pMetaDataDef, ErrText);
            if ( rc == errOK_els00 )
            {
				rc = ls28_InitTabLoadRec(pDBInfo, pTransformationCmd, pTabLoadRec, ErrText);
                if ( errOK_els00   == rc )
                {
                    //*
                    //*  execute transformationpackage
                    //*
					if (ptTable_els00 <  pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
                        SAPDB_FALSE  ==  pTransformationCmd->trInternal_ls00)
                    {
                        pMsgInstance->ls98PrintMessage(layTableLoad_els98, msgExecDataPackage_els98 );
                        pMsgInstance->ls98PrintMessage(layTableLoad_els98, msgNumTabToTransform_els98, pTransformationDef->m_nTableData);
                    }
                    //*
                    //*  update  transformationpackage
                    //*
                    if (ptAll_els00    == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                        ptUser_els00   == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                        ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
                    {
                        rc = ls28ImportPackageTables(pMetaDataDef, pTabLoadRec, ErrText);
                    }
                    else if (ptTable_els00  == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
                    {
                        rc = ls28_ImportTablePages(pMetaDataDef, pTabLoadRec, ErrText);
                    }
                }
				if (NULL != pTabLoadRec)
				{
					ls28_FreeTabLoadRec(pTabLoadRec, pTransformationCmd,ErrText);
					pTabLoadRec = NULL;
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
            if (errOK_els00 == rc )
                rc = pTransformationDef->ExportPackage( pMetaDataDef, ErrText);
        }
    }// if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )
    
    // export schemamap
    if ( ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
         0 != pTransformationCmd->trSchemaMap_ls00.msCount_ls00 )
         pTransformationDef->ExportSchemaMap(ErrText);

    // import statistics
    if ( errOK_els00 == rc && (NULL != pTransformationCmd->trStatisticFile_ls00) )
    {
        rc = pTransformationDef->ImportStatistic();
        if (errOK_els00 != rc)
        {
            if (SAPDB_TRUE == pMetaDataDef->m_pError->isError())
                pMsgInstance->ls98Msg(layTableLoad_els98, ErrText, STATIC_CAST(ErrorNumbers, rc), pMetaDataDef->m_pError->getErrorText().StrPtr());
        }
    }

    //*
    //* Add some runtime statistics information to the return string - CATALOG
    //*
    if ( NULL != pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 )
    {
        if (pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00)
        {
            pMsgInstance->ls98AppendStatusToString(layCatalogExtract_els98, ErrText, statNumTablesTransformed_Catalog_els98,
                                                                                     "imported",
                                                                                     pStatObj->GetCatalogObjTransformed());
        }
    }
    //*
    //* Add some runtime statistics information to the return string - DATA
    //*
    if (ptTable_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
    {
        if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
        {
            pMsgInstance->ls98AppendStatusToString(layTableExtract_els98, ErrText, statNumImportedPages_els98,
                                                                                    pStatObj->GetPagesCount(Loader_RTStatistics::ALLDATA),
                                                                                    pStatObj->GetPagesCount(Loader_RTStatistics::PRIMARYDATA),
                                                                                    pStatObj->GetPagesCount(Loader_RTStatistics::LONGDATA));
        }
    }
    else
    {
        pMsgInstance->ls98AppendStatusToString(layTableExtract_els98, ErrText, statNumTablesTransformed_Data_els98,
                                                                                "imported",
                                                                                pStatObj->GetTablesTranformed(),
                                                                                pStatObj->GetTablesExcluded(),
                                                                                pStatObj->GetTablesFailed());
    }

    return rc;
}
// ImportPackageData()

/*
  -----------------------------------------------------------------------------
  function:     ls28ImportPackageTables
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls28ImportPackageTables(MetaDataDef      *&pMetaDataDef,
                        tls00_TloadRec   *pTabLoadRec,
                        tsp00_Addr       ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28ImportPackageTables");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    tsp00_Int4       rc           = errOK_els00;

    TransformationDef*        pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd*  pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_DBInfo*             pDBInfo            = pMetaDataDef->getConnection();
    Loader_RTStatistics*      pStatObj           = Loader_RTStatistics::Instance();

    // Define count of excluded tables
    pStatObj->SetTablesExcluded(pTransformationDef->m_nTableData);

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
        StudioOAL_String    NativeErrorAsString;
        NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
        pMsgInstance->ls98PrintMessage(layDataLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        rc = pTransformationDef->getReturnCode();
    }

    StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;

    if (errOK_els00 == rc)
    {
        sessionUser.  Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
        currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,pDBInfo->dbiSession_ls00.siSchemaName_ls00));
        currentSchema = sessionSchema;
    }
    //*
    //*   initialize read buffer class
    //*
    ReadBuffer *pReadBuffer = NULL;
    // First time initializing; has to be done again upon opening new data files in ls28_SwitchStreams
    pReadBuffer = new ReadBuffer(pTabLoadRec->tlrInfile_ls00, pTabLoadRec->tlrPageSize_ls00, pTabLoadRec->tlrPgPerPckt_ls00);

	pTransformationDef->m_currentStream = stData_els00;
    /*-------------------------------------------------------------------------------*
     * Read/Write the CATALOG definition of the specified object from/to resources   *
     *-------------------------------------------------------------------------------*/
    while( pTransformationDef->initTransformationStep() && errOK_els00 == rc )
    {
          rc = pTransformationDef->initTransformationCmd(srFctTabLoad_els00);

          if (errOK_els00 == rc)
          {
              pTabLoadRec->tlrTableSwitched_ls00 = true;

              rc = pTransformationDef->switchUserSchema( currentUser, currentSchema, false, ErrText);

              if (errOK_els00 == rc)
              {
                  rc = ls28_SwitchStreams(pTransformationDef , pTabLoadRec, pReadBuffer, ErrText);
                  if (errOK_els00 == rc )
                  {
                      rc = ls28ImportTablePages( pMetaDataDef, pTabLoadRec,*pReadBuffer, ErrText);
                      if (errOK_els00 == rc)
                      {
                          pStatObj->IncreaseCounter_TablesTranformed();
                          ErrText[0] = 0;
                      }
                      else
                      {
                          if ( (STOP_NODATA_LS28 != rc) && (errInternal_els98 != rc) && (errCancelRequest_els98 != rc) )
                          {
                              pStatObj->IncreaseCounter_TablesFailed();
                              if (pStatObj->GetTablesFailed() <= pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors)
                              {
                                  ErrText[0] = 0;
                                  rc = errOK_els00;
                              }
                          }
                      }
                  } // end ls28_SwitchStreams
              } // end pTransformationDef->switchUserSchema
              currentUser   = pTransformationDef->m_owner ;
              currentSchema = pTransformationDef->m_schemaname ;
          }//pTransformationDef->initTransformationCmd
          pTransformationDef->freeTransformationCmd (srFctTabLoad_els00);
    }// while pTransformationDef->initTransformationStep

    if (errOK_els00 == rc)
    {
        pMsgInstance->ls98PrintMessage(layTableLoad_els98, msgNumTabExcludedExport_els98, pStatObj->GetTablesExcluded());

        pMsgInstance->ls98PrintMessage(layTableLoad_els98, msgNumTabTransformed_els98, pStatObj->GetTablesTranformed());
    }
    else if (STOP_NODATA_LS28 == rc)
    {
        rc = errOK_els00;
    }
    //*
    //* switch user back to session user
    //*
    pTransformationDef->switchUser( currentUser, sessionUser, ErrText);

    if (NULL != pReadBuffer)
    {
        pReadBuffer->ls51ResetFilePos();

        delete pReadBuffer;
        pReadBuffer = NULL;
    }
    return rc;
}
// ls28ImportPackageTables()

/*
  -----------------------------------------------------------------------------
  function:     ls28UpdateTransformationPackage
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls28UpdateTransformationPackage(MetaDataDef              *&pMetaDataDef,
                                tls00_TransformationCmd  *pTransformationCmd,
                                tls00_TloadRec           *pTabLoadRec,
                                ReadBuffer               &readBuffer,
                                tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28UpdateTransformationPackage");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    tsp00_Int4 rc = errOK_els00;

    if ( pTabLoadRec->tlrPartSpec_ls00.psPartType_ls00 > ptTable_els00  )
    {
        TransformationDef *pTransformationDef = pMetaDataDef->getTransformationModel();
        tls00_DBInfo      *pDBInfo            = pMetaDataDef->getConnection();

        SQLRETURN        ReturnCode = 0;
        StudioOAL_String NativeErrorAsString;

        //*
        //*  update transformation table columns:
        //*  taskid, storagedevice, servernode, serverdb, username, codetype, codepage, transformation, part, streamformat
        //*
        SAPDB_Bool rcBool = SAPDB_TRUE;

        tls00_VFile *_pInFile = pTabLoadRec->tlrInfile_ls00;
        StudioOAL_String dat_stream;

        _TCHAR HEADER_BLOCK[256];
        _TCHAR HEADER_POS[256];
        _TCHAR DATA_BLOCK[256];
        _TCHAR DATA_POS[256];
        HEADER_BLOCK[0] = 0;
        HEADER_POS[0]   = 0;
        DATA_BLOCK[0]   = 0;
        DATA_POS[0]     = 0;

		if ( true == pTabLoadRec->tlrTableSwitched_ls00 )
        {
            // TIMESTAMP BEGIN
            pTabLoadRec->tlrTableSwitched_ls00 = false;

            rcBool = pTransformationDef->updateBegin();

            if (SAPDB_TRUE == rcBool)
            {
                // <DATASTREAMNAME>
                pTransformationDef->m_datastreamname = pTabLoadRec->tlrInfile_ls00->vfName_ls00;

                rcBool = pTransformationDef->updateDataStream();

                if (SAPDB_TRUE == rcBool)
                {
                    // <HEADER_BLOCK>:<HEADER_POS>
                    if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
                    {
                        _itot(STATIC_CAST(tsp00_Int4, _pInFile->vfFilePos_ls00.lnu2GB_Blocks_ls00), HEADER_BLOCK, 10);
                        _itot(STATIC_CAST(tsp00_Int4, _pInFile->vfFilePos_ls00.lnuRemainder_ls00), HEADER_POS, 10);
                    }
                    else
                    {
                        tls00_LargeNumber NewFilePos;
                        readBuffer.ls51RecalculateFilePos(NewFilePos);

                        if (-1 == NewFilePos.lnu2GB_Blocks_ls00)
                        {
                            _itot(STATIC_CAST(tsp00_Int4, _pInFile->vfFilePos_ls00.lnu2GB_Blocks_ls00), HEADER_BLOCK, 10);
                            _itot(STATIC_CAST(tsp00_Int4, _pInFile->vfFilePos_ls00.lnuRemainder_ls00), HEADER_POS, 10);
                        }
                        else
                        {
                            _itot(STATIC_CAST(tsp00_Int4, NewFilePos.lnu2GB_Blocks_ls00), HEADER_BLOCK, 10);
                            _itot(STATIC_CAST(tsp00_Int4, NewFilePos.lnuRemainder_ls00), HEADER_POS, 10);
                        }
                    }
			        pTransformationDef->m_headerpos_block = HEADER_BLOCK;
			        pTransformationDef->m_headerpos_byte  = HEADER_POS;
                    rcBool = pTransformationDef->updateHeader();
                }
            }// end if (SAPDB_TRUE == rcBool)
            if (SAPDB_FALSE == rcBool)
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();

                pMsgInstance->ls98PrintMessage(layTableLoad_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
        }// end if ( pTabLoadRec->tlrTableSwitched_ls00 == true  )

        if ( (true == pTabLoadRec->tlrTabEmpty_ls00) ||
             (true == pTabLoadRec->tlrTabLoaded_ls00)||
             (true == pTransformationCmd->trExclude_ls00)     )
        {
            SAPDB_Bool rcBool = SAPDB_TRUE;

            pTabLoadRec->tlrTabEmpty_ls00   = false;
            pTabLoadRec->tlrTabLoaded_ls00  = false;

            // <DATA_BLOCK>:<DATA_POS>
            tls00_LargeNumber NewFilePos;
            readBuffer.ls51RecalculateFilePos(NewFilePos);

            if (-1 == NewFilePos.lnu2GB_Blocks_ls00)
            {
                _itot(STATIC_CAST(tsp00_Int4, _pInFile->vfFilePos_ls00.lnu2GB_Blocks_ls00), DATA_BLOCK, 10);
                _itot(STATIC_CAST(tsp00_Int4, _pInFile->vfFilePos_ls00.lnuRemainder_ls00), DATA_POS, 10);
            }
            else
            {
                _itot(STATIC_CAST(tsp00_Int4, NewFilePos.lnu2GB_Blocks_ls00), DATA_BLOCK, 10);
                _itot(STATIC_CAST(tsp00_Int4, NewFilePos.lnuRemainder_ls00), DATA_POS, 10);
            }

			pTransformationDef->m_rows_imported = "0";
		    pTransformationDef->m_datapos_block = DATA_BLOCK;
		    pTransformationDef->m_datapos_byte  = DATA_POS;
            rcBool = pTransformationDef->updateImportData();
            if (SAPDB_TRUE == rcBool)
            {
                // RESTART
                rcBool = pTransformationDef->updateRestart();
                if (SAPDB_TRUE == rcBool)
                    // TIMESTAMP END
                    rcBool = pTransformationDef->updateEnd();
            }
            else
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
        }// end if (  pTabLoadRec->tlrTabEmpty_ls00   == true )
    }
    return rc;
}
// ls28UpdateTransformationPackage()

/*===========================================================================*
 *     END
 *===========================================================================*/