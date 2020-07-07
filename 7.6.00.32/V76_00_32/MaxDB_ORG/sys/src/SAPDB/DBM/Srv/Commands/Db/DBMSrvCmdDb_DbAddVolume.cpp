/*!
    \file    DBMSrvCmdDb_DbAddVolume.cpp
    \author  MarcW, TiloH
    \ingroup DBMServer commands
    \brief   definition of class for handling DBMServer
             command db_addvolume

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

*/

#include <stdlib.h>

#include "hcn40.h"
#include "hcn50.h" // cn50DBMUser
#include "hcn90.h"

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAddVolume.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"

DBMSrvCmdDb_DbAddVolume::DBMSrvCmdDb_DbAddVolume()
: DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false) //not obsolete
{
    m_longHelp=
    "@command db_addvolume With this command, you define additional volumes "
        "to extend the disk space for the database instance.<NL><NL>"

        "When doing so, you can specify either content type, size, name and file "
        "system type of the new volume or a target fill level for the volumes "
        "of a given content type or just the content type of the new volume. "

        "If you specify content type, size, name and file system type of the new "
        "volume, exactly this new volume is added. "

        "If you specify only the content type of a volume, size and name of the "
        "new volume are calculated and the volume is created as a file. "

        "If you use a target fill level for volumes of a given content type, "
        "file volumes are added until the current fill level drops below the "
        "target fill level.<NL><NL>"
               
        "If name and size of the new volume must be calculated, this is done "
        "as follows.<NL><NL>"
        
        "If the DBM Server parameter AutoExtDir (for data volumes), "
        "AutoExtDirLog (for log volumes) or AutoExtDirLogMirror (for mirrors "
        "of log volumes) is set and contains the name of an existing "
        "directory, the new volume is created there. Otherwise the new volume "
        "is created in the same directory where the last file volume of "
        "the same content type was added previously. If no such volume was "
        "added before the new volume is created in the rundirectoy of the "
        "database instance.<NL><NL>"

        "The new volume's name is created as follows. The name of the lastly added volume of the "
        "same content type is searched for the last occurence of it's index in it's name. If found, "
        "the name of the new volume is the name of the old volume, in which the old index is "
        "replaced with the new index. If the old index is not found, the new volume's name is the old volume's name "
        "with a dot and the new index appended.<NL><NL>"

        "The new volume's size in database pages is calculated as follows. "
        "If the DBM Server parameter AutoExtSize (for data volumes) or AutoExtSizeLog (for log and "
        "mirrored log volumes) is set to a number value, this value is taken as the new volume's size. If the "
        "parameter is set to a percent value (ends with a '%' character), the new volume's size "
        "is calculated as the given percentage of the sum of all existing volumes of the same content type. "
        "If the parameter is not set, the new volume's size is the size of the lastly added volume "
        "of the same content type."

    "@preconditions You have the server authorization ParamFull or AccessUtility."
    "@syntax db_addvolume <vol_mode> [-fd <filldegree>]|[<vol_name> <vol_type> <vol_size> [<mvol_name> <mvol_type>]]"
    "@param <vol_mode> Content type of the volume that is to be added. Possible values are:"
        "@value DATA \"\" Data Volume"
        "@value LOG \"\" Log Volume"
        "@param <filldegree> The actual filling degree is compared to <filldegree>. "
            "If it is bigger, new volumes are added, until the actual filling degree "
            "is smaller than <filldegree>. To be used only together with "
            "parameter <vol_mode> == DATA. No other parameters besides these two "
            "may be set. 1 <= <filldegree> <= 100 must be true. If this parameter is given, "
            "the number of added volumes is replied."
    "@param <vol_name> Name of volume that is to be added. If it's a relative path, the new volume "
        "is created relative to the database instance's rundirectory."
    "@param <vol_type> Type of volume that is to be added."
        "@value F \"File\" File"
        "@value L \"Link\" Link"
        "@value R \"Raw Device\" Raw Device"
    "@param <size> Size of the volume to be added in pages."
    "@param <mvol_name> Name of mirror volume that is to be added. Use only "
        "with <vol_mode> LOG and log mirroring activated. If it's a relative path, the new volume "
        "is created relative to the database instance's rundirectory."
    "@param <mvol_type> Type of mirror volume that is to be added. Use only "
        "with <vol_mode> LOG and log mirroring activated."
        "@value F \"File\" File"
        "@value L \"Link\" Link"
        "@value R \"Raw Device\" Raw Device"
    "@reply OK<NL>"
        "[<num_added>]"; 
}

tcn00_Error DBMSrvCmdDb_DbAddVolume::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    int volsAdded(0);
        
    // check number of arguments
    SAPDB_Int argCount(command->oArguments.getCount());
    int fillDegree(0);
    bool useFillDegree(argCount==3);
    bool needMore(true);
    tcn00_Error  retVal(OK_CN00);

    if( argCount != 1 && argCount != 3 && argCount != 4 && argCount != 6 ) {
        retVal =  theReply.startWithError(ERR_PARAM_CN00);
        getUsageText(theReply);
        return retVal;
    }
    
    if( useFillDegree ) {
        char szMode[PARAMETER_MAXLEN_CN90];
        char szSwitch[PARAMETER_MAXLEN_CN90];
        char szFillDegree[PARAMETER_MAXLEN_CN90];

        cn90GetToken(command->args, szMode, 1, PARAMETER_MAXLEN_CN90);
        cn90GetToken(command->args, szSwitch, 2, PARAMETER_MAXLEN_CN90);
        cn90GetToken(command->args, szFillDegree, 3, PARAMETER_MAXLEN_CN90);

        if( stricmp(szMode, XP_DEV_DAT_CN00 ) != 0 ) {
            retVal = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_ADDVOLWRONGMODE,Msg_Arg(SDBMSGTAG_DBMSRV_ADDVOLWRONGMODE__DBMSRV_COMP,szMode)));
        } else if( stricmp(szSwitch, m_FillDegreeParameterName) != 0 ) {
            retVal = theReply.startWithError(ERR_PARAM_CN00);
        } else {
            fillDegree=atoi(szFillDegree);
            if( fillDegree <= 0 || fillDegree > 100 ) {
                retVal = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_ADDVOLERRFDEGRANGE,Msg_Arg(SDBMSGTAG_DBMSRV_ADDVOLERRFDEGRANGE__DBMSRV_COMP,szFillDegree)));
            }
        }

        if( retVal != OK_CN00 ) {
            getUsageText(theReply);
            return retVal;
        }
    }

    // create lock object
    DBMSrvMsg_Error lockError;

    DBMSrvShM_LineAccessorResourceLock addVolumeLock(DBMSrvShM_InternalResource::AddVolume, lockError);
    if( !lockError.IsEmpty() )
        return theReply.startWithMessageList(lockError);

    // now try to lock resource "volume"
    DBMSrvShM_LineAccessorResourceLock::LockState lockState;
    lockState = addVolumeLock.lock(lockError);
    
    if( DBMSrvShM_LineAccessorResourceLock::LockStateLocked != lockState )
        return theReply.startWithMessageList(lockError);

    // resource is locked here
    do {
        bool dbFull(false);
        if( useFillDegree ) {
            cn50DBMUser       usrDBM    ( vcontrol->dbname, cn50DBMUser::getUser(vcontrol->dbname, FIRSTDBM_CN50));
            DBMSrvKnl_Session oSession;
            DBMSrvMsg_Error   oSessionError;

            if (!oSession.NormalConnect(vcontrol->dbname.asCharp(),
                                        usrDBM.getUserName().asCharp(),
                                        usrDBM.getClearMasterPwd().asCharp(),
                                        oSessionError)) {
                dbFull = (oSessionError == DBMSrvMsg_Error::DBMSrv_DATAFULL);
                if( !dbFull )                
                    retVal = theReply.startWithMessageList(oSessionError);
            } // end if
            if( retVal != OK_CN00 )
                break;

            if( !dbFull ) {
                DBMSrvKnl_Session::ResultType resType;
                if( oSession.Execute(
                    m_sqlStmtGetDataFillingPercentage,
                    resType,
                    oSessionError) ) {

                    DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
                    SAPDB_Int4 realFillDegree(oRecord.fieldAsInt(1));

                    needMore = (retVal == OK_CN00) && (realFillDegree > fillDegree);
                }
                else {
                    // error
                    needMore = false;
                    retVal = theReply.startWithMessageList(oSessionError);
                    break;
                }
            }
        }

        if( dbFull || needMore ) {
            CommandT newCommand;
            char newArguments[1024];
            SAPDB_strcpy(newArguments, command->args);
            newCommand.nIndex=CommandT::InvalidIndex;
            newCommand.setArgs(newArguments);
            retVal = cn40DbAddDevice(
                                    vcontrol,
                                    &newCommand, // must do this , because it's changed...
                                    theReply.giveData(),
                                    theReply.giveLength(),
                                    theReply.giveMaximalLength());
            theReply.adaptLength();
            if( retVal == OK_CN00)
                ++volsAdded;
        }
    } while ( retVal == OK_CN00 && useFillDegree && needMore );

    // try to unlock resource
    lockState = addVolumeLock.unlock(lockError);
    if( DBMSrvShM_LineAccessorResourceLock::LockStateNotLocked != lockState )
        retVal = theReply.startWithMessageList(lockError);

    if( useFillDegree ) {
        // put number of added volumes into reply
        char volsAddedBuf[128];
        sprintf(volsAddedBuf, "%d", volsAdded);
        theReply.appendLine(volsAddedBuf);
    }

    return retVal;
}

const char* const DBMSrvCmdDb_DbAddVolume::m_FillDegreeParameterName = "-fd";

const char* const DBMSrvCmdDb_DbAddVolume::m_sqlStmtGetDataFillingPercentage =
    "select \"Data            (%)\" from SYSDBM.INFO_STATE";
