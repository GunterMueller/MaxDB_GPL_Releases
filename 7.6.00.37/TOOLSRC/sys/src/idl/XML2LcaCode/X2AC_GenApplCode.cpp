/*!*****************************************************************************

  module      : X2ACGenApplCode.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of application code ("COM routine wrappers")
  description : Generate files

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


#include "idl/XML2LcaCode/X2AC_GenApplCode.hpp"
#include "idl/XML2LcaCode/X2AC_WarnAndErrHandler.hpp"


/******************************************************************************/
/*     Implementation of class GenApplCode                                    */
/******************************************************************************/

GenApplCode::GenApplCode()
: m_pRawName(NULL)
, m_pPartList(NULL)
, m_pModule(NULL)
, m_Inccontent_spec(false)
, m_pFunctor(NULL)
, m_functorHeader(false)
, m_pGlobals(NULL)
, m_pLibrary(NULL)
, m_precompHeader(false)
, m_isWithin_Idl(false)
, m_generate(true)
, m_SchemaType_spec(false)
, m_TransType_spec(false)
, m_paramCntr(0)
, m_obj_cpp_lineLen(0)
, m_routines_cpp_lineLen(0)
{}

/*----------------------------------------------------------------------------*/

GenApplCode::~GenApplCode() {

  /* file name elements */
  m_pRawName  = NULL;
  m_pPartList = NULL;
  m_pModule   = NULL;

  /* INCCONTENT attributes */
  if (m_pFunctor) {
    delete [] m_pFunctor;
    m_pFunctor = NULL;
  }
  if (m_pGlobals) {
    delete [] m_pGlobals;
    m_pGlobals = NULL;
  }
  if (m_pLibrary) {
    delete [] m_pLibrary;
    m_pLibrary = NULL;
  }

  /* <RawName>_obj.h */
  m_obj_h << "#endif" << endl;
  m_obj_h << endl << endl;
  m_obj_h.close();

  /* <RawName>_obj.cpp */
  m_obj_cpp.close();

  /* <RawName>_routines.h */
  m_routines_h << "#endif" << endl;
  m_routines_h << endl << endl;
  m_routines_h.close();

  /* <RawName>_routines.cpp */
  m_routines_cpp.close();

  /* <RawName>_ifc.h */
  m_ifc_h << "#endif" << endl;
  m_ifc_h << endl << endl;
  m_ifc_h.close();

}

/*----------------------------------------------------------------------------*/

void GenApplCode::ToLower( const char * upper, char * lower, int transLen ) {
  int len = (transLen > 0) ? transLen : strlen(upper);
  for ( int i = 0; i < len; i++ ) {
    if (upper[i] == '_') {
      lower[i] = '_';
    }
    else {
      lower[i] = tolower(upper[i]);
    }
  }
  lower[len] = 0;
}

/*----------------------------------------------------------------------------*/

void GenApplCode::ToUpper( const char * lower, char * upper, int transLen ) {
  int len = (transLen > 0) ? transLen : strlen(lower);
  for ( int i = 0; i < len; i++ ) {
    if (lower[i] == '_') {
      upper[i] = '_';
    }
    else {
      upper[i] = toupper(lower[i]);
    }
  }
  upper[len] = 0;
}

/*----------------------------------------------------------------------------*/

bool GenApplCode::IsPointer(const char * type) {
  int len = strlen(type);
  if ( (len < 2) || (type[len-1] != '*') || (type[len-2] == ' ')|| (type[len-2] == '*') ) {
    return false;
  }
  else {
    return true;
  }
}

/*----------------------------------------------------------------------------*/

bool GenApplCode::IsTabhandle(const char * type) {
  int len = strlen(type);
  if ( (len < 11) || (0 != strcmp( &type[len-10], "TabHandle*" )) ) {
    return false;
  }
  else {
    return true;
  }
}

/*----------------------------------------------------------------------------*/

int GenApplCode::Init( int & error, char * RawName, List * PartList, char * Module ) {

  m_pRawName  = RawName;
  m_pPartList = PartList;
  m_pModule   = Module;

  int len = strlen(RawName);

  char * file = new char [len+13+1]; // 13 == strlen("_routines.cpp")
  strcpy( &file[0], RawName );

  char * RawBlocker = new char[len+1];
  ToUpper( RawName, RawBlocker );

  /* <RawName>_opj.h */

  strcpy( &file[len], "_obj.h" );

  m_obj_h.open(file);
  if (m_obj_h.fail()) {
    X2AC_ErrMsg( X2AC_FILE_OPEN_ERROR, file );
    error = 1;
    return 1;
  }

  m_obj_h << "//************************************************************************//" << endl;
  m_obj_h << "//                                                                          " << endl;
  m_obj_h << "//  This file is generated by tool                                          " << endl;
  m_obj_h << "//                                                                          " << endl;
  m_obj_h << "//                 XML2LcaCode                                              " << endl;
  m_obj_h << "//                                                                          " << endl;
  m_obj_h << "//  from input file " << RawName << "_methods.xml.                          " << endl;
  m_obj_h << "//                                                                          " << endl;
  m_obj_h << "//************************************************************************//" << endl;

  m_obj_h << endl << endl;
  m_obj_h << "#ifndef INC_" << RawBlocker << "_OBJ" << endl;
  m_obj_h << "#define INC_" << RawBlocker << "_OBJ" << endl;
  m_obj_h << endl << endl;

  /* <RawName>_opj.cpp */

  strcpy( &file[len], "_obj.cpp" );

  m_obj_cpp.open(file);
  if (m_obj_cpp.fail()) {
    X2AC_ErrMsg( X2AC_FILE_OPEN_ERROR, file );
    error = 1;
    return 1;
  }

  m_obj_cpp << "//************************************************************************//" << endl;
  m_obj_cpp << "//                                                                          " << endl;
  m_obj_cpp << "//  This file is generated by tool                                          " << endl;
  m_obj_cpp << "//                                                                          " << endl;
  m_obj_cpp << "//                 XML2LcaCode                                              " << endl;
  m_obj_cpp << "//                                                                          " << endl;
  m_obj_cpp << "//  from input file " << RawName << "_methods.xml.                          " << endl;
  m_obj_cpp << "//                                                                          " << endl;
  m_obj_cpp << "//************************************************************************//" << endl;

  m_obj_cpp << endl << endl;

  /* <RawName>_routines.h */

  strcpy( &file[len], "_routines.h" );

  m_routines_h.open(file);
  if (m_routines_h.fail()) {
    X2AC_ErrMsg( X2AC_FILE_OPEN_ERROR, file );
    error = 1;
    return 1;
  }

  m_routines_h << "//************************************************************************//" << endl;
  m_routines_h << "//                                                                          " << endl;
  m_routines_h << "//  This file is generated by tool                                          " << endl;
  m_routines_h << "//                                                                          " << endl;
  m_routines_h << "//                 XML2LcaCode                                              " << endl;
  m_routines_h << "//                                                                          " << endl;
  m_routines_h << "//  from input file " << RawName << "_methods.xml.                          " << endl;
  m_routines_h << "//                                                                          " << endl;
  m_routines_h << "//************************************************************************//" << endl;

  m_routines_h << endl << endl;
  m_routines_h << "#ifndef INC_" << RawBlocker << "_ROUTINES" << endl;
  m_routines_h << "#define INC_" << RawBlocker << "_ROUTINES" << endl;
  m_routines_h << endl << endl;

  /* <RawName>_routines.cpp */

  strcpy( &file[len], "_routines.cpp" );

  m_routines_cpp.open(file);
  if (m_routines_cpp.fail()) {
    X2AC_ErrMsg( X2AC_FILE_OPEN_ERROR, file );
    error = 1;
    return 1;
  }

  m_routines_cpp << "//************************************************************************//" << endl;
  m_routines_cpp << "//                                                                          " << endl;
  m_routines_cpp << "//  This file is generated by tool                                          " << endl;
  m_routines_cpp << "//                                                                          " << endl;
  m_routines_cpp << "//                 XML2LcaCode                                              " << endl;
  m_routines_cpp << "//                                                                          " << endl;
  m_routines_cpp << "//  from input file " << RawName << "_methods.xml.                          " << endl;
  m_routines_cpp << "//                                                                          " << endl;
  m_routines_cpp << "//************************************************************************//" << endl;

  m_routines_cpp << endl << endl;

  /* <RawName>_ifc.h */

  strcpy( &file[len], "_ifc.h" );

  m_ifc_h.open(file);
  if (m_ifc_h.fail()) {
    X2AC_ErrMsg( X2AC_FILE_OPEN_ERROR, file );
    error = 1;
    return 1;
  }

  m_ifc_h << "//************************************************************************//" << endl;
  m_ifc_h << "//                                                                          " << endl;
  m_ifc_h << "//  This file is generated by tool                                          " << endl;
  m_ifc_h << "//                                                                          " << endl;
  m_ifc_h << "//                 XML2LcaCode                                              " << endl;
  m_ifc_h << "//                                                                          " << endl;
  m_ifc_h << "//  from input file " << RawName << "_methods.xml.                          " << endl;
  m_ifc_h << "//                                                                          " << endl;
  m_ifc_h << "//************************************************************************//" << endl;

  m_ifc_h << endl << endl;
  m_ifc_h << "#ifndef INC_" << RawBlocker << "_IFC" << endl;
  m_ifc_h << "#define INC_" << RawBlocker << "_IFC" << endl;
  m_ifc_h << endl << endl;
  m_ifc_h << "#ifndef INC_" << RawBlocker << "_IFC_INC" << endl;
  m_ifc_h << "#include \"";
  Part * pPart = m_pPartList->First();
  while (pPart) {
    m_ifc_h << pPart->getName() << "/";
    pPart = pPart->getNext();
  }
  m_ifc_h << m_pModule << "/";
  m_ifc_h << m_pRawName << "_ifc_inc.h\"" << endl;
  m_ifc_h << "#endif" << endl;
  m_ifc_h << endl << endl;

  /* Ok */

  delete [] file;
  delete [] RawBlocker;

  return 0;

}

/*----------------------------------------------------------------------------*/

void GenApplCode::StartElement( int & error, XML_Parser parser, const char * name, const char ** atts ) {

  if ( error || !m_generate ) {
    return;
  }

  if (X2AC_getElementType(name) == elemInccontent_X2AC) {
    if (!m_Inccontent_spec) {
      Part * pPart = NULL;
      for ( int i = 0; atts[i]; i = i+2 ) {
        switch (X2AC_getAttributeType(atts[i])) {
          case attFunctor_X2AC : {
            m_pFunctor = new char[strlen(atts[i+1])+1];
            strcpy( m_pFunctor, atts[i+1] );
            break;
          }
          case attFunctorheader_X2AC : {
            const char * pBSlash = strrchr( atts[i+1], '/' );
            if (!pBSlash) {
              X2AC_ErrMsg( X2AC_BAD_FUNCTOR_HEADER, atts[i+1] );
              error = 1;
              return;
            }
            int lenBS = strlen(pBSlash+1);
            const char * pDot = strchr( pBSlash, '.' );
            if ( (!pDot) || (pDot == pBSlash+1) || (strchr( pDot+1, '.' )) ) {
              X2AC_ErrMsg( X2AC_BAD_FUNCTOR_HEADER, atts[i+1] );
              error = 1;
              return;
            }
            int lenD = strlen(pDot+1);
            if ( !( (lenD == 1) && (*(pDot+1) == 'h') ) && !(0 == strcmp( pDot+1, "hpp" )) ) {
              X2AC_ErrMsg( X2AC_BAD_FUNCTOR_HEADER, atts[i+1] );
              error = 1;
              return;
            }
            char * pUpper = new char[lenBS-lenD];
            ToUpper( pBSlash+1, pUpper, lenBS-lenD-1 );
            m_obj_h << "#ifndef INC_" << pUpper << endl;
            delete [] pUpper;
            pUpper = NULL;
            m_obj_h << "#include \"" << atts[i+1] << "\"" << endl;
            m_obj_h << "#endif" << endl;
            m_obj_h << endl;
            m_functorHeader = true;
            char * RawBlocker = new char[strlen(m_pRawName)+1];
            ToUpper( m_pRawName, RawBlocker );
            m_obj_h << "#ifndef INC_" << RawBlocker << "_OBJ_INC" << endl;
            m_obj_h << "#include \"";
            pPart = m_pPartList->First();
            while (pPart) {
              m_obj_h << pPart->getName() << "/";
              pPart = pPart->getNext();
            }
            m_obj_h << m_pModule << "/";
            m_obj_h << m_pRawName << "_obj_inc.h\"" << endl;
            m_obj_h << "#endif" << endl;
            m_obj_h << endl << endl;
            delete [] RawBlocker;
            RawBlocker = NULL;
            break;
          }
          case attGlobals_X2AC : {
            m_pGlobals = new char[strlen(atts[i+1])+1];
            strcpy( m_pGlobals, atts[i+1] );
            break;
          }
          case attLibrary_X2AC : {
            const char * pBSlash = strrchr( atts[i+1], '/' );
            if ( (!pBSlash) || (*(pBSlash+1) == 0) ) {
              X2AC_ErrMsg( X2AC_BAD_LIBRARY, atts[i+1] );
              error = 1;
              return;
            }
            m_pLibrary = new char[strlen(pBSlash)];
            strcpy( m_pLibrary, pBSlash+1 );
            m_routines_cpp << "#include \"" << atts[i+1] << ".h\"" << endl;
            m_routines_cpp << "#include \"";
            pPart = m_pPartList->First();
            while (pPart) {
              m_routines_cpp << pPart->getName() << "/";
              pPart = pPart->getNext();
            }
            m_routines_cpp << m_pModule << "/";
            m_routines_cpp << m_pRawName << "_obj.h\"" << endl;
            m_routines_cpp << endl << endl;
            break;
          }
          case attPrecompheader_X2AC : {
            m_obj_cpp << "#include \"" << atts[i+1] << "\"" << endl << endl;
            break;
          }
        }
      }
      if ( (!m_pFunctor) || (!m_functorHeader) || (!m_pGlobals) || (!m_pLibrary) ) {
        X2AC_ErrMsg(X2AC_INCCONTENT_INCOMPLETE);
        error = 1;
        return;
      }
      m_obj_cpp << "#include \"";
      pPart = m_pPartList->First();
      while (pPart) {
        m_obj_cpp << pPart->getName() << "/";
        pPart = pPart->getNext();
      }
      m_obj_cpp << m_pModule << "/";
      m_obj_cpp << m_pRawName << "_obj.h\"" << endl;
      m_obj_cpp << "#include \"";
      pPart = m_pPartList->First();
      while (pPart) {
        m_obj_cpp << pPart->getName() << "/";
        pPart = pPart->getNext();
      }
      m_obj_cpp << m_pModule << "/";
      m_obj_cpp << m_pRawName << "_ifc.h\"" << endl;
      m_obj_cpp << endl << endl;
      m_Inccontent_spec = true;
    }
    else {
      X2AC_ErrMsg(X2AC_INCCONTENT_DUPLICATED);
      error = 1;
      return;
    }
  }
  else if (!m_Inccontent_spec) {
    X2AC_ErrMsg(X2AC_INCCONTENT_NOT_FIRST);
    error = 1;
    return;
  }

  switch (X2AC_getElementType(name)) {

    /*---------- CPP ---------------*/

    case elemCpp_X2AC : {
      break;
    }

    /*---------- IDL ---------------*/

    case elemIdl_X2AC : {

      /* Check attributes */
      if (attName_X2AC != X2AC_getAttributeType(atts[0])) {
        X2AC_ErrMsg( X2AC_BAD_ATTR_SEQUENCE, "IDL", currLineNo(parser) );
        error = 1;
        return;
      }
      if (strlen(atts[1]) > maxNameLen) {
        X2AC_ErrMsg( X2AC_NAME_TOO_LONG, currLineNo(parser), atts[1] );
        error = 1;
        return;
      }
      for ( int i = 2; atts[i]; i = i+2 ) {
        switch (X2AC_getAttributeType(atts[i])) {
          case attGenerate_X2AC : {
            if (0 == strcmp( "NO", atts[i+1] )) {
              m_generate = false;
            }
            else if (0 != strcmp( "YES", atts[i+1] )) {
              X2AC_ErrMsg( X2AC_BAD_ATTR_VALUE, "GENERATE", "IDL", currLineNo(parser) );
              error = 1;
              return;
            }
            break;
          }
          case attSchematype_X2AC : {
            if (NameTooLong(atts[i+1])) {
              X2AC_ErrMsg( X2AC_NAME_TOO_LONG, currLineNo(parser), atts[i+1] );
              error = 1;
              return;
            }
            else {
              strcpy( &m_schemaType[0], atts[i+1] );
              m_SchemaType_spec = true;
            }
            break;
          }
          case attTranstype_X2AC : {
            if (NameTooLong(atts[i+1])) {
              X2AC_ErrMsg( X2AC_NAME_TOO_LONG, currLineNo(parser), atts[i+1] );
              error = 1;
              return;
            }
            else {
              strcpy( &m_transType[0], atts[i+1] );
              m_TransType_spec = true;
            }
            break;
          }
        }
      }
      if ( (!m_SchemaType_spec) || (!m_TransType_spec) ) {
        X2AC_ErrMsg( X2AC_IDL_INCOMPLETE, currLineNo(parser) );
        error = 1;
        return;
      }
      if (!m_generate) {
        return;
      }
      else {
        m_isWithin_Idl = true;
      }

      /* Set method name */
      strcpy( m_methodName, atts[1] );

      /* <RawName>_obj.h */
      m_obj_h << "class ALLAPI_API OF_" << m_methodName << " : public " << m_pFunctor << endl;
      m_obj_h << "{" << endl;
      m_obj_h << "public:" << endl;
      m_obj_h << "  OF_" << m_methodName << "(" << endl;
      //m_obj_h << "    OmsHandle& ah, SqlHandle& asql" << endl; PTS 1138070
      m_obj_h << "      OmsHandle& ah" << endl;
      m_obj_h_privateMembers = "private:";
      m_obj_h_privateMembers.append("\n");

      /* <RawName>_obj.cpp */
      m_obj_cpp << "tInt4 OF_" << m_methodName << "::apiProcess(" << m_pGlobals << "& G)" << endl;
      m_obj_cpp << "{" << endl;
      m_obj_cpp_execute = "  IF_";
      m_obj_cpp_execute.append(m_methodName);
      m_obj_cpp_execute.append("::execute(G, ma");
      m_obj_cpp_execute.append("\n");
      m_obj_cpp_ctorParams = "    OmsHandle& ah";
      m_obj_cpp_ctorParams.append("\n");

      /* <RawName>_routine.h */
      m_routines_h << "STDMETHODIMP " << m_methodName << "(" << endl;

      /* <RawName>_routine.cpp */
      m_routines_cpp << "STDMETHODIMP " << m_pLibrary << "::" << m_methodName << "(" << endl;
      m_routines_cpp_ofArguments = "  OF_";
      m_routines_cpp_ofArguments.append(m_methodName);
      m_routines_cpp_ofArguments.append(" of_");
      m_routines_cpp_ofArguments.append(m_methodName);
      m_routines_cpp_ofArguments.append("(*this");
      m_routines_cpp_ofArguments.append("\n");

      /* <RawName>_ifc.h */
      m_ifc_h << "class IF_" << m_methodName << endl;
      m_ifc_h << "{" << endl;
      m_ifc_h << "public:" << endl;
      m_ifc_h << "  static void execute(" << m_pGlobals << "& G, tMemIfc& ma" << endl;

      /* Done */
      break;

    }

    /*---------- PARAM -------------*/

    case elemParam_X2AC : {

      if (!m_isWithin_Idl) {
        X2AC_ErrMsg( X2AC_BAD_TAG_USE, "PARAM" );
        error = 1;
        return;
      }

      /* Check attributes */
      int nIdx = -1, dIdx = -1, tIdx = -1, uIdx = -1;
      for ( int i = 0; atts[i]; i = i+2 ) {
        switch (X2AC_getAttributeType(atts[i])) {
          case attName_X2AC : {
            nIdx = i;
            break;
          }
          case attDirection_X2AC : {
            dIdx = i;
            break;
          }
          case attType_X2AC : {
            tIdx = i;
            break;
          }
          case attUnknown_X2AC : {
            uIdx = i;
            break;
          }
        }
      }
      if (!( (nIdx == 0) && (dIdx == 2) && (tIdx == 4) && (uIdx == -1) )) {
        X2AC_ErrMsg( X2AC_BAD_ATTR_SEQUENCE, "PARAM", currLineNo(parser) );
        error = 1;
        return;
      }
      if ( (0 != strcmp( atts[3], "in" )) && (0 != strcmp( atts[3], "out" )) ) {
        X2AC_ErrMsg( X2AC_BAD_ATTR_VALUE, "DIRECTION", "PARAM", currLineNo(parser) );
        error = 1;
        return;
      }
      bool isIN = (atts[3][0] == 'i');
      int typeLen = strlen(atts[5]);
      if (typeLen > maxNameLen) {
        X2AC_ErrMsg( X2AC_BAD_ATTR_VALUE, "TYPE", "PARAM", currLineNo(parser) );
        error = 1;
        return;
      }
      bool isPtr = IsPointer(atts[5]);
      bool isTH  = IsTabhandle(atts[5]);
      if (isTH) {
        strncpy( m_tableType, atts[5], typeLen-10 );
        m_tableType[typeLen-10] = 0;
      }
      m_paramCntr++;
      if (m_paramCntr == 1) {
        if ( (strlen(atts[1]) > maxNameLen) || (!isIN) || (!isPtr) || (isTH) ) {
          X2AC_ErrMsg( X2AC_BAD_PARAM, "1st", currLineNo(parser), atts[1] );
          error = 1;
          return;
        }
        else {
          strcpy( m_param1, atts[1] );
        }
      }
      else if (m_paramCntr == 2) {
        if ( (strlen(atts[1]) > maxNameLen) || (isIN) || (!isPtr) || (isTH) ) {
          X2AC_ErrMsg( X2AC_BAD_PARAM, "2nd", currLineNo(parser), atts[1] );
          error = 1;
          return;
        }
        else {
          strcpy( m_param2, atts[1] );
        }
      }

      /* <RawName>_obj.h */
      m_obj_h << "    , ";
      m_obj_h_privateMembers.append("  ");
      if ( (isIN) && (!isPtr) ) {
        m_obj_h << "const " << atts[5] << "& a" << atts[1] << endl;
        m_obj_h_privateMembers.append("const ");
        m_obj_h_privateMembers.append(atts[5]);
        m_obj_h_privateMembers.append("& ");
        m_obj_h_privateMembers.append(atts[1]);
        m_obj_h_privateMembers.append(";");
        m_obj_h_privateMembers.append("\n");
      }
      else {
        m_obj_h << atts[5] << " a" << atts[1] << endl;
        m_obj_h_privateMembers.append(atts[5]);
        m_obj_h_privateMembers.append(" ");
        m_obj_h_privateMembers.append(atts[1]);
        m_obj_h_privateMembers.append(";");
        m_obj_h_privateMembers.append("\n");
      }

      /* <RawName>_obj.cpp */
      if (isTH) {
        m_obj_cpp << "  " << m_tableType;
        if (isIN) {
          m_obj_cpp << "_IStrm ten";
        }
        else {
          m_obj_cpp << "_OStrm ta";
        }
        m_obj_cpp << &atts[1][1] << "(G, *" << atts[1] << ", \"" << atts[1];
        m_obj_cpp << " stream:\", \"" << atts[1] << " item:\");" << endl;
      }
      if (m_paramCntr >= 3) {
        if (m_obj_cpp_lineLen + 2 + strlen(atts[1]) + 2 > 75) { 
          m_obj_cpp_execute.append("\n");
          m_obj_cpp_lineLen = 0;
        }
        if (m_obj_cpp_lineLen == 0) {
          m_obj_cpp_execute.append("    ");
        }
        m_obj_cpp_execute.append(", ");
        if (isTH) {
          if (isIN) {
            m_obj_cpp_execute.append("ten");
          }
          else {
            m_obj_cpp_execute.append("ta");
          }
          m_obj_cpp_execute.append(&atts[1][1]);
        }
        else {
          m_obj_cpp_execute.append(atts[1]);
        }
        m_obj_cpp_lineLen += (2 + strlen(atts[1]) + 2);
      }
      m_obj_cpp_ctorParams.append("  , ");
      if ( (isIN) && (!isPtr) ) {
        m_obj_cpp_ctorParams.append("const ");
        m_obj_cpp_ctorParams.append(atts[5]);
        m_obj_cpp_ctorParams.append("& a");
        m_obj_cpp_ctorParams.append(atts[1]);
      }
      else {
        m_obj_cpp_ctorParams.append(atts[5]);
        m_obj_cpp_ctorParams.append(" a");
        m_obj_cpp_ctorParams.append(atts[1]);
      }
      m_obj_cpp_ctorParams.append("\n");
      if (m_paramCntr == 1) {
        m_obj_cpp_ctorInitList = ", ";
      }
      else {
        m_obj_cpp_ctorInitList.append(", ");
      }
      m_obj_cpp_ctorInitList.append(atts[1]);
      m_obj_cpp_ctorInitList.append("(a");
      m_obj_cpp_ctorInitList.append(atts[1]);
      m_obj_cpp_ctorInitList.append(")");
      m_obj_cpp_ctorInitList.append("\n");

      /* <RawNamw>_routines.h and <RawNamw>_routines.cpp */
      if (m_paramCntr == 1) {
        m_routines_parameters = "    ";
      }
      else {
        m_routines_parameters.append("  , ");
      }
      m_routines_parameters.append("/*[");
      m_routines_parameters.append(atts[3]);
      m_routines_parameters.append("]*/");
      if (isIN) {
        m_routines_parameters.append("  ");
      }
      else {
        m_routines_parameters.append(" ");
      }
      m_routines_parameters.append(atts[5]);
      m_routines_parameters.append(" ");
      m_routines_parameters.append(atts[1]);
      m_routines_parameters.append("\n");

      /* <RawNamw>_routines.cpp only */
      if (m_routines_cpp_lineLen + 2 + strlen(atts[1]) > 75) {
        m_routines_cpp_ofArguments.append("\n");
        m_routines_cpp_lineLen = 0;
      }
      if (m_routines_cpp_lineLen == 0) {
        m_routines_cpp_ofArguments.append("    ");
      }
      m_routines_cpp_ofArguments.append(", ");
      m_routines_cpp_ofArguments.append(atts[1]);
      m_routines_cpp_lineLen += (2 + strlen(atts[1]));

      /* <RawName>_ifc.h */
      if (m_paramCntr >= 3) {
        if (isTH) {
          if (isIN) {
            m_ifc_h << "    , tResettablePipe<" << m_tableType << ">& ten" << &atts[1][1] << endl;
          }
          else {
            m_ifc_h << "    , tResettableAppender<" << m_tableType << ">& ta" << &atts[1][1] << endl;
          }
        }
        else if ( (isIN) && (!isPtr) ) {
          m_ifc_h << "    , const " << atts[5] << "& " << atts[1] << endl;
        }
        else {
          m_ifc_h << "    , " << atts[5] << " " << atts[1] << endl;
        }
      }

      /* Done */
      break;

    }

    /*---------- UNKNOWN -----------*/

    case elemUnknown_X2AC : {
      X2AC_ErrMsg( X2AC_BAD_TAG, name );
      error = 1;
      return;
    }

  }

}

/*----------------------------------------------------------------------------*/

void GenApplCode::EndElement( int & error, const char * name ) {

  if (error) {
    return;
  }

  if (X2AC_getElementType(name) == elemIdl_X2AC) {

    if (m_generate) {

      /* <RawName>_obj.h */
      m_obj_h << "  );" << endl;
      m_obj_h << endl;
      m_obj_h << "protected:" << endl;
      m_obj_h << "  tInt4 apiProcess(" << m_pGlobals << "&);" << endl;
      m_obj_h << endl;
      m_obj_h << m_obj_h_privateMembers.data();
      m_obj_h_privateMembers.erase();
      m_obj_h << "};" << endl;
      m_obj_h << endl << endl;

      /* <RawName>_obj.cpp */
      m_obj_cpp << endl;
      m_obj_cpp << m_obj_cpp_execute.data();
      m_obj_cpp_execute.erase();
      m_obj_cpp << endl;
      m_obj_cpp << "  );" << endl;
      m_obj_cpp << "  return 0;" << endl;
      m_obj_cpp << "}" << endl;
      m_obj_cpp << endl;
      m_obj_cpp << "OF_" << m_methodName << "::OF_" << m_methodName << "(" << endl;
      m_obj_cpp << m_obj_cpp_ctorParams.data();
      m_obj_cpp_ctorParams.erase();
      m_obj_cpp << ")" << endl;
      m_obj_cpp << ": " << m_pFunctor << "(ah, \"" << m_methodName << "\", " << m_transType << ", ";
      m_obj_cpp << m_schemaType << "," << endl;
      m_obj_cpp <<"    CbsUserData(*a" << m_param1 << "), a" << m_param2 << ", NULL)" << endl;
      m_obj_cpp << m_obj_cpp_ctorInitList.data();
      m_obj_cpp_ctorInitList.erase();
      m_obj_cpp << "{}" << endl;
      m_obj_cpp << endl << endl;

      /* <RawName>_routines.h */
      m_routines_h << m_routines_parameters.data();
      m_routines_h << ");" << endl;
      m_routines_h << endl << endl;

      /* <RawName>_routines.cpp */
      m_routines_cpp << m_routines_parameters.data();
      m_routines_parameters.erase();
      m_routines_cpp << ")" << endl;
      m_routines_cpp << "{" << endl;
      m_routines_cpp_ofArguments.append("\n");
      m_routines_cpp << m_routines_cpp_ofArguments.data();
      m_routines_cpp_ofArguments.erase();
      m_routines_cpp << "  );" << endl;
      m_routines_cpp << "  of_" << m_methodName << ".apiExecute();" << endl;
      m_routines_cpp << "  return 0;" << endl;
      m_routines_cpp <<"}" << endl;
      m_routines_cpp << endl << endl;

      /* <RawName>_ifc.h */
      m_ifc_h << "  );" << endl;
      m_ifc_h << endl;
      m_ifc_h << "  static tInt getTransactionType()" << endl;
      m_ifc_h << "  {" << endl;
      m_ifc_h << "    return " << m_transType << ";" << endl;
      m_ifc_h << "  }" << endl;
      m_ifc_h << endl;
      m_ifc_h << "  static tInt getSchemaType()" << endl;
      m_ifc_h << "  {" << endl;
      m_ifc_h << "    return " << m_schemaType << ";" << endl;
      m_ifc_h << "  }" << endl;
      m_ifc_h << "};" << endl;
      m_ifc_h << endl << endl;

    }

    m_generate = true;
    m_SchemaType_spec = false;
    m_TransType_spec = false;
    m_paramCntr = 0;
    m_obj_cpp_lineLen = 0;
    m_routines_cpp_lineLen = 0;

    m_isWithin_Idl = false;

  }

}

