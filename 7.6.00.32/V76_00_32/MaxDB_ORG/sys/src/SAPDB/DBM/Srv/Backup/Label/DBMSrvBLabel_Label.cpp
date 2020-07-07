/*!
    \file    DBMSrvBLabel_Label.cpp
    \author  TiloH
    \ingroup backup label handling for DBM Server
    \brief   a class handling labels of backups

\if EMIT_LICENCE

    ========== licence begin LGPL
    Copyright (c) 1998-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

\endif
*/

#include "DBM/Srv/Backup/Label/DBMSrvBLabel_Label.hpp"
#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "gcn00.h"
#include "hcn36.h"
#include "hcn90.h"
#include "heo02.h"
#include "heo06.h"
#include "gkb03.h"

int DBMSrvBLabel_Label::m_MaxAllowedBlockSize=32*1024*1024; //a maximal allowed block size of 32 MB should be sufficient for a long time

DBMSrvBLabel_Label::DBMSrvBLabel_Label()
  : m_Label(0),
    m_StartDate(0),
    m_StartTime(0),
    m_Server(0),
    m_StopDate(0),
    m_StopTime(0),
    m_DB(0),
    m_KernelVersion(0),
    m_IsConsistent(0),
    m_FirstLogPage(0),
    m_LastLogPage(0),
    m_DBStamp1Date(0),
    m_DBStamp1Time(0),
    m_DBStamp2Date(0),
    m_DBStamp2Time(0),
    m_PageCount(0),
    m_NumberDevices(0),
    m_DatabaseID(0),
    m_MaxUsedDataPage(0),
    m_ConverterPageCount(0)
{
}

DBMSrvBLabel_Label::~DBMSrvBLabel_Label()
{
    cn36_StrDealloc(m_Label);
    cn36_StrDealloc(m_StartDate);
    cn36_StrDealloc(m_StartTime);
    cn36_StrDealloc(m_Server);
    cn36_StrDealloc(m_StopDate);
    cn36_StrDealloc(m_StopTime);
    cn36_StrDealloc(m_DB);
    cn36_StrDealloc(m_KernelVersion);
    cn36_StrDealloc(m_IsConsistent);
    cn36_StrDealloc(m_FirstLogPage);
    cn36_StrDealloc(m_LastLogPage);
    cn36_StrDealloc(m_DBStamp1Date);
    cn36_StrDealloc(m_DBStamp1Time);
    cn36_StrDealloc(m_DBStamp2Date);
    cn36_StrDealloc(m_DBStamp2Time);
    cn36_StrDealloc(m_PageCount);
    cn36_StrDealloc(m_NumberDevices);
    cn36_StrDealloc(m_DatabaseID);
    cn36_StrDealloc(m_MaxUsedDataPage);
    cn36_StrDealloc(m_ConverterPageCount);
}

bool DBMSrvBLabel_Label::readLabelFrom(
    const char                    * deviceName,
    size_t                          blockSize,
    const DBMSrvBMedia_DeviceType & deviceType,
    char                          * replyData,
    int                           * replyLen,
    int                             replyLenMax)
{
    bool                rc=true;
    tsp00_Int4          bufferSize=(int)blockSize;
    void              * pBuffer=0;
    tsp00_Int4          fileHandle;
    tsp05_RteFileError  rteFileErr;

    // first try to open with given block size
    sqlfsaveopenc(deviceName, sp5vf_read, &bufferSize, &fileHandle, &pBuffer, &rteFileErr);

    if(rteFileErr.sp5fe_result!=vf_ok &&          //if first try was unsuccessful ...
       bufferSize!=blockSize &&                   //but returned an alternative block size ...
       0<bufferSize &&                            //that is ...
       bufferSize<=m_MaxAllowedBlockSize &&       //plausible ...
       deviceType==DBMSrvBMedia_DeviceType::File) //and we can easily reopen the medium again, because it is a file ...
    {
        sqlfsaveopenc(deviceName, sp5vf_read, &bufferSize, &fileHandle, &pBuffer, &rteFileErr); //... try open with new block size
    }

    if(rteFileErr.sp5fe_result!=vf_ok)
    {
        rc=false;
        DBMSrv_Reply(replyData, replyLen, replyLenMax)
            .startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
    }
    else
    {
        tkb3_info_stuff info;
        size_t          copyCount=0;
        tsp00_Longint   nOut=0;

        while(rc && copyCount<sizeof(tkb3_info_stuff))
        {
            sqlfreadc(fileHandle, pBuffer, bufferSize, &nOut, &rteFileErr);

            if(rteFileErr.sp5fe_result!=vf_ok)
            {
                rc=false;
                DBMSrv_Reply(replyData, replyLen, replyLenMax)
                    .startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
            }
            else
            {
                char * infoBuffer=(char *)(&info);
                size_t toCopy=nOut;

                if(toCopy>sizeof(tkb3_info_stuff)-copyCount)
                    toCopy=sizeof(tkb3_info_stuff)-copyCount;

                memcpy(infoBuffer+copyCount, pBuffer, toCopy);
                copyCount+=toCopy;
            }
        }

        if(rc)
        {
            SAPDB_ToStringClass isConsistent(info.inf_is_consistent, SAPDB_ToStringClass::boolalpha);
            SAPDB_ToStringClass firstLogPageAsString(info.inf_first_iosequence);
            SAPDB_ToStringClass lastLogPageAsString(info.inf_last_iosequence);
            SAPDB_ToStringClass bdPageCount(info.inf_bd_page_count);
            SAPDB_ToStringClass numberDevices(info.inf_tapes_used);
            SAPDB_ToStringClass maxUsedDataPage(info.inf_max_used_data_pno);
            SAPDB_ToStringClass usedConverterPages(info.inf_conv_page_count);

            if(!createCopy(m_Label,         info.inf_label,         sizeof(info.inf_label),         replyData, replyLen) ||
               !createCopy(m_StartDate,     info.inf_start_date,    sizeof(info.inf_start_date),    replyData, replyLen) ||
               !createCopy(m_StartTime,     info.inf_start_time,    sizeof(info.inf_start_time),    replyData, replyLen) ||
               !createCopy(m_Server,        info.inf_servernode,    sizeof(info.inf_servernode),    replyData, replyLen) ||
               !createCopy(m_StopDate,      info.inf_end_date,      sizeof(info.inf_end_date),      replyData, replyLen) ||
               !createCopy(m_StopTime,      info.inf_end_time,      sizeof(info.inf_end_time),      replyData, replyLen) ||   
               !createCopy(m_DB,            info.inf_serverdb,      sizeof(info.inf_serverdb),      replyData, replyLen) ||
               !createCopy(m_KernelVersion, info.inf_knlvers,       sizeof(info.inf_knlvers),       replyData, replyLen) ||
               !cn36_StrAllocCpy(m_IsConsistent, isConsistent)                                                           ||
               !cn36_StrAllocCpy(m_FirstLogPage, firstLogPageAsString)                                                   ||
               !cn36_StrAllocCpy(m_LastLogPage, lastLogPageAsString)                                                     ||
               !createCopy(m_DBStamp1Date,  info.inf_dbstamp1_date, sizeof(info.inf_dbstamp1_date), replyData, replyLen) ||
               !createCopy(m_DBStamp1Time,  info.inf_dbstamp1_time, sizeof(info.inf_dbstamp1_time), replyData, replyLen) ||
               !createCopy(m_DBStamp2Date,  info.inf_dbstamp2_date, sizeof(info.inf_dbstamp2_date), replyData, replyLen) ||
               !createCopy(m_DBStamp2Time,  info.inf_dbstamp2_time, sizeof(info.inf_dbstamp2_time), replyData, replyLen) ||
               !cn36_StrAllocCpy(m_PageCount, bdPageCount)                                                               ||
               !cn36_StrAllocCpy(m_NumberDevices, numberDevices)                                                         ||
               !createCopy(m_DatabaseID,    info.inf_db_ident,      sizeof(info.inf_db_ident),      replyData, replyLen) ||
               !cn36_StrAllocCpy(m_MaxUsedDataPage, maxUsedDataPage)                                                     ||
               !cn36_StrAllocCpy(m_ConverterPageCount, usedConverterPages)
               )
            {
                rc=false;
            }

            if(0==cn36_StrNUprCmp(m_Label, "LOG_", strlen("LOG_")))
            {
                cn36_StrDealloc(m_MaxUsedDataPage);
                cn36_StrDealloc(m_IsConsistent);
                cn36_StrDealloc(m_ConverterPageCount);
            }
            else
            {
                cn36_StrDealloc(m_LastLogPage);
                cn36_StrDealloc(m_DBStamp2Date);
                cn36_StrDealloc(m_DBStamp2Time);
            }
        }

        sqlfclosec(fileHandle, sp5vf_close_normal, &rteFileErr);
    }

    return rc;
}

const char * DBMSrvBLabel_Label::getLabel() const
{
    return m_Label?m_Label:"";
}

const char * DBMSrvBLabel_Label::getStartDate() const
{
    return m_StartDate?m_StartDate:"";
}

const char * DBMSrvBLabel_Label::getStartTime() const
{
    return m_StartTime?m_StartTime:"";
}

const char * DBMSrvBLabel_Label::getStopDate() const
{
    return m_StopDate?m_StopDate:"";
}

const char * DBMSrvBLabel_Label::getStopTime() const
{
    return m_StopTime?m_StopTime:"";
}

const char * DBMSrvBLabel_Label::getServer() const
{
    return m_Server?m_Server:"";
}

const char * DBMSrvBLabel_Label::getDB() const
{
    return m_DB?m_DB:"";
}

const char * DBMSrvBLabel_Label::getKernelVersion() const
{
    return m_KernelVersion?m_KernelVersion:"";
}

const char * DBMSrvBLabel_Label::getIsConsistent() const
{
    return m_IsConsistent?m_IsConsistent:"";
}

const char * DBMSrvBLabel_Label::getFirstLogPage() const
{
    return m_FirstLogPage?m_FirstLogPage:"";
}

const char * DBMSrvBLabel_Label::getLastLogPage() const
{
    return m_LastLogPage?m_LastLogPage:"";
}

const char * DBMSrvBLabel_Label::getDBStamp1Date() const
{
    return m_DBStamp1Date?m_DBStamp1Date:"";
}

const char * DBMSrvBLabel_Label::getDBStamp1Time() const
{
    return m_DBStamp1Time?m_DBStamp1Time:"";
}

const char * DBMSrvBLabel_Label::getDBStamp2Date() const
{
    return m_DBStamp2Date?m_DBStamp2Date:"";
}

const char * DBMSrvBLabel_Label::getDBStamp2Time() const
{
    return m_DBStamp2Time?m_DBStamp2Time:"";
}

const char * DBMSrvBLabel_Label::getPageCount() const
{
    return m_PageCount?m_PageCount:"";
}

const char * DBMSrvBLabel_Label::getNumberDevices() const
{
    return m_NumberDevices?m_NumberDevices:"";
}

const char * DBMSrvBLabel_Label::getDatabaseID() const
{
    return m_DatabaseID?m_DatabaseID:"";
}

const char * DBMSrvBLabel_Label::getMaxUsedDataPage() const
{
    return m_MaxUsedDataPage?m_MaxUsedDataPage:"";
}

const char * DBMSrvBLabel_Label::getConverterPageCount() const
{
    return m_ConverterPageCount?m_ConverterPageCount:"";
}

bool DBMSrvBLabel_Label::getStartDateTime(tcn36_DateTimeString & time) const
{
    bool rc=true;

    if(0<strlen(getStartDate()) && 0<strlen(getStartTime()))
    {
        char * h=0;

        rc=cn36_StrAlloc(h, strlen(getStartDate())+1+strlen(getStartTime()))?true:false;

        if(rc)
        {
            strcpy(h, getStartDate());
            strcat(h, " ");
            strcat(h, getStartTime());

            time.SetTo(h, "yyyymmdd 00HHMMSS");
        }

        cn36_StrDealloc(h);
    }

    return rc;
}

bool DBMSrvBLabel_Label::equals(const DBMSrvBLabel_Label & otherLabel) const
{
    return (0==strcmp(getLabel(),        otherLabel.getLabel()) &&
            0==strcmp(getStartDate(),    otherLabel.getStartDate()) &&
            0==strcmp(getStartTime(),    otherLabel.getStartTime()) &&
            0==strcmp(getServer(),       otherLabel.getServer()) &&
            0==strcmp(getStopDate(),     otherLabel.getStopDate()) &&
            0==strcmp(getStopTime(),     otherLabel.getStopTime()) &&
            0==strcmp(getDB(),           otherLabel.getDB()) &&
            0==strcmp(getDBStamp1Date(), otherLabel.getDBStamp1Date()) &&
            0==strcmp(getDBStamp1Time(), otherLabel.getDBStamp1Time()) &&
            0==strcmp(getDBStamp2Date(), otherLabel.getDBStamp2Date()) &&
            0==strcmp(getDBStamp2Time(), otherLabel.getDBStamp2Time())    );
}

bool DBMSrvBLabel_Label::createCopy(
        char       *& dest,
        const char *  src,
        size_t        maxLengthSrc,
        char       *  replyData,
        int        *  replyLen)
{
    bool rc=cn36_StrAlloc(dest, maxLengthSrc)?true:false;

    if(rc)
        cn90StringPtoC(dest, src, (int)maxLengthSrc);
    else
        cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);

    return rc;
}
