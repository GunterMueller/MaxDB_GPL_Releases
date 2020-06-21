/*!*****************************************************************************

  module      : X2DC_WarnAndErrHandler.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG(GoertzP)

  special area: Generator of persistent class description code
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


#ifndef X2DC_WARNANDERRHANDLER_HPP
#define X2DC_WARNANDERRHANDLER_HPP

#include <stdio.h>

#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#include <fstream>
#define endl     std::endl
#define ofstream std::ofstream
#define cout     std::cout
#else
#include <iostream.h>
#include <string.h>
#include <fstream.h>
#endif

#include "idl/expat/expat.h"


/*----------------------------------------------------------------------------*/

void SetParser(XML_Parser * pParser);

/*----------------------------------------------------------------------------*/

void ForceWarnings();

/*----------------------------------------------------------------------------*/

#define  M__TOO_FEW_INPUT_PARAMS                    1
#define  M__BAD_FIRST_INPUT_PARAM                   2
#define  M__FILE_OPEN_ERROR                         3
#define  M__XML_PARSE_ERROR                         4
#define  M__DUPLICATE                               5
#define  M__DUPLICATE_KEY_MEMBER                    6
#define  M__DUPLICATE_INDEX_MEMBER                  7
#define  M__INVALID_BITFIELDS_TYPE                  8
#define  M__BAD_TYPE_IN_TYPEDEF                     9
#define  M__INVALID_STRING_LENGTH                  10
#define  M__INVALID_ATTRPOS_IN_ELEMENT             11
#define  M__ATTR_MISSING_IN_ELEMENT                12
#define  M__EITHER_OR_ATTR_IN_ELEMENT              13
#define  M__MEMBER_NOT_FOUND                       14
#define  M__NOT_USABLE_IN_PERSISTENCY              15
#define  M__INVALID_ATTRPOS                        16
#define  M__INVALID_ATTRVAL                        17
#define  M__ATTR_ONLY_WITHIN_IMPORT                18
#define  M__INVALID_ATTR                           19
#define  M__INVALID_IDVALUE                        20
#define  M__ATTR_MISSING                           21
#define  M__TOO_MANY_ATTR                          22
#define  M__NULL_POINTER_IN_CASE                   23
#define  M__NULL_POINTER                           24
#define  M__TYPE_NOT_FOUND                         25
#define  M__VALUE_NOT_FOUND                        26
#define  M__BAD_NUMBER_FOR_IDPARAM                 27
#define  M__BAD_MEMBER                             28
#define  M__NULL_OID_PERSCLASSPTR                  29
#define  M__ZEROID_OID_PERSCLASSPTR                30
#define  M__DIM_GT1_FOR_MEMBER                     31
#define  M__TEMPLPARAM_KIND_MISMATCH               32
#define  M__BAD_TEMPLCLASSPARAM_USAGE              33
#define  M__TEMPLCLASSPARAM_USAGE_MISMATCH         34
#define  M__BAD_TEMPIN_CONSTRUCTION                35
#define  M__INTERNAL_ERROR_IN                      36
#define  M__INTERNAL_ERROR_IN_S                    37
#define  M__INTERNAL_ERROR_IN_S_D                  38
#define  M__TOO_MANY_OIDMEMBER_PARAMS              39
#define  M__NON_TEMPL_MEMBER                       40
#define  M__BAD_MEMBER_DIMENSION                   41
#define  M__BAD_NUMBER_FOR_DIMPARAM                42
#define  M__MEMBER_KIND_UNDEF                      43
#define  M__NULLPTR_FOR_MEMBER_TYPE                44
#define  M__PTR_FOR_MEMBER_TYPE                    45
#define  M__NON_TEMPL_TYPE                         46
#define  M__BAD_FIRST_BASE_CLASS                   47
#define  M__BAD_TYPE_MEMBER                        48
#define  M__NO_MEMBER_INFO                         49
#define  M__INTERNAL_ERROR_IN_S_S                  50
#define  M__NO_OPERATION                           51
#define  M__DUPLICATE_OPERATION                    52
#define  M__OPERATION_FIRST                        53
#define  M__TOO_MANY_OPERANDS                      54
#define  M__EXPRESSION_ALREADY_EMBEDDED            55
#define  M__BAD_OPERAND_SWITCH                     56
#define  M__BAD_INDEX_MEMBER                       57
#define  M__OPERATION_BEFORE_SECOND_OPERAND        58
#define  M__DUPLICATE_OPERAND                      59
#define  M__INVALID_OPERAND_COUNT                  60
#define  M__VALUE_SET_OPERAND_CNT1                 61
#define  M__VALUE_NOTSET_OPERAND_CNT2              62
#define  M__DUPLICATE_TYPE                         63
#define  M__DUPLICATE_CONST                        64
#define  M__BAD_TEMPIN_KIND                        65
#define  M__TEMPIN_TEMPAR_COUNT_MISMATCH           66
#define  M__TEMPIN_TEMPAR_MISMATCH                 67
#define  M__NO_VALUE_FOR_DIMPARAM                  68
#define  M__BAD_VALUE_FOR_DIMPARAM                 69
#define  M__MEMBER_KIND_UNDEF_TEMPL                70
#define  M__TEMPAR_NOT_FOUND                       71
#define  M__TEMPIN_NOT_FOUND                       72
#define  M__TEMPIN_CPPTYPE_NOT_FOUND               73
#define  M__MEMBER_TEMPIN_NOT_FOUND                74
#define  M__MEMBER_TEMPIN_BAD_KIND                 75
#define  M__BAD_OIDMEMBER_TEMPIN_COUNT             76
#define  M__INVALID_OIDMEMBER_TEMPIN_KIND          77
#define  M__OIDMEMBER_TEMPIN_NOT_FOUND             78
#define  M__BAD_EXPRESSION_EVALUATION              79
#define  M__BAD_EXPRESSION_OPERAND                 80
#define  M__BAD_PERSCLASS_ID                       81
#define  M__PERSBASECLASS_IS_TEMPLATE              82
#define  M__PERSBASECLASS_NOT_FOUND                83
#define  M__BAD_PERSCLASS_SIZE                     84
#define  M__BAD_INT_SYNONYM                        85
#define  M__BAD_BASECLASS                          86
#define  M__BAD_BITFIELDS_SIZE                     87
#define  M__BITFIELDS_TYPE_NOT_FOUND               88
#define  M__INVALID_MEMBER_TYPE_FOR_CLASS          89
#define  M__MEMBER_TYPE_NOT_FOUND                  90
#define  M__MEMBER_DIMENSION_NOT_SET               91
#define  M__NTH_TEMPAR_NOT_FOUND                   92
#define  M__OIDMEMBER_CLASSINSTANCE_IS_PERSTEMPL   93
#define  M__OIDMEMBER_CLASSINSTANCE_NOT_FOUND      94
#define  M__TEMPAR_SUPPOSED_FOR_PERSCLASS          95
#define  M__CLASSINSTANCE_IS_TEMPLATE              96
#define  M__TEMPAR_SUPPOSED_FOR_FULLTYPE           97
#define  M__TEMPAR_NOT_SUPPOSED_FOR_PERSCLASS      98
#define  M__CLASSINSTANCE_NOT_FOUND                99
#define  M__BAD_INTINSTANCE                       100
#define  M__TEMPLATE_NOT_FOUND                    101
#define  M__NOT_TEMPLATE                          102
#define  M__NO_MEMBERS                            103
#define  M__NO_TEMPLPARAMS                        104
#define  M__PERSTEMPL_ID_NOT_LINKED               105
#define  M__PERSCLASS_SIZE_MISMATCH               106
#define  M__BITFIELDS_SIZE_MISMATCH               107
#define  M__ID_NOT_SET_IN_PERSINSTANTIATION       108
#define  M__INVALID_UNION_MEMBER                  109
#define  M__INVALID_PADDING_SIZE                  110
#define  M__ONLY_WITH_M76                         111

#define  X2DC_MsgCnt                              111

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const char * p1, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const int p1, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const char * p1, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const char * p1, const int p2, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const int p1, const char * p2, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2, const int p3, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const int p1, const char * p2, const int p3, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const int p1, const char * p2, const char * p3, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2, const char * p3, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2,
                                   const char * p3, const int p4, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const int p1, const char * p2,
                                   const char * p3, const char * p4, bool atLine = true );
void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2,
                                   const char * p3, const char * p4, bool atLine = true );


void X2DC_WarnMsg( const char * p1, bool atLine = true );
void X2DC_WarnMsg( const int msgNo, const char * p1, const char * p2, const char * p3, bool atLine = true );
void X2DC_WarnMsg( const int msgNo, const char * p1, const char * p2,
                                    const char * p3, const char * p4, bool atLine = true );

#endif /* X2DC_WARNANDERRHANDLER_HPP */

