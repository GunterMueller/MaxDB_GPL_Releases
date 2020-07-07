/*!
    @file    hls26_TETable.h
    @author  SteffenS and d033893
    @brief   Definition of functions for TABLEEXTRACT of a single table

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

#ifndef HLS26_TETABLE
#define HLS26_TETABLE

#include "gls00.h"


//!  No more data to extract for the special kind of data currently handled
#define STOP_ANALYSE_LS26          -1

//!  Statement used to store short LONGs
#define SAVE_LONG_STMT_LS26        _T("BEGIN SAVE LONG \"%s\"")

//!  Statement used to store primary data
#define SAVE_TABLE_STMT_LS26       _T("BEGIN SAVE TABLE \"%s\"")

//!  Statement used to store long LONGs
#define SAVE_COLUMN_STMT_LS26      _T("BEGIN SAVE COLUMN")

//!  Defines the maximum file extension size for splitted data files
#define FILE_EXTENSION_SIZE_LS26   4

//!  Defines the maximum count of splitted data files
#define FILECOUNT_MXLS26           9999

//!  Statement length
const tsp00_Int4 lStmtLength_ls26 = 4 * KNL_IDENTIFIER_SIZE;


// AUFBAU DES HEADERS FUER EINE DATENDATEI (1 KBYTE)
// swap des servers bei extract     1 byte
// code type (ascii, ebcdic, ...)   1
// identifier                       64
// version                          40
// datum                            8
// time                             8

// page size der extrahierten db    4

// count                            4 gibt an, ob und wieviele datenseiten pro datei geschrieben wurden
// mehrere tabellen pro datei       1 wahr oder falsch


// DIESE ANGABEN SIND EIGENTLICH UNNOETIG, DA SIE IN DER MITEXTRAHIERTEN TABELLENBESCHREIBUNG ENTHALTEN SIND
// table name                       64
// user name                        18
// constraint count                 2
// index count                      2
// user defined defaults            2


/*!
    \brief  Header of meta data pages in tableextract data files

            Used for pages containing table description, index
            description, default and constraint definitions.
*/
typedef struct tls26_DescPageHeader
{
    tsp00_Int4       dphPageNo_ls26;                //!<  count of pages save in data file
    tsp00_Uint1      dphType_ls26;                  //!<  special type; marks a page as a Loader page
    tsp00_C3         dphFiller1_ls26;               //!<  filler
    tls00_TEPageType dphPageType_ls26;              //!<  type of trailer page: table desc, index desc, defaults ...
    tsp00_Int4       dphSpecialID_ls26;             //!<  for special purposes
    tsp00_Int4       dphMore_ls26;                  //!<  meta data spans more than one page
    tsp00_Int4       dphFillerInt_ls26;             //!<  filler
} tls26_DescPageHeader;


//!  trailer of tableextract data files
typedef struct tls26_TEFileTrailer
{
    tsp00_Int4       tftPageNo_ls26;                //!<  count of pages save in data file
    tsp00_Uint1      tftType_ls26;                  //!<  special type; marks a page as a Loader page
    tsp00_C3         tftFiller1_ls26;               //!<  filler
    tls00_TEPageType tftPageType_ls26;              //!<  type of trailer page: tptEOF_ls00
    tsp00_Int4       tftNumOfTabInFile_ls26;        //!<  number of tables in data file
    tsp00_Int4       tftNumOfPagesInFile_ls26;      //!<  number of pages in data file
    bool             tftTableIncomplete_ls26;       //!<  indicates if last table in data file is incomplete
    tls00_TEPageType tftDataTypeAtInterrupt_ls26;   //!<  in case last table has been saved incompletely - the last save type of data
} tls26_TEFileTrailer;



/*!
    \brief  Extracts a single table in database internal format

    \param  pDBInfo       [IN]  - structure with all info on the running
                                   session and the database connected to.
    \param  pDBInfoODBC   [IN]  - ODBC connection and meta data information
    \param  pTransformationCmd   [IN]  - user delivered information on TABLEEXTRACT
    \param  pTabExtrRec   [IN]  - structure with all info needed
                                   to execute table extract
    \param  ErrText       [OUT] - error text

    \return  errOK_els00   - No error
*/
tsp00_Int4
ls26ExportTablePages(MetaDataDef        *&pMetaDataDef,
                      tls00_TextractRec  *pTabExtrRec,
                      tsp00_Addr         ErrText); 

#endif /*HLS26_TETABLE*/