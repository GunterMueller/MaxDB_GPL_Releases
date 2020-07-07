/*!
    \file         DBMSrvVol_VolumeName.hpp
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

#if !defined(DBMSRVVOL_VOLUMENAME_HPP)
#define DBMSRVVOL_VOLUMENAME_HPP

#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp"

/*! \brief class for handling names of database volumes

    One can decide wether, a supplied volume name can contain escaping
    sequences, or if it must be used as supplied (e.g. because it is read
    from the parameter file).
    
    The following historically grown escaping rules can be applied to the
    name string: If a '#' is followed by two hexadecimal digits ([0-9], [A-F]
    or [a-f] the character represented by those digits is replacing the hash
    mark '#' and the two hexadecimal digits. So "X#20Y" would escape into
    "X Y".
    
    If the two hexadecimal digits are zero ("#00") the escape sequence is
    erased from the string, so "X#00Y" transforms into "XY". */
class DBMSrvVol_VolumeName
{
  public:
    /*! \brief enumeration used for switching on/off the escaping rules*/
    enum UseEscapeFlag
    {
        UseEscaping,        //!< the name is scanned for escape sequances initiated by '#' 
        DoNotUseEscaping    //!< the name is stored as it as, on escaping rules are applied
    };

    /*! \brief constructor
    
        \param name       [IN] volume name
        \param escapeFlag [IN] switch for using escaping rules or not*/
    DBMSrvVol_VolumeName(
        const char * name,
        UseEscapeFlag escapeFlag=UseEscaping);

    /*! \brief function to change the volume name managed by the object
    
        \param name       [IN] new volume name
        \param escapeFlag [IN] switch for using escaping rules or not*/
    bool setTo(
        const char * name,
        UseEscapeFlag escapeFlag=UseEscaping);

    /*! \brief function for accessing the current volume name
        \return pointer to the current volume name, 0 if not enough memory was allocated */
    const char * getValue() const;

  private:
    void removeEscapesInPlace(ToolsParsersUtil_String & s);

    ToolsParsersUtil_String m_Name;

    static unsigned char m_EscapeMark;
};

#endif
