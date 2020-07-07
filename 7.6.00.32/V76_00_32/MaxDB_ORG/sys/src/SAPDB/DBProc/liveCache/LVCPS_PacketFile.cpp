/*!
 * \file    LVC_PacketFile.cpp
 * \author  MarkusSi
 * \brief   ProcServer packet for file-handling.
 */
/*

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



*/

#include "SAPDB/DBProc/liveCache/LVCPS_PacketFile.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketFile::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tgg00_BasisError  error = 0;

  switch (m_type) {
    //--------------------------------------------------------------
    case msgOpen: {
      tsp00_Int4  fileHandle;

      sink->OpenFile((char*)(this+1) /*FileName*/, 
                     m_fileSize, m_doAppend, 
                     &fileHandle, &error);

      interf->fileReply(fileHandle, false, false, error);
      break;
    }

    //--------------------------------------------------------------
    case msgWrite: {
      sink->WriteFile(m_fileHandle, 
                     (char*)(this+1) /*msg*/, 
                     &error);

      interf->errorStateReply(false, error);
      break;
    }
    
    //--------------------------------------------------------------
    case msgSync: {
      sink->SyncFile((char*)(this+1) /*FileName*/,
                     m_fileHandle, 
                     &error);

      interf->errorStateReply(false, error);
      break;
    }
    
    //--------------------------------------------------------------
    case msgClose: {
      sink->CloseFile(m_fileHandle, 
                     &error);

      interf->errorStateReply(false, error);
      break;
    }
 
    //--------------------------------------------------------------
    case msgExists: {
      bool existsFile  = false;
      bool isDirectory = false;

      sink->ExistsFile((char*)(this+1) /*FileName*/,
                       existsFile, isDirectory, 
                       error);

      interf->fileReply(0, existsFile, isDirectory, error);
      break;
    }

    //--------------------------------------------------------------
    case msgDrop: {
      sink->DropFile((char*)(this+1) /*FileName*/,
                       error);

      interf->errorStateReply(false, error);
      break;
    }

    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketFile::execute"));
    //}
  }
}
