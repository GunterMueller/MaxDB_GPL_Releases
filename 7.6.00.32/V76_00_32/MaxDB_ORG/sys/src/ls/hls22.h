/*! 
  -----------------------------------------------------------------------------
 
  module: hls22.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Fastload - filling db pages (definitions)

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

#ifndef HLS22
#define HLS22

#include "gls00.h"

/*! Declaration: Special return codes */

#define STOP_LS22                    101
#define STOP_NODATA_LS22             102

/*! EndDeclaration: Special return codes */

typedef struct tls22_Statistics
{
    tsp00_Key  LastKey_ls22;                // Last key accepted
    SAPDB_Int4 lLastKeyLength_ls22;         // Length of last key accepted
    SAPDB_Int4 lPageCount_ls22;             // Count of built pages sent to kernel; limited by kernel to MAX_INT4_SP00
    SAPDB_Int8 lRecordCount_ls22;           // Count of processed records
} tls22_Statistics;


/*!
  -----------------------------------------------------------------------------
  Chapter: global functions
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  function:     ls22UpdateTransformationPackage
  -----------------------------------------------------------------------------

  description:  Writes restart entry to restart table
                Used only for dataload all/user/table

  arguments:    FloadRec   [IN]  - contains restart entry to write
                ErrText    [OUT] - error text

  returnvalue:  errOK_els00                     - no error
                errors from writing to file

                The function logs the error message to the log file and to
                the error text return buffer.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls22UpdateTransformationPackage( tls00_DBInfo      *pDBInfo,
                                 TransformationDef *pTransformationDef,
                                 tls00_FloadRec*   FloadRec,
                                 tsp00_Addr        ErrText);
/*!
  -----------------------------------------------------------------------------
 
  function:     ls22LoadPages

  -----------------------------------------------------------------------------

  description:  Loads data pages to a given db table (FASTLOAD).
                Initializes file structure for the given data file, opens it,
                reads data records, builds up a db data page and sends the
                pages to the db.
  
  arguments:    FloadCmd  [IN]   - fastload structure built by the parser from
                                    given command
                FloadRec  [IN]   - fastload structure built by the server
                DBInfo    [IN]   - dbinfo structure
                bRollback [OUT]  - indicates to roll back or commit inserted rows
                                    up to erroneous row in case of error
                ErrText   [OUT]  - error text

  returnvalue:  errOK_els00             - data loaded
                errors when loading data failed

                Logs the error in log file.
  -----------------------------------------------------------------------------
*/
int ls22LoadPages(TransformationDef *pTransformationDef,
                  tls00_FloadRec*    pFloadRec,
                  tls00_DBInfo*      pDBInfo,
                  SAPDB_Bool*        pbRollback,
                  tsp00_Addr         ErrText);

/*!
  -----------------------------------------------------------------------------
  End Chapter: global functions
  -----------------------------------------------------------------------------
 */

#endif /*HLS22*/