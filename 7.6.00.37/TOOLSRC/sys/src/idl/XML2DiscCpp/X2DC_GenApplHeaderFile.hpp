/*!*****************************************************************************

  module      : X2DC_GenApplHeaderFile.hpp

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


#ifndef X2DC_GENAPPLHEADERFILE_HPP
#define X2DC_GENAPPLHEADERFILE_HPP


//#if defined(_WIN32) && (_MSC_VER >= 1300)
//#include <fstream>
//#define  ofstream     std::ofstream
//#else
//#include <iostream.h>
//#endif
//
//#include <string>


#include <stdio.h>

#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#include <fstream>
#define endl     std::endl
#define ofstream std::ofstream
#else
#include <iostream.h>
#include <string.h>
#include <fstream.h>
#endif

#include "idl/XML2DiscCpp/X2DC_Cpp_Type_Registry.hpp"


class Def_Constr
{
public:
  /* Constructor */
  inline Def_Constr( const char * pName, const char * pType );
  /* Destructor */
  ~Def_Constr() {};
  /* Accessors */
  const char * getName() {return m_name.getName();}
  const char * getType() {return m_type;}
private:
  Cpp_Name m_name;
  char     m_type[10]; // public, private, protected
};

Def_Constr::Def_Constr( const char * pName, const char * pType )
: m_name(pName)
{
  strcpy( m_type, pType );
  m_type[strlen(pType)] = 0;
}

/*----------------------------------------------------------------------------*/

typedef enum {
  ApplHeader,
  PersClassHead,
  PersClassBody,
} WriteTo;

/*----------------------------------------------------------------------------*/

class GenApplHeaderFile
{
public:
  /* Constructor */
  GenApplHeaderFile( int * pError, const Cpp_Type_Registry * pCTR );
  /* Destructor */
  ~GenApplHeaderFile();
  /* Others */
  int         Init( int & error, char * name, bool make76, bool lcapps );
  void        StartElement( int & error, const char * name, const char ** atts );
  void        EndElement(const char * name);
  void        genDefconstrEntry( int & error, const char ** atts );
  inline void Append(const char * pStr);
//  inline void Append(int value);
  inline void EndLine();
private:
  int *                     m_pError;
  bool                      m_Make76;
  const Cpp_Type_Registry * m_pCTR;
  ofstream                  m_ApplHeader;
  char *                    m_pIncludeBlocker;
  int                       m_isWithin_Import;
  bool                      m_isWithin_Struct;
  bool                      m_isWithin_Union;
  bool                      m_isWithin_Class;
  bool                      m_isWithin_PersClass;
  bool                      m_isWithin_PersKeyedClass;
  bool                      m_isWithin_TemplClass;
  bool                      m_isWithin_TemplParams;
  bool                      m_isWithin_PersTemplClass;
  bool                      m_isWithin_Instantiation;
  bool                      m_isWithin_PersInstantiation;
  Cpp_Name *                m_pPersClass;
  Cpp_Name *                m_pPersTemplName;
  int                       m_gen_DefConstr;
  bool                      m_i_declared;
  bool                      m_isWithin_OidMember;
  bool                      m_isWithin_TemplMember;
  bool                      m_isFirst_TemplParam;
  char *                    m_pPersClassID;
  char *                    m_pPersTemplPBC;
  std::string               m_PersClass_Head;
  std::string               m_PersClass_Body;
  std::string               m_Temp_String;
  std::string               m_DefConstr_Head;
  std::string               m_DefConstr_Body;
  Cpp_Name *                m_pBfType;
  int                       m_isWithin_Enum;
  int                       m_isWithin_GenEnum;
  Def_Constr *              m_pDefConstr;
  char *                    m_pPcidEnum;
  bool                      m_isFirst_BaseClass;
  bool                      m_print_BaseClasses;
  bool                      m_write_LeftBrace;
  WriteTo                   m_write_to;
};

inline void GenApplHeaderFile::Append(const char * pStr) {
  switch (m_write_to) {
    case PersClassHead : {
      if (m_PersClass_Head.empty()) {
        m_PersClass_Head = pStr;
      }
      else {
        m_PersClass_Head.append(pStr);
      }
      break;
    }
    case PersClassBody : {
      if (m_PersClass_Body.empty()) {
        m_PersClass_Body = pStr;
      }
      else {
        m_PersClass_Body.append(pStr);
      }
      break;
    }
    case ApplHeader : {
      m_ApplHeader << pStr;
      break;
    }
  }
}
/*
inline void GenApplHeaderFile::Append(int value) {
  switch (m_write_to) {
    case PersClassHead : {
      if (m_PersClass_Head.empty()) {
        m_PersClass_Head = value;
      }
      else {
        m_PersClass_Head.append(value); 
      }
      break;
    }
    case PersClassBody : {
      if (m_PersClass_Body.empty()) {
        m_PersClass_Body = value;
      }
      else {
        m_PersClass_Body.append(value); 
      }
      break;
    }
    case ApplHeader : {
      m_ApplHeader << value;
      break;
    }
  }
}
*/
inline void GenApplHeaderFile::EndLine() {
  switch (m_write_to) {
    case PersClassHead : {
      if (m_PersClass_Head.empty()) {
        m_PersClass_Head = "\n";
      }
      else {
        m_PersClass_Body.append("\n");
      }
      break;
    }
    case PersClassBody : {
      if (m_PersClass_Body.empty()) {
        m_PersClass_Body = "\n";
      }
      else {
        m_PersClass_Body.append("\n");
      }
      break;
    }
    case ApplHeader : {
      m_ApplHeader << endl;
      break;
    }
  }
}


#endif /* X2DC_GENAPPLHEADERFILE_HPP */

