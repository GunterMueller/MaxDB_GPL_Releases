/*!
  @file           SAPDBFields_AttributedField.cpp
  @author         UweH
  @ingroup        Field
  @brief          methods for AttributedField

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
#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
//--------------------------------------------------------------------------------------------
const char * SAPDBFields_AttributedField::AttributeToString[] =
{
    "value",
    "null",
    "default",
    "blob",
    "special-null",
    "ignored",
    "unused"
};
//--------------------------------------------------------------------------------------------
void SAPDBFields_AttributedField::CheckAttributeFailed(const SAPDB_Char *file,
                                                       SAPDB_UInt2       line)
{
    RTE_Crash (Fields_Exception(file,line,FIELD_ATTRIBUTE_INVALID,
                                AttributeToString[m_Attribute],
                                SAPDB_ToString   (m_DataLen)));
}
//--------------------------------------------------------------------------------------------
void SAPDBFields_AttributedField::WriteToTrace(const SAPDB_Char *title) const
{
    SAPDBFields_Field::WriteToTrace(title);
    Kernel_VTrace() << "Attribute: " << AttributeToString[m_Attribute];
}
//--------------------------------------------------------------------------------------------
void SAPDBFields_AttributedField::AppendContextInfo(SAPDBErr_MessageList &errlist,
                                                    const SAPDB_Char     *filename,
                                                    const SAPDB_UInt      linenumber,
                                                    const SAPDB_Char     *title) const
{
    SAPDBFields_Field::AppendContextInfo(errlist,filename,linenumber,title);
    errlist = errlist + Fields_Exception(filename,linenumber,FIELDATTRIBUTED_CONTEXT_INFO,
                                         AttributeToString[m_Attribute]);
}
