/*
  -----------------------------------------------------------------------------
  module: vls12.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Cursor
                
  version:      7.6.
  -----------------------------------------------------------------------------
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

*/

#include "gls00.h"
#include "hls03.h"
#include "hls04.h"
#include "hls041.h"
#include "hls07.h"
#include "hls12.h"
#include "hls13.h"
#include "hls98msg.h"
#include "hls99.h"


/*
  -----------------------------------------------------------------------------
  function:     ls12DeclareCursor
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12DeclareCursor(tls00_DBInfo &DBInfo,
                  tsp00_Addr    pszResultTabName,
                  tsp00_Addr    pszSelectStmt,
                  tsp00_Addr    pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls12DeclareCursor");

    tsp00_Int4 rc       = errOK_els00;
    tsp00_Int4 ErrorPos = 0;

    _TCHAR *pszStmt     = NULL;
    size_t  lStmtLength = 0;

    if (NULL != pszResultTabName)
    {
        lStmtLength = _tcslen(pszResultTabName) + _tcslen(pszSelectStmt) + 64;
        pszStmt = new _TCHAR[lStmtLength];
        pszStmt[0] = '\0';

        _stprintf(pszStmt, _T("DECLARE %s CURSOR FOR %s"), pszResultTabName, pszSelectStmt);
    }
    else
    {
        pszStmt = pszSelectStmt;
    }

    rc = ls13ExecIntSQLStmt(&DBInfo, 0, pszStmt, pszErrText, ErrorPos, true);    // with info

    if (NULL != pszResultTabName)
    {
        delete [] pszStmt;
        pszStmt = NULL;
    }

    return rc;
}
// ls12DeclareCursor()


/*
  -----------------------------------------------------------------------------
  function:     ls12Fetch
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12Fetch(tls00_DBInfo &DBInfo,
          tsp00_Addr    pszResultTabName,
          tsp00_Int4   &RowCount,
          bool          bMassCmd,
          tsp00_Addr    pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls12Fetch");

    LoaderMessages*  MsgInstance = LoaderMessages::Instance();    // error reporting obj

    tsp00_Int4 rc = errOK_els00;

    //*
    //*     Build packet
    //*
    // The statement to build looks like this: 'FETCH POS (<number>) <result table name>'
    // The length sums up as
    //    the length of result table name (= sizeof(tsp00_KnlIdentifierc))
    //  + the length of the other words like FETCH ...
    // --> So 128 byte should be enough
    _TCHAR szStmt[128*sizeof(char)] = {'F','E','T','C','H',' '};
    if (NULL != pszResultTabName)
    {
        strcat(szStmt, pszResultTabName);
    }

    tls00_Parts4InternalSql PartStruct;
    PartStruct.pfiCount_ls00    = 2;
    PartStruct.pfiPartKind_ls00 = new tsp1_part_kind_Enum[PartStruct.pfiCount_ls00];
    PartStruct.pfiData_ls00     = new tls00_String[PartStruct.pfiCount_ls00];
    
    //*     command part
    PartStruct.pfiPartKind_ls00[0] = sp1pk_command;
    PartStruct.pfiData_ls00[0].strAddr_ls00 = szStmt;
    PartStruct.pfiData_ls00[0].strLeng_ls00 = STATIC_CAST(tsp00_Int4, _tcslen(szStmt) );

    //*     Result count part
    PartStruct.pfiPartKind_ls00[1] = sp1pk_resultcount;
    PartStruct.pfiData_ls00[1].strLeng_ls00 = mxsp_resnum;
    PartStruct.pfiData_ls00[1].strAddr_ls00 = new char[PartStruct.pfiData_ls00[1].strLeng_ls00];    // not null terminated
    PartStruct.pfiData_ls00[1].strAddr_ls00[0] = DEF_BYTE_LS00;
    rc = ls07IntToNumber(RowCount, &PartStruct.pfiData_ls00[1].strAddr_ls00[1]);

    // run fetch command
    tsp00_Int4 lErrorPos = 0;
    _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];

    rc = ls13ExecIntSQLMultParts(&DBInfo, 0, PartStruct, szErr, lErrorPos, false, sp1m_dbs, bMassCmd);

    delete [] PartStruct.pfiData_ls00[1].strAddr_ls00;
    delete [] PartStruct.pfiPartKind_ls00;
    delete [] PartStruct.pfiData_ls00;

    if (errOK_els00 == rc)
    {
        // Count of fetched rows = argument count of data part (result count is accumulated value)
        tsp1_part *pSqlPart = NULL;
        bool       Res     = ls04GetPart(DBInfo.dbiPktRcvList_ls00[0], sp1pk_data, pSqlPart);
        if (true == Res)
        {
            RowCount = ls04GetPartArgCount(pSqlPart);
        }
        else
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errUnknownPart_els98);
            rc = errUnknownPart_els98;
        }
    }
    else
    {
        if (dbRowNotFound_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternSQL_els98, szStmt, szErr);
            rc = errInternSQL_els98;
        }
    }
    return rc;
}
// ls12Fetch()

/*
  -----------------------------------------------------------------------------
  function:     ls12FetchEx
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12FetchEx(tls00_DBInfo  &DBInfo,
            tsp00_Addr     pszResultTabName,
            tsp00_Int4    &RowCount,
            bool           bMassCmd,
            _TCHAR       *&pszBuffer,
            tsp00_Int4    &lBufferLen,
            tsp00_Addr     pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls12FetchEx");

    tsp00_Int4 rc = errOK_els00;

    rc = ls12Fetch(DBInfo, pszResultTabName, RowCount, bMassCmd, pszErrText);
    if (errOK_els00 == rc)
    {
        // Get pointer to data in data part of packet; return code does not 
        // matter, at this point we know that data part exists
        tsp1_packet *pSqlPacket = DBInfo.dbiPktRcvList_ls00[0];
        ls04GetRawDataPtr(pSqlPacket, pszBuffer, lBufferLen, sp1pk_data);
    }

    return rc;
}
// ls12FetchEx()

/*
  -----------------------------------------------------------------------------
  function:     ls12FetchPos
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12FetchPos(tls00_DBInfo &DBInfo,
             tsp00_Addr    pszResultTabName,
             tsp00_Int4   &RowCount,
             tsp00_Longint Pos,
             bool          bMassCmd,
             tsp00_Addr    pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls12FetchPos");

    tsp00_Int4 rc = errOK_els00;

    //*
    //*     Build packet
    //*

    // The statement to build looks like this: 'FETCH POS (<number>) <result table name>'
    // The length sums up as
    //    the length of result table name (= sizeof(tsp00_KnlIdentifierc))
    //  + the length of the row pos (max 20 digits - thats a 64 bit integer)
    //  + the length of the other words like FETCH ...
    // --> So 256 byte should be enough
    _TCHAR szStmt[256*sizeof(char)] = {'F','E','T','C','H',' ','P','O','S',' ','('};
    _TCHAR szPosBuffer[32*sizeof(char)];

    // Make the position a string to put it into the statement
    _itot(STATIC_CAST(tsp00_Int4, Pos), szPosBuffer, 10);
    strcat(szStmt, szPosBuffer );

    // Append the result table name
    if (NULL != pszResultTabName)
    {
        strcat(szStmt, ") ");
        strcat(szStmt, pszResultTabName);
    }

    tls00_Parts4InternalSql PartStruct;
    PartStruct.pfiCount_ls00    = 2;
    PartStruct.pfiPartKind_ls00 = new tsp1_part_kind_Enum[PartStruct.pfiCount_ls00];
    PartStruct.pfiData_ls00     = new tls00_String[PartStruct.pfiCount_ls00];
    
    //*     command part
    PartStruct.pfiPartKind_ls00[0] = sp1pk_command;
    PartStruct.pfiData_ls00[0].strAddr_ls00 = szStmt;
    PartStruct.pfiData_ls00[0].strLeng_ls00 = STATIC_CAST(tsp00_Int4, _tcslen(szStmt) );

    //*     Result count part
    PartStruct.pfiPartKind_ls00[1] = sp1pk_resultcount;
    PartStruct.pfiData_ls00[1].strLeng_ls00 = mxsp_resnum;
    PartStruct.pfiData_ls00[1].strAddr_ls00 = new char[PartStruct.pfiData_ls00[1].strLeng_ls00];    // not null terminated
    PartStruct.pfiData_ls00[1].strAddr_ls00[0] = DEF_BYTE_LS00;
    rc = ls07IntToNumber(RowCount, &PartStruct.pfiData_ls00[1].strAddr_ls00[1]);

    // run fetch command
    tsp00_Int4 lErrorPos = 0;
    _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];

    rc = ls13ExecIntSQLMultParts(&DBInfo, 0, PartStruct, szErr, lErrorPos, false, sp1m_dbs, bMassCmd);

    delete [] PartStruct.pfiData_ls00[1].strAddr_ls00;
    delete [] PartStruct.pfiPartKind_ls00;
    delete [] PartStruct.pfiData_ls00;

    if (errOK_els00 == rc)
    {
        // Count of fetched rows = argument count of data part (result count is accumulated value)
        tsp1_packet *SqlPacket = DBInfo.dbiPktRcvList_ls00[0];
        tsp1_part   *SqlPart   = NULL;
        bool         Res       = ls04GetPart(SqlPacket, sp1pk_data, SqlPart);
        if (true == Res)
        {
            RowCount = ls04GetPartArgCount(SqlPart);
        }
        else
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errUnknownPart_els98);
            rc = errUnknownPart_els98;
        }
    }
    else
    {
        if (dbRowNotFound_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternSQL_els98, szStmt, szErr);
            rc = errInternSQL_els98;
        }
    }

    return rc;
}
// ls12FetchPos()


/*
  -----------------------------------------------------------------------------
  function:     ls12CloseCursor
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls12CloseCursor(tls00_DBInfo &DBInfo,
                tsp00_Addr    pszResultTabName,
                tsp00_Addr    pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls12CloseCursor");

    tsp00_Int4 rc       = errOK_els00;
    tsp00_Int4 ErrorPos = 0;

    //* Build the statement

    // The statement to build looks like this: 'FETCH POS (<number>) <result table name>'
    // The length sums up as
    //    the length of result table name (= sizeof(tsp00_KnlIdentifierc))
    //  + the length of the other words like FETCH ...
    // --> So 128 byte should be enough
    _TCHAR szStmt[128*sizeof(char)] = {'C','L','O','S','E',' '};
    strcat(szStmt, pszResultTabName);

    rc = ls13ExecIntSQLStmt(&DBInfo, 0, szStmt, pszErrText, ErrorPos);
    return rc;
}
// ls12CloseCursor()


/*
  -----------------------------------------------------------------------------
  function: ls12CreateResultTabName
  -----------------------------------------------------------------------------
*/
tsp00_Uint4
ls12CreateResultTabName(tsp00_Addr pszRTabName, tsp00_Int4 lRTabNameLen)
{
    ROUTINE_DBG_MEO00 ("ls12CreateResultTabName");

    pszRTabName[0] = 0;

    tsp00_Date Date;
    tsp00_Time Time;
    sqldattime(Date, Time);
    
    tsp00_Datec szDate;
    tsp00_Timec szTime;
    szDate.p2c(Date);
    szTime.p2c(Time);

    // Name of result table: LOA_<DATE><TIME>
    return (sp77sprintf(pszRTabName, lRTabNameLen, "%s_%s%s", APPL_NAME_LS00,
                                                              szDate.asCharp(), szTime.asCharp()) );
}
// ls12CreateResultTabName
