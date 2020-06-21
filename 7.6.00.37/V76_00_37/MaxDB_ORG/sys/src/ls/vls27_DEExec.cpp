/*!
  -----------------------------------------------------------------------------
  module: vls27_DEExec.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Implementation of functions for dataextract execution

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
#include "hls03.h"
#include "hls04.h"
#include "hls041.h"
#include "hls05.h"
#include "hls07.h"
#include "hls08_long.h"
#include "hls12.h"
#include "hls13.h"
#include "hls15.h"
#include "hls18.h"
#include "hls19_check.h"
#include "hls24.h"
#include "hls25.h"
#include "hls27_DEExec.h"
#include "hls27_dataextract.h"
#include "hls30.h"
#include "hls31_odbc_Result.h"
#include "hls98msg.h"
#include "hls99.h"

#include "hsp77.h"

_TCHAR const *pszLoadCmdBeginString[6] = {_T("NOT USED"),
                                          _T("DATALOAD TABLE "),
                                          _T("FASTLOAD WITH 80%% USAGE TABLE "),
                                          _T("DATAUPDATE TABLE "),
                                          _T("DATALOAD TABLE "),
                                          _T("FASTLOAD WITH 80%% USAGE TABLE ")};

_TCHAR const *pszTabLoadCmdBeginString[6]= {_T("NOT USED"),
                                           _T("LOAD TABLE "),
                                           _T("LOAD TABLE FAST WITH 80%% USAGE "),
                                           _T("UPDATE TABLE "),
                                           _T("LOAD TABLE "),
                                           _T("LOAD TABLE FAST WITH 80%% USAGE ")};



_TCHAR const *pszDateTimeType[6] = {_T("FREE MASK"),
                                    _T("ISO"),
                                    _T("USA"),
                                    _T("EUR"),
                                    _T("JIS"),
                                    _T("INTERNAL")};

_TCHAR const *pszEncoding[8] = {_T("NOT USED"),
                                _T("ASCII"),
                                _T("EBCDIC"),
                                _T("UTF8"),
                                _T("BINARY"),
                                _T("UCS2"),
                                _T("UCS2"),
                                _T("UCS2")};

/*
  -----------------------------------------------------------------------------
  Chapter: Private functions declaration
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------

  function:     ls27_InitDatExtrRec - local function

  -----------------------------------------------------------------------------

  description:  Sortiert die spaltenangaben nach startpositionen.
                Ueberprueft die positionsangaben fuer die spalten in der
                spaltenliste auf ueberlappungen oder ueberschneidungen.
                Ueberprueft die laengenangaben fuer die spalten in der
                spaltenliste.
                Erstellt den 'mapping index' zwischen den spaltenangaben in
                tls00_MultColSpec und in TableInfo.
                Wurden keine spalten in der spaltenliste angegeben, dann
                fuellt die funktion die struktur tls00_MultColSpec.
                Fuer BLOB-Spalten wird im fall von klartextdateien das ausgabeformat
                auf hex gesetzt.

  arguments:    pTransformationCmd  [IN]  - pointer to dataextract command structure
                pDatExtrRec  [IN]  - pointer to dataextract record structure
                ErrText      [OUT] - Error text

  returnvalue:  errNotInSelList_els98     - spalten in spaltenliste aber nicht in
                                            select liste
                errWrongFieldLength_els98 - spalten in spaltenliste mit falschen
                                            laengenangaben
                errOK_els00               - keine fehler
  -----------------------------------------------------------------------------
*/
static int
ls27_InitDatExtrRec(tls00_DBInfo              *pDBInfo,
                    MetaDataDef               *&pMetaDataDef,
                    tls00_TransformationCmd   *pTransformationCmd,
                    tls00_DextractRec         *&pDatExtrRec,
                    tsp00_Addr ErrText);
/*
  -----------------------------------------------------------------------------

  function:     ls27_FreeDatExtrRec

  -----------------------------------------------------------------------------

  description:  Deallocates memory in DataExtract structure

  arguments:    pTransformationCmd  [IN]  - pointer to dataextract command structure
                pDatExtrRec  [IN]  - pointer to dataextract record structure

  returnvalue:  None

  -----------------------------------------------------------------------------
*/
static void ls27_FreeDatExtrRec( tls00_DBInfo              *pDBInfo,
                                 tls00_TransformationCmd   *pTransformationCmd,
                                 tls00_DextractRec        *&pDatExtrRec );

/*
  -----------------------------------------------------------------------------

  function:     ls27_InitStreams  - local function

  -----------------------------------------------------------------------------

  description:


  arguments:    pDBInfo      [IN]   - structure with all information on the running
                                      session and the db connected to.
                pTransformationCmd  [IN]   - structure with information for te supplied by the user
                pDatExtrRec  [IN]   - dataextract record structure that must be initialized.

  returnvalue:

  -----------------------------------------------------------------------------
*/

static tsp00_Int4
ls27_InitStreams(tls00_TransformationCmd  *pTransformationCmd,
                 tls00_DextractRec        *pDatExtrRec,
                 tsp00_Addr                ErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls27_ParseSelectStmt - local function

  arguments:    pTransformationCmd  [IN]  - pointer to dataextract command structure
                pDatExtrRec  [IN]  - pointer to dataextract record structure
                ErrText  [OUT] - Error text

  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_ParseSelectStmt(tls00_DBInfo             *pDBInfo,
                     tls00_TransformationCmd  *pTransformationCmd,
                     tls00_DextractRec        *pDatExtrRec,
                     tsp00_Addr                pszErrText,
                     bool                      bNoMessage = false);

/*
  -----------------------------------------------------------------------------
  function:     ls27_InitAndOpenLongFiles
  -----------------------------------------------------------------------------

  description:  Initializes VFile structures for long files and opens them.

  arguments:    pMultCol        [IN]  - User delivered data on output columns;
                                         stores the long data file struct, too
                pColumnData     [IN]  - Internal struct for column data description
                pDataFile       [IN]  - Data file struct
                bCompactFormat  [IN]  - Output in compact format or not
                ErrText         [OUT] - Error text

  returnvalue:  errOK_els00        - Success
                errFileOpen_els98  - error opening file
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_InitAndOpenLongFiles(tls00_MultColSpec  *pMultCol,
                          tls00_ColumnData   *pColumnData,
                          tls00_VFile       *&pDataFile,
                          bool                bCompactFormat,
                          tsp00_Addr          pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls27_TransformTableRows

  -----------------------------------------------------------------------------

  description:

  attention:

  arguments:


  returnvalue:
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_TransformTableRows(tls00_DBInfo              *DBInfo,
                        tsp00_Int4                RowCount,
                        tls00_DextractRec         *pDatExtrRec,
                        tls00_TransformationCmd   *pTransformationCmd,
                        tls00_Buffer2             *pConvertBuffer,
                        tsp00_Addr                pszErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls27_TransformRowColumns
  -----------------------------------------------------------------------------

  description:

  arguments:    DBInfo        [IN]     - pointer; to db session structure
                pDatExtrRec       [IN]     - pointer; to dataextract record structure
                pColumn       [IN]     - pointer; to column structure
                pszMsgBuffer  [IN]     - zeiger; datenpuffer im fetch-paket
                pResultString [OUT]    - reference to pointer; enthaelt die parameter
                                         der longdatendatei fuer die zieldatendatei;
                                         muss als NULL-pointer uebergeben werden, da
                                         der wert neu angelegt wird
                pszErrText    [OUT]    - Fehlertext


  returnvalue:  errOK_els00          - wenn alles ohne fehler abgelaufen ist
                errLongFileNumGTMax_els98 - fehler beim auslesen der deskriptoren im

                Fehler von ls27_ExecGETVAL

                Liefert den fehlercode und auch gleich den fehlertext im fehlerstring.
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_TransformRowColumns(tls00_DBInfo      *DBInfo,
                         tls00_DextractRec *pDatExtrRec,
                         tls00_TransformationCmd  *pTransformationCmd,
                         tls00_Buffer2     *pConvertBuffer,
                         tsp00_Addr         pszErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls27_GetLongLength
  -----------------------------------------------------------------------------

  description:  Gets length of all not null long values in one record

  attention:

  arguments:


  returnvalue:
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_GetLongLength(tls00_DBInfo      *pDBInfo,
                   tls00_DextractRec *pDatExtrRec,
                   tls00_TransformationCmd  *pTransformationCmd,
                   tsp00_Addr         pszErrText);



/*
  -----------------------------------------------------------------------------
  function:     ls27_EditLongOffsetForOutput
  -----------------------------------------------------------------------------

  description:

  arguments:    pColumnData     [IN] - column data structure
                pLongExecInfo   [IN] - info on number of extracted longs
                pMultCol        [IN] - column descriptions supplied by user

  returnvalue:  errOK_els00               - everything ok
                errLongFileNumGTMax_els98 - counter for numbered files exceeds max
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_EditLongOffsetForOutput(tls00_ColumnData     *pColumnData,     //destination
                             tls00_LongExecInfo   *pLongExecInfo,   //source
                             tls00_MultColSpec    *pMultCol,
                             bool                  bUnicodeOI,
                             tsp00_SwapKind_Enum   SwapKind,
                             tsp81_CodePage       *pCodePage,
                             tls00_Buffer2        *pConvertBuffer,
                             tsp00_Addr            pszErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls27_SpecialConversion
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_SpecialConversion(tls00_ColumnData    *pSingleColData,
                       bool                 bUnicodeOI,
                       tsp81_CodePage      *pCodePage,
                       tls00_Buffer2       *pConvertBuffer,
                       tsp00_Addr           pszErrText);


/*!
  -----------------------------------------------------------------------------

  function:     ls27_AnalyzeLongInfo - local function

  -----------------------------------------------------------------------------

  description:  Analysiert die informationen, die fuer das entladen von
                LONG-Spalten benoetigt werden. Dafuer werden die informationen
                vom Kern ueber die spalten und die informationen vom benutzer
                in der struktur tls00_ParamInfo_LONG (innerhalb von tls00_Column
                gespeichert und angepasst.
                Passt die folgenden Variablen der Struktur an fuer jede zu entladende
                LONG-Spalte an:
                    loibFileNumbered_ls00
                    loiNumberSize_ls00
                    loiGreatestNum_ls00
                    loiFileName_ls00
                    loiFileOffset_ls00 ==> 1
                Alle anderen werte sind beim parsen bereits auf 0 gesetzt worden.
                Die funktion reserviert fuer das array pDatExtrRec->derLongDescArray_ls00
                speicher, wenn die anzahl der longspalten ungleich 0 ist

                Die folgenden szenarios sind beim entladen von LONG-Spalten moeglich
                1. es sollen longspalten entladen werden, aber es sind keine
                   longfileinfos im kommando
                   --> fehler               ODER
                   es sollen longspalten in normale datentypen entladen werden
                   --> fehler
                2. die LONG-spalte, die im kommando angegeben wurde, gibt es nicht
                   --> die longfileinfo wird nicht beachtet
                3. es werden keine long spalten entladen
                   --> sehr schoen, nichts zu tun
                4. es sollen long spalten entladen werden und alles ist richtig
                5. mehrere unterschiedliche/gleiche longfileinfos fuer eine (gleiche) spalte
                   --> die erste longfileinfo wird beachtet, die andere verworfen und
                       eine warnung ausgegeben, dass es ueberfluessige longfileinfos gibt

  arguments:    pTransformationCmd  [IN/OUT]  - pointer; extract command structure
                pDatExtrRec  [IN/OUT]  - pointer; extract record structure
                ErrText  [OUT]     - Error text

  returnvalue:  errOK_els00               - alles fehlerfrei gelaufen
                errSurplusLongInfo_els98  - warnung: zusaetzliche, ueberzaehlige long infos
                                            im kommando, die keiner der angegebenen spalten
                                            zugeordnet werden koennen
                errNoLongFileInfo_els98   - long infos fuer angegebene long spalten
                                            fehlen
                errAnalyzeLongSum_els98   - mehr als ein fehler aufgetreten; die fehler sind
                                            in der protokolldatei vermerkt
                errWrongFieldLength_els98 - fuer eine durchnumerierte datei, die die longdaten
                                            aufnehmen soll, wurde eine zu kleine laenge
                                            in der zieldatendatei angegeben
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_AnalyzeLongInfo(tls00_TransformationCmd  *pTransformationCmd,
                     tls00_DextractRec *pDatExtrRec,
                     tsp00_Addr         pszErrText);



/*!
  -----------------------------------------------------------------------------

  function:     ls27_FillColStruct - local function

  -----------------------------------------------------------------------------

  description:  Fuellt eine struktur vom typ tls00_MultColSpec mit allen fuer
                ein dataextract notwendigen informationen zu den spalten. Dazu
                wird die struktur TableInfo ausgewertet, die zuvor gefuellt worden
                sein muss.
                Bei der Laengenbestimmung der einzelnen spalten im formatted-fall
                werden die laengen fuer die ausgabewerte fuer boolsche spalten
                und nullwerte des replication servers beruecksichtigt.
                Das in der dataextract-anweisung keine spalten in der spaltenliste
                angegeben wurden, werden alle spalten mit dem ausgabeformat
                ioChar_ls00 beschrieben. Eine ausgabe im hex-format ist in diesem
                fall ebenfalls nicht moeglich.
                Die folgenden werte werden in der tls00_Column-struktur innerhalb
                struktur vom typ tls00_MultColSpec nicht oder auf den default gesetzt
                colNumber_ls00       --> not used in this case
                colIsLit_ls00        --> automatically set to false
                colIsKey_ls00        --> automatically set to false
                colIsSet_ls00        --> automatically set to false
                colAbstractType_ls00 --> automatically set to unknown!!
                colSpecialIndex      --> not used here
                colOption_ls00       --> not used here
                colAssign_ls00       --> not used here
                colNullCond_ls00     --> no condition in this case
                colLongInfo          --> must be defined later in any case

  arguments:    pTransformationCmd    [IN/OUT]  - pointer; to extract command structure, which
                                       contains the structure of type tls00_MultColSpec
                TableInfo  [IN]      - pointer; contains all necessary information
                                       on the columns

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls27_FillColStruct(tls00_TransformationCmd *pTransformationCmd, tls00_TableDescription *TableInfo);


/*
  -----------------------------------------------------------------------------

  function:     ls27_MakeMapIndex - local function

  -----------------------------------------------------------------------------

  description:  Builds 'mapping index' from column descriptions in structure
                MultCol and TableInfo.
                Checks for columns given in the output column list but not
                selected.
                Enthaelt das kommando literale, ist der vergebene index immer 0.
                Soll in weiteren funktionen mit diesem erstellten index gearbeitet
                werden, muessen die literale extra behandelt werden, da sonst fehler
                auftreten koennen.
                Es werden erst alle spalten ueberprueft und dann eine
                'gesammelte' fehlermeldung erzeugt, wenn notwendig.

                ATTENTION: Columns in MultCol must be sorted ascending by positions

  arguments:    MultCol     [IN]    - pointer; to column structure
                TableInfo   [IN]    - pointer; to table description
                pColumnData [OUT]   - Internal struct for column data description;
                                       stores the 'mapping index'
                pszErrText  [OUT]   - Error text

  returnvalue:  errNotInSelList_els98 - spalten in spaltenliste aber nicht in
                                        select liste
                errOK_els00           - keine fehler
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_MakeMapIndex(tls00_MultColSpec      *MultCol,
                  tls00_TableDescription *TableInfo,
                  tls00_ColumnData       *pColumnData,
                  tsp00_Addr              pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls27_CheckLength - local function

  -----------------------------------------------------------------------------

  description:  Prueft die laengenangaben fuer die spalten in der spaltenliste.
                Beruecksichtigt die darstellungen fuer boolsche werte und fuer
                die moegliche hexadezimale ausgabe der werte.
                Laengen der Nullwertdarstellungen werden ebenfalls ueberprueft,
                wenn die spalte nullwerte zulaesst. Ist die laenge fuer die
                ausgabe der nullwerte zu klein, werden aber nur warnungen
                erzeugt.
                Laengend der angegebenen Literale im Kommando werden ebenfalls
                geprueft, aber nur warnungen ins server log file ausgegeben.
                Es werden erst alle spalten ueberprueft und dann eine
                'gesammelte' fehlermeldung erzeugt, wenn notwendig.

  arguments:    MultCol     [IN]  - pointer to column structure
                TableInfo   [IN]  - pointer to table description
                FileFormat  [IN]  - pointer to data file format structure
                pColumnData [IN]  - Internal struct for column data description
                IntFracPart [IN]  - 2-dim array which holds calculated
                                   integral and fractional parts of numeric
                                   columns
                pszErrText  [OUT] - Error text

  returnvalue:  errWrongFieldLength_els98 - spalten in spaltenliste mit falschen
                                            laengenangaben
                errOK_els00               - keine fehler
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_CheckLength(tls00_MultColSpec      *MultCol,
                 tls00_TableDescription *TableInfo,
                 tls00_FileFormat       *FileFormat,
                 tls00_ColumnData       *pColumnData,
                 tsp00_Int4             (*IntFracPart)[2],
                 tsp00_Addr             pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls27_CheckCompatibility - local function

  -----------------------------------------------------------------------------

  description:  Prueft die kompatibilitaet von db und externen datentypen sowie
                der datentypen der gewaehlten nullwertdarstellung einer spalte und
                deren externen datentyp.

                Es werden erst alle spalten ueberprueft und dann eine
                'gesammelte' fehlermeldung erzeugt, wenn notwendig.

  arguments:    MultCol    [IN]  - pointer to column structure
                TableInfo  [IN]  - pointer to table description
                pColumnData [IN]  - Internal struct for column data description
                pszErrText [OUT] - Error text

  return type:  tsp00_Int4

  returnvalue:  errIncompatibleDataTypes_els98 - datentypen sind nicht kompatibel
                errCompatibilityCheckSum_els98 - mehr als eine spalte mit nicht
                                                 kompatiblen datentypen
                errIncompatibleNullValue_els98 - datentypen von nullwertdarstellung
                                                 und spalte sind inkompatibel
                errOK_els00                    - keine fehler
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_CheckCompatibility(tls00_MultColSpec      *MultCol,
                        tls00_TableDescription *TableInfo,
                        tls00_ColumnData       *pColumnData,
                        tls00_FileFormat       *FileFormat,
                        tsp00_Addr             pszErrText);


/*
  -----------------------------------------------------------------------------

  function:     ls27_RecalculateLength - local function

  -----------------------------------------------------------------------------

  description:  Berechnet fuer numerische spalten die anzahl der vorkomma- und
                nachkommastellen neu und bestimmt daraus die neue gesamtlaenge
                der zu extrahierenden werte und den anteil an nachkommastellen.
                Eingang in die berechnung finden die daten ueber die spalten
                von der db und eventuell vom kunden angegebene
                skalierungs- und rundungsfaktoren.

  arguments:    MultCol    [IN]  - pointer to column structure
                TableInfo  [IN]  - pointer to table description
                pColumnData [IN]  - Internal struct for column data description
                IntFracPart[OUT] - 2-dim array which holds newly to calculate
                                   integral and fractional parts of numeric
                                   columns

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls27_RecalculateLength(tls00_MultColSpec      *MultCol,
                       tls00_TableDescription *TableInfo,
                       tls00_ColumnData       *pColumnData,
                       tsp00_Int4             (*LenFrac)[2]);   // 2-dim array


/*!
  -----------------------------------------------------------------------------
 
  function:     ls27ExtractTransformation

  -----------------------------------------------------------------------------

  description:  extracts load command file.
                Only called if user specified
                'dataextract for dataload/fastload/dataupdate ...'.
                
                Writes 'create table' command if table belongs to current user
                and is a basic table.
                Writes a 'dataload/...' command if table is not empty. 
                Otherwise generates a message to log file that table is empty.
                Writes 'create index' command if table has indexes.

  arguments:    pDatExtrRec   [IN]   - pointer; extract record structure
                pTransformationCmd   [OUT]  - pointer; extract command structure
                pszOwner      [IN] 
                pszTable      [IN] 
                ErrText       [OUT]  - fehlertext

  returnvalue:  errOK_els00           everything went fine
                errFileOpen_els98     anlegen der kommandodatei klappt nicht
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_ExtractTransformation(MetaDataDef*&             pMetaDataDef,
                           tls00_DextractRec*        pDatExtrRec,
                           tls00_TransformationCmd*  pTransformationCmd,
                           tsp00_Addr                ErrText);

/*
  -----------------------------------------------------------------------------
  EndChapter:   Local function declaration
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  BeginChapter: Global function implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls27ExportTableRows
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27ExportTableRows(MetaDataDef        *&pMetaDataDef,
                    tls00_DextractRec  *pDatExtrRec,
                    tsp00_Addr         ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27ExportTableRows");

    tsp00_Int4  rc  = errOK_els00;

    LoaderMessages*  pMsgObj = LoaderMessages::Instance();

    TransformationDef       * pTransformationDef = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd * pTransformationCmd = pTransformationDef->getTransformationCmd();
    tls00_DBInfo            * pDBInfo            = pMetaDataDef->getConnection();
    SAPDB_Bool                bFormatSwitched    = SAPDB_FALSE;

    tls00_LargeNumber start_Data;
    start_Data.lnu2GB_Blocks_ls00 = 0;
    start_Data.lnuRemainder_ls00  = 0;

    if ( NULL != pDatExtrRec->derDataFile_ls00 )
    {
        start_Data.lnu2GB_Blocks_ls00 = pDatExtrRec->derDataFile_ls00->vfFileLen_ls00.lnu2GB_Blocks_ls00;
        start_Data.lnuRemainder_ls00  = pDatExtrRec->derDataFile_ls00->vfFileLen_ls00.lnuRemainder_ls00;
    }

    //*
    //*  create dataextract record structure
    //*
    rc = ls27_InitDatExtrRec( pDBInfo, pMetaDataDef, pTransformationCmd, pDatExtrRec, ErrText);

    //*
    //*  case table: extract transformation statement
    //*
    if ( errOK_els00 == rc && false == pTransformationCmd->trExclude_ls00  )
    {
        //* pipe
        if (resUnknown_els00 != pTransformationCmd->trRestore_ls00   &&
            NULL             != pDatExtrRec->derCmdFile_ls00  &&
            dtypePipe_ls00   == pDatExtrRec->derCmdFile_ls00->vfFileType_ls00     &&
            ptTable_els00    == pDatExtrRec->derPartSpec_ls00.psPartType_ls00    )
        {
            sp77sprintf(pDatExtrRec->derpStartPos_ls00, 256, "%d:%d", start_Data.lnu2GB_Blocks_ls00, start_Data.lnuRemainder_ls00 );

            rc = ls27_ExtractTransformation(pMetaDataDef, pDatExtrRec, pTransformationCmd, ErrText );
        }
        //* for table load
        if ( resUnknown_els00 != pTransformationCmd->trRestore_ls00 &&
             true             == pTransformationCmd->trInternal_ls00 &&
             errOK_els00      == rc )
        {
             rc = ls27_ExtractTransformation(pMetaDataDef, pDatExtrRec, pTransformationCmd, ErrText);

            return rc;
        }
        if ( NULL  != pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 &&
             true  == pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoDtl_ls00 &&
             errOK_els00 == rc )
        {
             if ( pTransformationCmd->trRestore_ls00 == resUnknown_els00 )
             {
                  pTransformationCmd->trRestore_ls00 = resDataload_els00;
                  rc = ls27_ExtractTransformation(pMetaDataDef, pDatExtrRec, pTransformationCmd, ErrText);
             }
             else if  ( pTransformationCmd->trRestore_ls00 != resUnknown_els00 )
             {
                  rc = ls27_ExtractTransformation(pMetaDataDef, pDatExtrRec, pTransformationCmd, ErrText);
                  return rc;
             }
        }
    }

    if ( errOK_els00 == rc )
         rc = ls27_InitStreams(pTransformationCmd, pDatExtrRec, ErrText);
    //*
    //*  update transformation table and write metadata header
    //*
    if ( errOK_els00    == rc &&
         true           == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00                 && (
         1              == pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 ||
         0              == pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 ||
         UNDEFINED_LS00 == pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00    ))
    {
         rc = ls27UpdateTransformationPackage( pMetaDataDef,pTransformationCmd,pDatExtrRec,ErrText);

         if (errOK_els00 != rc )
         {
             pMsgObj->ls98Msg(layDataExtract_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
             rc = errInternal_els98;
         }
         else
         {
            rc = ls27WriteMetaHeader(pDBInfo, pTransformationCmd, pDatExtrRec, ErrText);
         }
    }

    if ( (errOK_els00 == rc)                             &&
         (true        == pDatExtrRec->derTabEmpty_ls00 ) &&
         (false       == pTransformationCmd->trInternal_ls00)  &&
         (false       == pTransformationCmd->trExclude_ls00)    )
    {
        if (resUnknown_els00 == pTransformationCmd->trRestore_ls00)
        {
            // Here we have usually no information about table owner and table name as we do not parse the SQL statement
            pMsgObj->ls98PrintMessage(layDataExtract_els98, msgNumExtractedRecordsNoTabName_els98,0, 0);
        }
        else
        {
            char* _ptmp = pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.asCharp();
            if (0 == pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.length())
                _ptmp = pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp();

            pMsgObj->ls98PrintMessage(layDataExtract_els98, msgNumExtractedRecords_els98,
                                      _ptmp,pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),0, 0);
        }
    }

    //*
    //*   extract data records
    //*
    if ( (errOK_els00 == rc) && (false == pTransformationCmd->trExclude_ls00) )
    {
        //*
        //*  run dataextract
        //*

        //*
        //*  case table: extract transformation statements in command file
        //*
        if (pTransformationCmd->trRestore_ls00  != resUnknown_els00         &&
            pDatExtrRec->derCmdFile_ls00        != NULL                     &&
                pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 == NULL &&
            pDatExtrRec->derCmdFile_ls00->vfFileType_ls00 != dtypePipe_ls00 &&
            ptTable_els00 == pDatExtrRec->derPartSpec_ls00.psPartType_ls00  )
        {
            sp77sprintf(pDatExtrRec->derpStartPos_ls00, 256, "%d:%d", start_Data.lnu2GB_Blocks_ls00,
                                                                      start_Data.lnuRemainder_ls00);
            rc = ls27_ExtractTransformation(pMetaDataDef, pDatExtrRec, pTransformationCmd, ErrText);
        }

        //*
        //*  Switch DB date and time format
        //* User may have run the statement 'SET FORMAT <name>'. If the format set is different from
        //* INTERNAL the Loader must switch the format to INTERNAL before getting the data and
        //* switch the format again to the user requested value after getting the data.
        //* The Loader does only process date and time data delivered by the kernel in format INTERNAL.
        //*
        if ((errOK_els00 == rc) && (dtInternal_els00 != pDBInfo->dbiSession_ls00.siFormat_ls00))
        {
            rc = ls15SwitchFormat(pDBInfo, dtInternal_els00, ErrText);
            if (errOK_els00 == rc)
            {
                bFormatSwitched = SAPDB_TRUE;
            }
        }

        //*
        //*  extract data record column values
        //*
        if (errOK_els00 == rc)
            rc = ls27ExtractRowColumns(pDBInfo, pMetaDataDef, pDatExtrRec, ErrText);

        if (errOK_els00 != rc)
        {
            pMsgObj->ls98MsgToString(layDataExtract_els98, ErrText, errErrorAtDataExtract_els98);
        }


        //*
        //*  Switch DB date and time format back to user requested value
        //*
        if (SAPDB_TRUE == bFormatSwitched)
        {
            rc = ls15SwitchFormat(pDBInfo, pDBInfo->dbiSession_ls00.siFormat_ls00, ErrText);
        }


        if ( (false == pTransformationCmd->trInternal_ls00) && (false == pDatExtrRec->derTabEmpty_ls00) )
        {
            if (resUnknown_els00 == pTransformationCmd->trRestore_ls00)
            {
                // Here we have usually no information about table owner and table name as we do not parse the SQL statement
                pMsgObj->ls98PrintMessage(layDataExtract_els98, msgNumExtractedRecordsNoTabName_els98,
                                                                pDatExtrRec->derRecordCount_ls00,
                                                                (pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors -
                                                                 pDatExtrRec->derLRejected_ls00) );
                /*pMsgObj->ls98MsgToString(layDataExtract_els98, ErrText, msgNumExtractedRecordsNoTabName_els98,pDatExtrRec->derRecordCount_ls00,
                                                                (pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors -
                                                                 pDatExtrRec->derLRejected_ls00) );*/
            }
            else
            {
                char* _ptmp = pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp();
                if (0 == pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length()) {
                    _ptmp = pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp();
                }

                pMsgObj->ls98PrintMessage(layDataExtract_els98, msgNumExtractedRecords_els98,
                                                                _ptmp,
                                                                pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                                                pDatExtrRec->derRecordCount_ls00,
                                                                (pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors -
                                                                 pDatExtrRec->derLRejected_ls00) );
            }
        }
        //*
        //*  case db|user : extract transformation statements in command file
        //*
        if (pTransformationCmd->trRestore_ls00  <= resDataUpdt_els00 &&
            pTransformationCmd->trRestore_ls00  != resUnknown_els00  &&
            pDatExtrRec->derCmdFile_ls00  != NULL              &&
            pDatExtrRec->derPartSpec_ls00.psPartType_ls00 > ptTable_els00)
        {
            sp77sprintf(pDatExtrRec->derpStartPos_ls00, 256, "%d:%d", start_Data.lnu2GB_Blocks_ls00,
                                                                      start_Data.lnuRemainder_ls00 );
            rc = ls27_ExtractTransformation(pMetaDataDef, pDatExtrRec, pTransformationCmd, ErrText);
        }
    } // end if ( (errOK_els00 == rc) && (false == pTransformationCmd->trExclude_ls00) )

    //*
    //*  update transformation table and write metadata trailer
    //*
    if ( errOK_els00 == rc &&
         true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 )
    {
        rc = ls27WriteMetaTrailer(pTransformationDef,*pDatExtrRec->derDataFile_ls00, pTransformationCmd, pDatExtrRec, ErrText);

        rc = ls27UpdateTransformationPackage( pMetaDataDef,pTransformationCmd,pDatExtrRec,ErrText);

        if (errOK_els00 != rc )
        {
            pMsgObj->ls98Msg(layDataExtract_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
            rc = errInternal_els98;
        }
    }

    if (false == pTransformationCmd->trInternal_ls00)
    {
        pMsgObj->ls98AppendStatusToString(layDataExtract_els98, ErrText, statNumExtractedRecords_els98,
                                                                         STATIC_CAST(SAPDB_Int8, pDatExtrRec->derRecordCount_ls00),
                                                                         STATIC_CAST(SAPDB_Int8, (pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors -
                                                                                                   pDatExtrRec->derLRejected_ls00)));
    }


    //*
    //*   free dataextract record structure
    //*
    ls27_FreeDatExtrRec(pDBInfo,pTransformationCmd,pDatExtrRec );

    return rc;

} // ls27ExportTableRows


/*
  -----------------------------------------------------------------------------
  function:     ls27ExtractRowColumns
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27ExtractRowColumns(tls00_DBInfo        *pDBInfo,
                      MetaDataDef         *&pMetaDataDef,
                      tls00_DextractRec   *pDatExtrRec,
                      tsp00_Addr          ErrText)
{

    ROUTINE_DBG_MEO00 ("ls27ExtractRowColumns");

    tsp00_Longint rc = errOK_els00;

    TransformationDef*        pTransformationDef  = pMetaDataDef->getTransformationModel();
    tls00_TransformationCmd*  pTransformationCmd  = pTransformationDef->getTransformationCmd();
    //*
    //*  define the max count of rows to fetch in a single packet
    //*
    // In case that a table contains longs we limit the count of rows to fetch to the
    // half of the maximum
    if (0 != pDatExtrRec->derLongColCnt_ls00)
    {
        pDatExtrRec->derMaxRowsPPckt_ls00 = pDatExtrRec->derMaxRowsPPckt_ls00/2;
        if (0 == pDatExtrRec->derMaxRowsPPckt_ls00)
        {
            pDatExtrRec->derMaxRowsPPckt_ls00 = 1;
        }
    }
    //*
    //*    initialize file structure and write headers
    //*
    if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00)
    {
        ls30VFWriteHeader(*pDatExtrRec->derDataFile_ls00,
                          pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoBytesPerLine_ls00,
                          pTransformationCmd->trFullUserInput_ls00,
                          ErrText);
    }
    //*
    //*   Allocate conversion buffer
    //* This buffer is used (among other things) for conversion of LONG values between
    //* different encoding types and (additionally) hexadecimal representation;
    //* the maximum amount of data that the kernel can send is
    //*  the size of a communication packet (kernel parameter: _PACKET_SIZE; range (in Bytes): 16384 <= _PACKET_SIZE <= 131072)
    //*   +
    //*  the minimum reply (packet) size that is appended to a normal communication
    //*  packet (kernel parameter: _MINREPLY_SIZE; range (in Bytes): 400 <= _MINREPLY_SIZE <= 4096)
    //*
    //* In the worst case (assuming a packet size of 32 kB) we
    //*  1. get the LONG data as ASCII            --> ~  36 kB
    //*  2. have to convert it to UCS2            --> ~  72 kB)
    //*  3. have to convert to hex representation --> ~ 144 kB
    //*  4. have to convert to UCS2 again         --> ~ 288 kB
    //*
    //* Conversion buffer should be of size 8*(_PACKET_SIZE + max(_MINREPLY_SIZE))
    //*
    tls00_Buffer2 *pConvertBuffer = new tls00_Buffer2;

    pConvertBuffer->lBufLen_ls00 = 8*(pDBInfo->dbiPktSize_ls00 + 4096);
    pConvertBuffer->pBufPtr_ls00 = new _TCHAR[pConvertBuffer->lBufLen_ls00];
    pConvertBuffer->lBufPos_ls00 = 0;

    // define max count of rows to send before sending commit
    tsp1_packet   *SqlPacket = NULL;
    tsp1_part     *SqlPart   = NULL;
    tsp00_Longint lCommitCount         = 0;
    tsp00_Int4    lRowCntBeforeCommit  = (0 == pDBInfo->dbiSession_ls00.siTASize_ls00) ?
                                               pDatExtrRec->derMaxRowsPPckt_ls00 :
                                               pDBInfo->dbiSession_ls00.siTASize_ls00;

    tsp00_Int4    RowsFetched  = pDatExtrRec->derMaxRowsPPckt_ls00;
    tsp00_Longint RowPos       = pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 ;
    if ( (UNDEFINED_LS00 == RowPos) || (0 == RowPos) ) {
        RowPos = 1;
    }

    tsp00_Longint RowsToTransform = UNDEFINED_LS00;
    if ( UNDEFINED_LS00  != pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00 &&
         UNDEFINED_LS00  != pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 )
    {
        RowsToTransform  = pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feEnd_ls00.lnuRemainder_ls00 -
                            pTransformationCmd->trDataFile_ls00.fsExtract_ls00.feStart_ls00.lnuRemainder_ls00 + 1;
    }

    char* pszTmpStr;

    //*
    //*   main loop to extract data
    //*
    while (  RowsFetched     == pDatExtrRec->derMaxRowsPPckt_ls00 &&
		   ( RowsToTransform == UNDEFINED_LS00 || RowsToTransform >= pDatExtrRec->derRecordCount_ls00 ))
    {
        rc = ls12FetchPos(*pDBInfo, pDatExtrRec->derRTableName_ls00, RowsFetched, RowPos, true, ErrText);
        if (errOK_els00 != rc)
        {
            if (dbRowNotFound_els00 == rc)
            {
                rc = errOK_els00;
            }
            break;
        }

        SqlPacket = pDBInfo->dbiPktRcvList_ls00[0];

        bool Res = ls04GetPart(SqlPacket, sp1pk_data, SqlPart);
        if (false == Res)
        {
            rc = errUnknownPart_els98;
            break;
        }
        // partlength durch rowsFetched MUSS derRecordLen_ls00 ergeben, das koennte hier geprueft werden
        pDatExtrRec->derDataSource_ls00.dasInputBuffer.lBufLen_ls00 = ls04GetPartLength(SqlPart);
        ls04BufPtrFromPart(SqlPart, 0, pszTmpStr);
        pDatExtrRec->derDataSource_ls00.dasInputBuffer.pBufPtr_ls00 = pszTmpStr;

		if ( RowsToTransform != UNDEFINED_LS00 &&
			 RowsToTransform  < RowsFetched + pDatExtrRec->derRecordCount_ls00 )
             RowsFetched = RowsToTransform  - pDatExtrRec->derRecordCount_ls00;

        rc = ls27_TransformTableRows(pDBInfo, RowsFetched, pDatExtrRec, pTransformationCmd, pConvertBuffer, ErrText);
        if (errOK_els00 != rc)
        {
            break;
        }

        // commit in any case
        tsp00_Longint tmpCnt = pDatExtrRec->derExtractedRows_ls00 / lRowCntBeforeCommit;

        if ( tmpCnt > lCommitCount )
        {
            lCommitCount = tmpCnt;

            rc = ls27UpdateTransformationPackage(pMetaDataDef, pTransformationCmd, pDatExtrRec, ErrText);
            if (errOK_els00 != rc )
            {
                LoaderMessages::Instance()->ls98Msg(layDataExtract_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
                rc = errInternal_els98;
                break;
            }
            TRACE_PRNF_MLS99(("ls27ExtractRowColumns", "Commit after %d rows\n", pDatExtrRec->derRecordCount_ls00) );
        }
        RowPos += RowsFetched;

        // check for cancel request
        rc = ls01CheckForCancel(pDBInfo->dbiSession_ls00.sipCancelByte_ls00);
        if (errOK_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layDataExtract_els98, ErrText, errCancelRequest_els98);
            break;
        }
    }// while (RowsFetched == pDatExtrRec->derMaxRowsPPckt_ls00)

    // commit in any case and do not pay any attention to the return value
    // - we have to release the table lock (in case of an appropriate isolevel)
    char       pszErrTextCOMMIT[MAX_REPLY_LENGTH_LS00];
    pszErrTextCOMMIT[0] = 0;

    tsp00_Int4 rcCOMMIT = ls15RSCommit(pDBInfo, pszErrTextCOMMIT);
    if ( (errOK_els00 != rcCOMMIT) && (errOK_els00 == rc) )
    {
        rc = rcCOMMIT;

        tsp00_Uint4 _len = strlen(pszErrTextCOMMIT);
        memcpy(ErrText, pszErrTextCOMMIT, _len);
        ErrText[_len] = 0;
    }
    TRACE_PRNF_MLS99(("ls27ExtractRowColumns", "Commit after %d rows\n", pDatExtrRec->derRecordCount_ls00) );

    if (errOK_els00 == rc)
    {
        pDatExtrRec->derLastCommLine_ls00  = pDatExtrRec->derExtractedRows_ls00;
        pDatExtrRec->derTabExtracted_ls00  = true;
        
        rc = ls27UpdateTransformationPackage( pMetaDataDef,pTransformationCmd,pDatExtrRec,ErrText);
        if (errOK_els00 != rc ) {
            LoaderMessages::Instance()->ls98Msg(layDataExtract_els98, ErrText, errInternal_els98, __FILE__, __LINE__, rc );
            rc = errInternal_els98;
        }
    }

    //*
    //*  delete conversion buffer
    //*
    if (NULL != pConvertBuffer)
    {
        if (NULL != pConvertBuffer->pBufPtr_ls00)
        {
            delete [] STATIC_CAST(_TCHAR*, pConvertBuffer->pBufPtr_ls00);
            pConvertBuffer->pBufPtr_ls00 = NULL;
        }

        delete pConvertBuffer;
        pConvertBuffer = NULL;
    }

    return STATIC_CAST(tsp00_Int4, rc);
}
// ls27ExtractRowColumns()

/*
  -----------------------------------------------------------------------------
  function:     ls27_ExtractTransformation
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27_ExtractTransformation(MetaDataDef              *&pMetaDataDef,
                           tls00_DextractRec         *pDatExtrRec,
                           tls00_TransformationCmd   *pTransformationCmd,
                           tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_ExtractTransformation");

    LoaderMessages *pMsgObj  = LoaderMessages::Instance();
    tsp00_Int4       rc       = errOK_els00;
    tsp00_Int4       rcDDL    = errOK_els00;

    tls00_DBInfo    * pDBInfo = pMetaDataDef->getConnection();

    //*
    //*     'create table' - statement
    //*
    //* to generate the 'create table' and the 'create index' stmt we use an odbc-connection;
    //* therefore we have to release the already existing connection first and to reconnect
    //* using odbc; after work is done we connect the other way around
    //*
    Tools_DynamicUTF8String  TabSchema;
    Tools_DynamicUTF8String  TabName;

    if (pTransformationCmd->trRestore_ls00 <= resTableUpdt_els00     &&
        ptTable_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
    {
        if (0 != pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length ())
        {
            TabSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                              pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp()));
        }

        TabName.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                        pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp()));

        if ( errOK_els00 == pMetaDataDef->initResultSets( pTransformationCmd, pDBInfo, ErrText) )
        {
              // reuse established file connection
              tls00_FileInfo CmdFileInfo;
              CmdFileInfo.vFile    = *pDatExtrRec->derCmdFile_ls00;
              if ( NULL != pTransformationCmd->trCmdFile_ls00.fsFileName_ls00 )
              {
                   CmdFileInfo.fileSpec =  pTransformationCmd->trCmdFile_ls00;
                   pMetaDataDef->setMedium(CmdFileInfo);
                   rcDDL = pMetaDataDef->getRelationalModel()->TransformTable(TabSchema, TabName);
              }
              if ( NULL != pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 )
              {
                    CmdFileInfo.fileSpec =  pTransformationCmd->trPackageFile_ls00;
                    pMetaDataDef->setMedium(CmdFileInfo);
              }
        }
        else
        {
             pMsgObj->ls98PrintMessage(layDataExtract_els98, err_DE_GenerateCreateCmd_els98,
                                                            pMetaDataDef->getConnection()->dbiOdbcConnection.GetErr().GetErrMsg());

             // This is an error but not heavy enough to stop executing; so we simply
             // print a msg to the log file
             rcDDL = err_DE_GenerateCreateCmd_els98;
        }
        *pDatExtrRec->derCmdFile_ls00 = pMetaDataDef->getMedium().vFile;
    }// end if (pTransformationCmd->trRestore_ls00 < resDataUpdt_els00)

    //*
    //*    do we have to write as first the separator line?
    //*
    //*    in case the file is not created but contains already commands we need to write at first a separator line
    //*
    char szSepLine[] = CMD_SEPARATOR_TOKEN_LS00;
    rc = ls30VFWriteLn(*pDatExtrRec->derCmdFile_ls00, szSepLine, CMD_SEPARATOR_TOKEN_LEN_LS00, ErrText);

    //*
    //*    dataload command
    //*
    tls00_MultColSpec* pMultColumns = &pTransformationCmd->trColSpec_ls00;  // Spaltenangaben zur datendatei

    tls00_CodeType      ffoCodeType_ls00;       
    tls00_DateTime      ffoDateType_ls00;
    _TCHAR              ffoDateMask_ls00[128];           
    tls00_DateTime      ffoTimeType_ls00; 
    _TCHAR              ffoTimeMask_ls00[128];      
    tls00_DateTime      ffoTimeStampType_ls00; 
    _TCHAR              ffoTimeStampMask_ls00[128];         
    char                ffoThsdChar_ls00;
    char                ffoDecPoint_ls00;
    char                _ffoSeperator[2] = {0,0};     
    char                _ffoDelimiter[2] = {0,0};     
    tsp00_Int2          ffoTValLen_ls00;        
    _TCHAR              ffoTrueVal_ls00[11];
    tsp00_Int2          ffoFValLen_ls00;        
    _TCHAR              ffoFalseVal_ls00[11];
    _TCHAR              ffoNullSpec_ls00[21];
    tsp00_Int2          ffoCSetLen_ls00;
    tsp00_Int2          ffoNSpecLen_ls00;
    tsp00_SwapKind      ffoIntSpec_ls00; 
    bool                ffoBinary_ls00;         
    bool                ffoFormatted_ls00;
    bool                ffoCompress_ls00;
    bool                ffoPage_ls00;     
    bool                ffoCompact_ls00;  
    bool                ffoDdl_ls00;
    bool                ffoDtl_ls00;
    bool                ffoXml_ls00;

    if ( pTransformationCmd->trDataFile_ls00.fsFileName_ls00 != NULL )
    {
        tls00_FileFormat *pDatFileFormat= &pTransformationCmd->trDataFile_ls00.fsFormat_ls00;

        ffoCodeType_ls00                = pDatFileFormat->ffoCodeType_ls00; 
        ffoDateType_ls00                = pDatFileFormat->ffoDateType_ls00; 
        SAPDB_memcpy(ffoDateMask_ls00,pDatFileFormat->ffoDateMask_ls00,sizeof(pDatFileFormat->ffoDateMask_ls00[128])); 
        ffoTimeType_ls00                = pDatFileFormat->ffoTimeType_ls00;
        SAPDB_memcpy(ffoTimeMask_ls00,pDatFileFormat->ffoTimeMask_ls00,sizeof(pDatFileFormat->ffoTimeMask_ls00[128]));
        ffoTimeStampType_ls00           = pDatFileFormat->ffoTimeStampType_ls00;
        SAPDB_memcpy(ffoTimeStampMask_ls00,pDatFileFormat->ffoTimeStampMask_ls00,sizeof(pDatFileFormat->ffoTimeStampMask_ls00[128]));    
        ffoThsdChar_ls00                = pDatFileFormat->ffoThsdChar_ls00;
        ffoDecPoint_ls00                = pDatFileFormat->ffoDecPoint_ls00;
        
        _ffoSeperator[0]                = pDatFileFormat->ffoSeperat_ls00[0];
        _ffoDelimiter[0]                = pDatFileFormat->ffoDelimit_ls00[0];
        
        ffoTValLen_ls00                 = pDatFileFormat->ffoTValLen_ls00;    
        strcpy(ffoTrueVal_ls00,pDatFileFormat->ffoTrueVal_ls00);
        ffoFValLen_ls00                 = pDatFileFormat->ffoFValLen_ls00;
        strcpy(ffoFalseVal_ls00,pDatFileFormat->ffoFalseVal_ls00);
        strcpy(ffoNullSpec_ls00,pDatFileFormat->ffoNullSpec_ls00);
        ffoCSetLen_ls00                 = pDatFileFormat->ffoCSetLen_ls00;
        ffoNSpecLen_ls00                = pDatFileFormat->ffoNSpecLen_ls00;
        ffoIntSpec_ls00                 = pDatFileFormat->ffoIntSpec_ls00;
        ffoBinary_ls00                  = pDatFileFormat->ffoBinary_ls00;         
        ffoFormatted_ls00               = pDatFileFormat->ffoFormatted_ls00;
        ffoCompress_ls00                = pDatFileFormat->ffoCompress_ls00;
        ffoPage_ls00                    = pDatFileFormat->ffoPage_ls00;     
        ffoCompact_ls00                 = pDatFileFormat->ffoCompact_ls00;  
        ffoDdl_ls00                     = pDatFileFormat->ffoDdl_ls00;
        ffoDtl_ls00                     = pDatFileFormat->ffoDtl_ls00;
        ffoXml_ls00                     = pDatFileFormat->ffoXml_ls00;
    }
    else
    {
        ffoCodeType_ls00                = pDBInfo->dbiOI_Encoding_ls00;
        ffoDateType_ls00                = pDBInfo->dbiSession_ls00.siDateType_ls00;
        //ffoDateMask_ls00           = 
        //ffoSpecialDateMask_ls00       = 
        ffoTimeType_ls00                = pDBInfo->dbiSession_ls00.siTimeType_ls00;
        //ffoTimeMask_ls00          = 
        //ffoSpecialTimeMask_ls00       = 
        ffoTimeStampType_ls00           = pDBInfo->dbiSession_ls00.siStampType_ls00;
        //ffoTimeStampMask_ls00      = 
        ffoThsdChar_ls00                = pDBInfo->dbiSession_ls00.siThsdChar_ls00;
        ffoDecPoint_ls00                = pDBInfo->dbiSession_ls00.siDecPoint_ls00;

        _ffoSeperator[0]                = pDBInfo->dbiSession_ls00.siSeperat_ls00[0];
        _ffoDelimiter[0]                = pDBInfo->dbiSession_ls00.siDelimit_ls00[0];

        //ffoTValLen_ls00               = 
        strcpy(ffoTrueVal_ls00,pDBInfo->dbiSession_ls00.siTrueVal_ls00);
        //foFValLen_ls00                = 
        strcpy(ffoFalseVal_ls00,pDBInfo->dbiSession_ls00.siFalseVal_ls00);
        strcpy(ffoNullSpec_ls00,pDBInfo->dbiSession_ls00.siNullSpec_ls00);
        //ffoCSetLen_ls00               = 
        //ffoNSpecLen_ls00              = 
        //ffoIntSpec_ls00               = 
        //ffoBinary_ls00                = 
        //ffoFormatted_ls00             = 
        //ffoCompress_ls00              = 
        //ffoPage_ls00                  = 
        //ffoCompact_ls00               = 
        //ffoDdl_ls00                   = 
        //ffoXml_ls00                   = 
    }

    tsp00_Int4 lPos       = 0;
    tsp00_Int4 lCmdLength = ((pMultColumns->mcsCount_ls00 + 2) * (KNL_IDENTIFIER_SIZE + 50)) + 256;

    tsp00_Addr pszLoadCmd = new _TCHAR[lCmdLength];
    pszLoadCmd[0] = '\0';

    // old syntax
    if ( pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 == NULL )
    {
        //*    fastload,dataload,dataupdate?
        if ( ( (pTransformationCmd->trRestore_ls00 == resFastloadNoDDL_els00) ||
            (pTransformationCmd->trRestore_ls00 == resFastload_els00 )      ) &&
            (0 != pDatExtrRec->derLongColCnt_ls00) )
        {
            lPos = sp77sprintf(pszLoadCmd, lCmdLength, pszLoadCmdBeginString[STATIC_CAST(int, resDataload_els00)]);
        }
        else
        {
            lPos = sp77sprintf(pszLoadCmd, lCmdLength, pszLoadCmdBeginString[STATIC_CAST(int, pTransformationCmd->trRestore_ls00)]);
        }
        //*
        //*     table name
        //*
        lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, "\"%s\""NEW_LINE_LS00"", TabName.CharPtr() );
        //*     column description
        tsp00_Int4 i = 0;

        tls00_ParamInfo        *pParamInfo = NULL;
        tls00_Column           *pMyColumn  = NULL;
        tls00_ColumnData*       pColData   = NULL;
        tsp00_Addr              pszColName = NULL;
        tls00_TableDescription *pTInfo     = &pDatExtrRec->derTableInfo_ls00;

        for (i; i < pMultColumns->mcsCount_ls00; ++i)
        {
            pszColName = (*pTInfo->tdColName_ls00[i]).asCharp();
            pParamInfo = pTInfo->tdParamArray_ls00[i];
            pMyColumn  = pMultColumns->mcsColumn_ls00[i];

            if (true == pMyColumn->colIsLit_ls00)       // Extrabehandlung fuer literale im kommando
            {
                continue;
            }

            if (true == ffoFormatted_ls00)
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("\"%s\" %d-%d CHAR"),
                                                                        pszColName,
                                                                        pMyColumn->colFPos_ls00.fpoStart_ls00,
                                                                        pMyColumn->colFPos_ls00.fpoEnd_ls00);
            }
            else
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("\"%s\" %d CHAR"),
                                                                        pszColName,
                                                                        pMyColumn->colFPos_ls00.fpoStart_ls00);
            }

            pColData = &pDatExtrRec->pderColumnData_ls00[i];
            if ( (1 == szMap_BlobDataType_ls00[pColData->cdSQLDataType_ls00]) &&
                 (0 == szMap_LongDataType_ls00[pColData->cdSQLDataType_ls00])  )
            {
                if (false == ffoBinary_ls00)
                {
                    lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" HEX"));
                }
            }

            if ( (false == pParamInfo->piMode_ls00.includes(sp1ot_mandatory)) ||
                 (true  == pParamInfo->piMode_ls00.includes(sp1ot_default)) )
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" DEFAULT NULL"NEW_LINE_LS00""));
            }
            else
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(NEW_LINE_LS00));
            }
        }

        //*     infile-name
        lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("INSTREAM %s '%s'"NEW_LINE_LS00""),
                                                                pszDeviceTypeName_ls00[pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                pTransformationCmd->trDataFile_ls00.fsFileName_ls00);

        //*     infile format
        if (true == ffoCompact_ls00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" RECORDS "));
        }
        else if (true == ffoFormatted_ls00)
        {
            if (false == ffoBinary_ls00)
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" FORMATTED "));
            }
            else
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" FORMATTED BINARY"NEW_LINE_LS00""));
            }
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" COMPRESSED "));
        }

        //*     encoding (not if FORMATTED BINARY)
        if ( ( false == ffoFormatted_ls00) ||
             ( (true == ffoFormatted_ls00)  && (false == ffoBinary_ls00) ) )
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("%s"NEW_LINE_LS00""), pszEncoding[ffoCodeType_ls00]);
        }


        //*   decimal symbol
        if (NO_THSD_GROUPS_LS00 == ffoThsdChar_ls00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DECIMAL '//%c/'"NEW_LINE_LS00""),
                                                                    ffoDecPoint_ls00);
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DECIMAL '/%c/%c/'"NEW_LINE_LS00""),
                                                                    ffoThsdChar_ls00,
                                                                    ffoDecPoint_ls00);
        }
        if ( true == ffoCompress_ls00 )
        {
             if ( ('\'' == _ffoSeperator[0]) || (SLASH_LS00 == _ffoSeperator[0]) )
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("SEPARATOR '\\%c'"NEW_LINE_LS00""),
                                                                         _ffoSeperator[0]);
             }
             else if ('\t' == _ffoSeperator[0])
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("SEPARATOR '\\t'"NEW_LINE_LS00""));
             }
             else
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("SEPARATOR '%c'"NEW_LINE_LS00""),
                                                                         _ffoSeperator[0]);
             }
        
             if ( ('\'' == _ffoDelimiter[0]) || (SLASH_LS00 == _ffoDelimiter[0]) )
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DELIMITER '\\%c'"NEW_LINE_LS00""),
                                                                         _ffoDelimiter[0]);
             }
             else if ('\t' == _ffoDelimiter[0])
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DELIMITER '\\t'"NEW_LINE_LS00""));
             }
             else
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DELIMITER '%c'"NEW_LINE_LS00""),
                                                                         _ffoDelimiter[0]);
             }
        }

        //*     DATE format
        if (ffoDateType_ls00 == dtFreeMask_els00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DATE '%s'"NEW_LINE_LS00""),
                                                                    ffoDateMask_ls00);
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                _T("DATE %s"NEW_LINE_LS00""),
                                                pszDateTimeType[STATIC_CAST(int, ffoDateType_ls00)]);
        }


        //*     TIME format
        if (ffoTimeType_ls00 == dtFreeMask_els00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("TIME '%s'"NEW_LINE_LS00""),
                                                                    ffoTimeMask_ls00);
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                _T("TIME %s"NEW_LINE_LS00""),
                                                pszDateTimeType[STATIC_CAST(int, ffoTimeType_ls00)]);
        }


        //*     TIMESTAMP format
        if (ffoTimeStampType_ls00 == dtFreeMask_els00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("TIMESTAMP '%s'"NEW_LINE_LS00""),
                                                                    ffoTimeStampMask_ls00);
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                _T("TIMESTAMP %s"NEW_LINE_LS00""),
                                                pszDateTimeType[STATIC_CAST(int, ffoTimeStampType_ls00)]);
        }


        //*     NULL value representation
        lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("NULL '%s'"NEW_LINE_LS00""), ffoNullSpec_ls00);

        //*     boolean value representation
        lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("BOOLEAN '%s/%s'"NEW_LINE_LS00""),
                                                                ffoTrueVal_ls00,
                                                                ffoFalseVal_ls00);

        //*     startpos and recordcount
        if ( pDatExtrRec->derPartSpec_ls00.psPartType_ls00 > ptTable_els00)
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" START %s %d " ), pDatExtrRec->derpStartPos_ls00, pDatExtrRec->derRecordCount_ls00 );

        //*     long  columns?

        if ( pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 == false )
        {
            if (0 != pDatExtrRec->derLongColCnt_ls00)
            {
                for (i = 0; i < pMultColumns->mcsCount_ls00; ++i)
                {
                    pMyColumn = pMultColumns->mcsColumn_ls00[i];
                    if (1 == szMap_LongDataType_ls00[pDatExtrRec->pderColumnData_ls00[i].cdSQLDataType_ls00])
                    {
                        if (false == pMyColumn->colLongInfo.loibFileNumbered_ls00)
                        {   // long file not numbered
                            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                                _T("LONGFILE \"%s\" '%s' %s"NEW_LINE_LS00""),
                                                                pMyColumn->colName_ls00.asCharp(),
                                                                pMyColumn->colLongInfo.loiFileName_ls00,
                                                                pszEncoding[pMyColumn->colLongInfo.loiFileEncoding_ls00]);
                        }
                    }
                }
            }// if (0 != pDatExtrRec->derLongColCnt_ls00)
        }// if( pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 == false )

    }//if ( pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 == NULL )


    // new syntax
    if ( pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 != NULL )
    {
        //*    fastload,dataload,dataupdate?
        lPos = sp77sprintf(pszLoadCmd, lCmdLength, "IMPORT TABLE ");

        //*
        //*     table name
        //*
        lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, "\"%s\"", TabName.CharPtr() );
        if (  pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoUsage_ls00.uFastMode_ls00 == false) 
             lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" REJECT DUPLICATES "NEW_LINE_LS00""));
        else
             lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T( NEW_LINE_LS00""));

        if ( pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 != NULL )
        {
             //*     catalogstream-name
             if (NULL == getenv ("maxdbloaderpackagename"))
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("CATALOG INSTREAM %s '%s' "),
                                                                           pszDeviceTypeName_ls00[pTransformationCmd->trCatalogFile_ls00.fsDeviceType_ls00],
                                                                           pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00);
             }
             if (NULL != getenv ("maxdbloaderpackagename"))
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("CATALOG INSTREAM %s '%s.CATALOG' "),
                                                                           pszDeviceTypeName_ls00[pTransformationCmd->trCatalogFile_ls00.fsDeviceType_ls00],
                                                                           pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());
             }
             //*     catalogstream format
             if (true == pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00)
             {
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" DDL"NEW_LINE_LS00""));
             }
        }
        //*     datastream-name
        if (NULL == getenv ("maxdbloaderpackagename"))
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DATA    INSTREAM %s '%s' "),
                                                                      pszDeviceTypeName_ls00[pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                      pTransformationCmd->trDataFile_ls00.fsFileName_ls00);
        }
        if (NULL != getenv ("maxdbloaderpackagename"))
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DATA    INSTREAM %s '%s.DATA'    "),
                                                                      pszDeviceTypeName_ls00[pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                      pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());
        }

        //*     datastream format
        if (true == ffoCompact_ls00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" RECORDS "));
        }
        else if (true == ffoFormatted_ls00)
        {
            if (false == ffoBinary_ls00)
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" FWV "));
            }
            else
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" FWV BINARY"NEW_LINE_LS00""));
            }
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" CSV "));
        }

        //*     encoding (not if FORMATTED BINARY)
        if ( ( false == ffoFormatted_ls00) ||
            ( (true  == ffoFormatted_ls00)  && (false == ffoBinary_ls00) ) )
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("%s"NEW_LINE_LS00""), pszEncoding[ffoCodeType_ls00]);
        }

        if ( true == ffoCompress_ls00 )
        {
            if ( ('\'' == _ffoSeperator[0]) || (SLASH_LS00 == _ffoSeperator[0]) )
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("SEPARATOR '\\%c'"NEW_LINE_LS00""),
                                                                        _ffoSeperator[0]);
            }
            else if ('\t' == _ffoSeperator[0])
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("SEPARATOR '\\t'"NEW_LINE_LS00""));
            }
            else
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("SEPARATOR '%c'"NEW_LINE_LS00""),
                                                                        _ffoSeperator[0]);
            }

            if ( ('\'' == _ffoDelimiter[0]) || (SLASH_LS00 == _ffoDelimiter[0]) )
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DELIMITER '\\%c'"NEW_LINE_LS00""),
                                                                        _ffoDelimiter[0]);
            }
            else if ('\t' == _ffoDelimiter[0])
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DELIMITER '\\t'"NEW_LINE_LS00""));
            }
            else
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DELIMITER '%c'"NEW_LINE_LS00""),
                                                                        _ffoDelimiter[0]);
            }
        }

        //*     NULL value representation
        lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("NULL '%s'"NEW_LINE_LS00""), ffoNullSpec_ls00);

		//*     decimal symbol
        if (NO_THSD_GROUPS_LS00 == ffoThsdChar_ls00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DECIMAL '//%c/'"NEW_LINE_LS00""),
                                                                    ffoDecPoint_ls00);
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DECIMAL '/%c/%c/'"NEW_LINE_LS00""),
                                                                    ffoThsdChar_ls00,
                                                                    ffoDecPoint_ls00);
        }


        //*     boolean value representation
        lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("BOOLEAN '%s/%s'"NEW_LINE_LS00""),
                                                                ffoTrueVal_ls00,
                                                                ffoFalseVal_ls00);

	    //*     TIMESTAMP format
        if (ffoTimeStampType_ls00 == dtFreeMask_els00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("TIMESTAMP '%s'"NEW_LINE_LS00""),
                                                                    ffoTimeStampMask_ls00);
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                _T("TIMESTAMP %s"NEW_LINE_LS00""),
                                                pszDateTimeType[STATIC_CAST(int, ffoTimeStampType_ls00)]);
        }

        //*     DATE format
        if (ffoDateType_ls00 == dtFreeMask_els00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("DATE '%s'"NEW_LINE_LS00""),
                                                                    ffoDateMask_ls00);
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                _T("DATE %s"NEW_LINE_LS00""),
                                                pszDateTimeType[STATIC_CAST(int, ffoDateType_ls00)]);
        }


        //*     TIME format
        if (ffoTimeType_ls00 == dtFreeMask_els00)
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("TIME '%s'"NEW_LINE_LS00""),
                                                                    ffoTimeMask_ls00);
        }
        else
        {
            lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                _T("TIME %s"NEW_LINE_LS00""),
                                                pszDateTimeType[STATIC_CAST(int, ffoTimeType_ls00)]);
        }

        //*     startpos and recordcount
        if ( pDatExtrRec->derPartSpec_ls00.psPartType_ls00 > ptTable_els00)
             lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" START %s %d " ), pDatExtrRec->derpStartPos_ls00, pDatExtrRec->derRecordCount_ls00 );

        //*     column description
        tsp00_Int4 i = 0;

        tls00_ParamInfo        *pParamInfo = NULL;
        tls00_Column           *pMyColumn  = NULL;
        tls00_ColumnData*       pColData   = NULL;
        tsp00_Addr              pszColName = NULL;
        tls00_TableDescription *pTInfo     = &pDatExtrRec->derTableInfo_ls00;

        for (i; i < pMultColumns->mcsCount_ls00; ++i)
        {
            pszColName = (*pTInfo->tdColName_ls00[i]).asCharp();
            pParamInfo = pTInfo->tdParamArray_ls00[i];
            pMyColumn  = pMultColumns->mcsColumn_ls00[i];

            if (true == pMyColumn->colIsLit_ls00)       // Extrabehandlung fuer literale im kommando
            {
                continue;
            }

            if (true == ffoFormatted_ls00)
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("\"%s\" %d-%d CHAR"),
                                                                        pszColName,
                                                                        pMyColumn->colFPos_ls00.fpoStart_ls00,
                                                                        pMyColumn->colFPos_ls00.fpoEnd_ls00);
            }
            else
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T("\"%s\" %d CHAR"),
                                                                        pszColName,
                                                                        pMyColumn->colFPos_ls00.fpoStart_ls00);
            }

            pColData = &pDatExtrRec->pderColumnData_ls00[i];
            if ( (1 == szMap_BlobDataType_ls00[pColData->cdSQLDataType_ls00]) &&
                (0 == szMap_LongDataType_ls00[pColData->cdSQLDataType_ls00])  )
            {
                if (false == ffoBinary_ls00)
                {
                    lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" HEX"));
                }
            }

            if ( (false == pParamInfo->piMode_ls00.includes(sp1ot_mandatory)) ||
                (true == pParamInfo->piMode_ls00.includes(sp1ot_default)) )
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(" DEFAULT NULL "NEW_LINE_LS00""));
            }
            else
            {
                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos, _T(NEW_LINE_LS00));
            }
        }
        //*     long  columns?
        if ( pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 == false )
        {
            if (0 != pDatExtrRec->derLongColCnt_ls00)
            {
                for (i = 0; i < pMultColumns->mcsCount_ls00; ++i)
                {
                    pMyColumn = pMultColumns->mcsColumn_ls00[i];
                    if (1 == szMap_LongDataType_ls00[pDatExtrRec->pderColumnData_ls00[i].cdSQLDataType_ls00])
                    {
                        if (false == pMyColumn->colLongInfo.loibFileNumbered_ls00)
                        {  
                            // long file not numbered
                            if (NULL == getenv ("maxdbloaderpackagename"))
                            {
                                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                                    _T(" LOB  INSTREAM FILE \"%s\" '%s' %s "NEW_LINE_LS00""),
                                                                    pMyColumn->colName_ls00.asCharp(),
                                                                    pMyColumn->colLongInfo.loiFileName_ls00,
                                                                    pszEncoding[pMyColumn->colLongInfo.loiFileEncoding_ls00]);
                            }
                            if (NULL != getenv ("maxdbloaderpackagename"))
                            {
                                lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                                    _T(" LOB  INSTREAM FILE \"%s\" '%s_%s.DATA' %s "NEW_LINE_LS00""),
                                                                    pMyColumn->colName_ls00.asCharp(),
                                                                    pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp(),
                                                                    pMyColumn->colName_ls00.asCharp(),
                                                                    pszEncoding[pMyColumn->colLongInfo.loiFileEncoding_ls00]);
                            }
                        }
                    }
                }
            }// if (0 != pDatExtrRec->derLongColCnt_ls00)
        }// if( pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 == false )

        if ( pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoUsage_ls00.uFastMode_ls00 == true )
        {
            _TCHAR usagevalue[256];
            usagevalue[0] = 0;
            _itot(pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoUsage_ls00.uUsageValue_ls00,usagevalue,10);
            if  ( pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoUsage_ls00.uUsingRows_ls00 == true )
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                                    _T(" PAGE WITH %s ROWS USAGE "NEW_LINE_LS00""),
                                                                     usagevalue );
            if ( pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoUsage_ls00.uUsingRows_ls00 == false )
                 lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                                    _T(" PAGE WITH %s%% USAGE "NEW_LINE_LS00""),
                                                                    usagevalue);
        }
        else
        {
             lPos += sp77sprintf(pszLoadCmd + lPos, lCmdLength - lPos,
                                                                    _T(" RECORDS USAGE"NEW_LINE_LS00""));
        }
    }//if ( pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 != NULL )

    // Remove last carriage return-line feed
    lPos -= 1;
    while (iscntrl(pszLoadCmd[lPos]))
    {
        pszLoadCmd[lPos] = 0;
        --lPos;
    }
    lPos += 1;

    rc = ls30VFWriteLn(*pDatExtrRec->derCmdFile_ls00, pszLoadCmd, lPos, ErrText);

    //*
    //*    'create index' statements
    //*
    if ( pTransformationCmd->trRestore_ls00 <= resDataUpdt_els00     &&
         ptTable_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
    {
        if (errOK_els00 == rcDDL)
            pMetaDataDef->getRelationalModel()->TransformIndexes(TabSchema, TabName);
    }

    if ( pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 != NULL &&
         pTransformationCmd->trPackageFile_ls00.fsAppend_ls00   == false     ) 
    {
         rc = ls30VFWriteLn(*pDatExtrRec->derCmdFile_ls00, szSepLine, CMD_SEPARATOR_TOKEN_LEN_LS00, ErrText);
    }

    if (NULL != pszLoadCmd)
    {
        delete [] pszLoadCmd;
        pszLoadCmd = NULL;
    }

    if ( (errOK_els00 == rc) && (errOK_els00 != rcDDL) )
    {
        pMsgObj->ls98MsgToString(layDataExtract_els98, ErrText, errErrorAtDataExtract_els98);
        rc = errErrorAtDataExtract_els98;
    }

    return rc;
}
// ls27_ExtractTransformation()

/*
  -----------------------------------------------------------------------------
  function:     ls27WriteMetaHeader
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27WriteMetaHeader(tls00_DBInfo      *pDBInfo,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_DextractRec *pDatExtrRec,
                    tsp00_Addr         ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27WriteMetaHeader");

    tsp00_Int4 rc = errOK_els00;
    // define version
    tsp00_Versionc pszVersion;
    sp100_GetVersionString(COMP_NAME_REPL_MAN_SP100, s100buildnumber, pszVersion);


    //  version (contains some id, too)|swap|headerlengh|header|longpos|command
    //  full header length including preheader information -> full kB
    //  define header length without information describing the header
    size_t hlen = sizeof(char)                                       +  // code type (ASCII, UCS2, ...)
                  sizeof(char)                                       +  // flags use of default code page
                  sizeof(tls30_FileMetaDataHeader)                   +
                  pDatExtrRec->derLongColCnt_ls00*sizeof(tsp00_Int4) +
                  _tcslen(pTransformationCmd->trFullUserInput_ls00);

    //*
    //*     Write code page name and mapping table to file only if the
    //*     code type is ASCII and the code page is not ISO-8859-1 (which is the default)
    //*
    tsp81_CodePage &UsedCodePage = pDBInfo->dbiSession_ls00.siCodePage_ls00;
    bool bDefaultCPUsed = (0 != _tcsnicmp(UsedCodePage.name.asCharp(),
                                          pszDefaultCodePageName_ls00,
                                          UsedCodePage.name.length()) )
                        ? false
                        : true;


    if ( (ctAscii_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) &&
         (false         == bDefaultCPUsed) )
    {
        hlen += sizeof(tsp00_KnlIdentifierc) + sizeof(pDBInfo->dbiSession_ls00.siCodePage_ls00.map);
    }

    // define overall length of buffer to write - we need to be really careful with the
    // different integer types on 32bit and 64bit machines: size_t is 4 Bytes long on a
    // 32bit machine and 8 Bytes long on a 64bit machine; we do actually want only 4 Bytes:
    tsp00_Uint4 len = STATIC_CAST(tsp00_Uint4, (pszVersion.size() + sizeof(char) + sizeof(tsp00_Int4) + hlen));

    len = ALIGN_EO00(len, 1024);                    // align file header to full kB
    tsp00_Addr pszBuffer = new _TCHAR[len];
    memset(pszBuffer, 0, len);

    tsp00_Addr pszKeepBufPtr = pszBuffer;

    SAPDB_memcpy(pszBuffer, pszVersion.asCharp(), pszVersion.length());
    pszBuffer += pszVersion.size();

    *pszBuffer = STATIC_CAST(unsigned char, ls07DefineSwap(NULL));
    pszBuffer += sizeof(char);

    // store the full header length in header
    tsp00_Int4 auxlen = STATIC_CAST(tsp00_Int4, len);
    SAPDB_memcpy(pszBuffer, &auxlen, sizeof(tsp00_Int4));
    pszBuffer += sizeof(tsp00_Int4);

    //*
    //*     Write code type and code page information to header
    //*
    *pszBuffer = STATIC_CAST(unsigned char, pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00);
    if ( (true               == ls00IsUCS2(&pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) ) &&
         (ctUCS2Native_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) )
    {
        // Write the exact UCS2 swap kind
        if (0 == pEncodings_ls00[pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00]->charTable->blank.bytes[0])
        {
            *pszBuffer = STATIC_CAST(unsigned char, ctUCS2_els00);
        }
        else
        {
            *pszBuffer = STATIC_CAST(unsigned char, ctUCS2Swapped_els00);
        }
    }
    pszBuffer += sizeof(char);

    *pszBuffer = STATIC_CAST(unsigned char, bDefaultCPUsed);
    pszBuffer += sizeof(char);

    if ( (ctAscii_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) &&
         (false         == bDefaultCPUsed)         )
    {
        // Code page name
        SAPDB_memcpy(pszBuffer,
                     pDBInfo->dbiSession_ls00.siCodePage_ls00.name.asCharp(),
                     pDBInfo->dbiSession_ls00.siCodePage_ls00.name.length());
        pszBuffer += sizeof(tsp00_KnlIdentifierc);

        // Code page UCS2 map
        SAPDB_memcpy(pszBuffer,
                     pDBInfo->dbiSession_ls00.siCodePage_ls00.map,
                     sizeof(pDBInfo->dbiSession_ls00.siCodePage_ls00.map));
        pszBuffer += sizeof(pDBInfo->dbiSession_ls00.siCodePage_ls00.map);
    }

    // define meta information store
    tls30_FileMetaDataHeader pMeta; // = REINTERPRET_CAST(tls30_FileMetaDataHeader*, pszBuffer);

    pMeta.mdRecordLength_ls30 = pDatExtrRec->derRecordLen_ls00;
    pMeta.mdFieldCount_ls30   = pDatExtrRec->derTableInfo_ls00.tdFieldCount_ls00;
    pMeta.mdLongCount_ls30    = pDatExtrRec->derLongColCnt_ls00;
    pMeta.mdMetaData_ls30     = pTransformationCmd->trDataFile_ls00.fsFormat_ls00;

    pDatExtrRec->derTabEmpty_ls00 == true ?
        pMeta.mdTableEmpty_ls30 = 1 :
        pMeta.mdTableEmpty_ls30 = 0;

    SAPDB_memcpy(pszBuffer, REINTERPRET_CAST(unsigned char*, &pMeta), sizeof(tls30_FileMetaDataHeader));
    pszBuffer += sizeof(tls30_FileMetaDataHeader);      // move pointer behind tls30_FileMetaDataHeader to
                                                        // write additional information

    // write long positions in data record behind written header
    tls00_MultColSpec &MultCol = pTransformationCmd->trColSpec_ls00;
    tls00_Column      *pColumn = NULL;
    for (tsp00_Int4 i = 0; i < MultCol.mcsCount_ls00; ++i)
    {
        pColumn = MultCol.mcsColumn_ls00[i];

        if (1            == szMap_LongDataType_ls00[pDatExtrRec->pderColumnData_ls00[i].cdSQLDataType_ls00] ||
            atLong_els00 == pColumn->colAbstractType_ls00  )
        {
            tsp00_Int4 pos = i+1;
            SAPDB_memcpy(pszBuffer, &pos, sizeof(tsp00_Int4));
            pszBuffer += sizeof(tsp00_Int4);
        }
    }

    // write command to buffer
    SAPDB_memcpy(pszBuffer, pTransformationCmd->trFullUserInput_ls00, _tcslen(pTransformationCmd->trFullUserInput_ls00));

    // write structure to file; updates file pos
    rc = ls30VFWriteLn(*pDatExtrRec->derDataFile_ls00, pszKeepBufPtr, len, ErrText);
    if (NULL != pszKeepBufPtr)
    {
        delete [] pszKeepBufPtr;
        pszKeepBufPtr = NULL;
    }

    return rc;
}
// ls27WriteMetaHeader()


/*
  -----------------------------------------------------------------------------
  function:     ls27WriteMetaTrailer
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls27WriteMetaTrailer(TransformationDef       *&pTransformationDef,
                     tls00_VFile              &VFile,
                     tls00_TransformationCmd  *pTransformationCmd,
                     tls00_DextractRec        *pDatExtrRec,
                     tsp00_Addr               ErrText )
{
    ROUTINE_DBG_MEO00 ("ls27WriteMetaTrailer");

    tsp00_Int4 rc = errOK_els00;

    // initialize buffer and buffer position; we use pDataSink->dskMetaLine_ls27 here only
    tls00_Buffer2 *pMetaLine = &pDatExtrRec->derDataSink_ls00.dskMetaLine_ls27;
    pMetaLine->lBufPos_ls00  = 0;

    tsp00_Int4   i = 0;
    tsp00_Int2 i_2 = STOP_NOMORE_DATA_LS00;
    tsp00_Int4 i_4 = STOP_NOMORE_DATA_LS00;

    for ( i = 0; i < pDatExtrRec->derTableInfo_ls00.tdFieldCount_ls00; i++)
    {
         if (1 == szMap_LongDataType_ls00[pDatExtrRec->derTableInfo_ls00.tdParamArray_ls00[i]->piBasicInfo_ls00.fbDataType_ls00])
         {
             SAPDB_memcpy(&(STATIC_CAST(_TCHAR*, pMetaLine->pBufPtr_ls00)[pMetaLine->lBufPos_ls00]), &i_4, sizeof(tsp00_Int4));
             pMetaLine->lBufPos_ls00 += sizeof(tsp00_Int4);
         }
         else
         {
             SAPDB_memcpy(&(STATIC_CAST(_TCHAR*, pMetaLine->pBufPtr_ls00)[pMetaLine->lBufPos_ls00]), &i_2, sizeof(tsp00_Int2));
             pMetaLine->lBufPos_ls00 += sizeof(tsp00_Int2);
         }
    } // end for ( i = 0; i < pDatExtrRec->derTableInfo_ls00.tdFieldCount_ls00; i++)


    // define trailer
    Tools_DynamicUTF8String  _name;
    Tools_DynamicUTF8String  transformation("LOAD DATA TABLE \"");

    // OWNER
    if (0 != pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.length())
    {
        _name.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                      pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.asCharp()));
        transformation += _name;
        transformation += "\".\"";
    }

    // TABLE
    _name.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                  pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp()));
    transformation += _name;
    transformation += "\" ";

    // datafile
    _name.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,
                                  pDatExtrRec->derDataFile_ls00->vfFileName_ls00));
    transformation += "INSTREAM '";
    transformation += _name;
    transformation += "' RECORDS ";

    _TCHAR RECORD_LOADED[64];
    _TCHAR HEADER_BLOCK [64];
    _TCHAR HEADER_POS   [64];
    RECORD_LOADED  [0] = 0;
    HEADER_BLOCK   [0] = 0;
    HEADER_POS     [0] = 0;

    // <HEADER_BLOCK>:<HEADER_POS> (number strings have same encodings in ASCII and UTF8)
    _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derHeaderFilePos_ls00.lnu2GB_Blocks_ls00), HEADER_BLOCK, 10);
    _itot(STATIC_CAST(tsp00_Int4, pDatExtrRec->derHeaderFilePos_ls00.lnuRemainder_ls00), HEADER_POS, 10);

    // RECORD_LOADED
    _itot(pDatExtrRec->derRecordCount_ls00, RECORD_LOADED, 10);

    transformation += " START ";

    transformation += HEADER_BLOCK;
    transformation += ":";
    transformation += HEADER_POS;

    transformation += "  0:";     // record_block is always set to 0
    transformation += RECORD_LOADED;

    // prepare trailer buffer
    tsp00_Int4 transformation_length   = transformation.BasisSize();
    tsp00_Int4 trailerbuffer_len = STATIC_CAST(tsp00_Int4, pMetaLine->lBufLen_ls00) + transformation_length;
    trailerbuffer_len            = ALIGN_EO00(trailerbuffer_len, 1024);

    tsp00_Addr pTrailerBuffer    = new _TCHAR[trailerbuffer_len];
    memset(pTrailerBuffer, 0, trailerbuffer_len);

    // store metadata in trailer
    tsp00_Longint lBytes = pMetaLine->lBufPos_ls00;
    SAPDB_memcpy(&pTrailerBuffer[0], STATIC_CAST(_TCHAR*, pMetaLine->pBufPtr_ls00), lBytes);

    // store trailer length at the end of metadata as binary value (in machine swap)
    SAPDB_memcpy(&pTrailerBuffer[lBytes], &trailerbuffer_len, sizeof(tsp00_Int4));
    lBytes += sizeof(tsp00_Int4);

    // store command
    SAPDB_memcpy(&pTrailerBuffer[lBytes], STATIC_CAST(const unsigned char*, transformation.StrPtr()), transformation_length);

    // <TRAILER_BLOCK>:<TRAILER_POS>
    pTransformationDef->m_trailerpos.lnu2GB_Blocks_ls00 = VFile.vfFilePos_ls00.lnu2GB_Blocks_ls00;
    pTransformationDef->m_trailerpos.lnuRemainder_ls00  = VFile.vfFilePos_ls00.lnuRemainder_ls00;

    rc = ls30VFWriteLn(VFile, pTrailerBuffer, trailerbuffer_len, ErrText);

    if ( NULL != pTrailerBuffer )
    {
        delete [] pTrailerBuffer;
        pTrailerBuffer = NULL;
    }

    return rc;
}// ls27WriteMetaTrailer()

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
  function: ls27_PreParseSelectStmt
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_PreParseSelectStmt(tls00_DBInfo      *pDBInfo,
                        tls00_TransformationCmd  *pTransformationCmd,
                        tls00_DextractRec *pDatExtrRec,
                        tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_PreParseSelectStmt");

    tsp00_Int4 rc = errOK_els00;

    rc = ls27_ParseSelectStmt(pDBInfo, pTransformationCmd, pDatExtrRec, pszErrText);

    if (errOK_els00 == rc)
    {
        if (true == ls00UseUnicodeOI(pDBInfo))
        {
            TRACE_PRNF_MLS99(("ls27_PreParseSelectStmt", "Unicode Order Interface"));

            // User has either 
            //  - not specified an encoding for data file determine right encoding
            //  - has specified to save the data as ASCII; check if this is possible and if not (at
            //    least on column is of UNICODE data type) raise an error

            SAPDB_Bool bAllCharColumnsAreAscii = SAPDB_TRUE;
            
            if ( (ctUnknown_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) ||
                 (ctAscii_els00   == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) )
            {
                TRACE_PRNF_MLS99(("ls27_PreParseSelectStmt", "Check if ASCII Order Interface can be used"));

                tls00_TableDescription* pTD = &pDatExtrRec->derTableInfo_ls00;
                tls00_ParamInfo*        pPI = NULL;

                // Keep result table name for the sake of reusing it in case using
                // the ASCII order interface is impossible
                tsp00_Int4 len = _tcslen(pDatExtrRec->derRTableName_ls00);
                char* pSavedResultTabName = new char[sizeof(tsp00_KnlIdentifierc)];
                SAPDB_memcpy(pSavedResultTabName, pDatExtrRec->derRTableName_ls00, len);
                pSavedResultTabName[len] = 0;

                for (tsp00_Int4 i = 0; i < pTD->tdFieldCount_ls00; ++i)
                {
                    pPI = pTD->tdParamArray_ls00[i];

                    if (1 == szMap_UnicodeDataType_ls00[pPI->piBasicInfo_ls00.fbDataType_ls00])
                    {
                        if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
                        {
                            // In case of format RECORDS we have to handle CHAR and LONG CHAR
                            // data the same way because all data is stored in one single medium
                            bAllCharColumnsAreAscii = SAPDB_FALSE;
                            break;
                        }
                        else
                        {
                            // In case of format COMPRESSED/FORMATTED we have to handle CHAR and LONG CHAR
                            // data differently because 'normal' data and LONG data is stored in different media
                            if (0 == szMap_LongDataType_ls00[pPI->piBasicInfo_ls00.fbDataType_ls00])
                            {
                                bAllCharColumnsAreAscii = SAPDB_FALSE;
                                break;
                            }
                        }
                        // At least one unicode data type found -> we have to set the file encoding
                        // to UCS2 depending on OS because we have to use the UNICODE order interface
                    }
                }

                if (SAPDB_TRUE == bAllCharColumnsAreAscii)
                {
                    // Do parsing again, now with an ASCII order interface

                    // Set order interface encoding to ASCII for 'reparsing'; it has to be reset
                    // after successfully processed request

                    pDBInfo->dbiOI_Encoding_ls00 = ctAscii_els00;
                    rc = ls27_ParseSelectStmt(pDBInfo, pTransformationCmd, pDatExtrRec, pszErrText, true);
                    if (errOK_els00 == rc)
                    {
                        // 'Reparsing' succeeded:
                        // set in any case; we come here only if it is already set to ASCII or it is not specified
                        pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;

                        TRACE_PRNF_MLS99(("ls27_PreParseSelectStmt", "Switch to ASCII Order Interface"));
                    }
                    else
                    {
//TODOTODO  ARE THERE OTHER RETURNCODES THAT MARK THE IMPOSSIBILITY OF USING AN ASCII INTERFACE???
                        if ( (-4004 == rc) || (-8046 == rc) )
                        {
                            // Hmm, can't seem to work with the ASCII interface; 
                            rc = errOK_els00;

                            // Switch interface back to UNICODE
                            pDBInfo->dbiOI_Encoding_ls00 = pDBInfo->dbiEncoding_ls00;

                            // Set data file encoding type accordingly
                            if (ctUnknown_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                            {
                                // We set here to UNICODE to prevent double conversion from ASCII to
                                // UNICODE (kernel) and back from UNICODE to ASCII (Loader);
                                // in case the user specified the encoding type with ASCII we'll
                                // respect that, but a performance loss has to be taken into account
                                pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUCS2Native_els00;
                            }

                            // 'Reactivate' result table name from UNICODE-parsing
                            SAPDB_memcpy(pDatExtrRec->derRTableName_ls00, pSavedResultTabName, len);
                            pDatExtrRec->derRTableName_ls00[len] = 0;

                            delete pSavedResultTabName;
                            pSavedResultTabName = NULL;
                            
                            TRACE_PRNF_MLS99(("ls27_PreParseSelectStmt", "Unicode Order Interface must be used"));
                        }
                    }
                }
                else
                {
                    if (ctUnknown_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                    {
                        pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = pDBInfo->dbiOI_Encoding_ls00;
                    }
                    else
                    {
                        // Raise an error;
                        //  We cannot store UNICODE data in ASCII files without data loss
                        //   (would code page transformation be an option?)
                        LoaderMessages::Instance()->ls98Msg(layDataExtract_els98, pszErrText, errIncompatibleEncodings_els98);
                        rc = errIncompatibleEncodings_els98;
                    }
                }
            }   // end if ( (ctUnknown_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) || ...
        }
        else    // if (false == ls00IsDBUnicode(pDBInfo))
        {
            TRACE_PRNF_MLS99(("ls27_PreParseSelectStmt", "ASCII Order Interface"));
            if (ctUnknown_els00 == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
            {
                pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                
                TRACE_PRNF_MLS99(("ls27_PreParseSelectStmt", "Data file encoding default set to ASCII"));
            }
        }
    }
    return rc;
}
// ls27_PreParseSelectStmt()

/*
  -----------------------------------------------------------------------------
  function: ls27_InitDatExtrRec
  -----------------------------------------------------------------------------
*/
static int
ls27_InitDatExtrRec(tls00_DBInfo             *pDBInfo,
                    MetaDataDef             *&pMetaDataDef,
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_DextractRec       *&pDatExtrRec,
                    tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_InitDatExtrRec");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();

    tsp00_Int4 rc    = errOK_els00;

    bool       bSelf = false;

    TransformationDef* pTransformationDef = pMetaDataDef->getTransformationModel();
    //*
    //* performance
    //*
    extern tls00_Performance performance;
    memset(&performance,0,sizeof(tls00_Performance));

    // table empty|full
    pDatExtrRec->derTableSwitched_ls00 = true;
    pDatExtrRec->derTabEmpty_ls00      = false;
    pDatExtrRec->derTabExtracted_ls00  = false;
    //*
    //*  db|user
    //*
    pDatExtrRec->derLoadedRows_ls00    = 0;
    pDatExtrRec->derRecordCount_ls00   = 0;
    if (pDatExtrRec->derPartSpec_ls00.psPartType_ls00 > ptTable_els00)
    {
        // restart records
        pDatExtrRec->derExtractedRows_ls00 = pTransformationDef->m_exportcount;
        pDatExtrRec->derLastCommLine_ls00  = pDatExtrRec->derExtractedRows_ls00;
        strcpy(pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00, pDBInfo->dbiSession_ls00.siSchemaName_ls00);
    }
    else
    {
        pDatExtrRec->derExtractedRows_ls00 = 0;
        pDatExtrRec->derLastCommLine_ls00  = 0;
    }
    //*
    //* tabledescription
    //*
    tls00_TableDescription  *pTableDescription  = &pDatExtrRec->derTableInfo_ls00;
    memset( pTableDescription ,0, sizeof(tls00_TableDescription));
    memset(pDatExtrRec->derParseId_ls00,0,MAX_PARSEID_LENGTH_LS00 + 1 );
    pDatExtrRec->derTableInfo_ls00.tdKernelSwap_ls00.becomes(sw_dummy);
    // packets
    pDatExtrRec->derUseGetvalPckt_ls00 = false;
    pDatExtrRec->derMaxRowsPPckt_ls00  = 0;
    // datasource
    memset(&pDatExtrRec->derDataSource_ls00, 0, sizeof(tls00_DataSource));
    // datasink
    tls27_DataSink *pDataSink = &pDatExtrRec->derDataSink_ls00;
    memset(pDataSink, 0, sizeof(tls27_DataSink));
    // records
    pDatExtrRec->derErrNumInRecord_ls00    = 0;
    pDatExtrRec->derRecordCount_ls00       = 0;
    pDatExtrRec->derResultCount_ls00       = 0;
    pDatExtrRec->derRecordLen_ls00         = 0;
    pDatExtrRec->derExtLenFrac_ls00        = NULL;
    pDatExtrRec->derFillValue_ls00         = ls07GetDefByte(dcha);
    // columns
    pDatExtrRec->derLongColCnt_ls00        = 0;
    pDatExtrRec->derLongDescArray_ls00     = NULL;
    memset(&pDatExtrRec->derLongInfo_ls00, 0, sizeof(tls00_LongExecInfo));
    //*
    //*  start extract with preparation of select command and getting parameter info
    //*
    rc = ls27_PreParseSelectStmt(pDBInfo, pTransformationCmd, pDatExtrRec, ErrText);
    if (errOK_els00 == rc)
    {
        // Do this only for single table extracts; in case USER|ALL only RECORDS or PAGES would be allowed
        //if (ptTable_els00 == pDatExtrCmd->dexPartSpec_ls00.psPartType_ls00)
        if (ptTable_els00 == pDatExtrRec->derPartSpec_ls00.psPartType_ls00)
        {
             // pattern structure in case of external data file format COMPRESSED
            if ( NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00 &&
                 true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00)
            {
                pDatExtrRec->pderPattern_ls00 = new tls00_Pattern;
                memset(pDatExtrRec->pderPattern_ls00, 0, sizeof(tls00_Pattern));

                rc = ls18ConstructPattern(pDatExtrRec->pderPattern_ls00,
                                          pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoSeperat_ls00,
                                          pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoDelimit_ls00,
                                          pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00,
                                          ErrText);
            }
        }   // end if (ptTable_els00 == pDatExtrRec->derPartSpec_ls00.psPartType_ls00)
    }

    if( errOK_els00 == rc )
    {
        tls00_MultColSpec  &MultColSpec =  pTransformationCmd->trColSpec_ls00;
        tls00_Column       *pCol        =  NULL;
        tls00_ColumnData  *&pColumnData =  pDatExtrRec->pderColumnData_ls00;
        tls00_FileFormat   *pFileFormat = &pTransformationCmd->trDataFile_ls00.fsFormat_ls00;
        tls00_FieldBasics  *pBI         =  NULL;

        if (0 == MultColSpec.mcsCount_ls00)
        {
            ls27_FillColStruct(pTransformationCmd, pTableDescription);
            bSelf = true;
        }

        //*
        //*     1. Initialize column data structure with the count of columns to extract
        //*     2. Define fraction and hex representation
        //*
        //* This can't be done in the constructor because in case of DATAEXTRACT FOR DATALOAD the
        //* count of columns in the command is 0!
        //*
        pColumnData = new tls00_ColumnData[MultColSpec.mcsCount_ls00];
        memset(pColumnData, 0, (sizeof(tls00_ColumnData) * MultColSpec.mcsCount_ls00) );

        if (errOK_els00 == rc)
        {
            rc = ls27_MakeMapIndex(&MultColSpec, pTableDescription, pColumnData, ErrText);
        }

        if (errOK_els00 == rc)
        {
            rc = ls27_AnalyzeLongInfo(pTransformationCmd, pDatExtrRec, ErrText);
        }

//TODOTODO hex representation muss noch mal ueberarbeitet werden und nur in column data uebernommen werden
        tsp00_Int4        lValLen  = 0;
        tls00_ColumnData *pColData = NULL;
        tls00_ParamInfo  *pParamInfo = NULL;

        for (int k = 0, l = 0; k < MultColSpec.mcsCount_ls00; ++k)
        {
            pColData   = &pColumnData[k];
            pCol       = MultColSpec.mcsColumn_ls00[k];
            pParamInfo = pTableDescription->tdParamArray_ls00[pColData->cdPosInStmt_ls00];
            pBI        = &pParamInfo->piBasicInfo_ls00;

            pColData->cdSQLDataType_ls00  = pBI->fbDataType_ls00;
            pColData->cdSQLLen_ls00       = pBI->fbInOutLength_ls00;
            pColData->cdExtEncoding_ls00  = pFileFormat->ffoCodeType_ls00;

            // Blob data type -> make the output format hex if it is not long byte;
            // in case of long byte the output value to data file is the offset or file
            // name of the long file, the long value output must remain byte
            if ( (1 == szMap_BlobDataType_ls00[pColData->cdSQLDataType_ls00]) &&
                 (0 == szMap_LongDataType_ls00[pColData->cdSQLDataType_ls00])  )
            {
                if ( (false == pFileFormat->ffoBinary_ls00) && (false == pCol->colFormat_ls00.ffoIsHex_ls00) )
                {
                    pCol->colFormat_ls00.ffoIsHex_ls00 = true;
                }
            }

            pColData->cdExtHexRepresentation_ls00 = pCol->colFormat_ls00.ffoIsHex_ls00;

            if (false == pFileFormat->ffoFormatted_ls00)
            {
                if (1 == szMap_NumberDataType_ls00[pColData->cdSQLDataType_ls00])
                {
                    // We get the needed length by specifying a length of 0
                    lValLen = 0;
                    ls19CheckLengthForExtract(0, pCol, pBI, pFileFormat, lValLen, NULL);
                    pColData->cdDataBufferLen_ls00 = lValLen;
                }
                else if (dboolean == pColData->cdSQLDataType_ls00)
                {
                    // boolean values are delivered from kernel with a length of 1 byte; output
                    // length is derived from the external boolean representation
                    pColData->cdDataBufferLen_ls00 = MAX_BOOLVALUE_LENGTH_LS00;
                }
                else if (1 == szMap_LongDataType_ls00[pColData->cdSQLDataType_ls00])
                {
                    // In case the file is numbered the buffer length must fit to
                    // hold the file name + trailing null
                    pColData->cdDataBufferLen_ls00 = pBI->fbInOutLength_ls00;
                    if (true == pCol->colLongInfo.loibFileNumbered_ls00)
                    {
                        pColData->cdDataBufferLen_ls00 = _tcslen(pCol->colLongInfo.loiFileName_ls00)+ 2*sizeof(char);
                    }
                }
                else
                {
                    pColData->cdDataBufferLen_ls00 = pBI->fbInOutLength_ls00;
                }

                // Check for writing (possible) NULL values and lengthen the buffer accordingly
                if ( false == pParamInfo->piMode_ls00.includes(sp1ot_mandatory) )
                {
                    // column is not mandatory and thus MAY allow NULL values -> be prepared
                    if (0 != pCol->colAssign_ls00.fasAsgnLen_ls00)
                    {
                        // Customer specified special NULL value for this column
                        if (pColData->cdDataBufferLen_ls00 < pCol->colAssign_ls00.fasAsgnLen_ls00)
                        {
                            pColData->cdDataBufferLen_ls00 = pCol->colAssign_ls00.fasAsgnLen_ls00;
                        }
                    }
                    else
                    {
                        // Customer specified no special NULL value for this column
                        if (pColData->cdDataBufferLen_ls00 < pFileFormat->ffoNSpecLen_ls00)
                        {
                            pColData->cdDataBufferLen_ls00 = pFileFormat->ffoNSpecLen_ls00;
                        }
                    }
                }   // end if ( false == pParamInfo->piMode_ls00.includes(sp1ot_mandatory) )


                // be prepared for unicode and hex representation: make the buffer large enough
                if ( true == ls00IsUCS2(&pColData->cdExtEncoding_ls00) )
                {
                    if (true == pColData->cdExtHexRepresentation_ls00)
                    {
                        pColData->cdDataBufferLen_ls00 *= 8;        //2 (hex) *2 (unicode) *2 (hex)
                    }
                    else
                    {
                        pColData->cdDataBufferLen_ls00 *= 2;
                    }
                }
                else if (ctUTF8_els00 == pColData->cdExtEncoding_ls00)
                {
                    if (true == pColData->cdExtHexRepresentation_ls00)
                    {
                        pColData->cdDataBufferLen_ls00 *= 16;
                    }
                    else
                    {
                        pColData->cdDataBufferLen_ls00 *= 4;
                    }
                }
                else
                {
                    if (true == pColData->cdExtHexRepresentation_ls00)
                    {
                        pColData->cdDataBufferLen_ls00 *= 2;
                    }
                }
            }   // end if (false == pFileFormat->ffoFormatted_ls00)
            else
            {
                pColData->cdDataBufferLen_ls00 = pCol->colFPos_ls00.fpoEnd_ls00 - pCol->colFPos_ls00.fpoStart_ls00 + 1;
            }

            pColData->cdDataBuffer_ls00  = new _TCHAR[pColData->cdDataBufferLen_ls00];
            pColData->cdExtDataType_ls00 = pCol->colFormat_ls00.ffoFormat_ls00;
            pColData->cdIsNILValue_ls00  = false;
            pColData->cdPtrToData_ls00   = NULL;
            pColData->cdPtrUsed_ls00     = true;


            //*
            //*     fraction value and hex representation
            //*
            //* If fraction is still undefined make it have the db value
            if ( (false == pCol->colIsLit_ls00) &&
                 (UNDEFINED_LS00 == pCol->colFormat_ls00.ffoFrac_ls00) )
            {
                pCol->colFormat_ls00.ffoFrac_ls00 = pBI->fbFraction_ls00;
            }
        }   // end for (int k = 0, l = 0; k < pTransformationCmd->trColSpec_ls00.mcsCount_ls00; ++k)


        //*
        //*  initialize Long descriptor array
        //*
        //* This can't be done in the constructor because in case of DATAEXTRACT FOR DATALOAD with
        //* format COMPACT the count of long files in the command is 0!
        //*
        pDatExtrRec->derLongDescArray_ls00            = new tsp00_LongDescriptor[pDatExtrRec->derLongColCnt_ls00];
        pDatExtrRec->derLongInfo_ls00.dliNNLongs_ls00 = new tsp00_Int2[pDatExtrRec->derLongColCnt_ls00];
        memset(pDatExtrRec->derLongInfo_ls00.dliNNLongs_ls00, 0, sizeof(tsp00_Int2) * pDatExtrRec->derLongColCnt_ls00);


        //*
        //*  compatibility and length checks
        //*
        if (errOK_els00 == rc)
        {
            rc = ls27_CheckCompatibility(&MultColSpec, pTableDescription, pColumnData, pFileFormat, ErrText);
        }

        if (errOK_els00 == rc)
        {
            pDatExtrRec->derExtLenFrac_ls00 = new tsp00_Int4[MultColSpec.mcsCount_ls00][2];

            ls27_RecalculateLength(&MultColSpec, pTableDescription, pColumnData, pDatExtrRec->derExtLenFrac_ls00);
        }

        if ( (errOK_els00 == rc) && (false == bSelf) )
        {
            rc = ls27_CheckLength(&MultColSpec, pTableDescription, pFileFormat, pColumnData, pDatExtrRec->derExtLenFrac_ls00, ErrText);
        }

        //*
        //*  set long file encoding in column data structure
        //*  compute record length, bytes per line
        //*
        if (errOK_els00 == rc)
        {
            pColData = NULL;
            for (int i = 0, j = 0; i < pTransformationCmd->trColSpec_ls00.mcsCount_ls00; ++i)
            {
                pColData = &pColumnData[i];
                pCol     = MultColSpec.mcsColumn_ls00[i];
                pBI      = &pTableDescription->tdParamArray_ls00[pColData->cdPosInStmt_ls00]->piBasicInfo_ls00;

                // long file encoding
                if (1 == szMap_LongDataType_ls00[pColData->cdSQLDataType_ls00])
                {
                    pColData->cdExtLONGEncoding_ls00 = pCol->colLongInfo.loiFileEncoding_ls00;
                    ++j;
                }
            }

            pDatExtrRec->derRecordLen_ls00 = ls18GetRecordLength(*pTableDescription, true);

            pFileFormat->ffoBytesPerLine_ls00 = ls18GetRowLengthForExtract(&MultColSpec, pColumnData, pTableDescription, pFileFormat);
        }
        //*
        //*   compute maximum number of records fitting in one order
        //*   interface packet for this special command
        //*
        if (errOK_els00 == rc)
        {
            pDatExtrRec->derMaxRowsPPckt_ls00 = ( pDBInfo->dbiPktSize_ls00 -
                                                ( BlockHSize_ls00 + PartHSize_ls00 + mxsp_resnum + 16) ) /
                                                  pDatExtrRec->derRecordLen_ls00;
            if(0 == pDatExtrRec->derMaxRowsPPckt_ls00)
            {
                pDatExtrRec->derMaxRowsPPckt_ls00 = 1;
            }
        }

        // initialize buffer pointer
        pDataSink->dskOutLine_ls27.lBufLen_ls00  = 6 * pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoBytesPerLine_ls00;

        if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
        {
            if (errOK_els00 == rc)
            {
                pDataSink->dskMetaLine_ls27.lBufLen_ls00 =
                sizeof(tsp00_Int2   )*(pTableDescription->tdFieldCount_ls00 - pDatExtrRec->derLongColCnt_ls00) +
                sizeof(tsp00_Int4)* pDatExtrRec->derLongColCnt_ls00 ;

                pDataSink->dskOutLine_ls27 .lBufLen_ls00 += pDataSink->dskMetaLine_ls27.lBufLen_ls00 ;
            }
        }

        pDataSink->dskOutLine_ls27 .pBufPtr_ls00  = new _TCHAR[pDataSink->dskOutLine_ls27.lBufLen_ls00];

        pDataSink->dskMetaLine_ls27.pBufPtr_ls00  = STATIC_CAST(_TCHAR*, pDataSink->dskOutLine_ls27 .pBufPtr_ls00);   // metadata pointer
        pDataSink->dskOutLine_ls27 .pBufPtr_ls00  = STATIC_CAST(_TCHAR*, pDataSink->dskOutLine_ls27 .pBufPtr_ls00) +
                                                                         pDataSink->dskMetaLine_ls27.lBufLen_ls00;    // datapointer
    }

    return rc;
}
// ls27_InitDatExtrRec()

/*
  -----------------------------------------------------------------------------
  function:     ls27_FreeDatExtrRec
  -----------------------------------------------------------------------------
*/
static void
ls27_FreeDatExtrRec(tls00_DBInfo              *pDBInfo,
                    tls00_TransformationCmd   *pTransformationCmd,
                    tls00_DextractRec        *&pDatExtrRec)
{
    ROUTINE_DBG_MEO00 ("ls27_FreeDatExtrRec");

    //*
    //* performance
    //*
    extern tls00_Performance performance;
    memset(&performance,0,sizeof(tls00_Performance));

    pDatExtrRec->derTabEmpty_ls00    = false;
    pDatExtrRec->derRecordCount_ls00 = 0;
    pDatExtrRec->derLRejected_ls00   = pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors;

    // Reinitialize order interface encoding type
    if ( (ls00IsDBUnicode(pDBInfo)) && (ctAscii_els00 == pDBInfo->dbiOI_Encoding_ls00) )
    {
        pDBInfo->dbiOI_Encoding_ls00 = ctUCS2Native_els00;
    }

    if (NULL != pDatExtrRec)
    {
        //*
        //*  delete cursor
        //*
        char ErrText[256];
        ls12CloseCursor(*pDBInfo, pDatExtrRec->derRTableName_ls00, ErrText);
        // Error text is of no interest here
        ErrText[0] = 0;

        //*
        //*  delete  data sink
        //*
        tls27_DataSink *pDataSink = &pDatExtrRec->derDataSink_ls00;

        if ( NULL != pDataSink )
        {
            if (NULL != pDataSink->dskOutLine_ls27.pBufPtr_ls00)
            {
                if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
                {
                    pDataSink->dskOutLine_ls27.pBufPtr_ls00 = pDataSink->dskMetaLine_ls27.pBufPtr_ls00;
                }

                if (NULL != pDataSink->dskMetaLine_ls27.pBufPtr_ls00)
                {
                    pDataSink->dskMetaLine_ls27.pBufPtr_ls00 = NULL;
                }

                delete [] (STATIC_CAST(_TCHAR*, pDataSink->dskOutLine_ls27.pBufPtr_ls00));
                pDataSink->dskOutLine_ls27.pBufPtr_ls00 = NULL;
            }
        }

        if (NULL != pDatExtrRec->derExtLenFrac_ls00)
        {
            delete [] pDatExtrRec->derExtLenFrac_ls00;
            pDatExtrRec->derExtLenFrac_ls00 = NULL;
        }

        if (NULL != pDatExtrRec->derLongInfo_ls00.dliNNLongs_ls00)
        {
            delete [] pDatExtrRec->derLongInfo_ls00.dliNNLongs_ls00;
            pDatExtrRec->derLongInfo_ls00.dliNNLongs_ls00 = NULL;
        }

        if (NULL != pDatExtrRec->derLongDescArray_ls00)
        {
            delete [] pDatExtrRec->derLongDescArray_ls00;
            pDatExtrRec->derLongDescArray_ls00 = NULL;
        }

        if ( NULL!= pDatExtrRec->derRTableName_ls00 )
        {
             delete [] pDatExtrRec->derRTableName_ls00;
             pDatExtrRec->derRTableName_ls00 = NULL;
        }

        if ( NULL != pDatExtrRec->pderColumnData_ls00 )
        {
            tls00_ColumnData *pColData = NULL;

            for (int k = 0; k < pTransformationCmd->trColSpec_ls00.mcsCount_ls00; ++k)
            {
                pColData = &pDatExtrRec->pderColumnData_ls00[k];
                if (NULL != pColData->cdDataBuffer_ls00)
                {
                    // Delete data buffer
                    delete [] pColData->cdDataBuffer_ls00;
                    pColData->cdDataBuffer_ls00 = NULL;
                    // Reset pointer to data
                    pColData->cdPtrToData_ls00 = NULL;
                }
            }

            delete [] pDatExtrRec->pderColumnData_ls00;
            pDatExtrRec->pderColumnData_ls00 = NULL;
        }

        for ( int i = 0; i < csp_max_fields; ++i)
        {
            if (NULL != pDatExtrRec->derTableInfo_ls00.tdParamArray_ls00[i])
            {
                delete pDatExtrRec->derTableInfo_ls00.tdParamArray_ls00[i];
                pDatExtrRec->derTableInfo_ls00.tdParamArray_ls00[i] = NULL;
            }

            if (NULL != pDatExtrRec->derTableInfo_ls00.tdColName_ls00[i])
            {
                delete pDatExtrRec->derTableInfo_ls00.tdColName_ls00[i];
                pDatExtrRec->derTableInfo_ls00.tdColName_ls00[i] = NULL;
            }
        }
        memset(&pDatExtrRec->derTableInfo_ls00,0, sizeof(tls00_TableDescription));

        ls18DestructPattern(pDatExtrRec->pderPattern_ls00);
    } //if (NULL != pDatExtrRec)

    return;
}// ls27_FreeDatExtrRec

/*
  -----------------------------------------------------------------------------
  function: ls27_InitStreams
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_InitStreams(tls00_TransformationCmd  *pTransformationCmd,
                 tls00_DextractRec        *pDatExtrRec,
                 tsp00_Addr                ErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_InitStreams");

    tsp00_Int4 rc = errOK_els00;
    //*
    //*  data file
    //*
    if ( (NULL  != pDatExtrRec->derDataFile_ls00) && (0 == pDatExtrRec->derDataFile_ls00->vfFileHndl_ls00) )
     {
        // Do we have to 'reinitialize' the code type of the file?
        if (ctUnknown_els00 == pDatExtrRec->derDataFile_ls00->vfFileEncoding_ls00)
        {
            pDatExtrRec->derDataFile_ls00->vfFileEncoding_ls00 =
                pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00;
        }
        rc = ls30VFOpen(*pDatExtrRec->derDataFile_ls00, ErrText);
        pDatExtrRec->derDataFile_ls00->vfFileType_ls00 = pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00;
     }
     //*
     //*   long file
     //*
     if (errOK_els00 == rc)
     {
        rc = ls27_InitAndOpenLongFiles(&pTransformationCmd->trColSpec_ls00,
                                        pDatExtrRec->pderColumnData_ls00,
                                        pDatExtrRec->derDataFile_ls00,
                                        pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00,
                                        ErrText);
     }
     return rc ;
}

/*
  -----------------------------------------------------------------------------
  function: ls27_ParseSelectStmt
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_ParseSelectStmt(tls00_DBInfo             *pDBInfo,
                     tls00_TransformationCmd  *pTransformationCmd,
                     tls00_DextractRec        *pDatExtrRec,
                     tsp00_Addr                pszErrText,
                     bool                      bNoMessage)
{
    ROUTINE_DBG_MEO00 ("ls27_ParseSelectStmt");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    tsp00_Int4       rc      = errOK_els00;

    //* create cursor name: tempLOA_<DATUM><ZEIT>
    pDatExtrRec->derRTableName_ls00 = new char[sizeof(tsp00_KnlIdentifierc)];

    // result table name : LOA_<DATE><TIME>
    ls12CreateResultTabName(pDatExtrRec->derRTableName_ls00, sizeof(tsp00_KnlIdentifierc));

    tsp00_Addr pszStmt        = NULL;
    size_t     lSelectStmtLen = 0;
    if (NULL != pTransformationCmd->trSelOrdStr_ls00) {
        lSelectStmtLen = _tcslen(pTransformationCmd->trSelOrdStr_ls00);
    }

    size_t     lCursorStmtLen = KNL_IDENTIFIER_SIZE + lSelectStmtLen + 64;
    
    SAPDB_Bool trRestore_ls00 = false;
    if ((pTransformationCmd->trPartSpec_ls00.psPartType_ls00      == ptTable_els00 &&
         pTransformationCmd->trTable_ls00.tsTabName_ls00.length() != 0 )||
         pTransformationCmd->trRestore_ls00 != resUnknown_els00 )
         trRestore_ls00 = true;
    if (trRestore_ls00 == false && lSelectStmtLen != 0 )
    {
        pszStmt = new _TCHAR[lCursorStmtLen];

        sp77sprintf(pszStmt, STATIC_CAST(tsp00_Int4, lCursorStmtLen), CURSOR_FOR_SEL_LS27,
                                                                      pDatExtrRec->derRTableName_ls00,
                                                                      pTransformationCmd->trSelOrdStr_ls00);
    }
    else
    {
        if ( pTransformationCmd->trTable_ls00.tsTabSchema_ls00.length() != 0 &&
             pTransformationCmd->trTable_ls00.tsTabName_ls00.length()   != 0 )
        {
             lCursorStmtLen +=  _tcslen(pTransformationCmd->trTable_ls00.tsTabSchema_ls00);
             lCursorStmtLen +=  _tcslen(pTransformationCmd->trTable_ls00.tsTabName_ls00);
        }
        else 
        if ( pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length() != 0 &&
             pTransformationCmd->trPartSpec_ls00.psTabName_ls00  .length() != 0 )
        {
             lCursorStmtLen +=  _tcslen(pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00);
             lCursorStmtLen +=  _tcslen(pTransformationCmd->trPartSpec_ls00.psTabName_ls00);
        }

        pszStmt = new _TCHAR[lCursorStmtLen];

        if (lSelectStmtLen > 0)
        {
            if ( pTransformationCmd->trTable_ls00.tsTabSchema_ls00.length() != 0 &&
                 pTransformationCmd->trTable_ls00.tsTabName_ls00.length()   != 0 )
            {
                 sp77sprintf(pszStmt, STATIC_CAST(tsp00_Int4, lCursorStmtLen),
                                      CURSOR_FOR_SEL_STAR_ORDER_LS27, pDatExtrRec->derRTableName_ls00,
                                      pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp(),
                                      pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp(),
                                      pTransformationCmd->trSelOrdStr_ls00);
            }
            else 
            if ( pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length() != 0 &&
                 pTransformationCmd->trPartSpec_ls00.psTabName_ls00  .length() != 0 )
            {
                 sp77sprintf(pszStmt, STATIC_CAST(tsp00_Int4, lCursorStmtLen),
                                      CURSOR_FOR_SEL_STAR_ORDER_LS27, pDatExtrRec->derRTableName_ls00,
                                      pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp(),
                                      pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                      pTransformationCmd->trSelOrdStr_ls00);
            }
        }
        else
        {
            if ( pTransformationCmd->trTable_ls00.tsTabSchema_ls00.length() != 0 &&
                 pTransformationCmd->trTable_ls00.tsTabName_ls00.length()   != 0 )
            {
                 sp77sprintf(pszStmt, STATIC_CAST(tsp00_Int4, lCursorStmtLen),
                                     CURSOR_FOR_SEL_STAR_LS27,
                                     pDatExtrRec->derRTableName_ls00,
                                     pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp(),
                                     pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp()
                                     );
            }
            else 
            if ( pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length() != 0 &&
                 pTransformationCmd->trPartSpec_ls00.psTabName_ls00  .length() != 0 )
            {
                sp77sprintf(pszStmt, STATIC_CAST(tsp00_Int4, lCursorStmtLen),
                                     CURSOR_FOR_SEL_STAR_LS27,
                                     pDatExtrRec->derRTableName_ls00,
                                     pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp(),
                                     pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp()
                                     );
            }
        }
    }

    tls00_TableDescription &TableInfo = pDatExtrRec->derTableInfo_ls00;
    _TCHAR                  szErr[MAX_REPLY_LENGTH_LS00];
    szErr[0] = 0;
    //*
    //*     Open cursor
    //*
    rc = ls12DeclareCursor(*pDBInfo, NULL , pszStmt, szErr);
    if ( errOK_els00 == rc )
    {
        // meta data header
        pDatExtrRec->derTabEmpty_ls00 = false;

        // short info part
        rc = ls04GetShortInfo(pDBInfo->dbiPktRcvList_ls00[0], csp_max_fields,
                              TableInfo.tdParamArray_ls00,
                              TableInfo.tdFieldCount_ls00);
        if (errOK_els00 == rc)
        {
            tsp00_Int2 fieldcount = 0;

            // column names part
            rc = ls04GetColumnNames(pDBInfo->dbiPktRcvList_ls00[0],
                                    csp_max_fields,
                                    TableInfo.tdColName_ls00,
                                    fieldcount);
            if ( fieldcount != TableInfo.tdFieldCount_ls00)
            {
                rc = errUnknownPart_els98;
            }
        }
        // result count part
        if (errOK_els00 == rc)
        {
            rc = ls04GetResultCount(pDBInfo->dbiPktRcvList_ls00[0], NULL, pDatExtrRec->derResultCount_ls00);
        }
        if (errOK_els00 != rc)
        {
            pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc));
        }
        TRACE_TABPARAM_MLS99("ls27_ParseSelectStmt", &pDatExtrRec->derTableInfo_ls00);

        // meta data trailer
    }
    else
    {
        //*
        //*     Declaring cursor ended on error
        //* Check what exactly the error is - return code 100 means 'No row found' = table is empty
        //* which is in fact not treated as an error
        //*
        if (dbRowNotFound_els00 == rc || ( dbRowNotFound_els00 == rc  &&
            NULL != pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 ) )
        {
            // meta data header
            if (dbRowNotFound_els00 == rc)
                pDatExtrRec->derTabEmpty_ls00 = true;

            // meta data trailer
            tsp00_Int4  lErrPos                = 0;
            tsp00_Addr  pszDescribeResultTable = NULL;
            tsp00_Int2  iFieldCount            = 0;

            //*
            //*     Get information on empty table anyway
            //*
            pszDescribeResultTable = new _TCHAR[lCursorStmtLen];
            szErr[0]               = 0;

            sp77sprintf(pszDescribeResultTable, STATIC_CAST(tsp00_Int4, lCursorStmtLen), DESCRIBE_CURSOR_FOR_SEL_STAR_LS27,
                                                                                         pDatExtrRec->derRTableName_ls00 );

            rc = ls13ExecIntSQLStmt(pDBInfo, 0, pszDescribeResultTable, szErr, lErrPos);
            if (errOK_els00 != rc)
            {
                pMsgObj->ls98Msg(layCursor_els98, pszErrText, errInternal_els98, __FILE__, __LINE__, rc );
                rc = errInternal_els98;
            }
            else
            {
                // short info part
                rc = ls04GetShortInfo(pDBInfo->dbiPktRcvList_ls00[0], csp_max_fields,
                                      TableInfo.tdParamArray_ls00,
                                      TableInfo.tdFieldCount_ls00);
                if (errOK_els00 != rc )
                {
                    pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, errInternal_els98, __FILE__, __LINE__, rc );
                    rc = errInternal_els98;
                }
                else
                {

                    tsp00_Int2 fieldcount = 0;

                    // column names part
                    rc = ls04GetColumnNames(pDBInfo->dbiPktRcvList_ls00[0],
                                            csp_max_fields,
                                            TableInfo.tdColName_ls00,
                                            fieldcount);
                    if ( fieldcount != TableInfo.tdFieldCount_ls00)
                    {
                        rc = errUnknownPart_els98;
                    }
                }

                if (NULL != pszDescribeResultTable)
                {
                    delete [] pszDescribeResultTable;
                    pszDescribeResultTable = NULL;
                }
                TRACE_TABPARAM_MLS99("ls27_ParseSelectStmt", &pDatExtrRec->derTableInfo_ls00);
            }
        }   // if (dbRowNotFound_els00 == rc)
        else
        {
            if (false == bNoMessage)        // we do not need the message but only the error code
            {
                pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, errSimpleSQL_els98, szErr);
                rc = errInternSQL_els98;
            }
        }
    }

    if (NULL != pszStmt)
    {
        delete [] pszStmt;
        pszStmt = NULL;
    }

    return rc;
}
// ls27_ParseSelectStmt()

/*
  -----------------------------------------------------------------------------
  function:     ls27_TransformTableRows
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_TransformTableRows(tls00_DBInfo             *DBInfo,
                        tsp00_Int4                RowCount,
                        tls00_DextractRec        *pDatExtrRec,
                        tls00_TransformationCmd  *pTransformationCmd,
                        tls00_Buffer2            *pConvertBuffer,
                        tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_TransformTableRows");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();
    tsp00_Longint    rc     = errOK_els00;

    // datbase info
    bool                    bUnicodeOI      = ls00UseUnicodeOI(DBInfo);

    // table info
    tls00_TableDescription  &TableInfo      = pDatExtrRec->derTableInfo_ls00;
    tls00_MultColSpec       &MultCol        = pTransformationCmd->trColSpec_ls00;
    tls00_Column            *pColumn        = NULL;
    tls00_ColumnData        *pColumnData    = pDatExtrRec->pderColumnData_ls00;

    // file info
    tls00_FileFormat        &DataFileFormat = pTransformationCmd->trDataFile_ls00.fsFormat_ls00;

    tsp00_Int4              lErrNum         = pDatExtrRec->derErrNumInRecord_ls00;
    tsp00_Int4              BufOffset       = 0;

    // muss nach jedem fetch wieder initialisiert werden
    pDatExtrRec->derUseGetvalPckt_ls00 = false;

    // data source
    tls00_DataSource *pDataSource = &pDatExtrRec->derDataSource_ls00;
    tsp00_Addr        pszI_Buffer = STATIC_CAST(char*, pDataSource->dasInputBuffer.pBufPtr_ls00);

    // data sink
    tls27_DataSink *pDataSink    = &pDatExtrRec->derDataSink_ls00;
    tsp00_Addr      pszO_Buffer  = STATIC_CAST(_TCHAR*, pDataSink->dskMetaLine_ls27.pBufPtr_ls00);

    tsp00_Longint iRow;
    tsp00_Longint jColumn;

    for ( iRow = 0; iRow < RowCount && (errOK_els00 == rc) ; ++iRow )
    {
        //*
        //*   transform single row
        //*

        // Get data from order interface communication packet
        rc = ls18BindDataToCol(&pszI_Buffer[BufOffset],
                               pDatExtrRec->derRecordLen_ls00,
                               &TableInfo,
                               pColumnData,
                               pTransformationCmd->trColSpec_ls00.mcsCount_ls00,
                               bUnicodeOI,
                               pszErrText);

        // Initialize buffer pointer
        pDataSink->dskOutLine_ls27.lBufPos_ls00 = 0;

        if (true == DataFileFormat.ffoCompact_ls00)
        {
            pDataSink->dskMetaLine_ls27.lBufPos_ls00 = 0;
        }

        //*
        //* Transform single row column data in tls00_columndata structure
        //*
        if (errOK_els00 == rc)
        {
            rc = ls27_TransformRowColumns(DBInfo, pDatExtrRec, pTransformationCmd, pConvertBuffer, pszErrText);
        }

        //*
        //* Transform single row column longs
        //* This is handled different depending on the format used for the data file
        //* 1. Format RECORDS
        //*    We need to write the data line first to the file and subsequently write the
        //*    LONG values to the data file. Both - data and long data - are stored in the
        //*    same file. UTF8 encoded data files are not allowed because it is impossible
        //*    to compute the length of LONG data before having it converted either from
        //*    ASCII or UCS2.
        //* 2. Format COMPRESSED or FORMATTED
        //*    We need to store the LONGs first into separate LONG files to get the right
        //*    length back. With this information the data is updated (offsets for LONGs in
        //*    data file). Then the data is written to data file.
        //*
        rc = ls27_GetLongLength(DBInfo, pDatExtrRec, pTransformationCmd, pszErrText);

        if (errOK_els00 == rc)
        {
            if (true == DataFileFormat.ffoCompact_ls00)                 // RECORDS
            {
                pDataSink->dskOutLine_ls27.lBufPos_ls00 =
                    ls18PutCompactValue(&pDataSink->dskMetaLine_ls27,
                                        STATIC_CAST(_TCHAR*, pDataSink->dskOutLine_ls27.pBufPtr_ls00),
                                        pDataSink->dskOutLine_ls27.lBufLen_ls00,
                                        pColumnData,
                                        &pTransformationCmd->trColSpec_ls00);
                rc = ls30VFWriteLn(*pDatExtrRec->derDataFile_ls00,
                                   pszO_Buffer,
                                   pDataSink->dskOutLine_ls27.lBufPos_ls00 + pDataSink->dskMetaLine_ls27.lBufPos_ls00,
                                   pszErrText);
                if (errOK_els00 != rc)
                {
                    break;
                }
                else
                {
                    //*
                    //*    write longs to data file
                    //*
                    if (0 != pDatExtrRec->derLongInfo_ls00.dliCntNNLongs_ls00)
                    {
                        rc = ls08ExtractLongs(DBInfo,
                                              pszI_Buffer,
                                              &pTransformationCmd->trColSpec_ls00,
                                              pColumnData,
                                              &pDatExtrRec->derLongInfo_ls00,
                                              pDatExtrRec->derLongDescArray_ls00,
                                              pDatExtrRec->derUseGetvalPckt_ls00,
                                              pConvertBuffer,
                                              pszErrText);
                        if (errOK_els00 != rc)
                        {
                            pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                            pColumn->colName_ls00.asCharp());
                            ++pDatExtrRec->derErrNumInRecord_ls00;
                        }
                    }
                }
            }   // if (true == DataFileFormat.ffoCompact_ls00)
            else
            {
                //*
                //*    Write longs to separate long files and edit offset info for data file
                //*
                if (errOK_els00 == rc)
                {
                    if (0 != pDatExtrRec->derLongInfo_ls00.dliCntNNLongs_ls00)
                    {
                        rc = ls08ExtractLongs(DBInfo,
                                              pszI_Buffer,
                                              &pTransformationCmd->trColSpec_ls00,
                                              pColumnData,
                                              &pDatExtrRec->derLongInfo_ls00,
                                              pDatExtrRec->derLongDescArray_ls00,
                                              pDatExtrRec->derUseGetvalPckt_ls00,
                                              pConvertBuffer,
                                              pszErrText);
                        if (errOK_els00 != rc)
                        {
                            pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                            pColumn->colName_ls00.asCharp());
                            ++pDatExtrRec->derErrNumInRecord_ls00;
                        }
                        else
                        {
                            rc = ls27_EditLongOffsetForOutput(pColumnData,
                                                              &pDatExtrRec->derLongInfo_ls00,
                                                              &MultCol,
                                                              bUnicodeOI,
                                                              pDatExtrRec->derSwapKind_ls00,
                                                              &DBInfo->dbiSession_ls00.siCodePage_ls00,
                                                              pConvertBuffer,
                                                              pszErrText);
                            if (errOK_els00 != rc)
                            {
                                ++pDatExtrRec->derErrNumInRecord_ls00;
                                rc = errOK_els00;
                            }
                        }
                    }   // end if (0 != pDatExtrRec->derLongInfo_ls00.dliCntNNLongs_ls00)
                }

                //*
                //* Write data to data file
                //*
                if (errOK_els00 == rc)
                {
                    if (true == DataFileFormat.ffoFormatted_ls00)       // FORMATTED
                    {
                        pDataSink->dskOutLine_ls27.lBufPos_ls00 =
                            ls18PutFormattedValue(STATIC_CAST(_TCHAR*, pDataSink->dskOutLine_ls27.pBufPtr_ls00),
                                                  pDataSink->dskOutLine_ls27.lBufLen_ls00,
                                                  pColumnData,
                                                  &pTransformationCmd->trColSpec_ls00);
                    }
                    else if (true == DataFileFormat.ffoCompress_ls00)   // COMPRESSED
                    {
                        pDataSink->dskOutLine_ls27.lBufPos_ls00 =
                            ls18PutUnformattedValue(STATIC_CAST(_TCHAR*, pDataSink->dskOutLine_ls27.pBufPtr_ls00),
                                                    pDataSink->dskOutLine_ls27.lBufLen_ls00,
                                                    pTransformationCmd->trColSpec_ls00.mcsCount_ls00,
                                                    pColumnData,
                                                    pDatExtrRec->pderPattern_ls00);
                    }
                    rc = ls30VFWriteLn(*pDatExtrRec->derDataFile_ls00,
                                    pszO_Buffer,
                                    pDataSink->dskOutLine_ls27.lBufPos_ls00 + pDataSink->dskMetaLine_ls27.lBufPos_ls00,
                                    pszErrText);
                    if (errOK_els00 != rc)
                    {
                        break;
                    }
                }

            }//else if ( false == DataFileFormat.ffoCompact_ls00)

        }//  if (errOK_els00 == rc)

        //  count number of written records, even if errors occurred
        ++pDatExtrRec->derRecordCount_ls00;
        ++pDatExtrRec->derExtractedRows_ls00;

        if (errOK_els00 == rc)
        {
            // Check if errors occured
            if (lErrNum != pDatExtrRec->derErrNumInRecord_ls00)
            {
                tsp00_Int4 lPosition = 1;
                if (true == DataFileFormat.ffoFormatted_ls00)
                {
                    lPosition = STATIC_CAST(tsp00_Int4,
                                            (pDataSink->dskOutLine_ls27.lBufPos_ls00 * (pDatExtrRec->derRecordCount_ls00 - 1)) );
                }
                pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, errAtExtractedRec_els98,
                                                                   pDatExtrRec->derRecordCount_ls00, lPosition);
                --pDatExtrRec->derLRejected_ls00;
                if (pDatExtrRec->derLRejected_ls00 <= 0)
                {
                    // Error has already been reported, error code is set - so simply return
                    break;
                }
                else
                {
                    lErrNum = pDatExtrRec->derErrNumInRecord_ls00;
                }
            }
        }

        BufOffset += pDatExtrRec->derRecordLen_ls00;
        lErrNum    = pDatExtrRec->derErrNumInRecord_ls00;

        // initialize structure pColumnData
        for ( jColumn = 0; jColumn <  MultCol.mcsCount_ls00; ++jColumn)
        {
            pColumnData[jColumn].cdDataLen_ls00    = 0;
            pColumnData[jColumn].cdPtrUsed_ls00    = true;
            pColumnData[jColumn].cdPtrToData_ls00  = NULL;
            pColumnData[jColumn].cdLONGLen_ls00    = 0;
            pColumnData[jColumn].cdIsNILValue_ls00 = false;
        }

    }   // end for ( iRow = 0; iRow < RowCount && (errOK_els00 == rc) ; ++iRow )

    pszO_Buffer = NULL;
    pszI_Buffer = NULL;

    return rc;
}
// ls27_TransformTableRows()


/*
  -----------------------------------------------------------------------------
  function:     ls27_TransformRowColumns
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_TransformRowColumns(tls00_DBInfo      *DBInfo,
                         tls00_DextractRec *pDatExtrRec,
                         tls00_TransformationCmd  *pTransformationCmd,
                         tls00_Buffer2     *pConvertBuffer,
                         tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_TransformRowColumns");

    tsp00_Longint rc = errOK_els00;

    //*
    //*     Initialize LONG descriptions for a single record
    //*
    tsp00_LongDescriptor *pLongDescArray = NULL;
    if (0 != pDatExtrRec->derLongColCnt_ls00)
    {
        pDatExtrRec->derLongInfo_ls00.dliCntNNLongs_ls00      = 0;
        pDatExtrRec->derLongInfo_ls00.dliFinishedNNLongs_ls00 = 0;
    }

    tls00_MultColSpec *pMultColumns   = &pTransformationCmd->trColSpec_ls00;  // definitions of field properties in data file
    tls00_FileFormat  *DataFileFormat = &pTransformationCmd->trDataFile_ls00.fsFormat_ls00;
    tls00_ColumnData  *pColumnData    = pDatExtrRec->pderColumnData_ls00;

    tls00_ParamInfo *pParamDesc  = NULL;        // parameter description structure (from db)
    tls00_Column    *pColumnDesc = NULL;        // column description structure    (from user)
    bool             bFirstLong  = true;
    bool             bUnicodeOI  = ls00UseUnicodeOI(DBInfo);

    // This code is either
    // ASCII (the db is a pure ASCII db) or
    // some kind of UCS2 (the db is UNICODE which means UCS2)
    tls00_CodeType   Trim_CodeType = (true != bUnicodeOI) ? ctAscii_els00
                                                          : (ls07Swapped(NULL) ? ctUCS2Swapped_els00 : ctUCS2_els00);

    // Bei dieser schleife ist mehreres zu beachten. Zum einen existiert fuer jeden index in
    // multcol ein entsprechender index in pDatExtrRec->derMapIndex_ls00, der den multcol-index auf
    // den entsprechenden index von tableinfo abbildet. D.h. wir arbeiten bei TableInfo nicht
    // mit dem laufindex der schleife, sondern mit dem mapindex[von laufindex].
    // Ausserdem haben wir nur soviele pRawData-variablen angelegt, wie tableinfo member (tdFieldCount)
    // hat. Da aber in der spaltenausgabeliste auch literale eingestreut sein koennen, die
    // nicht in Tableinfo aufgenommen werden, muessen wir den index von TableInfo immer um die
    // bereits gefundene anzahl der Literale bereinigen (deshalb pRawData[i-j]).

    // Als loopcount kann der zaehler in pMultColumns gewaehlt werden, da diese struktur
    // IMMER gefuellt wird, auch wenn keine spaltenliste angegeben wurde im kommando.

    // l ist der laufindex fuer longdescriptoren

    for (tsp00_Int4 i = 0, l = 0; i < pMultColumns->mcsCount_ls00; ++i)
    {
        pColumnDesc  = pMultColumns->mcsColumn_ls00[i];
        pParamDesc = pDatExtrRec->derTableInfo_ls00.tdParamArray_ls00[pColumnData[i].cdPosInStmt_ls00];


        // first check for literal: if it is a literal simple assign the pointer to
        // the column name which is in this case the literal
        if (true == pColumnDesc->colIsLit_ls00)
        {
            pColumnData[i].cdPtrToData_ls00 = pColumnDesc->colName_ls00.asCharp();
            pColumnData[i].cdDataLen_ls00   = pColumnDesc->colName_ls00.length();
        }
        else
        {
            if (INSERT_NULL == pColumnData[i].cdDataLen_ls00)
            {
                if (0 == pColumnDesc->colAssign_ls00.fasAsgnLen_ls00)
                {
                    pColumnData[i].cdPtrToData_ls00 = DataFileFormat->ffoNullSpec_ls00;
                    pColumnData[i].cdDataLen_ls00   = DataFileFormat->ffoNSpecLen_ls00;
                }
                else
                {
                    pColumnData[i].cdPtrToData_ls00  = pColumnDesc->colAssign_ls00.fasAsgnVal_ls00;
                    pColumnData[i].cdDataLen_ls00    = pColumnDesc->colAssign_ls00.fasAsgnLen_ls00;
                }
                pColumnData[i].cdIsNILValue_ls00 = true;
                pColumnData[i].cdPtrUsed_ls00    = true;
            }
            else
            {
                if (1 == szMap_NumberDataType_ls00[pColumnData[i].cdSQLDataType_ls00])
                {
                    //*
                    //*     Numerical data type
                    //*     If successfully converted the numerical string is trimmed left
                    //*
                    rc = ls25ExtractNumbers(pColumnData[i].cdPtrToData_ls00,
                                            pColumnData[i].cdDataBuffer_ls00,
                                            pColumnData[i].cdDataLen_ls00,
                                            pColumnDesc,
                                            &pParamDesc->piBasicInfo_ls00,
                                            DataFileFormat,
                                            pDatExtrRec->derSwapKind_ls00,
                                            pDatExtrRec->derExtLenFrac_ls00[i]);
                    if (errOK_els00 != rc)
                    {
                        LoaderMessages::Instance()->ls98Msg(layDataExtract_els98,
                                                             pszErrText,
                                                             errNumberTruncAtExtract_els98,
                                                             pColumnDesc->colName_ls00.asCharp());
                        ++pDatExtrRec->derErrNumInRecord_ls00;
                        rc = errOK_els00;
                    }
                    else
                    {
                        pColumnData[i].cdPtrUsed_ls00 = false;
                    }
                }
                else if (1 == szMap_SpecConstDataType_ls00[pColumnData[i].cdSQLDataType_ls00])
                {
                    //*
                    //*     Special data type like DATE, TIME, TIMESTAMP and BOOLEAN
                    //*
                    if (dboolean == pColumnData[i].cdSQLDataType_ls00)
                    {
                        if (0 == (pColumnData[i].cdPtrToData_ls00)[0])
                        {
                            pColumnData[i].cdPtrToData_ls00  = DataFileFormat->ffoFalseVal_ls00;
                            pColumnData[i].cdDataLen_ls00    = DataFileFormat->ffoFValLen_ls00;
                        }
                        else
                        {
                            pColumnData[i].cdPtrToData_ls00  = DataFileFormat->ffoTrueVal_ls00;
                            pColumnData[i].cdDataLen_ls00    = DataFileFormat->ffoTValLen_ls00;
                        }
                    }
                    else
                    {
                        if (false == pColumnData[i].cdPtrUsed_ls00)
                        {
                            // let the pointer point to the buffer of it's own element
                            pColumnData[i].cdPtrToData_ls00 = pColumnData[i].cdDataBuffer_ls00;
                        }
                        ls24ExtractDateTime(pColumnData[i].cdPtrToData_ls00,
                                            pColumnData[i].cdDataBuffer_ls00,
                                            pColumnData[i].cdDataLen_ls00,
                                            pColumnData[i].cdSQLDataType_ls00,
                                            DataFileFormat);

                        pColumnData[i].cdPtrUsed_ls00 = false;
                    }
                }
                else if (1 == szMap_LongDataType_ls00[pColumnData[i].cdSQLDataType_ls00])
                {
                    //*
                    //*     LONG data type
                    //*
                    // We simply copy the longdescriptor to a long descriptor array
                    if (INSERT_NULL  != pColumnData[i].cdDataLen_ls00)
                    {
                        SAPDB_memcpy(&pDatExtrRec->derLongDescArray_ls00[pDatExtrRec->derLongInfo_ls00.dliCntNNLongs_ls00],
                                     pColumnData[i].cdPtrToData_ls00,
                                     LongDescSize_ls00);
                        ++pDatExtrRec->derLongInfo_ls00.dliCntNNLongs_ls00;
                        pDatExtrRec->derLongInfo_ls00.dliNNLongs_ls00[l] = i;
                        ++l;
                    }
                }

                // This handles blobs, too. In this case simply the output format is ALWAYS set
                // to hex for a blob column and the write functions treat them right.

            }   // end if (INSERT_NULL == pColumnData[i].cdDataLen_ls00)

        }   // endif (true == pColumnDesc->colIsLit_ls00)


        //*
        //*     Special conversion in case of char output if not a LONG value
        //*
        //* Do this special conversion only if
        //*  - the external type is CHAR
        //*  and
        //*    - the data is not of db data type LONG
        //*    or
        //*    - the data is of db data type LONG and is the NULL value
        //*
        //* LONG value extraction is handled in ls08ExtractLongs
        //*
        if ( (ioChar_els00 == pColumnData[i].cdExtDataType_ls00) &&
             ( (true == pColumnData[i].cdIsNILValue_ls00) ||
               (0    == szMap_LongDataType_ls00[pColumnData[i].cdSQLDataType_ls00]) ) )
        {
            rc = ls27_SpecialConversion(&pColumnData[i],
                                        bUnicodeOI,
                                        &DBInfo->dbiSession_ls00.siCodePage_ls00,
                                        pConvertBuffer,
                                        pszErrText);
        }

    }   // end for (i = 0; i < pMultColumns->mcsCount_ls00; ++i)

    return rc;
}
// ls27_TransformRowColumns()

/*
  -----------------------------------------------------------------------------
  function:     ls27_SpecialConversion
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_SpecialConversion(tls00_ColumnData    *pSingleColData,
                       bool                 bUnicodeOI,
                       tsp81_CodePage      *pCodePage,
                       tls00_Buffer2       *pConvertBuffer,
                       tsp00_Addr           pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_SpecialConversion");

    tsp00_Longint rc = errOK_els00;

    tsp00_DataType DataType = pSingleColData->cdSQLDataType_ls00;
    //*
    //*     No special conversion for binary data
    //*
    if (ioChar_els00 != pSingleColData->cdExtDataType_ls00)
    {
        return rc;
    }


    tsp00_Addr pDataPtr = pSingleColData->cdPtrToData_ls00;
    if (false == pSingleColData->cdPtrUsed_ls00)
    {
        pDataPtr = pSingleColData->cdDataBuffer_ls00;
    }

    // in case of unicode order interface (oi) (we use unicode oi only if the database
    // is unicode defined itself) char data is stored as unicode data. If the data will be
    // written to file also as unicode we don't transform it here to any other encoding. Therefor
    // the trim function must handle unicode, too.

    //*
    //*     Trim character string right; function adapts only the length
    //* Coming here we know there is surely some data; so if the data is trimmed
    //* to length 0 we should recover the old value - the data consists entirely
    //* of trim characters
    //*
    //* Define if it is necessary to trim unicode characters or only ascii characters
    //*
    tls00_CodeType CodeType = ctAscii_els00;
    if ( (true == bUnicodeOI) && (1 == szMap_CharDataType_ls00[DataType]) )
    {
        CodeType = ls07Swapped(NULL) ? ctUCS2Swapped_els00 : ctUCS2_els00;
    }
    char szTrim = ls07GetFillByte(pSingleColData->cdSQLDataType_ls00);

    ls05StrTrimRight(pDataPtr, pSingleColData->cdDataLen_ls00, szTrim, CodeType);

    //*
    //*     Trim numerical strings left, too
    //*
    //* In case of numerical values is always the buffer of tls00_ColumnData structure used
    if (1 == szMap_NumberDataType_ls00[DataType])
    {
        ls05StrTrimLeftEx(pDataPtr, pSingleColData->cdDataLen_ls00);
    }


    // fuer blobs ist immer das flag der hexdarstellung gesetzt! deshalb hier keine extrabehandlung

    // jetzt muss noch eine umwandlung in die hexadezimale und/oder unicode darstellung erfolgen,
    // da das noch nicht fuer alle datentypen gemacht wurde: speziell nicht fuer zeitstempel..., zahlen
    // und boolsche werte sowie fuer literale

    //*
    //*     Special transformation to right external encoding
    //*
    //* The used conversion buffer is large enough to hold the data
    //*
    tsp00_Longint  destlen   = 0;
    tsp00_Longuint lDestlenU = 0;

    // Conversion between different encodings (ASCII, UCS2 or UTF8) is ONLY needed (in any case) if
    //  either
    //   - table column data type is not BLOB (i.e. not dchb, dstrb, dstrdb, dlongb, dlongdb, dvarcharb)
    //  or
    //   - table column data type is BLOB but is a LONG BLOB;
    //
    // some additional blob type handling is done in the next transformation step
    //
    if ( (0 == szMap_BlobDataType_ls00[DataType]) ||
         ( (1 == szMap_BlobDataType_ls00[DataType]) && (1 == szMap_LongDataType_ls00[DataType]) ) )
    {
        switch (pSingleColData->cdExtEncoding_ls00)
        {
            case ctAscii_els00:
            {
                // Conversion needed if
                // - kernel is UCS2 and
                //   - data type is of type char and
                //   - data is not NULL (NULL value representation is in ASCII)
                if ( (true == bUnicodeOI) &&
                     (1 == szMap_CharDataType_ls00[DataType]) && (false == pSingleColData->cdIsNILValue_ls00))
                {
                    destlen = pSingleColData->cdDataBufferLen_ls00;

                    SAPDB_memcpy(pConvertBuffer->pBufPtr_ls00, pDataPtr, pSingleColData->cdDataLen_ls00);

                    rc  = ls05ConvertFromUCS2Simple(REINTERPRET_CAST(unsigned char*, pSingleColData->cdDataBuffer_ls00),
                                                    STATIC_CAST(unsigned char*, pConvertBuffer->pBufPtr_ls00),
                                                    destlen,
                                                    pSingleColData->cdDataLen_ls00,
                                                    ctAscii_els00,
                                                    pszErrText);
                    if (errOK_els00 == rc)
                    {
                        pSingleColData->cdDataLen_ls00 = STATIC_CAST(tsp00_Longint, destlen);

                        pDataPtr                       = pSingleColData->cdDataBuffer_ls00;
                        pSingleColData->cdPtrUsed_ls00 = false;
                    }
                }
                break;

            }
            case ctUCS2_els00:              // ATTENTION: triple fall through
            case ctUCS2Swapped_els00:
            case ctUCS2Native_els00:
            {
                // Conversion needed if
                // - the database is ASCII or
                // - the database is UCS2 and
                //    - the data is not of type char or
                //    - the data is NULL (NULL value representation is in ASCII)
                bool bConversionNecessary = true;
                if (true == bUnicodeOI)
                {
                    if ( (1 == szMap_CharDataType_ls00[DataType]) && (false == pSingleColData->cdIsNILValue_ls00) )
                    {
                        bConversionNecessary = false;
                    }
                }

                if (true == bConversionNecessary)
                {
                    // transform
                    SAPDB_memcpy(pConvertBuffer->pBufPtr_ls00, pDataPtr, pSingleColData->cdDataLen_ls00);

                    lDestlenU = pSingleColData->cdDataBufferLen_ls00;

                    rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, pSingleColData->cdDataBuffer_ls00),
                                                 lDestlenU,
                                                 STATIC_CAST(unsigned char*, pConvertBuffer->pBufPtr_ls00),
                                                 pSingleColData->cdDataLen_ls00,
                                                 ctAscii_els00,
                                                 pszErrText,
                                                 pCodePage);
                    if (errOK_els00 == rc)
                    {
                        pSingleColData->cdDataLen_ls00 = STATIC_CAST(tsp00_Longint, lDestlenU);

                        pDataPtr                       = pSingleColData->cdDataBuffer_ls00;
                        pSingleColData->cdPtrUsed_ls00 = false;
                    }
                }
                break;
            }
            case ctUTF8_els00:
            {
                tls00_CodeType CodeType = ctAscii_els00;

                // Char data is held up to this point in the code type of the database order interface;
                // All other data (date, time, timestamp, numerical, boolean) has to be converted from ASCII
                //  because it is processed by the repman as ASCII data;
                // Null value representation is still handled as ASCII data and must be converted here
                if (true == bUnicodeOI)
                {
                    CodeType = ctUCS2Native_els00;
                    if ( (true == pSingleColData->cdIsNILValue_ls00) || (0 == szMap_CharDataType_ls00[DataType]) )
                    {
                        CodeType = ctAscii_els00;
                    }
                }

                Tools_DynamicUTF8String ConvertedData;
                SAPDB_memcpy(pConvertBuffer->pBufPtr_ls00, pDataPtr, pSingleColData->cdDataLen_ls00);

                rc = ls05ConvertToUTF8(ConvertedData,
                                       STATIC_CAST(unsigned char*, pConvertBuffer->pBufPtr_ls00),
                                       pSingleColData->cdDataLen_ls00,
                                       CodeType,
                                       pszErrText);
                if (errOK_els00 == rc)
                {
                    pSingleColData->cdDataLen_ls00 = STATIC_CAST(tsp00_Longint, ConvertedData.BasisSize());
                    SAPDB_memcpy(pSingleColData->cdDataBuffer_ls00, ConvertedData.StrPtr(), pSingleColData->cdDataLen_ls00);

                    pDataPtr                       = pSingleColData->cdDataBuffer_ls00;
                    pSingleColData->cdPtrUsed_ls00 = false;
                }
                break;
            }
        }   // end switch (pSingleColData->cdExtEncoding_ls00)
    }   // end if ( (0 == szMap_BlobDataType_ls00[DataType]) || ...

    //*
    //*     Conversion to hex representation excluding LONG values
    //*
    if ( (errOK_els00 == rc) &&
         (true == pSingleColData->cdExtHexRepresentation_ls00) &&
         (0 == szMap_LongDataType_ls00[DataType]) )
    {
        //*
        //*     First conversion from byte value to hex representation
        //*
        SAPDB_memcpy(pConvertBuffer->pBufPtr_ls00, pDataPtr, pSingleColData->cdDataLen_ls00);

        ls07ConvertByteToHexEx(STATIC_CAST(char*, pConvertBuffer->pBufPtr_ls00),
                               STATIC_CAST(tsp00_Int4, pSingleColData->cdDataLen_ls00),
                               pSingleColData->cdDataBuffer_ls00);

        pSingleColData->cdDataLen_ls00 = 2*pSingleColData->cdDataLen_ls00;

        //*
        //*     Second conversion
        //* In case of external encoding type UCS2 further conversion of the hex
        //* representation to UCS2 hex is necessary;
        //* In case of external encoding type UTF8 no further conversion needed,
        //*  readable hex representation may only be US7ASCII which is already UTF8
        //*
        if ( true == ls00IsUCS2(&pSingleColData->cdExtEncoding_ls00) )
        {
            SAPDB_memcpy(pConvertBuffer->pBufPtr_ls00, pSingleColData->cdDataBuffer_ls00, pSingleColData->cdDataLen_ls00);

            lDestlenU = pSingleColData->cdDataBufferLen_ls00;

            rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, pSingleColData->cdDataBuffer_ls00),
                                         lDestlenU,
                                         STATIC_CAST(unsigned char*, pConvertBuffer->pBufPtr_ls00),
                                         pSingleColData->cdDataLen_ls00,
                                         ctAscii_els00,
                                         pszErrText,
                                         pCodePage);
            if (errOK_els00 == rc)
            {
                pSingleColData->cdDataLen_ls00 = STATIC_CAST(tsp00_Longint, lDestlenU);
            }
        }
        pSingleColData->cdPtrUsed_ls00 = false;     // Reset flag of pointer use
    }

    return rc;
}
// ls27_SpecialConversion()

/*
  -----------------------------------------------------------------------------
  function:     ls27_GetLongLength
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_GetLongLength(tls00_DBInfo      *pDBInfo,
                   tls00_DextractRec *pDatExtrRec,
                   tls00_TransformationCmd  *pTransformationCmd,
                   tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_GetLongLength");

    //*
    //*     This module is only called for data stream format RECORDS (COMPACT)
    //*     Coming to this point no long value of the extracted database record
    //*     has been processed yet.
    //*
    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // Error reporting object
    tsp00_Longint    rc      = errOK_els00;


    tls00_LongExecInfo   *pLongInfoStruct  = &pDatExtrRec->derLongInfo_ls00;
    tsp00_Int4            lLongCnt         = pLongInfoStruct->dliCntNNLongs_ls00; // Count of longs that must be processed
    tsp00_LongDescriptor *pLongD           = NULL;
    tls00_Column         *pCol             = NULL;
    tls00_ColumnData     *pColData         = NULL;  // Contains data and meta data for a certain column in record

    tsp00_Int4            lCompleteLongCnt = 0;     // Holds the number of longs completely included in the packet
    tsp00_Int4            j                = 0;

    //*
    //* Check wich LONGs did fit completely
    //* for those the descriptor shows values vm_datapart, vm_alldata or vm_lastdata for ld_valmode
    //*
    for (lCompleteLongCnt = 0;
         ( (lCompleteLongCnt < lLongCnt) &&
           (pDatExtrRec->derLongDescArray_ls00[lCompleteLongCnt].ld_valmode() < vm_nodata) );
         ++lCompleteLongCnt)
             ;

    //*
    //* If there are incomplete delivered LONGs we get their length here separately
    //* To do so we request only 2 Bytes of those LONGs - 2 Bytes because of the possibility of
    //* UNICODE databases
    //*
    if (lCompleteLongCnt < lLongCnt)
    {
        tsp1_packet  *SqlSentPckt     = pDBInfo->dbiPktSndList_ls00[1];;
        tsp1_packet  *SqlReceivedPckt = NULL;
        tsp1_segment *SqlSegm         = NULL;
        tsp1_part    *SqlPart         = NULL;

        lLongCnt -= lCompleteLongCnt;

        //*     Prepare getval-packet to get the LONG lengths
        rc = ls04FastInit(SqlSentPckt, &SqlSegm, &SqlPart, pDBInfo);
        ls04SetMsgType(SqlSegm, sp1m_getval);
        SqlPart->sp1p_part_kind().becomes(sp1pk_longdata);

        // Compute maximum number of descriptors fitting into one packet
        tsp00_Int4 lMaxLongDescInPckt = SqlPart->sp1p_buf_size()/(LongDescSize_ls00 + 1);

        // Limit number of LONG descriptors if necessary
        lLongCnt = ( lLongCnt >= lMaxLongDescInPckt) ? lMaxLongDescInPckt : lLongCnt;

        // Write LONG descriptors into packet to send to kernel
        for (j = lCompleteLongCnt; ( (j < (lLongCnt + lCompleteLongCnt)) && (errOK_els00 == rc) ); ++j)
        {
            pLongD = &pDatExtrRec->derLongDescArray_ls00[j];
            pLongD->ld_vallen() = 2;
            pLongD->ld_infoset().addElement(ld_no_close);

            rc = ls04CharToPart(SqlPart, 0);                                    // undef byte
            rc = ls04BufToPart(SqlPart, REINTERPRET_CAST(char*, pLongD), LongDescSize_ls00);
        }

        SqlPart->sp1p_arg_count() = lLongCnt;
        if (errOK_els00 == rc)
        {
            rc = ls04FinishPart(SqlSentPckt, SqlPart);
        }

        //*
        //*     Send descriptors to kernel and get new descriptors which contain the lengths
        //*
        if (errOK_els00 == rc)
        {
            tsp00_Int4 lErrPos = 0;
            rc = ls03ProcessSQLPacket(pDBInfo, 1, lErrPos, pszErrText);
            if (errOK_els00 == rc)
            {
                //  Get descriptors
                rc = ls04GetLONGDescriptor(pDBInfo->dbiPktRcvList_ls00[1],
                                           lLongCnt,
                                           pDatExtrRec->derLongDescArray_ls00,
                                           lCompleteLongCnt);
                if (errOK_els00 != rc)
                {
                    LoaderMessages::Instance()->ls98Msg(layDataExtract_els98, pszErrText,
                                                                               STATIC_CAST(ErrorNumbers, rc));
                }
            }
        }
    }   // end if (lCompleteLongCnt < lLongCntInPckt)


    //*
    //*     Now define the LONG lengths of all LONGs of the record extracted
    //*
    for (tsp00_Int4 k = 0; k < pLongInfoStruct->dliCntNNLongs_ls00; ++k)
    {
        pLongD = &pDatExtrRec->derLongDescArray_ls00[k];

        pCol     = pTransformationCmd->trColSpec_ls00.mcsColumn_ls00[pLongInfoStruct->dliNNLongs_ls00[k]];
        pColData = &pDatExtrRec->pderColumnData_ls00[pLongInfoStruct->dliNNLongs_ls00[k]];

        pCol->colLongInfo.loiLength_ls00 = pLongD->ld_maxlen();

        // Suppose the database encoding type is UNICODE; then all requests of the Loader to the
        // database are send as UCS2 requests and all data is delivered in UCS2
        // Suppose further the LONGs processed have code type ascii (which is allowed! -
        // for instance: create table <table name> (col1 long ascii) ); then
        // the kernel delivers the ascii length of the LONGs but the LONG data in UCS2 -->
        // thus we have to double the length in this case
        if ( ( (dstra == pColData->cdSQLDataType_ls00 || dlonga == pColData->cdSQLDataType_ls00) &&
               (true  == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) &&
               (true == ls00IsUCS2(&pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) ) )
             ||
             ( (dstra == pColData->cdSQLDataType_ls00 || dlonga == pColData->cdSQLDataType_ls00) && false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 && ls00UseUnicodeOI(pDBInfo) ) )
        {
            pCol->colLongInfo.loiLength_ls00 *= 2;
        }

        // For all incomplete sent LONGs we have to reinitialize intern position and value length
        if (vm_data_trunc == pLongD->ld_valmode())
        {
            pLongD->ld_valmode().becomes(vm_datapart);
            pLongD->ld_intern_pos() = 1;
            pLongD->ld_vallen()     = 0;
        }

        // For all LONGSs that didn't make it into the primary packet (thus belonging to the incomplete
        // LONGs) check if the kernel delivered the whole LONG because of a length of less than 3 Bytes.
        // If so manipulate valmode, internpos and vallen, too.
        if ( (k >= lCompleteLongCnt) &&
             (vm_lastdata == pLongD->ld_valmode()) &&
             (pLongD->ld_maxlen() > 0)  && (pLongD->ld_maxlen() < 3) && 
             (pLongD->ld_vallen() >= 0) && (pLongD->ld_vallen() < 3) )
        {
            pLongD->ld_valmode().becomes(vm_datapart);
            pLongD->ld_intern_pos() = 1;
            pLongD->ld_vallen()     = 0;
        }
    }

    return rc;
}
// ls27_GetLongLength()


/*
  -----------------------------------------------------------------------------
  function:     ls27_EditLongOffsetForOutput
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls27_EditLongOffsetForOutput(tls00_ColumnData     *pColumnData,     //destination
                             tls00_LongExecInfo   *pLongExecInfo,   //source
                             tls00_MultColSpec    *pMultCol,
                             bool                  bUnicodeOI,
                             tsp00_SwapKind_Enum   SwapKind,
                             tsp81_CodePage       *pCodePage,
                             tls00_Buffer2        *pConvertBuffer,
                             tsp00_Addr            pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_EditLongOffsetForOutput");

    tsp00_Longint rc = errOK_els00;

    tls00_ParamInfo_LONG *pColLongInfo = NULL;

    for (tsp00_Int4 i = 0, j = 0; ( i < pLongExecInfo->dliCntNNLongs_ls00) && rc == errOK_els00; ++i)
    {
        j = pLongExecInfo->dliNNLongs_ls00[i];
        pColLongInfo = &pMultCol->mcsColumn_ls00[j]->colLongInfo;

        if (true == pColLongInfo->loibFileNumbered_ls00)
        {
            size_t flen = _tcslen(pColLongInfo->loiFileName_ls00);
            size_t spos = flen - pColLongInfo->loiNumberSize_ls00;

            pColumnData[j].cdPtrToData_ls00 = pColLongInfo->loiFileName_ls00;
            pColumnData[j].cdDataLen_ls00   = STATIC_CAST(tsp00_Int4, flen);
        }
        else
        {
            //*
            //*     Start position in LONGFILE
            //*
            SAPDB_Int8 _lspos = 0;
            SAPDB_Int8 _lepos = 0;
            if (0 == pColLongInfo->loiFileStruct_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00)
            {
                _lspos = pColLongInfo->loiFileStruct_ls00->vfFilePos_ls00.lnuRemainder_ls00 - pColumnData[j].cdLONGLen_ls00 + 1;
            }
            else
            {
                _lspos = STATIC_CAST(tsp00_Int8, MAX_INT4_SP00*pColLongInfo->loiFileStruct_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00) +
                          pColLongInfo->loiFileStruct_ls00->vfFilePos_ls00.lnuRemainder_ls00 -
                          pColumnData[j].cdLONGLen_ls00 + 1;
            }
            if (0 != pColLongInfo->loiFileStruct_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00)
            {
                _lepos = STATIC_CAST(tsp00_Int8, MAX_INT4_SP00*pColLongInfo->loiFileStruct_ls00->vfFilePos_ls00.lnu2GB_Blocks_ls00) +
                         pColLongInfo->loiFileStruct_ls00->vfFilePos_ls00.lnuRemainder_ls00;
            }
            else
            {
                _lepos = pColLongInfo->loiFileStruct_ls00->vfFilePos_ls00.lnuRemainder_ls00;
            }


            // At this point we 'initialize' the data buffer every time we come here to be
            // sure the buffer contains afterwards the things we really want
            pColumnData[j].cdDataBuffer_ls00[0] = 0;
            sp77sprintf(pColumnData[j].cdDataBuffer_ls00, STATIC_CAST(tsp00_Int4, pColumnData[j].cdDataBufferLen_ls00),
                                                          "%qd-%qd", _lspos, _lepos);


            pColumnData[j].cdDataLen_ls00 = STATIC_CAST(tsp00_Int4, _tcslen(pColumnData[j].cdDataBuffer_ls00) );
            pColumnData[j].cdPtrUsed_ls00 = false;
        }

        rc = ls27_SpecialConversion(&pColumnData[j], bUnicodeOI, pCodePage, pConvertBuffer, pszErrText);

    }   // end for (tsp00_Int4 i = 0, j = 0; i < pLongExecInfo->dliFinishedNNLongs_ls00; ++i)

    return rc;
}
// ls27_EditLongOffsetForOutput()


/*
  -----------------------------------------------------------------------------
  function:     ls27_AnalyzeLongInfo - local function
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_AnalyzeLongInfo(tls00_TransformationCmd *pTransformationCmd, tls00_DextractRec *pDatExtrRec, tsp00_Addr pszErrText)
{
   ROUTINE_DBG_MEO00 ("ls27_AnalyzeLongInfo");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // Für error reports

    tsp00_Int4 rc = errOK_els00;

    tsp00_Int4 i       = 0;
    tsp00_Int4 j       = 0;
    tsp00_Int4 lErrNum = 0;
    bool       bFound  = false;

    tls00_MultColSpec    &MultCol        = pTransformationCmd->trColSpec_ls00;
    tls00_FileSpec       *myLongFileSpec = NULL;
    tls00_Column         *pColumn        = NULL;
    tls00_ParamInfo_LONG *pLongInfo      = NULL;

    tsp00_Int4           lLongFileCnt   = pTransformationCmd->trColLongSpec_ls00.mfsCount_ls00;
    bool               * bLongIsDone    = NULL;     // Array fuer bereits bearbeitete longinfos des kommandos

    if (0 != lLongFileCnt)
    {
        if (NULL == bLongIsDone)
        {
            bLongIsDone = new bool[lLongFileCnt];
            memset(bLongIsDone, 0, (sizeof(bool) * lLongFileCnt) );
        }
    }

    tls00_ColumnData *pColData = NULL;
    tls00_ParamInfo  *pParamInfo = NULL;

    for (i; i < MultCol.mcsCount_ls00; ++i)
    {
        pColumn = MultCol.mcsColumn_ls00[i];
        pColData = &pDatExtrRec->pderColumnData_ls00[i];

        pParamInfo = pDatExtrRec->derTableInfo_ls00.tdParamArray_ls00[pColData->cdPosInStmt_ls00];
        bFound  = false;

        // Literals may not be of type LONG
        if ( (false == pColumn->colIsLit_ls00) &&
             (1     == szMap_LongDataType_ls00[pParamInfo->piBasicInfo_ls00.fbDataType_ls00]) )
        {
            if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
            {   // in case of file format compact
                bFound = true;
            }
            else
            {
                // In der select-liste hat der benutzer eine spalte angegeben, die vom typ long ist.
                // Suche diese spalte auch in der spaltenliste.
                // Einmal abgearbeitete longs werden dabei ausgeschlossen.
                for (j = 0; ( (j < lLongFileCnt) && (false == bFound) ); ++j)
                {
                    if (false == bLongIsDone[j])
                    {
                        myLongFileSpec = pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[j];

                        if (NULL != myLongFileSpec->fsLongColName_ls00)     // Spaltennamen vergleichen
                        {
                            // Ist der vergleichsname auch wirklich ungleich NULL oder hat der benutzer in
                            // der spaltenliste keine namen sondern nummern angegeben? In diesem fall
                            // ist der string pColumn->colName_ls00 NULL und ein vergleich wuerde zu einem
                            // fehler fuehren
                            if (NULL == pColumn->colName_ls00.asCharp())
                            {
                                // Spaltenname ist NULL; versuche es noch mit der spalten-ID
                                if (pColumn->colNumber_ls00 == myLongFileSpec->fsLongColID_ls00)
                                {
                                    bFound = bLongIsDone[j] = true;
                                }
                            }
                            else
                            {
                                if (0 == _tcsicmp(pColumn->colName_ls00.asCharp(), myLongFileSpec->fsLongColName_ls00))
                                {
                                    bFound = bLongIsDone[j] = true;
                                }
                            }
                        }
                        else                                            // Spalten-Id's vergleichen
                        {
                            if (pColumn->colNumber_ls00 == myLongFileSpec->fsLongColID_ls00)
                            {
                                bFound = bLongIsDone[j] = true;
                            }
                        }
                    }
                }   // end for (j = 0; ( (j < lLongFileCnt) && (false == bFound) ); ++j)
            }   // end if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)

            if (true == bFound)
            {
                //*
                //* Initialize values needed for processing
                //*
                pLongInfo = &pColumn->colLongInfo;

                pLongInfo->loiFileOffset_ls00.lnuRemainder_ls00  = 1L;
                pLongInfo->loiFileOffset_ls00.lnu2GB_Blocks_ls00 = 0L;
                pLongInfo->loiGreatestNum_ls00 = 1;
                if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
                {
                    pLongInfo->loiFileName_ls00     = pTransformationCmd->trDataFile_ls00.fsFileName_ls00;
                    pLongInfo->loiFileEncoding_ls00 = pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00;
                }
                else
                {
                    pLongInfo->loiFileEncoding_ls00 = myLongFileSpec->fsFormat_ls00.ffoCodeType_ls00;
                    pLongInfo->loiFileName_ls00     = myLongFileSpec->fsFileName_ls00;

                    //*
                    //*     Check if the LONG data file is numbered or with offset
                    //*
                    tsp00_Int4 tmpFileNameLen = STATIC_CAST(tsp00_Int4, _tcslen(pLongInfo->loiFileName_ls00) );

                    while ( ('#' == pLongInfo->loiFileName_ls00[tmpFileNameLen - 1]) &&
                            (tmpFileNameLen > 0) )
                    {
                        ++pLongInfo->loiNumberSize_ls00;
                        --tmpFileNameLen;
                    }
                    if (0 != pLongInfo->loiNumberSize_ls00)
                    {
                        //*
                        //*     Numbered file
                        //*

                        // Determine highest possible number
//TODOTODO Ein ueberlauf muss hier unbedingt vermieden werden
                        for (tsp00_Int4 l = 0; l < pLongInfo->loiNumberSize_ls00; ++l)
                        {
                            pLongInfo->loiGreatestNum_ls00 *= 10;
                        }
                        pLongInfo->loiGreatestNum_ls00     -= 1;

                        pLongInfo->loibFileNumbered_ls00   = true;

                        // Ist bei datenformat FORMATTED die angegebene laenge kleiner als der
                        // dateiname, wird hier ein fehler generiert (hex wird auch beachtet).
                        if (true == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00)
                        {
                            tsp00_Int4 lValLen = pColumn->colFPos_ls00.fpoEnd_ls00 -
                                                 (pColumn->colFPos_ls00.fpoStart_ls00 - 1);
                            tmpFileNameLen += pLongInfo->loiNumberSize_ls00;

                            if (true == pColumn->colFormat_ls00.ffoIsHex_ls00)
                            {
                                tmpFileNameLen += tmpFileNameLen;
                            }

                            if (tmpFileNameLen > lValLen)
                            {
                                pMsgObj->ls98PrintMessage(layDataExtract_els98, errWrongFieldLength_els98,
                                                                                pColumn->colName_ls00.asCharp(),
                                                                                tmpFileNameLen);
                                ++lErrNum;
                            }
                        }
                    }   // end if (0 != pLongInfo->loiNumberSize_ls00)

                }// if (false == pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
                ++pDatExtrRec->derLongColCnt_ls00;

            }// if (true == bFound)
            else
            {
                // szenario 1
                pMsgObj->ls98PrintMessage(layDataExtract_els98, errNoLongFileInfo_els98,
                                                                pColumn->colName_ls00.asCharp());
                ++lErrNum;
            }   // end if (true == bFound)
        }   // end if ( (false == pColumn->colIsLit_ls00) && ...
    }       // end for (i; i < MultCol.mcsCount_ls00; ++i)


    // szenario 2: Warnungen in protokolldatei und ok zurueckgeben.
    if ( ( (0 == pDatExtrRec->derLongColCnt_ls00) && (0 != lLongFileCnt) ) ||
         (pDatExtrRec->derLongColCnt_ls00 < lLongFileCnt) )
    {
        for (j = 0; j < lLongFileCnt; ++j)
        {
            if (false == bLongIsDone[j])
            {
                // long file infos specified in command but no column assigned -> generate warning
                myLongFileSpec = pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[j];
                if (NULL != myLongFileSpec->fsLongColName_ls00)
                {
                    pMsgObj->ls98PrintMessage(layDataExtract_els98, errSurplusLongInfo_els98,
                                                                    myLongFileSpec->fsLongColName_ls00);
                }
                else
                {
                    char sztmpStr[10];  // nur notwendig, um die fehlerausgabe hinzubekommen
                    sztmpStr[0] = '\0';
                    _itot(myLongFileSpec->fsLongColID_ls00, sztmpStr, 10);
                    pMsgObj->ls98PrintMessage(layDataExtract_els98, errSurplusLongInfo_els98,
                                                                    sztmpStr);
                }
            }
        }   // end for (j = 0; j < lLongFileCnt; ++j)
    }

    if (0 != lErrNum)
    {
        rc = errAnalyzeLongSum_els98;
        pMsgObj->ls98MsgToString(layDataExtract_els98, pszErrText, errAnalyzeLongSum_els98);
    }

    if (NULL != bLongIsDone)
    {
        delete [] bLongIsDone;
        bLongIsDone = NULL;
    }

    return rc;
}
// ls27_AnalyzeLongInfo


/*
  -----------------------------------------------------------------------------
  function: ls27_FillColStruct
  -----------------------------------------------------------------------------
*/
static void
ls27_FillColStruct(tls00_TransformationCmd *pTransformationCmd, tls00_TableDescription *TableInfo)
{
    ROUTINE_DBG_MEO00 ("ls27_FillColStruct");

    tls00_MultColSpec &MultCol        = pTransformationCmd->trColSpec_ls00;
    tls00_FileFormat  &DataFileFormat = pTransformationCmd->trDataFile_ls00.fsFormat_ls00;

    MultCol.mcsCount_ls00 = TableInfo->tdFieldCount_ls00;

    tls00_FieldBasics *pBI              = NULL;
    tsp00_Int4         ValLen           = 0;
    tsp00_Int4         NILValLen        = 0;
    SAPDB_Int4         _long_file_index = 0;            // running loop variable for long file structure

    for (int i = 0; i < TableInfo->tdFieldCount_ls00; ++i)
    {
        pBI = &TableInfo->tdParamArray_ls00[i]->piBasicInfo_ls00;

        tls00_Column *pCol =  new tls00_Column;
        memset(pCol, DEF_BYTE_LS00, sizeof(tls00_Column));

        pCol->colName_ls00.rawAssign(TableInfo->tdColName_ls00[i]->asCharp());
        pCol->colNumber_ls00 = i + 1;           // necessary if long column and for 'DATAEXTRACT FOR DATALOAD/...'

        // Because we do define the output length here we need to define if the
        // column is to be dumped as hexadecimal
        if ( (1 == szMap_BlobDataType_ls00[pBI->fbDataType_ls00]) &&
             (0 == szMap_LongDataType_ls00[pBI->fbDataType_ls00])  )
        {
            if (false == DataFileFormat.ffoBinary_ls00)
            {
                pCol->colFormat_ls00.ffoIsHex_ls00 = true;
            }
        }


        if (true == DataFileFormat.ffoFormatted_ls00)       // formatierte ausgabe, d.h. FORMATTED
        {
            if (0 == i)
            {
                pCol->colFPos_ls00.fpoStart_ls00 = 1;
            }
            else
            {
                pCol->colFPos_ls00.fpoStart_ls00 = MultCol.mcsColumn_ls00[i-1]->colFPos_ls00.fpoEnd_ls00 + 1;
            }

            ValLen    = 0;
            NILValLen = 0;

            if (1 == szMap_LongDataType_ls00[pBI->fbDataType_ls00])
            {
                SAPDB_Int4 longlen =
                    STATIC_CAST(tsp00_Int4, _tcslen(pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[_long_file_index]->fsFileName_ls00));
                if ('#' != pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[_long_file_index]->fsFileName_ls00[longlen -1])
                {
                    longlen = 0;
                }
                ls19CheckLengthForExtract(longlen, pCol, pBI, &DataFileFormat, ValLen, NULL);
                ++_long_file_index;
            }
            else
            {
                // We get the needed length by specifying a length of 0
                ls19CheckLengthForExtract(0, pCol, pBI, &DataFileFormat, ValLen, NULL);
            }

            // Check for allowed null values
            ls19CheckNILValLength(0, pCol, TableInfo->tdParamArray_ls00[i]->piMode_ls00, &DataFileFormat, NILValLen);
            pCol->colFPos_ls00.fpoEnd_ls00 = (NILValLen > ValLen) ?
                                                 pCol->colFPos_ls00.fpoStart_ls00 + NILValLen - 1 :
                                                 pCol->colFPos_ls00.fpoStart_ls00 + ValLen - 1;
        }
        else
        {
            pCol->colFPos_ls00.fpoStart_ls00 = i + 1;   // schleifenvariable startet bei 0, die
            pCol->colFPos_ls00.fpoEnd_ls00 = 0;         // position aber bei 1 !!!
        }

        pCol->colFormat_ls00.ffoFormat_ls00 = ioChar_els00;
        MultCol.mcsColumn_ls00[i] = pCol;
    }

    return;
}
// ls27_FillColStruct()

/*
  -----------------------------------------------------------------------------
  function: ls27_MakeMapIndex
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_MakeMapIndex(tls00_MultColSpec      *MultCol,
                  tls00_TableDescription *TableInfo,
                  tls00_ColumnData       *pColumnData,
                  tsp00_Addr             pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_MakeMapIndex");

    tsp00_Int4 rc = errOK_els00;

    // Die daten in tableinfo muessen gegebenenfalls umsortiert und an die ausgabe angepasst werden.
    // Wir erstellen dafuer einen index, mit dem der index von MultCol auf den von TableInfo
    // gemapt wird.
    tsp00_Int4       lPos         = 0;
    const tsp00_Int4 ErrArraySize = (sizeof(tsp00_KnlIdentifier) + 4) * MultCol->mcsCount_ls00;
    _TCHAR*          ErrArray  = NULL;


    tls00_Column *pCol = NULL;

    for (tsp00_Int4 i=0, j = 0; i < MultCol->mcsCount_ls00; ++i)
    {
        pCol = MultCol->mcsColumn_ls00[i];

        bool bFound = false;
        if (0 != pCol->colNumber_ls00)
        {
            if (pCol->colNumber_ls00 <= TableInfo->tdFieldCount_ls00)
            {
                pColumnData[i].cdPosInStmt_ls00 = pCol->colNumber_ls00 - 1;
                bFound = true;
            }
        }
        else
        {
            for (j=0; j < TableInfo->tdFieldCount_ls00; ++j)
            {
                if ( 0 == _tcsicmp(pCol->colName_ls00.asCharp(), (*TableInfo->tdColName_ls00[j]).asCharp()))
                {
                    bFound = true;
                    pColumnData[i].cdPosInStmt_ls00 = j;
                }
            }
        }

        if (false == bFound)
        {
            // Nur wenn es sich nicht um eine zuweisung handelt, einen fehler generieren.
            // Eine zuweisung wird durch colIsLit_ls00 = true angezeigt.
            if (false == pCol->colIsLit_ls00)
            {
                if (NULL == ErrArray)
                {
                    ErrArray = new _TCHAR[ErrArraySize];
                }

                if (0 != pCol->colNumber_ls00)
                {
                    lPos += sp77sprintf(ErrArray + lPos, ErrArraySize - lPos, _T("%d, "), pCol->colNumber_ls00);
                }
                else
                {
                    lPos += sp77sprintf(ErrArray + lPos, ErrArraySize - lPos, _T("%s, "), pCol->colName_ls00.asCharp());
                }
            }
        }
    }   // end for (i=0; i < MultCol->mcsCount_ls00; ++i)

    if (NULL != ErrArray)
    {
        ErrArray[strlen(ErrArray) - 2] = '\0';  // letztes ueberzaehliges komma entfernen

        LoaderMessages::Instance()->ls98Msg(layDataExtract_els98, pszErrText, errNotInSelList_els98, ErrArray);
        delete [] ErrArray;

        rc = errNotInSelList_els98;
    }

    return rc;
}
// ls27_MakeMapIndex()

/*
  -----------------------------------------------------------------------------
  function: ls27_RecalculateLength
  -----------------------------------------------------------------------------
*/
static void
ls27_RecalculateLength(tls00_MultColSpec    *MultCol,
                       tls00_TableDescription *TableInfo,
                       tls00_ColumnData       *pColumnData,
                       tsp00_Int4             (*LenFrac)[2])
{
    ROUTINE_DBG_MEO00 ("ls27_RecalculateLength");

    for(tsp00_Int4 i = 0; i < MultCol->mcsCount_ls00; ++i)
    {
        tls00_Column      *pCol = MultCol->mcsColumn_ls00[i];
        tls00_FieldBasics  FB   = TableInfo->tdParamArray_ls00[pColumnData[i].cdPosInStmt_ls00]->piBasicInfo_ls00;

        if (true == pCol->colIsLit_ls00)
        {
            continue;
        }

        if (1 == szMap_NumberDataType_ls00[pColumnData[i].cdSQLDataType_ls00])
        {
            ls19CalcExtLenFrac(FB, pCol->colOption_ls00, LenFrac[i]);
        }
    }

    return;
}
// ls27_RecalculateLength()

/*
  -----------------------------------------------------------------------------
  function: ls27_CheckLength
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_CheckLength(tls00_MultColSpec      *MultCol,
                 tls00_TableDescription *TableInfo,
                 tls00_FileFormat       *FileFormat,
                 tls00_ColumnData       *pColumnData,
                 tsp00_Int4             (*IntFracPart)[2],
                 tsp00_Addr             pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_CheckLength");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // error reporting object

    tsp00_Int4 rc  = errOK_els00;
    tsp00_Int4 rc2 = errOK_els00;

    if (true == FileFormat->ffoFormatted_ls00)
    {
        tsp00_Int4       lErrNum = 0;

        tls00_Column    *pCol    = NULL;
        tls00_ParamInfo *pPI     = NULL;
        tsp00_Int4       ValLen  = 0;
        tsp00_Int4       len     = 0;

        for(tsp00_Int4 i = 0; i < MultCol->mcsCount_ls00; ++i)
        {
            pCol   = MultCol->mcsColumn_ls00[i];
            pPI    = TableInfo->tdParamArray_ls00[pColumnData[i].cdPosInStmt_ls00];
            ValLen = pCol->colFPos_ls00.fpoEnd_ls00 - pCol->colFPos_ls00.fpoStart_ls00 + 1;
            len    = 0;

            if (true == pCol->colIsLit_ls00)
            {   // Literals
                // in case the length is to little we create a warning; no further checks necessary

                if ( pCol->colName_ls00.length() > ValLen )
                {
                    pMsgObj->ls98PrintMessage(layDataExtract_els98, errLiteralTruncated_els98,
                                                                    pCol->colName_ls00.asCharp());
                }
            }
            else
            {
                rc = ls19CheckLengthForExtract(ValLen, pCol, &pPI->piBasicInfo_ls00, FileFormat, len, IntFracPart[i]);
                if ( errOK_els00 != rc )
                {
                    if (1 == szMap_LongDataType_ls00[pColumnData[i].cdSQLDataType_ls00])
                    {
                        rc = errOK_els00;
                    }
                    else
                    {
                        // generate a message to the error string and to the log file
                        if ( (errFractionTooSmall_els98 == rc) || (errWrongFieldLength_els98 == rc) )
                        {
                            pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, STATIC_CAST(ErrorNumbers,rc),
                                                                               pCol->colName_ls00.asCharp(),
                                                                               len);
                            ++lErrNum;
                        }
                        else if (errIntValueOverflow_els98 == rc)   // only a warning
                        {
                            pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, STATIC_CAST(ErrorNumbers,rc),
                                                                               pCol->colName_ls00.asCharp());
                            rc = errOK_els00;
                        }
                        else    // special error messages
                        {
                            pMsgObj->ls98Msg(layDataExtract_els98, pszErrText, STATIC_CAST(ErrorNumbers,rc),
                                                                               pCol->colName_ls00.asCharp());
                            ++lErrNum;
                        }
                    }
                }

                // Check length of null values. The used function generates only warnings.
                // REUSING variable len!
                rc2 = ls19CheckNILValLength(ValLen, pCol, pPI->piMode_ls00, FileFormat, len=0);
                if (errOK_els00 != rc2)
                {
                    pMsgObj->ls98PrintMessage(layDataExtract_els98, STATIC_CAST(ErrorNumbers,rc2),
                                                                     pCol->colName_ls00.asCharp());
                }
            }
        }   // end for(i = 0; i < MultCol->mcsCount_ls00; ++i)

        if (0 != lErrNum)
        {
            rc = errLengthCheckSum_els98;
            pMsgObj->ls98MsgToString(layDataExtract_els98, pszErrText, errLengthCheckSum_els98);
        }
    }   // end if (true == FileFormat->ffoFormatted_ls00)

    return rc;
}
// ls27_CheckLength()



/*
  -----------------------------------------------------------------------------
  function:     ls27_CheckCompatibility
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_CheckCompatibility(tls00_MultColSpec      *MultCol,
                        tls00_TableDescription *TableInfo,
                        tls00_ColumnData       *pColumnData,
                        tls00_FileFormat       *FileFormat,
                        tsp00_Addr             pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_CheckCompatibility");

    tsp00_Int4       rc      = errOK_els00;
    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // message object

    tsp00_Int4       lErrNum = 0;
    tls00_Column    *pCol    = NULL;
    tls00_ParamInfo *pPI     = NULL;


    for(tsp00_Int4 i = 0; i < MultCol->mcsCount_ls00; ++i)
    {
        pCol = MultCol->mcsColumn_ls00[i];
        pPI  = TableInfo->tdParamArray_ls00[pColumnData[i].cdPosInStmt_ls00];

        // Literal? Nothing to do.
        if (true == pCol->colIsLit_ls00)
        {
            continue;
        }

        rc = ls19CheckDataTypeCompatibility(pCol, &pPI->piBasicInfo_ls00);
        if (errOK_els00 != rc)
        {
            // Es wird immer eine meldung sowohl ins logfile als auch in den fehlerstring
            // ausgegeben. Wenn mehr als ein fehler auftreten, dann wird die letzte fehlermeldung
            // mit einer summenfehlermeldung weiter unten ueberschrieben.
            pMsgObj->ls98PrintMessage(layDataExtract_els98, STATIC_CAST(ErrorNumbers,rc),
                                                            pCol->colName_ls00.asCharp());
            ++lErrNum;
            rc = errOK_els00;
        }

        if (1 == szMap_LongDataType_ls00[pColumnData[i].cdSQLDataType_ls00])
        {
            rc = ls08CheckLONG_DT_Compatibility(pCol->colLongInfo.loiFileEncoding_ls00,
                                                pColumnData[i].cdSQLDataType_ls00,
                                                false);
            if (errOK_els00 != rc)
            {
                pMsgObj->ls98PrintMessage(layDataExtract_els98, STATIC_CAST(ErrorNumbers,rc),
                                                                pCol->colName_ls00.asCharp());
                ++lErrNum;
                rc = errOK_els00;
            }
        }

        rc = ls19CheckNILValCompatibility(pCol, pPI->piMode_ls00, FileFormat, pszErrText);
        if (errOK_els00 != rc)
        {
            // Es wird immer eine meldung sowohl ins logfile als auch in den fehlerstring
            // ausgegeben. Wenn mehr als ein fehler auftreten, dann wird die letzte fehlermeldung
            // mit einer summenfehlermeldung weiter unten ueberschrieben.
            pMsgObj->ls98PrintMessage(layDataExtract_els98, STATIC_CAST(ErrorNumbers,rc),
                                                            pCol->colName_ls00.asCharp());
            ++lErrNum;
            rc = errOK_els00;
        }

    }

    if (0 != lErrNum)
    {
        rc = errCompatibilityCheckSum_els98;
        pMsgObj->ls98MsgToString(layDataExtract_els98, pszErrText, errCompatibilityCheckSum_els98);
    }

    return rc;
}
// ls27_CheckCompatibility()

/*
  -----------------------------------------------------------------------------
  function:     ls27_InitAndOpenLongFiles
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls27_InitAndOpenLongFiles(tls00_MultColSpec  *pMultCol,
                          tls00_ColumnData   *pColumnData,
                          tls00_VFile       *&pDataFile,
                          bool                bCompactFormat,
                          tsp00_Addr          pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls27_InitAndOpenLongFiles");

    tsp00_Int4 rc = errOK_els00;

    tls00_Column *pCol = NULL;

    for (int i = 0; i < pMultCol->mcsCount_ls00; ++i)
    {
        pCol = pMultCol->mcsColumn_ls00[i];

        // Literals may not be of type LONG
        if ( (false == pCol->colIsLit_ls00) &&
             (1 == szMap_LongDataType_ls00[pColumnData[i].cdSQLDataType_ls00]) )
        {
            if (true == bCompactFormat)
            {
                // In case of compact format we use the data file for LONG, too.
                pCol->colLongInfo.loiFileStruct_ls00 = pDataFile;
            }
            else
            {
                pCol->colLongInfo.loiFileStruct_ls00 = new tls00_VFile;
                memset(pCol->colLongInfo.loiFileStruct_ls00, 0, sizeof(tls00_VFile));

                // Open the long value file at this point only in case it is not numbered;
                // numbered means every long value of a column is saved in its own file
                if ( (false == pCol->colLongInfo.loibFileNumbered_ls00) &&
                     (NULL  != pCol->colLongInfo.loiFileStruct_ls00)    &&
                     (0     == pCol->colLongInfo.loiFileStruct_ls00->vfFileHndl_ls00) )
                {
                    //*
                    //*     Long files are ALWAYS opened as binary files independent of the fact
                    //*     that the data is text
                    //*
                    ls30VFInitialize(*pCol->colLongInfo.loiFileStruct_ls00, pCol->colLongInfo.loiFileName_ls00,
                                     sp5vf_binary, sp5vf_write, pColumnData[i].cdExtLONGEncoding_ls00);

                    rc = ls30VFOpen(*pCol->colLongInfo.loiFileStruct_ls00, pszErrText);
                    if (errOK_els00 == rc)
                    {
                        rc = ls30Mark_BinaryFile_Encoding(*pCol->colLongInfo.loiFileStruct_ls00, pszErrText);
                    }
                }
            }
        }
    }   // end for (i; i < pTransformationCmd->trColSpec_ls00.mcsCount_ls00; ++i)

    return rc;
}
// ls27_InitAndOpenLongFiles()

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/
