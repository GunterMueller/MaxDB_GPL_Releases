/*!
  \file    DBMSrvUsr_AuthenticateIterator.cpp
  \author  BerndV
  \ingroup DBMServer User area
  \brief   definition of class to iterate through the allowed authentication methods

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

\endif
*/

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"

#include "DBM/Srv/User/DBMSrvUsr_AuthenticateIterator.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvUsr_AuthenticateIterator
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvUsr_AuthenticateIterator::DBMSrvUsr_AuthenticateIterator
//-----------------------------------------------------------------------------
DBMSrvUsr_AuthenticateIterator::DBMSrvUsr_AuthenticateIterator
  ( const char                                 * szDBName,
    const Tools_Array<Tools_DynamicUTF8String> & oRequested )
{
  DBMSrvMsg_Error oParamError;
    
  DBMSrvPar_Parameters * pXParamData = DBMSrvPar_Parameters::GetInstance(szDBName, true, oParamError);

  if (pXParamData != NULL) {
    readArray(pXParamData, KERNELPARAM_AUTHENTICATION_ALLOW, m_oAllowed);
    readArray(pXParamData, KERNELPARAM_AUTHENTICATION_DENY,  m_oDenied);
    delete pXParamData;
  } // end if

  m_oRequested = oRequested;

} // end DBMSrvUsr_AuthenticateIterator::DBMSrvUsr_AuthenticateIterator

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_AuthenticateIterator::readArray
//-----------------------------------------------------------------------------
void  DBMSrvUsr_AuthenticateIterator::readArray
  ( const DBMSrvPar_Parameters           * pXParamData,
    const SAPDB_UTF8                     * szParameter,
    Tools_Array<Tools_DynamicUTF8String> & oArray ) const 
{
    DBMSrvPar_Parameter * pParameter = NULL;
    const char          * pValue     = NULL;
    const char          * pComma     = NULL;
    int                   nIndex     = 0;

    // read the Array from the parameter
    pParameter = pXParamData->Get(szParameter);
    if (pParameter != NULL) {
      if (strlen(pParameter->szOldValue) > 0) {
        pValue = &(pParameter->szOldValue[0]);
      } else if (strlen(pParameter->szDefValue) > 0) {
        pValue = &(pParameter->szDefValue[0]);
      } // end if
      if (pValue != NULL) {
        pComma = strchr(pValue,',');
        nIndex = 0;
        while (pComma != NULL) {
          oArray[nIndex].Assign(pValue, pComma-pValue);
          oArray[nIndex].Trim();
          pValue = pComma + 1;
          pComma = strchr(pValue,',');
          ++nIndex;
        } // end while
        oArray[nIndex].Assign(pValue);
        oArray[nIndex].Trim();
      } // end if
    } // end if

} // end DBMSrvUsr_AuthenticateIterator::readArray

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_AuthenticateIterator::isAllowed
//-----------------------------------------------------------------------------
bool DBMSrvUsr_AuthenticateIterator::isAllowed
  ( const RTESec_Authentication                * pAuthentication,
    const Tools_Array<Tools_DynamicUTF8String> & oArray ) const
{
  bool bFound   = false;
  for (int nIndex = 0; !bFound && (nIndex < oArray.size());++nIndex)
    bFound = oArray[nIndex].Compare(pAuthentication->Name()) == 0;

  return ((oArray.size() == 0) || bFound);
} // end DBMSrvUsr_AuthenticateIterator::isAllowed

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_AuthenticateIterator::isDenied
//-----------------------------------------------------------------------------
bool DBMSrvUsr_AuthenticateIterator::isDenied
  ( const RTESec_Authentication                * pAuthentication,
    const Tools_Array<Tools_DynamicUTF8String> & oArray ) const
{
  bool bFound   = false;
  for (int nIndex = 0; !bFound && (nIndex < oArray.size());++nIndex)
    bFound =oArray[nIndex].Compare(pAuthentication->Name()) == 0;

  return ((oArray.size() != 0) && bFound);
} // end DBMSrvUsr_AuthenticateIterator::isAllowed

//-----------------------------------------------------------------------------
// public member function DBMSrvUsr_AuthenticateIterator::next
//-----------------------------------------------------------------------------
bool DBMSrvUsr_AuthenticateIterator::next
  ( const RTESec_Authentication * & pAuthentication )
{
  bool bRc = true;

  while ((bRc = RTESec_Authentication::Iterator::next(pAuthentication))) {
    if (isAllowed(pAuthentication, m_oRequested) &&  isAllowed(pAuthentication, m_oAllowed) && !isDenied(pAuthentication, m_oDenied)) {
      break;
    } // end if
  } // end while

  return bRc;
} // end DBMSrvUsr_AuthenticateIterator::next
