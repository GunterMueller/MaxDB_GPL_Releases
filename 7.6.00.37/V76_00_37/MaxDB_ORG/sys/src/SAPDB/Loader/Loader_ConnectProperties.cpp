/*!
  \file    Loader_ConnectProperties.cpp
  \author  SteffenS
  \brief   Implements the Loader connection class

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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
#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_ErrorMsgHandler.hpp"
#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_Loader.hpp"
#include "Interfaces/SQLDBC/SQLDBC.h"


#include "SAPDBCommon/SAPDB_string.h"       // SAPDB_memcpy


/*!
    @brief constructor
*/

Loader_ConnectProperties::Loader_ConnectProperties():
m_prop(NULL)
{
    m_prop = new SQLDBC_ConnectProperties();
}
// LOADER_ConnectProperties()



/*!
    @brief destructor
*/

Loader_ConnectProperties::~Loader_ConnectProperties()
{
    if (NULL != m_prop)
    {
        delete m_prop;
        m_prop = NULL;
    }
}
// ~LOADER_ConnectProperties()


void
Loader_ConnectProperties::setProperty(const LOADER_Char* key, const LOADER_Char* value)
{
    m_prop->setProperty(key, value);
}
// setProperty()


const char*
Loader_ConnectProperties::getProperty(const LOADER_Char*key, const LOADER_Char* defaultvalue) const
{
    return this->m_prop->getProperty(key, defaultvalue);
}
// getProperty()