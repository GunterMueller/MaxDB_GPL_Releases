/*!
  \file    DBMSrvUsr_Authentications.cpp
  \author  BerndV
  \ingroup DBMServer User area
  \brief   definition of class which contains all authentication methods with buffers

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
#include "DBM/Srv/User/DBMSrvUsr_Authentications.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvUsr_Authentications
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvUsr_Authentications::DBMSrvUsr_Authentications
//-----------------------------------------------------------------------------
DBMSrvUsr_Authentications::DBMSrvUsr_Authentications
  ( )
{
  RTESec_Authentication::Iterator oIterator;
  const RTESec_Authentication *   pAuthentication = NULL;
  int                             nIndex          = 0;

  while (oIterator.next(pAuthentication)) {
    m_oTheArray[nIndex].pAuthentication = pAuthentication;
    ++nIndex;
  } // end while
} // end DBMSrvUsr_Authentications::DBMSrvUsr_Authentications

//-----------------------------------------------------------------------------
// private member function DBMSrvUsr_Authentications::GiveObject
//-----------------------------------------------------------------------------
DBMSrvUsr_Authentications::AuthenticationObject & DBMSrvUsr_Authentications::GiveObject
  ( const char * szMethod )
{
  for (int nIndex = 0; nIndex < m_oTheArray.size(); ++nIndex) {
    if (strcmp(m_oTheArray[nIndex].pAuthentication->Name(), szMethod) == 0) {
      return m_oTheArray[nIndex];
    } // end if
  } // end for

  return m_oEmtpyObject;
} // end DBMSrvUsr_Authentications::GiveObject

