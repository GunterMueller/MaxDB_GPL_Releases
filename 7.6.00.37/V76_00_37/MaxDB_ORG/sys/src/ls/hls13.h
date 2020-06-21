/*! 
  -----------------------------------------------------------------------------
  module: hls13.h
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893 

  special area: SAP DB LOADER
 
  description:  Functions to get all necessary information on a table from the database
                
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
#ifndef HLS13
#define HLS13

#include "gls00.h"
#include "hls31_odbc_Result.h"      // MetaDataDef->Statement()


#define SINGLE_INDEX_NAME_LS13            _T("\"%s\".\"%s\"")
#define MULT_INDEX_NAME_LS13              _T("\"%s\"")

#define MULT_UNIQUE_INDEX_STMT_LS13       _T("CREATE UNIQUE INDEX %s ON \"%s\" (")
#define MULT_INDEX_STMT_LS13              _T("CREATE INDEX %s ON \"%s\" (")

#define SINGLE_UNIQUE_INDEX_STMT_LS13     _T("CREATE UNIQUE INDEX \"%s\".\"%s\"")
#define SINGLE_INDEX_STMT_LS13            _T("CREATE INDEX \"%s\".\"%s\"")

#define DROP_SI_STMT_LS28                 _T("DROP INDEX %s")               // drop single index stmt
#define DROP_MI_STMT_LS28                 _T("DROP INDEX %s ON \"%s\"")     // drop multiple index stmt


#define DESCRIBE_DEFAULT_STMT_LS13     _T("DESCRIBE DEFAULT \"%s\"")
#define DESCRIBE_TABLE_STMT_LS13       _T("DESCRIBE TABLE \"%s\"")
#define DESCRIBE_INDEX_STMT_LS13       _T("DESCRIBE INDEX \"%s\"")

/*!
  -----------------------------------------------------------------------------
  Chapter: Functions to process sql statements
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
   function:     ls13ExecExtSQLStmt
  -----------------------------------------------------------------------------

  description:  Executes an SQL statement thats sent by the user.

  arguments:    DBInfo          [IN]    - structure with all info on the running
                                           session and the database connected to
                SqlCmd          [IN]    - Structure holding all needed information on
                                          the stmt to be executed
                ErrText         [OUT]   - error text

  returnvalue:  errOK_els00                 - no errors
                errFinishPart_els00         - could not finish part to send - fatal error
                error code for sql error    - otherwise

                Error messages will be returned in ErrText in the following form:
                "SQL-Error <error number> = <error text> (error position: <position>)".
                Error text is logged to the log file, too.
  -----------------------------------------------------------------------------
*/
int
ls13ExecExtSQLStmt(tls00_DBInfo *DBInfo, tls00_DBSqlCmd *SqlCmd, tsp00_Addr ErrText);


/*!
  -----------------------------------------------------------------------------
   function:     ls13ExecExtSQLStmt (overloaded)
  -----------------------------------------------------------------------------
  description:  Executes an SQL statement thats sent by the user.

  arguments:    DBInfo          [IN]    - structure with all info on the running
                                           session and the database connected to
                pszSqlCmd       [IN]    - Stmt to be executed
                ErrText         [OUT]   - Error text

  returnvalue:  errOK_els00                 - no errors
                errFinishPart_els00         - could not finish part to send - fatal error
                error code for sql error    - otherwise

                Error messages will be returned in ErrText in the following form:
                "SQL-Error <error number> = <error text> (error position: <position>)".
                Error text is logged to the log file, too.
  -----------------------------------------------------------------------------
*/
int
ls13ExecExtSQLStmt(tls00_DBInfo *pDBInfo, const char* pszSqlCmd, tsp00_Addr ErrText);


/*!
  -----------------------------------------------------------------------------
   function:     ls13PingDB
  -----------------------------------------------------------------------------
  description:  Sends HELLO packet to kernel to test if connection is still alive.

  arguments:    DBInfo          [IN]    - structure with all info on the running
                                           session and the database connected to
                pszErrText      [OUT]   - error text

  returnvalue:  errOK_els00                 - no errors
                error code for sql error    - otherwise
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13PingDB(tls00_DBInfo* pDBInfo, SAPDB_Char* pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13ExecIntSQLStmt
  -----------------------------------------------------------------------------

  description:  Executes an SQL statement thats build by an repserver internal
                function.
                All commands are send as internal (producer = sp1pr_internal_cmd)

  arguments:    DBInfo          [IN]    - structure with all info on the running
                                           session and the database connected to
                lPcktNo         [IN]    - packet to use
                pszSqlStmt      [IN]    - Statement to execute (must be null terminated)
                ErrText         [OUT]   - error text
                ErrPos          [OUT]   - position of erroneous part in packet
                bWithInfo       [IN]    - with info flag (defaults to false)
                MessType        [IN]    - message type (defaults to sp1m_dbs)
                bMassCmd        [IN]    - mass command flag (defaults to false)

  returnvalue:  errOK_els00                 - no errors
                errFinishPart_els00         - could not finish part to send - fatal error
                error code for sql error    - otherwise

                Error messages will be returned in ErrText in the following form:
                "SQL-Error: <error number> = <error text> (error position: <position>)".
  -----------------------------------------------------------------------------
*/
int
ls13ExecIntSQLStmt(tls00_DBInfo            *DBInfo,
                   SAPDB_Int4               lPcktNo,
                   tsp00_Addr               pszSqlStmt,
                   tsp00_Addr               pszErrText,
                   SAPDB_Int4              &lErrPos,
                   bool                     bWithInfo = false,
                   tsp1_cmd_mess_type_Enum  MessType  = sp1m_dbs,
                   bool                     bMassCmd  = false);


/*!
  -----------------------------------------------------------------------------
  function:     ls13ExecIntSQLStmtEx
  -----------------------------------------------------------------------------

  description:  Executes an SQL statement thats built by a repserver internal
                function. The packet contains an additional data part necessary
                for command execution.
                In case the length of data (lDataLength) is set to 0 no data
                part is added.
                All commands are send as internal (producer = sp1pr_internal_cmd)

  arguments:    DBInfo       [IN]    - structure with all info on the running
                                        session and the database connected to
                lPcktNo      [IN]    - packet to use
                pszSqlStmt   [IN]    - Statement to execute (must be null terminated)
                pszData      [IN]    - Data to send in data part, not null terminated
                lDataLength  [IN]    - length of data
                pszErrText   [OUT]   - error text
                lErrorPos    [OUT]   - error position in sent packet in case of error
                bWithInfo    [IN]    - with info flag (defaults to false)
                MessType     [IN]    - message type (defaults to sp1m_dbs)
                bMassCmd     [IN]    - mass command flag (defaults to false)

  returnvalue:  errOK_els00              - no errors
                errFinishPart_els00      - could not finish part to send - fatal error
                error code for sql error - otherwise

                Error messages will be returned in ErrText in the following form:
                "SQL-Error <error number> = <error text> (error position: <position>)"
                No error text is logged by this function!
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13ExecIntSQLStmtEx(tls00_DBInfo            *DBInfo,
                     SAPDB_Int4               lPcktNo,
                     tsp00_Addr               pszSqlStmt,
                     tsp00_Addr               pszData,
                     SAPDB_Int4               lDataLength,
                     tsp00_Addr               pszErrText,
                     SAPDB_Int4              &ErrPos,
                     tsp1_part_kind_Enum      PartKind  = sp1pk_data,
                     bool                     bWithInfo = false,
                     tsp1_cmd_mess_type_Enum  MessType  = sp1m_dbs,
                     bool                     bMassCmd  = false);


/*!
  -----------------------------------------------------------------------------
  function:     ls13ExecIntSQLMultParts
  -----------------------------------------------------------------------------

  description:  Executes an SQL statement thats build by an repserver internal
                function.
                The PartStruct parameter contains all necessary data - the command
                itself, the data, that has to be sent - for kernel execution. This
                structure has to be filled by the caller.
                All commands are send as internal (producer = sp1pr_internal_cmd).

                The first member of PartStruct MUST be a command (part kind
                sp1pk_command) which is checked. If not the function fails with
                an assertion.
  
  attention:    Useful function only if command part is not immediately followed by
                a data part or if more than 2 parts need to be send because the
                filling of the needed structure is expensive.

  arguments:    DBInfo       [IN]    - structure with all info on the running
                                        session and the database connected to
                lPcktNo      [IN]    - packet to use
                PartStruct   [IN]    - structure that contains all info to process 
                                        the command
                pszErrText   [OUT]   - error text
                lErrorPos    [OUT]   - error position in sent packet in case of error
                bWithInfo    [IN]    - with info flag (defaults to false)
                MessType     [IN]    - message type (defaults to sp1m_dbs)
                bMassCmd     [IN]    - mass command flag (defaults to false)

  returnvalue:  errOK_els00                 - no errors
                errFinishPart_els00         - could not finish part to send - fatal error
                error code for sql error    - otherwise

                Error messages will be returned in ErrText in the following form:
                "SQL-Error <error number> = <error text> (error position: <position>)"
                No error text is logged by this function!
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13ExecIntSQLMultParts(tls00_DBInfo            *DBInfo,
                        SAPDB_Int4               lPcktNo,
                        tls00_Parts4InternalSql &PartStruct,
                        tsp00_Addr               pszErrText,
                        SAPDB_Int4              &lErrPos,
                        bool                     bWithInfo = false,
                        tsp1_cmd_mess_type_Enum  MessType  = sp1m_dbs,
                        bool                     bMassCmd  = false);


/*!
  -----------------------------------------------------------------------------
  function:     ls13Parse
  -----------------------------------------------------------------------------

  description:  Parses an SQL statement and returns parse id and shortinfo.
  
  arguments:    pszCmd       [IN]    - Command to parse
                pDBInfo      [IN]    - Structure with all info on the running
                                        session and the database connected to
                pszParseID   [OUT]   - Parse ID as DB number string
                pTableDesc   [OUT]   - Short info
                pszErrText   [OUT]   - Error text
                bWithInfo    [IN]    - With info flag (defaults to false)
                bMassCmd     [IN]    - Mass command flag (defaults to false)

  returnvalue:  errOK_els00                 - no errors
                errUnknownPart_els98        - could not find either parse id part or
                                               shortinfo part
                error code for sql error    - otherwise

                Error messages will be returned in ErrText in the following form:
                "SQL-Error <error number> = <error text> (error position: <position>)"
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13Parse(tsp00_Addr              pszCmd,
          tls00_DBInfo           *pDBInfo,
          char                   *pszParseID,
          tls00_TableDescription *pTableDesc,
          tsp00_Addr              pszErrText,
          bool                    bWithInfo = false,
          bool                    bMassCmd  = false);


/*!
  -----------------------------------------------------------------------------
  EndChapter: Functions to process sql statements
  -----------------------------------------------------------------------------
*/


/*!
  -----------------------------------------------------------------------------
  Chapter: Functions for DB object support
  -----------------------------------------------------------------------------
*/


/*!
  -----------------------------------------------------------------------------
  function:     ls13GetTableDescription
  -----------------------------------------------------------------------------
  description:  Executes "DESCRIBE TABLE <table name> and returns a pointer to
                the data in receive packet.
                The caller has to save the data because with a subsequent use
                of the same packet the data is lost.

  arguments:    DBInfo         [IN]  - structure with all info on the running
                                        session and the database connected to
                lPacketNo      [IN]  - packet number to use for request/receive
                pszTableName   [IN]  - fully qualified table name
                lTabDescLen    [OUT] - length of data pointed to
                pszTabDesc     [OUT] - pointer to data in receive packet
                pszErrtext     [OUT] - error text

  returnvalue:  errOK_els00
                errInternSQL_els98  - error executing statement: simple sql error message
                                      will be returned in ErrText in the following form
                                      "SQL-Error: <error number> = <error text>
                                        (error position: <position>)"
                                      No text is logged to log file.
                errInternal_els98   - special error when retrieving the data from
                                      request packet (highly unlikely)
                                      
                Function logs error messages to return string AND to log file.
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13GetTableDescription(tls00_DBInfo            *pDBInfo,
                        SAPDB_Int4               lPacketNo,
                        Tools_DynamicUTF8String  TableName,
                        SAPDB_Int4              &lTabDescLen,
                        tsp00_Addr              &pszTabDesc,
                        tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13GetIndexDescription
  -----------------------------------------------------------------------------

  description:  Executes "DESCRIBE INDEX <table name> and returns a pointer to
                the data in receive packet.
                The caller has to save the data because with a subsequent use
                of the same packet the data is lost.

  arguments:    DBInfo         [IN]  - structure with all info on the running
                                        session and the database connected to
                lPacketNo      [IN]  - packet number to use for request/receive
                TableName      [IN]  - fully qualified table name; UTF8 encoded
                lIndexDescLen  [OUT] - length of data pointed to
                pszIndexDesc   [OUT] - pointer to data in receive packet
                pszErrtext     [OUT] - error text

  returnvalue:  errOK_els00
                errInternSQL_els98  - error executing statement: simple sql error message
                                      will be returned in ErrText in the following form
                                      "SQL-Error: <error number> = <error text>
                                        (error position: <position>)"
                                      No text is logged to log file.
                errInternal_els98   - special error when retrieving the data from
                                      request packet (highly unlikely)

                Function logs error messages to return string AND to log file.
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13GetIndexDescription(tls00_DBInfo            *pDBInfo,
                        SAPDB_Int4               lPacketNo,
                        Tools_DynamicUTF8String  TableName,
                        SAPDB_Int4              &lIndexDescLen,
                        tsp00_Addr              &pszIndexDesc,
                        tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13GetColumnNames
  -----------------------------------------------------------------------------
  description:  Gets the set of column names from the database kernel for the
                specified table. Adapts value pTI.tdFieldCount_ls00.
                The function selects the column names from system table COLUMNS.

  arguments:    pMetaDataDef [IN]    - info on running session and db connected to
                pSchemaName  [IN]    - schema name
                pTableName   [IN]    - table name
                pTI          [OUT]   - structure that will be filled with column names
                pszErrText   [OUT]   - error text

  returnvalue:  errOK_els00                - no errors
                errSelectColumnNames_els98 - error selecting column names
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13GetColumnNames(MetaDataDef*&            pMetaDataDef,
                   tsp00_Addr               pSchemaName,
                   tsp00_Addr               pTableName,
                   tls00_TableDescription*  pTI,
                   tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13GetColumnNamesInRecordOrder
  -----------------------------------------------------------------------------

  description:  Separates column names off the table description structure
                tsp7_table_description in order of the columns in a record.

  arguments:    pTInfo      [IN]  - table description structure holding info on
                                     column names, too
                pNameArray  [OUT] - array of name strings, to be filled; UTF8
                                     encoded strings
                SrcEncoding [IN]  - encoding of names in table description
                                     structure
                pszErrText  [OUT] - error text

  returnvalue:  errOK_els00                 - no error
                errInternal_els00           - error while converting between encodings
                errConvertingEncoding_els98 - error while converting between encodings

                Function returns and logs error message.
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13GetColumnNamesInRecordOrder(tsp7_table_description  *pTInfo,
                                Tools_DynamicUTF8String  pNameArray[],
                                tls00_CodeType           SrcEncoding,       // must give a hint on swap type in case of UCS2
                                tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13GetColumnNamesInDefOrder
  -----------------------------------------------------------------------------

  description:  Separates column names off the table description structure
                tsp7_table_description in definition order of the columns.

  arguments:    pTInfo      [IN]  - table description structure holding info on
                                     column names, too
                pNameArray  [OUT] - array of name strings, to be filled; UTF8
                                     encoded strings
                SrcEncoding [IN]  - encoding of names in table description
                                     structure
                pszErrText  [OUT] - error text

  returnvalue:  errOK_els00                 - no error
                errInternal_els00           - error while converting between encodings
                errConvertingEncoding_els98 - error while converting between encodings

                Function returns and logs error message.
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13GetColumnNamesInDefOrder(tsp7_table_description  *pTInfo,
                             Tools_DynamicUTF8String  pNameArray[],
                             tls00_CodeType           SrcEncoding,      // must give a hint on swap type in case of UCS2
                             tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13RetrieveIndexNames
  -----------------------------------------------------------------------------

  description:  Gets index names of all described indexes out of
                tsp7_index_description structure.
                Names come in the following form:
                <ul>
                  <li>single indexes --> <table name>.<index name>
                      (for single indexes the index name corresponds to the column
                       name of that table the index was created)
                  </li>
                  <li>multiple (composite) indexes  --> <index name>
                  </li>
                </ul>

  arguments:    pDBInfo        [IN]  - structure with all info on the running
                                        session and the database connected to;
                                        mainly used for encoding type
                pTInfo         [IN]  - table description structure
                pIInfo         [IN]  - index description structure
                TableName      [IN]  - table name UTF8 encoded
                IndexNameArray [OUT] - array of UTF8 strings; must be allocated
                                        by the caller; the count corresponds
                                        to the number of indexes on that table
                pszErrText     [OUT] - error text

  returnvalue:  errOK_els00                 - no error
                errInternal_els00           - error while converting between encodings
                errConvertingEncoding_els98 - error while converting between encodings

                Function returns and logs error message.
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13RetrieveIndexNames(tls00_DBInfo            *pDBInfo,
                       tsp7_table_description  *pTInfo,
                       tsp7_index_description  *pIInfo,
                       Tools_DynamicUTF8String  TableName,
                       Tools_DynamicUTF8String  IndexNameArray[],
                       tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13DropAllIndexesOnTable
  -----------------------------------------------------------------------------

  description:  Drops all indexes on the table that are retrieved out of 
                tsp7_index_description structure.

  arguments:    pDBInfo      [IN]  - structure with all info on the running
                                      session and the database connected to
                pTInfo       [IN]  - tsp7_table_description structure
                pIInfo       [IN]  - tsp7_index_description structure
                TableName    [IN]  - full qualified table name; UTF8 encoded
                pszErrText   [OUT] - error text

  returnvalue:  errOK_els00        - no error
                errInternSQL_els98 - may be caused by an sql error creating indexes,
                                      by some communication error, or by an
                                      error while converting between different
                                      encodings
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13DropAllIndexesOnTable(tls00_DBInfo            *pDBInfo,
                          tsp7_table_description  *pTInfo,
                          tsp7_index_description  *pIInfo,
                          Tools_DynamicUTF8String  TableName,
                          tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13CreateIndexes
  -----------------------------------------------------------------------------

  description:  Creates all indexes on the table that are retrieved out of 
                tsp7_index_description structure.

  arguments:    pDBInfo      [IN]  - structure with all info on the running
                                      session and the database connected to
                pTInfo       [IN]  - tsp7_table_description structure
                pIInfo       [IN]  - tsp7_index_description structure
                TableName    [IN]  - full qualified table name; UTF8 encoded
                pszErrText   [OUT] - error text

  returnvalue:  errInternSQL_els98 - may be caused by an sql error creating indexes
                                      or by some communication error
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13CreateIndexes(tls00_DBInfo            *pDBInfo,
                  tsp7_table_description  *pTInfo,
                  tsp7_index_description  *pIInfo,
                  Tools_DynamicUTF8String  TableName,
                  tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13GetDefaultValue
  -----------------------------------------------------------------------------

  description:  Defines default value for a specified column of specified table.
                Distinguishes between kernel version 7.2 and 7.3.
                Gives default value and default value length back INCLUDING
                defined byte.

                The default value is only a pointer into the answer packet from
                kernel. Thus the content must be copied after function returned.
                The pointer will be lost with the next call to a function that
                communicates with the kernel.

  arguments:    pDBInfo        [IN]  - structure with all info on the running
                                        session and the database connected to
                pszTableName   [IN]  - full qualified table name
                lColumnNo      [IN]  - Numnber of column function defines value for
                pDefault       [OUT] - Defined default value
                lDefaultLength [OUT] - Defined default value length
                pszErrtext     [OUT] - error text

  returnvalue:  errOK_els00       - no error
                errInternal_els98 - error getting default value

                Function logs the error message to return string and log file.
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13GetDefaultValue(tls00_DBInfo             *pDBInfo,
                    Tools_DynamicUTF8String  &TableName,
                    tsp00_Int2                lColumnNo,
                    _TCHAR                  *&pDefault,
                    tsp00_Int2               &lDefaultLength,
                    tsp00_Addr                pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls13CheckTableExistence
  -----------------------------------------------------------------------------

  description:  Checks the existence of table <owner name>.<table name> in
                target db.

  arguments:    pDBInfo       [IN]  - contains all information on the running
                                       session and the database connected to.
                TableOwner    [IN]  - table owner UTF8 encoded
                TableName     [IN]  - table name UTF8 encoded
                bTableExists  [OUT] - indicates existence of table
                pszErrText    [OUT] - error text

  returnvalue:  errOK_els00                   - no error
                errCheckTableExistence_els98  - error executing command that
                                                checks the existence
                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13CheckTableExistence(tls00_DBInfo            *pDBInfo,
                        Tools_DynamicUTF8String  TableOwner,
                        Tools_DynamicUTF8String  TableName,
                        bool                    &bTableExists,
                        tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  EndChapter: Functions for DB object support
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Chapter: Special functions to handle packet exchange for FASTLOAD, TABLEEXTRACT and TABLELOAD
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
 
  function:     ls13Prepare_FL_TL_Packet

  -----------------------------------------------------------------------------

  description:  Prepares send packet for TABLELOAD and FASTLOAD. Initializes
                the packet with message type sp1m_load and puts first part
                in with part kind page.

  arguments:    pDBInfo    [IN]  - structure with all info on the running
                                    session and the database connected to
                lPcktNo    [IN]  - packet number to use
                pSqlPacket [OUT] - prepared packet
                pSqlPart   [OUT] - prepared page part

  returnvalue:  no
  -----------------------------------------------------------------------------
*/
void
ls13Prepare_FL_TL_Packet(tls00_DBInfo *pDBInfo,
                         SAPDB_Int4    lPcktNo,
                         tsp1_packet  *&pSqlPacket,
                         tsp1_part    *&pSqlPart);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls13SendBdinfoPckt

  -----------------------------------------------------------------------------

  description:  Sends packets used for FASTLOAD and TABLELOAD.
                Function takes the filled packet (part of part kind page), adds
                a part of part kind bdinfo and sends the packet to the kernel.
                It does not wait for the answer of the kernel.

  arguments:    pDBInfo     [IN]  - structure with all info on the running
                                     session and the database connected to
                lPcktNo     [IN]  - packet number to use for request/receive
                pBdInfo     [IN]  - bdinfo to send to kernel
                pszErrtext  [OUT] - error text

  returnvalue:  errOK_els00
                errors from sending the packet
                
                Function logs the error message and returns the message and the
                error number to caller.
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13SendBdinfoPckt(tls00_DBInfo*  pDBInfo,
                   SAPDB_Int4     lPcktNo,
                   tls00_BDInfo*  pBdinfo,
                   tsp00_Addr     pszErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls13ReceiveBdinfoPckt

  -----------------------------------------------------------------------------

  description:  Receives answer packet on those send in  ls13SendBdinfoPckt
                (only used for FASTLOAD and TABLELOAD).
                Gets the bdinfo from kernel out of the packet and delivers it
                to the caller.

  arguments:    pDBInfo     [IN]  - structure with all info on the running
                                     session and the database connected to
                lPcktNo     [IN]  - packet number to use for request/receive
                pBdInfo     [OUT] - bdinfo gotten from kernel
                pszErrtext  [OUT] - error text

  returnvalue:  errOK_els00
                errors from receiving the packet
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13ReceiveBdinfoPckt(tls00_DBInfo*  pDBInfo,
                      SAPDB_Int4     lPcktNo,
                      tls00_BDInfo*  pBdInfo,
                      tsp00_Addr     pszErrText);

/*!
  -----------------------------------------------------------------------------
  EndChapter: Special functions to handle packet exchange for FASTLOAD, TABLEEXTRACT and TABLELOAD
  -----------------------------------------------------------------------------
*/

#endif /*HLS13*/