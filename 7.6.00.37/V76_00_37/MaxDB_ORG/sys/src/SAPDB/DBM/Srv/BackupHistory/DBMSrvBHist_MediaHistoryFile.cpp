/*! 
  \file    DBMSrvBHist_MediaHistoryFile.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   a class for reading the media history file

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
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_MediaHistoryFile.hpp"


//-----------------------------------------------------------------------------
// class DBMSrvBHist_MediaHistoryFile
//-----------------------------------------------------------------------------

DBMSrvBHist_MediaHistoryFile::DBMSrvBHist_MediaHistoryFile()
    :DBMSrvBHist_File(FGET_BACKMDF_CN42, DBMSrvBHist_File::CanBeAbsent)  //"dbm.mdf" does not have to exist
{
}

DBMSrvBHist_MediaHistoryFile::~DBMSrvBHist_MediaHistoryFile()
{
    for(Tools_List<DBMSrvBHist_Part *>::iterator MediumPart=mediaList.begin(); MediumPart!=mediaList.end(); ++MediumPart)
        delete (*MediumPart);
}

    
Tools_List<DBMSrvBHist_Part *> & DBMSrvBHist_MediaHistoryFile::getMediaList()
{
    return mediaList;
}

tcn00_Error DBMSrvBHist_MediaHistoryFile::useReadLine(DBMSrv_Reply & reply)
{
    tcn00_Error        rc=OK_CN00;
    DBMSrvBHist_Part * newPart=new DBMSrvBHist_Part(DBMSrvBHist_Part::PartFromMediaFile);

    if(0!=newPart)
    {
        DBMSrvMsg_Error error;

        if(!newPart->SetToMedium(line, error))
        {
            if(error==DBMSrvMsg_Error::DBMSrv_BHIST_MED_INVALID_ENTRY)
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MDFDIRTY,
                                               Msg_Arg(SDBMSGTAG_DBMSRV_MDFDIRTY__FILENAME, m_fileName),
                                               Msg_Arg(SDBMSGTAG_DBMSRV_MDFDIRTY__LINE_NO, m_LineNumber)));
            }

            rc=reply.startWithMessageList(error);
        }

        if(OK_CN00==rc && !mediaList.push_back(newPart))               //try to add the pointer to the media list
            rc=reply.startWithError(ERR_MEM_CN00);

        if(OK_CN00!=rc)
            delete newPart; //could not add to the list, last chance to free the memory
    }
    else
        rc=reply.startWithError(ERR_MEM_CN00);

    return rc;
}
