/*!
  \file    DBMSrvUsr_Authentications.hpp
  \author  BerndV
  \ingroup DBMServer User area
  \brief   declaration of class which contains all authentication methods with buffers

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

#if !defined(DBMSrvUsr_Authentications_hpp)
#define DBMSrvUsr_Authentications_hpp

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "ToolsCommon/Tools_Array.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "RunTime/Security/RTESec_Authentication.hpp"

/*! \brief A class which contains all authentication methods with buffers.
 */
class DBMSrvUsr_Authentications
{
  public:
    /*! \brief Constructor
     */
    DBMSrvUsr_Authentications
     ( );

    /*! \brief get a pointer to the authentication method
        \param szMethod [IN] name of the method
     */
    const RTESec_Authentication * GiveMethod
      (const char * szMethod) {return GiveObject(szMethod).pAuthentication;}

    /*! \brief get a reference to the client buffer of a method

        This reference will be became invalid when the DBMSrvUsr_Authentications
        object will be destroyed.

        \param szMethod [IN] name of the method
     */
    RTESec_ClientAuthenticationBuffer & GiveClientBuffer
      (const char * szMethod) {return GiveObject(szMethod).oClientBuffer;}

    /*! \brief get a reference to the server buffer of a method

        This reference will be became invalid when the DBMSrvUsr_Authentications
        object will be destroyed.

        \param szMethod [IN] name of the method
     */
    RTESec_ServerAuthenticationBuffer & GiveServerBuffer
      (const char * szMethod) {return GiveObject(szMethod).oServerBuffer;}

  private:
    class AuthenticationObject 
    {
      public:
        AuthenticationObject ( )
          : pAuthentication(NULL)
        {}

      private:
        const RTESec_Authentication       * pAuthentication;
        RTESec_ClientAuthenticationBuffer   oClientBuffer;
        RTESec_ServerAuthenticationBuffer   oServerBuffer;
        
        friend class DBMSrvUsr_Authentications;
    };

    AuthenticationObject & GiveObject
      (const char * szMethod);

    Tools_Array<AuthenticationObject>  m_oTheArray;
    AuthenticationObject               m_oEmtpyObject;
}; // end class DBMSrvUsr_Authentications

#endif // DBMSrvUsr_Authentications_hpp
