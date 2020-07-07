/*!
    \file         DBMSrvVol_Volumes.hpp
    \author       TiloH
    \ingroup      volume handling by DBM Server
    \brief        A class for accessing database volumes

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

#if !defined(DBMSRVVOL_VOLUMES_HPP)
#define DBMSRVVOL_VOLUMES_HPP


#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Volumes/DBMSrvVol_Volume.hpp"
#include "DBM/Srv/Volumes/DBMSrvVol_VolumeSize.hpp"


/*! \brief class for finding/handling/creating database volumes */
class DBMSrvVol_Volumes
{
  public:
    /*! \brief function for "calculating" a new volume

        this function bundles the calculation of a volumes name and size, that
        is executed, when db_addvolume is called with name and size missing

        \param index       [IN] the index of the new volume in the parameter
            file
        \param contentType [IN] type data that will be stored on the volume
            (DATA or LOG)
        \param calculateMirror [IN] true if name of mirror must be calculated
            as well
        \param sizeOfAllCurrentVolumes [IN] size of all volumes currently known
            in the parameter file
        \param name        [OUT] pointer to new name, space allocated by operator new
        \param nameMirror  [OUT] if requested, pointer to name of mirror volume
        \param size        [OUT] calculated volume size in pages
        \param error       [OUT] the usual error list
        \return true if no problems occured, false otherwise */

    static bool calculateNameAndSizeOfNewVolume(
        int                              index,
        DBMSrvVol_Volume::ContentType    contentType,
        bool                             calculateMirror,
        int                              sizeOfAllCurrentVolumes,
        char                          *& name,
        char                          *& nameMirror,
        DBMSrvVol_VolumeSize           & size,
        DBMSrvMsg_Error                & error);

  private:
    static bool getIndexNameAndSizeOfLastAddedVolume(
        DBMSrvVol_Volume::ContentType    contentType,
        bool                             isMirrored,
        int                            & index,
        char                          *& name,
        char                          *& nameMirror,
        char                          *& size,
        char                          *& previousFileVolumeName,
        char                          *& previousMFileVolumeName,
        DBMSrvMsg_Error                & error);

    static bool allocCopyOnce(
        bool            & copiedBefore,
        char           *& string,
        const char      * original,
        DBMSrvMsg_Error & error);

    /*! brief function getting/checking for index in parameter name

        Function checks, if index matches the number in string. If index is
        0 it matches any number and is changed to that value.

        \return true, if index matches string (index 0 matches every number), false otherwise */
    static bool getOrCheckIndex(
        int        & index,
        const char * numberInParameterName);
    static bool checkIndex(
        int          index,
        const char * numberInParameterName);

    static bool calculateNextVolumeName(
        const char      *  autoExtensionDir,
        int                newIndex,
        int                previousIndex,
        const char      *  previousVolumeName,
        const char      *  previousFileVolumeName,
        char            *& nextVolumeName,
        DBMSrvMsg_Error  & error);

    static bool calculateSize(
        DBMSrvVol_Volume::ContentType    contentType,
        const char                    *  previousSize,
        int                              sizeOfAllCurrentVolumes,
        DBMSrvVol_VolumeSize           & size,
        DBMSrvMsg_Error                & error);
};

#endif
