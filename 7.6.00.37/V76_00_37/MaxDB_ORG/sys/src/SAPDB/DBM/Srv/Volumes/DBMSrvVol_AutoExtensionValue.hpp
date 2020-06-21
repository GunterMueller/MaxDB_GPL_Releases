/*!
    \file    DBMSrvVol_AutoExtensionValue.hpp
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

#if !defined(DBMSRVVOL_AUTOEXTENSIONVALUE_HPP)
#define DBMSRVVOL_AUTOEXTENSIONVALUE_HPP


#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Volumes/DBMSrvVol_Volume.hpp"


/*! \brief class for getting automatic extension values for volumes

    Those values are used during db_addvolume. */
class DBMSrvVol_AutoExtensionValue
{
  public:
    /*! \brief enum for supported types of auto extension values */
    enum Type
    {
        Size,               //<! size of the volume
        Directory,          //<! directory for new volumes
        DirectoryMirrored   //<! directory for new mirrored volumes
    };

    /*! \brief constructor
        
        determines the value for the auto extension setting, by
        searching the DBM Server environment (dbm.cfg etc.)
        
        \param setting     [IN] the setting that is determined
        \param contentType [IN] DATA/LOG/Mirrored Log
        \param error       [OUT] a message list to report any errors */
    DBMSrvVol_AutoExtensionValue(
        DBMSrvVol_AutoExtensionValue::Type   setting,
        DBMSrvVol_Volume::ContentType        contentType,
        DBMSrvMsg_Error                    & error);

    /*! \brief constructor

        determines the value for the auto extension setting, by
        searching the DBM Server environment (dbm.cfg etc.)
        
        \param setting     [IN] the setting that is determined
        \param contentType [IN] DATA/LOG/Mirrored Log
        \param error       [OUT] a message list to report any errors
        \param successful  [OUT] true if no error occured, false otherwise */
    DBMSrvVol_AutoExtensionValue(
        DBMSrvVol_AutoExtensionValue::Type   setting,
        DBMSrvVol_Volume::ContentType        contentType,
        DBMSrvMsg_Error                    & error,
        bool                               & successful);

    /*! \brief destructor */
    ~DBMSrvVol_AutoExtensionValue();

    /*! get function for value
        \return the found auto extension setting as string */
    const char * getValue() const;

    /*! get function for name
        \return name of the auto extension value as string */
    const char * getName() const;

    /*! \brief get percentage */
    int getValueAsPercentage() const;

    /*! \brief function for checking the value
    
        \return true if value os empty string, false otherwise */
    bool isEmpty() const;

    /*! \brief checks if setting ends with a '%'
    
        \return true if value ends with a '%', false otherwise */
    bool endsWithPercentageSign() const;

    /*! \brief get position of last '%' in the value

        \return position of the last '%' in the value, or 0 of there is no '%'
                in value */
    size_t getPositionOfLastPercentageSign() const;

  private:
    char * m_Name;  //! name of the setting as character string
    char * m_Value; //! value of the setting as character string

    bool determineValue(
        DBMSrvVol_AutoExtensionValue::Type   setting,
        DBMSrvVol_Volume::ContentType        contentType,
        DBMSrvMsg_Error                    & error);
};

#endif
