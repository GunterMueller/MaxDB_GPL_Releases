/*!*****************************************************************************

  module      : XML2LcaCode.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of application code ("COM routine wrappers")
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


#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <fstream>
#else
#include <fstream.h>
#include <iostream.h>
#endif

#include "idl/XML2LcaCode/XML2LcaCode.hpp"
#include "idl/XML2LcaCode/X2AC_GenApplCode.hpp"
#include "idl/XML2LcaCode/X2AC_WarnAndErrHandler.hpp"


const int XML_BufferSize = 4096;

typedef struct X2AC_UserData {
  GenApplCode m_genApplCode;
  int         m_error;
  XML_Parser  m_parser;
} X2AC_UserData;

/*----------------------------------------------------------------------------*/

XmlAttributeType_X2AC X2AC_getAttributeType(const char * attribute) {

  if (0 == strcmp( attribute, "DIRECTION" )    ) return attDirection_X2AC;
  if (0 == strcmp( attribute, "FUNCTOR" )      ) return attFunctor_X2AC;
  if (0 == strcmp( attribute, "FUNCTORHEADER" )) return attFunctorheader_X2AC;
  if (0 == strcmp( attribute, "GENERATE" )     ) return attGenerate_X2AC;
  if (0 == strcmp( attribute, "GLOBALS" )      ) return attGlobals_X2AC;
  if (0 == strcmp( attribute, "LIBRARY" )      ) return attLibrary_X2AC;
  if (0 == strcmp( attribute, "NAME" )         ) return attName_X2AC;
  if (0 == strcmp( attribute, "PRECOMPHEADER" )) return attPrecompheader_X2AC;
  if (0 == strcmp( attribute, "SCHEMATYPE" )   ) return attSchematype_X2AC;
  if (0 == strcmp( attribute, "TRANSTYPE" )    ) return attTranstype_X2AC;
  if (0 == strcmp( attribute, "TYPE" )         ) return attType_X2AC;

  return attUnknown_X2AC;

}

/*----------------------------------------------------------------------------*/

XmlElementType_X2AC X2AC_getElementType(const char * element) {

  if (0 == strcmp( element, "CPP" )       ) return elemCpp_X2AC;
  if (0 == strcmp( element, "IDL" )       ) return elemIdl_X2AC;
  if (0 == strcmp( element, "INCCONTENT" )) return elemInccontent_X2AC;
  if (0 == strcmp( element, "PARAM" )     ) return elemParam_X2AC;

  return elemUnknown_X2AC;

}

/*----------------------------------------------------------------------------*/

int currLineNo(XML_Parser parser) {
  return XML_GetCurrentLineNumber(parser);
}

/*----------------------------------------------------------------------------*/

int processCall( int argC, char * argV[],
                 FileName InputXMLFile, FileName RawName,
                 List * PartList, FileName Module ) {

  int rc = 0;

  // Input parameter must be file name of form: -F<RawName>_methods.xml
  // RawName must be of form <Part_1>_..._<Part_n>__<Module>.
  // Files to generate: <RawName>_obj.h
  //                    <RawName>_obj.cpp
  //                    <RawName>_routines.h
  //                    <RawName>_routines.cpp
  //                    <RawName>_ifc.h

  if (argC != 2) {
    X2AC_ErrMsg(X2AC_BAD_INPUT_PARAM_NO);
    rc = 1;
    return rc;
  }

  /* Basic file name requirements */
  int nameLen = strlen(&argV[1][0]) - 2;
  if (nameLen > maxFileNameLength-1) {
    X2AC_ErrMsg(X2AC_INPUT_FILE_NAME_TOO_LONG);
    rc = 1;
    return rc;
  }
  if ( (nameLen < 16)                         || // 16 == strlen("x__y_methods.xml")
       (0 != strncmp( &argV[1][0], "-F", 2 )) ||
       ('_' == argV[1][2])                    || // file name may not start with '_'
       (0 != strncmp( &argV[1][2+nameLen-12], "_methods.xml", 12 )) ) {
    X2AC_ErrMsg(X2AC_BAD_INPUT_FILE_NAME);
    rc = 1;
    return rc;
  }

  /* Copy file name and raw name */
  strcpy( &InputXMLFile[0], &argV[1][2] );
  strncpy( &RawName[0], &argV[1][2], nameLen-12 );
  RawName[nameLen-12] = 0;
  FileName Helper;
  strcpy( &Helper[0], RawName );

  /* Get module name */
  char * pDUS = strstr( Helper, "__" );
  if (!pDUS) {
    X2AC_ErrMsg(X2AC_BAD_INPUT_FILE_NAME);
    rc = 1;
    return rc;
  }
  if (strstr( pDUS+1, "__" )) {
    X2AC_ErrMsg(X2AC_BAD_INPUT_FILE_NAME);
    rc = 1;
    return rc;
  }
  if (strchr( pDUS+2, '_' )) {
    X2AC_ErrMsg(X2AC_BAD_INPUT_FILE_NAME);
    rc = 1;
    return rc;
  }
  strcpy( &Module[0], pDUS+2 );
  *pDUS = 0;

  /* Get part(s) */
  char * pUS = strrchr( Helper, '_' );
  while (pUS) {
    PartList->Unshift(pUS+1);
    *pUS = 0;
    pUS = strrchr( Helper, '_' );
  }
  PartList->Unshift(&Helper[0]);

  /* Done */
  return rc;

}

/*----------------------------------------------------------------------------*/

extern "C" void X2AC_StartElement( void * userData, const char * name, const char ** atts) {
  X2AC_UserData * pUserData = (X2AC_UserData*)userData;
  pUserData->m_genApplCode.StartElement( pUserData->m_error, pUserData->m_parser, name, atts );
}

/*----------------------------------------------------------------------------*/

extern "C" void X2AC_EndElement( void * userData, const char * name ) {
  X2AC_UserData * pUserData = (X2AC_UserData*)userData;
  pUserData->m_genApplCode.EndElement( pUserData->m_error, name );
}

/*----------------------------------------------------------------------------*/

//extern "C" __stdcall DebugBreak();

int main( int argC , char * argV[] ) {

  FileName      InputXMLFile;
  FileName      RawName;
  List *        PartList = new List;
  FileName      Module;
  FILE *        fpInputXML;
  X2AC_UserData userData;
  char          XML_Buffer[XML_BufferSize];
  int           rc = 0;
  int           done;

//  DebugBreak();

  /* Process call of program */
  if (processCall( argC, argV, InputXMLFile, RawName, PartList, Module )) {
    rc = 1;
    return rc;
  }

  /* Open input XML file */
  fpInputXML = fopen( InputXMLFile, "r" );
  if (NULL == fpInputXML) {
    X2AC_ErrMsg( X2AC_FILE_OPEN_ERROR, InputXMLFile );
    rc = 1;
    return rc;
  }

  /* Initialize application file */
  rc = userData.m_genApplCode.Init( userData.m_error, RawName, PartList, Module );
  if (rc) {
    return rc;
  }

  /* Initialize error, PTS 1138264 */
  userData.m_error = 0;

  /* create and initialize XML parser */
  userData.m_parser = XML_ParserCreate(NULL);
  XML_SetUserData( userData.m_parser, &userData );
  XML_SetElementHandler( userData.m_parser, &X2AC_StartElement, &X2AC_EndElement );

  /* Parse XML file */
  do {
    size_t len = fread( XML_Buffer, 1, XML_BufferSize, fpInputXML );
    done = len < XML_BufferSize;
    if (!XML_Parse( userData.m_parser, XML_Buffer, len, done )) {
      X2AC_ErrMsg( X2AC_XML_PARSE_ERROR,
                   XML_ErrorString(XML_GetErrorCode(userData.m_parser)), 
                   XML_GetCurrentLineNumber(userData.m_parser) );
      rc = 1;
      return rc;
    }
  } while (!done);

  /* Clean up */
  XML_ParserFree(userData.m_parser);
  if (PartList) {
    delete PartList;
    PartList = NULL;
  }

  /* Done */
  if (userData.m_error) {
    rc = 1;
  }
  return rc;

}

