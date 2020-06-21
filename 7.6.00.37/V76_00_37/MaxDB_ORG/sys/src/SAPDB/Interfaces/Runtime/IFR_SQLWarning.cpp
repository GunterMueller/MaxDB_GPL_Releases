/*!
  @file           IFR_SQLWarning.cpp
  @author         D039759
  @ingroup        IFR_Common
  @brief          SQL Warnings
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/
#include "Interfaces/Runtime/IFR_SQLWarning.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
IFR_SQLWarning::IFR_SQLWarning(IFR_SQLWarningCode warningcode)
:m_warningcode(warningcode),
 m_nextwarning(0)
{}

//----------------------------------------------------------------------
IFR_SQLWarning::~IFR_SQLWarning()
{
}

//----------------------------------------------------------------------
void
IFR_SQLWarning::addWarning(IFR_SQLWarning* warning)
{
    IFR_SQLWarning *p=this;
    while(p->m_nextwarning) {
        if(p==warning) {
            return;
        }
        p=p->m_nextwarning;
    }
    p->m_nextwarning=warning;
}

IFR_END_NAMESPACE
