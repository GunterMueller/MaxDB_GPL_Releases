/*!
  -----------------------------------------------------------------------------
  module: Tools_Expression.hpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: expression handling
  description:  some classes to handle expressions
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
#include <time.h>
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "ToolsCommon/Tools_Array.hpp"

#ifndef _Tools_Expression_HPP_
#define _Tools_Expression_HPP_

/*!
----------------------------------------------------------------------------
Class:        Tools_ExpressionValue
-----------------------------------------------------------------------------
description:  expression value object
-----------------------------------------------------------------------------
*/
class Tools_ExpressionValue {

public:

  /*! Chapter: Declarations */

  /*!
  -----------------------------------------------------------------------------
  declaration : ExDouble
  -----------------------------------------------------------------------------
  description:  data type for numeric values
  -----------------------------------------------------------------------------
  */
  typedef SAPDB_Real8             ExDouble;
  /*! Enddeclaration : ExDouble */

  /*!
  -----------------------------------------------------------------------------
  declaration : ExInt
  -----------------------------------------------------------------------------
  description:  data type for numeric values
  -----------------------------------------------------------------------------
  */
  typedef SAPDB_Int4             ExInt;
  /*! Enddeclaration : ExDouble */

  /*!
  -----------------------------------------------------------------------------
  declaration : ExString
  -----------------------------------------------------------------------------
  description:  data type for string values
  -----------------------------------------------------------------------------
  */
  typedef Tools_DynamicUTF8String ExString;
  /*! Enddeclaration : ExString */

  /*!
  -----------------------------------------------------------------------------
  declaration : ExType
  -----------------------------------------------------------------------------
  description:  data type specification
  -----------------------------------------------------------------------------
  */
  typedef enum {
    ExTypeUnknown,
    ExTypeNumeric,
    ExTypeString
  } ExType;
  /*! Enddeclaration : ExType */

  /*! EndChapter: Declarations */

  /*!
    -------------------------------------------------------------------------
    function:     Tools_ExpressionValue
    -------------------------------------------------------------------------
    description:  Constructor
    arguments:    oType      [IN] - value type
                  oInitValue [IN] - initial value (may define value type)
    -------------------------------------------------------------------------
   */
   Tools_ExpressionValue( );
   Tools_ExpressionValue(      ExType     oType);
   Tools_ExpressionValue(const ExDouble & oInitValue);
   Tools_ExpressionValue(const ExString & oInitValue);
   Tools_ExpressionValue(const ExDouble & oInitValue, ExType oType);
   Tools_ExpressionValue(const ExString & oInitValue, ExType oType);

   virtual ~Tools_ExpressionValue() {}

  /*!
    -------------------------------------------------------------------------
    function:     SetNewValue
    -------------------------------------------------------------------------
    description:  sets a new value
                  this becomes also the "active value"
    arguments:    oNewValue [IN] - new value
    -------------------------------------------------------------------------
   */
   void SetNewValue (const ExDouble & oNewValue);
   void SetNewValue (const ExString & oNewValue);
   void SetNewValue (const ExDouble & oNewValue, ExType oNewType);
   void SetNewValue (const ExString & oNewValue, ExType oNewType);

  /*!
    -------------------------------------------------------------------------
    function:     SetUndefined
    -------------------------------------------------------------------------
    description:  set a value to undefined
    -------------------------------------------------------------------------
   */
   void SetUndefined( ) { Init(m_oType, m_nPrecision); m_bUndef = true; }

  /*!
    -------------------------------------------------------------------------
    function:     SetPrecision
    -------------------------------------------------------------------------
    description:  set the precision for conversion to string
    -------------------------------------------------------------------------
   */
   void SetPrecision(ExInt nPrecision) { m_nPrecision = nPrecision; }

  /*!
    -------------------------------------------------------------------------
    function:     IsDefined
    -------------------------------------------------------------------------
    description:  is the value defined (valid)
    -------------------------------------------------------------------------
   */
   SAPDB_Bool IsDefined( ) {return !m_bUndef;}

  /*!
    -------------------------------------------------------------------------
    function:     GetStringValue
    -------------------------------------------------------------------------
    description:  reads the string value
    -------------------------------------------------------------------------
   */
   ExString GetStringValue ( ); 

  /*!
    -------------------------------------------------------------------------
    function:     GetNumericValue
    -------------------------------------------------------------------------
    description:  reads the numeric value
    -------------------------------------------------------------------------
   */
   ExDouble  GetNumericValue ();

  /*!
    -------------------------------------------------------------------------
    function:     GetIntegerValue
    -------------------------------------------------------------------------
    description:  reads the numeric value as Integer
    -------------------------------------------------------------------------
   */
   ExInt  GetIntegerValue () {return (ExInt) GetNumericValue();}

  /*!
    -------------------------------------------------------------------------
    function:     GetType
    -------------------------------------------------------------------------
    description:  reads the type
    -------------------------------------------------------------------------
   */
   ExType  GetType () {return m_oType;}

private:

   ExString GetLastStringValue  ( );
   ExDouble GetLastNumericValue ( );
   ExInt    GetLastIntegerValue ( ) {return (ExInt) GetLastNumericValue();}
   ExDouble GetAge              ( );
   ExString GetAgeString        ( );

   void SetNewValue (const Tools_ExpressionValue * pNewValue);

   void Init(ExType oType = ExTypeUnknown, ExInt m_nPrecision = -1);

   ExString ConvertToString(ExDouble nValue, ExInt nPrecision);

   ExType     m_oType;

   SAPDB_Bool m_bUndef;

   ExDouble   m_oNumericValue;
   ExString   m_oStringValue;

   ExDouble   m_oLastNumericValue;
   ExString   m_oLastStringValue;

   ExInt      m_nPrecision;
   static const ExInt UndefPrecision;

   time_t     m_tLastChanged;

   friend class Tools_Expression;

}; /*! EndClass: Tools_ExpressionValue */


/*!
----------------------------------------------------------------------------
Class:        Tools_ExpressionValueArray
-----------------------------------------------------------------------------
description:  value provide value array object
-----------------------------------------------------------------------------
*/
class Tools_ExpressionValueArrayElement : public Tools_ExpressionValue {
public:
  /*!
    -------------------------------------------------------------------------
    function:     SetKey
    -------------------------------------------------------------------------
    description:  sets the key value
    arguments:    sKey [IN] - the key value
    -------------------------------------------------------------------------
   */
  void SetKey(const ExString & sKey) {m_sKey = sKey;}

  /*!
    -------------------------------------------------------------------------
    function:     GetKey
    -------------------------------------------------------------------------
    description:  returns the key value
    return value: the key
    -------------------------------------------------------------------------
   */
  ExString GetKey() {return m_sKey;}

private:
  ExString m_sKey;

}; /*! EndClass: Tools_ExpressionValueArrayElement */

/*!
----------------------------------------------------------------------------
Class:        Tools_ExpressionValueArray
-----------------------------------------------------------------------------
description:  value provide value array object
-----------------------------------------------------------------------------
*/
typedef Tools_Array<Tools_ExpressionValueArrayElement> Tools_ExpressionValueArray;

/*! EndClass: Tools_ExpressionValueArray */

/*!
----------------------------------------------------------------------------
Class:        Tools_ValueProvider
-----------------------------------------------------------------------------
description:  value provide object
-----------------------------------------------------------------------------
*/
class Tools_ValueProvider {

  public:

  /*!
    -------------------------------------------------------------------------
    function:     AskForValue
    -------------------------------------------------------------------------
    description:  virtual function to get values
    arguments:    szName     [IN] - name of the value
    return value: pointer to the value object
    -------------------------------------------------------------------------
   */
   virtual Tools_ExpressionValue * AskForValue(const Tools_DynamicUTF8String & szName) = 0;

  /*!
    -------------------------------------------------------------------------
    function:     AskForArray
    -------------------------------------------------------------------------
    description:  virtual function to get arrays references
    arguments:    szName     [IN] - name of the array
    return value: pointer to the array object
    -------------------------------------------------------------------------
   */
   virtual Tools_ExpressionValueArray * AskForArray(const Tools_DynamicUTF8String & szName) = 0;

}; /*! EndClass: Tools_ValueProvider */

/*!
----------------------------------------------------------------------------
Class:        Tools_Expression
-----------------------------------------------------------------------------
description:  expression object
-----------------------------------------------------------------------------
*/
class Tools_Expression {
public:
  /*! Chapter: Declarations */

  /*!
  -----------------------------------------------------------------------------
  declaration : ExError
  -----------------------------------------------------------------------------
  description:  data type specification
  -----------------------------------------------------------------------------
  */
  typedef enum {
    ExError_OK,
    ExError_SYNTAX,    // syntax error
    ExError_DIV0,      // division by zero
    ExError_UNDEFVAR,  // undefined variable
    ExError_MEMORY,    // memory allocation problem
    ExError_TYPE,      // data type mismatch
    ExError_EMPTY,     // expression is empty
    ExError_BRACKET,   // unclosed bracket
    ExError_UNDEFVAL,  // undefined value
    ExError_PARSE,     // not parsed
    ExError_INDEX      // invalid index
  } ExError;
  /*! Enddeclaration : ExError */

  /*! EndChapter: Declarations */

  /*!
    -------------------------------------------------------------------------
    function:     Tools_Expression
    -------------------------------------------------------------------------
    description:  Constructor
    arguments:    oProvider   [IN] - a Value provider
                  sExpression [IN] - the expression
    -------------------------------------------------------------------------
   */
  Tools_Expression ( Tools_ValueProvider           & oProvider);
  Tools_Expression ( Tools_ValueProvider           & oProvider,
                     const Tools_DynamicUTF8String & sExpression );

  /*!
    -------------------------------------------------------------------------
    function:     ~Tools_Expression
    -------------------------------------------------------------------------
    description:  Destructor
    -------------------------------------------------------------------------
   */
  ~Tools_Expression ();


  /*!
    -------------------------------------------------------------------------
    function:     SetExpression
    -------------------------------------------------------------------------
    description:  sets the expression
    arguments:    sExpression [IN] - the new expression
    -------------------------------------------------------------------------
   */
  void SetExpression ( const Tools_DynamicUTF8String & sExpression );

  /*!
    -------------------------------------------------------------------------
    function:     GetExpression
    -------------------------------------------------------------------------
    description:  returns the current expression
    return value: the expression
    -------------------------------------------------------------------------
   */
  const Tools_DynamicUTF8String & GetExpression ( );


  /*!
    -------------------------------------------------------------------------
    function:     ParseExpression
    -------------------------------------------------------------------------
    description:  parses the expression
    arguments:    sExpression [IN] - the new expression
    return value: success or not
    -------------------------------------------------------------------------
   */
  SAPDB_Bool ParseExpression ( );
  SAPDB_Bool ParseExpression ( const Tools_DynamicUTF8String & sExpression );

  /*!
    -------------------------------------------------------------------------
    function:     Value
    -------------------------------------------------------------------------
    description:  computes the expression
    arguments:    oValue [OUT] - value object for the value
                  nIndex [IN]  - index for array elements
    return value: success or not
    -------------------------------------------------------------------------
   */
  SAPDB_Bool Value ( Tools_ExpressionValue & oValue, Tools_ExpressionValueArray::size_type nIndex = -1);

  /*!
    -------------------------------------------------------------------------
    function:     ExplainValue
    -------------------------------------------------------------------------
    description:  explains the value
                  
                  provides the original expression with names of variables
                  and the expression where the names are replaced through
                  values
    arguments:    sOrgExpression   [OUT] - original expression
                  sValueExpression [OUT] - expression with values
                  nIndex [IN]  - index for array elements
    -------------------------------------------------------------------------
   */
  void ExplainValue ( Tools_DynamicUTF8String & sOrgExpression, Tools_DynamicUTF8String & sValueExpression, Tools_ExpressionValueArray::size_type nIndex = -1);

  /*!
    -------------------------------------------------------------------------
    function:     LastError
    -------------------------------------------------------------------------
    description:  returns the last error code
    return value: the error
    -------------------------------------------------------------------------
   */
  ExError LastError ( ) { return m_oError;};

  /*!
    -------------------------------------------------------------------------
    function:     LastErrorPos
    -------------------------------------------------------------------------
    description:  returns the last error position
    return value: the error position
    -------------------------------------------------------------------------
   */
  Tools_DynamicUTF8String::BasisElementIndex LastErrorPos () { return m_nErrorPos;}
  
  /*!
    -------------------------------------------------------------------------
    function:     LastErrorText
    -------------------------------------------------------------------------
    description:  returns the last error text
    return value: the error
    -------------------------------------------------------------------------
   */
  Tools_DynamicUTF8String LastErrorText ( );

  /*
    -------------------------------------------------------------------------
    decalration:     TokenType
    -------------------------------------------------------------------------
   */
  typedef enum {
    // whitespace
    TokenTypeSpc,     //           Whitespace

    // operators
    TokenTypeAnd,     //  &&       And
    TokenTypeOr ,     //  ||       Or

    TokenTypeLik,     //  like     Like 
    TokenTypeNoe,     //  !=       Not equal
    TokenTypeEqu,     //  ==       Equal
    TokenTypeGre,     //  >=       Greather equal
    TokenTypeGrt,     //  >        Greather than
    TokenTypeLoe,     //  <=       Lower equal
    TokenTypeLot,     //  <        Lower than

    TokenTypeSub,     //  -        Substraction
    TokenTypeAdd,     //  +        Addition

    TokenTypeAlg,     //  #        Align Operator
    TokenTypeMod,     //  %        Modulo
    TokenTypeDiv,     //  /        Division
    TokenTypeMul,     //  *        Multiplication
    TokenTypePot,     //  ^        Power

    TokenTypeNot,     //  !        Not

    TokenTypeMin,     //  -        Unary Minus

    TokenTypeArr,     //  []       Array-Element

    // brackets
    TokenTypeOpb,     //  (        Open bracket
    TokenTypeClb,     //  )        Close bracket

    // functional operands
    TokenTypeAge,     //  age()    Age of value function
    TokenTypeLst,     //  last()   Last value function
    
    // Tokens with variable length
    TokenTypeStr,     //  "..."    String

    TokenTypeUnk,     //  unknown

    // tokens found by precheck
    TokenTypeIdt,     //  <char>   Identifier
    TokenTypeNum      //  <number> decimal number

  } TokenType;

  /*
    -------------------------------------------------------------------------
    class:     ExToken
    -------------------------------------------------------------------------
   */
  class ExToken {
    public:
      ExToken()
        : szToken(""),
          nTokenType(TokenTypeUnk),
          nTokenPos(0)
      { }

    private:
      Tools_DynamicUTF8String                    szToken;
      TokenType                                  nTokenType;
      Tools_DynamicUTF8String::BasisElementIndex nTokenPos;

      friend class Tools_Expression;
  }; /* EndClass: ExToken */

  /*
    -------------------------------------------------------------------------
    class:     ExNode
    -------------------------------------------------------------------------
   */
   class ExNode {
      ExNode(const ExToken & oInToken, Tools_ExpressionValue * pInValue)
        : oToken(oInToken),
          pValue(pInValue),
          pArray(NULL),
          pLeft(NULL),
          pRight(NULL)
      { }

      ExNode(const ExToken & oInToken, Tools_ExpressionValueArray * oArray)
        : oToken(oInToken),
          pValue(new Tools_ExpressionValue),
          pArray(oArray),
          pLeft(NULL),
          pRight(NULL)
      { }

      ExNode(const ExToken & oInToken)
        : oToken(oInToken),
          pValue(new Tools_ExpressionValue),
          pArray(NULL),
          pLeft(NULL),
          pRight(NULL)
      { }

      ExNode                     * pLeft;
      ExNode                     * pRight;
      const ExToken              & oToken;
      Tools_ExpressionValue      * pValue;
      Tools_ExpressionValueArray * pArray;

      friend class Tools_Expression;
   };  /* EndClass: ExNode */

   typedef ExNode * ExTree;

private:
  // private deklarations
  typedef struct  {
    Tools_DynamicUTF8String::ASCIIConstPointer szToken;
    Tools_DynamicUTF8String::ElementCount      nLength;
    Tools_DynamicUTF8String::ASCIIConstPointer szEndToken;
    Tools_DynamicUTF8String::ElementCount      nEndLength;
    TokenType                                  nTokenType;
  } ExTokens;

  typedef Tools_Array<ExToken> TokenArray;
  SAPDB_Bool CreateTokens(TokenArray & oArray, SAPDB_UInt & nArrSize);

  // private member variables
  Tools_ValueProvider                      & m_oProvider;
  Tools_DynamicUTF8String                    m_sExpression;

  ExError                                    m_oError;
  Tools_DynamicUTF8String::BasisElementIndex m_nErrorPos;
  Tools_DynamicUTF8String                    m_szError;

  ExTree                                     m_pTree;
  const TokenArray &                         m_oArray;
  TokenArray                                 x_oArray;

  // private member functions
  ExTree ParseLevel1(const TokenArray & oArray, SAPDB_UInt & nArrSize, SAPDB_UInt & nPosition );
  ExTree ParseLevel2(const TokenArray & oArray, SAPDB_UInt & nArrSize, SAPDB_UInt & nPosition );
  ExTree ParseLevel3(const TokenArray & oArray, SAPDB_UInt & nArrSize, SAPDB_UInt & nPosition );
  ExTree ParseLevel4(const TokenArray & oArray, SAPDB_UInt & nArrSize, SAPDB_UInt & nPosition );
  ExTree ParseLevel5(const TokenArray & oArray, SAPDB_UInt & nArrSize, SAPDB_UInt & nPosition );
  ExTree ParseLevel6(const TokenArray & oArray, SAPDB_UInt & nArrSize, SAPDB_UInt & nPosition );
  ExTree ParseLevel7(const TokenArray & oArray, SAPDB_UInt & nArrSize, SAPDB_UInt & nPosition );

  void CleanTree (ExTree & pTree);

  Tools_ExpressionValue * CalculateExpression (ExTree pTree,  Tools_ExpressionValueArray::size_type nIndex);
  Tools_ExpressionValue * Calculate           (ExTree pTree, Tools_ExpressionValue & oValueLeft, Tools_ExpressionValue & oValueRight);
  Tools_ExpressionValue * Calculate           (ExTree pTree, Tools_ExpressionValue & oValueRight);

  SAPDB_Bool Like ( Tools_DynamicUTF8String szValue, Tools_DynamicUTF8String szPattern );

}; /*! EndClass: Tools_Expression */

#endif // _Tools_Expression_HPP_
