/*!
  \file    DBMSrvUsr_ChallengeResponseServer.hpp
  \author  BerndV
  \ingroup DBMServer User area
  \brief   declaration of class to handle challenge/response

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

#if !defined(DBMSrvUsr_ChallengeResponseServer_hpp)
#define DBMSrvUsr_ChallengeResponseServer_hpp

#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "DBM/Srv/User/DBMSrvUsr_AuthenticateIterator.hpp"
#include "SAPDB/ToolsCommon/Tools_HexBuffer.hpp"

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

/*! \brief A class to handle challenge/response.
   
 */

class DBMSrvUsr_ChallengeResponseServer
{
  public:
    /*! \brief static member to create this object

               This function creates a instance of this object. An existing
               instance will be deleted.

        \param szDatabase   [IN] The name of the database instance.
        \param szUser       [IN] The name of the user.
        \param oMethods     [IN] A array with the methods which the client does support.
                                 If this array is empty the server select a method and initialise the
                                 request data with 0.
        \param oRequestData [IN] The request data from the client for each method in the oMethods array.
     */
    static DBMSrvUsr_ChallengeResponseServer * CreateInstance
      ( const char            * szDatabase,
        const char            * szUser,
        const Tools_Array<Tools_DynamicUTF8String> & oMethods,
        const Tools_Array<Tools_DynamicUTF8String> & oRequestData );

    /*! \brief static member to retrieve an existing instance
     */
    static DBMSrvUsr_ChallengeResponseServer * GiveInstance
      ( );

    /*! \brief static member to destroy an existing instance
     */
    static void DestroyInstance
      ( );

    /*! \brief check if the user is available

               Check with this function after the creation of the object
               whether the given user is available.
        \return true if user is available
     */
    bool IsUserAvailable
      ( ) {return (m_szUsername.Length() > 0);}

    /*! \brief check if a method is available

               Check with this function after the creation of the object
               whether a authentication method for the client is available 
               or not.
        \return true if method available
     */
    bool IsMethodAvailable
      ( ) {return (m_szMethod.Length() > 0);}

    /*! \brief  Gets the method which this server has selected

        \return A pointer to the challenge data. In case of errors this pointer maybe NULL.
     */
    const char * GetMethod
      ( ) {return m_szMethod.CharPtr();}

    /*! \brief  generate the challenge

                With this function the object generates a challenge with the data
                given by the CreateInstance member.

                Check before with IsUserAvailable and IsMethodAvailable whether the object
                is valid or not.
        \return A pointer to the challenge data.  In case of errors this maybe an empty string.
     */
    const char * GiveChallenge
      ( );

    /*! \brief  verify the response of the client

                This function verifies the response which was sent from the client.

        \param  szResponse [IN] the response data from the client

        \return True if the response was correct. 
     */
    bool VerifyResponse
      ( const char * szResponse );

    /*! \brief  get the name of the user

        \return A pointer to the user name which was given by CreateInstance. In case of errors this pointer maybe NULL.
     */
    const char * GetUser
      ( ) {return m_szUsername.CharPtr();}

  private:
    // constructor
    DBMSrvUsr_ChallengeResponseServer
      ( const char            * szDatabase,
        const char            * szUser,
        const Tools_Array<Tools_DynamicUTF8String> & oMethods,
        const Tools_Array<Tools_DynamicUTF8String> & oRequestData );

    // destructor
    ~DBMSrvUsr_ChallengeResponseServer
      ( );
    
    // check username
    bool CheckUser
      ( const char * szDatabaseIn, 
        const char * szUserIn ) const;

    RTESec_ServerAuthenticationBuffer m_oBuffer;

    Tools_DynamicUTF8String m_szUsername;
    Tools_DynamicUTF8String m_szDatabase;

    Tools_DynamicUTF8String m_szMethod;

    Tools_HexBuffer         m_oRequestData;
    Tools_HexBuffer         m_oChallenge;

    static DBMSrvUsr_ChallengeResponseServer * m_oInstance;

}; // end class DBMSrvUsr_ChallengeResponseServer

#endif // DBMSrvUsr_ChallengeResponseServer_hpp
