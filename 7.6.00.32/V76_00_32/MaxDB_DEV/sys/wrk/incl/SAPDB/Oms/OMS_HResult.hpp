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
#ifndef __OMS_HRESULT_HPP
#define __OMS_HRESULT_HPP

#include "Oms/OMS_Defines.h"
#include "Oms/OMS_DbpError.hpp"

class OMS_HResult {
public :
  OMS_HResult() : m_hresult(0) {}

  OMS_HResult(HRESULT hr) : m_hresult(hr) {
    if (FAILED(hr)) {
      throw DbpError(DbpError::HRESULT_ERROR, hr, __MY_FILE__, __LINE__);
    }
  }

  void operator= (HRESULT hr) {
    if (FAILED(hr)) {
      throw DbpError(DbpError::HRESULT_ERROR, hr, __MY_FILE__, __LINE__);
    }
    m_hresult = hr;
  }
private :
  HRESULT m_hresult;
};

#endif  // __OMS_HRESULT_HPP
