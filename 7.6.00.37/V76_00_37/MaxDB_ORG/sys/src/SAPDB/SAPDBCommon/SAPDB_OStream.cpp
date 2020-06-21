/*!******************************************************************************

  module:		 SAPDB_OStream.cpp

  ------------------------------------------------------------------------------

  responsible:  DanielD

  special area: Stream output

  description:  Definition of classes for stream output

  ------------------------------------------------------------------------------

  copyright:               Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




*******************************************************************************/


#include "SAPDBCommon/SAPDB_OStream.hpp"

#include "RunTime/RTE_CompilerFeatures.h"

// Special buffer for console output
// Each character goes directly to the screen, because the buffer is
// just one byte large
class SAPDB_ConsoleBuffer : public SAPDB_StreamBuffer
{
private:
   SAPDB_Char m_Char;
public:
   SAPDB_ConsoleBuffer() : SAPDB_StreamBuffer(&m_Char, (&m_Char) + 1)  {}
   ~SAPDB_ConsoleBuffer() {}
   virtual void Overflow();
};

/*----------------------------------------*/

void SAPDB_ConsoleBuffer::Overflow()
{
    fwrite (this->Begin (), 1, this->End () - this->Begin (), stdout);
}


/*----------------------------------------*/

SAPDB_ConsoleBuffer ConsoleBuffer;


SAPDB_OStream Console(ConsoleBuffer);


/*----------------------------------------*/
/* This implementation cannot be used on hp platforms due to
   an ambuguity with char*.

SAPDB_OStream& SAPDB_OStream::operator << (const void * ptr)
{
    SAPDB_Char buf [20];
    SAPDB_Char * current = buf + sizeof (buf) - 1;
    SAPDB_ULong asULong = REINTERPRET_CAST (SAPDB_ULong, ptr);
    static const SAPDB_Char hexDigits [] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    *current = '\0';
    --current;
    for (int i = 0; i < sizeof (asULong) * 2; ++i) {
        *current = hexDigits [asULong % 16];
        --current;
        asULong /= 16;
    }
    *current = 'x';
    --current;
    *current = '0';
    *this << current;
    return *this;
}
*/

/*----------------------------------------*/

// Implementation of manipulators
SAPDB_OStream& NewLine(SAPDB_OStream& os)
{
   return os << "\n";
}

/*----------------------------------------*/

// Implementation of manipulators
SAPDB_OStream& FlushLine(SAPDB_OStream& os)
{
    os.Flush ();
    return os;
}

/*----------------------------------------*/

/* workaround for possible compiler bug on Linux on PPC */

#if !defined(SDBonPPC64)
SAPDB_OStream& SAPDB_Blanks::ToStream(SAPDB_OStream& os) const
{
    static const int maxBlanksC = 64;
    static const char * blanksC =
        "                                                                ";
    int blanksRemaining = m_BlankCount;
    while (blanksRemaining > 0) {
        int thisChunk = (blanksRemaining > maxBlanksC)
            ? maxBlanksC : blanksRemaining;
        os << (blanksC + maxBlanksC - thisChunk);
        blanksRemaining -= thisChunk;
    }
    return os;
}
#endif

/*----------------------------------------*/


