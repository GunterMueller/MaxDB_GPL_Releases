/* ==================================================================
 *
 * responsible:   Bernd Vorsprach (D025588 - Bernd.Vorsprach@SAP-AG.de)
 *
 * created:       1998-02-06 by Bernd Vorsprach
 *
 * purpose:       header for param management (modules vcn2*c)
 *                               
 * ==================================================================
 *
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef _GCN002_H_
#define _GCN002_H_

#define XPARAM_NAMES_ONLY
#include "geo002.h"
#include "DBM/Srv/Parameters/DBMSrvPar_RTEConfParameter.hpp"

/* SPECIFICATION XParam SYNTAX ERRORS - CODES AND MESSAGES
 * ------------------------------------------------------------------
 */
/* error codes */
typedef enum {
  XPOK_CN002,
  XPERR_UNKNOWN_CN002,
  XPERR_LINESEQ_CN002,
  XPERR_LINEDET_CN002,
  XPERR_INVIDLINE_CN002,
  XPERR_INVPROP_CN002,
  XPERR_INVTYPE_CN002,
  XPERR_INVNAME_CN002,
  XPERR_INVDEF_CN002,
  XPERR_INVINT_CN002,
  XPERR_LONGDEF_CN002,
  XPERR_LONGID_CN002,
  XPERR_NOTKEY_CN002,
  XPERR_TOKEN_CN002,
  XPERR_INVVALUE_CN002,
  XPERR_INVFOLLOW_CN002,
  XPERR_IFSEQ_CN002,
  XPERR_INVID_CN002,
  XPERR_INVSTMT_CN002,
  XPERR_INVTYPES_CN002,
  XPERR_INVOP_CN002,
  XPERR_OVERFL_CN002,
  XPERR_MANYOP_CN002,
  XPERR_INVCONST_CN002,
  XPERR_UNKNOWNID_CN002,
  XPERR_CRYPTUSE_CN002,
  XPERR_FEWOP_CN002,
  XPERR_INVEXPT_CN002,
  XPERR_EOF_CN002,
  XPERR_DECLAFTER_CN002,
  XPERR_DUPCONSTR_CN002,
  XPERR_DIVBYZERO_CN002,
  XPERR_DUPLICATE_CN002
} tcn002_XPError;
/* error messages (<code>,<text>) */
#define XPERROR_TO_TEXT_CN002 { \
  { XPERR_LINESEQ_CN002,   "XPERR_LINESEQ   : wrong line sequence"                }, \
  { XPERR_LINEDET_CN002,   "XPERR_LINEDET   : unknown line type"                  }, \
  { XPERR_INVIDLINE_CN002, "XPERR_INVIDLINE : invalid id line"                    }, \
  { XPERR_INVPROP_CN002,   "XPERR_INVPROP   : invalid parameter property"         }, \
  { XPERR_INVTYPE_CN002,   "XPERR_INVTYPE   : invalid parameter type"             }, \
  { XPERR_INVNAME_CN002,   "XPERR_INVNAME   : invalid parameter name"             }, \
  { XPERR_INVDEF_CN002,    "XPERR_INVDEF    : invalid default specification"      }, \
  { XPERR_INVINT_CN002,    "XPERR_INVDEF    : invalid internal value"             }, \
  { XPERR_LONGDEF_CN002,   "XPERR_LONGDEF   : default value too long"             }, \
  { XPERR_LONGID_CN002,    "XPERR_LONGID    : identifier to long"                 }, \
  { XPERR_NOTKEY_CN002,    "XPERR_NOTKEY    : unknown keyword"                    }, \
  { XPERR_TOKEN_CN002,     "XPERR_TOKEN     : unexpected token"                   }, \
  { XPERR_INVVALUE_CN002,  "XPERR_INVVALUE  : missing value after keyword"        }, \
  { XPERR_INVFOLLOW_CN002, "XPERR_INVFOLLOW : follow line sign not allowed"       }, \
  { XPERR_IFSEQ_CN002,     "XPERR_IVSEQ     : error in IF-ELSIF-ELSE sequence"    }, \
  { XPERR_INVID_CN002,     "XPERR_INVID     : invalid identifier"                 }, \
  { XPERR_INVSTMT_CN002,   "XPERR_INVSTMT   : unexpected statement"               }, \
  { XPERR_INVTYPES_CN002,  "XPERR_INVTYPES  : type mismatch"                      }, \
  { XPERR_INVOP_CN002,     "XPERR_INVOP     : invalid operator"                   }, \
  { XPERR_OVERFL_CN002,    "XPERR_OVERFL    : value overflow"                     }, \
  { XPERR_MANYOP_CN002,    "XPERR_MANYOP    : to many operands in expression"     }, \
  { XPERR_INVCONST_CN002,  "XPERR_INVCONST  : invalid constant"                   }, \
  { XPERR_UNKNOWNID_CN002, "XPERR_UNKNOWNID : unknown identifier"                 }, \
  { XPERR_CRYPTUSE_CN002,  "XPERR_CRYPTUSE  : use of crypt parameter"             }, \
  { XPERR_FEWOP_CN002,     "XPERR_FEWOP     : to few operands in expression"      }, \
  { XPERR_INVEXPT_CN002,   "XPERR_INVEXPT   : invalid type of expression"         }, \
  { XPERR_EOF_CN002,       "XPERR_EOF       : unexpected end of file"             }, \
  { XPERR_DECLAFTER_CN002, "XPERR_DECLAFTER : refers to later declared parameter" }, \
  { XPERR_DUPCONSTR_CN002, "XPERR_DUPCONSTR : duplicate constraint found"         }, \
  { XPERR_DIVBYZERO_CN002, "XPERR_DIVBYZERO : division by zero"                   }, \
  { XPERR_DUPLICATE_CN002, "XPERR_DUPLICATE : duplicate identifier found"         }, \
  { XPERR_UNKNOWN_CN002,   NULL                                                   }}

/* SPECIFICATION SYNTAX
 * ------------------------------------------------------------------
 */
/* syntax key words */
#define XP_KEY_CALC_CN002           "CALC"
#define XP_KEY_FORCE_CN002          "FORCE"
#define XP_KEY_CONSTRAINT_CN002     "CONSTRAINT"
#define XP_KEY_IF_CN002             "IF"
#define XP_KEY_ELSIF_CN002          "ELSIF"
#define XP_KEY_ELSE_CN002           "ELSE"
#define XP_KEY_ID_CN002             "ID"
#define XP_KEY_TYPE_CN002           "TYPE"
#define XP_KEY_DEFAULT_CN002        "DEFAULT"
#define XP_KEY_DEFAULTFOR_CN002     "DEFAULT_FOR"
#define XP_KEY_CODE_CN002           "CODE"
#define XP_KEY_ENDCODE_CN002        "ENDCODE"
#define XP_KEY_HELP_CN002           "HELP"
#define XP_KEY_ENDHELP_CN002        "ENDHELP"
#define XP_KEY_EXPLAIN_CN002        "EXPLAIN"
#define XP_KEY_ENDEXPLAIN_CN002     "ENDEXPLAIN"
#define XP_KEY_GOOD_CN002           "LASTKNOWNGOOD"
#define XP_KEY_ACTIVE_CN002         "ACTIVEVALUE"
#define XP_KEY_CLASS_CN002          "CLASS"

/* special characters an strings */
#define XP_CHAR_OLDFOLLOW_CN002     '"'
#define XP_KEY_OLDFOLLOW_CN002      "\""
#define XP_CHAR_CONST_CN002         '$'
#define XP_KEY_IDLINESEP_CN002      "#"
#define XP_KEY_PROPSEP_CN002        ","

#define XP_KEY_DYNAMIC_CN002        "?"
#define XP_KEY_WILDCARD_CN002       "*"
#define XP_CHAR_NEWFOLLOW_CN002     '\\'
#define XP_CHAR_STRING_CN002        '"'
#define XP_KEY_NEWFOLLOW_CN002      "\\"
#define XP_CHAR_UNDERLINE_CN002     '_'
#define XP_CHAR_COMMENT_CN002       '\''
#define XP_KEY_TOKENSEP_CN002       " \t"
#define XP_CHAR_SPACE_CN002         ' '
#define XP_CHAR_TAB_CN002           '\t'
/*#define XP_FIRST_PRINT_CN002        0x20*/
/*#define XP_LAST_PRINT_CN002         0x7E*/

/* SPECIFICATION OPERATORS
 * ------------------------------------------------------------------
 */
/* comparsion operators */
#define XP_OP_EQUAL_CN002          "="
#define XP_OP_LESS_CN002           "<"
#define XP_OP_GREATER_CN002        ">"
#define XP_OP_LESSEQUAL_CN002      "<="
#define XP_OP_GREATEREQUAL_CN002   ">="
#define XP_OP_NOTEQUAL_CN002       "<>"
/* bool    operators */
#define XP_OP_AND_CN002            "AND"
#define XP_OP_OR_CN002             "OR"
#define XP_OP_XOR_CN002            "XOR"
/* arithmetic operators */
#define XP_OP_PLUS_CN002           "+"
#define XP_OP_MINUS_CN002          "-"
#define XP_OP_MULTIPLY_CN002       "*"
#define XP_OP_DIVIDE_CN002         "/"
#define XP_OP_MODULO_CN002         "%"
#define XP_OP_ALIGN_CN002          "#"
#define XP_OP_POWER_CN002       "^"
/* operator types */
typedef enum {
  XPOperatorUnknown,
  XPOperatorAND,
  XPOperatorOR,
  XPOperatorXOR,
  XPOperatorPLUS,
  XPOperatorMINUS,
  XPOperatorMULTIPLY,
  XPOperatorDIVIDE,
  XPOperatorMODULO,
  XPOperatorALIGN,
  XPOperatorPOWER,
  XPOperatorEQUAL,
  XPOperatorLESS,
  XPOperatorGREATER,
  XPOperatorLESSEQUAL,
  XPOperatorGREATEREQUAL,
  XPOperatorNOTEQUAL
} tcn002_XPOperators;
/* operator to keyword table */
#define XP_OPERATOR_TABLE_CN002  { \
  { XPOperatorAND,              XP_OP_AND_CN002           }, \
  { XPOperatorOR,               XP_OP_OR_CN002            }, \
  { XPOperatorXOR,              XP_OP_XOR_CN002           }, \
  { XPOperatorPLUS,             XP_OP_PLUS_CN002          }, \
  { XPOperatorMINUS,            XP_OP_MINUS_CN002         }, \
  { XPOperatorMULTIPLY,         XP_OP_MULTIPLY_CN002      }, \
  { XPOperatorDIVIDE,           XP_OP_DIVIDE_CN002        }, \
  { XPOperatorMODULO,           XP_OP_MODULO_CN002        }, \
  { XPOperatorALIGN,            XP_OP_ALIGN_CN002         }, \
  { XPOperatorPOWER,            XP_OP_POWER_CN002         }, \
  { XPOperatorEQUAL,            XP_OP_EQUAL_CN002         }, \
  { XPOperatorLESS,             XP_OP_LESS_CN002          }, \
  { XPOperatorGREATER,          XP_OP_GREATER_CN002       }, \
  { XPOperatorLESSEQUAL,        XP_OP_LESSEQUAL_CN002     }, \
  { XPOperatorGREATEREQUAL,     XP_OP_GREATEREQUAL_CN002  }, \
  { XPOperatorNOTEQUAL,         XP_OP_NOTEQUAL_CN002      }, \
  { XPOperatorUnknown,          NULL                     }}
/* type of operation */
typedef enum {
  XPOpTypeUnknown,
  XPOpTypeArith,
  XPOpTypeString,
  XPOpTypebool   
} tcn002_XPOpType;
/* operator to type of operation table */
/*  <operator>                  <in-type1>       <in-type2>       <out-type>    */
#define XP_OPERATOR_TO_TYPE_TABLE_CN002  { \
  { XPOperatorAND,              XPOpTypebool   , XPOpTypebool   , XPOpTypebool    }, \
  { XPOperatorOR,               XPOpTypebool   , XPOpTypebool   , XPOpTypebool    }, \
  { XPOperatorXOR,              XPOpTypebool   , XPOpTypebool   , XPOpTypebool    }, \
  { XPOperatorPLUS,             XPOpTypeString , XPOpTypeString , XPOpTypeString  }, \
  { XPOperatorPLUS,             XPOpTypeArith  , XPOpTypeArith  , XPOpTypeArith   }, \
  { XPOperatorPLUS,             XPOpTypeArith  , XPOpTypeString , XPOpTypeString  }, \
  { XPOperatorPLUS,             XPOpTypeString , XPOpTypeArith  , XPOpTypeString  }, \
  { XPOperatorMINUS,            XPOpTypeArith  , XPOpTypeArith  , XPOpTypeArith   }, \
  { XPOperatorMULTIPLY,         XPOpTypeArith  , XPOpTypeArith  , XPOpTypeArith   }, \
  { XPOperatorDIVIDE,           XPOpTypeArith  , XPOpTypeArith  , XPOpTypeArith   }, \
  { XPOperatorMODULO,           XPOpTypeArith  , XPOpTypeArith  , XPOpTypeArith   }, \
  { XPOperatorALIGN,            XPOpTypeArith  , XPOpTypeArith  , XPOpTypeArith   }, \
  { XPOperatorPOWER,            XPOpTypeArith  , XPOpTypeArith  , XPOpTypeArith   }, \
  { XPOperatorEQUAL,            XPOpTypeArith  , XPOpTypeArith  , XPOpTypebool    }, \
  { XPOperatorLESS,             XPOpTypeArith  , XPOpTypeArith  , XPOpTypebool    }, \
  { XPOperatorGREATER,          XPOpTypeArith  , XPOpTypeArith  , XPOpTypebool    }, \
  { XPOperatorLESSEQUAL,        XPOpTypeArith  , XPOpTypeArith  , XPOpTypebool    }, \
  { XPOperatorGREATEREQUAL,     XPOpTypeArith  , XPOpTypeArith  , XPOpTypebool    }, \
  { XPOperatorNOTEQUAL,         XPOpTypeArith  , XPOpTypeArith  , XPOpTypebool    }, \
  { XPOperatorEQUAL,            XPOpTypeString , XPOpTypeString , XPOpTypebool    }, \
  { XPOperatorLESS,             XPOpTypeString , XPOpTypeString , XPOpTypebool    }, \
  { XPOperatorGREATER,          XPOpTypeString , XPOpTypeString , XPOpTypebool    }, \
  { XPOperatorLESSEQUAL,        XPOpTypeString , XPOpTypeString , XPOpTypebool    }, \
  { XPOperatorGREATEREQUAL,     XPOpTypeString , XPOpTypeString , XPOpTypebool    }, \
  { XPOperatorNOTEQUAL,         XPOpTypeString , XPOpTypeString , XPOpTypebool    }, \
  { XPOperatorUnknown,          XPOpTypeUnknown, XPOpTypeUnknown, XPOpTypeUnknown }}

/* SPECIFICATION PROPERTIES
 * ------------------------------------------------------------------
 */

/* new property class keywords */
#define XP_PROP_CHANGE_CN002        "CHANGE"
#define XP_PROP_INTERN_CN002        "INTERN"
#define XP_PROP_MANDATORY_CN002     "MANDATORY"
#define XP_PROP_CLEAR_CN002         "CLEAR"
#define XP_PROP_DYNAMIC_CN002       "DYNAMIC"
#define XP_PROP_CASESENSITIVE_CN002 "CASESENSITIVE"
#define XP_PROP_DEVSPACE_CN002      "DEVSPACE"
#define XP_PROP_MODIFY_CN002        "MODIFY"
#define XP_PROP_GROUP_CN002         "GROUP"
#define XP_PROP_DISPLAY_CN002       "DISPLAYNAME"
#define XP_PROP_VALUESET_CN002      "VALUESET"
#define XP_PROP_MAX_CN002           "MAX"
#define XP_PROP_MIN_CN002           "MIN"
#define XP_PROP_INSTANCE_CN002      "INSTANCES"
#define XP_PROP_CLASS_CN002         "CLASS"

/* new property value keywords */
#define XP_PROP_YES_CN002           "YES"
#define XP_PROP_NO_CN002            "NO"
#define XP_PROP_HIGHER_CN002        "HIGHER"
#define XP_PROP_GENERAL_CN002       "GENERAL"
#define XP_PROP_EXTENDED_CN002      "EXTENDED"
#define XP_PROP_SUPPORT_CN002       "SUPPORT"
#define XP_PROP_TRACE_CN002         "TRACE"
#define XP_PROP_EVENTS_CN002        "EVENTS"
#define XP_PROP_INFO_CN002          "INFO"
#define XP_PROP_OFFLINE_CN002       "OFFLINE"
#define XP_PROP_RUNNING_CN002       "RUNNING"

/* new property value table (<value>,<keyword>) */
#define XP_PROP_VALUES_CN002  { \
  {DBMSrvPar_Parameter::XPPropValueYes,           XP_PROP_YES_CN002        }, \
  {DBMSrvPar_Parameter::XPPropValueNo,            XP_PROP_NO_CN002         }, \
  {DBMSrvPar_Parameter::XPPropValueHigher,        XP_PROP_HIGHER_CN002     }, \
  {DBMSrvPar_Parameter::XPPropValueGeneral,       XP_PROP_GENERAL_CN002    }, \
  {DBMSrvPar_Parameter::XPPropValueExtended,      XP_PROP_EXTENDED_CN002   }, \
  {DBMSrvPar_Parameter::XPPropValueSupport,       XP_PROP_SUPPORT_CN002    }, \
  {DBMSrvPar_Parameter::XPPropValueTrace,         XP_PROP_TRACE_CN002      }, \
  {DBMSrvPar_Parameter::XPPropValueEvents,        XP_PROP_EVENTS_CN002     }, \
  {DBMSrvPar_Parameter::XPPropValueInfo,          XP_PROP_INFO_CN002       }, \
  {DBMSrvPar_Parameter::XPPropValueOffline,       XP_PROP_OFFLINE_CN002    }, \
  {DBMSrvPar_Parameter::XPPropValueRunning,       XP_PROP_RUNNING_CN002    }, \
  {DBMSrvPar_Parameter::XPPropValueUnknown,       NULL                     }}

/* class value table (<value>,<keyword>) */
#define XP_CLASS_VALUES_CN002  { \
  {DBMSrvPar_Parameter::XPClassValueNothing,      XP_PROP_NO_CN002        }, \
  {DBMSrvPar_Parameter::XPClassValueGeneral,      XP_PROP_GENERAL_CN002    }, \
  {DBMSrvPar_Parameter::XPClassValueExtended,     XP_PROP_EXTENDED_CN002   }, \
  {DBMSrvPar_Parameter::XPClassValueSupport,      XP_PROP_SUPPORT_CN002    }, \
  {DBMSrvPar_Parameter::XPClassValueInfo,         XP_PROP_INFO_CN002       }, \
  {DBMSrvPar_Parameter::XPClassValueTrace,        XP_PROP_TRACE_CN002      }, \
  {DBMSrvPar_Parameter::XPClassValueEvents,       XP_PROP_EVENTS_CN002     }, \
{DBMSrvPar_Parameter::XPClassValueUnknown,      NULL                     }}

/* new property classes */
typedef enum {
   XPPropClassChange,
   XPPropClassIntern,
   XPPropClassMandatory,
   XPPropClassClear,
   XPPropClassDynamic,
   XPPropClassCasesensitive,
   XPPropClassDevspace,
   XPPropClassModify,
   XPPropClassGroup,
   XPPropClassDisplay,
   XPPropClassValueset,
   XPPropClassMax,
   XPPropClassMin,
   XPPropClassInstance,
   XPPropClassClass,
   XPPropClassUnknown,
   XPPropClassMAX
} tcn002_XPPropClass;

/* new property class table (<class>,<defaultvalue>,<keyword>) */
/* **** Attention **** 
    Maintain the same order like in the enum above! A class will be
    found by the index  */
#define XP_PROP_CLASSES_CN002  { \
  {XPPropClassChange,        DBMSrvPar_Parameter::XPPropValueOffline, XP_PROP_CHANGE_CN002        }, \
  {XPPropClassIntern,        DBMSrvPar_Parameter::XPPropValueNo,      XP_PROP_INTERN_CN002        }, \
  {XPPropClassMandatory,     DBMSrvPar_Parameter::XPPropValueNo,      XP_PROP_MANDATORY_CN002     }, \
  {XPPropClassClear,         DBMSrvPar_Parameter::XPPropValueNo,      XP_PROP_CLEAR_CN002         }, \
  {XPPropClassDynamic,       DBMSrvPar_Parameter::XPPropValueNo,      XP_PROP_DYNAMIC_CN002       }, \
  {XPPropClassCasesensitive, DBMSrvPar_Parameter::XPPropValueNo,      XP_PROP_CASESENSITIVE_CN002 }, \
  {XPPropClassDevspace,      DBMSrvPar_Parameter::XPPropValueNo,      XP_PROP_DEVSPACE_CN002      }, \
  {XPPropClassModify,        DBMSrvPar_Parameter::XPPropValueYes,     XP_PROP_MODIFY_CN002        }, \
  {XPPropClassGroup,         DBMSrvPar_Parameter::XPPropValueGeneral, XP_PROP_GROUP_CN002         }, \
  {XPPropClassDisplay,       DBMSrvPar_Parameter::XPPropValueUnknown, XP_PROP_DISPLAY_CN002       }, \
  {XPPropClassValueset,      DBMSrvPar_Parameter::XPPropValueUnknown, XP_PROP_VALUESET_CN002      }, \
  {XPPropClassMax,           DBMSrvPar_Parameter::XPPropValueUnknown, XP_PROP_MAX_CN002           }, \
  {XPPropClassMin,           DBMSrvPar_Parameter::XPPropValueUnknown, XP_PROP_MIN_CN002           }, \
  {XPPropClassInstance,      DBMSrvPar_Parameter::XPPropValueUnknown, XP_PROP_INSTANCE_CN002      }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueUnknown, XP_PROP_CLASS_CN002         }, \
  {XPPropClassUnknown,       DBMSrvPar_Parameter::XPPropValueUnknown, NULL                        }}

/* valid values for classes */
#define XP_PROP_VALID_CN002  { \
  {XPPropClassChange,        DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassChange,        DBMSrvPar_Parameter::XPPropValueOffline   }, \
  {XPPropClassChange,        DBMSrvPar_Parameter::XPPropValueRunning   }, \
  {XPPropClassIntern,        DBMSrvPar_Parameter::XPPropValueYes       }, \
  {XPPropClassIntern,        DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassMandatory,     DBMSrvPar_Parameter::XPPropValueYes       }, \
  {XPPropClassMandatory,     DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassClear,         DBMSrvPar_Parameter::XPPropValueYes       }, \
  {XPPropClassClear,         DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassDynamic,       DBMSrvPar_Parameter::XPPropValueYes       }, \
  {XPPropClassDynamic,       DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassCasesensitive, DBMSrvPar_Parameter::XPPropValueYes       }, \
  {XPPropClassCasesensitive, DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassDevspace,      DBMSrvPar_Parameter::XPPropValueYes       }, \
  {XPPropClassDevspace,      DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassModify,        DBMSrvPar_Parameter::XPPropValueYes       }, \
  {XPPropClassModify,        DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassGroup,         DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassGroup,         DBMSrvPar_Parameter::XPPropValueGeneral   }, \
  {XPPropClassGroup,         DBMSrvPar_Parameter::XPPropValueExtended  }, \
  {XPPropClassGroup,         DBMSrvPar_Parameter::XPPropValueSupport   }, \
  {XPPropClassGroup,         DBMSrvPar_Parameter::XPPropValueTrace     }, \
  {XPPropClassGroup,         DBMSrvPar_Parameter::XPPropValueEvents    }, \
  {XPPropClassGroup,         DBMSrvPar_Parameter::XPPropValueInfo      }, \
  {XPPropClassDisplay,       DBMSrvPar_Parameter::XPPropValueUnknown   }, \
  {XPPropClassValueset,      DBMSrvPar_Parameter::XPPropValueUnknown   }, \
  {XPPropClassMax,           DBMSrvPar_Parameter::XPPropValueUnknown   }, \
  {XPPropClassMin,           DBMSrvPar_Parameter::XPPropValueUnknown   }, \
  {XPPropClassInstance,      DBMSrvPar_Parameter::XPPropValueUnknown   }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueUnknown   }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueNo        }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueGeneral   }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueExtended  }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueSupport   }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueTrace     }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueEvents    }, \
  {XPPropClassClass,         DBMSrvPar_Parameter::XPPropValueInfo      }, \
  {XPPropClassUnknown,       DBMSrvPar_Parameter::XPPropValueUnknown   }}

#define XP_MAX_VALUES_PER_CLASS_CN002 5

///* SPECIFICATION TYPES
// * ------------------------------------------------------------------
// */
///* parameter types */
//#define XP_TYPE_INT2_CN002          "int2"
//#define XP_TYPE_INT4_CN002          "int4"
//#define XP_TYPE_C8_CN002            "c8"
//#define XP_TYPE_C18_CN002           "c18"
//#define XP_TYPE_C24_CN002           "c24"
//#define XP_TYPE_C40_CN002           "c40"
//#define XP_TYPE_C64_CN002           "c64"
//#define XP_TYPE_CRYPT_CN002         "crypt"
//
///* type of parameter table (<code>,<keyword>) */
//#define XP_TYPES_CN002 { \
//  { xp_int2_type,    XP_TYPE_INT2_CN002   }, \
//  { xp_int4_type,    XP_TYPE_INT4_CN002   }, \
//  { xp_c8_type,      XP_TYPE_C8_CN002     }, \
//  { xp_c18_type,     XP_TYPE_C18_CN002    }, \
//  { xp_c24_type,     XP_TYPE_C24_CN002    }, \
//  { xp_c40_type,     XP_TYPE_C40_CN002    }, \
//  { xp_c64_type,     XP_TYPE_C64_CN002    }, \
//  { xp_crypt_type,   XP_TYPE_CRYPT_CN002  }, \
//  { xp_crypt_type,   ""                  }}

/* SPECIFICATION TYPES
 * ------------------------------------------------------------------
 */
/* parameter types */
#define XP_TYPE_INT_CN002           "int"
#define XP_TYPE_REAL_CN002          "real"
#define XP_TYPE_STRING_CN002        "string"
#define XP_TYPE_CRYPT_CN002         "crypt"

/* type of parameter table (<code>,<keyword>) */
#define XP_TYPES_CN002 { \
  { DBMSrvPar_RTEConfParameter::t_Integer,    XP_TYPE_INT_CN002    }, \
  { DBMSrvPar_RTEConfParameter::t_Real,       XP_TYPE_REAL_CN002   }, \
  { DBMSrvPar_RTEConfParameter::t_String,     XP_TYPE_STRING_CN002 }, \
  { DBMSrvPar_RTEConfParameter::t_CryptInfo,  XP_TYPE_CRYPT_CN002  }, \
  { DBMSrvPar_RTEConfParameter::t_Invalid,    ""                   }}

/* SPECIFICATION LINES
 * ------------------------------------------------------------------
 */

/* relation of keyword to line type */
#define XP_KEYWORD_LINE_CN002 { \
  {DBMSrvPar_XPLine::XPLineID,             XP_KEY_ID_CN002             }, \
  {DBMSrvPar_XPLine::XPLineTYPE,           XP_KEY_TYPE_CN002           }, \
  {DBMSrvPar_XPLine::XPLineDEFAULT,        XP_KEY_DEFAULT_CN002        }, \
  {DBMSrvPar_XPLine::XPLineDEFAULTFOR,     XP_KEY_DEFAULTFOR_CN002     }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_CHANGE_CN002        }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_INTERN_CN002        }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_MANDATORY_CN002     }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_CLEAR_CN002         }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_DYNAMIC_CN002       }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_CASESENSITIVE_CN002 }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_DEVSPACE_CN002      }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_MODIFY_CN002        }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_INFO_CN002          }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_GROUP_CN002         }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_DISPLAY_CN002       }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_VALUESET_CN002      }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_MAX_CN002           }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_MIN_CN002           }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_INSTANCE_CN002      }, \
  {DBMSrvPar_XPLine::XPLineProperty,       XP_PROP_CLASS_CN002         }, \
  {DBMSrvPar_XPLine::XPLineCODE,           XP_KEY_CODE_CN002           }, \
  {DBMSrvPar_XPLine::XPLineENDCODE,        XP_KEY_ENDCODE_CN002        }, \
  {DBMSrvPar_XPLine::XPLineCodeCALC,       XP_KEY_CALC_CN002           }, \
  {DBMSrvPar_XPLine::XPLineCodeCONSTRAINT, XP_KEY_CONSTRAINT_CN002     }, \
  {DBMSrvPar_XPLine::XPLineCodeIF,         XP_KEY_IF_CN002             }, \
  {DBMSrvPar_XPLine::XPLineCodeELSIF,      XP_KEY_ELSIF_CN002          }, \
  {DBMSrvPar_XPLine::XPLineCodeELSE,       XP_KEY_ELSE_CN002           }, \
  {DBMSrvPar_XPLine::XPLineEXPLAIN,        XP_KEY_EXPLAIN_CN002        }, \
  {DBMSrvPar_XPLine::XPLineENDEXPLAIN,     XP_KEY_ENDEXPLAIN_CN002     }, \
  {DBMSrvPar_XPLine::XPLineHELP,           XP_KEY_HELP_CN002           }, \
  {DBMSrvPar_XPLine::XPLineENDHELP,        XP_KEY_ENDHELP_CN002        }, \
  {DBMSrvPar_XPLine::XPLineUnknown,        ""                         }}

/* valid line transitions */
#define XP_LINE_TRANSITIONS_CN002 { \
  {DBMSrvPar_XPLine::XPLineBOF,            DBMSrvPar_XPLine::XPLineID            }, \
  {DBMSrvPar_XPLine::XPLineBOF,            DBMSrvPar_XPLine::XPLineEOF           }, \
  {DBMSrvPar_XPLine::XPLineID,             DBMSrvPar_XPLine::XPLineTYPE          }, \
  {DBMSrvPar_XPLine::XPLineTYPE,           DBMSrvPar_XPLine::XPLineDEFAULT       }, \
  {DBMSrvPar_XPLine::XPLineTYPE,           DBMSrvPar_XPLine::XPLineProperty      }, \
  {DBMSrvPar_XPLine::XPLineTYPE,           DBMSrvPar_XPLine::XPLineCODE          }, \
  {DBMSrvPar_XPLine::XPLineTYPE,           DBMSrvPar_XPLine::XPLineEXPLAIN       }, \
  {DBMSrvPar_XPLine::XPLineTYPE,           DBMSrvPar_XPLine::XPLineHELP          }, \
  {DBMSrvPar_XPLine::XPLineTYPE,           DBMSrvPar_XPLine::XPLineEOF           }, \
  {DBMSrvPar_XPLine::XPLineTYPE,           DBMSrvPar_XPLine::XPLineID            }, \
  {DBMSrvPar_XPLine::XPLineDEFAULT,        DBMSrvPar_XPLine::XPLineProperty      }, \
  {DBMSrvPar_XPLine::XPLineDEFAULT,        DBMSrvPar_XPLine::XPLineCODE          }, \
  {DBMSrvPar_XPLine::XPLineDEFAULT,        DBMSrvPar_XPLine::XPLineEXPLAIN       }, \
  {DBMSrvPar_XPLine::XPLineDEFAULT,        DBMSrvPar_XPLine::XPLineHELP          }, \
  {DBMSrvPar_XPLine::XPLineDEFAULT,        DBMSrvPar_XPLine::XPLineEOF           }, \
  {DBMSrvPar_XPLine::XPLineDEFAULT,        DBMSrvPar_XPLine::XPLineID            }, \
  {DBMSrvPar_XPLine::XPLineDEFAULT,        DBMSrvPar_XPLine::XPLineDEFAULTFOR    }, \
  {DBMSrvPar_XPLine::XPLineDEFAULTFOR,     DBMSrvPar_XPLine::XPLineProperty      }, \
  {DBMSrvPar_XPLine::XPLineDEFAULTFOR,     DBMSrvPar_XPLine::XPLineCODE          }, \
  {DBMSrvPar_XPLine::XPLineDEFAULTFOR,     DBMSrvPar_XPLine::XPLineEXPLAIN       }, \
  {DBMSrvPar_XPLine::XPLineDEFAULTFOR,     DBMSrvPar_XPLine::XPLineHELP          }, \
  {DBMSrvPar_XPLine::XPLineDEFAULTFOR,     DBMSrvPar_XPLine::XPLineEOF           }, \
  {DBMSrvPar_XPLine::XPLineDEFAULTFOR,     DBMSrvPar_XPLine::XPLineID            }, \
  {DBMSrvPar_XPLine::XPLineDEFAULTFOR,     DBMSrvPar_XPLine::XPLineDEFAULTFOR    }, \
  {DBMSrvPar_XPLine::XPLineProperty,       DBMSrvPar_XPLine::XPLineProperty      }, \
  {DBMSrvPar_XPLine::XPLineProperty,       DBMSrvPar_XPLine::XPLineCODE          }, \
  {DBMSrvPar_XPLine::XPLineProperty,       DBMSrvPar_XPLine::XPLineEXPLAIN       }, \
  {DBMSrvPar_XPLine::XPLineProperty,       DBMSrvPar_XPLine::XPLineHELP          }, \
  {DBMSrvPar_XPLine::XPLineProperty,       DBMSrvPar_XPLine::XPLineEOF           }, \
  {DBMSrvPar_XPLine::XPLineProperty,       DBMSrvPar_XPLine::XPLineID            }, \
  {DBMSrvPar_XPLine::XPLineCODE,           DBMSrvPar_XPLine::XPLineCodeIF        }, \
  {DBMSrvPar_XPLine::XPLineCODE,           DBMSrvPar_XPLine::XPLineCodeCALC      }, \
  {DBMSrvPar_XPLine::XPLineCODE,           DBMSrvPar_XPLine::XPLineCodeCONSTRAINT}, \
  {DBMSrvPar_XPLine::XPLineCODE,           DBMSrvPar_XPLine::XPLineENDCODE       }, \
  {DBMSrvPar_XPLine::XPLineCodeIF,         DBMSrvPar_XPLine::XPLineCodeCALC      }, \
  {DBMSrvPar_XPLine::XPLineCodeELSIF,      DBMSrvPar_XPLine::XPLineCodeCALC      }, \
  {DBMSrvPar_XPLine::XPLineCodeELSE,       DBMSrvPar_XPLine::XPLineCodeCALC      }, \
  {DBMSrvPar_XPLine::XPLineCodeCALC,       DBMSrvPar_XPLine::XPLineCodeCALC      }, \
  {DBMSrvPar_XPLine::XPLineCodeCALC,       DBMSrvPar_XPLine::XPLineCodeCONSTRAINT}, \
  {DBMSrvPar_XPLine::XPLineCodeCALC,       DBMSrvPar_XPLine::XPLineCodeIF        }, \
  {DBMSrvPar_XPLine::XPLineCodeCALC,       DBMSrvPar_XPLine::XPLineCodeELSIF     }, \
  {DBMSrvPar_XPLine::XPLineCodeCALC,       DBMSrvPar_XPLine::XPLineCodeELSE      }, \
  {DBMSrvPar_XPLine::XPLineCodeCALC,       DBMSrvPar_XPLine::XPLineENDCODE       }, \
  {DBMSrvPar_XPLine::XPLineCodeCONSTRAINT, DBMSrvPar_XPLine::XPLineCodeCALC      }, \
  {DBMSrvPar_XPLine::XPLineCodeCONSTRAINT, DBMSrvPar_XPLine::XPLineCodeCONSTRAINT}, \
  {DBMSrvPar_XPLine::XPLineCodeCONSTRAINT, DBMSrvPar_XPLine::XPLineCodeIF        }, \
  {DBMSrvPar_XPLine::XPLineCodeCONSTRAINT, DBMSrvPar_XPLine::XPLineCodeELSIF     }, \
  {DBMSrvPar_XPLine::XPLineCodeCONSTRAINT, DBMSrvPar_XPLine::XPLineCodeELSE      }, \
  {DBMSrvPar_XPLine::XPLineCodeCONSTRAINT, DBMSrvPar_XPLine::XPLineENDCODE       }, \
  {DBMSrvPar_XPLine::XPLineENDCODE,        DBMSrvPar_XPLine::XPLineEXPLAIN       }, \
  {DBMSrvPar_XPLine::XPLineENDCODE,        DBMSrvPar_XPLine::XPLineHELP          }, \
  {DBMSrvPar_XPLine::XPLineENDCODE,        DBMSrvPar_XPLine::XPLineEOF           }, \
  {DBMSrvPar_XPLine::XPLineENDCODE,        DBMSrvPar_XPLine::XPLineID            }, \
  {DBMSrvPar_XPLine::XPLineEXPLAIN,        DBMSrvPar_XPLine::XPLineENDEXPLAIN    }, \
  {DBMSrvPar_XPLine::XPLineEXPLAIN,        DBMSrvPar_XPLine::XPLineEXPLAINFollow }, \
  {DBMSrvPar_XPLine::XPLineEXPLAINFollow,  DBMSrvPar_XPLine::XPLineEXPLAINFollow }, \
  {DBMSrvPar_XPLine::XPLineEXPLAINFollow,  DBMSrvPar_XPLine::XPLineENDEXPLAIN    }, \
  {DBMSrvPar_XPLine::XPLineENDEXPLAIN,     DBMSrvPar_XPLine::XPLineHELP,         }, \
  {DBMSrvPar_XPLine::XPLineENDEXPLAIN,     DBMSrvPar_XPLine::XPLineEOF,          }, \
  {DBMSrvPar_XPLine::XPLineENDEXPLAIN,     DBMSrvPar_XPLine::XPLineID,           }, \
  {DBMSrvPar_XPLine::XPLineHELP,           DBMSrvPar_XPLine::XPLineHELPFollow,   }, \
  {DBMSrvPar_XPLine::XPLineHELP,           DBMSrvPar_XPLine::XPLineENDHELP,      }, \
  {DBMSrvPar_XPLine::XPLineHELPFollow,     DBMSrvPar_XPLine::XPLineHELPFollow,   }, \
  {DBMSrvPar_XPLine::XPLineHELPFollow,     DBMSrvPar_XPLine::XPLineENDHELP,      }, \
  {DBMSrvPar_XPLine::XPLineENDHELP,        DBMSrvPar_XPLine::XPLineEOF,          }, \
  {DBMSrvPar_XPLine::XPLineENDHELP,        DBMSrvPar_XPLine::XPLineID,           }, \
  {DBMSrvPar_XPLine::XPLineUnknown,        DBMSrvPar_XPLine::XPLineUnknown       }}

/*  {DBMSrvPar_XPLine::XPLineCodeIF,         DBMSrvPar_XPLine::XPLineCodeCONSTRAINT}, \ */
/*  {DBMSrvPar_XPLine::XPLineCodeELSIF,      DBMSrvPar_XPLine::XPLineCodeCONSTRAINT}, \ */
/*  {DBMSrvPar_XPLine::XPLineCodeELSE,       DBMSrvPar_XPLine::XPLineCodeCONSTRAINT}, \ */

/* SPECIFICATION STUFF
 * ------------------------------------------------------------------
 */
#define XP_ANNOTATION_LENGTH_MXCN002    83
#define XP_PARAMLINE_LENGTH_MXCN002     90
#define XP_LINE_LENGTH_MXCN002          512
#define XP_IL_MINDYN_CN002              "MIN_DYN_VALUE"
#define XP_IL_MAXDYN_CN002              "MAX_DYN_VALUE"
#define XP_FILE_CN002                   "xparam.pcf"
#define XP_NEWFILE_CN002                "cserv.pcf"
#define XP_RUNDIR_CN002                 "wrk"
#define XP_FILENAME_LENGTH_MXCN002      1024
#define XP_PROPLINE_LENGTH_MXCN002      200
#define XP_OPERANDS_MXCN002             256
#define XP_STRING_LENGTH_MXCN002        RTECONF_MAXSTRINGLENGTH
#define XP_DEFAULTFOR_MXCN002             5
#define XP_DEFAULTFORINSTANCE_CN002       0
#define XP_DEFAULTFORVALUE_CN002          1

/* check state */
#define XP_CALC_STATES_TABLE_CN002 { \
  {DBMSrvPar_Parameter::XPCalcStateOK,          "OK"         }, \
  {DBMSrvPar_Parameter::XPCalcStateConstraint,  "Constraint" }, \
  {DBMSrvPar_Parameter::XPCalcStateMandatory,   "Mandatory"  }, \
  {DBMSrvPar_Parameter::XPCalcStateUnknown,     "Unknown"    }}

/* get requests */
typedef enum {
  XPRequestUnknown, 
  XPRequestDirectGet, 
  XPRequestExtGet, 
  XPRequestType, 
  XPRequestDefault, 
  XPRequestValue, 
  XPRequestValueIntern, 
  XPRequestProperties, 
  XPRequestClass, 
  XPRequestExplain, 
  XPRequestHelp, 
  XPRequestGood, 
  XPRequestActive, 
  XPRequestFull
} tcn002_XPRequest;

/* SPECIFICATION STRUCTURES
 * ------------------------------------------------------------------
 */
typedef CSTRING_TYPE_MSP00      (  tcn002_XpValueString, RTECONF_MAXSTRINGLENGTH);
typedef CSTRING_TYPE_MSP00      (  tcn002_XpValueName, RTECONF_MAXNAMELENGTH);

/* XParam operand structure */
typedef struct tcn002_XPOperand {
  tcn002_XPOpType            nOpType;            /* type of operand */ 
  union {
    long                    nValue;
    bool                    bValue;
    tcn002_XpValueString    szValue;
  } opValue; 
} tcn002_XPOperand;

#define PAHI_ACTIVE_CN002         _T('A')
#define PAHI_CHANGE_CN002         _T('C')
#define PAHI_DELETE_CN002         _T('D')

#endif /* _GCN002_H_ */
