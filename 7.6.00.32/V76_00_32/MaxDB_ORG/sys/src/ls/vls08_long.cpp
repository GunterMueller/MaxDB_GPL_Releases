/*
  -----------------------------------------------------------------------------
  module: vls08_long.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS

  special area: Replication Manager

  description:  Long data type support

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
#include "hls041.h"
#include "hls05.h"          // string routines
#include "hls07.h"
#include "hls08_long.h"
#include "hls30.h"          // virtual file interface
#include "hls98msg.h"       // logging
#include "hls99.h"          // tracing


// Datatype compatibility/conversion matrix for LONG values (mapping db LONG types to external code types) in case
// of unicode database when using dataload or dataupdate with UCS2 order interface


/* Possible db data types; includes long data types which are marked UPPERCASE

 0                                                                 6            7           8             9
 dfixed,   dfloat,    dcha,      dche,       dchb,     drowid,     DSTRA,       DSTRE,      DSTRB,        DSTRDB,
 10                                                                                                       19
 ddate,    dtime,     dvfloat,   dtimestamp, dunknown, dnumber,    dnonumber,   dduration,  ddbyteebcdic, DLONGA,
 20        21         22
 DLONGE,   DLONGB,    DLONGDB,   dboolean,   dunicode, ddtfiller1, ddtfiller2,  ddtfiller3, ddtfiller4,   dsmallint,
 30                                          34        35
 dinteger, dvarchara, dvarchare, dvarcharb,  DSTRUNI,  DLONGUNI,   dvarcharuni, dudt,       dabaptabhandle
*/


// Compatibility/conversion matrix loading data into LONG columns in UNICODE databases;
// In case of a UNICODE DB the Loader always uses the UCS2 order interface -
// so if the external code type is ascii, the data must be
// converted to UCS2 to put it into UCS2 packet and the kernel 'reconverts' it upon inserting

// 0 - conversion forbidden - results in error message (external binary may not be inserted/converted
//                            to LONG UNICODE OR LONG ASCII; same applies for external UNICODE and LONG ASCII
// 1 - no conversion needed
// 2 - ascii  -> ucs2
// 3 - utf8   -> ucs2
// 4 - ebcdic -> ucs2 (actually not longer supported)
static char
szLOAD_LONG_Compatibility_ls08[9][39] =
             {  {0},
                {0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0},    //ctAscii_els00
                {0,0,0,0,0,0,4,4,1,1,0,0,0,0,0,0,0,0,0,4,4,1,1,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0},    //ctEbcdic_els00
                {0,0,0,0,0,0,3,0,1,1,0,0,0,0,0,0,0,0,0,3,0,1,1,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0},    //ctUTF8_els00
                {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},    //ctIgnore_els00
                {0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2_els00
                {0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2Swapped_els00
                {0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2Native_els00
                {0}};


// Datatype compatibility/conversion matrix for the process of extracting LONG values
// (mapping db LONG types to external code types) in case
// of unicode database when using dataextract with UCS2 order interface

// 0 - conversion forbidden - results in error message (UCS2 char data may not be converted to ASCII without information loss)
// 1 - no conversion needed - internal binary LONGs (e.g. datatype LONG BYTE) will be extracted
//                            without conversion to any external char encoding like ASCII;
//                            any internal representation of LONG (either char or binary) may be
//                            extracted to binary files
// 2 - ucs2   -> ascii      - possible if data type is LONG CHAR ASCII and only sent over OI as UCS2;
//                            but highly dangerous (info loss possible)
// 3 - ucs2   -> utf8
// 4 - ucs2   -> ebcdic (actually not longer supported)
static char
szEXTRACT_LONG_UNICODE_ls08[9][39] =
             {  {0},
                {0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},    //ctAscii_els00
                {0,0,0,0,0,0,4,4,1,1,0,0,0,0,0,0,0,0,0,4,4,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},    //ctEbcdic_els00
                {0,0,0,0,0,0,3,3,1,1,0,0,0,0,0,0,0,0,0,3,3,1,1,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0},    //ctUTF8_els00
                {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctIgnore_els00
                {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2_els00
                {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2Swapped_els00
                {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2Native_els00
                {0}};

// Datatype compatibility/conversion matrix for the process of extracting LONG values
// (mapping db LONG types to external code types) in case
// of ASCII/UNICODE database when using dataextract with ASCII order interface

// 0 - conversion forbidden - results in error message
// 1 - no conversion needed - internal binary LONGs (e.g. datatype LONG BYTE) will be extracted
//                            without conversion to any external char encoding like ASCII;
//                            any internal representation of LONG (either char or binary) may be
//                            extracted to binary files
// 2 - ascii  -> ucs2
// 3 - ascii  -> utf8
// 4 - ascii  -> ebcdic (actually not longer supported)
static char
szEXTRACT_LONG_ASCII_ls08[9][39] =
             {  {0},
                {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctAscii_els00
                {0,0,0,0,0,0,4,4,1,1,0,0,0,0,0,0,0,0,0,4,4,1,1,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0},    //ctEbcdic_els00
                {0,0,0,0,0,0,3,3,1,1,0,0,0,0,0,0,0,0,0,3,3,1,1,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0},    //ctUTF8_els00
                {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctIgnore_els00
                {0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2_els00
                {0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2Swapped_els00
                {0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},    //ctUCS2Native_els00
                {0}};

// Maps external encoding types of long files to db long data types
static char
szLONG_Mapping_ls08[39] =
    {0,0,0,0,0,0,ctAscii_els00,ctEbcdic_els00,ctIgnore_els00,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,ctUCS2_els00,0,0,0,0};    //ctAscii_els00
             

//*******  S T A T I C   F U N C T I O N S   D E C L A R A T I O N  ********

/*
  -----------------------------------------------------------------------------
  function:     ls08_FillLong
  -----------------------------------------------------------------------------
  description:

  arguments:    pTransformationRTE  [IN] - structure with information on processing the
                                     dataload command
                pTransformationCmd [IN]  - structure with user information for
                                     processing dataload command.

  returnvalue:  errOK_els00                   - everything went fine
                errFileRead_els98             - error reading long file
                EOF_LS00                      - long file completely read
                errInternal_els98             - error upon converting between encodings
                errConvertingEncoding_els98   - error upon converting between encodings
                errInvalidHex_els98           - error upon converting between encodings
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls08_FillLong(tls00_Buffer         &LongDataBuffer,
              tsp00_LongDescriptor *pLongDescriptor,
              tls00_ParamInfo      &ParamInfo,
              tls00_ParamInfo_LONG &LCI,
              bool                  ExternalValueIsHex,
              bool                  bUnicodeOI,
              tsp81_CodePage       *pCodePage,
              tls00_Buffer2        *pConversionBuffer,
              tsp00_Addr            pszErrText);

// wenn lFrom = -1 dann wird leerer string gefordert, muss im caller geprueft werden
// oder besser: in diesem fall ist lfrom = lto = 0 ??? kann das gehen ???
/*
  -----------------------------------------------------------------------------
  function:     ls08_CheckPositionSpecification
  -----------------------------------------------------------------------------
*/
static int
ls08_CheckPositionSpecification(tsp00_Addr         pszString,
                                tls00_LargeNumber &lFromValue,
                                tsp00_Longint     &lLength,
                                SAPDB_Int8        &FileOffset);


/*
  -----------------------------------------------------------------------------
  function:     ls08_FillDataBufferForPutvalRequest
  -----------------------------------------------------------------------------
  description:  Fills data buffer for putval request packet. Used during
                async communication with alternating buffers.

  arguments:    

  returnvalue:  errOK_els00
                bLastPutVal = true if descriptor with last-putval could be send
                                   in last sent packet
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls08_FillDataBufferForPutvalRequest(tls00_MultColSpec*       pMultColSpec,
                                    tls00_OI_DataPartCache*  pDPC,
                                    tls00_TransformationRTE* pTransformationRTE,
                                    bool                     bIsDBUnicode,
                                    bool &                   bLastPutVal,
                                    tsp00_Addr               pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls08_FillPutvalRequestPacket
  -----------------------------------------------------------------------------
  description:  Fills putval request packet. Used during async communication with
                alternating buffers.

  arguments:    

  returnvalue:  
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4 
ls08_FillPutvalRequestPacket(tls00_DBInfo*            pDBInfo,
                             SAPDB_Int4               lSndPcktNo,
                             tls00_OI_DataPartCache*  pDPC,
                             tsp1_part_attributes*    pPartAttributes,
                             tsp00_LongDescriptor*    pDescriptorArray,     // starting with the right start member?
                             tsp00_Addr               pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls08_ReceivePacket
  -----------------------------------------------------------------------------
  description:  Receives answer to putval request packet. Used during async
                communication with alternating buffers.

  arguments:    

  returnvalue:  
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4 
ls08_ReceivePacket(tls00_DBInfo*          pDBInfo,
                   SAPDB_Int4             lRcvPcktNo,
                   tls00_LongExecInfo*    pLongInfo,
                   tsp00_LongDescriptor*  pDescriptorArray,      // starting with the right start member?
                   SAPDB_Int2             lStartMemberInLongDescriptorArray,
                   tsp00_Addr             pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls08_ClosePutVal
  -----------------------------------------------------------------------------
  description:  Close open longs if those were inserted using PUTVAL

  arguments:    pDBInfo         [IN]    - structure with info on running session
                                           and database connected to
                PartAttribute   [IN]    - special part attribute for long values
                pLongDescriptor [IN]    - long value descriptor
                pszErrText      [OUT]   - error text

  returnvalue:  errOK_els00                     - in case of NO errors
                errProcessLongsWithPutVal_els98 - closing failed by any reason
                The error text is given back in pszErrText.
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls08_ClosePutVal(tls00_DBInfo         *pDBInfo,
                 tsp1_part_attributes  PartAttribute,
                 tsp00_LongDescriptor *pLongDescriptor,
                 tsp00_Addr            pszErrText);



//*******  P U B L I C   F U N C T I O N S   I M P L E M E N T A T I O N  ********

/*
  -----------------------------------------------------------------------------
  function:     ls08InitLongFileInfo
  -----------------------------------------------------------------------------
*/
void
ls08InitLongFileInfo(tls00_MultColSpec *pMultColSpec, tls00_MultFileSpec *pMultLongFileSpec)
{
    ROUTINE_DBG_MEO00 ("ls08InitLongFileInfo");

    int i = 0;
    int j = 0;

    // Assigning LONFFILE Info's from command string
    //<longfile_spec_mlt> ::= [<longfile_spec>][<longfile_spec_mlt>]
    for (i = 0; i < pMultLongFileSpec->mfsCount_ls00; ++i)
    {
        tls00_FileSpec *pSingleFileSpec = pMultLongFileSpec->mfsFiles_ls00[i];

        if (NULL != pSingleFileSpec)
        {
            for (j = 0; j < pMultColSpec->mcsCount_ls00; ++j)
            {
                tls00_Column *pSingleColumn = pMultColSpec->mcsColumn_ls00[j];

                //<longfile_spec> ::=
                // LONGFILE <valCOLUMN_ID> <valFILE_NAME> <encoding> |
                // LONGFILE <valCOLUMN_ID> <encoding> |
                if (UNDEFINED_LS00 != pSingleFileSpec->fsLongColID_ls00)      // assign column number
                {
                    if (pSingleFileSpec->fsLongColID_ls00 == pSingleColumn->colFPos_ls00.fpoStart_ls00)
                    {
                        if (NULL != pSingleFileSpec->fsFileName_ls00)
                        {
                            pSingleColumn->colLongInfo.loiWithLongInfo_ls00 = true;
                            pSingleColumn->colLongInfo.loiLongFileName_ls00 = pSingleFileSpec->fsFileName_ls00;
                        }

                        // Define file encoding
                        pSingleColumn->colLongInfo.loiFileEncoding_ls00 = pSingleFileSpec->fsFormat_ls00.ffoCodeType_ls00;
  
                        break;
                    }
                }

                //<longfile_spec> ::=
                // LONGFILE <valCOLUMN_NAME> <valFILE_NAME> <encoding> |
                // LONGFILE <valCOLUMN_NAME> <encoding>
                if (NULL != pSingleFileSpec->fsLongColName_ls00)            // assign file name
                {
                    if(0 == strcmp(pSingleFileSpec->fsLongColName_ls00, pSingleColumn->colName_ls00.asCharp()))
                    {
                        if (NULL != pSingleFileSpec->fsFileName_ls00)
                        {
                            pSingleColumn->colLongInfo.loiWithLongInfo_ls00 = true;
                            pSingleColumn->colLongInfo.loiLongFileName_ls00 = pSingleFileSpec->fsFileName_ls00;
                        }

                        // Define file encoding
                        pSingleColumn->colLongInfo.loiFileEncoding_ls00 = pSingleFileSpec->fsFormat_ls00.ffoCodeType_ls00;
                        
                        break;
                    }
                }
            }
        }
    }//  for (i = 0; i < pMultLongFileSpec->mfsCount_ls00; ++i)
    
    return;
}
// ls08InitLongFileInfo()


/*
  -----------------------------------------------------------------------------
  function:     ls08ReadLongFileInfo
  -----------------------------------------------------------------------------
*/
int
ls08ReadLongFileInfo(tls00_Column    *pField,
                     tls00_ParamInfo *pColumnParameters,
                     tls00_String    *pValue,
                     tsp00_Addr       pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08ReadLongFileInfo");

    // We don't get in here if the value represents the NULL value
    tsp00_Int4 rc = errOK_els00;

    tls00_ParamInfo_LONG &pLCI  = pField->colLongInfo;
    SAPDB_Int8            FileOffsetAsInt8 = 0;

    if (NULL == pValue->strAddr_ls00)        // indicates compact format -> better using fileformat as a parameter?
    {
        // Initialize some values
        pLCI.loiLength_ls00          = pValue->strLeng_ls00;
        pLCI.loiPart_ls00            = true;   // Long value is part of the data file
        pLCI.loiFileOffset_ls00      = pLCI.loiFileStruct_ls00->vfFilePos_ls00;     // File Offset in input file
        pLCI.loiCountOfSendData_ls00 = 0L;     // count of already send data (to kernel)
    }
    else
    {
        // Initialize some values
        pLCI.loiLength_ls00          = 0L;                      // 0: unknown length-> LONG value = whole file content
        pLCI.loiPart_ls00            = false;                   // Long value = whole file content

        pLCI.loiFileOffset_ls00.lnu2GB_Blocks_ls00  = 0L;
        pLCI.loiFileOffset_ls00.lnuRemainder_ls00   = 0L;
        pLCI.loiCountOfSendData_ls00                = 0L;       // count of already send data (to kernel)

        // Because we use c-func atol we need to make the string null terminated
        tsp00_Longint lLongSpecLen = pValue->strLeng_ls00;
        _TCHAR       *pszLongSpec  = new _TCHAR[lLongSpecLen + 1];
        SAPDB_memcpy(pszLongSpec, pValue->strAddr_ls00, lLongSpecLen);
        pszLongSpec[lLongSpecLen] = 0;

        if (true == pField->colLongInfo.loiWithLongInfo_ls00)
        {
            //*
            //* LONGFILE Info assigned to column already in command
            //*
            pLCI.loiPart_ls00 = true;                           // Long value is only a part of the whole file

            // Analyze LONGINFO parameter in data
            rc = ls08_CheckPositionSpecification(pszLongSpec, pLCI.loiFileOffset_ls00, pLCI.loiLength_ls00, FileOffsetAsInt8);
        }
        else
        {
            // checking for specs like
            // '<file name>' <from position>-<to position>
            //  where the single quotes and the positions are optional
            rc = ls30VFClose(*pLCI.loiFileStruct_ls00, pszErrText);     // closing LONGFILE
            if (errOK_els00 != rc)
            {
                return rc;
            }

            if (NULL != pLCI.loiLongFileName_ls00)                      // reinitialize long data file name
            {
                delete [] pLCI.loiLongFileName_ls00;
                pLCI.loiLongFileName_ls00 = NULL;
            }

            ls05StrTrimRightEx(pszLongSpec);                            // trim string from left and right
            ls05StrTrimLeftEx(pszLongSpec, lLongSpecLen);

            _TCHAR *pszWorkPtr = pszLongSpec;
            _TCHAR *pDest      = NULL;
        
            if ('\'' == pszWorkPtr[0])
            {
                ++pszWorkPtr;                                           // skip first quote in file name (for later copying the name)

                pDest = strchr(pszWorkPtr, '\'');
                if ( (pDest - pszWorkPtr) == (lLongSpecLen - 2) )       // -2 because we moved ptr pszWorkPtr by 1
                {
                    // no positions specified
                    lLongSpecLen -= 2;
                }
                else
                {
                    lLongSpecLen = pDest - pszWorkPtr;          // adapt length for copying file name later on

                    // some more specs in string; find positions; starting point is pDest++
                    ++pDest;
                    rc = ls08_CheckPositionSpecification(pDest, pLCI.loiFileOffset_ls00, pLCI.loiLength_ls00, FileOffsetAsInt8);
                    if (errOK_els00 == rc)
                    {
                        pLCI.loiPart_ls00 = true;    // Long value is only a part of the whole file
                    }
                }
            }

            if (errOK_els00 == rc)
            {
                pLCI.loiLongFileName_ls00 = new _TCHAR[lLongSpecLen + 1];
                SAPDB_memcpy(pLCI.loiLongFileName_ls00, pszWorkPtr, lLongSpecLen);
                pLCI.loiLongFileName_ls00[lLongSpecLen] = 0;
            }

            pszWorkPtr = NULL;
        }   // end if (true == pField->colLongInfo.loiWithLongInfo_ls00)


        //*
        //* Everything is analyzed --> open the file and seek to the right position
        //*
        if (errOK_els00 == rc)
        {
            bool         _bEmptyLongFile = false;
            tls00_VFile* pLongFile       = pLCI.loiFileStruct_ls00;

            if ( (0  == pLongFile->vfFileHndl_ls00)  &&                     // LONGFILE already open ?
                 (-1 != pLCI.loiFileOffset_ls00.lnuRemainder_ls00)  )       // insert empty string --> no file to open
            {
                ls30VFInitialize(*pLongFile,                                // Init file as binary
                                 pLCI.loiLongFileName_ls00,
                                 sp5vf_binary, sp5vf_read, pLCI.loiFileEncoding_ls00);

                rc = ls30VFOpen(*pLongFile, pszErrText);                    // open file
                if (errOK_els00 == rc)
                {
                    if ( (0 == pLongFile->vfFileLen_ls00.lnu2GB_Blocks_ls00) &&
                         (0 == pLongFile->vfFileLen_ls00.lnuRemainder_ls00) )
                    {
                        // File is empty --> special treatment for LONG values: this indicates to insert empty string
                        pLCI.loiFileOffset_ls00.lnuRemainder_ls00 = 0L;
                        _bEmptyLongFile = true;
                    }
                    else
                    {
                        rc = ls30Detect_BinaryFile_Encoding(*pLongFile, pszErrText);
                        if (errOK_els00 == rc)
                        {
                            // Check if file contains anything else than the magic bytes; those are
                            // only found in UNICODE files
                            if ( (true         == ls00IsUCS2(&pLongFile->vfFileEncoding_ls00)) ||
                                 (ctUTF8_els00 == pLongFile->vfFileEncoding_ls00) )
                            {
                                if (0 == pLongFile->vfFileLen_ls00.lnu2GB_Blocks_ls00)
                                {
                                    if (pLongFile->vfFileLen_ls00.lnuRemainder_ls00 ==
                                        pLongFile->vfMagicBytesCache_ls00.lBufPos_ls00)
                                    {
                                        // File is empty --> special treatment for LONG values: this
                                        //                   indicates to insert empty string
                                        pLCI.loiFileOffset_ls00.lnuRemainder_ls00 = 0L;
                                        _bEmptyLongFile = true;
                                    }
                                }
                            }
                            if ( (false == _bEmptyLongFile)  &&
                                 (0     == FileOffsetAsInt8) &&
                                 (0     <  pLongFile->vfFilePos_ls00.lnuRemainder_ls00) )
                            {
                                pLCI.loiFileOffset_ls00.lnuRemainder_ls00 =
                                    pLongFile->vfFilePos_ls00.lnuRemainder_ls00;
                            }
                        }
                    }   // end else of if ( (0 == pLongFile->vfFileLen_ls00.lnu2GB_Blocks_ls00) && ...
                }
                if ( (errOK_els00 == rc) && (false == _bEmptyLongFile) )
                {
                    rc = ls08CheckLONG_DT_Compatibility(pLongFile->vfFileEncoding_ls00, 
                                                        pColumnParameters->piBasicInfo_ls00.fbDataType_ls00);
                    if (errOK_els00 == rc)
                    {
                        ls30VFSeekLong(*pLongFile, &pLCI.loiFileOffset_ls00, pszErrText);
                    }
                    else
                    {
                        // error: data types do not match (function returns only ok or errIncompatibleDataTypes_els98)
                        LoaderMessages::Instance()->ls98Msg(layDataLoad_els98, pszErrText,
                                                                               errIncompatibleDataTypes_els98,
                                                                               pField->colName_ls00.asCharp());
                    }
                }


                // In case that the LONG value to load is a part of the LONG file we have to check
                // the read position (file offset) to decide whether the MagicBytesCache should be emptied.
                if ( (true == pLCI.loiPart_ls00) &&
                     (false == _bEmptyLongFile)  &&
                     (false == ls30VFIsMagicBytesCacheEmpty(pLCI.loiFileStruct_ls00)) &&
                     (FileOffsetAsInt8 > STATIC_CAST(SAPDB_Int8, pLCI.loiFileStruct_ls00->vfMagicBytesCache_ls00.lBufLen_ls00) ) )
                {
                    ls30VFEmptyMagicBytesCache(pLCI.loiFileStruct_ls00, true);
                }

            }   // end if ( (0  == pLCI.loiFileStruct_ls00->vfFileHndl_ls00)  && ...
            else
            {
                if ( (pLongFile->vfFilePos_ls00.lnuRemainder_ls00 == pLongFile->vfMagicBytesCache_ls00.lBufLen_ls00) &&
                     (FileOffsetAsInt8 < STATIC_CAST(SAPDB_Int8, pLongFile->vfMagicBytesCache_ls00.lBufLen_ls00) ) )
                {
                    pLCI.loiFileOffset_ls00.lnuRemainder_ls00 = pLongFile->vfFilePos_ls00.lnuRemainder_ls00;
                }
            }

            if ( (false == pLCI.loiPart_ls00) && (false == _bEmptyLongFile) )    // LONG VALUE = whole content of data file?
            {
                if (0 == pLongFile->vfFileLen_ls00.lnu2GB_Blocks_ls00)
                {
                    pLCI.loiLength_ls00 = pLongFile->vfFileLen_ls00.lnuRemainder_ls00;
                }
                else
                {
                    // Not sure about size of file but surely bigger as packet size (if blocks > 0: filelen > 2GB)
                    pLCI.loiLength_ls00 = MAX_INT4_SP00;
                }
            }
        }       // end if (errOK_els00 == rc)

        if (NULL != pszLongSpec)
        {
            delete [] pszLongSpec;
            pszLongSpec = NULL;
        }
    }   // end if (NULL == pValue->strAddr_ls00)

    return rc;
}
// ls08ReadLongFileInfo()


/*
  -----------------------------------------------------------------------------
  function:     ls08InsertLongs
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls08InsertLongs(tls00_MultColSpec*        pMultCol,
                tls00_TransformationRTE*  pTransformationRTE,
                tls00_OI_DataPartCache*   pDataPartCache,
                bool &                    bPutVal,
                bool                      bUnicodeOI,
                tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08InsertLongs");

    tsp00_Longint rc = errOK_els00;

    bPutVal = true;     // Initialize to 'No Space Left In Buffer For LONG Data'

    tsp00_LongDescriptor  LongDescriptor;
    tls00_LongExecInfo*   pLongInfo      = &pTransformationRTE->rte_LongExecInfo_ls00;
    tls00_DataSource*     pDataSource    = &pTransformationRTE->rte_DataSource_ls00;
    tls00_Buffer &        LongDataBuffer = pDataPartCache->dpc_Buffer_ls00;
    tls00_ParamInfo*      pPI            = NULL;
    tsp00_Int2            Index          = 0;
    unsigned char*        pszRecord      =
                            &(STATIC_CAST(unsigned char*, LongDataBuffer.pBufPtr_ls00)[LongDataBuffer.lBufLen_ls00 -
                                                                                       pDataPartCache->dpc_RecordLength_ls00]);
    
    if (LongDataBuffer.lBufSize_ls00 > 0)
    {
        bPutVal = false;     // Reinitialize to 'All LONG data Fits Into Buffer'

        tls00_ParamInfo_LONG *pLongColInfo = NULL;
        tsp00_Int2            i            = pLongInfo->dliFinishedNNLongs_ls00;

        //*
        //*     Something to 'recover'
        //*
        //*
        //* We get in here if during insert of values the kernel returned an error for
        //* one of the records sent; in this case we have to refill the packet by moving up
        //* all 'good' records (located in the packet behind the erroneous record);
        //*
        //* in case there are LONG values to be sent there might already be some
        //* of the LONGs for the last 'good' record in the packet but not all (supposing
        //* they didn't fit all together in the data packet and putval packets were actually
        //* necessary); now, after moving the 'good' records we have again some space left
        //* in packet and can put the rest of the LONGs for the last 'good' record into
        //* the packet
        //*

        // To come to the real start of the last 'good' record we have to consider the
        // already inserted LONGs and have to move the pszRecord ptr back by the amount
        // of space occupied by the LONGs
        tsp00_Int4 j = 0;
        for (j; j < pLongInfo->dliFinishedNNLongs_ls00; ++j)
        {
            pszRecord -= pMultCol->mcsColumn_ls00[pLongInfo->dliNNLongs_ls00[j]]->colLongInfo.loiCountOfSendData_ls00;
        }
        pszRecord -= pMultCol->mcsColumn_ls00[pLongInfo->dliNNLongs_ls00[j]]->colLongInfo.loiCountOfSendData_ls00;


        //*
        //*     Fill the LONG values in
        //*
        for (i; ( (rc >= 0) && (i < pLongInfo->dliCntNNLongs_ls00) && (false == bPutVal) ); ++i)
        {
            Index        = pLongInfo->dliNNLongs_ls00[i];
            pLongColInfo = &pMultCol->mcsColumn_ls00[pDataSource->plDataToColIndex_ls00[Index]]->colLongInfo;
            pPI          = pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[pDataSource->plDataToParamIndex_ls00[Index]];

            // descriptor already initially filled in in ls20BuildRecord (this descriptor is ok for
            // NULL or empty values);
            // in case of real LONG values to be filled we'll save the descriptor here for reinialization
            // (to be sure not to write to odd adresses) during filling
            SAPDB_memcpy(REINTERPRET_CAST(char*, &LongDescriptor), pszRecord + pPI->piBufpos_ls00, LongDescSize_ls00);

            // In case of 'recovery' (see explanation above) the valpos is already set to the
            // right value (and thus not 0)
            if (0 == LongDescriptor.ld_valpos())
            {   // Write valpos (in PASCAL notation)
                LongDescriptor.ld_valpos() = STATIC_CAST(tsp00_Int4, (LongDataBuffer.lBufLen_ls00 + 1));
            }

            // Fill long data into buffer
            rc = ls08_FillLong(LongDataBuffer,
                               &LongDescriptor,
                               *pPI,
                               *pLongColInfo,
                               pMultCol->mcsColumn_ls00[Index]->colFormat_ls00.ffoIsHex_ls00,
                               bUnicodeOI,
                               pDataSource->pdasCodePage_ls00,
                               &pLongInfo->dliConversionBuffer_ls00,
                               pszErrText);
            if (rc >= 0)
            {
                // Write long descriptor to buffer
                SAPDB_memcpy(pszRecord + pPI->piBufpos_ls00, REINTERPRET_CAST(char*, &LongDescriptor), LongDescSize_ls00);

                // ls08_FillLong sets only ld_valmode either to vm_alldata, vm_lastdata or to vm_datapart
                if (vm_datapart == LongDescriptor.ld_valmode())
                {
                    bPutVal = true;      // stop if LONG could not completely be inserted into buffer
                    ++i;                 // increase counter to set all other longs in record to no_data
                }
                else
                {
                    ++pLongInfo->dliFinishedNNLongs_ls00;
                }
            }
        } // end  for (i; ( (i < pLongInfo->dliCntNNLongs_ls00) && (false == PutValFlag) ); i++)
    }

    //*
    //*     Everything ok at this point?
    //* all remaining (even NOT NULL LONG columns in this record) must have
    //* been set to vm_nodata, this is already done in ls20ValueToOIBuffer
    //*
    return STATIC_CAST(tsp00_Int4, rc);
}
// ls08InsertLongs()


/*
  -----------------------------------------------------------------------------
  function:     ls08ExecPutVal
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls08ExecPutVal(tls00_DBInfo*             pDBInfo,
               tls00_MultColSpec*        pMultColSpec,
               tls00_TransformationRTE*  pTransformationRTE,
               tls00_AlternatingBuffers* pAlternatingBuffers,
               tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08ExecPutVal");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // error msg obj
    tsp00_Longint   rc      = errOK_els00;

    bool                   _bIsDBUnicode   = ls00IsDBUnicode(pDBInfo);
    
    tls00_Column*          _pColumn        = NULL;
    tls00_LongExecInfo*    pLongInfo       = &pTransformationRTE->rte_LongExecInfo_ls00;
    tsp00_LongDescriptor*  pLongDescArray  = pTransformationRTE->rte_pLongDescriptorContainer_ls00;

    // Used for initialization during while-loop
    tsp00_Longint  lBufferLength   = pAlternatingBuffers->ab_pCache_ls00[0].dpc_Buffer_ls00.lBufLen_ls00 +
                                      pAlternatingBuffers->ab_pCache_ls00[0].dpc_Buffer_ls00.lBufSize_ls00;


    // At the first time get all descriptors for LONG values which need to be send using putval.
    //
    // In case there are no descriptors delivered by the kernel (which may only happen for DATAUPDATE
    // commands without treating it as an error) we simply return ok. 
    // Executing putval means processing of long values of a single record. Only for this record
    // long value descriptors are returned.
    //
    // The descriptor array and the array of NOT NULL LONG column indexes are correlated with each other.
    // Suppose we have to process 3 LONGS --> 2 cases:
    //
    // 1. case: first LONG has been completely sent at insert time
    // - 3 LONGs --> dliCntNNLongs_ls00 = 3
    // - dliFinishedNNLongs_ls00 = 1
    // - we need to fill array member 1 and 2
    // - --> dliCntNNLongs_ls00 - dliFinishedNNLongs_ls00 = 2: 2 values starting with 1 = dliFinishedNNLongs_ls00
    //
    // 2. case: first LONG has not been completely sent at insert time
    // - 3 long-werte --> dliCntNNLongs_ls00 = 3
    // - dliFinishedNNLongs_ls00 = 0
    // - we need to fill array member 1, 2 and 3
    // - --> dliCntNNLongs_ls00 - dliFinishedNNLongs_ls00 = 3: 3 values starting at 0 = dliFinishedNNLongs_ls00
    //
    
    // First we have to use the latest received packet (in data load loop in ls20-modul) that contains the long descriptors
    // This package is the one with pAlternatingBuffers->ab_lRcvPcktNo_ls20
    rc = ls04GetLONGDescriptor(pDBInfo->dbiPktRcvList_ls00[pAlternatingBuffers->ab_lRcvPcktNo_ls00],
                               pLongInfo->dliCntNNLongs_ls00 - pLongInfo->dliFinishedNNLongs_ls00,
                               pLongDescArray,
                               pLongInfo->dliFinishedNNLongs_ls00);
    if (errOK_els00 != rc)  // the only possible value is: errUnknownPart_els98
    {
        rc = errOK_els00;
    }
    else
    {
        char        _szErr[MAX_REPLY_LENGTH_LS00];
        bool        _bLastPutVal = false;       // flags if descriptor with last-putval is in the built packet
        SAPDB_Int4  _lErrorPos   = 0;
        SAPDB_Int4  _lSndPcktNo  = 0;
        SAPDB_Int4  _lRcvPcktNo  = 0;
        SAPDB_Int2  _lLongs_Finished[2];

		
		_lLongs_Finished[0] = 0;
		// Initialize the number of finished LONGs of the 2nd packet with the number of already finished LONGs
		// (coming from processing the primary data). This is necessary as we have to update the LONG descriptors
		// in every packet to send with the LONG descriptors returned by the kernel in the predecessor packet. As
		// the first packet to send does (naturally) not have a predecessor we prepare the number as if it had one.
		_lLongs_Finished[1] = pLongInfo->dliFinishedNNLongs_ls00;	


        //*
        //*     Main loop to asynchronously send the LONG data using 2 alternating buffers
        //*
        while ( (rc >= 0) && (pLongInfo->dliFinishedNNLongs_ls00 != pLongInfo->dliCntNNLongs_ls00) )
        {
            // Initialize (MISUSED here) counter for LONGs in packet
            pAlternatingBuffers->ab_pCache_ls00[_lSndPcktNo].dpc_lRowCount_ls00 = 0;

            // To be able to use the LONG data buffer (allocated once at the beginning of dataload) we
            // accept to waste some Bytes in order interface packet - in case of a 32 kB order
            // interface buffer it is exactly 56 bytes 

            // Make sure the buffer is initialized!
            pAlternatingBuffers->ab_pCache_ls00[_lSndPcktNo].dpc_Buffer_ls00.lBufSize_ls00 = lBufferLength;
            pAlternatingBuffers->ab_pCache_ls00[_lSndPcktNo].dpc_Buffer_ls00.lBufLen_ls00 = 0;

            rc = ls08_FillDataBufferForPutvalRequest(pMultColSpec,
                                                     &pAlternatingBuffers->ab_pCache_ls00[_lSndPcktNo],
                                                     pTransformationRTE,
                                                     _bIsDBUnicode,
                                                     _bLastPutVal,
                                                     pszErrText);
            if ( (0 == pAlternatingBuffers->ab_pCache_ls00[_lSndPcktNo].dpc_lRowCount_ls00) || (rc < 0) )
            {   // LONG Info's inserted? Errors?
                break;                  // Putval's finished; rc on errOK_els00 or on error
            }

            // ATTENTION!
            // We have to store the count of finished LONG values  S E PA R A T E L Y  for every packet that is processed:
            _lLongs_Finished[_lSndPcktNo] = pLongInfo->dliFinishedNNLongs_ls00;

            //*
            //*     Get a receive (answer) packet from kernel
            //*
            if (_lSndPcktNo != _lRcvPcktNo)
            {
                rc = ls08_ReceivePacket(pDBInfo,
                                        _lRcvPcktNo,
                                        pLongInfo,
                                        pLongDescArray,
                                        _lLongs_Finished[_lRcvPcktNo],
                                        pszErrText);
                _lRcvPcktNo = (_lRcvPcktNo + 1) % 2;
            }

            //*
            //*     Fill and send a request packet to the kernel
            //*
            if (errOK_els00 == rc)
            {
			    // Insert data and count of data into request
                rc = ls08_FillPutvalRequestPacket(pDBInfo,
                                                  _lSndPcktNo,
                                                  &pAlternatingBuffers->ab_pCache_ls00[_lSndPcktNo],
                                                  &pTransformationRTE->rte_PartAttr_ls00,
                                                  &pLongDescArray[_lLongs_Finished[(_lRcvPcktNo + 1) % 2]],
			                                      pszErrText);
                if (errOK_els00 == rc)
                {
                    // Send the request packet
                    _szErr[0] = 0;
                    rc = ls03SQLRequest(pDBInfo, _lSndPcktNo, _szErr);
                    if (errOK_els00 == rc)
                    {
                        _lSndPcktNo = (_lSndPcktNo + 1) % 2;  //  successful sent: recalculate packet number

                        // Adapt LONG descriptors only if it is not the last piece of that LONG value
                        for (SAPDB_Int2 i = pLongInfo->dliFinishedNNLongs_ls00; i < pLongInfo->dliCntNNLongs_ls00; ++i)
                        {
                            if ( (vm_alldata  != pLongDescArray[i].ld_valmode()) &&
                                 (vm_lastdata != pLongDescArray[i].ld_valmode()) )
                            {
                                pLongDescArray[i].ld_intern_pos() += pLongDescArray[i].ld_vallen();
                                pLongDescArray[i].ld_vallen()      = 0;
                            }
                        }
                    }
                }   // end if (errOK_els00 == rc)
            }

        }   // end while ( (rc >= 0) && (pLongInfo->dliFinishedNNLongs_ls00 != pLongInfo->dliCntNNLongs_ls00) )
        //*
        //* End of: Main loop to asynchronously send the LONG data using 2 alternating buffers
        //*

        //*
        //*     while-loop for sending packets is obviously done. But may be there is a last answer
        //*     packet that has to be receive prior to continuing processing
        //*
        if ( (errOK_els00 == rc) && (_lSndPcktNo != _lRcvPcktNo) )
        {
                rc = ls08_ReceivePacket(pDBInfo,
                                        _lRcvPcktNo,
                                        pLongInfo,
                                        pLongDescArray,
                                        pLongInfo->dliFinishedNNLongs_ls00,
                                        pszErrText);
        }


        // If so far no descriptor with last_putval could be sent we need to send an extra packet
        // with only this descriptor.
        if ( (errOK_els00 == rc) && (false == _bLastPutVal) )
        {
            _szErr[0] = 0;
            rc = ls08_ClosePutVal(pDBInfo, pTransformationRTE->rte_PartAttr_ls00, &pLongDescArray[0], _szErr);
            if (errOK_els00 != rc)
            {
                pMsgObj->ls98Msg(layDataLoad_els98, pszErrText, errProcessLongsWithPutVal_els98, _szErr);
            }
        }
    }   // end else of if (errOK_els00 != rc (= ls04GetLONGDescriptor(pDBInfo->dbiPktRcvList_ls00[...))


    // Reinitialize buffer
    pAlternatingBuffers->ab_pCache_ls00[0].dpc_Buffer_ls00.lBufSize_ls00 = lBufferLength;
    pAlternatingBuffers->ab_pCache_ls00[0].dpc_Buffer_ls00.lBufLen_ls00 = 0;
    pAlternatingBuffers->ab_pCache_ls00[1].dpc_Buffer_ls00.lBufSize_ls00 = lBufferLength;
    pAlternatingBuffers->ab_pCache_ls00[1].dpc_Buffer_ls00.lBufLen_ls00 = 0;

    // Reset values
    pAlternatingBuffers->ab_pCache_ls00[0].dpc_lRowCount_ls00 = 0;
    pAlternatingBuffers->ab_pCache_ls00[1].dpc_lRowCount_ls00 = 0;

    return STATIC_CAST(tsp00_Int4, rc);
}
// ls08ExecPutVal()



/*
  -----------------------------------------------------------------------------
  function:     ls08_ClosePutVal
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls08_ClosePutVal(tls00_DBInfo         *pDBInfo,
                 tsp1_part_attributes  PartAttribute,
                 tsp00_LongDescriptor *pLongDescriptor,
                 tsp00_Addr            pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08ClosePutVal");

    tsp00_Int4    rc            = errOK_els00;
    tsp1_packet  *pSqlReqPacket = NULL;
    tsp1_segment *pSqlSegm      = NULL;
    tsp1_part    *pSqlPart      = NULL;
    
    // Generating new PUTVAL-command
    pSqlReqPacket = pDBInfo->dbiPktSndList_ls00[0];

    rc = ls04FastInit(pSqlReqPacket, &pSqlSegm, &pSqlPart, pDBInfo);    // fast packet initialization
    ls04SetMsgType(pSqlSegm, sp1m_putval);                              // message type 'PUTVAL'
    pSqlPart->sp1p_part_kind().becomes(sp1pk_longdata);

    ls04SetPartAttribute(pSqlPart, PartAttribute);                      // set part attribut

    pSqlPart->sp1p_arg_count() = 1;

    //*
    //*  Define some LONG Descriptor values
    //*
    rc = ls04CharToPart(pSqlPart, DEF_BYTE_ASCII_LS00);                 // write defined byte
    if (errOK_els00 == rc)
    {
        pLongDescriptor->ld_valmode().becomes(vm_last_putval);
        rc = ls04BufToPart(pSqlPart, REINTERPRET_CAST(char*, pLongDescriptor), LongDescSize_ls00);  // write long descriptor
        if (errOK_els00 == rc)                                                                      // into packet
        {
            rc = ls04FinishPart(pSqlReqPacket, pSqlPart);
            if (errOK_els00 == rc)
            {
                tsp00_Int4 lErrPos = 0;
                rc = ls03ProcessSQLPacket(pDBInfo, 0, lErrPos, pszErrText);        // Send, receive and check SQL packet
                if (errOK_els00 != rc)
                {
                    rc = errProcessLongsWithPutVal_els98;
                }
            }
        }
    }

    return rc;
}
//  ls08_ClosePutVal()


/*
  -----------------------------------------------------------------------------
  function:     ls08ExtractLongs
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls08ExtractLongs(tls00_DBInfo           *pDBInfo,
                 tsp00_Addr              pszInputBuffer,    // source buffer
                 tls00_MultColSpec      *pMultColumn,       // user delivered column description
                 tls00_ColumnData       *pColumnData,       // array of structures
                 tls00_LongExecInfo     *pLongInfo,
                 tsp00_LongDescriptor   *pLongDescArray,
                 bool                   &bUseGetVal,
                 tls00_Buffer2          *pConvertBuffer,
                 tsp00_Addr              pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08ExtractLongs");

    tsp00_Longint rc = errOK_els00;

    // Parameters for a better handling
    tls00_ParamInfo_LONG   *pLParam        = NULL;

    tls00_ColumnData       *pData          = NULL;
    tsp00_LongDescriptor   *pLDesc         = NULL;
    tsp00_Int4              lPtrLen        = 0;
    tsp00_Longuint          lDestLen       = 0;     // used for conversion

    // Parameters for pointing to and writing the (converted - in case of UNICODE DB) data 
    tsp00_Int4              myValPos       = 0;
    tsp00_Longint           myValLen       = 0;
    tsp00_Addr              pszValueBuffer = NULL;
    tsp00_Addr              pszWriteBuffer = NULL;
    Tools_DynamicUTF8String UTF8ConvertBuffer;
    unsigned char*          pszConvertBuffer2 = NULL;


    // Parameter used to process unicode data
    bool                    bUnicodeOI     = ls00UseUnicodeOI(pDBInfo);  // inline function

    //*
    //*     Start processing LONGs
    //*
    for (tsp00_Int4 i = 0, j = 0; ( (errOK_els00 == rc) && (i < pLongInfo->dliCntNNLongs_ls00) ); ++i)
    {
        j                     = pLongInfo->dliNNLongs_ls00[i];
        pData                 = &pColumnData[j];
        pLParam               = &pMultColumn->mcsColumn_ls00[j]->colLongInfo;

        pData->cdLONGLen_ls00 = 0;
        pLDesc                = &pLongDescArray[i];

        if (false == bUseGetVal)
        {
            pszValueBuffer = pszInputBuffer;    // Initialize pointer to data in delivered fetch paket
        }
        else
        {
            if ( (vm_lastdata == pLDesc->ld_valmode()) || (vm_startpos_invalid == pLDesc->ld_valmode()) )
            {
                // Seems the whole data is delivered when requesting the length of the LONG;
                // In this case
                //  either
                // - the kernel has set valmode to vm_lastdata that is not modified by the Loader if it
                //   is the only long in a table row or the first one
                //  or
                // - upon requesting the LONGs over GETVAL in the while loop 
                // - the max length and vallen is lower than 3 as the Loader requests 2 Bytes when
                //   defining the maximal length of that LONG in ls27_GetLongLength
                if ( (pLDesc->ld_maxlen() > 0) && (pLDesc->ld_maxlen() < 3) && 
                     (pLDesc->ld_vallen() >= 0) && (pLDesc->ld_vallen() < 3) )
                {
                    pLDesc->ld_valmode().becomes(vm_datapart);
                    pLDesc->ld_intern_pos() = 1;
                    pLDesc->ld_vallen()     = 0;
                }
            }
        }

        // In case there is no long data in fetch paket we have to get them with getval-pakets

        //*
        //*     Open long data file
        //*
        //* In case of compact file format we use the data file as output for longs, too. So
        //* no opening is necessary here.
        if (true == pLParam->loibFileNumbered_ls00)
        {
            // Closing a not open file does not produce an error
            ls30VFClose(*pLParam->loiFileStruct_ls00, pszErrText);

            size_t spos = _tcslen(pLParam->loiFileName_ls00) - pLParam->loiNumberSize_ls00;
            tsp00_Int4 lNumber = atoi(&pLParam->loiFileName_ls00[spos]) + 1;
            if (lNumber > pLParam->loiGreatestNum_ls00)
            {
                rc = errLongFileNumGTMax_els98;
            }
            else
            {
                sp77sprintf(&pLParam->loiFileName_ls00[spos], STATIC_CAST(int, pLParam->loiNumberSize_ls00 + 1),
                                                              "%.*d", pLParam->loiNumberSize_ls00, lNumber);

                // ATTENTION: Name of the file will be adapted here
                ls30VFInitialize(*pLParam->loiFileStruct_ls00, pLParam->loiFileName_ls00,
                                 sp5vf_binary, sp5vf_write,
                                 pData->cdExtLONGEncoding_ls00);

                rc = ls30VFOpen(*pLParam->loiFileStruct_ls00, pszErrText);    // Open LONG data file; func write msg in case of error
                if (errOK_els00 == rc)
                {
                    rc = ls30Mark_BinaryFile_Encoding(*pLParam->loiFileStruct_ls00, pszErrText);
                }

            }
        }

        bool bFinished = false;

        while ( (errOK_els00 == rc) && (false == bFinished) )
        {
            // Possible values for tsp00_ValMode are:
            // vm_datapart, vm_alldata, vm_lastdata, vm_nodata, vm_no_more_data,
            // vm_last_putval, vm_data_trunc, vm_close

            if ( (vm_last_putval  != pLDesc->ld_valmode()) &&       // can't actually be but who knows
                 (vm_no_more_data != pLDesc->ld_valmode()) &&
                 (vm_close        != pLDesc->ld_valmode())  )       // can't be either
            {
                // ld_valmode == vm_nodata --> nothing for this long column in the packet;
                // we need to get the data using getval packets
                if (vm_nodata != pLDesc->ld_valmode())
                {
                    myValPos = pLDesc->ld_valpos() - 1;
                    myValLen = pLDesc->ld_vallen();

                    if (0 != myValLen)          // Empty value? Fine, nothing to write
                    {
                        //*
                        //*     Check if conversion from UCS2 is necessary and convert
                        //* for conversion rules check definition of szEXTRACT_LONG_UNICODE_ls08 at the top of file
                        //*
                        if (true == bUnicodeOI)
                        {
                            switch (szEXTRACT_LONG_UNICODE_ls08[pData->cdExtLONGEncoding_ls00][pData->cdSQLDataType_ls00])
                            {
                                // ATTENTION: double double fall through
                                case 1:     // = 1 - no conversion needed
                                case 4:     // = 4 ucs2 -> ebcdic not longer supported
                                {
                                    pszWriteBuffer = &pszValueBuffer[myValPos];
                                    break;
                                }
                                case 2:     // ucs2 -> ascii
                                case 3:     // ucs2 -> utf8
                                {
                                    pConvertBuffer->lBufPos_ls00 = pConvertBuffer->lBufLen_ls00;
                                    rc = ls05ConvertFromUCS2(REINTERPRET_CAST(unsigned char*, pConvertBuffer->pBufPtr_ls00),
                                                             pConvertBuffer->lBufPos_ls00, // dest buf size is modified here
                                                             pData->cdExtLONGEncoding_ls00,
                                                             REINTERPRET_CAST(unsigned char*, &pszValueBuffer[myValPos]),
                                                             myValLen,
                                                             pData->cdSQLDataType_ls00,
                                                             pszErrText);
                                    if (errOK_els00 == rc)
                                    {   // adapt length of value to write to file and write buffer pointer
                                        pszWriteBuffer = STATIC_CAST(_TCHAR*, pConvertBuffer->pBufPtr_ls00);
                                        myValLen       = pConvertBuffer->lBufPos_ls00;
                                    }
                                    break;
                                }
                                //case 0: conversion forbidden; this is already catched when checking compatibility
                                // at beginning of dataextract command processing 
                            }
                        }
                        else
                        {
                            switch (szEXTRACT_LONG_ASCII_ls08[pData->cdExtLONGEncoding_ls00][pData->cdSQLDataType_ls00])
                            {
                                // ATTENTION: double fall through
                                case 1:     // = 1 - no conversion needed
                                case 4:     // = 4 ucs2 -> ebcdic not longer supported
                                {
                                    pszWriteBuffer = &pszValueBuffer[myValPos];
                                    break;
                                }
                                case 2:     // ascii -> ucs2
                                {
                                    lDestLen = pConvertBuffer->lBufLen_ls00;

                                    rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, pConvertBuffer->pBufPtr_ls00),
                                                                 lDestLen,              // dest size is modified here,
                                                                 REINTERPRET_CAST(unsigned char*, &pszValueBuffer[myValPos]),
                                                                 myValLen,
                                                                 ctAscii_els00,
                                                                 pszErrText,
                                                                 &pDBInfo->dbiSession_ls00.siCodePage_ls00);
                                    if (errOK_els00 == rc)
                                    {   // adapt length of value to write to file and write buffer pointer
                                        pszWriteBuffer = STATIC_CAST(_TCHAR*, pConvertBuffer->pBufPtr_ls00);
                                        myValLen       = pConvertBuffer->lBufPos_ls00 = STATIC_CAST(tsp00_Longint, lDestLen);
                                    }
                                    break;
                                }
                                case 3:     // ascii -> utf8
                                {
                                    rc = ls05ConvertToUTF8(UTF8ConvertBuffer,
                                                           REINTERPRET_CAST(unsigned char*, &pszValueBuffer[myValPos]),
                                                           myValLen,
                                                           ctAscii_els00,
                                                           pszErrText);
                                    if (errOK_els00 == rc)
                                    {   // adapt length of value to write to file and write buffer pointer
                                        pszWriteBuffer = CONST_CAST(char*, (REINTERPRET_CAST(const _TCHAR*, UTF8ConvertBuffer.StrPtr())));
                                        myValLen       = STATIC_CAST(tsp00_Longint, UTF8ConvertBuffer.BasisSize());
                                    }
                                    break;
                                }
                                //case 0: conversion forbidden; this is already catched when checking compatibility
                                // at beginning of dataextract command processing 
                            }
                        }   // end if (true == bUnicodeOI)


                        //*
                        //*     Hexadecimal conversion
                        //*
                        if (true == pData->cdExtHexRepresentation_ls00)
                        {
                            // We need a second conversion buffer, unfortunately
                            if (NULL == pszConvertBuffer2)
                            {
                                pszConvertBuffer2 = new unsigned char[pConvertBuffer->lBufLen_ls00];
                            }

                            ls07ConvertByteToHexEx(pszWriteBuffer,
                                                   myValLen,
                                                   REINTERPRET_CAST(char*, pszConvertBuffer2));

                            pszWriteBuffer = REINTERPRET_CAST(char*, pszConvertBuffer2);
                            myValLen *= 2;

                            //* In case of external encoding type UCS2 further conversion of the hex
                            //* representation to UCS2 hex is necessary;
                            //* In case of external encoding type UTF8 no further conversion needed,
                            //* readable hex representation may only be US7ASCII which is already UTF8

                            if ( true == ls00IsUCS2(&pData->cdExtLONGEncoding_ls00) )
                            {
                                lDestLen = pConvertBuffer->lBufLen_ls00;
                                rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, pConvertBuffer->pBufPtr_ls00),
                                                             lDestLen,              // lDestLen is modified here
                                                             REINTERPRET_CAST(unsigned char*, pszWriteBuffer),
                                                             myValLen,
                                                             ctAscii_els00,
                                                             pszErrText,
                                                             &pDBInfo->dbiSession_ls00.siCodePage_ls00);
                                if (errOK_els00 == rc)
                                {   // adapt length of value to write to file and write buffer pointer
                                    pszWriteBuffer = STATIC_CAST(char*, pConvertBuffer->pBufPtr_ls00);
                                    myValLen       = STATIC_CAST(tsp00_Longint, lDestLen);
                                }
                            }
                        }


                        //*
                        //*     Write LONG to long data file
                        //*
                        if (errOK_els00 == rc)
                        {   // function writes message in case of error
                            rc = ls30VFWriteLn(*pLParam->loiFileStruct_ls00, pszWriteBuffer, STATIC_CAST(tsp00_Int4, myValLen), pszErrText);
                        }
                    }   // end if (0 != myValLen)

                }   // end if (vm_nodata != pLDesc->ld_valmode())

                if (errOK_els00 == rc)
                {
                    pData->cdLONGLen_ls00 += myValLen;

                    if ( (vm_datapart   != pLDesc->ld_valmode()) &&
                         (vm_data_trunc != pLDesc->ld_valmode()) &&
                         (vm_nodata     != pLDesc->ld_valmode()) )
                    {
                        bFinished = true;
                        ++pLongInfo->dliFinishedNNLongs_ls00;
                    }
                    else
                    {
                        rc = ls08ExecGetVal(pDBInfo, pLongInfo, pLongDescArray, pszErrText);
                        if (errOK_els00 == rc)
                        {  
                            // ATTENTION: WE REASSIGN pszValueBuffer HERE
                            lPtrLen = 0;
                            rc = ls04GetRawDataPtr(pDBInfo->dbiPktRcvList_ls00[1], pszValueBuffer, lPtrLen, sp1pk_longdata);
                            bUseGetVal = true;
                        }
                    }
                }
            }
        }   // end of while ( (errOK_els00 == rc) && (false == bFinished) )

        //*
        //*     - adapt data for data file
        //*     - calculate new offset in file
        //* This needs to be done even if the extract of LONGs failed
        if ( (errOK_els00 == rc) && (false == pLParam->loibFileNumbered_ls00) )
        {
            ls07AddLargeNumbers(pLParam->loiFileOffset_ls00, pData->cdLONGLen_ls00); // neuen offset speichern
        }
    }   // end for (i; ( (errOK_els00 == rc) && (i < pLongInfo->dliCntNNLongs_ls00) ); ++i)


    if ( (errOK_els00 == rc) && (true == bUseGetVal) )
    {
        // return code of closegetval doesn't matter
        tsp00_Int4 rc2 = ls08CloseGetVal(pDBInfo, pLongInfo, pLongDescArray, pszErrText);
    }

    pszWriteBuffer = NULL;      // remove all references

    if (NULL != pszConvertBuffer2)
    {
        delete [] pszConvertBuffer2;
        pszConvertBuffer2 = NULL;
    }

    return rc;
}
// ls08ExtractLongs


/*
  -----------------------------------------------------------------------------
  function:     ls08ExecGetVal
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls08ExecGetVal(tls00_DBInfo         *DBInfo,
               tls00_LongExecInfo   *pLongInfo,
               tsp00_LongDescriptor *pLongDescArray,
               tsp00_Addr            pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08ExecGetVal");

    tsp00_Int4 rc = errOK_els00;

    tsp1_packet  *SqlSentPckt     = DBInfo->dbiPktSndList_ls00[1];;
    tsp1_packet  *SqlReceivedPckt = NULL;
    tsp1_segment *SqlSegm         = NULL;
    tsp1_part    *SqlPart         = NULL;

    // Count of descriptors
    tsp00_Int4            lLongCnt = (pLongInfo->dliCntNNLongs_ls00 - pLongInfo->dliFinishedNNLongs_ls00);
    tsp00_LongDescriptor *pLongD   = NULL;


    //*
    //*     Initialize GETVAL packet
    //*
    rc = ls04FastInit(SqlSentPckt, &SqlSegm, &SqlPart, DBInfo);
    ls04SetMsgType(SqlSegm, sp1m_getval);
    SqlPart->sp1p_part_kind().becomes(sp1pk_longdata);

    // Limit count of descs in packet if necessary
    tsp00_Int4 lMaxLongDescInPckt = SqlPart->sp1p_buf_size()/(LongDescSize_ls00 + 1);
    lLongCnt = ( lLongCnt >= lMaxLongDescInPckt) ? lMaxLongDescInPckt : lLongCnt;

    TRACE_PRNF_MLS99(("ls08ExecGetVal", "Packet buffer size (bytes): %d\n", SqlPart->sp1p_buf_size()));
    TRACE_PRNF_MLS99(("ls08ExecGetVal", "Maximum number of long descriptors in packet: %d\n", lMaxLongDescInPckt));
    TRACE_PRNF_MLS99(("ls08ExecGetVal", "Number of long descriptors in packet: %d", lLongCnt));

    //*
    //*     Copy descriptors into packet
    //* Vallen = 0 ---> kernel sends maximum data for those longs fitting in single packet
    //*
    tsp00_Int4 j = pLongInfo->dliFinishedNNLongs_ls00;
    for (j; ( (j < pLongInfo->dliFinishedNNLongs_ls00 + lLongCnt) && (errOK_els00 == rc) ); ++j)
    {
        pLongD = &pLongDescArray[j];
        pLongD->ld_vallen() = 0;
        rc = ls04CharToPart(SqlPart, 0);                                                    // def-byte
        rc = ls04BufToPart(SqlPart, REINTERPRET_CAST(char*, pLongD), LongDescSize_ls00);    // data
    }
    SqlPart->sp1p_arg_count() = lLongCnt;   // Update count of data in packet

    if (errOK_els00 == rc)
    {
        rc = ls04FinishPart(SqlSentPckt, SqlPart);
    }
    if (errOK_els00 == rc)
    {
        tsp00_Int4 lErrPos = 0;
        rc = ls03ProcessSQLPacket(DBInfo, 1, lErrPos, pszErrText);        // Send, receive and check SQL packet
        if (errOK_els00 == rc)
        {   //  Get descriptors
            rc = ls04GetLONGDescriptor(DBInfo->dbiPktRcvList_ls00[1],
                                       lLongCnt,
                                       pLongDescArray,
                                       pLongInfo->dliFinishedNNLongs_ls00);
            if (errOK_els00 != rc)
            {
                LoaderMessages::Instance()->ls98Msg(layDataExtract_els98, pszErrText,
                                                                           STATIC_CAST(ErrorNumbers, rc));
            }
        }
    }

    return rc;
}
// ls08ExecGetVal


/*
  -----------------------------------------------------------------------------
  function:     ls08CloseGetVal
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls08CloseGetVal(tls00_DBInfo         *pDBInfo,
                tls00_LongExecInfo   *pLongInfoStruct,
                tsp00_LongDescriptor *pLongDescArray,
                tsp00_Addr            pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08CloseGetVal");

    tsp00_Int4    rc        = errOK_els00;
    tsp1_packet  *pSqlReqPacket = NULL;
    tsp1_segment *pSqlSegm      = NULL;
    tsp1_part    *pSqlPart      = NULL;
    tsp00_Int4    lPcktNo       = 1;
    
    // Generating new GETVAL-command - we use packet 1 here because we use this also when
    // executing the GETVALs
    pSqlReqPacket = pDBInfo->dbiPktSndList_ls00[lPcktNo];

    rc = ls04FastInit(pSqlReqPacket, &pSqlSegm, &pSqlPart, pDBInfo);   // fast packet initialization
    ls04SetMsgType(pSqlSegm, sp1m_getval);                          // message type 'PUTVAL'
    pSqlPart->sp1p_part_kind().becomes(sp1pk_longdata);

    pSqlPart->sp1p_arg_count() = 1;

    //*
    //*  Define some LONG Descriptor values
    //*
    tsp00_LongDescriptor *pLongD = NULL;
    tsp00_Int4 i = 0;
    for (i; ( (i < pLongInfoStruct->dliCntNNLongs_ls00) && (errOK_els00 == rc) ); ++i)
    {
        pLongD = &pLongDescArray[i];
        if (true == pLongD->ld_infoset().includes(ld_no_close))
        {
            pLongD->ld_valmode().becomes(vm_close);

            rc = ls04CharToPart(pSqlPart, DEF_BYTE_ASCII_LS00);         // write defined byte
            if (errOK_els00 == rc)
            {   // write long descriptor into packet
                rc = ls04BufToPart(pSqlPart, REINTERPRET_CAST(char*, pLongD), LongDescSize_ls00);
            }                           
        }
    }

    // We'll send the packet only if there are any long descriptors with ld_no_close which
    // is indicated by a part length not equal 0
    if ( (errOK_els00 == rc) && (0 != ls04GetPartLength(pSqlPart) ) )
    {
        rc = ls04FinishPart(pSqlReqPacket, pSqlPart);
        if (errOK_els00 == rc)
        {
            tsp00_Int4 lErrPos = 0;
            rc = ls03ProcessSQLPacket(pDBInfo, lPcktNo, lErrPos, pszErrText); // Send, receive and check SQL packet
            if (errOK_els00 != rc)
            {
                rc = errProcessLongsWithGetVal_els98;
            }
        }
    }

    return rc;
}
//  ls08CloseGetVal()


/*
  -----------------------------------------------------------------------------
  function:     ls08CheckLONG_DT_Compatibility
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls08CheckLONG_DT_Compatibility(tls00_CodeType &ExternalEncoding,
                               tsp00_DataType  DBDataType,
                               bool            bLOAD)
{
    ROUTINE_DBG_MEO00 ("ls08CheckLONG_DT_Compatibility");

    // We don't get in here if the value represents the NULL value

    tsp00_Int4 rc = errOK_els00;

    if (ctUnknown_els00 == ExternalEncoding)
    {
        // In case the code type has not been defined by the user we derive it
        // from the db data type
        ExternalEncoding = STATIC_CAST(tls00_CodeType, szLONG_Mapping_ls08[DBDataType]);
        if (ctUCS2_els00 == ExternalEncoding)
        {
            if (true == ls07Swapped(NULL))
            {
                ExternalEncoding = ctUCS2Swapped_els00;
            }
        }
    }
    else
    {
        if (true == bLOAD)
        {
            if  (0 == szLOAD_LONG_Compatibility_ls08[ExternalEncoding][DBDataType])
            {
                rc = errIncompatibleDataTypes_els98;    // error: data types do not match
            }
        }
        else
        {
            if ( (0 == szEXTRACT_LONG_UNICODE_ls08[ExternalEncoding][DBDataType]) ||
                 (0 == szEXTRACT_LONG_ASCII_ls08[ExternalEncoding][DBDataType])    )
            {
                rc = errIncompatibleDataTypes_els98;    // error: data types do not match
            }
        }
    }

    return rc;
}
// ls08CheckLONG_DT_Compatibility()



//*******  S T A T I C   F U N C T I O N S   I M P L E M E N T A T I O N  ********


// RUECKGABE VON ReadLen ueber ld_vallen
/*
  -----------------------------------------------------------------------------
  function:     ls08_FillLong
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls08_FillLong(tls00_Buffer         &LongDataBuffer,
              tsp00_LongDescriptor *pLongDescriptor,
              tls00_ParamInfo      &ParamInfo,
              tls00_ParamInfo_LONG &LCI,
              bool                  ExternalValueIsHex,
              bool                  bUnicodeOI,
              tsp81_CodePage       *pCodePage,
              tls00_Buffer2        *pConversionBuffer,
              tsp00_Addr            pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08_FillLong");

    tsp00_Longint      rc       = errOK_els00;
    tls00_FieldBasics *pFB      = &ParamInfo.piBasicInfo_ls00;

    // Compute input length of data to read
    tsp00_Longint      ReadLen  = LCI.loiLength_ls00 - LCI.loiCountOfSendData_ls00;

    //*
    //*     Emtpy values
    //* already handled in ls20ValueToOIBuffer and ls08InsertLongs
    //*
    if (0 == ReadLen)
    {
        if (0 == LCI.loiLength_ls00)
        {
            pLongDescriptor->ld_valmode().becomes(vm_alldata);
        }
        return rc;
    }

    // Define char buffer pointers for convenient use
    unsigned char     *pszBuffer        = &(STATIC_CAST(unsigned char*,
                                                        LongDataBuffer.pBufPtr_ls00)[LongDataBuffer.lBufLen_ls00]);
    unsigned char     *pszConvertBuffer = REINTERPRET_CAST(unsigned char*, pConversionBuffer->pBufPtr_ls00);
    
    // Define the count of Bytes of LONG data to read in order to submit it at once in Order Interface Packet
    // -> in case of ASCII OIP and ASCII data this corresponds to the size of the OIP;
    // depending on conversion needs this can get bigger or smaller
    tsp00_Int4         lBytesToRead = LongDataBuffer.lBufSize_ls00;

    //*
    //*     Define if conversion is needed
    //* If conversion is needed compute the real count of Bytes to read
    //*
    if ( (true == bUnicodeOI) || (true == ExternalValueIsHex) )
    {
        if ( (true == ExternalValueIsHex) && (true == bUnicodeOI) )
        {
            //   external   | DB                |
            //   data type  | data type         | Byte count to read from external LONG FILE
            //------------------------------------------------------------------------------
            // CHAR ASCII   | LONG CHAR ASCII   | exactly as much as fits into a single packet
            //              |                   |  --> 2 conversion steps
            //              |                   |   - conversion from hex readable to byte (halves the byte count)
            //              |                   |   - conversion to UCS2 (doubles the byte count)
            //              | LONG BYTE         | twice as much as fits into a single packet
            //              |                   |  --> 1 conversion step
            //              |                   |   - conversion from hex readable to byte (halves the byte count)
            //              |                   |
            // CHAR UNICODE | LONG CHAR UNICODE | 4 times as much as fits into a single packet
            //              |                   |  --> 2 conversion steps
            //              |                   |   - conversion from hex readable UCS2 to
            //              |                   |     hex readable ASCII (halves the byte count)
            //              |                   |   - conversion from hex readable to byte (halves the byte count)
            //              | LONG CHAR ASCII   | forbidden (information loss)
            //              | LONG CHAR BYTE    | same as for CHAR UNICODE - LONG CHAR UNICODE
            //              |                   |
            // BINARY       | impossible as external hexadecimal representation implies readable data (--> CHAR)

            lBytesToRead = (0 == (lBytesToRead % 2)) ? lBytesToRead : (lBytesToRead - 1);
            
            if (1 == (szLOAD_LONG_Compatibility_ls08[LCI.loiFileStruct_ls00->vfFileEncoding_ls00][pFB->fbDataType_ls00]) )
            {
                lBytesToRead *= 2;
            }
            if (true == ls00IsUCS2(&LCI.loiFileStruct_ls00->vfFileEncoding_ls00))
            {
                lBytesToRead *=2;
            }
        }
        else if ( (true == ExternalValueIsHex) && (false == bUnicodeOI) )
        {
            //   external   | DB              |
            //   data type  | data type       | Byte count to read from external LONG FILE
            //------------------------------------------------------------------------------
            // CHAR ASCII   | LONG CHAR ASCII | twice as much as fits into a single packet
            //              |                 |  --> 1 conversion step
            //              |                 |   - conversion from hex readable to byte (halves the byte count)
            //              | LONG BYTE       | same as for LONG CHAR ASCII
            //              |                 |
            // CHAR UNICODE | LONG CHAR ASCII | forbidden (information loss)
            //              | LONG CHAR BYTE  | 4 times as much as fits into a single packet
            //              |                 |  --> 2 conversion steps
            //              |                 |   - conversion from hex readable UCS2 to
            //              |                 |     hex readable ASCII (halves the byte count
            //              |                 |   - conversion from hex readable to byte (halves the byte count)
            //              |                 |
            // BINARY       | impossible as external hexadecimal representation implies readable data (--> CHAR)

            lBytesToRead = (0 == (lBytesToRead % 2)) ? lBytesToRead : (lBytesToRead - 1);

            lBytesToRead *=2;

            if (true == ls00IsUCS2(&LCI.loiFileStruct_ls00->vfFileEncoding_ls00))
            {
                lBytesToRead *=2;
            }
        }
        else if ( (false == ExternalValueIsHex) && (true == bUnicodeOI) )
        {
            //   external   | DB                |
            //   data type  | data type         | Byte count to read from external LONG FILE
            //------------------------------------------------------------------------------
            // CHAR ASCII   | LONG CHAR ASCII   | half of the amount that fits into a single packet
            //              |                   |  --> 1 conversion step
            //              |                   |   - conversion to UCS2 (doubles the byte count)
            //              | LONG BYTE         | exactly as much as fits into a single packet
            //              |                   |  --> no conversion
            //              |                   |
            // CHAR UNICODE | LONG CHAR UNICODE | same as for CHAR ASCII - LONG BYTE
            //              | LONG CHAR ASCII   | forbidden (information loss)
            //              | LONG CHAR BYTE    | same as for CHAR ASCII - LONG BYTE
            //              |                   |
            // BINARY       | LONG BYTE         | same as for CHAR ASCII - LONG BYTE

            
            // char data ascii:     we can read only half of the amount of data from file as fits into a single packet
            //                      --> 1 conversion steps
            //                        - conversion to UCS2 (doubles the byte count)
            // char data unicode:   we can read exactly as much data from file as fits into a single packet
            //                      --> no conversion step
            // binary data:         same as for char data unicode
            if (1 < (szLOAD_LONG_Compatibility_ls08[LCI.loiFileStruct_ls00->vfFileEncoding_ls00][pFB->fbDataType_ls00]) )
            {
                lBytesToRead /= 2;
            }

            // 2 Byte alignment of read buffer for UCS2 chars
            if (true == ls00IsUCS2(&LCI.loiFileStruct_ls00->vfFileEncoding_ls00))
            {
                lBytesToRead = (0 == (lBytesToRead % 2)) ? lBytesToRead : (lBytesToRead - 1);
            }
        }
    }   // end if ( (true == bUnicodeOI) || (true == ExternalValueIsHex) )
    
    if (lBytesToRead >= ReadLen)    // LONG fits completely into packet
    {
        if (LCI.loiCountOfSendData_ls00 > 0)
        {
            pLongDescriptor->ld_valmode().becomes(vm_lastdata);
        }
        else
        {
            pLongDescriptor->ld_valmode().becomes(vm_alldata);
        }
    }
    else                            // LONG does not fit completely into packet
    {
        pLongDescriptor->ld_valmode().becomes(vm_datapart);
        ReadLen = lBytesToRead;     // assign Byte count to value of length to read
    }

    //*
    //*     Read data from data file (taking necessary subsequent conversion to UCS2/from HEX into account) 
    //* We need to move the offset for reading by the amount of already read data
    //*
    tsp00_Int4 mbcount = 0;
    if (0 != ReadLen)
    {
        if ( (true == ExternalValueIsHex)
             ||
             ( (true == bUnicodeOI) &&
               (1 < szLOAD_LONG_Compatibility_ls08[LCI.loiFileStruct_ls00->vfFileEncoding_ls00][pFB->fbDataType_ls00]) ) )
        {
            pszBuffer = pszConvertBuffer;
        }


        // MAGIC BYTEs handling
        if (false == ls30VFIsMagicBytesCacheEmpty(LCI.loiFileStruct_ls00))
        {
            mbcount = ReadLen;
            ls30VFGetMagicBytes(pszBuffer, LCI.loiFileStruct_ls00, mbcount);
        }

        tls00_LargeNumber foffset = ls07AddLargeNumbersEx(LCI.loiFileOffset_ls00, LCI.loiCountOfSendData_ls00);

        if (ReadLen > mbcount)      // Read only if the whole LONG content is not yet in the magic bytes cache
        {
            ReadLen = ls30VFReadLongBin(REINTERPRET_CAST(char*, pszBuffer + mbcount),
                                        *LCI.loiFileStruct_ls00,
                                        ReadLen - mbcount,
                                        &foffset,
                                        pszErrText);
            if (ReadLen < 0)
            {
                // the negative value ReadLen IS the error code for reading file
                rc = ReadLen;
            }
            else
            {
                ReadLen += mbcount;
            }
        }
    }   // end if (0 != ReadLen)


    //*
    //*     Adapt counter in LONG FILE exec structure and if necessary the long descriptor
    //*     for communication with the kernel
    //*
    if (errOK_els00 == rc)
    {
        // Check if only part of requested data could be read
        if ( (vm_datapart == pLongDescriptor->ld_valmode()) && (ReadLen < lBytesToRead) )
        {
            // Could not read amount of data we expected? May be the file is shorter as aspected
            // and the user tried to read TOO MUCH data
            // --> correct LONG Descriptor Info because we have still free space in packet
            if (LCI.loiCountOfSendData_ls00 > 0)
            {
                pLongDescriptor->ld_valmode().becomes(vm_lastdata);
                TRACE_PRNF_MLS99(("ls08_FillLong", "\tChanged valmode from part to lastdata.\n"));
            }
            else
            {
                pLongDescriptor->ld_valmode().becomes(vm_alldata);
                TRACE_PRNF_MLS99(("ls08_FillLong", "\tChanged valmode from part to alldata.\n"));
            }
        }
        TRACE_PRNF_MLS99(("ls08_FillLong", "%d bytes data\n", ReadLen) );

        // save LONG Data length; it is the right length in unicode case, too
        LCI.loiCountOfSendData_ls00 += ReadLen;

        // adjust the file offset again if mbcount is gt 0; otherwise exactly this count
        // of data (mbcount) is not loaded if the data length exceeds the size of a single
        // order interface packet;
        // mbcount can only be gt 0 if the first bytes of a files are loaded
        if (mbcount > 0)
        {
            LCI.loiFileOffset_ls00.lnuRemainder_ls00 -= mbcount;
        }
    }


    //*
    //*     Make all necessary conversions
    //*

    //*
    //*     Convert data into binary data if hex representation;
    //*
    if ( (errOK_els00 == rc) && (true == ExternalValueIsHex) )
    {
        tsp00_Longint len = 0;

        //*
        //*     In case it is UCS2 and hex - first conversion from UCS2 to ASCII hex readable
        //*
        if (true == ls00IsUCS2(&LCI.loiFileStruct_ls00->vfFileEncoding_ls00))
        {
            len = pConversionBuffer->lBufLen_ls00;
    
            rc = ls05ConvertFromUCS2Simple(pszConvertBuffer, pszConvertBuffer, len, ReadLen, ctAscii_els00, pszErrText);
            if (errOK_els00 == rc)
            {
                ReadLen = STATIC_CAST(tsp00_Int4, len);
            }
        }

        //*
        //*     second conversion from hex readable to Byte
        //*
        len = LongDataBuffer.lBufSize_ls00;
        if ( (false == bUnicodeOI) ||
             (1 == szLOAD_LONG_Compatibility_ls08[LCI.loiFileStruct_ls00->vfFileEncoding_ls00][pFB->fbDataType_ls00]) )
        {
            pszBuffer = &(STATIC_CAST(unsigned char*, LongDataBuffer.pBufPtr_ls00)[LongDataBuffer.lBufLen_ls00]);
        }

        rc = ls07ConvertHexToByteEx(REINTERPRET_CAST(char*, pszConvertBuffer), ReadLen, pszBuffer, len);
        if (errOK_els00 == rc)
        {
            ReadLen = len;
        }
        else
        {
            LoaderMessages::Instance()->ls98Msg(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers,rc),
                                                                               ReadLen,
                                                                               pszConvertBuffer);
        }
    }   // end if ( (errOK_els00 == rc) && (true == ExternalValueIsHex) )

    
    //*
    //*     Convert character data to UCS2 in case of UNICODE order interface
    //*
    if ( (errOK_els00 == rc) && (true == bUnicodeOI) )
    {
        if (1 < szLOAD_LONG_Compatibility_ls08[LCI.loiFileStruct_ls00->vfFileEncoding_ls00][pFB->fbDataType_ls00])
        {
            pszBuffer = &(STATIC_CAST(unsigned char*, LongDataBuffer.pBufPtr_ls00)[LongDataBuffer.lBufLen_ls00]);

            // At this point we need to use the real free space for converting
            tsp00_Longuint lDestLen = LongDataBuffer.lBufSize_ls00;

            rc = ls05ConvertToUCS2(pszBuffer,
                                   lDestLen,
                                   pszConvertBuffer,
                                   STATIC_CAST(tsp00_Int4, ReadLen),
                                   LCI.loiFileStruct_ls00->vfFileEncoding_ls00,
                                   pFB->fbDataType_ls00,
                                   pszErrText,
                                   pCodePage);
            if (errOK_els00 == rc)
            {
                ReadLen = STATIC_CAST(tsp00_Longint, lDestLen);
            }
        }
        else if (true == ls00IsUCS2(&LCI.loiFileStruct_ls00->vfFileEncoding_ls00) )
        {
            tsp00_SwapKind_Enum Swap = ls07DefineSwap(NULL);
            if ( ( (sw_full_swapped == Swap) && (ctUCS2_els00 == LCI.loiFileStruct_ls00->vfFileEncoding_ls00) )
                 ||
                 ( (sw_normal == Swap)       && (ctUCS2Swapped_els00 == LCI.loiFileStruct_ls00->vfFileEncoding_ls00) ) )
            {
                ls07SwapUCS2StringInPlace(REINTERPRET_CAST(char*, pszBuffer), ReadLen);
            }
        }
    }

    // Check for errors and adapt buffer length and size
    if (errOK_els00 == rc)
    {
        pLongDescriptor->ld_vallen() += STATIC_CAST(tsp00_Int4, ReadLen);   // change VALLEN in LONG Descriptor
        LongDataBuffer.lBufSize_ls00 -= STATIC_CAST(tsp00_Int4, ReadLen);   // decrease free space by read length
        LongDataBuffer.lBufLen_ls00  += STATIC_CAST(tsp00_Int4, ReadLen);   // increase used space by read length
    }

    pszBuffer = NULL;

    return rc;
}
// ls08_FillLong()


// wenn lFrom = -1 dann wird leerer string gefordert, muss im caller geprueft werden
// oder besser: in diesem fall ist lfrom = lto = 0 ??? kann das gehen ???
/*
  -----------------------------------------------------------------------------
  function:     ls08_CheckPositionSpecification
  -----------------------------------------------------------------------------
*/
static int
ls08_CheckPositionSpecification(tsp00_Addr          pszString,
                                tls00_LargeNumber  &lFromValue,
                                tsp00_Longint      &lLength,
                                SAPDB_Int8         &FileOffset)     // This is an auxiliary var to get an offset as int8
{
    ROUTINE_DBG_MEO00 ("ls08_CheckPositionSpecification");
    
    tsp00_Int4    rc       = errLongfileSpecMissing_els98;

    size_t i = 0;
    int    j = 0;
    bool   bValid = true;

    char*       pszValue[2];        // 0. value = lFrom, 1. value = lTo
    SAPDB_Int8  lValue[2];

    pszValue[1] = strchr(pszString, '-');                   // find "-"
    if (NULL != pszValue[1])                                // found -> convert numbers
    {
        pszValue[1][0] = 0;                                 // set null terminator
        pszValue[1] += 1;                                   // 1st convert last (to) number
        pszValue[0] = pszString;

        for (i; i < 2; ++i)
        {
#if defined (_WIN32)
            lValue[i] = _atoi64(pszValue[i]);
#else
  #if defined (BIT64) 
            lValue[i] = atol(pszValue[i]);
  #else
            lValue[i] = atoll(pszValue[i]);
  #endif
#endif
            if (0L == lValue[i])                            // this is actually an error from atol but it could be
            {                                               // that the string to convert simply consists entirely of 0
                while ( (0 != pszValue[i][j]) && ('0' == pszValue[i][j]) )
                {
                    ++j;
                }
                if (i < _tcslen(pszValue[i]))
                {
                    bValid = false;
                    break;
                }
            }
        }

        if (true == bValid)
        {
            lValue[0] -= 1;                // convert to c-position specification
            FileOffset = lValue[0];

            lFromValue.lnu2GB_Blocks_ls00 = STATIC_CAST(SAPDB_Int4, (lValue[0] / MAX_INT4_SP00));
            lFromValue.lnuRemainder_ls00  = STATIC_CAST(SAPDB_Int4, (lValue[0] - lFromValue.lnu2GB_Blocks_ls00*MAX_INT4_SP00));
            lLength                       = STATIC_CAST(SAPDB_Int4, (lValue[1] - lValue[0]));    // can't actually be gt MAX_INT4_SP00
            
            // Next check  - invalid from-to specification?
            if (lLength > 0)
            {
                rc = errOK_els00;
            }
            else if (0L == lLength)
            {
                // special treatment for LONG values: this indicates to insert empty string
                lFromValue.lnuRemainder_ls00 = 0L;
                rc = errOK_els00;
            }
        }
    }
    // else  -->  No "-" found. Error!    done by initialization of return code

    return rc;
}
// ls08_CheckPositionSpecification()


/*
  -----------------------------------------------------------------------------
  function:     ls08_FillDataBufferForPutvalRequest
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls08_FillDataBufferForPutvalRequest(tls00_MultColSpec*       pMultColSpec,
                                    tls00_OI_DataPartCache*  pDPC,
                                    tls00_TransformationRTE* pTransformationRTE,
                                    bool                     bIsDBUnicode,
                                    bool &                   bLastPutVal,
                                    tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08_FillDataBufferForPutvalRequest");

    SAPDB_Int4 rc = errOK_els00;
   
    tls00_Buffer*         _pBuffer          = &pDPC->dpc_Buffer_ls00;
    SAPDB_Int4&           _FreeSpace        = _pBuffer->lBufSize_ls00;
    SAPDB_Int4&           _UsedSpace        = _pBuffer->lBufLen_ls00;

    // working data pointer - use the buffer allocated at start of dataextract
    unsigned char*        _pszBufPtr        = STATIC_CAST(unsigned char*, _pBuffer->pBufPtr_ls00);

    tsp00_LongDescriptor* _pLongDescriptor  = NULL;
    SAPDB_UInt4           _lLongDescPos     = 0;

    tls00_Column*         _pColumn          = NULL;
    tls00_LongExecInfo*   _pLongInfo        = &pTransformationRTE->rte_LongExecInfo_ls00;

    SAPDB_Int4            i                 = _pLongInfo->dliFinishedNNLongs_ls00;
    SAPDB_Int2            _Index            = 0;
    SAPDB_Int4            _FinishedLongsCnt = 0;
    _UsedSpace                              = 0;


    //*
    //* Loop until count of LONGs to insert is reached or there is no space left
    //* to insert at least the undef-byte, the LONG descriptor, and 1 byte of data
    //*
    for (i;
         ( (i < _pLongInfo->dliCntNNLongs_ls00) && (_pBuffer->lBufSize_ls00 >= (2 + LongDescSize_ls00)) && (rc >= 0) );
         ++i)
    {
        _Index    = _pLongInfo->dliNNLongs_ls00[i];
        _pColumn = pMultColSpec->mcsColumn_ls00[_Index];            // Parameter description

        TRACE_PRNF_MLS99(("ls08ExecPutVal", "\tloop value: %d\n", i));
        TRACE_PRNF_MLS99(("ls08ExecPutVal", "\tcolumn: %s\n", _pColumn->colName_ls00.asCharp()));

        _pszBufPtr[_UsedSpace]  = DEF_BYTE_ASCII_LS00;          // Insert first byte
        _UsedSpace             += 1;

        _lLongDescPos           = _UsedSpace;                   // Position in buffer where to insert descriptor
        _pLongDescriptor        = &pTransformationRTE->rte_pLongDescriptorContainer_ls00[i];

        _UsedSpace             += LongDescSize_ls00;            // Increase used space by length of the descriptor
        _FreeSpace             -= (1 + LongDescSize_ls00);      // Decrease free space by length of descriptor and undef-byte

        TRACE_PRNF_MLS99(("ls08ExecPutVal", "\tfree space for data (in bytes): %d\n", _FreeSpace));

        // Insert ValPos (=starting point of LONG in packet); ATTENTION: converting VALPOS to PASCAL Notation !!!
        _pLongDescriptor->ld_valpos() = STATIC_CAST(tsp00_Int4, (_UsedSpace + 1));

        rc = ls08_FillLong(*_pBuffer,
                            _pLongDescriptor,
                            *pTransformationRTE->rte_TableInfo_ls00.tdParamArray_ls00[_Index],
                            _pColumn->colLongInfo,
                            _pColumn->colFormat_ls00.ffoIsHex_ls00,
                            bIsDBUnicode,
                            pTransformationRTE->rte_DataSource_ls00.pdasCodePage_ls00,
                            &_pLongInfo->dliConversionBuffer_ls00,
                            pszErrText);
        if (rc < 0)
        {
            LoaderMessages::Instance()->ls98Msg(layDataLoad_els98, pszErrText, errProcessLongs_els98,
                                                                               _pColumn->colName_ls00.asCharp(), rc);
        }
        else
        {
            // Copy long descriptor into data buffer
            SAPDB_memcpy(&_pszBufPtr[_lLongDescPos], REINTERPRET_CAST(char*, _pLongDescriptor), LongDescSize_ls00);

            if ( (vm_alldata == _pLongDescriptor->ld_valmode()) || (vm_lastdata == _pLongDescriptor->ld_valmode()) )
            {
                ++_FinishedLongsCnt;
            }
        }
    }   // end for ... ( (i < pLongInfo->dliCntNNLongs_ls00) && (FreeSpace >= (2 + LongDescSize_ls00)) ) ...

    // compute count of longs in packet: MISUSE (!!!) pDPC->dpc_lRowCount_ls00 as a long value counter here
    pDPC->dpc_lRowCount_ls00 =  i - _pLongInfo->dliFinishedNNLongs_ls00;

    if ( (0 != pDPC->dpc_lRowCount_ls00) && (rc >= errOK_els00) )                   // LONG Info's inserted? Errors?
    {
        _pLongInfo->dliFinishedNNLongs_ls00 += _FinishedLongsCnt;

        //*
        //*     Last PUTVAL-packet?
        //* insert descriptor with valmode=vm_last_putval
        //*
        if (_pLongInfo->dliFinishedNNLongs_ls00 == _pLongInfo->dliCntNNLongs_ls00)
        {
            if (_FreeSpace >= 1 + LongDescSize_ls00 )           // Do last descriptor and undef-byte fit into  packet?
            {
                _pszBufPtr[_UsedSpace] = DEF_BYTE_LS00;         // Insert def/undef-byte
                ++_UsedSpace;

                // Let Pointer point to LONG Descriptor Info
                _pLongDescriptor = &pTransformationRTE->rte_pLongDescriptorContainer_ls00[i-1];
        
                // Define LONG Descriptor values
                _pLongDescriptor->ld_valmode().becomes(vm_last_putval);
                _pLongDescriptor->ld_vallen() = 0;
                _pLongDescriptor->ld_valpos() = 0;
                
                // Copy long descriptor into data buffer
                SAPDB_memcpy(&_pszBufPtr[_UsedSpace], REINTERPRET_CAST(char*, _pLongDescriptor), LongDescSize_ls00);

                _UsedSpace  += LongDescSize_ls00;
                bLastPutVal  = true;
                ++pDPC->dpc_lRowCount_ls00;
            }
        }
    }   // end if ( (0 != LongsInPcktCnt) && (rc >= 0) )

    return rc;
}
// ls08_FillDataBufferForPutvalRequest


/*
  -----------------------------------------------------------------------------
  function:     ls08_FillPutvalRequestPacket
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4 
ls08_FillPutvalRequestPacket(tls00_DBInfo*            pDBInfo,
                             SAPDB_Int4               lSndPcktNo,
                             tls00_OI_DataPartCache*  pDPC,
                             tsp1_part_attributes*    pPartAttributes,
                             tsp00_LongDescriptor*    pDescriptorArray,     // starting with the right start member?
                             tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls08_FillPutvalRequestPacket");

    SAPDB_Int4 rc = errOK_els00;

    tsp1_segment *_pSqlSegm = NULL;
    tsp1_part    *_pSqlPart = NULL;

    TRACE_PRNF_MLS99(("ls08_FillPutvalRequestPacket", "NEW PACKET\n"));

    rc = ls04FastInit(pDBInfo->dbiPktSndList_ls00[lSndPcktNo], &_pSqlSegm, &_pSqlPart, pDBInfo);
    ls04SetMsgType(_pSqlSegm, sp1m_putval);                       // Message Type 'PUTVAL'
    _pSqlPart->sp1p_part_kind().becomes(sp1pk_longdata);
    ls04SetPartAttribute(_pSqlPart, *pPartAttributes);  // Set part attribut (always pointing to next_packet)
    _pSqlPart->sp1p_arg_count() = pDPC->dpc_lRowCount_ls00;

    // Check descriptors
    SAPDB_Int2           i = 0;
    tsp00_LongDescriptor _aLongDescriptor;
    SAPDB_Int4           _pos = 1;

    // We need to copy the long descriptor before manipulating it to be
    // sure not to cause a crash when casting it to an odd memory address

    while(1)        // we don't know at this point how many LONGs are in the current packet
    {
        SAPDB_memcpy(&_aLongDescriptor,
                     &(STATIC_CAST(char*, pDPC->dpc_Buffer_ls00.pBufPtr_ls00)[_pos]),
                     sizeof(tsp00_LongDescriptor));

        if (vm_last_putval == _aLongDescriptor.ld_valmode())
        {
            break;          // Last PUTVAL packet shouldn't be touched !
        }

        //ld_intern_pos should be equal
        pDescriptorArray[i].ld_valpos()  = _aLongDescriptor.ld_valpos();
        pDescriptorArray[i].ld_vallen()  = _aLongDescriptor.ld_vallen();
        pDescriptorArray[i].ld_valmode() = _aLongDescriptor.ld_valmode();

        SAPDB_memcpy(&(STATIC_CAST(char*, pDPC->dpc_Buffer_ls00.pBufPtr_ls00)[_pos]),
                     &pDescriptorArray[i],
                     sizeof(tsp00_LongDescriptor));

        if (vm_datapart == _aLongDescriptor.ld_valmode())
        {
            break;
        }
        else
        {
            // vm_last_putval is already handled
            // In any case we must ensure that the position pointed to by _pos is a valid position.
            // A position is valid only if following that position there is still enough space
            // in buffer to carry another descriptor and additionally at least 1 Byte of LONG data.
            _pos = _aLongDescriptor.ld_valpos() + _aLongDescriptor.ld_vallen();
            if ( (_pos >= pDPC->dpc_Buffer_ls00.lBufLen_ls00) ||    // _pos points behind buffer
                 ( (pDPC->dpc_Buffer_ls00.lBufLen_ls00 - _pos) < (sizeof(tsp00_LongDescriptor) + sizeof(char)) ) )
            {
                // There can't be another LONG in that buffer
                break;
            }
        }
        ++i;
    }

    rc = ls04BufToPart(_pSqlPart,
                       STATIC_CAST(char*, pDPC->dpc_Buffer_ls00.pBufPtr_ls00),
                       STATIC_CAST(SAPDB_Int4, pDPC->dpc_Buffer_ls00.lBufLen_ls00));
    if (errOK_els00 == rc)
    {
        rc = ls04FinishPart(pDBInfo->dbiPktSndList_ls00[lSndPcktNo], _pSqlPart);
    }

    return rc;
}
// ls08_FillPutvalDataIntoPacket



/*
  -----------------------------------------------------------------------------
  function:     ls08_ReceivePacket
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4 
ls08_ReceivePacket(tls00_DBInfo*          pDBInfo,
                   SAPDB_Int4             lRcvPcktNo,
                   tls00_LongExecInfo*    pLongInfo,
                   tsp00_LongDescriptor*  pDescriptorArray,      // starting with the right start member?
                   SAPDB_Int2             lStartMemberInLongDescriptorArray,
                   tsp00_Addr             pszErrText)
{
    SAPDB_Int4 rc = errOK_els00;
        
    rc = ls03SQLReceive(pDBInfo, lRcvPcktNo, pszErrText);
    if (errOK_els00 == rc)
    {
        char        _szErr[MAX_REPLY_LENGTH_LS00];
        SAPDB_Int4  _lErrorPos   = 0;
        _szErr[0] = 0;

        // Analyse error status of sent packet
        rc = ls04RcFromPacket(pDBInfo->dbiPktRcvList_ls00[lRcvPcktNo], _szErr, _lErrorPos);
        if (errOK_els00 != rc)
        {
            pDBInfo->dbiPktRc_ls00 = rc;                // Set return code in info structure
            LoaderMessages::Instance()->ls98Msg(layDataLoad_els98, pszErrText, errProcessLongsWithPutVal_els98, _szErr);
            rc = errProcessLongsWithPutVal_els98;
        }
    }

    //*
    //*     Get LONG-descriptors from last answer packet#
    //*
    rc = ls04GetLONGDescriptor(pDBInfo->dbiPktRcvList_ls00[lRcvPcktNo],
                               pLongInfo->dliCntNNLongs_ls00,
                               pDescriptorArray,
                               lStartMemberInLongDescriptorArray);
    if (errUnknownPart_els98 == rc)
    {
        if (lStartMemberInLongDescriptorArray == pLongInfo->dliCntNNLongs_ls00)
        {
            rc = errOK_els00;       // at putval if all longs (and may be the last putval packet) have been sent
        }                           // no descriptor is returned by the kernel

    }

    return rc;
}
// ls08_ReceivePacket()