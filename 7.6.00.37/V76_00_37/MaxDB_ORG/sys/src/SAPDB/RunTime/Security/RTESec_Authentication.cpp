/*!*********************************************************************

    @file         RTESec_Authentication.cpp

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

#include "RunTime/Security/RTESec_Authentication.hpp"

#include <string.h>

#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/Security/Crypto/RTECrypto_RNG.hpp"
#include "RunTime/Security/RTESec_SCRAMMD5.h"
#include "RunTime/Security/RTESec_DumpHex.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_VarData.hpp"

/*----------------------------------------*/

static const char *
stripUnicodeBlanks (
    const char * password,
    const char * passwordEnd,
    char c0,
    char c1)
{
    while ((passwordEnd > password)
           && (passwordEnd [-1] == c0) && (passwordEnd [-2] == c1))
    {
        passwordEnd -= 2;
    }
    return passwordEnd;
}

/*----------------------------------------*/

static const char *
stripAsciiBlanks (
    const char * password,
    const char * passwordEnd)
{
    while ((passwordEnd > password) && (passwordEnd [-1] == ' '))
    {
        --passwordEnd;
    }
    return passwordEnd;
}

/*----------------------------------------*/

static int
stripTrailingBlanks (
    const void * password_in,
    int pwdlen)
{
    const char * password = (const char *) password_in;
    const char * passwordEnd = password + pwdlen;
    const char * strippedEnd;

    strippedEnd = stripUnicodeBlanks (password, passwordEnd, '\0', ' ');
    if (strippedEnd != passwordEnd) {
        return strippedEnd - password;
    }
    strippedEnd = stripUnicodeBlanks (password, passwordEnd, ' ', '\0');
    if (strippedEnd != passwordEnd) {
        return strippedEnd - password;
    }
    strippedEnd = stripAsciiBlanks (password, passwordEnd);
    return strippedEnd - password;
}

/*----------------------------------------*/

RTESec_Authentication::
RTESec_Authentication (
    const char * name_init)
: name (name_init)
{
}

/*----------------------------------------*/

RTESec_Authentication::
~RTESec_Authentication ()
{
}

/*----------------------------------------*/

const char * RTESec_Authentication::
Name () const
{
    return this->name;
}

/*----------------------------------------*/

/*!

  @class RTESec_ScramMD5

 */
class RTESec_ScramMD5
    : public RTESec_Authentication
{
public:
    RTESec_ScramMD5 (const char * name_init);

    virtual bool DataForChallengeRequest (
        RTESec_ClientAuthenticationBuffer & clientBuffer,
        const char *  username, int namelen,
        const void *& requestData, int & outlen,
        RTECrypto_RNG * randomGenerator = 0) const;

    virtual bool CreateChallenge (
        RTESec_ServerAuthenticationBuffer & serverBuffer,
        const void *& challenge, int & challengeLen,
        const void * seed, int seedLen,
        RTECrypto_RNG * randomGenerator = 0) const;

    virtual bool CreateResponse (
        RTESec_ClientAuthenticationBuffer & clientBuffer,
        const void *& response, int & responseLen,
        const void * serverChallenge, int serverChLen,
        const char * password, int passwordLen) const;

    virtual bool VerifyResponse (
        RTESec_ServerAuthenticationBuffer & serverBuffer,
        const void * clientResponse, int clientResponseLen,
        const void * password, int pwdlen) const;
};

/*----------------------------------------*/

/*!
  client buffer for ScramMD5
 */
typedef struct ScramMD5_ClientBufferT_ {
    char            requestData [64];
    SCRAMMD5_CLIENT responseData;
    unsigned char   serverproof [256];
} ScramMD5_ClientBufferT;


/*----------------------------------------*/

static const int challengeSizeC = 2 * SCRAMMD5_DATASIZE;
static const int challengeBufSizeC = SCRAMMD5_SALTSIZE
    + challengeSizeC + 2 /* countbytes */ + 2 /* length bytes */;

/*!
  server buffer for ScramMD5
 */
typedef struct ScramMD5_ServerBufferT {
    char            requestData [64];
    char            challenge [challengeBufSizeC];
    SCRAMMD5_CLIENT responseData;
    unsigned char   serverproof [256];
} ScramMD5_ServerBufferT;

/*----------------------------------------*/

RTESec_ScramMD5::
RTESec_ScramMD5 (
    const char * name_init)
: RTESec_Authentication (name_init)
{
}

/*----------------------------------------*/

bool RTESec_ScramMD5::
DataForChallengeRequest (
    RTESec_ClientAuthenticationBuffer & clientBuffer,
    const char *  username,
    int namelen,
    const void *& requestData,
    int & outlen,
    RTECrypto_RNG * randomGenerator) const
{
	RTECrypto_RNG *randomGeneratorFallback = 0;
    if (randomGenerator == 0) {
		randomGeneratorFallback = RTECrypto_RNG::createInstance( RTECrypto_RNG::SysRNG, RTEMem_RteAllocator::Instance( ));
		randomGenerator = randomGeneratorFallback;
    }

    ScramMD5_ClientBufferT * clientBuf = reinterpret_cast <ScramMD5_ClientBufferT*> (&clientBuffer);
    if (namelen > sizeof (clientBuf->requestData)) {
        namelen = sizeof (clientBuf->requestData);
    }
    memcpy (clientBuf->requestData, username, namelen);
    bool ok = randomGenerator->nextBytes (clientBuf->requestData + namelen,
        sizeof (clientBuf->requestData) - namelen);
    if (!ok && randomGeneratorFallback )
	{
		RTECrypto_RNG::destroy( randomGeneratorFallback );
		randomGeneratorFallback =
		RTECrypto_RNG::createInstance( RTECrypto_RNG::SHA1PRNG, RTEMem_RteAllocator::Instance( ));
		randomGenerator = randomGeneratorFallback;
		
		ok = randomGenerator->nextBytes (clientBuf->requestData + namelen,
        sizeof (clientBuf->requestData) - namelen);
    }

	if( randomGeneratorFallback )
		RTECrypto_RNG::destroy( randomGeneratorFallback );

    if (!ok) {
		return false;
    }

	requestData = clientBuf->requestData;
    outlen = sizeof (clientBuf->requestData);
    return true;
}

/*----------------------------------------*/

bool RTESec_ScramMD5::
CreateChallenge (
    RTESec_ServerAuthenticationBuffer & serverBuffer,
    const void *& challenge,
    int & challengeLen,
    const void * seed,
    int seedLen,
    RTECrypto_RNG * randomGenerator) const
{
	RTECrypto_RNG *randomGeneratorFallback = 0;
    if (randomGenerator == 0) {
		randomGeneratorFallback = RTECrypto_RNG::createInstance( RTECrypto_RNG::SysRNG, RTEMem_RteAllocator::Instance( ));
		randomGenerator = randomGeneratorFallback;
	}
    bool ok;

    ScramMD5_ServerBufferT * serverBuf = reinterpret_cast<ScramMD5_ServerBufferT*> (& serverBuffer);
    SAPDBFields_VarData::Writer writer (serverBuf->challenge, sizeof (serverBuf->challenge));
    /*
     * memorize client data
     */
    if (seedLen > sizeof (serverBuf->requestData)) {
	    if (randomGeneratorFallback != 0)
			RTECrypto_RNG::destroy( randomGenerator );

        return false;
    }

    memcpy (serverBuf->requestData, seed, seedLen);
    /*
     * set salt
     */
    void * target = writer.reserveField (SCRAMMD5_SALTSIZE);
    ok = randomGenerator->nextBytes (target, SCRAMMD5_SALTSIZE);
    if (!ok && randomGeneratorFallback )
	{
		RTECrypto_RNG::destroy( randomGeneratorFallback );
		randomGeneratorFallback =
		RTECrypto_RNG::createInstance( RTECrypto_RNG::SHA1PRNG, RTEMem_RteAllocator::Instance( ));
		randomGenerator = randomGeneratorFallback;

	    ok = randomGenerator->nextBytes (target, SCRAMMD5_SALTSIZE);
	}

	if (!ok) {
	    if (randomGeneratorFallback != 0)
			RTECrypto_RNG::destroy( randomGenerator );

		return false;
    }
    /*
     * set key
     */
    target = writer.reserveField (challengeSizeC);
    ok = randomGenerator->nextBytes (target, challengeSizeC);
    if (!ok && randomGeneratorFallback )
	{
		RTECrypto_RNG::destroy( randomGeneratorFallback );
		randomGeneratorFallback =
		RTECrypto_RNG::createInstance( RTECrypto_RNG::SHA1PRNG, RTEMem_RteAllocator::Instance( ));
		randomGenerator = randomGeneratorFallback;
	    ok = randomGenerator->nextBytes (target, challengeSizeC);
	}

	if (randomGeneratorFallback != 0)
		RTECrypto_RNG::destroy( randomGenerator );

	if (!ok) {
		return false;
    }

	/*
     * return
     */
    int fieldCount;
    writer.close (fieldCount, challengeLen);
    challenge = serverBuf->challenge;
    return true;
}

/*----------------------------------------*/

bool RTESec_ScramMD5::
CreateResponse (
    RTESec_ClientAuthenticationBuffer & clientBuffer,
    const void *& response,
    int & responseLen,
    const void * serverChallenge,
    int serverChLen,
    const char * password,
    int passwordLen) const

{
    ScramMD5_ClientBufferT * clientBuf = reinterpret_cast<ScramMD5_ClientBufferT*> (&clientBuffer);
    passwordLen = stripTrailingBlanks (password, passwordLen);
    const void * salt;
    int          saltlen;
    const void * challengeData;
    int          challengeDataLen;

    if (serverChLen == 40) {
        /*
         * format was first try at SCRAMMD5,
         * buffer with 8 bytes salt and 32 bytes challenge
         * sent by kernels up to 7.6.00.05
         */
        const char * rawChallenge = (const char *) serverChallenge;
        salt = rawChallenge;
        saltlen = SCRAMMD5_SALTSIZE;
        challengeData = rawChallenge + SCRAMMD5_SALTSIZE;
        challengeDataLen = serverChLen - saltlen;
    }
    else {
        /*
         * list sequence, salt and challenge as two separate fields
         */
        SAPDBFields_VarData::Reader reader (serverChallenge);
        if (!reader.next (salt, saltlen)) {
            return false;
        }
        if (!reader.next (challengeData, challengeDataLen)) {
            return false;
        }
    }
    int rc = SCRAMMD5GenClient (&clientBuf->responseData,
        clientBuf->requestData, sizeof (clientBuf->requestData),
        (const char *) salt, saltlen,
        (const char *) challengeData, challengeDataLen,
        password, passwordLen,
        clientBuf->serverproof);
    if (rc != 0) {
        return false;
    }
    response = &clientBuf->responseData;
    responseLen = SCRAMMD5_DATASIZE;
    return true;
}

/*----------------------------------------*/

bool RTESec_ScramMD5::
VerifyResponse (
    RTESec_ServerAuthenticationBuffer & serverBuffer,
    const void * clientResponse,
    int clientResponseLen,
    const void * password,
    int pwdlen) const
{
    ScramMD5_ServerBufferT * serverBuf = reinterpret_cast<ScramMD5_ServerBufferT*> (&serverBuffer);
    SCRAMMD5_VRFY verifier;

    if (clientResponseLen != sizeof (SCRAMMD5_CLIENT)) {
        return false;
    }
    pwdlen = stripTrailingBlanks (password, pwdlen);
    SAPDBFields_VarData::Reader reader (serverBuf->challenge);
    const void * salt;
    int          saltlen;
    const void * challengeData;
    int          challengeDataLen;
    if (!reader.next (salt, saltlen)) {
        return false;
    }
    if (!reader.next (challengeData, challengeDataLen)) {
        return false;
    }

    SCRAMMD5GenVerifier (&verifier, (const unsigned char *) salt, (const char*) password, pwdlen, 0);
    int rc = SCRAMMD5Gen ((SCRAMMD5_CLIENT*) clientResponse,
        serverBuf->requestData, sizeof (serverBuf->requestData),
        (const char *) salt, saltlen,
        (const char *) challengeData, challengeDataLen,
        (const char * )&verifier, sizeof (SCRAMMD5_VRFY), SCRAM_VERIFY,
        serverBuf->serverproof);
    return rc == 0;
}

/*----------------------------------------*/

static const RTESec_ScramMD5 scramMD5 ("SCRAMMD5"); ///< static object for SCRAMMD5

/*----------------------------------------*/

static const RTESec_Authentication * allAuthentications [] = {
    &scramMD5,
    0
};

/*----------------------------------------*/

const RTESec_Authentication * RTESec_Authentication::
FindByName (
    const char * name,
    int len)
{
    if (len == -1) {
        len = strlen (name);
    }
    RTESec_Authentication::Iterator iter;
    const RTESec_Authentication * current;
    while (iter.next (current)) {
        const char * currentName = current->Name ();
        if ((len == strlen (currentName)) && (memcmp (name, currentName, len) == 0) ) {
            return current;
        }
    }
    return 0;
}

/*----------------------------------------*/

RTESec_Authentication::Iterator::
Iterator ()
{
    this->index = 0;
}

/*----------------------------------------*/

bool RTESec_Authentication::Iterator::
next (
    const RTESec_Authentication * & current)
{
    if (allAuthentications [this->index] == 0) {
        return false;
    }
    current = allAuthentications [this->index];
    ++this->index;
    return true;
}

/*----------------------------------------*/

extern "C" const char *
RTESec_Authentication_Name (
    const RTESec_Authentication * authentication)
{
    return authentication->Name ();
}

/*----------------------------------------*/

extern "C" bool
RTESec_Authentication_DataForChallengeRequest (
    const RTESec_Authentication * authentication,
    RTESec_ClientAuthenticationBuffer * clientBuffer,
    const char  * username,
    int           namelen,
    const void ** buf,
    int  * outlen)
{
    return authentication->DataForChallengeRequest (*clientBuffer,
        username, namelen,
        *buf, *outlen);
}

/*----------------------------------------*/

extern "C" bool
RTESec_Authentication_CreateResponse (
    const RTESec_Authentication * authentication,
    RTESec_ClientAuthenticationBuffer * clientBuffer,
    const void ** response,
    int * responseLen,
    const void * serverChallenge,
    int serverChLen,
    const char * password,
    int passwordLen)
{
    return authentication->CreateResponse (
        *clientBuffer, *response, *responseLen,
        serverChallenge, serverChLen,
        password, passwordLen);
}

/*----------------------------------------*/

extern "C" bool
RTESec_Authentication_VerifyResponse (
    const RTESec_Authentication * authentication,
    RTESec_ServerAuthenticationBuffer * serverBuffer,
    const void * clientResponse,
    int clientResponseLen,
    const void * password,
    int pwdlen)
{
    return authentication->VerifyResponse (
        *serverBuffer,
        clientResponse, clientResponseLen,
        password, pwdlen);
}

/*----------------------------------------*/

extern "C" const RTESec_Authentication *
RTESec_Authentication_FindByName (const char * name, int len)
{
    return RTESec_Authentication::FindByName (name, len);
}

/*----------------------------------------*/

extern "C" int
RTESec_Authentication_Next (
    const RTESec_Authentication ** current,
    int * index)
{
    if (allAuthentications [*index] == NULL) {
        return 0;
    }
    *current = allAuthentications [*index];
    *index += 1;
    return 1;
}

/*----------------------------------------*/

#if defined (RTESec_DumpHex)

/* has been defined as an empty macro */

#else

#include <stdio.h>

/*
 This is an array of lines to be overwritten in cycles.
 That way, a debugger can show a bit of history.
 */
static char dumpLines [20] [300];
static int currentLine = 0;
static char * dumpLine;

void
RTESec_DumpHex (const char * comment,
    const void * buf_in,
    int          buflen)
{
    static const char * hexdigits = "0123456789abcdef";
    const unsigned char * buf = (const unsigned char *) buf_in;
    dumpLine = dumpLines [currentLine];
    sprintf (dumpLine, "%s (%d):\n", comment, buflen);
    unsigned char * target = (unsigned char *) dumpLine + strlen (dumpLine);
    for (int i = 0; i < buflen; ++i) {
        if (i % 16 == 0) {
            target [0] = ' ';
            ++target;
        }
        unsigned char current = buf [i];
        target [0] = hexdigits [current / 16];
        target [1] = hexdigits [current % 16];
        target += 2;
    }
    target [0] = '\0';
    fprintf (stdout, "%s\n", dumpLine);
    fflush (stdout);
    ++currentLine;
    if (currentLine >= 20) {
        currentLine = 0;
    }
}

#endif

/*----------------------------------------*/
