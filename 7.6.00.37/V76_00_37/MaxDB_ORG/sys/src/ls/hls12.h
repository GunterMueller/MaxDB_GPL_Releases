/*! 
  -----------------------------------------------------------------------------
 
  module: hls12.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Cursor
                
  version:      7.6.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1999-2005 SAP AG-2003
 
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

#ifndef HLS12
#define HLS12

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------
 
  function:     ls12DeclareCursor

  -----------------------------------------------------------------------------

  description:  Open named (pszResultTabName != NULL) or unnamed result table.
                The open cursor command is always executed with info flag.

  arguments:    DBInfo           [IN]  - db session structure
                pszResultTabName [IN]  - name of cursor; if NULL a unnamed result
                                          table is created by simply running the
                                          select statement
                pszSelectStmt    [IN]  - select statement
                pszErrText       [OUT] - error text

  returnvalue:  errOK_els00          - no error
                SQL error code       - running open cursor command failed
                dbRowNotFound_els00  - empty table

                Function delivers in case of error the SQL error text to the caller.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12DeclareCursor(tls00_DBInfo  &DBInfo,
                  tsp00_Addr     pszResultTabName,
                  tsp00_Addr     pszSelectStmt,
                  tsp00_Addr     pszErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls12Fetch

  -----------------------------------------------------------------------------

  description:  Fetches named (pszResultTabName != NULL) or unnamed result table
                and gives number of fetched rows back.

  arguments:    DBInfo           [IN]     - db session structure
                pszResultTabName [IN]     - cursor name; if NULL an unnamed result
                                             table is fetched
                RowCount         [IN/OUT] - number of rows requested/number of
                                             rows gotten
                bMassCmd         [IN]     - execute as mass command or not
                pszErrText       [OUT]    - error text

  returnvalue:  errOK_els00                      - no error
                dbRowNotFound_els00 (= 100)      - no error, but no more row found        
                sql error, errUnknownPart_els98  - running fetch command failed
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12Fetch(tls00_DBInfo &DBInfo,
          tsp00_Addr    pszResultTabName,
          tsp00_Int4   &RowCount,
          bool          bMassCmd,
          tsp00_Addr    pszErrText);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls12FetchPos

  -----------------------------------------------------------------------------

  description:  Fetches named (pszResultTabName != NULL) or unnamed result table
                and gives number of fetched rows back.

  arguments:    DBInfo           [IN]     - db session structure
                pszResultTabName [IN]     - cursor name; if NULL an unnamed result
                                             table is fetched
                RowCount         [IN/OUT] - number of rows requested/number of
                                             rows gotten
                Pos              [IN]     - position for FETCH POS command
                                            
                bMassCmd         [IN]     - execute as mass command or not
                pszErrText       [OUT]    - error text

  returnvalue:  errOK_els00                      - no error
                dbRowNotFound_els00 (= 100)      - no error, but no more row found        
                sql error, errUnknownPart_els98  - running fetch command failed
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12FetchPos(tls00_DBInfo &DBInfo,
             tsp00_Addr    pszResultTabName,
             tsp00_Int4   &RowCount,
             tsp00_Longint Pos,
             bool          bMassCmd,
             tsp00_Addr    pszErrText);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls12FetchEx

  -----------------------------------------------------------------------------

  description:  Fetches named (pszResultTabName != NULL) or unnamed result table
                and gives back:
                - number of fetched rows
                - pointer to data in packet

  arguments:    DBInfo           [IN]     - db session structure
                pszResultTabName [IN]     - cursor name; if NULL a unnamed result
                                             table is fetched
                RowCount         [IN/OUT] - number of rows requested/number of
                                             rows gotten
                bMassCmd         [IN]     - execute as mass command or not
                pszBuffer        [OUT]    - pointer to data
                lBufferLen       [OUT]    - length of data
                pszErrText       [OUT]    - error text

  returnvalue:  errOK_els00                      - no error
                sql error, errUnknownPart_els98  - running fetch command failed
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12FetchEx(tls00_DBInfo  &DBInfo,
            tsp00_Addr     pszResultTabName,
            tsp00_Int4    &RowCount,
            bool           bMassCmd,
            _TCHAR       *&pszBuffer,
            tsp00_Int4    &lBufferLen,
            tsp00_Addr     pszErrText);


/*!
  -----------------------------------------------------------------------------
   function:     ls12CloseCursor
  -----------------------------------------------------------------------------

  description:  Closes named result table.

  arguments:    DBInfo           [IN]   - db session structure
                pszResultTabName [IN]   - name of result table
                pszErrText       [OUT]  - error text; caller must supply a buffer
                                          of at least 256 Bytes

  returnvalue:  errOK_els00     - no error
                SQL error code  - running open cursor command failed

                Function delivers in case of error the SQL error text to
                the caller.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12CloseCursor(tls00_DBInfo &DBInfo,
                tsp00_Addr    pszResultTabName,
                tsp00_Addr    pszErrText);


/*
  -----------------------------------------------------------------------------

  function: ls12CreateResultTabName

  -----------------------------------------------------------------------------

  description:  Builds a result table name of application name date, and time.
                (Prevents buffer overflow bug)

  arguments:    pszRTabName  [IN] - buffer to hold name, should at least have a
                                     size of KNL_IDENTIFIER_SIZE
                lRTabNameLen [IN] - buffer length

  returnvalue:  >= 0 - bytes actually written to buffer
                <  0 - buffer to small
  -----------------------------------------------------------------------------
*/
tsp00_Uint4
ls12CreateResultTabName(tsp00_Addr pszRTabName, tsp00_Int4 lRTabNameLen);

#endif  //#define HLS12