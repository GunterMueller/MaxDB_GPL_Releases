/*!*****************************************************************************

  module      : X2DC_GenDisclosureCode.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of persistent class description code
  description : Generate code disclosing structure of persistent classes
				
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


#ifndef X2DC_GENDISCLOSURECODE_HPP
#define X2DC_GENDISCLOSURECODE_HPP


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

#include <math.h>

#include "idl/XML2DiscCpp/X2DC_Cpp_Type_Registry.hpp"


class MemberDescriptor {
public:
  /* Constructor */
  inline MemberDescriptor(const char * pName);
  /* Destructor */
  ~MemberDescriptor() {};
  /* Accessors */
  const char * getName() const {return m_name.getName();}
  void         setLevel(int level) {m_level = level;}
  int          getLevel() const {return m_level;}
  void         setVtType(short VtType) {m_VtType = VtType;}
  short        getVtType() const {return m_VtType;}
  void         setClassID(int ClassID) {m_ClassID = ClassID;}
  int          getClassID() const {return m_ClassID;}
  void         setDimension(int dimension) {m_dimension = dimension;}
  int          getDimension() const {return m_dimension;}
  void         setSize(int size) {m_size = size;}
  int          getSize() const {return m_size;}
  void         setAlignment(int alignment) {m_alignment = alignment;}
  int          getAlignment() const {return m_alignment;}
  void         setOffset(int offset) {m_offset = offset;}
  int          getOffset() const {return m_offset;}
private:
  Cpp_Name m_name;
  int      m_level;
  short    m_VtType;
  int      m_ClassID;
  int      m_dimension;
  int      m_size;
  int      m_alignment;
  int      m_offset;
};

inline MemberDescriptor::MemberDescriptor(const char * pName)
: m_name(pName)
, m_level(0)
, m_VtType(csp1_vt_empty)
, m_ClassID(0)
, m_dimension(0)
, m_size(0)
, m_alignment(0)
, m_offset(0)
{}

/*----------------------------------------------------------------------------*/

class IndexDescriptor {
public:
  IndexDescriptor( int ID, unsigned int maxLen, List<MemberDescriptor> * pMDList )
   : m_ID(ID), m_maxLen(maxLen), m_pMDList(pMDList) {};
  inline ~IndexDescriptor();
  int                      getID() const {return m_ID;}
  unsigned int             getMaxLen() const {return m_maxLen;}
  List<MemberDescriptor> * getMDList() const {return m_pMDList;}
private:
  int                      m_ID;
  unsigned int             m_maxLen;
  List<MemberDescriptor> * m_pMDList;
};

inline IndexDescriptor::~IndexDescriptor() {
  m_pMDList->Clear();
  delete m_pMDList;
  m_pMDList = NULL;
}

/*----------------------------------------------------------------------------*/

typedef enum {
  ClassBase,
  MemtypeBase,
  NoBase
} BaseClass_Kind;

/*----------------------------------------------------------------------------*/

class GenDisclosureCode
{
public:
  /* Constructor */
  GenDisclosureCode( int * pError, const Cpp_Type_Registry * pCTR );
  /* Destructor */
  ~GenDisclosureCode();
  /* Others */
  void        traverseType( int & error, const char * pPreFix, const Cpp_Type * pType,
                            List<MemberDescriptor> * pMDList, int & memCount,
                            unsigned int & maxLen, int level, int offset );
  void        traverseMember( int & error, const char * pPreFix, List<MemberDescriptor> * pMDList,
                              int & memCount, unsigned int & maxLen,
                              const Member * pMember, int level, int offset );
  void        removeBaseClassInfo( const char * pOldName, char * pNewName );
  void        printDescription( int offset, const char * pIndexID, List<MemberDescriptor> * pMDList, int maxLen );
  inline void addBlanksToDS( int max, int value );
  inline void addBlanksToDS( int max, const char * pName );
  inline void addNTimesCharToDS( int N, char c );
  int         Init( int & error, char * name, bool make76, bool offsetcheck );
  void        StartElement( int & error, const char * name, const char ** atts );
  void        EndElement( int & error, const char * name );
private:
  int *                     m_pError;
  bool                      m_Make76;
  bool                      m_OffsetCheck;
  const Cpp_Type_Registry * m_pCTR;
  Cpp_Type                  m_CT;
  ofstream                  m_DiscoHeader;
  ofstream                  m_DiscoSource;
  Cpp_Name *                m_pPCName;
  Cpp_Name *                m_pPTName;
  const Pers_Class *        m_pPC;
  Cpp_Name *                m_pFirstMember;
  std::string               m_Temp_String;
  bool                      m_is_FirstTPI;
  int                       m_isWithin_Import;
};

inline void GenDisclosureCode::addBlanksToDS( int max, int value ) {
  int d = 1;
  if (value > 0) {
    d = (int)(floor(log10((double)value)+(double)1));
  }
  else if (value < 0) {
    d = (int)(floor(log10((double)(-value))+(double)1)) + 1;
  }
  if (d < max) {
    char * blBuf = new char[(max-d)+1];
    memset( &blBuf[0], ' ', (max-d) );
    blBuf[(max-d)] = 0;
    m_DiscoSource << blBuf;
    delete [] blBuf;
  }
}

inline void GenDisclosureCode::addBlanksToDS( int max, const char * pName ) {
  int d = max - strlen(pName);
  if (d > 0) {
    char * blBuf = new char[d+1];
    memset( &blBuf[0], ' ', d );
    blBuf[d] = 0;
    m_DiscoSource << blBuf;
    delete [] blBuf;
  }
}

inline void GenDisclosureCode::addNTimesCharToDS( int N, char c ) {
  char cBuf[2];
  cBuf[0] = c;
  cBuf[1] = 0;
  for ( int i = 0; i < N; i++ ) {
    m_DiscoSource << cBuf;
  }
}


#endif /* X2DC_GENDISCLOSURECODE_HPP */

