/*!
    \file         DBMSrvVol_VolumeSize.cpp
    \author       TiloH
    \ingroup      volume handling by DBM Server
    \brief        A class for handling size of a database volume

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

#include "hcn36.h"
#include "DBM/Srv/Volumes/DBMSrvVol_VolumeSize.hpp"

DBMSrvVol_VolumeSize::DBMSrvVol_VolumeSize()
  : m_Size(0)
{
}

DBMSrvVol_VolumeSize::DBMSrvVol_VolumeSize(
    int               size,
    DBMSrvMsg_Error & error)
  : m_Size(size)
{
    checkSize(error);
}

DBMSrvVol_VolumeSize::DBMSrvVol_VolumeSize(
    const char      * size,
    DBMSrvMsg_Error & error)
  : m_Size(atoi(size))
{
     checkSize(error);
}

DBMSrvVol_VolumeSize::DBMSrvVol_VolumeSize(
    const char      * percentage,
    size_t            lengthPercentage,
    int               baseSize,
    DBMSrvMsg_Error & error)
  : m_Size(0)
{
    setTo(percentage, lengthPercentage, baseSize, error);
}

bool DBMSrvVol_VolumeSize::setTo(
        const char      * size,
        DBMSrvMsg_Error & error)
{
    m_Size=atoi(size);
    return checkSize(error);
}

bool DBMSrvVol_VolumeSize::setTo(
        const char      * percentage,
        size_t            lengthPercentage,
        int               baseSize,
        DBMSrvMsg_Error & error)
{
    bool rc=true;
    int  percentageAsInt;
    
    if(!cn36_StrNToInt(percentageAsInt, percentage, lengthPercentage))
    {
        rc=false;
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PERCENTVAL_ILLEGAL,
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_PERCENTVAL_ILLEGAL__PERCENTVALUE, percentage)));
    }
    else
    {
        m_Size=(int)((((double) percentageAsInt) / 100.0 ) * ((double) baseSize));

        rc=checkSize(error);
    }

    return rc;
}

int DBMSrvVol_VolumeSize::getSize()
{
    return m_Size;
}

bool DBMSrvVol_VolumeSize::checkSize(DBMSrvMsg_Error & error)
{
    if(0>=m_Size)
    {
        m_Size=0;
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_VOLSIZEILLEGAL,
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_VOLSIZEILLEGAL__SIZE, m_Size)));
        return false;
    }
    else
        return true;
}
