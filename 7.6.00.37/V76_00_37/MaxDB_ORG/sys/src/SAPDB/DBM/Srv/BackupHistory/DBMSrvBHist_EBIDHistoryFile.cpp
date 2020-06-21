/*! 
  \file    DBMSrvBHist_EBIDHistoryFile.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   definition of a class for reading the external backup history file

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
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_EBIDHistoryFile.hpp"


//-----------------------------------------------------------------------------
// member of class DBMSrvBHist_EBIDHistoryFile
//-----------------------------------------------------------------------------

DBMSrvBHist_EBIDHistoryFile::DBMSrvBHist_EBIDHistoryFile()
:DBMSrvBHist_File(FGET_BACKEBF_CN42, DBMSrvBHist_File::CanBeAbsent) //"dbm.ebf" must not exist
{
}

DBMSrvBHist_EBIDHistoryFile::~DBMSrvBHist_EBIDHistoryFile()
{
    for(Tools_List<DBMSrvBHist_Part *>::iterator EBID=eBIDList.begin(); EBID!=eBIDList.end(); ++EBID)
        if(!(*EBID)->IsAdded()) //only Parts that where not added meanwhile to other structures can be deleted
           delete (*EBID);
}

tcn00_Error DBMSrvBHist_EBIDHistoryFile::readBasicFile(const char * Path, DBMSrv_Reply & Reply)
{
    tsp00_Pathc FileName;

    FileName.rawAssign(Path);

    return readFile(FileName, Reply);
}

Tools_List<DBMSrvBHist_Part *> & DBMSrvBHist_EBIDHistoryFile::getEBIDList()
{
    return eBIDList;
}

tcn00_Error DBMSrvBHist_EBIDHistoryFile::useReadLine(DBMSrv_Reply & Reply)
{
    tcn00_Error        rc=0;
    DBMSrvBHist_Part * NewPart=new DBMSrvBHist_Part(DBMSrvBHist_Part::PartFromEBIDFile);

    if(0!=NewPart)
    {
        DBMSrvMsg_Error error;

        if(!NewPart->SetToEBID(line, error))
        {
            if(error==DBMSrvMsg_Error::DBMSrv_BHIST_EXT_INVALID_LINE)
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EBFDIRTY,
                                           Msg_Arg(SDBMSGTAG_DBMSRV_EBFDIRTY__FILENAME, m_fileName),
                                           Msg_Arg(SDBMSGTAG_DBMSRV_EBFDIRTY__LINE_NO, m_LineNumber)));
            }

            rc=Reply.startWithMessageList(error);
        }

        if(OK_CN00==rc && !eBIDList.push_back(NewPart))               //try to add the pointer to the list
            rc=Reply.startWithError(ERR_MEM_CN00);

        if(OK_CN00!=rc)
            delete NewPart; //could not add to the list, last chance to free the memory
    }
    else
        rc=Reply.startWithError(ERR_MEM_CN00);

    return rc;
}
