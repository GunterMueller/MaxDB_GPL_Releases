/*!
 * \file    OMS_StackTraceInterface.hpp
 * \author  IvanS
 * \brief   Callback interface for stack trace evaluation.
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

#ifndef  __OMS_STACKTRACEINTERFACE_HPP
#define  __OMS_STACKTRACEINTERFACE_HPP

#include "Oms/OMS_AFX.h"

class AFX_EXT_CLASS OmsStackTraceCallbackInterface {
public:
  /*!
   * \brief Callback for one level in stack.
   *
   * \param frame pointer to the code segment,
   * \param symbol symbol name,
   * \param demangledSymbol demangled symbol name,
   * \param offsetToSymbol offset from the beginning of the symbol,
   * \param moduleName name of the module,
   * \param sourceFileName name of the source file (if available),
   * \param lineNumber line number in source file (if available).
   */
  virtual void omsStackTrace(const void *frame,
    const char *symbol, const char *demangledSymbol,
    unsigned int offsetToSymbol, const char *moduleName,
    const char *sourceFileName, int lineNumber) = 0;
};

#endif  // __OMS_STACKTRACEINTERFACE_HPP
