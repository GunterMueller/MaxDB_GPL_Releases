/*!================================================================
 module:    vpr01Con.c

 responsible: BurkhardD

 special area:   CPCDrv | Client | Connection Verwaltung

 description: Connection Verwaltung

 see:

 change history: 

    2001-11-08 D025086 Cancel session bugfix PTS 1112515 and PASCAL 
               port from p01cancel,
    2001-09-04 d025086 No implicite connect. Added OldStyle attribute.


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




 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpr01Con.h"
#include "gmt90.h"
#include "gpr00.h"
#include "gpr01.h"
#include "gpr03.h"
#include "gpr04.h"
#include "gpr08.h"
#include "vpr07Macro.h"
#include "vpr01Env.h"
#include "vpr01Error.h"
#include "vpr01Cursor.h"
#include "vpr01StmtName.h"
#include "vpr01SQL.h"
#include "vpr03Con.h"
#include "vpr03Packet.h"
#include "vpr02Con.h"
#include "vpr03Segment.h"
#include "vpr06ParseId.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

const tpr01Con_FeatureSet pr01ConFeatureSetWanted = FeatureSet743_cpr00;

const tpr01Con_FeatureSet pr01ConFeatureSetEmpty  = { {sp1f_nil_feature,           0},
                                                      {sp1f_multiple_drop_parseid, 0},
                                                      {sp1f_space_option,          0},
                                                      {sp1f_variable_input,        0},
                                                      {sp1f_optimized_streams,     0}};


/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr01ConInitCont(tpr01_ConContainer *Connection, tpr01_EnvDesc *EnvDesc);

static tsp00_Int4 pr01ConOpenCont(tpr01_ConContainer *Connection);

static tsp00_Int4 pr01ConGetCount(tpr01_ConContainer *Connection);

static tpr01_ConDesc *pr01ConEnumDesc(tpr01_ConContainer *Connection);

static tpr01_ConDesc *pr01ConAddDesc(tpr01_ConContainer *Connection, tpr00_DatabaseName szDatabaseName, sqlxatype *sqlxa, epr01Con_SessionType SessionType);

static void pr01ConDeleteDesc(tpr01_ConDesc *ConDesc);

static tsp00_Int4 pr01ConSetIndex(tpr01_ConContainer *Connection, tsp00_Int4 cbIndex);

static  tpr01_ConDesc *pr01ConGetConDescByParseID (tpr01_ConContainer *Connection, tpr00_ParseId *ParseID);

static  tpr01_ConDesc *pr01ConFindDesc (tpr01_ConContainer *Connection, tpr00_DatabaseName szDatabaseName, epr01Con_SessionType SessionType);

static tpr03_SegmDesc *pr01ConInitPacket(tpr01_ConDesc *ConDesc, tpr05_StringEncoding PacketEncoding, tsp1_cmd_mess_type MessType);
static void pr01ConConnect(tpr01_ConDesc *ConDesc, struct tpr01_StmtNameDesc *StmtNameDesc);

static BOOLEAN pr01ConIsConnected(tpr01_ConDesc *ConDesc);

static void pr01ConAutoConnect (tpr01_ConDesc *ConDesc);

static void pr01ConOpen (tpr01_ConDesc *ConDesc, sqlkaentry *ka);

static void pr01ConRelease (tpr01_ConDesc *ConDesc);

static BOOLEAN pr01ConSetAttr (tpr01_ConDesc *ConDesc, epr01Con_Attr Attr, void *pAttr, tsp00_Int4 cbLen);

static tpr03_SegmDesc *pr01ConSegmentNew(tpr01_ConDesc *ConDesc);

static void pr01ConSegmentDelete(tpr01_ConDesc *ConDesc);

static void pr01ConEmptyCont(tpr01_ConContainer *Connection);

static tsp1_sqlmode pr01ConSqlMode (tpr01_ConDesc *ConDesc);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

static void pr01ConInitCont(tpr01_ConContainer *Connection, tpr01_EnvDesc *EnvDesc)
{
  Connection->Desc = pr09NewDescriptor(3, sizeof(tpr01_ConDesc));
  Connection->EnvDesc    = EnvDesc;
  Connection->sqlca      = EnvDesc->sqlca;
  Connection->NoLog      = cpr_is_false;
  Connection->SessionCnt = 0;
  Connection->OptimizedStreams = pr01ConFeatureSetWanted[sp1f_optimized_streams].value;
  Connection->ParseInfoParams.CacheSize = 0; /*<= Cache is disabled DEFAULT_HASHTABLE_SIZE; */
  Connection->ParseInfoParams.HashTableLoadFactor = DEFAULT_HASHTABLE_LOADFACTOR;
  Connection->OldStyle   = FALSE;
  Connection->InitCont   = pr01ConInitCont;
  Connection->OpenCont   = pr01ConOpenCont;
  Connection->EmptyCont  = pr01ConEmptyCont;
  Connection->GetCount   = pr01ConGetCount;
  Connection->EnumDesc   = pr01ConEnumDesc;
  Connection->AddDesc    = pr01ConAddDesc;
  Connection->DeleteDesc = pr01ConDeleteDesc;
  Connection->SetIndex   = pr01ConSetIndex;
  Connection->FindDesc   = pr01ConFindDesc;
  Connection->GetConDescByParseID   = pr01ConGetConDescByParseID;
  Connection->InitPacket = pr01ConInitPacket;
  Connection->Connect    = pr01ConConnect;
  Connection->IsConnected = pr01ConIsConnected;
  Connection->AutoConnect = pr01ConAutoConnect;
  Connection->Open       = pr01ConOpen;
  Connection->Release    = pr01ConRelease;
  Connection->SetAttr    = pr01ConSetAttr;
  Connection->SegmentNew = pr01ConSegmentNew;
  Connection->SegmentDelete = pr01ConSegmentDelete;
  Connection->SqlMode    = pr01ConSqlMode;
}

void pr01ConDeleteCont(tpr01_ConContainer *Connection)
{
  if (Connection) {
    pr01ConEmptyCont(Connection);
    pr09DeleteDescriptor(Connection->Desc);
    Connection->Desc = NULL;
    pr03mFree((tsp00_Uint1*)Connection, "Connection");
  }
}

static void pr01ConEmptyCont(tpr01_ConContainer *Connection)
{
  if (Connection) {
    if (pr01ConOpenCont(Connection)) {
      /* returns <> 0 wenn Module Descriptoren vorhanden sind */
      tpr01_ConDesc *ConDesc;
      while(ConDesc = Connection->EnumDesc(Connection))
        Connection->DeleteDesc(ConDesc);
    }
    Connection->SessionCnt = 0;
  }
}

tpr01_ConContainer *pr01ConNewCont(tpr01_EnvDesc *EnvDesc)
{
  tpr01_ConContainer *Connection;
  Connection = pr03mAllocat(sizeof(tpr01_ConContainer), "Connection");
  if (Connection)
    pr01ConInitCont(Connection, EnvDesc);
  return Connection;
}


static tsp00_Int4 pr01ConOpenCont(tpr01_ConContainer *Connection)
{
  if (!Connection->Desc) {
    Connection->Desc = pr09NewDescriptor(10, sizeof(tpr01_ConDesc));
  }
  pr09SetIndex(Connection->Desc, -1);
  return(Connection->GetCount(Connection));
}

static tsp00_Int4 pr01ConGetCount(tpr01_ConContainer *Connection)
{
  return(pr09GetItemCount(Connection->Desc));
}

static tpr01_ConDesc *pr01ConEnumDesc(tpr01_ConContainer *Connection)
{
  tpr01_ConDesc *Desc=pr09GetNextItem(Connection->Desc);
  return(Desc);
}

static tpr01_ConDesc *pr01ConAddDesc(tpr01_ConContainer *Connection, tpr00_DatabaseName szDatabaseName, sqlxatype *sqlxa, epr01Con_SessionType SessionType)
{
  M90TRACE(M90_TR_ENTRY, "pr01ConAddDesc", 0);
  {
    tpr01_ConDesc *ConDesc;
    tsp00_Int2 readxuser;
    ConDesc = pr09AddItem(Connection->Desc);
    ConDesc->DescType = ConDesc_epr01;
    ConDesc->Connection = Connection;
    ConDesc->SessionNr = 0;
    strcpy(ConDesc->szDatabaseName, szDatabaseName);
    ConDesc->ErrorDesc = pr01NewErrorDesc();
    ConDesc->Status    = Empty_epr01Con;
    ConDesc->Option    = NoSelectDirectFast_bpr01;
    ConDesc->Seg       = NULL;
    ConDesc->sqlca = Connection->sqlca;
    ConDesc->sqlxa = sqlxa;
    ConDesc->ka = pr01cDupKa(NULL);
    ConDesc->SessionType = SessionEmpty_epr01Con;
    memcpy(ConDesc->DBFeatures, pr01ConFeatureSetWanted, sizeof(ConDesc->DBFeatures));
    ConDesc->Connection->SetAttr(ConDesc, OptimizedStreams_epr01Con, (void*)&ConDesc->Connection->OptimizedStreams, sizeof(ConDesc->Connection->OptimizedStreams));    
    if (!ConDesc->ka)
      pr07CheckAssert(false);

    if (Connection->OldStyle) {
      if (Connection->SessionCnt == 0) {
        SessionType = (SessionType != SessionTemp_epr01Con) ? SessionPrimary_epr01Con : SessionType;
      }
      /* check what kind of session should this descriptor contain */
      if (SessionType == SessionPrimary_epr01Con) {
        /* On a primary session xuser and option arguments are possible */
        ConDesc->SessionNr = cpr_se_primary;
        Connection->SessionCnt++;
      }
      else if (SessionType == SessionDefault_epr01Con) {
        /* This is the normal way to connect */
        ConDesc->SessionNr = ++Connection->SessionCnt;
      }
      else if (SessionType == SessionTemp_epr01Con) {
        /* a temp session is only used for holding session parameter
           tracing etc no one will connect with this ConDesc */
        ConDesc->SessionNr = 0;
      }
      ConDesc->SessionType = SessionType;
      
      /* Init gaentry and read XUSER Entry */
      
      ConDesc->ga = pr03mAllocat(sizeof(sqlgaentry), "ConDesc->ga");
      if (!ConDesc->ga)
        pr07CheckAssert(false);
      ConDesc->XUserRecord = &ConDesc->ga->gaxuserrec;
      
      /* on a primary session initialize with the entry from XUSER */
      readxuser = (SessionType == SessionPrimary_epr01Con) ? 1 : 0;
      p03sqlgaentryinit(ConDesc->ga, readxuser, ConDesc->sqlca->sqlemp);
      if ( ConDesc->sqlca->sqlemp->elzu != sp1ce_ok) {
        p04trint4(ConDesc->sqlca->sqlrap, "No XUSER entry found. Continue", Connection->sqlca->sqlemp->elzu);
        ConDesc->sqlca->sqlemp->elzu = sp1ce_ok;
      }
    }
    else {
      if (SessionType == SessionTemp_epr01Con) {
        /* a temp session is only used for holding session parameter
           tracing etc no one will connect with this ConDesc */
        ConDesc->SessionNr = 0;
      }
      else {
        ConDesc->SessionNr = ++Connection->SessionCnt;
      }
      ConDesc->ga = pr03mAllocat(sizeof(sqlgaentry), "ConDesc->ga");
      if (!ConDesc->ga)
        pr07CheckAssert(false);
      ConDesc->XUserRecord = &ConDesc->ga->gaxuserrec;      
      p03gaentryinitdefault(ConDesc->ga);
    }

    if (ConDesc->szDatabaseName[0] != ':') {
      /* set the alias for Oracle connection */
      pr07C2P(ConDesc->ga->gaatdbname, ConDesc->szDatabaseName, sizeof(ConDesc->ga->gaatdbname));
    }
    ConDesc->ParseIdCache = NULL;
    /* Drop Parsid Cache activate */
    ConDesc->ParseIdCache = pr06ParseIDNewCache(ConDesc, 20);
    ConDesc->SegmDesc = NULL;
    /* initialize Parseinfo Cache */
    ConDesc->ParseInfoCache = NULL;
    /* initialize function pointer to parse method */
    ConDesc->Parse = pr01cParse;
    M90TRACE(M90_TR_EXIT, "pr01ConAddDesc", 0);
    return(ConDesc);
  }
}

static void pr01ConDeleteDesc(tpr01_ConDesc *ConDesc)
{
  int cbIndex = pr09ItemFindIndex(ConDesc->Connection->Desc, ConDesc);
  if (cbIndex >= 0) {
    pr01DeleteErrorDesc(ConDesc->ErrorDesc);
    ConDesc->ErrorDesc = NULL;
    if (ConDesc->ga){
      pr03mFree(ConDesc->ga, "ConDesc->ga");
      ConDesc->ga = NULL;
    }  
    if(ConDesc->ka){
      pr01cFreeKa(ConDesc->ka);
      ConDesc->ka = NULL;
    }  

    pr09CloseItemEx(ConDesc->Connection->Desc, cbIndex);

    if (ConDesc->SegmDesc)
      pr01ConSegmentDelete(ConDesc);

    /* delete Parseinfo Cache */
    if (ConDesc->ParseInfoCache){
      pr01ParseInfoDeleteCont(ConDesc->ParseInfoCache);
      ConDesc->ParseInfoCache = NULL;
    }  
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127862 */
    if (ConDesc->ParseIdCache){
      pr06ParseIDDeleteCache(ConDesc->ParseIdCache);
      ConDesc->ParseIdCache = NULL;
    }  
  }
}

static tsp00_Int4 pr01ConSetIndex(tpr01_ConContainer *Connection, tsp00_Int4 cbIndex)
{
  return(pr09SetIndex(Connection->Desc, cbIndex));
}

static  tpr01_ConDesc *pr01ConGetConDescByParseID (tpr01_ConContainer *Connection, tpr00_ParseId *ParseID)
{
  M90TRACE(M90_TR_ENTRY, "pr01ConGetConDescByParseID", 0);
  {
    tsp00_Int4 cbOldIndex = Connection->SetIndex(Connection, -1);
    tpr01_ConDesc *ConDesc=NULL;
    while(ConDesc=Connection->EnumDesc(Connection)) {
      if (pr06ParseIdCheckKnlSessionID(&ConDesc->ga->gaKnlSessionID, ParseID)) {
        break;
      }  
    }
    Connection->SetIndex(Connection, cbOldIndex);
    M90TRACE(M90_TR_EXIT, "pr01ConGetConDescByParseID", 0);
    M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
    return(ConDesc);
  }
}

static  tpr01_ConDesc *pr01ConFindDesc (tpr01_ConContainer *Connection, tpr00_DatabaseName szDatabaseName, epr01Con_SessionType SessionType)
{
  M90TRACE(M90_TR_ENTRY, "pr01ConFindDesc", 0);
  {
    tsp00_Int4 cbOldIndex = Connection->SetIndex(Connection, -1);
    tpr01_ConDesc *ConDesc=NULL;
    tpr01_ConDesc *ConDescPrimary=NULL;
    while(ConDesc=Connection->EnumDesc(Connection)) {
      if (!strcmp(szDatabaseName, ConDesc->szDatabaseName)) {
	    M90TRACE(M90_TR_STRING, "szDatabaseName", ConDesc->szDatabaseName);
	    break;
	  }  
      else if (ConDesc && ConDesc->SessionType == SessionPrimary_epr01Con) {
        ConDescPrimary = ConDesc;
      }
    }
    if (!ConDesc) {
      if ( SessionType == SessionPrimary_epr01Con
           && ConDescPrimary) {
        ConDesc = ConDescPrimary;    
      }      
      else
        Connection->SetIndex(Connection, cbOldIndex);
    }
    M90TRACE(M90_TR_EXIT, "pr01ConFindDesc", 0);
    M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
    return(ConDesc);
  }
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01ConInitPacket

  see also:

  Description:  Initializes a new order interface packet by calling
                pr03PacketInit. After this it sets the packetencoding
                to the client encoding and does some other initialization
                depending from the kind of the connection.

  Arguments:

                *ConDesc   [in] Descriptor of the connection.

  return value: A Pointer to SegmDesc or NULL if the packet could not
                initialize.
 */
static tpr03_SegmDesc *pr01ConInitPacket(tpr01_ConDesc *ConDesc, tpr05_StringEncoding PacketEncoding, tsp1_cmd_mess_type MessType)
{
  tpr03_SegmDesc *SegmDesc=NULL
  M90TRACE(M90_TR_ENTRY, "pr01ConInitPacket", 0);
  PR07_CHECKDESC(ConDesc, ConDesc_epr01);
  if (ConDesc->ga->gareference != 0) {
    ConDesc->SegmDesc = pr01ConSegmentNew(ConDesc);
    if (ConDesc->SegmDesc) {
      pr03PacketInit(ConDesc, MessType);
      pr03PacketSetEncoding(ConDesc->SegmDesc->packetPtr, PacketEncoding);
      pr03PacketSetSwapKind(ConDesc->SegmDesc->packetPtr, ConDesc->Connection->EnvDesc->swapKind);
      pr03SegmentSetSqlMode(ConDesc->SegmDesc->segmPtr, pr01ConSqlMode(ConDesc));
    }
    SegmDesc=ConDesc->SegmDesc;
  }
  M90TRACE(M90_TR_EXIT, "pr01ConInitPacket", 0);
  return SegmDesc;
}

static void pr01ConConnect(tpr01_ConDesc *ConDesc, tpr01_StmtNameDesc *StmtNameDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01ConConnect", 0);
  {
    PR07_CHECKDESC(ConDesc, ConDesc_epr01);
    {
      sqlgaentry *ga = ConDesc->ga;
      p03csqlcaareainit(ConDesc->sqlca);
      p01xtracefilecheck(ConDesc->sqlca, ConDesc->sqlxa);
      if (ga->gareference == 0) {
	tpr01_ConContainer *Connection = ConDesc->Connection;
	tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName;
	sqlcatype *sqlca = Connection->sqlca;
	sqlgatype *sqlga = sqlca->sqlgap;
	sqlga->gasqlconntyp = cpr_ci_program;
	ga->gareconnect = cpr_is_false;
	pr02ConAnalyseConnect(ConDesc, StmtNameDesc);
      }
      pr01ConAutoConnect(ConDesc);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01ConConnect", 0);
}

static BOOLEAN pr01ConIsConnected(tpr01_ConDesc *ConDesc) {
  PR07_CHECKDESC(ConDesc, ConDesc_epr01);
  {
    if (ConDesc->ga->gareference == 0)
      ConDesc->Status = Empty_epr01Con;
    return (ConDesc->Status == Connected_epr01Con);
  }
}

static void pr01ConRelease (tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01ConRelease", 0);
  {
    tpr01_ConContainer *Connection = ConDesc->Connection;
    sqlcatype *sqlca = Connection->sqlca;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
/*    if (ConDesc->ParseInfoCache)
      ConDesc->ParseInfoCache->TraceStatistic (ConDesc->ParseInfoCache, sqlca->sqlrap);*/
    if (sqlemp->ereturncode >= 0) {
      Connection->DeleteDesc(ConDesc);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01ConRelease", 0);
}

static void pr01ConAutoConnect (tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01ConAutoConnect", 0);
  {
    tpr01_ConContainer *Connection = ConDesc->Connection;
    sqlcatype *sqlca = Connection->sqlca;
    sqlxatype *sqlxa = ConDesc->sqlxa;
    sqlgatype *sqlga = sqlca->sqlgap;
    sqlratype *sqlra = sqlca->sqlrap;
    sqlgaentry *ga   = ConDesc->ga;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    M90TRACE (M90_TR_PTR,   "ga", &ga);
    if (sqlemp->ereturncode >= 0 && ga->gareference == 0) {
      if (sqlga->gasqlconntyp == cpr_ci_program
          || sqlga->gasqlconntyp == cpr_ci_ora_connect) {
	ga->gastatus = cpr_co_ok;
	sqlra->rakamode = ConDesc->ka->kamode;
	pr03ConConnect(ConDesc);
	if (sqlxa->xalang != CPR_LA_CALL)
	  p01pparsidtrace (sqlca, sqlxa, ConDesc->ka, NULL, TraceParseIDSQLStmt_epr00);
	p01xprofinit(sqlca, sqlxa, ConDesc->ga);
	p03returncodeget(sqlca, sqlxa);
	p01xtimetrace(sqlca, sqlxa, ConDesc->ga);
      }
      else {
	if  (sqlga->gasqlconntyp == cpr_ci_explicit) {
	  p01cmd_trace(sqlca, sqlxa, NULL, ConDesc->ga);
	  p08runtimeerror (sqlca, sqlxa, cpr_session_not_connected);
	}
	else {
	  ga->gastatus = cpr_co_implicit;
	  sqlra->rakamode = ConDesc->ka->kamode;
	  pr03ConConnect(ConDesc);
	  if (sqlxa->xalang != CPR_LA_CALL)
	    p01pparsidtrace (sqlca, sqlxa, ConDesc->ka, NULL, TraceParseIDSQLStmt_epr00);
	  p01xprofinit(sqlca, sqlxa, ConDesc->ga);
	  p03returncodeget (sqlca, sqlxa);
	  p01xtimetrace (sqlca, sqlxa, ConDesc->ga);
	}
      }
      if (ga->gareference) {
	ConDesc->Status = Connected_epr01Con;
        sqlga->gasqlconntyp = cpr_ci_connect;
        ga->gareconnect = cpr_is_false;
      }
      if (!ConDesc->ParseInfoCache
          && Connection->ParseInfoParams.CacheSize) {
        ConDesc->ParseInfoCache = pr01ParseInfoNewCont(ConDesc, Connection->ParseInfoParams.CacheSize,Connection->ParseInfoParams.HashTableLoadFactor);
        ConDesc->Parse = ConDesc->ParseInfoCache->Parse; 
      }  
    }
    else {
      struct SQLERROR sqlemp;
      tsp00_Int4 gareference = ConDesc->ga->gareference;
      void *gareqptr = ga->gareqptr;
      ConDesc->ga->gareference = 9999;
      ga->gareqptr = pr03PacketNewPacket(1000);
      Connection->InitPacket(ConDesc, sp77encodingAscii, sp1m_dbs);
      sqlemp.ereturncode = 0;
      sqlemp.elzu = sp1ce_ok;
      pr03ConMakeConnectPart(ConDesc, &sqlemp);
      p03connectcmdtrace (sqlra, ga);
      p01pparsidtrace (sqlca, sqlxa, ConDesc->ka, NULL, TraceParseIDSQLStmt_epr00);
      p03returncodeget(sqlca, sqlxa);
      p01xtimetrace(sqlca, sqlxa, ConDesc->ga);
      if (ConDesc->SegmDesc)
        pr01ConSegmentDelete(ConDesc);
      pr03PacketDeletePacket(ga->gareqptr);
      ga->gareqptr = gareqptr;
      ConDesc->ga->gareference = gareference;
    }
    /*create parseinfocache */

  }
  M90TRACE(M90_TR_EXIT, "pr01ConAutoConnect", 0);
}

static void pr01ConOpen (tpr01_ConDesc *ConDesc, sqlkaentry *ka)
{
  M90TRACE(M90_TR_ENTRY, "pr01ConOpen", 0);
  {
    PR07_CHECKDESC(ConDesc, ConDesc_epr01);
    memcpy(ConDesc->ka, ka, sizeof(sqlkaentry));
    M90TRACE(M90_TR_EXIT, "pr01ConOpen", 0);
  }
}

static void pr01ConC2C(char *cout, char *cin, int clen, int len)
{
  if (len == CPR_NTS)
    len = (int) strlen(cin);
  len = PR07MIN(len, clen);
  memcpy(cout, cin, len);
  cout[len] = '\0';
}

static BOOLEAN pr01ConSetAttr (tpr01_ConDesc *ConDesc, epr01Con_Attr Attr, void *pAttr, tsp00_Int4 cbLen)
{
  M90TRACE(M90_TR_ENTRY, "pr01ConSetAttr", 0);
  {
    BOOLEAN ret = TRUE;
    PR07_CHECKDESC(ConDesc, ConDesc_epr01);
    switch(Attr) {
    case (DatabaseName_epr01Con) : {
      pr01ConC2C(ConDesc->szDatabaseName, pAttr, sizeof(ConDesc->szDatabaseName), cbLen);
      break;
    }
    case (ServerNode_epr01Con) : {
      pr07C2P(ConDesc->XUserRecord->xu_servernode, pAttr, sizeof(ConDesc->XUserRecord->xu_servernode));
      break;
    }
    case (ServerName_epr01Con) : {
      pr07C2P(ConDesc->XUserRecord->xu_serverdb, pAttr, sizeof(ConDesc->XUserRecord->xu_serverdb));
      break;
    }
    case (UserName_epr01Con) : {
      pr07C2P(ConDesc->XUserRecord->xu_user, pAttr, sizeof(ConDesc->XUserRecord->xu_user));
      break;
    }
    case (UserNameUCS2_epr01Con) : {
      pr07C2P(ConDesc->XUserRecord->xu_userUCS2, pAttr, sizeof(ConDesc->XUserRecord->xu_user));
      break;
    }
    case (PWD_epr01Con) : {
      p03encrypt (pAttr, ConDesc->XUserRecord->xu_password);
      break;
    }
    case (PWDUCS2_epr01Con) : {
      p03encrypt (pAttr, ConDesc->XUserRecord->xu_passwordUCS2);
      break;
    }
    case (CryptPWD_epr01Con) : {
      memcpy(ConDesc->XUserRecord->xu_password, pAttr, cbLen);
      break;
    }
    case (ParseInfoCacheSize_epr01Con) : {
      memcpy((void*)&(ConDesc->Connection->ParseInfoParams.CacheSize), pAttr, cbLen);
      break;
    }
    case (HashTableLoadFactor_epr01Con) : {
      memcpy((void*)&(ConDesc->Connection->ParseInfoParams.HashTableLoadFactor), pAttr, cbLen);
      break;
    }
    case (OldStyleConnect_epr01Con) : {
      ConDesc->Connection->OldStyle=((BOOLEAN*)pAttr) ? TRUE : FALSE;
      break;
    }
    case (ActualSession_epr01Con) : {      
      ConDesc->Connection->Actual = (tpr01_ConDesc*)pAttr;
      break;
    }
    case (OptimizedStreams_epr01Con) : {
      ConDesc->DBFeatures[sp1f_optimized_streams].value = (*(BOOLEAN*)pAttr) ? 1 : 0;
      break;
    }
    default: {
      ret = FALSE;
      break;
    }
    }
    M90TRACE(M90_TR_EXIT, "pr01ConSetAttr", 0);
    return ret;
  }
}

static tsp1_sqlmode pr01ConSqlMode (tpr01_ConDesc *ConDesc)
{
  tpr01_ConContainer *Connection = ConDesc->Connection;
  sqlcatype *sqlca = Connection->sqlca;
  sqlratype *sqlra = sqlca->sqlrap;
  return pr03cMapSqlMode(sqlra->rakamode, sqlra->rasqlansi);
}

static tpr03_SegmDesc *pr01ConSegmentNew(tpr01_ConDesc *ConDesc)
{
  tpr01_ConContainer *Connection = ConDesc->Connection;
  sqlcatype *sqlca = Connection->sqlca;
  sqlratype *sqlra = sqlca->sqlrap;
  sqlgaentry *ga   = ConDesc->ga;
  tsp1_packet    *pck = (tsp1_packet *) ga->gareqptr;
  if (ConDesc->SegmDesc)
    pr01ConSegmentDelete(ConDesc);
  ConDesc->SegmDesc = pr03SegmentNew(pck, &sqlra->rasegmpartptr, (unsigned char *)sqlra->ratermid);
  if (!pr03SegmentInit(ConDesc->SegmDesc)) {
    pr01ConSegmentDelete(ConDesc);
  }
  return ConDesc->SegmDesc;
}

static void pr01ConSegmentDelete(tpr01_ConDesc *ConDesc)
{
  if (ConDesc->SegmDesc) {
    sqlratype *sqlra = ConDesc->sqlca->sqlrap;
    pr03SegmentDelete(ConDesc->SegmDesc);
    sqlra->rasegptr = NULL;
    ConDesc->SegmDesc = NULL;
  }
}

