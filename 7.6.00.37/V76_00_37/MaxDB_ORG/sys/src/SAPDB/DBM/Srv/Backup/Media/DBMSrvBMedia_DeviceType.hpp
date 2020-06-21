/*!
  \file    DBMSrvBMedia_DeviceType.hpp
  \author  TiloH
  \ingroup backup media handling by the DBM Server
  \brief   declaring a class for differentiating device types

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

#if !defined(DBMSRVBMEDIA_DEVICETYPE_HPP)
#define DBMSRVBMEDIA_DEVICETYPE_HPP

/*! \brief class for managing device types */
class DBMSrvBMedia_DeviceType
{
  public:
    enum DeviceType
    {
        File,   //!< used for files
        Pipe,   //!< used for named pipes
        Tape,   //!< used for tapes
        Unknown //!<use if file type is unknown
    };

    /*! \brief constructor */
    DBMSrvBMedia_DeviceType(const DBMSrvBMedia_DeviceType::DeviceType deviceType);

    /*! \brief get device type as string
        \return device type as C string */
    const char * getAsString() const;

    /*! \brief operator ==
        \param otherDeviceType [IN] device type to compare to
        \return true if otherDeviceType equals *this, false otherwise */
    bool operator == (const DBMSrvBMedia_DeviceType::DeviceType otherDeviceType) const;

    /*! \brief set *this to a new device type
        \param deviceType [IN] the new device type */
    void setTo(const char * deviceType);

  private:
    DeviceType m_DeviceType; //!< storage for the actual device type
};

/*! \brief compare two device types
    \param deviceType1 [IN] 1st device type as basic enum
    \param deviceType2 [IN] 2nd device type as object
    \return true if both types are equal, false otherwise */
bool operator == (
        const DBMSrvBMedia_DeviceType::DeviceType deviceType1,
        const DBMSrvBMedia_DeviceType             deviceType2);

/*! \brief compare two device types
    \param deviceType1 [IN] 1st device type as basic enum
    \param deviceType2 [IN] 2nd device type as object
    \return false if both types are equal, true otherwise */
bool operator != (
        const DBMSrvBMedia_DeviceType::DeviceType deviceType1,
        const DBMSrvBMedia_DeviceType             deviceType2);

#endif
