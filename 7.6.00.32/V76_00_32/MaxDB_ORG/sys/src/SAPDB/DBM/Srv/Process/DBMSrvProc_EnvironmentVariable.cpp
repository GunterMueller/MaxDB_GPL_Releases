/*!
  @file           DBMSrvProc_EnvironmentVariable.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          change environment of a process - Implementation

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "hcn36.h"
#include "DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp"

#include "SAPDB/DBM/Srv/Process/DBMSrvProc_EnvironmentVariable.hpp"

// --------------------------------------------
// public construtor DBMSrvProc_EnvironmentVariable
// --------------------------------------------
DBMSrvProc_EnvironmentVariable :: DBMSrvProc_EnvironmentVariable
        ( const char     * sName )

          // initialization of members
          : m_sName         (NULL)
          , m_sOldValue     (NULL)
          , m_sCurrentValue (NULL)
          , m_oDone         (ManipulateNothing)
{
    if ((sName  != NULL) && (strlen(sName)  > 0)) {
        cn36_StrAllocCpy(m_sName, sName);
    } // end if
} // end DBMSrvProc_EnvironmentVariable :: DBMSrvProc_EnvironmentVariable

// --------------------------------------------
// public destructor DBMSrvProc_EnvironmentVariable
// --------------------------------------------
DBMSrvProc_EnvironmentVariable :: ~DBMSrvProc_EnvironmentVariable
        ( )
{
    // free memory
    cn36_StrDealloc(m_sName);
    cn36_StrDealloc(m_sOldValue);
    cn36_StrDealloc(m_sCurrentValue);
}  // end DBMSrvProc_EnvironmentVariable :: ~DBMSrvProc_EnvironmentVariable

// --------------------------------------------
// public member Manipulate
// --------------------------------------------
DBMSrvProc_EnvironmentVariable::ManipulateType DBMSrvProc_EnvironmentVariable :: Manipulate
        ( const char      * sValue,
          const char      * sSeparator,
          ManipulateType    oRequested,
          DBMSrvMsg_Error & oMessage )
{
    // reset the informations from the last manipulate
    m_oDone = ManipulateNothing;
    cn36_StrDealloc(m_sOldValue);

    // check name and requested operation
    if ((m_sName  == NULL) || (strlen(m_sName)  == 0) ||
        (oRequested == ManipulateNothing) || (oRequested == ManipulateError)) { 
        return m_oDone;
    } // end if

    // check value (depend from requested operation)
    if ( (oRequested != ManipulateRemove) && ((sValue == NULL) || (strlen(sValue) == 0))) {
        return m_oDone;
    } // end if

    // check and read the old value
    const char * pCurrent = CurrentValue();
    if (pCurrent != NULL) {
      cn36_StrAllocCpy(m_sOldValue, pCurrent);
    } // end if

    // set the new value
    m_oDone = SetNewValue(m_sName, sValue, sSeparator, m_sOldValue, oRequested, oMessage );

    // set out value
    return m_oDone;
} // end DBMSrvProc_EnvironmentVariable :: DBMSrvProc_EnvironmentVariable

// -------------------
// public member Reset
// -------------------
DBMSrvProc_EnvironmentVariable::ManipulateType DBMSrvProc_EnvironmentVariable :: Reset
        ( DBMSrvMsg_Error & oMessage )
{
    ManipulateType oDone = ManipulateNothing;

    switch (m_oDone) {
        case ManipulateAdd:
            oDone = ManipulateRemove;
            break;
        case ManipulateReplace:       
        case ManipulateAppend:        
        case ManipulatePrepend:       
            oDone = ManipulateReplace;
            break;
        case ManipulateRemove:
            oDone = ManipulateAdd;
            break;
    } // end switch

    if (oDone != ManipulateNothing) {
        oDone = SetNewValue(m_sName, m_sOldValue, NULL, CurrentValue(), oDone, oMessage);
    } // end if

    // reset the informations from the last manipulate (also in case of errors)
    m_oDone = ManipulateNothing;
    cn36_StrDealloc(m_sOldValue);

    return oDone;
} // end DBMSrvProc_EnvironmentVariable :: Reset

// ---------------------------
// private member CurrentValue
// ---------------------------
const char * DBMSrvProc_EnvironmentVariable :: CurrentValue
        (  )
{
    cn36_StrDealloc(m_sCurrentValue);

    const char * pRead  = cn36_GetEnv(m_sName);

    if (pRead != NULL) {
        cn36_StrAllocCpy(m_sCurrentValue, pRead);
    } // end if

    return m_sCurrentValue;
} // end DBMSrvProc_EnvironmentVariable :: CurrentValue

// --------------------------
// private member SetNewValue
// --------------------------
DBMSrvProc_EnvironmentVariable::ManipulateType DBMSrvProc_EnvironmentVariable :: SetNewValue
        ( const char      * sName,
          const char      * sValue,
          const char      * sSeparator,
          const char      * sOldValue,
          ManipulateType    oRequested,
          DBMSrvMsg_Error & oMessage ) const
{
    DBMSrvMsg_ErrorHandler errorHandler;

    char           * sNewValue = NULL;
    ManipulateType   oExpected = ManipulateNothing;

    errorHandler.activate();
    if (!cn36_StrAlloc(sNewValue, 1 + ((sValue     == NULL) ? 0 : strlen(sValue))
                                    + ((sSeparator == NULL) ? 0 : strlen(sSeparator))
                                    + ((sOldValue  == NULL) ? 0 : strlen(sOldValue )) )) {
        errorHandler.getLastError(oMessage);
        oMessage.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SETENV, Msg_Arg(SDBMSGTAG_DBMSRV_SETENV__VALUE, sName)));
        oExpected = ManipulateError;
    } // end if
    errorHandler.deactivate();

    if (oExpected == ManipulateError) {
        return oExpected;
    } // end if

    // prepare new value and compute expected action
    switch (oRequested) {
        case ManipulateAdd:           
            if (sOldValue != NULL) {
                oExpected = ManipulateNothing;
            } else {
                oExpected = ManipulateAdd;
                strcpy(sNewValue, sValue);
            } // end if
            break;

        case ManipulateReplace:       
            if (sOldValue == NULL) {
                oExpected = ManipulateNothing;
            } else {
                oExpected = ManipulateReplace;
                strcpy(sNewValue, sValue);
            } // end if
            break;

        case ManipulateAppend:        
            if (sOldValue == NULL) {
                oExpected = ManipulateNothing;
            } else {
                oExpected = ManipulateAppend;
                strcpy(sNewValue, sOldValue);
                strcat(sNewValue, ((sSeparator == NULL) ? "" : sSeparator));
                strcat(sNewValue, sValue);
            } // end if
            break;

        case ManipulatePrepend:       
            if (sOldValue == NULL) {
                oExpected = ManipulateNothing;
            } else {
                oExpected = ManipulatePrepend;
                strcpy(sNewValue, sValue);
                strcat(sNewValue, ((sSeparator == NULL) ? "" : sSeparator));
                strcat(sNewValue, sOldValue);
            } // end if
            break;

        case ManipulateReplaceOrAdd:  
            if (sOldValue == NULL) {
                oExpected = ManipulateAdd;
                strcpy(sNewValue, sValue);
            } else {
                oExpected = ManipulateReplace;
                strcpy(sNewValue, sValue);
            } // end if
            break;

        case ManipulateAppendOrAdd:   
            if (sOldValue == NULL) {
                oExpected = ManipulateAdd;
                strcpy(sNewValue, sValue);
            } else {
                oExpected = ManipulateAppend;
                strcpy(sNewValue, sOldValue);
                strcat(sNewValue, ((sSeparator == NULL) ? "" : sSeparator));
                strcat(sNewValue, sValue);
            } // end if
            break;

        case ManipulatePrependOrAdd:  
            if (sOldValue == NULL) {
                oExpected = ManipulateAdd;
                strcpy(sNewValue, sValue);
            } else {
                oExpected = ManipulatePrepend;
                strcpy(sNewValue, sValue);
                strcat(sNewValue, ((sSeparator == NULL) ? "" : sSeparator));
                strcat(sNewValue, sOldValue);
            } // end if
            break;

        case ManipulateRemove:        
            if (sOldValue == NULL) {
                oExpected = ManipulateNothing;
            } else {
                oExpected = ManipulateRemove;
            } // end if
            break;

    } // end switch

    // do nothing
    if (oExpected == ManipulateNothing) {
        cn36_StrDealloc(sNewValue);
        return oExpected;
    } // end if

    // unset variable
    if (oExpected == ManipulateRemove) {
        cn36_UnsetEnv(sName);
        cn36_StrDealloc(sNewValue);
        return oExpected;
    } // end if

    // set variable
    errorHandler.activate();
    if (cn36_PutEnv(sName, sNewValue) == 0) {
        errorHandler.getLastError(oMessage);
        oMessage.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SETENV, Msg_Arg(SDBMSGTAG_DBMSRV_SETENV__VALUE, sName)));
        oExpected = ManipulateError;
    } // end if
    errorHandler.deactivate();
    cn36_StrDealloc(sNewValue);
    return oExpected;

} // end DBMSrvProc_EnvironmentVariable :: SetNewValue
