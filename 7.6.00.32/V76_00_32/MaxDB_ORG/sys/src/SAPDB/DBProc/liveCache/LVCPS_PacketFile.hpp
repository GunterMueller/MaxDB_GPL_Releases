/*!
 * \file    LVC_PacketFile.hpp
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

#ifndef __LVCPS_PACKETFILE_HPP
#define __LVCPS_PACKETFILE_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"

/*!
 * \brief Request packet for file-handling.
 */
class LVCPS_PacketFile : public LVC_ProcServerPacket<LVCPS_PacketFile> {
public:
  enum {
    CODE  = 110, ///< Packet code.

    msgOpen    = 0,
    msgWrite   = 1,
    msgSync    = 2,
    msgClose   = 3,
    msgExists  = 4,
    msgDrop    = 5
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int            m_type;

  tsp00_Uint4    m_fileSize;
  tsp00_Int4     m_fileHandle;
  boolean        m_doAppend;

  // variable sized components are placed behind the fixed sized
  // on demand:
  // - char  *msg,
  // - char  *pFileName,

};

#endif  // __LVCPS_PACKETFILE_HPP

