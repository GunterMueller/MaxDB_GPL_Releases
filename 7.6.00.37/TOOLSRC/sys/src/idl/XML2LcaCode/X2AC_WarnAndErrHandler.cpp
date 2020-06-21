/*!*****************************************************************************

  module      : X2AC_WarnAndErrHandler.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

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


#include "idl/XML2LcaCode/X2AC_WarnAndErrHandler.hpp"


static const char * EMsgPrefix = "XML2LcaCode Error ";


static const char * X2AC_ErrorMsg[NUMBER_OF_X2AC_ERRORS] = {
 "File %s can not be opened!",                                                  // X2AC_FILE_OPEN_ERROR
 "Bad use of tag %s!",                                                          // X2AC_BAD_TAG_USE
 "Bad attribute sequence for tag %s at line %d!",                               // X2AC_BAD_ATTR_SEQUENCE
 "Bad value for attribute %s of tag %s at line %d!",                            // X2AC_BAD_ATTR_VALUE
 "Not supported tag %s!",                                                       // X2AC_BAD_TAG
 "Bad number of input paramaters, min. 1, max. 2!",                             // X2AC_BAD_INPUT_PARAM_NO
 "First input parameter must be of form -F<...>_..._<...>__<...>_methods.xml!", // X2AC_BAD_INPUT_FILE_NAME
 "Not supported input parameter %s!",                                           // X2AC_BAD_INPUT_PARAM
 "Input file name too long!",                                                   // X2AC_INPUT_FILE_NAME_TOO_LONG
 "XML Parse Error \"%s\" at line %d!",                                          // X2AC_XML_PARSE_ERROR
 "Bad functor header %s!",                                                      // X2AC_BAD_FUNCTOR_HEADER
 "Bad library %s!",                                                             // X2AC_BAD_LIBRARY
 "INCCONTENT tag incomplete!",                                                  // X2AC_INCCONTENT_INCOMPLETE
 "INCCONTENT tag specified twice!",                                             // X2AC_INCCONTENT_DUPLICATED
 "INCCONTENT tag must be first tag!",                                           // X2AC_INCCONTENT_NOT_FIRST
 "Name at line %d too long: %s!",                                               // X2AC_NAME_TOO_LONG
 "IDL tag at line %d incomplete!",                                              // X2AC_IDL_INCOMPLETE
 "Bad %s parameter at line %d: %s!"                                             // X2AC_BAD_PARAM
};

/*----------------------------------------------------------------------------*/

void X2AC_ErrMsg(const int errNo) {
  if ( (0 < errNo) && (errNo < NUMBER_OF_X2AC_ERRORS+1) ) {
    cout << EMsgPrefix << errNo << ": " << X2AC_ErrorMsg[errNo-1] << endl;  
  }
}

/*----------------------------------------------------------------------------*/

void X2AC_ErrMsg( const int errNo, const char * p1 ) {
  if ( (0 < errNo) && (errNo < NUMBER_OF_X2AC_ERRORS+1) ) {
    char buffer[256];
    if (p1) {
      sprintf( buffer, X2AC_ErrorMsg[errNo-1], p1 );
      cout << EMsgPrefix << errNo << ": " << buffer << endl;  
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2AC_ErrMsg( const int errNo, const int p1 ) {
  if ( (0 < errNo) && (errNo < NUMBER_OF_X2AC_ERRORS+1) ) {
    char buffer[256];
    if (p1) {
      sprintf( buffer, X2AC_ErrorMsg[errNo-1], p1 );
      cout << EMsgPrefix << errNo << ": " << buffer << endl;  
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2AC_ErrMsg( const int errNo, const char * p1, const char * p2 ) {
  if ( (0 < errNo) && (errNo < NUMBER_OF_X2AC_ERRORS+1) ) {
    char buffer[256];
    if ( p1 && p2 ) {
      sprintf( buffer, X2AC_ErrorMsg[errNo-1], p1, p2 );
      cout << EMsgPrefix << errNo << ": " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2AC_ErrMsg( const int errNo, const char * p1, const int p2 ) {
  if ( (0 < errNo) && (errNo < NUMBER_OF_X2AC_ERRORS+1) ) {
    char buffer[256];
    if (p1) {
      sprintf( buffer, X2AC_ErrorMsg[errNo-1], p1, p2 );
      cout << EMsgPrefix << errNo << ": " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2AC_ErrMsg( const int errNo, const int p1, const char * p2 ) {
  if ( (0 < errNo) && (errNo < NUMBER_OF_X2AC_ERRORS+1) ) {
    char buffer[256];
    if (p1) {
      sprintf( buffer, X2AC_ErrorMsg[errNo-1], p1, p2 );
      cout << EMsgPrefix << errNo << ": " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2AC_ErrMsg( const int errNo, const char * p1, const char * p2, const int p3 ) {
  if ( (0 < errNo) && (errNo < NUMBER_OF_X2AC_ERRORS+1) ) {
    char buffer[256];
    if ( p1 && p2 ) {
      sprintf( buffer, X2AC_ErrorMsg[errNo-1], p1, p2, p3 );
      cout << EMsgPrefix << errNo << ": " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2AC_ErrMsg( const int errNo, const char * p1, const int p2, const char * p3 ) {
  if ( (0 < errNo) && (errNo < NUMBER_OF_X2AC_ERRORS+1) ) {
    char buffer[256];
    if ( p1 && p2 ) {
      sprintf( buffer, X2AC_ErrorMsg[errNo-1], p1, p2, p3 );
      cout << EMsgPrefix << errNo << ": " << buffer << endl;
    }
  }
}

