/*!*****************************************************************************

  module      : XML2DiscCpp.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of persistent class description code
  description : Main program

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
#include <ctype.h>

#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#include <fstream>
#define endl std::endl
#define cout std::cout
#else
#include <fstream.h>
#include <iostream.h>
#endif

#ifdef WIN32
#define  STRICMP stricmp
#else
#include <strings.h>
#define  STRICMP strcasecmp
#endif


#include "idl/XML2DiscCpp/XML2DiscCpp.hpp"
#include "idl/expat/expat.h"
#include "idl/XML2DiscCpp/X2DC_Attribute_Checker.hpp"
#include "idl/XML2DiscCpp/X2DC_Cpp_Type_Registry.hpp"
#include "idl/XML2DiscCpp/X2DC_GenApplHeaderFile.hpp"
#include "idl/XML2DiscCpp/X2DC_GenDisclosureCode.hpp"


const int maxFileNameLength = 255;
typedef char FileName[maxFileNameLength+1];

const int XML_BufferSize = 4096;

/*----------------------------------------------------------------------------*/

typedef struct X2DC_UserData {
  inline X2DC_UserData();
  Attribute_Checker m_Attribute_Checker;
  Cpp_Type_Registry m_Cpp_Type_Registry;
  GenApplHeaderFile m_genApplHeaderFile;
  GenDisclosureCode m_genDisclosureCode;
  int               m_error;
  XML_Parser        m_parser;
} X2DC_UserData;

inline X2DC_UserData::X2DC_UserData()
: m_Attribute_Checker(&m_error)
, m_Cpp_Type_Registry(&m_error)
, m_genApplHeaderFile( &m_error, &m_Cpp_Type_Registry )
, m_genDisclosureCode( &m_error, &m_Cpp_Type_Registry )
, m_error(0)
{}

/*----------------------------------------------------------------------------*/

XmlAttributeType_X2DC X2DC_getAttributeType(const char * attribute) {

  if (0 == strcmp( attribute, "ACCESSTYPE" )   ) return attAccesstype_X2DC;
  if (0 == strcmp( attribute, "BASECLASS" )    ) return attBaseclass_X2DC;
  if (0 == strcmp( attribute, "BITCOUNT" )     ) return attBitcount_X2DC;
  if (0 == strcmp( attribute, "CHAR" )         ) return attChar_X2DC;
  if (0 == strcmp( attribute, "CONST" )        ) return attConst_X2DC;
  if (0 == strcmp( attribute, "CONSTPTR" )     ) return attConstptr_X2DC;
  if (0 == strcmp( attribute, "CONSTVAL" )     ) return attConstval_X2DC;
  if (0 == strcmp( attribute, "DIMENSION"  )   ) return attDimension_X2DC;
  if (0 == strcmp( attribute, "EXPRESSION" )   ) return attExpression_X2DC;
  if (0 == strcmp( attribute, "EXTENSION" )    ) return attExtension_X2DC;
  if (0 == strcmp( attribute, "FILE" )         ) return attFile_X2DC;
  if (0 == strcmp( attribute, "GENDEFCONSTR" ) ) return attGendefconstr_X2DC;
  if (0 == strcmp( attribute, "GENTABHANDLE" ) ) return attGentabhandle_X2DC;
  if (0 == strcmp( attribute, "ID" )           ) return attId_X2DC;
  if (0 == strcmp( attribute, "IDENTIFIER" )   ) return attIdentifier_X2DC;
  if (0 == strcmp( attribute, "IDENUM" )       ) return attIdenum_X2DC;
  if (0 == strcmp( attribute, "INTSYNONYM" )   ) return attIntsynonym_X2DC;
  if (0 == strcmp( attribute, "KEYCLASS" )     ) return attKeyclass_X2DC;
  if (0 == strcmp( attribute, "NAME" )         ) return attName_X2DC;
  if (0 == strcmp( attribute, "NUMBER" )       ) return attNumber_X2DC;
  if (0 == strcmp( attribute, "PACK" )         ) return attPack_X2DC;
  if (0 == strcmp( attribute, "PERSBASECLASS" )) return attPersbaseclass_X2DC;
  if (0 == strcmp( attribute, "SIZE" )         ) return attSize_X2DC;
  if (0 == strcmp( attribute, "STATIC" )       ) return attStatic_X2DC;
  if (0 == strcmp( attribute, "STRING" )       ) return attString_X2DC;
  if (0 == strcmp( attribute, "TEMPL" )        ) return attTempl_X2DC;
  if (0 == strcmp( attribute, "TEMPLATE" )     ) return attTemplate_X2DC;
  if (0 == strcmp( attribute, "TYPE" )         ) return attType_X2DC;
  if (0 == strcmp( attribute, "UUID" )         ) return attUuid_X2DC;
  if (0 == strcmp( attribute, "VALUE" )        ) return attValue_X2DC;

  return attUnknown_X2DC;

}

/*----------------------------------------------------------------------------*/

XmlElementType_X2DC X2DC_getElementType(const char * element) {

  if (0 == strcmp( element, "ADD" )            ) return elemAdd_X2DC;
  if (0 == strcmp( element, "ALIGNMENTMEMBER" )) return elemAlignmentmember_X2DC;
  if (0 == strcmp( element, "BITFIELDS" )      ) return elemBitfields_X2DC;
  if (0 == strcmp( element, "CLASS" )          ) return elemClass_X2DC;
  if (0 == strcmp( element, "CLASSES" )        ) return elemClasses_X2DC;
  if (0 == strcmp( element, "CLASSINSTANCE" )  ) return elemClassinstance_X2DC;
  if (0 == strcmp( element, "CLASSMEMBER" )    ) return elemClassmember_X2DC;
  if (0 == strcmp( element, "CLASSPARAM" )     ) return elemClassparam_X2DC;
  if (0 == strcmp( element, "CONST" )          ) return elemConst_X2DC;
  if (0 == strcmp( element, "CPP" )            ) return elemCpp_X2DC;
  if (0 == strcmp( element, "DEFINE" )         ) return elemDefine_X2DC;
  if (0 == strcmp( element, "DERIVEFROM" )     ) return elemDerivefrom_X2DC;
  if (0 == strcmp( element, "DIV" )            ) return elemDiv_X2DC;
  if (0 == strcmp( element, "ENUM" )           ) return elemEnum_X2DC;
  if (0 == strcmp( element, "EXPR" )           ) return elemExpr_X2DC;
  if (0 == strcmp( element, "EXTOIDMEMBER" )   ) return elemExtoidmember_X2DC;
  if (0 == strcmp( element, "FIELD" )          ) return elemField_X2DC;
  if (0 == strcmp( element, "FIXED" )          ) return elemFixed_X2DC;
  if (0 == strcmp( element, "FRIEND" )         ) return elemFriend_X2DC;
  if (0 == strcmp( element, "GENERALENUM" )    ) return elemGeneralenum_X2DC;
  if (0 == strcmp( element, "IDENT" )          ) return elemIdent_X2DC;
  if (0 == strcmp( element, "IMPORT" )         ) return elemImport_X2DC;
  if (0 == strcmp( element, "INDEX" )          ) return elemIndex_X2DC;
  if (0 == strcmp( element, "INSTANTIATE" )    ) return elemInstantiate_X2DC;
  if (0 == strcmp( element, "INTINSTANCE" )    ) return elemIntinstance_X2DC;
  if (0 == strcmp( element, "INTPARAM" )       ) return elemIntparam_X2DC;
  if (0 == strcmp( element, "INTRINSICMEMBER" )) return elemIntrinsicmember_X2DC;
  if (0 == strcmp( element, "KEY" )            ) return elemKey_X2DC;
  if (0 == strcmp( element, "MEMBER" )         ) return elemMember_X2DC;
  if (0 == strcmp( element, "MUL" )            ) return elemMul_X2DC;
  if (0 == strcmp( element, "OIDMEMBER" )      ) return elemOidmember_X2DC;
  if (0 == strcmp( element, "PERSCLASS" )      ) return elemPersclass_X2DC;
  if (0 == strcmp( element, "PERSINTERFACE" )  ) return elemPersinterface_X2DC;
  if (0 == strcmp( element, "PERSKEYEDCLASS" ) ) return elemPerskeyedclass_X2DC;
  if (0 == strcmp( element, "PERSTEMPLCLASS" ) ) return elemPerstemplclass_X2DC;
  if (0 == strcmp( element, "PRAGMA" )         ) return elemPragma_X2DC;
  if (0 == strcmp( element, "PTRMEMBER" )      ) return elemPtrmember_X2DC;
  if (0 == strcmp( element, "SECTION" )        ) return elemSection_X2DC;
  if (0 == strcmp( element, "SIZE" )           ) return elemSize_X2DC;
  if (0 == strcmp( element, "SIZEOF" )         ) return elemSizeof_X2DC;
  if (0 == strcmp( element, "STRUCT" )         ) return elemStruct_X2DC;
  if (0 == strcmp( element, "SUB" )            ) return elemSub_X2DC;
  if (0 == strcmp( element, "TEMPLCLASS" )     ) return elemTemplclass_X2DC;
  if (0 == strcmp( element, "TEMPLMEMBER" )    ) return elemTemplmember_X2DC;
  if (0 == strcmp( element, "TEMPLPARAMS" )    ) return elemTemplparams_X2DC;
  if (0 == strcmp( element, "TYPEDEF" )        ) return elemTypedef_X2DC;
  if (0 == strcmp( element, "UNION" )          ) return elemUnion_X2DC;
  if (0 == strcmp( element, "VAROIDMEMBER" )   ) return elemVaroidmember_X2DC;

  return elemUnknown_X2DC;

}

/*----------------------------------------------------------------------------*/

bool X2DC_isExprParam(const char * element) {

  if ( (elemFixed_X2DC == X2DC_getElementType(element)) ||
       (elemIdent_X2DC == X2DC_getElementType(element)) ) {
    return true;
  }
  else {
    return false;
  }

}

/*----------------------------------------------------------------------------*/

static int AliOfOid = 8;

int AlignmentOfOid () {
  return AliOfOid;
}

/*----------------------------------------------------------------------------*/

int processCall( int argC, char * argV[], FileName InputXMLFile, FileName RawName,
                 bool & debug, bool & warnings,
                 bool & make76, bool & offsetcheck, bool & oid4bytealigned, bool & lcapps ) {

  int rc = 0;
  int len;

  if (argC < 2) {
    X2DC_ErrMsg( M__TOO_FEW_INPUT_PARAMS, false );
    rc = 1;
    return rc;
  }

  /* First input parameter must be file name of form -F<RawName>__mgd.xml. */

  len = strlen(argV[1]);

  if ( (len < 12) || (len > 2+maxFileNameLength) ) {
    rc = 1;
  }

  else if (strncmp( &argV[1][0], "-F", 2 )) {
    rc = 1;
  }

  else if ( !(argV[1][len-4] == '.') ||
            !(argV[1][len-3] == 'x') ||
            !(argV[1][len-2] == 'm') ||
            !(argV[1][len-1] == 'l') ) {
    rc = 1;
  }

  else if (strncmp( &argV[1][len-9], "__mgd", 5)) {
    rc = 1;
  }

  if (rc) {
    X2DC_ErrMsg( M__BAD_FIRST_INPUT_PARAM, maxFileNameLength-9, false );
    return rc;
  }

  /* File name ok, copy name and part relevant for output files */

  strncpy( &InputXMLFile[0], &argV[1][2], len-2 );
  InputXMLFile[len-2] = 0;

  strncpy( &RawName[0], &argV[1][2], len-2-9 );
  RawName[len-2-9] = 0;

  /* Further input parameters */

  for ( int i = 2; i < argC; i++ ) {
    if (argV[i][0] == '-') {
      if (0 == strcmp( &argV[i][1], "DBG" )) {
        debug = true;
      }
      else if (0 == strcmp( &argV[i][1], "W" )) {
        warnings = true;
      }
      else if (0 == strcmp( &argV[i][1], "M76" )) {
        make76 = true;
      }
      else if (0 == strcmp( &argV[i][1], "OSC" )) {
        if (make76) {
          offsetcheck = true;
        }
        else {
          X2DC_ErrMsg( M__ONLY_WITH_M76, "OSC", false );
          rc = 1;
          return rc;
        }
      }
      else if (0 == strcmp( &argV[i][1], "OID4BA" )) {
        if (make76) {
          oid4bytealigned = true;
        }
        else {
          X2DC_ErrMsg( M__ONLY_WITH_M76, "OID4BA", false );
          rc = 1;
          return rc;
        }
      }
      else if (0 == strcmp( &argV[i][1], "LCA" )) {
        lcapps = true;
      }
    }
  }

  /* Done */

  return rc;

}

/*----------------------------------------------------------------------------*/

extern "C" void X2DC_CharData( void * userData, const char * s, int len ) {

  X2DC_UserData * pUserData = (X2DC_UserData*)userData;

  if (pUserData->m_error) {
    return;
  }

  pUserData->m_Cpp_Type_Registry.CharData( pUserData->m_error, s, len );

}

/*----------------------------------------------------------------------------*/

extern "C" void X2DC_StartElement( void * userData, const char * name, const char ** atts) {

  X2DC_UserData * pUserData = (X2DC_UserData*)userData;

  if (X2DC_isExprParam(name)) {
    XML_SetCharacterDataHandler( pUserData->m_parser, &X2DC_CharData );
  }

  pUserData->m_Attribute_Checker.StartElement( pUserData->m_error, name, atts );

  if (pUserData->m_error) {
    return;
  }

  pUserData->m_Cpp_Type_Registry.StartElement( pUserData->m_error, name, atts );

  if (pUserData->m_error) {
    return;
  }

  pUserData->m_genApplHeaderFile.StartElement( pUserData->m_error, name, atts );

  if (pUserData->m_error) {
    return;
  }

  pUserData->m_genDisclosureCode.StartElement( pUserData->m_error, name, atts );

}

/*----------------------------------------------------------------------------*/

extern "C" void X2DC_EndElement( void * userData, const char * name ) {

  X2DC_UserData * pUserData = (X2DC_UserData*)userData;

  pUserData->m_Attribute_Checker.EndElement( name );

  if (pUserData->m_error) {
    goto reset_chardata_handler;
  }

  pUserData->m_Cpp_Type_Registry.EndElement( pUserData->m_error, name );

  if (pUserData->m_error) {
    goto reset_chardata_handler;
  }

  pUserData->m_genApplHeaderFile.EndElement( name );

  pUserData->m_genDisclosureCode.EndElement( pUserData->m_error, name );

  reset_chardata_handler:
  if (X2DC_isExprParam(name)) {
    XML_SetCharacterDataHandler( pUserData->m_parser, NULL );
  }

}

/*----------------------------------------------------------------------------*/

# if defined(_WIN32) && !defined(BIT64)
    extern "C" __stdcall DebugBreak();
# endif

/*----------------------------------------------------------------------------*/

int main( int argC , char * argV[] ) {

  FileName      InputXMLFile;
  FileName      RawName;
  bool          debug           = false;
  bool          warnings        = false;
  bool          make76          = false;
  bool          offsetcheck     = false;
  bool          oid4bytealigned = false;
  bool          lcapps          = false;
  FILE *        fpInputXML;
  X2DC_UserData userData;
  char          XML_Buffer[XML_BufferSize];
  int           rc = 0;
  int           done;

  /* Process call of program */

  if (processCall( argC, argV, InputXMLFile, RawName, debug, warnings,
                   make76, offsetcheck, oid4bytealigned, lcapps )) {
    rc = 1;
    return rc;
  }

  /* Debugging? */

# if defined(_WIN32) && !defined(BIT64)
    if (debug) {
      DebugBreak();
    }
# endif

  /* Alignment of OID */

  if (oid4bytealigned) {
    AliOfOid = 4;
  }

  /* Open input XML file */

  fpInputXML = fopen( InputXMLFile, "r" );
  if (NULL == fpInputXML) {
    X2DC_ErrMsg( M__FILE_OPEN_ERROR, InputXMLFile, false );
    rc = 1;
    return rc;
  }

  /* Initialize C++ type registry */

  rc = userData.m_Cpp_Type_Registry.Init();
  if (rc) {
    return rc;
  }

  /* Initialize application header file */

  rc = userData.m_genApplHeaderFile.Init( userData.m_error, RawName, make76, lcapps );
  if (rc) {
    return rc;
  }

  /* Initialize disclosure code */

  rc = userData.m_genDisclosureCode.Init( userData.m_error, RawName, make76, offsetcheck );
  if (rc) {
    return rc;
  }

  /* Create and initialize XML parser */

  userData.m_parser = XML_ParserCreate(NULL);
  XML_SetUserData( userData.m_parser, &userData );
  XML_SetElementHandler( userData.m_parser, &X2DC_StartElement, &X2DC_EndElement );

  /* Initialize error and warning handler */

  SetParser(&userData.m_parser);
  if (warnings) {
    ForceWarnings();
  }

  /* Parse XML file */

  do {
    size_t len = fread( XML_Buffer, 1, XML_BufferSize, fpInputXML);
    done = len < XML_BufferSize;
    if (!XML_Parse( userData.m_parser, XML_Buffer, len, done )) {
      X2DC_ErrMsg( M__XML_PARSE_ERROR, XML_ErrorString(XML_GetErrorCode(userData.m_parser)) );
      rc = 1;
      return rc;
    }
  } while (!done);

  /* Free XML parser */

  XML_ParserFree(userData.m_parser);

  /* Return */

  if (userData.m_error) {
    rc = 1;
  }
  return rc;

}

