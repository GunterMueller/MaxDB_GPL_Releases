/*!
  @file           SAPDBTrace_FileTrace.hpp
  @author         DanielD
  @brief          writes trace output into file.

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


#ifndef SAPDBTRACE_FILETRACE_HPP
#define SAPDBTRACE_FILETRACE_HPP

class SAPDBTrace_Stream;
class SAPDBMem_IRawAllocator;

/*!
   @brief          creates a new file based SAPDBTrace_Stream
   @param          filename [in] the name of the trace file.
   @param          allocator_param [in,out] the SAPDBMem_IRawAllocator used
        to allocate the instance.


       The file is overwritten, not appended.
       This is usable only for test programs as there is currently
       no way to free the associated ressources.

 */

SAPDBTrace_Stream * SAPDBTrace_NewFileTrace (
    const char * filename,
    SAPDBMem_IRawAllocator * allocator_param);

#endif

