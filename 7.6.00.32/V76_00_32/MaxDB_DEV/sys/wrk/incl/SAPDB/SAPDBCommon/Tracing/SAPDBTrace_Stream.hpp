/*!
  @file           SAPDBTrace_Stream.hpp
  @author         DanielD
  @brief          an output stream with special methods
              to support tracing.

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


#ifndef SAPDBTRACE_STREAM_HPP
#define SAPDBTRACE_STREAM_HPP

#include "SAPDBCommon/SAPDB_OStream.hpp"

/*!
  @class          SAPDBTrace_LinePrinter
   @brief          an interface to write complete lines.


       This is used be &lt;Class>SAPDBTrace_Stream&lt;/> such
       that concrete instances can be created on the stack
       while still being flexible about the stream target.
       Instances of this class are expected to synchronize
       different tasks writing to it.

 */

class SAPDBTrace_LinePrinter
{
public:
    /* function: PrintLn
       description: writes a line to the output medium
     */
    virtual void PrintLn (const SAPDB_Char * buf, int buflen) = 0;
};




/*!
  @class          SAPDBTrace_Stream
   @brief          a &lt;Class>SAPDB_OStream&lt;/> with special support
          to trace execution in a multitasking environment.


             Different output media are implemented by subclassing
             &lt;Class>SAPDBTrace_LinePrinter&lt;/> and passing such an
             instance to the constructor.

 */


class SAPDBTrace_Stream
    : public SAPDB_OStream
{
public:
    /*!
       @brief          creates a new stream using &lt;Param>printer&lt;/>

     */

    SAPDBTrace_Stream (SAPDBTrace_LinePrinter * printer);

    /*!
       @brief          frees resources

     */

    ~SAPDBTrace_Stream () { Flush(); /* added to PTS 1110953 detected during test... */ }

private:
    /*******************************************************************

      Class: LineBuffer

     ******************************************************************/
    class LineBuffer
        : public SAPDB_StreamBuffer
    {
    public:
        LineBuffer (SAPDBTrace_LinePrinter * printer);
        virtual void Overflow ();
    private:
        SAPDBTrace_LinePrinter * linePrinter;
        char line [132];
    };

    /* EndClass: LineBuffer */

private:
    LineBuffer               buffer;
};




/***********************************************************************

    I M P L E M E N T A T I O N

 **********************************************************************/

inline SAPDBTrace_Stream::LineBuffer::LineBuffer (
    SAPDBTrace_LinePrinter * printer)
:   linePrinter (printer),
    SAPDB_StreamBuffer (line, line + sizeof(line))
{
}

/*----------------------------------------*/

inline SAPDBTrace_Stream::SAPDBTrace_Stream (
    SAPDBTrace_LinePrinter* printer)
:   buffer (printer),
    SAPDB_OStream (buffer)
{
}

#endif

