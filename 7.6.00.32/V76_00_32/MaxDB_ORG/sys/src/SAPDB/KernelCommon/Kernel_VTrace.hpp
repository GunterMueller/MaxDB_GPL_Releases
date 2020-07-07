  /*!
  @file           Kernel_VTrace.hpp
  @author         DanielD
  @brief          Stream to write formatted vtrace entries

\if EMIT_LICENCE
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

\endif
*/



#ifndef KERNEL_VTRACE_HPP
#define KERNEL_VTRACE_HPP

#include "ggg00.h"
#include "SAPDBCommon/SAPDB_OStream.hpp"

/*!
   @class          Kernel_VTrace
   @brief          stream interface to vtrace.
   
   @code           
    
    // write two lines to the vtrace
    Kernel_VTrace () << "line1" << NewLine << line2;
    // the second new line is implicit when the stream is destructed
    
   @endcode        


     Stream objects are instantiated on the stack rather than
     shared between tasks.
     A new line is written to the vtrace when
       - a '\n' character is found in the stream buffer
       - the buffer size is exceeded
       - the stream object is destructed
     Warning: For performance reasons, strings should be formatted using
     the classes SAPDB_LeftJ and SAPDB_RightJ. Unlike
     SAPDB_ToString, these classes will not copy the string, but
     rather keep only the pointer.
     Warning: Never use multiple temporary Kernel_VTrace () objects in
     the same scope. They will be destructed (and flushed) in the reverse
     order, thus showing all output lines in the reverse order.

 */

class Kernel_VTrace
    : public SAPDB_OStream
{
public:
    Kernel_VTrace ();
    ~Kernel_VTrace ();
private:
    /*******************************************************************

      Class: LineBuffer

     ******************************************************************/
    class LineBuffer
        : public SAPDB_StreamBuffer
    {
    public:
        LineBuffer ();
        virtual void Overflow ();
    private:
        char line [132];
        tgg00_TransContext transContext;
    };

    /* EndClass: LineBuffer */

private:
    LineBuffer         buf;
};



/*
    function: FlushLine
    description: flushes the stream and breaks to a new line

        Without this manipulator, the trace will be flushed at
        arbitrary times, this introducing many unwanted line breaks.

    example:
        Kernel_VTrace trace;
        trace << "line 1" << FlushLine;
        trace << "line 2" << FlushLine;
 */
extern SAPDB_OStream& FlushLine(SAPDB_OStream& os);

#endif // KERNEL_EXCEPTION_HPP
