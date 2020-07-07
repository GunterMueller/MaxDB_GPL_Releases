/*!
  @file           DBMSrvPar_RTEConfParameter.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer database parameters handling - Implementation

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
#include "DBM/Srv/Parameters/DBMSrvPar_RTEConfParameter.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp"
#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"

/*-----------------------------------------------------------------------------*/
/*! @brief public constructor */
DBMSrvPar_RTEConfParameter::DBMSrvPar_RTEConfParameter
    ( const SAPDB_Char * dbname, 
            SAPDB_Int4   version,
            SAPDB_Int4   maxLengthOfName,
            SAPDB_Int4   maxLengthOfStringValue )
            : RTEConf_Parameter(dbname, version, maxLengthOfName, maxLengthOfStringValue)
{
    szDbName.rawAssign(dbname);
} // end DBMSrvPar_RTEConfParameter

SAPDB_Bool DBMSrvPar_RTEConfParameter::Read
    ( DBMSrvMsg_Error & err )
{
    DBMSrvMsg_Error initResourceLockError;
    DBMSrvShM_LineAccessorResourceLock accessParam(DBMSrvShM_InternalResource::ParameterFileAccess, initResourceLockError);

    if( !initResourceLockError.IsEmpty() ) {
        err.Overrule(initResourceLockError);
        return false;
    } // end if
    
    // resource lock "parameter file" successfully created
    
    if( accessParam.lock(err) != DBMSrvShM_LineAccessorResourceLock::LockStateLocked ) {
        return false;
    } // end if
    
    // parameter file successfully locked
    
    // read the parameters
    SAPDB_Bool dataRead;
    if (!RTEConf_Parameter::Read(dataRead, err)) {
        DBMSrvMsg_Error tmpError;
        accessParam.unlock(tmpError);
        
        err = DBMSrvMsg_RTEError(err);
        // exists configuration?
        if (!dataRead) {
            err.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN, Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME, szDbName)));
        } // end if
        return false;
    } else if (!dataRead) {
        err = DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN, Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME, szDbName));
        return false;
    } // end if
    
    if( accessParam.unlock(err) != DBMSrvShM_LineAccessorResourceLock::LockStateNotLocked ) {
        return false;
    } // end if
    
    return true;
} // end DBMSrvPar_RTEConfParameter::Read

SAPDB_Bool DBMSrvPar_RTEConfParameter::Write
    ( DBMSrvMsg_Error & err )
{
    DBMSrvMsg_Error initResourceLockError;
    DBMSrvShM_LineAccessorResourceLock accessParam(DBMSrvShM_InternalResource::ParameterFileAccess, initResourceLockError);

    if( !initResourceLockError.IsEmpty() ) {
        err.Overrule(initResourceLockError);
        return false;
    }
    
    // resource lock "parameter file" successfully created
    
    if( accessParam.lock(err) != DBMSrvShM_LineAccessorResourceLock::LockStateLocked ) {
        return false;
    } // end if
    
    // parameter file successfully locked
    
    // write the parameters
    if (!RTEConf_Parameter::Write(err)) {
        DBMSrvMsg_Error tmpError;
        accessParam.unlock(tmpError);
        err = DBMSrvMsg_RTEError(err);
        return false;
    } // end if
    
    if( accessParam.unlock(err) != DBMSrvShM_LineAccessorResourceLock::LockStateNotLocked ) {
        return false;
    } // end if
    
    return true;
} // end DBMSrvPar_RTEConfParameter::Write

SAPDB_Bool DBMSrvPar_RTEConfParameter::GetValue
	( Name              name, 
	  Integer         & value, 
	  DBMSrvMsg_Error & err ) 
{
	if (!RTEConf_Parameter::GetValue(name, value, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::GetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::GetValue
	( Name              name, 
	  Real            & value, 
	  DBMSrvMsg_Error & err ) 
{
	if (!RTEConf_Parameter::GetValue(name, value, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::GetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::GetValue
	( Name              name, 
	  String            value, 
	  DBMSrvMsg_Error & err ) 
{
	if (!RTEConf_Parameter::GetValue(name, value, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::GetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::GetValue
	( Name              name, 
	  CryptInfo       & value, 
	  DBMSrvMsg_Error & err ) 
{
	if (!RTEConf_Parameter::GetValue(name, value, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::GetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::GetRundirectoryAbsolutePath( String value,DBMSrvMsg_Error & err)
{
    if (!RTEConf_Parameter::GetRundirectoryAbsolutePath(value, err)) {
        return GenerateParamError(err, KERNELPARAM_RUNDIRECTORY);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::GetRundirectoryAbsolutePath

SAPDB_Bool DBMSrvPar_RTEConfParameter::SetValue
	( Name              name, 
	  const Integer   & value, 
	  SAPDB_Bool        updateOnlinePossible,
	  SAPDB_Bool        permanentChange, 
	  DBMSrvMsg_Error & err)
{
	if (!RTEConf_Parameter::SetValue(name, value, updateOnlinePossible, permanentChange, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::SetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::SetValue
	( Name              name, 
	  const Real      & value, 
	  SAPDB_Bool        updateOnlinePossible,
	  SAPDB_Bool        permanentChange, 
	  DBMSrvMsg_Error & err)
{
	if (!RTEConf_Parameter::SetValue(name, value, updateOnlinePossible, permanentChange, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::SetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::SetValue
	( Name              name, 
	  const String    & value, 
	  SAPDB_Bool        updateOnlinePossible,
	  SAPDB_Bool        permanentChange, 
	  DBMSrvMsg_Error & err)
{
	if (!RTEConf_Parameter::SetValue(name, value, updateOnlinePossible, permanentChange, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::SetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::SetValue
	( Name              name, 
	  const CryptInfo & value, 
	  SAPDB_Bool        updateOnlinePossible,
	  SAPDB_Bool        permanentChange, 
	  DBMSrvMsg_Error & err)
{
	if (!RTEConf_Parameter::SetValue(name, value, updateOnlinePossible, permanentChange, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::SetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::SetValue
	( Name              name, 
	  const Integer   & value, 
	  DBMSrvMsg_Error & err)
{
	if (!RTEConf_Parameter::SetValue(name, value, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::SetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::SetValue
	( Name              name, 
	  const String    & value, 
	  DBMSrvMsg_Error & err)
{
	if (!RTEConf_Parameter::SetValue(name, value, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::SetValue

SAPDB_Bool DBMSrvPar_RTEConfParameter::Delete 
	( Name       name, 
	  DBMSrvMsg_Error & err)
{
	if (!RTEConf_Parameter::Delete(name, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::Delete

SAPDB_Bool DBMSrvPar_RTEConfParameter::GetType
	( Name       name, 
	  Type     & type,  
	  DBMSrvMsg_Error & err)
{
	if (!RTEConf_Parameter::GetType(name, type, err)) {
        return GenerateParamError(err, name);
	} // end if
	return true;
} // end DBMSrvPar_RTEConfParameter::GetType

SAPDB_Bool DBMSrvPar_RTEConfParameter::GenerateParamError
    ( DBMSrvMsg_Error & err,
      Name              name ) 
{
    if (err.ID() == RTEERR_CONF_NOT_FOUND_ID) {
        err = DBMSrvMsg_RTEError(err);
        err.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, name)));
    } else {
        err = DBMSrvMsg_RTEError(err);
    } // end if
    return false;
} // end DBMSrvPar_RTEConfParameter::GenerateParamError
