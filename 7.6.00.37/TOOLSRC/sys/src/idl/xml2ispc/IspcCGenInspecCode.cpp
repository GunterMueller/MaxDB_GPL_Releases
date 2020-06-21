/*!*****************************************************************************

  module      : IspcCGenInspecCode.cpp

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


#include <ctype.h>
#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#define endl std::endl
#else
#include <iostream.h>
#endif
#include "idl/xml2ispc/IspcErrorHandling.h"
#define  INITGUID    1     /* Define needed CoClassIDs + IIDs in geo36.h */
#include "idl/xml2ispc/IspcCGenInspecCode.h"
//#include "vsp001.h"
#include "idl/xml2ispc/Ispc_geo36.h"
#include "idl/xml2ispc/IspcVersion.h"


/******************************************************************************/
/*     Static variables                                                       */
/******************************************************************************/

static const GUID g_null_guid = { 0x0, 0x0, 0x0, { 0x0, 0x0, 0x0, 0xa0, 0x0, 0x0, 0x0, 0x0 } };

static GUID g_udt_guid = { 0x0, 0x0, 0x0, { 0x0, 0x0, 0x0, 0xa0, 0x0, 0x0, 0x0, 0x0 } };

static cgg251DCList< IspcCTypedef *, IspcCGenInspecCode > g_struct_level;

static IspcCTypedef * g_interface = NULL;
static IspcCTypedef * g_coclass   = NULL;

static short g_func_no  = 0;
static short g_param_no = 0;

static char g_act_method[65];


/******************************************************************************/
/*     Static functions                                                       */
/******************************************************************************/

static unsigned int hex2int(char x) {
  unsigned int i = 0;
  if ( ('0' <= x) && (x <= '9') ) {
    i = x - '0';
  }
  if ( ('A' <= x) && (x <= 'F') ) {
    i = (x - 'A') + 10;
  }
  if ( ('a' <= x) && (x <= 'f') ) {
    i = (x - 'a') + 10;
  }
  return i;
}

/*----------------------------------------------------------------------------*/

static double Ispc_GetFloat(IspcCExprElem & t) {
  return 0;
}

/*----------------------------------------------------------------------------*/

static void Ispc_PutFloat( double i, IspcCExprElem & r) {
}

/*----------------------------------------------------------------------------*/

static bool Ispc_IsInteger( IspcCGenInspecCode & CGenInsepcCode, IspcCExprElem & token ) {
  if (token.m_token_type == elemFixed_ispc) {
    return true;
  }
  if (token.m_token_type == elemFloat_ispc) {
    if (strchr( token.m_token_value, '.' )) {
      return true;
    }
  }
  return false;
}

/*----------------------------------------------------------------------------*/

static int Ispc_GetInteger( char * s, int & i ) {
  char * c = strchr( s, '+' );
  if (c) {
    *c = ' ';
  }
  c = strchr( s, '-' );
  while ( c && (*c+1) && (' ' == *c+1) ) {
    *c = ' ';
    c++;
    *c = '-';
  }
  return sscanf( s, "%d", &i );
}

/*----------------------------------------------------------------------------*/

static int Ispc_GetInteger( IspcCExprElem & t ) {
  int l;
  if (Ispc_GetInteger( t.m_token_value, l )) {
    return l;
  }
  else {
    return 0;
  }
}

/*----------------------------------------------------------------------------*/

static void Ispc_PutInteger( long l, IspcCExprElem & r) {
  char buf[32];
  r.m_token_type = elemFixed_ispc;
  sprintf( buf, "%d", l );
  r.m_token_value = new char[strlen(buf)+1];
  strcpy( r.m_token_value, buf );
}

/*----------------------------------------------------------------------------*/

static bool Ispc_isOperator( IspcCGenInspecCode & CGenInsepcCode, XmlElementType_Enum_ispc type ) {
  if ( (elemFloat_ispc == type) || (elemFixed_ispc == type) || (elemIdent_ispc == type) ) {
    return false;
  }
  else {
    return true;
  }
}

/*----------------------------------------------------------------------------*/

static bool Ispc_ExecuteOperator( IspcCGenInspecCode & CGenInsepcCode,
                                  cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode > & iter,
                                  IspcCExprElem & ResultValue );

static bool Ispc_GetParm( IspcCGenInspecCode & CGenInsepcCode,
                          cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode > & iter,
                          IspcCExprElem & ResultValue ) {
  iter++;
  if (iter) {
    IspcCExprElem * const token = *iter();
    if (Ispc_isOperator( CGenInsepcCode, token->m_token_type )) {
      if (!Ispc_ExecuteOperator( CGenInsepcCode, iter, ResultValue )) {
        return false;
      }
    }
    else {
      if (token->m_token_type == elemIdent_ispc) {
        IspcCTypedef findNode;
        findNode.setName( elemUnknown_ispc, token->m_token_value );
        IspcCTypedef * const * foundptr = CGenInsepcCode.findNode(findNode);
        if ( foundptr && ((*foundptr)->getValType() == attNumber_ispc) ) {
          if (strchr( (*foundptr)->getValue(), '.' )) {
            ResultValue.m_token_type = elemFloat_ispc;
          }
          else {
            ResultValue.m_token_type = elemFixed_ispc;
          }
          ResultValue.m_token_value = new char[strlen((*foundptr)->getValue())+1];
          strcpy( ResultValue.m_token_value, (*foundptr)->getValue() );
        }
        else {
          return false;
        }
      }
      else {
        ResultValue = *token;
      }
    }
    return true;
  }
  else {
    return false;
  }
}

/*----------------------------------------------------------------------------*/

static bool Ispc_Add( IspcCGenInspecCode & CGenInsepcCode,
                      cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode > & iter,
                      IspcCExprElem & ResultValue) {
  IspcCExprElem prm1, prm2;
  if ( (!Ispc_GetParm( CGenInsepcCode, iter, prm1 )) ||
       (!Ispc_GetParm( CGenInsepcCode, iter, prm2 )) ) {
    return false;
  }
  if ( Ispc_IsInteger( CGenInsepcCode, prm1 ) && Ispc_IsInteger( CGenInsepcCode, prm2 ) ) {
    Ispc_PutInteger( Ispc_GetInteger(prm1) + Ispc_GetInteger(prm2), ResultValue );
  }
  else {
    Ispc_PutFloat( Ispc_GetFloat(prm1) + Ispc_GetFloat(prm2), ResultValue );
  }
  return true;
}

/*----------------------------------------------------------------------------*/

static bool Ispc_Mul( IspcCGenInspecCode & CGenInsepcCode,
                      cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode > & iter,
                      IspcCExprElem & ResultValue) {
  IspcCExprElem prm1, prm2;
  if ( (!Ispc_GetParm( CGenInsepcCode, iter, prm1 )) ||
       (!Ispc_GetParm( CGenInsepcCode, iter, prm2 )) ) {
    return false;
  }  
  if ( Ispc_IsInteger( CGenInsepcCode, prm1 ) && Ispc_IsInteger( CGenInsepcCode, prm2 ) ) {
    Ispc_PutInteger( Ispc_GetInteger(prm1) * Ispc_GetInteger(prm2), ResultValue );
  }
  else {
    Ispc_PutFloat( Ispc_GetFloat(prm1) * Ispc_GetFloat(prm2), ResultValue );
  }
  return true;
}

/*----------------------------------------------------------------------------*/

static bool Ispc_Div( IspcCGenInspecCode & CGenInsepcCode,
                      cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode > & iter,
                      IspcCExprElem &ResultValue ) {
  IspcCExprElem prm1, prm2;
  if ( (!Ispc_GetParm( CGenInsepcCode, iter, prm1 )) ||
       (!Ispc_GetParm( CGenInsepcCode, iter, prm2 )) ) {
    return false;
  }
  if ( Ispc_IsInteger( CGenInsepcCode, prm1 ) && Ispc_IsInteger( CGenInsepcCode, prm2 ) ) {
    Ispc_PutInteger( Ispc_GetInteger(prm1) / Ispc_GetInteger(prm2), ResultValue );
  }
  else {
    Ispc_PutFloat( Ispc_GetFloat(prm1) / Ispc_GetFloat(prm2), ResultValue );
  }
  return true;
}

/*----------------------------------------------------------------------------*/

static bool Ispc_Mod( IspcCGenInspecCode & CGenInsepcCode,
                      cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode > & iter,
                      IspcCExprElem &ResultValue ) {
  IspcCExprElem prm1, prm2;
  if ( (!Ispc_GetParm( CGenInsepcCode, iter, prm1 )) ||
       (!Ispc_GetParm( CGenInsepcCode, iter, prm2 )) ) {
    return false;
  }
  if ( Ispc_IsInteger( CGenInsepcCode, prm1 ) && Ispc_IsInteger( CGenInsepcCode, prm2 ) ) {
    Ispc_PutInteger( Ispc_GetInteger(prm1) % Ispc_GetInteger(prm2), ResultValue );
  }
  else {
    return false;
  }
  return true;
}

/*----------------------------------------------------------------------------*/

static bool Ispc_Subtract( IspcCGenInspecCode & CGenInsepcCode,
                           cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode > & iter,
                           IspcCExprElem & ResultValue ) {
  IspcCExprElem prm1, prm2;
  if ( (!Ispc_GetParm( CGenInsepcCode, iter, prm1 )) ||
       (!Ispc_GetParm( CGenInsepcCode, iter, prm2 )) ) {
    return false;
  }
  if ( Ispc_IsInteger( CGenInsepcCode, prm1 ) && Ispc_IsInteger( CGenInsepcCode, prm2 ) ) {
    Ispc_PutInteger( Ispc_GetInteger(prm1) - Ispc_GetInteger(prm2), ResultValue );
  }
  else {
    Ispc_PutFloat( Ispc_GetFloat(prm1) - Ispc_GetFloat(prm2), ResultValue );
  }
  return true;
}

/*----------------------------------------------------------------------------*/

static bool Ispc_ExecuteOperator( IspcCGenInspecCode & CGenInsepcCode,
                           cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode > & iter,
                           IspcCExprElem & ResultValue ) {
  IspcCExprElem * const token = *iter();
  switch(token->m_token_type) {
    case elemSub_ispc : {
      return Ispc_Subtract( CGenInsepcCode, iter, ResultValue );
    }
    case elemAdd_ispc : {
      return Ispc_Add( CGenInsepcCode, iter, ResultValue );
    }
    case elemMul_ispc : {
      return Ispc_Mul( CGenInsepcCode, iter, ResultValue );
    }
    case elemDiv_ispc : {
      return Ispc_Div( CGenInsepcCode, iter, ResultValue );
    }
    case elemMod_ispc : {
      return Ispc_Mod( CGenInsepcCode, iter, ResultValue );
    }
    default : {
      break;
    }
  }
  return false;
}


/******************************************************************************/
/*     IspcCExprElem                                                          */
/******************************************************************************/

IspcCExprElem::IspcCExprElem()
: m_token_type(elemUnknown_ispc)
, m_token_value(NULL)
{}

/*----------------------------------------------------------------------------*/

IspcCExprElem::~IspcCExprElem() {
  if (m_token_value) {
    delete [] m_token_value;
    m_token_value = NULL;
  }
}

/*----------------------------------------------------------------------------*/

const IspcCExprElem & IspcCExprElem::operator= (const IspcCExprElem & right) {
  this->m_token_type = right.m_token_type;
  if (right.m_token_value) {
    this->m_token_value = new char[strlen(right.m_token_value)+1];
    strcpy( this->m_token_value, right.m_token_value );
  }
  else {
    this->m_token_value = NULL;
  }
  return *this;
}


/******************************************************************************/
/*     IspcCTypedef                                                           */
/******************************************************************************/

IspcCTypedef::IspcCTypedef() {
  m_uuid.Data1       = 0;
  m_uuid.Data2       = 0;
  m_uuid.Data3       = 0;
  memset( m_uuid.Data4, '\0', sizeof(m_uuid.Data4) );
  m_def_type         = elemUnknown_ispc;
  m_name             = NULL;
  m_type             = NULL;
  m_value_type       = attUnknown_ispc;
  m_value_string     = NULL;
  m_ptr              = 0;
  m_server_size      = -1;
  m_asc_client_size  = -1;
  m_uni_client_size  = -1;
  m_dimension        = -1;
  m_server_align     = -1;
  m_asc_client_align = -1;
  m_uni_client_align = -1;
  m_member_num       = 0;
  m_vt_type          = csp1_vt_empty;
}

/*----------------------------------------------------------------------------*/

IspcCTypedef::~IspcCTypedef() {
  if (m_name) {
    delete [] m_name;
    m_name = NULL;
  }
  if (m_type) {
    delete [] m_type;
    m_type = NULL;
  }
  if (m_value_string) {
    delete [] m_value_string;
    m_value_string = NULL;
  }
}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::appendMember(IspcCTypedef * m) {
  IspcCTypedef findNode;
  m_member_list.push_back(m);
  m_member_num++;
}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::appendToken(IspcCExprElem * t) {
  m_expr_list.push_back(t);
}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::setDimension(short d) {
  m_dimension = d;
}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::setName( const XmlElementType_Enum_ispc vt, const char * n ) {
  m_def_type  = vt;
  m_name      = new char[strlen(n)+1];
  strcpy( m_name, n );
  if ( (vt == elemStruct_ispc) || (vt == elemUnion_ispc) ) { /* PTS 1141233, PG */
    m_vt_type = csp1_vt_userdefined;
  }
}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::setStructCharacteristics(IspcCGenInspecCode & CGenInsepcCode) {

  IspcCTypedef * curr;
  m_server_size      = 0;
  m_asc_client_size  = 0;
  m_uni_client_size  = 0;
  m_server_align     = 0;
  m_asc_client_align = 0;
  m_uni_client_align = 0;
  IspcCTypedef findNode;

  for ( cgg251dclIterator< IspcCTypedef *, IspcCGenInspecCode > iter = m_member_list.begin(); iter; ++iter ) {

    curr = *iter();

    short l_dim = 1;
    if (curr->m_dimension > 0) {
      l_dim = curr->m_dimension;
    }
    if (curr->m_type) {
      findNode.setName( elemUnknown_ispc, curr->m_type );
      IspcCTypedef * const * foundptr = CGenInsepcCode.findNode(findNode);
      if ( foundptr && ((*foundptr)->m_dimension > 0) ) {
        l_dim = (*foundptr)->m_dimension * l_dim;
      }
    }

    if (curr->m_server_align > m_server_align) {
      m_server_align = curr->m_server_align;
    }
    if (curr->m_asc_client_align > m_asc_client_align) {
      m_asc_client_align = curr->m_asc_client_align;
    }
    if (curr->m_uni_client_align > m_uni_client_align) {
      m_uni_client_align = curr->m_uni_client_align;
    }

    m_server_size = m_server_size + (curr->m_server_align - 1);
    m_server_size = m_server_size - (m_server_size % curr->m_server_align);
    m_server_size = m_server_size + (l_dim * curr->m_server_size);

    m_asc_client_size = m_asc_client_size + (curr->m_asc_client_align - 1);
    m_asc_client_size = m_asc_client_size - ( m_asc_client_size % curr->m_asc_client_align );
    m_asc_client_size = m_asc_client_size + (l_dim * curr->m_asc_client_size);

    m_uni_client_size = m_uni_client_size + (curr->m_uni_client_align - 1);
    m_uni_client_size = m_uni_client_size - ( m_uni_client_size % curr->m_uni_client_align );
    m_uni_client_size = m_uni_client_size + (l_dim * curr->m_uni_client_size);

  }

}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::setType( IspcCGenInspecCode & CGenInsepcCode, const char * t,
                            short server_size, short server_align,
                            short asc_client_size, short asc_client_align,
                            short uni_client_size, short uni_client_align ) {

  if (strchr( t, '*' )) {
    m_type = new char[strlen(t)];
    strncpy( m_type, t, strlen(t)-1 );
    m_type[strlen(t)-1] = '\0';
    m_ptr = 1;
  }
  else {
    m_type = new char[strlen(t)+1];
    strcpy( m_type, t );
    m_ptr = 0;
  }

  m_server_size      = server_size;
  m_server_align     = server_align;
  m_asc_client_size  = asc_client_size;
  m_asc_client_align = asc_client_align;
  m_uni_client_size  = uni_client_size;
  m_uni_client_align = uni_client_align;

  /* 8 byte integers, PTS 1141233, PG */

  if ( ( ( (!strcmp( "hyper", t )) ||
           (!strcmp( "long" , t )) || (!strcmp( "long long" , t )) ) &&
         (!strcmp( "OmsTypeInt64", m_name )) ) ||
       ( (!strcmp( "OmsTypeInt64", t )) && (!strcmp( "OmsTypeInt8", m_name )) ) ) {
    m_server_size      = 8;
    m_server_align     = 8;
    m_asc_client_size  = 8;
    m_asc_client_align = 8;
    m_uni_client_size  = 8;
    m_uni_client_align = 8;
    m_vt_type          = csp1_vt_i8;
    return;
  }
  if ( ( ( (!strcmp( "unsigned hyper", t )) ||
           (!strcmp( "unsigned long" , t )) || (!strcmp( "unsigned long long" , t )) ) &&
         (!strcmp( "OmsTypeUInt64", m_name )) ) ||
       ( (!strcmp( "OmsTypeUInt64", t )) && (!strcmp( "OmsTypeUInt8", m_name )) ) ) {
    m_server_size      = 8;
    m_server_align     = 8;
    m_asc_client_size  = 8;
    m_asc_client_align = 8;
    m_uni_client_size  = 8;
    m_uni_client_align = 8;
    m_vt_type          = csp1_vt_ui8;
    return;
  }

  /* Check other OMS datatypes */

  if (0 == memcmp( &GUID_LC_CHAR, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type          = csp1_vt_lc_char;
    m_uni_client_size  = 2;
    m_uni_client_align = 2;
    return;
  }

  if (0 == memcmp( &GUID_LC_WCHAR, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type = csp1_vt_lc_wchar;
    return;
  }

  if (0 == memcmp( &GUID_LC_BYTE, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type = csp1_vt_lc_byte;
    return;
  }

  if (0 == memcmp( &GUID_LC_BOOL, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type = csp1_vt_lc_bool;
    return;
  }

  if (0 == memcmp( &GUID_LC_INT1, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type = csp1_vt_lc_int1;
    return;
  }

  if (0 == memcmp( &GUID_LC_UINT1, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type = csp1_vt_lc_uint1;
    return;
  }

  if (0 == memcmp( &GUID_LC_ABAP_TAB_HANDLE, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type = csp1_vt_lc_abap_tab_handle;
    return;
  }

  if (0 == memcmp( &GUID_OMSTYPESTREAMHANDLE, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type = csp1_vt_lc_stream_handle;
    return;
  }

  if (0 == memcmp( &GUID_LC_PARAM_DESC, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type = csp1_vt_lc_kb71_param_desc;
    return;
  }

  if (0 == memcmp( &GUID_LC_ASCII7, &m_uuid, sizeof(m_uuid) )) {
    m_server_size       = 1;
    m_server_align      = 1;
    m_asc_client_size   = 1;
    m_asc_client_align  = 1;
    m_uni_client_size   = 2;
    m_uni_client_align  = 2;
    m_vt_type           = csp1_vt_lc_ascii7;
    return;
  }

  if (0 == memcmp( &GUID_LC_WYDE, &m_uuid, sizeof(m_uuid) )) {
    m_server_size       = 2;
    m_server_align      = 2;
    m_asc_client_size   = 1;
    m_asc_client_align  = 1;
    m_uni_client_size   = 2;
    m_uni_client_align  = 2;
    m_vt_type           = csp1_vt_lc_wyde;
    return;
  }

  /* PTS 1131555, PG */
  if (0 == memcmp( &GUID_LC_NUMC, &m_uuid, sizeof(m_uuid) )) {
    m_vt_type          = csp1_vt_lc_numc;
    m_uni_client_size  = 2;
    m_uni_client_align = 2;
    return;
  }

  /* Check basis C++ types */

  if (csp1_vt_empty == m_vt_type) {
    m_vt_type = CGenInsepcCode.checkBasisType(m_type);
    // check if user defined Type
    if (csp1_vt_empty == m_vt_type) {
      IspcCTypedef findNode;
      findNode.setName( elemUnknown_ispc, m_type );
      IspcCTypedef * const * foundptr = CGenInsepcCode.findNode(findNode);
      if (foundptr) {
        m_vt_type = (*foundptr)->m_vt_type;
        while ( foundptr && (csp1_vt_userdefined == m_vt_type) ) { /* PTS 1108040 UH 2000-10-13 */
          if ((*foundptr)->m_type) {
            findNode.setName( elemUnknown_ispc, (*foundptr)->m_type );
            foundptr = CGenInsepcCode.findNode(findNode);
            m_vt_type = (*foundptr)->m_vt_type;
          }
          else {
            foundptr = NULL;
          }
        }
        if (m_vt_type == csp1_vt_empty) {
          m_vt_type = csp1_vt_userdefined;
        }
        foundptr = NULL;
      }
    }
  }

}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::setValue( const XmlAttributeType_Enum_ispc vt, const char * vs) {
  m_value_type   = vt;
  m_value_string = new char[strlen(vs)+1];
  strcpy( m_value_string, vs );
}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::setUuid(const char * uuid_p) {

  int i;
  int j;
  char uuid[64];
  const char * pc = uuid_p;

  i = 0;
  for ( ; (*pc != '\0'); pc++ ) {
    if (*pc != ' ') {
      uuid[i] = *pc;
      i++;
    }
  }
  uuid[i] = 0;

  m_uuid.Data1 = 0;
  m_uuid.Data2 = 0;
  m_uuid.Data3 = 0;

  if (0 == strlen(uuid)) {
    m_uuid = g_null_guid;
  }
  else {
    for ( i = 0; i < 8; i = i+2 ) {
      m_uuid.Data1 = m_uuid.Data1 * 256 + hex2int(uuid[i]) * 16 + hex2int(uuid[i+1]);
    }
    for ( i = 9; i < 13; i = i+2 ) {
      m_uuid.Data2 = m_uuid.Data2 * 256 + hex2int(uuid[i]) * 16 + hex2int(uuid[i+1]);
    }
    for ( i = 14; i < 18; i = i+2 ) {
      m_uuid.Data3 = m_uuid.Data3 * 256 + hex2int(uuid[i]) * 16 + hex2int(uuid[i+1]);
    }
    for ( i = 19, j = 0; i < 23; i = i+2, j++ ) {
      m_uuid.Data4[j] = hex2int(uuid[i]) * 16 + hex2int(uuid[i+1]);
    }
    for ( i = 24, j = 2; i < 36; i = i+2, j++ ) {
      m_uuid.Data4[j] = hex2int(uuid[i]) * 16 + hex2int(uuid[i+1]);
    }
  }

}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::checkIfAbapOrStreamHandle( IspcCGenInspecCode & CGenInsepcCode,
                                              short * prm_type, short * prm_size, short * prm_elem_num,
                                              IspcCTypedef * const ** row_def ) {
  if (2 == m_member_num) {
    cgg251dclIterator< IspcCTypedef *, IspcCGenInspecCode > iter = m_member_list.begin();
    IspcCTypedef * first  = *iter();
    if (first->m_ptr) {
      ++iter;
      IspcCTypedef * second = *iter();
      if (csp1_vt_userdefined == second->m_vt_type) {
        IspcCTypedef findNode;
        findNode.setName( elemUnknown_ispc, second->m_type );
        IspcCTypedef * const * foundptr = CGenInsepcCode.findNode(findNode);
        findNode.setName( elemUnknown_ispc, first->m_type );
        if ( (0 == memcmp( &GUID_LC_ABAP_TAB_HANDLE,  &(*foundptr)->m_uuid, sizeof(m_uuid) )) ||
             (0 == memcmp( &GUID_OMSTYPESTREAMHANDLE, &(*foundptr)->m_uuid, sizeof(m_uuid) )) ||
             (0 == memcmp( &GUID_LC_PARAM_DESC,       &(*foundptr)->m_uuid, sizeof(m_uuid) )) ) {
          if (0 == memcmp( &GUID_LC_ABAP_TAB_HANDLE, &(*foundptr)->m_uuid, sizeof(m_uuid) )) {
            *prm_type = csp1_vt_lc_abap_tab_handle;
          }
          else if (0 == memcmp( &GUID_OMSTYPESTREAMHANDLE, &(*foundptr)->m_uuid, sizeof(m_uuid) )) {
            *prm_type = csp1_vt_lc_stream_handle;
          }
          else {
            *prm_type = csp1_vt_lc_kb71_param_desc;
          }
          IspcCTypedef * const * foundptr = CGenInsepcCode.findNode(findNode);
          /*+++++ PTS 1110440 +++++*/
          while ( foundptr &&
                  (elemTypedef_ispc == (*foundptr)->getDefType()) &&
                  (csp1_vt_userdefined == (*foundptr)->getVtType()) &&
                  ((*foundptr)->getType()) ) {
            findNode.setName( elemUnknown_ispc, (*foundptr)->getType() );
            foundptr = CGenInsepcCode.findNode(findNode);
          }
          /*----- PTS 1110440 -----*/
          if ( foundptr && (*foundptr) ) { /* PTS 1110445 */
            *row_def = foundptr;
            *prm_size = (*foundptr)->m_server_size;
            /* if ( (*foundptr)->m_def_type == elemTypedef_ispc )
             *   prm_elem_num = 1;
             * else  PTS 1110440 */
            *prm_elem_num = (*foundptr)->getMemberNum(CGenInsepcCode);
          }                                                                      /* PTS 1110445 */
          else {                                                                 /* PTS 1110445 */
            *row_def      = NULL;                                                /* PTS 1110445 */
            *prm_size     = -1;                                                  /* PTS 1110445 */
            *prm_elem_num = -1;                                                  /* PTS 1110445 */
            Ispc_ErrorMsg( C_TYPE_UNKNOWN_ISPC, this->m_name, findNode.m_name ); /* PTS 1110445 */
          }                                                                      /* PTS 1110445 */
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

bool IspcCTypedef::isAbapTableHandle(IspcCGenInspecCode & CGenInsepcCode) {
  bool isATH = false;
  if (2 == m_member_num) {
    cgg251dclIterator< IspcCTypedef *, IspcCGenInspecCode > iter = m_member_list.begin();
    IspcCTypedef * first  = *iter();
    if (first->m_ptr) {
      ++iter;
      IspcCTypedef * second = *iter();
      if (csp1_vt_userdefined == second->m_vt_type) {
        IspcCTypedef findNode;
        findNode.setName( elemUnknown_ispc, second->m_type );
        IspcCTypedef * const * foundptr = CGenInsepcCode.findNode(findNode);
        if (0 == memcmp( &GUID_LC_ABAP_TAB_HANDLE, &(*foundptr)->m_uuid, sizeof(m_uuid) )) {
          isATH = true;
        }
      }
    }
  }
  return isATH;
}

/*----------------------------------------------------------------------------*/

int IspcCTypedef::calExprValue(IspcCGenInspecCode & CGenInsepcCode) {
  int rc = 0;
  m_value_type = attNumber_ispc;
  cgg251dclIterator< IspcCExprElem *, IspcCGenInspecCode> iter = m_expr_list.begin();
  if (iter) {
    IspcCExprElem * const token = *iter();
    if (Ispc_isOperator( CGenInsepcCode, token->m_token_type )) {
      IspcCExprElem l_ResultValue;
      if (!Ispc_ExecuteOperator( CGenInsepcCode, iter, l_ResultValue )) {
        switch (this->getDefType()) {
          case elemConst_ispc : {
            Ispc_ErrorMsg( C_CONST_EXPR_NOT_VALID_ISPC, m_value_string, m_name, m_type );
            rc = C_CONST_EXPR_NOT_VALID_ISPC;
            break;
          }
          default : {
            break;
          }
        }
      }
      else {
        if (m_value_string) {
          delete [] m_value_string;
        }
        m_value_string = new char[strlen(l_ResultValue.m_token_value)+1];
        strcpy( m_value_string, l_ResultValue.m_token_value );
      }
    }
    else {
      iter++;
      if (iter) {
        switch (this->getDefType()) {
          case elemConst_ispc: {
            Ispc_ErrorMsg( C_CONST_EXPR_NOT_VALID_ISPC, m_value_string, m_name, m_type );
            rc = C_CONST_EXPR_NOT_VALID_ISPC;
            break;
          }
          default : {
            break;
          }
        }
      }
      else {
        if (m_value_string) {
          delete [] m_value_string;
        }
        m_value_string = new char[strlen(token->m_token_value)+1];
        strcpy( m_value_string, token->m_token_value );
      }
    }
  }
  else {
    switch (this->getDefType()) {
      case elemConst_ispc : {
        Ispc_ErrorMsg( C_CONST_EXPR_NOT_VALID_ISPC, m_value_string, m_name, m_type );
        rc = C_CONST_EXPR_NOT_VALID_ISPC;
        break;
      }
      default : {
        break;
      }
    }
  }
  return rc;
}

/*----------------------------------------------------------------------------*/

short IspcCTypedef::getMemberNum(IspcCGenInspecCode & CGenInsepcCode) {
  short l_member_num = m_member_num;
  if (m_def_type == elemTypedef_ispc) {
    l_member_num = 1;
  }
  else {
    for ( cgg251dclIterator< IspcCTypedef *, IspcCGenInspecCode > iter = m_member_list.begin();
          iter; iter++ ) {
      IspcCTypedef const * curr  = *iter();
      IspcCTypedef findNode;
      findNode.setName( elemUnknown_ispc, curr->m_type );
      IspcCTypedef * const * foundptr = CGenInsepcCode.findNode(findNode);
      while ( foundptr && (0 == (*foundptr)->m_member_num) ) {
        findNode.setName( elemUnknown_ispc, (*foundptr)->m_type );
        foundptr = CGenInsepcCode.findNode(findNode);
      }
      if ( foundptr && (1 < (*foundptr)->m_member_num) ) {
        l_member_num = l_member_num + (*foundptr)->getMemberNum(CGenInsepcCode) - 1;
      }
    }
  }
  return l_member_num;
}

/*----------------------------------------------------------------------------*/

void IspcCTypedef::printSubParameter( IspcCGenInspecCode & CGenInsepcCode, ofstream & fout,
                                      short & posServer, short & posAscClient, short & posUniClient ) {

  IspcCTypedef * curr = NULL;
  IspcCTypedef * base = NULL;
  short l_typ;
  short l_subtyp;
  short l_dim;
  short l_server_size, l_asc_client_size, l_uni_client_size;
  unsigned int i;

  cgg251dclIterator< IspcCTypedef *, IspcCGenInspecCode > iter = m_member_list.begin();
  if (this->m_def_type == elemTypedef_ispc) {
    curr = this;
  }
  else {
    if (iter) {
      curr = *iter();
    }
  } 

  for ( ; curr; ) {

    g_param_no++;
    const GUID * const guid = curr->getUuid();
    const char * t = curr->getType();

    if ( (0 == strcmp( t, "hyper" ))             ||
         (0 == strcmp( t, "signed hyper" ))      ||
         (0 == strcmp( t, "unsigned hyper" ))    ||
         (0 == strcmp( t, "unsigned" ))          ||
         (0 == strcmp( t, "long" ))              ||
         (0 == strcmp( t, "signed long" ))       ||
         (0 == strcmp( t, "unsigned long" ))     ||
         (0 == strcmp( t, "long int" ))          ||
         (0 == strcmp( t, "signed long int" ))   ||
         (0 == strcmp( t, "unsigned long int" )) ||
         (0 == strcmp( t, "bool" ))              ||
         (0 == strcmp( t, "boolean" ))           ||
         (0 == strcmp( t, "char" )) ) {
      char l_buf[128];
      strcpy( l_buf, "member:" );
      strcat( l_buf, curr->getName() );
      l_server_size = l_asc_client_size = l_uni_client_size = -1;
      Ispc_WarnMsg( C_TYPE_NOT_PORTABLE_ISPC, l_buf, t );
    }

    if (curr->m_dimension > 0) {
      l_dim = curr->m_dimension;
    }
    else {
      l_dim = 0;
    }

    IspcCTypedef findNode;
    findNode.setName( elemUnknown_ispc, curr->m_type );
    IspcCTypedef * const * foundptr = CGenInsepcCode.findNode(findNode);
    while ( foundptr && (0 == (*foundptr)->m_member_num) &&
            (csp1_vt_userdefined /*empty **FF** */ == (*foundptr)->m_vt_type) ) {
      if ((*foundptr)->m_dimension > 0) {
        if (l_dim) {
          l_dim = l_dim * (*foundptr)->m_dimension;
        }
        else {
          l_dim = (*foundptr)->m_dimension;
        }
      }
      findNode.setName( elemUnknown_ispc, (*foundptr)->m_type );
      foundptr = CGenInsepcCode.findNode(findNode);
    }

    if ( foundptr && (1 < (*foundptr)->m_member_num) ) {

      g_param_no--;
      short struct_align_server     = (*foundptr)->getServerAlign();
      short struct_align_asc_client = (*foundptr)->getAscClientAlign();
      short struct_align_uni_client = (*foundptr)->getUniClientAlign();
      /* align position */
      if (posServer % struct_align_server != 0) {
        posServer += (struct_align_server - posServer % struct_align_server);
        if ( ispc_getWarnLevel() &&  (!ispc_getMultiOffsets()) ) {
          Ispc_WarnMsg( C_STRUCT_NOT_ALIGNED_ISPC, g_act_method, this->getName(), (*foundptr)->getName() );
        }
      }
      /* align position */
      if (posAscClient % struct_align_asc_client != 0) {
        posAscClient += (struct_align_asc_client - posAscClient % struct_align_asc_client);
      }
      /* align position */
      if (posUniClient % struct_align_uni_client != 0) {
        posUniClient += (struct_align_uni_client - posUniClient % struct_align_uni_client);
      }
      (*foundptr)->printSubParameter( CGenInsepcCode, fout, posServer, posAscClient, posUniClient );
      if (posServer % struct_align_server != 0) {
        posServer += (struct_align_server - posServer % struct_align_server);
        if ( ispc_getWarnLevel() && (!ispc_getMultiOffsets()) ) { /* +++ PTS 1108533 +++ */
          Ispc_WarnMsg( C_STRUCT_NOT_ALIGNED_ISPC, g_act_method, this->getName(), (*foundptr)->getName() );
        }
      }
      if (posAscClient % struct_align_asc_client != 0) {
        posAscClient += (struct_align_asc_client - posAscClient % struct_align_asc_client);
      }
      if (posUniClient % struct_align_uni_client != 0) {
        posUniClient += (struct_align_uni_client - posUniClient % struct_align_uni_client);
      }

    }
    else {

      if ( foundptr && (1 == (*foundptr)->m_member_num) ) {
        base = *((*foundptr)->m_member_list.begin())();
        l_server_size     = curr->getServerSize();
        l_asc_client_size = curr->getAscClientSize();
        l_uni_client_size = curr->getUniClientSize();
        l_typ = base->m_vt_type;
        l_subtyp = 0;
        if (0 < base->m_dimension) {
          l_dim = base->m_dimension;
        }
        if (base->m_type) {
          findNode.setName( elemUnknown_ispc, base->m_type );
          foundptr = CGenInsepcCode.findNode(findNode);
        }
        while (foundptr) {
          l_typ = (*foundptr)->m_vt_type;
          if ((*foundptr)->m_dimension > 0) {
            if (l_dim) {
              l_dim = l_dim * (*foundptr)->m_dimension;
            }
            else {
              l_dim = (*foundptr)->m_dimension;
            }
          }
          if ((*foundptr)->m_type) {
            findNode.setName( elemUnknown_ispc, (*foundptr)->m_type );
            foundptr = CGenInsepcCode.findNode(findNode);
          }
// Hier geht es los.
          else if (1 == (*foundptr)->m_member_num) {
            base = *((*foundptr)->m_member_list.begin())();
            if (base->m_type) {
              findNode.setName( elemUnknown_ispc, base->m_type );
              foundptr = CGenInsepcCode.findNode(findNode);
              if (!foundptr) {
                l_typ = base->m_vt_type;
              }
              if (0 < base->m_dimension) {
                l_dim = base->m_dimension;
              }
            }
            else {
              foundptr = NULL;
            }
          }
// Hier ist es fertig.
          else {
            foundptr = NULL;
          }
        }
      }
      else {
        l_server_size     = curr->getServerSize();
        l_asc_client_size = curr->getAscClientSize();
        l_uni_client_size = curr->getUniClientSize();
        if (m_ptr) {
          l_typ = csp1_vt_ptr;
          l_subtyp = curr->getVtType();
        }
        else {
          l_typ = curr->getVtType();
          l_subtyp = 0;
          if (curr->getVtType() == csp1_vt_empty) {
            if (foundptr) {
              l_typ = (*foundptr)->m_vt_type;
            }
          }
          if ( foundptr && (0 < (*foundptr)->m_dimension) ) {
            if (l_dim) {
              l_dim = (*foundptr)->m_dimension * l_dim;
            }
            else {
              l_dim = (*foundptr)->m_dimension;
            }
          }
          if (l_dim > 1) {
            l_server_size     = l_server_size * l_dim;
            l_asc_client_size = l_asc_client_size * l_dim;
            l_uni_client_size = l_uni_client_size * l_dim;
          }
        }
      }

      if ( (l_dim > 1) && (l_typ != csp1_vt_ptr) ) {
        l_subtyp = l_typ;
        l_typ    = csp1_vt_carray;
      }

      /* align position */
      if (posServer % curr->getServerAlign() != 0) {
        posServer += (curr->getServerAlign() - posServer % curr->getServerAlign());
        if ( ispc_getWarnLevel() && (!ispc_getMultiOffsets()) ) { /* +++ PTS 1108533 +++ */
          Ispc_WarnMsg( C_STRUCT_NOT_ALIGNED_ISPC, g_act_method, this->getName(), curr->getName() );
        }
      }
      if (posAscClient % curr->getAscClientAlign() != 0) {
        posAscClient += (curr->getAscClientAlign() - posAscClient % curr->getAscClientAlign());
      }
      if (posUniClient % curr->getUniClientAlign() != 0) {
        posUniClient += (curr->getUniClientAlign() - posUniClient % curr->getUniClientAlign());
      }

      if (!m_ptr) {
        fout << "," << endl << "    (char*)\"MAN\"";
        /* within struct-parameters don't signal csp1_vt_lc_uint1 */
        if (l_typ == csp1_vt_lc_uint1) {
          l_typ = csp1_vt_lc_byte;
        }
        //       if ( l_dim > l_server_size) DebugBreak();
        char cBuf[6];
        strcpy( cBuf, "I/O" );
        for ( i = 0; i < strlen(cBuf); i++ ) {
          cBuf[i] = toupper(cBuf[i]);
        }
        fout << ", (char*)\"" << cBuf << "\""
             << ", " << l_typ 
             << ", " << l_subtyp
             << ", " << l_server_size
             << ", " << l_dim;
        if (ispc_getMultiOffsets()) {
          fout << ", " << posServer
               << ", " << posAscClient
               << ", " << posUniClient;
        }
        else {
          fout << ", " << posServer;
        }
        fout << ", 0,";
        // fout.setf(ios::hex); doesn't work on sun
        char cData[9];
        sprintf( cData, "%x", guid->Data1 );
        fout << " { 0x" << cData;
        sprintf( cData, "%x", guid->Data2 );
        fout << ", 0x" << cData;
        sprintf( cData, "%x", guid->Data3 );
        fout << ", 0x" << cData << ", {";
        //fout << " { 0x" << guid->Data1 << ", 0x" << guid->Data2 << ", 0x"
        //  << guid->Data3 << ", {";
        for ( i = 0; i < 7; i++ ) {
          sprintf( cData, "%x", (unsigned short)guid->Data4[i] );
          fout << " 0x" << cData << ",";
          //fout << " 0x" << (unsigned short)guid->Data4[i] << ",";
        }
        //      fout << " 0x" << (unsigned short)guid->Data4[7] << " }}, (char*)\"" << curr->getName() << "\"";
        sprintf( cData, "%x", (unsigned short)guid->Data4[7] );
        fout << " 0x" << cData << " }}, (char*)\"\"";
        //fout << " 0x" << (unsigned short)guid->Data4[7] << " }}, (char*)\"\"";
        // fout.unsetf(ios::hex);
      }
      else {
        Ispc_ErrorMsg( C_POINTER_NOT_ALLOWED_ISPC, g_act_method, this->getName(), curr->getName() );
        return;
      }

      posServer    = posServer + l_server_size;
      posAscClient = posAscClient + l_asc_client_size;
      posUniClient = posUniClient + l_uni_client_size;

    }

    curr = NULL;
    if (this->m_def_type != elemTypedef_ispc) {
      iter++;
      if (iter) {
        curr = *iter();
      }
    }

  }

}


/******************************************************************************/
/*     IspcCGenInspecCode                                                     */
/******************************************************************************/

IspcCGenInspecCode::IspcCGenInspecCode() {
  m_act_pType = NULL;
  m_expr_flag = false;
  m_gen_tab_handle = NULL;  /* PTS 1131325, PG */
  m_features_76    = false; /* PTS 1131325, PG */
  m_is_livecachetypes        = 0;     /* PTS 1141233, PG */
  m_is_livecachetypes_import = 0;     /* PTS 1141233, PG */
  m_alignment_type           = NULL;  /* PTS 1141233, PG */
  m_oid_8byte_aligned        = false; /* PTS 1141233, PG */
  m_oid_cnt                  = 0;     /* PTS 1141233, PG */
}

/*----------------------------------------------------------------------------*/

IspcCGenInspecCode::~IspcCGenInspecCode() {

  delete g_interface;
  g_interface = NULL;
  delete g_coclass;
  g_coclass = NULL;

  m_cpp_out << endl; /* PTS 1110440 */
  m_cpp_out.close();
  m_h_out << endl;   /* PTS 1110440 */
  m_h_out.close(); 

}

/*----------------------------------------------------------------------------*/

int IspcCGenInspecCode::Init( char *  cFilename, bool features_76 ) {

  m_is_livecachetypes = !strncmp( cFilename, "OMS_LiveCacheTypes" , 18 ); /* PTS 1141233, PG */
  m_features_76 = features_76; /* PTS 1131325, PG */

  strcpy( m_filename, cFilename );
  char * pc = strrchr( m_filename, '.' );
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

  m_cpp_out.open(strcat( m_filename, "Ispc.cpp" ));
  if (m_cpp_out.fail()) {
    return 0;
  }

  pc = strrchr( m_filename, '.' );
  if (pc) {
    *(pc) = '\0';
  }
  m_h_out.open(strcat( m_filename, ".h" ));
  if (m_h_out.fail()) {
    return 0;
  }

  pc = strrchr( m_filename, '\\' );
  if (pc) {
    *(pc+1) = '\0';
  }

  m_cpp_out                                                     << endl;
  m_cpp_out << "#include \"" << &m_name[0] << "Ispc.h\""        << endl;
  m_cpp_out                                                     << endl;
  m_cpp_out                                                     << endl;
  m_cpp_out << "extern \"C\" void __stdcall"                    << endl;
  m_cpp_out << "CO_GetVersion ( int *major, int *minor,"        << endl;
  m_cpp_out << "    int *corr, int *build, int *moption ){"     << endl;
  m_cpp_out << "  if ( major )"                                 << endl;
  m_cpp_out << "    *major=" << MAJOR_VERSION_NO_ISPCV << ";"   << endl;
  m_cpp_out << "    *minor=" << MINOR_VERSION_NO_ISPCV << ";"   << endl;
  m_cpp_out << "    *corr =" << CORRECTION_LEVEL_ISPCV << ";"   << endl;
  m_cpp_out << "    *build=" << BUILD_PREFIX_ISPCV << ";"       << endl;
  m_cpp_out << "    *build=" << BUILD_PREFIX_ISPCV << ";"       << endl;
  m_cpp_out << "    *moption=" << ispc_getMultiOffsets() << ";" << endl;
  m_cpp_out << "    return;"                                    << endl;
  m_cpp_out << "};"                                             << endl;
  m_cpp_out                                                     << endl;
  m_cpp_out << "const t_FuncBufDesc CO_FunctionDesc[] ="        << endl;
  m_cpp_out << "{"                                              << endl;

  m_h_out                                                              << endl;
  m_h_out   << "#include \"lcbasetypes.h\""                            << endl;
  m_h_out                                                              << endl;
  m_h_out                                                              << endl;
  if (ispc_getMultiOffsets()) {
    m_h_out << "typedef void ( __stdcall *CO_INTROSPECTFUNCTION_MO) (" << endl;
  }
  else {
    m_h_out << "typedef void ( __stdcall *CO_INTROSPECTFUNCTION) ("    << endl;
  }
  m_h_out   << "  unsigned short       ParmIdx,"                       << endl;
  m_h_out   << "  char                 **lplpMode,"                    << endl;
  m_h_out   << "  char                 **lplpIOtype,"                  << endl;
  m_h_out   << "  short int            *lpDatatype,"                   << endl;
  m_h_out   << "  short int            *lpSubdatatype,"                << endl;
  m_h_out   << "  short int            *lpLength,"                     << endl;
  m_h_out   << "  short int            *lpDim,"                        << endl;
  if (ispc_getMultiOffsets()) {
    m_h_out << "  int                  *lpOffsetServer,"               << endl;
    m_h_out << "  int                  *lpOffsetASCIIClient,"          << endl;
    m_h_out << "  int                  *lpOffsetUniClient,"            << endl;
  }
  else {
    m_h_out << "  long                 *lpOffset,"                     << endl;
  }
  m_h_out   << "  unsigned char        *lpFrac,"                       << endl;
  m_h_out   << "  LPGUID               *lplpGUID,"                     << endl;
  m_h_out   << "  char                 **lplpParmName);"               << endl;
  m_h_out   << ""                                                      << endl;
  m_h_out   << "typedef struct"                                        << endl;
  m_h_out   << "{"                                                     << endl;
  m_h_out   << "  unsigned short           FuncIdx;"                   << endl;
  m_h_out   << "  char                    *lpMethod;"                  << endl;
  m_h_out   << "  unsigned short           ParamCount;"                << endl;
  if (ispc_getMultiOffsets()) {
    m_h_out << "  CO_INTROSPECTFUNCTION_MO lpFunction;"                << endl;
  }
  else {
    m_h_out << "  CO_INTROSPECTFUNCTION lpFunction;"                   << endl;
  }
  m_h_out   << "} t_FuncBufDesc;"                                      << endl;
  m_h_out   << ""                                                      << endl;
  m_h_out   << "typedef struct"                                        << endl;
  m_h_out   << "{"                                                     << endl;
  m_h_out   << "  char                 *Mode;"                         << endl;
  m_h_out   << "  char                 *IOtype;"                       << endl;
  m_h_out   << "  short int            Datatype;"                      << endl;
  m_h_out   << "  short int            Subdatatype;"                   << endl;
  m_h_out   << "  short int            Length;"                        << endl;
  m_h_out   << "  short int            Dim;"                           << endl;
  if (ispc_getMultiOffsets()) {
    m_h_out << "  int                  OffsetServer;"                  << endl;
    m_h_out << "  int                  OffsetASCIIClient;"             << endl;
    m_h_out << "  int                  OffsetUniClient;"               << endl;
  }
  else {
    m_h_out << "  long int             Offset;"                        << endl;
  }
  m_h_out   << "  unsigned char        Frac;"                          << endl;
  m_h_out   << "  GUID                 guid;"                          << endl;
  m_h_out   << "  char                 *lpParmName;"                   << endl;
  m_h_out   << "} t_ParmsDesc;"                                        << endl;

  return 1;

}

/*----------------------------------------------------------------------------*/

void IspcCGenInspecCode::startElement( int & error, DboType_Enum_ispc & dboType,
                                       const char * name, const char ** atts ) {

  IspcCTypedef * pType;
  XmlElementType_Enum_ispc elemtype = ispc_getElementType(name);
  unsigned int i;
  char buf[64];
  const char * name_idx = NULL;
  const char * type_idx = NULL;
  short basis_err = e_ok;

  if (m_is_livecachetypes) { /* PTS 1141233, PG */
    return;
  }

  switch (elemtype) {

    case elemDefine_ispc : { /* PTS 1141233, PG */
      if (attName_ispc == ispc_getAttributeType(atts[0])) {
        if (!strcmp( atts[1], "OID_8BYTE_ALIGNED" )) {
          m_oid_8byte_aligned = true;
        }
      }
      break;
    }

    case elemImport_ispc : {
      if ( !strcmp( atts[1], "livecachetypes.xml" ) || !strcmp( atts[1], "livecachetypes.idl" ) ) {
        m_is_livecachetypes_import = 1;
      }
      break;
    }

    case elemConst_ispc : {
      m_act_pType = new IspcCTypedef;
      for ( i = 0; atts[i]; i = i+2) {
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            name_idx = atts[i+1];
            m_act_pType->setName( elemtype, atts[i+1] );
            break;
          }
          case attType_ispc : {
            short server_size,  asc_client_size,  uni_client_size;
            short server_align, asc_client_align, uni_client_align;
            getTypeCharacteristics( atts[i+1], "definition of CONST", name_idx, server_size, server_align,
                                    asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            m_act_pType->setType( *this, atts[i+1], server_size, server_align,
                                  asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            break;
          }
          case attString_ispc :
          case attNumber_ispc :
          case attChar_ispc :
          case attExpression_ispc : {
            m_act_pType->setValue( attrtype, atts[i+1] ); 
            break;
          }
          case attPack_ispc :
          case attUuid_ispc :
          case attDimension_ispc :
          case attDirection_ispc :
          case attUnknown_ispc :
          default : {
            break;
          }
        }
      }
      break;
    }

    case elemTypedef_ispc : {
      pType = new IspcCTypedef;
      for ( i = 0; atts[i]; i = i+2 ) {
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            name_idx = atts[i+1];
            pType->setName( elemtype, atts[i+1] );
            break;
          }
          case attUuid_ispc : {
            pType->setUuid(atts[i+1]);
            break;
          }
          case attType_ispc : {
            /* NULL == name_idx shouldn't be the case:
               - NAME is required attribute
               - attribute order is checked in IspcCGenHFile::startElement */
            if (NULL != name_idx) {
              short server_size,  asc_client_size,  uni_client_size;
              short server_align, asc_client_align, uni_client_align;
              getTypeCharacteristics( atts[i+1], "TYPEDEF", name_idx, server_size, server_align,
                                      asc_client_size, asc_client_align, uni_client_size, uni_client_align );
              pType->setType( *this, atts[i+1], server_size, server_align,
                              asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            }
            break;
          }
          case attDimension_ispc : {
            pType->setDimension(getTypeDimension(atts[i+1]));
            break;
          }
          case attGenTabHandle_ispc : { /* PTS 1134734 */
            if (m_features_76) {
              /* NULL == name_idx shouldn't be the case:
                 - NAME is required attribute
                 - attribute order is checked in IspcCGenHFile::startElement */
              if (NULL != name_idx) {
                m_gen_tab_handle = new char[strlen(name_idx)+1];
                strcpy( m_gen_tab_handle, name_idx );
              }
            }
            break;
          }
          case attString_ispc :
          case attNumber_ispc :
          case attChar_ispc :
          case attPack_ispc :
          case attDirection_ispc :
          case attUnknown_ispc :
          default : {
            break;
          }
        }
      }
      basis_err = m_index.Insert(pType);
      if (basis_err) {
        Ispc_ErrorMsg( C_DUP_DEFINITION_ISPC, pType->getName() );
        if (!error) {
          error = - C_DUP_DEFINITION_ISPC;
        }
      }
      break;
    }

    case elemStruct_ispc : {
      char * RenOid = NULL;
      pType = new IspcCTypedef;
      g_struct_level.push_front(pType);
      for ( i = 0; atts[i]; i = i+2 ) {
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            if (!strncmp( atts[i+1], "OmsTypeOid", 10 )) {
              if (!m_is_livecachetypes_import) {
                Ispc_ErrorMsg( C_BAD_OMSTYPEOID_ISPC, atts[i+1] );
                if (!error) {
                  error = - C_BAD_OMSTYPEOID_ISPC;
                }
                return;
              }
              else {
                ++m_oid_cnt;
              }
            }
            if ( (!strcmp( atts[i+1], "OmsTypeOid" )) &&
                 ( ( (m_oid_cnt == 3) && (!m_oid_8byte_aligned) ) ||
                   ( (m_oid_cnt == 4) && ( m_oid_8byte_aligned) ) ) ) {
              /* PTS 1141233, PG */
              //  8 == strlen("Renamed_")
              // 10 == strlen("OmsTypeOid")
              //  5 == strlen("_Ispc")
              RenOid = new char[7+10+1];
              strcpy( RenOid, "Renamed_" );
              strcpy( &RenOid[8], "OmsTypeOid" );
              strcpy( &RenOid[18], "_Ispc" );
              RenOid[23] = 0;
              name_idx = RenOid;
            }
            else {
              name_idx = atts[i+1];
            }
            pType->setName( elemtype, name_idx );
            break;
          }
          case attUuid_ispc : {
            pType->setUuid(atts[i+1]);
            break;
          }
          case attGenTabHandle_ispc : {
            if (m_features_76) {
              /* NULL == name_idx shouldn't be the case:
                 - NAME is required attribute
                 - attribute order is checked in IspcCGenHFile::startElement */
              if (NULL != name_idx) {
                m_gen_tab_handle = new char[strlen(name_idx)+1];
                strcpy( m_gen_tab_handle, name_idx );
              }
            }
            break;
          }
          case attType_ispc :
          case attDimension_ispc :
          case attString_ispc :
          case attNumber_ispc :
          case attChar_ispc :
          case attPack_ispc :
          case attDirection_ispc :
          case attUnknown_ispc :
          default : {
            break;
          }
        }
      }
      if (RenOid) {
        delete [] RenOid;
        RenOid = NULL;
      }
      basis_err = m_index.Insert(pType);
      if (basis_err) {
        Ispc_ErrorMsg( C_DUP_DEFINITION_ISPC, pType->getName() );
        if (!error) {
          error = - C_DUP_DEFINITION_ISPC;
        }
      }
      break;
    }

    case elemMember_ispc : {
      pType = new IspcCTypedef;
      for ( i = 0; atts[i]; i = i+2 ) {
        short dim;
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            name_idx = atts[i+1];
            pType->setName( elemtype, atts[i+1] );
            break;
          }
          case attUuid_ispc : {
            pType->setUuid( atts[i+1] );
            break;
          }
          case attType_ispc : {
            type_idx = atts[i+1];
            short server_size,  asc_client_size,  uni_client_size;
            short server_align, asc_client_align, uni_client_align;
            getTypeCharacteristics( atts[i+1], "STRUCT member", name_idx, server_size, server_align,
                                    asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            pType->setType( *this, atts[i+1], server_size, server_align,
                            asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            break;
          }
          case attDimension_ispc : {
            dim = getTypeDimension( atts[i+1] );
            if (dim < 0) {
              Ispc_ErrorMsg( C_DIMENSION_TYPE_UNDEF_ISPC, atts[i+1], name_idx, type_idx );
              if (!error) {
                error = - C_PTR_OF_PTR_UNSUPPORTED_ISPC;
              }
            }
            else {
              pType->setDimension( dim );
            }
            break;
          }
          case attString_ispc :
          case attNumber_ispc :
          case attChar_ispc :
          case attPack_ispc :
          case attDirection_ispc :
          case attUnknown_ispc :
          default : {
            break;
          }
        }
      }
      if (!g_struct_level.empty()) {
        (g_struct_level.front())->appendMember(pType);
      }
      break;
    }

    case elemUnion_ispc : { /* PTS 1141233, PG */
      pType = new IspcCTypedef;
      g_struct_level.push_front(pType);
      if (!strncmp( atts[1], "OmsTypeOid", 10 ) ) {
        if (!m_is_livecachetypes_import) {
          Ispc_ErrorMsg( C_BAD_OMSTYPEOID_ISPC, atts[1] );
          if (!error) {
            error = - C_BAD_OMSTYPEOID_ISPC;
          }
          return;
        }
        else {
          ++m_oid_cnt;
        }
      }
      name_idx = atts[1];
      pType->setName( elemtype, name_idx );
      basis_err = m_index.Insert(pType);
      if (basis_err) {
        Ispc_ErrorMsg( C_DUP_DEFINITION_ISPC, pType->getName() );
        if (!error) {
          error = - C_DUP_DEFINITION_ISPC;
        }
      }
      break;
    }

    case elemAlignmentmember_ispc : { /* PTS 1141233, PG */
      pType = new IspcCTypedef;
      for ( i = 0; atts[i]; i = i+2 ) {
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            name_idx = atts[i+1];
            pType->setName( elemtype, atts[i+1] );
            break;
          }
          case attType_ispc : {
            type_idx = atts[i+1];
            short server_size,  asc_client_size,  uni_client_size;
            short server_align, asc_client_align, uni_client_align;
            getTypeCharacteristics( atts[i+1], "STRUCT member", name_idx, server_size, server_align,
                                    asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            pType->setType( *this, atts[i+1], server_size, server_align,
                            asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            break;
          }
          default : {
            break;
          }
        }
      }
      m_alignment_type = pType;
      break;
    }

    case elemIntrinsicmember_ispc : { /* PTS 1141233, PG */
      pType = new IspcCTypedef;
      for ( i = 0; atts[i]; i = i+2 ) {
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            name_idx = atts[i+1];
            pType->setName( elemtype, atts[i+1] );
            break;
          }
          case attType_ispc : {
            type_idx = atts[i+1];
            short server_size,  asc_client_size,  uni_client_size;
            short server_align, asc_client_align, uni_client_align;
            getTypeCharacteristics( atts[i+1], "STRUCT member", name_idx, server_size, server_align,
                                    asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            pType->setType( *this, atts[i+1], server_size, server_align,
                            asc_client_size, asc_client_align, uni_client_size, uni_client_align );
            break;
          }
          default : {
            break;
          }
        }
      }
      if (!g_struct_level.empty()) {
        (g_struct_level.front())->appendMember(pType);
      }
      break;
    }

    case elemInterface_ispc : {
      g_interface = new IspcCTypedef;
      for ( i = 0; atts[i]; i = i+2 ) {
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            g_interface->setName( elemtype, atts[i+1] );
            break;
          }
          case attUuid_ispc : {
            g_interface->setUuid( atts[i+1] );
            break;
          }
          case attType_ispc :
          case attDimension_ispc :
          case attString_ispc :
          case attNumber_ispc :
          case attChar_ispc :
          case attPack_ispc :
          case attDirection_ispc :
          case attUnknown_ispc :
          default : {
            break;
          }
        }
      }
      m_h_out                                       << endl;
      m_h_out << "static const   IID   IID_Intro =" << endl;
      printGuidV1( m_h_out, *g_interface );
      break;
    }

    case elemCoClass_ispc : {
      g_coclass = new IspcCTypedef;
      for ( i = 0; atts[i]; i = i+2 ) {
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            g_coclass->setName( elemtype, atts[i+1] );
            break;
          }
          case attUuid_ispc : {
            g_coclass->setUuid(atts[i+1]);
            break;
          }
          case attType_ispc :
          case attDimension_ispc :
          case attString_ispc :
          case attNumber_ispc :
          case attChar_ispc :
          case attPack_ispc :
          case attDirection_ispc :
          case attUnknown_ispc :
          default : {
            break;
          }
        }
      }
      // header file
      m_h_out                                           << endl;
      m_h_out << "static const   CLSID CLSID_Intro ="   << endl;
      printGuidV1( m_h_out, *g_coclass );
      m_h_out                                           << endl;
      m_h_out << "#define PROCEDURE_NUM " << g_func_no  << endl;
      m_h_out << "#define VERSION_NUMBER 1"             << endl;
      // source file
      m_cpp_out                                                                                  << endl;
      m_cpp_out   << "};"                                                                        << endl;
      m_cpp_out                                                                                  << endl;
      m_cpp_out   << "extern \"C\" void __stdcall"                                               << endl;
      m_cpp_out   << "  Co_IntrospectObject (LPCLSID        *lpClsId,"                           << endl;
      m_cpp_out   << "                       LPIID          *lpIId,"                             << endl;
      m_cpp_out   << "                       unsigned short *lpNum,"                             << endl;
      m_cpp_out   << "                       long           *lpVersion ) {"                      << endl;
      m_cpp_out   << "  *lpClsId        = (LPCLSID)&CLSID_Intro;"                                << endl;
      m_cpp_out   << "  *lpIId          = (LPIID)&IID_Intro;"                                    << endl;
      m_cpp_out   << "  *lpNum          = PROCEDURE_NUM;"                                        << endl;
      m_cpp_out   << "  *lpVersion      = VERSION_NUMBER;"                                       << endl;
      m_cpp_out   << "};"                                                                        << endl;
      m_cpp_out                                                                                  << endl;
      m_cpp_out   << "enum Co_AccessType {"                                                      << endl;
      m_cpp_out   << "  get_by_name,"                                                            << endl;
      m_cpp_out   << "  get_by_index"                                                            << endl;
      m_cpp_out   << "};"                                                                        << endl;
      m_cpp_out                                                                                  << endl;
      m_cpp_out   << "extern \"C\" void __stdcall"                                               << endl;
      if (ispc_getMultiOffsets()) {
        m_cpp_out << "  Co_IntrospectMethod_MO (Co_AccessType          accessType,"              << endl;
      }
      else {
        m_cpp_out << "  Co_IntrospectMethod (Co_AccessType          accessType,"                 << endl;
      }
      m_cpp_out   << "                          char                 **lplpMethod,"              << endl;
      m_cpp_out   << "                          unsigned short         Num,"                     << endl;
      m_cpp_out   << "                          unsigned short        *lpIdx,"                   << endl;
      m_cpp_out   << "                          unsigned short        *lpParamCount,"            << endl;
      if (ispc_getMultiOffsets()) {
        m_cpp_out << "                          CO_INTROSPECTFUNCTION_MO *lplpIntrospecFunc ) {" << endl;
      }
      else {
        m_cpp_out << "                          CO_INTROSPECTFUNCTION *lplpIntrospecFunc ) {"    << endl;
      }
      m_cpp_out << "  *lplpIntrospecFunc = NULL;"                                                << endl;
      m_cpp_out << "  if (accessType == get_by_name) {"                                          << endl;
      m_cpp_out << "    if ( **lplpMethod == '\\0' ) {"                                          << endl;
      m_cpp_out << "      *lpIdx             = CO_FunctionDesc[0].FuncIdx;"                      << endl;
      m_cpp_out << "      *lpParamCount      = CO_FunctionDesc[0].ParamCount;"                   << endl;
      m_cpp_out << "      *lplpIntrospecFunc = CO_FunctionDesc[0].lpFunction;"                   << endl;
      m_cpp_out << "      *lplpMethod        = CO_FunctionDesc[0].lpMethod;"                     << endl;
      m_cpp_out << "    }"                                                                       << endl;
      m_cpp_out << "    else"                                                                    << endl;
      m_cpp_out << "      for ( int i=0; i < PROCEDURE_NUM; i++ )"                               << endl;
      m_cpp_out << "        if ( 0 == strcmp(*lplpMethod, CO_FunctionDesc[i].lpMethod )) {"      << endl;
      m_cpp_out << "          *lpIdx             = CO_FunctionDesc[i].FuncIdx;"                  << endl;
      m_cpp_out << "          *lpParamCount      = CO_FunctionDesc[i].ParamCount;"               << endl;
      m_cpp_out << "          *lplpIntrospecFunc = CO_FunctionDesc[i].lpFunction;"               << endl;
      m_cpp_out << "        }"                                                                   << endl;
      m_cpp_out << "  }"                                                                         << endl;
      m_cpp_out << "  else {"                                                                    << endl;
      m_cpp_out << "    if (( Num <= PROCEDURE_NUM ) && ( Num >= 0 )) {"                         << endl;
      m_cpp_out << "      *lpIdx             = CO_FunctionDesc[Num].FuncIdx;"                    << endl;
      m_cpp_out << "      *lpParamCount      = CO_FunctionDesc[Num].ParamCount;"                 << endl;
      m_cpp_out << "      *lplpIntrospecFunc = CO_FunctionDesc[Num].lpFunction;"                 << endl;
      m_cpp_out << "      *lplpMethod        = CO_FunctionDesc[Num].lpMethod;"                   << endl;
      m_cpp_out << "    }"                                                                       << endl;
      m_cpp_out << "  }"                                                                         << endl;
      m_cpp_out << "};"                                                                          << endl;
      break;
    }

    case elemIdl_ispc : {
      g_func_no++;
      g_param_no = 0;
      strcpy( g_act_method, atts[1] );
      // header file
      m_h_out                                                                         << endl;
      m_h_out   << "static void __stdcall co_IntrospectFunction" << g_func_no << " (" << endl;
      m_h_out   << "  unsigned short       ParmIdx,"                                  << endl;
      m_h_out   << "  char                 **lplpMode,"                               << endl;
      m_h_out   << "  char                 **lplpIOtype,"                             << endl;
      m_h_out   << "  short int            *lpDatatype,"                              << endl;
      m_h_out   << "  short int            *lpSubdatatype,"                           << endl;
      m_h_out   << "  short int            *lpLength,"                                << endl;
      m_h_out   << "  short int            *lpDim,"                                   << endl;
      if (ispc_getMultiOffsets()) {
        m_h_out << "  int                  *lpOffsetServer,"                          << endl;
        m_h_out << "  int                  *lpOffsetASCIIClient,"                     << endl;
        m_h_out << "  int                  *lpOffsetUniClient,"                       << endl;
      }
      else {
        m_h_out << "  long                 *lpOffset,"                                << endl;
      }
      m_h_out   << "  unsigned char        *lpFrac,"                                  << endl;
      m_h_out   << "  LPGUID               *lplpGUID,"                                << endl;
      m_h_out   << "  char                 **lplpParmName)"                           << endl;
      m_h_out   << "{"                                                                << endl;
      // source file
      if (g_func_no > 1) {
        m_cpp_out << "," << endl << "  ";
      }
      else {
        m_cpp_out << endl << "  ";
      }
      m_cpp_out << g_func_no + 2 << ", (char*)\"" << atts[1] << "\", ";
      break;
    }

    case elemParam_ispc : {

      const char * name_idx      = NULL;
      const char * direction_idx = NULL;
      const char * uuid_idx      = NULL;
      const char * type_idx      = NULL;
      const char * dimension_idx = NULL;
      short is_ptr;
      char  generatedName[11];

      g_param_no++;
      sprintf( generatedName, "_idl_%5.5d", g_param_no );

      if (1 == g_param_no) {
        m_h_out << "  static const t_ParmsDesc CO_ParmDesc[] =" << endl;
        m_h_out << "  {"                                        << endl;
      }

      for ( i = 0; atts[i]; i = i+2 ) {
        XmlAttributeType_Enum_ispc attrtype = ispc_getAttributeType(atts[i]);
        switch (attrtype) {
          case attName_ispc : {
            if (0 == strlen(atts[i+1])) {
              name_idx = &generatedName[0];
            }
            else {
              name_idx = atts[i+1];
            }
            break;
          }
          case attUuid_ispc : {
            uuid_idx = atts[i+1];
            break;
          }
          case attType_ispc : {
            type_idx = atts[i+1];
            break;
          }
          case attDimension_ispc : {
            dimension_idx = atts[i+1];
            break;
          }
          case attString_ispc :
          case attNumber_ispc :
          case attChar_ispc :
          case attPack_ispc :
          case attDirection_ispc : {
            direction_idx = atts[i+1];
            break;
          }
          case attUnknown_ispc :
          default : {
            break;
          }
        }
      }

      if (strrchr( type_idx, '*' )) {
        strncpy( buf, type_idx, strlen(type_idx)-1 );
        buf[strlen(type_idx)-1] = '\0';
        is_ptr = 1;
        if (strrchr( buf, '*' )) {
          Ispc_ErrorMsg( C_PTR_OF_PTR_UNSUPPORTED_ISPC, g_act_method, name_idx, type_idx );
          if (!error) {
            error = - C_PTR_OF_PTR_UNSUPPORTED_ISPC;
          }
          break;
        }
      }
      else {
        strcpy( buf, type_idx );
        is_ptr = 0;
      }

      if ( (dboType != dbo_method_ispc) || checkFirstParam( name_idx, uuid_idx, buf, direction_idx ) ) {

        IspcCTypedef findNode;
        findNode.setName( elemUnknown_ispc, buf );
        IspcCTypedef * const * foundptr = (m_index.FindNode(&findNode))->GetKey();
        while ( foundptr && 
                (elemTypedef_ispc == (*foundptr)->getDefType()) &&
                (csp1_vt_userdefined == (*foundptr)->getVtType()) &&
                (*foundptr)->getType() ) {
          findNode.setName( elemUnknown_ispc, (*foundptr)->getType() );
          foundptr = (m_index.FindNode(&findNode))->GetKey();
        }

        if (1 == g_param_no) {
          m_h_out << "    (char*)\"MAN\"";
        }
        else {
          m_h_out << "," << endl << "    (char*)\"MAN\"";
        }

        if ( direction_idx && strlen(direction_idx) ) {
          char cBuf[6];
          strcpy( cBuf, direction_idx );
          for ( i = 0; i < strlen(cBuf); i++ ) {
            cBuf[i] = toupper(cBuf[i]);
            /* if ( ' ' == cBuf[i] ) 
               cBuf[i] = '\0';              PTS 1109826 */
          }
          /*+++++ PTS 1109826 +++++*/
          bool in  = (NULL != strstr( &cBuf[0], "IN"  ));
          bool out = (NULL != strstr( &cBuf[0], "OUT" ));
          if ( in && out ) {
            m_h_out << ", (char*)\"I/O\"";
          }
          else if (in) {
            m_h_out << ", (char*)\"IN\"";
          }
          else if (out) {
            /* PTS 1140184, PG */
            if ( is_ptr && foundptr && ((*foundptr)->isAbapTableHandle(*this)) ) {
              m_h_out << ", (char*)\"IN\"";
            }
            else {
              m_h_out << ", (char*)\"OUT\"";
            }
          }
          else {
            m_h_out << ", (char*)\"\"";
          }
          /*----- PTS 1109826 -----*/
          /* m_h_out << ", (char*)\"" << cBuf << "\""; PTS 1109826 */
        }
        else {
          Ispc_WarnMsg( C_NO_DIR_SPECIFIC_ISPC, g_act_method, name_idx );
          m_h_out << ", (char*)\"I/O\"";
        }

        short l_typ;
        short l_subtyp;
        short l_server_size;
        short l_asc_client_size;
        short l_uni_client_size;
        short l_dim = getTypeDimension(dimension_idx);
        
        if (foundptr) {

          /* parameter is a user defined type */

          l_typ         = csp1_vt_ptr;
          l_subtyp      = csp1_vt_userdefined;
          l_server_size = (*foundptr)->getServerSize();

          // if ((*foundptr)->getDimension() > 0) {
          //   if (l_dim > 0) {
          //     Ispc_ErrorMsg( C_UNSUPPORTED_VT_CARRAY1_ISPC, g_act_method, type_idx);
          //   }
          // }

          short l_elem_num  = (*foundptr)->getMemberNum(*this);

          const GUID * const guid = (*foundptr)->getUuid();

          if ( (elemStruct_ispc == (*foundptr)->getDefType()) ||
               (elemUnion_ispc  == (*foundptr)->getDefType()) ) { /* PTS 1141233, PG */

            /* parameter is a struct */

            if ( is_ptr || ((*foundptr)->getPtr()) ) {
              (*foundptr)->checkIfAbapOrStreamHandle( *this, &l_typ, &l_server_size, &l_elem_num, &foundptr );
              if (!foundptr) { /* PTS 1110445 */
                break;         /* PTS 1110445 */
              }                /* PTS 1110445 */
              short l_align = (*foundptr)->getServerAlign();
              if (0 != l_server_size % l_align) {
                l_server_size += (l_align - l_server_size % l_align); /* xxx */
              }
              m_h_out << ", " << l_typ
                      << ", " << l_subtyp
                      << ", " << l_server_size
                      << ", " << l_elem_num;
              if (ispc_getMultiOffsets()) {
                m_h_out << ", 0, 0, 0, 0,";
              }
              else {
                m_h_out << ", 0, 0,";
              }
              char cData[9];
              sprintf( cData, "%x", guid->Data1 );
              m_h_out << " { 0x" << cData;
              sprintf( cData, "%x", guid->Data2 );
              m_h_out << ", 0x" << cData;
              sprintf( cData, "%x", guid->Data3 );
              m_h_out << ", 0x" << cData << ", {";
              //m_h_out.setf(ios::hex);
              //m_h_out << " { 0x" << guid->Data1 << ", 0x" << guid->Data2 << ", 0x" 
              //  << guid->Data3 << ", {";
              for ( i = 0; i < 7; i++ ) {
                sprintf( cData, "%x", (unsigned short)guid->Data4[i] );
                m_h_out << " 0x" << cData << ",";
                //m_h_out << " 0x" << (unsigned short)guid->Data4[i] << ",";
              }
              sprintf( cData, "%x", (unsigned short)guid->Data4[7] );
              m_h_out << " 0x" << cData << " }}, (char*)\"" << name_idx << "\"";
              //m_h_out << " 0x" << (unsigned short)guid->Data4[7] << " }}, (char*)\"" << name_idx <<"\"";
              //m_h_out.unsetf(ios::hex);
              short posServer    = 0;
              short posAscClient = 0;
              short posUniClient = 0;
              (*foundptr)->printSubParameter( *this, m_h_out, posServer, posAscClient, posUniClient );
            }
            else {
              /* struct must be passed via pointer */
              Ispc_ErrorMsg( C_PASS_STRUCT_BY_PTR_ISPC, g_act_method, name_idx );
              if (!error) {
                error = - C_PASS_STRUCT_BY_PTR_ISPC;
              }
              break;
            }

          }
          else { 

            /* parameter is no struct */

            IspcCTypedef * const * searchptr = foundptr;
            findNode.setName( elemUnknown_ispc, (*foundptr)->getType() );
            while ( searchptr && ((*searchptr)->getDefType() == elemTypedef_ispc) &&
                    (0 == (*searchptr)->getMemberNum()) ) {
              if ((*searchptr)->getPtr()) {
                if (is_ptr) {
                  Ispc_ErrorMsg( C_PTR_OF_PTR_UNSUPPORTED_ISPC, g_act_method, name_idx, (*foundptr)->getType() );
                  if (!error) {
                    error = - C_PTR_OF_PTR_UNSUPPORTED_ISPC;
                  }
                  break;
                }
                else {
                  is_ptr = 1;
                }
              }
              if ((*searchptr)->getDimension() > 0) {
                if (l_dim > 0) {
                  Ispc_ErrorMsg( C_UNSUPPORTED_VT_CARRAY2_ISPC, g_act_method, name_idx );
                  if (!error) {
                    error = - C_UNSUPPORTED_VT_CARRAY2_ISPC;
                  }
                  break;
                }
                else {
                  l_dim = (*searchptr)->getDimension();
                }
              }
              findNode.setName( elemUnknown_ispc, (*searchptr)->getType() );
              searchptr = this->findNode(findNode);
            }
            if (error) {
              break;
            }

            if (is_ptr) {
              IspcCTypedef findNode;
              l_subtyp = (*foundptr)->getVtType();
              l_server_size = (*foundptr)->getServerSize();
              if ( foundptr && (1 == (*foundptr)->getMemberNum()) && (l_typ == csp1_vt_userdefined) ) {
                IspcCTypedef * base = *((*foundptr)->getMemberList().begin())();
                if (l_dim > 0) {
                  l_typ = csp1_vt_carray;
                  if ( (l_server_size != 1) && (csp1_vt_lc_wyde != base->getVtType()) ) {
                    Ispc_ErrorMsg( C_UNSUPPORTED_VT_CARRAY1_ISPC, g_act_method, (*foundptr)->getType() );
                    if (!error) {
                      error = - C_UNSUPPORTED_VT_CARRAY1_ISPC;
                    }
                    /* break; PTS 1109283 */
                    l_typ = l_subtyp = l_server_size = l_dim = -1;
                    break; /* PTS 1109283 */
                  }
                  else {
                    l_server_size = l_server_size * l_dim;
                  }
                }
                else {
                  l_server_size = (*foundptr)->getServerSize();
                  l_dim = 0;
                  l_subtyp = base->getVtType();
                }
              }
              else {
                if (l_dim) {
                  l_typ = csp1_vt_carray;
                  l_dim = (*foundptr)->getDimension();
                  if ( (l_server_size != 1) && (csp1_vt_lc_wyde != l_subtyp) ) {
                    Ispc_ErrorMsg( C_UNSUPPORTED_VT_CARRAY1_ISPC, g_act_method, (*foundptr)->getType() );
                    if (!error) {
                      error = - C_UNSUPPORTED_VT_CARRAY1_ISPC;
                    }
                    l_typ = l_subtyp = l_server_size = l_dim = -1;
                    break;
                  }
                  else {
                    l_server_size = l_server_size * l_dim;
                  }
                }
              }
              if ( (l_dim < 1) && direction_idx && (!strcmp( "in", direction_idx )) && searchptr &&
                   (elemStruct_ispc != (*searchptr)->getDefType()) &&
                   (elemUnion_ispc  != (*searchptr)->getDefType()) ) { /* PTS 1141233, PG */
                l_dim = l_server_size = l_subtyp = -1;
                Ispc_ErrorMsg( C_IN_SCALAR_BYPTR_ISPC, g_act_method, name_idx, buf );
                if (!error) {
                  error = - C_IN_SCALAR_BYPTR_ISPC;
                }
                break;
              }
              /* PTS 1128797 FF 2004-MAR-26
              if (l_subtyp == csp1_vt_i8) {
                Ispc_ErrorMsg( C_UNSUPPORTED_PARAM_TYPE_ISPC, g_act_method, name_idx, type_idx );
                if (!error) {
                  error = - C_UNSUPPORTED_PARAM_TYPE_ISPC;
                }
                break;
              }
              END PTS 1128797 */
              /* following patch has been done, because liveCache uses in this
                 cases only l_dim to determine the length of the parameter     */
              if ( ( (l_typ == csp1_vt_ptr) || (l_typ == csp1_vt_userdefined) ) &&
                   (0 == l_dim) && (1 == l_server_size) ) {
                l_dim = l_server_size;
              }
              m_h_out << ", " << l_typ
                      << ", " << l_subtyp
                      << ", " << l_server_size
                      << ", " << l_dim;
              if (ispc_getMultiOffsets()) {
                m_h_out << ", 0, 0, 0, 0,";
              }
              else {
                m_h_out << ", 0, 0,";
              }
            }
            else {
              /* PTS 1128797 FF 2004-MAR-26
              if ((*foundptr)->getVtType() == csp1_vt_i8) {
                Ispc_ErrorMsg( C_UNSUPPORTED_PARAM_TYPE_ISPC, g_act_method, name_idx, type_idx );
                if (!error) {
                  error = - C_UNSUPPORTED_PARAM_TYPE_ISPC;
                }
                break;
              }
              END PTS 1128797 */
              if (l_dim > 0) {
                Ispc_ErrorMsg( C_PASS_STRUCT_BY_PTR_ISPC, g_act_method, name_idx );
                if (!error) {
                  error = - C_PASS_STRUCT_BY_PTR_ISPC;
                }
                break;
              }
              else {
                m_h_out << ", " << (*foundptr)->getVtType()
                        << ", " << 0
                        << ", " << l_server_size
                        << ", " << 0;
                if (ispc_getMultiOffsets()) {
                  m_h_out << ", 0, 0, 0, 0,";
                }
                else {
                  m_h_out << ", 0, 0,";
                }
              }
            }

            char cData[9];
            sprintf( cData, "%x", guid->Data1 );
            m_h_out << " { 0x" << cData;
            sprintf( cData, "%x", guid->Data2 );
            m_h_out << ", 0x" << cData;
            sprintf( cData, "%x", guid->Data3 );
            m_h_out << ", 0x" << cData << ", {";
            // m_h_out.setf(ios::hex);
            // m_h_out << " { 0x" << guid->Data1 << ", 0x" << guid->Data2 << ", 0x" 
            //         << guid->Data3 << ", {";
            for ( i = 0; i < 7; i++ ) {
              sprintf( cData, "%x", (unsigned short)guid->Data4[i] );
              m_h_out << " 0x" << cData << ",";
              // m_h_out << " 0x" << (unsigned short)guid->Data4[i] << ",";
            }
            sprintf( cData, "%x", (unsigned short)guid->Data4[i] );
            m_h_out << " 0x" << cData << " }}, (char*)\"" << name_idx << "\"";
            // m_h_out << " 0x" << (unsigned short)guid->Data4[7] << " }}, (char*)\"" << name_idx << "\"";
            // m_h_out.unsetf(ios::hex);

          }

        }
        else {

          /* parameter is not a user defined type */

          if (strchr( type_idx, '*' )) {
            strncpy( buf, type_idx, strlen(type_idx)-1 );
            buf[strlen(type_idx)-1] = '\0';
            l_typ = csp1_vt_ptr;
            l_subtyp = checkBasisType(buf);
            /* PTS 1128797 FF 2004-MAR-26
            if (l_subtyp == csp1_vt_i8) {
              Ispc_ErrorMsg( C_UNSUPPORTED_PARAM_TYPE_ISPC, g_act_method, name_idx, type_idx );
              if (!error) {
                error = - C_UNSUPPORTED_PARAM_TYPE_ISPC;
              }
              break;
            }
            END PTS 1128797 */
          }
          else {
            strcpy( buf, type_idx );
            l_typ = checkBasisType(buf);
            /* PTS 1128797 FF 2004-MAR-26
            if (l_typ == csp1_vt_i8) {
              Ispc_ErrorMsg( C_UNSUPPORTED_PARAM_TYPE_ISPC, g_act_method, name_idx, type_idx );
              if (!error) {
                error = - C_UNSUPPORTED_PARAM_TYPE_ISPC;
              }
              break;
            }
            END PTS 1128797 */
            l_subtyp = 0;
          }

          if ( (0 == strcmp( buf, "hyper" ))             ||
               (0 == strcmp( buf, "unsigned" ))          ||
               (0 == strcmp( buf, "long" ))              ||
               (0 == strcmp( buf, "signed long" ))       ||
               (0 == strcmp( buf, "unsigned long" ))     ||
               (0 == strcmp( buf, "long int" ))          ||
               (0 == strcmp( buf, "signed long int" ))   ||
               (0 == strcmp( buf, "unsigned long int" )) ||
               (0 == strcmp( buf, "bool" ))              ||
               (0 == strcmp( buf, "boolean" ))           ||
               (0 == strcmp( buf, "char" )) ) {
            char l_buf[128];
            strcpy( l_buf, "parameter" );
            strcat( l_buf, ":" );
            strcat( l_buf, name_idx );
            l_server_size = -1;
            Ispc_WarnMsg( C_TYPE_NOT_PORTABLE_ISPC, l_buf, buf );
          }

          short l_tmp1, l_tmp2, l_tmp3;
          if (!getTypeCharacteristics( buf, "parameter", name_idx, l_server_size, l_tmp1,
                                       l_asc_client_size, l_tmp2, l_uni_client_size, l_tmp3 )) {
            break;
          }

          if (is_ptr) {
            if ( direction_idx && (!strcmp( "in", direction_idx )) ) {
              Ispc_ErrorMsg( C_IN_SCALAR_BYPTR_ISPC, g_act_method, name_idx, buf );
              if (!error) {
                error = - C_IN_SCALAR_BYPTR_ISPC;
              }
              break;
            }
          }
          else {
            if (l_dim > 0) {
              Ispc_ErrorMsg( C_PASS_STRUCT_BY_PTR_ISPC, g_act_method, name_idx );
              if (!error) {
                error = - C_PASS_STRUCT_BY_PTR_ISPC;
              }
              break;
            }
          }

          m_h_out << ", " << l_typ
                  << ", " << l_subtyp
                  << ", " << l_server_size
                  << ", " << l_dim;
          if (ispc_getMultiOffsets()) {
            m_h_out << ", 0, 0, 0, 0,";
          }
          else {
            m_h_out << ", 0, 0,";
          }
          m_h_out << " { 0x0, 0x0, 0x0, { 0x0, 0x0, 0x0, 0xa0, 0x0, 0x0, 0x0, 0x0 }}, (char*)\""
                  << name_idx << "\"";

        }

      }

      break;

    }

    case elemExpr_ispc : {
      m_expr_flag = true;
      break;
    }

    case elemSub_ispc :
    case elemAdd_ispc :
    case elemFloat_ispc :
    case elemFixed_ispc :
    case elemMul_ispc :
    case elemDiv_ispc :
    case elemMod_ispc :
    case elemIdent_ispc : {
      if ( m_expr_flag && (NULL != m_act_pType) ) {
        m_expr_token.m_token_type  = elemtype;
        m_expr_token.m_token_value = NULL;
      }
      break;
    }

    case elemLibrary_ispc :
    case elemCpp_ispc :
    case elemPragma_ispc :
    case elemEnum_ispc :
    case elemInclude_ispc :
    case elemUnknown_ispc : {
      break;
    }

    default : {
      break;
    }

  }

}

/*----------------------------------------------------------------------------*/

void IspcCGenInspecCode::endElement( int & error, const char * name ) {

  if (m_is_livecachetypes) { /* PTS 1141233, PG */
    return;
  }

  IspcCTypedef * pType;
  XmlElementType_Enum_ispc elemtype = ispc_getElementType(name);
  tgg00_BasisError basis_err = e_ok;

  switch (elemtype) {

    case elemConst_ispc : {
      basis_err = m_index.Insert(m_act_pType);
      if (basis_err) {
        Ispc_ErrorMsg( C_DUP_DEFINITION_ISPC, m_act_pType->getName() );
        if (!error) {
          error = - C_DUP_DEFINITION_ISPC;
        }
      }
      m_act_pType = NULL;
      break;
    }

    case elemStruct_ispc : {
      pType = g_struct_level.front();
      g_struct_level.pop_front();
      pType->setStructCharacteristics(*this);
      /* PTS 1131325, PG */
      if (m_features_76) {
        if (m_gen_tab_handle) {
          genABAPTabHandle( error, m_gen_tab_handle );
          delete [] m_gen_tab_handle;
          m_gen_tab_handle = NULL;
        }
      }
      break;
    }

    case elemUnion_ispc : { /* PTS 1141233, PG */
      pType = g_struct_level.front();
      g_struct_level.pop_front();
      pType->setStructCharacteristics(*this);
      if (pType->getMemberNum() != 1) {
        Ispc_ErrorMsg( C_BAD_UNION_DECLARATION, pType->getName() );
        if (!error) {
          error = -C_BAD_UNION_DECLARATION;
        }
      }
      short alimem_server_size,  alimem_asc_client_size,  alimem_uni_client_size;
      short alimem_server_align, alimem_asc_client_align, alimem_uni_client_align;
      getTypeCharacteristics( m_alignment_type->getType(), "UNION member", m_alignment_type->getName(),
                              alimem_server_size, alimem_server_align,
                              alimem_asc_client_size, alimem_asc_client_align,
                              alimem_uni_client_size, alimem_uni_client_align );
      short union_server_size,  union_asc_client_size,  union_uni_client_size;
      short union_server_align, union_asc_client_align, union_uni_client_align;
      getTypeCharacteristics( pType->getName(), "UNION", "",
                              union_server_size, union_server_align,
                              union_asc_client_size, union_asc_client_align,
                              union_uni_client_size, union_uni_client_align );
      if ( (alimem_server_size      != union_server_size)      ||
           (alimem_asc_client_size  != union_asc_client_size)  ||
           (alimem_uni_client_size  != union_uni_client_size)  ||
           (alimem_server_align      < union_server_align)     ||
           (alimem_asc_client_align  < union_asc_client_align) ||
           (alimem_uni_client_align  < union_uni_client_align) ) {
        Ispc_ErrorMsg( C_BAD_UNION_DECLARATION, pType->getName() );
        if (!error) {
          error = -C_BAD_UNION_DECLARATION;
        }
      }
      else {
        pType->setServerAlign(alimem_server_align);
      }
      break;
    }

    case elemImport_ispc : {          // note:
      m_is_livecachetypes_import = 0; // within livecachetypes.xml no import takes place
      break;                          // so value can (resp. must) always be set to 0
    }

    case elemLibrary_ispc :
    case elemCpp_ispc :
    case elemPragma_ispc :
    case elemInclude_ispc : {
      break;
    }

    case elemTypedef_ispc : { /* PTS 1134734, PG */
      if (m_features_76) {
        if (m_gen_tab_handle) {
          genABAPTabHandle( error, m_gen_tab_handle );
          delete [] m_gen_tab_handle;
          m_gen_tab_handle = NULL;
        }
      }
      break;
    }

    case elemMember_ispc :
    case elemAlignmentmember_ispc : /* PTS 1141233, PG */
    case elemIntrinsicmember_ispc : /* PTS 1141233, PG */
    case elemEnum_ispc :
    case elemInterface_ispc : {
      break;
    }

    case elemIdl_ispc : {
      if (g_param_no) {
        m_h_out   << "  };"                                                                               << endl;
        m_h_out                                                                                           << endl;
        m_h_out   << "  if (( ParmIdx > (sizeof(CO_ParmDesc) / sizeof(t_ParmsDesc))) || ( ParmIdx < 1 ))" << endl;
        m_h_out   << "  {"                                                                                << endl;
        m_h_out   << "    *lplpMode = NULL;"                                                              << endl;
        m_h_out   << "  }"                                                                                << endl;
        m_h_out   << "  else"                                                                             << endl;
        m_h_out   << "  {"                                                                                << endl;
        m_h_out   << "    *lplpMode       = CO_ParmDesc[ParmIdx - 1].Mode;"                               << endl;
        m_h_out   << "    *lplpIOtype     = CO_ParmDesc[ParmIdx - 1].IOtype;"                             << endl;
        m_h_out   << "    *lpDatatype     = CO_ParmDesc[ParmIdx - 1].Datatype;"                           << endl;
        m_h_out   << "    *lpSubdatatype  = CO_ParmDesc[ParmIdx - 1].Subdatatype;"                        << endl;
        m_h_out   << "    *lpLength       = CO_ParmDesc[ParmIdx - 1].Length;"                             << endl;
        m_h_out   << "    *lpDim          = CO_ParmDesc[ParmIdx - 1].Dim;"                                << endl;
        if (ispc_getMultiOffsets()) {
          m_h_out << "    *lpOffsetServer      = CO_ParmDesc[ParmIdx - 1].OffsetServer;"                  << endl;
          m_h_out << "    *lpOffsetASCIIClient = CO_ParmDesc[ParmIdx - 1].OffsetASCIIClient;"             << endl;
          m_h_out << "    *lpOffsetUniClient   = CO_ParmDesc[ParmIdx - 1].OffsetUniClient;"               << endl;
        }
        else {
          m_h_out << "    *lpOffset       = CO_ParmDesc[ParmIdx - 1].Offset;"                             << endl;
        }
        m_h_out   << "    *lpFrac         = CO_ParmDesc[ParmIdx - 1].Frac;"                               << endl;
        m_h_out   << "    *lplpGUID       = (GUID*)&CO_ParmDesc[ParmIdx - 1].guid;"                       << endl;
        m_h_out   << "    *lplpParmName   = CO_ParmDesc[ParmIdx - 1].lpParmName;"                         << endl;
        m_h_out   << "  }"                                                                                << endl;
        m_h_out   << "};"                                                                                 << endl;
      }
      else {
        m_h_out << "  *lplpMode = NULL;"                                                                  << endl;
        m_h_out << "}"                                                                                    << endl;
      }
      m_cpp_out << g_param_no << ", &co_IntrospectFunction" << g_func_no;
      break;
    }

    case elemExpr_ispc : {
      if ( m_expr_flag && (NULL != m_act_pType) ) {
        int rc = m_act_pType->calExprValue(*this);
        if (rc) {
          if (!error) {
            error = - rc;
          }
        }
      }
      m_expr_flag = false;
      break;
    }

    case elemSub_ispc :
    case elemAdd_ispc :
    case elemFloat_ispc :
    case elemFixed_ispc :
    case elemMul_ispc :
    case elemDiv_ispc :
    case elemMod_ispc :
    case elemIdent_ispc : {
      if ( m_expr_flag && (NULL != m_act_pType) ) {
         IspcCExprElem * p = new IspcCExprElem;
         *p = m_expr_token;
         m_act_pType->appendToken(p);
      }
      break;
    }

    case elemCoClass_ispc :
    case elemUnknown_ispc :
    default : {
      break;
    }

  }

}

/*----------------------------------------------------------------------------*/

short IspcCGenInspecCode::checkBasisType(const char * type) {

  if (!strcmp( type, "hyper" )) {
    return csp1_vt_i8;
  }
  if (!strcmp( type, "signed hyper" )) {
    return csp1_vt_i8;
  }
  if (!strcmp( type, "unsigned hyper" )) {
    return csp1_vt_ui8;
  }
  if (!strcmp( type, "long long" )) {
    return csp1_vt_i8;
  }
  if (!strcmp( type, "signed long long" )) {       /* PTS 1112805 */
    return csp1_vt_i8;
  }
  if (!strcmp( type, "unsigned long long" )) {     /* PTS 1112805 */
    return csp1_vt_ui8;
  }
  if (!strcmp( type, "long long int" )) {          /* PTS 1112805 */
    return csp1_vt_i8;
  }
  if (!strcmp( type, "signed long long int" )) {   /* PTS 1112805 */
    return csp1_vt_i8;
  }
  if (!strcmp( type, "unsigned long long int" )) { /* PTS 1112805 */
    return csp1_vt_ui8;
  }

  if (!strcmp( type, "short" )) {
    return csp1_vt_i2;
  }
  if (!strcmp( type, "signed short" )) {
    return csp1_vt_i2;
  }
  if (!strcmp( type, "unsigned short" )) {
    return csp1_vt_ui2;
  }
  if (!strcmp( type, "short int" )) {
    return csp1_vt_i2;
  }
  if (!strcmp( type, "signed short int" )) {
    return csp1_vt_i2;
  }
  if (!strcmp( type, "unsigned short int" )) {
    return csp1_vt_ui2;
  }

  if (!strcmp( type, "signed" )) {
    return csp1_vt_int;
  }
  if (!strcmp( type, "unsigned" )) {
    return csp1_vt_uint;
  }
  if (!strcmp( type, "int" )) {
    return csp1_vt_int;
  }
  if (!strcmp( type, "signed int" )) {
    return csp1_vt_int;
  }
  if (!strcmp( type, "unsigned int" )) {
    return csp1_vt_uint;
  }
  if (!strcmp( type, "long" )) {
    return csp1_vt_i4;
  }
  if (!strcmp( type, "signed long" )) {
    return csp1_vt_i4;
  }
  if (!strcmp( type, "unsigned long" )) {
    return csp1_vt_ui4;
  }
  if (!strcmp( type, "long int" )) {
    return csp1_vt_int;
  }
  if (!strcmp( type, "signed long int" )) {
    return csp1_vt_int;
  }
  if (!strcmp( type, "unsigned long int" )) {
    return csp1_vt_uint;
  }

  if (!strcmp( type, "float" )) {
    return csp1_vt_r4;
  }
  if (!strcmp( type, "double" )) {
    return csp1_vt_r8;
  }
  
  if (!strcmp( type, "char" )) {
    return csp1_vt_i1;
  }
  if (!strcmp( type, "signed char" )) {
    return csp1_vt_i1;
  }
  if (!strcmp( type, "unsigned char" )) {
    return csp1_vt_ui1;
  }
  if (!strcmp( type, "wchar_t" )) {
    return csp1_vt_i2;
  } 
  if (!strcmp( type, "byte" )) {
    return csp1_vt_ui1;
  }
  if (!strcmp( type, "boolean" )) {
    return csp1_vt_i1;
  }
  if (!strcmp( type, "bool" )) {
    return csp1_vt_i4;
  }

  return csp1_vt_empty;

}

/*----------------------------------------------------------------------------*/

bool IspcCGenInspecCode::checkFirstParam( const char * pname, const char * puuid,
                                          const char * ptype, const char * pdirection ) {
  if ( pname && puuid && ptype && pdirection ) {
    IspcCTypedef findNode;
    findNode.setName( elemUnknown_ispc, ptype );
    IspcCTypedef * const * foundptr = (m_index.FindNode(&findNode))->GetKey();
    /* if first method */
    //    if ( g_udt_guid == g_null_guid ) {
    if (!memcmp( &g_udt_guid, &g_null_guid, sizeof(g_udt_guid) )) {
      if (!( foundptr && (csp1_vt_userdefined == (*foundptr)->getVtType()) )) {
        Ispc_ErrorMsg( C_P1_NO_USERDEFINED_TYPE_ISPC, g_act_method, (*foundptr)->getType() );
        return true; /* error */
      }
      if (!strcmp( "in", pdirection )) {
        Ispc_ErrorMsg( C_P1_IO_TYPE_INPUT_ISPC, g_act_method );
        return true; /* error */
      }
      g_udt_guid = *(*foundptr)->getUuid();
    }
    else {
      if (!strcmp( "out", pdirection )) {
        Ispc_ErrorMsg( C_P1_IO_TYPE_OUTPUT_ISPC, g_act_method );
        return true; /* error */
      }
      //      if ( g_udt_guid != *(*foundptr)->getUuid() ) {
      if (memcmp( &g_udt_guid, (*foundptr)->getUuid(), sizeof(g_udt_guid) )) {
        Ispc_ErrorMsg( C_P1_WRONG_GUID_ISPC, g_act_method );
      }
    }
  }
  else {
    if (!puuid) {
      Ispc_ErrorMsg( C_P1_NO_GUID_ISPC, g_act_method );
    }
    else {
      Ispc_ErrorMsg( C_P1_TYPE_SPEC_INCOMPLETE_ISPC, g_act_method );
    }
    return true; /* error */
  }
  return false; /* no error */
}

/*----------------------------------------------------------------------------*/

short IspcCGenInspecCode::getTypeDimension(const char * d){
  short dim = 0;
  if ( d && strlen(d) ) {
    char buf[64];
    strcpy( buf, d );
    bool found = false;
    IspcCTypedef findNode;
    do {
      findNode.setName( elemUnknown_ispc, buf );
      int l;
      if (Ispc_GetInteger( buf, l )) {
        dim   = (short)l;
        found = true;
      }
      else {
        IspcCTypedef * const * foundptr = (m_index.FindNode(&findNode))->GetKey();
        if ( foundptr && ((*foundptr)->getValue()) ) {
          strcpy( buf, (*foundptr)->getValue() );
        }
        else {
          return (-1);
        }
      }
    } while (!found);
  }
  return dim;
}

/*----------------------------------------------------------------------------*/

int IspcCGenInspecCode::getTypeCharacteristics( const char * type,
                                                const char * usedby,
                                                const char * username,
                                                short & server_size,
                                                short & server_align,
                                                short & asc_client_size,
                                                short & asc_client_align,
                                                short & uni_client_size,
                                                short & uni_client_align ) {

  char t[64];
  strcpy( t, type );

  /* trim trailing '*' char */
  char * pc = strrchr( t, '*' );
  if (pc) {
    *pc = '\0';
  }

  if ( (0 == strcmp( t, "char" ))          ||
       (0 == strcmp( t, "signed char" ))   ||
       (0 == strcmp( t, "unsigned char" )) ||
       (0 == strcmp( t, "byte" ))          ||
       (0 == strcmp( t, "boolean" ))       ||
       (0 == strcmp( t, "bool" )) ) {
    server_size  = asc_client_size  = uni_client_size  = 1;
    server_align = asc_client_align = uni_client_align = 1;
    return 1;
  }

  if ( (0 == strcmp( t, "short" ))              ||
       (0 == strcmp( t, "signed short" ))       ||
       (0 == strcmp( t, "unsigned short"))      ||
       (0 == strcmp( t, "short int" ))          ||
       (0 == strcmp( t, "signed short int" ))   ||
       (0 == strcmp( t, "unsigned short int" )) ||
       (0 == strcmp( t, "wchar_t" )) ) {
    server_size  = asc_client_size  = uni_client_size  = 2;
    server_align = asc_client_align = uni_client_align = 2;
    return 1;
  }

  if ( (0 == strcmp( t, "signed" ))            ||
       (0 == strcmp( t, "unsigned" ))          ||
       (0 == strcmp( t, "int" ))               ||
       (0 == strcmp( t, "signed int" ))        ||
       (0 == strcmp( t, "unsigned int" ))      ||
       (0 == strcmp( t, "long" ))              ||
       (0 == strcmp( t, "signed long" ))       ||
       (0 == strcmp( t, "unsigned long" ))     ||
       (0 == strcmp( t, "long int" ))          ||
       (0 == strcmp( t, "signed long int" ))   ||
       (0 == strcmp( t, "unsigned long int" )) ||
       (0 == strcmp( t, "float" )) ) {
    server_size  = asc_client_size  = uni_client_size  = sizeof(int);
    server_align = asc_client_align = uni_client_align = sizeof(int);
    return 1;
  }

  if ( (0 == strcmp( t, "hyper" ))                  ||
       (0 == strcmp( t, "signed hyper" ))           ||
       (0 == strcmp( t, "unsigned hyper" ))         ||
       (0 == strcmp( t, "long long" ))              ||
       (0 == strcmp( t, "signed long long" ))       ||
       (0 == strcmp( t, "unsigned long long" ))     ||
       (0 == strcmp( t, "long long int" ))          || /* PTS 1112805 */
       (0 == strcmp( t, "signed long long int" ))   || /* PTS 1112805 */
       (0 == strcmp( t, "unsigned long long int" )) || /* PTS 1112805 */
       (0 == strcmp( t, "double" )) ) {
    server_size  = asc_client_size  = uni_client_size  = 8;
    server_align = asc_client_align = uni_client_align = 8;
    return 1;
  }

  IspcCTypedef findNode;
  findNode.setName( elemUnknown_ispc, t );
  IspcCTypedef * const * foundptr = (m_index.FindNode(&findNode))->GetKey();
  if (foundptr) {
    server_size      = (*foundptr)->getServerSize();
    asc_client_size  = (*foundptr)->getAscClientSize();
    uni_client_size  = (*foundptr)->getUniClientSize();
    server_align     = (*foundptr)->getServerAlign();
    asc_client_align = (*foundptr)->getAscClientAlign();
    uni_client_align = (*foundptr)->getUniClientAlign();
    return 1;
  }
  else {
    char buf[128];
    strcpy( buf, usedby );
    strcat( buf, ":" );
    strcat( buf, username );
    server_size  = asc_client_size  = uni_client_size  = -1;
    server_align = asc_client_align = uni_client_align = -1;
    Ispc_ErrorMsg( C_TYPE_UNKNOWN_ISPC, buf, t );
  }
  return 0;

}

/*----------------------------------------------------------------------------*/

void IspcCGenInspecCode::printGuidV1( ofstream & out, IspcCTypedef & type ) {

  int i;
  const GUID * const guid = type.getUuid();

  //out.setf(ios::hex); doesn't work on sun

  char cData[9];

  sprintf( cData, "%x", guid->Data1 );
  out << " { 0x" << cData;

  sprintf( cData, "%x", guid->Data2 );
  out << ", 0x" << cData;

  sprintf( cData, "%x", guid->Data3 );
  out << ", 0x" << cData << "," << endl << " {";

  //out << "{ 0x" << guid->Data1 << ", 0x" << guid->Data2 << ", 0x" 
  //  << guid->Data3 << "," << endl << "{";

  for ( i = 0; i < 7; i++ ) {
    sprintf( cData, "%x" , (unsigned short)guid->Data4[i] );
    out << " 0x" << cData << ",";
    //out << " 0x" << (unsigned short)guid->Data4[i] << ",";
  }

  sprintf( cData, "%x", (unsigned short)guid->Data4[7] );
  out << " 0x" << cData << " }};" << endl;

  //out << " 0x" << (unsigned short)guid->Data4[8] << " }};" << endl;
  //out.unsetf(ios::hex);

}

/*----------------------------------------------------------------------------*/

IspcCTypedef * const * IspcCGenInspecCode::findNode(IspcCTypedef & findNode) {
  return (m_index.FindNode(&findNode))->GetKey();
}

/*----------------------------------------------------------------------------*/

void IspcCGenInspecCode::charData( int & error, const char * s, int len ) {

  if (m_is_livecachetypes) { /* PTS 1141233, PG */
    return;
  }

  if ( m_expr_flag && (NULL != m_act_pType) ) {
    /*+++++ PTS 1109412 & PTS 1118779 +++++*/
    if (m_expr_token.m_token_value) {
      char * p = new char [strlen(m_expr_token.m_token_value)+len+1];
      strcpy( p, m_expr_token.m_token_value );
      int pos = strlen(p);
      memcpy( p+pos, s, len );
      p[pos+len] = '\0';
      delete m_expr_token.m_token_value;
      m_expr_token.m_token_value = p;
    }
    else {
    /*----- PTS 1109412 -----*/
      m_expr_token.m_token_value = new char[len+1];
      memcpy( m_expr_token.m_token_value, s, len );
      m_expr_token.m_token_value[len] = '\0';
    } // PTS 1109412
  }

}

/*----- PTS 1131325, PG ------------------------------------------------------*/

void IspcCGenInspecCode::genABAPTabHandle( int &error, char * structName ) {

  IspcCTypedef * pType;
  char  name[256];
  short basis_err = e_ok;
  short server_size  = 0, asc_client_size  = 0, uni_client_size  = 0,
        server_align = 0, asc_client_align = 0, uni_client_align = 0;

  /* structure */
  pType = new IspcCTypedef;
  g_struct_level.push_front(pType);
  strncpy( name, structName, strlen(structName) );
  strcpy( &name[strlen(structName)], "TabHandle" );
  pType->setName( elemStruct_ispc, name );
//  pType->setUuid("<8>-<4>-<4>-<4>-<12>");
  basis_err = m_index.Insert(pType);
  if (basis_err) {
    Ispc_ErrorMsg( C_DUP_DEFINITION_ISPC, pType->getName() );
    if (!error) {
      error = - C_DUP_DEFINITION_ISPC;
    }
  }

  /* member RowDef */
  pType = new IspcCTypedef;
  pType->setName( elemMember_ispc, "RowDef" );
  strncpy( name, structName, strlen(structName) );
  strcpy( &name[strlen(structName)], "*" );
  getTypeCharacteristics( name, "STRUCT member", "RowDef",
                          server_size, server_align,
                          asc_client_size, asc_client_align,
                          uni_client_size, uni_client_align );
  pType->setType( *this, name,
                  server_size, server_align,
                  asc_client_size, asc_client_align,
                  uni_client_size, uni_client_align );
  if (!g_struct_level.empty()) {
    g_struct_level.front()->appendMember(pType);
  }
  server_size  = asc_client_size  = uni_client_size  = 0;
  server_align = asc_client_align = uni_client_align = 0;

  /* member Handle */
  pType = new IspcCTypedef;
  pType->setName( elemMember_ispc, "Handle" );
  getTypeCharacteristics( "OmsTypeABAPTabHandle", "STRUCT member", "Handle", server_size, server_align,
                          asc_client_size, asc_client_align, uni_client_size, uni_client_align );
  pType->setType( *this, "OmsTypeABAPTabHandle", server_size, server_align,
                  asc_client_size, asc_client_align, uni_client_size, uni_client_align );
  if (!g_struct_level.empty()) {
    g_struct_level.front()->appendMember(pType);
  }

}

