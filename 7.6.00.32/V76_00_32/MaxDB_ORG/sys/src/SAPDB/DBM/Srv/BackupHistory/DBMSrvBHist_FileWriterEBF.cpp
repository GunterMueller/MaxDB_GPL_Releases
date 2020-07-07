/*! 
  \file    DBMSrvBHist_FileWriterEBF.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   definition of a class for writing the external
           backup history file dbm.ebf

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


#include "hcn42.h"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_FileWriterEBF.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_FileLineEBF.hpp"


DBMSrvBHist_FileWriterEBF::DBMSrvBHist_FileWriterEBF()
:DBMSrvBHist_File(FGET_BACKEBF_CN42, DBMSrvBHist_File::CanBeAbsent)
{
}

DBMSrvBHist_FileWriterEBF::~DBMSrvBHist_FileWriterEBF()
{
    for(Tools_List<DBMSrvBHist_Part *>::iterator EBID=eBIDList.begin(); EBID!=eBIDList.end(); ++EBID)
        if(!(*EBID)->IsAdded()) //only Parts that where not added meanwhile to other structures can be deleted
           delete (*EBID);

    if(0!=m_file)
        delete m_file;
}

Tools_List<DBMSrvBHist_Part *> & DBMSrvBHist_FileWriterEBF::getEBIDList()
{
    return eBIDList;
}

bool DBMSrvBHist_FileWriterEBF::openFile(const char * fileName, DBMSrvMsg_Error & error)
{
    bool rc=false;

    m_file=new tni34_ATextFile(fileName, tni34_AFile::ToDiskAppend, 1, 1);

    if(0==m_file)
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEMORY));
    else
        if(!m_file->IsGood())
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BHIST_EXT_OPEN, Msg_Arg("DBMSRV_COMP", m_file->GiveName())));
        else
            rc=true;

    return rc;
}

bool DBMSrvBHist_FileWriterEBF::appendEntry(DBMSrvBHist_Part & part, DBMSrvMsg_Error & error)
{
    bool rc=false;

    if(0==m_file)
        ;//TODO internal error
    else
    {
        long                    BytesWritten=0;
        DBMSrvBHist_FileLineEBF EBFline(part);

        if(m_file->WriteLine(EBFline.asString(), BytesWritten))
            rc=true;
        else
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BHIST_EXT_WRITE, Msg_Arg("DBMSRV_COMP", 0==m_file?"":m_file->GiveName())));
    }

    return rc;
}

bool DBMSrvBHist_FileWriterEBF::closeFile(DBMSrvMsg_Error & error)
{
    return (m_file->Close()?true:false);
}

//TODO check if this function is used (besides the virtual function table)
tcn00_Error DBMSrvBHist_FileWriterEBF::useReadLine(DBMSrv_Reply & reply)
{
    tcn00_Error        rc=0;
    DBMSrvBHist_Part * NewPart=new DBMSrvBHist_Part(DBMSrvBHist_Part::PartFromEBIDFile);

    if(0!=NewPart)
    {
        DBMSrvMsg_Error error;

        if(!NewPart->SetToEBID(line, error))
            rc=reply.startWithMessageList(error);

        if(OK_CN00==rc && !eBIDList.push_back(NewPart))               //try to add the pointer to the list
            rc=reply.startWithError(ERR_MEM_CN00);

        if(OK_CN00!=rc)
            delete NewPart; //could not add to the list, last chance to free the memory
    }
    else
        rc=reply.startWithError(ERR_MEM_CN00);

    return rc;
}
