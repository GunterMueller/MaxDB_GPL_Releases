/*! 
  \file    DBMSrvBHist_FileLineEBF.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   a class determining the layout of a line of dbm.ebf

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#include "hcn36.h"

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_FileLineEBF.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_ToString.hpp"


DBMSrvBHist_FileLineEBF::DBMSrvBHist_FileLineEBF(const DBMSrvBHist_Part & part)
    :m_line(0)
{
    size_t lineLength=16; //there are 16 '|' in the line as separators

    lineLength+=strlen(part.GiveKey());
    lineLength+=strlen(part.GiveLabel());
    lineLength+=strlen(part.GiveEBID());
    lineLength+=strlen(part.getBackupType());
    lineLength+=strlen(part.getBackupDateTime());
    lineLength+=strlen(part.GiveUsedBackupTool().AsString());
    lineLength+=strlen(SAPDB_ToString(part.GiveDBMReturnCode()));
    lineLength+=strlen(part.GiveDBMReturnText());
    lineLength+=strlen(part.GiveMediumName());
    lineLength+=strlen(part.getMediumType());
    lineLength+=strlen(part.getMediumOverwrite());
    lineLength+=strlen(part.getMediumSize());
    lineLength+=strlen(part.getMediumBlockSize());
    lineLength+=strlen(part.getMediumKind());
    lineLength+=strlen(part.getMediumLocation());
    lineLength+=strlen(SAPDB_ToString(part.GivePartNumber()));

    if(cn36_StrAlloc(m_line, lineLength))
    {
        sprintf(m_line,
            "%s|%s|%s|%s|%s|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%d|",
            part.GiveKey(),
            part.GiveLabel(),
            part.GiveEBID(),
            part.getBackupType(),
            part.getBackupDateTime(),
            part.GiveUsedBackupTool().AsString(),
            (int)part.GiveDBMReturnCode(),
            part.GiveDBMReturnText(),
            part.GiveMediumName(),
            part.getMediumType(),
            part.getMediumOverwrite(),
            part.getMediumSize(),
            part.getMediumBlockSize(),
            part.getMediumKind(),
            part.getMediumLocation(),
            (int)part.GivePartNumber());
    }
}

DBMSrvBHist_FileLineEBF::~DBMSrvBHist_FileLineEBF()
{
    cn36_StrDealloc(m_line);
}

const char * DBMSrvBHist_FileLineEBF::asString()
{
    return m_line;
}
