/*!*********************************************************************

  module:       SAPDBTrace_FileTrace.cpp

  ----------------------------------------------------------------------

  responsible:  DanielD

  special area: SAPDB Tracing

  description:  writes trace output into file.

  ----------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




  *********************************************************************/

#include <stdio.h>

#include "SAPDBCommon/Tracing/SAPDBTrace_FileTrace.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

class FilePrinter : public SAPDBTrace_LinePrinter
{
public:
    FilePrinter (const char * filename);
    ~FilePrinter ();
    /* function: PrintLn
       description: writes a line to the output medium
     */
    virtual void PrintLn(const SAPDB_Char *buf, int buflen);
private:
    FILE * ostream;
};

/*----------------------------------------*/

/* function: PrintLn
   description: writes a line to the output medium
 */
void FilePrinter::PrintLn(const SAPDB_Char *buf, int buflen)
{
    fwrite (buf, 1, buflen, this->ostream);
    fflush (this->ostream);
}

/*----------------------------------------*/

SAPDBTrace_Stream * SAPDBTrace_NewFileTrace (
    const char * filename,
    SAPDBMem_IRawAllocator * allocator)
{
    FilePrinter * printer;
    SAPDBTrace_Stream * result;
    if (allocator != NULL) {
        printer = new (*allocator) FilePrinter (filename);
        result = new (*allocator) SAPDBTrace_Stream (printer);
    }
    else {
        printer = new FilePrinter (filename);
        result = new SAPDBTrace_Stream (printer);
    }
    return result;
}

