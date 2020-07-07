/*!*********************************************************************

    @file         SAPDBTrace_HexBuf.hpp

    @author       DanielD

    @brief        converts data to hex for tracing

\if EMIT_LICENCE
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

\endif
***********************************************************************/

#ifndef SAPDBTRACE_HEXBUF_HPP
#define SAPDBTRACE_HEXBUF_HPP


class SAPDB_OStream;

/*!

  @class SAPDBTrace_HexBuf

  @brief converts data to hex for tracing

  The output spans several lines, where each line consists of
  offset (in hex)
  hex dump
  ascii dump

  @code
      char data [] = "ABCD"
      stream << SAPDBTrace_HexBuf (data, 4);
      0000  65 66 67 68                              |ABCD|

 */
class SAPDBTrace_HexBuf
{
public:
    /*!
        @brief create a new hex object
     */
    SAPDBTrace_HexBuf (const void * bufParm, unsigned int lenParm);
    /*!
        @brief create a new hex object with an additional offset
     */
    SAPDBTrace_HexBuf (const void * bufParm, unsigned int offsetParm, unsigned int lenParm);
    SAPDB_OStream & Print (SAPDB_OStream & ostream) const;
protected:
    const unsigned char * buf;
    unsigned int          offset;
    unsigned int          len;
};

inline SAPDB_OStream & operator<< (SAPDB_OStream & ostream, SAPDBTrace_HexBuf const & hex)
{
    return hex.Print (ostream);
}


#endif
