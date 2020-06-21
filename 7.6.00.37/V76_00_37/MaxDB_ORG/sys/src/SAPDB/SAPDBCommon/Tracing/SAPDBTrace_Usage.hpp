/*!
  @file           SAPDBTrace_Usage.hpp
  @author         DanielD
  @brief          macros und utility

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


#ifndef SAPDBTRACE_USAGE_HPP
#define SAPDBTRACE_USAGE_HPP

#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_MethodBrace.hpp"

#if defined (SAPDB_SLOW)
    /*!
       @brief          traces routine entry and exit


           Insert this macro as the first line of a routine:
       - &lt;Param>name&lt;/> is the name of the routine
       - &lt;Param>topic&lt;/> is a &lt;Class>SAPDBTrace_Topic&lt;/> to test against
       - &lt;Param>level&lt;/> is the level to indicate when this routine         will be traced

     */

    #define SAPDBTRACE_ROUTINE_DEBUG(name, topic, level) \
        SAPDBTrace_MethodBrace methodBrace (name, topic, level)
    /*! enddeclaration */

    /*!
       @brief          traces method entry and exit


           Insert this macro as the first line of a instance method.
           Parameters are the same as SAPDBTRACE_ROUTINE_DEBUG.
           Additionally, the value of the &lt;Param>this&lt;/> pointer
           will appear in the trace.

     */

    #define SAPDBTRACE_METHOD_DEBUG(name, topic, level) \
        SAPDBTrace_MethodBrace methodBrace (name, this, topic, level)
    /*! enddeclaration */


    /*!
       @brief          write expressions to the trace.
       @code           
        
        SAPDBTRACE_WRITELN (SQLParserTrace, 5, var1 << var2);
        
            @endcode        


       - output is supressed when level &lt;Param>level&lt;/> is not         switched on for &lt;Param>topic&lt;/>
       - the output will end in a new line
       - the trace stream will be flushed

     */

    #define SAPDBTRACE_WRITELN(topic, level, traceExpr)         \
        if (topic.TracesLevel (level)) {                         \
            SAPDBTrace_Stream stream (&topic.GetTracePrinter ()); \
            stream << traceExpr << NewLine;                      \
        }
    /*! enddeclaration */

    #define SAPDBTRACE_INDENTLN(topic, level, indent, traceExpr) \
        SAPDBTRACE_WRITELN (topic, level, SAPDB_Indent (indent) << traceExpr)

    /*!
       @brief          write expressions to the trace using an existing stream

       This is more efficient that adding a local stream for every SAPDBTRACE_WRITELN.

       @code

        SAPDBTRACE_STREAMVAR (traceStream, topic);

        SAPDBTRACE_STREAMWRITELN (traceStream, SQLParserTrace, 5, var1 << var2);
        
        @endcode


       - output is supressed when level is not switched on for topic
       - the output will end in a new line
       - the trace stream will be flushed

       Alternative usage if one source uses only one trace topic:

       @code

        #if defined (SAPDB_SLOW)
        #define JOIN_TRACEVAR   SAPDBTrace_Stream traceStream (&Join_Trace.GetTracePrinter ())
        #define JOIN_WRITELN(level, traceExpr)      \
            if (Join_Trace.TracesLevel (level) {    \
                traceStream << traceExpr;
                traceStream.Flush ();
            }
        #else
        #define JOIN_TRACEVAR   (void*);
        #define JOIN_WRITELN(level, traceExpr)
        #endif

        JOIN_TRACEVAR;

        JOIN_WRITELN(5, var1 << var2);
       @endcode
     */

    #define SAPDBTRACE_STREAMVAR(varname, topic)    SAPDBTrace_Stream varname (&topic.GetTracePrinter ())

    #define SAPDBTRACE_STREAMWRITELN(stream, topic, level, traceExpr)         \
        if (topic.TracesLevel (level)) {                         \
            stream << traceExpr;                                 \
            stream.Flush ();                                     \
        }

    /*!
       @brief          &lt;Param>stmt&lt;/> is executed dependent on the trace level
       @code           
        
        SAPDBTRACE_IF (SQLParserTrace, 5, HexDump (SQLParserTrace.GetTracePrinter (), buffer, sizeof (buffer));
        
            @endcode        

     */

    #define SAPDBTRACE_IF(topic, level, stmt)                   \
        if (topic.TracesLevel (level)) {                             \
            stmt;                                               \
        }
        /*! enddeclaration */
#else
    #define SAPDBTRACE_ROUTINE_DEBUG(name, topic, level)
    #define SAPDBTRACE_METHOD_DEBUG(name, topic, level)
    #define SAPDBTRACE_WRITELN(topic, level, traceExpr)
    #define SAPDBTRACE_STREAMVAR(varname, topic)
    #define SAPDBTRACE_STREAMWRITELN(traceStream, topic, level, traceExpr)
    #define SAPDBTRACE_IF(topic, level, stmt)
#endif

#endif

