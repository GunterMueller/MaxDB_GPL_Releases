/*!
    \file         DBMSrvVol_VolumeSize.hpp
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

#if !defined(DBMSRVVOL_VOLUMESIZE_HPP)
#define DBMSRVVOL_VOLUMESIZE_HPP


#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"


/*! \brief class for handling size of a database volume */
class DBMSrvVol_VolumeSize
{
  public:
    /*! \brief constructor
        object is initialised with illegal size 0, use setTo() function to
        change this */
    DBMSrvVol_VolumeSize();

    /*! \brief constructor
        \param size  [IN] size of the volume in pages
        \param error [IN] the usual error list */
    DBMSrvVol_VolumeSize(
        int               size,
        DBMSrvMsg_Error & error);

    /*! \brief constructor
        \param size  [IN] size of the volume in pages
        \param error [IN] the usual error list */
    DBMSrvVol_VolumeSize(
        const char      * size,
        DBMSrvMsg_Error & error);

    /*! \brief constructor

        This constructor calculates the actual size in relation to another size
        (size=percantage/100*baseSize).

        \param percentage [IN] size as percentage value of baseSize
        \param lengthPercentage [IN] length of the percentage string
        \param baseSize   [IN] size that is used as basis of the calculation
        \param error      [IN] the usual error list */
    DBMSrvVol_VolumeSize(
        const char      * percentage,
        size_t            lengthPercentage,
        int               otherSize,
        DBMSrvMsg_Error & error);

    /*! \brief set function
        \param size  [IN] size of the volume in pages
        \param error [IN] the usual error list */
    bool setTo(
        const char      * size,
        DBMSrvMsg_Error & error);

    /*! \brief set function

        This function calculates the actual size in relation to another size
        (size=percantage/100*baseSize).

        \param percentage [IN] size as percentage value of baseSize
        \param lengthPercentage [IN] length of the percentage string
        \param baseSize   [IN] size that is used as basis of the calculation
        \param error      [IN] the usual error list */
    bool setTo(
        const char      * percentage,
        size_t            lengthPercentage,
        int               baseSize,
        DBMSrvMsg_Error & error);

    /*! get function
        \return the size as integer */
    int getSize();

  private:
    int m_Size; //! the size in pages

    bool checkSize(DBMSrvMsg_Error & error);
};

#endif
