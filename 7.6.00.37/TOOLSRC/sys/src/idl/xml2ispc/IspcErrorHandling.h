/*!*****************************************************************************

  module      : IspcErrorHandling.h

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


#ifndef ISPCERRORHANDLING_H
#define ISPCERRORHANDLING_H


#if defined(_IBMR2)
/*
 * These keywords are not supported by the AIX compiler (C++ Standard of 1992 ...)
 * If you are carefull, you are able to ignore them sometimes...
 */

// PTS 1106619
#if (__IBMCPP__ < 500)
#define CONST_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
#define REINTERPRET_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
#define STATIC_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
#else
#define CONST_CAST(TYPE_,EXPR_) const_cast< TYPE_ >(EXPR_)
#define REINTERPRET_CAST(TYPE_,EXPR_) reinterpret_cast< TYPE_ >(EXPR_)
#define STATIC_CAST(TYPE_,EXPR_) static_cast< TYPE_ >(EXPR_)
#endif

#else

#define CONST_CAST(TYPE_,EXPR_) const_cast< TYPE_ >(EXPR_)
#define REINTERPRET_CAST(TYPE_,EXPR_) reinterpret_cast< TYPE_ >(EXPR_)
#define STATIC_CAST(TYPE_,EXPR_) static_cast< TYPE_ >(EXPR_)

#endif


#define NOWARN 0 /* PTS 1109175 */
#define WARN1  1 /* PTS 1109175 */

#define C_WRONG_PARAMETERS_ISPC               1
#define C_XML_PARSE_ERROR_ISPC                2
#define C_P1_NO_USERDEFINED_TYPE_ISPC         3
#define C_P1_IO_TYPE_INPUT_ISPC               4
#define C_P1_IO_TYPE_OUTPUT_ISPC              5
#define C_P1_WRONG_GUID_ISPC                  6
#define C_P1_NO_GUID_ISPC                     7
#define C_P1_TYPE_SPEC_INCOMPLETE_ISPC        8
#define C_TYPE_UNKNOWN_ISPC                   9
#define C_UNSUPPORTED_VT_CARRAY1_ISPC        10
#define C_NO_DIR_SPECIFIC_ISPC               11
#define C_PASS_STRUCT_BY_PTR_ISPC            12
#define C_IN_SCALAR_BYPTR_ISPC               13
#define C_UNSUPPORTED_PARAM_TYPE_ISPC        14
#define C_PTR_OF_PTR_UNSUPPORTED_ISPC        15
#define C_UNSUPPORTED_VT_CARRAY2_ISPC        16
#define C_TYPE_NOT_PORTABLE_ISPC             17
#define C_STRUCT_NOT_ALIGNED_ISPC            18
#define C_POINTER_NOT_ALLOWED_ISPC           19
#define C_DIMENSION_TYPE_UNDEF_ISPC          20
#define C_CONST_EXPR_NOT_VALID_ISPC          21
#define C_DUP_DEFINITION_ISPC                22
#define C_DIMENSION_IN_TYPEDEF               23  /* PTS 1131325, PG */
#define C_DUPLICATE_OPERATOR_SPECIFICATION   24  /* P */
#define C_DUPLICATE_OPERAND_SPECIFICATION    25  /* T */
#define C_TOO_MANY_OPERANDS                  26  /* S */
#define C_MISSING_OPERATOR                   27  /*   */
#define C_ILLEGAL_EXPR_ELEMENT               28  /* 1 */
#define C_MISSING_CONST_VALUE                29  /* 1 */
#define C_ILLEGAL_ATTRORDER_IN_TYPEDEF       30  /* 3 */
#define C_ILLEGAL_ATTRORDER_IN_STRUCT        31  /* 4 */
#define C_ILLEGAL_ATTRORDER_IN_CONST         32  /* 7 */
#define C_ILLEGAL_ATTRORDER_IN_STRUCTMEMBER  33  /* 3 */
#define C_ILLEGAL_ATTRORDER_IN_ENUMMEMBER    34  /* 4 */
#define C_MISSING_STRUCTMEMBER_TYPE          35  /* , */
#define C_ILLEGAL_ATTRORDER_IN_INTERFACE     36  /*   */
#define C_ILLEGAL_ATTRORDER_IN_COCLASS       37  /* P */
#define C_DUPLICATE_MEMBERTYPE_SPECIFICATION 38  /* G */
#define C_NOT_ALL_MEMBERTYPES_REGISTERED     39  /* * */
#define C_CPPSQLDBC_NOT_SUPPORTED            40  /* PTS             */
#define C_ONLY_CPPSQLDBC_SUPPORTED           41  /*     1138438, PG */ 
#define C_BAD_UNION_DECLARATION              42  /* PTS 1141233, PG */
#define C_BAD_OMSTYPEOID_ISPC                43  /* PTS 1141233, PG */

#define C_NUMBER_OF_ERROR_ISPC               43  /* PTS 1141233, PG */

void Ispc_ErrorMsg( const int myerrno);
void Ispc_ErrorMsg( const int myerrno, const char * p1 );
void Ispc_ErrorMsg( const int myerrno, const char * p1, const int p2 );
void Ispc_ErrorMsg( const int myerrno, const char * p1, const char * p2 );
void Ispc_ErrorMsg( const int myerrno, const char * p1, const char * p2, const char * p3 );
void Ispc_WarnMsg( const int myerrno, const char * p1 ); /* PTS 1134734, PG */
void Ispc_WarnMsg( const int myerrno, const char * p1, const char * p2 );
void Ispc_WarnMsg( const int myerrno, const char * p1, const char * p2, const char * p3 );


#endif /* ISPCERRORHANDLING_H */

