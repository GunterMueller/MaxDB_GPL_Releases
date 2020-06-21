/*!*****************************************************************************

  module      : X2DC_GenApplHeaderFile.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of persistent class description code
  description : Generate application header file
				
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


#include "idl/XML2DiscCpp/X2DC_GenApplHeaderFile.hpp"


/******************************************************************************/
/*     Implementation of class GenApplHeaderFile                              */
/******************************************************************************/

GenApplHeaderFile::GenApplHeaderFile( int * pError, const Cpp_Type_Registry * pCTR )
: m_pError(pError)
, m_pCTR(pCTR)
, m_Make76(false)
, m_pIncludeBlocker(false)
, m_isWithin_Import(0)
, m_isWithin_Struct(false)
, m_isWithin_Union(false)
, m_isWithin_Class(false)
, m_isWithin_PersClass(false)
, m_isWithin_PersKeyedClass(false)
, m_isWithin_TemplClass(false)
, m_isWithin_TemplParams(false)
, m_isWithin_PersTemplClass(false)
, m_isWithin_Instantiation(false)
, m_isWithin_PersInstantiation(false)
, m_pPersClass(NULL)
, m_pPersTemplName(NULL)
, m_pPersClassID(NULL)
, m_pPersTemplPBC(NULL)
, m_gen_DefConstr(0)
, m_i_declared(false)
, m_isWithin_OidMember(false)
, m_isWithin_TemplMember(false)
, m_isFirst_TemplParam(false)
, m_pBfType(NULL)
, m_isWithin_Enum(0)
, m_isWithin_GenEnum(0)
, m_pDefConstr(NULL)
, m_pPcidEnum(NULL)
, m_isFirst_BaseClass(false)
, m_print_BaseClasses(false)
, m_write_LeftBrace(false)
, m_write_to(ApplHeader)
{}

/*----------------------------------------------------------------------------*/

GenApplHeaderFile::~GenApplHeaderFile() {

  if ( (!*m_pError) && ( m_isWithin_Import            ||
                         m_isWithin_Struct            ||
                         m_isWithin_Union             ||
                         m_isWithin_Class             ||
                         m_isWithin_PersClass         ||
                         m_isWithin_PersKeyedClass    ||
                         m_isWithin_TemplClass        ||
                         m_isWithin_TemplParams       ||
                         m_isWithin_PersTemplClass    ||
                         m_isWithin_Instantiation     ||
                         m_isWithin_PersInstantiation ||
                         m_pPersClass                 ||
                         m_pPersTemplName             ||
                         m_gen_DefConstr              ||
                         m_i_declared                 ||
                         m_isWithin_OidMember         ||
                         m_isWithin_TemplMember       ||
                         m_isFirst_TemplParam         ||
                         m_pPersClassID               ||
                         m_pPersTemplPBC              ||
                         m_pBfType                    ||
                         m_isWithin_Enum              ||
                         m_isWithin_GenEnum           ||
                         m_pDefConstr                 ||
                         m_pPcidEnum                  ||
                         m_isFirst_BaseClass          ||
                         m_print_BaseClasses          ||
                         m_write_LeftBrace ) ) {
    X2DC_WarnMsg( "Application header file generator not cleaned up correctly!", false );
  }

  m_ApplHeader << endl << endl;
  m_ApplHeader << "#endif";
  if (m_pIncludeBlocker) {
    m_ApplHeader << " // " << m_pIncludeBlocker;
    delete [] m_pIncludeBlocker;
    m_pIncludeBlocker = NULL;
  }
  m_ApplHeader << endl << endl;
  m_ApplHeader.close();

}

/*----------------------------------------------------------------------------*/

int GenApplHeaderFile::Init( int & error, char * name, bool make76, bool lcapps ) {

  m_Make76 = make76;

  /* Initialize application header file */

  int len = strlen(name);
  char * file = new char[len+3];
  strcpy( file, name );
  file[len]   = '.';
  file[len+1] = 'h';
  file[len+2] = 0;

  m_ApplHeader.open(file);
  if (m_ApplHeader.fail()) {
    X2DC_ErrMsg( M__FILE_OPEN_ERROR, file);
    error = 1;
    return 1;
  }
  delete [] file;
  file = NULL;

  char * pName = strrchr( name, '/' );
  if (pName) {
    pName++;
  }
  else {
    pName = strrchr( name, '\\' );
    if (pName) {
      pName++;
    }
    else {
      pName = name;
    }
  }

  if (lcapps) {
    char * pPathAndName = new char[strlen(name)+1];
    strcpy( pPathAndName, name );
    char * pBSlash = strchr( pPathAndName, '\\' );
    while (pBSlash) {
      *pBSlash = '/';
      pBSlash = strchr( pBSlash+1, '\\' );
    }
    m_pIncludeBlocker = new char[4+strlen(pName)+1];
    strcpy( m_pIncludeBlocker, "INC_" );
    ToUpper( pName, m_pIncludeBlocker+4, strlen(pName) );
    m_ApplHeader << "/*#" << endl;
    m_ApplHeader << "#ifndef " << m_pIncludeBlocker << endl;
    m_ApplHeader << "#include \"" << pPathAndName << ".h\" // dep=" << pPathAndName << ".xml" << endl;
    m_ApplHeader << "#endif" << endl;
    m_ApplHeader << "*/" << endl;
    m_ApplHeader << endl;
    m_ApplHeader << "#ifndef " << m_pIncludeBlocker << endl;
    m_ApplHeader << "#define " << m_pIncludeBlocker << endl;
    m_ApplHeader << endl << endl;
    delete [] pPathAndName;
    pPathAndName = NULL;
  }
  else {
    m_ApplHeader << endl << endl;
    char * pNameUpper = new char[strlen(pName)+1];
    ToUpper( pName, pNameUpper, strlen(pName) );
    m_ApplHeader << "#ifndef " << pNameUpper << "__H" << endl;
    m_ApplHeader << "#define " << pNameUpper << "__H" << endl;
    m_ApplHeader << endl << endl;
    delete [] pNameUpper;
    pNameUpper = NULL;
  }

  m_ApplHeader   << "#include \"stddef.h\"" << endl;
  if (!m_Make76) {
    m_ApplHeader << endl;
    m_ApplHeader << "#include \"livecachepcif.h\" /* no check */" << endl;
  }

  /* Ok */

  return 0;

}

/*----------------------------------------------------------------------------*/

void GenApplHeaderFile::StartElement( int & error, const char * name, const char ** atts ) {

  /* Within import mode */

  if (elemImport_X2DC == X2DC_getElementType(name)) {
    if (0 == m_isWithin_Import) {
      int strLen = strlen(atts[1]);
      char * header = new char[strLen-1];
      strncpy( header, atts[1], strLen-3 );
      *(header+strLen-3) = 'h';
      *(header+strLen-2) = 0;
      m_ApplHeader << endl;
      m_ApplHeader << "#include \"" << header << "\" /* no check */" << endl;
      delete [] header;
      header = NULL;
    }
    m_isWithin_Import++;
    return;
  }
  else if (m_isWithin_Import) {
    return;
  }

  /* Generate coding */

  switch (X2DC_getElementType(name)) {

    /*---------- CPP ---------------------*/

    case elemCpp_X2DC : {
      if ( m_isWithin_Class || m_isWithin_TemplClass ||
           m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
        Append(atts[1]);
        EndLine();
      }
      else {
        m_ApplHeader << atts[1] << endl;
      }
      break;
    }

    /*---------- STRUCT ------------------*/

    case elemStruct_X2DC : {
      m_isWithin_Struct = true;
      m_gen_DefConstr = 0;
      m_i_declared = false;
      m_ApplHeader << endl;
      m_ApplHeader << "struct " << atts[1] << " {" << endl;
      for ( int i = 2; atts[i]; i = i+2 ) {
        if ( (attGendefconstr_X2DC == X2DC_getAttributeType(atts[i])) &&
             (0 == strcmp( atts[i+1], "YES" )) ) {
          m_gen_DefConstr = 2;
        }
      }
      if (m_gen_DefConstr) {
        m_ApplHeader << "  inline " << atts[1] << "(); " << endl;
        m_DefConstr_Head = "inline ";
        m_DefConstr_Head.append(atts[1]);
        m_DefConstr_Head.append("::");
        m_DefConstr_Head.append(atts[1]);
        m_DefConstr_Head.append("()");
        m_DefConstr_Head.append("\n");
        m_DefConstr_Body = "{";
        m_DefConstr_Body.append("\n");
      }
      break;
    }

    /*---------- UNION -------------------*/

    case elemUnion_X2DC : {
      m_isWithin_Union = true;
      m_ApplHeader << endl;
      m_ApplHeader << "union " << atts[1] << " {" << endl;
      break;
    }

    /*---------- CLASS -------------------*/

    case elemClass_X2DC : {
      m_isWithin_Class = true;
      m_gen_DefConstr = 0;
      m_pDefConstr = NULL;
      m_i_declared = false;
      m_ApplHeader << endl;
      m_ApplHeader << "class ";
      int exIdx = 0, dfIdx = 0;
      for ( int i = 2; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attExtension_X2DC : {
            exIdx = i;
            break;
          }
          case attGendefconstr_X2DC : {
            dfIdx = i;
            break;
          }
        }
      }
      if (exIdx) {
        m_ApplHeader << atts[exIdx+1] << " ";
      }
      m_ApplHeader << atts[1] << endl;
      if ( dfIdx && (0 != strcmp( atts[dfIdx+1], "NO" )) ) {
        m_gen_DefConstr = 2;
        m_pDefConstr = new Def_Constr( atts[1], atts[dfIdx+1] );
        m_DefConstr_Head = "inline ";
        m_DefConstr_Head.append(atts[1]);
        m_DefConstr_Head.append("::");
        m_DefConstr_Head.append(atts[1]);
        m_DefConstr_Head.append("()");
        m_DefConstr_Head.append("\n");
        m_DefConstr_Body = "{";
        m_DefConstr_Body.append("\n");
      }
      m_write_to = ApplHeader;
      m_isFirst_BaseClass = true;
      m_write_LeftBrace = true;
      break;
    }

    /*---------- PERSCLASS ---------------*/

    case elemPersclass_X2DC : {
      m_isWithin_PersClass = true;
      m_pPersClass = new Cpp_Name(atts[1]);
      m_pPersClassID = new char[strlen(atts[3])+1];
      strcpy( m_pPersClassID, atts[3] );
      m_gen_DefConstr = 0;
      m_pDefConstr = new Def_Constr( atts[1], "public" );
      m_PersClass_Head = "class ";
      int ideIdx = 0;
      int extIdx = 0;
      int pbcIdx = 0;
      for ( int i = 4; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attIdenum_X2DC : {
            ideIdx = i;
            break;
          }
          case attExtension_X2DC : {
            extIdx = i;
            break;
          }
          case attPersbaseclass_X2DC : {
            pbcIdx = i;
            break;
          }
        }
      }
      if (ideIdx) {
        m_pPcidEnum = new char[strlen(atts[ideIdx+1])+1];
        strcpy( m_pPcidEnum, atts[ideIdx+1] );
      }
      if (extIdx) {
        m_PersClass_Head.append(atts[extIdx+1]);
        m_PersClass_Head.append(" ");
      }
      m_PersClass_Head.append(atts[1]);
      m_PersClass_Head.append("\n");
      if (pbcIdx) {
        m_PersClass_Head.append(": public OmsInheritedObject< ");
        m_PersClass_Head.append(atts[1]);
        m_PersClass_Head.append(", ");
        m_PersClass_Head.append(atts[pbcIdx+1]);
      }
      else {
        m_PersClass_Head.append(": public OmsObject< ");
        m_PersClass_Head.append(atts[1]);
      }
      m_PersClass_Head.append(", ");
      m_PersClass_Head.append(atts[3]);
      m_PersClass_Head.append(" >");
      m_PersClass_Head.append("\n");
      m_write_to = PersClassBody;
      m_isFirst_BaseClass = false;
      m_write_LeftBrace = true;
      break;
    }

    /*---------- PERSKEYEDCLASS ----------*/

    case elemPerskeyedclass_X2DC : {
      m_isWithin_PersKeyedClass = true;
      m_pPersClass = new Cpp_Name(atts[1]);
      m_pPersClassID = new char[strlen(atts[3])+1];
      strcpy( m_pPersClassID, atts[3] );
      m_gen_DefConstr = 0;
      m_pDefConstr = new Def_Constr( atts[1], "public" );
      m_PersClass_Head = "class ";
      int ideIdx = 0;
      int keyIdx = 0;
      int extIdx = 0;
      for ( int i = 4; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attIdenum_X2DC : {
            ideIdx = i;
            break;
          }
          case attKeyclass_X2DC : {
            keyIdx = i;
            break;
          }
          case attExtension_X2DC : {
            extIdx = i;
            break;
          }
        }
      }
      if (ideIdx) {
        m_pPcidEnum = new char[strlen(atts[ideIdx+1])+1];
        strcpy( m_pPcidEnum, atts[ideIdx+1] );
      }
      if (extIdx) {
        m_PersClass_Head.append(atts[extIdx+1]);
        m_PersClass_Head.append(" ");
      }
      m_PersClass_Head.append(atts[1]);
      m_PersClass_Head.append("\n");
      m_PersClass_Head.append(": public OmsKeyedObject< ");
      m_PersClass_Head.append(atts[1]);
      m_PersClass_Head.append(", ");
      m_PersClass_Head.append(atts[keyIdx+1]);
      m_PersClass_Head.append(", ");
      m_PersClass_Head.append(atts[3]);
      m_PersClass_Head.append(" >");
      m_PersClass_Head.append("\n");
      m_write_to = PersClassBody;
      m_isFirst_BaseClass = false;
      m_write_LeftBrace = true;
      break;
    }

    /*---------- DERIVATION --------------*/

    case elemDerivefrom_X2DC : {
      if (m_isFirst_BaseClass) {
        Append(": ");
        m_isFirst_BaseClass = false;
      }
      else {
        Append(", ");
      }
      Append(atts[3]);
      Append(" ");
      Append(atts[1]);
      EndLine();
      break;
    }

    /*---------- TEMPLCLASS --------------*/

    case elemTemplclass_X2DC : {
      m_isWithin_TemplClass = true;
      m_gen_DefConstr = 0;
      m_pDefConstr = NULL;
      m_ApplHeader << endl;
      m_ApplHeader << "template< ";
      m_Temp_String = " >";
      m_Temp_String.append("\n");
      m_Temp_String.append("class ");
      if (atts[2]) {
        m_Temp_String.append(atts[3]);
        m_Temp_String.append(" ");
      }
      m_Temp_String.append(atts[1]);
      m_Temp_String.append("\n");
      m_write_to = ApplHeader;
      m_isFirst_BaseClass = true;
      m_write_LeftBrace = true;
      break;
    }

    case elemTemplparams_X2DC : {
      if ( m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
        m_isWithin_TemplParams = true;
        m_isFirst_TemplParam = true;
      }
      break;
    }

    case elemClassparam_X2DC : {
      if (m_isWithin_TemplParams) {
        if (m_isFirst_TemplParam) {
          if (m_isWithin_PersTemplClass) {
            m_Temp_String.append(atts[1]);
          }
          Append("class ");
          Append(atts[1]);
          m_isFirst_TemplParam = false;
        }
        else {
          if (m_isWithin_PersTemplClass) {
            m_Temp_String.append(", ");
            m_Temp_String.append(atts[1]);
          }
          Append(", class ");
          Append(atts[1]);
        }
      }
      break;
    }

    case elemIntparam_X2DC : {
      if (m_isWithin_TemplParams) {
        if (m_isFirst_TemplParam) {
          if (m_isWithin_PersTemplClass) {
            m_Temp_String.append(atts[3]);
          }
          Append(atts[1]);
          Append(" ");
          Append(atts[3]);
          m_isFirst_TemplParam = false;
        }
        else {
          if (m_isWithin_PersTemplClass) {
            m_Temp_String.append(", ");
            m_Temp_String.append(atts[3]);
          }
          Append(", ");
          Append(atts[1]);
          Append(" ");
          Append(atts[3]);
        }
      }
      break;
    }

    /*---------- PERSTEMPLCLASS ----------*/

    case elemPerstemplclass_X2DC : {
      m_isWithin_PersTemplClass = true;
      m_pPersClass = new Cpp_Name(atts[1]);
      m_pPersClassID = new char[strlen(atts[3])+1];
      strcpy( m_pPersClassID, atts[3] );
      m_gen_DefConstr = 0;
      m_pDefConstr = new Def_Constr( atts[1], "public" );
      m_write_to = PersClassHead;
      m_PersClass_Head  = "template< ";
      m_Temp_String = " >";
      m_Temp_String.append("\n");
      m_Temp_String.append("class ");
      int ideIdx = 0;
      int extIdx = 0;
      int pbcIdx = 0;
      for ( int i = 4; atts[i]; i = i+2 ) {
        switch (X2DC_getAttributeType(atts[i])) {
          case attIdenum_X2DC : {
            ideIdx = i;
            break;
          }
          case attExtension_X2DC : {
            extIdx = i;
            break;
          }
          case attPersbaseclass_X2DC : {
            pbcIdx = i;
            break;
          }
        }
      }
      if (ideIdx) {
        m_pPcidEnum = new char[strlen(atts[ideIdx+1])+1];
        strcpy( m_pPcidEnum, atts[ideIdx+1] );
      }
      if (extIdx) {
        m_Temp_String.append(atts[extIdx+1]);
        m_Temp_String.append(" ");
      }
      m_Temp_String.append(atts[1]);
      m_Temp_String.append("\n");
      if (pbcIdx) {
        m_Temp_String.append(": public OmsInheritedObject< ");
        m_pPersTemplPBC = new char[strlen(atts[pbcIdx+1])+1];
        strcpy( m_pPersTemplPBC, atts[pbcIdx+1] );
      }
      else {
        m_Temp_String.append(": public OmsObject< ");
      }
      m_Temp_String.append(atts[1]);
      m_Temp_String.append("< ");
      m_write_to = PersClassHead;
      m_isFirst_BaseClass = false;
      m_write_LeftBrace = true;
      break;
    }

    /*---------- INSTANTIATE -------------*/

    case elemInstantiate_X2DC : {
      m_isWithin_Instantiation = true;
      m_ApplHeader << endl;
      m_ApplHeader << "typedef " << atts[1] << "< ";
      m_isFirst_TemplParam = true;
      m_Temp_String = " > ";
      m_Temp_String.append(atts[3]);
      m_Temp_String.append(";");
      m_Temp_String.append("\n");
      break;
    }

    /*---------- PERSINTERFACE -----------*/

    case elemPersinterface_X2DC : {
      m_ApplHeader << endl;
      m_ApplHeader << "class ";
      if (atts[4]) {
        m_ApplHeader << atts[5] << " ";
      }
      m_ApplHeader << atts[1] << endl;
      m_ApplHeader << ": public OmsObject< " << atts[1] << ", " << atts[3] << " >" << endl;
      m_ApplHeader << "{" << endl;
      break;
    }

    /*---------- FRIEND ------------------*/

    case elemFriend_X2DC : {
      if (m_write_LeftBrace) {
        Append("{");
        EndLine();
        m_write_LeftBrace = false;
      }
      Append("  friend class ");
      Append(atts[1]);
      Append(";");
      EndLine();
      break;
    }

    /*---------- SECTION -----------------*/

    case elemSection_X2DC : {
      if (m_write_LeftBrace) {
        Append("{");
        EndLine();
        m_write_LeftBrace = false;
      }
      if (m_pDefConstr) {
        Append(m_pDefConstr->getType());
        Append(":");
        EndLine();
        Append("  ");
        if (m_isWithin_Class) {
          Append("inline ");
        }
        Append(m_pDefConstr->getName());
        Append("()");
        if ( m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
          if (!m_Make76) {
            Append(" {}");
          }
        }
        Append(";");
        EndLine();
        if ( m_isWithin_PersClass || m_isWithin_PersKeyedClass || m_isWithin_PersTemplClass ) {
          if (m_pPcidEnum) {
            Append("  enum {");
            Append(m_pPcidEnum);
            delete [] m_pPcidEnum;
            m_pPcidEnum = NULL;
            Append(" = ");
            Append(m_pPersClassID);
            Append("};");
            EndLine();
          }
          if (!m_Make76) {
            Append("  ");
            Append(m_pDefConstr->getName());
            Append("( X2DC_ClassID id, X2DC_PersClass ** pIF );");
            EndLine();
          }
        }
        delete m_pDefConstr;
        m_pDefConstr = NULL;
      }
      Append(atts[1]);
      Append(":");
      EndLine();
      break;
    }

    /*---------- CONSTANTS ---------------*/

    case elemEnum_X2DC : {
      m_isWithin_Enum = 2;
      m_ApplHeader << endl << "enum " << atts[1] << " {" << endl;
      break;
    }

    case elemGeneralenum_X2DC : {
      m_isWithin_GenEnum = 2;
      m_ApplHeader << endl << "enum";
      if (atts[0]) {
        m_ApplHeader << " " << atts[1];
      }
      m_ApplHeader << " {" << endl;
      break;
    }

    case elemConst_X2DC : {
      m_ApplHeader << endl;
      if (attString_X2DC == X2DC_getAttributeType(atts[4])) {
        m_ApplHeader << "#define " << atts[1] << " ( \"" << atts[5] << "\" )" << endl;
      }
      else {
        m_ApplHeader << "#define " << atts[1] << " ( " << atts[5] << " )" << endl;
      }
      break;
    }

    /*---------- MEMBERS -----------------*/

    case elemBitfields_X2DC : {
      if ( m_isWithin_Class || m_isWithin_PersClass || m_isWithin_PersKeyedClass ||
           m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
        m_pBfType = new Cpp_Name(atts[1]);
      }
      break;
    }

    case elemField_X2DC : {
      if (m_pBfType) {
        Append("  ");
        Append(m_pBfType->getName());
        Append(" ");
        Append(atts[1]);
        Append(" : ");
        Append(atts[3]);
        Append(";");
        EndLine();
      }
      else {
        X2DC_ErrMsg( M__NULL_POINTER_IN_CASE, "X2DC_GenApplHeaderFile.cpp", "m_pBfType", "Start", "elemField_X2DC" );
        error = 1;
        return;
      }
      break;
    }

    case elemClassmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_PersClass || m_isWithin_PersKeyedClass ||
           m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
        Append("  ");
        int dimIdx = 0, conIdx = 0, staIdx = 0;
        for ( int i = 4; atts[i]; i = i+2 ) {
          switch (X2DC_getAttributeType(atts[i])) {
            case attDimension_X2DC : {
              dimIdx = i;
              break;
            }
            case attConst_X2DC : {
              conIdx = i;
              break;
            }
            case attStatic_X2DC : {
              staIdx = i;
              break;
            }
          }
        }
        if ( staIdx && (0 == strcmp( atts[staIdx+1], "YES" )) ) {
          Append("static ");
        }
        if ( conIdx && (0 == strcmp( atts[conIdx+1], "YES" )) ) {
          Append("const ");
        }
        Append(atts[3]);
        Append(" ");
        Append(atts[1]);
        if (dimIdx) {
          Append("[");
          Append(atts[dimIdx+1]);
          Append("]");
        }
        Append(";");
        EndLine();
        if (m_gen_DefConstr) {
          genDefconstrEntry( error, atts );
        }
      }
      break;
    }

    case elemMember_X2DC : {
      if (m_isWithin_Struct) {
        m_ApplHeader  << "  " << atts[3] << " " << atts[1];
        if (atts[4]) {
          m_ApplHeader << "[" << atts[5] << "]";
        }
        m_ApplHeader << ";" << endl;
        if (m_gen_DefConstr) {
          genDefconstrEntry( error, atts );
        }
      }
      else if (m_isWithin_Enum) {
        if (2 == m_isWithin_Enum) {
          m_ApplHeader << "  ";
          m_isWithin_Enum = 1;
        }
        else {
          m_ApplHeader << ", ";
        }
        m_ApplHeader << atts[1] << " = " << atts[3] << endl;
      }
      else if (m_isWithin_GenEnum) {
        if (2 == m_isWithin_GenEnum) {
          m_ApplHeader << "  ";
          m_isWithin_GenEnum = 1;
        }
        else {
          m_ApplHeader << ", ";
        }
        m_ApplHeader << atts[1] << " = " << atts[3] << endl;
      }
      break;
    }

    case elemAlignmentmember_X2DC :
    case elemIntrinsicmember_X2DC : {
      if (m_isWithin_Union) {
        m_ApplHeader  << "  " << atts[3] << " " << atts[1] << ";" << endl;
      }
      break;
    }

    case elemOidmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_PersClass || m_isWithin_PersKeyedClass ||
           m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
        m_isWithin_OidMember = true;
        Append("  OmsOid< ");
        m_Temp_String = atts[1];
        if (atts[2]) {
          m_Temp_String.append("[");
          m_Temp_String.append(atts[3]);
          m_Temp_String.append("]");
        }
        m_Temp_String.append(";");
        m_Temp_String.append("\n");
      }
      break;
    }

    case elemExtoidmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_PersClass || m_isWithin_PersKeyedClass ||
           m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
        Append("  OmsExternalOid ");
        Append(atts[1]);
        if (atts[2]) {
          Append("[");
          Append(atts[3]);
          Append("]");
        }
        Append(";");
        EndLine();
      }
      break;
    }

    case elemVaroidmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_PersClass || m_isWithin_PersKeyedClass ||
           m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
        Append("  OmsVarOid ");
        Append(atts[1]);
        if (atts[2]) {
          Append("[");
          Append(atts[3]);
          Append("]");
        }
        Append(";");
        EndLine();
      }
      break;
    }

    case elemTemplmember_X2DC : {
      if ( m_isWithin_Class || m_isWithin_PersClass || m_isWithin_PersKeyedClass ||
           m_isWithin_TemplClass || m_isWithin_PersTemplClass ) {
        m_isWithin_TemplMember = true;
        m_isFirst_TemplParam = true;
        Append("  ");
        Append(atts[3]);
        Append("< ");
        m_Temp_String = " > ";
        m_Temp_String.append(atts[1]);
        if (atts[4]) {
          m_Temp_String.append("[");
          m_Temp_String.append(atts[3]);
          m_Temp_String.append("]");
        }
        m_Temp_String.append(";");
        m_Temp_String.append("\n");
      }
      break;
    }

    case elemClassinstance_X2DC : {
      if (m_isWithin_OidMember) {
        Append(atts[1]);
        Append(" >");
      }
      else if ( m_isWithin_TemplMember || m_isWithin_Instantiation ) {
        if (m_isFirst_TemplParam) {
          Append(atts[1]);
          m_isFirst_TemplParam = false;
        }
        else {
          Append(", ");
          Append(atts[1]);
        }
      }
      break;
    }

    case elemIntinstance_X2DC : {
      if ( m_isWithin_TemplMember || m_isWithin_Instantiation ) {
        if (m_isFirst_TemplParam) {
          Append(atts[1]);
          m_isFirst_TemplParam = false;
        }
        else {
          Append(", ");
          Append(atts[1]);
        }
      }
      break;
    }

    /*---------- TYPEDEF -----------------*/

    case elemTypedef_X2DC : {
      int nameIdx = IndexOfNameAtt(atts);
      m_ApplHeader << endl;
      m_ApplHeader << "typedef " << atts[nameIdx+3] << " "
                   << atts[nameIdx+1] << ";" << endl;
      break;
    }

  }

}

/*----------------------------------------------------------------------------*/

void GenApplHeaderFile::EndElement(const char * name) {

  /* Within import mode */

  if (elemImport_X2DC == X2DC_getElementType(name)) {
    m_isWithin_Import--;
    return;
  }
  else if (m_isWithin_Import) {
    return;
  }

  switch (X2DC_getElementType(name)) {

    /*---------- CPP ---------------------*/

    case elemCpp_X2DC : {
      break;
    }

    /*---------- STRUCT ------------------*/

    case elemStruct_X2DC : {
      m_ApplHeader << "};" << endl;
      if (m_gen_DefConstr) {
        m_DefConstr_Body.append("}");
        m_DefConstr_Body.append("\n");
        m_ApplHeader << endl;
        m_ApplHeader << m_DefConstr_Head.data();
        m_ApplHeader << m_DefConstr_Body.data();
        m_DefConstr_Head.erase();
        m_DefConstr_Body.erase();
      }
      m_gen_DefConstr = 0;
      m_i_declared = false;
      m_isWithin_Struct = false;
      break;
    }

    /*---------- UNION -------------------*/

    case elemUnion_X2DC : {
      m_ApplHeader << "};" << endl;
      m_isWithin_Union = false;
      break;
    }

    /*---------- CLASS -------------------*/

    case elemClass_X2DC : {
      m_ApplHeader << "};" << endl;
      if (m_gen_DefConstr) {
        m_DefConstr_Body.append("}");
        m_DefConstr_Body.append("\n");
        m_ApplHeader << endl;
        m_ApplHeader << m_DefConstr_Head.data();
        m_ApplHeader << m_DefConstr_Body.data();
        m_DefConstr_Head.erase();
        m_DefConstr_Body.erase();
      }
      m_gen_DefConstr = 0;
      m_i_declared = false;
      m_isWithin_Class = false;
      break;
    }

    /*---------- DERIVATION --------------*/

    case elemDerivefrom_X2DC : {
      break;
    }

    /*---------- TEMPLCLASS --------------*/

    case elemTemplclass_X2DC : {
      m_ApplHeader << "};" << endl;
      m_isWithin_TemplClass = false;
      break;
    }

    case elemTemplparams_X2DC : {
      if (m_isWithin_PersTemplClass) {
        m_Temp_String.append(" >, ");
        if (m_pPersTemplPBC) {
          m_Temp_String.append(m_pPersTemplPBC);
          delete [] m_pPersTemplPBC;
          m_pPersTemplPBC = NULL;
          m_Temp_String.append(", ");
        }
        m_Temp_String.append(m_pPersClassID);
        m_Temp_String.append(" >");
        m_Temp_String.append("\n");
      }
      Append(m_Temp_String.data());
      m_Temp_String.erase();
      if (m_isWithin_PersTemplClass) {
        m_write_to = PersClassBody;
      }
      m_isWithin_TemplParams = false;
      break;
    }

    case elemClassparam_X2DC :
    case elemIntparam_X2DC : {
      break;
    }

    /*---------- PERS(TEMPL)CLASS --------*/

    case elemPersclass_X2DC :
    case elemPerskeyedclass_X2DC :
    case elemPerstemplclass_X2DC : {

      // Pers. class declaration
      m_ApplHeader << endl;
      m_ApplHeader << m_PersClass_Head.data();
      m_PersClass_Head.erase();
      const Pers_Class * pPC = m_pCTR->findPersClass(m_pPersClass->getName());
      if ( (pPC->getKey()) || (!pPC->getIndexList()->IsEmpty()) ) {
        m_ApplHeader << ", public OmsKeyIndex" << endl;
      }
      m_ApplHeader << m_PersClass_Body.data();
      m_PersClass_Body.erase();
      m_write_to = ApplHeader;
      m_ApplHeader << "};" << endl;

      // Key class
      const Index * pKey = pPC->getKey();
      if (pKey) {
        m_ApplHeader << endl;
        if (m_isWithin_PersTemplClass) {
          m_ApplHeader << "template< ClassID " << m_pPersClassID << " >" << endl;
        }
        m_ApplHeader << "class X2DC_" << m_pPersClass->getName() << "_KEY" << endl;
        m_ApplHeader << "{" << endl;
        m_ApplHeader << "public:" << endl;
        m_ApplHeader << "  enum Specified_Members {NONE = 0};" << endl;
        m_ApplHeader << "  explicit X2DC_" << m_pPersClass->getName() << "_KEY";
        m_ApplHeader << "( Specified_Members ) : m_PersClass_ID(" << m_pPersClassID << ")";
        m_ApplHeader <<                       ", m_KeyOrInd_ID(0), m_SpecMem_NO(0) {};" << endl;
        int memCnt = 0;
        int cntr = 0;
        Iterator<Index_Member> iter = pKey->getMemberList()->Begin();
        while (iter()) {
          memCnt++;
          iter++;
        }
        for ( int max = 1; max <= memCnt; max++ ) {
          iter.reset();
          cntr = 1;
          m_ApplHeader << "  inline explicit X2DC_" << m_pPersClass->getName() << "_KEY(";
          while (cntr <= max) {
            if (cntr == 1) {
              m_ApplHeader << " ";
            }
            else {
              m_ApplHeader << ", ";
            }
            m_ApplHeader << iter()->getType()->getName();
            iter++;
            cntr++;
          }
          m_ApplHeader << " );" << endl;
        }
        m_ApplHeader << "  OmsTypeInt4  getIdentifierOfPersClass() const {return m_PersClass_ID;}" << endl;
        m_ApplHeader << "  OmsTypeUInt2 getIdentifierOfKeyOrIndex() const {return m_KeyOrInd_ID;}" << endl;
        m_ApplHeader << "  OmsTypeUInt2 getNumberOfSpecifiedMembers() const {return m_SpecMem_NO;}" << endl;
        m_ApplHeader << "private:" << endl;
        m_ApplHeader << "  OmsTypeInt4  m_PersClass_ID;" << endl;
        m_ApplHeader << "  OmsTypeUInt2 m_KeyOrInd_ID;" << endl;
        m_ApplHeader << "  OmsTypeUInt2 m_SpecMem_NO;" << endl;
        iter.reset();
        cntr = 1;
        char memName[17];
        while (iter()) {
          memset( memName, 0, 17 );
          sprintf( memName, "m_Mem_%d", cntr );
          m_ApplHeader << "  " << iter()->getType()->getName() << " " << memName << ";" << endl;
          cntr++;
          iter++;
        }
        m_ApplHeader << "};" << endl;
        for ( int max = 1; max <= memCnt; max++ ) {
          m_ApplHeader << endl;
          if (m_isWithin_PersTemplClass) {
            m_ApplHeader << "template< ClassID " << m_pPersClassID << " >" << endl;
            m_ApplHeader << "inline X2DC_" << m_pPersClass->getName() << "_KEY< " << m_pPersClassID << " >::";
          }
          else {
            m_ApplHeader << "inline X2DC_" << m_pPersClass->getName() << "_KEY::";
          }
          m_ApplHeader << "X2DC_" << m_pPersClass->getName() << "_KEY(";
          char no[11];
          iter.reset();
          cntr = 1;
          while (cntr <= max) {
            if (cntr == 1) {
              m_ApplHeader << " ";
            }
            else {
              m_ApplHeader << ", ";
            }
            memset( no, 0, 11 );
            sprintf( no, "%d", cntr );
            m_ApplHeader << iter()->getType()->getName() << " Mem_" << no;
            iter++;
            cntr++;
          }
          m_ApplHeader << " )" << endl;
          m_ApplHeader << ": m_PersClass_ID(" << m_pPersClassID << ")" << endl;
          m_ApplHeader << ", m_KeyOrInd_ID(0)" << endl;
          char maxC[11];
          memset( maxC, 0, 11 );
          sprintf( maxC, "%d", max );
          m_ApplHeader << ", m_SpecMem_NO(" << maxC << ")" << endl;
          iter.reset();
          cntr = 1;
          while (cntr <= max) {
            m_ApplHeader << ", ";
            memset( no, 0, 11 );
            sprintf( no, "%d", cntr );
            m_ApplHeader << "m_Mem_" << no << "(Mem_" << no << ")" << endl;
            iter++;
            cntr++;
          }
          m_ApplHeader << "{}" << endl;
        }
      }

      // Index class(es)
      Iterator<Index> indIter = pPC->getIndexList()->Begin();
      while (indIter()) {
        char id[11];
        memset( id, 0, 11 );
        sprintf( id, "%d", indIter()->getID() );
        m_ApplHeader << endl;
        if (m_isWithin_PersTemplClass) {
          m_ApplHeader << "template< ClassID " << m_pPersClassID << " >" << endl;
        }
        m_ApplHeader << "class X2DC_" << m_pPersClass->getName() << "_INDEX" << id << endl;
        m_ApplHeader << "{" << endl;
        m_ApplHeader << "public:" << endl;
        m_ApplHeader << "  enum Specified_Members {NONE = 0};" << endl;
        m_ApplHeader << "  explicit X2DC_" << m_pPersClass->getName() << "_INDEX" << id;
        m_ApplHeader << "( Specified_Members ) : m_PersClass_ID(" << m_pPersClassID << ")";
        m_ApplHeader <<                       ", m_KeyOrInd_ID(" << id << "), m_SpecMem_NO(0) {};" << endl;
        int memCnt = 0;
        int cntr = 0;
        Iterator<Index_Member> iter = indIter()->getMemberList()->Begin();
        while (iter()) {
          memCnt++;
          iter++;
        }
        for ( int max = 1; max <= memCnt; max++ ) {
          iter.reset();
          cntr = 1;
          m_ApplHeader << "  inline explicit X2DC_" << m_pPersClass->getName() << "_INDEX" << id << "(";
          while (cntr <= max) {
            if (cntr == 1) {
              m_ApplHeader << " ";
            }
            else {
              m_ApplHeader << ", ";
            }
            m_ApplHeader << iter()->getType()->getName();
            iter++;
            cntr++;
          }
          m_ApplHeader << " );" << endl;
        }
        m_ApplHeader << "  OmsTypeInt4  getIdentifierOfPersClass() const {return m_PersClass_ID;}" << endl;
        m_ApplHeader << "  OmsTypeUInt2 getIdentifierOfKeyOrIndex() const {return m_KeyOrInd_ID;}" << endl;
        m_ApplHeader << "  OmsTypeUInt2 getNumberOfSpecifiedMembers() const {return m_SpecMem_NO;}" << endl;
        m_ApplHeader << "private:" << endl;
        m_ApplHeader << "  OmsTypeInt4  m_PersClass_ID" << endl;
        m_ApplHeader << "  OmsTypeUInt2 m_KeyOrInd_ID;" << endl;
        m_ApplHeader << "  OmsTypeUInt2 m_SpecMem_NO;" << endl;
        iter.reset();
        cntr = 1;
        char memName[17];
        while (iter()) {
          memset( memName, 0, 17 );
          sprintf( memName, "m_Mem_%d", cntr );
          m_ApplHeader << "  " << iter()->getType()->getName() << " " << memName << ";" << endl;
          cntr++;
          iter++;
        }
        m_ApplHeader << "};" << endl;
        for ( int max = 1; max <= memCnt; max++ ) {
          m_ApplHeader << endl;
          if (m_isWithin_PersTemplClass) {
            m_ApplHeader << "template< ClassID " << m_pPersClassID << ">" << endl;
            m_ApplHeader << "inline X2DC_" << m_pPersClass->getName() << "_INDEX" << id << "< " << m_pPersClassID << " >::";
          }
          else {
            m_ApplHeader << "inline X2DC_" << m_pPersClass->getName() << "_INDEX" << id << "::";
          }
          m_ApplHeader << "X2DC_" << m_pPersClass->getName() << "_INDEX" << id << "(";
          char no[11];
          iter.reset();
          cntr = 1;
          while (cntr <= max) {
            if (cntr == 1) {
              m_ApplHeader << " ";
            }
            else {
              m_ApplHeader << ", ";
            }
            memset( no, 0, 11 );
            sprintf( no, "%d", cntr );
            m_ApplHeader << iter()->getType()->getName() << " Mem_" << no;
            iter++;
            cntr++;
          }
          m_ApplHeader << " )" << endl;
          m_ApplHeader << ": m_PersClass_ID(" << m_pPersClassID << ")" << endl;
          m_ApplHeader << ", m_KeyOrInd_ID(" << id << ")" << endl;
          char maxC[11];
          memset( maxC, 0, 11 );
          sprintf( maxC, "%d", max );
          m_ApplHeader << ", m_SpecMem_NO(" << maxC << ")" << endl;
          iter.reset();
          cntr = 1;
          while (cntr <= max) {
            m_ApplHeader << ", ";
            memset( no, 0, 11 );
            sprintf( no, "%d", cntr );
            m_ApplHeader << "m_Mem_" << no << "(Mem_" << no << ")" << endl;
            iter++;
            cntr++;
          }
          m_ApplHeader << "{}" << endl;
        }
        indIter++;
      }

      // Done
      if (m_isWithin_PersClass) {
        m_isWithin_PersClass = false;
      }
      else if (m_isWithin_PersKeyedClass) {
        m_isWithin_PersKeyedClass = false;
      }
      else {
        m_isWithin_PersTemplClass = false;
      }
      delete m_pPersClass;
      m_pPersClass = NULL;
      delete [] m_pPersClassID;
      m_pPersClassID = NULL;
      break;

    }

    /*---------- INSTANTIATE -------------*/

    case elemInstantiate_X2DC : {
      m_ApplHeader << m_Temp_String.data();
      m_Temp_String.erase();
      m_isWithin_Instantiation = false;
      break;
    }

    /*-----------PERSINTERFACE -----------*/

    case elemPersinterface_X2DC : {
      m_ApplHeader << "};" << endl;
      break;
    }

    /*---------- FRIEND ------------------*/

    case elemFriend_X2DC : {
      break;
    }

    /*---------- SECTION -----------------*/

    case elemSection_X2DC : {
      break;
    }

    /*---------- CONSTANTS ---------------*/

    case elemEnum_X2DC : {
      m_ApplHeader << "};" << endl;
      m_isWithin_Enum = 0;
      break;
    }

    case elemGeneralenum_X2DC : {
      m_ApplHeader << "};" << endl;
      m_isWithin_GenEnum = 0;
      break;
    }

    case elemConst_X2DC : {
      break;
    }

    /*---------- MEMBERS -----------------*/

    case elemBitfields_X2DC : {
      delete m_pBfType;
      m_pBfType = NULL;
      break;
    }

    case elemField_X2DC : {
      break;
    }

    case elemClassmember_X2DC : {
      break;
    }

    case elemMember_X2DC : {
      break;
    }

    case elemAlignmentmember_X2DC :
    case elemIntrinsicmember_X2DC : {
      break;
    }

    case elemOidmember_X2DC : {
      Append(m_Temp_String.data());
      m_Temp_String.erase();
      m_isWithin_OidMember = false;
      break;
    }

    case elemExtoidmember_X2DC :
    case elemVaroidmember_X2DC : {
      break;
    }

    case elemTemplmember_X2DC : {
      Append(m_Temp_String.data());
      m_Temp_String.erase();
      m_isWithin_TemplMember = false;
      break;
    }

    case elemClassinstance_X2DC :
    case elemIntinstance_X2DC : {
      break;
    }

    /*---------- TYPEDEF -----------------*/

    case elemTypedef_X2DC : {
      break;
    }

  }

}

/*----------------------------------------------------------------------------*/

void GenApplHeaderFile::genDefconstrEntry( int & error, const char ** atts ) {

  const Cpp_Type * pT = m_pCTR->findType(atts[3]);

  if (!pT) {
    X2DC_ErrMsg( M__TYPE_NOT_FOUND, atts[3] );
    error = 1;
    return;
  }

  if (pT->getDefault()) {
    if ( atts[4] && (attDimension_X2DC == X2DC_getAttributeType(atts[4])) ) {
      int dim = atoi(atts[5]); // atoi(atts[5]) >= 0, see Cpp_Type_Registry elem(Classm/M)ember_X2DC
      if (0 == dim) {
        const int * pDim = m_pCTR->getConstValue(atts[5]);
        if (pDim) {    // if pDim != NULL then *pDim > 0
          dim = *pDim; // see Cpp_Type_Registry elem(Classm/M)ember_X2DC
        }
      }
      if (dim > 0) {
        char iBuf[11];
        for ( int i = 0; i < dim; i++ ) {
          memset( iBuf, 0, sizeof(iBuf) );
          sprintf( iBuf, "%d", i );
          m_DefConstr_Body.append("  ");
          m_DefConstr_Body.append(atts[1]);
          m_DefConstr_Body.append("[");
          m_DefConstr_Body.append(iBuf);
          m_DefConstr_Body.append("] = ");
          m_DefConstr_Body.append(pT->getDefault());
          m_DefConstr_Body.append(";");
          m_DefConstr_Body.append("\n");
        }
      }
      else {
        if ( !m_isWithin_TemplClass && !m_isWithin_PersTemplClass ) {
          X2DC_ErrMsg( M__VALUE_NOT_FOUND, atts[5] );
          error = 1;
          return;
        }
        else {
          if (!m_i_declared) {
            m_DefConstr_Body.append("  int i;");
            m_DefConstr_Body.append("\n");
            m_i_declared = true;
          }
          m_DefConstr_Body.append("  for ( i = 0; i < ");
          m_DefConstr_Body.append(atts[5]);
          m_DefConstr_Body.append("; i++ ) {");
          m_DefConstr_Body.append("\n");
          m_DefConstr_Body.append("    ");
          m_DefConstr_Body.append(atts[1]);
          m_DefConstr_Body.append("[i] = ");
          m_DefConstr_Body.append(pT->getDefault());
          m_DefConstr_Body.append(";");
          m_DefConstr_Body.append("\n");
          m_DefConstr_Body.append("  }");
          m_DefConstr_Body.append("\n");
        }
      }
    }
    else {
      if (2 == m_gen_DefConstr) {
        m_DefConstr_Head.append(": ");
        m_gen_DefConstr = 1;
      }
      else {
        m_DefConstr_Head.append(", ");
      }
      m_DefConstr_Head.append(atts[1]);
      m_DefConstr_Head.append("(");
      m_DefConstr_Head.append(pT->getDefault());
      m_DefConstr_Head.append(")");
      m_DefConstr_Head.append("\n");
    }
  }
  else {
    /* It's a class or structure. Nothing to do. */
  }

}

