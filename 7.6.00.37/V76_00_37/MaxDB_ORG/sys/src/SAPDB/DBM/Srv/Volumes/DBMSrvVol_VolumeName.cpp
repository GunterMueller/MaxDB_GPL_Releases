/*!
    \file         DBMSrvVol_VolumeName.cpp
    \author       TiloH
    \ingroup      volume handling by DBM Server
    \brief        A class for handling names of database volumes

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

#include "DBM/Srv/Volumes/DBMSrvVol_VolumeName.hpp"

DBMSrvVol_VolumeName::DBMSrvVol_VolumeName(
    const char * name,
    UseEscapeFlag escapeFlag)
  : m_Name(name)
{
    if(m_Name.isValid() && DBMSrvVol_VolumeName::UseEscaping==escapeFlag)
        removeEscapesInPlace(m_Name);
}

bool DBMSrvVol_VolumeName::setTo(
    const char * name,
    UseEscapeFlag escapeFlag)
{   
    if(m_Name.allocCopy(name) && DBMSrvVol_VolumeName::UseEscaping==escapeFlag)
        removeEscapesInPlace(m_Name);

    return m_Name.isValid();
}

const char * DBMSrvVol_VolumeName::getValue() const
{
    return m_Name;
}

void DBMSrvVol_VolumeName::removeEscapesInPlace(ToolsParsersUtil_String & s)
{
    size_t sourcePos=0;
    size_t targetPos=0;

    while('\0'!=s[sourcePos])
    {
        unsigned char character=s[sourcePos++]; //we use an unsigned char, because of following hex calculations

        if(DBMSrvVol_VolumeName::m_EscapeMark==character) //found a '#'
        {
            if(DBMSrvVol_VolumeName::m_EscapeMark==s[sourcePos]) //'#' escapes '#' -> "##" means "#"
                sourcePos++;                                     // just skip second '#', character has already value '#'
            else
                if(isxdigit(s[sourcePos]) && isxdigit(s[sourcePos+1]))
                {
                    unsigned char hexDigit1=ToolsParsersUtil_String::getHexValueFor(s[sourcePos]);
                    unsigned char hexDigit2=ToolsParsersUtil_String::getHexValueFor(s[sourcePos+1]);

                    character=(16*hexDigit1)+hexDigit2;

                    sourcePos+=2; //skip both hex values that followed the '#'
                }
        }

        if(0!=character) //do not insert escaped zero bytes ("...#00..."), just ignore them
            s[targetPos++]=character;
    }

    s[targetPos]='\0';
}

unsigned char DBMSrvVol_VolumeName::m_EscapeMark='#';
