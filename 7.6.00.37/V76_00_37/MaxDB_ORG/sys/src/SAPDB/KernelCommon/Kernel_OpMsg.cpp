/*!*********************************************************************

  module:       Kernel_OpMsg.cpp

  ----------------------------------------------------------------------

  responsible:  ThomasA

  special area: Kernel | Debug

  description:  Stream to write formatted knldiag entries

  ----------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


***********************************************************************/

#include "heo60.h"
#include "SAPDB/KernelCommon/Kernel_OpMsg.hpp"

Kernel_OpMsg::LineBuffer::LineBuffer (tsp3_priority_Enum prio,tsp3_msg_type_Enum type, int msgNo, const char* label)
: SAPDB_StreamBuffer (m_txt, m_txt + sizeof(m_txt))
, m_prio(prio)
, m_type(type)
, m_no(msgNo)
{
	memset(&m_label[0], ' ', sizeof(m_label));
	int copyLen = strlen(label);
	if (copyLen > sizeof(m_label))
	{
		copyLen = sizeof(m_label);
	}
	memcpy(&m_label[0], label, copyLen); 
}

/*----------------------------------------*/

void Kernel_OpMsg::LineBuffer::Overflow ()
{
    const char * start = this->Begin ();
    const char * end = this->Next ();
    const char * nlPos;
    tsp00_C40    msg;
    /*
     * break into lines
     */
    nlPos = REINTERPRET_CAST (const char *, memchr (start, '\n', end - start));
    while (nlPos != NULL) {
		memset(msg, ' ', sizeof(msg));
		memcpy(&msg[0], start, nlPos - start);
		vmessage (m_prio, m_type, m_no, m_label, msg);
        start = nlPos + 1;
        nlPos = REINTERPRET_CAST (const char *, memchr (start, '\n', end - start));
    }
    /*
     * write remainder as one line
     */
    if (end - start > 0) {
		memset(msg, ' ', sizeof(msg));
		memcpy(&msg[0], start, end - start);
		vmessage (m_prio, m_type,m_no, m_label, msg);
    }
}

/*----------------------------------------*/

Kernel_OpMsg::Kernel_OpMsg (tsp3_priority_Enum prio,tsp3_msg_type_Enum type, int msgNo, const char* label)
    : m_buf(prio, type, msgNo, label)
	, SAPDB_OStream(m_buf)
{
}

/*----------------------------------------*/

Kernel_OpMsg::~Kernel_OpMsg ()
{
    this->Flush ();
}

/*----------------------------------------*/


