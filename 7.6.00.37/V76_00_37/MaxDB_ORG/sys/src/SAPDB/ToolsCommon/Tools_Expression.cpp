/*!
  -----------------------------------------------------------------------------
  module: Tools_Expression.cpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: expression handling
  description:  some classes to handle expressions

  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ToolsCommon/Tools_Expression.hpp"

/*
  -----------------------------------------------------------------------------
  implementation class Tools_ExpressionValue
  -----------------------------------------------------------------------------
 */
const Tools_ExpressionValue::ExInt Tools_ExpressionValue::UndefPrecision = -1;

/*
  -----------------------------------------------------------------------------
  public constructor Tools_ExpressionValue::Tools_ExpressionValue
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::Tools_ExpressionValue
      (  )
{
  Init();
} // end Tools_ExpressionValue::Tools_ExpressionValue
/*
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::Tools_ExpressionValue
      ( ExType oType )
{
  Init(oType);
} // end Tools_ExpressionValue::Tools_ExpressionValue
/*
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::Tools_ExpressionValue
      ( const ExDouble & oInitValue )
{
  Init(ExTypeNumeric);
  SetNewValue(oInitValue);
} // end Tools_ExpressionValue::Tools_ExpressionValue
/*
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::Tools_ExpressionValue
      ( const ExString & oInitValue )
{
  Init(ExTypeString);
  SetNewValue(oInitValue);
} // end Tools_ExpressionValue::Tools_ExpressionValue
/*
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::Tools_ExpressionValue
      ( const ExDouble & oInitValue,
        ExType           oType )
{
  Init(oType);
  SetNewValue(oInitValue);
} // end Tools_ExpressionValue::Tools_ExpressionValue
/*
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::Tools_ExpressionValue
      ( const ExString & oInitValue,
        ExType           oType )
{
  Init(oType);
  SetNewValue(oInitValue);
} // end Tools_ExpressionValue::Tools_ExpressionValue

/*
  -----------------------------------------------------------------------------
  public function Tools_ExpressionValue::SetNewValue
  -----------------------------------------------------------------------------
 */
void Tools_ExpressionValue::SetNewValue
      ( const ExDouble & oNewValue )
{
  m_oType  = (m_oType == ExTypeUnknown) ? ExTypeNumeric : m_oType;
  m_bUndef = false;
  
  if (m_oType == ExTypeNumeric) {
    m_oLastNumericValue = m_oNumericValue;
    m_oNumericValue     = oNewValue;
    if (m_oLastNumericValue != m_oNumericValue) {
      time(&m_tLastChanged);
    } // end if
  } else if (m_oType == ExTypeString) {
    SetNewValue(ConvertToString(oNewValue, m_nPrecision));
  } // end if
} // end Tools_ExpressionValue::SetNewValue
/*
  -----------------------------------------------------------------------------
 */
void Tools_ExpressionValue::SetNewValue
      ( const ExString & oNewValue )
{
  m_oType  = (m_oType == ExTypeUnknown) ? ExTypeString : m_oType;
  m_bUndef = false;

  if (m_oType == ExTypeNumeric) {
    ExString szTmp = oNewValue;
    SetNewValue(atof(szTmp.CharPtr()));
  } else if (m_oType == ExTypeString) {
    m_oLastStringValue    = m_oStringValue;
    m_oStringValue        = oNewValue;
    if (m_oLastStringValue != m_oStringValue) {
      time(&m_tLastChanged);
    } // end if
  } // end if
} // end Tools_ExpressionValue::SetNewValue
/*
  -----------------------------------------------------------------------------
 */
void Tools_ExpressionValue::SetNewValue
      ( const ExDouble & oNewValue,
        ExType           oNewType )
{
  m_oType = oNewType;
  SetNewValue(oNewValue);
} // end Tools_ExpressionValue::SetNewValue
/*
  -----------------------------------------------------------------------------
 */
void Tools_ExpressionValue::SetNewValue
      ( const ExString & oNewValue,
        ExType           oNewType )
{
  m_oType = oNewType;
  SetNewValue(oNewValue);
} // end Tools_ExpressionValue::SetNewValue

/*
  -----------------------------------------------------------------------------
  public function Tools_ExpressionValue::GetStringValue
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::ExString Tools_ExpressionValue::GetStringValue 
      ( ) 
{
  if (IsDefined()) {
    if (m_oType == ExTypeString) {
      return m_oStringValue;
    } else if (m_oType == ExTypeNumeric) {
      return ConvertToString(m_oNumericValue, m_nPrecision);
    } else {
      return "";
    } // end if
  } else {
      return "";
  } // end if
} // end Tools_ExpressionValue::GetStringValue

/*
  -----------------------------------------------------------------------------
  public function Tools_ExpressionValue::GetNumericValue
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::ExDouble Tools_ExpressionValue::GetNumericValue
      ( )
{
  if (IsDefined()) {
    if (m_oType == ExTypeString) {
      return atof(m_oStringValue.CharPtr());
    } else if (m_oType == ExTypeNumeric) {
      return m_oNumericValue;
    } else {
      return 0;
    } // end if
  } else {
    return 0;
  } // end if
} // end Tools_ExpressionValue::GetNumericValue

/*
  -----------------------------------------------------------------------------
  private function Tools_ExpressionValue::SetNewValue
  -----------------------------------------------------------------------------
 */
void Tools_ExpressionValue::SetNewValue
      ( const Tools_ExpressionValue * pNewValue )
{
  if (pNewValue != NULL) {
    m_oType               =  pNewValue->m_oType;                   
    m_oNumericValue       =  pNewValue->m_oNumericValue;           
    m_oStringValue        =  pNewValue->m_oStringValue;            
    m_oLastNumericValue   =  pNewValue->m_oLastNumericValue;       
    m_oLastStringValue    =  pNewValue->m_oLastStringValue;        
    m_bUndef              =  pNewValue->m_bUndef;        
    m_nPrecision          =  pNewValue->m_nPrecision;
    m_tLastChanged        =  pNewValue->m_tLastChanged;
  } else {
    Init();
  } // end if
} // end Tools_ExpressionValue::SetNewValue

/*
  -----------------------------------------------------------------------------
  private function Tools_ExpressionValue::GetLastStringValue
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::ExString Tools_ExpressionValue::GetLastStringValue 
      ( )
{
  if (IsDefined()) {
    if (m_oType == ExTypeString) {
      return m_oLastStringValue;
    } else if (m_oType == ExTypeNumeric) {
      return ConvertToString(m_oLastNumericValue, m_nPrecision);
    } else {
      return "";
    } // end if
  } else {
    return "";
  } // end if
} // end Tools_ExpressionValue::GetLastStringValue

/*
  -----------------------------------------------------------------------------
  private function Tools_ExpressionValue::GetLastNumericValue
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::ExDouble Tools_ExpressionValue::GetLastNumericValue
      ( )
{
  if (IsDefined()) {
    if (m_oType == ExTypeString) {
      return atof(m_oLastStringValue.CharPtr());
    } else if (m_oType == ExTypeNumeric) {
      return m_oLastNumericValue;
    } else {
      return 0;
    } // end if
  } else {
    return 0;
  } // end if
} // end Tools_ExpressionValue::GetLastNumericValue

/*
  -----------------------------------------------------------------------------
  private function Tools_ExpressionValue::GetAge
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::ExDouble Tools_ExpressionValue::GetAge
      ( )
{
  time_t tNow;

  time(&tNow);
  return (ExDouble) (tNow - m_tLastChanged);
} // end Tools_ExpressionValue::GetAge

/*
  -----------------------------------------------------------------------------
  private function Tools_ExpressionValue::GetAgeString
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::ExString Tools_ExpressionValue::GetAgeString
      ( )
{
  return ConvertToString(GetAge(), 0);
} // end Tools_ExpressionValue::GetAgeString

/*
  -----------------------------------------------------------------------------
  private function Tools_ExpressionValue::Init
  -----------------------------------------------------------------------------
 */
void Tools_ExpressionValue::Init
      ( ExType oType,
        ExInt  nPrecision )
{
  m_oType                   = oType;
  m_oNumericValue           = 0;
  m_oStringValue            = "";
  m_oLastNumericValue       = 0;
  m_oLastStringValue        = "";
  m_bUndef                  = (m_oType == ExTypeUnknown);
  m_nPrecision              = nPrecision;
  time(&m_tLastChanged);
} // end Tools_ExpressionValue::Init


/*
  -----------------------------------------------------------------------------
  private function Tools_ExpressionValue::ConvertToString
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue::ExString Tools_ExpressionValue::ConvertToString
  ( ExDouble nValue, 
    ExInt    nPrecision)
{
  char szTmp[100];

  if (nPrecision > Tools_ExpressionValue::UndefPrecision) {
    sprintf(szTmp, "%.*f", (int) nPrecision, nValue);
  } else {
    sprintf(szTmp, "%.*f", (int) 2, nValue);
  } // end if
  ExString szTrimmer(szTmp);
  if (nPrecision == Tools_ExpressionValue::UndefPrecision) {
    szTrimmer.TrimTrailing("0");
    szTrimmer.TrimTrailing(".");
  } // end if

  return szTrimmer;
} // end Tools_ExpressionValue::ConvertToString

/*
  -----------------------------------------------------------------------------
  implementation class Tools_Expression
  -----------------------------------------------------------------------------
 */

/*
  -----------------------------------------------------------------------------
  public constructor Tools_Expression::Tools_Expression
  -----------------------------------------------------------------------------
 */
Tools_Expression::Tools_Expression
      ( Tools_ValueProvider           & oProvider )
      : m_oProvider(oProvider),
        m_oArray(x_oArray)
{
  m_sExpression = "";
  m_oError      = ExError_OK;
  m_nErrorPos   = 0;
  m_pTree       = NULL;
} // end Tools_Expression::Tools_Expression
/*
  -----------------------------------------------------------------------------
 */
Tools_Expression::Tools_Expression
      ( Tools_ValueProvider           & oProvider,
        const Tools_DynamicUTF8String & sExpression )
      : m_oProvider(oProvider),
        m_oArray(x_oArray)
{
  m_sExpression = sExpression;
  m_oError      = ExError_OK;
  m_nErrorPos   = 0;
  m_pTree       = NULL;
} // end Tools_Expression::Tools_Expression

/*
  -----------------------------------------------------------------------------
  public destructor Tools_Expression::~Tools_Expression
  -----------------------------------------------------------------------------
 */
Tools_Expression::~Tools_Expression
      (  )
{
} // end Tools_Expression::~Tools_Expression

/*
  -----------------------------------------------------------------------------
  public function Tools_Expression::SetExpression
  -----------------------------------------------------------------------------
 */
void Tools_Expression::SetExpression
      ( const Tools_DynamicUTF8String & sExpression )
{
  m_sExpression = sExpression;
} // end Tools_Expression::SetExpression

/*
  -----------------------------------------------------------------------------
  public function Tools_Expression::GetExpression
  -----------------------------------------------------------------------------
 */
const Tools_DynamicUTF8String & Tools_Expression::GetExpression
      (  )
{
  return m_sExpression;
} // end Tools_Expression::GetExpression

/*
  -----------------------------------------------------------------------------
  public function Tools_Expression::Value
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_Expression::Value
      ( Tools_ExpressionValue & oValue,
        Tools_ExpressionValueArray::size_type nIndex )
{
  SAPDB_Bool              bState       = true;
  Tools_ExpressionValue * pValueResult = NULL;

  oValue.SetUndefined();

  if (m_pTree != NULL) {
    pValueResult = CalculateExpression(m_pTree, nIndex);
    if (pValueResult != NULL) {
      if (pValueResult->GetType() == Tools_ExpressionValue::ExTypeNumeric) {
        oValue.SetNewValue(pValueResult->GetNumericValue(), Tools_ExpressionValue::ExTypeNumeric);
      } else if (pValueResult->GetType() == Tools_ExpressionValue::ExTypeString) {
        oValue.SetNewValue(pValueResult->GetStringValue(), Tools_ExpressionValue::ExTypeString);
      } else {
        bState = false;
      } // end if
    } else {
      bState = false;
    } // end if
  } else {
    // no expression found
    bState = false;
    m_oError    = ExError_PARSE;
    m_nErrorPos = 0;
  } // end if

  return bState;
} // end Tools_Expression::Value

/*
  -----------------------------------------------------------------------------
  public function Tools_Expression::ExplainValue
  -----------------------------------------------------------------------------
 */
void Tools_Expression::ExplainValue 
      ( Tools_DynamicUTF8String & sOrgExpression, 
        Tools_DynamicUTF8String & sValueExpression,
        Tools_ExpressionValueArray::size_type nIndex )
{
  // Trace the formula to calculate
  Tools_ExpressionValue      * pValue    = NULL;
  Tools_ExpressionValueArray * pArray    = NULL;
  SAPDB_UInt                   nPosition = 0;
  Tools_DynamicUTF8String      sOriginal;
  Tools_DynamicUTF8String      sValue;
  Tools_DynamicUTF8String      sOpenBracket;
  Tools_DynamicUTF8String      sCloseBracket;

  sOrgExpression.Erase();
  sValueExpression.Erase();

  while (m_oArray[nPosition].szToken.Length() > 0) {

    sOpenBracket.Erase();
    sCloseBracket.Erase();
    sOriginal.Erase();
    sValue.Erase();

    pValue = NULL;
    pArray = NULL;

    switch (m_oArray[nPosition].nTokenType) {
      case TokenTypeArr:
        sCloseBracket = "[\"";
        pArray = m_oProvider.AskForArray (m_oArray[nPosition].szToken);
        if (pArray != NULL) {
          if (nIndex != -1) {
            sCloseBracket.Append((*pArray)[nIndex].GetKey());
            if ((*pArray)[nIndex].IsDefined()) {
              sValue.Append(((*pArray)[nIndex].GetType() == Tools_ExpressionValue::ExTypeString) ? "\"" : "");
              sValue.Append((*pArray)[nIndex].GetStringValue());
              sValue.Append(((*pArray)[nIndex].GetType() == Tools_ExpressionValue::ExTypeString) ? "\"" : "");
            } else {
              sValue.Append("<undef>");
            } // end if
          } else {
            sValue.Append("<undef>");
          } // end if
        } else {
          sValue.Append("<unknown>");
        } // end if

        sCloseBracket.Append("\"]");
        sOriginal  = m_oArray[nPosition].szToken;
        break;

      case TokenTypeIdt:
        sOriginal  = m_oArray[nPosition].szToken;

        pValue = m_oProvider.AskForValue (m_oArray[nPosition].szToken);
        if (pValue != NULL) {
          if (pValue->IsDefined()) {
            sValue.Append((pValue->GetType() == Tools_ExpressionValue::ExTypeString) ? "\"" : "");
            sValue.Append(pValue->GetStringValue());
            sValue.Append((pValue->GetType() == Tools_ExpressionValue::ExTypeString) ? "\"" : "");
          } else {
            sValue.Append("<undef>");
          } // end if
        } else {
          sValue.Append("<unknown>");
        } // end if
        break;

      case TokenTypeAge:
        ++nPosition;
        if (m_oArray[nPosition].nTokenType == TokenTypeArr) {
          sOpenBracket    = "age(";
          sCloseBracket   = "[\"";
          pArray = m_oProvider.AskForArray (m_oArray[nPosition].szToken);
          if (pArray != NULL) {
            sCloseBracket.Append((*pArray)[nIndex].GetKey());
            pValue = &((*pArray)[nIndex]);
          } // end if
          sCloseBracket.Append("\"])");
        } else {
          sOpenBracket    = "age(";
          sCloseBracket   = ")";
          pValue = m_oProvider.AskForValue (m_oArray[nPosition].szToken);
        } // end if
        sOriginal  = m_oArray[nPosition].szToken;

        if (pValue) {
          if (pValue->IsDefined()) {
            sValue.Append(pValue->GetAgeString());
          } else {
            sValue.Append("<undef>");
          } // end if
        } else {
          sValue.Append("<unknown>");
        } // end if
        break;

      case TokenTypeLst:
        ++nPosition;
        if (m_oArray[nPosition].nTokenType == TokenTypeArr) {
          sOpenBracket    = "last(";
          sCloseBracket   =  "[\"";
          pArray = m_oProvider.AskForArray (m_oArray[nPosition].szToken);
          if (pArray != NULL) {
            sCloseBracket.Append((*pArray)[nIndex].GetKey());
            pValue = &((*pArray)[nIndex]);
          } // end if
          sCloseBracket.Append("\"])");
        } else {
          sOpenBracket    = "last(";
          sCloseBracket   = ")";
          pValue = m_oProvider.AskForValue (m_oArray[nPosition].szToken);
        } // end if
        sOriginal  = m_oArray[nPosition].szToken;

        if (pValue) {
          if (pValue->IsDefined()) {
            sValue.Append((pValue->GetType() == Tools_ExpressionValue::ExTypeString) ? "\"" : "");
            sValue.Append(pValue->GetLastStringValue());
            sValue.Append((pValue->GetType() == Tools_ExpressionValue::ExTypeString) ? "\"" : "");
          } else {
            sValue.Append("<undef>");
          } // end if
        } else {
          sValue.Append("<unknown>");
        } // end if
        break;

      case TokenTypeStr:
        sOpenBracket    = "\"";
        sCloseBracket   = "\"";
        sOriginal  = m_oArray[nPosition].szToken;
        sValue.Append("\"");
        sValue.Append(m_oArray[nPosition].szToken);
        sValue.Append("\"");
        break;

      default:
        sValue.Append(m_oArray[nPosition].szToken);
        sOriginal.Append(m_oArray[nPosition].szToken);
        break;
    } // end switch

    sOrgExpression.Append(sOpenBracket);
    sOrgExpression.Append(sOriginal);
    sOrgExpression.Append(sCloseBracket);
    sOrgExpression.Append(" ");

    sValueExpression.Append(sValue);
    sValueExpression.Append(" ");

    if (sOrgExpression.Length() > sValueExpression.Length()) {
      sValueExpression.Append(sOrgExpression.Length() - sValueExpression.Length(), ' ');
    } else if (sOrgExpression.Length() < sValueExpression.Length()) {
      sOrgExpression.Append(sValueExpression.Length() - sOrgExpression.Length(), ' ');
    } // end if

    ++nPosition;
  } // end while

} // end Tools_Expression::ExplainValue

/*
  -----------------------------------------------------------------------------
  public function Tools_Expression::ParseExpression
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_Expression::ParseExpression
      (  )
{
  SAPDB_Bool bState     = true;
  SAPDB_UInt nPosition  = 0;
  SAPDB_UInt nArrSize   = 0;
 
  CleanTree(m_pTree);

  bState = CreateTokens(x_oArray, nArrSize);

  if (bState) {
    if (nArrSize == 0) {
      m_oError   = ExError_EMPTY;
      m_nErrorPos = 0;
      bState     = false;
    } else {
      m_pTree = ParseLevel1(m_oArray, nArrSize, nPosition);
      bState = !(m_pTree == NULL);
      if (bState && (nPosition <  nArrSize)) {
        CleanTree (m_pTree);
        m_oError    = ExError_SYNTAX;
        m_nErrorPos = m_oArray[nPosition].nTokenPos;
        bState = false;
      } // end if
    } // end if
  } // end if

/* // BEGIN TRACE

  if (bState) {
    printf("Parsed: %s\n" , m_sExpression.CharPtr());
  } else {
    printf("Error:  %s\n" , m_sExpression.CharPtr());
  } // end if

*/ // END TRACE

  return bState;
} // end Tools_Expression::ParseExpression
/*
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_Expression::ParseExpression
      ( const Tools_DynamicUTF8String & sExpression )
{
  SetExpression(sExpression);
  return ParseExpression();
} // end Tools_Expression::ParseExpression

/*
  -----------------------------------------------------------------------------
  public function Tools_Expression::LastErrorText
  -----------------------------------------------------------------------------
 */
Tools_DynamicUTF8String Tools_Expression::LastErrorText
      (  )
{

  switch (m_oError) {
    case ExError_OK:
      m_szError = "OK";
      break;
    case ExError_SYNTAX:    // syntax error
      m_szError = "Syntax error";
      break;
    case ExError_DIV0:      // division by zero
      m_szError = "Division by zero";
      break;
    case ExError_UNDEFVAR:  // undefined variable
      m_szError = "Undefined variable";
      break;
    case ExError_MEMORY:    // memory allocation problem
      m_szError = "Memory allocation error";
      break;
    case ExError_TYPE:      // data type mismatch
      m_szError = "Data type mismatch";
      break;
    case ExError_EMPTY:     // expression is empty
      m_szError = "Expression is empty";
      break;
    case ExError_BRACKET:   // unclosed bracket
      m_szError = "Unclosed bracket";
      break;
    case ExError_PARSE:     // not parsed
      m_szError = "No parsed expression";
      break;
    case ExError_UNDEFVAL:  // undefined value
      m_szError = "Undefined value";
      break;
    case ExError_INDEX:     // invalid index
      m_szError = "Invalid Index";
      break;
    default:
      m_szError = "Unknown error code";
      break;
  } // end switch
  
  return m_szError;
} // end Tools_Expression::LastErrorText

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::CreateTokens
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_Expression::CreateTokens
      ( TokenArray & oArray,
        SAPDB_UInt & nArrSize )
{
  Tools_DynamicUTF8String::BasisElementIndex nExpIndex  = 0;
  Tools_DynamicUTF8String::BasisElementIndex nEndIndex  = 0;
  SAPDB_UInt                                 nTokIndex  = 0;
  SAPDB_UInt                                 nArrIndex  = 0;
  Tools_DynamicUTF8String::ElementCount      nExpLength = m_sExpression.Length();
  SAPDB_Bool                                 bPoint     = false;

  static ExTokens TokenList[] = {
    {" " ,      1, NULL, 0, TokenTypeSpc},
    {"\t",      1, NULL, 0, TokenTypeSpc},
    {"\n",      1, NULL, 0, TokenTypeSpc},
    {"\r",      1, NULL, 0, TokenTypeSpc},
    {"+" ,      1, NULL, 0, TokenTypeAdd},
    {"-" ,      1, NULL, 0, TokenTypeSub},
    {"*" ,      1, NULL, 0, TokenTypeMul},
    {"/" ,      1, NULL, 0, TokenTypeDiv},
    {"last(",   5, ")",  1, TokenTypeLst},
    {"age(",    4, ")",  1, TokenTypeAge},
    {"!=",      2, NULL, 0, TokenTypeNoe},
    {"like",    4, NULL, 0, TokenTypeLik},
    {"==",      2, NULL, 0, TokenTypeEqu},
    {"&&",      2, NULL, 0, TokenTypeAnd},
    {"||",      2, NULL, 0, TokenTypeOr },
    {"^" ,      1, NULL, 0, TokenTypePot},
    {">=",      2, NULL, 0, TokenTypeGre},
    {">" ,      1, NULL, 0, TokenTypeGrt},
    {"<=",      2, NULL, 0, TokenTypeLoe},
    {"<" ,      1, NULL, 0, TokenTypeLot},
    {"#" ,      1, NULL, 0, TokenTypeAlg},
    {"!" ,      1, NULL, 0, TokenTypeNot},
    {"(" ,      1, NULL, 0, TokenTypeOpb},
    {")" ,      1, NULL, 0, TokenTypeClb},
    {"%" ,      1, NULL, 0, TokenTypeMod},
    {"\"",      1, "\"", 1, TokenTypeStr},
    {"[]",      2, NULL, 0, TokenTypeArr},
    {NULL,      0, NULL, 0, TokenTypeUnk}
  };

  nArrSize = 0;

  while (nExpIndex < nExpLength) {

    // check token array
    nTokIndex = 0;
    while (TokenList[nTokIndex].szToken != NULL) {

      if (m_sExpression.Compare(nExpIndex, TokenList[nTokIndex].nLength, TokenList[nTokIndex].szToken) == 0) {

        switch (TokenList[nTokIndex].nTokenType) {
          // string
          case TokenTypeStr:
            nExpIndex = nExpIndex + TokenList[nTokIndex].nLength;
            nEndIndex = m_sExpression.Find(TokenList[nTokIndex].szEndToken, nExpIndex);

            if (nEndIndex == Tools_DynamicUTF8String::NPos) {
              // unclosed multi character token found
              m_oError    = ExError_SYNTAX;
              m_nErrorPos = nExpIndex + 1;
              return false;
            } else {
              oArray[nArrIndex].nTokenType = TokenList[nTokIndex].nTokenType;
              oArray[nArrIndex].szToken    = m_sExpression.SubStr(nExpIndex, nEndIndex - nExpIndex);

              oArray[nArrIndex].nTokenPos  = nExpIndex + 1;
              ++nArrIndex;
              nExpIndex = nEndIndex + TokenList[nTokIndex].nEndLength;
            } // end if
            break;

          // built in function
          case TokenTypeLst:
          case TokenTypeAge:
            // add operator token
            oArray[nArrIndex].nTokenType = TokenList[nTokIndex].nTokenType;
            oArray[nArrIndex].szToken    = m_sExpression.SubStr(nExpIndex, TokenList[nTokIndex].nLength);
            oArray[nArrIndex].nTokenPos  = nExpIndex + 1;
            ++nArrIndex;

            // add identifier token
            nExpIndex = nExpIndex + TokenList[nTokIndex].nLength;
            nEndIndex = m_sExpression.Find(TokenList[nTokIndex].szEndToken, nExpIndex);
                
            if (nEndIndex == Tools_DynamicUTF8String::NPos) {
              // unclosed multi character token found
              m_oError    = ExError_SYNTAX;
              m_nErrorPos = nExpIndex + 1;
              return false;
            } else {
              // check for array id
              if ((m_sExpression[nEndIndex-2] == '[') &&
                  (m_sExpression[nEndIndex-1] == ']')    ) {
                ----nEndIndex;
                oArray[nArrIndex].nTokenType = TokenTypeArr;
                oArray[nArrIndex].szToken    = m_sExpression.SubStr(nExpIndex, nEndIndex - nExpIndex).Trim();
                oArray[nArrIndex].nTokenPos  = nExpIndex + 1;
                ++++nEndIndex;
              } else {
                oArray[nArrIndex].nTokenType = TokenTypeIdt;
                oArray[nArrIndex].szToken    = m_sExpression.SubStr(nExpIndex, nEndIndex - nExpIndex).Trim();
                oArray[nArrIndex].nTokenPos  = nExpIndex + 1;
              } // end if
              // add token
              ++nArrIndex;
              nExpIndex = nEndIndex + TokenList[nTokIndex].nEndLength;
            } // end if
            break;

          // array operator
          case TokenTypeArr:
            if (nArrIndex > 0) {
              if (oArray[nArrIndex - 1].nTokenType == TokenTypeIdt) {
                oArray[nArrIndex - 1].nTokenType = TokenTypeArr;
              } else {
                m_oError    = ExError_SYNTAX;
                m_nErrorPos = nExpIndex + 1;
              } // end if
            } else {
              m_oError    = ExError_SYNTAX;
              m_nErrorPos = nExpIndex + 1;
            } // end if
            nExpIndex = nExpIndex + TokenList[nTokIndex].nLength;
            break;

          // whitespace
          case TokenTypeSpc:
            // whitespace do nothing
            nExpIndex = nExpIndex + TokenList[nTokIndex].nLength;
            break;

          // minus
          case TokenTypeSub:
            oArray[nArrIndex].nTokenType = TokenList[nTokIndex].nTokenType;
            // check for unary minus
            if (nArrIndex > 0) {
              switch (oArray[nArrIndex - 1].nTokenType) {
                case TokenTypeAnd:     //  &&       And
                case TokenTypeOr :     //  ||       Or
                case TokenTypeLik:     //  like     Like 
                case TokenTypeNoe:     //  !=       Not equal
                case TokenTypeEqu:     //  ==       Equal
                case TokenTypeGre:     //  >=       Greather equal
                case TokenTypeGrt:     //  >        Greather than
                case TokenTypeLoe:     //  <=       Lower equal
                case TokenTypeLot:     //  <        Lower than
                case TokenTypeSub:     //  -        Substraction
                case TokenTypeAdd:     //  +        Addition
                case TokenTypeAlg:     //  #        Align Operator
                case TokenTypeMod:     //  %        Modulo
                case TokenTypeDiv:     //  /        Division
                case TokenTypeMul:     //  *        Multiplication
                case TokenTypePot:     //  ^        Potenz
                case TokenTypeNot:     //  !        Not
                case TokenTypeMin:     //  -        Unary Minus
                  oArray[nArrIndex].nTokenType = TokenTypeMin;
                  break;
              } // end switch
            } // end if
            oArray[nArrIndex].szToken    = m_sExpression.SubStr(nExpIndex, TokenList[nTokIndex].nLength);
            oArray[nArrIndex].nTokenPos  = nExpIndex + 1;
            ++nArrIndex;
            nExpIndex = nExpIndex + TokenList[nTokIndex].nLength;
            break;

          // normal operator
          default:
            oArray[nArrIndex].nTokenType = TokenList[nTokIndex].nTokenType;
            oArray[nArrIndex].szToken    = m_sExpression.SubStr(nExpIndex, TokenList[nTokIndex].nLength);
            oArray[nArrIndex].nTokenPos  = nExpIndex + 1;
            ++nArrIndex;
            nExpIndex = nExpIndex + TokenList[nTokIndex].nLength;
            break;
        } // end switch

        break;
      } // end if

      ++nTokIndex;
    } // end while

    if (TokenList[nTokIndex].szToken == NULL) {

      // check for number
      if ((m_sExpression[nExpIndex] >= '0') && (m_sExpression[nExpIndex] <= '9')) {

        // number found
        nEndIndex = nExpIndex + 1;
        bPoint = false;
        while ((nEndIndex < nExpLength) &&
               (((m_sExpression[nEndIndex] >= '0') && (m_sExpression[nEndIndex] <= '9')) ||
                ((m_sExpression[nEndIndex] == '.') && (!bPoint                        ))    ) ) {
          bPoint = (m_sExpression[nEndIndex] == '.') || bPoint;
          ++nEndIndex;
        } // end while
        oArray[nArrIndex].nTokenType = TokenTypeNum;
        oArray[nArrIndex].szToken    = m_sExpression.SubStr(nExpIndex, nEndIndex - nExpIndex);
        oArray[nArrIndex].nTokenPos  = nExpIndex + 1;
        ++nArrIndex;
        nExpIndex = nEndIndex;

      // check for identifier
      } else if (((m_sExpression[nExpIndex] >= 'a') && (m_sExpression[nExpIndex] <= 'z')) ||
                 ((m_sExpression[nExpIndex] >= 'A') && (m_sExpression[nExpIndex] <= 'Z')) ||
                 ((m_sExpression[nExpIndex] == '_')                                     )    ) {

        // identifier found
        nEndIndex = nExpIndex + 1;
        while ((nEndIndex < nExpLength) &&
               (((m_sExpression[nEndIndex] >= 'a') && (m_sExpression[nEndIndex] <= 'z')) ||
                ((m_sExpression[nEndIndex] >= 'A') && (m_sExpression[nEndIndex] <= 'Z')) ||
                ((m_sExpression[nEndIndex] >= '0') && (m_sExpression[nEndIndex] <= '9')) ||
                ((m_sExpression[nEndIndex] == '_')                                     )    ) ) {
          ++nEndIndex;
        } // end while
        oArray[nArrIndex].nTokenType = TokenTypeIdt;
        oArray[nArrIndex].szToken    = m_sExpression.SubStr(nExpIndex, nEndIndex - nExpIndex);
        oArray[nArrIndex].nTokenPos  = nExpIndex + 1;
        ++nArrIndex;
        nExpIndex = nEndIndex;

      } else {
        // nothing found
        m_oError    = ExError_SYNTAX;
        m_nErrorPos = nExpIndex + 1;
        return false;
      } // end if

    } // end if

  } // end while

  nArrSize = nArrIndex;

  return true;
} // end Tools_Expression::CreateTokens

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::CleanTree
  -----------------------------------------------------------------------------
 */
void Tools_Expression::CleanTree
      ( ExTree & pTree )
{
  if (pTree != NULL) {
    if (pTree->pLeft  != NULL) CleanTree(pTree->pLeft);
    if (pTree->pRight != NULL) CleanTree(pTree->pRight);

    if ( (pTree->pValue            != NULL        ) && 
         (pTree->oToken.nTokenType != TokenTypeIdt)    ) {
      delete pTree->pValue;
    } // end if
    delete pTree ;
  } // end if
  pTree = NULL;
} // end Tools_Expression::CleanTree

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::ParseLevel1
  -----------------------------------------------------------------------------
  description: &&  ||
  -----------------------------------------------------------------------------
 */
Tools_Expression::ExTree Tools_Expression::ParseLevel1
      ( const TokenArray & oArray, 
        SAPDB_UInt       & nArrSize,
        SAPDB_UInt       & nPosition )
{
  ExTree      pNode      = NULL;
  ExTree      pTree      = ParseLevel2(oArray, nArrSize, nPosition);

  while ((pTree     != NULL    ) &&
         (nPosition <  nArrSize) &&
         ((oArray[nPosition].nTokenType == TokenTypeAnd) ||
          (oArray[nPosition].nTokenType == TokenTypeOr )    ) ) {

    // create new node
    pNode = new ExNode(oArray[nPosition]);
    
    if (pNode == NULL) {
      this->m_oError = ExError_MEMORY;
      CleanTree(pTree);
    } else {
      pNode->pLeft = pTree;
      ++nPosition;
      pNode->pRight = ParseLevel2(oArray, nArrSize, nPosition);
      if (pNode->pRight == NULL) {
        CleanTree (pNode);
      } // end if

      pTree = pNode;
    } // end if

  } // end while

  return pTree;
} // end Tools_Expression::ParseLevel1

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::ParseLevel2
  -----------------------------------------------------------------------------
  description: !=  ==  >=  >   <=  <  like
  -----------------------------------------------------------------------------
 */
Tools_Expression::ExTree Tools_Expression::ParseLevel2
      ( const TokenArray & oArray, 
        SAPDB_UInt       & nArrSize,
        SAPDB_UInt       & nPosition )
{
  ExTree      pNode      = NULL;
  ExTree      pTree      = ParseLevel3(oArray, nArrSize, nPosition);

  while ((pTree     != NULL    ) &&
         (nPosition <  nArrSize) &&
         ((oArray[nPosition].nTokenType == TokenTypeLik) ||
          (oArray[nPosition].nTokenType == TokenTypeNoe) ||
          (oArray[nPosition].nTokenType == TokenTypeEqu) ||
          (oArray[nPosition].nTokenType == TokenTypeGre) ||
          (oArray[nPosition].nTokenType == TokenTypeGrt) ||
          (oArray[nPosition].nTokenType == TokenTypeLoe) ||
          (oArray[nPosition].nTokenType == TokenTypeLot)    ) ) {

    // create new node
    pNode = new ExNode(oArray[nPosition]);
    
    if (pNode == NULL) {
      m_oError = ExError_MEMORY;
      CleanTree(pTree);
    } else {
      pNode->pLeft = pTree;
      ++nPosition;
      pNode->pRight = ParseLevel3(oArray, nArrSize, nPosition);
      if (pNode->pRight == NULL) {
        CleanTree (pNode);
      } // end if

      pTree = pNode;
    } // end if

  } // end while

  return pTree;
} // end Tools_Expression::ParseLevel2

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::ParseLevel3
  -----------------------------------------------------------------------------
  description: -  +
  -----------------------------------------------------------------------------
 */
Tools_Expression::ExTree Tools_Expression::ParseLevel3
      ( const TokenArray & oArray, 
        SAPDB_UInt       & nArrSize,
        SAPDB_UInt       & nPosition )
{
  ExTree      pNode      = NULL;
  ExTree      pTree      = ParseLevel4(oArray, nArrSize, nPosition);

  while ((pTree     != NULL    ) &&
         (nPosition <  nArrSize) &&
         ((oArray[nPosition].nTokenType == TokenTypeAdd) ||
          (oArray[nPosition].nTokenType == TokenTypeSub)    ) ) {

    // create new node
    pNode = new ExNode(oArray[nPosition]);
    
    if (pNode == NULL) {
      m_oError = ExError_MEMORY;
      CleanTree(pTree);
    } else {
      pNode->pLeft = pTree;
      ++nPosition;
      pNode->pRight = ParseLevel4(oArray, nArrSize, nPosition);
      if (pNode->pRight == NULL) {
        CleanTree (pNode);
      } // end if

      pTree = pNode;
    } // end if

  } // end while

  return pTree;
} // end Tools_Expression::ParseLevel3

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::ParseLevel4
  -----------------------------------------------------------------------------
  description: ^ # % / * 
  -----------------------------------------------------------------------------
 */
Tools_Expression::ExTree Tools_Expression::ParseLevel4
      ( const TokenArray & oArray, 
        SAPDB_UInt       & nArrSize,
        SAPDB_UInt       & nPosition )
{
  ExTree      pNode      = NULL;
  ExTree      pTree      = ParseLevel5(oArray, nArrSize, nPosition);

  while ((pTree     != NULL    ) &&
         (nPosition <  nArrSize) &&
         ((oArray[nPosition].nTokenType == TokenTypeAlg) ||
          (oArray[nPosition].nTokenType == TokenTypeMod) ||
          (oArray[nPosition].nTokenType == TokenTypeDiv) ||
          (oArray[nPosition].nTokenType == TokenTypeMul) ||
          (oArray[nPosition].nTokenType == TokenTypePot)    ) ) {

    // create new node
    pNode = new ExNode(oArray[nPosition]);
    
    if (pNode == NULL) {
      m_oError = ExError_MEMORY;
      CleanTree(pTree);
    } else {
      pNode->pLeft = pTree;
      ++nPosition;
      pNode->pRight = ParseLevel5(oArray, nArrSize, nPosition);
      if (pNode->pRight == NULL) {
        CleanTree (pNode);
      } // end if

      pTree = pNode;
    } // end if

  } // end while

  return pTree;
} // end Tools_Expression::ParseLevel4

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::ParseLevel5
  -----------------------------------------------------------------------------
  description: !, - (unary minus), last(), age()
  -----------------------------------------------------------------------------
 */
Tools_Expression::ExTree Tools_Expression::ParseLevel5
      ( const TokenArray & oArray, 
        SAPDB_UInt       & nArrSize,
        SAPDB_UInt       & nPosition )
{
  ExTree      pNode      = NULL;

  if (nPosition <  nArrSize) {
    if ((oArray[nPosition].nTokenType == TokenTypeNot) ||
//        (oArray[nPosition].nTokenType == TokenTypeSub) ||
        (oArray[nPosition].nTokenType == TokenTypeMin) ||
        (oArray[nPosition].nTokenType == TokenTypeLst) ||
        (oArray[nPosition].nTokenType == TokenTypeAge)    ) {

      // correct token type
//      oArray[nPosition].nTokenType = (oArray[nPosition].nTokenType == TokenTypeSub) ? TokenTypeMin : oArray[nPosition].nTokenType;

      // create new node
      pNode = new ExNode(oArray[nPosition]);
      
      if (pNode == NULL) {
        m_oError = ExError_MEMORY;
        CleanTree(pNode);
      } else {
        pNode->pLeft = NULL; // no left operand
        ++nPosition;
        pNode->pRight = ParseLevel5(oArray, nArrSize, nPosition);
        if (pNode->pRight == NULL) {
          CleanTree (pNode);
        } // end if
      } // end if

    } else {
      pNode = ParseLevel6(oArray, nArrSize, nPosition);
    } // end if
  } else {
    m_oError    = ExError_SYNTAX;
    m_nErrorPos = oArray[nPosition - 1].nTokenPos;
  } // end if

  return pNode;
} // end Tools_Expression::ParseLevel5

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::ParseLevel6
  -----------------------------------------------------------------------------
  description: (  )  brackets
  -----------------------------------------------------------------------------
 */
Tools_Expression::ExTree Tools_Expression::ParseLevel6
      ( const TokenArray & oArray, 
        SAPDB_UInt       & nArrSize,
        SAPDB_UInt       & nPosition )
{
  ExTree     pNode   = NULL;
  SAPDB_UInt nOrgPos = nPosition;

  if (nPosition < nArrSize) {
    if (oArray[nPosition].nTokenType == TokenTypeOpb) {
      ++nPosition;
      pNode = ParseLevel1(oArray, nArrSize, nPosition);

      if ((pNode != NULL) && (nPosition < nArrSize)) {
        if (oArray[nPosition].nTokenType != TokenTypeClb) {
          CleanTree (pNode);
          m_oError    = ExError_BRACKET;
          m_nErrorPos = oArray[nPosition].nTokenPos;
        } else {
          ++nPosition;
        } // end if
      } else if (pNode != NULL) {
        CleanTree (pNode);
        m_oError    = ExError_SYNTAX;
        m_nErrorPos = oArray[nPosition].nTokenPos;
      } // end if
    } else {
      pNode = ParseLevel7(oArray, nArrSize, nPosition);
    }  // end if
  } else {
    m_oError    = ExError_SYNTAX;
    m_nErrorPos = oArray[nPosition - 1].nTokenPos;
  } // end if

  return pNode;
} // end Tools_Expression::ParseLevel6

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::ParseLevel7
  -----------------------------------------------------------------------------
  description: leaf nodes
  -----------------------------------------------------------------------------
 */
Tools_Expression::ExTree Tools_Expression::ParseLevel7
      ( const TokenArray & oArray, 
        SAPDB_UInt       & nArrSize,
        SAPDB_UInt       & nPosition )
{
  ExTree                       pNode  = NULL;
  Tools_ExpressionValue      * pValue = NULL;
  Tools_ExpressionValueArray * pArray = NULL;
  Tools_DynamicUTF8String      szToken;

  if (nPosition < nArrSize) {

    switch (oArray[nPosition].nTokenType) {
      case TokenTypeNum:
        szToken = oArray[nPosition].szToken;
        pValue = new Tools_ExpressionValue(atof(szToken.CharPtr()));
        break;
      case TokenTypeStr:
        pValue = new Tools_ExpressionValue(oArray[nPosition].szToken);
        break;
      case TokenTypeIdt:
        pValue = m_oProvider.AskForValue (oArray[nPosition].szToken);
        break;
      case TokenTypeArr:
        pArray = m_oProvider.AskForArray (oArray[nPosition].szToken);
        break;
      default:
        m_oError    = ExError_SYNTAX;
        m_nErrorPos = oArray[nPosition].nTokenPos;
        return NULL;
        break;
    } // end switch

    if (pValue != NULL) {
      pNode  = new ExNode(oArray[nPosition], pValue);
      ++nPosition;
    } else if (pArray != NULL) {
      pNode  = new ExNode(oArray[nPosition], pArray);
      ++nPosition;
    } else {
      // undefined variable found
      m_oError    = ExError_UNDEFVAR;
      m_nErrorPos = oArray[nPosition].nTokenPos;
    } // end if

  } else {
    m_oError    = ExError_SYNTAX;
    m_nErrorPos = oArray[nPosition - 1].nTokenPos;
  } // end if

  return pNode;
} // end Tools_Expression::ParseLevel7

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::CalculateExpression
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue * Tools_Expression::CalculateExpression 
   ( ExTree                                pTree,
     Tools_ExpressionValueArray::size_type nIndex)
{
  Tools_ExpressionValue   oValueLeft;
  Tools_ExpressionValue   oValueRight;
  Tools_ExpressionValue * pValueResult = NULL;

  if (pTree != NULL) {

    // compute values for operations
    switch (pTree->oToken.nTokenType) {

      // operators with left and right operand
      case TokenTypeAnd:     //  &&       And
      case TokenTypeOr :     //  ||       Or
      case TokenTypeLik:     //  like     Like     
      case TokenTypeNoe:     //  !=       Not equal
      case TokenTypeEqu:     //  ==       Equal
      case TokenTypeGre:     //  >=       Greather equal
      case TokenTypeGrt:     //  >        Greather than
      case TokenTypeLoe:     //  <=       Lower equal
      case TokenTypeLot:     //  <        Lower than
      case TokenTypeSub:     //  -        Substraction
      case TokenTypeAdd:     //  +        Addition
      case TokenTypeAlg:     //  #        Align Operator
      case TokenTypeMod:     //  %        Modulo
      case TokenTypeDiv:     //  /        Division
      case TokenTypeMul:     //  *        Multiplication
      case TokenTypePot:     //  ^        Potenz
        pValueResult = CalculateExpression(pTree->pLeft, nIndex);
        if (pValueResult != NULL) {
          oValueLeft.SetNewValue(pValueResult);
          pValueResult = CalculateExpression(pTree->pRight, nIndex);
          if (pValueResult != NULL) {
            oValueRight.SetNewValue(pValueResult);
            pValueResult = Calculate(pTree, oValueLeft, oValueRight);
          } // end if
        } // end if
        break;

      // operators with only right operand
      case TokenTypeNot:     //  !         Not
      case TokenTypeMin:     //  -         Unary Minus
      case TokenTypeLst:     //  last(...) Last Value 
      case TokenTypeAge:     //  age(...)  Age of Value
        pValueResult = CalculateExpression(pTree->pRight, nIndex);
        if (pValueResult != NULL) {
          oValueRight.SetNewValue(pValueResult);
          pValueResult = Calculate(pTree, oValueRight);
        } // end if
        break;

      // operands
      case TokenTypeStr:     //  "..."    String
      case TokenTypeIdt:     //  <char>   Identifier
      case TokenTypeNum:     //  <number> decimal number
        pValueResult = pTree->pValue;
        break;

      // operands
      case TokenTypeArr:     //  [...]    Array
        if (nIndex == -1) {
          m_oError    = ExError_INDEX;
          m_nErrorPos = pTree->oToken.nTokenPos;
        } else {
          pValueResult = &((*(pTree->pArray))[nIndex]);
        } // end if
        break;

      default:
        m_oError    = ExError_SYNTAX;
        m_nErrorPos = pTree->oToken.nTokenPos;
        break;
    } // end switch

  } else {
    m_oError    = ExError_SYNTAX;
    m_nErrorPos = pTree->oToken.nTokenPos;
  } // end if

//  if (pValueResult == 0) {
//    m_oError    = ExError_SYNTAX;
//    m_nErrorPos = pTree->oToken.nTokenPos;
  //} // end if

  return pValueResult;
} // end Tools_Expression::CalculateExpression

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::Calculate
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue * Tools_Expression::Calculate
      ( ExTree                  pTree, 
        Tools_ExpressionValue & oValueLeft, 
        Tools_ExpressionValue & oValueRight)
{
  Tools_ExpressionValue * pValueResult = NULL;
  m_oError = ExError_OK;

  if (pTree != NULL) {

    if (oValueLeft.IsDefined() && oValueRight.IsDefined()) {

      pValueResult = pTree->pValue;
      Tools_DynamicUTF8String szTmp;

      // compute values for operations
      switch (pTree->oToken.nTokenType) {

        // operators with left and right operand
        case TokenTypeAnd:     //  &&       And
          if ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)     ) {
            pValueResult->SetNewValue(((oValueLeft.GetNumericValue() != 0) && (oValueRight.GetNumericValue() != 0)) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;

        case TokenTypeOr :     //  ||       Or
          if ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)     ) {
            pValueResult->SetNewValue(((oValueLeft.GetNumericValue() != 0) || (oValueRight.GetNumericValue() != 0)) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;

        case TokenTypeLik:     //  like     Like
          if ((oValueLeft.GetType() == oValueRight.GetType()              ) &&
              (oValueLeft.GetType() == Tools_ExpressionValue::ExTypeString)    ) {
            pValueResult->SetNewValue((Like(oValueLeft.GetStringValue(), oValueRight.GetStringValue())) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;

        case TokenTypeNoe:     //  !=       Not equal
          if (oValueLeft.GetType() == oValueRight.GetType() ) {
            if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
              pValueResult->SetNewValue((oValueLeft.GetNumericValue() != oValueRight.GetNumericValue()) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeString) {
              pValueResult->SetNewValue((oValueLeft.GetStringValue() != oValueRight.GetStringValue()) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else {
              m_oError    = ExError_SYNTAX;
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;

        case TokenTypeEqu:     //  ==       Equal
          if (oValueLeft.GetType() == oValueRight.GetType() ) {
            if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
              pValueResult->SetNewValue((oValueLeft.GetNumericValue() == oValueRight.GetNumericValue()) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeString) {
              pValueResult->SetNewValue((oValueLeft.GetStringValue() == oValueRight.GetStringValue()) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else {
              m_oError    = ExError_SYNTAX;
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;
      
        case TokenTypeGre:     //  >=       Greather equal
          if (oValueLeft.GetType() == oValueRight.GetType() ) {
            if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
              pValueResult->SetNewValue((oValueLeft.GetNumericValue() >= oValueRight.GetNumericValue()) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeString) {
              pValueResult->SetNewValue(((oValueLeft.GetStringValue() == oValueRight.GetStringValue()) || 
                                         (oValueLeft.GetStringValue().Compare(oValueRight.GetStringValue()) > 0)) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else {
              m_oError    = ExError_SYNTAX;
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;
      
        case TokenTypeGrt:     //  >        Greather than
          if (oValueLeft.GetType() == oValueRight.GetType() ) {
            if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
              pValueResult->SetNewValue((oValueLeft.GetNumericValue() > oValueRight.GetNumericValue()) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeString) {
              pValueResult->SetNewValue((oValueLeft.GetStringValue().Compare(oValueRight.GetStringValue()) > 0) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else {
              m_oError    = ExError_SYNTAX;
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;
      
        case TokenTypeLoe:     //  <=       Lower equal
          if (oValueLeft.GetType() == oValueRight.GetType() ) {
            if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
              pValueResult->SetNewValue((oValueLeft.GetNumericValue() <= oValueRight.GetNumericValue()) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeString) {
              pValueResult->SetNewValue(((oValueLeft.GetStringValue() == oValueRight.GetStringValue()) || 
                                         (oValueLeft.GetStringValue().Compare(oValueRight.GetStringValue()) < 0)) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else {
              m_oError    = ExError_SYNTAX;
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;
      
        case TokenTypeLot:     //  <        Lower than
          if (oValueLeft.GetType() == oValueRight.GetType() ) {
            if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
              pValueResult->SetNewValue((oValueLeft.GetNumericValue() < oValueRight.GetNumericValue()) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeString) {
              pValueResult->SetNewValue((oValueLeft.GetStringValue().Compare(oValueRight.GetStringValue()) < 0) ? 1 : 0, Tools_ExpressionValue::ExTypeNumeric);
            } else {
              m_oError    = ExError_SYNTAX;
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;
      
        case TokenTypeSub:     //  -        Substraction
          if ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)     ) {
            pValueResult->SetNewValue(oValueLeft.GetNumericValue() - oValueRight.GetNumericValue(), Tools_ExpressionValue::ExTypeNumeric);
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;
      
        case TokenTypeAdd:     //  +        Addition
          if ((oValueLeft.GetType()  == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)    ) {
              pValueResult->SetNewValue(oValueLeft.GetNumericValue() + oValueRight.GetNumericValue(), Tools_ExpressionValue::ExTypeNumeric);
          } else if (((oValueLeft.GetType()  == Tools_ExpressionValue::ExTypeString ) ||
                      (oValueLeft.GetType()  == Tools_ExpressionValue::ExTypeNumeric)    ) &&
                     ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeString ) ||
                      (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)    )    ) {
            szTmp = oValueLeft.GetStringValue();
            szTmp.Append(oValueRight.GetStringValue());
            pValueResult->SetNewValue(szTmp, Tools_ExpressionValue::ExTypeString);
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;
      
        case TokenTypeAlg:     //  #        Align Operator
          if ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)     ) {
            if (oValueRight.GetIntegerValue() == 0) {
              m_oError    = ExError_DIV0;
            } else {
              if ((oValueLeft.GetIntegerValue() % oValueRight.GetIntegerValue()) > 0) {
                pValueResult->SetNewValue(oValueLeft.GetIntegerValue() + oValueRight.GetIntegerValue() 
                                          - (oValueLeft.GetIntegerValue() % oValueRight.GetIntegerValue()), Tools_ExpressionValue::ExTypeNumeric);
              } else {
                pValueResult->SetNewValue(oValueLeft.GetIntegerValue(), Tools_ExpressionValue::ExTypeNumeric);
              } // end if
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if */
          break;
      
        case TokenTypeMod:     //  %        Modulo
          if ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)     ) {
            if (oValueRight.GetIntegerValue() == 0) {
              m_oError    = ExError_DIV0;
            } else {
              pValueResult->SetNewValue(oValueLeft.GetIntegerValue() % oValueRight.GetIntegerValue(), Tools_ExpressionValue::ExTypeNumeric);
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if */
          break;
      
        case TokenTypeDiv:     //  /        Division
          if ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)     ) {
            if (oValueRight.GetNumericValue() == 0) {
              m_oError    = ExError_DIV0;
            } else {
              pValueResult->SetNewValue(oValueLeft.GetNumericValue() / oValueRight.GetNumericValue(), Tools_ExpressionValue::ExTypeNumeric);
            } // end if
          } else {
            m_oError    = ExError_TYPE;
          } // end if */
          break;
      
        case TokenTypeMul:     //  *        Multiplication
          if ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)     ) {
            pValueResult->SetNewValue(oValueLeft.GetNumericValue() * oValueRight.GetNumericValue(), Tools_ExpressionValue::ExTypeNumeric);
          } else {
            m_oError    = ExError_TYPE;
          } // end if */
          break;
      
        case TokenTypePot:     //  ^        Potenz
          if ((oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) &&
              (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric)     ) {
			  double powerResult = pow(oValueLeft.GetNumericValue(), oValueRight.GetNumericValue());
            pValueResult->SetNewValue(powerResult, Tools_ExpressionValue::ExTypeNumeric);
          } else {
            m_oError    = ExError_TYPE;
          } // end if */
          break;

        default:
          m_oError    = ExError_SYNTAX;
          m_nErrorPos = pTree->oToken.nTokenPos;
          break;
      } // end switch

    } else {
      m_oError    = ExError_UNDEFVAL;
    }// end if

  } else {
    m_oError    = ExError_SYNTAX;
  }// end if

  if (m_oError != ExError_OK) {
    pValueResult = NULL;
    m_nErrorPos = pTree->oToken.nTokenPos;
  } // end if

  return pValueResult;
} 
/*
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue * Tools_Expression::Calculate
      ( ExTree                  pTree, 
        Tools_ExpressionValue & oValueRight)
{
  Tools_ExpressionValue * pValueResult = NULL;

  m_oError = ExError_OK;

  if (pTree != NULL) {

    if (oValueRight.IsDefined()) {

      pValueResult = pTree->pValue;

      switch (pTree->oToken.nTokenType) {
        case TokenTypeNot:     //  !        Not
          if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
            pValueResult->SetNewValue((oValueRight.GetNumericValue() != 0) ? 0 : 1, Tools_ExpressionValue::ExTypeNumeric);
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;

        case TokenTypeMin:     //  -        Unary Minus
          if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
            pValueResult->SetNewValue(oValueRight.GetNumericValue() * -1, Tools_ExpressionValue::ExTypeNumeric);
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;

        case TokenTypeLst:     //  last(...)  Last Value 
          if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
            pValueResult->SetNewValue(oValueRight.GetLastNumericValue(), Tools_ExpressionValue::ExTypeNumeric);
          } else if (oValueRight.GetType() == Tools_ExpressionValue::ExTypeString) {
            pValueResult->SetNewValue(oValueRight.GetLastStringValue(), Tools_ExpressionValue::ExTypeString);
          } else {
            m_oError    = ExError_TYPE;
          } // end if
          break;

        case TokenTypeAge:     //  age(...)  Age of Value
          pValueResult->SetNewValue(oValueRight.GetAge(), Tools_ExpressionValue::ExTypeNumeric);
          break;

        default:
          m_oError    = ExError_SYNTAX;
          break;
      } // end switch

    } else {
      m_oError    = ExError_UNDEFVAL;
    }// end if

  } else {
    m_oError    = ExError_SYNTAX;
  }// end if

  if (m_oError != ExError_OK) {
    pValueResult = NULL;
    m_nErrorPos  = pTree->oToken.nTokenPos;
  } // end if

  return pValueResult;
} // end Tools_Expression::Calculate

/*
  -----------------------------------------------------------------------------
  private function Tools_Expression::Like
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_Expression::Like
      ( Tools_DynamicUTF8String szValue,
        Tools_DynamicUTF8String szPattern ) 
{
  SAPDB_Bool                                 bMatch      = false;
  Tools_DynamicUTF8String::BasisElementIndex nValueIndex = 0;
  Tools_DynamicUTF8String::BasisElementIndex nPatternLen = 0;

  if (szValue.Size() == 0 && szPattern.Size() == 0) {

    bMatch = true;
  
  } else if (szValue.Size() == 0 && szPattern.Size() > 0) {

    if (szPattern[0] == '*') {
        bMatch = Like(szValue, szPattern.SubStr(1)); 
    } // end if

  } else if (szPattern.Size() > 0) {

    switch (szPattern[0]) {
    case '?':
      bMatch = Like(szValue.SubStr(1), szPattern.SubStr(1)); 
      break;
    case '*':
      if (szPattern.Size() > 1) {
        for (nValueIndex = 0; (nValueIndex < szValue.Size()) && !bMatch; ++nValueIndex) {
          bMatch = Like(szValue.SubStr(nValueIndex), szPattern.SubStr(1)); 
        } // end for
      } else {
        bMatch = true;
      } // end if
      break;
    default:
      nPatternLen = szPattern.FindFirstOf("?*");
      if (nPatternLen == Tools_DynamicUTF8String::NPos) {
        bMatch = (szValue == szPattern);
      } else {
        bMatch = (szValue.SubStr(0, nPatternLen) == szPattern.SubStr(0, nPatternLen));
        if (bMatch) {
          bMatch = bMatch && Like(szValue.SubStr(nPatternLen), szPattern.SubStr(nPatternLen));
        } // end if
      } // end if
      break;
    } // end switch

  } // end if

  return bMatch;
} // end Tools_Expression::Like