/*!
  @file           SAPDBFields_Field.cpp
  @author         UweH
  @ingroup        Field
  @brief          methods for Field

\if EMIT_LICENCE
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

\endif
*/
#include "SAPDBCommon/Fields/SAPDBFields_Exceptions.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
//--------------------------------------------------------------------------------------------
const SAPDBFields_Field::Pointer
SAPDBFields_Field::GetPointer( IndexType offset,
                               SizeType  length) const
{
    if ( offset >= m_DataLen )
        RTE_Crash (Fields_Exception(__CONTEXT__,FIELDS_OFFSET_OUT_OF_RANGE,
                                    SAPDB_ToString(offset),
                                    SAPDB_ToString(m_DataLen)));
    if ( offset+length > m_DataLen )
        RTE_Crash (Fields_Exception(__CONTEXT__,FIELDS_OFFSET_AND_LENGTH_OUT_OF_RANGE,
                                    SAPDB_ToString(offset),
                                    SAPDB_ToString(length),
                                    SAPDB_ToString(m_DataLen)));
    return m_DataPtr + offset;
}
//--------------------------------------------------------------------------------------------
SAPDBFields_Field::Pointer
SAPDBFields_Field::GetPointer( IndexType offset,
                               SizeType  length)
{
    if ( offset >= m_DataLen )
        RTE_Crash (Fields_Exception(__CONTEXT__,FIELDS_OFFSET_OUT_OF_RANGE,
                                    SAPDB_ToString(offset),
                                    SAPDB_ToString(m_DataLen)));
    if ( offset+length > m_DataLen )
        RTE_Crash (Fields_Exception(__CONTEXT__,FIELDS_OFFSET_AND_LENGTH_OUT_OF_RANGE,
                                    SAPDB_ToString(offset),
                                    SAPDB_ToString(length),
                                    SAPDB_ToString(m_DataLen)));
    return m_DataPtr + offset;
}
//--------------------------------------------------------------------------------------------
void SAPDBFields_Field::WriteToTrace(const SAPDB_Char *title) const
{
    #ifndef INDIAGNOSE
    Kernel_TraceBuffer (m_DataPtr, m_DataLen, title, m_DataLen);
    #endif
}
//--------------------------------------------------------------------------------------------
void SAPDBFields_Field::AppendContextInfo(SAPDBErr_MessageList &errlist,
                                          const SAPDB_Char     *filename,
                                          const SAPDB_UInt      linenumber,
                                          const SAPDB_Char     *title) const
{
    errlist = errlist + Fields_Exception(filename,linenumber,FIELD_CONTEXT_INFO,
                                         (title==0 ? "Field" : title),
                                         SAPDB_ToString(m_DataLen),
                                         SAPDB_ToString(reinterpret_cast<void*>(m_DataPtr)));
}
