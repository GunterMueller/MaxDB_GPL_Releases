/* 
  -----------------------------------------------------------------------------
  module: vls03.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Functions to connect to the database
                
  version:      7.6.
  -----------------------------------------------------------------------------
    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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
#include "hls99.h"
#include "hls98msg.h"


tls00_Performance performance ;
/*  
  -----------------------------------------------------------------------------
  Chapter: Private functions declaration
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  function:     ls03_ConnectDB
  -----------------------------------------------------------------------------

  description:  Connects server to db kernel.

  arguments:    DBInfo   [IN]     - load session info structure 
                ErrText  [OUT]    - Error text

  returnvalue:  errOK_els00           - everything went fine
                errDbHost_els98       - db host not found
                errSqlConnectDb_els98 - no connection possible

                Returns only error codes. The error text that is given by the
                runtime environment is saved in DBInfo which needs to be queried
                by the caller.
  -----------------------------------------------------------------------------
*/
static int
ls03_ConnectDB(tls00_DBInfo* DBInfo);


/*
  -----------------------------------------------------------------------------
 
  function:     ls03_InitPacket

  -----------------------------------------------------------------------------

  description:  FIRST TIME INITIALIZATION of a communication packet.
                The initialized header data is saved into a structure which
                allows later on a quick reinitialization of packets.
                
                Defines swap behavior of server. Adapts name and version of
                application in packet to LOAD. Adjusts SegmentKind and part
                kind to command.

  arguments:    DBInfo   [IN]     - load session info structure 

  returnvalue:  errOK_els00       - everything went fine
                errors during initialization of part or segment
  -----------------------------------------------------------------------------
*/
static int
ls03_InitPacket(tls00_DBInfo* DBInfo);


/*  
  -----------------------------------------------------------------------------
  EndChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  Chapter: Public functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls03SQLConnect
  -----------------------------------------------------------------------------
*/
int
ls03SQLConnect(tls00_DBInfo *DBInfo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls03SQLConnect");

    SAPDB_Int4 rc = errOK_els00;
    
    // Reset database error text
    DBInfo->dbiSqlErr_ls00.SetZero();

    // Release existing db connection
    if (true == DBInfo->dbiSession_ls00.siDBConnection_ls00)
    {
        ls03SQLRelease (DBInfo);
    }

    // New connection to database
    rc = ls03_ConnectDB (DBInfo);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(laySQLConnect_els98, ErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                           STATIC_CAST(int, DBInfo->dbiSqlRc_ls00),
                                                                           DBInfo->dbiSqlErr_ls00.length(),
                                                                           DBInfo->dbiSqlErr_ls00.asCharp() );
    }
    else
    {
        // Initialize SQL packet
        rc = ls03_InitPacket(DBInfo);
        if ( errOK_els00 == rc )
        {
            TRACE_INFO_MLS99("ls03SQLConnect", "", DBInfo);
        }
        else
        {
            ls03SQLRelease(DBInfo);
            LoaderMessages::Instance()->ls98Msg(laySQLConnect_els98, ErrText, errInitPacket_els98, rc);
        }
    }
    return rc;
}
// ls03SQLConnect()


/*
  -----------------------------------------------------------------------------
   function:     ls03SQLRelease
  -----------------------------------------------------------------------------
*/
void
ls03SQLRelease (tls00_DBInfo *DBInfo)
{
    ROUTINE_DBG_MEO00 ("ls03SQLRelease");

    // Release existing connection
    if (true == DBInfo->dbiSession_ls00.siDBConnection_ls00)
    {
        sqlarelease(DBInfo->dbiSqlRef_ls00);        // returns no error

        // Flag that no connection exists
        DBInfo->dbiSession_ls00.siDBConnection_ls00 = false;
    }

}
// ls03SQLRelease()


/*
  -----------------------------------------------------------------------------
  function:     ls03ProcessSQLPacket
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls03ProcessSQLPacket(tls00_DBInfo *pDBInfo,
                     SAPDB_Int4    lPcktNo,
                     SAPDB_Int4   &lErrPos,
                     tsp00_Addr    pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls03ProcessSQLPacket");

    SAPDB_Int4 rc = errOK_els00;

    rc = ls03SQLRequest (pDBInfo, lPcktNo, pszErrText);     // send SQLPacket
    if (errOK_els00 == rc)
    {
        rc = ls03SQLReceive (pDBInfo, lPcktNo, pszErrText); // receive SQLPacket
        if (errOK_els00 == rc)
        {
            // Analyse error status of sent packet
            rc = ls04RcFromPacket(pDBInfo->dbiPktRcvList_ls00[lPcktNo], pszErrText, lErrPos);
            if (errOK_els00 != rc)
            {
                pDBInfo->dbiPktRc_ls00 = rc;                // Set return code in info structure
            }
        }
    }

    return rc;
}
// ls03ProcessSQLPacket()


/*
  -----------------------------------------------------------------------------
  function:     ls03SQLRequest
  -----------------------------------------------------------------------------
*/
int
ls03SQLRequest(tls00_DBInfo *DBInfo, SAPDB_Int4 PacketNo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls03SQLRequest");

    if (false == DBInfo->dbiSession_ls00.siDBConnection_ls00)
    {
        return errOK_els00;
    }

    SAPDB_Int4  rc          = errOK_els00;
    tsp1_packet *SqlPacket  = DBInfo->dbiPktSndList_ls00[PacketNo];
    SAPDB_Int4  PacketLen   = SqlPacket->sp1_header.sp1h_varpart_len + sizeof (tsp1_packet_header);

    DBInfo->dbiSqlErr_ls00.SetZero();

    TRACE_PCKT_MLS99("ls03SQLRequest", SqlPacket);

    //*
    //* Performance
    //*
    extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;

    start =  RTESys_MicroSecTimer();

    sqlarequest(DBInfo->dbiSqlRef_ls00,     // send SQLPacket
                SqlPacket,
                PacketLen,
                DBInfo->dbiSqlErr_ls00,
                DBInfo->dbiSqlRc_ls00); 

    finish   =  RTESys_MicroSecTimer();
    duration = finish - start;
    performance.perRequest_ls00 += duration ;

    DBInfo->dbiSession_ls00.siReconnect_ls00.rcoLastSuccessfulKnlContact_ls00 = start;

    if (0 != DBInfo->dbiSqlRc_ls00)         // error at request
    {
        LoaderMessages::Instance()->ls98Msg(laySQLConnect_els98, ErrText, errDbComm_els98,
                                                                           "request",
                                                                           STATIC_CAST(int, DBInfo->dbiSqlRc_ls00),
                                                                           DBInfo->dbiSqlErr_ls00.length(),
                                                                           DBInfo->dbiSqlErr_ls00.asCharp());
        rc = errDbComm_els98;

        if ( (DBInfo->dbiSqlRc_ls00 >= commErrTimeout_esp01) && (DBInfo->dbiSqlRc_ls00 <= commErrShutdown_esp01))
        {
            DBInfo->dbiSession_ls00.siDBConnection_ls00 = false;
        }
    }

    return rc;
}
// ls03SQLRequest()


/*
  -----------------------------------------------------------------------------
  function:     ls03SQLReceive
  -----------------------------------------------------------------------------
*/
int
ls03SQLReceive(tls00_DBInfo *DBInfo, SAPDB_Int4 PacketNo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls03SQLReceive");

    if (false == DBInfo->dbiSession_ls00.siDBConnection_ls00)
    {
        return errOK_els00;
    }

    SAPDB_Int4  rc = errOK_els00;
    tsp1_packet *SqlPacket;
    SAPDB_Int4  PacketLen;

    DBInfo->dbiSqlErr_ls00.SetZero(); // reset db error text

    //*
    //* Performance
    //*
    extern tls00_Performance performance;
    SAPDB_UInt8    start, finish;
    SAPDB_UInt8    duration;

    start =  RTESys_MicroSecTimer();

    sqlareceive(DBInfo->dbiSqlRef_ls00,     // receive SQLPacket
                (void**)&SqlPacket,
                PacketLen, 
                DBInfo->dbiSqlErr_ls00,
                DBInfo->dbiSqlRc_ls00);

    finish = RTESys_MicroSecTimer();

    duration = finish - start;

    performance.perReceive_ls00 += duration ;

    DBInfo->dbiSession_ls00.siReconnect_ls00.rcoLastSuccessfulKnlContact_ls00 = start;

    if ( 0 != DBInfo->dbiSqlRc_ls00 )       // error at receive
    {
        LoaderMessages::Instance()->ls98Msg(laySQLConnect_els98, ErrText, errDbComm_els98,
                                                                           "receive",
                                                                           STATIC_CAST(int, DBInfo->dbiSqlRc_ls00),
                                                                           DBInfo->dbiSqlErr_ls00.length(),
                                                                           DBInfo->dbiSqlErr_ls00.asCharp());
        rc = errDbComm_els98;

        if ( (DBInfo->dbiSqlRc_ls00 >= commErrTimeout_esp01) && (DBInfo->dbiSqlRc_ls00 <= commErrShutdown_esp01))
        {
            DBInfo->dbiSession_ls00.siDBConnection_ls00 = false;
        }
    }
    else
    {
        // Set pointer in packet-receive-list with the right index to received packet
        DBInfo->dbiPktRcvList_ls00[PacketNo] = SqlPacket;

        TRACE_PCKT_MLS99("ls03SQLReceive", SqlPacket);
    }

    return rc;
}
// ls03SQLReceive()


/*
  -----------------------------------------------------------------------------
  function:     ls03GetHostname
  -----------------------------------------------------------------------------
*/
int
ls03GetHostname(tsp00_NodeIdc &DBHostname, tsp00_ErrText szErrText)
{
    ROUTINE_DBG_MEO00 ("ls03GetHostname");

    tsp01_CommErr  rc;

    tsp00_NodeId tmpDBHost_p;
    tmpDBHost_p.SetBlank();

    sqlhostname(tmpDBHost_p, szErrText, rc);
    if (0 != rc)
    {
        return errDbHost_els98;
    }
    else
    {
        SAPDB_Int4    DBHostnameLen = 0;
        DBHostname.Init();
        DBHostname.p2c(tmpDBHost_p);

        DBHostnameLen = DBHostname.length();
    }
    return errOK_els00;
}
// ls03GetHostname()

/*
  -----------------------------------------------------------------------------
  EndChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  Chapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls03_ConnectDB
  -----------------------------------------------------------------------------
*/
static int
ls03_ConnectDB(tls00_DBInfo* DBInfo)
{
    ROUTINE_DBG_MEO00 ("ls03_ConnectDB");

    SAPDB_Int4 rc = errOK_els00;
    //*
    //*     Get database host
    //* If no host is given we suppose that the local host should be used
    //*
    if (NULL == DBInfo->dbiSession_ls00.si_pDBNode_ls00)
    {
        tsp00_NodeIdc tmpDBHost_c;
        rc = ls03GetHostname(tmpDBHost_c, DBInfo->dbiSqlErr_ls00);
        if (errOK_els00 == rc)
        {
            SAPDB_Int4 len = tmpDBHost_c.length();
            DBInfo->dbiSession_ls00.si_pDBNode_ls00 = new char[len + 1];
            SAPDB_memcpy(DBInfo->dbiSession_ls00.si_pDBNode_ls00, tmpDBHost_c.asCharp(), len);
            DBInfo->dbiSession_ls00.si_pDBNode_ls00[len] = 0;
        }
    }

    // Connect to db
    if (errOK_els00 == rc)
    {
        // No database name given - we cannot connect
        if (0 == DBInfo->dbiSession_ls00.siDBName_ls00[0])
        {
            rc = errDBConnectNoDBName_els98;
        }
        else
        {
            tsp00_ErrTextc tmpErrText;
            tmpErrText.Init();

            tsp01_CommErr_Enum rcEnum;
            rcEnum = commErrOk_esp01;

            rcEnum = SqlAConnect(DBInfo->dbiSession_ls00.si_pDBNode_ls00,
                                 DBInfo->dbiSession_ls00.siDBName_ls00,
                                 DBInfo->dbiSqlService_ls00,
                                 DBInfo->dbiPktCount_ls00,
                                 DBInfo->dbiSqlRef_ls00,
                                 DBInfo->dbiPktSize_ls00,
                                 (void**)&DBInfo->dbiPktSndList_ls00,
                                 tmpErrText);
            if (0 != rcEnum)
            {
                DBInfo->dbiSqlRc_ls00.becomes(rcEnum);

                rc = errSqlConnectDb_els98;

                // Copy error text
                DBInfo->dbiSqlErr_ls00.SetBlank();
                DBInfo->dbiSqlErr_ls00.rawAssign(tmpErrText.asCharp());
            }
            else
            {
                DBInfo->dbiSession_ls00.siDBConnection_ls00 = true;          // flag successful connection

                // Get the terminal id used for request calls to the database.
                sqltermid(DBInfo->dbiSqlTerm_ls00);
            }
        }   // end else of if (0 == DBInfo->dbiSession_ls00.siDBName_ls00[0])
    }   // end if (errOK_els00 == rc)

    return rc;
}
// ls03_ConnectDB()


/*
  -----------------------------------------------------------------------------
  function:     ls03_InitPacket
  -----------------------------------------------------------------------------
*/
static int
ls03_InitPacket(tls00_DBInfo* DBInfo)
{
    ROUTINE_DBG_MEO00 ("ls03_InitPacket");

    SAPDB_Int4   rc         = errOK_els00;
    tsp1_packet  *SqlPacket = DBInfo->dbiPktSndList_ls00[0];
    tsp1_segment *SqlSegm;
    tsp1_part    *SqlPart;

    memset(DBInfo->dbiSqlErr_ls00, BLANK_LS00, sizeof(tsp00_ErrText));

    //* Initialize packet
    ls04InitPacket(SqlPacket, DBInfo->dbiPktSize_ls00);

    // define swap
    SqlPacket->sp1_header.sp1h_mess_swap = DBInfo->dbiOwnSwap_ls00;

    // define version
    _TCHAR            pszVersion[6];
    tsp100_VersionID1 VersionID1;

    sp100_GetVersionID(VersionIDType1_esp100, s100buildnumberEx, &VersionID1);

    pszVersion[0] = 0;
    sp77sprintf(pszVersion, 6, "%01d%02d%02d", STATIC_CAST(SAPDB_Int4, VersionID1.MajorVersion_sp100),
                                               STATIC_CAST(SAPDB_Int4, VersionID1.MinorVersion_sp100),
                                               STATIC_CAST(SAPDB_Int4, VersionID1.CorrLevel_sp100) );
    
    SAPDB_memcpy(SqlPacket->sp1_header.sp1h_appl_version, pszVersion, 5);
    SAPDB_memcpy(SqlPacket->sp1_header.sp1h_application, APPL_NAME_LS00, 3);

    //*
    //*     Initialize segment and part in packet
    //* Save header data for later fast reinitialization
    rc = ls04InitSegment(SqlPacket, &SqlSegm, sp1sk_cmd);
    if (errOK_els00 == rc)
    {
        rc = ls04InitPart(SqlPacket, SqlSegm, &SqlPart, sp1pk_command);
    }
    if (errOK_els00 == rc)
    {
        SAPDB_memcpy(DBInfo->dbiPktInitBlock_ls00,
                     REINTERPRET_CAST(char*, &SqlPacket->sp1_header),
                     sizeof(tsp1_packet_header));
        SAPDB_memcpy(DBInfo->dbiPktInitBlock_ls00 + sizeof(tsp1_packet_header),
                     REINTERPRET_CAST(char*, &SqlPacket->sp1_varpart()),
                     sizeof(tsp1_segment_header) + sizeof(tsp1_part_header));
    }

    return rc;
}
// ls03_InitPacket()

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/
