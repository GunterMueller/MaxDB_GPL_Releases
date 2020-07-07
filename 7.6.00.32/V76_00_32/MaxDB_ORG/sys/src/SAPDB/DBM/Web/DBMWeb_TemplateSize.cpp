/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateSize.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Size
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
#include "DBM/Web/DBMWeb_TemplateSize.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME   "DBMSize.htm"

#define OBJ_SIZE        "Size*" 
#define OBJ_PAGES       "Pages"
#define OBJ_PAGESIZE    "Pagesize"
#define OBJ_KB          "KB"
#define OBJ_MB          "MB"
#define OBJ_GB          "GB"

/*
  =============================================================================
  class:        DBMWeb_TemplateSize
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateSize :: DBMWeb_TemplateSize
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateSize :: DBMWeb_TemplateSize ( sapdbwa_WebAgent & wa,
                                             const SAPDB_Int4 nPagesize,
                                             const SAPDB_Int4 nPages )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_nPagesize( nPagesize ),
                            m_nPages( nPages )
{
  double dPageKB = nPagesize / 1024;

  m_dKB = dPageKB * nPages;
  m_dMB = m_dKB / 1024;
  m_dGB = m_dMB / 1024;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateSize :: ~DBMWeb_TemplateSize
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateSize :: ~DBMWeb_TemplateSize (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateSize :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateSize :: askForWriteCount  ( const Tools_DynamicUTF8String  & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_SIZE) == 0) {
    nReturn = 1;
  }

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateSize :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateSize :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_PAGESIZE) == 0) {
    sValue.Format("%d", m_nPagesize);
  } else if (szName.Compare(OBJ_PAGES) == 0) {
    sValue.Format("%d", m_nPages);
  } else if (szName.Compare(OBJ_KB) == 0) {
    sValue.Format("%.2f", m_dKB);
  } else if (szName.Compare(OBJ_MB) == 0) {
    sValue.Format("%.2f", m_dMB);
  } else if (szName.Compare(OBJ_GB) == 0) {
    sValue.Format("%.2f", m_dGB);
  }

  pReturn = sValue;

  return _Tools_UTF8Ptr(pReturn);
} 

