/*!*********************************************************************

    @file         SQLMan_Connect.cpp

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

#include "SQLManager/SQLMan_Connect.hpp"

#include "SAPDBCommon/Fields/SAPDBFields_VarData.hpp"
#include "RunTime/Security/RTESec_Authentication.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "KernelCommon/Kernel_Random.hpp"

/*----------------------------------------*/

SQLMan_Connect::
SQLMan_Connect (
    SQLMan_Context            & context_init,
    const tsp00_KnlIdentifier & userName_init)
: context (context_init),
  user (userName_init),
  userNameWasParam (false)
{
}

/*----------------------------------------*/

typedef struct PropertyT {
    const char * name;
    int          nameLen;
    const void * data;
    int          dataLen;
} PropertyT;

/*----------------------------------------*/

static bool
nextProperty (
    SAPDBFields_VarData::Reader & reader,
    PropertyT                   & property)
{
    const void * nameBuf;
    int          indicator;

    if (reader.next (nameBuf, property.nameLen, indicator)) {
        property.name = (const char *) nameBuf;
    }
    else {
        return false;
    }
    if (reader.next (property.data, property.dataLen, indicator)) {
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------*/

typedef struct MethodConfigT {
    const SAPDB_UTF8 * parameterName;
    bool         isInitialized;
    int          length;
    char         value [256];
    void         load ();
    bool         valueExists (const char * searchString, int searchStringLen);
} MethodConfigT;

MethodConfigT authAllow = {
    (const SAPDB_UTF8 *) "AUTHENTICATION_ALLOW",
    false,
    0,
};

MethodConfigT authDeny = {
    (const SAPDB_UTF8 *) "AUTHENTICATION_DENY",
    false,
    0,
};

/*----------------------------------------*/

void MethodConfigT::
load ()
{
    RTEConf_ParameterAccess * parameters = RTEConf_ParameterAccess::Instance ();
    SAPDBErr_MessageList msgList;
    RTEConf_Parameter::String value;
    if (parameters->GetString (this->parameterName, value, msgList)) {
        this->length = strlen ((const char *) value);
        memcpy (this->value, value, this->length + 1);
    }
    else {
        this->value [0] = '\0';
        this->length = 0;
    }
    this->isInitialized = true;
}

/*----------------------------------------*/

bool MethodConfigT::
valueExists (
    const char * searchString,
    int searchStringLen)
{
    const char * eoConfig = this->value + this->length;
    const char * current = this->value;
    const char * eoCurrent;
    while (current < eoConfig) {
        eoCurrent = (const char *) memchr (current, ',', eoConfig - current);
        if (eoCurrent == NULL) {
            eoCurrent = eoConfig;
        }
        int currentLen = eoCurrent - current;
        if (currentLen == searchStringLen) {
            if (memcmp (current, searchString, currentLen) == 0) {
                return true;
            }
        }
        current = eoCurrent;
        if (current [0] == ',') {
            ++current;
        }
    }
    return false;
}

/*----------------------------------------*/

static SAPDB_Bool
updateAuthenticationParameter (
    RTEConf_Parameter::Name parameterName,
    RTEConf_Parameter::Type parameterKind,
    void                  * newValueVoid,
    void                  * context,
    SAPDBErr_MessageList  & msgList)
{
    if (parameterKind != RTEConf_Parameter::t_String) {
        return false;
    }
    MethodConfigT * methodConfig = (MethodConfigT*) context;
    const char    * newValue = (const char *) newValueVoid;
    methodConfig->length = (int) strlen ((const char *) newValue);
    memcpy (methodConfig->value, newValue, methodConfig->length + 1);
    methodConfig->isInitialized = true;
    return true;
}

/*----------------------------------------*/

static bool
authenticationMethodIsAllowed (
    const char * authName,
    int authNameLen)
{
    if (!authAllow.isInitialized) {
        authAllow.load ();
    }
    if (!authDeny.isInitialized) {
        authDeny.load ();
    }
    bool denied = authDeny.valueExists (authName, authNameLen);
    if (denied) {
        return false;
    }
    if (authAllow.length == 0) {
        return true;
    }
    bool allowed = authAllow.valueExists (authName, authNameLen);
    if (allowed) {
        return true;
    }
    return false;
}

/*----------------------------------------*/

bool SQLMan_Connect::
CreateChallenge (
    const void                * dataBuf,
    int                         dataLen,
    int                         fieldsToSkip,
    const char               *& algorithm,
    const void               *& challengeOut,
    int                       & challengeLenOut)
{
    SAPDBFields_VarData::Reader   reader (dataBuf);
    reader.skip (fieldsToSkip);
    /*
     * find algorithm
     */
    PropertyT    authScheme;


    this->authenticator = NULL;
    while ((this->authenticator == NULL) && nextProperty (reader, authScheme)) {
        this->authenticator = RTESec_Authentication::FindByName (
            authScheme.name, authScheme.nameLen);
        if (this->authenticator == NULL) {
            continue;
        }
        const char * authName = authenticator->Name ();
        int authNameLen = (int) strlen (authName);
        if (!authenticationMethodIsAllowed (authName, authNameLen)) {
            /* clear to try next authentication method */
            this->authenticator = NULL;
        }

    }
    if (this->authenticator == NULL) {
        return false;
    }
    /*
     * create challenge
     */
    this->authenticator->CreateChallenge (
        this->serverState, challengeOut, challengeLenOut,
        authScheme.data, authScheme.dataLen, &Kernel_Random::Generator);
    algorithm = this->authenticator->Name ();
    /*
     * return
     */
    return true;
}

/*----------------------------------------*/

bool SQLMan_Connect::
VerifyResponse (
    const tsp00_Name          & password,
    const void                * dataBuf,
    int                         dataLen,
    int                         fieldsToSkip)
{
    bool result;
    /*
     * get response data
     */
    SAPDBFields_VarData::Reader   reader (dataBuf);
    reader.skip (fieldsToSkip);
    PropertyT response;

    if (!nextProperty (reader, response)) {
        return false;
    }
    // TODO: verify that response.name and authenticator->Name () match
    /*
     * verify
     */
    result = this->authenticator->VerifyResponse (
        this->serverState,
        response.data, response.dataLen,
        &password, sizeof (password));
    return result;
}

/*----------------------------------------*/

SAPDB_Bool SQLMan_Connect::
RegisterAuthenticationParameters (
    SAPDBErr_MessageList  & msgList)
{
    SAPDB_Bool result = true;

    RTEConf_ParameterAccess * parameters = RTEConf_ParameterAccess::Instance ();
    result = parameters->RegisterUpdateFunction (
        authAllow.parameterName, updateAuthenticationParameter,
        &authAllow, msgList) && result;
    result = parameters->RegisterUpdateFunction (
        authDeny.parameterName, updateAuthenticationParameter,
        &authDeny, msgList) && result;
    return result;
}

/*----------------------------------------*/

bool SQLMan_Connect::
BasicAuthenticationAllowed ()
{
    return authenticationMethodIsAllowed ("BASIC", 5);
}

/*----------------------------------------*/

void SQLMan_Connect::
markUsernameAsParam ()
{
    this->userNameWasParam = true;
}

/*----------------------------------------*/

bool SQLMan_Connect::
wasUsernameParam () const
{
    return this->userNameWasParam;
}

