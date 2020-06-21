/*!*****************************************************************************

  module      : X2DC_Cpp_Type_Registry.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of persistent class description code
  description : Type registry
				
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


//#include <ctype.h>
//#if defined(_WIN32) && (_MSC_VER >= 1300)
//#include <fstream>
//#define endl std::endl
//#else
//#include <fstream.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <iostream.h>
//#endif


#include "idl/XML2DiscCpp/X2DC_Cpp_Type_Registry.hpp"


/******************************************************************************/
/*     Implementation of class Cpp_Type_Registry                              */
/******************************************************************************/

Cpp_Type_Registry::Cpp_Type_Registry(int * pError)
: m_pError(pError)
, m_pTempinList(NULL)
, m_isWithin_LCTypes(false)
, m_isWithin_Enum(false)
, m_isWithin_GenEnum(false)
, m_isWithin_Struct(false)
, m_isWithin_Union(false)
, m_isWithin_Class(false)
, m_isWithin_TemplClass(false)
, m_isWithin_Instantiation(false)
, m_isWithin_PersInstantiation(false)
, m_isWithin_TemplParams(false)
, m_isWithin_PersClass(false)
, m_isWithin_PersKeyedClass(false)
, m_isWithin_PersInterface(false)
, m_isWithin_PersTemplClass(false)
, m_isWithin_PersTemplClassSize(false)
, m_pCurrPtsExpr(NULL)
, m_pPtsOper(NULL)
, m_isWithin_Index(false)
, m_isWithin_TemplMember(false)
, m_isWithin_OidMember(false)
, m_is_FullType(false)
, m_pType(NULL)
, m_pTempl(NULL)
, m_pMember(NULL)
, m_pPersClass(NULL)
, m_pcSize(0)
, m_pPersTempl(NULL)
, m_pID(NULL)
, m_pIndex(NULL)
, m_pConst(NULL)
, m_pConstExpr(NULL)
, m_pBitFields(NULL)
, m_genTypeName_Count(0)
, m_TemplParam_Count(0)
, m_pTabHandle(NULL)
{
  m_pTypeList          = new List<Cpp_Type>();
  m_pPersInterfaceList = new List<Pers_Interface>();
  m_pPersClassList     = new List<Pers_Class>();
  m_pConstList         = new List<Cpp_Const>();
  CTR_pCurrType        = &m_pType;
}

/*----------------------------------------------------------------------------*/

Cpp_Type_Registry::~Cpp_Type_Registry() {

  if (!*m_pError) {

    m_pTypeList->Clear();
    delete m_pTypeList;
    m_pTypeList = NULL;

    m_pPersInterfaceList->Clear();
    delete m_pPersInterfaceList;
    m_pPersInterfaceList = NULL;

    m_pPersClassList->Clear();
    delete m_pPersClassList;
    m_pPersClassList = NULL;

    m_pConstList->Clear();
    delete m_pConstList;
    m_pConstList = NULL;

    if ( m_pTempinList  ||
         m_pCurrPtsExpr ||
         m_pPtsOper     ||
         m_pType        ||
         m_pTempl       ||
         m_pMember      ||
         m_pPersClass   ||
         m_pPersTempl   ||
         m_pID          ||
         m_pIndex       ||
         m_pConst       ||
         m_pConstExpr   ||
         m_pBitFields   ||
         m_pTabHandle ) {
      X2DC_WarnMsg( "C++ type registry not cleaned up correctly (pointers)!", false );
    }

    if ( m_isWithin_LCTypes            ||
         m_isWithin_Enum               ||
         m_isWithin_Struct             ||
         m_isWithin_Union              ||
         m_isWithin_Class              ||
         m_isWithin_TemplClass         ||
         m_isWithin_Instantiation      ||
         m_isWithin_TemplParams        ||
         m_isWithin_PersClass          ||
         m_isWithin_PersKeyedClass     ||
         m_isWithin_PersInterface      ||
         m_isWithin_PersTemplClass     ||
         m_isWithin_PersTemplClassSize ||
         m_isWithin_PersInstantiation  ||
         m_isWithin_Index              ||
         m_isWithin_TemplMember        ||
         m_isWithin_OidMember          ||
         m_is_FullType ) {
      X2DC_WarnMsg( "C++ type registry not cleaned up correctly (bools)!", false );
    }

  }

}

/*----------------------------------------------------------------------------*/

int Cpp_Type_Registry::Init() {

  int & error = *m_pError;

  /* Register types from livecachetypes.xml */

  Cpp_Type * pT = NULL;
  Member   * pM = NULL;

  // signed 1-byte integer
  pT = new Cpp_Type("OmsTypeInt1");
  pT->setSize(1);
  pT->setAlignment(1);
  pT->setDefault("0");
  pT->setVtType(csp1_vt_i1);
  registerFullType( error, pT );

  // unsigned 1-byte integer
  pT = new Cpp_Type("OmsTypeUInt1");
  pT->setSize(1);
  pT->setAlignment(1);
  pT->setDefault("0");
  pT->setVtType(csp1_vt_ui1);
  registerFullType( error, pT );

  // signed 2-byte integer
  pT = new Cpp_Type("OmsTypeInt2");
  pT->setSize(2);
  pT->setAlignment(2);
  pT->setDefault("0");
  pT->setVtType(csp1_vt_i2);
  registerFullType( error, pT );

  // unsigned 2-byte integer
  pT = new Cpp_Type("OmsTypeUInt2");
  pT->setSize(2);
  pT->setAlignment(2);
  pT->setDefault("0");
  pT->setVtType(csp1_vt_ui2);
  registerFullType( error, pT );

  // signed 4-byte integer
  pT = new Cpp_Type("OmsTypeInt4");
  pT->setSize(4);
  pT->setAlignment(4);
  pT->setDefault("0");
  pT->setVtType(csp1_vt_i4);
  registerFullType( error, pT );
  pT->addSynonym( error, "ClassID" );

  // unsigned 4-byte integer
  pT = new Cpp_Type("OmsTypeUInt4");
  pT->setSize(4);
  pT->setAlignment(4);
  pT->setDefault("0");
  pT->setVtType(csp1_vt_ui4);
  registerFullType( error, pT );

  // signed 8-byte integer
  pT = new Cpp_Type("OmsTypeInt8");
  pT->setSize(8);
  pT->setAlignment(8);
  pT->setDefault("0");
  pT->setVtType(csp1_vt_i8);
  registerFullType( error, pT );

  // unsigned 8-byte integer
  pT = new Cpp_Type("OmsTypeUInt8");
  pT->setSize(8);
  pT->setAlignment(8);
  pT->setDefault("0");
  pT->setVtType(csp1_vt_ui8);
  registerFullType( error, pT );

  // character
  pT = new Cpp_Type("OmsTypeChar");
  pT->setSize(1);
  pT->setAlignment(1);
  pT->setDefault("' '");
  pT->setVtType(csp1_vt_lc_char);
  registerFullType( error, pT );

  // 7-Bit-Ascii character
  pT = new Cpp_Type("OmsTypeAscii7");
  pT->setSize(1);
  pT->setAlignment(1);
  pT->setDefault("' '");
  pT->setVtType(csp1_vt_lc_ascii7);
  registerFullType( error, pT );

  // UTF8 character
  pT = new Cpp_Type("OmsTypeUTF8");
  pT->setSize(1);
  pT->setAlignment(1);
  pT->setDefault("' '");
  pT->setVtType(csp1_vt_lc_utf8);
  registerFullType( error, pT );

  // UCS2 character
  pT = new Cpp_Type("OmsTypeWyde");
  pT->setSize(2);
  pT->setAlignment(2);
  pT->setDefault("' '");
  pT->setVtType(csp1_vt_lc_wyde);
  registerFullType( error, pT );

  // numerical character
  pT = new Cpp_Type("OmsTypeNumc");
  pT->setSize(1);
  pT->setAlignment(1);
  pT->setDefault("'0'");
  pT->setVtType(csp1_vt_lc_numc);
  registerFullType( error, pT );

  // byte
  pT = new Cpp_Type("OmsTypeByte");
  pT->setSize(1);
  pT->setAlignment(1);
  pT->setDefault("' '");
  pT->setVtType(csp1_vt_lc_byte);
  registerFullType( error, pT );

  // bool
  pT = new Cpp_Type("OmsTypeBool");
  pT->setSize(1);
  pT->setAlignment(1);
  pT->setDefault("' '");
  pT->setVtType(csp1_vt_lc_bool);
  registerFullType( error, pT );

  // double
  pT = new Cpp_Type("double");
  pT->setSize(8);
  pT->setAlignment(8);
  pT->setVtType(csp1_vt_r8);
  registerFullType( error, pT );
  pT = new Cpp_Type("OmsTypeDouble");
  pT->setVtType(csp1_vt_userdefined);
  pM = new Member("d");
  pM->setType( error, CppType, findType("double") );
  pT->addMember( error, pM );
  registerFullType( error, pT );
  setHidden( error, findType("double") );

  // container number
  pT = new Cpp_Type("OmsContainerNo");
  pT->setSize(4);
  pT->setAlignment(4);
  pT->setDefault("'0'");
  pT->setVtType(csp1_vt_ui4);
  registerFullType( error, pT );

  // OID
  if (AlignmentOfOid() == 8) {
    pT = new Cpp_Type("OmsTypeOid4BA");
  }
  else { // 4
    pT = new Cpp_Type("OmsTypeOid");
  }
  pT->setVtType(csp1_vt_lc_oid);
  pM = new Member("pno");
  pM->setType( error, CppType, findType("OmsTypeUInt4") );
  pT->addMember( error, pM );
  pM = new Member("pagePos");
  pM->setType( error, CppType, findType("OmsTypeUInt2") );
  pT->addMember( error, pM );
  pM = new Member("generation");
  pM->setType( error, CppType, findType("OmsTypeUInt2") );
  pT->addMember( error, pM );
  registerFullType( error, pT );
  if (AlignmentOfOid() == 8) {
    pT = new Cpp_Type("OmsTypeOid");
    pT->setVtType(csp1_vt_userdefined);
    pM = new Member("OidAsStruct");
    pM->setType( error, CppType, findType("OmsTypeOid4BA") );
    pT->addMember( error, pM );
    pT->setAlignment(8);
    registerFullType( error, pT );
  }

  // types registered only with name
  registerOnlyName( error, "OmsTypeABAPTimeStamp" );
  registerOnlyName( error, "OmsTypePacked_8_3" );
  registerOnlyName( error, "OmsTypePacked_15_3" );
  registerOnlyName( error, "OmsSchemaHandle" );
  registerOnlyName( error, "TParamDescriptor" );
  registerOnlyName( error, "OmsVersionId" );
  registerOnlyName( error, "OmsLockHandle" );
  registerOnlyName( error, "OmsMonitorData" );
  registerOnlyName( error, "SqlMonitorData" );
  registerOnlyName( error, "OmsVersionInfo" );
  registerOnlyName( error, "OmsTypeABAPColDesc" );
  registerOnlyName( error, "OmsTypeABAPTabHandle" );
  registerOnlyName( error, "OmsTypeStreamMemberDesc" );
  registerOnlyName( error, "OmsTypeStreamHandle" );

  /* Check error */

  if (error) {
    X2DC_ErrMsg( "C++ type registry initialization failed", false );
  }

  /* Done */

  return 0;

}

/*----------------------------------------------------------------------------*/

void Cpp_Type_Registry::StartElement( int & error, const char * name, const char ** atts ) {

  /* Types from livecachetypes.xml are registered in Init-method. */

  if ( (elemImport_X2DC == X2DC_getElementType(name)) &&
       (0 == strcmp( atts[1], "livecachetypes.xml" )) ) {
    m_isWithin_LCTypes = true;
    return;
  }
  else if (m_isWithin_LCTypes) {
    return;
  }

  /* Register types */

  switch (X2DC_getElementType(name)) {

    /*---------- STRUCT ------------------*/

    case elemStruct_X2DC : {
      m_isWithin_Struct = true;
      m_pType = new Cpp_Type(atts[1]);
      m_pType->setVtType(csp1_vt_userdefined);
      m_is_FullType = true;
      for ( int i = 0; atts[i]; i = i+2 ) {
        if ( (attGentabhandle_X2DC == X2DC_getAttributeType(atts[i])) &&
             (0 == strcmp( atts[i+1], "YES" )) ) {
          m_pTabHandle = new char[strlen(atts[1])+strlen("TabHandle")+1];
          strcpy( m_pTabHandle, atts[1] );
          strcat( m_pTabHandle, "TabHandle" );
          break;
        }
      }
      break;
    }

    /*---------- UNION -------------------*/

    case elemUnion_X2DC : {
      m_isWithin_Union = true;
      m_pType = new Cpp_Type(atts[1]);
      m_pType->setVtType(csp1_vt_userdefined);
      m_is_FullType = true;
      break;
    }

    /*---------- CLASS -------------------*/

    case elemClass_X2DC : {
      m_isWithin_Class = true;
      m_pType = new Cpp_Type(atts[1]);
      m_pType->setVtType(csp1_vt_userdefined);
      break;
    }

    /*---------- PERSCLASS ---------------*/

    case elemPersclass_X2DC : {
      m_isWithin_PersClass = true;
      m_pType = new Cpp_Type(atts[1]);
      m_pType->setVtType(csp1_vt_userdefined);
      m_pType->setSize(SizeOfPointer());
      m_pType->setAlignment(AlignmentOfPointer());
      int id = atoi(atts[3]);
      if (0 == id) {
        if (0 != strcmp( atts[3], "0" )) {
          const int * pId = getConstValue(atts[3]);
          if (pId) {
            id = *pId;
          }
          else {
            X2DC_ErrMsg( M__BAD_PERSCLASS_ID, atts[1] );
            error = 1;
            return;
          }
        }
      }
      m_pPersClass = new Pers_Class( error, id, m_pType );
      int pbcIdx = 0;
      for ( int i = 4; atts[i]; i = i+2 ) {
        if (attPersbaseclass_X2DC == X2DC_getAttributeType(atts[i])) {
          pbcIdx = i;
          break;
        }
      }
      if (pbcIdx) {
        const Pers_Class * pPC = findPersClass(atts[pbcIdx+1]);
        if (pPC) {
          if (pPC->isTemplate()) {
            X2DC_ErrMsg( M__PERSBASECLASS_IS_TEMPLATE, atts[pbcIdx+1], atts[1] );
            error = 1;
            return;
          }
          else {
            Base_Class * pBC = new Base_Class(pPC->getClass());
            m_pType->addBaseClass( error, pBC, true );
          }
        }
        else if (!findPersInterface(atts[pbcIdx+1])) {
          X2DC_ErrMsg( M__PERSBASECLASS_NOT_FOUND, atts[pbcIdx+1], atts[1] );
          error = 1;
          return;
        }
      }
      break;
    }

    case elemSize_X2DC : {
      if ( m_isWithin_PersClass || m_isWithin_PersKeyedClass ) {
        m_pcSize = atoi(atts[1]);
        if (0 == m_pcSize) {
          X2DC_ErrMsg( M__BAD_PERSCLASS_SIZE, m_pType->getName() );
          error = 1;
          return;
        }
      }
      else if (m_isWithin_PersTemplClass) {
        m_isWithin_PersTemplClassSize = true;
        m_pCurrPtsExpr = new PTS_Expression();
        m_pPersClass->setPtsExpression( error, m_pCurrPtsExpr );
      }
      break;
    }

    case elemKey_X2DC : {
      m_isWithin_Index = true;
      m_pIndex = new Index(0);
      break;
    }

    case elemIndex_X2DC : {
      m_isWithin_Index = true;
      m_pIndex = new Index(atoi(atts[1]));
      break;
    }

    /*---------- PERSKEYEDCLASS ----------*/

    case elemPerskeyedclass_X2DC : {
      m_isWithin_PersKeyedClass = true;
      m_pType = new Cpp_Type(atts[1]);
      m_pType->setVtType(csp1_vt_userdefined);
      m_pType->setSize(SizeOfPointer());
      m_pType->setAlignment(AlignmentOfPointer());
      int id = atoi(atts[3]);
      if (0 == id) {
        if (0 != strcmp( atts[3], "0" )) {
          const int * pId = getConstValue(atts[3]);
          if (pId) {
            id = *pId;
          }
          else {
            X2DC_ErrMsg( M__BAD_PERSCLASS_ID, atts[1] );
            error = 1;
            return;
          }
        }
      }
      m_pPersClass = new Pers_Class( error, id, m_pType );
      int keyIdx = 0;
      for ( int i = 4; atts[i]; i = i+2 ) {
        if (attKeyclass_X2DC == X2DC_getAttributeType(atts[i])) {
          keyIdx = i;
          break;
        }
      }
      if ( keyIdx && (!findType(atts[keyIdx+1])) ) {
        X2DC_ErrMsg( M__TYPE_NOT_FOUND, atts[keyIdx+1] );
        error = 1;
        return;
      }
      break;
    }

    /*---------- TEMPLCLASS --------------*/

    case elemTemplclass_X2DC : {
      m_isWithin_TemplClass = true;
      m_pType = new Cpp_Type( atts[1], true, true );
      m_pType->setVtType(csp1_vt_userdefined);
      break;
    }

    case elemTemplparams_X2DC : {
      m_isWithin_TemplParams = true;
      m_TemplParam_Count = 0;
      break;
    }

    case elemClassparam_X2DC : {
      if (m_isWithin_TemplParams) {
        m_TemplParam_Count++;
        Templ_Param * pTempar = new Templ_Param( atts[1], ClassParam );
        m_pType->addTemplParam( error, pTempar );
      }
      break;
    }

    case elemIntparam_X2DC : {
      if (m_isWithin_TemplParams) {
        m_TemplParam_Count++;
        const Cpp_Type * pT = findType(atts[1]);
        if ( (!pT) || (0 != strcmp( pT->getName(), "OmsTypeInt4" )) ) {
          X2DC_ErrMsg( M__BAD_INT_SYNONYM, atts[1] );
          error = 1;
          return;
        }
        else {
          Templ_Param * pTempar = new Templ_Param( atts[3], IntParam );
          m_pType->addTemplParam( error, pTempar );
        }
      }
      break;
    }

    /*---------- PERSTEMPLCLASS ----------*/

    case elemPerstemplclass_X2DC : {
      m_isWithin_PersTemplClass = true;
      m_pType = new Cpp_Type( atts[1], true, true );
      m_pType->setVtType(csp1_vt_userdefined);
      m_pID = new char[strlen(atts[3])+1];
      strcpy( m_pID, atts[3] );
      m_pPersClass = new Pers_Class( error, m_pType );
      int pbcIdx = 0;
      for ( int i = 4; atts[i]; i = i+2 ) {
        if (X2DC_getAttributeType(atts[i]) == attPersbaseclass_X2DC) {
          pbcIdx = i;
          break;
        }
      }
      if (pbcIdx) {
        const Pers_Class * pPC = findPersClass(atts[pbcIdx+1]);
        if (pPC) {
          if (pPC->isTemplate()) {
            X2DC_ErrMsg( M__PERSBASECLASS_IS_TEMPLATE, atts[pbcIdx+1], atts[1] );
            error = 1;
            return;
          }
          else {
            Base_Class * pBC = new Base_Class(pPC->getClass());
            m_pType->addBaseClass( error, pBC, true );
          }
        }
        else if (!findPersInterface(atts[pbcIdx+1])) {
          X2DC_ErrMsg( M__PERSBASECLASS_NOT_FOUND, atts[pbcIdx+1], atts[1] );
          error = 1;
          return;
        }
      }
      break;
    }

    /*---------- PERSINTERFACE -----------*/

    case elemPersinterface_X2DC : {
      int id = atoi(atts[3]);
      if (0 == id) {
        if (0 != strcmp( atts[3], "0" )) {
          const int * pId = getConstValue(atts[3]);
          if (pId) {
            id = *pId;
          }
          else {
            X2DC_ErrMsg( M__BAD_PERSCLASS_ID, atts[1] );
            error = 1;
            return;
          }
        }
      }
      Pers_Interface * pPIF = new Pers_Interface( atts[1], id );
      registerPersInterface( error, pPIF );
      if (error) {
        return;
      }
      break;
    }

    /*---------- DERIVATION --------------*/

    case elemDerivefrom_X2DC : {
      if ( m_isWithin_Class || m_isWithin_TemplClass ||
           m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
        const Cpp_Type * pT = findType(atts[1]);
        if ( (!pT) || (!pT->isFullType()) || (pT->isTemplate()) ) {
          X2DC_ErrMsg( M__BAD_BASECLASS, atts[1], m_pType->getName() );
          error = 1;
          return;
        }
        else {
          Base_Class * pC = new Base_Class(pT);
          m_pType->addBaseClass( error, pC, ( m_isWithin_PersClass || m_isWithin_PersKeyedClass ) );
        }
      }
      break;
    }

    /*---------- CONSTANTS/SIZE ----------*/

    case elemEnum_X2DC : {
      m_isWithin_Enum = true;
      break;
    }

    case elemGeneralenum_X2DC : {
      m_isWithin_GenEnum = true;
      break;
    }

    case elemConst_X2DC : {
      if ( (attNumber_X2DC == X2DC_getAttributeType(atts[4])) ||
           (attExpression_X2DC == X2DC_getAttributeType(atts[4])) ) {
        m_pConst = new Cpp_Const(atts[1]);
        if (attNumber_X2DC == X2DC_getAttributeType(atts[4])) {
          m_pConst->setValue(atoi(atts[5]));
        }
        else {
          m_pConstExpr = new Const_Expr;
        }
      }
      break;
    }

    case elemExpr_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<EXPR>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        m_pPtsOper = new PTS_Operand(Expression);
        PTS_Expression * pPtsExpr = new PTS_Expression();
        m_pPtsOper->setPointer(pPtsExpr);
        pPtsExpr->setWrapper( error, m_pCurrPtsExpr );
        if (error) {
          return;
        }
        m_pCurrPtsExpr->setNextOperand( error, m_pPtsOper );
        if (error) {
          return;
        }
        m_pCurrPtsExpr = pPtsExpr;
        m_pPtsOper = NULL;
      }
      else { // within const or general enum
        if (!m_pConst) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<EXPR>", "m_pConst == NULL" );
          error = 1;
          return;
        }
        else if (!m_pConstExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<EXPR>", "m_pConstExpr == NULL" );
          error = 1;
          return;
        }
      }
      break;
    }

    case elemAdd_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<ADD/>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pCurrPtsExpr->setOperation( error, Add );
        }
      }
      else { // within const or general enum
        if (!m_pConst) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<ADD/>", "m_pConst == NULL" );
          error = 1;
          return;
        }
        else if (!m_pConstExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<ADD/>", "m_pConstExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pConstExpr->setOperator( error, Plus );
        }
      }
      break;
    }

    case elemSub_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<SUB/>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pCurrPtsExpr->setOperation( error, Sub );
        }
      }
      else { // within const or general enum
        if (!m_pConst) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<SUB/>", "m_pConst == NULL" );
          error = 1;
          return;
        }
        else if (!m_pConstExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<SUB/>", "m_pConstExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pConstExpr->setOperator( error, Minus );
        }
      }
      break;
    }

    case elemMul_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<MUL/>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pCurrPtsExpr->setOperation( error, Mul );
        }
      }
      break;
    }

    case elemDiv_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<DIV/>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pCurrPtsExpr->setOperation( error, Div );
        }
      }
      break;
    }

    case elemFixed_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        m_pPtsOper = new PTS_Operand(Value);
      }
      else { // within const or general enum
        if (!m_pConst) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<FIXED>", "m_pConst == NULL" );
          error = 1;
          return;
        }
        else if (!m_pConstExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<FIXED>", "m_pConstExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pConstExpr->setOperand( error, Fixed );
        }
      }
      break;
    }

    case elemIdent_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        m_pPtsOper = new PTS_Operand(Identifier);
      }
      else { // within const or general enum
        if (!m_pConst) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<IDENT>", "m_pConst == NULL" );
          error = 1;
          return;
        }
        else if (!m_pConstExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "<IDENT>", "m_pConstExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pConstExpr->setOperand( error, Ident );
        }
      }
      break;
    }

    case elemSizeof_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        m_pPtsOper = new PTS_Operand(SizeOf);
      }
      break;
    }

    /*---------- MEMBERS -----------------*/

    case elemBitfields_X2DC : {
      if ( m_isWithin_Class || m_isWithin_TemplClass ||
           m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
        const Cpp_Type * pT = findType(atts[1]);
        if (pT) {
          if (0 != strcmp( pT->getName(), "OmsTypeUInt4" )) {
            X2DC_ErrMsg( M__INVALID_BITFIELDS_TYPE, atts[1] );
            error = 1;
            return;
          }
          int size = atoi(atts[3]);
          if (0 != (size % 32)) {
            X2DC_ErrMsg( M__BAD_BITFIELDS_SIZE, m_pType->getName() );
            error = 1;
            return;
          }
          m_pBitFields = new Bit_Fields( atts[1], size/8 );
        }
        else {
          X2DC_ErrMsg( M__BITFIELDS_TYPE_NOT_FOUND, atts[1], m_pType->getName() );
          error = 1;
          return;
        }
      }
      break;
    }

    case elemField_X2DC : {
      int bitCnt = atoi(atts[3]);
      Field * pF = new Field( atts[1], bitCnt );
      m_pBitFields->addField( error, pF );
      break;
    }

    case elemClassmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_TemplClass ||
           m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
        Member * pM = new Member(atts[1]);
        const Cpp_Type * pT = findType(atts[3]);
        if (pT) {
          if ( (pT->isFullType()) && (!pT->isTemplate()) ) {
            pM->setType( error, CppType, pT );
            if (error) {
              return;
            }
          }
          else {
            X2DC_ErrMsg( M__INVALID_MEMBER_TYPE_FOR_CLASS, atts[3], atts[1], m_pType->getName() );
            error = 1;
            return;
          }
        }
        else {
          bool typeSet = false;
          if ( m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
            Iterator<Templ_Param> iter = m_pType->getTemplParamList()->Begin();
            while (iter()) {
              if ( (0 == strcmp( atts[3], iter()->getIdentifier() )) &&
                   (iter()->getKind() == ClassParam) ) {
                setTemplClassparamUsage( error, iter(), FullType );
                if (error) {
                  return;
                }
                pM->setType( error, TemparCppType, iter()->getIdentifier() );
                if (error) {
                  return;
                }
                typeSet = true;
                break;
              }
              iter++;
            }
          }
          if (!typeSet) {
            X2DC_ErrMsg( M__MEMBER_TYPE_NOT_FOUND, atts[3], atts[1], "class", m_pType->getName() );
            error = 1;
            return;
          }
        }
        if ( atts[4] && (attDimension_X2DC == X2DC_getAttributeType(atts[4])) ) {
          /* If attribute DIMENSION exists, then it's the third one. */
          int dim = atoi(atts[5]);
          if (dim < 0) {
            X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, dim, atts[1], m_pType->getName() );
            error = 1;
            return;
          }
          else if (dim > 0) {
            pM->setDimension( error, dim );
            if (error) {
              return;
            }
          }
          else {
            bool linked = false;
            if ( m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
              Iterator<Templ_Param> iter = m_pType->getTemplParamList()->Begin();
              while (iter()) {
                if ( (0 == strcmp( atts[5], iter()->getIdentifier() )) &&
                     (iter()->getKind() == IntParam) ) {
                  pM->linkDimensionToTemplParam( error, -iter()->getNumber() );
                  if (error) {
                    return;
                  }
                  linked = true;
                  break;
                }
                iter++;
              }
            }
            if (!linked) {
              const int * pI = getConstValue(atts[5]);
              if (pI) {
                if (*pI > 0) {
                  pM->setDimension( error, *pI );
                  if (error) {
                    return;
                  }
                }
                else {
                  X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, *pI, atts[1], m_pType->getName() );
                  error = 1;
                  return;
                }
              }
              else {
                X2DC_ErrMsg( M__MEMBER_DIMENSION_NOT_SET, atts[1], m_pType->getName() );
                error = 1;
                return;
              }
            }
          }
        }
        m_pType->addMember( error, pM );
      }
      break;
    }

    case elemMember_X2DC : {
      if (m_isWithin_Struct) {
        if (!m_is_FullType) {
          return;
        }
        Member * pM = new Member(atts[1]);
        const Cpp_Type * pT = findType(atts[3]);
        if (pT) {
          if ( (pT->isFullType()) && (!pT->isTemplate()) ) {
            pM->setType( error, CppType, pT );
            if (error) {
              return;
            }
          }
          else {
            X2DC_WarnMsg( M__NOT_USABLE_IN_PERSISTENCY, "Struct", m_pType->getName(), pT->getName() );
            m_is_FullType = false;
            return;
          }
        }
        else {
          /* Special handling for TabHandle */
          if ( (0 == strcmp( atts[1], "RowDef" )) &&
               (atts[3][strlen(atts[3])-1] == '*') &&
               (!m_pType->hasMember()) ) {
            m_is_FullType = false;
            return;
          }
          else {
            X2DC_ErrMsg( M__MEMBER_TYPE_NOT_FOUND, atts[3], atts[1], "struct", m_pType->getName() );
            error = 1;
            return;
          }
        }
        if (atts[4]) {
          /* If a third attribute exists, then it's DIMENSION. */
          int dim = atoi(atts[5]);
          if (dim < 0) {
            X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, dim, atts[1], m_pType->getName() );
            error = 1;
            return;
          }
          else if (dim > 0) {
            pM->setDimension( error, dim );
            if (error) {
              return;
            }
          }
          else {
            const int * pI = getConstValue(atts[5]);
            if (pI) {
              if (*pI > 0) {
                pM->setDimension( error, *pI );
                if (error) {
                  return;
                }
              }
              else {
                X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, *pI, atts[1], m_pType->getName() );
                error = 1;
                return;
              }
            }
            else {
              X2DC_ErrMsg( M__MEMBER_DIMENSION_NOT_SET, atts[1], m_pType->getName() );
              error = 1;
              return;
            }
          }
        }
        m_pType->addMember( error, pM );
      }
      else if (m_isWithin_Index) {
        Index_Member * pM = new Index_Member( error, atts[1], m_pType );
        if (error) {
          return;
        }
        m_pIndex->addMember( error, pM );
      }
      else if (m_isWithin_Enum) {
        Cpp_Const * pConst = new Cpp_Const( atts[1], atoi(atts[3]) );
        registerConst( error, pConst );
      }
      else if (m_isWithin_GenEnum) {
        if (attValue_X2DC == X2DC_getAttributeType(atts[2])) {
          Cpp_Const * pConst = new Cpp_Const( atts[1], atoi(atts[3]) );
          registerConst( error, pConst );
        } 
        else { // attExpression_X2DC
          m_pConst = new Cpp_Const(atts[1]);
          m_pConstExpr = new Const_Expr;
        }
      }
      break;
    }

    case elemIntrinsicmember_X2DC : {
      if (m_isWithin_Union) {
        Member * pM = new Member(atts[1]);
        const Cpp_Type * pT = findType(atts[3]);
        if (pT) {
          if ( (pT->isFullType()) && (!pT->isTemplate()) ) {
            pM->setType( error, CppType, pT );
            if (error) {
              return;
            }
          }
          else {
            X2DC_ErrMsg( M__INVALID_UNION_MEMBER, pT->getName(), "intrinsic", m_pType->getName() );
            error = 1;
            return;
          }
        }
        m_pType->addMember( error, pM );
      }
      break;
    }

    case elemAlignmentmember_X2DC : {
      if (m_isWithin_Union) {
        const Cpp_Type * pT = findType(atts[3]);
        if (pT) {
          if ( (pT->isFullType()) && (!pT->isTemplate()) &&
               (pT->getAlignment() >= m_pType->getAlignment()) &&
               (pT->getSize() == m_pType->getSize()) ) {
            m_pType->setAlignment(pT->getAlignment());
          }
          else {
            X2DC_ErrMsg( M__INVALID_UNION_MEMBER, pT->getName(), "alignment", m_pType->getName() );
            error = 1;
            return;
          }
        }
      }
      break;
    }

    case elemOidmember_X2DC :
    case elemExtoidmember_X2DC :
    case elemVaroidmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_TemplClass ||
           m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
        Member * pM = new Member(atts[1]);
        switch (X2DC_getElementType(name)) {
          case elemOidmember_X2DC : {
            m_isWithin_OidMember = true;
            m_pMember = pM;
            pM->setType( error, Oid );
            break;
          }
          case elemExtoidmember_X2DC : {
            pM->setType( error, ExtOid );
            break;
          }
          case elemVaroidmember_X2DC : {
            pM->setType( error, VarOid );
            break;
          }
        }
        if (error) {
          return;
        }
        if (atts[2]) {
          /* If a second attribute exists, then it's guaranteed by the validator that it is DIMENSION. */
          int dim = atoi(atts[3]);
          if (dim < 0) {
            X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, dim, atts[1], m_pType->getName() );
            error = 1;
            return;
          }
          else if (dim > 0) {
            pM->setDimension( error, dim );
            if (error) {
              return;
            }
          }
          else {
            bool linked = false;
            if ( m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
              Iterator<Templ_Param> iter = m_pType->getTemplParamList()->Begin();
              while (iter()) {
                if ( (0 == strcmp( atts[3], iter()->getIdentifier() )) &&
                     (iter()->getKind() == IntParam) ) {
                  pM->linkDimensionToTemplParam( error, -iter()->getNumber() );
                  if (error) {
                    return;
                  }
                  linked = true;
                  break;
                }
                iter++;
              }
            }
            if (!linked) {
              const int * pI = getConstValue(atts[3]);
              if (pI) {
                if (*pI > 0) {
                  pM->setDimension( error, *pI );
                  if (error) {
                    return;
                  }
                }
                else {
                  X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, *pI, atts[1], m_pType->getName() );
                  error = 1;
                  return;
                }
              }
              else {
                X2DC_ErrMsg( M__MEMBER_DIMENSION_NOT_SET, atts[1], m_pType->getName() );
                error = 1;
                return;
              }
            }
          }
        }
        m_pType->addMember( error, pM );
      }
      break;
    }

    case elemPtrmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_TemplClass ||
           m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
        Member * pM = new Member(atts[1]);
        pM->setType( error, Pointer );
        if (error) {
          return;
        }
        m_pType->addMember( error, pM );
      }
      break;
    }

    case elemTemplmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_TemplClass ||
           m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
        m_isWithin_TemplMember = true;
        m_pTempl = findType(atts[3]);
        if (!m_pTempl) {
          X2DC_ErrMsg( M__MEMBER_TYPE_NOT_FOUND, atts[3], atts[1], "class", m_pType->getName() );
          error = 1;
          return;
        }
        else if (!m_pTempl->isTemplate()) {
          X2DC_ErrMsg( M__INVALID_MEMBER_TYPE_FOR_CLASS, atts[3], atts[1], m_pType->getName() );
          error = 1;
          return;
        }
        m_pMember = new Member(atts[1]);
        m_pMember->setType( error, TempinCppType, m_pTempl );
        if (error) {
          return;
        }
        if (atts[4]) {
          /* If a third attribute exists, then it's DIMENSION. */
          int dim = atoi(atts[5]);
          if (dim < 0) {
            X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, dim, atts[1], m_pType->getName() );
            error = 1;
            return;
          }
          else if (dim > 0) {
            m_pMember->setDimension( error, dim );
          }
          else {
            bool linked = false;
            if ( m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
              Iterator<Templ_Param> iter = m_pType->getTemplParamList()->Begin();
              while (iter()) {
                if ( (0 == strcmp( atts[5], iter()->getIdentifier() )) &&
                     (iter()->getKind() == IntParam) ) {
                  m_pMember->linkDimensionToTemplParam( error, -iter()->getNumber() );
                  if (error) {
                    return;
                  }
                  linked = true;
                  break;
                }
                iter++;
              }
            }
            if (!linked) {
              const int * pI = getConstValue(atts[5]);
              if (pI) {
                if (*pI > 0) {
                  m_pMember->setDimension( error, *pI );
                }
                else {
                  X2DC_ErrMsg( M__BAD_MEMBER_DIMENSION, *pI, atts[1], m_pType->getName() );
                  error = 1;
                  return;
                }
              }
              else {
                X2DC_ErrMsg( M__MEMBER_DIMENSION_NOT_SET, atts[1], m_pType->getName() );
                error = 1;
                return;
              }
            }
          }
        }
      }
      break;
    }

    case elemClassinstance_X2DC : {
      if ( m_isWithin_TemplMember || m_isWithin_OidMember || m_isWithin_Instantiation ) {
        // If within a template-member or within instantiation of a template
        // determine the number of the current parameter instance.
        int nextTempinNo = 1;
        if (m_isWithin_TemplMember) {
          nextTempinNo += m_pMember->getTemplParamInstanceCount();
        }
        else if ( (m_isWithin_Instantiation) && (m_pTempinList->getLastItem()) ) {
          nextTempinNo += m_pTempinList->getLastItem()->getNumber();
        }
        // If within a template-member or within instantiation of a template
        // get the nextTempinNo-th parameter description of the template.
        const Templ_Param * pTempar = NULL;
        if (m_pTempl) { // Then: m_isWithin_TemplMember || m_isWithin_Instantiation
          pTempar = m_pTempl->getNthTemplParam(nextTempinNo);
          if (!pTempar) {
            X2DC_ErrMsg( M__NTH_TEMPAR_NOT_FOUND, nextTempinNo, m_pTempl->getName() );
            error = 1;
            return;
          }
          else if (pTempar->getKind() != ClassParam) {
            X2DC_ErrMsg( M__TEMPLPARAM_KIND_MISMATCH,
                         "Non-class", pTempar->getIdentifier(), m_pTempl->getName(), "class" );
            error = 1;
            return;
          }
        }
        else if (m_pPersTempl) { // Then: m_isWithin_Instantiation
          pTempar = m_pPersTempl->getClass()->getNthTemplParam(nextTempinNo);
          if (!pTempar) {
            X2DC_ErrMsg( M__NTH_TEMPAR_NOT_FOUND, nextTempinNo, m_pPersTempl->getClass()->getName() );
            error = 1;
            return;
          }
          else if (pTempar->getKind() != ClassParam) {
            X2DC_ErrMsg( M__TEMPLPARAM_KIND_MISMATCH,
                         "Non-class", pTempar->getIdentifier(), m_pPersTempl->getClass()->getName(), "class" );
            error = 1;
            return;
          }
        }
        // If within a template check if the current class instance
        // isn't a concrete type but one of the template's parameters.
        Templ_ParamInstance * pTempin = NULL;
        if ( m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
          // Then: ( m_isWithin_TemplMember || m_isWithin_OidMember ) && (!m_isWithin_Instantiation)
          Iterator<Templ_Param> iter = m_pType->getTemplParamList()->Begin();
          while (iter()) {
            if ( (0 == strcmp( atts[1], iter()->getIdentifier() )) 
                 && (ClassParam == iter()->getKind()) ) {
              pTempin = new Templ_ParamInstance( error, ParamForClass, iter()->getNumber() );
              if (error) {
                return;
              }
              if (m_isWithin_TemplMember) {
                switch (pTempar->getClassparamUsage()) {
                  case Unspecified : {
                    break;
                  }
                  case FullType :
                  case PersClass : {
                    setTemplClassparamUsage( error, iter(), pTempar->getClassparamUsage() );
                    if (error) {
                      return;
                    }
                    break;
                  }
                }
              }
              else { // m_isWithin_OidMember
                setTemplClassparamUsage( error, iter(), PersClass );
                if (error) {
                  return;
                }
              }
              break;
            }
            iter++;
          }
        }
        // Not within a template or the current class instance isn't one of the template's parameters.
        if (!pTempin) {
          if (m_isWithin_OidMember) {
            const Pers_Class * pPC = findPersClass(atts[1]);
            if (pPC) {
              if (pPC->isTemplate()) {
                X2DC_ErrMsg( M__OIDMEMBER_CLASSINSTANCE_IS_PERSTEMPL,
                             atts[1], m_pMember->getName(), m_pType->getName() );
                error = 1;
                return;
              }
              else {
                pTempin = new Templ_ParamInstance( error, pPC );
                if (error) {
                  return;
                }
              }
            }
            else {
              X2DC_ErrMsg( M__OIDMEMBER_CLASSINSTANCE_NOT_FOUND,
                           atts[1], m_pMember->getName(), m_pType->getName() );
              error = 1;
              return;
            }
          }
          else { // m_isWithin_TemplMember || m_isWithin_Instantiation
            // Note: pTempar->getKind() == ClassParam
            const Cpp_Type * pT = findType(atts[1]);
            if (pT) {
              if (pTempar->getClassparamUsage() == PersClass) {
                X2DC_ErrMsg( M__TEMPAR_SUPPOSED_FOR_PERSCLASS,
                             pTempar->getIdentifier(), m_pTempl->getName(), atts[1] );
                error = 1;
                return;
              }
              else if (pT->isTemplate()) {
                X2DC_ErrMsg( M__CLASSINSTANCE_IS_TEMPLATE,
                             atts[1], pTempar->getIdentifier(), m_pTempl->getName() );
                error = 1;
                return;
              }
              else if ( (!pT->isFullType()) && (pTempar->getClassparamUsage() == FullType) ) {
                X2DC_ErrMsg( M__TEMPAR_SUPPOSED_FOR_FULLTYPE,
                             pTempar->getIdentifier(), m_pTempl->getName(), atts[1] );
                error = 1;
                return;
              }
              else {
                pTempin = new Templ_ParamInstance( error, /*InstanceForClass,*/ pT );
                if (error) {
                  return;
                }
              }
            }
            else {
              const Pers_Class * pPC = findPersClass(atts[1]);
              if (pPC) {
                if (pTempar->getClassparamUsage() == PersClass) {
                  pTempin = new Templ_ParamInstance( error, pPC );
                  if (error) {
                    return;
                  }
                }
                else {
                  X2DC_ErrMsg( M__TEMPAR_NOT_SUPPOSED_FOR_PERSCLASS,
                               pTempar->getIdentifier(), m_pTempl->getName(), atts[1] );
                  error = 1;
                  return;
                }
              }
              else if (pTempar->getClassparamUsage() == Unspecified) {
                registerOnlyName( error, atts[1] );
                if (error) {
                  return;
                }
                pT = findType(atts[1]);
                pTempin = new Templ_ParamInstance( error, /*InstanceForClass,*/ pT );
                if (error) {
                  return;
                }
              }
              else {
                X2DC_ErrMsg( M__CLASSINSTANCE_NOT_FOUND,
                             atts[1], pTempar->getIdentifier(), m_pTempl->getName() );
                error = 1;
                return;
              }
            }
          }
        }
        // Add tempalte parameter instance
        if ( m_isWithin_TemplMember || m_isWithin_OidMember) {
//          addTemplParamInstance( error, m_pMember->getTemplParamInstanceList(), pTempin );
          m_pMember->addTemplParamInstance( error, pTempin );
        }
        else { // m_isWithin_Instantiation
          addTemplParamInstance( error, m_pTempinList, pTempin );
        }
      }
      break;
    }

    case elemIntinstance_X2DC : {
      if ( m_isWithin_TemplMember || m_isWithin_Instantiation ) {
        int value = atoi(atts[1]);
        if (value == 0) {
          if (0 != strcmp( atts[1], "0" )) {
            const int * pI = getConstValue(atts[1]);
            if (pI) {
              value = *pI;
            }
            else {
              X2DC_ErrMsg( M__BAD_INTINSTANCE, atts[1], m_pTempl->getName() );
              error = 1;
              return;
            }
          }
        }
        Templ_ParamInstance * pTempin = new Templ_ParamInstance( error, InstanceForInt, value );
        if (error) {
          return;
        }
        if (m_isWithin_TemplMember) {
//          addTemplParamInstance( error, m_pMember->getTemplParamInstanceList(), pTempin );
          m_pMember->addTemplParamInstance( error, pTempin );
        }
        else {
          addTemplParamInstance( error, m_pTempinList, pTempin );
        }
      }
      break;
    }

    /*---------- INSTANTIATE -------------*/

    case elemInstantiate_X2DC : {
      m_isWithin_Instantiation = true;
      m_pTempl = findType(atts[1]);
      if (!m_pTempl) {
        m_pPersTempl = findPersClass(atts[1]);
        if (!m_pPersTempl) {
          X2DC_ErrMsg( M__TEMPLATE_NOT_FOUND, atts[1] );
          error = 1;
          return;
        }
        else if (!m_pPersTempl->isTemplate()) {
          X2DC_ErrMsg( M__NOT_TEMPLATE, "Persistent class", atts[1] );
          error = 1;
          return;
        }
      }
      else if (!m_pTempl->isTemplate()) {
        X2DC_ErrMsg( M__NOT_TEMPLATE, "Class", atts[1] );
        error = 1;
        return;
      }
      m_pType = new Cpp_Type(atts[3]);
      m_pType->setVtType(csp1_vt_userdefined);
      if (m_pPersTempl) {
        m_pType->setSize(SizeOfPointer());
        m_pType->setAlignment(AlignmentOfPointer());
      }
      m_pTempinList = new List<Templ_ParamInstance>();
      break;
    }

    /*---------- TYPEDEF -----------------*/

    case elemTypedef_X2DC : {
      int nIdx = -1, tIdx = -1, gIdx = -1;
      for ( int i = 0; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attName_X2DC : {
            nIdx = i;
            break;
          }
          case attType_X2DC : {
            tIdx = i;
            break;
          }
          case attGentabhandle_X2DC : {
            gIdx = i;
            break;
          }
        }
      }
      const Cpp_Type * pT = findType(atts[tIdx+1]);
      if (pT) {
        addSynonym( error, pT, atts[nIdx+1] );
        if (error) {
          return;
        }
        if ( (gIdx > -1) && (0 == strcmp( atts[gIdx+1], "YES" )) ) {
          char * pTH = new char[strlen(atts[nIdx+1])+strlen("TabHandle")+1];
          strcpy( pTH, atts[nIdx+1] );
          registerOnlyName( error, strcat( pTH, "TabHandle" ) );
          delete [] pTH;
          pTH = NULL;
        }
      }
      else {
        X2DC_ErrMsg( M__BAD_TYPE_IN_TYPEDEF, atts[tIdx+1], atts[nIdx+1] );
        error = 1;
        return;
      }
      break;
    }

  }

}

/*----------------------------------------------------------------------------*/

void Cpp_Type_Registry::EndElement( int & error, const char * name ) {

  /* Within livecachetypes */

  if (m_isWithin_LCTypes) {
    if (elemImport_X2DC == X2DC_getElementType(name)) {
      /* Within livecachetypes.xml no further import occurs,
         so </IMPORT> must correspond to <IMPORT FILE="livecachetypes.xml">. */
      m_isWithin_LCTypes = false;
    }
    return;
  }

  /* Others */

  switch (X2DC_getElementType(name)) {

    /*---------- STRUCT ------------------*/

    case elemStruct_X2DC : {
      if (m_is_FullType) {
        /* Check existence of member(s) */
        if (!m_pType->hasMember()) {
          X2DC_ErrMsg( M__NO_MEMBERS, "Struct", m_pType->getName() );
          error = 1;
          return;
        }
        /* Adjust size */
        m_pType->setSize(m_pType->getSize() + PadSize( m_pType->getSize(), m_pType->getAlignment() ));
        /* Register */
        registerFullType( error, m_pType );
        if (error) {
          return;
        }
        m_is_FullType = false;
      }
      else {
        registerOnlyName( error, m_pType->getName() );
        if (error) {
          return;
        }
        delete m_pType;
      }
      if (m_pTabHandle) {
        registerOnlyName( error, m_pTabHandle );
        if (error) {
          return;
        }
        delete [] m_pTabHandle;
        m_pTabHandle = NULL;
      }
      m_pType = NULL;
      m_isWithin_Struct = false;
      break;
    }

    /*---------- UNION -------------------*/

    case elemUnion_X2DC : {
      /* Check size */
      int padSz = PadSize( m_pType->getSize(), m_pType->getAlignment() );
      if (padSz != 0) {
        X2DC_ErrMsg( M__INVALID_PADDING_SIZE, padSz, m_pType->getName() );
        error = 1;
        return;
      }
      /* Register */
      registerFullType( error, m_pType );
      if (error) {
        return;
      }
      m_pType = NULL;
      m_isWithin_Union = false;
      break;
    }

    /*---------- CLASS -------------------*/

    case elemClass_X2DC : {
      /* Check existence of member(s) */
      if (!m_pType->hasMember()) {
        X2DC_ErrMsg( M__NO_MEMBERS, "Class", m_pType->getName() );
        error = 1;
        return;
      }
      /* Adjust size */
      m_pType->setSize(m_pType->getSize() + PadSize( m_pType->getSize(), m_pType->getAlignment() ));
      /* Register */
      registerFullType( error, m_pType );
      m_pType = NULL;
      m_isWithin_Class = false;
      break;
    }

    /*---------- TEMPLCLASS --------------*/

    case elemTemplclass_X2DC : {
      /* Check existence of member(s) */
      if (!m_pType->hasMember()) {
        X2DC_ErrMsg( M__NO_MEMBERS, "Template class", m_pType->getName() );
        error = 1;
        return;
      }
      /* Register */
      registerFullType( error, m_pType );
      m_pType = NULL;
      m_isWithin_TemplClass = false;
      break;
    }

    case elemTemplparams_X2DC : {
      if (0 == m_TemplParam_Count) {
        if (m_pPersClass) {
          X2DC_ErrMsg( M__NO_TEMPLPARAMS, "persistent template", m_pType->getName() );
        }
        else {
          X2DC_ErrMsg( M__NO_TEMPLPARAMS, "template", m_pType->getName() );
        }
        error = 1;
        return;
      }
      if (m_isWithin_PersTemplClass) {
        bool linked = false;
        Iterator<Templ_Param> temparIter = m_pType->getTemplParamList()->Begin();
        while (temparIter()) {
          if ( (0 == strcmp( temparIter()->getIdentifier(), m_pID )) &&
               (temparIter()->getKind() == IntParam) ) {
            m_pPersClass->linkIDToTemplParam( error, -temparIter()->getNumber() );
            if (error) {
              return;
            }
            linked = true;
            break;
          }
          temparIter++;
        }
        delete [] m_pID;
        m_pID = NULL;
        if (!linked) {
          X2DC_ErrMsg( M__PERSTEMPL_ID_NOT_LINKED, m_pType->getName() );
          error = 1;
          return;
        }
      }
      m_isWithin_TemplParams = false;
      break;
    }

    case elemClassparam_X2DC :
    case elemIntparam_X2DC : {
      break;
    }

    /*---------- PERSCLASS ---------------*/

    case elemPersclass_X2DC : {
      /* Check existence of member(s) */
      if (!m_pType->hasMember()) {
        X2DC_ErrMsg( M__NO_MEMBERS, "Persistent class", m_pType->getName() );
        error = 1;
        return;
      }
      /* Adjust size */
      m_pType->setSize( m_pType->getSize() +
                        PadSize( m_pType->getSize(), m_pType->getAlignment() ) -
                        SizeOfPointer() );
      /* Check size */
      if (!PersSizesMatch( m_pType->getSize(), m_pcSize - SizeOfPointer() )) {
        X2DC_ErrMsg( M__PERSCLASS_SIZE_MISMATCH,
                     m_pcSize - SizeOfPointer(), m_pType->getName(), m_pType->getSize() );
        error = 1;
        return;
      }
      /* Register */
      registerPersClass( error, m_pPersClass );
      m_pType = NULL;
      m_pPersClass = NULL;
      m_isWithin_PersClass = false;
      break;
    }

    case elemSize_X2DC : {
      if (m_isWithin_PersTemplClass) {
        m_pCurrPtsExpr = NULL;
        m_isWithin_PersTemplClassSize = false;
      }
      break;
    }

    case elemIndex_X2DC : {
      m_pPersClass->addIndex( error, m_pIndex );
      m_pIndex = NULL;
      m_isWithin_Index = false;
      break;
    }

    case elemKey_X2DC : {
      m_pPersClass->setKey(m_pIndex);
      m_pIndex = NULL;
      m_isWithin_Index = false;
      break;
    }

    /*---------- PERSKEYEDCLASS ----------*/

    case elemPerskeyedclass_X2DC : {
      /* Check existence of member(s) */
      if (!m_pType->hasMember()) {
        X2DC_ErrMsg( M__NO_MEMBERS, "Persistent keyed class", m_pType->getName() );
        error = 1;
        return;
      }
      /* Adjust size */
      m_pType->setSize( m_pType->getSize() +
                        PadSize( m_pType->getSize(), m_pType->getAlignment() ) -
                        SizeOfPointer() );
      /* Check size */
      if (!PersSizesMatch( m_pType->getSize(), m_pcSize - SizeOfPointer() )) {
        X2DC_ErrMsg( M__PERSCLASS_SIZE_MISMATCH,
                     m_pcSize - SizeOfPointer(), m_pType->getName(), m_pType->getSize() );
        error = 1;
        return;
      }
      /* Register */
      registerPersClass( error, m_pPersClass );
      m_pType = NULL;
      m_pPersClass = NULL;
      m_isWithin_PersKeyedClass = false;
      break;
    }

    /*---------- PERSTEMPLCLASS ----------*/

    case elemPerstemplclass_X2DC : {
      /* Check existence of member(s) */
      if (!m_pType->hasMember()) {
        X2DC_ErrMsg( M__NO_MEMBERS, "Persistent template class", m_pType->getName() );
        error = 1;
        return;
      }
      /* Register */
      registerPersClass( error, m_pPersClass );
      m_pType = NULL;
      m_pPersClass = NULL;
      m_isWithin_PersTemplClass = false;
      break;
    }

    /*---------- DERIVATION --------------*/

    case elemDerivefrom_X2DC : {
      break;
    }

    /*---------- CONSTANTS/SIZE ----------*/

    case elemEnum_X2DC : {
      m_isWithin_Enum = false;
      break;
    }

    case elemGeneralenum_X2DC : {
      m_isWithin_GenEnum = false;
      break;
    }

    case elemConst_X2DC : {
      if (m_pConst) {
        registerConst( error, m_pConst );
        m_pConst = NULL;
      }
      break;
    }

    case elemExpr_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</EXPR>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        else if (!m_pCurrPtsExpr->getWrapper()) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</EXPR>", "m_pCurrPtsExpr->getWrapper() == NULL" );
          error = 1;
          return;
        }
        else if (!m_pCurrPtsExpr->getOperand1()) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</EXPR>", "m_pCurrPtsExpr->getOperand1() == NULL" );
          error = 1;
          return;
        }
        else if (!m_pCurrPtsExpr->getOperand2()) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</EXPR>", "m_pCurrPtsExpr->getOperand2() == NULL" );
          error = 1;
          return;
        }
        else {
          m_pCurrPtsExpr = m_pCurrPtsExpr->getWrapper();
        }
      }
      else { // within const or general enum
        if (!m_pConst) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</EXPR>", "m_pConst == NULL" );
          error = 1;
          return;
        }
        else if (!m_pConstExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</EXPR>", "m_pConstExpr == NULL" );
          error = 1;
          return;
        }
        else {
          const int * pI = m_pConstExpr->getValue();
          if (!pI) {
            X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</EXPR>", "m_pConstExpr->getValue() == NULL" );
            error = 1;
            return;
          }
          else {
            if (m_pConst->getValue()) {
              X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</EXPR>", "m_pConst->getValue() != NULL" );
              error = 1;
              return;
            }
            else {
              m_pConst->setValue(*pI);
            }
          }
          delete m_pConstExpr;
          m_pConstExpr = NULL;
        }
      }
      break;
    }

    case elemAdd_X2DC :
    case elemSub_X2DC :
    case elemMul_X2DC :
    case elemDiv_X2DC : {
      break;
    }

    case elemFixed_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</FIXED>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        else if (!m_pPtsOper) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</FIXED>", "m_pPtsOper == NULL" );
          error = 1;
          return;
        }
        else if (!m_pPtsOper->getPointer()) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</FIXED>", "m_pPtsOper->getPointer() == NULL" );
          error = 1;
          return;
        }
        else {
          m_pCurrPtsExpr->setNextOperand( error, m_pPtsOper );
          m_pPtsOper = NULL;
        }
      }
      else { // within const or general enum
        if (!m_pConst) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</FIXED>", "m_pConst == NULL" );
          error = 1;
          return;
        }
        else if (!m_pConstExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</FIXED>", "m_pConstExpr == NULL" );
          error = 1;
          return;
        }

        else {
          m_pConstExpr->resetOperand();
        }
      }
      break;
    }

    case elemIdent_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</IDENT>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        else if (!m_pPtsOper) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</IDENT>", "m_pPtsOper == NULL" );
          error = 1;
          return;
        }
        else if (!m_pPtsOper->getPointer()) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</IDENT>", "m_pPtsOper->getPointer() == NULL" );
          error = 1;
          return;
        }
        else {
          m_pCurrPtsExpr->setNextOperand( error, m_pPtsOper );
          m_pPtsOper = NULL;
        }
      }
      else { // within const or general enum
        if (!m_pConst) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</IDENT>", "m_pConst == NULL" );
          error = 1;
          return;
        }
        else if (!m_pConstExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</IDENT>", "m_pConstExpr == NULL" );
          error = 1;
          return;
        }
        else {
          m_pConstExpr->resetOperand();
        }
      }
      break;
    }

    case elemSizeof_X2DC : {
      if (m_isWithin_PersTemplClassSize) {
        if (!m_pCurrPtsExpr) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</SIZEOF>", "m_pCurrPtsExpr == NULL" );
          error = 1;
          return;
        }
        else if (!m_pPtsOper) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</SIZEOF>", "m_pPtsOper == NULL" );
          error = 1;
          return;
        }
        else if (!m_pPtsOper->getPointer()) {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "</SIZEOF>", "m_pPtsOper->getPointer() == NULL" );
          error = 1;
          return;
        }
        else {
          m_pCurrPtsExpr->setNextOperand( error, m_pPtsOper );
          m_pPtsOper = NULL;
        }
      }
      break;
    }

    /*---------- MEMBERS -----------------*/

    case elemField_X2DC : {
      break;
    }

    case elemBitfields_X2DC : {
      if (!m_pBitFields->sizeConsistent()) {
        X2DC_ErrMsg( M__BITFIELDS_SIZE_MISMATCH,
                     m_pBitFields->getSize()*8, m_pType->getName(), m_pBitFields->getBitCount() );
        error = 1;
        return;
      }
      m_pType->incBitfieldCount();
      char name[20];
      memset( name, 0, sizeof(name) );
      sprintf( name, "X2DC_bfm_%d", m_pType->getBitfieldCount());
      Member * pM = new Member(name);
      pM->setType( error, BitFields, m_pBitFields );
      if (error) {
        return;
      }
      m_pBitFields = NULL;
      m_pType->addMember( error, pM );
      break;
    }

    case elemClassmember_X2DC : {
      break;
    }

    case elemMember_X2DC : {
      if ( m_isWithin_GenEnum && m_pConst ) {
        registerConst( error, m_pConst );
        m_pConst = NULL;
      }
      break;
    }

    case elemIntrinsicmember_X2DC :
    case elemAlignmentmember_X2DC : {
      break;
    }

    case elemOidmember_X2DC : {
      m_pMember = NULL;
      m_isWithin_OidMember = false;
      break;
    }

    case elemExtoidmember_X2DC :
    case elemVaroidmember_X2DC :
    case elemPtrmember_X2DC : {
      break;
    }

    case elemTemplmember_X2DC : {
      Iterator<Templ_ParamInstance> iter = m_pMember->getTemplParamInstanceList()->Begin();
      bool instantiate = true;
      while (iter()) {
        if (ParamForClass == iter()->getKind()) {
          instantiate = false;
          break;
        }
        iter++;
      }
      iter.reset();
      if (instantiate) {
        const char * pN = genNextTypeName();
        Cpp_Type * pT = new Cpp_Type(pN);
        delete [] pN;
        pN = NULL;
        instantiateTemplate( error, m_pMember->getCpptypePtr(), &iter, pT );
        if (error) {
          return;
        }
        else {
          pT->setVtType(csp1_vt_userdefined);
          registerFullType( error, pT );
          if (error) {
            return;
          }
        }
        m_pMember->promoteTempinCppTypeToCppType( error, pT );
        if (error) {
          return;
        }
      }
      m_pType->addMember( error, m_pMember );
      m_pMember = NULL;
      m_pTempl = NULL;
      m_isWithin_TemplMember = false;
      break;
    }

    case elemClassinstance_X2DC :
    case elemIntinstance_X2DC : {
      break;
    }

    /*---------- INSTANTIATE -------------*/

    case elemInstantiate_X2DC : {
      if (m_isWithin_Instantiation) {
        Iterator<Templ_ParamInstance> tempinIter = m_pTempinList->Begin();
        if (m_pTempl) { // non-pers. template
          instantiateTemplate( error, m_pTempl, &tempinIter, m_pType );
          if (error) {
            return;
          }
          /* Adjust size */
          m_pType->setSize(m_pType->getSize() + PadSize( m_pType->getSize(), m_pType->getAlignment() ));
          /* Register */
          registerFullType( error, m_pType );
          if (error) {
            return;
          }
        }
        else { // pers. template
          instantiateTemplate( error, m_pPersTempl->getClass(), &tempinIter, m_pType, true );
          if (error) {
            return;
          }
          tempinIter.reset();
          int  id = 0;
          bool idSet = false;
          while (tempinIter()) {
            if (tempinIter()->getNumber() == -m_pPersTempl->getID()) {
              id = *tempinIter()->getIntPtr();
              idSet = true;
              break;
            }
            tempinIter++;
          }
          if (!idSet) {
            X2DC_ErrMsg( M__ID_NOT_SET_IN_PERSINSTANTIATION, m_pPersTempl->getName(), m_pType->getName() );
            error = 1;
            return;
          }
          tempinIter.reset();
          Pers_Class * pPC = new Pers_Class( error, id, m_pType );
          if (error) {
            return;
          }
          if (m_pPersTempl->getKey()) {
            Index * pI = new Index(m_pPersTempl->getKey()->getID());
            Iterator<Index_Member> keyMemIter = m_pPersTempl->getKey()->getMemberList()->Begin();
            while (keyMemIter()) {
              Index_Member * pIM = new Index_Member( error, keyMemIter()->getName(), m_pType );
              if (error) {
                return;
              }
              pI->addMember( error, pIM );
              if (error) {
                return;
              }
              keyMemIter++;
            }
            pPC->setKey(pI);
          }
          Iterator<Index> indIter = m_pPersTempl->getIndexList()->Begin();
          while (indIter()) {
            Index * pI = new Index(indIter()->getID());
            Iterator<Index_Member> indMemIter = indIter()->getMemberList()->Begin();
            while (indMemIter()) {
              Index_Member * pIM = new Index_Member( error, indMemIter()->getName(), m_pType );
              if (error) {
                return;
              }
              pI->addMember( error, pIM );
              if (error) {
                return;
              }
              indMemIter++;
            }
            pPC->addIndex( error, pI );
            if (error) {
              return;
            }
            indIter++;
          }
          /* Adjust size */
          m_pType->setSize(m_pType->getSize() + PadSize( m_pType->getSize(), m_pType->getAlignment() )
                           - SizeOfPointer());
          /* Check size */
          int size = getExprValue( error, m_pType, m_pPersTempl->getPtsExpression(), &tempinIter );
          if (error) {
            return;
          }
          if (!PersSizesMatch( m_pType->getSize(), size - SizeOfPointer() )) {
            X2DC_ErrMsg( M__PERSCLASS_SIZE_MISMATCH,
                         size - SizeOfPointer(), m_pType->getName(), m_pType->getSize() );
            error = 1;
            return;
          }
          /* Register */
          registerPersClass( error, pPC );
          if (error) {
            return;
          }
        }
        m_pType = NULL;
        m_pTempl = NULL;
        m_pPersTempl = NULL;
        m_pTempinList->Clear();
        delete m_pTempinList;
        m_pTempinList = NULL;
        m_isWithin_Instantiation = false;
        break;
      }

    }

  }

}

/*----------------------------------------------------------------------------*/

void Cpp_Type_Registry::CharData( int & error, const char * s, int len ) {

  if (m_isWithin_PersTemplClassSize) {

    if (!m_pPtsOper) {
      X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "persistent template size calculation", "m_pPtsOper == NULL" );
      error = 1;
      return;
    }
    else {
      switch (m_pPtsOper->getType()) {
        case Value : {
          char * val = new char[len+1];
          strncpy( val, s, len );
          val[len] = '\0';
          int * pI = new int;
          *pI = atoi(val);
          delete [] val;
          val = NULL;
          m_pPtsOper->setPointer(pI);
          break;
        }
        case Identifier : {
          char * pIdent = new char[len+1];
          strncpy( pIdent, s, len );
          pIdent[len] = '\0';
          Iterator<Templ_Param> temparIter = m_pType->getTemplParamList()->Begin();
          const Templ_Param * pTempar = findTemplParam( &temparIter, pIdent );
          if ( pTempar && (pTempar->getKind() == IntParam) ) {
            m_pPtsOper->switchTypeToTemplParam(error);
            if (error) {
              return;
            }
            int * pI = new int;
            *pI = -pTempar->getNumber();
            m_pPtsOper->setPointer(pI);
          }
          else {
            if (getConstValue(pIdent)) {
              int * pI = new int;
              *pI = *getConstValue(pIdent);
              m_pPtsOper->setPointer(pI);
            }
            else {
              X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S,
                           "persistent template size formula, can not find constant", pIdent );
              error = 1;
              return;
            }
          }
          delete [] pIdent;
          pIdent = NULL;
          break;
        }
        case SizeOf : {
          char * pName = new char[len+1];
          strncpy( pName, s, len );
          pName[len] = '\0';
          const Member * pMem = m_pType->findCpptypeMember( error, pName );
          if (error) {
            return;
          }
          delete [] pName;
          pName = NULL;
          if (pMem) {
            m_pPtsOper->setPointer(pMem);
          }
          else {
            X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S,
                         "persistent template size formula, can not find C++ type member", pMem->getName() );
            error = 1;
            return;
          }
          break;
        }
      }
    }

  }
  else { // within const or general enum

    if (!m_pConst) {
      X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "constant calculation", "m_pConst == NULL" );
      error = 1;
      return;
    }
    else if (!m_pConstExpr) {
      X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "constant calculation", "m_pConstExpr == NULL" );
      error = 1;
      return;
    }
    else if (!m_pConstExpr->getOperand()) {
      X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S, "constant calculation", "m_pConstExpr->getOperand() == NULL" );
      error = 1;
      return;
    }

    char * operand = new char[len+1];
    strncpy( operand, s, len );
    operand[len] = '\0';

    switch (*(m_pConstExpr->getOperand())) {
      case Fixed : {
        m_pConstExpr->setValue( error, atoi(operand) );
        break;
      }
      case Ident : {
        const int * pI = getConstValue(operand);
        if (pI) {
          m_pConstExpr->setValue( error, *pI );
        }
        else {
          X2DC_ErrMsg( M__INTERNAL_ERROR_IN_S_S,
                       "constant calculation", "can not find constant", operand );
          error = 1;
          return;
        }
        break;
      }
    }

    delete [] operand;
    operand = NULL;

  }

}

