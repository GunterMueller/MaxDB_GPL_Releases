/*!
    @file    hls28_TLTable.h
    @author  SteffenS and d033893
    @brief   Definition of functions for TABLELOAD of a single table


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

#ifndef HLS28_TLTABLE
#define HLS28_TLTABLE

#include "gls00.h"
#include "hls51.h"

//!  Statement used to check if table is empty
#define EMPTY_TABLE_STMT_LS28        _T("SELECT 1 FROM \"%s\" WHERE ROWNO <= 1")

//!  Statement used to create a table using a binary table description
#define INTO_TABLE_STMT_LS28         _T("INTO TABLE \"%s\"")

//!  Statement used to signal the kernel the end of creating a table
#define END_LOAD_TABLE_STMT_LS28     _T("END LOAD \"%s\"")

//!  Statement used to restore short LONGs
#define RESTORE_STRING_STMT_LS28     _T("RESTORE LONG \"%s\"")

//!  Statement used to restore primary data
#define RESTORE_DATA_STMT_LS28       _T("RESTORE TABLE \"%s\"")

//!  Statement used to restore long LONGs
#define RESTORE_LONG_STMT_LS28       _T("RESTORE COLUMN")

//!  Statement used to signal the kernel the end of restoring a table
#define END_RESTORE_STMT_LS28        _T("END RESTORE")



#define STOP_LS28                    -1     //!<  Return value - stop processing
#define STOP_NODATA_LS28             -2     //!<  Return value - stop processing; no data
#define UNKNOWN_TE_HEADER_LS28       -3     //!<  Return value - unknown page header type
#define WRONG_TE_HEADER_LS28         -4     //!<  Return value - wrong page header type; expected other


/*!
    Restores a single table with the data found in a data file 
    generated by TABLEEXTRACT.

    \param  DBInfo       [IN]   - structure that contains all information on the
                                   running session and the database connected to.
    \param  pDBInfoODBC  [IN]   - ODBC connection and meta data information
    \param  pTransformationCmd  [IN]   - user delivered information on TABLELOAD
    \param  pTabLoadRec  [IN]   - structure, holding information for executing
                                   TABLELOAD for a single table
    \param  readBuffer   [IN]   - Readbuffer class object
    \param  ErrText      [OUT]  - error text

    \return  errOK_els00  - no error
*/
tsp00_Int4
ls28ImportTablePages(MetaDataDef     *&pMetaDataDef,
                   tls00_TloadRec  *pTabLoadRec,
                   ReadBuffer      &readBuffer,
                   tsp00_Addr      ErrText); 

#endif


/*===========================================================================*
 *     END
 *===========================================================================*/
