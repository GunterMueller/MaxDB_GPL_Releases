/*!
    \file    DBMSrvVol_AutoExtensionValue.cpp
    \author  TiloH
    \ingroup volume handling by DBM Server
    \brief   class for determine the automatic extension values (directories
             and size) for volumes

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
#include "hcn51.h"
#include "DBM/Srv/Volumes/DBMSrvVol_AutoExtensionValue.hpp"

DBMSrvVol_AutoExtensionValue::DBMSrvVol_AutoExtensionValue(
    DBMSrvVol_AutoExtensionValue::Type   setting,
    DBMSrvVol_Volume::ContentType        contentType,
    DBMSrvMsg_Error                    & error)
  : m_Name(0),
    m_Value(0)
{
    determineValue(setting, contentType, error);
}

DBMSrvVol_AutoExtensionValue::DBMSrvVol_AutoExtensionValue(
    DBMSrvVol_AutoExtensionValue::Type   setting,
    DBMSrvVol_Volume::ContentType        contentType,
    DBMSrvMsg_Error                    & error,
    bool                               & successful)
  : m_Name(0),
    m_Value(0)
{
    successful=determineValue(setting, contentType, error);
}

DBMSrvVol_AutoExtensionValue::~DBMSrvVol_AutoExtensionValue()
{
    cn36_StrDealloc(m_Name);
    cn36_StrDealloc(m_Value);
}

const char * DBMSrvVol_AutoExtensionValue::getValue() const
{
    return 0==m_Value?"":m_Value;
}

const char * DBMSrvVol_AutoExtensionValue::getName() const
{
    return 0==m_Value?"":m_Name;
}

bool DBMSrvVol_AutoExtensionValue::isEmpty() const
{
    return (0==m_Value || '\0'==m_Value[0]);
}

bool DBMSrvVol_AutoExtensionValue::endsWithPercentageSign() const
{
    bool rc=false;

    if(0!=m_Value)
    {
        const char * lastPercentageSign=strrchr(m_Value, '%');

        if(0!=lastPercentageSign && lastPercentageSign+1==cn36_StartOfTrailingWhiteSpaces(lastPercentageSign)) //there is a '%' and it is followed only by spaces
            rc=true;
    }

    return rc;
}

size_t DBMSrvVol_AutoExtensionValue::getPositionOfLastPercentageSign() const
{
   if(0!=m_Value)
   {
       const char * lastPercentageSign=strrchr(m_Value, '%');

       if(0!=lastPercentageSign)
           return lastPercentageSign-m_Value;
       else
           return 0;
   }
   else
       return 0;
}

bool DBMSrvVol_AutoExtensionValue::determineValue(
    DBMSrvVol_AutoExtensionValue::Type   setting,
    DBMSrvVol_Volume::ContentType        contentType,
    DBMSrvMsg_Error                    & error)
{
    bool rc=true;

    if(DBMSrvVol_AutoExtensionValue::Size==setting)
    {
        tsp00_Namec keyAutoExtSize=(DBMSrvVol_Volume::Data==contentType)?CFG_AUTOEXTSIZE_CN51:CFG_AUTOEXTSIZELOG_CN51;

        if(!cn36_StrAllocCpy(m_Name, keyAutoExtSize.asCharp()))
            rc=false;
        else
        {
            cn51DBMConfigValue autoExtSize(cn00DBMServerData::vcontrol()->dbname, keyAutoExtSize);

            if(!cn36_StrAllocCpy(m_Value, ((const char *)autoExtSize)))
                rc=false;
        }
    }
    else
    {
        tsp00_Namec keyAutoExtDir;

        if(DBMSrvVol_Volume::Data==contentType)
            keyAutoExtDir=CFG_AUTOEXTDIR_CN51;
        else
        {
            if(DBMSrvVol_AutoExtensionValue::DirectoryMirrored==setting)
                keyAutoExtDir=CFG_AUTOEXTDIRMLOG_CN51;
            else
                keyAutoExtDir=CFG_AUTOEXTDIRLOG_CN51;
        }

        if(!cn36_StrAllocCpy(m_Name, keyAutoExtDir.asCharp()))
            rc=false;
        else
        {
            cn51DBMConfigValue cfgAutoExtDir(cn00DBMServerData::vcontrol()->dbname, keyAutoExtDir);

            if(!cn36_StrAllocCpy(m_Value, ((const char *)cfgAutoExtDir)))
                rc=false;
        }
    }

    if(!rc)
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));

    return rc;
}
