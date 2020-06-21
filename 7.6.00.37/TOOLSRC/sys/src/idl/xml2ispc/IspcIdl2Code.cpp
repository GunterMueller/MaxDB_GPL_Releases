/*!*****************************************************************************

  module      : IspcIdl2Code.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
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


#include "idl/xml2ispc/IspcErrorHandling.h"
/* #include "idl/expat/xmlparse.h" PTS 1109175 */
#include "idl/expat/expat.h"    /* PTS 1109175 */
#include "idl/xml2ispc/IspcIdl2Code.h"
#include "idl/xml2ispc/IspcCGenHFile.h"
#include "idl/xml2ispc/IspcCGenInspecCode.h"
#include "idl/xml2ispc/IspcVersion.h"


const int cmx_filename_len      = 256;
const int cmx_input_buffer_size = 4096;
      int ispc_error_lvl        = 0;    /* PTS 1122587 2003-Nov-11 FF */

typedef char t_filename[cmx_filename_len];

typedef struct t_xmlUserArgs {
  IspcCGenHFile      m_IspcCGenHFile;
  IspcCGenInspecCode m_IspcCGenInspecCode;
  int                m_depth;
  DboType_Enum_ispc  m_dboType; 
  int                m_error;
  XML_Parser         m_parser;
} t_xmlUserArgs;

static int  g_warn_level = 0;      /* PTS 1109175 */
static int  g_multiple_offsets =0; /* PTS 1115374 2002-04-23 FF */

int ispc_getWarnLevel() {
  return g_warn_level;
}

int ispc_getMultiOffsets() {
  return g_multiple_offsets;
}


/*----------------------------------------------------------------------------*/
// upper case conversion of a null terminated s

inline char * strupper (char * s) {
  char * c = s;
  while (*c) {
    if (islower(*c)) {
      *c = toupper(*c);
    }
    c++;
  }
  return s;
}

/*----------------------------------------------------------------------------*/

XmlAttributeType_Enum_ispc ispc_getAttributeType(const char * attrib) {

  const char cChar[]         = "CHAR";
  const char cDimension[]    = "DIMENSION";
  const char cDirection[]    = "DIRECTION";
  const char cExpression[]   = "EXPRESSION";
  const char cGenDefConstr[] = "GENDEFCONSTR"; /* PTS 1131325, PG */
  const char cGenTabHandle[] = "GENTABHANDLE"; /* PTS 1131325, PG */
  const char cName[]         = "NAME";
  const char cNumber[]       = "NUMBER";
  const char cPack[]         = "PACK";
  const char cString[]       = "STRING";
  const char cType[]         = "TYPE";
  const char cUuid[]         = "UUID";
  const char cValue[]        = "VALUE";        /* PTS 1131325, PG */

  if (0 == strcmp( attrib, cChar )        ) return attChar_ispc;
  if (0 == strcmp( attrib, cDimension )   ) return attDimension_ispc;
  if (0 == strcmp( attrib, cDirection )   ) return attDirection_ispc;
  if (0 == strcmp( attrib, cExpression )  ) return attExpression_ispc;
  if (0 == strcmp( attrib, cGenDefConstr )) return attGenDefConstr_ispc; /* PTS 1131325, PG */
  if (0 == strcmp( attrib, cGenTabHandle )) return attGenTabHandle_ispc; /* PTS 1131325, PG */
  if (0 == strcmp( attrib, cName )        ) return attName_ispc;
  if (0 == strcmp( attrib, cNumber )      ) return attNumber_ispc;
  if (0 == strcmp( attrib, cPack )        ) return attPack_ispc;
  if (0 == strcmp( attrib, cString )      ) return attString_ispc;
  if (0 == strcmp( attrib, cType )        ) return attType_ispc;
  if (0 == strcmp( attrib, cUuid )        ) return attUuid_ispc;
  if (0 == strcmp( attrib, cValue )       ) return attValue_ispc;        /* PTS 1131325, PG */

  return attUnknown_ispc;

}

/*----------------------------------------------------------------------------*/

XmlElementType_Enum_ispc ispc_getElementType(const char * element) {

  const char cAdd[]             = "ADD";
  const char cAlignmentmember[] = "ALIGNMENTMEMBER"; /* PTS 1141233, PG */
  const char cCoClass[]         = "COCLASS";
  const char cConst[]           = "CONST";
  const char cCpp[]             = "CPP";
  const char cCppsqldbc[]       = "CPPSQLDBC"; /* PTS 1138438, PG */
  const char cDefine[]          = "DEFINE"; /* PTS 1141233, PG */
  const char cDiv[]             = "DIV";
  const char cEnum[]            = "ENUM";
  const char cExpr[]            = "EXPR";
  const char cFixed[]           = "FIXED";
  const char cFloat[]           = "FLOAT";
  const char cIdent[]           = "IDENT";
  const char cIdl[]             = "IDL";
  const char cIdl2Xml[]         = "IDL2XML";
  const char cImport[]          = "IMPORT";
  const char cInclude[]         = "INCLUDE";
  const char cInterface[]       = "INTERFACE";
  const char cIntrinsicmember[] = "INTRINSICMEMBER"; /* PTS 1141233, PG */
  const char cLibrary[]         = "LIBRARY";
  const char cMember[]          = "MEMBER";
  const char cMod[]             = "MOD";
  const char cMul[]             = "MUL";
  const char cParam[]           = "PARAM";
  const char cPragma[]          = "PRAGMA";
  const char cStruct[]          = "STRUCT";
  const char cSub[]             = "SUB";
  const char cTypedef[]         = "TYPEDEF";
  const char cUnion[]           = "UNION";   /* PTS 1141233, PG */

  if (0 == strcmp( element, cAdd )            ) return elemAdd_ispc;
  if (0 == strcmp( element, cAlignmentmember )) return elemAlignmentmember_ispc; /* PTS 1141233, PG */
  if (0 == strcmp( element, cCoClass )        ) return elemCoClass_ispc;
  if (0 == strcmp( element, cConst )          ) return elemConst_ispc;
  if (0 == strcmp( element, cCpp )            ) return elemCpp_ispc;
  if (0 == strcmp( element, cCppsqldbc )      ) return elemCppsqldbc_ispc;
  if (0 == strcmp( element, cDefine )         ) return elemDefine_ispc; /* PTS 1141233, PG */
  if (0 == strcmp( element, cDiv )            ) return elemDiv_ispc;
  if (0 == strcmp( element, cEnum )           ) return elemEnum_ispc;
  if (0 == strcmp( element, cExpr )           ) return elemExpr_ispc;
  if (0 == strcmp( element, cFixed )          ) return elemFixed_ispc;
  if (0 == strcmp( element, cFloat )          ) return elemFloat_ispc;
  if (0 == strcmp( element, cIdent )          ) return elemIdent_ispc;
  if (0 == strcmp( element, cIdl )            ) return elemIdl_ispc;
  if (0 == strcmp( element, cIdl2Xml )        ) return elemIdl2Xml_ispc;
  if (0 == strcmp( element, cImport )         ) return elemImport_ispc;
  if (0 == strcmp( element, cInclude )        ) return elemInclude_ispc;
  if (0 == strcmp( element, cInterface )      ) return elemInterface_ispc;
  if (0 == strcmp( element, cIntrinsicmember )) return elemIntrinsicmember_ispc; /* PTS 1141233, PG */
  if (0 == strcmp( element, cLibrary )        ) return elemLibrary_ispc;
  if (0 == strcmp( element, cMember )         ) return elemMember_ispc;
  if (0 == strcmp( element, cMod )            ) return elemMod_ispc;
  if (0 == strcmp( element, cMul )            ) return elemMul_ispc;
  if (0 == strcmp( element, cParam )          ) return elemParam_ispc;
  if (0 == strcmp( element, cPragma )         ) return elemPragma_ispc;
  if (0 == strcmp( element, cStruct )         ) return elemStruct_ispc;
  if (0 == strcmp( element, cSub )            ) return elemSub_ispc;
  if (0 == strcmp( element, cTypedef )        ) return elemTypedef_ispc;
  if (0 == strcmp( element, cUnion )          ) return elemUnion_ispc;   /* PTS 1141233, PG */

  return elemUnknown_ispc;

}

/*----------------------------------------------------------------------------*/

bool ispc_isExprParam(const char * elem) {
  if ( (elemFloat_ispc == ispc_getElementType(elem)) ||
       (elemFixed_ispc == ispc_getElementType(elem)) ||
       (elemIdent_ispc == ispc_getElementType(elem)) ) {
    return true;
  }
  else {
    return false;
  }
}

/*----------------------------------------------------------------------------*/

static int ProcessCommandLine( int argc,
                               char * argv[],
                               t_filename & pcInputXML,
                               DboType_Enum_ispc & dboType,
                               bool & features_76,
                               bool & typedef_check ) { /* PTS 1131325, PG */
  if (argc < 2) {
    Ispc_ErrorMsg(C_WRONG_PARAMETERS_ISPC);
    return 0;
  }
  else {
    features_76   = false; /* PTS 1131325, PG */
    typedef_check = true;  /* PTS 1131325, PG */
    char buf[/*64 PTS 1111810*/cmx_filename_len];
    /* PTS 1110536
      strcpy( &pcInputXML[0], argv[1] );
      if (argc > 2) {
        strcpy(buf,argv[2]); */
    /*+++++ PTS 1110536 +++++*/
    int argno = 1;
    while (argv[argno][0] == '-') {
      if (argc < argno + 1) {
        Ispc_ErrorMsg(C_WRONG_PARAMETERS_ISPC);
        return 0;
      }
      if (STRICMP( argv[argno], "-w" ) == 0) {
        g_warn_level = WARN1;
        argno++;
      }
      else if (STRICMP( argv[argno], "-m" ) == 0) {
        g_multiple_offsets = 1;
        argno++;
      }
      else if (STRICMP( argv[argno], "-v" ) == 0) {
        cout << ">>> Version: "
             << MAJOR_VERSION_NO_STR_ISPCV
             << "." << MINOR_VERSION_NO_STR_ISPCV
             << "." << CORRECTION_LEVEL_STR_ISPCV
             << " Build " << BUILD_PREFIX_STR_ISPCV
             << "   " << DATE_ISPCV
             << endl;
        return 0;
      }
      /* PTS 1131325, PG */
      else if (STRICMP( argv[argno], "-f76" ) == 0) {
        features_76 = true;
        argno++;
      }
      else if (STRICMP( argv[argno], "-notdc" ) == 0) {
        typedef_check = false;
        argno++;
      }
      else {
        argno++;
        /*Ispc_ErrorMsg( C_WRONG_PARAMETERS_ISPC );*/
        /*return 0;*/
      }
    }
    strcpy( &pcInputXML[0], argv[argno] );
    argno++;               //PTS 1111810
    if ( argc >  argno /* + 1 PTS 1111810 */ ) {
      strcpy( buf, argv[argno] );
      /*----- PTS 1110536 -----*/
      if (!strcmp( "UDT", strupper(buf) )) {
        dboType = dbo_method_ispc;
      }
      else if (!strcmp( "DBPROC", strupper(buf) )) {
        dboType = dbo_proc_ispc;
      }
      else {
        Ispc_ErrorMsg(C_WRONG_PARAMETERS_ISPC);
        return 0;
      }
    }
    else {
      dboType = dbo_proc_ispc;
    }
  }
  return 1;
}

/*----------------------------------------------------------------------------*/

extern "C" void charData( void * userData, const char * s, int len) {
  t_xmlUserArgs * puserArgs = (t_xmlUserArgs*)userData;
  puserArgs->m_IspcCGenHFile.charData( puserArgs->m_error, s, len );
  puserArgs->m_IspcCGenInspecCode.charData( puserArgs->m_error, s, len );
}

/*----------------------------------------------------------------------------*/

extern "C" void startElement(void * userData, const char * name, const char ** atts ) {
  t_xmlUserArgs * puserArgs = (t_xmlUserArgs*)userData;
  if (ispc_isExprParam(name)) {
    XML_SetCharacterDataHandler( puserArgs->m_parser, &charData );
  }
  puserArgs->m_IspcCGenHFile.startElement( puserArgs->m_error, name, atts ); /* PTS 1131325, PG */
  puserArgs->m_IspcCGenInspecCode.startElement( puserArgs->m_error, puserArgs->m_dboType, name, atts );
  puserArgs->m_depth++;
}

/*----------------------------------------------------------------------------*/

extern "C" void endElement( void * userData, const char * name) {
  t_xmlUserArgs * puserArgs = (t_xmlUserArgs*)userData;
  puserArgs->m_IspcCGenHFile.endElement(name);
  puserArgs->m_IspcCGenInspecCode.endElement( puserArgs->m_error, name );
  puserArgs->m_depth--;
  if (ispc_isExprParam(name)) {
    XML_SetCharacterDataHandler( puserArgs->m_parser, NULL );
  }
}

/*----------------------------------------------------------------------------*/

//extern "C" __stdcall DebugBreak();

int main( int argc , char * argv[] ) {

  t_filename    cInputXML;
  // PTS 1111810  tsp00_VFilenamec  cFilename;
  char          buf[cmx_input_buffer_size];
  t_xmlUserArgs userArgs;
  bool          features_76   = false; /* PTS 1131325, PG */
  bool          typedef_check = true;  /* PTS 1131325, PG */

//  DebugBreak();

  // PTS 1111810  strcpy(cFilename,"");
  if (ProcessCommandLine( argc , argv, cInputXML, userArgs.m_dboType, features_76, typedef_check )) {
    userArgs.m_depth = 0;
    userArgs.m_error = 0;
    FILE * strmInputXML;
    if ( userArgs.m_IspcCGenHFile.Init( cInputXML, features_76, typedef_check ) &&
         ((strmInputXML = fopen( cInputXML, "r" )) != NULL) ) {
      userArgs.m_IspcCGenInspecCode.Init( cInputXML, features_76 );
      userArgs.m_parser = XML_ParserCreate(NULL);
      int done;
      XML_SetUserData( userArgs.m_parser, &userArgs );
      XML_SetElementHandler( userArgs.m_parser, &startElement, &endElement );
      do {
        size_t len = fread( buf, 1, sizeof(buf), strmInputXML );
        done = (len < sizeof(buf));
        if (!XML_Parse( userArgs.m_parser, buf, len, done )) {
          Ispc_ErrorMsg( C_XML_PARSE_ERROR_ISPC,
                         XML_ErrorString(XML_GetErrorCode(userArgs.m_parser)),
                         XML_GetCurrentLineNumber(userArgs.m_parser) );
          return C_XML_PARSE_ERROR_ISPC;
        }
      } while (!done);
      XML_ParserFree(userArgs.m_parser);
    }
  }

  /* PTS 1122587 2003-Nov-11 FF          */
  /* instead of: return userArgs.m_error */
  if (userArgs.m_error) {
    ispc_error_lvl = 8;
  }
  return ispc_error_lvl;

}

