/*!*********************************************************************

    @file         SAPDB_IPersistenceStream.hpp

    @author       ThomasA

    @brief        stream class to serialize objects

    This interface has been introduced to store and restore
    objects of class SAPDBFields_IncrementalMemorySequence
    (= SQL literals).

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
***********************************************************************/

#ifndef SAPDB_IPERSISTENCESTREAM_HPP
#define SAPDB_IPERSISTENCESTREAM_HPP


class Msg_List;

/*!

  @class SAPDB_IPersistenceStream
  @brief simple stream to serialize objects

 */
class SAPDB_IPersistenceStream
{
public:
    /*!
      @brief reads a given number of bytes.

      It is an error if the end of the stream is reached before
      RequiredBytes bytes could be read.

      @return true if call succeeded
     */
    virtual bool ReadBytes (void* pDestination, int RequiredBytes, Msg_List& MsgList) = 0;

    /*!
      @brief writes a given number of bytes
      @return true if call succeeded
     */
    virtual bool WriteBytes (const void* pSource, int ByteCount, Msg_List& MsgList) = 0;
};

#endif
