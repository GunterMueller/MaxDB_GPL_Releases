/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateFiles.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of files
  version:      7.3 and higher
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Web/DBMWeb_TemplateFiles.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME   "DBMFiles.htm"

#define OBJ_TITLE       "GroupBoxTitle"

#define OBJ_TABFILES    "TabFiles"
#define OBJ_TABDIAG     "TabDiag"
#define OBJ_TABPACK     "TabPack"

#define OBJ_ROWPAIR     "RowPair"

#define OBJ_EVENROW     "EvenRow*"
#define OBJ_ODDROW      "OddRow*"

#define OBJ_NAME        "Name"
#define OBJ_COMMENT     "Comment"
#define OBJ_SIZE        "Size"
#define OBJ_MODIFIED    "Modified"
#define OBJ_SHORTNAME   "ShortName"
#define OBJ_DATE        "Date"
#define OBJ_TIMESTAMP   "Timestamp"
#define OBJ_PATH        "Path"
#define OBJ_DIAGS       "Diagnosises"
#define OBJ_DIAGLIST    "DiagnosisList"
#define OBJ_DIAGFILE    "DiagFile"
#define OBJ_TOOLBAR     "Toolbar"
#define OBJ_DIAGCHK     "DiagnosisCheckbox"
#define OBJ_BTNBACK     "ButtonBack"
#define OBJ_BTNPACK     "ButtonPack"


/*
  =============================================================================
  class:        DBMWeb_TemplateFiles
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateFiles :: DBMWeb_TemplateFiles
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateFiles :: DBMWeb_TemplateFiles ( sapdbwa_WebAgent   & wa,
                                               Mode                 nMode,
                                               DBMCli_Files       & oFiles,
                                               DBMCli_Diagnosises & oDiagnosises,
                                               SAPDB_Int2           nIndex )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                          m_nMode        (nMode ),
                          m_oFiles       ( oFiles ),
                          m_oDiagnosises ( oDiagnosises ),
                          m_nIndex       ( nIndex )

{
  m_nCol  = 0;
  m_nRow  = 0;
}  // end DBMWeb_TemplateFiles :: DBMWeb_TemplateFiles


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFiles :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateFiles :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_TITLE ) ) {
    nReturn = 1;
  } else if ( ISOBJ(OBJ_TABFILES)) {
    nReturn = (m_nMode == MODE_FILES) ? 1 : 0;
  } else if ( ISOBJ(OBJ_TABDIAG )) {
    nReturn = (m_nMode == MODE_DIAG) || (m_nMode == MODE_DIAGLIST) ? 1 : 0;
  } else if ( ISOBJ(OBJ_TABPACK )) {
    nReturn = (m_nMode == MODE_PACK) ? 1 : 0;
  } else if ( ISOBJ(OBJ_DIAGCHK )) {
    nReturn = ( (m_nMode == MODE_PACK) && (m_oDiagnosises.DiagnosisArray( ).GetSize( ) > 0) ) ? 1 : 0;
  } else if ( ISOBJ( OBJ_ROWPAIR ) ) {
    switch (m_nMode) {
    case MODE_FILES:
      nReturn = (m_oFiles.FileArray( ).GetSize( )  / 2) + 
                (m_oFiles.FileArray( ).GetSize( ) % 2); 
      break;
    case MODE_DIAG:
      nReturn = (m_oDiagnosises.DiagnosisArray( ).GetSize( )  / 2) + 
                (m_oDiagnosises.DiagnosisArray( ).GetSize( ) % 2); 
      break;
    case MODE_DIAGLIST:
      nReturn = (m_oDiagnosises.DiagnosisArray()[m_nIndex].FileArray().GetSize( )  / 2) + 
                (m_oDiagnosises.DiagnosisArray()[m_nIndex].FileArray().GetSize( ) % 2); 
      break;
    case MODE_PACK:
      nReturn = (m_oDiagnosises.DiagnosisArray( ).GetSize( )  / 2) + 
                (m_oDiagnosises.DiagnosisArray( ).GetSize( ) % 2); 
      break;
    } // end switch
  } else if( ISOBJ(  OBJ_EVENROW) || ISOBJ(  OBJ_ODDROW) ) {
    switch (m_nMode) {
    case MODE_FILES:
      nReturn = (m_nRow < m_oFiles.FileArray( ).GetSize( )) ? 1 : 0;
      m_nRow++;
      break;
    case MODE_DIAG:
      nReturn = (m_nRow < m_oDiagnosises.DiagnosisArray( ).GetSize( )) ? 1 : 0;
      m_nRow++;
      break;
    case MODE_DIAGLIST:
      nReturn = (m_nRow < m_oDiagnosises.DiagnosisArray()[m_nIndex].FileArray().GetSize( )) ? 1 : 0;
      m_nRow++;
      break;
    case MODE_PACK:
      nReturn = (m_nRow < m_oDiagnosises.DiagnosisArray( ).GetSize( )) ? 1 : 0;
      m_nRow++;
      break;
    } // end switch
  } else if ( ISOBJ(OBJ_DIAGS)) {
    nReturn = (m_nMode == MODE_DIAG) ? 1 : 0;
  } else if ( ISOBJ(OBJ_DIAGLIST)) {
    nReturn = (m_nMode == MODE_DIAGLIST) ? 1 : 0;
  } else if ( ISOBJ(OBJ_TOOLBAR)) {
    switch (m_nMode) {
    case MODE_FILES:
      nReturn = 0;
      break;
    case MODE_DIAG:
      nReturn = 0;
      break;
    case MODE_DIAGLIST:
      nReturn = 1;
      break;
    case MODE_PACK:
      nReturn = 1;
      break;
    } // end switch
  } else if ( ISOBJ(OBJ_BTNBACK)) {
    switch (m_nMode) {
    case MODE_FILES:
      nReturn = 0;
      break;
    case MODE_DIAG:
      nReturn = 0;
      break;
    case MODE_DIAGLIST:
      nReturn = 1;
      break;
    case MODE_PACK:
      nReturn = 0;
      break;
    } // end switch
  } else if ( ISOBJ(OBJ_BTNPACK)) {
    switch (m_nMode) {
    case MODE_FILES:
      nReturn = 0;
      break;
    case MODE_DIAG:
      nReturn = 0;
      break;
    case MODE_DIAGLIST:
      nReturn = 0;
      break;
    case MODE_PACK:
      nReturn = 1;
      break;
    } // end switch
  }  // end if

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFiles :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateFiles :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn   = NULL;
  int                nHashmark = 0;
  DBMCli_String      sValue;
  
  if( ISOBJ( OBJ_TITLE ) ) {
    sValue = m_oFiles.GetDatabase().DatabaseName();  
  } else if( ISOBJ( OBJ_NAME ) ) {
    DBMCli_FileArray & aFiles    = m_oFiles.FileArray( );
    DBMCli_String      sName     = aFiles[m_nRow - 1].Name( );
    DBMCli_String      sTemp;

    sValue = "";
    for (int i = 0; i < sName.GetLength (); ++i) {
//      sTemp.Format("%%25%02x", (int) sName.GetAt(i));
      sTemp.Format("%%%02x", (int) sName.GetAt(i));
      sValue = sValue + sTemp;
    } // end for

//    pReturn = sValue;
  } else if( ISOBJ( OBJ_COMMENT ) ) {
    DBMCli_FileArray & aFiles = m_oFiles.FileArray( );
    sValue = aFiles[m_nRow - 1].Comment( );
  } else if( ISOBJ( OBJ_SIZE ) ) {
    DBMCli_FileArray & aFiles = m_oFiles.FileArray( );
    sValue = aFiles[m_nRow - 1].Size( );
  } else if( ISOBJ( OBJ_MODIFIED ) ) {
    DBMCli_FileArray & aFiles = m_oFiles.FileArray( );
    sValue = aFiles[m_nRow - 1].Modified( );
  } else if( ISOBJ( OBJ_SHORTNAME ) ) {
    DBMCli_FileArray & aFiles = m_oFiles.FileArray( );
    sValue = aFiles[m_nRow - 1].Name( );
  } else if( ISOBJ( OBJ_DATE ) ) {
    sValue = m_oDiagnosises.DiagnosisArray( )[m_nRow - 1].GetTimestamp().Get(DBMCLI_DT_INT_DATETIME_FMT);
  } else if( ISOBJ( OBJ_TIMESTAMP ) ) {
    sValue = m_oDiagnosises.DiagnosisArray( )[m_nRow - 1].GetTimestamp().Get(DBMCLI_DT_NUM_DATETIME_FMT);
  } else if( ISOBJ( OBJ_PATH ) ) {
    sValue = m_oDiagnosises.DiagnosisArray( )[m_nRow - 1].GetPath();
  } else if( ISOBJ( OBJ_DIAGFILE ) ) {
    sValue = m_oDiagnosises.DiagnosisArray()[m_nIndex].FileArray()[m_nRow - 1].Name( );
  } // end if

  pReturn = sValue;
  return _Tools_UTF8Ptr(pReturn);
} // end askForValue

