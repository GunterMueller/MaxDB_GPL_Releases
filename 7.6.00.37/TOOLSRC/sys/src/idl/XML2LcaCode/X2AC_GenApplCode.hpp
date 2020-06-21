/*!*****************************************************************************

  module      : X2AC_GenApplCode.hpp

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


#ifndef X2AC_GENAPPLCODE_HPP
#define X2AC_GENAPPLCODE_HPP


#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <fstream>
#define ofstream std::ofstream
#define endl     std::endl
#else
#include <fstream.h>
#include <iostream.h>
#endif

#include <string>


#include "idl/XML2LcaCode/XML2LcaCode.hpp"


class GenApplCode
{
public:
  GenApplCode();
  ~GenApplCode();
  void ToLower( const char * upper, char * lower, int transLen = 0 );
  void ToUpper( const char * lower, char * upper, int transLen = 0 );
  bool IsPointer(const char * type);
  bool IsTabhandle(const char * type);
  bool NameTooLong(const char * pName) {return strlen(pName) > maxNameLen;}
  int  Init( int & error, char * RawName, List * PartList, char * Module );
  void StartElement( int & error, XML_Parser parser, const char * name, const char ** atts);
  void EndElement( int & error, const char * name );
private:
  char *      m_pRawName;
  List *      m_pPartList;
  char *      m_pModule;
  bool        m_Inccontent_spec;
  char *      m_pFunctor;
  bool        m_functorHeader;
  char *      m_pGlobals;
  char *      m_pLibrary;
  bool        m_precompHeader;
  bool        m_isWithin_Idl;
  char        m_methodName[maxNameLen+1];
  char        m_param1[maxNameLen+1];
  char        m_param2[maxNameLen+1];
  bool        m_generate;
  char        m_schemaType[maxNameLen+1];
  bool        m_SchemaType_spec;
  char        m_transType[maxNameLen+1];
  bool        m_TransType_spec;
  int         m_paramCntr;
  char        m_tableType[maxNameLen+1];
  ofstream    m_obj_h;
  std::string m_obj_h_privateMembers;
  ofstream    m_obj_cpp;
  std::string m_obj_cpp_execute;
  int         m_obj_cpp_lineLen;
  std::string m_obj_cpp_ctorParams;
  std::string m_obj_cpp_ctorInitList;
  ofstream    m_routines_h;
  std::string m_routines_parameters;
  ofstream    m_routines_cpp;
  std::string m_routines_cpp_ofArguments;
  int         m_routines_cpp_lineLen;
  ofstream    m_ifc_h;
};


#endif /* X2AC_GENAPPLCODE_HPP */

