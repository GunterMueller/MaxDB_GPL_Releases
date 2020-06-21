/*!*****************************************************************************

  module      : IspcCGenInspecCode.h

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : Generate <project>Ispc.h and <project>Ispc.cpp

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


#ifndef ISPCCGENINSPECCODE_H
#define ISPCCGENINSPECCODE_H


//#include "gsp00.h"
#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <fstream>
#define  ofstream  std::ofstream
#else
#include <fstream.h>
#endif

#include <string.h>

#ifndef ISPCIDL2CODE_H
#include "idl/xml2ispc/IspcIdl2Code.h"
#endif

//#include "gco00.h"
#include "idl/xml2ispc/Ispc_ggg250.h"
#include "idl/xml2ispc/Ispc_ggg251.h"


/******************************************************************************/
/*     GUID                                                                   */
/******************************************************************************/

typedef struct _GUID { // size is 16
  unsigned int   Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char  Data4[8];
} GUID;


/******************************************************************************/
/*     IspcCExprElem                                                          */
/******************************************************************************/

struct IspcCExprElem {
  XmlElementType_Enum_ispc m_token_type;
  char *                   m_token_value;
  IspcCExprElem();
  ~IspcCExprElem();
  const IspcCExprElem & operator= (const IspcCExprElem & right);
};


/******************************************************************************/
/*     IspcCGenInspecCode                                                     */
/******************************************************************************/

class IspcCTypedef;

class IspcCGenInspecCode
{
public:
  IspcCGenInspecCode();
  ~IspcCGenInspecCode();
  int                    Init( char * cFilename, bool features_76 ); /* PTS 1131325, PG */
  void                   startElement( int & error, DboType_Enum_ispc & dboType,
                                       const char * name, const char ** atts);
  void                   endElement( int & error, const char * name );
  void                   charData( int & error, const char * s, int len );
  inline void *          allocate(size_t sz);
  inline void            deallocate(void * p);
  IspcCTypedef * const * findNode(IspcCTypedef & findNode);
  short                  checkBasisType( const char * type);
  void                   genABAPTabHandle( int & error, char * structName ); /* PTS 1131325, PG */
private:
  short                  getTypeDimension(const char *);
  int                    getTypeCharacteristics( const char * type,
                                                 const char * usedby, const char * username,
                                                 short & server_size, short & server_align,
                                                 short & asc_client_size, short & asc_client_align,
                                                 short & uni_client_size, short & uni_client_align );
  void                   printGuidV1( ofstream & out, IspcCTypedef & type );
  bool                   checkFirstParam( const char * pname, const char * puuid,
                                          const char * ptype, const char * pdirection );
  char                                m_filename[256];
  char                                m_name[64];
  ofstream                            m_cpp_out;
  ofstream                            m_h_out;
  IspcCTypedef *                      m_act_pType;  /* type description in work */
  bool                                m_expr_flag;  /* an <expr> is expected    */
  IspcCExprElem                       m_expr_token; /* expr token in work       */
  char *                              m_gen_tab_handle; /* PTS 1131325, PG */
  bool                                m_features_76;    /* PTS 1131325, PG */
  short                               m_is_livecachetypes;        /* PTS 1141233, PG */
  short                               m_is_livecachetypes_import; /* PTS 1141233, PG */
  IspcCTypedef *                      m_alignment_type;           /* PTS 1141233, PG */
  bool                                m_oid_8byte_aligned;        /* PTS 1141233, PG */
  short                               m_oid_cnt;                  /* PTS 1141233, PG */
  cgg250AvlTree< IspcCTypedef *,
                 IspcCTypedef,
                 IspcCGenInspecCode > m_index;
};

/*----------------------------------------------------------------------------*/

inline void * IspcCGenInspecCode::allocate(size_t sz) {
  return new char [sz];
}

/*----------------------------------------------------------------------------*/

inline void IspcCGenInspecCode::deallocate(void * p) {
  /* delete p            PTS 1109175 */
  delete [] (char*)p; /* PTS 1109175 */
}


/******************************************************************************/
/*     IspcCTypedef                                                           */
/******************************************************************************/

class IspcCTypedef
{
public:
  IspcCTypedef();
  ~IspcCTypedef();
  inline int                 Compare( const IspcCTypedef * l, const IspcCTypedef * r );
  void                       appendMember(IspcCTypedef * m);
  void                       appendToken(IspcCExprElem * s);
  short                      getMemberNum(IspcCGenInspecCode & CGenInsepcCode);
  int                        calExprValue(IspcCGenInspecCode & CGenInsepcCode);
  void                       setDimension(short d);
  void                       setName( const XmlElementType_Enum_ispc dt, const char * n );
  void                       setType( IspcCGenInspecCode & CGenInsepcCode, const char * t,
                                      short server_size, short server_align,
                                      short asc_client_size, short asc_client_align,
                                      short uni_client_size, short uni_client_align );
  void                       setValue( const XmlAttributeType_Enum_ispc vt, const char * vs );
  void                       setUuid(const char * uuid);
  void                       setStructCharacteristics(IspcCGenInspecCode & CGenInsepcCode);
  void                       printSubParameter( IspcCGenInspecCode & CGenInsepcCode, ofstream & fout,
                                                short & posServer, short & posAscClient, short & posUniClient );
  void                       checkIfAbapOrStreamHandle( IspcCGenInspecCode & CGenInsepcCode,
                                                        short * prm_type, short * prm_size, short * prm_elem_num,
                                                        IspcCTypedef * const ** row_def );
  bool                       isAbapTableHandle(IspcCGenInspecCode & CGenInsepcCode);
  short                      getServerAlign() const {return m_server_align;}
  void                       setServerAlign(short align) {m_server_align = align;} /* PTS 1141233, PG */
  short                      getAscClientAlign() const {return m_asc_client_align;}
  short                      getUniClientAlign() const {return m_uni_client_align;}
  short                      getDimension() const {return m_dimension;}
  cgg251DCList< IspcCTypedef *, IspcCGenInspecCode >
                             getMemberList() const {return m_member_list;}
  short                      getMemberNum() const {return m_member_num;}
  XmlElementType_Enum_ispc   getDefType() const {return m_def_type;}
  XmlAttributeType_Enum_ispc getValType() const {return m_value_type;}
  short                      getServerSize() const {return m_server_size;}
  short                      getAscClientSize() const {return m_asc_client_size;}
  short                      getUniClientSize() const {return m_uni_client_size;}
  short                      getVtType() const {return m_vt_type;}
  const char *               getType() const {return m_type;}
  const GUID * const         getUuid() const {return &m_uuid;}
  const char * const         getValue() const {return m_value_string;}
  const char * const         getName() const {return m_name;}
  unsigned char              getPtr() const {return m_ptr;}
  inline void *              allocate(size_t sz);
  inline void                deallocate(void * p);
private:
  cgg251DCList< IspcCTypedef *,  IspcCGenInspecCode> m_member_list;
  cgg251DCList< IspcCExprElem *, IspcCGenInspecCode> m_expr_list;
  GUID                                               m_uuid;
  XmlElementType_Enum_ispc                           m_def_type;
  char *                                             m_name;
  char *                                             m_type;
  XmlAttributeType_Enum_ispc                         m_value_type;
  char *                                             m_value_string;
  unsigned char                                      m_ptr;
  short                                              m_server_size;      /* C++ type size on server side */
  short                                              m_asc_client_size;  /* ASCII client type size       */
  short                                              m_uni_client_size;  /* Unicode client type size     */
  short                                              m_dimension;
  short                                              m_server_align;     /* C++ type alignment on server side */
  short                                              m_asc_client_align; /* ASCII client type alignment       */
  short                                              m_uni_client_align; /* Unicode client type alignment     */
  short                                              m_member_num;
  short                                              m_vt_type;
};

/*----------------------------------------------------------------------------*/

inline int IspcCTypedef::Compare( const IspcCTypedef * l, const IspcCTypedef * r ) {
  if (strcmp( l->m_name, r->m_name ) > 0) {
    return 1;
  }
  else if (strcmp( l->m_name, r->m_name ) < 0) {
    return -1;
  }
  return 0;
}


#endif /* ISPCCGENINSPECCODE_H */

