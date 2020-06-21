/*!*****************************************************************************

  module      : X2DC_GenDisclosureCode.cpp

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


#include "idl/XML2DiscCpp/X2DC_GenDisclosureCode.hpp"


/******************************************************************************/
/*     Implementation of class GenDisclosureCode                              */
/******************************************************************************/

GenDisclosureCode::GenDisclosureCode( int * pError, const Cpp_Type_Registry * pCTR )
: m_pError(pError)
, m_Make76(false)
, m_OffsetCheck(false)
, m_pCTR(pCTR)
, m_CT( "localHelper", false )
, m_pPCName(NULL)
, m_pPTName(NULL)
, m_is_FirstTPI(false)
, m_pPC(NULL)
, m_pFirstMember(NULL)
, m_isWithin_Import(0)
{}

/*----------------------------------------------------------------------------*/

GenDisclosureCode::~GenDisclosureCode() {

  m_pCTR = NULL;

  if ( (!*m_pError) && ( m_pPCName || m_pPC ) ) {
    X2DC_WarnMsg( "Disclosure code generator not cleaned up correctly!", false );
  }

  /* Header file */

  m_DiscoHeader << "#endif";
  m_DiscoHeader << endl << endl;
  m_DiscoHeader.close();

  /* Source file */

  m_DiscoSource.close();

}

/*----------------------------------------------------------------------------*/

int GenDisclosureCode::Init( int & error, char * name, bool make76, bool offsetcheck ) {

  m_Make76      = make76;
  m_OffsetCheck = offsetcheck;

  int len = strlen(name) + strlen("_X2DC_PersClassDesc");
  char * file = new char[len+5];
  strcpy( file, name );
  strcpy( &file[strlen(name)], "_X2DC_PersClassDesc" );
  file[len]   = '.';
  file[len+2] = 0;
  file[len+3] = 0;
  file[len+4] = 0;

  /* Initialize header file */

  file[len+1] = 'h';

  m_DiscoHeader.open(file);
  if (m_DiscoHeader.fail()) {
    X2DC_ErrMsg( M__FILE_OPEN_ERROR, file );
    error = 1;
    return 1;
  }

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
  m_DiscoHeader << endl << endl;
  m_DiscoHeader << "#ifndef " << pName << "_X2DC_PersClassDesc_H" << endl;
  m_DiscoHeader << "#define " << pName << "_X2DC_PersClassDesc_H" << endl;
  m_DiscoHeader << endl << endl;
  m_DiscoHeader << "#include \"" << name << ".h\"" << endl;
  m_DiscoHeader << endl << endl;

  /* Initialize source file */

  file[len+1] = 'c';
  file[len+2] = 'p';
  file[len+3] = 'p';

  m_DiscoSource.open(file);
  if (m_DiscoSource.fail()) {
    X2DC_ErrMsg( M__FILE_OPEN_ERROR, file );
    error = 1;
    return 1;
  }

  m_DiscoSource << endl << endl;
  m_DiscoSource << "#include \"" << name << "_X2DC_PersClassDesc.h\"" << endl;
  m_DiscoSource << endl << endl;

  /* Ok */

  delete [] file;
  file = NULL;

  return 0;

}

/*----------------------------------------------------------------------------*/

void GenDisclosureCode::StartElement( int & error, const char * name, const char ** atts) {

  if (elemImport_X2DC == X2DC_getElementType(name)) {
    m_isWithin_Import++;
    return;
  }
  else if (m_isWithin_Import) {
    return;
  }

  if ( (elemPersclass_X2DC == X2DC_getElementType(name)) ||
       (elemPerskeyedclass_X2DC == X2DC_getElementType(name)) ) {
    m_pPCName = new Cpp_Name(atts[1]);
  }
  else if (elemInstantiate_X2DC == X2DC_getElementType(name)) {
    const Pers_Class * pPC = m_pCTR->findPersClass(atts[1]);
    if ( pPC && (pPC->isTemplate()) ) {
      m_pPTName = new Cpp_Name(atts[1]);
      m_pPCName  = new Cpp_Name(atts[3]);
    }
  }

}

/*----------------------------------------------------------------------------*/

void GenDisclosureCode::EndElement( int & error, const char * name ) {

  if (elemImport_X2DC == X2DC_getElementType(name)) {
    m_isWithin_Import--;
    return;
  }
  else if (m_isWithin_Import) {
    return;
  }

  if ( (elemPersclass_X2DC == X2DC_getElementType(name)) ||
       (elemPerskeyedclass_X2DC == X2DC_getElementType(name)) ||
       ( (elemInstantiate_X2DC == X2DC_getElementType(name)) && m_pPTName ) ) {

    m_pPC = m_pCTR->findPersClass(m_pPCName->getName());
    delete m_pPCName;
    m_pPCName = NULL;

    /* Global variables */

    unsigned int maxLen = 0;
    int memCount = 0;
    List<MemberDescriptor> * pMDList = NULL;

    /* Member(s) */

    pMDList = new List<MemberDescriptor>();
    traverseType( error, NULL, m_pPC->getClass(), pMDList, memCount, maxLen, 1, -SizeOfPointer() );
    if (error) {
      return;
    }

    Iterator<MemberDescriptor> mdIter = pMDList->Begin();
    m_Temp_String = "  // size of pointer";
    m_Temp_String.append("\n");
    m_Temp_String.append("  int PtrSz = ");
    char PtrSz[11];
    sprintf( PtrSz, "%d", SizeOfPointer() );
    m_Temp_String.append(PtrSz);
    m_Temp_String.append(";");
    m_Temp_String.append("\n");
    m_Temp_String.append("  // verify size");
    m_Temp_String.append("\n");
    char PersSz[11];
    sprintf( PersSz, "%d", m_pPC->getSize() );
    int szLen = strlen(PersSz);
    m_Temp_String.append("  if (PtrSz + ");
    m_Temp_String.append(PersSz);
    m_Temp_String.append(" != sizeof(");
    m_Temp_String.append(m_pPC->getName());
    m_Temp_String.append(")) {throw DbpError( -1, \"size mismatch\" );}");
    m_Temp_String.append("\n");
    if ( ( m_Make76 && m_OffsetCheck ) || (!m_Make76) ) {
      m_Temp_String.append("  // verify offsets");
      m_Temp_String.append("\n");
      while (mdIter()) {
        char off[11];
        memset( off, 0, 11 );
        sprintf( off, "%d", mdIter()->getOffset() );
        int len = szLen - strlen(off);
        char * filler = NULL;
        if (len > 0) {
          filler = new char[len+1];
          memset( filler, ' ', len );
          filler[len] = 0;
        }
        if (mdIter()->getVtType() != csp1_vt_lc_bitfields) {
          m_Temp_String.append("  if (PtrSz + ");
          if (len > 0) {
            m_Temp_String.append(filler);
          }
          m_Temp_String.append(off);
          m_Temp_String.append(" != offsetof( ");
          m_Temp_String.append(m_pPC->getName());
          m_Temp_String.append(", ");
          m_Temp_String.append(mdIter()->getName());
          m_Temp_String.append(" )) {throw DbpError( -1, \"offset mismatch\" );}");
        }
        else {
          m_Temp_String.append("  //  PtrSz + ");
          if (len > 0) {
            m_Temp_String.append(filler);
          }
          m_Temp_String.append(off);
          m_Temp_String.append(" :  csp1_vt_lc_bitfields");
        }
        if (len > 0) {
          delete [] filler;
          filler = NULL;
        }
        m_Temp_String.append("\n");
        mdIter++;
      }
    }

    /* 76 make */

    if (m_Make76) {
      m_DiscoSource << m_pPC->getName() << "::";
      if ( (elemPersclass_X2DC == X2DC_getElementType(name)) ||
           (elemPerskeyedclass_X2DC == X2DC_getElementType(name)) ) {
        m_DiscoSource << m_pPC->getName();
      }
      else {
        m_DiscoSource << m_pPTName->getName();
        delete m_pPTName;
        m_pPTName = NULL;
      }
      m_DiscoSource << "() {" << endl;
      m_DiscoSource << m_Temp_String.data();
      m_Temp_String.erase();
      m_DiscoSource << "}" << endl;
      m_DiscoSource << endl << endl;
      m_pPC = NULL;
      return;
    }

    /* Begin of class declaration */

    m_DiscoHeader << "class X2DC_PersClass_ID" << m_pPC->getID() << "_Desc : public X2DC_PersClass" << endl;
    m_DiscoHeader << "{" << endl;
    m_DiscoHeader << "public:" << endl;
    m_DiscoHeader << "  int                   PersistentSize() {return " << m_pPC->getSize() << ";}" << endl;
    m_DiscoHeader << "  int                   Alignment() {return " << m_pPC->getAlignment() << ";}" << endl;
    m_DiscoHeader << "  X2DC_MemDesc_Iterator FirstMember();" << endl;
    m_DiscoHeader << "  X2DC_MemDesc_Iterator FirstKeyMember();" << endl;
    m_DiscoHeader << "  X2DC_Index_Iterator   FirstIndex();" << endl;
    m_DiscoHeader << "private:" << endl;

//    /* Member(s) */
//
//    pMDList = new List<MemberDescriptor>();
//    traverseType( error, NULL, m_pPC->getClass(), pMDList, memCount, maxLen, 1, -SizeOfPointer() );
//    if (error) {
//      return;
//    }

    m_DiscoHeader << "  static X2DC_MemDesc X2DC_ClassMember[" << memCount << "+1];" << endl;

    m_DiscoSource << "X2DC_MemDesc X2DC_PersClass_ID" << m_pPC->getID() << "_Desc::";
    m_DiscoSource << "X2DC_ClassMember[" << memCount << "+1] =" << endl;
    printDescription( 0, NULL, pMDList, maxLen );
    m_DiscoSource << ";" << endl;
    m_DiscoSource << endl;

//    Iterator<MemberDescriptor> mdIter = pMDList->Begin();
//    m_Temp_String = "  // verify offsets";
//    m_Temp_String.append("\n");
//    while (mdIter()) {
//      char off[11];
//      memset( off, 0, 11 );
//      sprintf( off, "%d", mdIter()->getOffset() );
//      int len = 4 - strlen(off);
//      char * filler = NULL;
//      if (len > 0) {
//        filler = new char[len+1];
//        memset( filler, ' ', len );
//        filler[len+1] = 0;
//      }
//      else if (len < 0) {
//        X2DC_ErrMsg( M__OFFSET_TOO_BIG, mdIter()->getName(), m_pPC->getName(), "9999", false );
//        error = 1;
//        return;
//      }
//      if (mdIter()->getVtType() != csp1_vt_lc_bitfields) {
//        m_Temp_String.append("  if (PtrSz + ");
//        if (len > 0) {
//          m_Temp_String.append(filler);
//        }
//        m_Temp_String.append(off);
//        m_Temp_String.append(" != offsetof( ");
//        m_Temp_String.append(m_pPC->getName());
//        m_Temp_String.append(", ");
//        m_Temp_String.append(mdIter()->getName());
//        m_Temp_String.append(" )) { /*error(\"");
//        m_Temp_String.append(mdIter()->getName());
//        m_Temp_String.append("\");*/ return; }");
//      }
//      else {
//        m_Temp_String.append("  //  PtrSz + ");
//        if (len > 0) {
//          m_Temp_String.append(filler);
//        }
//        m_Temp_String.append(off);
//        m_Temp_String.append(" :  csp1_vt_lc_bitfields");
//      }
//      if (len > 0) {
//        delete [] filler;
//        filler = NULL;
//      }
//      m_Temp_String.append("\n");
//      mdIter++;
//    }

    maxLen = 0;
    memCount = 0;
    pMDList->Clear();
    delete pMDList;
    pMDList = NULL;

    m_DiscoSource << "X2DC_MemDesc_Iterator X2DC_PersClass_ID" << m_pPC->getID() << "_Desc::FirstMember() {" << endl;
    m_DiscoSource << "  X2DC_MemDesc_Iterator iter = X2DC_MemDesc_Iterator(X2DC_ClassMember);" << endl;
    m_DiscoSource << "  return iter;" << endl;
    m_DiscoSource << "}" << endl;
    m_DiscoSource << endl;

    /* Key */

    const Index * pKey = m_pPC->getKey();
    if (pKey) {
      Iterator<Index_Member> keyMemIter = pKey->getMemberList()->Begin();
      pMDList = new List<MemberDescriptor>();
      while (keyMemIter()) {
        char * pName = new char[strlen(keyMemIter()->getName())+1];
        removeBaseClassInfo( keyMemIter()->getName(), pName );
        Member * pMember = new Member(pName);
        delete [] pName;
        pName = NULL;
        if (keyMemIter()->isOid()) {
          pMember->setType( error, Oid );
        }
        else {
          pMember->setType( error, CppType, keyMemIter()->getType() );
        }
        if (error) {
          return;
        }
        pMember->setDimension( error, keyMemIter()->getDimension() );
        if (error) {
          return;
        }
        pMember->setOffset(keyMemIter()->getOffset());
        traverseMember( error, NULL, pMDList, memCount, maxLen, pMember, 1, -SizeOfPointer() );
        if (error) {
          return;
        }
        keyMemIter++;
      }
      m_DiscoHeader << "  static X2DC_MemDesc X2DC_KeyMember[" << memCount << "+1];" << endl;
      m_DiscoSource << "X2DC_MemDesc X2DC_PersClass_ID" << m_pPC->getID() << "_Desc::";
      m_DiscoSource << "X2DC_KeyMember[" << memCount << "+1] =" << endl;
      printDescription( 0, NULL, pMDList, maxLen );
      m_DiscoSource << ";" << endl;
    }
    else {
      m_DiscoHeader << "  static X2DC_MemDesc * X2DC_KeyMember;" << endl;
      m_DiscoSource << "X2DC_MemDesc * X2DC_PersClass_ID" << m_pPC->getID() << "_Desc::";
      m_DiscoSource << "X2DC_KeyMember = NULL;" << endl;
    }
    m_DiscoSource << endl;

    m_DiscoSource << "X2DC_MemDesc_Iterator X2DC_PersClass_ID" << m_pPC->getID() << "_Desc::FirstKeyMember() {" << endl;
    m_DiscoSource << "  X2DC_MemDesc_Iterator iter = X2DC_MemDesc_Iterator(X2DC_KeyMember);" << endl;
    m_DiscoSource << "  return iter;" << endl;
    m_DiscoSource << "}" << endl;
    m_DiscoSource << endl;

    /* Secondary index(es) */

    List<IndexDescriptor> * pIDList = new List<IndexDescriptor>();
    Iterator<Index> indIter = m_pPC->getIndexList()->Begin();
    int indCount = 0;
    int maxMemCount = 0;

    while (indIter()) {
      indCount++;
      maxLen = 0;
      memCount = 0;
      Iterator<Index_Member> indMemIter = indIter()->getMemberList()->Begin();
      pMDList = new List<MemberDescriptor>();
      while (indMemIter()) {
        char * pName = new char[strlen(indMemIter()->getName())+1];
        removeBaseClassInfo( indMemIter()->getName(), pName );
        Member * pMember = new Member(pName);
        delete [] pName;
        pName = NULL;
        if (indMemIter()->isOid()) {
          pMember->setType( error, Oid );
        }
        else {
          pMember->setType( error, CppType, indMemIter()->getType() );
        }
        if (error) {
          return;
        }
        pMember->setDimension( error, indMemIter()->getDimension() );
        if (error) {
          return;
        }
        pMember->setOffset(indMemIter()->getOffset());
        traverseMember( error, NULL, pMDList, memCount, maxLen, pMember, 1, -SizeOfPointer() );
        if (error) {
          return;
        }
        indMemIter++;
      }
      IndexDescriptor * pID = new IndexDescriptor( indIter()->getID(), maxLen, pMDList );
      pIDList->Append(pID);
      pMDList = NULL;
      maxMemCount = (memCount > maxMemCount) ? memCount : maxMemCount;
      indIter++;
    }

    if (indCount) {
      m_DiscoHeader << "  static X2DC_MemDesc ";
      m_DiscoHeader << "X2DC_IndexMember[" << indCount << "][1+" << maxMemCount << "+1];" << endl;
      m_DiscoSource << "X2DC_MemDesc X2DC_PersClass_ID" << m_pPC->getID() << "_Desc::";
      m_DiscoSource << "X2DC_IndexMember[" << indCount << "][1+" << maxMemCount << "+1] =" << endl;
      m_DiscoSource << "{" <<endl;
      Iterator<IndexDescriptor> IDIter = pIDList->Begin();
      while (IDIter()) {
        char id[11];
        memset( id, 0, 11 );
        sprintf( id, "%d", IDIter()->getID() );
        printDescription( 2, id, IDIter()->getMDList(), IDIter()->getMaxLen() );
        IDIter++;
        if (IDIter()) {
          m_DiscoSource << ",";
        }
        m_DiscoSource << endl;
      }
      m_DiscoSource << "};" << endl;
      m_DiscoSource << endl;
    }
    else {
      m_DiscoHeader << "  static X2DC_MemDesc * X2DC_IndexMember;" << endl;
      m_DiscoSource << "X2DC_MemDesc * X2DC_PersClass_ID" << m_pPC->getID() << "_Desc::";
      m_DiscoSource << "X2DC_IndexMember = NULL;" << endl;
      m_DiscoSource << endl;
    }

    pIDList->Clear();
    delete pIDList;
    pIDList = NULL;

    m_DiscoSource << "X2DC_Index_Iterator X2DC_PersClass_ID" << m_pPC->getID() << "_Desc::FirstIndex() {" << endl;
    m_DiscoSource << "  X2DC_Index_Iterator iter = X2DC_Index_Iterator( ";
    if (indCount) {
      m_DiscoSource << "&X2DC_IndexMember[0][0], " << indCount << ", 1+" << maxMemCount << "+1";
    }
    else {
      m_DiscoSource << "X2DC_IndexMember, 0, 0";
    }
    m_DiscoSource << " );" << endl;
    m_DiscoSource << "  return iter;" << endl;
    m_DiscoSource << "}" << endl;
    m_DiscoSource << endl;

    /* Special constructor for pers. class */

    m_DiscoSource << m_pPC->getName() << "::";
    if ( (elemPersclass_X2DC == X2DC_getElementType(name)) ||
         (elemPerskeyedclass_X2DC == X2DC_getElementType(name)) ) {
      m_DiscoSource << m_pPC->getName();
    }
    else {
      m_DiscoSource << m_pPTName->getName();
      delete m_pPTName;
      m_pPTName = NULL;
    }
    m_DiscoSource << "( X2DC_ClassID id, X2DC_PersClass ** pIF ) {" << endl;
    m_DiscoSource << "  // initialize interface pointer" << endl;
    m_DiscoSource << "  *pIF = NULL;" << endl;
//    m_DiscoSource << "  // size of pointer" << endl;
//    m_DiscoSource << "  int PtrSz = " << SizeOfPointer() << ";" << endl;
    m_DiscoSource << m_Temp_String.data();
    m_Temp_String.erase();
    m_DiscoSource << "  // get interface pointer" << endl;
    m_DiscoSource << "  if (id == " << m_pPC->getID() << ") {" << endl;
    m_DiscoSource << "    *pIF = static_cast<X2DC_PersClass *>";
    m_DiscoSource << "(new X2DC_PersClass_ID" << m_pPC->getID() << "_Desc);" << endl;
    m_DiscoSource << "  }" << endl;
    m_DiscoSource << "}" << endl;
    m_DiscoSource << endl << endl;

    /* End of class declaration */

    m_DiscoHeader << "};" << endl;
    m_DiscoHeader << endl << endl;

    /* Done */

    m_pPC = NULL;

  }

}

/*----------------------------------------------------------------------------*/

void GenDisclosureCode::traverseType( int & error, const char * pPreFix, const Cpp_Type * pType,
                                      List<MemberDescriptor> * pMDList, int & memCount,
                                      unsigned int & maxLen, int level, int offset ) {

  if (!pType) {
    X2DC_ErrMsg( M__NULL_POINTER, "X2DC_GenDisclosureCode.cpp", "pType", "traverseType", false );
    error = 1;
    return;
  }

  if (!pMDList) {
    X2DC_ErrMsg( M__NULL_POINTER, "X2DC_GenDisclosureCode.cpp", "pMDList", "traverseType", false );
    error = 1;
    return;
  }

  /* Traverse base classes */

  if (pType->isDerived()) {
    Iterator<Base_Class> bcIter = pType->getBaseClassList()->Begin();
    while(bcIter()) {
      traverseType( error, pPreFix, bcIter()->getType(), pMDList,
                    memCount, maxLen, level, offset+bcIter()->getOffset() );
      if (error) {
        return;
      }
      bcIter++;
    }
  }

  /* Traverse members */

  if (pType->hasMember()) {
    Iterator<Member> memIter = pType->getMemberList()->Begin();
    while (memIter()) {
      traverseMember( error, pPreFix, pMDList, memCount, maxLen, memIter(), level, offset );
      if (error) {
        return;
      }
      memIter++;
    }
  }

}

/*----------------------------------------------------------------------------*/

void GenDisclosureCode::traverseMember( int & error, const char * pPreFix, List<MemberDescriptor> * pMDList,
                                        int & memCount, unsigned int & maxLen,
                                        const Member * pMember, int level, int offset ) {

  if (!pMember) {
    X2DC_ErrMsg( M__NULL_POINTER, "X2DC_GenDisclosureCode.cpp", "pMember", "traverseMember", false );
    error = 1;
    return;
  }

  if (m_CT.getMemberVtType(pMember) == csp1_vt_empty) {
    X2DC_ErrMsg( M__BAD_MEMBER, "VtType", pMember->getName(), "csp1_vt_empty" );
    error = 1;
    return;
  }

  MemberDescriptor * pMD = NULL;
  char * pName = NULL;
  int len = 0;

  /* New leaf member */

  // Name
  if (pPreFix) {
    len = strlen(pPreFix) + 1;
  }
  pName = new char[len+strlen(pMember->getName())+1];
  if (pPreFix) {
    strcpy( pName, pPreFix );
    pName[strlen(pPreFix)] = '.';
  }
  strcpy( &pName[len], pMember->getName() );
  pMD = new MemberDescriptor(pName);
  if (strlen(pName) > maxLen) {
    maxLen = strlen(pName);
  }
  // Level
  pMD->setLevel(level);
  // VT type
//  if (pMember->getDimension() > 1) {
//    if (m_CT.getMemberVtType(pMember) == csp1_vt_lc_char) {
//      pMD->setVtType(csp1_vt_carray);
//    }
//    else {
//      pMD->setVtType(csp1_vt_lc_userdefined_array);
//    }
//  }
//  else {
    pMD->setVtType(m_CT.getMemberVtType(pMember));
    if (m_CT.getMemberVtType(pMember) == csp1_vt_lc_oid) {
      if (!pMember->getOidPersclassPtr()) {
        X2DC_ErrMsg( M__NULL_OID_PERSCLASSPTR, pMember->getName() );
        error = 1;
        return;
      }
      else if (pMember->getOidPersclassPtr()->getID() == 0) {
        X2DC_ErrMsg( M__ZEROID_OID_PERSCLASSPTR, pMember->getName() );
        error = 1;
        return;
      }
      else {
        pMD->setClassID(pMember->getOidPersclassPtr()->getID());
      }
    }
//  }
  // Dimension
  pMD->setDimension(pMember->getDimension());
  // Size
  pMD->setSize(m_CT.getMemberSize( error, pMember ));
  if (error) {
    return;
  }
  // Alignment
  pMD->setAlignment(m_CT.getMemberAlignment( error, pMember ));
  if (error) {
    return;
  }
  // Offset
  pMD->setOffset(offset+pMember->getOffset());
  // Add
  pMDList->Append(pMD);
  memCount++;

  /* VT type == csp1_vt_lc_char */

//  if (m_CT.getMemberVtType(pMember) == csp1_vt_lc_char) {
//    delete [] pName;
//    pName = NULL;
//    return;
//  }

  /* VT type != csp1_vt_lc_char */

//  if (pMember->getDimension() > 1) {
  if ( (pMember->getDimension() > 1) && (m_CT.getMemberVtType(pMember) == csp1_vt_userdefined) ) {
    int dim = pMember->getDimension();
    int len = strlen(pName);
    int d = (int)(floor(log10((double)dim)+(double)1));
    char * pN = new char[len+1+d+1+1]; // [-max-d-digits-]\0
    strcpy( pN, pName );
    pN[len] = '[';
    memset( &pN[len+1], 0, d+2 );
    for ( int i = 0; i < dim; i++ ) {
      sprintf( &pN[len+1], "%d]", i );
      if (strlen(pN) > maxLen) {
        maxLen = strlen(pN);
      }
      switch (pMember->getKind()) {
        case Oid : 
        case ExtOid :
        case VarOid : {
          pMD = new MemberDescriptor(pN);
          pMD->setLevel(level+1);
          if (pMember->getKind() == Oid) {
            pMD->setVtType(csp1_vt_lc_oid);
          }
          else if (pMember->getKind() == ExtOid) {
            pMD->setVtType(csp1_vt_lc_extoid);
          }
          else { // VarOid
            pMD->setVtType(csp1_vt_lc_varoid);
          }
          pMD->setDimension(1);
          pMD->setSize(8);
          pMD->setAlignment(4);
          pMD->setOffset(offset + pMember->getOffset() + i*SizeOfOid());
          pMDList->Append(pMD);
          memCount++;
          break;
        }
        case CppType : {
          const Cpp_Type * pCT = pMember->getCpptypePtr();
          pMD = new MemberDescriptor(pN);
          pMD->setLevel(level+1);
          pMD->setVtType(m_CT.getMemberVtType(pMember));
          pMD->setDimension(1);
          pMD->setSize(pCT->getSize());
          pMD->setAlignment(pCT->getAlignment());
          pMD->setOffset(offset + pMember->getOffset() + i*pCT->getSize());
          pMDList->Append(pMD);
          memCount++;
          // Members of base classes
          if (pCT->isDerived()) {
            Iterator<Base_Class> bcIter = pCT->getBaseClassList()->Begin();
            while(bcIter()) {
              traverseType( error, pN, bcIter()->getType(), pMDList, memCount, maxLen, level + 2,
                            offset + pMember->getOffset() + i*pCT->getSize() + bcIter()->getOffset() );
              if (error) {
                return;
              }
              bcIter++;
            }
          }
          // Own members
          if (pCT->hasMember()) {
            Iterator<Member> iter = pCT->getMemberList()->Begin();
            while (iter()) {
              traverseMember( error, pN, pMDList, memCount, maxLen, iter(), level+2,
                              offset + pMember->getOffset() + i*pCT->getSize() );
              if (error) {
                return;
              }
              iter++;
            }
          }
          break;
        }
        default : {
          X2DC_ErrMsg( M__DIM_GT1_FOR_MEMBER, pMember->getName() );
          error = 1;
          return;
        }
      }
    }
    delete [] pN;
    pN = NULL;
  }
  else {
    // Traverse member if Kind is CppType
    const Cpp_Type * pCT = pMember->getCpptypePtr();
    if (pCT) {
      // Members of base classes
      if (pCT->isDerived()) {
        Iterator<Base_Class> bcIter = pCT->getBaseClassList()->Begin();
        while(bcIter()) {
          traverseType( error, pName, bcIter()->getType(), pMDList, memCount, maxLen,
                        level+1, offset+pMember->getOffset()+bcIter()->getOffset() );
          if (error) {
            return;
          }
          bcIter++;
        }
      }
      // Own members
      if (pCT->hasMember()) {
        Iterator<Member> iter = pCT->getMemberList()->Begin();
        while (iter()) {
          traverseMember( error, pName, pMDList, memCount, maxLen, iter(), level+1, offset+pMember->getOffset() );
          if (error) {
            return;
          }
          iter++;
        }
      }
    }

  }

  /* Done */

  delete [] pName;
  pName = NULL;

}

/*----------------------------------------------------------------------------*/

void GenDisclosureCode::printDescription( int offset, const char * pIndexID,
                                          List<MemberDescriptor> * pMDList, int maxLen ) {

  if (pIndexID) {
    addNTimesCharToDS( offset, ' ' );
    m_DiscoSource << "{ //                    ID" << endl;
    addNTimesCharToDS( offset+2, ' ' );
    m_DiscoSource << "X2DC_MemDesc( (char*)\"" << pIndexID << "\" )," << endl;
    addNTimesCharToDS( offset, ' ' );
    m_DiscoSource << "  //                    Member";
    //                  X2DC_MemDesc( (char*)"...<maxLen>...",
    addBlanksToDS( maxLen+2, "Member" );
    addNTimesCharToDS( 3, ' ' );
    m_DiscoSource << "Level   VT_Type   PersClass_ID   Dimension   Size   Alignment   Offset" << endl;
  }
  else {
    addNTimesCharToDS( offset, ' ' );
    m_DiscoSource << "{ //                    Member";
    //                  X2DC_MemDesc( (char*)"...<maxLen>...",
    addBlanksToDS( maxLen+2, "Member" );
    addNTimesCharToDS( 3, ' ' );
    m_DiscoSource << "Level   VT_Type   PersClass_ID   Dimension   Size   Alignment   Offset" << endl;
  }

  Iterator<MemberDescriptor> deIter = pMDList->Begin();
  while (deIter()) {
    addNTimesCharToDS( offset+2, ' ' );
    m_DiscoSource << "X2DC_MemDesc( ";
    // Name
    m_DiscoSource << "(char*)\"" << deIter()->getName() << "\",";
    addBlanksToDS( maxLen, deIter()->getName() );
    addNTimesCharToDS( 6-(maxLen+2), ' ');
    addNTimesCharToDS( 3, ' ' );
    // Level
    int level = deIter()->getLevel();
    m_DiscoSource << level << ",";
    addBlanksToDS( strlen("Level")-1, level );
    addNTimesCharToDS( 3, ' ' );
    // VT type
    short vtT = deIter()->getVtType();
    m_DiscoSource << vtT << ",";
    addBlanksToDS( strlen("VT_Type")-1, vtT );
    addNTimesCharToDS( 3, ' ' );
    // PersClass ID
    int ClassID = deIter()->getClassID();
    m_DiscoSource << ClassID << ",";
    addBlanksToDS( strlen("PersClass_ID")-1, ClassID );
    addNTimesCharToDS( 3, ' ' );
    // Dimension
    int dim = deIter()->getDimension();
    m_DiscoSource << dim << ",";
    addBlanksToDS( strlen("Dimension")-1, dim );
    addNTimesCharToDS( 3, ' ' );
    // Size
    int sz = deIter()->getSize();
    m_DiscoSource << sz << ",";
    addBlanksToDS( strlen("Size")-1, sz );
    addNTimesCharToDS( 3, ' ' );
    // Alignment
    int ali = deIter()->getAlignment();
    m_DiscoSource << ali << ",";
    addBlanksToDS( strlen("Alignment")-1, ali );
    addNTimesCharToDS( 3, ' ' );
    // Offset
    int off = deIter()->getOffset();
    m_DiscoSource << off;
    addBlanksToDS( strlen("Offset"), off );
    // End line
    m_DiscoSource << ")," << endl;
    deIter++;
  }

  addNTimesCharToDS( offset+2, ' ' );
  m_DiscoSource << "X2DC_MemDesc( )" << endl;
  addNTimesCharToDS( offset, ' ' );
  m_DiscoSource << "}";

}

/*----------------------------------------------------------------------------*/

void GenDisclosureCode::removeBaseClassInfo(const char * pOldName, char * pNewName) {

  char * name = NULL;
  unsigned int startPos = 0, writePos = 0;

  while (strchr( pOldName+startPos, '.' )) {
    unsigned int dotPos = strcspn( pOldName+startPos, "." );
    name = new char[dotPos+1];
    strncpy( name, pOldName+startPos, dotPos );
    name[dotPos] = 0;
    startPos += dotPos + 1;
    unsigned int readPos = 0;
    while (strchr( name+readPos, ':' )) {
      readPos += strcspn( name+readPos, ":" ) + 1;
    }
    strcpy( pNewName+writePos, name+readPos );
    pNewName[writePos+strlen(name+readPos)] = '.';
    writePos += strlen(name+readPos) + 1;
  }

  unsigned int readPos = 0;
  while (strchr( pOldName+startPos+readPos, ':' )) {
    readPos += strcspn( pOldName+startPos+readPos, ":" ) + 1;
  }
  strcpy( pNewName+writePos, pOldName+startPos+readPos );
  pNewName[writePos+strlen(pOldName+startPos)] = 0;

}

