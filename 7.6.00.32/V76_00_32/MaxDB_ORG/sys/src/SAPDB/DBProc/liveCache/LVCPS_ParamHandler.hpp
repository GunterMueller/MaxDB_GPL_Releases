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

#ifndef __LVCPS_PARAMHANDLER_HPP
#define __LVCPS_PARAMHANDLER_HPP

#include "liveCache/LVC_LoadComponentLibrary.hpp"
#include "liveCache/LVC_DispatcherParams.hpp"
#include "vak001.h"

/*!
 * \brief Parameter handling for ProcServer.
 *
 * \see \ref procserver
 *
 * \ingroup procserver
 */
class LVCPS_ParamHandler {
public:
  /// Construct a parameter handler for ProcServer.
  LVCPS_ParamHandler(const char *libname, const tsp00_C32 &md5);

  /// Check, whether the library is correctly loaded.
  inline bool isLoaded() const throw()
  {
    return m_loaded;
  }

  /// Get class GUID.
  const CLSID &getCLSID() const throw()
  {
    return *m_clsid;
  }

  /// Get interface GUID.
  const CLSID &getIID() const throw()
  {
    return *m_iid;
  }

  /// Get method count.
  unsigned short getMethodCount() const throw()
  {
    return m_methcnt;
  }

  /// Select method for reading parameter types.
  bool selectMethod(const char *name);

  /// Return dispatcher ID for selected method.
  unsigned short getDispID() const throw()
  {
    return m_dispid;
  }

  /// Return parameter count for selected method.
  unsigned short getParamCnt() const throw()
  {
    return m_paramcnt;
  }

  /// Create one parameter in debug stack from va_list.
  int createParam(unsigned short idx,
    tak_dbproc_stack_param_debug *pp, va_list &args);

private:
  HANDLE          m_handle;   ///< Library handle.
  CLSID           *m_clsid;   ///< Class ID.
  IID             *m_iid;     ///< Interface ID.
  unsigned short  m_methcnt;  ///< Count of methods.
  bool            m_loaded;   ///< Loaded flag.

  unsigned short  m_dispid;   ///< Dispatcher ID (for selected method).
  unsigned short  m_paramcnt; ///< Parameter count (for selected method).

  LVC_DispIntrospectMethodMO  m_methodfncmo;  ///< New-style method introspection function.
  LVC_DispIntrospectMethod    m_methodfnc;    ///< Old-style method introspection function.

  LVC_DispIntrospectFuncMO    m_paramfncmo;   ///< New-style parameter introspection function.
  LVC_DispIntrospectFunc      m_paramfnc;     ///< Old-style parameter introspection function.
};

#endif  // __LVCPS_PARAMHANDLER_HPP
