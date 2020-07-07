/*!
  \file    DBMSrvCmdMedium_MediumLabelOffline.cpp
  \author  TiloH
  \ingroup DBM Server commands
  \brief   implementation of a classes handling DBM Server
           command medium_put

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2006 SAP AG

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

#include "hcn30.h"
#include "hcn31.h"
#include "hcn36.h"
#include "hcn90.h"

#include "DBM/Srv/Backup/Label/DBMSrvBLabel_Label.hpp"
#include "DBM/Srv/Backup/Media/DBMSrvBMedia_DeviceType.hpp"
#include "DBM/Srv/Backup/Media/DBMSrvBMedia_Medium.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumLabelOffline.hpp"
#include "DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"

DBMSrvCmdMedium_MediumLabelOffline::DBMSrvCmdMedium_MediumLabelOffline()
    :DBMSrv_Command(DBMSrv_Command::KeyUsingCommandFactory, false)
{
    m_longHelp=
        "@command medium_labeloffline You display the information stored on a "
            "backup medium about the backup. The backup medium is accessed "
            "directly. This means that the data is displayed regardless of the "
            "database state."
        "@preconditions You have the DBM operator authorization Backup."
        "@syntax medium_labeloffline <medium> [<nnn>] | <location> "
            "[[DEVICETYPE] FILE|TAPE|PIPE ] BLOCKSIZE <block_size> "
            "@param <medium> "
                "The name of the backup medium that is read by the command."
            "@param <nnn> "
                "If <medium> is a log backup file, option <nnn> can be used to "
                "specify the version number of the log backup file. "
            "@param <location> "
                "The name of the backup device or file with which the backup "
                "can be addressed through operating system functions. "
            "@param <block_size> "
                "Number of pages that are transferred when accessing the "
                "medium once. "
        "@reply OK";
}

tcn00_Error DBMSrvCmdMedium_MediumLabelOffline::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & reply)
{
    tcn00_Error rc=OK_CN00;
    char        mediumName[ME_NAME_LENGTH_MXCN003+1]="";
    char        originalLocation[ME_LOCATION_LENGTH_MXCN003+1]="";
    char        location[ME_LOCATION_LENGTH_MXCN003+1]="";
    char        parameter[256]="";
    char        fileNumber[256]="";
    char        blockSizeAsString[256]="";
    size_t      blockSize=0;
    bool        useMediumName=true;

    DBMSrvBMedia_DeviceType deviceType=DBMSrvBMedia_DeviceType::Unknown;

    if(cn90GetToken(command->args, parameter, 2, 255) &&                              // we have 2 arguments and the second one is
       (0==stricmp(parameter, "BLOCKSIZE") ||                                         // blocksize,
        0==stricmp(parameter, "DEVICETYPE") ||                                        // devicetype or
        (deviceType.setTo(parameter), DBMSrvBMedia_DeviceType::Unknown!=deviceType))) // one of the known device types
    {
        useMediumName=false;

        if(!cn90GetToken(command->args, location, 1, ME_LOCATION_LENGTH_MXCN003))
            rc=reply.startWithError(ERR_PARAM_CN00);
        else
            cn36_RemoveOuterDoubleQuotes(location);

        int i=2;

        while(OK_CN00==rc && cn90GetToken(command->args, parameter, i, 255))
        {
            if(0==stricmp(parameter, "BLOCKSIZE"))
            {
                if(!cn90GetToken(command->args, blockSizeAsString, ++i, 255))
                    rc=reply.startWithError(ERR_PARAM_CN00);
            }
            else
                if(0!=stricmp(parameter, "DEVICETYPE"))
                {
                    deviceType.setTo(parameter);

                    if(DBMSrvBMedia_DeviceType::Unknown==deviceType)
                        rc=reply.startWithError(ERR_PARAM_CN00);
                }

            i++;
        }

        if(OK_CN00==rc) // got valid parameters?
        {
            DBMSrvMsg_Error error;

            SAPDB_strcpy(originalLocation, location);

            if(0<atoi(blockSizeAsString)) //allow only non negative block sizes
            {   
                size_t pageSize;

                if(DBMSrvBMedia_Medium::getDefaultPageSizeInByte(pageSize, error))
                    blockSize=pageSize*atoi(blockSizeAsString);
                else
                    rc=reply.startWithMessageList(error);
            }
            else
            {
                if(!DBMSrvBMedia_Medium::getDefaultBlockSizeInByte(blockSize, error))
                    rc=reply.startWithMessageList(error);
            }
        }
    }
    else // classical medium_labeloffline
    {
        if(cn90GetToken(command->args, mediumName, 1, ME_NAME_LENGTH_MXCN003))
        {
            cn36_RemoveOuterDoubleQuotes(mediumName);
            cn90GetToken(command->args, fileNumber, 2, 255);

            if('\0'==vcontrol->dbname)
            {
                rc=reply.startWithError(ERR_PARAM_CN00);
                reply.appendMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBNAME));
            }
        }
        else
            rc=reply.startWithError(ERR_PARAM_CN00);

        if(OK_CN00==rc) // got valid parameters?
        {
            tcn003_Media media;

            rc=cn30ReadMedia(vcontrol->dbname, reply.giveData(), reply.giveLength(), reply.giveMaximalLength(), &media);

            if(OK_CN00==rc)
            {
                tcn003_Medium     * pMedium         =0;
                tcn003_MediaGroup * pMediaGroupFound=0;

                rc=cn30FindMediumByName(reply.giveData(), reply.giveLength(), &media, mediumName, false, &pMediaGroupFound, &pMedium, true);

                reply.adaptLength();

                if(OK_CN00==rc && 0!=pMedium)
                {
                    DBMSrvMsg_Error error;

                    if('\0'!=fileNumber[0])
                    {
                        strcat(pMedium->szLocation, ".");
                        strcat(pMedium->szLocation, fileNumber);
                    }

                    cn36_StrNCpy(originalLocation, pMedium->szLocation, ME_LOCATION_LENGTH_MXCN003);

                    if(DBMSrvBMedia_Medium::makeLocationAbsolute(pMedium, error))
                    {
                        SAPDB_strcpy(location, pMedium->szLocation);
                        deviceType.setTo(pMedium->szType);

                        if(!DBMSrvBMedia_Medium::getBlocksizeInByte(blockSize, *pMedium, error))
                            rc=reply.startWithMessageList(error);
                    }
                    else
                        rc=reply.startWithMessageList(error);
                }
            }

            cn30FreeMedia(&media);
        }
    }

    if(OK_CN00==rc)
    {
        DBMSrvBLabel_Label label;

        if(label.readLabelFrom(
                        location,
                        blockSize,
                        deviceType,
                        reply.giveData(),
                        reply.giveLength(),
                        reply.giveMaximalLength()))
        {
            tcn003_BackupResult info;

            cn31_InitResult(&info);

            cn36_StrNCpy(info.szDate,           label.getStartDate(),           sizeof(info.szDate)-1);
            cn36_StrNCpy(info.szTime,           label.getStartTime(),           sizeof(info.szTime)-1);
            cn36_StrNCpy(info.szDatabase,       label.getDB(),                  sizeof(info.szDatabase)-1);
            cn36_StrNCpy(info.szServer,         label.getServer(),              sizeof(info.szServer)-1);
            cn36_StrNCpy(info.szKernelVersion,  label.getKernelVersion(),       sizeof(info.szKernelVersion)-1);
//                       info.szPagesTransfered
//                       info.szPagesLeft
//                       info.szVolumesCount
//                       info.szMediaName
            SAPDB_strcpy(info.szLocation,        originalLocation);
//                       info.szErrortext
            cn36_StrNCpy(info.szLabel,           label.getLabel(),              sizeof(info.szLabel)-1);
            cn36_StrNCpy(info.szIsConsistent,    label.getIsConsistent(),       sizeof(info.szIsConsistent)-1);
            cn36_StrNCpy(info.szFirstLogPageNo,  label.getFirstLogPage(),       sizeof(info.szFirstLogPageNo)-1);
            cn36_StrNCpy(info.szLastLogPageNo,   label.getLastLogPage(),        sizeof(info.szLastLogPageNo)-1);
            cn36_StrNCpy(info.szDBStamp1Date,    label.getDBStamp1Date(),       sizeof(info.szDBStamp1Date)-1);
            cn36_StrNCpy(info.szDBStamp1Time,    label.getDBStamp1Time(),       sizeof(info.szDBStamp1Time)-1);
            cn36_StrNCpy(info.szDBStamp2Date,    label.getDBStamp2Date(),       sizeof(info.szDBStamp2Date)-1);
            cn36_StrNCpy(info.szDBStamp2Time,    label.getDBStamp2Time(),       sizeof(info.szDBStamp2Time)-1);
            cn36_StrNCpy(info.szBDPageCount,     label.getPageCount(),          sizeof(info.szBDPageCount)-1);
            cn36_StrNCpy(info.szDevicesUsed,     label.getNumberDevices(),      sizeof(info.szDevicesUsed)-1);
            cn36_StrNCpy(info.szDatabaseID,      label.getDatabaseID(),         sizeof(info.szDatabaseID)-1);
            cn36_StrNCpy(info.szMaxUsedDataPage, label.getMaxUsedDataPage(),    sizeof(info.szMaxUsedDataPage)-1);
            cn36_StrNCpy(info.szConvPageCnt,     label.getConverterPageCount(), sizeof(info.szConvPageCnt)-1);

            cn31_PrintBackupAnswer(reply.giveData(), &info, false);
            reply.adaptLength();
        }
    }

    return rc;
}
