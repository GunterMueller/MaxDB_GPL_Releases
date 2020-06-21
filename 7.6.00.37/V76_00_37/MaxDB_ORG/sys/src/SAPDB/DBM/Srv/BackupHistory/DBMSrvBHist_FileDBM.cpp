/*! 
  \file    DBMSrvBHist_File-DBM.cpp
  \author  TiloH
  \ingroup handling of backup history files by the DBMServer
  \brief   definition of DBMServer specific part of a class for
           reading the backup history files

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "hcn42.h"
#include "heo06.h"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"


//-----------------------------------------------------------------------------
// member of class DBMSrvBHist_File (DBMServer specific part)
//-----------------------------------------------------------------------------

tcn00_Error DBMSrvBHist_File::openFile(DBMSrv_Reply & Reply)
{
    tcn00_Error        Rc=OK_CN00;
    tsp05_RteFileError RTEFileError;
    tsp00_Pathc        FileName;
    tsp05_RteFileInfo  RTEFileInfo;

    // Does the file exist? If not, can it be restored ?
    cn42GetFileName(cn00DBMServerData::vcontrol()->dbname, fileType, FileName);

    sqlfinfoc(FileName, &RTEFileInfo, &RTEFileError);

    if(vf_ok==RTEFileError.sp5fe_result && !RTEFileInfo.sp5fi_exists)
    {
        DBMSrvMsg_Error oError;
        cn42RestoreFile(cn00DBMServerData::vcontrol()->dbname, fileType, oError);
        sqlfinfoc(FileName, &RTEFileInfo, &RTEFileError);
    }

    if(vf_ok==RTEFileError.sp5fe_result)
    {
        if(!RTEFileInfo.sp5fi_exists)
            Rc=Reply.startWithError(ERR_NOHISTORY_CN00);
    }
    else
        Rc=Reply.startWithMessageList(DBMSrvMsg_RTEError(RTEFileError));

    // open file
    if(OK_CN00==Rc)
        Rc=openFile(FileName, Reply);
    
    return Rc;
}

tcn00_Error DBMSrvBHist_File::readFile(DBMSrv_Reply & Reply)
{
    tcn00_Error Rc=OK_CN00;

    Rc=openFile(Reply);
    readFileWorkPart(Rc, Reply);

    return Rc;
}
