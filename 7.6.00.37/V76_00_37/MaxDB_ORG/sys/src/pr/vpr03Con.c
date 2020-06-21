/*!================================================================
 module:    vpr03Con.c

 responsible: BurkhardD

 special area:   Connection Verwaltung

 description: Connection Verwaltung

 see:




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
#include "gpr03.h"
#include "hsp78_0.h"
#include "gpr01e.h"
#include "vpr01Con.h"
#include "vpr03Packet.h"
#include "vpr03Segment.h"
#include "vpr03Part.h"
#include "vpr03Con.h"
#include "SAPDB/RunTime/Security/RTESec_Authentication.h"
/*==================================================================*
 *  DIRTY DECLARATIONS                                                    *
 *==================================================================*/

#define   USER_ALREADY_CONNECTED   -8002
void p03connectcmdtrace (sqlratype * sqlra, sqlgaentry * gaen);
extern tsp00_Int2 s30lnr (tsp00_C18, char val, int pos, int cnt);
extern tsp00_Int2 s30lnr1 (tsp00_C70, char val, int pos, int cnt);

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/
extern tsp00_SwapKind_Enum sqlLocalMachineSwapKind;
/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr03ConSendConnect(tpr01_ConDesc *ConDesc);
static void pr03ConSessionInfoGet(tpr01_ConDesc *ConDesc);
static void pr03ConMakeSQLStmt(tpr01_ConDesc *ConDesc, tpr05_String *pConString, struct SQLERROR *sqlemp);

static void pr03ConMakePasswordPart(tpr01_ConDesc *ConDesc);
static void pr03ConMakeFeaturePart(tpr01_ConDesc *ConDesc);


/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

void pr03ConConnect(tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr03ConConnect", 0);
  {
    tpr01_ConContainer *Connection = ConDesc->Connection;
    sqlcatype *sqlca = Connection->sqlca;
    sqlxatype *sqlxa = ConDesc->sqlxa;
    sqlgatype *sqlga = sqlca->sqlgap;
    sqlratype *sqlra = sqlca->sqlrap;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    sqlgaentry *gaen = ConDesc->ga;
    sqlra->raactsession = ConDesc->SessionNr;
    p01xtracefilecheck  (sqlca, sqlxa);
    p03initsqlem (sqlemp);
    pr03ConSendConnect(ConDesc);
    sqlga->gasqlconntyp = cpr_ci_connect;
    gaen->gareconnect = cpr_is_false;
    {
      tsp1_packet    *pck = (tsp1_packet*) gaen->garecptr;
      tsp1_segment_header *seghp
	= (pck) ? (tsp1_segment_header *)&pck->variant.C_2.sp1_segm_F.variant.C_1.sp1s_segm_header_F : NULL;

      if ((sqlemp->ereturncode == 0) &&
	  (seghp && seghp->sp1r_returncode == 0)) {
	pr03ConSessionInfoGet(ConDesc);
	if (sqlga->gacmpkind != CPR_LA_ODBC)
	  p03setformat (ConDesc->sqlxa->xaSQLDesc, sqlra, gaen, sqlca->sqldatetime, sqlemp);
	if (sqlemp->ereturncode != 0)
	  p03cseterror (sqlemp, cpr_set_format_error);
      }
    }
  }
  M90TRACE(M90_TR_EXIT, "pr03ConConnect", 0);
}

static void pr03ConMakeChallengeStmt(tpr01_ConDesc *ConDesc, tpr05_String *pConString, struct SQLERROR *sqlemp)
{
  tpr01_ConContainer *Connection = ConDesc->Connection;
  tsp4_xuser_record  *XUser = ConDesc->XUserRecord;
  tpr05_StringEncoding encType = pr03ConXUserEncoding(XUser);
  sqlcatype *sqlca = Connection->sqlca;
  sqlgaentry *gae  = ConDesc->ga;
  sqlgatype *sqlga = sqlca->sqlgap;
  int len;
  BOOLEAN OldStyle = TRUE;

  if (sqlga->gaConnection) {
    tpr01_ConContainer *Connection = sqlga->gaConnection;
    OldStyle = Connection->OldStyle;
  }
  
  pr05IfCom_String_ConvertP (pConString,
                             "CONNECT ",
                             8,
                             sp77encodingAscii);

  if (!strncmp ((char*)XUser->xu_user, bsp_c64, sizeof(XUser->xu_user)) &&
      (*(const unsigned short *) &XUser->xu_userUCS2[0] == ((const unsigned short)' '))) {
    p03cseterror (sqlemp, cpr_missing_user_passw);
    if (OldStyle)
      gae->gastatus = CPR_CO_IMPLICIT;
  }
  /* truncate username */
  if (encType == sp77encodingAscii)
    len = s30lnr1 (XUser->xu_user, bsp_c1, 1, sizeof(XUser->xu_user));
  else
    len = s30lnr1 (XUser->xu_userUCS2, bsp_c1, 1, sizeof(XUser->xu_userUCS2));

  if (len < mxsp_name)
    len = mxsp_name;

  pr05IfCom_String_strcatP (pConString,
                            "\"",
                            1,
                            sp77encodingAscii);

  if (encType == sp77encodingAscii)
    pr05IfCom_String_strcatP (pConString,
                              (char *) XUser->xu_user, 
                              len,
                              sp77encodingAscii);
  else
    pr05IfCom_String_strcatP (pConString,
                              (char *) XUser->xu_userUCS2, 
                              len,
                              encType);

  pr05IfCom_String_strcatP (pConString,
                            "\"",
                            1,
                            sp77encodingAscii);

  pr05IfCom_String_strcatP (pConString,
                            " AUTHENTICATION",
                            15,
                            sp77encodingAscii);
}

static void pr03ConAddTermid (tpr01_ConDesc *ConDesc)
{
  tsp00_TermId tid;
  tpr01_ConContainer *Connection = ConDesc->Connection;
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_data);
  sqlcatype *sqlca = Connection->sqlca;
  sqlgaentry *gae  = ConDesc->ga;
  sqlgatype *sqlga = sqlca->sqlgap;

  pr03PartCopy(partPtr, "\0", 1);
  memset (&tid, bsp_c1, sizeof(tid));
  sqltermid (tid);
  pr03PartAppend(partPtr, tid, sizeof(tid));
  pr03SegmentFinishPart(SegmDesc);
}

static void pr03ConAddClientid (tpr01_ConDesc *ConDesc)
{
  tsp00_TermId tid;
  tpr01_ConContainer *Connection = ConDesc->Connection;
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_clientid);
  sqlcatype *sqlca = Connection->sqlca;
  sqlgaentry *gae  = ConDesc->ga;
  sqlgatype *sqlga = sqlca->sqlgap;

  pr03PartCopy(partPtr, "\0", 1);
  memset (&tid, bsp_c1, sizeof(tid));
  sqltermid (tid);
  pr03PartAppend(partPtr, tid, sizeof(tid));
  pr03SegmentFinishPart(SegmDesc);
}

static void pr03ConGetField (tsp00_Byte **source_in, tsp00_Byte **fieldptr, int *fieldlen)
{
  tsp00_Byte *source = *source_in;
  int indicatorLen = 1;
  
  switch (source [0]) {
  case csp1_fi_ignored:
  case csp1_fi_special_null:
  case csp1_fi_blob_description:
  case csp1_fi_default_value:
  case csp1_fi_null_value:
    *fieldptr = NULL;
    *fieldlen = 0;
    break;
  case csp1_fi_2byte_length:
    indicatorLen = 3;
    *fieldptr = source + indicatorLen;
    *fieldlen = source [1] * 256 + source [2];
    break;
  default:
    *fieldptr = source + indicatorLen;
    *fieldlen = source [0];
    break;
  }
  *source_in = source + indicatorLen + *fieldlen;
}

static void pr03ConAddDataField (tsp1_part *partPtr, const void *data, int datalen)
{
  tsp00_Byte buf[3];
  
  if (datalen <= csp1_fi_max_1byte_length) {
    buf[0] = (tsp00_Byte) datalen;
    pr03PartAppend(partPtr, buf, sizeof(tsp00_Byte));
  } else {
    buf[0] = (tsp00_Byte) csp1_fi_2byte_length;
    buf[1] = (tsp00_Byte) (datalen / 256);
    buf[2] = (tsp00_Byte) (datalen % 256);
    pr03PartAppend(partPtr, buf, sizeof(buf));
  }
  pr03PartAppend(partPtr, (void *) data, datalen);
}

static void pr03ConSetVardataFieldCount (tsp1_part *partPtr, int valueCount)
{
  tsp00_Byte *buf = (tsp00_Byte *) pr03PartGetRawPtr(partPtr);
  buf [0] = (tsp00_Byte) (valueCount / 256);
  buf [1] = (tsp00_Byte) (valueCount % 256);
}

static void pr03ConMakeAuthStmt(tpr01_ConDesc *ConDesc, 
                                tsp1_part *partPtr, 
                                RTESec_ClientAuthenticationBuffer *clientBuffer,
                                struct SQLERROR *sqlemp)
{
  tsp4_xuser_record  *XUser = ConDesc->XUserRecord;
  tpr05_StringEncoding encType = pr03ConXUserEncoding(XUser);
  const RTESec_Authentication *authentication = NULL;
  int index = 0;
  int namelen;
  char *buf;


  if (encType == sp77encodingAscii) {
    namelen = sizeof(XUser->xu_user);
    buf = (char *) XUser->xu_user;
  } else {
    namelen = sizeof(XUser->xu_userUCS2);
    buf = (char *) XUser->xu_userUCS2;
  }
  while (RTESec_Authentication_Next (&authentication, &index)) {
    const char *algoname = RTESec_Authentication_Name (authentication);
    const void *requestData;
    int requestLen;
      
    if (RTESec_Authentication_DataForChallengeRequest (authentication,
                                                       clientBuffer,
                                                       (const char *) buf,
                                                       namelen,
                                                       &requestData,
                                                       &requestLen)) {
      pr03ConAddDataField (partPtr, algoname, strlen(algoname));
      pr03ConAddDataField (partPtr, requestData, requestLen);
    }
  }
  pr03ConSetVardataFieldCount (partPtr, index*2);
}

typedef struct PropertyT {
  char          *name;
  int            namelen;
  unsigned char *data;
  int            datalen;
} PropertyT;

static tsp1_part *pr03ConNewVardataPart (tpr03_SegmDesc *SegmDesc, int valueCount)
{
  tsp1_part *result = pr03SegmentAddPart(SegmDesc, sp1pk_vardata);
  tsp00_Byte buf[2];
  
  buf[0] = (tsp00_Byte) (valueCount / 256);
  buf[1] = (tsp00_Byte) (valueCount % 256);

  pr03PartCopy(result, buf, 2);
  return result;
}

static void pr03ConMakeConnectTrace (sqlratype * sqlra, sqlgaentry *gaen, tpr05_String *pString)
{
  sqltatype      *tap = sqlra->rasqltap;

  M90TRACE (M90_TR_STRING, "pr03ConMakeConnectTrace", "entry");
  if (tap->tatrout != CPR_TRACE_OFF) {
    short           *t80l = &sqlra->rasqltap->tastr80l;
    struct SQLERROR errmsg;
    tsp00_Int4        partl;
    tsp00_Int4        zeropos;
    tsp00_Int4        offset;
    tsp1_part      *part_ptr;
    tsp1_segment   *segmp;
    tpr05_StringEncoding encType;
    
    if (gaen && gaen->gareqptr) {
      encType = pr03PacketGetEncoding (gaen->gareqptr);
    } else {
      /*default encoding is Ascii, because not all calling
        functions have a gaentry */
      encType = sp77encodingAscii;
    }
    
    if (encType == sp77encodingAscii) {
      zeropos = 0;
      offset = 1;
    } else if (encType == sp77encodingUCS2) {
      zeropos = 0;
      offset = 2;
    } else if (encType == sp77encodingUCS2Swapped) {
      zeropos = -1;
      offset = 2;
    } else {
      zeropos = 0;
      offset = 1;
    }
    
    p03find_part (sqlra, (char) sp1pk_command,&part_ptr);
    if (part_ptr != NULL) {
      partl = (part_ptr->sp1p_part_header.sp1p_buf_len - 1);
      /*ignore padding blanks*/
      while ((  (part_ptr->sp1p_buf[partl+zeropos] == bsp_c1)
                ||(part_ptr->sp1p_buf[partl+zeropos] == CPR_PC_ENDSIGN))
             && (partl > 1)) {
        partl -= offset;
      }          
      if (partl <= 0)
        /*no command in part the exit*/
        {
          return;
        }
      segmp = sqlra->rasegptr;
      if (segmp->sp1s_segm_header.sp1c_mass_cmd == TRUE) {
        memcpy (&tap->tastr80[*t80l], "MASS STATEMENT :  ", 18);
        *t80l += 18;
        p03tvfwritetrace (sqlra, &errmsg);
      }
      /*put command to trace*/
      if (gaen && gaen->gareqptr) {
        pString = pr05IfCom_String_InitString(pString,
                                              (char*)part_ptr->sp1p_buf, 
                                              partl+1, 
                                              pr03PacketGetEncoding(gaen->gareqptr),
                                              allocated_epr05);
      }
      else {
        /*default encoding is Ascii, because not all calling
          functions have a gaentry */
        pString = pr05IfCom_String_InitString(pString,
                                              (char*)part_ptr->sp1p_buf, 
                                              partl+1, 
                                              sp77encodingAscii,
                                              allocated_epr05);
      }
    }/*end if part*/
  }/*end if trace off*/
}

static void pr03ConMakeVardataPart(tpr01_ConDesc *ConDesc, 
                                   const RTESec_Authentication* authenticator,
                                   PropertyT response)
{
  tsp00_TermId tid;
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_vardata);
  tsp00_Byte *buf = (tsp00_Byte *) pr03PartGetRawPtr(partPtr);
  const char * authName;

  pr03ConSetVardataFieldCount (partPtr, 2);
  pr03PartSetPartLength(partPtr, 2);
  authName = RTESec_Authentication_Name (authenticator);
  pr03ConAddDataField (partPtr, authName, strlen (authName));
  pr03ConAddDataField (partPtr, response.data, response.datalen);
  pr03ConSetVardataFieldCount (partPtr, 2);  
  pr03SegmentFinishPart(SegmDesc);
}

static void pr03ConTraceConnectCmd (tpr01_ConDesc *ConDesc, struct SQLERROR *sqlemp, tpr05_String *pString)
{
  tpr01_ConContainer *Connection = ConDesc->Connection;
  sqlcatype *sqlca = Connection->sqlca;
  sqlxatype *sqlxa = ConDesc->sqlxa;
  short *t80l = &sqlca->sqlrap->rasqltap->tastr80l;
  struct SQLERROR errmsg;
  
  *t80l = 0;
  p03tvfwritetrace (sqlca->sqlrap, &errmsg);
  pr01TraceCmd (sqlca->sqlrap, pString);
  p03traceerror (sqlca->sqlrap, sqlemp);
  p01pparsidtrace (sqlca, sqlxa, ConDesc->ka, NULL, TraceParseIDSQLStmt_epr00);
}

static bool pr03ConChallengeResponse(tpr01_ConDesc *ConDesc, struct SQLERROR *sqlemp)
{
  tpr01_ConContainer *Connection = ConDesc->Connection;
  sqlcatype *sqlca = Connection->sqlca;
  sqlxatype *sqlxa = ConDesc->sqlxa;
  sqlgatype *sqlga = sqlca->sqlgap;
  sqlgaentry *gaen = ConDesc->ga;
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  tpr05_String ConString;
  tsp1_part *partPtr;
  tsp00_Byte *buf;
  int fieldCount;
  PropertyT challenge;
  PropertyT response;
  unsigned char *vardata;
  tsp00_Name clearPwd;
  const char * authName;

  tsp4_xuser_record *XUser = ConDesc->XUserRecord;
  tpr05_StringEncoding encType = pr03ConXUserEncoding(XUser);
  const RTESec_Authentication * authenticator;
  RTESec_ClientAuthenticationBuffer clientBuffer;
  tpr05_StringEncoding PacketEncoding;
  tpr05_String pString;

  partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_command);
  ConString.allocationType = constant_epr05;

  pr05IfCom_String_InitString(&ConString,
                              partPtr->variant.C_1.sp1p_buf_F,
	                      partPtr->variant.C_1.sp1p_part_header_F.sp1p_buf_size,
	                      encType,
	                      constant_epr05);
  pr03ConMakeChallengeStmt(ConDesc, &ConString, sqlemp);
  pr03PartSetPartLength(partPtr, ConString.cbLen);
  pr03SegmentFinishPart(SegmDesc);

  pr03ConAddTermid(ConDesc);

  partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_vardata);
  pr03ConSetVardataFieldCount (partPtr, 0);
  pr03PartSetPartLength(partPtr, 2);
  pr03ConMakeAuthStmt(ConDesc, partPtr, &clientBuffer, sqlemp);
  pr03SegmentFinishPart(SegmDesc);

  pString.allocationType = constant_epr05;

  pr03ConMakeConnectTrace (sqlca->sqlrap, gaen, &pString);
  p03creqrecpacket(sqlca->sqlrap, gaen, sqlemp);
  if (sqlemp->ereturncode != 0) {
    pr03ConTraceConnectCmd (ConDesc, sqlemp, &pString);
    pr05IfCom_String_DeleterawString(&pString);
    return false;
  }
  pr05IfCom_String_DeleterawString(&pString);

  p03find_part(sqlca->sqlrap, sp1pk_vardata, &partPtr);
  if (!partPtr) {
    return false;
  }
  vardata = partPtr->sp1p_buf;
  fieldCount = vardata[0] * 256 + vardata[1];
  if (fieldCount < 2) {
    return false;
  }
  vardata += 2;
  pr03ConGetField (&vardata, (unsigned char **) &challenge.name, &challenge.namelen);
  pr03ConGetField (&vardata, &challenge.data, &challenge.datalen);

  authenticator = RTESec_Authentication_FindByName (challenge.name, challenge.namelen);
  if (authenticator == NULL) {
    return false;
  }
  memset (clearPwd, 0, sizeof(tsp00_Name));
  if (encType == sp77encodingAscii) {
    s02appldecrypt (clearPwd, XUser->xu_password);
  } else {
    s02appldecrypt (clearPwd, XUser->xu_passwordUCS2);
  }
  if (!RTESec_Authentication_CreateResponse (authenticator,
                                             &clientBuffer, (void *) &response.data, &response.datalen,
                                             challenge.data, challenge.datalen,
                                             (const char *) &clearPwd [0], sizeof (clearPwd))) {
    return false;
  }
  PacketEncoding = pr03ConXUserEncoding(ConDesc->XUserRecord);
  Connection->SegmentDelete(ConDesc);
  Connection->InitPacket(ConDesc, PacketEncoding, sp1m_dbs);
  pr03ConMakeConnectPart(ConDesc, sqlemp);
  pr03ConMakeFeaturePart(ConDesc);
  pr03ConMakeVardataPart(ConDesc, authenticator, response);
  pr03ConAddClientid(ConDesc);
  p03connectcmdtrace(sqlca->sqlrap, gaen);

#ifdef SQLODBC
  if (!pa01UtilityConnect())
#endif
    p03creqrecpacket(sqlca->sqlrap, gaen, sqlemp);
  if ((sqlemp->ereturncode != 0)
      && (sqlemp->ereturncode != USER_ALREADY_CONNECTED)) {
    p03sqlrelease(sqlca->sqlrap, sqlga, gaen, sqlemp);
    return false;
  }

  return true;
}

static void
pr03ConSendConnect (struct tpr01_ConDesc *ConDesc)
{
  M90TRACE (M90_TR_ENTRY, "pr03ConSendConnect", 0);
  {
    tpr01_ConContainer *Connection = ConDesc->Connection;
    sqlcatype *sqlca = Connection->sqlca;
    sqlxatype *sqlxa = ConDesc->sqlxa;
    sqlgatype *sqlga = sqlca->sqlgap;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    sqlgaentry *gaen = ConDesc->ga;
    sqlca->sqlrap->raactsession = ConDesc->SessionNr;
    M90TRACE (M90_TR_PTR,   "gaen", &gaen);
    if (sqlemp->ereturncode == 0) {
      tsp00_ErrTextc szErrText;
      SAPDB_Bool hasBasicAuthOnly = true;
      Connection->SegmentDelete(ConDesc);
      p03sqlaconnect (sqlca->sqlrap, sqlga, gaen, sqlemp);
      hasBasicAuthOnly = SqlBasicAuthenticationOnly (gaen->gareference, szErrText);
      pr03PacketHandleComError(ConDesc);
      if (sqlemp->ereturncode == 0) {
        tpr05_StringEncoding PacketEncoding = pr03ConXUserEncoding(ConDesc->XUserRecord);
	Connection->InitPacket(ConDesc, PacketEncoding, sp1m_dbs);
        if (hasBasicAuthOnly == true) {
          pr03ConMakeConnectPart(ConDesc, sqlemp);
          pr03ConMakePasswordPart(ConDesc);
          pr03ConMakeFeaturePart(ConDesc);
          p03connectcmdtrace(sqlca->sqlrap, gaen);
          if (pr01TraceIsTrace(sqlca->sqlrap)) {
            pr01TracePrintf(sqlca->sqlrap, "AUTHENTICATION: NO");
          }
#ifdef SQLODBC
          if (!pa01UtilityConnect())
#endif
            p03creqrecpacket(sqlca->sqlrap, gaen, sqlemp);
          if ((sqlemp->ereturncode != 0)
              && (sqlemp->ereturncode != USER_ALREADY_CONNECTED))
            p03sqlrelease(sqlca->sqlrap, sqlga, gaen, sqlemp);
        } else if (!pr03ConChallengeResponse (ConDesc, sqlemp)) {
          p03cseterror (sqlemp, cpr_session_not_connected);
          p03sqlrelease(sqlca->sqlrap, sqlga, gaen, sqlemp);
        }
      }
    }
  }
  M90TRACE (M90_TR_EXIT, "pr03ConSendConnect", 0);
}     /*--------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*!
  Function:     pr03ConSessionInfoGet

  see also:

  Description:    Gets Information about the connected Database.

  This function only works after a connect has procced. Otherwise the
  infos about the connection will fill with default values.

  currently the function will determine the following infos:

  1. The Version of the database.
         gaKernelVersion  (default VersionUnknown_cpr00)
  2. The Databaseencoding and the swap type of the client.
         PacketEncoding   (default sp77encodingAscii)
  3. The KnlSessionID - ID of current session is part of every parseid in this session.
         gaSessionID   (default 0x00 )

  Arguments:
    *ConDesc   [in] Descriptor of the connection.

  return value: void
 */

static tpr00_ClientSession pr03ConClientSessionID = 0;

static void pr03ConSessionInfoGet(tpr01_ConDesc *ConDesc)
{
  M90TRACE (M90_TR_ENTRY, "pr03ConSessionInfoGet", 0);
  {
    tpr01_ConContainer *Connection = ConDesc->Connection;
    sqlgaentry *ga = ConDesc->ga;
    sqlcatype *sqlca = Connection->sqlca;
    sqlratype *sqlra = sqlca->sqlrap;
    tsp1_segment *segm = sqlra->rasegptr;
    char IsUnicode = 0;
    tpr03_KernelVersionC szKernelVersion;
    if (pr03PacketGetKernelParameters(segm, KernelVersionString_epr03, szKernelVersion)) {
      sscanf(szKernelVersion, "%d", &ConDesc->ga->gaKernelVersion);
    }
    else {
      ConDesc->ga->gaKernelVersion = VersionUnknown_cpr00;
    }
    if (pr03PacketGetKernelParameters( segm, UnicodeCatalog_epr03, &IsUnicode)) {
      if (IsUnicode) {
        ConDesc->IsUnicode = true;
      }
      else {
        ConDesc->IsUnicode = false;
      }
    }
    if (!pr03PacketGetKernelParameters(segm, KnlSessionID_epr03, ga->gaKnlSessionID.KnlSession)) {
       memset (ga->gaKnlSessionID.KnlSession, '\0', sizeof(ga->gaKnlSessionID.KnlSession));
       ga->gaKnlSessionID.ClientSession = 0;      
    }
    else {
      pr03ConClientSessionID++;
      memcpy (&ga->gaKnlSessionID.ClientSession, &pr03ConClientSessionID, sizeof(pr03ConClientSessionID));      
    }
    /* Database FeatureSet */
    memcpy(ConDesc->DBFeatures, pr01ConFeatureSetEmpty, sizeof(ConDesc->DBFeatures));
    if (!pr03PacketGetKernelParameters(segm, KernelFeatures_epr03, ConDesc->DBFeatures)) {
      memcpy(ConDesc->DBFeatures, pr01ConFeatureSetEmpty, sizeof(ConDesc->DBFeatures));
    }
    if (pr01TraceIsTrace(sqlca->sqlrap))  {
      boolean first = true; 
      int i;
      for (i=0;i < sizeof(tpr01Con_FeatureSet)/2;i++) {
        if (!ConDesc->DBFeatures[i].value) 
          continue;
        if (first)
          pr01TracePrintf(sqlca->sqlrap, "KERNEL FEATURES :");
        first = false;
        switch(ConDesc->DBFeatures[i].feature) {
        case (sp1f_multiple_drop_parseid) : {
          pr01TracePrintf(sqlca->sqlrap, "MULTI DROP PARSEID");
          break;
        }
        case (sp1f_space_option) : {
          pr01TracePrintf(sqlca->sqlrap, "SPACE OPTION");
          break;
        }
        case (sp1f_variable_input) : {
          pr01TracePrintf(sqlca->sqlrap, "VARIABLE INPUT");
          break;
        }
        case (sp1f_optimized_streams) : {
          pr01TracePrintf(sqlca->sqlrap, "OPTIMIZED STREAMS");
          break;
        }
        default: {
          pr01TracePrintf(sqlca->sqlrap, "UNKNOWN(%d)", ConDesc->DBFeatures[i].feature);
          break;
        }
        }
      }
    }
  }
  M90TRACE (M90_TR_EXIT, "pr03ConSessionInfoGet", 0);
}

static void pr03ConMakePasswordPart(tpr01_ConDesc *ConDesc)
{
  tsp00_TermId tid;
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_data);
  tsp4_xuser_record  *XUser = ConDesc->XUserRecord;
  tpr05_StringEncoding encType = pr03ConXUserEncoding(XUser);
  pr03PartCopy(partPtr, "\0", 1);
  if (encType == sp77encodingAscii)
    pr03PartAppend(partPtr, XUser->xu_password, mxsp_c24);
  else
    pr03PartAppend(partPtr, XUser->xu_passwordUCS2, mxsp_c24);	  

  memset (&tid, bsp_c1, sizeof(tid));
  pr03PartAppend(partPtr, " ", 1);
  memset (&tid, bsp_c1, sizeof(tid));
  sqltermid (tid);
  pr03PartAppend(partPtr, tid, sizeof(tid));
  pr03SegmentFinishPart(SegmDesc);
}


static void pr03ConMakeFeaturePart(tpr01_ConDesc *ConDesc)
{
  tsp00_TermId tid;
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_feature);
  pr03PartCopy(partPtr, (void*)ConDesc->DBFeatures, sizeof(ConDesc->DBFeatures));
  partPtr->sp1p_part_header.sp1p_arg_count = sizeof(ConDesc->DBFeatures)/2;
  pr03SegmentFinishPart(SegmDesc);
}

void pr03ConMakeConnectPart(tpr01_ConDesc *ConDesc, struct SQLERROR *sqlemp)
{
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  tpr05_String ConString;
  tsp1_part *partPtr;

  tsp4_xuser_record *XUser = ConDesc->XUserRecord;
  tpr05_StringEncoding encType = pr03ConXUserEncoding(XUser);

  partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_command);
  ConString.allocationType = constant_epr05;

  pr05IfCom_String_InitString(&ConString,
                              partPtr->variant.C_1.sp1p_buf_F,
	                      partPtr->variant.C_1.sp1p_part_header_F.sp1p_buf_size,
	                      encType,
	                      constant_epr05);
  pr03ConMakeSQLStmt(ConDesc, &ConString, sqlemp);
  pr03PartSetPartLength(partPtr, ConString.cbLen);
  pr03SegmentFinishPart(SegmDesc);
}

static void pr03ConMakeSQLStmt(tpr01_ConDesc *ConDesc, tpr05_String *pConString, struct SQLERROR *sqlemp)
{
  tpr01_ConContainer *Connection = ConDesc->Connection;
  sqlcatype *sqlca = Connection->sqlca;
  sqlratype *sqlra = sqlca->sqlrap;
  sqlgaentry *gae  = ConDesc->ga;
  tsp4_xuser_record  *XUser = ConDesc->XUserRecord;
  tpr05_StringEncoding encType = pr03ConXUserEncoding(XUser);
  int len;
  char  chr12[12];
  BOOLEAN OldStyle = TRUE;
  sqlgatype *sqlga = sqlca->sqlgap;
  if (sqlga->gaConnection) {
    tpr01_ConContainer *Connection = sqlga->gaConnection;
    OldStyle = Connection->OldStyle;
  }
  
  pr05IfCom_String_ConvertP (pConString,
                             "CONNECT ",
                             8,
                             sp77encodingAscii);

  if (!strncmp ((char*)XUser->xu_user, bsp_c64, sizeof(XUser->xu_user)) &&
      (*(const unsigned short *) &XUser->xu_userUCS2[0] == ((const unsigned short)' '))) {
    p03cseterror (sqlemp, cpr_missing_user_passw);
    if (OldStyle)
      gae->gastatus = CPR_CO_IMPLICIT;
  }
  /* truncate username */
  if (encType == sp77encodingAscii)
    len = s30lnr1 (XUser->xu_user, bsp_c1, 1, sizeof(XUser->xu_user));
  else
    len = s30lnr1 (XUser->xu_userUCS2, bsp_c1, 1, sizeof(XUser->xu_userUCS2));

  if (len < mxsp_name)
    len = mxsp_name;

  pr05IfCom_String_strcatP (pConString,
                            "\"",
                            1,
                            sp77encodingAscii);

  if (encType == sp77encodingAscii)
    pr05IfCom_String_strcatP (pConString,
                              (char *) XUser->xu_user, 
                              len,
                              sp77encodingAscii);
  else
    pr05IfCom_String_strcatP (pConString,
                              (char *) XUser->xu_userUCS2, 
                              len,
                              encType);

  pr05IfCom_String_strcatP (pConString,
                            "\"",
                            1,
                            sp77encodingAscii);

  pr05IfCom_String_strcatP (pConString,
                            " IDENTIFIED BY :A  ",
                            19,
                            sp77encodingAscii);

  switch (sqlra->radbmode) {
  case CPR_KIND_DB2: {
    pr05IfCom_String_strcatP (pConString,
                              "SQLMODE DB2 ",
                              12,
                              sp77encodingAscii);
    break;
  }
  case CPR_KIND_ORACLE: {
    pr05IfCom_String_strcatP (pConString,
                              "SQLMODE ORACLE ",
                              15,
                              sp77encodingAscii);
    break;
  }
  case CPR_KIND_SAPR3: {
    pr05IfCom_String_strcatP (pConString,
                              "SQLMODE SAPR3 ",
                              14,
                              sp77encodingAscii);
    break;
  }
  default: {
      /** cpr_kind_internal**/
      if (sqlra->rasqlansi == CPR_KIND_ANSI) {
        pr05IfCom_String_strcatP (pConString,
                                  "SQLMODE ANSI ",
                                  13,
                                  sp77encodingAscii);
      }
      break;
    }
  }
  if ((gae->ganolog == cpr_is_true)
      && (sqlra->racomponent[0] == 'R')) {
    pr05IfCom_String_strcatP (pConString,
                              " NOLOG ",
                              7,
                              sp77encodingAscii);
  }
  if (XUser->xu_isolation != CPR_LO_EMPTY) {
    BOOLEAN         defaultlevel;
    defaultlevel = FALSE;
    switch (sqlra->radbmode) {
    case CPR_KIND_INTERNAL: {
      if (sqlra->rasqlansi == CPR_KIND_ANSI) {
	if ((XUser->xu_isolation == CPR_LO_ISOLEV3)
	    || (XUser->xu_isolation == CPR_LO_ISOLEV30))
	  defaultlevel = TRUE;
      }
      else {
	if (XUser->xu_isolation == CPR_LO_ISOLEV10)
	  defaultlevel = TRUE;
      }
      break;
    }
    case CPR_KIND_DB2: {
      if ((XUser->xu_isolation == CPR_LO_ISOLEV1)
	  || (XUser->xu_isolation == CPR_LO_ISOLEV10))
	defaultlevel = TRUE;
      break;
    }
    case CPR_KIND_ORACLE:{}
    case CPR_KIND_SAPR3:{
      if (sqlra->rasqlansi == CPR_KIND_ANSI) {
	if ((XUser->xu_isolation == CPR_LO_ISOLEV3)
	    || (XUser->xu_isolation == CPR_LO_ISOLEV30))
	  defaultlevel = TRUE;
      }
      else
	{
	  if ((XUser->xu_isolation == CPR_LO_ISOLEV1)
	      || (XUser->xu_isolation == CPR_LO_ISOLEV10))
	    defaultlevel = TRUE;
	}
      break;
    }
    default: {
      defaultlevel = FALSE;
      break;
    }
    }
    if (defaultlevel == FALSE) {
      pr05IfCom_String_strcatP (pConString,
                                " ISOLATION LEVEL ",
                                17,
                                sp77encodingAscii);

      len = sprintf (chr12, "%d", XUser->xu_isolation);
      pr05IfCom_String_strcatP (pConString,
                                chr12,
                                len,
                                sp77encodingAscii);
    }
  }
  if (XUser->xu_timeout != -1) {
    pr05IfCom_String_strcatP (pConString,
                              "  TIMEOUT ",
                              10,
                              sp77encodingAscii);
    len = sprintf (chr12, "%d", XUser->xu_timeout);
    pr05IfCom_String_strcatP (pConString,
                              chr12,
                              len,
                              sp77encodingAscii);
  }
  if (XUser->xu_cachelimit != -1) {
    pr05IfCom_String_strcatP (pConString,
                              "  CACHELIMIT ",
                              13,
                              sp77encodingAscii);
    len = sprintf (chr12, "%d", XUser->xu_cachelimit);
    pr05IfCom_String_strcatP (pConString,
                              chr12,
                              len,
                              sp77encodingAscii);
  }

  {
    tsp00_KnlIdentifier  termchar;
    sqlcharsetname (termchar);
    if (strncmp ((char*)termchar, bsp_c64, sizeof(tsp00_KnlIdentifier)) != 0) {
      pr05IfCom_String_strcatP (pConString,
                                "  TERMCHAR SET ",
                                15,
                                sp77encodingAscii);
      pr05IfCom_String_strcatP (pConString,
                                (char*)termchar,
                                sizeof(termchar),
                                sp77encodingAscii);
    }
  }
  if (strncmp ((char*)XUser->xu_dblang, bsp_c18, mxsp_c18) != 0) {
    pr05IfCom_String_strcatP (pConString,
                              " CHARACTER SET ",
                              15,
                              sp77encodingAscii);
    pr05IfCom_String_strcatP (pConString,
                              (char*)XUser->xu_dblang,
                              mxsp_c18,
                              sp77encodingAscii);
  }
}

tpr05_StringEncoding pr03ConXUserEncoding(tsp4_xuser_record  *XUserRecord)
{
  if (!strncmp ((const char *) XUserRecord->xu_user, bsp_c64, sizeof(XUserRecord->xu_user)) &&
      (*(const unsigned short *) &XUserRecord->xu_userUCS2[0] != ((const unsigned short) 0))) {
    return (sqlLocalMachineSwapKind == sw_full_swapped) ? sp77encodingUCS2Swapped : sp77encodingUCS2;
  }
  return sp77encodingAscii;
}  
