/*!*****************************************************************************

  module      : X2AC_WarnAndErrHandler.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG(GoertzP)

  special area: Generator of application code ("COM routine wrappers")
  description : Error handling

  ----------------------------------------------------------------------------

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
    ========== licence end

*******************************************************************************/


#ifndef X2AC_WARNANDERRHANDLER_HPP
#define X2AC_WARNANDERRHANDLER_HPP


#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#include <fstream>
#define endl std::endl
#define cout std::cout
#else
#include <iostream.h>
#include <fstream.h>
#endif

#include <string>


#define X2AC_FILE_OPEN_ERROR           1
#define X2AC_BAD_TAG_USE               2
#define X2AC_BAD_ATTR_SEQUENCE         3
#define X2AC_BAD_ATTR_VALUE            4
#define X2AC_BAD_TAG                   5
#define X2AC_BAD_INPUT_PARAM_NO        6
#define X2AC_BAD_INPUT_FILE_NAME       7
#define X2AC_BAD_INPUT_PARAM           8
#define X2AC_INPUT_FILE_NAME_TOO_LONG  9
#define X2AC_XML_PARSE_ERROR          10
#define X2AC_BAD_FUNCTOR_HEADER       11 /* PTS */
#define X2AC_BAD_LIBRARY              12 /*  1  */
#define X2AC_INCCONTENT_INCOMPLETE    13 /*  1  */
#define X2AC_INCCONTENT_DUPLICATED    14 /*  4  */
#define X2AC_INCCONTENT_NOT_FIRST     15 /*  0  */
#define X2AC_NAME_TOO_LONG            16 /*  1  */
#define X2AC_IDL_INCOMPLETE           17 /*  8  */
#define X2AC_BAD_PARAM                18 /*  4  */

#define NUMBER_OF_X2AC_ERRORS         18


void X2AC_ErrMsg( const int errNo);
void X2AC_ErrMsg( const int errNo, const char * p1 );
void X2AC_ErrMsg( const int errNo, const int p1 ); // PTS 1140184
void X2AC_ErrMsg( const int errNo, const char * p1, const char * p2 );
void X2AC_ErrMsg( const int errNo, const char * p1, const int p2 );
void X2AC_ErrMsg( const int errNo, const int p1, const char * p2 ); // PTS 1140184
void X2AC_ErrMsg( const int errNo, const char * p1, const char * p2, const int p3 );
void X2AC_ErrMsg( const int errNo, const char * p1, const int p2, const char * p3 ); // PTS 1140184


#endif /* X2AC_WARNANDERRHANDLER_HPP */

