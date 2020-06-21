/*!***************************************************************************

  module      : Catalog_DBProcObject.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : Catalog DBProc Object

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#include "SQLManager/Catalog/Catalog_DBProcObject.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*!
   @brief predefined dbproc languages
*/

static char* Catalog_DBProcLanguages[] = { 
    (char*) "PL",    //<! internal language PLSQL 
    (char*) "C++",   //<! dbproc written in C++
    (char*) "JAVA",  //<! dbproc writen in JAVA
    (char*) "SYSTEM" //<! SYSTEM dbproc
};

//-------------------------------------------------------------------------

Catalog_DBProcObject::Catalog_Key::Catalog_Key(const SQLMan_Surrogate& procId)
: Catalog_Object::Catalog_Key()
{
    SAPDB_MemCopyNoCheck (&this->stableid()[0], &procId[0], sizeof(procId));
    this->sentrytyp()[1] = TYPE_METHOD[1];
    this->slinkage()[1]  = 1;
}

//-------------------------------------------------------------------------

Catalog_DBProcObject::ParameterIterator::ParameterIterator(const Catalog_DBProcObject& dbproc)
: m_dbproc(dbproc)
, m_index(0)
, m_currParameter((dbproc.me_param_cnt > 0) ? dbproc.me_param_list()[0] : NULL)
{
}

//-------------------------------------------------------------------------

Catalog_DBProcObject::ParameterIterator::~ParameterIterator()
{
}

//-------------------------------------------------------------------------

Catalog_DBProcObject::ParameterIterator::operator bool() const
{
    return (m_index < m_dbproc.me_param_cnt);
}

//-------------------------------------------------------------------------

void Catalog_DBProcObject::ParameterIterator::operator++()
{
    ++m_index;
    m_currParameter = (m_index < m_dbproc.me_param_cnt) ? m_dbproc.me_param_list()[m_index] : NULL;
}

//-------------------------------------------------------------------------

SQLMan_DataType Catalog_DBProcObject::ParameterIterator::ParamDataType() const
{
    return m_currParameter->param_datatype;
}

//-------------------------------------------------------------------------

int Catalog_DBProcObject::ParameterIterator::Fraction() const
{
    return m_currParameter->param_datafrac;
}

//-------------------------------------------------------------------------

int Catalog_DBProcObject::ParameterIterator::DataLength() const
{
    return m_currParameter->param_datalength;
}

//-------------------------------------------------------------------------

int Catalog_DBProcObject::ParameterIterator::InOutLength() const
{
    return m_currParameter->param_inoutlength;
}

//-------------------------------------------------------------------------

bool Catalog_DBProcObject::ParameterIterator::IsInputParameter() const
{
    return ((sp1io_input == m_currParameter->param_in_out[1]) || 
            (sp1io_inout == m_currParameter->param_in_out[1]));
}

//-------------------------------------------------------------------------

bool Catalog_DBProcObject::ParameterIterator::IsOutputParameter() const
{
    return ((sp1io_output == m_currParameter->param_in_out[1]) || 
            (sp1io_inout  == m_currParameter->param_in_out[1]));
}

//------------------------------------------------------------------------- 

int Catalog_DBProcObject::ParamCount() const
{
    return me_param_cnt;
}

//------------------------------------------------------------------------- 

Catalog_DBProcObject::ParameterIterator Catalog_DBProcObject::FirstParameter() const
{
    return ParameterIterator(*this);
}

//------------------------------------------------------------------------- 

const Catalog_ObjectId& Catalog_DBProcObject::GetId() const
{
    return *reinterpret_cast<const Catalog_ObjectId*>(&me_surrogate[0]);
}

//------------------------------------------------------------------------- 
Catalog_DBProcObject::ParameterIterator Catalog_DBProcObject::GetParameter(int idx) const
{
    ParameterIterator iter(*this);
    if ((idx < 0) || (idx >= me_param_cnt))
    {
        // out of bounds
        iter.m_index         = me_param_cnt;
        iter.m_currParameter = NULL;
    }
    return iter;
}

//------------------------------------------------------------------------- 

Catalog_DBProcObject::Language Catalog_DBProcObject::GetLanguage() const
{
    switch (me_language())
    {
    case lang_pl     : return PL;
    case lang_cpp    : return Cpp;
    case lang_java   : return Java;
    case lang_system : return System;
    default :
        return Unknown;
    }
}

//------------------------------------------------------------------------- 

const char* Catalog_DBProcObject::GetLanguageString() const
{
    return Catalog_DBProcLanguages[me_language()];
}

//-------------------------------------------------------------------------

