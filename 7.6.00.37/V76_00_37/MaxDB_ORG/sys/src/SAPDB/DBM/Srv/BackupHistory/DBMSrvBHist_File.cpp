/*! 
    \file    DBMSrvBHist_File.cpp
    \author  TiloH
    \ingroup handling of backup history files by the DBMServer
    \brief   definition of a class for reading the backup history files

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "hcn36.h"
#include "heo06.h"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"


//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const size_t DBMSrvBHist_File_EXPECTED_LINE_LENGTH=1024;

//-----------------------------------------------------------------------------
// member of class DBMSrvBHist_File
//-----------------------------------------------------------------------------

DBMSrvBHist_File::DBMSrvBHist_File(const tsp00_Pathc & FileType, DBMSrvBHist_File::ExistFlag MustExistFlag)
  :line(0),
   lineLength(0),
   m_LineNumber(0),
   fileType(FileType),
   fileOpened(0),
   fileDescriptor(0),
   mustExistFlag(MustExistFlag)
{
    m_fileName.rawAssign("");
}

DBMSrvBHist_File::~DBMSrvBHist_File()
{
    if(fileOpened)
    {
        tcn00_Error        Rc=OK_CN00;
        tsp05_RteFileError RTEFileError;

        sqlfclosec(fileDescriptor, sp5vf_close_normal, &RTEFileError);
    }

    cn36_StrDealloc(line);
}

const tsp00_Pathc & DBMSrvBHist_File::getFileName()
{
    return m_fileName;
}

tcn00_Error DBMSrvBHist_File::openFile(const tsp00_Pathc FileName, DBMSrv_Reply & Reply)
{
    tcn00_Error        Rc=OK_CN00;
    tsp05_RteFileError RTEFileError;

    m_fileName.rawAssign(FileName);

    // open file
    if(OK_CN00==Rc)
    {
        sqlfopenc(FileName, sp5vf_text, sp5vf_read, sp5bk_buffered, &fileDescriptor, &RTEFileError);

        if(vf_ok!=RTEFileError.sp5fe_result)
            Rc=Reply.startWithMessageList(DBMSrvMsg_RTEError(RTEFileError));
        else
            fileOpened=1;
    }

    if(fileOpened && 0==line)   //file is opened, but read buffer has no memory yet
    {
        if(cn36_StrAlloc(line, DBMSrvBHist_File_EXPECTED_LINE_LENGTH))    //get memory for a line used with sqlfreadc(), it is expanded later if needed
            lineLength=DBMSrvBHist_File_EXPECTED_LINE_LENGTH;
        else
            Rc=Reply.startWithError(ERR_MEM_CN00);
    }

    return Rc;
}

tcn00_Error DBMSrvBHist_File::readFile(const tsp00_Pathc FileName, DBMSrv_Reply & Reply)
{
    tcn00_Error Rc=OK_CN00;

    Rc=openFile(FileName, Reply);
    readFileWorkPart(Rc, Reply);

    return Rc;
}

void DBMSrvBHist_File::readFileWorkPart(tcn00_Error & rc, DBMSrv_Reply & reply)
{
    if(OK_CN00==rc)
    {
        int reachedEndOfFile=0;

        m_LineNumber=1; //files start at line number one at least in devenv

        while(OK_CN00==rc && !reachedEndOfFile)
        {
            rc=readNextTextLine(reply, reachedEndOfFile);

            if(OK_CN00==rc && !reachedEndOfFile && '\0'!=line[0])   //ignore empty lines
                rc=useReadLine(reply);

            m_LineNumber++;
        }

        if(OK_CN00==rc)
            rc=closeFile(reply, ReportError);
        else
            closeFile(reply, DoNotReportError);
    }
    else
    {
        if(DBMSrvBHist_File::CanBeAbsent==mustExistFlag)
            rc=OK_CN00; // if external backup history and its copy are not present -> it's no error, all are EBIDs are simply unknown
    }
}

tcn00_Error DBMSrvBHist_File::readNextTextLine(DBMSrv_Reply & Reply, int & ReachedEndOfFile)
{
    tcn00_Error        Rc=OK_CN00;
    tsp00_Longint      BytesRead=0;
    tsp05_RteFileError FileError;

    int    LineIncomplete=1;
    size_t Used=0;

    while(OK_CN00==Rc && LineIncomplete)
    {
        sqlfreadc(fileDescriptor, line+Used, lineLength-Used, &BytesRead, &FileError);

        if(vf_ok!=FileError.sp5fe_result)
        {
            LineIncomplete=0;   //no more data to read, end while loop

            if(vf_eof==FileError.sp5fe_result)
                ReachedEndOfFile=1;
            else
                Rc=Reply.startWithMessageList(DBMSrvMsg_RTEError(FileError));
        }
        else
        {
            if(sp5vfw_no_warning==FileError.sp5fe_warning)
                LineIncomplete=0; // we could read a complete line into Line without errors or warnings, end the while loop
            else
                if(sp5vfw_no_eol_found==FileError.sp5fe_warning)   //line was to big for current buffer
                {
                    char * BiggerLine=0;

                    if(cn36_StrAlloc(BiggerLine, 2*lineLength))    //allocate more memory
                    {
                        SAPDB_strcpy(BiggerLine, line);   //copy the part of the line read so far
                    
                        cn36_StrDealloc(line);         //forget Line and us BiggerLine as Line
                        line=BiggerLine;
                        lineLength*=2;

                        Used=strlen(BiggerLine);
                    }
                    else
                        Rc=Reply.startWithError(ERR_MEM_CN00);
                }
                else    //we don't know about other warnings, report them as errors
                {
                    LineIncomplete=0;   //no more data to read, end while loop
                    Rc=Reply.startWithMessageList(DBMSrvMsg_RTEError(FileError));
                }
        }
    }

    return Rc;
}

tcn00_Error DBMSrvBHist_File::closeFile(DBMSrv_Reply & Reply, CloseFileFlag ReportErrorFlag)
{
    tcn00_Error        Rc=OK_CN00;
    tsp05_RteFileError RTEFileError;

    sqlfclosec(fileDescriptor, sp5vf_close_normal, &RTEFileError);

    if(RTEFileError.sp5fe_result!=vf_ok && ReportError==ReportErrorFlag)
        Rc=Reply.startWithMessageList(DBMSrvMsg_RTEError(RTEFileError));
    else
        fileOpened=0;

    return Rc;
}
