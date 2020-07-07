/*!
    \file         DBMSrvVol_Volume.hpp
    \author       TiloH
    \ingroup      volume handling by DBM Server
    \brief        A class for handling a database volume

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

#if !defined(DBMSRVVOL_VOLUME_HPP)
#define DBMSRVVOL_VOLUME_HPP


#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Volumes/DBMSrvVol_VolumeName.hpp"


/*! \brief class for handling a database volume

    \TODO introduce class DBMSrvVol_Volumes */
class DBMSrvVol_Volume
{
  public:
    /*! \brief enumeration for allowed content types of volumes */
    enum ContentType
    {
        Data,       //!< a data volume
        Log,        //!< a log volume
        MirroredLog //!< a mirrored log volume
    };

    /*! \brief enumeration of device types allowed for volumes */
    enum DeviceType
    {
        File,       //!< volume is a file
        RawDevice,  //!< volume is a raw device
        Link        //!< volume is provided as a link
    };

    enum Mode
    {
        Normal,     //!< volume is normal (not sequential aka archive volume)
        Archive     //!< volume is a "write once read many" device
    };

    /*! \brief constructor
        \param name        [IN] name of the volume
        \param contentType [In] content type of the volume */
    DBMSrvVol_Volume(
        const char                    * name,
        DBMSrvVol_Volume::ContentType   contentType);

    /*! \brief constructor
        \param volumeIndex [IN] number of the volume within the parameter file
        \param contentType [In] content type of the volume */
    DBMSrvVol_Volume(
        int volumeIndex,
        DBMSrvVol_Volume::ContentType contentType);

    /*! \brief check state of object
        \return true if enough memory was allocated in constructor, false otherwise */
    bool isValid() const;

    /*! \brief function droping a volume from the database
        \param error [OUT] a message list with error information
        \return true if successful, false otherwise */
    bool deleteFromDB(DBMSrvMsg_Error & error);

    /*! \brief function for deleting a volume from the parameter file
        \param error [OUT] a message list with error information
        \return true if successful, false otherwise */
    bool deleteFromParameters(DBMSrvMsg_Error & error);

    /*! \brief function for deleting a file volume from the file system

        \param error [OUT] a message list with error information
        \return true if successful or volume has other device type
            than file, false otherwise */
    bool deleteFromFileSystem(DBMSrvMsg_Error & error);

    /*! \brief function for reading missing volume parameters from the
        parameter file

        \param error [OUT] a message list with error information
        \return true if successful, false otherwise */
    bool readCompleteParameters(DBMSrvMsg_Error & error);

    /*! \brief function checking for volume mode Archive (stored as SEQUENTIAL)
        \return true if volume has volume mode Archive, false otherwise*/
    bool hasVolumeModeArchive() const;

    /*! \brief function for getting the volumes device type as string
        \return 0 terminated string, representing the device type */
    const char * getDeviceTypeAsString() const;

    /*! \brief function for getting the volume's mode as string
        \return 0 terminated string, representing the mode */
    const char * getVolumeModeAsString() const;

    /*! \brief static function for determining a content type enum for a given
        string
        
        \param contentTypeString [IN]  volume content type as string
        \param contentType       [OUT] volume content type as enum
        \return true if string represents a valid volume content type, false
                otherwise */
    static bool getContentType(
        const char                    * contentTypeString,
        DBMSrvVol_Volume::ContentType & contentType);

  private:
    ContentType          m_ContentType;
    DeviceType           m_DeviceType;
    Mode                 m_Mode;
    DBMSrvVol_VolumeName m_Name;
    DBMSrvVol_VolumeName m_AbsoluteName;
    int                  m_Index; //! index of the volume in the parameter file, 0 is the invalid index
};

#endif
