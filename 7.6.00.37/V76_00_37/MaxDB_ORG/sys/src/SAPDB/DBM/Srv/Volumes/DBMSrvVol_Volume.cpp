/*!
    \file         DBMSrvVol_Volume.cpp
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

#include "hsp77.h"
#include "gcn00.h"
#include "gcn002.h"
#include "hcn20.h"
#include "hcn36.h"
#include "hcn46.h"
#include "hcn50.h"

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/Volumes/DBMSrvVol_Volume.hpp"
#include "DBM/Srv/Volumes/DBMSrvVol_VolumeName.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"


DBMSrvVol_Volume::DBMSrvVol_Volume(
    const char                    * name,
    DBMSrvVol_Volume::ContentType   contentType)
  : m_ContentType(contentType),
    m_DeviceType(DBMSrvVol_Volume::File),
    m_Mode(DBMSrvVol_Volume::Normal),
    m_Name(name),
    m_AbsoluteName(name),
    m_Index(0)
{
}

DBMSrvVol_Volume::DBMSrvVol_Volume(
    int                           volumeIndex,
    DBMSrvVol_Volume::ContentType contentType)
  : m_ContentType(contentType),
    m_DeviceType(DBMSrvVol_Volume::File),
    m_Mode(DBMSrvVol_Volume::Normal),
    m_Name(""),
    m_AbsoluteName(""),
    m_Index(volumeIndex)
{
}

bool DBMSrvVol_Volume::isValid() const
{
    return (0!=m_Name.getValue() && 0!=m_AbsoluteName.getValue());
}

bool DBMSrvVol_Volume::deleteFromDB(DBMSrvMsg_Error & error)
{
    bool rc=true;

    // checking dbstate
    DBMSrvKnl_State::StateValue kernelState=DBMSrvKnl_State::State(cn00DBMServerData::vcontrol()->dbname);

    if(kernelState!=DBMSrvKnl_State::StateOnline)
    {
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBSTATENEEDED1, Msg_Arg("STATE", DBMSrvKnl_State::getStateAsString(DBMSrvKnl_State::StateOnline))));
        error.MarkAsSubMessages();
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_WRONGDBSTATE, Msg_Arg("STATE", DBMSrvKnl_State::getStateAsString(kernelState))));

        return false;
    }

    // build kernel command
    SAPDB_ToStringClass indexAsString(m_Index);
    char * kernelCommand=0;

    if(cn36_StrAlloc(kernelCommand, strlen("DROP DATA VOLUME ")+strlen((const char *)indexAsString)))
    {
        sprintf(kernelCommand,
            "DROP %s VOLUME %s",
            DBMSrvVol_Volume::Data==m_ContentType?"DATA":"LOG",
            (const char *)indexAsString);

        // execute kernel command
        cn50DBMUser user(cn00DBMServerData::vcontrol()->dbname, cn50DBMUser::getColdUser(cn00DBMServerData::vcontrol()->dbname));

        if(!DBMSrvKnl_Session::Execute(
                cn00DBMServerData::vcontrol()->dbname.asCharp(),
                user.getUserName().asCharp(),
                user.getClearMasterPwd().asCharp(),
                kernelCommand,
                error))
        {
            rc=false;
        }

        cn36_StrDealloc(kernelCommand);
    }
    else
    {
        rc=false;
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
    }

    return rc;
}

bool DBMSrvVol_Volume::deleteFromParameters(DBMSrvMsg_Error & error)
{
    bool rc=false;
    DBMSrvInfrReply_ReplyBufferWritingIntoMsgList reply(error);

    if(reply.isValid())
    {
        SAPDB_ToStringClass indexAsString(m_Index);
        char * newArguments=0;

        if(cn36_StrAlloc(newArguments, strlen((const char *)indexAsString)+strlen(" DATA")))
        {
            CommandT newCommand;

            sprintf(newArguments, "%s DATA", (const char *)indexAsString);

            newCommand.nIndex=CommandT::InvalidIndex;
            newCommand.setArgs(newArguments);
        
            if(OK_CN00==cn20ParamDelDevSpace(
                                cn00DBMServerData::vcontrol(),
                                &newCommand, // must do this , because it's changed...
                                reply.getData(),
                                reply.getLength(),
                                reply.getMaximalLength()))
            {
                rc=true;
            }
            else
                reply.fillMessageList();

            cn36_StrDealloc(newArguments);
        }
        else
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
    }

    return rc;
}

bool DBMSrvVol_Volume::deleteFromFileSystem(DBMSrvMsg_Error & error)
{
    bool rc=false;

    if(DBMSrvVol_Volume::File==m_DeviceType &&
       0!=m_AbsoluteName.getValue() &&
       '\0'!=(*m_AbsoluteName.getValue()))
    {
        tni34_AVirtualFile file(m_AbsoluteName.getValue(),
                                tni34_AVirtualFile::ToDiskTruncate,
                                tni34_AVirtualFile::DoNotOpenNow);

        if(!file.IsGood())
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        else
            if(!file.Remove())
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEDEL, Msg_Arg(SDBMSGTAG_DBMSRV_FILEDEL__FILENAME, file.GiveName()))); //dirty, but vni34.cpp must be transformed anyway
            else
                rc=true;
    }
    /*
    else if(DBMSrvVol_Volume::Link==m_DeviceType && 0!=m_AbsoluteName.getValue() && '\0'!=(*m_AbsoluteName.getValue())) {
        // if it's a link we remove just the link, not the link target
        
        tsp05_RteFileInfo  rteInfo;
        tsp05_RteFileError rteError;
        sqlfinfoc(m_AbsoluteName.getValue(), &rteInfo, &rteError);

        if( vf_ok==rteError.sp5fe_result && rteInfo.sp5fi_exists ) { //&& rteInfo.sp5fi_is_link
            // it exists, try to delete it
            sqlferasec( m_AbsoluteName.getValue(), &rteError);
            rc = (vf_ok==rteError.sp5fe_result);
        }

        if( !rc ) {
            error.Overrule(DBMSrvMsg_RTEError(rteError));
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEDEL,
                    Msg_Arg(SDBMSGTAG_DBMSRV_FILEDEL__FILENAME, m_AbsoluteName.getValue())));
        }
    }
    */
    else
        rc=true;

    return rc;
}

bool DBMSrvVol_Volume::readCompleteParameters(DBMSrvMsg_Error & error)
{
    bool rc=true;
    DBMSrvPar_Parameters * kernelParameters=0;
    DBMSrvPar_Parameter * par=0;
    DBMSrvPar_Parameter * runDir=0;
    int numberOfDataVolumes=0;

    bool useIndex=(0!=m_Index);
    bool useName=(0!=m_Name.getValue() && 0!=strlen(m_Name.getValue()));

    // find number of next volume for the requested mode
    // find key for parameter table

    tcn00_DevspaceKeywords aKeyTable  [] = XP_DEVSPACE_KEYWORDS_CN00;
    tcn00_DevspaceParams   aParamTable[] = XP_DEVSPACE_PARAMS_CN00;
    int nKey=0;
    int nParam=0;

    for (nKey = 0; stricmp(aKeyTable[nKey].szText, "DATA") != 0; nKey++);

    if (aKeyTable[nKey].nDevSpace == XPDevSpaceUnknown_ecn00)
    {
        rc=false;
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));
    }
    else
    {
        // search in paramtable with that key
        for (nParam = 0; aKeyTable[nKey].nDevSpace != aParamTable[nParam].nDevSpace; nParam++);

        kernelParameters=DBMSrvPar_Parameters::GetInstance(cn00DBMServerData::vcontrol()->dbname, true, error);

        if(0==kernelParameters)
            rc=false;
    }

    //get maximum number of data volumes
    if(rc)
    {
        par=kernelParameters->Get(aParamTable[nParam].szMaxCount);

        if(0==par || 0>=strlen(par->szOldValue.asCharp()) || 0==atoi(par->szOldValue.asCharp()))
        {
            rc=false;
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPVALUNEX,
                                           Msg_Arg("VALUE", (0==par)?"": par->szOldValue.asCharp()),
	                                       Msg_Arg("DATABASE_PARAMETER", aParamTable[nParam].szMaxCount)));
        }
        else
            numberOfDataVolumes=atoi(par->szOldValue.asCharp());
    }

    if(rc)
    {
        runDir=kernelParameters->Get(PAN_RUNDIR);

        if(0==runDir || 0>=strlen(runDir->szOldValue.asCharp()))
        {
            rc=false;
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPVALUNEX,
                                           Msg_Arg("VALUE", (0==runDir)?"": runDir->szOldValue.asCharp()),
	                                       Msg_Arg("DATABASE_PARAMETER", PAN_RUNDIR)));
        }
    }

    // iterate through volume parameters
    if(rc)
    {
        tcn002_XpValueName szParam;
        bool found=false;

        int nNumber=useIndex?m_Index:1; //start at right one (if index is known), or at first (if name or nothing is known)
        int stopAt=useIndex?m_Index:numberOfDataVolumes;

        for(;rc && nNumber<=stopAt; nNumber++)
        {
            sp77sprintf(szParam, szParam.size(), aParamTable[nParam].szName, nNumber);
            par=kernelParameters->Get(szParam.asCharp());

            if(0!=par && 0<strlen(par->szOldValue.asCharp())) //found a name
            {   
                //check if it is the right volume
                if(!useName || (!found && 0==strcmp(par->szOldValue.asCharp(), m_Name.getValue()))) //if name is given, use only first matching
                {
                    if(!useName && !m_Name.setTo(par->szOldValue.asCharp(), DBMSrvVol_VolumeName::DoNotUseEscaping)) // if needed, save the volume name
                    {
                        rc=false;
                        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
                    }

                    m_Index=nNumber;
                    found=true;
                }
            }
        }

        if(!found) //one volume must be found
        {
            rc=false;

            if(useIndex)
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, szParam)));

            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_VOLNFOUNDXP));
        }
        
        if(rc && found) //if volume is found, get its device type
        {
            //get device type of volume
            sp77sprintf(szParam, szParam.size(), aParamTable[nParam].szType, m_Index);
            par=kernelParameters->Get(szParam.asCharp());

            if(0!=par && 0<strlen(par->szOldValue.asCharp())) //type must be found for db_deletevolume
            {
                //get device type
                if(0==stricmp(par->szOldValue.asCharp(), XP_VAL_F_CN00))
                    m_DeviceType=DBMSrvVol_Volume::File;
                else if(0==stricmp(par->szOldValue.asCharp(), "R"))
                    m_DeviceType=DBMSrvVol_Volume::RawDevice;
                else //if(0==stricmp(par->szOldValue.asCharp(), "L"))
                    m_DeviceType=DBMSrvVol_Volume::Link;
            }
            else //device type not found -> can not delete volume properly, because only files must be deleted physically
            {
                rc=false;
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, szParam)));
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_VOLTYPE,
                                            Msg_Arg(SDBMSGTAG_DBMSRV_VOLTYPE__VOLUME_CONT_TYPE, "DATA"),
	                                        Msg_Arg(SDBMSGTAG_DBMSRV_VOLTYPE__VOLUME_ID, m_Index),
                                            Msg_Arg(SDBMSGTAG_DBMSRV_VOLTYPE__PARAMETER, szParam)));
            }
        }

        if( rc && found ) {
            // no error so far, get volume's mode

            sp77sprintf(szParam, szParam.size(), aParamTable[nParam].szMode, m_Index);
            par=kernelParameters->Get(szParam.asCharp());
            
            if(0!=par && 0<strlen(par->szOldValue.asCharp())) {
                if(0==stricmp(par->szOldValue.asCharp(), "NORMAL"))
                    m_Mode=DBMSrvVol_Volume::Normal;
                else if(0==stricmp(par->szOldValue.asCharp(), "SEQUENTIAL"))
                    m_Mode=DBMSrvVol_Volume::Archive;
                else
                    rc = false;

                if( !rc )
                    // unrecognized volume device mode
                    error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_VOLMODE,
                            Msg_Arg(SDBMSGTAG_DBMSRV_VOLMODE__VOLUME_CONT_TYPE, "DATA"),
	                        Msg_Arg(SDBMSGTAG_DBMSRV_VOLMODE__VOLUME_ID, m_Index),
                            Msg_Arg(SDBMSGTAG_DBMSRV_VOLMODE__VOLUME_MODE, par->szOldValue.asCharp()),
                            Msg_Arg(SDBMSGTAG_DBMSRV_VOLMODE__PARAMETER, szParam)));
            }
            else {
                // parameter does not exist or value is empty -> use default "NORMAL"
            }
        }
            
        if(rc)
        {
            if(cn36_IsAbsolutePath(m_Name.getValue()))
                rc=m_AbsoluteName.setTo(m_Name.getValue(), DBMSrvVol_VolumeName::DoNotUseEscaping);
            else
            {
                char * completePath=0;

                if(!cn36_ConstructCompletePath(completePath, runDir->szOldValue.asCharp(), m_Name.getValue()) ||
                   !m_AbsoluteName.setTo(completePath, DBMSrvVol_VolumeName::DoNotUseEscaping))
                {
                    rc=false;
                }
                
                cn36_StrDealloc(completePath);
            }

            if(!rc)
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        }
    }

    if(0!=kernelParameters)
        delete kernelParameters;

    return rc;
}

bool DBMSrvVol_Volume::hasVolumeModeArchive() const
{
    return DBMSrvVol_Volume::Archive==m_Mode;
}

const char * DBMSrvVol_Volume::getDeviceTypeAsString() const
{
    const char *rc="UNKOWN";

    switch(m_DeviceType)
    {
        case DBMSrvVol_Volume::File: rc="FILE"; break;
        case DBMSrvVol_Volume::RawDevice: rc="RAW"; break;
        case DBMSrvVol_Volume::Link: rc="LINK"; break;
    }

    return rc;
}

const char * DBMSrvVol_Volume::getVolumeModeAsString() const
{
    const char* rc=0;

    switch(m_Mode)
    {
        case DBMSrvVol_Volume::Normal: rc="NORMAL"; break;
        case DBMSrvVol_Volume::Archive: rc="ARCHIVE"; break;
        default: rc = "UNKNOWN"; break;
    }

    return rc;
}

bool DBMSrvVol_Volume::getContentType(
    const char                    * contentTypeString,
    DBMSrvVol_Volume::ContentType & contentType)
{
    bool rc=true;

    if(0==stricmp(contentTypeString, XP_DEV_DAT_CN00))
        contentType=DBMSrvVol_Volume::Data;
    else if(0==stricmp(contentTypeString, XP_DEV_LOG_CN00))
        contentType=DBMSrvVol_Volume::Log;
    else
        rc=false;

    return rc;
}
