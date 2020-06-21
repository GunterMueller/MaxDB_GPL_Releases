/*!*****************************************************************************

  module      : IspcErrorHandling.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
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


#include <stdio.h>
#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#include <fstream>
#define endl std::endl
#define cout std::cout
#else
#include <iostream.h>
#endif


#include "idl/xml2ispc/IspcErrorHandling.h"


extern int ispc_error_lvl; /* PTS 1122587 2003-Nov-11 FF */

static const char *EMsgPrefix = "xml2ispc -E- code:";
static const char *WMsgPrefix = "xml2ispc -W- code:";

static const char* ispc_error_msg[C_NUMBER_OF_ERROR_ISPC] = {
 "invalid call parameters passed \n\n   USAGE:xml2ispc [-w] [-m] <xml-file output of idlchecker> DBPROC | UDT\n"
   "   -w generate warnings\n"
   "   -m generate multiple offsets ( server, ASCII/Unicode client )",                        /* C_WRONG_PARAMETERS_ISPC              */
 "XML Parse Error: %s at line %d",                                                            /* C_XML_PARSE_ERROR_ISPC               */
 "method: %s, first parameter MUST be of type VT_USERDEFINED; type found %d",                 /* C_P1_NO_USERDEFINED_TYPE_ISPC        */
 "method: %s, first parameter MUST not be of I/O type INPUT",                                 /* C_P1_IO_TYPE_INPUT_ISPC              */
 "method: %s, first parameter MUST not be of I/O type OUTPUT",                                /* C_P1_IO_TYPE_OUTPUT_ISPC             */
 "method: %s, first parameter has wrong uid",                                                 /* C_P1_WRONG_GUID_ISPC                 */
 "method: %s, first parameter has no uid assigned",                                           /* C_P1_NO_GUID_ISPC                    */
 "method: %s, type specification of first parameter incomplete",                              /* C_P1_TYPE_SPEC_INCOMPLETE_ISPC       */
 "%s, type '%s' is unknown or not supported",                                                 /* C_TYPE_UNKNOWN_ISPC                  */
 "method: %s, unsupported parameter type, VT_CARRAY with elements of type '%s'",              /* C_UNSUPPORTED_VT_CARRAY1_ISPC        */
 "method: %s, directional spec. on parameter %s missing, [IN,OUT] assumed",                   /* C_NO_DIR_SPECIFIC_ISPC               */
 "method: %s, structure or array %s must be passed by pointer",                               /* C_PASS_STRUCT_BY_PTR_ISPC            */
 "method: %s, parameter: %s, scalar datatype '%s' passed by ptr but parameter is input only", /* C_IN_SCALAR_BYPTR_ISPC               */
 "method: %s, parameter: %s, datatype '%s' not allowed",                                      /* C_UNSUPPORTED_PARAM_TYPE_ISPC        */
 "method: %s, parameter: %s, pointer of pointer of datatype '%s' not allowed",                /* C_PTR_OF_PTR_UNSUPPORTED_ISPC        */
 "method: %s, parameter: %s, unsupported type, VT_CARRAY with dimension > 1",                 /* C_UNSUPPORTED_VT_CARRAY2_ISPC        */
 "%s, type '%s' is NOT portable",                                                             /* C_TYPE_NOT_PORTABLE_ISPC             */
 "method: %s, parameter:'%s' member:'%s' not properly aligned",                               /* C_STRUCT_NOT_ALIGNED_ISPC            */
 "method: %s, parameter:'%s' member:'%s' pointer within struct not allowed",                  /* C_POINTER_NOT_ALLOWED_ISPC           */
 "dimension: '%s' of struct_member: '%s', type '%s' is undefined",                            /* C_DIMENSION_TYPE_UNDEF_ISPC          */
 "invalid expression: '%s' defining CONST with name: '%s', type '%s' failed",                 /* C_CONST_EXPR_NOT_VALID_ISPC          */
 "duplicate definition of '%s'",                                                              /* C_DUP_DEFINITION_ISPC                */
 "dimension in typedef from '%s' to '%s' disallowed",                                         /* C_DIMENSION_IN_TYPEDEF               */
 "duplicate operator specification in definition of constant '%s'",                           /* C_DUPLICATE_OPERATOR_SPECIFICATION   */
 "duplicate specification of 1st or 2nd operand in definition of constant '%s'",              /* C_DUPLICATE_OPERAND_SPECIFICATION    */
 "too many operands in definition of constant '%s' specified",                                /* C_TOO_MANY_OPERANDS                  */
 "operator not specified in definition of constant '%s'",                                     /* C_MISSING_OPERATOR                   */
 "illegal use of element EXPR in definition of constant '%s'",                                /* C_ILLEGAL_EXPR_ELEMENT               */
 "no value for EXPRESSION or NUMBER for constant '%s' calculated",                            /* C_MISSING_CONST_VALUE                */
 "illegal order of attributes in TYPEDEF with NAME '%s'",                                     /* C_ILLEGAL_ATTRORDER_IN_TYPEDEF       */
 "illegal order of attributes in STRUCT with NAME '%s'",                                      /* C_ILLEGAL_ATTRORDER_IN_STRUCT        */
 "illegal order of attributes in CONST with NAME '%s'",                                       /* C_ILLEGAL_ATTRORDER_IN_CONST         */
 "illegal order of attributes in STRUCT MEMBER with NAME '%s'",                               /* C_ILLEGAL_ATTRORDER_IN_STRUCTMEMBER  */
 "illegal order of attributes in ENUM MEMBER with NAME '%s'",                                 /* C_ILLEGAL_ATTRORDER_IN_ENUMMEMBER    */
 "TYPE '%s' for MEMBER '%s' of STRUCT '%s' not found",                                        /* C_MISSING_STRUCTMEMBER_TYPE          */
 "illegal order of attributes in INTERFACE with NAME '%s'",                                   /* C_ILLEGAL_ATTRORDER_IN_INTERFACE     */
 "illegal order of attributes in coclass with NAME '%s'",                                     /* C_ILLEGAL_ATTRORDER_IN_COCLASS       */
 "duplicate type specification for MEMBER '%s' of STRUCT '%s'",                               /* C_DUPLICATE_MEMBERTYPE_SPECIFICATION */
 "not all (sub) member types of STRUCT '%s' registered, no column enum generated",            /* C_NOT_ALL_MEMBERTYPES_REGISTERED     */
 "tag CPPSQLDBC not supported",                                                               /* C_CPPSQLDBC_NOT_SUPPORTED            */
 "only tag CPPSQLDBC supported",                                                              /* C_ONLY_CPPSQLDBC_SUPPORTED           */
 "invalid declaration of union '%s'",                                                         /* C_BAD_UNION_DECLARATION              */
 "invalid type name '%s', pre-fix 'OmsTypeOid' may not be used for user defined types"        /* C_BAD_OMSTYPEOID_ISPC                */
};

/*----------------------------------------------------------------------------*/

void Ispc_ErrorMsg(const int myerrno) {
  if ( (0 < myerrno) && (myerrno <= C_NUMBER_OF_ERROR_ISPC) ) {
    cout << EMsgPrefix << myerrno << " " << ispc_error_msg[myerrno-1] << endl;
  }
}

/*----------------------------------------------------------------------------*/

void Ispc_ErrorMsg( const int myerrno, const char * p1 ) {
  ispc_error_lvl = 8; /* PTS 1122587 2003-Nov-11 FF */
  if ( (0 < myerrno) && (myerrno <= C_NUMBER_OF_ERROR_ISPC) ) {
    char buffer[256];
    if (p1) {
      sprintf( buffer, ispc_error_msg[myerrno-1], p1 );
      cout << EMsgPrefix << myerrno << " " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void Ispc_ErrorMsg( const int myerrno, const char * p1, const int p2 ) {
  ispc_error_lvl = 8; /* PTS 1122587 2003-Nov-11 FF */
  if ( (0 < myerrno) && (myerrno <= C_NUMBER_OF_ERROR_ISPC) ) {
    char buffer[256];
    if ( p1 && p2 ) {
      sprintf( buffer, ispc_error_msg[myerrno-1], p1, p2 );
      cout << EMsgPrefix << myerrno << " " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void Ispc_ErrorMsg( const int myerrno, const char * p1, const char * p2 ) {
  ispc_error_lvl = 8; /* PTS 1122587 2003-Nov-11 FF */
  if ( (0 < myerrno) && (myerrno <= C_NUMBER_OF_ERROR_ISPC) ) {
    char buffer[256];
    if ( p1 && p2 ) {
      sprintf( buffer, ispc_error_msg[myerrno-1], p1, p2 );
      cout << EMsgPrefix << myerrno << " " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void Ispc_ErrorMsg( const int myerrno, const char * p1, const char * p2,
                    const char* p3 ) {
  ispc_error_lvl = 8; /* PTS 1122587 2003-Nov-11 FF */
  if ( (0 < myerrno) && (myerrno <= C_NUMBER_OF_ERROR_ISPC) ) {
    char buffer[256];
    if ( p1 && p2 && p3 ) {
      sprintf( buffer, ispc_error_msg[myerrno-1], p1, p2, p3 );
      cout << EMsgPrefix << myerrno << " " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void Ispc_WarnMsg( const int myerrno, const char * p1 ) {
  if ( (0 < myerrno) && (myerrno <= C_NUMBER_OF_ERROR_ISPC) ) {
    char buffer[256];
    if (p1) {
      sprintf( buffer, ispc_error_msg[myerrno-1], p1 );
      cout << WMsgPrefix << myerrno << " " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void Ispc_WarnMsg( const int myerrno, const char * p1, const char * p2 ) {
  if ( (0 < myerrno) && (myerrno <= C_NUMBER_OF_ERROR_ISPC) ) {
    char buffer[256];
    if ( p1 && p2 ) {
      sprintf( buffer, ispc_error_msg[myerrno-1], p1, p2 );
      cout << WMsgPrefix << myerrno << " " << buffer << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void Ispc_WarnMsg( const int myerrno, const char * p1, const char * p2,
                   const char * p3 ) {
  if ( (0 < myerrno) && (myerrno <= C_NUMBER_OF_ERROR_ISPC) ) {
    char buffer[256];
    if ( p1 && p2 && p3 ) {
      sprintf( buffer, ispc_error_msg[myerrno-1], p1, p2, p3 );
      cout << WMsgPrefix << myerrno << " " << buffer << endl;
    }
  }
}

