/*!
 * \file    LVCPS_Dispatcher.hpp
 * \author  IvanS
 * \brief   OMS Stored Procedure dispatcher for ProcServer client side.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



*/

#include "DBProc/liveCache/LVCPS_ParamHandler.hpp"
#include "geo00d.h"

/*!
 * Construct a parameter handler for ProcServer.
 *
 * \param libname library name,
 * \param md5 MD5 checksum of the library for checking.
 */
LVCPS_ParamHandler::LVCPS_ParamHandler(const char *libname, const tsp00_C32 &md5)
  : m_loaded(false), m_clsid(NULL), m_iid(NULL), m_dispid(0), m_paramcnt(0), m_methodfncmo(NULL),
    m_methodfnc(NULL), m_paramfncmo(NULL), m_paramfnc(NULL)
{
  tsp00_C32 libmd5;
  tsp00_C8 packageId;
  SAPDB_MemFillNoCheck(&packageId[0], 0, sizeof(packageId));
  LVC_LoadComponentLibrary(libname, packageId, &m_handle, libmd5);
  if (libmd5 != md5) {
    // MD5 doesn't match; TODO: protocol
    return;
  }
  // look up query method
  LVC_DispIntrospectObject introobj = (LVC_DispIntrospectObject)sql36GetProcAddress(
    (HINSTANCE) m_handle, "Co_IntrospectObject");
  if (NULL == introobj) {
    // no introspect function; TODO: protocol
    return;
  }
  // query object
  long version;
  introobj(&m_clsid, &m_iid, &m_methcnt, &version);
  // get method introspection function
  m_methodfncmo = (LVC_DispIntrospectMethodMO)sql36GetProcAddress(
    (HINSTANCE) m_handle, "Co_IntrospectMethod_MO");
  if (NULL == m_methodfncmo) {
    m_methodfnc = (LVC_DispIntrospectMethod)sql36GetProcAddress(
      (HINSTANCE) m_handle, "Co_IntrospectMethod");
    if (NULL == m_methodfnc) {
      // no method introspect function; TODO: protocol
      return;
    }
  }

  // loading done
  m_loaded = true;
}

/*----------------------------------------------------------------------------*/

/*!
 * Select method for parameter type reading.
 *
 * \param name method name.
 *
 * \return \c true, if method found, \c false otherwise.
 */
bool LVCPS_ParamHandler::selectMethod(const char *name)
{
  char *n = const_cast<char*>(name);
  if (m_methodfncmo) {
    m_methodfncmo(LVC_Disp_get_by_name, &n, 0, &m_dispid,
      &m_paramcnt, (void**) &m_paramfncmo);
    if (NULL == m_paramfncmo)
      return false;
  } else {
    m_methodfnc(LVC_Disp_get_by_name, &n, 0, &m_dispid,
      &m_paramcnt, (void**) &m_paramfnc);
    if (NULL == m_paramfnc)
      return false;
  }
  return true;
}

/*----------------------------------------------------------------------------*/

/*!
 * Create one parameter in debug stack from va_list.
 * 
 * \param idx index of the paramter (0-based),
 * \param pp pointer into constructed debug stack,
 * \param args argument list from which to initialize parameter.
 * \return count of subparameters (if any, e.g., structure or table handle) or 0.
 */
int LVCPS_ParamHandler::createParam(unsigned short idx, tak_dbproc_stack_param_debug *pp, va_list &args)
{
  if (idx >= m_paramcnt) {
    // TODO: report error
    return 0;
  }
  char *mode, *iotype, *name;
  short int datatype, subdatatype, length, dim;
  long offset;
  int offsetserv, offsetasc, offsetuni;
  unsigned char frac;
  GUID *guid;
  if (m_paramfncmo) {
    m_paramfncmo(idx + 1, &mode, &iotype, &datatype, &subdatatype, &length, &dim,
      &offsetserv, &offsetasc, &offsetuni, &frac, &guid, &name);
  } else {
    m_paramfnc(idx + 1, &mode, &iotype, &datatype, &subdatatype, &length, &dim,
      &offset, &frac, &guid, &name);
  }

  pp->dbg_vt = datatype;
  pp->dbg_inout.becomes((strcmp(iotype, "IN") == 0) ? sp1io_input :
    ((strcmp(iotype, "OUT") == 0) ? sp1io_output : sp1io_inout));
  pp->dbg_is_ptr = false;
  if (pp->dbg_inout == sp1io_output || pp->dbg_inout == sp1io_inout)
    pp->dbg_is_ptr = true;
  if (datatype == csp1_vt_carray || datatype == csp1_vt_userdefined ||
    datatype == csp1_vt_lc_abap_tab_handle ||
    datatype == csp1_vt_lc_stream_handle)
    pp->dbg_is_ptr = true;
  pp->dbg_length = length;
  if (pp->dbg_is_ptr) {
    pp->dbg_value.ptr() = va_arg(args, tsp00_Void*);
  } else
  switch (datatype) {
    case csp1_vt_i1:
    case csp1_vt_ui1:
    case csp1_vt_lc_char:
    case csp1_vt_lc_ascii7:
    case csp1_vt_lc_byte:
    case csp1_vt_lc_bool:
    case csp1_vt_lc_int1:
    case csp1_vt_lc_uint1:
      pp->dbg_length = 1;
      {
        //pp->dbg_value.i1() = va_arg(args, tsp00_C1);
        int i = va_arg(args, int);
        char c = static_cast<char>(i);
        pp->dbg_value.i1() = *(reinterpret_cast<tsp00_C1*>(&c));
      }
      break;
    case csp1_vt_lc_wchar:
    case csp1_vt_i2:
    case csp1_vt_ui2:
      pp->dbg_length = 2;
      {
        //pp->dbg_value.i2() = va_arg(args, short);
        int i = va_arg(args, int);
        pp->dbg_value.i2() = static_cast<short>(i);
      }
      break;
    case csp1_vt_i4:
    case csp1_vt_int:
    case csp1_vt_ui4:
    case csp1_vt_uint:
    case csp1_vt_r4:
      pp->dbg_length = 4;
      pp->dbg_value.i4() = va_arg(args, int);
      break;
    case csp1_vt_r8:
      pp->dbg_length = 8;
      pp->dbg_value.r8() = va_arg(args, double);
      break;
    default:
      pp->dbg_length = sizeof(void*);
      pp->dbg_value.ptr() = va_arg(args, tsp00_Void*);
      break;
  }
  return dim;
}

/*----------------------------------------------------------------------------*/
