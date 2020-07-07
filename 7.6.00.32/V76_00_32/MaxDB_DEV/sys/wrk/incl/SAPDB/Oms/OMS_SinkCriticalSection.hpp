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
#ifndef __OMS_SINKCRITICALSECTION_HPP
#define __OMS_SINKCRITICALSECTION_HPP

#include "ilivecachesink.h" // nocheck
#include "Oms/OMS_DbpBase.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "Oms/OMS_Globals.hpp"

class OMS_SinkCriticalSection {
private :
  IliveCacheSink*   m_lcSink;
  int               m_regionId;
  bool              m_inSection;
public :
  OMS_SinkCriticalSection(IliveCacheSink* lcSink,int regionId) 
    : m_lcSink(lcSink)
    , m_regionId(regionId)
    , m_inSection(false) 
  {
    if (m_regionId < 0 || m_regionId >= OMS_INTERNAL_CRITICAL_SECTIONS){
      char buf[128];
      sp77sprintf(buf, sizeof(buf), "OMS_SinkCriticalSection::Cto: invalid regionId %d", m_regionId); 
      OMS_Globals::Throw(e_invalid_csid, buf, __MY_FILE__, __LINE__);
    }
  } 

  void Enter() {
    if (m_inSection) {
      char buf[128];
      sp77sprintf(buf, sizeof(buf), "OMS_SinkCriticalSection::Enter: Region %d is already entered by this session", m_regionId); 
      OMS_Globals::Throw(e_already_in_critical_section, buf, __MY_FILE__, __LINE__); 
    }
    else {
      m_lcSink->EnterCriticalSection(m_regionId);
      m_inSection = true;
    }
  }

  void Leave() {
    if (m_inSection) {
      m_lcSink->LeaveCriticalSection(m_regionId);
      m_inSection = false;
    }
    else {
      char buf[128];
      sp77sprintf(buf, sizeof(buf), "OMS_SinkCriticalSection::Leave: Region %d was not entered by this session", m_regionId); 
      OMS_Globals::Throw(e_not_in_critical_section, buf, __MY_FILE__, __LINE__); 
    }
  }
  ~OMS_SinkCriticalSection() {
    if (m_inSection) {
      Leave();
    }
  }
};

#endif  // __OMS_SINKCRITICALSECTION_HPP
