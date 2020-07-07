/*!
  -----------------------------------------------------------------------------
  module: vls30.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of functions for Virtual File access

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
#include "heo01.h"
#include "hni00b.h"     // define UNIX
#include "hls30.h"
#include "hls07.h"
#include "hls18.h"      // check for escaped quotes in file names
#include "hls99.h"
#include "hls98msg.h"


/* Local function definitions ********************************************/

/*!
  -----------------------------------------------------------------------------
  function:     ls30_ReadLnTxt
  -----------------------------------------------------------------------------
  description:  Read line of a file opened in text mode. If the buffer max size
                is too small to hold a whole data row an error is generated.
                Skips empty lines.

  arguments:    pszBuffer   [OUT]   - data buffer
                Vfile       [IN]    - virtual file structure
                lBytes      [IN]    - Count of bytes to read.
                ErrText     [OUT]   - error text
                MaxReadLen  [IN]    - maximum bytes to read;
                                      defaults to LINELENGTH_MXSP00

  returnvalue:  Read Length     - number of bytes read
                EOF_LS00 = -1   - reached end of file
                errFileRead     - error reading data from file

                In case of error the function logs an error message in the error
                text string AND in the log file.
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls30_ReadLnTxt(tsp00_Addr     pszBuffer,
               tls00_VFile   &VFile,
               tsp00_Longint  lBytes,
               tsp00_Addr     pszErrorText,
               tsp00_Longint  MaxReadLen = LINELENGTH_MXSP00);

/*!
  -----------------------------------------------------------------------------
  function:     ls30_ReadLnBin
  -----------------------------------------------------------------------------
  description:  Read requested count of bytes of a file opened in binary mode.

  arguments:    pszBuffer       [OUT]   - data buffer
                Vfile           [IN]    - virtual file structure
                lBytes          [IN]    - Count of bytes to read.
                ErrText         [OUT]   - error text

  returnvalue:  Read Length     - number of bytes read
                EOF_LS00 = -1   - reached end of file
                errFileRead     - error reading data from file

                In case of error the function logs an error message in the error
                text string AND in the log file.
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls30_ReadLnBin(tsp00_Addr    &pszBuffer,
               tls00_VFile   &VFile,
               tsp00_Longint  lBytes,
               tsp00_Addr     pszErrorText);


/*!
  -----------------------------------------------------------------------------
  function:     ls30_VFCheckFileName
  -----------------------------------------------------------------------------
  description:  Make file name ASCII (if UTF8) and remove escape quotes.

  arguments:    pName  [OUT]  - file name to handle

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls30_VFCheckFileName(tls00_String* pName);

/*
  -----------------------------------------------------------------------------
  Global functions: implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls30VFInitialize
  -----------------------------------------------------------------------------
*/
void ls30VFInitialize(tls00_VFile&                   NewFileStruct,
                      tsp00_Addr                     pszFileName,
                      tsp05_RteDataKind_Param        DataType,
                      tsp05_RteFileMode_Param        FileMode,
                      tls00_CodeType                 Encoding,
                      tsp05_RteBufferingKind_Param   BufferingKind,
                      tsp05_RteCloseOption_Param     CloseOption)
{
    ROUTINE_DBG_MEO00 ("ls30VFInitialize");

    LoaderMessages* pMsgInstance = LoaderMessages::Instance();

    NewFileStruct.vfFileHndl_ls00                   = 0;
    NewFileStruct.vfFileName_ls00                   = pszFileName;
    NewFileStruct.vfDataType_ls00.becomes (DataType);
    NewFileStruct.vfFileMode_ls00.becomes (FileMode);
    NewFileStruct.vfFileBuf_ls00.becomes (BufferingKind);
    NewFileStruct.vfFileCOpt_ls00.becomes (CloseOption);
    NewFileStruct.vfFilePos_ls00.lnu2GB_Blocks_ls00 = 0;
    NewFileStruct.vfFilePos_ls00.lnuRemainder_ls00  = 0;
    NewFileStruct.vfFileLen_ls00.lnu2GB_Blocks_ls00 = 0;
    NewFileStruct.vfFileLen_ls00.lnuRemainder_ls00  = 0;
    NewFileStruct.vfFileLineNum_ls00                = 0;
    NewFileStruct.vfFileEncoding_ls00               = Encoding;

    // Initialize magic byte array
    NewFileStruct.vfMagicBytesCache_ls00.lBufLen_ls00 = 3;
    NewFileStruct.vfMagicBytesCache_ls00.lBufPos_ls00 = 0;
    NewFileStruct.vfMagicBytesCache_ls00.pBufPtr_ls00 = new unsigned char[NewFileStruct.vfMagicBytesCache_ls00.lBufLen_ls00];
    memset(NewFileStruct.vfMagicBytesCache_ls00.pBufPtr_ls00, 0, NewFileStruct.vfMagicBytesCache_ls00.lBufLen_ls00);

    if ( NewFileStruct.vfFileType_ls00   == dtypeTape_ls00 &&
         NewFileStruct.vfBackupTool_ls00 != btTypeUnknown_els00 )
    {
         tcn35d_BackupInfo::ADataDirection  TheDataDirection;
         tcn35d_BackupInfo::ADataType       TheDataType;
         Tools_DynamicUTF8String            TheMediaName;
         int                                TheNumberOfPipes;
         char       *                       ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D];
         int                                TheBlockSize;
         int                                ThePagesPerBlock;
         long                               TheCountInPages[2];
         tsp00_Pathc                        TheDefaultDirectory;
         Tools_DynamicUTF8String            TheNodeName = NewFileStruct.vfNodeName_ls00;
         Tools_DynamicUTF8String            TheDBName   = NewFileStruct.vfDBName_ls00;
         Tools_DynamicUTF8String            TheBackupTool;
         Tools_DynamicUTF8String            TheExternalBackupIDList = "";

         TheMediaName            = "PIPE";
         TheCountInPages[0]      = 1;
         TheCountInPages[1]      = 1;
         TheBlockSize            = 8096;
         ThePagesPerBlock        = 8;
         TheDataType             = tcn35d_BackupInfo::DataMigration;
         TheNumberOfPipes        = 1;

         _TCHAR   szDefaultDirectory[PATH_MXSP00];
         szDefaultDirectory[0] = '\0';
		 pMsgInstance->ls98GetLoaderConfigPath(szDefaultDirectory);
		 TheDefaultDirectory.rawAssign(szDefaultDirectory);

         if ( FileMode == sp5vf_write )
         {
              TheBackupTool      = pszBackupToolName_ls00[NewFileStruct.vfBackupTool_ls00];
              TheDataDirection   = tcn35d_BackupInfo::ABackup;
              ThePipeNames[0]    = pszFileName;
              TheExternalBackupIDList = "";
         }
         if ( FileMode == sp5vf_read )
         {
              TheBackupTool           = pszBackupToolName_ls00[NewFileStruct.vfBackupTool_ls00];
              TheDataDirection        = tcn35d_BackupInfo::ARestore;
              ThePipeNames[0]         = NewFileStruct.vfFileName_ls00;
              TheExternalBackupIDList = NewFileStruct.vfBackupId_ls00;
         }
         BackupInfo  * pBackupInfo = NULL;
         
         pBackupInfo =  new BackupInfo(
                            TheBackupTool,
                            TheExternalBackupIDList,
                            TheDataDirection,
                            TheDataType,
                            REINTERPRET_CAST(SAPDB_Char *,(CONST_CAST (unsigned char* ,TheMediaName.StrPtr()))),
                            TheNumberOfPipes,
                            ThePipeNames,
                            TheBlockSize,
                            ThePagesPerBlock,
                            TheCountInPages,
                            TheDefaultDirectory,
                            REINTERPRET_CAST(SAPDB_Char *,(CONST_CAST (unsigned char* ,TheNodeName.StrPtr()))),
                            REINTERPRET_CAST(SAPDB_Char *,(CONST_CAST (unsigned char* ,TheDBName  .StrPtr())))  );

         NewFileStruct.vfBackupControllerFactory.ConstructBackupController( 
         NewFileStruct.vfpBackupController_ls00, pBackupInfo );

    } //if ( NewFileStruct.vfFileType_ls00 == dtypeTape_ls00 )

    return;
}
// ls30VFInitialize()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFOpen
  -----------------------------------------------------------------------------
*/
int
ls30VFOpen(tls00_VFile &VFile, tsp00_Addr ErrText)
{
&if $OS = UNIX
# ifndef UNIX
#  define UNIX
# endif
&endif

#line 246 "vls30.cpp"         /* make line directive always by 1 gt line number */

    ROUTINE_DBG_MEO00 ("ls30VFOpen");
    tsp00_Int4         rc = errOK_els00;
    tsp05_RteFileError RteError;
    tsp05_RteFileInfo  RteInfo;

    VFile.vfFilePos_ls00.lnuRemainder_ls00 = EOF_LS00;   
    //*
    //*  create pipe
    //* 
    if ( VFile.vfFileType_ls00 == dtypePipe_ls00 )
    {
#if defined (UNIX)
         mode_t fileMask=umask(S_IROTH|S_IWOTH);  // dont let the file creation mask be in our way
#ifndef FREEBSD
         // mknod = ok, mkfifo = fail
         if(mknod(VFile.vfFileName_ls00, S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP, 0))
             rc=errOK_els00;
#else
         if(mkfifo(VFile.vfFileName_ls00, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP))
             rc=errOK_els00;
#endif // FREEBSD
             umask(fileMask); // try to leave the file creation mask unchanged
#endif // UNIX
    }
    //*
    //*  open stream connector
    //* 
    if ( VFile.vfFileType_ls00 == dtypeTape_ls00 )
    {
        if ( VFile.vfBackupTool_ls00 != btTypeUnknown_els00 )
             VFile.vfpBackupController_ls00->BeginBackupOperation();
        else
        {
             LoaderMessages::Instance()->ls98Msg(layVFile_els98, ErrText, errFileOpen_els98,
                                                                          VFile.vfFileName_ls00,
                                                                          "Backuptool not specified");
             rc = errFileOpen_els98;
             return rc;
        }
    }


    //*
    //* Check for UTF8 in file name and for escaped quotes. We use a temporary
    //* file name from here on.
    //*
    tls00_String _FileNameToUse;
    _FileNameToUse.strLeng_ls00 = _tcslen(VFile.vfFileName_ls00);
    _FileNameToUse.strAddr_ls00 = new char[_FileNameToUse.strLeng_ls00 + sizeof(char)];
    SAPDB_memcpy(_FileNameToUse.strAddr_ls00, VFile.vfFileName_ls00, _FileNameToUse.strLeng_ls00);
    _FileNameToUse.strAddr_ls00[_FileNameToUse.strLeng_ls00] = 0;

    ls30_VFCheckFileName(&_FileNameToUse);

    //*
    //*  open stream
    //*
    if ( (sp5vf_binary == VFile.vfDataType_ls00) || (ctAscii_els00 == VFile.vfFileEncoding_ls00) )
    {
        sqlfopenc(_FileNameToUse.strAddr_ls00, VFile.vfDataType_ls00, VFile.vfFileMode_ls00,
                  VFile.vfFileBuf_ls00, &VFile.vfFileHndl_ls00, &RteError);
    }
    else
    {
        // UNICODE files are always treated as text files; opening a file binary will
        // always result in opening it not encoded
        const tsp77encoding* pEncoding = pEncodings_ls00[VFile.vfFileEncoding_ls00];

        sqlfopenEncodedc(_FileNameToUse.strAddr_ls00,
                         &pEncoding,
                         pEncoding,
                         VFile.vfFileMode_ls00,
                         VFile.vfFileBuf_ls00,
                         &VFile.vfFileHndl_ls00,
                         &RteError);
        if ( (vf_ok == RteError.sp5fe_result) && (ctUnknown_els00 == VFile.vfFileEncoding_ls00) )
        {
            // Set the encoding of the file
            switch (pEncoding->EncodingType)
            {
                case csp_unicode:
                {
                    VFile.vfFileEncoding_ls00 = ctUCS2_els00;
                    break;
                }
                case csp_unicode_swap:
                {
                    VFile.vfFileEncoding_ls00 = ctUCS2Swapped_els00;
                    break;
                }
                case csp_utf8:
                {
                    VFile.vfFileEncoding_ls00 = ctUTF8_els00;
                    break;
                }
                default:
                {
                    VFile.vfFileEncoding_ls00 = ctAscii_els00;
                }
            }
        }   // end if ( (vf_ok == RteError.sp5fe_result) && ...
        
        // In case there is a real error it is handled in the code snippet that follows
    }

    if (vf_ok == RteError.sp5fe_result)             
    {
        VFile.vfFilePos_ls00.lnu2GB_Blocks_ls00 = 0;
        VFile.vfFilePos_ls00.lnuRemainder_ls00  = 0;

        sqlfinfoc(_FileNameToUse.strAddr_ls00, &RteInfo, &RteError); // Get some file information
        if ( ( vf_ok == RteError.sp5fe_result ) && (sp5vf_write != VFile.vfFileMode_ls00) )
        {
            // in case of WRITE the initial values for file size are valid; so we get
            // in here only for read or append
            VFile.vfFileLen_ls00.lnu2GB_Blocks_ls00 = RteInfo.sp5fi_size_x;
            VFile.vfFileLen_ls00.lnuRemainder_ls00  = RteInfo.sp5fi_size;
        }
    }

    // Check for error; both functions sqlfopenc and sqlfinfoc deliver errors in RteError
    if (vf_ok != RteError.sp5fe_result)
    {
        // File handle is undefined in this case (-1); we set it to 0 to be later able to 
        // decide if the file is opened and has to be close (file handle == 0 --> file not open)
        VFile.vfFileHndl_ls00 = 0;

		if (ErrText != NULL)
		{
			tsp00_C40c pszTmpLZUText;
			pszTmpLZUText.p2c(RteError.sp5fe_text);
			LoaderMessages::Instance()->ls98Msg(layVFile_els98, ErrText, errFileOpen_els98,
																		VFile.vfFileName_ls00,
																		pszTmpLZUText.asCharp());
			VFile.vfFilePos_ls00.lnuRemainder_ls00 = EOF_LS00;    // Indicates error
		}
        rc = errFileOpen_els98;
    }

    // delete temp file name string
    delete [] _FileNameToUse.strAddr_ls00;
    _FileNameToUse.strAddr_ls00 = 0;

    return rc;
}
// ls30VFOpen()

/*
  -----------------------------------------------------------------------------
  function:     ls30VFClose
  -----------------------------------------------------------------------------
*/
int ls30VFClose(tls00_VFile &VFile, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls30VFClose");

    tsp00_Int4          rc = errOK_els00;
    tsp05_RteFileError  RteError;

    if ( 0 != VFile.vfFileHndl_ls00 )
    {
        sqlfclosec(VFile.vfFileHndl_ls00, VFile.vfFileCOpt_ls00, &RteError);
        if (vf_ok != RteError.sp5fe_result)
        {
            tsp00_C40c pszTmpLZUText;
            pszTmpLZUText.p2c(RteError.sp5fe_text);
            LoaderMessages::Instance()->ls98Msg(layVFile_els98, ErrText,
                                                                 errFileClose_els98,
                                                                 VFile.vfFileName_ls00,
                                                                 pszTmpLZUText.asCharp());
            rc = errFileClose_els98;
        }
        else
        {
            VFile.vfFileHndl_ls00 = 0;      // Reset file handle
        }
    }

    VFile.vfFilePos_ls00.lnuRemainder_ls00 = EOF_LS00;        // Set file position to eof

    //*
    //*  close stream connector
    //* 
    if ( VFile.vfFileType_ls00 == dtypeTape_ls00 )
    {
         if ( VFile.vfBackupTool_ls00 != btTypeUnknown_els00 )
         {
              VFile.vfpBackupController_ls00->EndBackupOperation();
              VFile.vfBackupId_ls00 = VFile.vfpBackupController_ls00->m_BackupId;
              VFile.vfBackupControllerFactory.DestroyBackupController(VFile.vfpBackupController_ls00);
         }
         else
         {
             Tools_DynamicUTF8String errmsg = "Backuptool not specified";
             LoaderMessages::Instance()->ls98Msg(layVFile_els98, ErrText, errFileClose_els98,
                                                  VFile.vfFileName_ls00,errmsg.StrPtr());
             rc = errFileClose_els98;
             return rc;
         }

    }

    return rc;
}
// ls30VFClose()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFReadLn
  -----------------------------------------------------------------------------
*/
// lcount muss das anzuhaengende null mit einschliessen
int ls30VFReadLn(tls00_VFile   &VFile,
                 tsp00_Addr    *pszLineBuffer,
                 tsp00_Addr     ErrText,
                 tsp00_Longint  lCount,
                 tsp00_Longint  MaxReadLen)
{
    ROUTINE_DBG_MEO00 ("ls30VFReadLn");

    LoaderMessages *pMsgObj = LoaderMessages::Instance(); // error reporting obj

    tsp00_Longint ReadLen   = 0;
    tsp00_Addr    pszBuffer = NULL;

    if (lCount > MaxReadLen)
    {
        pMsgObj->ls98Msg(layVFile_els98, ErrText,errFileRead_els98, VFile.vfFileName_ls00,
                                                                    "Maximum read length exceeded");
        return errFileRead_els98;
    }

    // adjust bytes to read
    tsp00_Longint lBytesToRead = (lCount==0) ? MaxReadLen : lCount;

    pszBuffer = new char[lBytesToRead + sizeof(_TCHAR)];    // allocate memory for data to read and initialize it
    pszBuffer[0] = 0;

    ReadLen = ls30VFReadLnEx(VFile, pszBuffer, ErrText, lBytesToRead);
    if (ReadLen > 0)
    {
        *pszLineBuffer = pszBuffer;
    }
    else
    {
        delete [] pszBuffer;
        pszBuffer = NULL;
    }

    return STATIC_CAST( tsp00_Int4, ReadLen);           // cast because incompatibility to IA64
}
//  ls30VFReadLn()

/*
  -----------------------------------------------------------------------------
  function:     ls30VFReadLnEx
  -----------------------------------------------------------------------------
*/
int
ls30VFReadLnEx(tls00_VFile   &VFile,
               tsp00_Addr     pszLineBuffer,
               tsp00_Addr     pszErrText,
               tsp00_Longint  lCount,
               tsp00_Longint  MaxReadLen)
{
    ROUTINE_DBG_MEO00 ("ls30VFReadLnEx");

    tsp00_Longint       ReadLen   = 0;
    tsp00_Addr          pszBuffer = NULL;

    if (lCount > MaxReadLen)
    {
        LoaderMessages::Instance()->ls98Msg(layVFile_els98, pszErrText,
                                                             errFileRead_els98,
                                                             VFile.vfFileName_ls00,
                                                             "Maximum read length exceeded");
        return errFileRead_els98;
    }

    //*     Set count of bytes to read
    tsp00_Longint lBytesToRead = (0 == lCount) ? MaxReadLen : lCount;
    if (sp5vf_text == VFile.vfDataType_ls00)                        // text mode
    {
        ReadLen = ls30_ReadLnTxt(pszLineBuffer, VFile, lBytesToRead, pszErrText);
    }
    else                                                            // binary mode
    {
        ReadLen = ls30_ReadLnBin(pszLineBuffer, VFile, lBytesToRead, pszErrText);
    }


    if (ReadLen > 0)
    {
        // Adapt VFile structure members
        tsp00_Longint lFilePosIncrement = ReadLen;
        ++VFile.vfFileLineNum_ls00;                     // increase count of read rows

        if (sp5vf_text == VFile.vfDataType_ls00)
        {
            lFilePosIncrement += NEW_LINE_LENGTH;
        }
        ls07AddLargeNumbers(VFile.vfFilePos_ls00, lFilePosIncrement);
    }
    
    return STATIC_CAST(tsp00_Int4, ReadLen);        // IA64
}
//  ls30VFReadLnEx()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFReadLongBin
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls30VFReadLongBin(tsp00_Addr                pszBuffer,
                  tls00_VFile              &VFile,
                  tsp00_Longint             lBytes,
                  const tls00_LargeNumber  *plReadPosition,
                  tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls30VFReadLongBin");

    tsp05_RteFileError RteError;
    tsp00_Longint      ReadLen = 0;
    tsp00_Int4         rc      = errOK_els00;

	extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;
	start =  RTESys_MicroSecTimer();

    // Seek to right position
    if ( VFile.vfFileType_ls00 != dtypePipe_ls00 )
         rc = ls30VFSeekLong(VFile, plReadPosition, pszErrText);
    if (errOK_els00 == rc)
    {
        // Read data
        sqlfreadc(VFile.vfFileHndl_ls00, pszBuffer, lBytes, &ReadLen, &RteError);
        if (RteError.sp5fe_result  != vf_ok)
        {
            if (vf_eof == RteError.sp5fe_result)
            {
                VFile.vfFilePos_ls00.lnuRemainder_ls00 = EOF_LS00;

                ReadLen = EOF_LS00;
            }
            else
            {
                tsp00_C40c pszTmpLZUText;
                pszTmpLZUText.p2c(RteError.sp5fe_text);
                LoaderMessages::Instance()->ls98Msg(layVFile_els98, pszErrText,
                                                                     errFileRead_els98,
                                                                     VFile.vfFileName_ls00,
                                                                     pszTmpLZUText.asCharp());
                ReadLen = errFileRead_els98;
            }
        }
        else
        {            
            ls07AddLargeNumbers(VFile.vfFilePos_ls00, ReadLen);    // adapt file position
        }
    }
    else
    {
        ReadLen = rc;   // In case of error the error code is negative
    }

	finish = RTESys_MicroSecTimer();
    duration = finish - start;
    performance.perIO_ls00 += duration ;

    return ReadLen;
}
//  ls30VFReadLongBin()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFWriteLn
  -----------------------------------------------------------------------------
*/
int
ls30VFWriteLn(tls00_VFile   &VFile,
              tsp00_Addr     pszBuffer,
              tsp00_Longint  lCount,
              tsp00_Addr     pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls30VFWriteLn");

    tsp00_Int4          rc = errOK_els00;
    tsp05_RteFileError  RteError;

    //*
    //* Performance
    //*
    extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;
    start =  RTESys_MicroSecTimer();

    sqlfwritec(VFile.vfFileHndl_ls00, pszBuffer, lCount, &RteError);

    finish = RTESys_MicroSecTimer();
    duration = finish - start;
    performance.perIO_ls00 += duration ;

    if (vf_ok != RteError.sp5fe_result)
    {
        tsp00_C40c pszTmpLZUText;
        pszTmpLZUText.p2c(RteError.sp5fe_text);
        LoaderMessages::Instance()->ls98Msg(layVFile_els98, pszErrText,
                                                             errFileWrite_els98,
                                                             VFile.vfFileName_ls00,
                                                             pszTmpLZUText.asCharp());
        rc = errFileWrite_els98;
    }
    else
    {
        if (sp5vf_text == VFile.vfDataType_ls00)
        {
            ++VFile.vfFileLineNum_ls00;
            lCount += NEW_LINE_LENGTH;
        }

        ls07AddLargeNumbers(VFile.vfFileLen_ls00, lCount);
        VFile.vfFilePos_ls00 = VFile.vfFileLen_ls00;        // file pos and file len are identical
    }

    return rc;
}
//  ls30VFWriteLn()


/*
  -----------------------------------------------------------------------------
   function:     ls30VFWriteHeader
  -----------------------------------------------------------------------------
*/
int
ls30VFWriteHeader(tls00_VFile & VFile,
                  tsp00_Int4    lRecordLength,
                  tsp00_Addr    pszCommand,
                  tsp00_Addr    pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls30VFWriteHeader");

    tsp00_Int4          rc = errOK_els00;

    tls30_DatafileHeader HeaderStruct;              // only for length information

    tsp00_Int4 lLengthOfBuffer = STATIC_CAST( tsp00_Int4, (sizeof(tls30_DatafileHeader) + strlen(pszCommand)) );
    lLengthOfBuffer = ALIGN_EO00(lLengthOfBuffer, 1024);    // align to full kB
    tsp00_Addr pszBuffer       = NULL;

    pszBuffer = new _TCHAR[lLengthOfBuffer];
    tsp00_Addr pszBuffer2 = pszBuffer;
    memset(pszBuffer, 0, lLengthOfBuffer);      // initialize

    // Define swap kind and log it
    tsp00_SwapKind_Enum Swap;
    Swap = ls07DefineSwap(NULL);

    ls07Swap4BIntToChar(lLengthOfBuffer, Swap, sw_normal, pszBuffer2);
    pszBuffer2 += HeaderStruct.dhHeaderLength_ls30.size();

    SAPDB_memcpy(pszBuffer2, HEADER_IDENTIFIER_LS00, sizeof(HEADER_IDENTIFIER_LS00));
    pszBuffer2 += HeaderStruct.dhIdentifier_ls30.size();

    ls07Swap4BIntToChar(lRecordLength, Swap, sw_normal, pszBuffer2);
    pszBuffer2 += HeaderStruct.dhRecordLength_ls30.size();

    SAPDB_memcpy(pszBuffer2, pszCommand, strlen(pszCommand));

    rc = ls30VFWriteLn(VFile, pszBuffer, lLengthOfBuffer, pszErrText);  // error is already logged by ls30VFWriteLn

    return rc;
}
// ls30VFWriteHeader()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFReadHeader
  -----------------------------------------------------------------------------
*/
int
ls30VFReadHeader(tls00_VFile & VFile,
                 tsp00_Int4  & lRecordLength,
                 tsp00_Addr    pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls30VFReadHeader");

    tsp00_Int4 rc = errOK_els00;

    tsp00_Int4 lLengthOfBuffer = sizeof(tls30_DatafileHeader);
    tsp00_Addr pszBuffer       = NULL;

    pszBuffer = new _TCHAR[lLengthOfBuffer];
    memset(pszBuffer, 0, lLengthOfBuffer);      // initialize

    tsp00_Int4 ReadLen = STATIC_CAST( tsp00_Int4, ls30_ReadLnBin(pszBuffer, VFile, lLengthOfBuffer, pszErrText) );
    if ( ReadLen <= 0 )         // In case of error length corresponds to error number
    {
        if (EOF_LS00 == ReadLen)  // file is empty
        {
            rc = errNoDatafileHeader_ls98;
        }
        else
        {
            rc = ReadLen;
        }
    }
    else
    {
        tls30_DatafileHeader HeaderStruct;
        tsp00_Int4           lPosition = HeaderStruct.dhHeaderLength_ls30.size();
        if (0 != memcmp(&pszBuffer[lPosition], HEADER_IDENTIFIER_LS00, sizeof(HEADER_IDENTIFIER_LS00)))
        {
            // We need to seek back to the beginning of file if we do not find a header
            ls30VFSeek(VFile, 0, pszErrText);     // not expecting errors

            rc = errNoDatafileHeader_ls98;
        }
        else
        {
            tsp00_SwapKind_Enum Swap;
            Swap = ls07DefineSwap(NULL);

            // At this point we assume that the header may not exceed 2GB so that we use a simple long int
            tsp00_Longint lfilepos = ls07Swap4BHexToLong(pszBuffer, sw_normal, Swap);

            lPosition     = HeaderStruct.dhHeaderLength_ls30.size() + HeaderStruct.dhIdentifier_ls30.size();
            lRecordLength = ls07Swap4BHexToLong(&pszBuffer[lPosition], sw_normal, Swap);

            // After work is done we need to seek to the beginning of data; the
            // seek function updates the vfFilePos member in VFile, too.
            ls30VFSeek(VFile, lfilepos, pszErrText);     // not expecting errors
        }
    }

    if (NULL != pszBuffer)
    {
        delete [] pszBuffer;
        pszBuffer = NULL;
    }

    return rc;
}
//  ls30VFReadHeader()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFSeek
  -----------------------------------------------------------------------------
*/
int ls30VFSeek(tls00_VFile &VFile, tsp00_Longint FilePos, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls30VFSeek");

    tsp00_Int4          rc = errOK_els00;
    tsp05_RteFileError  RteError;

	extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;
	start =  RTESys_MicroSecTimer();

    if ( 0 != VFile.vfFileHndl_ls00 )
    {
        sqlfseekc(VFile.vfFileHndl_ls00, FilePos, sp5vf_seek_begin, &RteError);
        if (vf_ok != RteError.sp5fe_result)
        {
            tsp00_C40c pszTmpLZUText;
            pszTmpLZUText.p2c(RteError.sp5fe_text);
            LoaderMessages::Instance()->ls98Msg(layVFile_els98, ErrText,
                                                                 errFileRead_els98,
                                                                 VFile.vfFileName_ls00,
                                                                 pszTmpLZUText.asCharp());
            rc = errFileRead_els98;
        }
        else
        {
            // Adapt file pos in VFile structure
            ls07AddLargeNumbers(VFile.vfFilePos_ls00, FilePos);
        }
    }

    finish = RTESys_MicroSecTimer();
    duration = finish - start;
    performance.perIO_ls00 += duration ;

    return rc;
}
// ls30VFSeek()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFSeekLong (overloaded)
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFSeekLong(tls00_VFile             &VFile,
               const tls00_LargeNumber *pFilePos,
               tsp00_Addr               pszErrText,
               tsp05_RteSeekKind_Param  lOrigin)
{
    ROUTINE_DBG_MEO00 ("ls30VFSeekLong");

    tsp00_Int4          rc = errOK_els00;
    tsp05_RteFileError  RteError;
    RteError.sp5fe_result.becomes(vf_ok);           // initialize to no-error

	extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;
	start =  RTESys_MicroSecTimer();
    
    if (0 != VFile.vfFileHndl_ls00)
    {
        tls00_LargeNumber distance = *pFilePos;

        // Unterscheiden, ob vom anfang gesucht wird oder von der current pos
        if (sp5vf_seek_cur == lOrigin)
        {
            distance = ls07SubtractLargeNumbers(*pFilePos, VFile.vfFilePos_ls00);
        }
        //else if (sp5vf_seek_begin == lOrigin)   // adapting distance here is done by initialization

        // position to seek to is higher than the 2 GB limit in both directions - pos and neg
        if (0 != distance.lnu2GB_Blocks_ls00)
        {
            tsp00_Longint maxdistance = (distance.lnu2GB_Blocks_ls00 < 0) ? -MAX_INT4_SP00 : MAX_INT4_SP00;
            tsp00_Longint cnt         =  distance.lnu2GB_Blocks_ls00;
            if (distance.lnu2GB_Blocks_ls00 < 0)
            {
                cnt *= -1;
            }

            sqlfseekc(VFile.vfFileHndl_ls00, maxdistance, lOrigin, &RteError);
            if ( (vf_ok == RteError.sp5fe_result) && (cnt > 1) )
            {
                for (tsp00_Longint i = 1; ( (i < cnt) && (vf_ok == RteError.sp5fe_result) ); ++i)
                {
                    sqlfseekc(VFile.vfFileHndl_ls00, maxdistance, sp5vf_seek_cur, &RteError);
                }
            }

            // taking into account pos and neg movements in file
            if ( (0 != distance.lnuRemainder_ls00) && (vf_ok == RteError.sp5fe_result) )
            {
                sqlfseekc(VFile.vfFileHndl_ls00, distance.lnuRemainder_ls00, sp5vf_seek_cur, &RteError);
            }
        }
        else    // position to seek to is lower than the 2GB limit
        {
            if (0 != distance.lnuRemainder_ls00)        // taking into account pos and neg movements in file
            {
                sqlfseekc(VFile.vfFileHndl_ls00, distance.lnuRemainder_ls00, lOrigin, &RteError);
            }
        }
    }

    // Error handling
    if (vf_ok != RteError.sp5fe_result)
    {
        tsp00_C40c pszTmpLZUText;
        pszTmpLZUText.p2c(RteError.sp5fe_text);
        LoaderMessages::Instance()->ls98Msg(layVFile_els98, pszErrText, errFileRead_els98,
                                                                         VFile.vfFileName_ls00,
                                                                         pszTmpLZUText.asCharp());
        rc = errFileRead_els98;
    }
    else
    {
        VFile.vfFilePos_ls00.lnu2GB_Blocks_ls00 = pFilePos->lnu2GB_Blocks_ls00;
        VFile.vfFilePos_ls00.lnuRemainder_ls00  = pFilePos->lnuRemainder_ls00;
    }

    finish = RTESys_MicroSecTimer();
    duration = finish - start;
    performance.perIO_ls00 += duration ;

    return rc;
}
// ls30VFSeekLong()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFSeekLong (overloaded)
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFSeekLong(tls00_VFile             &VFile,
               const tsp00_Longint      lDistance,
               tsp00_Addr               pszErrText,
               tsp05_RteSeekKind_Param  lOrigin)
{
    ROUTINE_DBG_MEO00 ("ls30VFSeekLong II");

    tsp00_Int4          rc = errOK_els00;
    tsp05_RteFileError  RteError;

	extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;
	start =  RTESys_MicroSecTimer();

    if (0 != VFile.vfFileHndl_ls00)
    {
        sqlfseekc(VFile.vfFileHndl_ls00, lDistance, lOrigin, &RteError);

        // Error handling
        if (vf_ok != RteError.sp5fe_result)
        {
            tsp00_C40c pszTmpLZUText;
            pszTmpLZUText.p2c(RteError.sp5fe_text);
            LoaderMessages::Instance()->ls98Msg(layVFile_els98, pszErrText, errFileRead_els98,
                                                                             VFile.vfFileName_ls00,
                                                                             pszTmpLZUText.asCharp());
            rc = errFileRead_els98;
        }
        else
        {
            // Adapt file position
            ls07AddLargeNumbers(VFile.vfFilePos_ls00, lDistance);
        }
    }

    finish = RTESys_MicroSecTimer();
    duration = finish - start;
    performance.perIO_ls00 += duration ;
    return rc;
}
// ls30VFSeekLong()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFTruncate
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFTruncate(tls00_VFile &VFile, tsp00_Addr ErrText, tsp00_Longint lNewSize)
{
    ROUTINE_DBG_MEO00 ("ls30VFTruncate");

    tsp00_Int4          rc = errOK_els00;
    tsp05_RteFileError  RteError;

    if ( 0 != VFile.vfFileHndl_ls00 )
    {
        sqlftruncatec(VFile.vfFileHndl_ls00, lNewSize, &RteError);
        if (vf_ok != RteError.sp5fe_result)
        {
            tsp00_C40c pszTmpLZUText;
            pszTmpLZUText.p2c(RteError.sp5fe_text);
            LoaderMessages::Instance()->ls98MsgToString(layVFile_els98, ErrText,
                                                                         errFileTruncate_els98,
                                                                         VFile.vfFileName_ls00,
                                                                         pszTmpLZUText.asCharp());
            rc = errFileTruncate_els98;
        }
        else
        {
            VFile.vfFilePos_ls00.lnu2GB_Blocks_ls00 = 0;
            VFile.vfFilePos_ls00.lnuRemainder_ls00  = lNewSize;
            VFile.vfFileLen_ls00.lnu2GB_Blocks_ls00 = 0;
            VFile.vfFileLen_ls00.lnuRemainder_ls00  = lNewSize;
        }
    }

    return rc;
}
// ls30VFTruncate()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFGetFileSize
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFGetFileSize(tsp00_Addr pszFileName, tls00_LargeNumber *plFileSize)
{
    ROUTINE_DBG_MEO00 ("ls30VFGetFileSize");

    tsp00_Int4 rc = errOK_els00;

    tsp05_RteFileError  RteError;
    tsp05_RteFileInfo   RteInfo;

    sqlfinfoc(pszFileName, &RteInfo, &RteError);
    if (vf_ok == RteError.sp5fe_result)
    {
        if (RteInfo.sp5fi_exists)
        {
            plFileSize->lnu2GB_Blocks_ls00 = RteInfo.sp5fi_size_x;
            plFileSize->lnuRemainder_ls00  = RteInfo.sp5fi_size;            
        }
    }
    else
    {
        rc = errFileRead_els98;
    }

    return rc;
}
//  ls30VFGetFileSize()


/*
  -----------------------------------------------------------------------------
  function:     ls30VFMoveToStartPosition
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFMoveToStartPosition(tls00_VFile              &VFile,
                          const tls00_LargeNumber  *plStartPos,
                          tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls30VFMoveToStartPosition");

    tsp00_Int4    rc      = errOK_els00;
    tsp00_Longint ReadLen = 0;

    // In case of binary data file the lStartPos is the physical pos
    // In case of text file it is the line/record number, so we have to read until the right
    //  line number is reached
    if (sp5vf_binary == VFile.vfDataType_ls00)
    {
        rc = ls30VFSeekLong(VFile, plStartPos, pszErrText, sp5vf_seek_begin);
    }
    else
    {
        tsp00_Longint i = 1;        // because working in c/c++
        _TCHAR        szLineBuffer[LINELENGTH_MXSP00];

        for (i; ( (i < plStartPos->lnuRemainder_ls00) && (rc >= 0) ); ++i)
        {
            ReadLen = ls30VFReadLnEx(VFile, szLineBuffer, pszErrText, 0);
        }
        // VFile.vfFilePos_ls00 is adapted with every call of ls30VFReadLnEx

        if (ReadLen < 0)        // adapt error code
        {
            rc = STATIC_CAST(tsp00_Int4, ReadLen);
        }
        VFile.vfFileLineNum_ls00 = 0;       // nothing read here!
    }

    return rc;
}
// ls30VFMoveToStartPosition()


/*
  -----------------------------------------------------------------------------
  function:     ls30Detect_BinaryFile_Encoding
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30Detect_BinaryFile_Encoding(tls00_VFile& VFile, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls30Detect_BinaryFile_Encoding");

    tsp00_Int4 rc     = errOK_els00;
    tsp00_Int4 rcRead = errOK_els00;

    tls00_Buffer2 *_pMB_Cache = &VFile.vfMagicBytesCache_ls00;
    tls00_LargeNumber lReadPos;
    lReadPos.lnu2GB_Blocks_ls00 = 0;
    lReadPos.lnuRemainder_ls00  = 0;

    tls00_CodeType FileEncoding = ctUnknown_els00;

    rcRead = ls30VFReadLongBin(STATIC_CAST(char*, _pMB_Cache->pBufPtr_ls00),
                               VFile,
                               _pMB_Cache->lBufLen_ls00,
                               &lReadPos,
                               pszErrText);
    if (rcRead < 0)
    {
        rc = rcRead;
    }
    else
    {
        int                   i          = ctCodeSet_els00;
        const tsp77charConst* pIndicator = NULL;

        if (ctUnknown_els00 == VFile.vfFileEncoding_ls00)       // we have to check them all
        {
            do
            {
                if (NULL != pEncodings_ls00[i])
                {
                    pIndicator = &pEncodings_ls00[i]->charTable->indicator;
                    if (sp77charIsEqual(pIndicator, _pMB_Cache->pBufPtr_ls00, pIndicator->byteCount))
                    {
                        VFile.vfFileEncoding_ls00 = STATIC_CAST(tls00_CodeType, i);
                    }
                }
                --i;
            }
            while ((ctUnknown_els00 == VFile.vfFileEncoding_ls00) && (i > 0));
                // At the end the ASCII encoding fits in any case
        }
        else
        {
            // Wir koennen gezielt ueberpruefen, ob das richtige encoding angegeben ist
            if (NULL != pEncodings_ls00[i])
            {
                pIndicator = &pEncodings_ls00[VFile.vfFileEncoding_ls00]->charTable->indicator;
                if (SAPDB_FALSE == sp77charIsEqual(pIndicator, _pMB_Cache->pBufPtr_ls00, pIndicator->byteCount))
                {
                    LoaderMessages::Instance()->ls98Msg(layDataLoad_els98, pszErrText, errFileWrongEncoding_els98,
                                                                                        VFile.vfFileName_ls00,
                                                                                        pszCodeTypeName_ls00[FileEncoding]);
                    rc = errFileWrongEncoding_els98;
                }
            }
        }
    }

    if (errOK_els00 == rc)
    {
        // In case of ASCII encoding we have to use the read magic bytes as data later when
        // loading the real data; in all other cases we can ignore the magic bytes in further processing
        if ( (ctAscii_els00 != VFile.vfFileEncoding_ls00) && (ctIgnore_els00 != VFile.vfFileEncoding_ls00) )
        {
            ls30VFEmptyMagicBytesCache(&VFile);
        }
// TODOTODO altlasten unterstuetzen? wenn die dateien noch ohne magic bytes geschrieben wurden, ist ein vergleich
//           hier sinnlos
    }

    return rc;
}
// ls30Detect_BinaryFile_Encoding()


/*
  -----------------------------------------------------------------------------
  function:     ls30Mark_BinaryFile_Encoding
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30Mark_BinaryFile_Encoding(tls00_VFile& VFile, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls30Mark_BinaryFile_Encoding");

    tsp00_Int4 rc     = errOK_els00;

    if ( (ctAscii_els00  != VFile.vfFileEncoding_ls00) &&
         (ctIgnore_els00 != VFile.vfFileEncoding_ls00) )
    {
        if (ctUTF8_els00 == VFile.vfFileEncoding_ls00)
        {
            rc = ls30VFWriteLn(VFile, 
                               CONST_CAST(char*, sp77encodingUTF8->charTable->indicator.bytes),
                               sp77encodingUTF8->charTable->indicator.byteCount,
                               pszErrText);
        }
        else
        {
            rc = ls30VFWriteLn(VFile, 
                               CONST_CAST(char*, sp77encodingUCS2Native->charTable->indicator.bytes),
                               sp77encodingUCS2Native->charTable->indicator.byteCount,
                               pszErrText);
        }
    }
    return rc;
}
// ls30Mark_BinaryFile_Encoding()


/*
  -----------------------------------------------------------------------------
  Local functions: implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls30_ReadLnTxt
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls30_ReadLnTxt(tsp00_Addr     pszBuffer,
               tls00_VFile   &VFile,
               tsp00_Longint  lBytes,
               tsp00_Addr     pszErrorText,
               tsp00_Longint  MaxReadLen)
{
    ROUTINE_DBG_MEO00 ("ls30_ReadLnTxt");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // error reporting obj

    tsp05_RteFileError RteError;
    tsp00_Longint      ReadLen = 0;

	extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;
	start =  RTESys_MicroSecTimer();

    do
    {
        sqlfreadc(VFile.vfFileHndl_ls00, pszBuffer, MaxReadLen, &ReadLen, &RteError);
        if (vf_ok != RteError.sp5fe_result)     // error???
        {
            if (vf_eof == RteError.sp5fe_result)
            {
                VFile.vfFilePos_ls00.lnuRemainder_ls00 = ReadLen = EOF_LS00;
            }
            else
            {
                tsp00_C40c pszTmpLZUText;
                pszTmpLZUText.p2c(RteError.sp5fe_text);
                pMsgObj->ls98Msg(layVFile_els98, pszErrorText, errFileRead_els98,
                                                               VFile.vfFileName_ls00,
                                                               pszTmpLZUText.asCharp());
                ReadLen = errFileRead_els98;
            }
        }
        else
        {
            if (vf_ok != RteError.sp5fe_warning)    // warning  --> buffer not sufficient
            {
                pMsgObj->ls98Msg(layVFile_els98, pszErrorText, errFileRead_els98,
                                                               VFile.vfFileName_ls00,
                                                               "Buffer (32 kByte) too small to hold whole data row");
                ReadLen = errFileRead_els98;
            }
        }
    }
    while (0 == ReadLen);

    finish = RTESys_MicroSecTimer();
    duration = finish - start;
    performance.perIO_ls00 += duration ;
    
    return ReadLen;
}
//  ls30_ReadLnTxt()


/*
  -----------------------------------------------------------------------------
  function:     ls30_ReadLnBin
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls30_ReadLnBin(tsp00_Addr    &pszBuffer,
               tls00_VFile   &VFile,
               tsp00_Longint  lBytes,
               tsp00_Addr     pszErrorText)
{
    ROUTINE_DBG_MEO00 ("ls30_ReadLnBin");

    tsp05_RteFileError RteError;
    tsp00_Longint      ReadLen = 0;

	extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;
	start =  RTESys_MicroSecTimer();

    sqlfreadc(VFile.vfFileHndl_ls00, pszBuffer, lBytes, &ReadLen, &RteError);
    if (vf_ok != RteError.sp5fe_result)
    {
        if (vf_eof == RteError.sp5fe_result)
        {
            VFile.vfFilePos_ls00.lnuRemainder_ls00 = EOF_LS00;

            ReadLen = EOF_LS00;
        }
        else
        {
            tsp00_C40c pszTmpLZUText;
            pszTmpLZUText.p2c(RteError.sp5fe_text);
            LoaderMessages::Instance()->ls98Msg(layVFile_els98, pszErrorText,
                                                                 errFileRead_els98,
                                                                 VFile.vfFileName_ls00,
                                                                 pszTmpLZUText.asCharp());
            ReadLen = errFileRead_els98;
        }
    }

    finish = RTESys_MicroSecTimer();
    duration = finish - start;
    performance.perIO_ls00 += duration ;

    return ReadLen;
}
//  ls30_ReadLnBin()


/*
  -----------------------------------------------------------------------------
  function:     ls30_VFCheckFileName
  -----------------------------------------------------------------------------
*/
void
ls30_VFCheckFileName(tls00_String* pName)
{
    SAPDB_Int4  i = 0;
    SAPDB_Char  notUsedErrText[MAX_REPLY_LENGTH_LS00];

    for (i; i < pName->strLeng_ls00; ++i)
    {
        if (0 == sp77encodingUTF8->isAscii7(STATIC_CAST(const void*, &pName->strAddr_ls00[i])))
        {
            break;
        }
    }
    if (i != pName->strLeng_ls00)
    {
        SAPDB_Char* _pEndPtr = NULL;

        Tools_DynamicUTF8String _filename;
        _filename.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pName->strAddr_ls00));
        _filename.ConvertToASCII_Latin1(pName->strAddr_ls00, pName->strAddr_ls00 + pName->strLeng_ls00, _pEndPtr);
        *_pEndPtr = 0;
    }

    //*
    //* Check for escaped single and double quotes in file name and remove the escape quotes
    //*
    SAPDB_Char* _pszAuxName = pName->strAddr_ls00;                     // Memorize the ptr

    // We use a function thought for undoubling delimiters in char type data. So we have to
    // deal with some shortcomings like getting a new string if escaped (double) quotes were found.
    // Coming here the filename is ASCII
    tls00_Pattern _pattern;
    SAPDB_Char _szQuote[2][2] = {{"'"}, {"\""}};
    SAPDB_Char _pszSeparatorIgnore[2] = ","; 
    for (i = 0; i < 2; ++i)
    {
        memset(&_pattern, 0, sizeof(tls00_Pattern));
        ls18ConstructPattern(&_pattern, _pszSeparatorIgnore, _szQuote[i], ctAscii_els00, notUsedErrText);
        ls18UndoubleDelimitersInCharData(pName, &_pattern, ctAscii_els00);
        if (_pszAuxName != pName->strAddr_ls00)                  // In case we have 'undoubled' single quotes we get a new string.
        {                                                       // --> Delete the old string and use the new.
            delete [] _pszAuxName;
            pName->strAddr_ls00[pName->strLeng_ls00] = 0;
            _pszAuxName = pName->strAddr_ls00;
        }
        ls18DestructPattern(&_pattern);
    }
}
// ls30_VFCheckFileName()