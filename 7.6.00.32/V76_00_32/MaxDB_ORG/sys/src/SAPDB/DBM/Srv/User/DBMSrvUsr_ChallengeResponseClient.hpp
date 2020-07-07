/*!
  \file    DBMSrvUsr_ChallengeResponseClient.hpp
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

#if !defined(DBMSrvUsr_ChallengeResponseClient_hpp)
#define DBMSrvUsr_ChallengeResponseClient_hpp

//#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "DBM/Srv/User/DBMSrvUsr_Authentications.hpp"
#include "SAPDB/ToolsCommon/Tools_HexBuffer.hpp"

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

/*! \brief A class to handle challenge/response.

    Please use the same object for one challnge-response action. The object
    stores data 
   
 */

class DBMSrvUsr_ChallengeResponseClient
{
  public:
    /*! \brief   constructor 
        \param   szUserPwd [IN] usr,pwd for which the object should calculate the data.
     */
    DBMSrvUsr_ChallengeResponseClient
      ( const char            * szUserPwd );

    /*! \brief   destructor
     */
    ~DBMSrvUsr_ChallengeResponseClient
      ( );

    /*! \brief   Generates the user_getchallenge command with arguments
        \return  A pointer to the command with arguments. This pointer will
                 be became invalid if the object will be destroyed.
     */
    const char * GiveGetChallengeCommand
      ( );

    /*! \brief   sets the get challenge answer.

                 After the user_getchallnege the client gets a answer from the server. The
                 client must give this answer (without the OK line) to this object before
                 generating the user_response command.

        \param   pAnswer [IN] a pointer to the answer
        \param   nLen    [IN] the length of the answer data
     */
    bool SetGetChallengeAnswer
      ( const char * pAnswer,
        int          nLen );

    /*! \brief   Generates the user_response command with arguments
        \return  A pointer to the command with arguments. This pointer will
                 be became invalid if the object will be destroyed.
     */
    const char * GiveResponseCommand
      ( );


  private:
    Tools_DynamicUTF8String   m_szGetChallengeCommand;
    Tools_DynamicUTF8String   m_szResponseCommand;

    tsp00_Namec               m_szUsername;
    tsp00_C64c                m_szPassword;

    Tools_DynamicUTF8String   m_szMethod;

    Tools_HexBuffer           m_oChallenge;

    DBMSrvUsr_Authentications m_oAuthentications;

}; // end class DBMSrvUsr_ChallengeResponseClient

#endif // DBMSrvUsr_ChallengeResponseClient_hpp
