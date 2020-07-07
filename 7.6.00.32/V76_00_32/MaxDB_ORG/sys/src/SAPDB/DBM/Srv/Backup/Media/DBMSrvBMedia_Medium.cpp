/*!
  \file    DBMSrvBMedia_Medium.cpp
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

#include "gcn002.h"
#include "hcn20.h"
#include "hcn29.h"
#include "hcn36.h"
#include "hcn90.h"

#include "DBM/Srv/Backup/Media/DBMSrvBMedia_Medium.hpp"
#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

bool DBMSrvBMedia_Medium::makeLocationAbsolute(
        tcn003_Medium   * medium,
        DBMSrvMsg_Error & error)
{
    if(0==cn90Stricmp(medium->szType, ME_TYPE_FILE_CN003))
    {
        bool hasRelativePath=
                    #if defined(WIN32)
                    ('\\'!=medium->szLocation[0] && ':'!=medium->szLocation[1]);
                    #elif defined(UNIX)
                    ('/'!=medium->szLocation[0]);
                    #else
                    #error DBMSrvBMedia_Medium::makeLocationAbsolute() is only coded for WIN32 and UNIX (define one of them)
                    #endif

        if(hasRelativePath)
        {
            char absoluteLocation[ME_LOCATION_LENGTH_MXCN003+1];

            //read run directory
            tcn002_XpValueString parameterValue;

            parameterValue.Init();
                            
            if(cn20XParamGetValue(cn00DBMServerData::vcontrol()->dbname, PAN_RUNDIR, parameterValue, error))
            {
                //construct absolute location (run directory + relative path)
                SAPDB_strcpy(absoluteLocation, parameterValue);

                #if defined (WIN32)
                strcat(absoluteLocation, "\\");
                #else
                strcat(absoluteLocation, "/");
                #endif

                strcat(absoluteLocation, medium->szLocation);

                //replace relative location with absolute location
                strcpy(medium->szLocation, absoluteLocation);
            }
            else
                return false;
        }
    }

    return true;
}

bool DBMSrvBMedia_Medium::getDefaultPageSizeInByte(
        size_t          & pageSize,
        DBMSrvMsg_Error & error)
{
    DBMSrvPar_Parameters                          * kernelParameters=0;
    DBMSrvInfrReply_ReplyBufferWritingIntoMsgList   reply(error);

    if(reply.isValid())
    {
        if(OK_CN00==cn29ParamStartSessionDBIndependent(
                        kernelParameters,
                        "",
                        reply.getData(),
                        reply.getLength(),
                        reply.getMaximalLength()))
        {
            DBMSrvPar_Parameter * parameter=kernelParameters->Get("_PAGE_SIZE");

            if(0==parameter || 0>=strlen(parameter->szDefValue.asCharp()) || 0==atoi(parameter->szDefValue.asCharp()))
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPVALUNEX,
                                           Msg_Arg("VALUE", (0==parameter)?"": parameter->szDefValue.asCharp()),
	                                       Msg_Arg("DATABASE_PARAMETER", "_PAGE_SIZE")));
                return false;
            }
            else
            {
                pageSize=atoi(parameter->szDefValue.asCharp());
                return true;
            }
        }
        else
        {
            reply.fillMessageList();
            return false;
        }
    }
    else
        return false;
}

bool DBMSrvBMedia_Medium::getDefaultBlockSizeInByte(
    size_t          & blockSize,
    DBMSrvMsg_Error   error)
{
    size_t pageSize=0;

    if(getDefaultPageSizeInByte(pageSize, error))
    {
        blockSize=pageSize*atoi(ME_DEFAULT_BLOCKSIZE_CN003);
        return true;
    }
    else
        return false;
}

bool DBMSrvBMedia_Medium::getBlocksizeInByte(
        size_t              & blockSize,
        const tcn003_Medium & medium,
        DBMSrvMsg_Error       error)
{
    bool rc=false;

    //get block size in pages
    long pagesPerBlock;

    if(cn36_StrToLong(pagesPerBlock, medium.szBlocksize) &&
       0<=pagesPerBlock)
    {
        if(0==pagesPerBlock)    //if medium has block size 0, use the DBM Server default (db kernel parameter BACKUP_BLOCK_CNT is no longer existent starting with 7.6.00)
            cn36_StrToLong(pagesPerBlock, ME_DEFAULT_BLOCKSIZE_CN003);
    }
    else
    {
        error.Overrule(
            DBMSrvMsg_Error(
                SDBMSG_DBMSRV_ILLEGALBLOCKINGFACTOR,
	            Msg_Arg(SDBMSGTAG_DBMSRV_ILLEGALBLOCKINGFACTOR__BLOCKSIZE, medium.szBlocksize)));
        return rc;
    }

    //get page size in bytes
    tsp00_DbNamec        dbName;
    tcn002_XpValueString pageSize;

    dbName.rawAssign(cn00DBMServerData::vcontrol()->dbname.asCharp());

    if(cn20XParamGetValue(dbName, PAN_PAGE_SIZE, pageSize, error))
    {
        long bytesPerPage;

        if(!cn36_StrToLong(bytesPerPage, (char *)pageSize) ||
           0>=bytesPerPage)
        {
            error.Overrule(
                DBMSrvMsg_Error(
                    SDBMSG_DBMSRV_ILLEGALPAGESIZE,
	                Msg_Arg(SDBMSGTAG_DBMSRV_ILLEGALPAGESIZE__PAGESIZE, pageSize)));
        }
        else
        {
            blockSize=bytesPerPage*pagesPerBlock;
            rc=true;
        }
    }

    return rc;
}
