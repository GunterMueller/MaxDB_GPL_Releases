/*!
    \file    DBMSrvCmdDb_DBDeleteVolume.cpp
    \author  TiloH
    \ingroup DBM Server commands, section DB
    \brief   definition of class for handling DBM Server command
             db_deletevolume

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

#include <stdlib.h>

#include "hcn40.h"
#include "hcn50.h"
#include "hcn90.h"

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DBDeleteVolume.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"

#include "DBM/Srv/Volumes/DBMSrvVol_Volume.hpp"


DBMSrvCmdDb_DBDeleteVolume::DBMSrvCmdDb_DBDeleteVolume()
: DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false ) //not obsolete
{
    m_longHelp=
    "@command db_deletevolume With this command, you remove one data volume. "
        "The data contained in this volume is distributed onto the other data "
        "volumes. If the volume was a file (volume type F), it is deleted from "
        "the file system. "
        "If you specify neither name nor number of the volume the data volume "
        "with the highest number is deleted."
    "@preconditions You have the server authorization ParamFull or AccessUtility."
    "@syntax db_deletevolume [DATA] [ NAME <vol_name> | [ID] <vol_no> ]"
    /*"@param <vol_mode> Mode of volume that is to be deleted. Possible values are:"
        "@value DATA \"\" Data Volume"*/
    "@param <vol_name> Name of volume. "
    "@param <vol_no> Number of the volume. "
    "@reply OK<NL>"; 
}

tcn00_Error DBMSrvCmdDb_DBDeleteVolume::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & reply)
{
    tcn00_Error rc=OK_CN00;

    //parse arguments
    int  argIndex=1;
    char arg[PARAMETER_MAXLEN_CN90+1]="";
    int  volumeID=0;
    char volumeName[PARAMETER_MAXLEN_CN90+1]="";

    if(cn90GetToken(command->args, arg, argIndex++, PARAMETER_MAXLEN_CN90))
    {
        if(0==cn90Stricmp(arg, XP_DEV_DAT_CN00)) // if found keyword DATA, just ignore it
            cn90GetToken(command->args, arg, argIndex++, PARAMETER_MAXLEN_CN90);

        if(0==cn90Stricmp(arg, "ID"))
        {
            if(!cn90GetToken(command->args, arg, argIndex++, PARAMETER_MAXLEN_CN90) || 0==atoi(arg))
                rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
            else
                volumeID=atoi(arg);

        }
        else if(0==cn90Stricmp(arg, "NAME"))
        {
            if(!cn90GetToken(command->args, volumeName, argIndex++, PARAMETER_MAXLEN_CN90))
                rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
        }
        else
        {
            if('\0'!=arg[0] && 0==atoi(arg))
                rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
            else
                volumeID=atoi(arg);
        }
    }

    if(rc==ERR_PARAM_CN00)
        getUsageText(reply);
    else
    {
        // create lock object
        DBMSrvMsg_Error lockError;

        DBMSrvShM_LineAccessorResourceLock addVolumeLock(DBMSrvShM_InternalResource::AddVolume, lockError);
        if( !lockError.IsEmpty() )
            return reply.startWithMessageList(lockError);

        // now try to lock resource "volume"
        DBMSrvShM_LineAccessorResourceLock::LockState lockState;
        lockState = addVolumeLock.lock(lockError);
        
        if( DBMSrvShM_LineAccessorResourceLock::LockStateLocked != lockState )
            return reply.startWithMessageList(lockError);

        // resource is locked here
        DBMSrvVol_Volume * vol=0;
        DBMSrvMsg_Error err;

        if('\0'==volumeName[0])
            vol=new DBMSrvVol_Volume(volumeID, DBMSrvVol_Volume::Data);
        else
            vol=new DBMSrvVol_Volume(volumeName, DBMSrvVol_Volume::Data);

        if(0==vol || !vol->isValid())
            rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        else
        {
            if(!vol->readCompleteParameters(err))
                rc=reply.startWithMessageList(err);
            else
            { 
                if(vol->hasVolumeModeArchive())
                    rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_VOLMODENOTALLOWED,
                            Msg_Arg(SDBMSGTAG_DBMSRV_VOLMODENOTALLOWED__VOLUME_MODE, vol->getVolumeModeAsString())));
                else
                {
                    if(!vol->deleteFromDB(err) ||
                        !vol->deleteFromParameters(err) ||
                        !vol->deleteFromFileSystem(err)   )
                    {
                        rc=reply.startWithMessageList(err);
                    }
                }
            }

            delete vol;
        }

        // try to unlock resource
        lockState = addVolumeLock.unlock(lockError);
        if( DBMSrvShM_LineAccessorResourceLock::LockStateNotLocked != lockState ) {
            err.Overrule(lockError);
            rc = reply.startWithMessageList(err);
        }
    }
    return rc;
}
