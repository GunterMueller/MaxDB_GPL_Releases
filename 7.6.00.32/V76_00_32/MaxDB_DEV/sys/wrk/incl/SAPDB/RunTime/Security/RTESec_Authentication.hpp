/*!*********************************************************************

    @file         RTESec_Authentication.hpp

    @author       DanielD

    @brief

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

#ifndef RTESEC_AUTHENTICATION_HPP
#define RTESEC_AUTHENTICATION_HPP

/*!
    @brief a buffer to keep client state independent of the
        authentication algorithm
 */
typedef struct RTESec_ClientAuthenticationBuffer_ {
    unsigned char data [512];
} RTESec_ClientAuthenticationBuffer;


/*!
    @brief a buffer to keep server state independent of the
        authentication algorithm
 */
typedef struct RTESec_ServerAuthenticationBuffer_ {
    unsigned char data [512];
} RTESec_ServerAuthenticationBuffer;


class RTECrypto_RNG;
/*!

  @class RTESec_Authentication

 */
class RTESec_Authentication
{
public:
    /*!
        @brief return the name of the authentication algorithm.
     */
    virtual const char * Name () const;

    /*!
        @brief client: initialize a buffer with random content
     */
    virtual bool         DataForChallengeRequest (
        RTESec_ClientAuthenticationBuffer & clientBuffer,
        const char *  username, int namelen,
        const void *& requestData, int & outlen,
        RTECrypto_RNG * randomGenerator = 0) const = 0;

    /*!
        @brief server: create a challenge
     */
    virtual bool         CreateChallenge (
        RTESec_ServerAuthenticationBuffer & serverBuffer,
        const void *& challenge, int & challengeLen,
        const void * seed, int seedLen,
        RTECrypto_RNG * randomGenerator = 0) const = 0;

    /*!
        @brief client: create authentication packet
     */
    virtual bool         CreateResponse (
        RTESec_ClientAuthenticationBuffer & clientBuffer,
        const void *& response, int & responseLen,
        const void * serverChallenge, int serverChLen,
        const char * password, int passwordLen) const = 0;

    /*!
        @brief server: authenticate by verifying client response
     */
    virtual bool         VerifyResponse (
        RTESec_ServerAuthenticationBuffer & serverBuffer,
        const void * clientResponse, int clientResponseLen,
        const void * password, int pwdlen) const = 0;

    /*!
        @brief find a authentication algorithm by name
     */
    static const RTESec_Authentication * FindByName (const char * name, int len = -1);

    /*!
        @class Iterator
        @brief iterates through all available authentication algorithms
     */
    class Iterator
    {
    public:
        Iterator ();

        /*!
            @brief returns the next authentication algorithm
         */
        bool next (const RTESec_Authentication * & current);
    private:
        int index;
    };

protected:
    RTESec_Authentication (const char * name_init);
    virtual ~RTESec_Authentication ();

private:
    const char * name;
};

#endif
