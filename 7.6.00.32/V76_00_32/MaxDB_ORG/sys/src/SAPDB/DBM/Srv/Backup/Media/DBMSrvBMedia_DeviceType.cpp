/*!
  \file    DBMSrvBMedia_DeviceType.cpp
  \author  TiloH
  \ingroup backup media handling by the DBM Server
  \brief   implementing a class for differentiating device types

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#include "gcn003.h"
#include "hcn90.h"
#include "DBM/Srv/Backup/Media/DBMSrvBMedia_DeviceType.hpp"


DBMSrvBMedia_DeviceType::DBMSrvBMedia_DeviceType(
    const DBMSrvBMedia_DeviceType::DeviceType deviceType)
  : m_DeviceType(deviceType)
{
}

const char * DBMSrvBMedia_DeviceType::getAsString() const
{
    switch(m_DeviceType)
    {
        case File: return ME_TYPE_FILE_CN003; break;
        case Pipe: return ME_TYPE_PIPE_CN003; break; 
        case Tape: return ME_TYPE_TAPE_CN003; break;
        default:   return ME_TYPE_UNKN_CN003;
    }
}

bool DBMSrvBMedia_DeviceType::operator == (const DBMSrvBMedia_DeviceType::DeviceType otherDeviceType) const
{
    return (int)m_DeviceType==(int)otherDeviceType;
}

void DBMSrvBMedia_DeviceType::setTo(const char * deviceType)
{
    if(0==stricmp(deviceType, ME_TYPE_FILE_CN003))
        m_DeviceType=DBMSrvBMedia_DeviceType::File;
    else
        if(0==stricmp(deviceType, ME_TYPE_PIPE_CN003))
            m_DeviceType=DBMSrvBMedia_DeviceType::Pipe;
        else
            if(0==stricmp(deviceType, ME_TYPE_TAPE_CN003))
                m_DeviceType=DBMSrvBMedia_DeviceType::Tape;
            else
                m_DeviceType=DBMSrvBMedia_DeviceType::Unknown;
}

bool operator == (
        const DBMSrvBMedia_DeviceType::DeviceType deviceType1,
        const DBMSrvBMedia_DeviceType             deviceType2)
{
    return deviceType2==deviceType1;
}

bool operator != (
        const DBMSrvBMedia_DeviceType::DeviceType deviceType1,
        const DBMSrvBMedia_DeviceType             deviceType2)
{
    return !(deviceType2==deviceType1);
}
