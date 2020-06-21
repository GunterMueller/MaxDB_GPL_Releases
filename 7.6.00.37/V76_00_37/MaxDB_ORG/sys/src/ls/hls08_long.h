/*!
  -----------------------------------------------------------------------------
  module: hls08_long.h
  -----------------------------------------------------------------------------

  responsible:  SteffenS

  special area: Replication Manager

  description:  Manipulation of LONG values - loading and extracting

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
#ifndef HLS08_LONG
#define HLS08_LONG

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------
  function:     ls08CheckLONG_DT_Compatibility
  -----------------------------------------------------------------------------
  description:  Checks compatibility of LONG file encoding type (using db
                column data type). If the encoding type is not set yet the
                function sets the type according to the db data type.

  arguments:    ExternalEncoding [IN/OUT] - encoding type of long file
                DBDataType       [IN]     - database column data type
                bLOAD            [IN]     - indicates process of loading data
                                            (= true) or extracting (= false);
                                            defaults to loading

  returnvalue:  errOK_els00                    - in case of NO errors
                errIncompatibleDataTypes_els98 - incompatibility found
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls08CheckLONG_DT_Compatibility(tls00_CodeType &ExternalEncoding,
                               tsp00_DataType  DBDataType,
                               bool            bLOAD = true);


/*!
  -----------------------------------------------------------------------------
  function:     ls08InitLONGDescriptor
  -----------------------------------------------------------------------------

  description:  Initializes LONG descriptors.

  arguments:    pDesc       [OUT]  - long descriptor

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void
ls08InitLONGDescriptor(tsp00_LongDescriptor *pDesc)
{
    ROUTINE_DBG_MEO00 ("ls08InitLONGDescriptor");
    
    //      Extract from description of communication protocol
    // If a LONG descriptor has to be initialized for the EXECUTE of an INSERT
    // or UPDATE with LONGs, the first 20 bytes of the block should be set to
    // binary zero. INTERN POS has to be set to 1, INFO SET to 0.
    // The 26th and 27th byte should be set to hexadecimal 00.
    // VALMODE, VALPOS and VALLEN have to be set to values according to the
    // description in PUTVAL, Communication Protocol.
    memset(pDesc, 0, 40);
    pDesc->ld_valmode().becomes(vm_nodata);
    pDesc->ld_intern_pos() = 1;
    return;
}
// ls08InitLONGDescriptor()



/*!
  -----------------------------------------------------------------------------
  function:     ls08InitLongFileInfo
  -----------------------------------------------------------------------------
  description:  Initializes LONG data file info for every long column in the
                given column structure.
                Adjusts the flag that indicates if the file name was given
                with the dataload command and if so
                defines long data file names from command info.
                
  arguments:    pMultColSpec       [IN]  - Column structure
                pMultLongFileSpec  [IN]  - Long file specification structure
                                            built during parsing dataload command

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls08InitLongFileInfo(tls00_MultColSpec  *pMultColSpec,
                     tls00_MultFileSpec *pMultLongFileSpec);


/*!
  -----------------------------------------------------------------------------
  function:     ls08ReadLongFileInfo
  -----------------------------------------------------------------------------
  description:  Analyzes LONG information read from data file

  arguments:    myColumn  [IN]    - Long column
                Value     [IN]    - Long value parameter 
                Errtext   [OUT]   - error text

  returnvalue:  errOK_els00                  - in case of NO errors
                errLongfileSpecMissing_els98 - invalid LONG input format
  -----------------------------------------------------------------------------
*/
int
ls08ReadLongFileInfo(tls00_Column    *pField,
                     tls00_ParamInfo *pColumnParameters,
                     tls00_String    *pValue,
                     tsp00_Addr       pszErrText);


/*!
  -----------------------------------------------------------------------------
  Chapter:   Functions used to LOAD long values
  -----------------------------------------------------------------------------
*/


/*!
  -----------------------------------------------------------------------------
  function:     ls08InsertLongs
  -----------------------------------------------------------------------------
  description:  Insert value for LONG columns of a certain record into data part
                in communication packet.
                If LONG values don't fit completely into the packet the function
                sets the flag bPutVal to signal the need of using subsequent
                putval-requests to completely load the LONG values for this record.

  arguments:    pMultColSpec       [IN]  - Structure with information on columns to
                                            process
                pTransformationRTE [IN]  - Transformation runtime structure
                pDataPartCache     [IN]  - Alternating buffers
                bPutVal            [OUT] - Flag indicating if putval-requests are necessary
                                            to completely fill the longs of processing record
                bUnicodeOI         [IN]  - Order interface is unicode
                pszErrText         [OUT] - Error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors may happen

                The error text is given back in ErrText.
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls08InsertLongs(tls00_MultColSpec*        pMultColSpec,
                tls00_TransformationRTE*  pTransformationRTE,
                tls00_OI_DataPartCache*   pDataPartCache,
                bool &                    bPutVal,
                bool                      bUnicodeOI,
                tsp00_Addr                pszErrText);



/*!
  -----------------------------------------------------------------------------
  function:     ls08ExecPutVal
  -----------------------------------------------------------------------------
  description:  Insert longs using putval requests. Works with alternating buffers.

  arguments:    pDBInfo              [IN]  - structure with info on running session and
                                              database connected to
                pMultColSpec         [IN]  - structure with information on columns to
                                              process
                pTransformationRTE   [IN]  - Transformation runtime structure
                pAlternatingBuffers  [IN]  - Alternating buffers
                pszErrText           [OUT] - error text

  returnvalue:  errOK_els00         - in case of NO errors
                errPutVal_els98     - in case something went wrong inserting the long
                some other errors may happen

                The error text is given back in ErrText.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls08ExecPutVal(tls00_DBInfo*             pDBInfo,
               tls00_MultColSpec*        pMultColSpec,
               tls00_TransformationRTE*  pTransformationRTE,
               tls00_AlternatingBuffers* pAlternatingBuffers,
               tsp00_Addr                pszErrText);


/*!
  -----------------------------------------------------------------------------
  EndChapter:   Function used to LOAD long values
  -----------------------------------------------------------------------------
*/


/*!
  -----------------------------------------------------------------------------
  Chapter:   Functions used to EXTRACT long values
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------

  function:     ls08ExtractLongs

  -----------------------------------------------------------------------------

  description:  Extracts long value of one single record from order interface
                buffer using long descriptors. Additionally gets the available
                LONG descriptors of the remaining LONGs of that record and
                the data if fitting into the packet.
                
  attention:    Uses ALWAYS the 2nd packet of the packet arrays dbiPktSndList_ls00
                and dbiPktRcvList_ls00 in tls00_DBInfo.

  arguments:    pDBInfo         [IN]    - structure with info on running session
                                           and database connected to
                pszInputBuffer  [IN]    - 
                pMultColumn     [IN]    -
                pColumnData     [IN]    -
                pLongInfo       [IN]    -
                pLongDescArray  [IN]    -
                bUseGetVal      [IN]    -
                pConvertBuffer  [IN]    -
                pszErrText      [OUT]   - error text

  returnvalue:  errOK_els00         - in case of NO errors
                The error text is given back in pszErrText.
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
                 tsp00_Addr              pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls08ExecGetVal
  -----------------------------------------------------------------------------
  description:  Get LONGs using getval requests

                Structure pLongInfo contains count of already completely processed
                long values and the overall count of longs in one record that are 
                NOT NULL.
                Structure pLongDescArray contains all long descriptors in order
                of processing. 
                When successfully run the descriptors of processed longs are replaced
                by new descriptors delivered from kernel if their data has partially
                or completely fit into the packet. All not touched longs remain the
                same.
                The caller has to get a pointer to the packet data.

  attention:    Func always uses the 2nd packet of the packet array of one session.

  arguments:    DBInfo      [IN]  - structure with info on running session and
                                     database connected to
                pLongInfo   [IN]  - structure with info on processed longs
                DescArray   [IN]  - one-dim array of long descriptors
                pszErrText  [OUT] - error text

  returnvalue:  errOK_els00         - in case of NO errors
                some other errors may happen

                errUnknownPart_els98 - fehler beim auslesen der deskriptoren im
                                       empfangspaket
                errInternalSQL_els98 - schwerer sql-fehler im request-paket. Benutzer
                                       kann in diesem fall nicht weitermachen.
                Folgefehler des sendens und empfangens von paketen

                Liefert den fehlercode und auch gleich den fehlertext im fehlerstring.

                The error text is given back in pszErrText.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls08ExecGetVal(tls00_DBInfo         *DBInfo,
               tls00_LongExecInfo   *pLongInfo,
               tsp00_LongDescriptor *pLongDescArray,
               tsp00_Addr            pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls08CloseGetVal
  -----------------------------------------------------------------------------
  description:  Closes open longs which were extracted using GETVAL.

  arguments:    pDBInfo         [IN]    - structure with info on running session
                                           and database connected to
                pLongInfoStruct [IN]    - structure with info on processed longs
                pLongDescArray  [IN]    - one-dim. array of long descriptors
                pszErrText      [OUT]   - error text

  returnvalue:  errOK_els00         - in case of NO errors
                errPutVal_els98     - in case something went wrong inserting the long
                The error text is given back in pszErrText.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls08CloseGetVal(tls00_DBInfo         *pDBInfo,
                tls00_LongExecInfo   *pLongInfoStruct,
                tsp00_LongDescriptor *pLongDescArray,
                tsp00_Addr            pszErrText);


/*!
  -----------------------------------------------------------------------------
  EndChapter:   Functions used to EXTRACT long values
  -----------------------------------------------------------------------------
*/
#endif //HLS08_LONG
