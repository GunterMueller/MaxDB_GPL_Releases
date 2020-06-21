/*!
  \file    DBMSrvBMedia_Medium.hpp
  \author  TiloH
  \ingroup backup media handling by the DBMServer
  \brief   declaring a class for handling backup non parallel media

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#if !defined(DBMSRVBMEDIA_MEDIUM_HPP)
#define DBMSRVBMEDIA_MEDIUM_HPP

#include "gcn003.h"

/*! \brief class for future handling of non parallel media */
class DBMSrvBMedia_Medium
{
  public:
    /*! \brief add rundirectory to relative media locations of file media
        \param medium [IN/OUT] medium structure that contains the relative
            location and is changed by the function
        \param error  [OUT] error list (overruled in case of errors)
        \return true if successful, false otherwise */
    static bool makeLocationAbsolute(
        tcn003_Medium   * medium,
        DBMSrvMsg_Error & error);

    /*! \brief calculate default page size of the software release
        \param pageSize [OUT] default page size in byte
        \param error    [OUT] error list (overruled in case of errors)
        \return true if successful, false otherwise */
    static bool getDefaultPageSizeInByte(
        size_t          & pageSize,
        DBMSrvMsg_Error & error);

    /*! \brief calculate default backup block size of the software release
        \param blockSize [OUT] default backup block size in byte
        \param error     [OUT] error list (overruled in case of errors)
        \return true if successful, false otherwise */
    static bool getDefaultBlockSizeInByte(
        size_t          & blockSize,
        DBMSrvMsg_Error   error);

    /*! \brief calculate backup block size for a given backup medium
        \param blockSize [OUT] backup block size defined by backup medium in
            byte
        \param medium    [IN]  the medium defining the blocksize
        \param error     [OUT] error list (overruled in case of errors)
        \return true if successful, false otherwise */
    static bool getBlocksizeInByte(
        size_t              & blockSize,
        const tcn003_Medium & medium,
        DBMSrvMsg_Error       error);
};

#endif
