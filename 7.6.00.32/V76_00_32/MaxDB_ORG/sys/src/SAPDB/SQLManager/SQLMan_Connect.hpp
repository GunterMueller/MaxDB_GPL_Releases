/*!*********************************************************************

    @file         SQLMan_Connect.hpp

    @author       DanielD

    @brief        Kernel side of challenge / response

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#ifndef SQLMAN_CONNECT_HPP
#define SQLMAN_CONNECT_HPP

#include "gsp00.h"
#include "RunTime/Security/RTESec_Authentication.hpp"
#include "Messages/Msg_List.hpp"

class SQLMan_Context;

/*!

  @class SQLMan_Connect

 */
class SQLMan_Connect
{
public:
    SQLMan_Connect (SQLMan_Context & context_init, const tsp00_KnlIdentifier & userName_init);

    bool CreateChallenge (
        const void                * dataBuf,
        int                         dataLen,
        int                         fieldToSkip,
        const char               *& algorithm,
        const void               *& challengeOut,
        int                       & challengeLen);

    bool VerifyResponse (
        const tsp00_Name          & password,
        const void                * dataBuf,
        int                         dataLen,
        int                         fieldsToSkip);

    static SAPDB_Bool RegisterAuthenticationParameters (
        SAPDBErr_MessageList  & msgList);

    static bool BasicAuthenticationAllowed ();

    void markUsernameAsParam ();

    bool wasUsernameParam () const;

private:
    SQLMan_Context      & context;
    tsp00_KnlIdentifier   user;
    RTESec_ServerAuthenticationBuffer serverState;
    const RTESec_Authentication * authenticator;
    bool                  userNameWasParam;
};

#endif
