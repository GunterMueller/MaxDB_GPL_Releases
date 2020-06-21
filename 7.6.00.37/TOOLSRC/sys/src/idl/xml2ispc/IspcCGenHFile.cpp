/*!*****************************************************************************

  module      : IspcCGenHFile.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : Generate <project>.h and <project>_i.c

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


#include <ctype.h>
#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <fstream>
#define endl std::endl
#else
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#endif

#include "idl/xml2ispc/IspcCGenHFile.h"
#include "idl/xml2ispc/IspcErrorHandling.h" /* PTS 1131325, PG */
#ifndef ISPCIDL2CODE_H
#include "idl/xml2ispc/IspcIdl2Code.h"
#endif


/******************************************************************************/
/*     IspcCGenHFile                                                          */
/******************************************************************************/

IspcCGenHFile::IspcCGenHFile() {
  m_is_within_import  = '\0';
  m_is_within_include = '\0';
  m_is_within_struct  = 0;
  m_is_within_union   = 0;     /* PTS 1141233, PG */
  m_is_within_enum    = 0;     /* PTS 1131325, PG */
  m_is_livecachetypes = 0;     /* PTS 1131325, PG */
  m_is_cppsqldbc_sec  = false; /* PTS 1138438, PG */
  m_gen_def_constr    = 0;     /* PTS 1131325, PG */
  m_i_declared        = false; /* PTS 1134734, PG */
  m_gen_tab_handle    = false; /* PTS 1131325, PG */
  m_features_76       = false; /* PTS 1131325, PG */
  m_typedef_check     = false; /* PTS 1131325, PG */
  m_ifStruct          = NULL;  /* PTS 1134618, PG */
  /* PTS 1134734, PG */
  m_ifConst           = NULL;
  memset( m_constname, '\0', sizeof(m_constname) );
}

/*----------------------------------------------------------------------------*/

IspcCGenHFile::~IspcCGenHFile() {
  m_out_h << endl; /* PTS 1110140 */
  m_out_h.close();
  m_out_c << endl; /* PTS 1110140 */
  m_out_c.close();
}

/*----------------------------------------------------------------------------*/

int IspcCGenHFile::Init( char* cFilename, bool features_76, bool typedef_check ) {

  m_is_within_import  = '\0';
  m_is_within_include = '\0';
  m_is_within_struct  = 0;
  m_is_livecachetypes = !strncmp( cFilename, "OMS_LiveCacheTypes" , 18 );
  m_features_76       = features_76;                      /* PTS 1131325, PG */
  m_typedef_check     = ( features_76 && typedef_check ); /* PTS 1131325, PG */
  strcpy( &m_filename[0], cFilename );

  char * pc = strrchr( &m_filename[0], '.' );
  if (pc) {
    *pc = '\0';
  }
  if (m_features_76) { /* PTS 1131325, PG */
    int len = strlen(&m_filename[0]) - 5;
    if (0 == strncmp( &m_filename[len], "__mgd", 5 )) {
      m_filename[len] = '\0';
    }
  }
# if defined(_WIN32)
    pc = strrchr( m_filename, '\\' );
# else
    pc = strrchr( m_filename, '/' );
# endif
  if (pc) {
    strcpy( m_name, pc+1 );
  }
  else {
    strcpy( m_name, &m_filename[0] );
  }

  m_out_h.open(strcat( &m_filename[0], ".h" ));
  if (m_out_h.fail()) {
    return 0;
  }
  strcpy( &m_filename[0], m_name );
  m_out_c.open(strcat( &m_filename[0], "_i.c") );
  if (m_out_c.fail()) {
    return 0;
  }

  /* PTS 1131325, PG */
  if (!m_is_livecachetypes) {
    m_out_h << "#ifndef __" << &m_name[0] << "_h__"                          << endl;
    m_out_h << "#define __" << &m_name[0] << "_h__"                          << endl;
    m_out_h                                                                  << endl;
# if defined(_WIN32)
    m_out_h << "#include \"windows.h\" /* no check */"                       << endl; /* PTS 1112974 */
    m_out_h                                                                  << endl; /* PTS 1140335 */
# endif
    m_out_h << "#ifndef __I" << &m_name[0] << "_FWD_DEFINED__"               << endl;
    m_out_h << "#define __I" << &m_name[0] << "_FWD_DEFINED__"               << endl;
    m_out_h << "typedef struct I" << &m_name[0] << " I" << &m_name[0] << ";" << endl; /* PTS XXXXXX */
    m_out_h << "#endif  /* __I" << &m_name[0] << "_FWD_DEFINED__ */"         << endl;
    m_out_h                                                                  << endl;
    m_out_h << "#ifndef __" << &m_name[0] << "_FWD_DEFINED__"                << endl;
    m_out_h << "#define __" << &m_name[0] << "_FWD_DEFINED__"                << endl;
    m_out_h                                                                  << endl;
    m_out_h << "#ifdef __cplusplus"                                          << endl;
    m_out_h << "typedef class " << &m_name[0] << " " << &m_name[0] << ";"    << endl;
    m_out_h << "#else"                                                       << endl;
    m_out_h << "typedef struct " << &m_name[0] << " " << &m_name[0] << ";"   << endl;
    m_out_h << "#endif   /* __cplusplus */"                                  << endl;
    m_out_h                                                                  << endl;
    m_out_h << "#endif /* __" << &m_name[0] << "_FWD_DEFINED__ */"           << endl;
  }
  else {
    m_out_h                                                                         << endl;
# if defined(_WIN32)
    m_out_h << "#pragma warning( disable: 4049 )  /* more than 64k source lines */" << endl;
    m_out_h                                                                         << endl;
# endif
    m_out_h << "#ifndef __" << &m_name[0] << "_h__"                                 << endl;
    m_out_h << "#define __" << &m_name[0] << "_h__"                                 << endl;
    m_out_h                                                                         << endl;
# if defined(_WIN32) /* PTS 1140335 */
    m_out_h << "#include \"windows.h\" /* no check */"                              << endl;
    m_out_h                                                                         << endl;
# endif
    m_out_h << "#ifdef __cplusplus"                                                 << endl;
    m_out_h << "extern \"C\"{"                                                      << endl;
    m_out_h << "#endif"                                                             << endl;
    m_out_h                                                                         << endl;
  }

  m_out_c << "#ifdef __cplusplus"        << endl;
  m_out_c << "extern \"C\"{"             << endl;
  m_out_c << "#endif"                    << endl;
  m_out_c                                << endl;
  m_out_c << "#ifndef __IID_DEFINED__"   << endl;
  m_out_c << "#define __IID_DEFINED__"   << endl;
  m_out_c << "typedef struct _IID"       << endl;
  m_out_c << "{"                         << endl;
  m_out_c << "    unsigned long  x;"     << endl;
  m_out_c << "    unsigned short s1;"    << endl;
  m_out_c << "    unsigned short s2;"    << endl;
  m_out_c << "    unsigned char  c[8];"  << endl;
  m_out_c << "} IID;"                    << endl;
  m_out_c << "#endif // __IID_DEFINED__" << endl;
  m_out_c                                << endl;
  m_out_c << "#ifndef CLSID_DEFINED"     << endl;
  m_out_c << "#define CLSID_DEFINED"     << endl;
  m_out_c << "typedef IID CLSID;"        << endl;
  m_out_c << "#endif // CLSID_DEFINED"   << endl;
  m_out_c                                << endl;

  return 1;

}

/*----------------------------------------------------------------------------*/

void IspcCGenHFile::startElement( int & error, const char * name, const char ** atts) { /* PTS 1131325, PG */

  char lbuf[256];
  char lbuf2[256];

  bool onlyReg = false;

  if ( !m_is_within_import || (elemImport_ispc == ispc_getElementType(name)) ||
       ( ( elemTypedef_ispc         == ispc_getElementType(name) ||
           elemStruct_ispc          == ispc_getElementType(name) ||
           elemUnion_ispc           == ispc_getElementType(name) || /* PTS 1141233, PG */
           elemMember_ispc          == ispc_getElementType(name) ||
           elemIntrinsicmember_ispc == ispc_getElementType(name) || /* PTS 1141233, PG */
           elemConst_ispc           == ispc_getElementType(name) ||
           m_ifConst ) && m_features_76 ) ) { /* PTS 1131325, PTS 1134734, PG */
           /* m_ifConst is set in CONST in case of correct order of
              attributes and attribute NUMBER or EXPRESSION used */

    /* in case of 76 features, also within import mode
         - types          have to be registered in m_ift_list,
         - structs        have to be registered in m_ifs_registry,
         - unions         have to be registered in m_ifs_registry,
         - num. constants have to be registered in m_ifc_list */
    if ( ( elemTypedef_ispc         == ispc_getElementType(name) ||
           elemStruct_ispc          == ispc_getElementType(name) ||
           elemUnion_ispc           == ispc_getElementType(name) || /* PTS 1141233, PG */
           elemMember_ispc          == ispc_getElementType(name) ||
           elemIntrinsicmember_ispc == ispc_getElementType(name) || /* PTS 1141233, PG */
           elemConst_ispc           == ispc_getElementType(name) ) && m_features_76 ) {
      if (m_is_within_import) {
        onlyReg = true;
      }
      else {
        onlyReg = false;
      }
    }

    /* PTS 1141233, PG
     * OMS_LiveCacheTypes.idl exists in 7.5 and 7.6:
       - In 7.5 it is processed by MIDL (Win) or convert_pl (Unix).
       - In 7.6 it is only copied to usr\incl as livecachetypes.idl.
     * OMS_LiveCacheTypes.xml exists as of 7.6 and is processed by xml2ispc.
     * In older 7.6 and 7.7 OMS_LiveCacheTypes.xml
       - struct OmsTypeOid, 4 byte aligned version
       is declared.
     * In newer 7.6 OMS_LiveCacheTypes.xml
       - struct OmsTypeOid4BA
       - union  OmsTypeOid8BA
       - struct OmsTypeOid, 8 byte aligned version
       - struct OmsTypeOid, 4 byte aligned version
       are declared. 
     * In newer 7.7 OMS_LiveCacheTypes.xml
       - struct OmsTypeOid4BA
       - union  OmsTypeOid8BA
       - struct OmsTypeOid, 8 byte aligned version
       are declared.
     * Note:
       - Struct OmsTypeOid4BA is not registered in the interface type registry as it
         has members of not registered types (OmsTypeUInt4, unsigned short).
       - Union  OmsTypeOid8BA is not registered as the intrinsic member has type OmsTypeOid4BA
         that is not registered.
       - Struct OmsTypeOid, 8 byte aligned version, is not registered as the member type
         OmsTypeOid8BA is not registered.
       - Struct OmsTypeOid, 4 byte aligned version, is not registered for the same reason
         struct OmsTypeOid4BA is not registered. */

    /* in OMS_LiveCacheTypes after first tag CPPSQLDBC
       only further tags of this type tag are allowed
       PTS 1138438, PG */
    if ( m_is_livecachetypes && m_is_cppsqldbc_sec &&
         (elemCppsqldbc_ispc != ispc_getElementType(name)) ) {
      Ispc_ErrorMsg( C_ONLY_CPPSQLDBC_SUPPORTED );
      if (!error) {
        error = - C_ONLY_CPPSQLDBC_SUPPORTED;
      }
      return;
    }

    switch (ispc_getElementType(name)) {

      case elemDefine_ispc : { /* PTS 1141233, PG */
        if (attName_ispc == ispc_getAttributeType(atts[0])) {
          if ( strcmp( atts[1], "OID_8BYTE_ALIGNED" ) || !m_is_livecachetypes ) {
            m_out_h << endl;
            m_out_h << "#ifndef " << atts[1] << endl;
            m_out_h << "#define " << atts[1] << endl;
            m_out_h << "#endif" << endl;
          }
        }
        break;
      }

      case elemLibrary_ispc : {
        if (attName_ispc == ispc_getAttributeType(atts[0])) {
          strcpy( &m_libname[0], atts[1] );
          m_out_h  << endl;
          m_out_h  << "#ifndef __" << &m_libname[0] << "_LIBRARY_DEFINED__" << endl;
          m_out_h  << "#define __" << &m_libname[0] << "_LIBRARY_DEFINED__" << endl;
        }
        break;
      }

      case elemCpp_ispc : {
        if (attString_ispc == ispc_getAttributeType(atts[0])) {
          /* +++++ PTS 1109570 +++++ */
          const char * pc1 = atts[1];
          char * pc2 = (char*)strchr( pc1, '\\' );
          lbuf[0] = '\0';
          while (pc2) {
            pc2++;
            if (*pc2 == '\\') {
              *pc2 = '\0';
              strcat( lbuf, pc1 );
              pc1 = ++pc2;
            }
            pc2 = (char*)strchr( pc2, '\\' );
          }
          strcat( lbuf, pc1 );
          /* ----- PTS 1109570 ----- */
          /* strcat( lbuf, atts[1] );  PTS 1109570 */
          m_out_h  << lbuf << endl;
        }
        break;
      }

      case elemCppsqldbc_ispc : { /* PTS 1138438, PG */
        if (m_is_livecachetypes) {
          if (attString_ispc == ispc_getAttributeType(atts[0])) {
            if (!m_is_cppsqldbc_sec) {
              m_out_h << endl;
              m_out_h << "#ifdef __cplusplus" << endl;
              m_out_h << "}" << endl;
              m_out_h << "#endif" << endl;
              m_out_h << endl;
            }
            m_is_cppsqldbc_sec = true;
            strcpy( lbuf, atts[1] );
            m_out_h  << lbuf << endl;
          }
        }
        else {
          Ispc_ErrorMsg( C_CPPSQLDBC_NOT_SUPPORTED );
          if (!error) {
            error = - C_CPPSQLDBC_NOT_SUPPORTED;
          }
        }
        break;
      }

      case elemPragma_ispc : {
        if (attPack_ispc == ispc_getAttributeType(atts[0])) {
          strcpy( lbuf, atts[1] );
          m_out_h  << endl << "#pragma pack(" << lbuf << ")" << endl;
        }
        break;
      }

      case elemImport_ispc : {
        if (0 == m_is_within_import) {
          strcpy( lbuf, atts[1] );
          char * pc = strrchr( lbuf, '.' );
          if (pc) {
            *pc = '\0';
            strcat( lbuf, ".h" );
          }
          m_out_h  << endl << "#include \"" << lbuf << "\"  /* no check */" << endl; /* PTS 1112974 */
        }
        m_is_within_import++;
        break;
      }

      case elemConst_ispc : {
        if ( attName_ispc == ispc_getAttributeType(atts[0]) &&
             attType_ispc == ispc_getAttributeType(atts[2]) ) { /* PTS 1134734, PG */
          strcpy( m_constname, atts[1] );                       /* PTS 1134734, PG */
          strcpy(lbuf,atts[1]);
          /* +++++ PTS 1109570 +++++ */
          const char * pc1 = atts[1];
          char * pc2 = (char*)strchr( pc1, '\\' );
          pc1 = atts[5];
          pc2 = (char*)strchr( pc1, '\"' );
          lbuf2[0] = '\0';
          while ( pc2 && *(pc2+1) ) {
            *pc2 = '\0';
            strcat( lbuf2, pc1 );
            strcat( lbuf2, "\\\"" );
            pc2++;
            pc1 = pc2;
            pc2 = (char*)strchr( pc1, '\"' );
          }
          strcat( lbuf2, pc1 );
          /* ----- PTS 1109570 ----- */
          /*strcpy(lbuf2,atts[5]); PTS 1109570 */
          if (!onlyReg) { /* PTS 1134734, PG */
            if (attString_ispc == ispc_getAttributeType(atts[4])) {
              m_out_h << endl << "#define " << lbuf << " ( \"" << lbuf2 << "\" )" << endl;
            }
            else {
              m_out_h << endl << "#define " << lbuf << " ( " << lbuf2 << " )" << endl;
            }
          }
          /* PTS 1134734, PG */
          if ( attNumber_ispc     == ispc_getAttributeType(atts[4]) ||
               attExpression_ispc == ispc_getAttributeType(atts[4]) ) {
            m_ifConst = new Ispc_InterfaceConst(atts[1]);
            if (attNumber_ispc == ispc_getAttributeType(atts[4])) {
              m_ifConst->setValue( error, atoi(atts[5]) );
            }
          }
        }
        else {
          for ( int i = 0; atts[i]; i = i+2 ) {
            if (attName_ispc == ispc_getAttributeType(atts[i])) {
              strcpy( &m_constname[0], atts[i+1] );
              break;
            }
          }
          Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_CONST, m_constname );
          m_constname[0] = '\0';
          if (!error) {
            error = - C_ILLEGAL_ATTRORDER_IN_CONST;
          }
        }
        break;
      }

      case elemAdd_ispc : { /* PTS 1134734, PG */
        if (NULL == m_ifConst) {
          Ispc_ErrorMsg( C_ILLEGAL_EXPR_ELEMENT, m_constname );
          if (!error) {
            error = - C_ILLEGAL_EXPR_ELEMENT;
          }
        }
        else {
          m_ifConst->setOperator( error, coPlus );
        }
        break;
      }

      case elemSub_ispc : { /* PTS 1134734, PG */
        if (NULL == m_ifConst) {
          Ispc_ErrorMsg( C_ILLEGAL_EXPR_ELEMENT, m_constname );
          if (!error) {
            error = - C_ILLEGAL_EXPR_ELEMENT;
          }
        }
        else {
          m_ifConst->setOperator( error, coMinus );
        }
        break;
      }

      case elemFixed_ispc : { /* PTS 1134734, PG */
        if (NULL == m_ifConst) {
          Ispc_ErrorMsg( C_ILLEGAL_EXPR_ELEMENT, m_constname );
          if (!error) {
            error = - C_ILLEGAL_EXPR_ELEMENT;
          }
        }
        else {
          m_ifConst->setNextOperandType( error, cotFixed );
        }
        break;
      }

      case elemIdent_ispc : { /* PTS 1134734, PG */
        if (NULL == m_ifConst) {
          Ispc_ErrorMsg( C_ILLEGAL_EXPR_ELEMENT, m_constname );
          if (!error) {
            error = - C_ILLEGAL_EXPR_ELEMENT;
          }
        }
        else {
          m_ifConst->setNextOperandType( error, cotIdent );
        }
        break;
      }

      case elemTypedef_ispc : {
        const char *name_idx = "";
        const char *type_idx = "";
        const char *dim_idx  = "";
        /* PTS 1134734 */
        const char *genH_idx = "";
        int uuid_attrNo = -1;
        int name_attrNo = -1;
        for ( int i = 0; atts[i]; i = i+2 ) {
          XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
          switch (attrtype) {
            case attUuid_ispc : {
              uuid_attrNo = i;
              break;
            }
            case attName_ispc : {
              name_attrNo = i;
              name_idx = atts[i+1];
              break;
            }
            case attType_ispc : {
              type_idx = atts[i+1];
              break;
            }
            case attDimension_ispc : {
              dim_idx  = atts[i+1];
              break;
            }
            case attGenTabHandle_ispc : { /* PTS 1134734, PG */
              genH_idx = atts[i+1];
              break;
            }
            default : {
              break;
            }
          }
        }
        if ( !( (-1 == uuid_attrNo) && (0 == name_attrNo) ) &&
             !( ( 0 == uuid_attrNo) && (2 == name_attrNo) ) ) {
          Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_TYPEDEF, name_idx );
          if (!error) {
            error = - C_ILLEGAL_ATTRORDER_IN_TYPEDEF;
          }
          break;
        }
        /* PTS 1131325, PG */
        if (m_features_76) {
          if ( ('\0' != *dim_idx) && (m_typedef_check) && (strcmp( name_idx, "OmsVersionId" )) ) {
            Ispc_ErrorMsg( C_DIMENSION_IN_TYPEDEF, type_idx, name_idx );
            if (!error) {
              error = - C_DIMENSION_IN_TYPEDEF;
            }
            break;
          }
          if ( (!m_ift_list.skipTypeReg(name_idx)) && (!m_ift_list.isRegistered(name_idx)) ) {
            const Ispc_InterfaceType * ifType = m_ift_list.findType(type_idx);
            if (ifType) {
              m_ift_list.registerType( name_idx, ifType->getDefault() );
            }
            else { /* PTS 1134734 */
              Ispc_InterfaceStruct * ifStruct = m_ifs_registry.findStruct(type_idx);
              if (ifStruct) {
                ifStruct->addSynonym(name_idx);
              }
            }
            /*
            else {
              It's neither based on a scalar OmsType nor an OmsType-structure.
            }
            */
          }
          if (onlyReg) {
            break;
          }
        }
        m_out_h << endl << "typedef " << type_idx << " " << name_idx;
        if (strlen(dim_idx) > 0) {
          m_out_h << "[" << dim_idx << "]";
        }
        m_out_h << ";" << endl;
        /* PTS 1134734 */
        if ( m_features_76 && !strcmp( genH_idx, "YES" ) ) {
          m_out_h << endl;
          m_out_h << "typedef struct " << name_idx << "TabHandle" << " {" << endl;
          m_out_h << "   " << name_idx << "* RowDef;" << endl;
          m_out_h << "   " << "OmsTypeABAPTabHandle Handle;" << endl;
          m_out_h << "} " << name_idx << "TabHandle;" << endl;
        }
        break;
      }

      case elemStruct_ispc : {
        m_is_within_struct++;
        m_structname[0] = '\0';
        if (attName_ispc == ispc_getAttributeType(atts[0])) {
          strcpy( &m_structname[0], atts[1] );
          if (!onlyReg) {
            m_out_h << endl << "typedef struct " << &m_structname[0] << " { " << endl;
          }
          /* PTS 1131325, PTS 1134734, PG */
          if (m_features_76) {
            m_ifStruct = new Ispc_InterfaceStruct(m_structname); /* PTS 1134618, PG */
            if (!onlyReg) {
              for ( int i = 2; atts[i]; i = i+2 ) {
                XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
                switch (attrtype) {
                  case attGenTabHandle_ispc : {
                    if (0 == strcmp( atts[i+1], "YES" )) {
                      m_gen_tab_handle = true;
                    }
                    break;
                  }
                  case attGenDefConstr_ispc : {
                    if (0 == strcmp( atts[i+1], "YES" )) {
                      m_gen_def_constr = 2;
                      m_out_h << "   inline " << &m_structname[0] << "(); " << endl;
                      m_def_constr_head = "inline ";
                      m_def_constr_head.append(&m_structname[0]);
                      m_def_constr_head.append("::");
                      m_def_constr_head.append(&m_structname[0]);
                      m_def_constr_head.append("()"); /* PTS 1134589 */
                      m_def_constr_head.append("\n");
                      m_def_constr_body = "{";
                      m_def_constr_body.append("\n");
                      m_i_declared = false;
                    }
                    break;
                  }
                  default: {
                    break;
                  }
                }
              }
            }
          }
          /* PTS 1131520, PG */
          if (m_is_livecachetypes) {
            if (!strcmp( &m_structname[0], "OmsTypeABAPTimeStamp" ) ||
                !strcmp( &m_structname[0], "OmsTypePacked_8_3" )    ||
                !strcmp( &m_structname[0], "OmsTypePacked_15_3" )) {
              m_out_h << "#ifdef __cplusplus" << endl;
              m_out_h << "   inline " << &m_structname[0] << "(); " << endl;
              m_out_h << "#endif" << endl;
            }
          }
        }
        else {
          for ( int i = 2; atts[i]; i = i+2 ) {
            if (attName_ispc == ispc_getAttributeType(atts[i])) {
              strcpy( &m_structname[0], atts[i+1] );
              break;
            }
          }
          Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_STRUCT, m_structname );
          if (!error) {
            error = - C_ILLEGAL_ATTRORDER_IN_STRUCT;
          }
        }
        break;
      }

      case elemUnion_ispc : { /* PTS 1141233, PG */
        m_is_within_union++;
        m_structname[0] = '\0';
        strcpy( &m_structname[0], atts[1] );
        if (!onlyReg) {
          m_out_h << endl << "union " << &m_structname[0] << " { " << endl;
        }
        if (m_features_76) {
          m_ifStruct = new Ispc_InterfaceStruct(m_structname);
        }
        break;
      }

      case elemMember_ispc : {
        if (m_is_within_struct){
          /* PTS 1131325, PG */
          char sbuf[3];   /* only for values "  " and " ," */
          char nbuf[256]; /* for name */
          char tbuf[256]; /* for type */
          char dbuf[256]; /* for dimension */
          char ibuf[11];  /* for index, PTS 1134734, PG */
          if ( attName_ispc == ispc_getAttributeType(atts[0]) &&
               attType_ispc == ispc_getAttributeType(atts[2]) ) { /* PTS 1134734, PG */
            strcpy( nbuf, atts[1] );
            strcpy( tbuf, atts[3] );
            if (!onlyReg) {
              m_out_h  << "   " << tbuf << " " << nbuf;
            }
            if ( atts[4] && (attDimension_ispc == ispc_getAttributeType(atts[4])) ) {
              strcpy( dbuf, atts[5] );
              if (!onlyReg) {
                m_out_h  << "[" << dbuf << "]";
              }
            }
            if (!onlyReg) {
              m_out_h  << ";" << endl;
            }
          }
          else {
            for ( int i = 0; atts[i]; i = i+2 ) {
              if (attName_ispc == ispc_getAttributeType(atts[i])) {
                strcpy( &nbuf[0], atts[i+1] );
                break;
              }
            }
            Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_STRUCTMEMBER, nbuf );
            if (!error) {
              error = - C_ILLEGAL_ATTRORDER_IN_STRUCTMEMBER;
            }
            break;
          }
          /* PTS 1131325, PG */
          if (m_features_76) {
            const Ispc_InterfaceType * ifType = m_ift_list.findType(tbuf); /* PTS 1134618, PG */
            if ( !onlyReg && m_gen_def_constr ) {
              if (ifType) { /* PTS 1138213, PG */
                if ( atts[4] && (attDimension_ispc == ispc_getAttributeType(atts[4])) ) {
                  int dimValue = atoi(dbuf);
                  if (0 == dimValue) {
                    const Ispc_InterfaceConst * ifConst = m_ifc_list.findConst(dbuf);
                    if (ifConst) {
                      dimValue = ifConst->getValue();
                    }
                  }
                  if (0 < dimValue) {
                    for ( int i = 0; i < dimValue; i++ ) {
                      memset( ibuf, '\0', sizeof(ibuf) );
                      sprintf( ibuf, "%d", i );
                      m_def_constr_body.append("  ");
                      m_def_constr_body.append(nbuf);
                      m_def_constr_body.append("[");
                      m_def_constr_body.append(ibuf);
                      m_def_constr_body.append("] = ");
                      m_def_constr_body.append(ifType->getDefault());
                      m_def_constr_body.append(";");
                      m_def_constr_body.append("\n");
                    }
                  }
                  else {
                    if (!m_i_declared) {
                      m_def_constr_body.append("  int i;");
                      m_def_constr_body.append("\n");
                      m_i_declared = true;
                    }
                    m_def_constr_body.append("  for ( i = 0; i < ");
                    m_def_constr_body.append(dbuf);
                    m_def_constr_body.append("; i++ ) {");
                    m_def_constr_body.append("\n");
                    m_def_constr_body.append("    ");
                    m_def_constr_body.append(nbuf);
                    m_def_constr_body.append("[i] = ");
                    m_def_constr_body.append(ifType->getDefault());
                    m_def_constr_body.append(";");
                    m_def_constr_body.append("\n");
                    m_def_constr_body.append("  }");
                    m_def_constr_body.append("\n");
                  }
                }
                else {
                  if (2 == m_gen_def_constr) { /* next member to be initialized is the first to be initialized */
                    strcpy( sbuf, ": " );
                    m_gen_def_constr = 1;
                  }
                  else {
                    strcpy( sbuf, ", " );
                  }
                  m_def_constr_head.append(sbuf);
                  m_def_constr_head.append(nbuf);
                  m_def_constr_head.append("(");
                  m_def_constr_head.append(ifType->getDefault());
                  m_def_constr_head.append(")");
                  m_def_constr_head.append("\n"); /* PTS 1134589 */
                }
              }
            }
            /* PTS 1134618, PG */
            Ispc_Member * member = new Ispc_Member(nbuf);
            if (ifType) {
              if (!(member->setTypeScalar())) {
                Ispc_ErrorMsg( C_DUPLICATE_MEMBERTYPE_SPECIFICATION, nbuf, m_ifStruct->getName() );
                if (!error) {
                  error = - C_DUPLICATE_MEMBERTYPE_SPECIFICATION;
                }
                break;
              }
            }
            else {
              Ispc_InterfaceStruct * ifStruct = m_ifs_registry.findStruct(tbuf);
              if (NULL != ifStruct) {
                if (!(member->setTypeStruct(ifStruct))) {
                  Ispc_ErrorMsg( C_DUPLICATE_MEMBERTYPE_SPECIFICATION, nbuf, m_ifStruct->getName() );
                  if (!error) {
                    error = - C_DUPLICATE_MEMBERTYPE_SPECIFICATION;
                  }
                  break;
                }
              }
              else { /* PTS 1134734 */
                if ( !strcmp( m_ifStruct->getName(), "OmsTypeABAPTimeStamp" ) ||
                     !strcmp( m_ifStruct->getName(), "OmsTypePacked_8_3" )    ||
                     !strcmp( m_ifStruct->getName(), "OmsTypePacked_15_3" ) ) {
                  if (!(member->setTypeScalar())) {
                    Ispc_ErrorMsg( C_DUPLICATE_MEMBERTYPE_SPECIFICATION, nbuf, m_ifStruct->getName() );
                    if (!error) {
                      error = - C_DUPLICATE_MEMBERTYPE_SPECIFICATION;
                    }
                    break;
                  }
                }
              }
            }
            m_ifStruct->addMember(member);
          }
        }
        else if (m_is_within_enum) {
          if ( attName_ispc  == ispc_getAttributeType(atts[0]) && /* PTS 1134734 */
               attValue_ispc == ispc_getAttributeType(atts[2]) ) {
            strcpy( lbuf,  atts[1] );
            strcpy( lbuf2, atts[3] );
            if (2 == m_is_within_enum) {
              m_out_h  << "  " << lbuf << " = " << lbuf2 << endl;
              m_is_within_enum = 1;
            }
            else {
              m_out_h  << ", " << lbuf << " = " << lbuf2 << endl;
            }
          }
          else {
            for ( int i = 0; atts[i]; i = i+2 ) {
              if (attName_ispc == ispc_getAttributeType(atts[i])) {
                strcpy( &lbuf[0], atts[i+1] );
                break;
              }
            }
            Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_ENUMMEMBER, lbuf );
            if (!error) {
              error = - C_ILLEGAL_ATTRORDER_IN_ENUMMEMBER;
            }
            break;
          }
        }
        break;
      }

      case elemIntrinsicmember_ispc : { /* PTS 1141233, PG */
        if (m_is_within_union){
          char nbuf[256]; /* for name */
          char tbuf[256]; /* for type */
          if ( attName_ispc == ispc_getAttributeType(atts[0]) &&
               attType_ispc == ispc_getAttributeType(atts[2]) ) {
            strcpy( nbuf, atts[1] );
            strcpy( tbuf, atts[3] );
            if (!onlyReg) {
              m_out_h << "  " << tbuf << " " << nbuf << ";" << endl;
            }
          }
          else {
            for ( int i = 0; atts[i]; i = i+2 ) {
              if (attName_ispc == ispc_getAttributeType(atts[i])) {
                strcpy( &nbuf[0], atts[i+1] );
                break;
              }
            }
            Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_STRUCTMEMBER, nbuf );
            if (!error) {
              error = - C_ILLEGAL_ATTRORDER_IN_STRUCTMEMBER;
            }
            break;
          }
          if (m_features_76) {
            Ispc_Member * member = new Ispc_Member(nbuf);
            const Ispc_InterfaceType * ifType = m_ift_list.findType(tbuf);
            if (ifType) {
              if (!(member->setTypeScalar())) {
                Ispc_ErrorMsg( C_DUPLICATE_MEMBERTYPE_SPECIFICATION, nbuf, m_ifStruct->getName() );
                if (!error) {
                  error = - C_DUPLICATE_MEMBERTYPE_SPECIFICATION;
                }
                break;
              }
            }
            else {
              Ispc_InterfaceStruct * ifStruct = m_ifs_registry.findStruct(tbuf);
              if (NULL != ifStruct) {
                if (!(member->setTypeStruct(ifStruct))) {
                  Ispc_ErrorMsg( C_DUPLICATE_MEMBERTYPE_SPECIFICATION, nbuf, m_ifStruct->getName() );
                  if (!error) {
                    error = - C_DUPLICATE_MEMBERTYPE_SPECIFICATION;
                  }
                  break;
                }
              }
              else {
                if (!strcmp( m_ifStruct->getName(), "OmsTypeDouble" )) {
                  if (!(member->setTypeScalar())) {
                    Ispc_ErrorMsg( C_DUPLICATE_MEMBERTYPE_SPECIFICATION, nbuf, m_ifStruct->getName() );
                    if (!error) {
                      error = - C_DUPLICATE_MEMBERTYPE_SPECIFICATION;
                    }
                    break;
                  }
                }
              }
            }
            m_ifStruct->addMember(member);
          }
        }
        break;
      }

      case elemAlignmentmember_ispc : { /* PTS 1141233, PG */
        if (m_is_within_union){
          char nbuf[256]; /* for name */
          char tbuf[256]; /* for type */
          if ( attName_ispc == ispc_getAttributeType(atts[0]) &&
               attType_ispc == ispc_getAttributeType(atts[2]) ) {
            strcpy( nbuf, atts[1] );
            strcpy( tbuf, atts[3] );
            if (!onlyReg) {
              m_out_h  << "  " << tbuf << " " << nbuf << ";" << endl;
            }
          }
          else {
            for ( int i = 0; atts[i]; i = i+2 ) {
              if (attName_ispc == ispc_getAttributeType(atts[i])) {
                strcpy( &nbuf[0], atts[i+1] );
                break;
              }
            }
            Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_STRUCTMEMBER, nbuf );
            if (!error) {
              error = - C_ILLEGAL_ATTRORDER_IN_STRUCTMEMBER;
            }
            break;
          }
        }
        break;
      }

      case elemEnum_ispc : {
        /* PTS 1131325, PG */
        m_is_within_enum = 2;
        if (attName_ispc == ispc_getAttributeType(atts[0])) {
          strcpy( &m_enumname[0], atts[1] );
          m_out_h << endl << "typedef enum { " << endl;
        }
        break;
      }

      case elemInclude_ispc : {
        m_is_within_include++;
        break;
      }

      case elemInterface_ispc : {
        if ( attName_ispc == ispc_getAttributeType(atts[0]) &&
             attUuid_ispc == ispc_getAttributeType(atts[2]) ) { /* PTS 1134734, PG */
          strcpy( &m_interfacename[0], atts[1] );
          const char * pc = atts[3];
          int i = 0;
          for ( ; *pc != '\0'; pc++ ) {
            if (*pc != ' ') {
              m_iid[i] = *pc;
              i++;
            }
          }
          m_iid[i] = 0;
          //strcpy(&m_iid[0],atts[3]);
          m_out_h                                                                  << endl;
          m_out_h << "#ifndef __" << &m_interfacename[0] << "_INTERFACE_DEFINED__" << endl;
          m_out_h << "#define __" << &m_interfacename[0] << "_INTERFACE_DEFINED__" << endl;
          m_out_h                                                                  << endl;
          m_out_h << "EXTERN_C const IID IID_" << &m_interfacename[0] << ";"       << endl;
          m_out_h                                                                  << endl;
          m_out_h << "MIDL_INTERFACE(\"" << &m_iid[0] << "\")"                     << endl;
          m_out_h << &m_interfacename[0] << " : public IUnknown {"                 << endl;
          m_out_h << "public:"                                                     << endl;
          m_out_c << "const IID IID_" << &m_interfacename[0] << " = {0x";
          for ( i = 0; i < 8; i++ ) {
            m_out_c << m_iid[i];
          }
          m_out_c << ", 0x";
          for ( i = 9; i < 13; i++ ) {
            m_out_c << m_iid[i];
          }
          m_out_c << ", 0x";
          for ( i = 14; i < 18; i++ ) {
            m_out_c << m_iid[i];
          }
          m_out_c << ", {";
          for ( i = 19; i < 23; i = i+2 ) {
            m_out_c << "0x" << m_iid[i] << m_iid[i+1] << ", ";
          }
          for ( i = 24; i < 34; i = i+2 ) {
            m_out_c << "0x" << m_iid[i] << m_iid[i+1] << ", ";
          }
          m_out_c << "0x" << m_iid[34] << m_iid[35] << "}};" << endl << endl;
        }
        else {
          strcpy( &m_interfacename[0], atts[3] );
          Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_INTERFACE, m_interfacename );
          if (!error) {
            error = - C_ILLEGAL_ATTRORDER_IN_INTERFACE;
          }
        }
        break;
      }

      case elemIdl_ispc : {
        m_methode_param = 0;
        if (attName_ispc == ispc_getAttributeType(atts[0])) {
          strcpy( lbuf, atts[1] );
          m_out_h  << endl << "    virtual HRESULT STDMETHODCALLTYPE " << lbuf << "(";
        }
        break;
      }

      case elemParam_ispc : {
        const char * name_idx = "";
        const char * type_idx = "";
        const char * direction_idx = "";
        unsigned int i;
        for ( i = 0; atts[i]; i = i+2 ) {
          XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
          switch (attrtype) {
            case attName_ispc : {
              name_idx = atts[i+1];
              break;
            }
            case attType_ispc : {
              type_idx = atts[i+1];
              break;
            }
            case attDirection_ispc : {
              direction_idx = atts[i+1];
              break;
            }
            default : {
              break;
            }
          }
        }
        if (m_methode_param) {
          m_out_h << ",";
        }
        m_methode_param++;
        m_out_h  << endl;
        strcpy( lbuf, type_idx );
        if ( (!strcmp( "bool", lbuf )) || (!strcmp( "bool*", lbuf )) ) {
          for ( i = 0; i < strlen(lbuf); i++ ) {
            lbuf[i] = toupper(lbuf[i]);
          }
        }
        if ( (!strcmp( "char", lbuf )) || (!strcmp( "char*", lbuf )) ) {
#         if defined(_WIN32)
            m_out_h << "      /* " << direction_idx << " */ unsigned " << lbuf << " " << name_idx ;
#         else
            m_out_h << "      /* " << direction_idx << " */ signed " << lbuf << " " << name_idx ;
#         endif
        }
        else {
          m_out_h << "      /* " << direction_idx << " */ " << lbuf << " " << name_idx ;
        }
        break;
      }

      case elemCoClass_ispc : {
        int i;
        m_out_h  << "};" << endl;
        m_out_h  << "#endif /* __" << &m_interfacename[0] << "_INTERFACE_DEFINED__ */" << endl;
        if ( (attName_ispc == ispc_getAttributeType(atts[0])) ||
             (attUuid_ispc == ispc_getAttributeType(atts[2])) ) { /* PTS 1134734, PG */
          strcpy( lbuf, atts[1] );
          const char * pcAtts = atts[3];
          i = 0;
          for ( ; *pcAtts != '\0'; pcAtts++ ) {
            if (*pcAtts != ' ') {
              lbuf2[i] = *pcAtts;
              i++;
            }
          }
          lbuf2[i] = 0;
          m_out_h << endl << "EXTERN_C const CLSID CLSID_" << lbuf << ";" << endl;
          m_out_h << "class DECLSPEC_UUID(\"" << lbuf2 << "\") " << lbuf << ";" << endl;
          m_out_c << "const CLSID CLSID_" << lbuf << " = {0x";
          for ( i = 0; i < 8; i++ ) {
            m_out_c << lbuf2[i];
          }
          m_out_c << ", 0x";
          for ( i = 9; i < 13; i++ ) {
            m_out_c << lbuf2[i];
          }
          m_out_c << ", 0x";
          for ( i = 14; i < 18; i++ ) {
            m_out_c << lbuf2[i];
          }
          m_out_c << ", {";
          for ( i = 19; i < 23; i = i+2 ) {
            m_out_c << "0x" << lbuf2[i] << lbuf2[i+1] << ", ";
          }
          for ( i = 24; i < 34; i = i+2 ) {
            m_out_c << "0x" << lbuf2[i] << lbuf2[i+1] << ", ";
          }
          m_out_c << "0x" << lbuf2[34] << lbuf2[35] << "}};" << endl << endl;
        }
        else {
          strcpy( lbuf, atts[3] );
          Ispc_ErrorMsg( C_ILLEGAL_ATTRORDER_IN_COCLASS, lbuf );
          if (!error) {
            error = - C_ILLEGAL_ATTRORDER_IN_COCLASS;
          }
        }
        break;
      }

      case elemUnknown_ispc : {
        break;
      }

      default : {
        break;
      }

    }

  }

}

/*----------------------------------------------------------------------------*/

void IspcCGenHFile::endElement(const char * name) {

  if ( m_is_within_import || (m_constname[0] != '\0') ) { /* PTS 1134734, PG */

    switch (ispc_getElementType(name)) {
      case elemImport_ispc : {
        m_is_within_import--;
        break;
      }
      case elemStruct_ispc :
      case elemUnion_ispc : { /* PTS 1141233, PG */
        if ( m_is_within_import && m_ifStruct ) {
          if (m_ifStruct->allMemberTypesRegistered()) {
            m_ifs_registry.registerStruct(m_ifStruct);
          }
          else {
            delete m_ifStruct;
          }
          m_ifStruct = NULL;
        }
        if (elemStruct_ispc == ispc_getElementType(name)) {
          m_is_within_struct--;
        }
        else {
          m_is_within_union--;
        }
        break;
      }
      case elemConst_ispc : {
        if (NULL != m_ifConst) {
          if (NULL == m_ifConst->getValuePtr()) {
            Ispc_WarnMsg( C_MISSING_CONST_VALUE, m_constname );
            delete m_ifConst;
          }
          else {
            m_ifConst->resetOperator();
            m_ifc_list.registerConst(m_ifConst);
          }
          m_ifConst = NULL;
        }
        memset( m_constname, '\0', sizeof(m_constname) );
        break;
      }
      case elemFixed_ispc :
      case elemIdent_ispc : {
        if (NULL != m_ifConst) {
          m_ifConst->resetNextOperandType();
        }
        /* in case NULL == m_ifConst error already logged in startElement */
        break;
      }
      default : {
        break;
      }
    }

  }
  else {

    switch (ispc_getElementType(name)) {
      case elemIdl2Xml_ispc : { /* PTS 1138438, PG */
        if ( (m_is_livecachetypes) && (!m_is_cppsqldbc_sec) ) {
          m_out_h << endl;
          m_out_h << "#ifdef __cplusplus" << endl;
          m_out_h << "}" << endl;
          m_out_h << "#endif" << endl;
        }
        m_out_h << endl;
        m_out_h << "#endif" << endl;
        m_out_c << "#ifdef __cplusplus" << endl;
        m_out_c << "}" << endl;
        m_out_c << "#endif" << endl;
        break;
      }
      case elemLibrary_ispc : {
        m_out_h  << endl << "#endif /* __" << &m_libname[0] << "_LIBRARY_DEFINED__ */" << endl;
        break;
      }
      case elemCpp_ispc : {
        break;
      }
      case elemCppsqldbc_ispc : { /* PTS 1138438, PG */
        break;
      }
      case elemPragma_ispc : {
        break;
      }
      case elemInclude_ispc : {
        m_is_within_include--;
        break;
      }
      case elemTypedef_ispc : {
        break;
      }
      case elemStruct_ispc : {
        m_is_within_struct--;
        m_out_h << "} " << &m_structname[0] << ";" << endl;
        /* PTS 1131325, PG */
        if (m_features_76) {
          if (m_gen_def_constr) {
            m_def_constr_body.append("}");
            m_def_constr_body.append("\n");
            m_out_h << endl;
            m_out_h << m_def_constr_head.data();
            m_out_h << m_def_constr_body.data();
            m_def_constr_head.erase();
            m_def_constr_body.erase();
            m_i_declared = false; /* PTS 1134734, PG */
          }
          m_gen_def_constr = 0;
          /* PTS 1134618, PG */
          if (m_ifStruct->allMemberTypesRegistered()) {
            m_ifs_registry.registerStruct(m_ifStruct);
          }
          else {
            delete m_ifStruct;
            m_ifStruct = NULL;
          }
          if (m_gen_tab_handle) {
            m_out_h << endl;
            m_out_h << "typedef struct " << &m_structname[0] << "TabHandle" << " {" << endl;
            m_out_h << "   " << &m_structname[0] << "* RowDef;" << endl;
            m_out_h << "   " << "OmsTypeABAPTabHandle Handle;" << endl;
            m_out_h << "} " << &m_structname[0] << "TabHandle;" << endl;
            m_out_h << endl;
            if (m_ifStruct) { /* all member types are registered */
              m_ifStruct->genColEnum(m_col_enum);
              m_out_h << m_col_enum.data();
              m_col_enum.erase();
            }
            else { /* not all member types are registered */
              Ispc_WarnMsg( C_NOT_ALL_MEMBERTYPES_REGISTERED, m_structname );
            }
          }
          m_gen_tab_handle = false;
          m_ifStruct = NULL;
        }
        /* PTS 1131520, PG */
        if (m_is_livecachetypes) {
          int isATS     = !strcmp( &m_structname[0], "OmsTypeABAPTimeStamp" ),
              isSPacked = !strcmp( &m_structname[0], "OmsTypePacked_8_3" ),
              isLPacked = !strcmp( &m_structname[0], "OmsTypePacked_15_3" );
          if ( isATS || isSPacked || isLPacked ) {
            m_out_h << endl;
            m_out_h << "#ifdef __cplusplus" << endl;
            m_out_h << "inline " << &m_structname[0] << "::" << &m_structname[0] << "() {" << endl;
            m_out_h << "  for ( int i = 0; i < ";
            if ( isATS || isSPacked ) {
              m_out_h << "7";
            }
            else {
              m_out_h << "14";
            }
            m_out_h << "; i++ ) {" << endl;
            if (isATS) {
              m_out_h << "    m_timeStamp";
            }
            else {
              m_out_h << "    m_packed";
            }
            m_out_h << "[i] = 0x00;" << endl;
            m_out_h << "  }" << endl;
            if (isATS) {
              m_out_h << "  m_timeStamp[7]";
            }
            else if (isSPacked) {
              m_out_h << "  m_packed[7]";
            }
            else {
              m_out_h << "  m_packed[14]";
            }
            m_out_h << " = 0x0c;" << endl;
            m_out_h << "}" << endl;
            m_out_h << "#endif" << endl;
          }
        }
        break;
      }
      case elemUnion_ispc : { /* PTS 1141233, PG */
        m_is_within_union--;
        m_out_h << "};" << endl;
        if (m_features_76) {
          if (m_ifStruct->allMemberTypesRegistered()) {
            m_ifs_registry.registerStruct(m_ifStruct);
          }
          else {
            delete m_ifStruct;
          }
          m_ifStruct = NULL;
        }
        break;
      }
      case elemMember_ispc : 
      case elemAlignmentmember_ispc :   /* PTS 1141233, PG */
      case elemIntrinsicmember_ispc : { /* PTS 1141233, PG */
        break;
      }
      case elemEnum_ispc : {
        /* PTS 1131325, PG */
        m_out_h << "} " << &m_enumname[0] << ";" << endl;
        m_is_within_enum = 0;
        break;
      }
      case elemInterface_ispc : {
        break;
      }
      case elemIdl_ispc : {
         m_out_h  << "   ) = 0;" << endl;
         break;
      }
      case elemCoClass_ispc : {
        break;
      }
      case elemUnknown_ispc : {
        break;
      }
      default : {
        break;
      }
    }

  }

}

/*----- PTS 1134734, PG ------------------------------------------------------*/

void IspcCGenHFile::charData( int & error, const char * s, int len ) {

  if (NULL == m_ifConst) {
    /* either elemFloat_ispc or error already logged */
    return;
  }

  if (NULL == m_ifConst->getNextOperandType()) {
    /* error already logged */
    return;
  }

  char * operand = new char[len+1];
  strncpy( operand, s, len );
  operand[len] = '\0';

  switch (*(m_ifConst->getNextOperandType())) {
    case cotFixed : {
      m_ifConst->setValue( error, atoi(operand) );
      break;
    }
    case cotIdent : {
      const Ispc_InterfaceConst * ifConst = m_ifc_list.findConst(operand);
      if (ifConst) {
        m_ifConst->setValue( error, ifConst->getValue() );
      }
      break;
    }
  }

  delete [] operand;

}

