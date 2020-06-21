/*!
    @file           DBMSrvShM_Persistence.cpp
    @author         MarcW
    @brief          description of the shared memory, of maintenance "from outside"

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "hcn42.h"
#include "heo06.h"

#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp"
#include "ToolsCommon/CString/ToolsCStr_Str.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

const char* const DBMSrvShM_Persistence::m_FileNameBase(FGET_DBNAME);
const char* const DBMSrvShM_Persistence::m_FileExtensionData(".dbm.shm");
const char* const DBMSrvShM_Persistence::m_FileExtensionAdmin(".dbm.shi");


DBMSrvShM_Persistence::DBMSrvShM_Persistence(
            const char* rootDir,
            const char* dbname,
            DBMSrvMsg_Error& errOut)
            : m_RootDir(NULL), 
              m_InfoFileAbs(NULL), 
              m_DataFileAbs(NULL),
              m_IsValid(false) {
    
    while( rootDir != NULL && dbname != NULL ) {

        if( !ToolsCStr_Str::AllocCpy(m_RootDir, rootDir, errOut) )
            break;
                
        // put database name in filenames
        Tools_DynamicUTF8String fileBase(m_FileNameBase);
        Tools_DynamicUTF8String::BasisElementIndex  mark = fileBase.RFind(FGET_DBNAME);
        fileBase.Replace(mark, (Tools_DynamicUTF8String::ElementCount)strlen(FGET_DBNAME), dbname);

        Tools_DynamicUTF8String infoFileAbs(m_RootDir);
        infoFileAbs.Append(fileBase);
        infoFileAbs.Append(m_FileExtensionAdmin);
        if( !ToolsCStr_Str::AllocCpy(m_InfoFileAbs, infoFileAbs.CharPtr(), errOut) )
            break;
        
        Tools_DynamicUTF8String dataFileAbs(m_RootDir);
        dataFileAbs.Append(fileBase);
        dataFileAbs.Append(m_FileExtensionData);
        if( !ToolsCStr_Str::AllocCpy(m_DataFileAbs, dataFileAbs.CharPtr(), errOut) )
            break;

        m_IsValid = true;
        break;
    }
}

DBMSrvShM_Persistence::~DBMSrvShM_Persistence() {
    ToolsCStr_Str::Dealloc(m_RootDir);
    ToolsCStr_Str::Dealloc(m_InfoFileAbs);
    ToolsCStr_Str::Dealloc(m_DataFileAbs);
}

bool DBMSrvShM_Persistence::exists(bool& doesExist, DBMSrvMsg_Error& errList) const {

    if( !m_IsValid ) {
        errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        return false;
    }

    bool errorFound(false);
    tsp05_RteFileInfo info_Info;
    tsp05_RteFileError err_Info;
    tsp05_RteFileInfo info_Data;
    tsp05_RteFileError err_Data;

    sqlfinfoc( m_InfoFileAbs, &info_Info, &err_Info );
    sqlfinfoc( m_DataFileAbs, &info_Data, &err_Data );

    errorFound = (err_Info.sp5fe_result == vf_notok);
    if( errorFound ) {
        // report: error checking info file
        errList.Overrule(DBMSrvMsg_RTEError(err_Info));
        errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECHK,
                                         Msg_Arg(SDBMSGTAG_DBMSRV_FILECHK__FILENAME, m_InfoFileAbs)));
    }

    errorFound = errorFound || (err_Data.sp5fe_result == vf_notok);
    if( err_Data.sp5fe_result == vf_notok ) {
        // report: error checking data file
        errList.Overrule(DBMSrvMsg_RTEError(err_Data));
        errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECHK,
                                         Msg_Arg(SDBMSGTAG_DBMSRV_FILECHK__FILENAME, m_DataFileAbs)));
    }

    if( !errorFound ) {
        /*
        we cannot check this since if someone else has opened the shared memory,
        it is not writable for us...
        if( info_Info.sp5fi_exists &&
            info_Data.sp5fi_exists &&
            (!info_Info.sp5fi_readable ||
            !info_Info.sp5fi_writeable ||
            !info_Data.sp5fi_readable ||
            !info_Data.sp5fi_writeable) ) {
            // shm cannot be accessed correctly
            errorFound = true;
            errList = errList +
                DBMSrvMsg_Error(
                    DBMSrvMsg_ Error::SHMFILEPERM,
                    m_InfoFileAbs.CharPtr(),
                    m_DataFileAbs.CharPtr());
        }
        else
        */
        if( !info_Data.sp5fi_exists ) {
            // report: data file does not exist
            errorFound = true;
            errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNODATAFILE));
        }
        if( !info_Info.sp5fi_exists ) {
            // report: info file does not exist
            errorFound = true;
            errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOINFOFILE));
        }
        doesExist = !errorFound;
    }
    return !errorFound;
}

bool DBMSrvShM_Persistence::remove(DBMSrvMsg_Error& errList) {
    
    if( !m_IsValid ) {
        errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        return false;
    }

    bool errorInfo(false);
    bool errorData(false);
    tsp05_RteFileInfo info_Info;
    tsp05_RteFileError err_Info;
    tsp05_RteFileInfo info_Data;
    tsp05_RteFileError err_Data;

    sqlfinfoc( m_InfoFileAbs, &info_Info, &err_Info );
    if( info_Info.sp5fi_exists ) {
        sqlferasec( m_InfoFileAbs, &err_Info );

        errorInfo = err_Info.sp5fe_result == vf_notok;
        if( errorInfo ) {
            // report: could not remove info file
            errList.Overrule(DBMSrvMsg_RTEError(err_Info));
            errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEDEL,
                                            Msg_Arg(SDBMSGTAG_DBMSRV_FILEDEL__FILENAME, m_InfoFileAbs)));
        }
    }

    sqlfinfoc( m_DataFileAbs, &info_Data, &err_Data );
    if( info_Data.sp5fi_exists ) {
        sqlferasec( m_DataFileAbs, &err_Data );
        
        errorData = err_Data.sp5fe_result == vf_notok;
        if( errorData ) {
            // report: cound not remove data file
            errList.Overrule(DBMSrvMsg_RTEError(err_Data));
            errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEDEL,
                                            Msg_Arg(SDBMSGTAG_DBMSRV_FILEDEL__FILENAME,m_DataFileAbs)));
        }
    }
    
    return !(errorInfo || errorData);
}
