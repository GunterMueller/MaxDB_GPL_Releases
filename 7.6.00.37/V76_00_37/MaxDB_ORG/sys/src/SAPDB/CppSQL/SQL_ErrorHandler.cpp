/*!
 * \file    SQL_ErrorHandler.cpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Error handler
 */

/*

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



*/


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef SQLERRORHANDLER_HPP
#include "CppSQL/SQL_ErrorHandler.hpp"
#endif

#ifndef SQL_SESSIONCONTEXT_HPP
#include "CppSQL/SQL_SessionContext.hpp"
#endif


/************************************************************************/
/* Implementation of class SQL_ErrorHandler                             */
/************************************************************************/

const char * SQL_ErrorHandler::m_error_msg[SQLCLS_ErrCnt] = {
  "unknown error "                        , /* e_unknown_error         */
  "input parameter cannot be null value"  , /* e_inp_null_not_allowed  */
  "invalid numeric input parameter value" , /* e_inp_number_invalid    */
  "numeric input parameter overflow"      , /* e_inp_number_overflow   */
  "parameter list too long"               , /* e_param_list_too_long   */
  "parameter list too short"              , /* e_param_list_too_short  */
  "incompatible data types"               , /* e_incompatible_datatype */
  "too many host variables"               , /* e_too_many_hostvar      */
  "missing variable address"              , /* e_missing_variable_addr */
  "command too long"                      , /* e_cmd_too_long          */
  "     "                                 , /* e_copyop_not_allowed    */
  "input string truncated"                , /* e_inp_string_truncated  */
  "   "                                   , /* e_use_callstm           */
  "command not available"                 , /* e_cmd_not_available     */
  "connect not ok"                        , /* e_dbconnect_notok       */
  "connection down"                       , /* e_dbconnection_down     */
  "db shutdown"                           , /* e_dbshutdown            */
  "too many users"                        , /* e_too_many_users        */
  "connection error"                      , /* e_notok                 */
  "Unicode/ASCII convertion failed"       , /* e_uni_to_ascii_conv_failed PTS 1116580 2002-07-22 */
  "NONASCII character in ASCII column"    , /* e_nonascii_char            PTS 1121342 */
  "No writable buffer provided"             /* e_no_writable_buffer       PTS 1128431 */
};

/*----------------------------------------------------------------------*/

SQL_ErrorHandler::SQL_ErrorHandler() {
}

/*----------------------------------------------------------------------*/

const char * SQL_ErrorHandler::getErrorDesc(tgg00_BasisError e) {
  int idx = abs(e) - 27000;
  if ( !( (idx >= 0) && (idx <= SQLCLS_ErrCnt-1) ) ) {
    idx = 0;
  }
  return m_error_msg[idx];
}

