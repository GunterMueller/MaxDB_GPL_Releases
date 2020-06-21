/*!*****************************************************************************

  module      : IspcCGenHFile.h

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


#ifndef ISPCCGENHFILE_H
#define ISPCCGENHFILE_H


#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <fstream>
#define  ofstream  std::ofstream
#else
#include <iostream.h>
#endif

#include <string>

#include "idl/xml2ispc/Ispc_InterfaceTypeList.hpp"
#include "idl/xml2ispc/Ispc_InterfaceStructRegistry.hpp" /* PTS 1134618, PG */
#include "idl/xml2ispc/Ispc_InterfaceConstList.hpp"      /* PTS 1134734, PG */


class IspcCGenHFile {
  char                         m_filename[256];
  char                         m_name[64];
  char                         m_libname[64];
  char                         m_structname[64];
  char                         m_interfacename[64];
  char                         m_enumname[64];      /* PTS 1131325, PG */
  char                         m_constname[64];     /* PTS 1134734, PG */
  char                         m_iid[64];
  ofstream                     m_out_h;
  ofstream                     m_out_c;
  std::string                  m_def_constr_head;   /* PTS 1131325, PG */
  std::string                  m_def_constr_body;   /* PTS 1131325, PG */
  std::string                  m_abap_tab_handle;   /* PTS 1131325, PG */
  short                        m_is_within_import;
  short                        m_is_within_include;
  short                        m_is_within_struct;
  short                        m_is_within_union;   /* PTS 1141233, PG */
  short                        m_is_within_enum;    /* PTS 1131325, PG */
  short                        m_is_livecachetypes; /* PTS 1131325, PG: special handling for OMS_LiveCacheTypes.xml */
  bool                         m_is_cppsqldbc_sec;  /* PTS 1138438, PG: special handling for SQLDBC related coding */
  short                        m_gen_def_constr;    /* PTS 1131325, PG */
  bool                         m_i_declared;        /* PTS 1134734, PG */
  bool                         m_gen_tab_handle;    /* PTS 1131325, PG */
  short                        m_methode_param;
  Ispc_InterfaceTypeList       m_ift_list;          /* PTS 1131325, PG */
  bool                         m_features_76;       /* PTS 1131325, PG */
  bool                         m_typedef_check;     /* PTS 1131325, PG */
  Ispc_InterfaceStructRegistry m_ifs_registry; /* PTS 1134618, PG */
  Ispc_InterfaceStruct *       m_ifStruct;     /* PTS 1134618, PG */
  std::string                  m_col_enum;     /* PTS 1134618, PG */
  Ispc_InterfaceConst *        m_ifConst;      /* PTS 1134734, PG */
  Ispc_InterfaceConstList      m_ifc_list;     /* PTS 1134734, PG */
public:
  IspcCGenHFile();
  ~IspcCGenHFile();
  int Init( char * cFilename, bool features_76, bool typedef_check ); /* PTS 1131325, PG */
  void startElement(int &error, const char *name, const char **atts); /* PTS 1131325, PG */
  void endElement(const char *name);
  void charData( int & error, const char * s, int len ); /* PTS 1134734, PG */
};


#endif

