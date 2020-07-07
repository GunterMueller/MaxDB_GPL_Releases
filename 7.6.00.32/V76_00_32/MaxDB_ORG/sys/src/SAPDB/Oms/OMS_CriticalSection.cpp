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
#include "Oms/OMS_CriticalSection.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_ObjectContainer.hpp"

/*----------------------------------------------------------------------*/
/* Implementation of class OmsCriticalSection                           */
/*----------------------------------------------------------------------*/

OmsCriticalSection::OmsCriticalSection(OmsHandle* h, int regionId) 
  : m_session(h->m_pSession), 
    m_inSection(false), 
    m_regionId(regionId)
{
  int cnt = OMS_Globals::KernelInterfaceInstance->GetOmsRegionCnt();
  if ((m_regionId <= 0) || (m_regionId > cnt - OMS_INTERNAL_CRITICAL_SECTIONS)) // PTS 1112197
  {
    h->dbpOpError("OmsCriticalSection : invalid regionId %d", m_regionId); 
    char buf[128];
    sp77sprintf(buf, sizeof(buf), "OmsCriticalSection::Cto: invalid regionId %d", m_regionId); 
    m_session->ThrowDBError (e_invalid_csid, buf, __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

void OmsCriticalSection::omsEnter() {
  if ((m_inSection) || (m_session->m_critical_section_in_use[m_regionId-1])) {
    char buf[128];
    sp77sprintf(buf, sizeof(buf), "OmsCriticalSection::omsEnter: Region %d is already entered by this session", m_regionId); 
    m_session->ThrowDBError (e_already_in_critical_section, buf, __MY_FILE__, __LINE__);
  }
  m_session->m_critical_section_in_use[m_regionId - 1] = true;
  m_session->m_lcSink->EnterCriticalSection(m_regionId + OMS_INTERNAL_CRITICAL_SECTIONS - 1);
  m_inSection = true;
}

/*----------------------------------------------------------------------*/

void OmsCriticalSection::omsLeave() {
  if (m_inSection) {
    m_session->m_lcSink->LeaveCriticalSection(m_regionId + OMS_INTERNAL_CRITICAL_SECTIONS - 1);
    m_session->m_critical_section_in_use[m_regionId - 1] = false;
    m_inSection = false;
  }
  else {
    char buf[128];
    sp77sprintf(buf, sizeof(buf), "OmsCriticalSection::omsLeave: Region %d was not entered by this session", m_regionId); 
    m_session->ThrowDBError ( e_not_in_critical_section, buf, __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

OmsCriticalSection::~OmsCriticalSection() {
  if (m_inSection) {
    omsLeave();
  }
}

