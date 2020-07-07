/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateHistory.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  parameters
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
#include "DBM/Web/DBMWeb_TemplateHistory.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME    "DBMHistory.htm"

#define OBJ_TITLE        "GroupBoxTitle"

#define OBJ_TABGEN       "TabGeneral"
#define OBJ_TABEXT       "TabExtended"
#define OBJ_TABSUP       "TabSupport"

#define OBJ_TABLEHEADER  "TableHeader"

#define OBJ_ROWPAIR      "RowPair"

#define OBJ_EVENROW      "EvenRow*"
#define OBJ_ODDROW       "OddRow*"

#define OBJ_LABEL        "Label"
#define OBJ_ACTION       "Action"
#define OBJ_BEGINNING    "Beginning"
#define OBJ_RESULT       "Result"
#define OBJ_MEDIANAME    "MediaName"
#define OBJ_LOGREQUIRED  "LogRequired"
#define OBJ_SIZEPAGES    "SizePages"
#define OBJ_VOLUMES      "Volumes"
#define OBJ_LOGPAGE      "LogPage"
#define OBJ_FROMPAGE     "FromPage"
#define OBJ_TOPAGE       "ToPage"

#define HTML_FNT_COL_ERR_B     "<font color=\"#FF0000\">"
#define HTML_FNT_E             "</font>"
/*
  =============================================================================
  class:        DBMWeb_TemplateHistory
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateHistory :: DBMWeb_TemplateHistory
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateHistory :: DBMWeb_TemplateHistory ( sapdbwa_WebAgent & wa,
                                                   DBMCli_History & oHistory,
                                                   DBMCli_HistoryItemArray & aItems )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oHistory( oHistory ),
                            m_aItems( aItems )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateHistory :: ~DBMWeb_TemplateHistory
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateHistory :: ~DBMWeb_TemplateHistory (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateHistory :: initTemplate
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateHistory :: initTemplate ( )
{
  m_nItem = m_aItems.GetSize( );

  return true;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateHistory :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateHistory :: askForWriteCount ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_TABLEHEADER) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
    nReturn = (m_nItem >= 0) ? -1 : 0; 
  } else if ( (szName.Compare(OBJ_EVENROW) == 0) || 
              (szName.Compare(OBJ_ODDROW) == 0)     ) {
    nReturn = (FindNextItem( ) == true) ? 1 : 0;
  } 
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateHistory :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateHistory :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
  SAPDB_Bool bColor = SAPDB_TRUE;

  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_TITLE) == 0) {
    pReturn = m_oHistory.GetDatabase().DatabaseName();  
  } else if (szName.Compare(OBJ_TABLEHEADER) == 0) {
    sValue = "Backup History";
    pReturn = sValue;
  } else {
		if (m_aItems[m_nItem].Action() == HISTORY_ACTION_LOST) {
			if (szName.Compare(OBJ_LABEL) == 0) {
				sValue = m_aItems[m_nItem].ActionStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_BEGINNING) == 0) {
				sValue = m_aItems[m_nItem].StartDateStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			} else {
				sValue = "&nbsp;";
			}
		} else {
			if (szName.Compare(OBJ_LABEL) == 0) {
				sValue = m_aItems[m_nItem].LabelStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_ACTION) == 0) {
				sValue = m_aItems[m_nItem].ActionStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_BEGINNING) == 0) {
				sValue = m_aItems[m_nItem].StartDateStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_MEDIANAME) == 0) {
				sValue = m_aItems[m_nItem].MediaNameStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_LOGREQUIRED) == 0) {
				sValue = m_aItems[m_nItem].LogRequiredStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_SIZEPAGES) == 0) {
				sValue = m_aItems[m_nItem].SizePagesStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
				bColor = SAPDB_FALSE;
			} else if (szName.Compare(OBJ_VOLUMES) == 0) {
				sValue = m_aItems[m_nItem].VolumesStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_LOGPAGE) == 0) {
				if( ( m_aItems[m_nItem].Label( ) == HISTORY_LABEL_DATA )  ||
						( m_aItems[m_nItem].Label( ) == HISTORY_LABEL_PAGES )    ) {
					sValue = m_aItems[m_nItem].LogPageStr( );
				} else {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_FROMPAGE) == 0) {
				if( m_aItems[m_nItem].Label( ) == HISTORY_LABEL_LOG ) {
					sValue = m_aItems[m_nItem].FromPageStr( );
				} else {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_TOPAGE) == 0) {
				if( m_aItems[m_nItem].Label( ) == HISTORY_LABEL_LOG ) {
					sValue = m_aItems[m_nItem].ToPageStr( );
				} else {
					sValue = "&nbsp;";
				}
			} else if (szName.Compare(OBJ_RESULT) == 0) {
				sValue = m_aItems[m_nItem].ResultStr( );
				if (sValue.IsEmpty()) {
					sValue = "&nbsp;";
				}
			}

			if( bColor ) {
				if( m_aItems[m_nItem].ResultNum( ) != 0 ) {
					sValue = HTML_FNT_COL_ERR_B + sValue + HTML_FNT_E;
				}
			}
		}

    pReturn = sValue;
  }

  return _Tools_UTF8Ptr(pReturn);
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFile :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateHistory :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  
  if( szName.Compare(OBJ_ROWPAIR ) == 0 ) {
    bRC = ( m_nItem >= 0 );
  }

  return bRC;
}

bool DBMWeb_TemplateHistory :: FindNextItem ( )
{
  m_nItem--;

  return ( m_nItem >= 0 );
}