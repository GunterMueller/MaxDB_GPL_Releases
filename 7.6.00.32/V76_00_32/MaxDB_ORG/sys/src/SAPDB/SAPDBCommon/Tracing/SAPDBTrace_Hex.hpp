/*!*********************************************************************

    @file         SAPDBTrace_Hex.hpp
    
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

#ifndef SAPDBTRACE_HEX_HPP
#define SAPDBTRACE_HEX_HPP

class SAPDB_OStream;

/*!

  @class SAPDBTrace_Hex
  
  @brief converts data to hex for tracing
  
  @code 
      char data [] = "ABCD"
      stream << "data: " << SAPDBTrace_Hex (data, 5);
      result: "data: 6566676800"

 */
class SAPDBTrace_Hex
{
public:
    /*!
        @brief create a new hex object
     */
    SAPDBTrace_Hex (const void * bufParm, unsigned int lenParm);
    /*!
        @brief write the hex representation of the buffer to the stream
     */
    SAPDB_OStream & Print (SAPDB_OStream & ostream) const;
protected:
    const unsigned char * buf;
    unsigned int          len;
};

/*!
    @brief shortcut for SAPDBTrace_Hex (&item, sizeof (item))
 */
#define SAPDBTRACE_HEX(item) SAPDBTrace_Hex (&item, sizeof (item))

inline SAPDB_OStream & operator<< (SAPDB_OStream & ostream, SAPDBTrace_Hex const & hex)
{
    return hex.Print (ostream);
}

#endif

