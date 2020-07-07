/*!
  @file           IFRPacket_FunctionCode.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Util
  @brief          
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
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_FunctionCode.h"

IFR_BEGIN_NAMESPACE

IFRPacket_FunctionCode::IFRPacket_FunctionCode(int functioncode)
: m_functionCode((enum FunctionCode)functioncode)
{
  m_isQuery = isQuery(functioncode);
  m_isMassCommand = isMassCommand(functioncode);
}

IFR_Bool IFRPacket_FunctionCode::isQuery(int functioncode) {
  if (isMassCommand(functioncode)) {
    functioncode -= MinMasscmd_FC;
  }
  return (functioncode == Select_FC ||
          functioncode == Show73_FC ||
          functioncode == DBProcWithResultSetExecute_FC ||
          functioncode == Explain_FC);
}

IFR_Bool IFRPacket_FunctionCode::isMassCommand(int functioncode) {
  return (functioncode >= MinMasscmd_FC && functioncode <= MaxMasscmd_FC);
}

IFR_Bool IFRPacket_FunctionCode::isConnectionReleasing(int functioncode)
{
    return (functioncode ==  CommitRelease_FC) || (functioncode == RollbackRelease_FC);
}

IFR_Bool IFRPacket_FunctionCode::isConnectionReleasing()
{
    return isConnectionReleasing(m_functionCode);
}

IFR_TraceStream& operator << (IFR_TraceStream& s, const IFRPacket_FunctionCode &p )
{
  s << p.m_functionCode << " ";
  return s;
}

IFR_END_NAMESPACE
