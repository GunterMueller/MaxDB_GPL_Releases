/*! 
    \file    DBMSrvBHist_File.hpp
    \author  TiloH
    \ingroup handling of backup history files by the DBMServer
    \brief   a class for reading the backup history files

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

#if !defined(DBMSrvBHist_File_hpp)
#define DBMSrvBHist_File_hpp


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/DBMSrv_Reply.hpp"


//-----------------------------------------------------------------------------
// declaration of class DBMSrvBHist_File
//-----------------------------------------------------------------------------

class DBMSrvBHist_File
{
  public:
    enum ExistFlag{MustExist, CanBeAbsent};

    DBMSrvBHist_File(const tsp00_Pathc & FileType, ExistFlag MustExistFlag);
    ~DBMSrvBHist_File();

    tcn00_Error readFile(DBMSrv_Reply & Reply);
    tcn00_Error readFile(const tsp00_Pathc FileName, DBMSrv_Reply & Reply);

    const tsp00_Pathc & getFileName();

  protected:
    enum CloseFileFlag{ReportError, DoNotReportError};

    char   * line;
    size_t   lineLength;
    int      m_LineNumber; //!< number of line in the file

    tsp00_Pathc m_fileName;//!< name of the file, if opened

    tcn00_Error openFile (DBMSrv_Reply & Reply);
    tcn00_Error closeFile(DBMSrv_Reply & Reply, CloseFileFlag ReportErrorFlag);

  private:
    tcn00_Error         openFile        (const tsp00_Pathc FileName, DBMSrv_Reply & Reply);
    void                readFileWorkPart(tcn00_Error & rc, DBMSrv_Reply & reply);
    tcn00_Error         readNextTextLine(DBMSrv_Reply & Reply, int & ReachedEndOfFile);
    virtual tcn00_Error useReadLine     (DBMSrv_Reply & Reply)=0; //!< virtual function used by derived classes, to store values from a line

    tsp00_Pathc fileType;
    int         fileOpened;
    tsp00_Int4  fileDescriptor;

    ExistFlag mustExistFlag;
};

#endif
