/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef __OMS_SIMDUMPCOLLECTOR_HPP
#define __OMS_SIMDUMPCOLLECTOR_HPP

#include "Oms/OMS_AFX.h"

class AFX_EXT_CLASS OmsSimDumpCollector {
public:
  OmsSimDumpCollector();  // to make Win32 linker happy...

  virtual void printf(const char *fmt, ...) = 0;
  virtual const char* str() const = 0;
  virtual void flush() = 0;
  inline operator const char *() const { return str(); }
};

#endif  // __OMS_SIMDUMPCOLLECTOR_HPP
