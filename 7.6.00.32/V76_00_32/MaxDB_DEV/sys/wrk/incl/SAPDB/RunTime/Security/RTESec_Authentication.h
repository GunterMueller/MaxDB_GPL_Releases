/*!*********************************************************************

    @file         RTESec_Authentication.h

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

#ifndef RTESEC_AUTHENTICATION_H
#define RTESEC_AUTHENTICATION_H

/*!
    @brief a buffer to keep client state independent of the
        authentication algorithm
 */
typedef struct RTESec_ClientAuthenticationBuffer {
    unsigned char data [512];
} RTESec_ClientAuthenticationBuffer;


/*!
    @brief a buffer to keep server state independent of the
        authentication algorithm
 */
typedef struct RTESec_ServerAuthenticationBuffer {
    unsigned char data [512];
} RTESec_ServerAuthenticationBuffer;


typedef struct RTESec_Authentication RTESec_Authentication;

const char * RTESec_Authentication_Name (
    const RTESec_Authentication * authentication);

bool RTESec_Authentication_DataForChallengeRequest (
    const RTESec_Authentication * authentication,
    RTESec_ClientAuthenticationBuffer * clientBuffer,
    const char  * username,
    int           namelen,
    const void ** buf,
    int  * outlen);

bool RTESec_Authentication_CreateResponse (
    const RTESec_Authentication * authentication,
    RTESec_ClientAuthenticationBuffer * clientBuffer,
    const void ** response,
    int * responseLen,
    const void * serverChallenge,
    int serverChLen,
    const char * password,
    int passwordLen);

bool RTESec_Authentication_VerifyResponse (
    const RTESec_Authentication * authentication,
    RTESec_ServerAuthenticationBuffer * serverBuffer,
    const void * clientResponse, int clientResponseLen,
    const void * password, int pwdlen);

const RTESec_Authentication * RTESec_Authentication_FindByName (
    const char * name,
    int len);

int RTESec_Authentication_Next (
    const RTESec_Authentication ** current,
    int * index);

#endif
