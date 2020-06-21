/*!*****************************************************************************

  module      : X2DC_WarnAndErrHandler.cpp

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


#include <stdio.h>
#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#include <fstream>
#define endl std::endl
#define cout std::cout
#else
#include <iostream.h>
#endif


#include "idl/XML2DiscCpp/X2DC_WarnAndErrHandler.hpp"


/*----------------------------------------------------------------------------*/

static XML_Parser * wae_pParser = NULL;

void SetParser(XML_Parser * pParser) {
  wae_pParser = pParser;
}

int CurrLine() {
  if (wae_pParser) {
    return XML_GetCurrentLineNumber(*wae_pParser);
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------------*/

static bool Warnings = false;

void ForceWarnings() {
  Warnings = true;
}

/*----------------------------------------------------------------------------*/

static const char * X2DC_Msgs[X2DC_MsgCnt] = {

  // M__TOO_FEW_INPUT_PARAMS (1)
  "Input parameter(s) missing!",

  // M__BAD_FIRST_INPUT_PARAM (2)
  "First input parameter must be of form -F<RawName>__mgd.xml with strlen(RawName) < %d!",

  // M__FILE_OPEN_ERROR (3)
  "File %s can not be opened!",

  // M__XML_PARSE_ERROR (4)
  "XML parse error \"%s\"!",

  // M__DUPLICATE (5)
  "For %s \"%s\" duplicate specification of %s \"%s\"!",

  // M__DUPLICATE_KEY_MEMBER (6)
  "For %s \"%s\" duplicate specification of key member \"%s\"!",

  // M__DUPLICATE_INDEX_MEMBER (7)
  "For %s \"%s\" duplicate specification of member \"%s\" for index \"%d\"!",

  // M__INVALID_BITFIELDS_TYPE (8)
  "Invalid type \"%s\" for bitfields!",

  // M__BAD_TYPE_IN_TYPEDEF (9)
  "Type \"%s\" to be provided with synonym \"%s\" not registered!",

  // M__INVALID_STRING_LENGTH (10)
  "Invalid length of string \"%s\"!",

  // M__INVALID_ATTRPOS_IN_ELEMENT (11)
  "Invalid position of attribute %s in element %s of %s!",

  // M__ATTR_MISSING_IN_ELEMENT (12)
  "In element %s of %s %s attribute missing!",

  // M__EITHER_OR_ATTR_IN_ELEMENT (13)
  "In element %s of %s either attribute %s or attribute %s allowed!",

  // M__MEMBER_NOT_FOUND (14)
  "Member \"%s\" of %s \"%s\" not found!",

  // M__NOT_USABLE_IN_PERSISTENCY (15)
  "%s \"%s\" using \"%s\" may not be used in persistent classes!",

  // M__INVALID_ATTRPOS (16)
  "Invalid position of attribute %s of %s!",

  // M__INVALID_ATTRVAL (17)
  "Invalid value for attribute %s of %s!",

  // M__ATTR_ONLY_WITHIN_IMPORT (18)
  "Attribute %s for %s only within import mode allowed!",

  // M__INVALID_ATTR (19)
  "Invalid attribute %s for %s!",

  // M__INVALID_IDVALUE (20)
  "Invalid value for ID of %s!",

  // M__ATTR_MISSING (21)
  "For %s %s attribute missing!",

  // M__TOO_MANY_ATTR (22)
  "Too may attributes for %s!",

  // M__NULL_POINTER_IN_CASE (23)
  "INTERNAL ERROR in %s, %s == NULL in %sElement in case %s!",

  // M__NULL_POINTER (24)
  "INTERNAL ERROR in %s, %s == NULL in %s!",

  // M__TYPE_NOT_FOUND (25)
  "Type \"%s\" not registered!",

  // M__VALUE_NOT_FOUND (26)
  "Value of \"%s\" can not be determined!",

  // M__BAD_NUMBER_FOR_IDPARAM (27)
  "Can not link template parameter %d to ID of \"%s\"!",

  // M__BAD_MEMBER (28)
  "%s of member \"%s\" invalid (%s)!",

  // M__NULL_OID_PERSCLASSPTR (29)
  "Persistent class for OID member \"%s\" not found!",

  // M__ZEROID_OID_PERSCLASSPTR (30)
  "ID of persistent class of OID member \"%s\" equals 0!",

  // M__DIM_GT1_FOR_MEMBER (31)
  "Dimension > 1 for member \"%s\" not allowed!",

  // M__TEMPLPARAM_KIND_MISMATCH (32)
  "%s parameter \"%\" of tempalte \"%s\" may not be used as %s parameter!",

  // M__BAD_TEMPLCLASSPARAM_USAGE (33)
  "Class parameter \"%s\" of tempalte \"%s\" can not hold unspecified type!",

  // M__TEMPLCLASSPARAM_USAGE_MISMATCH (34)
  "Class parameter of \"%s\" template \"%s\" already supposed to be instantiated with a %s!",

  // M__BAD_TEMPIN_CONSTRUCTION (35)
  "INTERNAL ERROR in constructor of Templ_ParamInstance with %s!",

  // M__INTERNAL_ERROR_IN (36)
  "INTERNAL ERROR in %s!",

  // M__INTERNAL_ERROR_IN_S (37)
  "INTERNAL ERROR in %s, %s!",

  // M__INTERNAL_ERROR_IN_S_D (38)
  "INTERNAL ERROR in %s, %s %d!",

  // M__TOO_MANY_OIDMEMBER_PARAMS (39)
  "Too many parameters for member of type OmsOid!",

  // M__NON_TEMPL_MEMBER (40)
  "Can not assign template parameter to non-template member \"%s\"!",

  // M__BAD_MEMBER_DIMENSION (41)
  "Invalid dimension %d for member \"%s\" of class/struct \"%s\"!",

  // M__BAD_NUMBER_FOR_DIMPARAM (42)
  "Can not link template parameter %d to dimension of member \"%s\"!",

  // M__MEMBER_KIND_UNDEF (43)
  "Usage of member \"%s\" of type \"%s\" not specified!",

  // M__NULLPTR_FOR_MEMBER_TYPE (44)
  "NULL pointer for type of member \"%s\"!",

  // M__PTR_FOR_MEMBER_TYPE (45)
  "NOTNULL pointer for type of member \"%s\"!",

  // M__NON_TEMPL_TYPE (46)
  "Can not assign template parameter to non-template \"%s\"!",

  // M__BAD_FIRST_BASE_CLASS (47)
  "Size preconditions for first base class of \"%s\" not met!",

  // M__BAD_TYPE_MEMBER (48)
  "%s of member \"%s\" of \"%s\" invalid (%s)!",

  // M__NO_MEMBER_INFO (49)
  "Can not determine %s of member \"%s\" (%s)!",

  // M__INTERNAL_ERROR_IN_S_S (50)
  "INTERNAL ERROR in %s, %s %s!",

  // M__NO_OPERATION (51)
  "No arithmic operation specified for pts-expression!",

  // M__DUPLICATE_OPERATION (52)
  "Arithmic operation already specified for %s-expression!",

  // M__OPERATION_FIRST (53)
  "Arithmic operation must be specified first for pts-expression!",

  // M__TOO_MANY_OPERANDS (54)
  "More than two operands specified for %s-expression!",

  // M__EXPRESSION_ALREADY_EMBEDDED (55)
  "Pts expression already used in outer pts-expression!",

  // M__BAD_OPERAND_SWITCH (56)
  "Can not switch type of Non-Identifier operand to TemplParam for pts-expression!",

  // M__BAD_INDEX_MEMBER (57)
  "Invalid index member \"%s\" for %s \"%s\"!",

  // M__OPERATION_BEFORE_SECOND_OPERAND (58)
  "Arithmetic operation must be set before second operand may be specified for const-expression!",

  // M__DUPLICATE_OPERAND (59)
  "Operand still in process in const-expression, next operand may not be set yet!",

  // M__INVALID_OPERAND_COUNT (60)
  "Can not set value for const-expression due to invalid operand count %d!",

  // M__VALUE_SET_OPERAND_CNT1 (61)
  "INTERNAL ERROR in const-expression, value already set even though operand count equals 1!",

  // M__VALUE_NOTSET_OPERAND_CNT2 (62)
  "Value not yet set for const-expression even though operand count equals 2!",

  // M__DUPLICATE_TYPE (63)
  "Type \"%s\" already registered!",

  // M__DUPLICATE_CONST (64)
  "Constant \"%s\" already registered!",

  // M__BAD_TEMPIN_KIND (65)
  "Invalid argument for parameter %d in instantion of template \"%s\" for specialization \"%s\"!",

  // M__TEMPIN_TEMPAR_COUNT_MISMATCH (66)
  "Number of arguments does not match number of parameters in instantion of tempalte \"%s\" for specialization \"%s\"!",

  // M__TEMPIN_TEMPAR_MISMATCH (67)
  "Not-matching argument for parameter %d in instantion of template \"%s\" for specialization \"%s\"!",

  // M__NO_VALUE_FOR_DIMPARAM (68)
  "Can not set dimension of member \"%s\" in instantiation of template \"%s\" for specialization \"%s\"!",

  // M__BAD_VALUE_FOR_DIMPARAM (69)
  "Invalid dimension \"%d\" for member \"%s\" in instantiation of template \"%s\" for specialization \"%s\"!",

  // M__MEMBER_KIND_UNDEF_TEMPL (70)
  "Usage of member \"%s\" not specified in instantiation of template \"%s\" for specialization \"%s\"!",

  // M__TEMPAR_NOT_FOUND (71)
  "Cant not find parameter \"%s\" of template \"%s\" in instantiation for specialization \"%s\"!",

  // M__TEMPIN_NOT_FOUND (72)
  "Cant not find arguement %d in instantiation of template \"%s\" for specialization \"%s\"!",

  // M__TEMPIN_CPPTYPE_NOT_FOUND (73)
  "Cant not determine C++ type of arguement %d in instantiation of template \"%s\" for specialization \"%s\"!",

  // M__MEMBER_TEMPIN_NOT_FOUND (74)
  "Cant not find argument %d in instantiation of template \"%s\" for specialization for member \"%s\"!",

  // M__MEMBER_TEMPIN_BAD_KIND (75)
  "Usage of argument %d not specified in instantiation of template \"%s\" for specialization for member \"%s\"!",

  // M__BAD_OIDMEMBER_TEMPIN_COUNT (76)
  "Bad number (%d) of arguments for OID member \"%s\" in instantiation of template \"%s\" for specialization \"%s\"!",

  // M__INVALID_OIDMEMBER_TEMPIN_KIND (77)
  "Invalid usage of argument for OID member \"%s\" in instantiation of template \"%s\" for specialization \"%s\"!",

  // M__OIDMEMBER_TEMPIN_NOT_FOUND (78)
  "Can not find argument for OID member \"%s\" in instantiation of template \"%s\" for specialization \"%s\"!",

  // M__BAD_EXPRESSION_EVALUATION (79)
  "Can not evaluate expression due to incomplete input!",

  // M__BAD_EXPRESSION_OPERAND (80)
  "Can not compute operand %d in expression evaluation!",

  // M_BAD_PERSCLASS_ID (81)
  "Can not evaluate ID of persistent class \"%s\"!",

  // M__PERSBASECLASS_IS_TEMPLATE
  "Persistent base class \"%s\" of persistent class \"%s\" is a template!",

  // M__PERSBASECLASS_NOT_FOUND
  "Persistent base class \"%s\" of persistent class \"%s\" not found!",

  // M_BAD_PERSCLASS_SIZE (84)
  "Can not evaluate size of persistent class \"%s\"!",

  // M__BAD_INT_SYNONYM (85)
  "Can not find int synonym \"%s\"!",

  // M__BAD_BASECLASS (86)
  "Invlaid base class \"%s\" for class \"%s\"!",

  // M__BAD_BITFIELDS_SIZE (87)
  "Can not evaluate size of bitfields of class \"%s\"!",

  // M__BITFIELDS_TYPE_NOT_FOUND (88)
  "Can not find type \"%s\" of bitfields of class \"%s\"!",

  // M__INVALID_MEMBER_TYPE_FOR_CLASS (89)
  "Invalid type \"%s\" for member \"%s\" of class \"%s\"!",

  // M__MEMBER_TYPE_NOT_FOUND (90)
  "Can not find type \"%s\" of member \"%s\" of %s \"%s\"!",

  // M__MEMBER_DIMENSION_NOT_SET (91)
  "Can not set dimension for member \"%s\" of type \"%s\"!",

  // M__NTH_TEMPAR_NOT_FOUND (92)
  "Can not find parameter %d of template \"%s\"!",

  // M__OIDMEMBER_CLASSINSTANCE_IS_PERSTEMPL (93)
  "Argument \"%s\" for OmsOid member \"%s\" of class \"%s\" is a template!",

  // M__OIDMEMBER_CLASSINSTANCE_NOT_FOUND (94)
  "Argument \"%s\" for member \"%s\" of type OmsOid of class \"%s\" not found!",

  // M__TEMPAR_SUPPOSED_FOR_PERSCLASS (95)
  "Parameter \"%s\" of template \"%s\" supposed to be instantiated with a persistent class can not be instantiated with \"%s\"!",

  // M__CLASSINSTANCE_IS_TEMPLATE (96)
  "Argument \"%s\" for parameter \"%s\" of template \"%s\" is a template by itself!",

  // M__TEMPAR_SUPPOSED_FOR_FULLTYPE (97)
  "Parameter \"%s\" of template \"%s\" supposed to be instantiated with a fully specified type can not be instantiated with \"%s\"!",

  // M__TEMPAR_NOT_SUPPOSED_FOR_PERSCLASS (98)
  "Parameter \"%s\" of template \"%s\" not supposed to be instantiated with a persistent class can not be instantiated with \"%s\"!",

  // M__CLASSINSTANCE_NOT_FOUND (99)
  "Argument \"%s\" for parameter \"%s\" of template \"%s\" not found!",

  // M__BAD_INTINSTANCE (100)
  "Can not evaluate int argument \"%s\" for template \"%s\"!",

  // M__TEMPLATE_NOT_FOUND (101)
  "Can not find template \"%s\" to be instantiated in INSTANTIATION!",

  // M__NOT_TEMPLATE (102)
  "%s %s to be instantiated in INSTANTIATION is not a template!",

  // M__NO_MEMBERS (103)
  "%s %s has no member(s)!",

  // M__NO_TEMPLPARAMS (104)
  "No parameters for %s class \"%s\" specified!",

  // M__PERSTEMPL_ID_NOT_LINKED (105)
  "None of the parameters of persistent template class \"%s\" is supposed to hold the class ID!",

  // M__PERSCLASS_SIZE_MISMATCH (106)
  "Specified size \"%d\" for persistent class \"%s\" does not match calculated size \"%d\"!",

  // M__BITFIELDS_SIZE_MISMATCH (107)
  "Specified size \"%d\" for bit fields in class \"%s\" does not match calculated size \"%d\"!",

  // M__ID_NOT_SET_IN_PERSINSTANTIATION (108)
  "Class ID not set in instantiation of persistent template class \"%s\" for specialization \"%s\"!",

  // M__INVALID_UNION_MEMBER (109)
  "Invalid type \"%s\" for %s member of union \"%s\"!",

  // M__INVALID_UNION_SIZE (110)
  "Invalid padding size %d for union \"%s\"!",

  // M__ONLY_WITH_M76 (111)
  "Flag \"%s\" may only be set if flag \"M76\" ist set!"

};

/*----------------------------------------------------------------------------*/

static const char * EMsgPrefix = "XML2DiscCpp error ";

static const char * WMsgPrefix = "XML2DiscCpp warning ";

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const char * p1, bool atLine ) {
  cout << EMsgPrefix;
  if (atLine) {
    int lineNo = CurrLine();
    if (lineNo > -1) {
      cout << " at line " << lineNo;
    }
  }
  if (p1) {
    cout << ": " << p1 << endl;
  }
  else {
    cout << "!" << endl;
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    cout << ": " << X2DC_Msgs[msgNo-1] << endl;
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const int p1, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    char buffer[256];
    sprintf( buffer, X2DC_Msgs[msgNo-1], p1 );
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    cout << ": " << buffer << endl;
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const char * p1, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if (p1) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const char * p1, const int p2, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if (p1) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const int p1, const char * p2, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if (p2) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p1 && p2 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2, const int p3, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p1 && p2 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const int p1, const char * p2, const int p3, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if (p2) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const int p1, const char * p2, const char * p3, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p2 && p3 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2, const char * p3, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p1 && p2 && p3 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2,
                                   const char * p3, const int p4, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p1 && p2 && p3 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3, p4 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const int p1, const char * p2,
                                   const char * p3, const char * p4, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p2 && p3 && p4 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3, p4 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_ErrMsg( const int msgNo, const char * p1, const char * p2,
                                   const char * p3, const char * p4, bool atLine ) {
  if ( (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << EMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p1 && p2 && p3 && p4 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3, p4 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_WarnMsg( const char * p1, bool atLine ) {
  if (Warnings) {
    cout << WMsgPrefix;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if (p1) {
      cout << ": " << p1 << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_WarnMsg( const int msgNo, const char * p1, const char * p2, const char * p3, bool atLine ) {
  if ( Warnings && (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << WMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p1 && p2 && p3 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

/*----------------------------------------------------------------------------*/

void X2DC_WarnMsg( const int msgNo, const char * p1, const char * p2,
                                    const char * p3, const char * p4, bool atLine ) {
  if ( Warnings && (0 < msgNo) && (msgNo <= X2DC_MsgCnt) ) {
    cout << WMsgPrefix << msgNo;
    if (atLine) {
      int lineNo = CurrLine();
      if (lineNo > -1) {
        cout << " at line " << lineNo;
      }
    }
    if ( p1 && p2 && p3 && p4 ) {
      char buffer[256];
      sprintf( buffer, X2DC_Msgs[msgNo-1], p1, p2, p3, p4 );
      cout << ": " << buffer << endl;
    }
    else {
      cout << "!" << endl;
    }
  }
}

