/*!
  -----------------------------------------------------------------------------
  module: vls26.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of TABLEEXTRACT

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
#include "hls04.h"
#include "hls041.h"
#include "hls05.h"
#include "hls07.h"
#include "hls12.h"
#include "hls13.h"
#include "hls15.h"
#include "hls27_dataextract.h"
#include "hls29_dataupdate.h"
#include "hls30.h"
#include "hls31_catalogextract.h"
#include "hls98msg.h"
#include "hls99.h"

#include "hls31_odbc_Result.h"

#include "hls26_TETable.h"
#include "hls26.h"


/*
  -----------------------------------------------------------------------------
  BeginChapter  : Local functions declaration
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------

  function:     ls26_InitTabExtrRec  - local function

  -----------------------------------------------------------------------------

  description:  initialize structure used for execution of table extract


  arguments:    pDBInfo      [IN]   - structure with all information on the running
                                      session and the db connected to.
                pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pTabExtrRec  [IN]   - tableextract record structure that must be initialized.

  returnvalue:  errOK_els00, errNoMoreMemory_els00
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_InitTabExtrRec(tls00_DBInfo             *DBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_TextractRec        *&pTabExtrRec,
                    tsp00_Addr               ErrText);

/*
  -----------------------------------------------------------------------------
  function: ls26_FreeTabExtrRec
  -----------------------------------------------------------------------------
  description:  destroys the tableextract structure built in ls26_InitTabExtrRec

  arguments:    pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pTabExtrRec  [IN]   - tableextract record structure that must be destroyed.

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls26_FreeTabExtrRec( tls00_TextractRec        *pTabExtrRec,
                     tls00_TransformationCmd  *pTransformationCmd,
                     tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls26_InitStreams  - local function

  -----------------------------------------------------------------------------

  description:  initialize streams used for execution of table extract


  arguments:    pDBInfo      [IN]   - structure with all information on the running
                                      session and the db connected to.
                pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pTabExtrRec  [IN]   - tableextract record structure that must be initialized.

  returnvalue:  errOK_els00
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_InitStreams(tls00_DBInfo             *pDBInfo,
                 tls00_TransformationCmd  *pTransformationCmd,
                 tls00_TextractRec        *pTabExtrRec,
                 tsp00_Addr               ErrText);

/*!
  -----------------------------------------------------------------------------
   function:     ls26_SwitchStreams
  -----------------------------------------------------------------------------

  description:

  arguments:    pDBInfo          [IN]  - structure with all info on the running
                                         session and the database connected to.
                pTabExtrRec      [IN]  - structure with all info needed
                                         to execute table extract
                pTransformationCmd      [IN]  - structure with information for te supplied by the user

  returnvalue:  errOK_els00          - No error
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_SwitchStreams(MetaDataDef       *pMetaDataDef,
                   tls00_TextractRec *pTabExtrRec,
                   tsp00_Addr        ErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls26_CloseStreams
  -----------------------------------------------------------------------------

  description:  initialize structure used for execution of table extract


  arguments:    pTabExtrRec  [IN]   - tableextract record structure that must be initialized.
                ErrText      [OUT]  - error text

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
static void
ls26_CloseStreams(tls00_TextractRec *pTabExtrRec, tsp00_Addr ErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls26_FreeStreams  - local function

  -----------------------------------------------------------------------------

  description:  initialize structure used for execution of table extract


  arguments:    pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pTabExtrRec  [IN]   - tableextract record structure that must be initialized.
                ErrText      [OUT]  - error text

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
static void
ls26_FreeStreams(tls00_TransformationCmd  *pTransformationCmd,
                 tls00_TextractRec        *pTabExtrRec,
                 tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------
  function: ls26_FileLTMediumSize
  -----------------------------------------------------------------------------

  description:  Checks if the file length is lower than the user given medium size.
                At the moment the given medium size is defined by the server itself
                and has a value of 1 GB.
                Handles files greater than 2 GB.


  arguments:    lFileLenBlock    [IN] - number of 2GB blocks of file length
                lFileLen         [IN] - rest of file length
                lMediumSizeBlock [IN] - number of 2GB blocks of medium size
                lMediumSize      [IN] - rest of medium size
                lAddToFileLen    [IN] - addend that shculd be added to file size

  returnvalue:  true   - file length is lower than medium size
                false  - the opposite
  -----------------------------------------------------------------------------
*/
bool
ls26_FileLenLTMediumSize(tls00_LargeNumber lFileLen,
                         tls00_LargeNumber lMediumSize,
                         tsp00_Longint     lAddToFileLen);     // in Byte ?! assuming that not greater than 2 GB

/*
  -----------------------------------------------------------------------------
  function: ls26_GetTableSize
  -----------------------------------------------------------------------------

  description:  Gets the size of a table from db in blocks of 8kB.

  arguments:    pDBInfo      [IN] - structure with all information on the running
                                     session and the db connected to.
                schemaname   [IN] - name of schema of table
                tablename    [IN] - table name

  returnvalue:  > 0 - size of table in kB
                0   - error while executing
                In case of error the error is only logged to the trace because
                it has no impact on the extraction of this table.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_GetTableSize(tls00_DBInfo *pDBInfo,
                  Tools_DynamicUTF8String &schemaname,
                  Tools_DynamicUTF8String &tablename);


/*
  -----------------------------------------------------------------------------

  function: ls26_OpenNewOutfile

  -----------------------------------------------------------------------------

  description:  Opens new outfile (DATA file). By doing that it initializes the
                number of pages in a file to 0 and the number of tables in a file
                to 0. It writes the file header and it writes the file name of the
                DATA file to the mapping file in case of TE USER or TE ALL.

                Behavior:
                If no file is open (beginning of extraction, the first file is
                initialized in ls26_InitTabExtrRec) the first file will be opened.
                If a file is already open function checks the size of this function
                (gt medium size?) and
                  - file size is gt or et medium size
                    function writes the file trailer and closes the open file
                    function opens a new file (defines the name of the file by use
                    of the file counter that was increased by 1) as described above
                  - file size is lt medium size
                    function does nothing


  arguments:    pDBInfo          [IN] - structure with all information on the running
                                         session and the db connected to.
                pTabExtrRec           [IN] - tableextract record structure
                bTableIncomplete [IN] - indicates if DATA file contains incompletely
                                        extracted table DATA which is logged in the
                                        file trailer
                pszErrText       [IN] - error text

  returnvalue:  errOK_els00                    - no error
                errTEFileCounterExceeded_els98 - file counter exceeded limit of 9999
                err_FileWrite_els98            - error writing to files
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_OpenNewOutfile(tls00_DBInfo      *pDBInfo,
                    tls00_TextractRec *pTabExtrRec,
                    bool               bTabIncomplete,
                    tsp00_Addr         pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls26_WriteDataFileHeader
  -----------------------------------------------------------------------------

  description:  Writes the DATA file header.

  arguments:    pDBInfo       [IN]  - structure with all info on the running
                                       session and the database connected to.
                pTabExtrRec   [IN]  - structure with all info needed
                                       to execute table extract
                pszErrText    [OUT] - error text

  returnvalue:  errOK_els00         - No error
                errFileWrite_els98  - error writing header to DATA file
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteDataFileHeader(tls00_DBInfo      *pDBInfo,
                         tls00_TextractRec *pTabExtrRec,
                         tsp00_Addr         ErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls26_WriteDataFileTrailer

  -----------------------------------------------------------------------------

  description:  Writes the DATA file trailer.

  arguments:    pDBInfo          [IN]  - structure with all info on the running
                                          session and the database connected to.
                pTabExtrRec      [IN]  - structure with all info needed
                                          to execute table extract
                bTableIncomplete [IN]  - shows if last table is only incompletely
                                          saved in DATA file
                pszFileName      [IN]  - name of next used DATA file (only for
                                          TE USER or TE ALL)
                ErrText          [OUT] - error text

  returnvalue:  errOK_els00          - No error
                errFileWrite_els98   - error writing trailer to DATA file
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_WriteDataFileTrailer(tls00_DBInfo      *pDBInfo,
                          tls00_TextractRec *pTabExtrRec,
                          bool               bTableIncomplete,
                          tsp00_Addr         pszFileName,
                          tsp00_Addr         ErrText);

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
  function:     ls26_InitTabExtrRec  - local function
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_InitTabExtrRec(tls00_DBInfo             *pDBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_TextractRec        *&pTabExtrRec,
                    tsp00_Addr               ErrText )
{
    ROUTINE_DBG_MEO00 ("ls26_InitTabExtrRec");

    tsp00_Int4 rc = errOK_els00;

    pTabExtrRec = new tls00_TextractRec;
    if (NULL == pTabExtrRec)
    {
        rc = errNoMoreMemory_els98;
    }
    else
    {
        memset(pTabExtrRec, 0, sizeof(tls00_TextractRec));

        //create and initialize mapping file and outfile
        pTabExtrRec->terPartSpec_ls00 = pTransformationCmd->trPartSpec_ls00;
        if (pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00)
        {
            pTabExtrRec->terRestore_ls00  = pTransformationCmd->trRestore_ls00;
        }
        else
        {
            ls05ConvertToUTF8(pTabExtrRec->terExtrStmt_ls00, REINTERPRET_CAST(unsigned char*, pTransformationCmd->trFullUserInput_ls00),
                                                             _tcslen(pTransformationCmd->trFullUserInput_ls00),
                                                             ctAscii_els00,
                                                             ErrText);
       }

        // session

        //  Number of possible pages per packet to receive
        //  tls00_PktBlock contains packet-, segment- and part header size. Because each packet
        //  consists of 2 parts the part header has to be substracted again.
        pTabExtrRec->terPgPerPckt_ls00 = STATIC_CAST( tsp00_Int2, (pDBInfo->dbiPktSize_ls00 - sizeof(tls00_PktBlock)
                                                                - sizeof(tsp1_part_header)
                                                                - sizeof(pTabExtrRec->terBdInfo_ls00.szbdiData_ls00)) / pDBInfo->dbiPageSize_ls00 );

        pTabExtrRec->terPageSize_ls00                      = pDBInfo->dbiPageSize_ls00;
        pTabExtrRec->terpPageBuffer_ls00                   = new _TCHAR[pDBInfo->dbiPageSize_ls00];
        pTabExtrRec->terMediumSize_ls00.lnu2GB_Blocks_ls00 = 0;
        pTabExtrRec->terMediumSize_ls00.lnuRemainder_ls00  = MAX_MEDIUM_SIZE_IN_BYTES_LS26;    // 1 GB in Byte so far !
        pTabExtrRec->terPageTypeExtracting_ls00            = tptUnknown_els00;

        // table DATA
        /*pTabExtrRec->terConstraintCnt_ls00 = 0;
        pTabExtrRec->terDefaultCnt_ls00    = 0;
        pTabExtrRec->terLongColCnt_ls00    = 0;*/
        rc = ls26_InitStreams(pDBInfo, pTransformationCmd, pTabExtrRec, ErrText);
    }

    return rc;
}
//  ls26_InitTabExtrRec()

/*
  -----------------------------------------------------------------------------
  function:     ls26_InitStreams
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_InitStreams(tls00_DBInfo      *pDBInfo,
                 tls00_TransformationCmd  *pTransformationCmd,
                 tls00_TextractRec *pTabExtrRec,
                 tsp00_Addr         ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_InitStreams");

    tsp00_Int4 rc = errOK_els00;

    assert(dtypeUnknown_els00 != pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00);


    if ( NULL == pTabExtrRec->terOutFile_ls00 )
    {
        pTabExtrRec->terOutFile_ls00 = new tls00_VFile;
        memset(pTabExtrRec->terOutFile_ls00, 0, sizeof(tls00_VFile));
    }
    //*
    //*   initialize datastream - type: file, tape or pipe
    //*
    if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
    {
        pTabExtrRec->terOutFile_ls00->vfFileName_ls00 = new _TCHAR[PATH_MXSP00];
        pTabExtrRec->terOutFile_ls00->vfFileName_ls00[0] = 0;

        if (pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00)
        {
            ++pTabExtrRec->terFileCounter_ls00;
            sp77sprintf(pTabExtrRec->terOutFile_ls00->vfFileName_ls00, PATH_MXSP00, "%s%0*d", pTransformationCmd->trDataFile_ls00.fsFileName_ls00,
                                                                                              FILE_EXTENSION_SIZE_LS26,
                                                                                              pTabExtrRec->terFileCounter_ls00);
        }
        else
            sp77sprintf(pTabExtrRec->terOutFile_ls00->vfFileName_ls00, PATH_MXSP00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00 );

        pTabExtrRec->terOutFile_ls00->vfFileType_ls00   = dtypeFile_ls00;

        ls30VFInitialize(*pTabExtrRec->terOutFile_ls00, pTabExtrRec->terOutFile_ls00->vfFileName_ls00,sp5vf_binary,sp5vf_write);
        //*
        //*  open stream and write pagefile header
        //*
        rc = ls26_OpenNewOutfile(pDBInfo, pTabExtrRec, false, ErrText);
        //*
        //* streamname
        //*
        if ( NULL != pTabExtrRec->terOutFile_ls00->vfFileName_ls00 )
        {
            _TCHAR* pszDest = strrchr(pTabExtrRec->terOutFile_ls00->vfFileName_ls00, SLASH_LS00);
            if (NULL == pszDest)
            {
                pszDest = strrchr(pTabExtrRec->terOutFile_ls00->vfFileName_ls00, BACKSLASH_LS00);
                if (NULL == pszDest)
                {
                    pszDest = pTabExtrRec->terOutFile_ls00->vfFileName_ls00;
                }
            }
            pszDest = (pszDest == pTabExtrRec->terOutFile_ls00->vfFileName_ls00) ? pszDest : pszDest + 1;

            pTabExtrRec->terOutFile_ls00->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                              pszDest));
        }
    }
    else if (dtypeTape_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
    {
        pTabExtrRec->terOutFile_ls00->vfFileType_ls00   = dtypeTape_ls00;
        pTabExtrRec->terOutFile_ls00->vfBackupTool_ls00 = pDBInfo->dbiSession_ls00.siBackupTool_ls00;
        pTabExtrRec->terOutFile_ls00->vfNodeName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                             pDBInfo->dbiSession_ls00.si_pDBNode_ls00));
        pTabExtrRec->terOutFile_ls00->vfDBName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                           pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp()));

        ls30VFInitialize(*pTabExtrRec->terOutFile_ls00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00,sp5vf_binary, sp5vf_write);
        //*
        //* open stream
        //*
        rc = ls30VFOpen(*pTabExtrRec->terOutFile_ls00, ErrText);
        //*
        //*  write pagefile header
        //*
        if ( errOK_els00 == rc )
             rc = ls26_WriteDataFileHeader(pDBInfo, pTabExtrRec, ErrText);
        //*
        //* streamname
        //*
        if ( NULL != pTabExtrRec->terOutFile_ls00->vfFileName_ls00 )
        {
            pTabExtrRec->terOutFile_ls00->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                              pTabExtrRec->terOutFile_ls00->vfFileName_ls00));
        }
    }
    else if (dtypePipe_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
    {
        pTabExtrRec->terOutFile_ls00->vfFileType_ls00 = dtypePipe_ls00;

        ls30VFInitialize(*pTabExtrRec->terOutFile_ls00, pTransformationCmd->trDataFile_ls00.fsFileName_ls00, sp5vf_binary, sp5vf_write);
        //*
        //* open stream
        //*
        rc = ls30VFOpen(*pTabExtrRec->terOutFile_ls00, ErrText);
        //*
        //*  write pagefile header
        //*
        if ( errOK_els00 == rc )
            rc = ls26_WriteDataFileHeader(pDBInfo, pTabExtrRec, ErrText);
        //*
        //* streamname
        //*
        if ( NULL != pTabExtrRec->terOutFile_ls00->vfFileName_ls00 )
        {
            pTabExtrRec->terOutFile_ls00->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                              pTabExtrRec->terOutFile_ls00->vfFileName_ls00));
        }
    }

    return rc;
}
// ls26_InitStreams()

/*
  -----------------------------------------------------------------------------
  function:     ls26_SwitchStreams
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_SwitchStreams(MetaDataDef       *pMetaDataDef,
                   tls00_TextractRec *pTabExtrRec,
                   tsp00_Addr        ErrText)
{

    ROUTINE_DBG_MEO00 ("ls26_SwitchStreams");

    TransformationDef       * pTransformationDef  = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd * pTransformationCmd  = pTransformationDef->getTransformationCmd();
    tls00_DBInfo            * pDBInfo             = pMetaDataDef->getConnection();

    tsp00_Int4 rc = errOK_els00;

    if (dtypeFile_ls00 == pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00)
    {
        StudioOAL_String schemaname;
        StudioOAL_String tablename;

        schemaname.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.asCharp()));
        tablename.Assign (REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp()));

        tsp00_Int4 lTabSize = ls26_GetTableSize(pDBInfo, schemaname, tablename );

        if ( false == ls26_FileLenLTMediumSize(pTabExtrRec->terOutFile_ls00->vfFileLen_ls00,
                                               pTabExtrRec->terMediumSize_ls00,
                                               lTabSize * 1024) )
        {
            rc = ls26_OpenNewOutfile(pDBInfo, pTabExtrRec, false, ErrText);
        }
    }
    return rc;
}
//ls26_SwitchStreams


/*
  -----------------------------------------------------------------------------
  function:     ls26_CloseStreams
  -----------------------------------------------------------------------------
*/
static void
ls26_CloseStreams(tls00_TextractRec *pTabExtrRec, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_CloseStreams");
    //*
    //* close datafile
    //*
    if (NULL != pTabExtrRec->terOutFile_ls00)
        if (0 != pTabExtrRec->terOutFile_ls00->vfFileHndl_ls00)
            ls30VFClose(*pTabExtrRec->terOutFile_ls00, ErrText);
}
// ls26_CloseStreams

/*
  -----------------------------------------------------------------------------
  function:     ls26_FreeStreams
  -----------------------------------------------------------------------------
*/
static void
ls26_FreeStreams(tls00_TransformationCmd  *pTransformationCmd,
                 tls00_TextractRec *pTabExtrRec,
                 tsp00_Addr         ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_FreeStreams");

    if (NULL != pTabExtrRec->terOutFile_ls00)
    {
        //*
        //* delete outfile structure
        //*
        if (pTabExtrRec->terOutFile_ls00->vfFileName_ls00 != pTransformationCmd->trDataFile_ls00.fsFileName_ls00)
        {
            //delete [] pTabExtrRec->terOutFile_ls00->vfFileName_ls00;
            pTabExtrRec->terOutFile_ls00->vfFileName_ls00 = NULL;
        }
        delete pTabExtrRec->terOutFile_ls00;
        pTabExtrRec->terOutFile_ls00 = NULL;
    }
}
// ls26_FreeStreams()

/*
  -----------------------------------------------------------------------------
  function: ls26_FreeTabExtrRec
  -----------------------------------------------------------------------------
*/
static void
ls26_FreeTabExtrRec(tls00_TextractRec*        pTabExtrRec,
                    tls00_TransformationCmd*  pTransformationCmd,
                    tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_FreeTabExtrRec");
    ls26_CloseStreams(pTabExtrRec, ErrText);
    ls26_FreeStreams(pTransformationCmd, pTabExtrRec, ErrText);
    if (NULL != pTabExtrRec->terpPageBuffer_ls00)
    {
        delete [] pTabExtrRec->terpPageBuffer_ls00;
        pTabExtrRec->terpPageBuffer_ls00 = NULL;
    }
    delete pTabExtrRec;     // at last delete the structure itself
    return;
}
// ls26_FreeTabExtrRec()


/*
  -----------------------------------------------------------------------------
  function: ls26_FileLTMediumSize
  -----------------------------------------------------------------------------
*/
bool
ls26_FileLenLTMediumSize(tls00_LargeNumber lFileLen,
                         tls00_LargeNumber lMediumSize,
                         tsp00_Longint     lAddToFileLen)      // in Byte ?! assuming that not greater than 2 GB
{
    bool brc = true;

    if (lFileLen.lnuRemainder_ls00 > (MAX_INT4_SP00 - lAddToFileLen) )    // do we exceed the limit of 2 GB when adding the value to len?
    {
        ++lFileLen.lnu2GB_Blocks_ls00;
        lFileLen.lnuRemainder_ls00 = -(MAX_INT4_SP00 - lFileLen.lnuRemainder_ls00 - lAddToFileLen);
    }

    if (lFileLen.lnu2GB_Blocks_ls00 == lMediumSize.lnu2GB_Blocks_ls00)
    {
        if ( (lFileLen.lnuRemainder_ls00 + lAddToFileLen) >= lMediumSize.lnuRemainder_ls00)
        {
            brc = false;
        }
    }
    else if (lFileLen.lnu2GB_Blocks_ls00 > lMediumSize.lnu2GB_Blocks_ls00)
    {
        brc = false;
    }

    return brc;
}
// ls26_FileLTMediumSize()

/*
  -----------------------------------------------------------------------------
  function: ls26_GetTableSize
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_GetTableSize(tls00_DBInfo            *pDBInfo,
                  Tools_DynamicUTF8String &schemaname,
                  Tools_DynamicUTF8String &tablename   )
{
    ROUTINE_DBG_MEO00 ("ls26_GetTableSize");

    tsp00_Int4 rc      = errOK_els00;
    tsp00_Int4 lResult = 0;

    _TCHAR statement[1024];
    statement       [0]=0;
    tsp00_Int4 lErrPos;
    _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 0);

    Tools_DynamicUTF8String SELECT_TABLE_SIZE_stmt;
    if (IsEqual_esp100 == vres || IsNewer_esp100 == vres ) {
        SELECT_TABLE_SIZE_stmt  = _T("SELECT NUMERIC_VALUE + SHORT_COLUMN_PAGES FROM SYSDBA.PAGES WHERE SCHEMANAME = '%s' AND TABLENAME = '%s'");
    }
    else {
        SELECT_TABLE_SIZE_stmt  = _T("SELECT NUMERIC_VALUE + SHORT_COLUMN_PAGES FROM SYSDBA.PAGES WHERE OWNER      = '%s' AND TABLENAME = '%s'");
    }
    sp77sprintf(statement, 1024, SELECT_TABLE_SIZE_stmt.CharPtr(), schemaname.StrPtr(), tablename.StrPtr());

    rc = ls13ExecIntSQLStmt(pDBInfo, 0, statement, szErr, lErrPos);
    if (errOK_els00 != rc)
        TRACE_PRNF_MLS99(("ls26_GetTableSize", "Error getting table size for %s (%s)\n", tablename.StrPtr(), szErr));
    else
    {
        tsp00_Int4 tmpRowCnt = 1;
        rc = ls12Fetch(*pDBInfo, NULL, tmpRowCnt, false, szErr);
        if (errOK_els00 == rc)
        {
            tsp1_packet *pSqlPacket = pDBInfo->dbiPktRcvList_ls00[0];
            tsp1_part   *pSqlPart = NULL;

            bool Res = ls04GetPart(pSqlPacket, sp1pk_data, pSqlPart);
            if (false == Res)
                TRACE_PRNF_MLS99(("ls26_GetTableSize", "Error getting table size for %s (error no: %d)\n", tablename.StrPtr(), rc));
            else
            {
                // we expect the result with a length of 8 byte (fixed number (10,0))
                _TCHAR szResult[8];
                tsp00_Int4 PartLength = ls04GetPartLength(pSqlPart);
                ls04BufFromPartEx(pSqlPart, 0, PartLength, szResult, PartLength);
                rc = ls07NumberToInt(szResult + 1, lResult);                        // szResult: jump over def byte
                if (errOK_els00 == rc)
                    lResult *= (pDBInfo->dbiPageSize_ls00/1024);
            }
        }
    }
    return lResult;
}
// ls26_GetTableSize()
/*
  -----------------------------------------------------------------------------
  function: ls26_OpenNewOutfile
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls26_OpenNewOutfile(tls00_DBInfo      *pDBInfo,
                    tls00_TextractRec *pTabExtrRec,
                    bool               bTabIncomplete,
                    tsp00_Addr         ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_OpenNewOutfile");

    tsp00_Int4 rc = errOK_els00;

    //*
    //*     First check if the file is already open and close it.
    //*     Only then we open the new file.
    //*
    if (0 != pTabExtrRec->terOutFile_ls00->vfFileHndl_ls00)     // is file already open?
    {
        // WRITE DATA FILE TRAILER FOR OLD FILE
        _TCHAR szFileName[PATH_MXSP00];
        szFileName[0] = 0;
        sprintf(szFileName, "%s%0*d",
                            &pTabExtrRec->terOutFile_ls00->vfFileName_ls00[_tcslen(pTabExtrRec->terOutFile_ls00->vfFileName_ls00) - FILE_EXTENSION_SIZE_LS26],
                            FILE_EXTENSION_SIZE_LS26,
                            pTabExtrRec->terFileCounter_ls00);

        ls26_WriteDataFileTrailer(pDBInfo, pTabExtrRec, bTabIncomplete, szFileName, ErrText);

        ls30VFClose(*pTabExtrRec->terOutFile_ls00, ErrText);    // close it; return value is of no interest here

        if (pTabExtrRec->terFileCounter_ls00 < FILECOUNT_MXLS26)
        {
            // generate new file
            ++pTabExtrRec->terFileCounter_ls00;
            sprintf(&pTabExtrRec->terOutFile_ls00->vfFileName_ls00[_tcslen(pTabExtrRec->terOutFile_ls00->vfFileName_ls00) - FILE_EXTENSION_SIZE_LS26],
                    "%0*d", FILE_EXTENSION_SIZE_LS26, pTabExtrRec->terFileCounter_ls00);
            ls30VFInitialize(*pTabExtrRec->terOutFile_ls00, pTabExtrRec->terOutFile_ls00->vfFileName_ls00, sp5vf_binary, sp5vf_write);
        }
        else
        {
            LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, ErrText, errTEFileCounterExceeded_els98, FILECOUNT_MXLS26);
            rc = errTEFileCounterExceeded_els98;
        }
    }

    if ( (errOK_els00 == rc) && (0 == pTabExtrRec->terOutFile_ls00->vfFileHndl_ls00) )      // check again if file is already open
    {
        rc = ls30VFOpen(*pTabExtrRec->terOutFile_ls00, ErrText);    // open it
        if (errOK_els00 == rc)
        {
            // Write file name to mapping file only if not extracting a single table
            if (ptTable_els00 != pTabExtrRec->terPartSpec_ls00.psPartType_ls00)
            {
                _TCHAR* pszDest = strrchr(pTabExtrRec->terOutFile_ls00->vfFileName_ls00, SLASH_LS00);
                if (NULL == pszDest)
                {
                    pszDest = strrchr(pTabExtrRec->terOutFile_ls00->vfFileName_ls00, BACKSLASH_LS00);
                    if (NULL == pszDest)
                        pszDest = pTabExtrRec->terOutFile_ls00->vfFileName_ls00;
                }
                // if pszDest does NOT point to the file name itself but to a part of it
                // we need to add 1 to the pointer
                pszDest = (pszDest == pTabExtrRec->terOutFile_ls00->vfFileName_ls00) ? pszDest : pszDest + 1;

                pTabExtrRec->terOutFile_ls00->vfName_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                                                                  pszDest));
            }

            //*
            //*     Write DATA file header
            //*
            rc = ls26_WriteDataFileHeader(pDBInfo, pTabExtrRec, ErrText);

            // Reinitialize page count and number of tables in file
            pTabExtrRec->terPageCount_ls00    = 0;
            pTabExtrRec->terNumTabInFile_ls00 = 0;
        }   // end if (errOK_els00 == rc) - opening DATA file

    }   // end if ( (errOK_els00 == rc) && (0 == pTabExtrRec->terOutFile_ls00->vfFileHndl_ls00) )

    return rc;
}
// ls26_OpenNewOutfile()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteDataFileHeader
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26_WriteDataFileHeader(tls00_DBInfo      *DBInfo,
                         tls00_TextractRec *pTabExtrRec,
                         tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WriteDataFileHeader");

    tsp00_Int4 rc = errOK_els00;

    // Header looks as follows
    //---------------------------------------------------------------------------------------------------------------
    // loader    | code |            | loader  |      |      | page size    | count of        | more than | kernel  |
    // swap at   | type | identifier | version | date | time | of extracted | pages in        | one table | version |
    // extract   |      |            |         |      |      | db           | file (blockung) | in file ? |         |
    //---------------------------------------------------------------------------------------------------------------
    // 1 byte    |  1   |      64    | 40      | 8    | 8    |    4         |  4              |   1       | 40      |


    tsp00_Int4 lBufPos = 0;

    _TCHAR pszBuffer[TE_FILE_HEADER_LENGTH_LS00];
    memset(pszBuffer, 0, TE_FILE_HEADER_LENGTH_LS00);

    pszBuffer[lBufPos] = STATIC_CAST(unsigned char, ls07DefineSwap(NULL));
    lBufPos += sizeof(unsigned char);

    pszBuffer[lBufPos] = STATIC_CAST(unsigned char, DBInfo->dbiEncoding_ls00);
    lBufPos += sizeof(unsigned char);

    tsp00_C64c pszId;
    pszId.Init();
    pszId.rawAssign(HEADER_IDENTIFIER_LS00);
    SAPDB_memcpy(&pszBuffer[lBufPos], pszId.asCharp(), pszId.length());
    lBufPos += pszId.size();

    // loader version
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
    SAPDB_memcpy(&pszBuffer[lBufPos], pszDate.asCharp(), pszDate.length());
    lBufPos += pszDate.size();
    SAPDB_memcpy(&pszBuffer[lBufPos], pszTime.asCharp(), pszTime.length());
    lBufPos += pszTime.size();

    SAPDB_memcpy(&pszBuffer[lBufPos], REINTERPRET_CAST(char*, &DBInfo->dbiPageSize_ls00), sizeof(DBInfo->dbiPageSize_ls00));
    lBufPos += sizeof(DBInfo->dbiPageSize_ls00);

    SAPDB_memcpy(&pszBuffer[lBufPos], REINTERPRET_CAST(char*, &pTabExtrRec->terCount), sizeof(pTabExtrRec->terCount));
    lBufPos += sizeof(pTabExtrRec->terCount);

    // Define if DATA of a single or DATA of multiple tables in DATA file
    pszBuffer[lBufPos] = (ptTable_els00 != pTabExtrRec->terPartSpec_ls00.psPartType_ls00);
    lBufPos += sizeof(char);

    // kernel version
    char v[VERSION_MXSP00];
    memset(v, 0, VERSION_MXSP00);
    sp77sprintf(v, VERSION_MXSP00, "%s %.1d.%.1d.%.1d", COMP_NAME_KERNEL_SP100,
                                                        STATIC_CAST(tsp00_Int4, DBInfo->dbiDBVersion_ls00.MajorVersion_sp100),
                                                        STATIC_CAST(tsp00_Int4, DBInfo->dbiDBVersion_ls00.MinorVersion_sp100),
                                                        STATIC_CAST(tsp00_Int4, DBInfo->dbiDBVersion_ls00.CorrLevel_sp100));
    SAPDB_memcpy(&pszBuffer[lBufPos], v, VERSION_MXSP00);
    lBufPos += VERSION_MXSP00;


    rc = ls30VFWriteLn(*pTabExtrRec->terOutFile_ls00, pszBuffer, TE_FILE_HEADER_LENGTH_LS00, pszErrText);

    return rc;
}
// ls26_WriteDataFileHeader()


/*
  -----------------------------------------------------------------------------
  function:     ls26_WriteDataFileTrailer
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26_WriteDataFileTrailer(tls00_DBInfo      *DBInfo,
                          tls00_TextractRec *pTabExtrRec,
                          bool               bTableIncomplete,
                          tsp00_Addr         pszFileName,         // NULL indicates no next or last file --> no entry for file
                          tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls26_WriteDataFileTrailer");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR *pszBuffer = pTabExtrRec->terpPageBuffer_ls00;        // has length of one db page

    ++pTabExtrRec->terPageCount_ls00;

    tls26_TEFileTrailer Trailer;
    memset(&Trailer, 0, sizeof(tls26_TEFileTrailer));

    Trailer.tftPageNo_ls26           = pTabExtrRec->terPageCount_ls00;
    Trailer.tftType_ls26             = MY_PAGE_LS00;
    Trailer.tftPageType_ls26         = tptEOF_els00;
    Trailer.tftNumOfTabInFile_ls26   = pTabExtrRec->terNumTabInFile_ls00;
    Trailer.tftNumOfPagesInFile_ls26 = pTabExtrRec->terPageCount_ls00 - 1;
    Trailer.tftTableIncomplete_ls26  = bTableIncomplete;
    if (true == bTableIncomplete)
        // type of last extracted DATA
        Trailer.tftDataTypeAtInterrupt_ls26 = pTabExtrRec->terPageTypeExtracting_ls00;

    size_t lNextFileLen = 0;
    if (NULL != pszFileName)        // indicating no next or last file
        lNextFileLen = _tcslen(pszFileName);

    // Write trailer to beginning of page
    SAPDB_memcpy(pszBuffer, REINTERPRET_CAST(_TCHAR*, &Trailer), sizeof(tls26_TEFileTrailer));

    // Write file name to beginning of page
    SAPDB_memcpy(&pszBuffer[sizeof(tls26_TEFileTrailer)], pszFileName, lNextFileLen);
    pszBuffer[sizeof(tls26_TEFileTrailer) + lNextFileLen] = 0;


    // Write same trailer again to the end of the page (include 0 of filename)
    SAPDB_memcpy(&pszBuffer[pTabExtrRec->terPageSize_ls00 - STATIC_CAST(tsp00_Int4, sizeof(tls26_TEFileTrailer)) - lNextFileLen - 1],
                 REINTERPRET_CAST(_TCHAR*, &Trailer),
                 sizeof(tls26_TEFileTrailer));

    // Write same file name to end of page
    SAPDB_memcpy(&pszBuffer[pTabExtrRec->terPageSize_ls00 - lNextFileLen - 1], pszFileName, lNextFileLen);
    pszBuffer[pTabExtrRec->terPageSize_ls00 - 1] = 0;

    // Write trailer page to file
    rc = ls30VFWriteLn(*pTabExtrRec->terOutFile_ls00, pszBuffer, pTabExtrRec->terPageSize_ls00, pszErrText);

    return rc;
}
// ls26_WriteDataFileTrailer()

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
  function:     ExportPackageData
  -----------------------------------------------------------------------------
*/
tsp00_Int4 
ExportPackageData(MetaDataDef *pMetaDataDef, tls00_DBInfo *pDBInfo, tsp00_Addr  ErrText) 
{
    ROUTINE_DBG_MEO00 ("ExportPackageData");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    tsp00_Int4  rc  = errOK_els00;

    TransformationDef       *pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();
    RelationalDef           *pRelationalDef     = pMetaDataDef->getRelationalModel();
    tls00_TextractRec*       pTabExtrRec        =  NULL;
    Loader_RTStatistics*     pStatObj           = Loader_RTStatistics::Instance();

    // Initialize statistics object
    pStatObj->Init();

    //*
    //*  execute transformationpackage to transform catalog
    //*
    if ( NULL != pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 )
    {
        if (true == pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoUsage_ls00.uFastMode_ls00) 
            pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdlNoIndex_ls00 = true;

        rc = TransportPackageCatalog( pMetaDataDef, pDBInfo, ErrText);
    }
    //*
    //*  db|user|table records, compressed, formattted, pages
    //*
    if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 && rc == errOK_els00 )
    {
        //*
        //*  db|user|table records, compressed, formattted
        //*
        if ( false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00 )
             rc = ExportPackageDataRecord (pMetaDataDef, pDBInfo, ErrText );
        //*
        //*  db|user|table pages
        //*
        if ( true  == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00 )
        {
            pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = true;
            //*
            //*  import transformationpackage 
            //*
            rc = pTransformationDef->ImportPackage( pMetaDataDef, ErrText );
            if (errOK_els00 == rc)
            {
                rc = ls26_InitTabExtrRec(pDBInfo, pTransformationCmd, pTabExtrRec, ErrText);
                if (errOK_els00 == rc)
                {
                    //*
                    //*  execute transformationpackage to transform data
                    //*
                    if (ptTable_els00 < pTransformationCmd->trPartSpec_ls00.psPartType_ls00  )
                    {
                        pMsgInstance->ls98PrintMessage(layTableExtract_els98, msgExecDataPackage_els98);
                        pMsgInstance->ls98PrintMessage(layTableExtract_els98, msgNumTabToTransform_els98, pTransformationDef->m_nTableCount- 
                                                                                                          pTransformationDef->m_nTableData);
                    }
                    //*
                    //*  update transformationpackage
                    //*
                    if (ptAll_els00    == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                        ptUser_els00   == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
                        ptSchema_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
                    {
                        rc = ls26ExportPackageTables( pMetaDataDef, pTabExtrRec, ErrText);

                    }
                    else if (ptTable_els00  == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
                    {
                        rc = ls26ExportTablePages( pMetaDataDef, pTabExtrRec, ErrText);

                    }
                }
                if (NULL != pTabExtrRec)
                {
                    ls26_FreeTabExtrRec( pTabExtrRec, pTransformationCmd, ErrText);
                    pTabExtrRec = NULL;
                }
            }   // end rc = pTransformationDef->ImportPackage( pMetaDataDef, ErrText );
            pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00 = false;
            //*
            //*  execute transformationpackage to transform catalog
            //*
            if (pRelationalDef->SplitPackage())
            {
                StudioOAL_String catalogfilename(pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00);
                catalogfilename += "0001";
                delete [] pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00;
                pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 = new char [catalogfilename.BasisSize()+1];
                sp77sprintf(pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00, PATH_MXSP00, "%s", CONST_CAST(SAPDB_Char*, catalogfilename.CharPtr()) );
                rc = TransportPackageCatalog( pMetaDataDef, pDBInfo, ErrText);
            }
            //*
            //*  export transformationpackage
            //*
            if (errOK_els00 == rc )
                rc = pTransformationDef->ExportPackage( pMetaDataDef, ErrText );
        }
    }// if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )

    //*
    //*  export database statistics
    //*
    if ( (errOK_els00 == rc) && (NULL != pTransformationCmd->trStatisticFile_ls00) )
    {
        rc = pTransformationDef->ExportStatistic();
        if (errOK_els00 != rc)
        {
            // Check pMetaDataDef's error object for errors
            if (SAPDB_TRUE == pMetaDataDef->m_pError->isError())
            {
                pMsgInstance->ls98Msg(layTableExtract_els98, ErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                      pMetaDataDef->m_pError->getErrorText().StrPtr());
            }
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
                                                                                     "exported",
                                                                                     (pTransformationDef->m_nTableCount +
                                                                                      pTransformationDef->m_nTableData));
        }
    }
    //*
    //* Add some runtime statistics information to the return string - DATA
    //*
    if (ptTable_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
    {
        if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
        {
            pMsgInstance->ls98AppendStatusToString(layTableExtract_els98, ErrText, statNumExportedPages_els98,
                                                                                   pStatObj->GetPagesCount(Loader_RTStatistics::ALLDATA),
                                                                                   pStatObj->GetPagesCount(Loader_RTStatistics::PRIMARYDATA),
                                                                                   pStatObj->GetPagesCount(Loader_RTStatistics::LONGDATA));
        }
    }
    else
    {
        pMsgInstance->ls98AppendStatusToString(layTableExtract_els98, ErrText, statNumTablesTransformed_Data_els98,
                                                                               "exported",
                                                                               pStatObj->GetTablesTranformed(),
                                                                               pStatObj->GetTablesExcluded(),
                                                                               pStatObj->GetTablesFailed());
    }
    return rc;
}
// ExportPackageData

/*
  -----------------------------------------------------------------------------
  function: ls26ExportPackageTables
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26ExportPackageTables( MetaDataDef *&pMetaDataDef, tls00_TextractRec *pTabExtrRec, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26ExportPackageTables");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    tsp00_Int4       rc          = errOK_els00;

    TransformationDef*        pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd*  pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_DBInfo*             pDBInfo            = pMetaDataDef->getConnection();
    Loader_RTStatistics*      pStatObj = Loader_RTStatistics::Instance();

    // Define count of excluded tables
    pStatObj->SetTablesExcluded(pTransformationDef->m_nTableData);

    StudioOAL_String  select;
    _TCHAR            szselect[2048];
    szselect[0] = 0;

    sp77sprintf(szselect, 2048, pTransformationDef->Statement(sel_Transformation_708).CharPtr(), pTransformationCmd->trPackageid_ls00.StrPtr());
    select.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, szselect));

    if ( !pTransformationDef->executeSQL( select ) )
    {
        StudioOAL_String    NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
        SQLRETURN           ReturnCode          = pTransformationDef->getReturnCode();

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
    while( pTransformationDef->initTransformationStep() && errOK_els00 == rc )
    {
        rc = pTransformationDef->switchUserSchema( currentUser, currentSchema, false, ErrText);
        
        currentUser   = pTransformationDef->m_owner ;
        currentSchema = pTransformationDef->m_schemaname ;

        if (errOK_els00 == rc)
        {
            rc = pTransformationDef->initTransformationCmd(srFctTabExtract_els00);
            pTabExtrRec->terTableSwitched_ls00 = true;
            
            if (errOK_els00 == rc)
            {
                rc = ls26_SwitchStreams(pMetaDataDef, pTabExtrRec,ErrText);
                if (errOK_els00 == rc)
                {
                    rc = ls26ExportTablePages(pMetaDataDef, pTabExtrRec, ErrText);
                    if (errOK_els00 == rc)
                    {
                        if (true == pTransformationDef->getTransformationCmd()->trExclude_ls00)
                        {
                            pStatObj->IncreaseCounter_TablesExcluded();
                        }
                        else
                        {
                            pStatObj->IncreaseCounter_TablesTranformed();
                        }
                        ++pTabExtrRec->terNumTabInFile_ls00;
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
                }
            }//pTransformationDef->initTransformationCmd
        }
        pTabExtrRec->terConstraintCnt_ls00 = 0;
        pTabExtrRec->terDefaultCnt_ls00    = 0;
        pTabExtrRec->terLongColCnt_ls00    = 0;
    }// while pTransformationDef->initTransformationStep

    if (errOK_els00 == rc)
    {
        pMsgInstance->ls98PrintMessage(layTableExtract_els98, msgNumTabTransformed_els98, pStatObj->GetTablesTranformed());
    }
    //*
    //* switch user back to session user
    //*
    pTransformationDef->switchUser( currentUser, sessionUser, ErrText);
    //*
    //*  write pagefile trailer
    //*
    if ( errOK_els00 == rc &&  0 != pTabExtrRec->terOutFile_ls00->vfFileHndl_ls00 )
        ls26_WriteDataFileTrailer(pDBInfo, pTabExtrRec, false, NULL, ErrText);

    if ( NULL           != pTransformationDef->getTransformationCmd()  &&
         dtypeTape_ls00 == pTransformationDef->getTransformationCmd()->trDataFile_ls00.fsDeviceType_ls00 )
    {
         ls26_CloseStreams(pTabExtrRec, ErrText);
         pTransformationDef->getTransformationCmd()->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00 =
             CONST_CAST(SAPDB_Char*, pTabExtrRec->terOutFile_ls00->vfBackupId_ls00.CharPtr());
    }
    pTransformationDef->freeTransformationCmd(srFctTabExtract_els00);

    return rc;
}
// ls26ExportPackageTables()

/*
  -----------------------------------------------------------------------------
  function:     ls26UpdateTransformationPackage
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls26UpdateTransformationPackage( tls00_DBInfo             *pDBInfo,
                                 MetaDataDef              *&pMetaDataDef,
                                 tls00_TransformationCmd  *pTransformationCmd,
                                 tls00_TextractRec        *pTabExtrRec,
                                 tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls26UpdateTransformationPackage");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    tsp00_Int4 rc     = errOK_els00;
    SAPDB_Bool rcBool = SAPDB_TRUE;

    if (pTabExtrRec->terPartSpec_ls00.psPartType_ls00 > ptTable_els00)
    {
        TransformationDef* pTransformationDef = pMetaDataDef->getTransformationModel();
        //*
        //*  storagedevice, servernode, serverdb, username, codetype, codepage, transformation, part, streamformat
        //*
        _TCHAR HEADER_BLOCK[256];
        _TCHAR HEADER_POS[256];
        _TCHAR DATA_BLOCK[256];
        _TCHAR DATA_POS[256];
        HEADER_BLOCK[0] = 0;
        HEADER_POS[0] = 0;
        DATA_BLOCK[0] = 0;
        DATA_POS[0] = 0;
        if (true == pTabExtrRec->terTableSwitched_ls00)
        {
            // TIMESTAMP BEGIN
            pTabExtrRec->terTableSwitched_ls00 = false;
            rcBool = pTransformationDef->updateBegin();
            // <DATASTREAMNAME>
            if (SAPDB_TRUE == rcBool)
            {
                pTransformationDef->m_datastreamname = pTabExtrRec->terOutFile_ls00->vfName_ls00;
                rcBool = pTransformationDef->updateDataStream();
            }
            // <HEADER_BLOCK>:<HEADER_POS>
            if (SAPDB_TRUE == rcBool)
            {
                _itot(STATIC_CAST(tsp00_Int4, pTabExtrRec->terOutFile_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00),HEADER_BLOCK, 10);
                _itot(STATIC_CAST(tsp00_Int4, pTabExtrRec->terOutFile_ls00->vfFilePos_ls00.lnuRemainder_ls00), HEADER_POS,   10);
                pTransformationDef->m_headerpos_block = HEADER_BLOCK;
                pTransformationDef->m_headerpos_byte  = HEADER_POS;
                rcBool = pTransformationDef->updateHeader();
            }
        }
        if ( (SAPDB_TRUE == rcBool) &&
             ( (true == pTabExtrRec->terTabEmpty_ls00) || (true == pTabExtrRec->terTabExtracted_ls00) ) )
        {
            pTabExtrRec->terTabEmpty_ls00     = false;
            pTabExtrRec->terTabExtracted_ls00 = false;
            // <DATA_BLOCK>:<DATA_POS>
            _itot(STATIC_CAST(tsp00_Int4, pTabExtrRec->terOutFile_ls00->vfFileLen_ls00.lnu2GB_Blocks_ls00),DATA_BLOCK, 10);
            _itot(STATIC_CAST(tsp00_Int4, pTabExtrRec->terOutFile_ls00->vfFileLen_ls00.lnuRemainder_ls00), DATA_POS, 10);
            pTransformationDef->m_rows_exported = "0";
            pTransformationDef->m_datapos_block = DATA_BLOCK;
            pTransformationDef->m_datapos_byte  = DATA_POS;
            rcBool = pTransformationDef->updateExportData();
            // RESTART
            if (SAPDB_TRUE == rcBool)
                rcBool = pTransformationDef->updateRestart();
            // TIMESTAMP END
            if (SAPDB_TRUE == rcBool)
                rcBool = pTransformationDef->updateEnd();
        }
        if (SAPDB_FALSE == rcBool)
        {
            StudioOAL_String NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            rc = errODBCErrors_els98;
        }
    }// end if (pTabExtrRec->terPartSpec_ls00.psPartType_ls00 > ptTable_els00)
    return rc;
}// ls26UpdateTransformationPackage

/*
  -----------------------------------------------------------------------------
  EndChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/

/*===========================================================================*
 *     END
 *===========================================================================*/
