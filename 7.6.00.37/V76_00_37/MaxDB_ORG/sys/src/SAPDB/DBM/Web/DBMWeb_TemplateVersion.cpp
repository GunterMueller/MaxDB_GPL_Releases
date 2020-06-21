/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateVersion.cpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: Database Manager WebAgent Extension
  description:  Version
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
#include "DBM/Web/DBMWeb_TemplateVersion.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME   "DBMVersion.htm"

#define OBJ_WEBDBM     "VersionWebdbm*"
#define OBJ_DBMSRV     "VersionDbmsrv*"
#define OBJ_KERNEL     "VersionKernel*"
#define OBJ_BACKBUTTON "BackButton"

#define VAL_COMPONENT  "Component"
#define VAL_VERSION    "Version"
#define VAL_BUILD      "Build"

/*
  =============================================================================
  class:        DBMWeb_TemplateVersion
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateVersion :: DBMWeb_TemplateVersion
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateVersion :: DBMWeb_TemplateVersion ( sapdbwa_WebAgent    & wa,
                                                   const DBMCli_String & sWebdbm,
                                                   const DBMCli_String & sDbmsrv,
                                                   const DBMCli_String & sKernel )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                          m_sWebdbm (sWebdbm),
                          m_sDbmsrv (sDbmsrv),
                          m_sKernel (sKernel)
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateVersion :: ~DBMWeb_TemplateVersion
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateVersion :: ~DBMWeb_TemplateVersion (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateVersion :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateVersion :: askForWriteCount  ( const Tools_DynamicUTF8String  & szName )
{
  SAPDB_Int2 nReturn = 0;
   
  if (szName.Compare(OBJ_WEBDBM) == 0) {
    nReturn = m_sWebdbm.GetLength() > 0 ? 1 : 0;
    m_sCurrent = m_sWebdbm;
  } else if (szName.Compare(OBJ_DBMSRV) == 0) {
    nReturn = m_sDbmsrv.GetLength() > 0 ? 1 : 0;
    m_sCurrent = m_sDbmsrv;
  } else if (szName.Compare(OBJ_KERNEL) == 0) {
    nReturn = m_sKernel.GetLength() > 0 ? 1 : 0;
    m_sCurrent = m_sKernel;
  } else if (szName.Compare(OBJ_BACKBUTTON) == 0) {
    nReturn = m_sDbmsrv.GetLength() > 0 ? 0 : 1;
  } // end if

  return nReturn;
} // DBMWeb_TemplateVersion :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateVersion :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateVersion :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
 Tools_DynamicUTF8String sString;

  if (szName.Compare(VAL_COMPONENT) == 0) {
    sString.Assign(m_sCurrent.Left(9));
  } else if (szName.Compare(VAL_VERSION) == 0) {
    sString.Assign(m_sCurrent.Mid(10, 8));
  } else if (szName.Compare(VAL_BUILD) == 0) {
    sString.Assign(m_sCurrent.Mid(25));
  } // end if

  return sString;
} // end DBMWeb_TemplateVersion :: askForValue

