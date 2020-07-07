/*!
  @file           DBMSrvPar_RTEConfParameter.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer database parameters handling - Specification

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

#ifndef _DBMSrvPar_RTEConfParameter_HPP_
#define _DBMSrvPar_RTEConfParameter_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*! @brief wrapper for RTE parameter file class RTEConf_Parameter 

    The original class RTEConf_Parameter does not support any locks
    during read and write the parameter file. This class overwrites
    the read and write method with an own version using locks
 */
class DBMSrvPar_RTEConfParameter : protected RTEConf_Parameter
{
public:

  /* @brief constructor 
     
     This constructor does only call the original construtor of 
     RTEConf_Parameter.

     See class RTEConf_Parameter ( file RTEConf_ParameterAccess.hpp) for more information.
   */
  DBMSrvPar_RTEConfParameter
    ( const SAPDB_Char * dbname, 
            SAPDB_Int4   version,
            SAPDB_Int4   maxLengthOfName,
            SAPDB_Int4   maxLengthOfStringValue);

  /* @brief Read method
     
     This method sets the lock "ParamAccess". After this the original method
     RTEConf_Parameter::Read will be called. 
     
     See class RTEConf_Parameter ( file RTEConf_ParameterAccess.hpp) for more information.
   */
  SAPDB_Bool Read
    ( DBMSrvMsg_Error & err );

  /* @brief Read method
     
     This method sets the lock "ParamAccess". After this the original method
     RTEConf_Parameter::Write will be called. 
     
     See class RTEConf_Parameter ( file RTEConf_ParameterAccess.hpp) for more information.
   */
  SAPDB_Bool Write
    ( DBMSrvMsg_Error & err );
	
	
	using        RTEConf_Parameter::StringElement;
	using        RTEConf_Parameter::Integer;
	using        RTEConf_Parameter::Real;
	using        RTEConf_Parameter::String;
	using        RTEConf_Parameter::CryptInfo;
	using        RTEConf_Parameter::TimeStamp;
	using        RTEConf_Parameter::Name;
	
	using        RTEConf_Parameter::Type;
	using        RTEConf_Parameter::t_Integer;
	using        RTEConf_Parameter::t_Real;
	using        RTEConf_Parameter::t_String;
	using        RTEConf_Parameter::t_CryptInfo;
	using        RTEConf_Parameter::t_Invalid;
	
	SAPDB_Bool GetValue(Name name, Integer   & value, DBMSrvMsg_Error & err);
	SAPDB_Bool GetValue(Name name, Real      & value, DBMSrvMsg_Error & err);
	SAPDB_Bool GetValue(Name name, String      value, DBMSrvMsg_Error & err); 
	SAPDB_Bool GetValue(Name name, CryptInfo & value, DBMSrvMsg_Error & err);
	
	RTEConf_ParameterIterator GetIterator(SAPDB_UTF8 *beginningOfParameterName = NULL)
	{
		return RTEConf_Parameter::GetIterator(beginningOfParameterName);
	} // end GetIterator
	
	SAPDB_Bool UpdateOnlinePossible(Name name)
	{
		return RTEConf_Parameter::UpdateOnlinePossible(name);
	} // end UpdateOnlinePossible

	SAPDB_Bool GetLastVerifyTime(TimeStamp & timeStamp)
	{
		return RTEConf_Parameter::GetLastVerifyTime(timeStamp);
	} // end GetLastVerifyTime
	
	SAPDB_Bool GetOriginalCrashFlag()
	{
		return RTEConf_Parameter::GetOriginalCrashFlag();
	} // end GetOriginalCrashFlag
		
	SAPDB_Bool SetValue(Name name, const Integer   & value,SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,DBMSrvMsg_Error & err);
	SAPDB_Bool SetValue(Name name, const Real      & value,SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,DBMSrvMsg_Error & err);
	SAPDB_Bool SetValue(Name name, const String    & value,SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,DBMSrvMsg_Error & err);
	SAPDB_Bool SetValue(Name name, const CryptInfo & value,SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,DBMSrvMsg_Error & err);

	SAPDB_Bool SetValue(Name name, const Integer   & value,DBMSrvMsg_Error & err);
	SAPDB_Bool SetValue(Name name, const String    & value,DBMSrvMsg_Error & err);
	
	SAPDB_Bool Delete ( Name name,               DBMSrvMsg_Error & err);
	SAPDB_Bool GetType( Name name, Type & type,  DBMSrvMsg_Error & err);
    
    SAPDB_Bool GetRundirectoryAbsolutePath( String value,DBMSrvMsg_Error & err); 
private:
    SAPDB_Bool GenerateParamError ( DBMSrvMsg_Error & err, Name name );
    
    tsp00_DbNamec szDbName; 

	
}; // end class DBMSrvPar_Parameters

#endif /* _DBMSrvPar_RTEConfParameter_HPP_ */

