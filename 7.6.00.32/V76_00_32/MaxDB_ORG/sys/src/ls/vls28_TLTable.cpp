/*!
  -----------------------------------------------------------------------------
  module: vls28_TLTable.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of TABLELOAD for single table

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
#include "hls01.h"
#include "hls04.h"
#include "hls05.h"      // UCS2 - ASCII conversion routines
#include "hls041.h"
#include "hls07.h"
#include "hls10.h"
#include "hls11.h"
#include "hls13.h"
#include "hls15.h"
#include "hls18.h"
#include "hls51.h"
#include "hls98msg.h"
#include "hls99.h"
#include "gbd00.h"

#include "hls31_odbc_Result.h"
#include "hls26_TETable.h"
#include "hls28_TLoad.h"
#include "hls28_TLSwap.h"
#include "hls28_TLTable.h"

char const *pszLengthType_ls28[3] = {"fix length", "variable length", "variable length long"};

char const *pszColumnMode_ls28[5] = {"syskey", "key", "mandatory", "optional", "dropped"};

char const *pszDefaultType_ls28[12] = {"no default",    "user",     "usergroup", "date", "time",
                                       "stamp",         "timestamp","uid",       "true", "false",
                                       "other default", "serial"};

static SAPDB_Char szLocalCmdString[lStmtLength_ls26];
static SAPDB_Char szLocalErrString[MAX_REPLY_LENGTH_LS00];

typedef struct DefaultDescription
{
    bool                    IsUserDefined;
    tsp00_Uint2             DefaultColumnNo;
    tsp00_Uint2             DefaultValueLength;
    Tools_DynamicUTF8String DefaultValue;
}
DefaultDescription;

typedef struct ConstraintDescription
{
    tsp00_Uint4             ConstraintNameLength;
    Tools_DynamicUTF8String ConstraintName;
    tsp00_Uint4             ConstraintValueLength;
    Tools_DynamicUTF8String ConstraintValue;
}
ConstraintDescription;


/*
  -----------------------------------------------------------------------------
  BeginChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls28_SQLTypeToString
  -----------------------------------------------------------------------------

  description: Maps data types to strings describing the data type

  arguments:

  returnvalue:
  -----------------------------------------------------------------------------
*/
static const char *
ls28_SQLTypeToString(const tsp00_DataType sqltype);


/*
  -----------------------------------------------------------------------------
  function:     ls28_ReadMetaHeader
  -----------------------------------------------------------------------------

  description:

  arguments:

  returnvalue:
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ReadMetaHeader(tls00_DBInfo             *pDBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_TloadRec           *pTabLoadRec,
                    ReadBuffer               &readBuffer,
                    tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls28_GetSourceTableDesc
  -----------------------------------------------------------------------------

  description:  Reads table description (saved in format of tsp7_table_description)
                from data file and swaps the integers in the description.
                Sets the values tlrSwapFromDb_ls00 in pTLRec and (in case TL ALL
                and TL USER) psFullTabName_ls00, psTabName_ls00 in
                pTransformationCmd->trPartSpec_ls00.
                Replaces table name and table owner in table description.

  arguments:    pDBInfo      [IN]     - contains all information on the running
                                         session and the database connected to.
                pTabLoadRec  [IN]     - structure, holding information for
                                         executing TABLELOAD for a single table
                UserName     [IN]     - user name
                TableName    [IN]     - table name
                pPartSpec    [IN|OUT] - structure with information about
                                         type of TL (ALL, USER, TABLE) and values
                                         for table name and table owner
                readBuffer   [IN]     - read file buffer object
                pszErrText   [OUT]    - error text

  returnvalue:  errOK_els00              - no error
                errWrongTEFileType_els98 - corrupted data file (no table desc found)
                some error from reading the file

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetSourceTableDesc(tls00_DBInfo             *pDBInfo,
                        tls00_TloadRec           *pTabLoadRec,
                        Tools_DynamicUTF8String  &UserName,
                        Tools_DynamicUTF8String  &TableName,
                        tls00_PartSpec           *pPartSpec,
                        ReadBuffer               &readBuffer,
                        tsp00_Addr               pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls28_GetSourceIndexDesc
  -----------------------------------------------------------------------------

  description:  Reads index description (saved in format of tsp7_index_description)
                from data file and swaps the integers in the description.

  arguments:    pTLRec       [IN]  - pointer; structure, holding information for
                                      executing TABLELOAD for a single table
                readBuffer [IN]  - reference; read file buffer object
                pszErrText   [OUT] - pointer; error text

  returnvalue:  errOK_els00              - no error
                some error from reading the file

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetSourceIndexDesc(tls00_TloadRec *pTLRec,
                        ReadBuffer     &readBuffer,
                        tsp00_Addr      pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls28_ReadMetaTrailer
  -----------------------------------------------------------------------------

  description:

  arguments:

  returnvalue:
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ReadMetaTrailer(tls00_DBInfo             *pDBInfo,
                     tls00_TransformationCmd  *pTransformationCmd,
                     tls00_TloadRec           *pTabLoadRec,
                     ReadBuffer               &readBuffer,
                     tsp00_Addr               ErrText );




/*
  -----------------------------------------------------------------------------

  function:     ls28_CheckHeader

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CheckHeader(tls00_DBInfo            *pDBInfo,
                 tls00_TransformationCmd *pTabLoadCmd,
                 Tools_DynamicUTF8String  TabExtrStmt,
                 tsp00_Addr               ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls28_CreateTable
  -----------------------------------------------------------------------------

  description:  Creates a table on db using binary table description structure
                tsp7_table_description. Constraints and defaults are created using
                DML.

                The table will only be created in versions 7.3 and older.
                In 7.4 versions the function returns error.

  arguments:    pDBInfo      [IN]   - contains all information on the running
                                       session and the database connected to.
                pTLRec       [IN]   - structure, holding information for
                                       executing TABLELOAD for a single table
                TableName    [IN]   - table name UTF8 encoded
                readBuffer   [IN]   - read file buffer object
                pszErrText   [OUT]  - error text

  returnvalue:  errOK_els00                 - no error
                errTLTableNotExisting_els98 - version > 7.3 and table does not exist
                errInternSQL_els98          - error executing table using binary table
                                               structure or
                                              error creating constraints/defaults
                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CreateTable(tls00_DBInfo            *pDBInfo,
                 tls00_TloadRec          *pTLRec,
                 Tools_DynamicUTF8String &TableName,
                 ReadBuffer              &readBuffer,
                 tsp00_Addr               pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls28_GetSourceConstraintList
  -----------------------------------------------------------------------------

  description:  Reads constraint definitions from data file and stores them in
                a list of constraint structures.

  arguments:    pDBInfo           [IN]   - contains all information on the running
                                            session and the database connected to.
                pTLRec            [IN]   - structure, holding information for
                                            executing TABLELOAD for a single table
                PageBuffer        [IN]   - read file buffer object
                SrcConstraintList [OUT]  - list of read constraints; constraints are
                                            stored as structure ConstraintDescription;
                                            if no constraints are read (supposing the
                                            source table did not have constraints
                                            defined) the list remains empty
                pszErrText        [OUT]  - error text

  returnvalue:  errOK_els00              - no error
                STOP_NODATA_LS28         - no constraints found in data file - thats
                                            actually no error and should be handled
                                            respectively
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetSourceConstraintList(tls00_DBInfo    *pDBInfo,
                             tls00_TloadRec  *pTLRec,
                             ReadBuffer      &PageBuffer,
                             Studio_List     &SrcConstraintList,
                             tsp00_Addr       pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls28_CreateConstraint
  -----------------------------------------------------------------------------

  description:  Creates constraints on a table using DML
                (ALTER TABLE ADD CONSTRAINT...)

  arguments:    pDBInfo         [IN]   - contains all information on the running
                                         session and the database connected to.
                TableName       [IN]   - table name (UTF8)
                ConstraintList  [IN]   - list of constraints to create;
                                          constraints are stored as structure
                                          ConstraintDescription
                pszErrText      [OUT]  - error text

  returnvalue:  errOK_els00         - no error
                errInternSQL_els98  - error creating constraints
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CreateConstraint(tls00_DBInfo            *pDBInfo,
                      Tools_DynamicUTF8String &TableName,
                      Studio_List             &ConstraintList,
                      tsp00_Addr               pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls28_EraseConstraintList
  -----------------------------------------------------------------------------

  description:  Erases a list with contraint definitions (structures
                ConstraintDescription)

  arguments:    ConstraintList [IN] - list of constraints to remove

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
static void
ls28_EraseConstraintList(Studio_List &ConstraintList);


/*
  -----------------------------------------------------------------------------
  function:     ls28_GetSourceDefaultList
  -----------------------------------------------------------------------------

  description:  Reads default definitions from data file and stores them in
                a list of default structures.

  arguments:    pDBInfo           [IN]   - contains all information on the running
                                            session and the database connected to.
                pTLRec            [IN]   - structure, holding information for
                                            executing TABLELOAD for a single table
                PageBuffer        [IN]   - read file buffer object
                bAllDefaults      [IN]   - indicates if all default definitions should
                                            be delivered to caller in the default
                                            definition list or only user defined
                                            default values
                SrcDefaultList    [OUT]  - list of read defaults; defaults are
                                            stored as structure DefaulttDescription;
                                            if no defaults are read (supposing the
                                            source table did not have defaults
                                            defined) the list remains empty
                pszErrText        [OUT]  - error text

  returnvalue:  errOK_els00              - no error
                STOP_NODATA_LS28         - no defaults found in data file - thats
                                            actually no error and should be handled
                                            respectively
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetSourceDefaultList(tls00_DBInfo    *pDBInfo,
                          tls00_TloadRec  *pTLRec,
                          ReadBuffer      &PageBuffer,
                          bool             bAllDefaults,
                          Studio_List     &SrcDefaultList,
                          tsp00_Addr       pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls28_CreateDefault
  -----------------------------------------------------------------------------

  description:  Creates defaults on a table using DML
                (ALTER TABLE <table name> COLUMN <column name> ADD DEFAULT
                 <default definition>)

  arguments:    pDBInfo      [IN]   - contains all information on the running
                                       session and the database connected to.
                pTLRec       [IN]   - structure, holding information for
                                       executing TABLELOAD for a single table
                TableName    [IN]   - table name (UTF8)
                DefaultList  [IN]   - list of defaults to create;
                                       defaults are stored as structure
                                       DefaultDescription
                pszErrText   [OUT]  - error text

  returnvalue:  errOK_els00         - no error
                errInternSQL_els98  - error creating defaults
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CreateDefault(tls00_DBInfo            *pDBInfo,
                   tls00_TloadRec          *pTLRec,
                   Tools_DynamicUTF8String &TableName,
                   Studio_List             &DefaultList,
                   tsp00_Addr               pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls28_EraseDefaultList
  -----------------------------------------------------------------------------

  description:  Erases a list with default definitions (structures
                DefaultDescription)

  arguments:    DefaultList [IN] - list of defaults to remove

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
static void
ls28_EraseDefaultList(Studio_List &DefaultList);


/*
  -----------------------------------------------------------------------------
  function:     ls28_CompareTableSchemes
  -----------------------------------------------------------------------------

  description:  Compare table schemes of source and target table.
                Function uses structures tsp7_table_description and list of
                constraint and default definitions for comparison.

  arguments:    pDBInfo        [IN]   - contains all information on the running
                                         session and the database connected to
                pTLRec         [IN]   - structure, holding information for
                                         executing TABLELOAD for a single table
                pTableDef      [IN]   - object with information on target table;
                                         used to retrieve constraint and
                                         default list of target table
                readBuffer     [IN]   - read file buffer object
                TableName      [IN]   - table name UTF8 encoded
                pszErrText     [OUT]  - error text

  returnvalue:  errOK_els00                 - no error
                errTLTableNotRestored_els98 - table schemes different
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls28_CompareTableSchemes(tls00_DBInfo               *pDBInfo,
                         tls00_TloadRec             *pTLRec,
                         TableDef                   *pTableDef,
                         ReadBuffer                 &readBuffer,
                         Tools_DynamicUTF8String    &TableName,
                         tsp00_Addr                  pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls28_CompareConstraintDefinitions
  -----------------------------------------------------------------------------

  description:  Compares constraint definitions of source and target table.

  arguments:    pDBInfo        [IN]   - contains all information on the running
                                         session and the database connected to
                pTableDef      [IN]   - object with information on target table;
                                         holds lists of constraints of target table
                PageBuffer     [IN]   - read file buffer object
                pTLRec         [IN]   - structure, holding information for
                                         executing TABLELOAD for a single table
                pszErrText     [OUT]  - error text

  returnvalue:  errOK_els00                  - no differences
                errTLSchemeDifferences_els98 - schemes different in at least one
                                               constraint definition
                The function does not write an error message but simply returns
                the error code.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CompareConstraintDefinitions(tls00_DBInfo    *pDBInfo,
                                  TableDef        *pTableDef,
                                  tls00_TloadRec  *pTLRec,
                                  ReadBuffer      &PageBuffer,
                                  tsp00_Addr      pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls28_NormalizeConstraintDefinitions
  -----------------------------------------------------------------------------
  description:  Normalizes constraint definitions.

  arguments:    TgtConstraintDef  [IN]  - definition to normalize
                SrcConstraintDef  [IN]  - definition to normalize

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls28_NormalizeConstraintDefinitions(Tools_DynamicUTF8String&  TgtConstraintDef,
                                    Tools_DynamicUTF8String&  SrcConstraintDef);


/*
  -----------------------------------------------------------------------------

  function:     ls28_CompareDefaultDefinitions
  -----------------------------------------------------------------------------

  description:  Compares default definitions of source and target table.

  arguments:    pDBInfo        [IN]   - contains all information on the running
                                         session and the database connected to
                pTableDef [IN]   - object with information on target table;
                                         holds lists of defaults of target table
                PageBuffer     [IN]   - read file buffer object
                pTLRec         [IN]   - structure, holding information for
                                         executing TABLELOAD for a single table
                pszErrText     [OUT]  - error text

  returnvalue:  errOK_els00                  - no differences
                errTLSchemeDifferences_els98 - schemes different in at least one
                                               constraint definition
                The function does not write an error message but simply returns
                the error code.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CompareDefaultDefinitions(tls00_DBInfo    *pDBInfo,
                               TableDef        *pTableDef,
                               tls00_TloadRec  *pTLRec,
                               ReadBuffer      &PageBuffer,
                               tsp00_Addr       pszErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls28_DropTableIndexes
  -----------------------------------------------------------------------------

  description:  Drops all indexes on table


  arguments:    pDBInfo      [IN]   - contains all information on the running
                                      session and the database connected to
                TableName    [IN]   - table name UTF8 encoded
                pszErrText   [OUT]  - error text

  returnvalue:  errOK_els00     No error
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls28_DropTableIndexes(tls00_DBInfo            *pDBInfo,
                      Tools_DynamicUTF8String &TableName,
                      tsp00_Addr               pszErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls28_IsTableEmpty
  -----------------------------------------------------------------------------

  description:  Checks if table is empty

  arguments:    pDBInfo      [IN]   - contains all information on the running
                                      session and the database connected to
                TableName    [IN]   - table name UTF8 encoded
                IsEmpty      [OUT]  - flags if table is empty
                pszErrText   [OUT]  - error text

  returnvalue:  errOK_els00        - no error
                errInternSQL_els98 - error querying table
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls28_IsTableEmpty(tls00_DBInfo            *pDBInfo,
                  Tools_DynamicUTF8String  &TableName,
                  bool                     &IsEmpty,
                  tsp00_Addr               pszErrText);



/*
  -----------------------------------------------------------------------------

  function:     ls28_RestoreShortStrings
  -----------------------------------------------------------------------------

  description:  Restores short strings in table to be loaded.

  arguments:    pDBInfo      [IN]  - contains all information on the running
                                      session and the database connected to.
                pTLRec       [IN]  - structure, holding information for
                                      executing TABLELOAD for a single table
                TableName    [IN]  - table name UTF8 encoded
                PageBuffer   [IN]  - read file buffer object
                pszErrText   [OUT] - error text

  returnvalue:  errOK_els00              - no error
                errWrongTEFileType_els98 - manipulated short string header page ?
                errInternSQL_els98       - error executing commands to load the shorts
                errInternal_els98        - error loading the pages

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_RestoreShortStrings(tls00_DBInfo            *pDBInfo,
                         tls00_TloadRec          *pTLRec,
                         Tools_DynamicUTF8String &TableName,
                         ReadBuffer              &PageBuffer,
                         tsp00_Addr               pszErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls28_RestorePrimaryData
  -----------------------------------------------------------------------------

  description:  Restores primary data in table to be loaded.

  arguments:    pDBInfo      [IN]  - contains all information on the running
                                      session and the database connected to.
                pTLRec       [IN]  - structure, holding information for
                                      executing TABLELOAD for a single table
                TableName    [IN]  - table name UTF8 encoded
                PageBuffer   [IN]  - read file buffer object
                pszErrText   [OUT] - error text

  returnvalue:  errOK_els00              - no error
                errWrongTEFileType_els98 - manipulated primary data header page ?
                errInternSQL_els98       - error executing commands to load the shorts
                errInternal_els98        - error loading the pages

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_RestorePrimaryData(tls00_DBInfo             *pDBInfo,
                        tls00_TloadRec           *pTLRec,
                        Tools_DynamicUTF8String  &TableName,
                        ReadBuffer               &PageBuffer,
                        tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls28_RestoreLongStrings
  -----------------------------------------------------------------------------

  description:  Restores primary data in table to be loaded.

  arguments:    pDBInfo      [IN]  - contains all information on the running
                                      session and the database connected to.
                pTLRec       [IN]  - structure, holding information for
                                      executing TABLELOAD for a single table
                PageBuffer   [IN]  - read file buffer object
                pszErrText   [OUT] - error text

  returnvalue:  errOK_els00              - no error
                errWrongTEFileType_els98 - manipulated primary data header page ?
                errInternSQL_els98       - error executing commands to load the shorts
                errInternal_els98        - error loading the pages

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_RestoreLongStrings(tls00_DBInfo    *pDBInfo,
                        tls00_TloadRec  *pTLRec,
                        ReadBuffer      &PageBuffer,
                        tsp00_Addr       pszErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls28_ReadDescFromFile
  -----------------------------------------------------------------------------

  description:  Extracts table|index description from data file.

  arguments:    readBuffer    [IN]  - read file buffer object
                DescriptionType [IN]  - enumeration; type of description to extract
                SourceSwap      [IN]  - swap of source system; is used to read the header
                                         of the page containing the description
                lPageSize       [IN]  - source system page size; used to read the header of
                                         the page containing the description
                pDescription    [OUT] - refrence to pointer; takes the description and
                                         its length
                pszErrText      [OUT] - pointer; error text

  returnvalue:  errOK_els00
                WRONG_TE_HEADER_LS28   - description type page not found (caller must
                                            handle it as error or warning)
                EOF_LS00                 - eof reached reading the description (highly unlikely)

                some error from reading the file - only these errors will be logged
                                                   to the log file and return text
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ReadDescFromFile(ReadBuffer       &readBuffer,
                      tls00_TEPageType  DescriptionType,
                      tsp00_SwapKind    SourceSwap,
                      tsp00_Int4        lPageSize,      // source page size, but source and target size may not differ!
                      tls00_String     &pDescription,
                      tsp00_Addr        pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls28_CheckTypeHeaderPage
  -----------------------------------------------------------------------------

  description:  Checks the headers on pages for every extracted type of data.
                The page size is necessary because for some types of data a header
                and a footer are written and both are identically so they can
                be compared.
                Swaps the integers in the header in place.

  arguments:    pszBuffer       [IN] - pointer; buffer of size page size which
                                        contains the header
                lPageSize       [IN] - source system page size; size of pszBuffer
                PageType        [IN] - type of data we're looking for
                SourceSwapKind  [IN] - swap kind used to swap integers

  returnvalue:  errOK_els00
                UNKNOWN_TE_HEADER_LS28 - not a known header - only for DATA of type
                                            primary data (tptData_els00), short string
                                            (tptShortString_els00) and long
                                            string (tptLongString_els00)
                WRONG_TE_HEADER_LS28   - for descriptions it is an error (w/o table
                                            description from source restore is not
                                            possible; for data this can be a message
                                            that no data of this type was extracted
                                            during TABLEEXTRACT
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CheckTypeHeaderPage(tsp00_Addr       pszBuffer,
                         tsp00_Int4       lPageSize,
                         tls00_TEPageType PageType,
                         tsp00_SwapKind   SourceSwapKind);


/*
  -----------------------------------------------------------------------------

  function:     ls28_BeginRestore
  -----------------------------------------------------------------------------

  description:  Executes the command "restore long|table". Stores the bdinfo
                coming from kernel as answer in pszBdinfo.

  arguments:    pDBInfo    [IN]  - contains all information on the running
                                      session and the database connected to.
                pszCmd     [IN]  - command (restore long or restore table);
                                    UTF8 encoded
                pszBdinfo  [OUT] - buffer holding bdinfo coming from kernel
                pszErrText [OUT] - error text

  returnvalue:  errOK_els00              - no error
                errInternSQL_els98       - error executing command
                errInternal_els98        - error getting bdinfo

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_BeginRestore(tls00_DBInfo*  pDBInfo,
                  tsp00_Addr     pszCmd,
                  tls00_BDInfo*  pBDInfo,
                  tsp00_Addr     pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls28_DataLoadLoop
  -----------------------------------------------------------------------------

  description:  Main module to execute the page load. Used for all kind of data.

  arguments:    pDBInfo    [IN]     - contains all information on the running
                                       session and the database connected to.
                pTLRec     [IN]     - structure, holding information for
                                       executing TABLELOAD for a single table
                ReadBuffer [IN]     - read file buffer object
                pBdinfo    [IN|OUT] - buffer holding bdinfo for first packet to
                                       send and of last answer packet; the latter
                                       is used to execute "end restore" command
                pszErrText [OUT]    - error text

  returnvalue:  errOK_els00              - no error
                errInternal_els98        - error sending/getting page to/from kernel
                error reading the data file

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_DataLoadLoop(tls00_DBInfo*    pDBInfo,
                  tls00_TloadRec*  pTLRec,
                  ReadBuffer&      PageBuffer,
                  tls00_BDInfo*    pBDInfo,
                  tsp00_Addr       pszErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls28_FillPacketBuffer
  -----------------------------------------------------------------------------

  description:  Fills a (request) packet with data pages. Initializes the packet to take
                the data AND the bdinfo.
                Used by ls28_DataLoadLoop.

  arguments:    pDBInfo    [IN] - contains all information on the running
                                      session and the database connected to.
                pTLRec     [IN] - structure, holding information for
                                   executing TABLELOAD for a single table
                lPcktNo    [IN] - number of packet to fill
                ReadBuffer [IN] - read file buffer object
                pszErrText [OUT] - error text


  returnvalue:  errOK_els00 - no error
                STOP_LS28   - no more data found; process last packet
                errors from reading file
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_FillPacketBuffer(tls00_DBInfo   *pDBInfo,
                      tls00_TloadRec *pTLRec,
                      tsp00_Int4      lPcktNo,
                      ReadBuffer     &PageBuffer,
                      tsp00_Addr      pszErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls28_EndRestore
  -----------------------------------------------------------------------------

  description:  Executes the command "end restore"

  arguments:    pDBInfo    [IN]  - contains all information on the running
                                    session and the database connected to.
                pBdinfo    [IN]  - buffer holding bdinfo needed to execute command
                pszErrText [OUT] - error text

  returnvalue:  errOK_els00
                errors from executing the command

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_EndRestore(tls00_DBInfo*  pDBInfo,
                tls00_BDInfo*  pBDInfo,
                tsp00_Addr     pszErrText);

/*
  -----------------------------------------------------------------------------

  function:     ls28_BeginRestoreLong
  -----------------------------------------------------------------------------

  description:  Executes command "restore column". Is called for every single
                row in table that contains long values.

  arguments:    pDBInfo         [IN]  - contains all information on the running
                                         session and the database connected to.
                PartStruct      [IN]  - structure, holding data for parts to send
                                         to kernel (necessary for executing the cmd)
                pszColumnBdinfo [OUT] - bdinfo for row that has long values
                pszErrText      [OUT] - error text

  returnvalue:  errOK_els00       - no error
                errInternal_els98 - error executing command

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_BeginRestoreLong(tls00_DBInfo*             pDBInfo,
                      tls00_Parts4InternalSql&  PartStruct,
                      tls00_BDInfo*             pszColumnBdinfo,
                      tsp00_Addr                pszErrText);


/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  BeginChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls28ImportTablePages
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls28ImportTablePages( MetaDataDef       *&pMetaDataDef ,
                      tls00_TloadRec    *pTabLoadRec,
                      ReadBuffer        &readBuffer,
                      tsp00_Addr         ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28ImportTablePages");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    SAPDB_Int4      rc     = errOK_els00;

    TransformationDef       * pTransformationDef  = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd * pTransformationCmd  = pTransformationDef->getTransformationCmd();
    tls00_DBInfo            * pDBInfo             = pMetaDataDef->getConnection();

    //*
    //*  update transformation table and read metadata header
    //*
    if ( false == pTransformationCmd->trExclude_ls00 )
    {
         rc = ls28UpdateTransformationPackage( pMetaDataDef , pTransformationCmd, pTabLoadRec, readBuffer, ErrText);
         if (errOK_els00 != rc )
         {
             pMsgObj->ls98Msg(layTableLoad_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
             return errInternal_els98;
         }
         rc = ls28_ReadMetaHeader(pDBInfo, pTransformationCmd, pTabLoadRec, readBuffer, ErrText );
    }
    //*
    //*   load data pages
    //*
    if ( (errOK_els00 == rc) && (false == pTransformationCmd->trExclude_ls00) )
    {
        //*
        //*  get owner, tablename
        //*
        Tools_DynamicUTF8String owner = REINTERPRET_CAST(SAPDB_UTF8*,
                                                         pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp());
        Tools_DynamicUTF8String tablename = REINTERPRET_CAST(SAPDB_UTF8*,
                                                             pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp());

        //*
        //*  get source table description
        //*
        if (errOK_els00 == rc)
            rc = ls28_GetSourceTableDesc(pDBInfo, pTabLoadRec, owner, tablename, &pTransformationCmd->trPartSpec_ls00, readBuffer, ErrText);

        //*
        //*  get get source table index description
        //*
        if (errOK_els00 == rc)
            rc = ls28_GetSourceIndexDesc(pTabLoadRec, readBuffer, ErrText);

        //*
        //*    Check existence of table in target system
        //*
        //* - If table is not present and kernel version is 7.3 or lower the table is created using
        //*   the binary table description (tsp7_table_description).
        //*   If table is not present and kernel version is 7.4.3 and higher TL exits with error.
        //*
        //* - If table is present the schemes of source and target tables are compared.
        //*   If they match the table is been loaded.
        //*   If they don't match TL exits with error.
        //*
        bool bTableExists = false;
        if (errOK_els00 == rc)
        {
            rc = ls13CheckTableExistence(pDBInfo, owner, tablename, bTableExists, ErrText);
            if (errOK_els00 == rc)
            {
                if (false == bTableExists)
                {
                    // This function creates the table if not found if kernel version < 7.4.3
                    // and stops processing if kernel version >= 7.4.3
                    rc = ls28_CreateTable(pDBInfo, pTabLoadRec, tablename, readBuffer, ErrText);
                }
                else
                {
                    // Get table definition object thats used to hold information on
                    // constraints and defaults of target table
                    TableDef *pTableDef = pMetaDataDef->getRelationalModel()->getTableDef();
                    rc = pTableDef->Init(owner, tablename);
                    if (errOK_els00 == rc)
                    {
                        rc = ls28_CompareTableSchemes(pDBInfo, pTabLoadRec, pTableDef, readBuffer, tablename, ErrText);
                        if (errOK_els00 == rc)
                        {
                            // Drop existing indexes
                            rc = ls28_DropTableIndexes(pDBInfo, tablename, ErrText);

                            //*
                            //*     Check if table is empty
                            //*
                            if (errOK_els00 == rc)
                            {
                                bool bTableIsEmpty = true;

                                rc = ls28_IsTableEmpty(pDBInfo, tablename, bTableIsEmpty, ErrText);
                                if ( (errOK_els00 == rc) && (false == bTableIsEmpty) )
                                {
                                    LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, ErrText, errTLTableNotEmpty_els98,
                                                                                                      tablename.StrPtr());
                                    rc = errTLTableNotEmpty_els98;
                                }
                                // in case of an error checking the emptyness the functions reports the error and
                                // sets the return code accordingly
                            }
                        }
                    }
                }
            }   // end if (errOK_els00 == rc) for ls13CheckTableExistence
        }

        // check for cancel request
        if (errOK_els00 == rc)
            rc = ls01CheckForCancel(pDBInfo->dbiSession_ls00.sipCancelByte_ls00);


        // In case the table schema has been successfully restored we save that
        if (errOK_els00 == rc)
            rc = ls15Commit(pDBInfo, ErrText);

        //*
        //*  restore short string data
        //*
        if (errOK_els00 == rc)
        {
            pTabLoadRec->tlrPageTypeExtracting_ls00 = tptShortString_els00;

            rc = ls28_RestoreShortStrings(pDBInfo, pTabLoadRec, tablename, readBuffer, ErrText);
        }

        if (errOK_els00 == rc)
            rc = ls01CheckForCancel(pDBInfo->dbiSession_ls00.sipCancelByte_ls00);   // Check for cancel request

        //*
        //*  restore primary data
        //*
        if (errOK_els00 == rc)
        {
            pTabLoadRec->tlrPageTypeExtracting_ls00 = tptData_els00;

            rc = ls28_RestorePrimaryData(pDBInfo, pTabLoadRec, tablename, readBuffer, ErrText);
        }


        if (errOK_els00 == rc)
            rc = ls01CheckForCancel(pDBInfo->dbiSession_ls00.sipCancelByte_ls00);   // Check for cancel request

        //*
        //*  restore long string data
        //*
        if (errOK_els00 == rc)
        {
            pTabLoadRec->tlrPageTypeExtracting_ls00 = tptLongString_els00;

            rc = ls28_RestoreLongStrings(pDBInfo, pTabLoadRec, readBuffer, ErrText);
        }

        if (errOK_els00 == rc)
            rc = ls15Commit(pDBInfo, ErrText);

        if (errOK_els00 == rc)
            rc = ls01CheckForCancel(pDBInfo->dbiSession_ls00.sipCancelByte_ls00);   // Check for cancel request

        //*
        //*  create indexes
        //*
        if (errOK_els00 == rc && NULL == pTransformationDef->m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 )
            rc = ls13CreateIndexes(pDBInfo,
                                   REINTERPRET_CAST(tsp7_table_description*, pTabLoadRec->tlrExtrTableInfo_ls00.strAddr_ls00),
                                   REINTERPRET_CAST(tsp7_index_description*, pTabLoadRec->tlrExtrIndexInfo_ls00.strAddr_ls00),
                                   tablename,
                                   ErrText);
        if (errOK_els00 == rc)
        {
            rc = ls15Commit(pDBInfo, ErrText);
            pTabLoadRec->tlrTabLoaded_ls00  = true ;
        }

        // Write message in case of cancel request
        if (errCancelRequest_els98 == rc)
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, ErrText, errCancelRequest_els98);

    } // if ( false == pTransformationCmd->trExclude_ls00 )

    //*
    //*  update transformation table and read metadata trailer
    //*
    if ( errOK_els00 == rc && false == pTransformationCmd->trExclude_ls00 )
    {
         rc = ls28_ReadMetaTrailer( pDBInfo, pTransformationCmd, pTabLoadRec, readBuffer, ErrText);
         if (errOK_els00 == rc)
         {
             rc = ls28UpdateTransformationPackage( pMetaDataDef , pTransformationCmd, pTabLoadRec, readBuffer, ErrText);
             if (errOK_els00 != rc )
             {
                 pMsgObj->ls98Msg(layTableLoad_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
                 return errInternal_els98;
             }
         }
         else
         {
             //!!ErrorHandling
         }
    }

    return STATIC_CAST(tsp00_Int4, rc);
}
// ls28ImportTablePages()


/*
  -----------------------------------------------------------------------------
  EndChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  BeginChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  function:     ls28_ReadMetaHeader
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ReadMetaHeader(tls00_DBInfo             *pDBInfo,
                    tls00_TransformationCmd  *pTabLoadCmd,
                    tls00_TloadRec           *pTabLoadRec,
                    ReadBuffer               &readBuffer,
                    tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_ReadMetaHeader");

    tsp00_Int4        rc          = errOK_els00;

    LoaderMessages  *MsgInstance = LoaderMessages::Instance();

	// header to read
    //--------------------------------------------------------------------------------------------------------------
    // repserver | code | header |            | loader  |      |      | page size    | count of        | more than  |
    // swap at   | type | length | identifier | version | date | time | of extracted | pages in        | one table  |
    // extract   |      | fix 8K |            |         |      |      | db           | file (blockung) | in file ?  |
    //--------------------------------------------------------------------------------------------------------------
    // 1 byte    |  1   |  4     |      64    | 40      | 8    | 8    |    4         |  4              |   1


    _TCHAR  *pszBuffer = readBuffer.ls51GetNext();
    size_t lBufPos = 0;

    Tools_DynamicUTF8String  TabExtrStmt;

    // swap type of source database
    pTabLoadRec->tlrSwapFromTE_ls00.becomes(tsp00_SwapKind::fromInt(STATIC_CAST(int, pszBuffer[lBufPos])));
    pszBuffer += sizeof(unsigned char);

    // encoding type of source database
    pTabLoadRec->tlrSourceEncoding_ls00 = STATIC_CAST(tls00_CodeType, pszBuffer[lBufPos]);
    pszBuffer += sizeof(unsigned char);

    // header length of header
    tsp00_Int4 hlen ;
    hlen = pTabLoadRec->tlrPageSize_ls00;
    pszBuffer += sizeof(tsp00_Int4);

    // identifier
    int result = memcmp(&pszBuffer[lBufPos], HEADER_IDENTIFIER_LS00, sizeof(HEADER_IDENTIFIER_LS00));
    if (0 != result)
    {
        MsgInstance->ls98Msg(layTableLoad_els98, ErrText, errWrongTEFileType_els98,pTabLoadRec->tlrInfile_ls00->vfFileName_ls00,"wrong identifier");
        rc = errWrongTEFileType_els98;
    }
    else
    {
        // version, date, time
        lBufPos += 64 + 40 + 8 + 8;
        // pagesize
        tsp00_Int4 lExtractedPageSize = ls07Swap4BHexToLong(&pszBuffer[lBufPos], pTabLoadRec->tlrSwapFromTE_ls00,
                                                                                 ls07DefineSwap(NULL));
        lBufPos += sizeof(tsp00_Int4);

        if (lExtractedPageSize != pTabLoadRec->tlrPageSize_ls00)
        {
            MsgInstance->ls98Msg(layTableLoad_els98, ErrText, errWrongTEPageSize_els98, lExtractedPageSize,
                                                                                        pTabLoadRec->tlrPageSize_ls00);
            rc = errWrongTEPageSize_els98;
        }
        else
        {
            pTabLoadRec->tlrCount_ls00 = ls07Swap4BHexToLong(&pszBuffer[lBufPos], pTabLoadRec->tlrSwapFromTE_ls00,
                                                                                  ls07DefineSwap(NULL));

            lBufPos += sizeof(tsp00_Int4);

            // multiple tables
            pTabLoadRec->tlrMultTabInFile_ls00 = (pszBuffer[lBufPos] != 0);
            lBufPos   += sizeof(bool);
            pszBuffer += lBufPos;
        }

        // transformation statement
        TabExtrStmt.AssignRaw( REINTERPRET_CAST(unsigned char*, pszBuffer), STATIC_CAST(SAPDB_UInt, _tcslen(pszBuffer)) );

        // parse   owner, tablename from command string in   data    file header
        // compare owner, tablename to   command string from catalog table
        rc = ls28_CheckHeader ( pDBInfo, pTabLoadCmd, TabExtrStmt, ErrText );

        if (errOK_els00 == rc)
        {
            if ( true == pTabLoadCmd->trExclude_ls00 )
            {
                Loader_RTStatistics::Instance()->IncreaseCounter_TablesExcluded();
            }
        }
    }
    return rc;

}//ls28_ReadMetaHeader


/*
  -----------------------------------------------------------------------------
  function:     ls28_CheckHeader
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CheckHeader(tls00_DBInfo            *pDBInfo,
                 tls00_TransformationCmd *pTabLoadCmd,
                 Tools_DynamicUTF8String  TabExtrStmt,
                 tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_CheckHeader");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();

    tsp00_Int4 rc = errOK_els00;

    tls00_Command* pSvrCmd = new tls00_Command;
    memset(pSvrCmd, DEF_BYTE_LS00, sizeof(tls00_Command));

    // initialize the request string
    pSvrCmd->cmdReqDat_ls00 = CONST_CAST(SAPDB_Char*, TabExtrStmt.CharPtr());

    // initialize the reply string
    pSvrCmd->cmdRpyDat_ls00 = new char[MAX_REPLY_LENGTH_LS00];
    memset(pSvrCmd->cmdRpyDat_ls00, DEF_BYTE_LS00, MAX_REPLY_LENGTH_LS00);

    // initialize logging behaviour
    pSvrCmd->cmdLogging_ls00  = SAPDB_FALSE;
    pSvrCmd->cmdInternal_ls00 = SAPDB_TRUE;

    rc = ls11ParseLoadCommand( pDBInfo, pSvrCmd );

    tls00_TransformationCmd  *pTabExtrCmd = REINTERPRET_CAST(tls00_TransformationCmd*, pSvrCmd->cmdScnDat_ls00);

    if ( pSvrCmd->cmdScanRC_ls00 != srFctSqlStmt_els00 )
    {
        if ( true == pTabExtrCmd->trExclude_ls00 )
            pTabLoadCmd->trExclude_ls00 = pTabExtrCmd->trExclude_ls00;

        tsp00_Int4 _len = pTabExtrCmd->trPartSpec_ls00.psTabName_ls00.length();
        if ( (_len != pTabLoadCmd->trPartSpec_ls00.psTabName_ls00.length()))
        {
            rc = errTableToLoadNotCurrentTable_els98;
        }
        else
        {
            if (0 != memcmp(pTabExtrCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                            pTabLoadCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                            _len))
            {
                rc = errTableToLoadNotCurrentTable_els98;
            }
        }
        if (errTableToLoadNotCurrentTable_els98 == rc)

        {
             pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errTableToLoadNotCurrentTable_els98,
                                                          pTabLoadCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                                          pTabExtrCmd->trPartSpec_ls00.psTabName_ls00.asCharp());
        }
    }
    else
    {
           pMsgObj->ls98Msg(layDataLoad_els98, ErrText, errCheckHeader_els98 );
           rc = errCheckHeader_els98;
    }

    if ( NULL != pTabExtrCmd )
    {
        ls10FreeTransformation ( srFctTabExtract_els00, pSvrCmd->cmdScnDat_ls00 );
        if (NULL != pSvrCmd->cmdRpyDat_ls00)
        {
            delete [] pSvrCmd->cmdRpyDat_ls00;
            pSvrCmd->cmdRpyDat_ls00 = NULL;
        }
        if (NULL != pSvrCmd)
        {
            delete pSvrCmd;
            pSvrCmd = NULL;
        }
    }

    return rc;
}// ls28_CheckHeader

/*
  -----------------------------------------------------------------------------
  function:     ls28_GetSourceTableDesc
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetSourceTableDesc(tls00_DBInfo             *pDBInfo,
                        tls00_TloadRec           *pTabLoadRec,
                        Tools_DynamicUTF8String  &UserName,
                        Tools_DynamicUTF8String  &TableName,
                        tls00_PartSpec           *pPartSpec,
                        ReadBuffer               &readBuffer,
                        tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_GetSourceTableDesc");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();    // for error reports
    tsp00_Longint    rc           = errOK_els00;

    rc = ls28_ReadDescFromFile(readBuffer,
                               tptTableDescription_els00,
                               pTabLoadRec->tlrSwapFromTE_ls00,
                               pTabLoadRec->tlrPageSize_ls00,
                               pTabLoadRec->tlrExtrTableInfo_ls00,
                               pszErrText);
    if (errOK_els00 == rc)
    {
        tsp7_table_description *pTD = REINTERPRET_CAST(tsp7_table_description*, pTabLoadRec->tlrExtrTableInfo_ls00.strAddr_ls00);

        // get (source db-)kernel swap
        pTabLoadRec->tlrSwapFromDb_ls00 = pTD->sp7t_kernel_swap();

        // now swap the integers in description if necessary
        ls28SwapTableDescription(*pTD, pTabLoadRec->tlrSwapFromTE_ls00);

        TRACE_PRNF_MLS99(("ls28_GetSourceTableDesc", "Source DB kernel swap = %d\n", STATIC_CAST(int, pTD->sp7t_kernel_swap())));

        //*
        //*     Replace user name and table name in table description and fill
        //*     psFullTabName_ls00
        //*
        //* TL ALL or TL USER:
        //*     - table name in table description has the right value -> check it and report error in case
        //*     - user name in table description needs to be replaced (we do not compare it before replacing)
        //*
        //* TL TABLE <table name>:
        //*     - table name in table description must be replaced (no comparing beforehand)
        //*     - user name in table description must be replaced (no comparing beforehand)
        //*     - psFullTabName_ls00 and psTabName_ls00 have already the right values


        //*
        //*     Replace user name in any case
        //*
        tsp00_KnlIdentifierc szUserName;
        tsp00_Longuint       lUserNameLen  = sizeof(tsp00_KnlIdentifierc);
        tsp00_KnlIdentifierc szTableName;
        tsp00_Longuint       lTableNameLen = sizeof(tsp00_KnlIdentifierc);
        tls00_CodeType       DestCodeType  = ctAscii_els00;
        tsp00_DataType       DT;
        DT.becomes(dcha);

        // Define if DB is UCS2
        bool bDBIsUnicode = ls00IsDBUnicode(pDBInfo);
        if (true == bDBIsUnicode)
        {
            DestCodeType = ctUCS2Native_els00;
        }

        // Fill szTableName with blanks for comparing later on
        ls05InsertFillBytes(REINTERPRET_CAST(unsigned char*,szTableName.asCharp()),
                            0,
                            DT,
                            sizeof(tsp00_KnlIdentifierc),
                            bDBIsUnicode,
                            pDBInfo->dbiOwnSwap_ls00);

        rc = ls05ConvertFromUTF8(REINTERPRET_CAST(unsigned char*, szUserName.asCharp()), lUserNameLen,
                                 UserName, DestCodeType, pszErrText);
        if (errOK_els00 == rc)
        {
            rc = ls05ConvertFromUTF8(REINTERPRET_CAST(unsigned char*, szTableName.asCharp()), lTableNameLen,
                                     TableName, DestCodeType, pszErrText);;
        }

        if (errOK_els00 == rc)
        {
            //*
            //*     Replace user name
            //*
            tsp00_KnlIdentifier &UserName = pTD->sp7t_authid;

            ls05InsertFillBytes(REINTERPRET_CAST(unsigned char*,UserName.asCharp()),
                                0,
                                DT,
                                sizeof(tsp00_KnlIdentifier),
                                bDBIsUnicode,
                                pDBInfo->dbiOwnSwap_ls00);
            SAPDB_memcpy(UserName.asCharp(), szUserName.asCharp(), lUserNameLen);


            //*
            //*     Replace/check table name
            //*
            tsp00_KnlIdentifier &TabName = pTD->sp7t_tablename;

            if (pPartSpec->psPartType_ls00 > ptTable_els00)
            {   // check table name
                if (0 != memcmp(TabName.asCharp(),szTableName.asCharp(), sizeof(tsp00_KnlIdentifier)) )
                {
                    // error, table not at this position in file; report this and quit
                    pMsgInstance->ls98Msg(layTableLoad_els98, pszErrText, errTLTableDataNotFound_els98,
                                                                          pPartSpec->psTabName_ls00.asCharp(),
                                                                          pTabLoadRec->tlrInfile_ls00->vfFileName_ls00);
                    rc = errTLTableDataNotFound_els98;
                }
            }
            else
            {   // replace table name
                ls05InsertFillBytes(REINTERPRET_CAST(unsigned char*,TabName.asCharp()),
                                    0,
                                    DT,
                                    sizeof(tsp00_KnlIdentifier),
                                    bDBIsUnicode,
                                    pDBInfo->dbiOwnSwap_ls00);
                SAPDB_memcpy(TabName.asCharp(), szTableName.asCharp(), lTableNameLen);
            }
        }
    }
    else
    {
        if ( (WRONG_TE_HEADER_LS28 == rc) || (EOF_LS00 == rc) )
        {
            pMsgInstance->ls98Msg(layTableLoad_els98, pszErrText, errWrongTEFileType_els98,
                                                                  pTabLoadRec->tlrInfile_ls00->vfFileName_ls00,
                                                                  "missing table description");
            rc = errWrongTEFileType_els98;
        }
    }

    return STATIC_CAST(tsp00_Int4, rc);
}
// ls28_GetSourceTableDesc()


/*
  -----------------------------------------------------------------------------
  function:     ls28_GetSourceIndexDesc
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetSourceIndexDesc(tls00_TloadRec *pTLRec,
                        ReadBuffer     &readBuffer,
                        tsp00_Addr      pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_GetSourceIndexDesc");

    tsp00_Int4 rc = errOK_els00;

    rc = ls28_ReadDescFromFile(readBuffer,
                               tptIndexDescription_els00,
                               pTLRec->tlrSwapFromTE_ls00,
                               pTLRec->tlrPageSize_ls00,
                               pTLRec->tlrExtrIndexInfo_ls00,
                               pszErrText);
    if (errOK_els00 == rc)              // now swap the integers in description if necessary
    {
        ls28SwapIndexDescription(*(REINTERPRET_CAST(tsp7_index_description*, pTLRec->tlrExtrIndexInfo_ls00.strAddr_ls00)),
                                 pTLRec->tlrSwapFromTE_ls00);
    }
    else
    {
        if ( (WRONG_TE_HEADER_LS28 == rc) || (EOF_LS00 == rc) )
        {
            rc = errOK_els00;
        }
    }

    return rc;
}
// ls28_GetSourceIndexDesc()


/*
  -----------------------------------------------------------------------------
  function:     ls28_CreateTable
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CreateTable(tls00_DBInfo            *pDBInfo,
                 tls00_TloadRec          *pTLRec,
                 Tools_DynamicUTF8String &TableName,
                 ReadBuffer              &readBuffer,
                 tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_CreateTable");

    tsp00_Int4 rc = errOK_els00;

    //*
    //*     Create table using binary table description only if kernel
    //*     is of version 7.3 and older
    //*
    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 4, 3, 0);
    if ( (IsEqual_esp100 == vres) || (IsNewer_esp100 == vres) )
    {
        LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errTLTableNotExisting_els98,
                                                                             TableName.StrPtr());
        rc = errTLTableNotExisting_els98;
    }
    else // IsOlder_esp100 --> <= 7.3
    {
        szLocalCmdString[0] = 0;
        sp77sprintf(szLocalCmdString, lStmtLength_ls26, INTO_TABLE_STMT_LS28, TableName.StrPtr());

        tsp00_Int4 lErrorPos = 0;
        rc = ls13ExecIntSQLStmtEx(pDBInfo,
                                  0,
                                  szLocalCmdString,
                                  pTLRec->tlrExtrTableInfo_ls00.strAddr_ls00,
                                  pTLRec->tlrExtrTableInfo_ls00.strLeng_ls00,
                                  szLocalErrString,
                                  lErrorPos);
        if (errOK_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98, szLocalCmdString, szLocalErrString);
            rc = errInternSQL_els98;
        }
        else
        {
            //*
            //*     create constraints
            //*
            Studio_List ConstraintList;
            rc = ls28_GetSourceConstraintList(pDBInfo, pTLRec, readBuffer, ConstraintList, pszErrText);
            if (errOK_els00 == rc)
            {
                rc = ls28_CreateConstraint(pDBInfo, TableName, ConstraintList, pszErrText);

                ls28_EraseConstraintList(ConstraintList);
            }
            else
            {
                if (STOP_NODATA_LS28 == rc)
                {
                    // Thats no error - simply no constraints defined on source table
                    //                  -> nothing to create here
                    rc = errOK_els00;
                }
            }


            //*
            //*     create defaults
            //* this creates only user defined defaults; function type defaults like
            //* DEFAULT DATE for date type columns are already created with the INTO TABLE stmt
            //*
            Studio_List DefaultList;

            // Get only user defined defaults of the source table
            rc = ls28_GetSourceDefaultList(pDBInfo, pTLRec, readBuffer, false, DefaultList, pszErrText);
            if (errOK_els00 == rc)
            {
                rc = ls28_CreateDefault(pDBInfo, pTLRec, TableName, DefaultList, pszErrText);

                ls28_EraseDefaultList(DefaultList);
            }
            else
            {
                if (STOP_NODATA_LS28 == rc)
                {
                    // Thats no error - simply no defaults defined on source table
                    //                  -> nothing to create here
                    rc = errOK_els00;
                }
            }

        }   // end if (errOK_els00 == rc) - ls13ExecIntSQLStmtEx

        //*
        //*     Close transaction of creating table with END LOAD stmt
        //*
        if (errOK_els00 == rc)
        {
            szLocalCmdString[0]  = 0;
            lErrorPos = 0;

            sp77sprintf(szLocalCmdString, lStmtLength_ls26, END_LOAD_TABLE_STMT_LS28, TableName.StrPtr());

            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szLocalCmdString, szLocalErrString, lErrorPos);
            if (errOK_els00 != rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98, szLocalCmdString, szLocalErrString);
                rc = errInternSQL_els98;
            }
        }
    }

    return rc;
}
// ls28_CreateTable()


/*
  -----------------------------------------------------------------------------
  function:     ls28_GetSourceConstraintList
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetSourceConstraintList(tls00_DBInfo    *pDBInfo,
                             tls00_TloadRec  *pTLRec,
                             ReadBuffer      &PageBuffer,
                             Studio_List     &SrcConstraintList,
                             tsp00_Addr       pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_GetSourceConstraintList");

    tsp00_Int4 rc = errOK_els00;

    // page header variables
    tls26_DescPageHeader   *pHeader                = NULL;
    tsp00_Uint4             lConstraintCountOnPage = 0;
    tsp00_Int4              MoreCPages             = 0;
    ConstraintDescription  *pConstraint            = NULL;
    tsp00_Uint4             j                      = 0;

    do
    {
        _TCHAR *pszBuffer = PageBuffer.ls51GetNext();       // get next page from data file
        if (NULL == pszBuffer)                  // error or eof
        {
            rc = PageBuffer.ls51GetError(pszErrText);
            if (EOF_LS00 == rc)                 // shouldn't happen - we hardly have reached the end here
            {
                rc = errOK_els00;
            }
        }
        else
        {
            rc = ls28_CheckTypeHeaderPage(pszBuffer, pDBInfo->dbiPageSize_ls00, tptConstraint_els00, pTLRec->tlrSwapFromTE_ls00);
            if (errOK_els00 != rc)                              // No constraint definitions in data file even if
            {                                                   // constraint count > 0 --> must be an error
                if (WRONG_TE_HEADER_LS28 == rc)
                {
                    // Obviously not a page with constraints at all - this points to
                    // the fact that the source table did not have constraints;
                    // that's hardly an error but should be handled by the caller
                    rc = STOP_NODATA_LS28;
                }
                PageBuffer.ls51NotUsed();                       // mark page as 'not used'
            }
            else
            {
                // Read page header information
                pHeader = REINTERPRET_CAST(tls26_DescPageHeader*, pszBuffer);

                lConstraintCountOnPage = pHeader->dphSpecialID_ls26;
                MoreCPages             = pHeader->dphMore_ls26;

                // Move page buffer pointer by the amount of header size
                pszBuffer += sizeof(*pHeader);
                TRACE_PRNF_MLS99(("ls28_GetSourceConstraintList", "%d values on this page\n", lConstraintCountOnPage));


                // Read source constraint info from page and save in constraint list;
                // do it as long as constraint definitions found on that page
                for (j = 0; j < lConstraintCountOnPage; ++j)
                {
                    // To be on the safe site when handling not aligned 4 Byte integers we copy them

                    // constraints are stored in a page as follows:
                    //
                    //---------------------------------------------------
                    //| constraint 1 | constraint 2 | ... | constraint N|
                    //---------------------------------------------------
                    //
                    // where each constraint is stored as follows:
                    //
                    //------------------------------------------------------------------------------------------
                    // constraint name len | constraint definition len | constraint name | constraint definition
                    //    4 Byte           |    4 Byte                 |
                    //------------------------------------------------------------------------------------------
                    //
                    //
                    pConstraint = new ConstraintDescription;

                    SAPDB_memcpy(&pConstraint->ConstraintNameLength, pszBuffer, sizeof(tsp00_Int4));
                    pszBuffer += sizeof(tsp00_Int4);
                    SAPDB_memcpy(&pConstraint->ConstraintValueLength, pszBuffer, sizeof(tsp00_Int4));
                    pszBuffer += sizeof(tsp00_Int4);

                    pConstraint->ConstraintName.AssignRaw(REINTERPRET_CAST(unsigned char*, pszBuffer), pConstraint->ConstraintNameLength);
                    pszBuffer += pConstraint->ConstraintNameLength;

                    pConstraint->ConstraintValue.AssignRaw(REINTERPRET_CAST(unsigned char*, pszBuffer), pConstraint->ConstraintValueLength);
                    pszBuffer += pConstraint->ConstraintValueLength;

                    SrcConstraintList.append(pConstraint);

                    pConstraint = NULL;     // reinitialize
                }
            }
        }   // end else branch of if (NULL == pszBuffer)
    }
    while ( (0 != MoreCPages) && (errOK_els00 == rc) );

    return rc;
}
// ls28_GetSourceConstraintList()


/*
  -----------------------------------------------------------------------------
  function:     ls28_CreateConstraint
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CreateConstraint(tls00_DBInfo            *pDBInfo,
                      Tools_DynamicUTF8String &TableName,
                      Studio_List             &ConstraintList,
                      tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_CreateConstraint");

    tsp00_Int4 rc = errOK_els00;


    // ALTER TABLE <table name> ADD CONSTRAINT <constraint_name> CHECK <search_condition>
    if (0 != ConstraintList.size())
    {
        ConstraintDescription *pConstraint  = NULL;
        tsp00_Uint2         j         = 0;
        tsp00_Int4          lErrorPos = 0;

        for (pConstraint = REINTERPRET_CAST(ConstraintDescription*, ConstraintList.first());
             ( (errOK_els00 == rc) && (pConstraint) );
             pConstraint = REINTERPRET_CAST(ConstraintDescription*, ConstraintList.next()))
        {
            szLocalCmdString[0] = 0;

            // Build command to create CONSTRAINT value
            sp77sprintf(szLocalCmdString, lStmtLength_ls26, "ALTER TABLE %s ADD CONSTRAINT %s CHECK %s",
                                                            TableName.StrPtr(),
                                                            pConstraint->ConstraintName.StrPtr(),
                                                            pConstraint->ConstraintValue.StrPtr());

            // Create the CONSTRAINT value
            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szLocalCmdString, szLocalErrString, lErrorPos);
            if (0 != rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98,
                                                                                    szLocalCmdString, szLocalErrString);
                rc = errInternSQL_els98;
            }

        }   // end for (pDefault = REINTERPRET_CAST(DefaultDescription*, Defaults.first()); ...

    }   // end if (0 != Defaults.size())

    return rc;
}
// ls28_CreateConstraint()


/*
  -----------------------------------------------------------------------------
  function:     ls28_EraseConstraintList
  -----------------------------------------------------------------------------
*/
static void
ls28_EraseConstraintList(Studio_List &ConstraintList)
{
    if (0 != ConstraintList.size())
    {
        ConstraintDescription *pConstraint = REINTERPRET_CAST(ConstraintDescription*, ConstraintList.first());

        while (NULL != pConstraint)
        {
            delete pConstraint;
            pConstraint = NULL;
            pConstraint = REINTERPRET_CAST(ConstraintDescription*, ConstraintList.next());
        }
//      ConstraintList.clear();     // done by destructor of list class
    }
}
// ls28_EraseConstraintList()


/*
  -----------------------------------------------------------------------------
  function:     ls28_GetSourceDefaultList
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_GetSourceDefaultList(tls00_DBInfo    *pDBInfo,
                          tls00_TloadRec  *pTLRec,
                          ReadBuffer      &PageBuffer,
                          bool             bAllDefaults,
                          Studio_List     &SrcDefaultList,
                          tsp00_Addr       pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_GetSourceDefaultList");

    tsp00_Int4 rc = errOK_els00;

    // page header variables
    tls26_DescPageHeader  *pHeader                  = NULL;
    tsp00_Uint4            lDefaultCountOnPage      = 0;
    tsp00_Int4             MoreDPages               = 0;        // Initialize to 'false' for first run

    tsp00_Uint2            lSrcDefaultCount         = 0;
    DefaultDescription    *pDefault = NULL;

    tsp00_Uint2            lDefLen = 0;
    bool                   bUserDefined = false;
    tsp00_Uint4            j = 0;


    do
    {
        _TCHAR *pszBuffer = PageBuffer.ls51GetNext();       // get next page from data file
        if (NULL == pszBuffer)                              // error or eof
        {
            rc = PageBuffer.ls51GetError(pszErrText);
            if (EOF_LS00 == rc)                             // shouldn't happen - we hardly have reached the end here
            {
                rc = errOK_els00;
            }
        }
        else
        {
            rc = ls28_CheckTypeHeaderPage(pszBuffer, pDBInfo->dbiPageSize_ls00, tptDefault_els00, pTLRec->tlrSwapFromTE_ls00);
            if (errOK_els00 != rc)
            {
                if (WRONG_TE_HEADER_LS28 == rc)
                {
                    // Obviously not a page with defaults at all - this points to
                    // the fact that the source table did not have defaults;
                    // that's hardly an error but should be handled by the caller
                    rc = STOP_NODATA_LS28;
                }
                PageBuffer.ls51NotUsed();                   // mark page as 'not used'
            }
            else
            {
                // Read page header information
                pHeader             = REINTERPRET_CAST(tls26_DescPageHeader*, pszBuffer);
                lSrcDefaultCount    = pHeader->dphFillerInt_ls26;       // overall number of defaults on source table
                lDefaultCountOnPage = pHeader->dphSpecialID_ls26;
                MoreDPages          = pHeader->dphMore_ls26;

                // Move page buffer pointer by the amount of header size
                pszBuffer += sizeof(*pHeader);
                TRACE_PRNF_MLS99(("ls28_GetSourceDefaultList", "%d values on this page\n", lDefaultCountOnPage));

                //*
                //*     Read source default info from page
                //*
                for (j = 0; (errOK_els00 == rc) && (j < lDefaultCountOnPage); ++j)
                {
                    // Defaults are saved in pos order of columns in table as follows
                    //
                    //------------------------------------------
                    //| default 1 | default 2 | ... | default N|
                    //------------------------------------------
                    //
                    // where each default is stored as follows:
                    //
                    //-------------------------------------------------------------------------------
                    //|     default type     | no of col default | default len | default definition   |
                    //| function or user def |  is defined for   |             |                      |
                    //|        1 Byte        |     2 Byte        |    2 Byte   |  default len Byte    |
                    //-------------------------------------------------------------------------------
                    //
                    //

                    bUserDefined = (*pszBuffer != 0);                       // Info if default value is user defined value
                    pszBuffer += sizeof(char);

                    //*
                    //*     Check if caller wants all defaults or only the user defined values
                    //*
                    if ( (false == bUserDefined) && (false == bAllDefaults) )
                    {
                        // Caller is not interested in defaults that are not user defined
                        // values - skip those
                        pszBuffer += sizeof(tsp00_Uint2);                   // Skip column number info

                        // Default value length
                        SAPDB_memcpy(&lDefLen, pszBuffer, sizeof(tsp00_Uint2));   // Get default value length
                        pszBuffer += sizeof(tsp00_Uint2);

                        pszBuffer += lDefLen;                               // Skip default value
                    }
                    else
                    {
                        // Add requested default values to the list

                        pDefault = new DefaultDescription;                  // Create new default list entry

                        pDefault->IsUserDefined  = bUserDefined;            // Info if user defined value

                        // We're sure here to have no swap problem ??!!
                        SAPDB_memcpy(&pDefault->DefaultColumnNo, pszBuffer, sizeof(tsp00_Uint2));     // Column number
                        pszBuffer += sizeof(tsp00_Uint2);

                        SAPDB_memcpy(&pDefault->DefaultValueLength, pszBuffer, sizeof(tsp00_Uint2));  // Default value length
                        pszBuffer += sizeof(tsp00_Uint2);

                        // Default value itself
                        pDefault->DefaultValue.AssignRaw(REINTERPRET_CAST(unsigned char*, pszBuffer), pDefault->DefaultValueLength);
                        pszBuffer += pDefault->DefaultValueLength;

                        SrcDefaultList.append(pDefault);


                        pDefault = NULL;        // reinitialize
                    }

                }   // end for (j = 0; (errOK_els00 == rc) && (j < lDefaultCountOnPage); ++j)

            }   // end else of if (errOK_els00 != rc)

        }   // end else branch of if (NULL == pszBuffer)
    }
    while ( (0 != MoreDPages) && (errOK_els00 == rc) );

    return rc;
}
// ls28_GetSourceDefaultList()


/*
  -----------------------------------------------------------------------------
  function:     ls28_CreateDefault
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CreateDefault(tls00_DBInfo            *pDBInfo,
                   tls00_TloadRec          *pTLRec,
                   Tools_DynamicUTF8String &TableName,
                   Studio_List             &DefaultList,
                   tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_CreateDefault");

    tsp00_Int4 rc = errOK_els00;

    //*
    //*     Coming here ALL defaults in the list are user defined
    //*
    //* ALTER TABLE <table name> COLUMN <column name> ADD DEFAULT <default definition>
    //*
    if (0 != DefaultList.size())
    {
        //*
        //*     Get source table column names
        //*
        tsp7_table_description   *pTabDesc         = REINTERPRET_CAST(tsp7_table_description*,
                                                                      pTLRec->tlrExtrTableInfo_ls00.strAddr_ls00);
        Tools_DynamicUTF8String  *pColumnName_UTF8 = new Tools_DynamicUTF8String[pTabDesc->sp7t_field_cnt() + 1];

        //*
        //*     Define encoding type for names in table descriptions;
        //*     because we allow only extracting/loading between databases of the same
        //*     encoding type we can define the encoding type here using the target database
        //*
        tls00_CodeType SrcEncoding = ctAscii_els00;
        if (true == ls00IsDBUnicode(pDBInfo))
        {
            SrcEncoding = ctUCS2_els00;     // not swapped!
        }

        rc = ls13GetColumnNamesInDefOrder(pTabDesc, pColumnName_UTF8, SrcEncoding, pszErrText);

        tsp00_Uint2 skipsyskeycolumn = (pColumnName_UTF8[1] == "SYSKEY") ? 1 : 0;

        tsp7_ffield_info   *pSrcField = NULL;
        DefaultDescription *pDefault  = NULL;
        tsp00_Uint2         colnum    = 0;
        tsp00_Uint2         j         = 0;
        tsp00_Int4          lErrorPos = 0;

        for (pDefault = REINTERPRET_CAST(DefaultDescription*, DefaultList.first());
             ( (errOK_els00 == rc) && (pDefault) );
             pDefault = REINTERPRET_CAST(DefaultDescription*, DefaultList.next()))
        {
            szLocalCmdString[0] = 0;

            colnum = pDefault->DefaultColumnNo + skipsyskeycolumn;

            // Get the column with the corresponding position - we need the data type of the
            // column to create the default for to build the right statement
            for (j = 0, pSrcField = &(pTabDesc->sp7t_fields()[j]);
                 colnum != pSrcField->sp7fi_def_col_no;
                 ++j, pSrcField = &(pTabDesc->sp7t_fields()[j]))
                ;

            // Build command to create DEFAULT value - in case of a numeric column
            // don't use quotes to mask the default value
            if (1 == szMap_NumberDataType_ls00[pSrcField->sp7fi_data_type])
            {
                sp77sprintf(szLocalCmdString, lStmtLength_ls26, "ALTER TABLE %s COLUMN %s ADD DEFAULT %.*s",
                                                                TableName.StrPtr(),
                                                                pColumnName_UTF8[colnum].StrPtr(),
                                                                pDefault->DefaultValueLength,
                                                                pDefault->DefaultValue.StrPtr());
            }
            else
            {
                sp77sprintf(szLocalCmdString, lStmtLength_ls26, "ALTER TABLE %s COLUMN %s ADD DEFAULT '%.*s'",
                                                                TableName.StrPtr(),
                                                                pColumnName_UTF8[colnum].StrPtr(),
                                                                pDefault->DefaultValueLength,
                                                                pDefault->DefaultValue.StrPtr());
            }

            // Create the DEFAULT value finally
            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szLocalCmdString, szLocalErrString, lErrorPos);
            if (0 != rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98,
                                                                                    szLocalCmdString, szLocalErrString);
                rc = errInternSQL_els98;
            }

        }   // end for (pDefault = REINTERPRET_CAST(DefaultDescription*, Defaults.first()); ...


        if (NULL != pColumnName_UTF8)
        {
            delete [] pColumnName_UTF8 ;
            pColumnName_UTF8 = NULL;
        }

    }   // end if (0 != Defaults.size())

    return rc;
}
// ls28_CreateDefault()


/*
  -----------------------------------------------------------------------------
  function:     ls28_EraseDefaultList
  -----------------------------------------------------------------------------
*/
static void
ls28_EraseDefaultList(Studio_List &DefaultList)
{
    if (0 != DefaultList.size())
    {
        DefaultDescription *pDefault = REINTERPRET_CAST(DefaultDescription*, DefaultList.first());

        while (NULL != pDefault)
        {
            delete pDefault;
            pDefault = NULL;
            pDefault = REINTERPRET_CAST(DefaultDescription*, DefaultList.next());
        }
//      DefaultList.clear();        // done by destructor of list class
    }
}
// ls28_EraseDefaultList()


/*
  -----------------------------------------------------------------------------
  function:     ls28_CompareTableSchemes
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls28_CompareTableSchemes(tls00_DBInfo*             pDBInfo,
                         tls00_TloadRec*           pTLRec,
                         TableDef*                 pTableDef,
                         ReadBuffer               &readBuffer,
                         Tools_DynamicUTF8String  &TableName,
                         tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_CompareTableSchemes");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    tsp00_Longint    rc      = errOK_els00;

    // variables used to hold binary description of target table
    tsp00_Int4 lDescLen = 0;
    tsp00_Addr pszDesc  = NULL;

    //*
    //*     Get constraint and default definitions of target table
    //*
    rc = pTableDef->readConstraintDef(); 
    if (errOK_els00 == rc)
    {
        rc = pTableDef->readDefaultDef();
    }

    //*
    //*     Get binary table description of target table
    //*
    if (errOK_els00 == rc)
    {
        rc = ls13GetTableDescription(pDBInfo, 0, TableName, lDescLen, pszDesc, pszErrText);
    }

    //*
    //*     Compare target and source table schemes
    //*
    if (errOK_els00 == rc)
    {
        char          txt[MAX_REPLY_LENGTH_LS00];
        tsp00_Longint lError    = 0;
        txt[0]                  = 0;

        tsp7_table_description *SrcTabDesc = REINTERPRET_CAST(tsp7_table_description*, pTLRec->tlrExtrTableInfo_ls00.strAddr_ls00);
        tsp7_table_description *TgtTabDesc = REINTERPRET_CAST(tsp7_table_description*, pszDesc);

        //*
        //*     First comparison: column count (dropped columns are counted??)
        //*


//TODOTODO INFORMATIONEN UEBER GELOESCHTE SPALTEN IN 7.3 UND LOESUNGSMOEGLICHKEIT


        if (SrcTabDesc->sp7t_field_cnt() != TgtTabDesc->sp7t_field_cnt())
        {
            sp77sprintf(txt, MAX_REPLY_LENGTH_LS00, "different field count: target = %d, source = %d",
                                                    TgtTabDesc->sp7t_field_cnt(), SrcTabDesc->sp7t_field_cnt());
            pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), txt);
            ++lError;
            // That's all we need to know to stop
        }
        else
        {
            //*
            //*     Column count of target and source table are equal; now compare
            //*     the length of the binary descriptions
            //*
            if (pTLRec->tlrExtrTableInfo_ls00.strLeng_ls00 != lDescLen)
            {
                ++lError;
            }
            else
            {
                SAPDB_Char* _pszAuxStr = new SAPDB_Char[512];
                _pszAuxStr[0] = 0;
                sp77sprintf(_pszAuxStr, 512, ""NEW_LINE_LS00"//\t\tMake sure that the database parameter COLUMNCOMPRESSION has got the same value in the source and target databases."
                                             ""NEW_LINE_LS00"//\t\tIf the source database version is < 7.6 and the target database version is >= 7.6, the parameter COLUMNCOMPRESSION must be set to NO in the target database.");
                //*
                //*     Length of binary descriptions is equal; now compare
                //*     the table description structures
                //* Doing that log the differences in definition of table in source and target system
                //*
                if (0 != memcmp(SrcTabDesc, TgtTabDesc, lDescLen))
                {
                    //*
                    //*     Kernel swap (sp7t_kernel_swap()) - must not be equal
                    //*
                    //*  different swap is handled by load server internally

                    //*
                    //*     Table kind (syskey or not that's the question) - must be equal
                    //*
                    if (SrcTabDesc->sp7t_table_kind() != TgtTabDesc->sp7t_table_kind())
                    {
                        sp77sprintf(txt, MAX_REPLY_LENGTH_LS00, "table kind: target = %d, source = %d",
                                                                TgtTabDesc->sp7t_table_kind(), SrcTabDesc->sp7t_table_kind());
                        pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), txt);
                        txt[0] = 0;

                        ++lError;
                    }

                    // sp7t_kernel_loc: not relevant - leftover from times of distributed database

                    //*
                    //*     Constraint count - must be equal
                    //*
                    if (SrcTabDesc->sp7t_constraint_cnt() != TgtTabDesc->sp7t_constraint_cnt())
                    {
                        sp77sprintf(txt, MAX_REPLY_LENGTH_LS00, "constraint count: target = %d, source = %d",
                                                                TgtTabDesc->sp7t_constraint_cnt(), SrcTabDesc->sp7t_constraint_cnt());
                        pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), txt);
                        txt[0] = 0;

                        ++lError;
                    }

                    tsp7_ffield_info         *pSrcField           = NULL;
                    tsp7_ffield_info         *pTgtField           = NULL;
                    Tools_DynamicUTF8String  *pTgtColumnName_UTF8 = new Tools_DynamicUTF8String[TgtTabDesc->sp7t_field_cnt() + 1];

                    //*
                    //*     Define encoding type for names in table descriptions;
                    //*     because we allow only extracting/loading between databases of the same
                    //*     encoding type we can define the encoding type here using the target database
                    //*
                    tls00_CodeType SrcEncoding = ctAscii_els00;
                    if (true == ls00IsDBUnicode(pDBInfo))
                    {
                        SrcEncoding = ctUCS2_els00;     // not swapped!
                    }

                    //*
                    //*     Get target table column names; they're needed in any case for all subsequent comparisons
                    //*
                    rc = ls13GetColumnNamesInRecordOrder(TgtTabDesc, pTgtColumnName_UTF8, SrcEncoding, pszErrText);

                    //*
                    //*     Compare column sorting in the record structure in src and tgt table
                    //* We assume that columns in src and tgt table are equally named.
                    //* If sorting is not equal we can stop here.
                    //*
                    tsp00_Int4  lColNameBufLen   = lDescLen - sizeof(tsp7_ffield_info) * TgtTabDesc->sp7t_field_cnt() -
                                                    sizeof(tsp7_head_buf) - 2*sizeof(tsp00_KnlIdentifier);
                    _TCHAR     *pszTgtColNameBuf = TgtTabDesc->sp7t_colnames().asCharp() + sizeof(tsp7_ffield_info) * TgtTabDesc->sp7t_field_cnt();
                    _TCHAR     *pszSrcColNameBuf = SrcTabDesc->sp7t_colnames().asCharp() + sizeof(tsp7_ffield_info) * SrcTabDesc->sp7t_field_cnt();
                    if (0 != memcmp(pszTgtColNameBuf, pszSrcColNameBuf, lColNameBufLen))
                    {
                        // first increase error counter
                        ++lError;

                        // create error message
                        tsp00_Int4 ErrTxtLen        = 2*TgtTabDesc->sp7t_field_cnt()*sizeof(tsp00_KnlIdentifierc) + MAX_REPLY_LENGTH_LS00;
                        tsp00_Int4 lBufCnt          = 0;
                        _TCHAR*    pszColSortErrTxt = new _TCHAR[ErrTxtLen];
                        pszColSortErrTxt[0] = 0;

                        lBufCnt = sp77sprintf(pszColSortErrTxt, MAX_REPLY_LENGTH_LS00, "different column sorting of source and target table");

                        //*
                        //*     Get source table column names
                        //*
                        Tools_DynamicUTF8String *pSrcColumnName_UTF8 = new Tools_DynamicUTF8String[SrcTabDesc->sp7t_field_cnt() + 1];
                        rc = ls13GetColumnNamesInRecordOrder(SrcTabDesc, pSrcColumnName_UTF8, SrcEncoding, pszErrText);

                        lBufCnt += sp77sprintf(pszColSortErrTxt + lBufCnt, ErrTxtLen - lBufCnt,
                                                                           ""NEW_LINE_LS00"//\t\t%-32s%-32s",
                                                                           "source table columns", "target table columns");
                        for (int k = 0; k < TgtTabDesc->sp7t_field_cnt(); ++k)
                        {
                            lBufCnt += sp77sprintf(pszColSortErrTxt + lBufCnt, ErrTxtLen - lBufCnt,
                                                                               ""NEW_LINE_LS00"//\t\t%-32s%-32s",
                                                                               pSrcColumnName_UTF8[k].StrPtr(), pTgtColumnName_UTF8[k].StrPtr());
                        }
                        lBufCnt += sp77sprintf(pszColSortErrTxt + lBufCnt, ErrTxtLen - lBufCnt, _pszAuxStr);
                        // Finally print the message
                        pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), pszColSortErrTxt);

                        delete [] pSrcColumnName_UTF8;
                        delete [] pszColSortErrTxt;
                    }

                    //*
                    //*     Compare single column information
                    //*
                    if (0 == lError)
                    {
                        SAPDB_Int4 i = 0;
                        SAPDB_Bool bEqual = SAPDB_FALSE;
                        for (i = 0; i < TgtTabDesc->sp7t_field_cnt(); ++i)
                        {
                            SAPDB_Bool bEqual = SAPDB_FALSE;

                            pSrcField = &(SrcTabDesc->sp7t_fields()[i]);
                            pTgtField = &(TgtTabDesc->sp7t_fields()[i]);

                            //*
                            //*     Data type - must be equal
                            //*
                            if ( tsp00_DataType_Enum(pSrcField->sp7fi_data_type) !=
                                 tsp00_DataType_Enum(pTgtField->sp7fi_data_type)  )
                            {
                                if ( ( (dcha        == tsp00_DataType_Enum(pSrcField->sp7fi_data_type)) &&
                                       (dvarchara   == tsp00_DataType_Enum(pTgtField->sp7fi_data_type) ) ) ||
                                     ( (dche        == tsp00_DataType_Enum(pSrcField->sp7fi_data_type)) &&
                                       (dvarchare   == tsp00_DataType_Enum(pTgtField->sp7fi_data_type) ) ) ||
                                     ( (dchb        == tsp00_DataType_Enum(pSrcField->sp7fi_data_type)) &&
                                       (dvarcharb   == tsp00_DataType_Enum(pTgtField->sp7fi_data_type) ) ) ||
                                     ( (dunicode    == tsp00_DataType_Enum(pSrcField->sp7fi_data_type)) &&
                                       (dvarcharuni == tsp00_DataType_Enum(pTgtField->sp7fi_data_type) ) ) )
                                {
                                    // VARCHAR in target system means ALWAYS a length type of sp7_var_len or sp7_var_len_long.
                                    // To have 2 characters equal the length type must be the same
                                    if (tsp7_len_type_Enum(pSrcField->sp7fi_len_type) ==
                                        tsp7_len_type_Enum(pTgtField->sp7fi_len_type))
                                    {
                                        bEqual = SAPDB_TRUE;
                                    }
                                }

                                if (SAPDB_FALSE == bEqual)
                                {
                                    sp77sprintf(txt, MAX_REPLY_LENGTH_LS00, "data type of %s: source = %s, target = %s",
                                                                            pTgtColumnName_UTF8[i].StrPtr(),
                                                                            ls28_SQLTypeToString(pSrcField->sp7fi_data_type),
                                                                            ls28_SQLTypeToString(pTgtField->sp7fi_data_type));
                                    pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), txt);
                                    txt[0] = 0;
                                    ++lError;
                                }
                            }

                            //*
                            //*     Fraction - must be equal
                            //*
                            if (pSrcField->sp7fi_frac != pTgtField->sp7fi_frac)
                            {
                                sp77sprintf(txt, MAX_REPLY_LENGTH_LS00, "fraction of %s: source = %d, target = %d",
                                                                        pTgtColumnName_UTF8[i].StrPtr(),
                                                                        pSrcField->sp7fi_frac, pTgtField->sp7fi_frac);
                                pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), txt);
                                txt[0] = 0;
                                ++lError;
                            }

                            //*
                            //*     Column definition length - must be equal
                            //*
                            if (pSrcField->sp7fi_len != pTgtField->sp7fi_len)
                            {
                                sp77sprintf(txt, MAX_REPLY_LENGTH_LS00, "length of %s: source = %d, target = %d",
                                                                        pTgtColumnName_UTF8[i].StrPtr(),
                                                                        pSrcField->sp7fi_len, pTgtField->sp7fi_len);
                                pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), txt);
                                txt[0] = 0;
                                ++lError;
                            }

                            //*
                            //*     Deliver length using order interface (sp7fi_in_out_l) - must be equal
                            //*
                            //* But actually of no interest because the definition length says it all


                            //*
                            //*     Column mode - target = mandatory and source = optional is
                            //*                   the only allowed combination
                            //*
                            //* Possible values: mod_syskey
                            //*                  mod_key
                            //*                  mod_mandatory
                            //*                  mod_optional
                            //*                  mod_dropped
                            //*
                            if ( tsp00_ColMode_Enum(pSrcField->sp7fi_col_mode) !=
                                 tsp00_ColMode_Enum(pTgtField->sp7fi_col_mode)  )
                            {
                                if ( (mod_mandatory == pSrcField->sp7fi_col_mode) &&
                                     (mod_optional == pTgtField->sp7fi_col_mode)   )
                                {
                                    // nothing to do here - this difference is allowed
                                }
                                else
                                {
                                    sp77sprintf(txt, MAX_REPLY_LENGTH_LS00, "column mode of %s: source = %s, target = %s",
                                                                            pTgtColumnName_UTF8[i].StrPtr(),
                                                                            pszColumnMode_ls28[pSrcField->sp7fi_col_mode],
                                                                            pszColumnMode_ls28[pTgtField->sp7fi_col_mode]);
                                    pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), txt);
                                    txt[0] = 0;
                                    ++lError;
                                }
                            }

                            //*
                            //*     Kind of default - must be equal
                            //* This is checked in a special function if no other error occurred

                        }   // end for (int i = 0; i < TgtTabDesc->sp7t_field_cnt(); ++i)

                        //*
                        //*     Length type - must be equal
                        //*
                        //* Possible values: sp7_fix_len
                        //*                  sp7_var_len
                        //*                  sp7_var_len_long
                        SAPDB_Int4 _pos = 0;
                        SAPDB_Bool _bLengthTypeProblems = SAPDB_FALSE;

                        txt[0] = 0; // reinitialize
                        for (i = 0; i < TgtTabDesc->sp7t_field_cnt(); ++i)
                        {
                            SAPDB_Bool bEqual = SAPDB_FALSE;

                            pSrcField = &(SrcTabDesc->sp7t_fields()[i]);
                            pTgtField = &(TgtTabDesc->sp7t_fields()[i]);
                            if ( tsp7_len_type_Enum(pSrcField->sp7fi_len_type) !=
                                 tsp7_len_type_Enum(pTgtField->sp7fi_len_type)  )
                            {
                                _pos += sp77sprintf(txt + _pos, MAX_REPLY_LENGTH_LS00 - _pos,
                                                                ""NEW_LINE_LS00"//\t\tlength type of column \"%s\": source = %s, target = %s",
                                                                pTgtColumnName_UTF8[i].StrPtr(),
                                                                pszLengthType_ls28[pSrcField->sp7fi_len_type],
                                                                pszLengthType_ls28[pTgtField->sp7fi_len_type]);
                                _bLengthTypeProblems = SAPDB_TRUE;
                                ++lError;
                            }
                        }
                        if (SAPDB_TRUE == _bLengthTypeProblems)
                        {
                            _pos += sp77sprintf(txt + _pos, MAX_REPLY_LENGTH_LS00 - _pos, _pszAuxStr);
                            pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98, TableName.StrPtr(), txt);
                        }

                    }   // end if (0 == lError)

                    delete [] pTgtColumnName_UTF8;
                }   // end if (0 != memcmp(SrcTabDesc, TgtTabDesc, lDescLen))

                delete [] _pszAuxStr;
            }   // end if (pTLRec->tlrExtrTableInfo_ls00.strLeng_ls00 == lDescLen)

        }   // end else branch of if (SrcTabDesc->sp7t_field_cnt() != TgtTabDesc->sp7t_field_cnt())

        if (0 == lError)
        {
            //*
            //*     Compare constraint definitions of source and target table
            //* We know at this point if constraints exist at all from the binary table description
            //*
            rc = ls28_CompareConstraintDefinitions(pDBInfo, pTableDef, pTLRec, readBuffer, pszErrText);
            if (errOK_els00 == rc)
            {
                //*
                //*     Compare default definitions of source and target table
                //*
                rc = ls28_CompareDefaultDefinitions(pDBInfo, pTableDef, pTLRec, readBuffer, pszErrText);
                if (errOK_els00 != rc)
                {
                    pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98,
                                                                  TableName.StrPtr(),
                                                                  "default definitions differ");
                    ++lError;
                }
            }
            else
            {
                pMsgObj->ls98PrintMessage(layTableLoad_els98, errTLSchemeDifferences_els98,
                                                              TableName.StrPtr(),
                                                              "constraint definitions differ");
                ++lError;
            }
        }

        if (lError > 0)
        {
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errTLTableNotRestored_els98,
                                                                                 TableName.StrPtr());
            rc = errTLTableNotRestored_els98;
        }

    }   // end if (errOK_els00 == rc)
    return rc;
}
// ls28_CompareTableSchemes()


/*
  -----------------------------------------------------------------------------
  function:     ls28_CompareConstraintDefinitions
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls28_CompareConstraintDefinitions(tls00_DBInfo*    pDBInfo,
                                  TableDef*        pTableDef,
                                  tls00_TloadRec*  pTLRec,
                                  ReadBuffer&      PageBuffer,
                                  tsp00_Addr       pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_CompareConstraintDefinitions");

    tsp00_Int4 rc = errOK_els00;

    // Coming here we know:
    // - variable pTableDef contains a list of all constraints of the target table
    // - constraint count in target and source db are the same - either 0 or not

    tsp00_Uint4 lTgtConstraintCount = pTableDef->constraintCount();

    if (0 != lTgtConstraintCount)
    {
        // Get all constraints of the source table
        Studio_List SrcConstraintList;

        rc = ls28_GetSourceConstraintList(pDBInfo, pTLRec, PageBuffer, SrcConstraintList, pszErrText);
        if (errOK_els00 != rc)
        {
            if (STOP_NODATA_LS28 == rc)     // shouldn't happen!
            {
                rc = errTLSchemeDifferences_els98;
            }
            // else - here is nothing to do; no data expected no data gotten
        }
        else
        {
            ConstraintDescription   *pSrcConstraint      = NULL;
            Tools_DynamicUTF8String TgtConstraintDef;
            Tools_DynamicUTF8String SrcConstraintDef;
            tsp00_Uint4             lTempConstraintCount = 0;
            tsp00_Uint2             lErrorCnt            = 0;
            tsp00_Int4              cindex               = 0;
            tsp00_Uint4             i                    = 0;

            //*
            //*     Iterate through the source constraints and find them in the target table definition
            //*
            for (pSrcConstraint = REINTERPRET_CAST(ConstraintDescription*, SrcConstraintList.first());
                 ( (errOK_els00 == rc) && (pSrcConstraint) );
                 pSrcConstraint = REINTERPRET_CAST(ConstraintDescription*, SrcConstraintList.next()))
            {

                // Try to find the src constraint in the tgt constraint list
                // if not found the return value is -1
                cindex = pTableDef->findConstraint(pSrcConstraint->ConstraintName);
                if (-1 != cindex)
                {
                    // Get the indexed constraint and compare src and tgt constraint definition
                    TgtConstraintDef = pTableDef->constraint(cindex)->definition();
                    SrcConstraintDef = pSrcConstraint->ConstraintValue;

                    // Normalize strings before comparing
                    ls28_NormalizeConstraintDefinitions(TgtConstraintDef, SrcConstraintDef);

                    if (SrcConstraintDef.BasisSize() == TgtConstraintDef.BasisSize())
                    {
                        if (0 == TgtConstraintDef.Compare(SrcConstraintDef))
                        {
                            // both are identical; remove the constraint from the list
                            pTableDef->removeConstraint(cindex);
                        }
                        else
                        {
                            rc = errTLSchemeDifferences_els98;
                        }
                    }
                    else
                    {
                        rc = errTLSchemeDifferences_els98;
                    }
                }
                else
                {
                    // Constraint not found by name - check all definitions with the src definition

                    // Re-compute the tgt constraint count here to search only
                    // those constraints not yet deleted from tgt constraint list
                    lTempConstraintCount = pTableDef->constraintCount();

                    for (i = 0; ( (errOK_els00 == rc) && (i < lTempConstraintCount) ); ++i)
                    {
                        TgtConstraintDef = pTableDef->constraint(i)->definition();
                        SrcConstraintDef = pSrcConstraint->ConstraintValue;

                        // Normalize strings before comparing
                        ls28_NormalizeConstraintDefinitions(TgtConstraintDef, SrcConstraintDef);

                        if (SrcConstraintDef.BasisSize() == TgtConstraintDef.BasisSize())
                        {
                            // same size; compare the definitions
                            if (0 == TgtConstraintDef.Compare(SrcConstraintDef))
                            {
                                // both are identical; remove the constraint from the list
                                pTableDef->removeConstraint(i);
                                break;
                            }
                            else
                            {
                                rc = errTLSchemeDifferences_els98;
                            }
                        }
                    }
                }   // end else of if (-1 != cindex)

            }   // end for (pSrcConstraint = REINTERPRET_CAST(ConstraintDescription*, Constraints.first()); ...

        }   // end else of if (errOK_els00 != rc)

        // Some cleaning
        ls28_EraseConstraintList(SrcConstraintList);
    }   // end if (0 != lTgtConstraintCount)

    return rc;
}
// ls28_CompareConstraintDefinitions()


/*
  -----------------------------------------------------------------------------
  function:     ls28_NormalizeConstraintDefinitions
  -----------------------------------------------------------------------------
*/
void
ls28_NormalizeConstraintDefinitions(Tools_DynamicUTF8String&  TgtConstraintDef,
                                    Tools_DynamicUTF8String&  SrcConstraintDef)
{
    // Normalize strings before comparing
    TgtConstraintDef.Trim();
    SrcConstraintDef.Trim();


    SAPDB_UInt4 _len = SrcConstraintDef.BasisSize();
    SAPDB_Char* _pszSrc = new SAPDB_Char[_len + 1];
    ls18NormalizeString(SrcConstraintDef.CharPtr(), SrcConstraintDef.Size(), _pszSrc, ctUTF8_els00);
    SrcConstraintDef = REINTERPRET_CAST(SAPDB_UTF8*, _pszSrc);
    delete [] _pszSrc;

    _len = TgtConstraintDef.BasisSize();
    SAPDB_Char* _pszTgt = new SAPDB_Char[_len + 1];
    ls18NormalizeString(TgtConstraintDef.CharPtr(), TgtConstraintDef.Size(), _pszTgt, ctUTF8_els00);
    TgtConstraintDef = REINTERPRET_CAST(SAPDB_UTF8*, _pszTgt);
    delete [] _pszTgt;
}
// ls28_NormalizeConstraintDefinitions()


/*
  -----------------------------------------------------------------------------
  function:     ls28_CompareDefaultDefinitions
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CompareDefaultDefinitions(tls00_DBInfo    *pDBInfo,
                               TableDef    *pTableDef,
                               tls00_TloadRec  *pTLRec,
                               ReadBuffer      &PageBuffer,
                               tsp00_Addr       pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_CompareDefaultDefinitions");

    tsp00_Int4   rc = errOK_els00;

    tsp00_Uint2  lSrcDefaultCount = 0;
    tsp00_Uint2  lTgtDefaultCount = pTableDef->defaultCount();

    //*
    //*     Get all defaults of the source table
    //*
    Studio_List  SrcDefaultList;
    rc = ls28_GetSourceDefaultList(pDBInfo, pTLRec, PageBuffer, true, SrcDefaultList, pszErrText);
    if (errOK_els00 != rc)
    {
        if ( (STOP_NODATA_LS28 == rc) && (0 != lTgtDefaultCount) )
        {
            rc = errTLSchemeDifferences_els98;
        }
        else
        {
            rc = errOK_els00;
        }
    }
    else
    {
        // Check if we should expect defaults at all - if the target table does not have
        // defaults defined the source table shouldn't have had, too.
        if (0 == lTgtDefaultCount)
        {
            rc = errTLSchemeDifferences_els98;
        }
        else
        {
            lSrcDefaultCount = SrcDefaultList.size();

            if (lSrcDefaultCount != lTgtDefaultCount)
            {
                rc = errTLSchemeDifferences_els98;
            }
            else
            {
                DefaultDescription      *pSrcDefault              = NULL;
                Tools_DynamicUTF8String  TgtDefault;
                tsp00_Uint4              colindex                 = 0;
                StudioTD_ColumnDef      *pTgtTableCol             = NULL;
                bool                     bTgtDefaultIsUserDefined = false;

                //*
                //*     Compare source and target table defaults
                //*
                // Read source default info from page and compare with target default info;
                // do it as long as default definitions found on that page
                for (pSrcDefault = REINTERPRET_CAST(DefaultDescription*, SrcDefaultList.first());
                    ( (errOK_els00 == rc) && (pSrcDefault) );
                    pSrcDefault = REINTERPRET_CAST(DefaultDescription*, SrcDefaultList.next()))
                {
                    //*
                    //*     Target table - column default definition
                    //* Get the default of column with column number pSrcDefault->DefaultColumnNo
                    //*
                    colindex     = pTableDef->findColumn(pSrcDefault->DefaultColumnNo);
                    pTgtTableCol = pTableDef->column(colindex);    // we start counting at 0
                    TgtDefault   = pTgtTableCol->defaultValue();

                    // User defined value ?
                    bTgtDefaultIsUserDefined = pTgtTableCol->IsDefaultUserDefinedValue();

                    if ( (pSrcDefault->IsUserDefined      != bTgtDefaultIsUserDefined) ||
                         (pSrcDefault->DefaultValueLength != TgtDefault.BasisSize())   ||
                         (pSrcDefault->DefaultValue       != TgtDefault)  )
                    {
                        rc = errTLSchemeDifferences_els98;
                    }
                }   // end for (pSrcDefault = REINTERPRET_CAST(DefaultDescription*, SrcDefaultList.first()); ...

            }   // end if (lSrcDefaultCount != lTgtDefaultCount)

        }   // end if (0 == lTgtDefaultCount)

    }   // end if (errOK_els00 != rc)

    ls28_EraseDefaultList(SrcDefaultList);

    return rc;
}
// ls28_CompareDefaultDefinitions()


/*
  -----------------------------------------------------------------------------
   function:     ls28_DropTableIndexes
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls28_DropTableIndexes(tls00_DBInfo             *pDBInfo,
                      Tools_DynamicUTF8String  &TableName,
                      tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_DropTableIndexes");

    tsp00_Longint rc = errOK_els00;

    //*
    //* Get target table description, and description of indexes on target table, too
    //*
    tsp00_Int4 lTabDescLen   = 0;
    tsp00_Addr pszTabDesc    = NULL;
    tsp00_Int4 lIndexDescLen = 0;
    tsp00_Addr pszIndexDesc  = NULL;

    rc = ls13GetTableDescription(pDBInfo, 0, TableName, lTabDescLen, pszTabDesc, pszErrText);
    if (errOK_els00 == rc)
    {
        rc = ls13GetIndexDescription(pDBInfo, 1, TableName, lIndexDescLen, pszIndexDesc, pszErrText);
    }

    if (errOK_els00 == rc)
    {
        // drop existing indexes
        rc = ls13DropAllIndexesOnTable(pDBInfo,
                                       REINTERPRET_CAST(tsp7_table_description*, pszTabDesc),
                                       REINTERPRET_CAST(tsp7_index_description*, pszIndexDesc),
                                       TableName,
                                       pszErrText);
        // commit changes
        if (errOK_els00 == rc)
        {
            rc = ls15Commit(pDBInfo, pszErrText);
        }
    }

    return rc;
}
// ls28_DropTableIndexes()


/*
  -----------------------------------------------------------------------------
   function:     ls28_IsTableEmpty
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls28_IsTableEmpty(tls00_DBInfo             *pDBInfo,
                  Tools_DynamicUTF8String  &TableName,
                  bool                     &IsEmpty,
                  tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_IsTableEmpty");

    tsp00_Longint rc = errOK_els00;

    IsEmpty = false;

    szLocalCmdString[0] = 0;
    sp77sprintf(szLocalCmdString, lStmtLength_ls26, EMPTY_TABLE_STMT_LS28, TableName.StrPtr());

    tsp00_Int4 lErrPos;
    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szLocalCmdString, szLocalErrString, lErrPos);
    if (errOK_els00 != rc)
    {
        if (dbRowNotFound_els00 == rc)      // table is empty
        {
            IsEmpty = true;
            rc = errOK_els00;
        }
        else
        {
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98,
                                                                                szLocalCmdString, szLocalErrString);
            rc = errInternSQL_els98;
        }
    }
    return rc;
}
// ls28_IsTableEmpty()


/*
  -----------------------------------------------------------------------------
  function:     ls28_RestoreShortStrings
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_RestoreShortStrings(tls00_DBInfo             *pDBInfo,
                         tls00_TloadRec           *pTLRec,
                         Tools_DynamicUTF8String  &TableName,
                         ReadBuffer               &PageBuffer,
                         tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_RestoreShortStrings");

    tsp00_Int4  rc = errOK_els00;

    _TCHAR     *pszBuffer = PageBuffer.ls51GetNext();       // get next page from data file
    if (NULL == pszBuffer)                  // error or eof
    {
        rc = PageBuffer.ls51GetError(pszErrText);
        if (EOF_LS00 == rc)
        {
            rc = errOK_els00;
        }
    }
    else
    {
        rc = ls28_CheckTypeHeaderPage(pszBuffer, pDBInfo->dbiPageSize_ls00, tptShortString_els00, pTLRec->tlrSwapFromTE_ls00);
        if (errOK_els00 != rc)
        {
            // error numbers can be
            //  - WRONG_TE_HEADER_LS28   --> no shorts to restore
            //  - UNKNOWN_TE_HEADER_LS28 --> no shorts to restore
            //  - STOP_NODATA_LS28         --> no more data
            if (WRONG_TE_HEADER_LS28 == rc)
            {
                TRACE_PRNF_MLS99(("ls28_RestoreShortStrings", "No short strings.\n"));

                PageBuffer.ls51NotUsed();                       // mark page as 'not used'
                rc = errOK_els00;
            }
            else if (UNKNOWN_TE_HEADER_LS28 == rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errWrongTEFileType_els98,
                                                                                     pTLRec->tlrInfile_ls00->vfFileName_ls00,
                                                                                     "manipulated short string header page");
                PageBuffer.ls51NotUsed();                       // mark page as 'not used'
                rc = errWrongTEFileType_els98;
            }
            // else if (STOP_NODATA_LS28 == rc)
            // --> no more data to process for this table; this branch must not be handled, return code has
            //     already the right value. It is highly unlikely that the return code becomes STOP_NODATA_LS28
            //     in this function because for every extracted table exists at least ONE page of primary data
            //     and this comes AFTER short string pages.
            //     But we cannot release the 'unused' header page here.

        }
        else
        {
            szLocalCmdString[0] = 0;

            sp77sprintf(szLocalCmdString, lStmtLength_ls26, RESTORE_STRING_STMT_LS28, TableName.StrPtr());

            rc = ls28_BeginRestore(pDBInfo, szLocalCmdString, &pTLRec->tlrBdInfo_ls00, pszErrText);
            if (errOK_els00 == rc)
            {
                Loader_RTStatistics::Instance()->SetCounter(0);         // initialize

                rc = ls28_DataLoadLoop(pDBInfo, pTLRec, PageBuffer, &pTLRec->tlrBdInfo_ls00, pszErrText);
                if (errInternal_els98 == rc)
                {
                    LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                            __FILE__,
                                                                                            __LINE__,
                                                                                            STATIC_CAST(ErrorNumbers, rc));
                }
            }

            if (errOK_els00 == rc)
            {
                rc = ls28_EndRestore(pDBInfo, &pTLRec->tlrBdInfo_ls00, pszErrText);

                Loader_RTStatistics::Instance()->IncreasePagesCounter(Loader_RTStatistics::SHORTLONGDATA, Loader_RTStatistics::Instance()->GetCounter());
            }
        }
    }   // end else branch of if (NULL == pszBuffer)

    return rc;
}
// ls28_RestoreShortStrings()

/*
  -----------------------------------------------------------------------------
  function:     ls28_RestorePrimaryData
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_RestorePrimaryData(tls00_DBInfo              *pDBInfo,
                        tls00_TloadRec            *pTLRec,
                         Tools_DynamicUTF8String  &TableName,
                        ReadBuffer                &PageBuffer,
                        tsp00_Addr                 pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_RestorePrimaryData");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR *pszBuffer = PageBuffer.ls51GetNext();       // get next page from data file
    if (NULL == pszBuffer)                  // error or eof
    {
        rc = PageBuffer.ls51GetError(pszErrText);
        if (EOF_LS00 == rc)
        {
            rc = errOK_els00;
        }
    }
    else
    {
        rc = ls28_CheckTypeHeaderPage(pszBuffer, pDBInfo->dbiPageSize_ls00, tptData_els00, pTLRec->tlrSwapFromTE_ls00);
        if (errOK_els00 != rc)                          // There is no primary data to restore
        {
            // error numbers can be
            //  - WRONG_TE_HEADER_LS28   --> no primary data to restore
            //  - UNKNOWN_TE_HEADER_LS28 --> no primary data to restore
            //  - STOP_NODATA_LS28         --> no more data
            if (WRONG_TE_HEADER_LS28 == rc)
            {
                TRACE_PRNF_MLS99(("ls28_RestorePrimaryData", "No primary data.\n"));

                PageBuffer.ls51NotUsed();                   // mark page as 'not used'
                rc = errOK_els00;
            }
            else if (UNKNOWN_TE_HEADER_LS28 == rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errWrongTEFileType_els98,
                                                                                     pTLRec->tlrInfile_ls00->vfFileName_ls00,
                                                                                     "manipulated primary data header page");
                PageBuffer.ls51NotUsed();                   // mark page as 'not used'
                rc = errWrongTEFileType_els98;
            }
            // else if (STOP_NODATA_LS28 == rc)
            // --> no more data to process for this table; this branch must not be handled, return code has
            //     already the right value. It is highly unlikely that the return code becomes STOP_NODATA_LS28
            //     in this function because for every extracted table exists at least ONE page of primary data.
            //     But we cannot release the 'unused' header page here.
        }
        else
        {
            szLocalCmdString[0] = 0;
            sp77sprintf(szLocalCmdString, lStmtLength_ls26, RESTORE_DATA_STMT_LS28, TableName.StrPtr());

            rc = ls28_BeginRestore(pDBInfo, szLocalCmdString, &pTLRec->tlrBdInfo_ls00, pszErrText);
            if (errOK_els00 == rc)
            {
                Loader_RTStatistics::Instance()->SetCounter(0);        // initialize

                rc = ls28_DataLoadLoop(pDBInfo, pTLRec, PageBuffer, &pTLRec->tlrBdInfo_ls00, pszErrText);
                if (errInternal_els98 == rc)
                {
                    LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                            __FILE__,
                                                                                            __LINE__,
                                                                                            STATIC_CAST(ErrorNumbers, rc));
                }
            }

            if (errOK_els00 == rc)
            {
                rc = ls28_EndRestore(pDBInfo, &pTLRec->tlrBdInfo_ls00, pszErrText);

                Loader_RTStatistics::Instance()->IncreasePagesCounter(Loader_RTStatistics::PRIMARYDATA, Loader_RTStatistics::Instance()->GetCounter());
            }
        }
    }   // end else branch of if (NULL == pszBuffer)

    return rc;
}
// ls28_RestorePrimaryData()


/*
  -----------------------------------------------------------------------------
  function:     ls28_RestoreLongStrings
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_RestoreLongStrings(tls00_DBInfo    *pDBInfo,
                        tls00_TloadRec  *pTLRec,
                        ReadBuffer      &PageBuffer,
                        tsp00_Addr       pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_RestoreLongStrings");

    tsp00_Int4 rc = errOK_els00;

    tls26_DescPageHeader *pHeader  = NULL;
    tsp00_SwapKind_Enum   CurrSwap = ls07DefineSwap(NULL);

    tls00_Parts4InternalSql PartStruct;
    PartStruct.pfiPartKind_ls00 = NULL;
    PartStruct.pfiData_ls00     = NULL;

    tls00_String *pMyData = NULL;


    _TCHAR *pszBuffer = PageBuffer.ls51GetNext();
    if (NULL == pszBuffer)
    {
        rc = PageBuffer.ls51GetError(pszErrText);
    }

    if (errOK_els00 == rc)
    {
        rc = ls28_CheckTypeHeaderPage(pszBuffer, pDBInfo->dbiPageSize_ls00, tptLongString_els00, pTLRec->tlrSwapFromTE_ls00);
        if (errOK_els00 != rc)                          // There are no long strings to restore
        {
            // error numbers can be
            //  - WRONG_TE_HEADER_LS28   --> no longs to restore
            //  - UNKNOWN_TE_HEADER_LS28 --> no longs to restore
            //  - STOP_NODATA_LS28         --> no more data
            if (WRONG_TE_HEADER_LS28 == rc)
            {
                TRACE_PRNF_MLS99(("ls28_RestoreLongStrings", "No long strings.\n"));

                PageBuffer.ls51NotUsed();                   // mark page as 'not used'
            }
            else if (UNKNOWN_TE_HEADER_LS28 == rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errWrongTEFileType_els98,
                                                                                     pTLRec->tlrInfile_ls00->vfFileName_ls00,
                                                                                     "manipulated long string header page");
                PageBuffer.ls51NotUsed();                   // mark page as 'not used'
                rc = errWrongTEFileType_els98;
            }
            // else if (STOP_NODATA_LS28 == rc)
            // --> no more data to process for this table; this branch must not be handled, return code has
            //     already the right value.
            //     We cannot release the 'unused' header page here.
        }
        else
        {
            PartStruct.pfiCount_ls00 = 3;               // 3 parts to deliver to kernel
            PartStruct.pfiPartKind_ls00 = new tsp1_part_kind_Enum[PartStruct.pfiCount_ls00];
            PartStruct.pfiPartKind_ls00[0] = sp1pk_command;
            PartStruct.pfiPartKind_ls00[1] = sp1pk_bdinfo;
            PartStruct.pfiPartKind_ls00[2] = sp1pk_data;

            PartStruct.pfiData_ls00 = new tls00_String[PartStruct.pfiCount_ls00];

            //*
            //*     Build statement
            //*
            PartStruct.pfiData_ls00[0].strAddr_ls00 = new _TCHAR[lStmtLength_ls26];
            _tcscpy(&PartStruct.pfiData_ls00[0].strAddr_ls00[0], RESTORE_LONG_STMT_LS28);
            PartStruct.pfiData_ls00[0].strLeng_ls00 = STATIC_CAST(tsp00_Int4, _tcslen(PartStruct.pfiData_ls00[0].strAddr_ls00));

            //*
            //*     Build bdinfo part
            //*
            PartStruct.pfiData_ls00[1].strAddr_ls00 = new _TCHAR[pTLRec->tlrBdInfo_ls00.bdiLength_ls00];
            PartStruct.pfiData_ls00[1].strLeng_ls00 = pTLRec->tlrBdInfo_ls00.bdiLength_ls00;
            SAPDB_memcpy(PartStruct.pfiData_ls00[1].strAddr_ls00, pTLRec->tlrBdInfo_ls00.szbdiData_ls00, pTLRec->tlrBdInfo_ls00.bdiLength_ls00);


            //*
            //*     Build data part
            //*
            tsp00_Int4 lDataPartLen = PERM_KEY_MXSP00 + INT2_MXSP00 + INT2_MXSP00;
            PartStruct.pfiData_ls00[2].strAddr_ls00 = new _TCHAR[lDataPartLen];

            // Because this is the part we have to renew for every new long value to send to kernel we
            // assign a pointer to this data
            pMyData = &PartStruct.pfiData_ls00[2];

            pHeader = REINTERPRET_CAST(tls26_DescPageHeader*, pszBuffer);
        }
    }


    while (errOK_els00 == rc)
    {
        // to restore long string we need to deliver the key and the column number; they're saved at
        // the header page

        // insert key length
        pMyData->strLeng_ls00 = INT2_MXSP00 + pHeader->dphSpecialID_ls26 + INT2_MXSP00;
        ls07Swap2BIntToChar(STATIC_CAST(tsp00_Int2, pHeader->dphSpecialID_ls26),
                            pTLRec->tlrSwapFromTE_ls00,
                            CurrSwap,
                            &pMyData->strAddr_ls00[0]);

        // insert key
        SAPDB_memcpy(&pMyData->strAddr_ls00[INT2_MXSP00], &pszBuffer[sizeof(tls26_DescPageHeader)], pHeader->dphSpecialID_ls26);

        ls07Swap2BIntToChar(STATIC_CAST(tsp00_Int2, pHeader->dphFillerInt_ls26),
                            pTLRec->tlrSwapFromTE_ls00,
                            CurrSwap,
                            &pMyData->strAddr_ls00[pHeader->dphSpecialID_ls26 + INT2_MXSP00]);

        rc = ls28_BeginRestoreLong(pDBInfo, PartStruct, &pTLRec->tlrLongBdInfo_ls00, pszErrText);
        if (errOK_els00 == rc)
        {
            Loader_RTStatistics::Instance()->SetCounter(0);        // initialize

            rc = ls28_DataLoadLoop(pDBInfo, pTLRec, PageBuffer, &pTLRec->tlrLongBdInfo_ls00, pszErrText);
            if (errInternal_els98 == rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98,
                                                                                        __FILE__, __LINE__,
                                                                                        STATIC_CAST(ErrorNumbers, rc));
            }
        }

        if (errOK_els00 == rc)
        {
            rc = ls28_EndRestore(pDBInfo, &pTLRec->tlrLongBdInfo_ls00, pszErrText);

            Loader_RTStatistics::Instance()->IncreasePagesCounter(Loader_RTStatistics::LONGLONGDATA, Loader_RTStatistics::Instance()->GetCounter());
        }

        // coming here w/o errors the load of the n-th long value has finished; we're now looking for the next
        // long value (next long type header page) in data file
        if (errOK_els00 == rc)
        {
            pszBuffer = PageBuffer.ls51GetNext();
            if (NULL == pszBuffer)
            {
                rc = PageBuffer.ls51GetError(pszErrText);
            }
            if (errOK_els00 == rc)
            {
                rc = ls28_CheckTypeHeaderPage(pszBuffer, pDBInfo->dbiPageSize_ls00, tptLongString_els00, pTLRec->tlrSwapFromTE_ls00);
                if (errOK_els00 != rc)                          // There are no more long strings to restore
                {
                    // error numbers can only be WRONG_TE_HEADER_LS28 and UNKNOWN_TE_HEADER_LS28;
                    // both indicate that there are no short strings to restore
                    if (WRONG_TE_HEADER_LS28 == rc)
                    {
                        TRACE_PRNF_MLS99(("ls28_RestoreLongStrings", "No long strings.\n"));

                        PageBuffer.ls51NotUsed();                   // mark page as 'not used'
                    }
                    else if (UNKNOWN_TE_HEADER_LS28 == rc)
                    {
                        LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errWrongTEFileType_els98,
                                                                                             pTLRec->tlrInfile_ls00->vfFileName_ls00,
                                                                                             "manipulated long string header page");
                        PageBuffer.ls51NotUsed();                   // mark page as 'not used'
                        rc = errWrongTEFileType_els98;
                    }
                    // else if (STOP_NODATA_LS28 == rc)
                    // --> no more data to process for this table; this branch must not be handled, return code has
                    //     already the right value.
                    //     We cannot release the 'unused' header page here.
                }
                else
                {
                    pHeader = REINTERPRET_CAST(tls26_DescPageHeader*, pszBuffer);
                }
            }
        }
    }   // end while


    // some cleaning work
    if (NULL != PartStruct.pfiData_ls00)
    {
        delete [] PartStruct.pfiData_ls00[0].strAddr_ls00;
        PartStruct.pfiData_ls00[0].strAddr_ls00 = NULL;

        delete [] PartStruct.pfiData_ls00[1].strAddr_ls00;
        PartStruct.pfiData_ls00[1].strAddr_ls00 = NULL;

        delete [] PartStruct.pfiData_ls00[2].strAddr_ls00;
        PartStruct.pfiData_ls00[2].strAddr_ls00 = NULL;

        delete [] PartStruct.pfiData_ls00;
    }

    if (NULL != PartStruct.pfiPartKind_ls00)
    {
        delete [] PartStruct.pfiPartKind_ls00;
        PartStruct.pfiPartKind_ls00 = NULL;
    }

    if ( (WRONG_TE_HEADER_LS28 == rc) || (EOF_LS00 == rc) || (STOP_NODATA_LS28 == rc) )
    {
        // This is the last data processing function for one table. So we can set
        // the return code to ok in these cases.
        rc = errOK_els00;
    }

    return rc;
}
// ls28_RestoreLongStrings()


/*
  -----------------------------------------------------------------------------
  function:     ls28_ReadDescFromFile
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ReadDescFromFile(ReadBuffer       &readBuffer,
                      tls00_TEPageType  DescriptionType,
                      tsp00_SwapKind    SourceSwap,
                      tsp00_Int4        lPageSize,
                      tls00_String     &pDescription,
                      tsp00_Addr        pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_ReadDescFromFile");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR               *pszBuffer = NULL;
    tls26_DescPageHeader *pHeader   = NULL;
    tsp00_Int4            lOffset   = 0;

    while (errOK_els00 == rc)
    {
        pszBuffer = readBuffer.ls51GetNext();
        if (NULL == pszBuffer)                      // error or eof
        {
            rc = readBuffer.ls51GetError(pszErrText);
            readBuffer.ls51NotUsed();             // mark page as 'not used'
        }
        else
        {
            rc = ls28_CheckTypeHeaderPage(pszBuffer, lPageSize, DescriptionType, SourceSwap);
            if (errOK_els00 != rc)                  // There is no description to read. We should never reach this point
            {                                       // or the file is corrupted. At this moment we cannot recover from this.
                readBuffer.ls51NotUsed();         // mark page as 'not used'
            }
            else
            {
                pHeader = REINTERPRET_CAST(tls26_DescPageHeader*, pszBuffer);

                if (0 == lOffset)                   // 1st time we come here: allocate memory for description
                {
                    pDescription.strLeng_ls00 = pHeader->dphFillerInt_ls26;
                    pDescription.strAddr_ls00 = new _TCHAR[pDescription.strLeng_ls00];
                }

                //*     copy n-th part of table|index description
                //* dphSpecialID_ls26 contains the length of written description info on THIS buffer page
                SAPDB_memcpy(&pDescription.strAddr_ls00[lOffset],
                             &pszBuffer[sizeof(tls26_DescPageHeader)],
                             pHeader->dphSpecialID_ls26);
                lOffset += pHeader->dphSpecialID_ls26;
                if (false == pHeader->dphMore_ls26)
                {
                    break;
                }
            }
        }
    }       // while (errOK_els00 == rc)

    return rc;
}
// ls28_ReadDescFromFile()

/*
  -----------------------------------------------------------------------------
  function:     ls28_CheckTypeHeaderPage
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_CheckTypeHeaderPage(tsp00_Addr       pszBuffer,
                         tsp00_Int4       lPageSize,
                         tls00_TEPageType PageType,
                         tsp00_SwapKind   SourceSwapKind)
{
    ROUTINE_DBG_MEO00 ("ls28_CheckTypeHeaderPage");

    tsp00_Int4 rc = errOK_els00;

    tls26_DescPageHeader *pHeader = REINTERPRET_CAST(tls26_DescPageHeader*, pszBuffer);
    tls26_DescPageHeader  Footer;

    if ( (tptData_els00 == PageType)        ||
         (tptShortString_els00 == PageType) ||
         (tptLongString_els00 == PageType)  )
    {
        SAPDB_memcpy(&Footer, &pszBuffer[lPageSize - sizeof(tls26_DescPageHeader)], sizeof(tls26_DescPageHeader));
        if (0 != memcmp(pHeader, &Footer, sizeof(tls26_DescPageHeader)))
        {
            rc = UNKNOWN_TE_HEADER_LS28;
        }
    }
    else
    {
        SAPDB_memcpy(&Footer, pHeader, sizeof(tls26_DescPageHeader));
    }

    if (errOK_els00 == rc)
    {
        ls28SwapHeader(SourceSwapKind, pHeader);

        if ( (MY_PAGE_LS00 != pHeader->dphType_ls26) || (PageType != pHeader->dphPageType_ls26) )
        {
            if (tptEOT_els00 == pHeader->dphPageType_ls26)      // no more table data in file (end of table)
            {
                rc = STOP_NODATA_LS28;
            }
            else
            {
                rc = WRONG_TE_HEADER_LS28;                    // actually not an error: only a message

                // there is a small thing to do - undo header swap to enable others to use the page freshly
                // ATTENTION: REUSING FOOTER HERE FOR DATA PAGES
                // we can use the footer or the temp saved header here to restore the header;
                // header and footer MUST originally have been the same only for data pages
                SAPDB_memcpy(pszBuffer, &Footer, sizeof(tls26_DescPageHeader));
            }
        }
    }

    return rc;
}
// ls28_CheckTypeHeaderPage()


/*
  -----------------------------------------------------------------------------
  function:     ls28_BeginRestore
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_BeginRestore(tls00_DBInfo*  pDBInfo,
                  tsp00_Addr     pszCmd,
                  tls00_BDInfo*  pBDInfo,
                  tsp00_Addr     pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_BeginRestore");
    tsp00_Int4 rc  = errOK_els00;

    tsp00_Int4 lErrPos = 0;

    rc = ls13ExecIntSQLStmt(pDBInfo, 0, pszCmd, szLocalErrString, lErrPos);
    if (0 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98,
                                                                             pszCmd, szLocalErrString);
        rc = errInternSQL_els98;
    }
    else
    {
        rc = ls04GetBdinfo(pDBInfo->dbiPktRcvList_ls00[0], pBDInfo);
        if (errOK_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternal_els98,
                                                                                 __FILE__, __LINE__,
                                                                                 rc);
            rc = errInternal_els98;
        }
    }
    return rc;
}
// ls28_BeginRestore()

/*
  -----------------------------------------------------------------------------
  function:     ls28_DataLoadLoop
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_DataLoadLoop(tls00_DBInfo*    pDBInfo,
                  tls00_TloadRec*  pTLRec,
                  ReadBuffer&      PageBuffer,
                  tls00_BDInfo*    pBDInfo,
                  tsp00_Addr       pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_DataLoadLoop");
    
    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    tsp00_Int4      rc      = errOK_els00;
    tsp00_Int4      rcFill  = errOK_els00;

    tsp00_Int4 lSndPcktNo  = 0;
    tsp00_Int4 lRcvPcktNo  = 0;
    szLocalErrString[0]    = 0;

    while ( (errOK_els00 == rcFill) && (errOK_els00 == rc) )
    {
        //  Fill packet
        rcFill = ls28_FillPacketBuffer(pDBInfo, pTLRec, lSndPcktNo, PageBuffer, pszErrText);

        if (lSndPcktNo != lRcvPcktNo)   // Gegebenenfalls anderes SQL-Packet empfangen
        {
            //*
            //*     Receive packet
            //* We adapt the number of received packet even if receiving fails.
            //* If it fails we do not need to receive it again.
            //*
            rc = ls13ReceiveBdinfoPckt(pDBInfo, lRcvPcktNo, pBDInfo, pszErrText);
            if (errOK_els00 != rc)
            {
                if (rc > 0)
                {
                    // Positive return value -> sql error from kernel
                    // This could actually only be one of the following:
                    //   300 - integrity violation
                    //   320 - view violation
                    //   350 - referential integrity violated
                    //   360 - foreign key integrity violated

                    // Log sql error to log file
                    if ( (300 <= rc) && (rc < 400) )
                    {
                        pMsgObj->ls98Msg(layTableLoad_els98, pszErrText, errFL_IntegrityViolation_els98, rc,"");
                        rc = errFL_IntegrityViolation_els98;
                    }
                    else
                    {
                        pMsgObj->ls98Msg(layTableLoad_els98, pszErrText, errSimpleSQL_els98, szLocalErrString);
                        rc = errSimpleSQL_els98;
                    }
                }
                else
                {
                    pMsgObj->ls98Msg(layTableLoad_els98, pszErrText, errInternal_els98, __FILE__,
                                                                                        __LINE__,
                                                                                        STATIC_CAST(ErrorNumbers, rc));
                    rc = errInternal_els98;
                }
            }

            lRcvPcktNo = (lRcvPcktNo + 1) % 2;
        }

        //*     Send packet
        //* Only if filling the packet and receiving the answer ran without errors
        //*
        if ( ( (errOK_els00 == rcFill) || (STOP_LS28 == rcFill) ) && (errOK_els00 == rc) )
        {
            rc = ls13SendBdinfoPckt(pDBInfo, lSndPcktNo, pBDInfo, pszErrText);
            if (errOK_els00 == rc)
            {
                lSndPcktNo = (lSndPcktNo + 1) % 2;  //  successful sent: recalculate packet number
            }
        }
    }   // end while ( (errOK_els00 == rcFill) && (errOK_els00 == rc) )


    if (lSndPcktNo != lRcvPcktNo)
    {
        //*     Receive last packet
        rc = ls13ReceiveBdinfoPckt(pDBInfo, lRcvPcktNo, pBDInfo, szLocalErrString);
        if (errOK_els00 != rc)
        {
            if (rc > 0)
            {
                // Log sql error to log file
                if ( (300 <= rc) && (rc < 400) )
                {   // For these errors see the first call of ls13ReceiveBdinfoPckt
                    pMsgObj->ls98Msg(layTableLoad_els98, pszErrText, errFL_IntegrityViolation_els98, rc,"");
                    rc = errFL_IntegrityViolation_els98;
                }
                else
                {
                    pMsgObj->ls98Msg(layTableLoad_els98, pszErrText, errSimpleSQL_els98, szLocalErrString);
                    rc = errSimpleSQL_els98;
                }
            }
            else
            {

                pMsgObj->ls98Msg(layTableLoad_els98, pszErrText, errInternal_els98, __FILE__, __LINE__,
                                                                                    STATIC_CAST(ErrorNumbers, rc));
                rc = errInternal_els98;
            }
        }


//TODO      Bei der fehlerbehandlung haben wir ein problem. Wenn jetzt etwas schiefgeht beim empfangen
        //  des paketes aber vorher auch schon ein fehler bei der bearbeitung der pakete aufgetreten
        //  ist, welcher fehler wird dann gemeldet? Die fehler muessen nicht notwendigerweise voneinander
        //  abhaengen.
        //  Momentan wird der fehler beim empfang des letzten paketes nur dann nach oben gemeldet,
        //  wenn vorher kein fehler aufgetreten ist.
    }

    // some comparing
    if (errOK_els00 == rc)
    {
        if ( (errOK_els00 != rcFill) && (STOP_LS28 != rcFill) && (STOP_NODATA_LS28 != rcFill))
        {
            rc = rcFill;
        }
    }

    return rc;
}
// ls28_DataLoadLoop()

/*
  -----------------------------------------------------------------------------
  function:     ls28_FillPacketBuffer
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_FillPacketBuffer(tls00_DBInfo   *pDBInfo,
                      tls00_TloadRec *pTLRec,
                      tsp00_Int4      lPcktNo,
                      ReadBuffer     &PageBuffer,
                      tsp00_Addr      pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_FillPacketBuffer");

    tsp00_Int4 rc  = errOK_els00;
    tsp00_Int4 rc2 = errOK_els00;

    tsp1_packet *SqlPacket = NULL;
    tsp1_part   *SqlPart   = NULL;

    // prepare the packet to take the data -> puts a part of part kind page into packet
    ls13Prepare_FL_TL_Packet(pDBInfo, lPcktNo, SqlPacket, SqlPart);

    tsp00_Int2 i          = 0;
    _TCHAR     *pszBuffer = NULL;

    //*
    //*     Now fill the packet with data pages
    //*
    //* When computing tlrPgPerPckt_ls00 it is already taken into account, that a sent packet
    //* must contain a bdinfo AND the data.
    //*
    //* This function is ONLY called if we know exactly that there is some data for
    //* the requested type in the data file. Additionally the last page of every single
    //* table data file is a special header page. So we will never reach the end of
    //* file here if the tableextract went without any problems and the file is not
    //* corrupted afterwards (hopefully).
    //*
    for (i; ((i < pTLRec->tlrPgPerPckt_ls00) && (errOK_els00 == rc)); ++i)
    {
        pszBuffer = PageBuffer.ls51GetNext();
        if (NULL == pszBuffer)
        {
            rc = PageBuffer.ls51GetError(pszErrText);   // can never be eof, must be an error
        }

        if (errOK_els00 == rc)
        {
            //*
            //*     Check if page is the right one or if it is already the next data type
            //* In case this is the first page read here there is nothing to process.
            //*
            if (ptNil_egg00 != (REINTERPRET_CAST(tbd_node*, pszBuffer))->nd_pt())
            {
                PageBuffer.ls51NotUsed();
                if (0 == i)
                {
                    rc = STOP_NODATA_LS28;  // no page read from file here; nothing more to process
                }
                else
                {
                    rc = STOP_LS28;         // At least one page read that must be send to the kernel
                }
            }
            else
            {
                // swap length information on page
                ls28SwapPageValues(pszBuffer, pDBInfo->dbiPageSize_ls00, pTLRec);

                ls04BufToPart(SqlPart, pszBuffer, pTLRec->tlrPageSize_ls00);

                Loader_RTStatistics::Instance()->IncreaseCounter();
            }
        }
    }   // end for (i; ((i < pTLRec->tlrPgPerPckt_ls00) && (errOK_els00 == rc)); ++i)

    // finish part
    if ( (errOK_els00 == rc) || (STOP_LS28 == rc) )
    {
        rc2 = ls04FinishPart(SqlPacket, SqlPart);
        if (errOK_els00 != rc2)
        {
            rc = rc2;
        }
    }

    return rc;
}
// ls28_FillPacketBuffer()


/*
  -----------------------------------------------------------------------------
  function:     ls28_EndRestore
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_EndRestore(tls00_DBInfo *pDBInfo, tls00_BDInfo* pBDInfo, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_EndRestore");
    tsp00_Int4 rc  = errOK_els00;

    szLocalCmdString[0] = 0;
    _tcscpy(szLocalCmdString, END_RESTORE_STMT_LS28);

    tsp00_Int4 lErrorPos = 0;

    rc = ls13ExecIntSQLStmtEx(pDBInfo,
                              0,
                              szLocalCmdString,
                              pBDInfo->szbdiData_ls00,
                              pBDInfo->bdiLength_ls00,
                              szLocalErrString,
                              lErrorPos,
                              sp1pk_bdinfo);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98,
                                                                             szLocalCmdString, szLocalErrString);
        rc = errInternSQL_els98;
    }

    return rc;
}
// ls28_EndRestore()

/*
  -----------------------------------------------------------------------------
  function:     ls28_BeginRestoreLong
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_BeginRestoreLong(tls00_DBInfo*             pDBInfo,
                      tls00_Parts4InternalSql&  PartStruct,
                      tls00_BDInfo*             pszColumnBdinfo,
                      tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_BeginRestoreLong");

    tsp00_Int4 rc = errOK_els00;

    tsp00_Int4 lErrorPos = 0;
    szLocalErrString[0] = 0;

    rc = ls13ExecIntSQLMultParts(pDBInfo, 0, PartStruct, szLocalErrString, lErrorPos);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98,
                                                                             PartStruct.pfiData_ls00[0].strAddr_ls00,
                                                                             szLocalErrString);
        rc = errInternSQL_els98;
    }
    else
    {
        rc = ls04GetBdinfo(pDBInfo->dbiPktRcvList_ls00[0], pszColumnBdinfo);
        if (errOK_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternal_els98,
                                                                                 __FILE__, __LINE__,
                                                                                 rc);
            rc = errInternal_els98;
        }
    }

    return rc;
}
// ls28_BeginRestoreLong()

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  function:     ls28_ReadMetaTrailer
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls28_ReadMetaTrailer(tls00_DBInfo     *pDBInfo,
                     tls00_TransformationCmd *pTransformationCmd,
                     tls00_TloadRec   *pTabLoadRec,
                     ReadBuffer       &readBuffer,
                     tsp00_Addr        ErrText)
{
    ROUTINE_DBG_MEO00 ("ls28_ReadMetaTrailer");

    tsp00_Int4 rc = errOK_els00;

    _TCHAR  *tlrpPageBuffer_ls00 = readBuffer.ls51GetNext();

    if (NULL == tlrpPageBuffer_ls00)
    {
        rc = readBuffer.ls51GetError(ErrText);
        if (EOF_LS00 == rc)
        {
            rc = errOK_els00;
        }
    }
    else
    {
        // transformation statement
        Tools_DynamicUTF8String  TabLoadStmt;

        TabLoadStmt.AssignRaw(REINTERPRET_CAST(unsigned char*,tlrpPageBuffer_ls00),
                              STATIC_CAST(SAPDB_UInt,_tcslen(tlrpPageBuffer_ls00)));
    }
    return rc;

}// ls28_ReadMetaTrailer



static const char *
ls28_SQLTypeToString(const tsp00_DataType sqltype)
{
    switch(sqltype)
    {
        case dfixed:    // 0
            return "FIXED";
        case dfloat:
            return "FLOAT";
        case dcha:
            return "CHAR ASCII";
        case dche:
            return "CHAR EBCDIC";
        case dchb:
            return "CHAR BYTE";
        case drowid:
            return "ROWID";
        case dstra:
            return "LONG ASCII";
        case dstre:
            return "LONG EBCDIC";
        case dstrb:
            return "LONG BYTE";
        case dstrdb:
            return "LONG DOUBLE BYTE";
        case ddate:     // 10
            return "DATE";
        case dtime:
            return "TIME";
        case dvfloat:
            return "FLOAT";
        case dtimestamp:
            return "TIMESTAMP";
        case dunknown:
            return "UNKNOWN";
        case dnumber:
            return "NUMBER";
        case dnonumber:
            return "UNKNOWN";
        case dduration:
            return "DURATION";
        case ddbyteebcdic:
            return "DOUBLE BYTE EBCDIC";
        case dlonga:
            return "LONG ASCII";
        case dlonge:    // 20
            return "LONG EBCDIC";
        case dlongb:
            return "LONG BYTE";
        case dlongdb:
            return "LONG DOUBLE BYTE";
        case dboolean:
            return "BOOLEAN";
        case dunicode:
            return "UNICODE";
        case ddtfiller1:
            return "UNKNOWN";
        case ddtfiller2:
            return "UNKNOWN";
        case ddtfiller3:
            return "UNKNOWN";
        case ddtfiller4:
            return "UNKNOWN";
        case dsmallint:
            return "SMALLINT";
        case dinteger:  // 30
            return "INTEGER";
        case dvarchara:
            return "VARCHAR ASCII";
        case dvarchare:
            return "VARCHAR EBCDIC";
        case dvarcharb:
            return "VARCHAR BYTE";
        case dstruni:
            return "UNICODE";
        case dlonguni:
            return "LONG UNICODE";
        case dvarcharuni:
            return "VARCHAR UNICODE";
        case dudt:
            return "UDT";
        case dabaptabhandle:
            return "ABABTABHANDLE";
        case dwyde:
            return "DWYDE";
        default:
            return "UNKNOWN";
    }
}
// ls28_SQLTypeToString()