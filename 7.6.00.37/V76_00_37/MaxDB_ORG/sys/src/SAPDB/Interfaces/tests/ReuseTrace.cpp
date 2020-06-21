/*!================================================================
 module:        ReuseTrace.cpp

 -------------------------------------------------------------------

 responsible:   ThomasS

 special area:  Tracing

 description:   FileTraceStream

 -------------------------------------------------------------------

 Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




 ===================================================================*/

#include "SAPDBCommon/SAPDB_ReuseLib.h"

/*----------------------------------------*/

/*!
  Class: PIn_FileOutputStream
 */
class PIn_UnbufferedTextWriter : public isp12_OutputStream
{
public:
    PIn_UnbufferedTextWriter (const char * fname, bool append = false);
    virtual ~PIn_UnbufferedTextWriter ();
    virtual int write (const void * buf, int len);
    virtual int writeln (const void * buf, int len = -1);
    virtual int close ();
    virtual bool isOpen ();

private:
    tsp00_Int4 fileHandle;
};

/*----------------------------------------*/

PIn_UnbufferedTextWriter::PIn_UnbufferedTextWriter (
    const char * fname,
    bool append)
{
    tsp05_RteFileError errInfo;
    tsp05_RteFileMode_Enum fileMode;

    if (append) {
        fileMode = sp5vf_append;
    }
    else {
        fileMode = sp5vf_write;
    }
    sqlfopenc (fname, sp5vf_text, fileMode, sp5bk_unbuffered, &this->fileHandle, &errInfo);
    if (errInfo.sp5fe_result != vf_ok) {
        this->fileHandle = -1;
    }
}

/*----------------------------------------*/

PIn_UnbufferedTextWriter::~PIn_UnbufferedTextWriter ()
{
    this->close ();
}

/*----------------------------------------*/

int PIn_UnbufferedTextWriter::write (const void * buf, int len)
{
    return this->writeln (buf, len);
}

/*----------------------------------------*/

int PIn_UnbufferedTextWriter::writeln (const void * buf, int len)
{
    tsp05_RteFileError errInfo;
    int result;

    sqlfwritec (this->fileHandle, buf, len, &errInfo);
    if (errInfo.sp5fe_result == vf_ok) {
        result = len;
    }
    else {
        result = -1;
    }
    return result;
}

/*----------------------------------------*/

int PIn_UnbufferedTextWriter::close ()
{
    tsp05_RteFileError errInfo;

    if (this->fileHandle != -1) {
        sqlfclosec (this->fileHandle, sp5vf_close_normal, &errInfo);
        this->fileHandle = -1;
    }
    return 0;
}

/*----------------------------------------*/

bool PIn_UnbufferedTextWriter::isOpen ()
{
    return (this->fileHandle != -1);
}

/*----------------------------------------*/

/* PIn_TraceStream * PIn_GlobalTrace = NULL; */
static PIn_UnbufferedTextWriter * outstream = NULL;

/*----------------------------------------*/

externCpp void PIn_openTrace (const char * fname, bool append)
{
    outstream = new PIn_UnbufferedTextWriter (fname, append);
    if (outstream != NULL) {
        if (outstream->isOpen ()) {
            PIn_GlobalTrace = new PIn_TraceStream (outstream);
        }
        else {
            delete outstream;
            outstream = NULL;
        }
    }
}

/*----------------------------------------*/

/*!
  Class: PIn_AutoDelete
 */
class PIn_AutoDelete
{
public:
    ~PIn_AutoDelete ();
};

/*----------------------------------------*/

PIn_AutoDelete::~PIn_AutoDelete ()
{
    if (PIn_GlobalTrace != NULL) {
        delete PIn_GlobalTrace;
    }
    if (outstream != NULL) {
        delete outstream;
    }
}

/*----------------------------------------*/

PIn_AutoDelete autoDelete;

/*----------------------------------------*/

PIn_Tracer::PIn_Tracer (const char * procName)
{
    this->name = procName;
    PIn_GlobalTrace->procEntry (procName);
}

/*----------------------------------------*/

PIn_Tracer::~PIn_Tracer ()
{
    PIn_GlobalTrace->procExit (this->name);
}

/*----------------------------------------*/

void PIn_Tracer::trace (int layer, const char * format, ...)
{
    va_list ap;

    va_start (ap, format);

    PIn_GlobalTrace->vprintf (format, ap);
    va_end (ap);
}

/*----------------------------------------*/


