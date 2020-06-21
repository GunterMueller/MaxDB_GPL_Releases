/*! 
  \file    DBMSrvBHist_HistoryFile.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   a class for reading the backup history file

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
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_HistoryFile.hpp"


//-----------------------------------------------------------------------------
// class DBMSrvBHist_HistoryFile
//-----------------------------------------------------------------------------

DBMSrvBHist_HistoryFile::DBMSrvBHist_HistoryFile()
    :DBMSrvBHist_File(FGET_BACKHIST_CN42, DBMSrvBHist_File::MustExist) //"dbm.knl" must exist
{
}

DBMSrvBHist_HistoryFile::~DBMSrvBHist_HistoryFile()
{
    for(Tools_List<DBMSrvBHist_Backup *>::iterator Backup=historyList.begin(); Backup!=historyList.end(); ++Backup)
        delete (*Backup);
}

Tools_List<DBMSrvBHist_Backup *> & DBMSrvBHist_HistoryFile::getHistoryList()
{
    return historyList;
}

tcn00_Error DBMSrvBHist_HistoryFile::useReadLine(DBMSrv_Reply & reply)
{
    tcn00_Error rc=OK_CN00;

    DBMSrvBHist_Backup * newBackup=new DBMSrvBHist_Backup;

    if(0==newBackup)
        rc=reply.startWithError(ERR_MEM_CN00);
    else
    {
        DBMSrvMsg_Error error;

        if(!newBackup->SetTo(line, error))
        {
            if(error==DBMSrvMsg_Error::DBMSrv_BHIST_INVALID_ENTRY)
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_KNLDIRTY,
                                               Msg_Arg(SDBMSGTAG_DBMSRV_KNLDIRTY__FILENAME, m_fileName),
                                               Msg_Arg(SDBMSGTAG_DBMSRV_KNLDIRTY__LINE_NO, m_LineNumber)));
            }

            rc=reply.startWithMessageList(error);
        }

        if(OK_CN00==rc && !historyList.push_back(newBackup))               //try to add the pointer to the list
            rc=reply.startWithError(ERR_MEM_CN00);

        if(OK_CN00!=rc)
            delete newBackup; //could not add to the list, last chance to free the memory
    }

    return rc;
}
