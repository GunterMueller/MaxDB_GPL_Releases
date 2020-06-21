/*!
  \file    DBMSrvUsr_Password.cpp
  \author  BerndV
  \ingroup DBMServer User area
  \brief   definition of class which contains all authentication methods with buffers
  \brief   definition of class which handle passwords

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
#include "ToolsCommon/Tools_HexBuffer.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "hsp02.h"

#include "DBM/Srv/User/DBMSrvUsr_Password.hpp"

//-----------------------------------------------------------------------------
// Constructor DBMSrvUsr_Password::DBMSrvUsr_Password
//-----------------------------------------------------------------------------
DBMSrvUsr_Password::DBMSrvUsr_Password
     ( )
{
    m_szUser.Init();
    m_szPassword.Init();
} // end DBMSrvUsr_Password::DBMSrvUsr_Password

//-----------------------------------------------------------------------------
// Constructor DBMSrvUsr_Password::DBMSrvUsr_Password
//-----------------------------------------------------------------------------
DBMSrvUsr_Password::DBMSrvUsr_Password
     ( const void * pPassword,
       StringType   oType )
{
    setPassword(pPassword, oType);
} // end DBMSrvUsr_Password::DBMSrvUsr_Password

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_Password::setPassword
//-----------------------------------------------------------------------------
void DBMSrvUsr_Password::setPassword
     ( const void * pPassword,
       StringType   oType )
{
    m_szUser.Init();
    m_szPassword.Init();

    switch (oType) {
        case UnknownString:
            setUnknownStringPassword((const char *) pPassword);
            break;
        case ClearString:
            setClearStringPassword((const char *) pPassword);
            break;
        case CryptedString:
            setCryptStringPassword((const char *) pPassword);
            break;
        case UnknownStringWithUser:
        case ClearStringWithUser:
        case CryptedStringWithUser:
            setUserAndPassword((const char *) pPassword, oType);
            break;
        case CryptedBufferDB:
            setCryptedBufferDBPassword(pPassword);
            break;
    } // end switch

} // end DBMSrvUsr_Password::setPassword

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_Password::asClearString
//-----------------------------------------------------------------------------
const char *  DBMSrvUsr_Password::asClearString
     (  ) const
{
    return m_szPassword.asCharp();
} // end DBMSrvUsr_Password::asClearString

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_Password::getUser
//-----------------------------------------------------------------------------
const char *  DBMSrvUsr_Password::getUser
     (  ) const
{
    return m_szUser.asCharp();
} // end DBMSrvUsr_Password::getUser

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_Password::isValid
//-----------------------------------------------------------------------------
bool  DBMSrvUsr_Password::isValid
     (  ) const
{
    return (m_szPassword.length() > 0);
} // end DBMSrvUsr_Password::isValid

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_Password::setUserAndPassword
//-----------------------------------------------------------------------------
void DBMSrvUsr_Password::setUserAndPassword
     ( const char * pUserPassword,
       StringType   oType )
{
    // split token in username and passwod
    tsp00_C64c szTemp;
    szTemp.rawAssign(pUserPassword);
    char * pPassword = strchr(szTemp, ',');
    if (pPassword != NULL) {
        *pPassword = 0;
        pPassword++;
        m_szUser.rawAssign(szTemp);
    } else {
        return;
    } // end if

    // handle password
    switch (oType) {
        case UnknownStringWithUser:
            setUnknownStringPassword(pPassword);
            break;
        case ClearStringWithUser:
            setClearStringPassword(pPassword);
            break;
        case CryptedStringWithUser:
            setCryptStringPassword(pPassword);
            break;
    } // end switch

} // end DBMSrvUsr_Password::setUserAndPassword

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_Password::setClearStringPassword
//-----------------------------------------------------------------------------
void DBMSrvUsr_Password::setClearStringPassword
     ( const char * pPassword )
{
    m_szPassword.rawAssign(pPassword);
} // end DBMSrvUsr_Password::setClearStringPassword

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_Password::setCryptStringPassword
//-----------------------------------------------------------------------------
void DBMSrvUsr_Password::setCryptStringPassword
     ( const char * pPassword )
{
    // create a binary representation of the passsword
    Tools_HexBuffer oHexBuffer(pPassword);

    // convert byte buffer to four byte unsigned integers (high byte and low byte)
    tsp00_CryptPw   pwCrypted;
    memset(&pwCrypted, 0, sizeof(pwCrypted));
    for (unsigned int i = 0; i < (oHexBuffer.GiveSizeOfBinBuffer()) && (i < sizeof(pwCrypted)); i++) {
        ((tsp00_Uint4 *)pwCrypted.asCharp())[i / 4] = (((tsp00_Uint4 *)pwCrypted.asCharp())[i / 4] * 0x100) + ((const unsigned char *)oHexBuffer.GiveBinBuffer())[i];
    } // end if

    // uncrypt
    tsp00_Name      pwClear;
    s02appldecrypt(pwClear, pwCrypted);

    // set meber with value
    m_szPassword.p2c(pwClear);
} // end DBMSrvUsr_Password::setCryptStringPassword

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_Password::setUnknownStringPassword
//-----------------------------------------------------------------------------
void DBMSrvUsr_Password::setUnknownStringPassword
     ( const char * pPassword )
{
    if ( SAPDB_strlen(pPassword) == (sizeof(tsp00_CryptName) * 2)) {
        const char * pCurrent = pPassword;
        while (*pCurrent != 0) {
            if (!isHexDigit(*pCurrent)) {
                return setClearStringPassword(pPassword);
            } // end if
            ++pCurrent;
        } // end while
        return setCryptStringPassword(pPassword);
    } // end if

    return setClearStringPassword(pPassword);
} // end DBMSrvUsr_Password::setUnknownStringPassword

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_Password::setCryptedBufferDBPassword
//-----------------------------------------------------------------------------
void DBMSrvUsr_Password::setCryptedBufferDBPassword
     ( const void * pPassword )
{
    tsp00_Name    pwClear;
    tsp00_Namec   pwClearc; 
    tsp00_CryptPw pwCrypted;

    memcpy(&pwCrypted, pPassword, sizeof(pwCrypted));

    s02decrypt(pwClear, pwCrypted);

    pwClearc.p2c(pwClear);

    return setClearStringPassword(pwClearc);
} // end DBMSrvUsr_Password::setCryptedBufferDBPassword

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_Password::isHexDigit
//-----------------------------------------------------------------------------
bool DBMSrvUsr_Password::isHexDigit
  ( char c ) 
{
  switch (c) {
    case '0':case '1':case '2':case '3':case '4':
    case '5':case '6':case '7':case '8':case '9':
    case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
    case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
      return true;;
      break;
  } // end switch
  
  return false;
} // end DBMSrvUsr_Password::isHexDigit
