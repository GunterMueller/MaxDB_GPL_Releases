/*!
 * \file    SQL_ErrorHandler.hpp
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


#ifndef SQL_ERRORHANDLER_HPP
#define SQL_ERRORHANDLER_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef GGG00_H
#include "ggg00.h"
#endif


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

class SQL_SessionContext;


/************************************************************************/
/* Error codes                                                          */
/************************************************************************/

/*
  e_unknown_error            = -27000;
  e_inp_null_not_allowed     = -27001;
  e_inp_number_invalid       = -27002;
  e_inp_number_overflow      = -27003;
  e_param_list_too_long      = -27004;
  e_param_list_too_short     = -27005;
  e_incompatible_datatype    = -27006;
  e_too_many_hostvar         = -27007;
  e_missing_variable_addr    = -27008;
  e_cmd_too_long             = -27009;
  e_copyop_not_allowed       = -27010;
  e_inp_string_truncated     = -27011;
  e_use_callstm              = -27012;
  e_cmd_not_available        = -27013;
  e_uni_to_ascii_conv_failed = -27019; ** PTS 1116580 2002-07-22
*/

#define e_dbconnect_notok    -27014
#define e_dbconnection_down  -27015
#define e_dbshutdown         -27016
#define e_too_many_users     -27017
#define e_notok              -27018
#define e_nonascii_char      -27020 /* PTS 1121342 */
#define e_no_writable_buffer -27021 /* PTS 1128431 */
 

/************************************************************************/
/* Class SQL_ErrorHandler                                               */
/************************************************************************/

#define SQLCLS_ErrCnt 22 /* number of SQL class errors, PTS 1128431 */

class  SQL_ErrorHandler
{
  static const char * m_error_msg[SQLCLS_ErrCnt];
public:  
  SQL_ErrorHandler();
  const char * getErrorDesc(tgg00_BasisError e);
};


#endif /* SQL_ERRORHANDLER_HPP */

