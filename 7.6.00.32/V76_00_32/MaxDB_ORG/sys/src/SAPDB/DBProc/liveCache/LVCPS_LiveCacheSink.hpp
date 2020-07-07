/*!
 * \file    LVCPS_LiveCacheSink.hpp
 * \author  MarkusSi
 * \brief   LiveCache Interface used by DB-Procedures in ProcServer mode.
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

#ifndef __LVCPS_LIVECACHESINK_HPP
#define __LVCPS_LIVECACHESINK_HPP

#include "liveCache/LVC_IliveCacheSink.hpp"

class LVC_ProcServerInterface;

/*!
 * \brief ProcServer liveCache sink.
 *
 * \see \ref omsprocserver, IliveCacheSink for reference of methods.
 *
 * \ingroup oms procserver
 */
class LVCPS_LiveCacheSink : public IliveCacheSink {
public:
  /// Construct new ProcServer sink.
  LVCPS_LiveCacheSink(LVC_ProcServerInterface *interf, pasbool *cancelByte)
    : m_interf(interf),
      m_pDefaultContext(NULL),
      m_pCancelByte(cancelByte)
  {
  }

  ~LVCPS_LiveCacheSink();

// include sink method declarations
#define LIVECACHE_SINK_ONLY_MEMBER_FUNCS
#include "liveCache/LVC_IliveCacheSink.hpp" 
#undef LIVECACHE_SINK_ONLY_MEMBER_FUNCS

  HRESULT STDMETHODCALLTYPE QueryInterface(const IID &iid,void **ppv);


  // Dummy methods
  ULONG STDMETHODCALLTYPE AddRef(void)
  {
    /* !"§$ TODO ?? */
    return 1;
  }

  ULONG STDMETHODCALLTYPE Release(void)
  {
    /* !"§$ TODO ?? */
    return 1;
  }


private:
  LVC_ProcServerInterface     *m_interf;                    ///< ProcServer communication interface.

  void                        *m_pDefaultContext;           ///< Default context pointer.
  pasbool                     *m_pCancelByte;               ///< Cancel byte.
};

#endif // __LVCPS_LIVECACHESINK_HPP
