/*! 
  -----------------------------------------------------------------------------
 
  module: hls18.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: Replication Server

  description:  Data file row and value functionality               

  version:      7.6.

  -----------------------------------------------------------------------------

  Copyright (c) 1999-2005 SAP AG-2004

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

#ifndef HLS18
#define HLS18

#include "gls00.h"
#include "hls98msg.h"

/*!
  -----------------------------------------------------------------------------
  function:     ls18GetRowLength
  -----------------------------------------------------------------------------

  description:  Computes length of record in data file from specs in command.
                This length defines later on while loading the space allocated 
                for reading the records from data file.

                In case of binary data file (formatted/formatted binary) the
                length is the exact length of a record in data file.
                In case of text data file (compressed/formatted) the length is
                a hint for the read function. The length is computed from the 
                info on the column lengths gotten from db kernel.                    

  arguments:    Columns     [IN]        - structure with all info on the columns
                                           to be loaded specified by the user.
                TableInfo   [IN]        - table description structure
                bParam      [IN]        - boolean value indicating which part of
                                          the union in the TableInfo structure to
                                          use (defaults to false):
                                          false - use fields part
                                          true  - use param part
                bBinary     [IN]        - boolean value indicating the file format

  returnvalue:  length of record
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18GetRowLength(tls00_MultColSpec      &Columns,
                 tls00_TableDescription &TableInfo,
                 //bool                    bBinary = false,
                 bool                    bUseLastColPos = false,
                 bool                    bParam         = false);


/*!
  -----------------------------------------------------------------------------
  function:     ls18GetRowLengthForExtract
  -----------------------------------------------------------------------------

  description:  Computes the length of output record (for dataextract).

                We compute somewhat generously to prevent a new computation for
                every record to write.
                The following special treatments apply
                literal            --> + length of literal
                date/time column   --> + length of chosen date or time representation
                null values        --> + length of chosen null value representation
                bool columns       --> + length of chosen bool value representation

  arguments:    Columns     [IN]    - structure with all info on the columns
                                       to be loaded specified by the user.
                pColumnData [IN]    - internal information on column values
                TableInfo   [IN]    - table description structure
                FileFormat  [IN]    - file format structure

  returnvalue:  record length
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18GetRowLengthForExtract(tls00_MultColSpec      *Columns,
                           tls00_ColumnData       *pColumnData,
                           tls00_TableDescription *TableInfo,
                           tls00_FileFormat       *FileFormat);

/*!
  -----------------------------------------------------------------------------
  function:     ls18GetRecordLength
  -----------------------------------------------------------------------------

  description:  Computes length of record in database from info from db kernel.

  arguments:    TableInfo   [IN    - table description structure
                bParam      [IN]   - boolean value indicating which part of
                                      the union in the TableInfo structure to use
                                      (defaults to false):
                                      false - use fields part
                                      true  - use param part
                                      
  returnvalue:  record length
  -----------------------------------------------------------------------------
*/
tsp00_Int2
ls18GetRecordLength(tls00_TableDescription & TableInfo, bool bParam = false);


/*
  -----------------------------------------------------------------------------
  function:     ls18RecordInit
  -----------------------------------------------------------------------------

  description:  Initialize Record Info's


  arguments:    ColumnCount  [IN] - column count
                DataArray    [IN] - data array to initialize
                pszString    [IN] - input line buffer

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls18RecordInit(tsp00_Int2 ColumnCount, tls00_String *& DataArray, tls00_String & pszString);


/*!
  -----------------------------------------------------------------------------
  function:     ls18RecordInit - overloaded
  -----------------------------------------------------------------------------

  description:  Initialize Record Info's


  arguments:    ColumnCount  [IN]  - column count
                DataArray    [IN]  - data array to initialize
                pszString    [IN]  - input line buffer
                lStrLen      [IN]  - buffer length
  
  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls18RecordInit(tsp00_Int2      ColumnCount,
               tls00_String *& DataArray,
               tsp00_Addr      pszString,
               tsp00_Int4      lStrLen);


/*!
  -----------------------------------------------------------------------------
  function:     ls18RecordConstructor
  -----------------------------------------------------------------------------

  description:  Pseudo constructor for data structure.
                Creates data structure and initializes values. Length of the
                data string is set to INSERT_DEFAULT, the data string pointer to NULL.

  arguments:    ColumnCount  [IN]  - column count
                DataArray    [IN]  - data array to create
  
  returnvalue:  errOK_els00           - no error
                errNoMoreMemory_els98 - allocating memory failed
  -----------------------------------------------------------------------------
*/
tsp00_Int2
ls18RecordConstructor(tsp00_Int2 ColumnCount, tls00_String *& DataArray);


/*!
  -----------------------------------------------------------------------------
  function:     ls18RecordDestructor
  -----------------------------------------------------------------------------

  description:  Pseudo destructor for data structure.
                Deletes pointer values.


  arguments:    ColumnCount  [IN]  - column count
                DataArray    [IN]  - data array to delete

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
void
ls18RecordDestructor(tsp00_Int2 ColumnCount, tls00_String *& DataArray);


/*!
  -----------------------------------------------------------------------------
  function:     ls18CheckValueLength
  -----------------------------------------------------------------------------

  description:  Check value length depending on db internal length of column to load.


  arguments:    ValueLength  [IN]  - length of value to check
                FieldInfo    [IN]  - field info structure (contains the db length)

  returnvalue:  errOK_els00                 - length is ok
                errIncompatibleConst_els98  - opposite

  -----------------------------------------------------------------------------
*/
inline tsp00_Int4
ls18CheckValueLength(const tsp00_Uint4 ValueLength, tls00_FieldBasics & FieldInfo)
{
    ROUTINE_DBG_MEO00 ("ls18CheckValueLength");

    return ( (ValueLength > STATIC_CAST(tsp00_Uint4, (FieldInfo.fbLength_ls00)) ) ?
            STATIC_CAST(int, errIncompatibleConst_els98) :
            errOK_els00);
}


/*!
  -----------------------------------------------------------------------------
  function:     ls18GetUnformattedValue
  -----------------------------------------------------------------------------

  description:  Separate data from a data file record
  
                This function separates the values from a data file record. The
                data is separated by a separators and may be enclosed in delimiters.

                The function 'memorizes' the positions within the row so that in
                subsequent calls to the function it only navigates from the last
                position not from the beginning of the row.
                Prerequisite:
                the lIndex-value has to be in ascending order. Indicator for subsequent
                separating is a line length of -1.

  arguments:    pszLine    [IN]  - char string holding the data read from data file
                LineLength [IN]  - Length of pszLine. If LineLength=-1 the function starts
                                    searching at the position where in the forgoing call
                                    to this function the matching separator was found
                lIndex     [IN]  - Index of value to find in record
                cSep       [IN]  - Separator symbol
                cQuot      [IN]  - Delimiter symbol
                Value      [OUT] - string structure containing the found value;
                                    If the value is the empty string (consider the following
                                    value: ""; or the index of the value goes beyond the number
                                    of values in the record) the value and the value length
                                    remain unchanged (e.g. INSERT_DEFAULT).

  returnvalue:  errOK_els00              - no error
                errCorruptDataFile_els98 - the 2nd (matching) delimiter of a column not found
                errMissingData_els98     - no data for specified column found
  -----------------------------------------------------------------------------
*/
int
ls18GetUnformattedValue(tsp00_Addr    pszLine,
                        tsp00_Int4    LineLength,
                        tsp00_Int4    lIndex,
                        char          szSep[],    // 2 byte char array
                        char          szQuot[],   // 2 byte char array
                        tls00_String *Value);


/*!
  -----------------------------------------------------------------------------
  function:     ls18GetCSVData
  -----------------------------------------------------------------------------
   
  description:  Parses a data line and assigns single data to special structure
                members. The data is separated by separators and may be enclosed
                in delimiters.
                This is true CSV parsing without a head record.

  arguments:    pString       [IN]  - char string to parse
                lStrLen       [IN]  - string length of pString
                pPattern      [IN]  - structure with pattern to search for and a 
                                          pattern failure array
                szDelimiter   [IN]  - single delimiter
                lCount        [IN]  - count of values to find in string
                lPosToRead    [IN]  - array of read positions in string
                CodeType      [IN]  - external encoding of data file
                pValue        [OUT] - structure where the data is assigned to
                lMissingIndex [OUT] - Index of value that should be read but
                                       was not found because string was too short

  returnvalue:  errOK_els00              - no error
                errMissingData_els98     - no data for specified read position found
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls18GetCSVData(char          *pString,
               tsp00_Uint4    lStrLen,
               tls00_Pattern *pPattern,
               int            lCount,
               tsp00_Uint4    lPosToRead[],
               tls00_CodeType CodeType,
               tls00_String   pValue[],
               int           &lMissingIndex);


/*!
  -----------------------------------------------------------------------------
  function:     ls18NormalizeString
  -----------------------------------------------------------------------------
   
  description:  Parses a string of a given code type (ASCII, UTF8, UCS2) and
                replaces all occurrences of single or more whitespaces with a single
                whitespace.
                It is thus not a real normalization but a 'sub'normalization.

  arguments:    pString               [IN]   - char string to normalize
                lStrLen               [IN]   - string length of pString in characters
                pszNormalizedString   [OUT]  - normalized string; memory must have been
                                                allocated by caller in size of length in
                                                Bytes
                CodeType      [IN]  - external encoding of data file

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls18NormalizeString(const char*     pString,
                    tsp00_Uint4     lStrLen,
                    char*           pszNormalizedString,
                    tls00_CodeType  CodeType);


/*!
  -----------------------------------------------------------------------------
  function:     ls18PutUnformattedValue
  -----------------------------------------------------------------------------

  description:  Writes extracted data (from a table) to the output buffer in
                external file format COMPACT.
                For structure of the data see description of ls18GetCompactValue.

  arguments:    pszBuffer    [OUT]  - Buffer to fill
                lBufLen      [IN]   - Length of buffer
                pColumnData  [IN]   - columns data structure with data to write
                pColumnDesc  [IN]   - columns structure with info on positions
                                       of the data in string
                pPattern     [IN]   - defines delimiter and separator

  returnvalue:  length of buffer filled
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls18PutUnformattedValue(tsp00_Addr        pszBuffer,
                        tsp00_Longint     lBufLen,
                        tsp00_Int4        lColumnCount,
                        tls00_ColumnData *pColumnData,
                        tls00_Pattern    *pPattern);


/*!
  -----------------------------------------------------------------------------
  function:     ls18GetCompactValue
  -----------------------------------------------------------------------------

  description:  Parses a buffer of data read from data file for single data fields
                and their length.
                The data is structured as follows:
                |------------------------------------------------------------------------...
                | meta data1 | data1 | [long data1] | meta data2 | data2 | [long data2] |...
                |------------------------------------------------------------------------...
                
                Meta data contains the length of the following data fields as follows:
                - length of a not LONG field is stored as signed 2 byte integer
                - length of a LONG field is stored as a signed 4 byte integer

                Additionally the function defines for LONG values the file offset
                to start read from.

  arguments:    pString       [IN]  - char string to parse
                pMeta         [IN]  - structure with position and length info
                                       on every single data field
                lCount        [IN]  - count of values to find in string
                lPosToRead    [IN]  - array of read positions in string
                pValue        [OUT] - structure where the data is assigned to
                pMultCol      [OUT] - structure of column structures; for LONG
                                       columns the file offset to read from is
                                       defined
                lMissingIndex [OUT] - Index of value that should be read but
                                       was not found because string was too short

  returnvalue:  errOK_els00              - no error
                errMissingData_els98     - no data for specified read position found
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18GetCompactValue(char              *pString,
                    tls00_MetaData    *pMeta,
                    tsp00_Uint4        lCount,
                    tsp00_Uint4        lPosToRead[],
                    tls00_String       pValue[],
                    tls00_MultColSpec *pMultCol,
                    int               &lMissingIndex);


/*!
  -----------------------------------------------------------------------------
  function:     ls18PutCompactValue
  -----------------------------------------------------------------------------

  description:  Writes extracted data (from a table) to the output buffer in
                external file format COMPACT.
                For structure of the data see description of ls18GetCompactValue.

  arguments:    pszMeta      [OUT]  - meta data buffer to fill
                pszBuffer    [OUT]  - Buffer to fill
                lBufLen      [IN]   - Length of buffer
                pColumnData  [IN]   - columns data structure with data to write
                pColumnDesc  [IN]   - columns structure with info on positions
                                       of the data in string

  returnvalue:  length of buffer filled
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls18PutCompactValue(tls00_Buffer2     *pszMeta,
                    tsp00_Addr         pszBuffer,        // zeigt auf pszBuffer[StartPos]
                    tsp00_Longint      lBufLen,          // definiert die zulaessige laenge EndPos - StartPos + 1
                    tls00_ColumnData  *pColumnData,
                    tls00_MultColSpec *pColumnDesc);


/*!
  -----------------------------------------------------------------------------
  function:     ls18GetFormattedValue
  -----------------------------------------------------------------------------

  description:  Separate data from a data file record
  
                This function separates the values from a data file record. The
                data is is described by its start and end position within the record.
                In case the end position equals 0 the length of the value is set to 1. 

  arguments:    pszLine     [IN]    - char string holding the data read from data file
                LineLength  [IN]    - Length of pszLine
                lStartPos   [IN]    - Start position of data in data record
                lEndPos     [IN]    - End position of data in data record
                Value       [OUT]   - string structure containing the found value;
                                       If the value is the empty string (consider the following
                                       value: ""; or the index of the value goes beyond the number
                                       of values in the record) the value and the value length
                                       remain unchanged (e.g. INSERT_DEFAULT).

  returnvalue:  errOK_els00               - no error
                errCorruptDataFile_els98  - length of the value separated is greater than
                                             the line length
  -----------------------------------------------------------------------------
*/
int
ls18GetFormattedValue(tsp00_Addr    pszLine,
                      tsp00_Int4    LineLength,
                      tsp00_Int2&   lStartPos,
                      tsp00_Int2&   lEndPos,
                      tls00_String *Value);


/*!
  -----------------------------------------------------------------------------
   function:    ls18GetFormattedValueEx
  -----------------------------------------------------------------------------

  description:  Parse a data line and assign single data to special structure
                members. The data is is described by its start and end position
                within the record.
                In case the end position equals 0 the length of the value is set to 1. 

  arguments:    pszLine        [IN]  - char string to parse
                LineLength     [IN]  - Length of pszLine
                pMultCol       [IN]  - columns structure with info on all start and end
                                       positions of the data in string
                lReadFieldsCnt [IN]  - count of values to find
                lValToColIndex [IN]  - special index to assign the right data
                                        structure member to the right column structure
                                        member
                pValue         [OUT] - structure where the data is assigned to
                lMissingIndex  [OUT] - Index of value that should be read but
                                       was not found because string was too short


  returnvalue:  errOK_els00           - no error
                errMissingData_els98  - offset of data is beyond string length
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18GetFormattedValueEx(tsp00_Addr         pszLine,
                        tsp00_Int4         LineLength,
                        tls00_MultColSpec *pMultCol,
                        tsp00_Uint4        lReadFieldsCnt,
                        tsp00_Uint4        lValToColIndex[],
                        tls00_String       pValue[],
                        tsp00_Int4        &lMissingIndex);


/*!
  -----------------------------------------------------------------------------
   function:    ls18PutFormattedValue
  -----------------------------------------------------------------------------

  description:  Writes extracted data (from a table) to the output buffer in
                external file format FORMATTED.

  arguments:    pszBuffer    [OUT]  - Buffer to fill
                lBufLen      [IN]   - Length of buffer
                pColumnData  [IN]   - columns data structure with data to write
                pColumnDesc  [IN]  - columns structure with info on all start and
                                      end positions of the data in string

  returnvalue:  length of buffer filled
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls18PutFormattedValue(tsp00_Addr         pszBuffer,
                      tsp00_Longint      lBufLen,
                      tls00_ColumnData  *pColumnData,
                      tls00_MultColSpec *pColumnDesc);


/*!
  -----------------------------------------------------------------------------
  function:     ls18BindDataToCol
  -----------------------------------------------------------------------------

  description:  Extract data from an communication packet and bind it to the
                corresponding column value in tls00_ColumnData.
                Convert to ascii if db or order interface is UNICODE and data type
                is data, time or timestamp.

  arguments:    pszBuffer  [IN]   - Buffer holding the data read from
                                     a database packet
                BufLength  [IN]   - Length of pszBuffer
                pTableDesc [IN]   - Table description structure; contains
                                     all parameter infos
                ColData    [OUT]  - Structure the data is bind to;NULL value is
                                     indicated by having the length INSERT_NULL
                ColCount   [IN]   - Count of data to bind
                bUnicodeOI [IN]   - Unicode db/order interface or not
                pszErrText [OUT]  - Error text

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18BindDataToCol(tsp00_Addr               pszBuffer,
                   tsp00_Int4              BufLength,
                   tls00_TableDescription *pTableDesc,
                   tls00_ColumnData        ColData[],
                   tsp00_Int4              ColCount,
                   bool                    bUnicodeOI,
                   tsp00_Addr              pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls18ConstructPattern
  -----------------------------------------------------------------------------

  description:  Build the tls00_Pattern structure used to read values in
                COMPRESSED format. The pattern is build using the delimiter and
                separator in the following form: 'DELIMITER-SEPARATOR-DELIMITER'.
                In case of delimiter = " and separator = ; the pattern would
                look like ";".
                The pattern is build taking into account the external encoding.

  arguments:    pPattern      [IN/OUT] - Structure to build; must have been
                                          allocated by caller
                pszSeparator  [IN]     - Separator (ascii)
                pszDelimiter  [IN]     - Delimiter (ascii)
                ExtEncoding   [IN]     - External encoding
                pszErrText    [OUT]    - Error text

  returnvalue:  errOK_els00       - successful construction of pattern
                errInternal_els98 - error converting to an encoding
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18ConstructPattern(tls00_Pattern  *pPattern,
                     tsp00_Addr       pszSeparator,
                     tsp00_Addr       pszDelimiter,
                     tls00_CodeType   ExtEncoding,
                     tsp00_Addr       pszErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls18ComputePatFailFunc
  -----------------------------------------------------------------------------

  description:  Compute a pattern failure function for pattern search.

  arguments:    pPatStruct  [IN]  - structure that holds the pattern to analyze

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls18ComputePatFailFunc(tls00_Pattern *pPattern);

/*
  -----------------------------------------------------------------------------
  function:     ls18DestructPattern
  -----------------------------------------------------------------------------

  description:  Free allocated resources in pattern structure

  arguments:    pPatStruct  [IN]  - pattern structure

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls18DestructPattern(tls00_Pattern *pPattern);

/*
  -----------------------------------------------------------------------------
  function:     ls18UndoubleDelimitersInCharData
  -----------------------------------------------------------------------------

  description:  Undoubles delimiter in char data for COMPRESSED format

  arguments:    

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls18UndoubleDelimitersInCharData(tls00_String*   pValue,
                                 tls00_Pattern*  pPattern,
                                 tls00_CodeType  CodeType);

/*
  -----------------------------------------------------------------------------
  function:     ls18CheckForNLInData
  -----------------------------------------------------------------------------

  description:  Returns if a char string is complete when loading

  arguments:    

  returnvalue:  SAPDB_TRUE  - NL in data; char string incomplete
                SAPDB_FALSE - char string is complete
  -----------------------------------------------------------------------------
*/
SAPDB_Bool
ls18CheckForNLInData(tls00_Buffer2* pBuffer, tls00_Pattern* pPattern, tls00_FileFormat* pFileFormat);

#endif /*HLS18*/